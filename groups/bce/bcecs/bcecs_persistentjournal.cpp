// bcecs_persistentjournal.cpp                  -*-C++-*-
#include <bcecs_persistentjournal.h>


#include <bdema_default.h>
#include <bdes_assert.h>
#include <bdet_date.h>
#include <bdet_time.h>
#include <bdetu_systemtime.h>
#include <iomanip>
#include <cstddef>
#include <cstring>


#ifdef BDES_PLATFORM__OS_UNIX
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <sys/mman.h>
    #include <fcntl.h>
    #include <unistd.h>
#endif


#include <iomanip>
#include <ostream>


#ifdef BDES_PLATFORM__OS_WINDOWS
    #undef min
    #undef max
#endif

namespace {

// CONSTANTS


enum {
    EOL = -1 // end of list
};

enum CommitState {
    STABLE,               //  --> ie state[0] == state[1]
    MIDDLE_OF_TRASACTION, //  --> ie state[0] is good for sure
    MIDDLE_OF_SYNC        //  --> ie state[1] is good for sure
};


class ReaderWriterLockGuard {
    // This class provides a very simple lock guard for any kinds of lock.
    // Since the bcemt lock guards do not support bcemt_ReaderWriterLock yet,
    // it was designed to be used only for this kind of objects.

    BloombergLP::bcemt_ReaderWriterLock *d_lock;  // guarded lock

    // not implemented
    ReaderWriterLockGuard(const ReaderWriterLockGuard&);
    ReaderWriterLockGuard& operator=(const ReaderWriterLockGuard&);

    public:
      // CREATORS
      ReaderWriterLockGuard(BloombergLP::bcemt_ReaderWriterLock *lock);
        // Creates a guard for the specified *locked* 'lock'.  The behavior
        // is undefined if 'lock' is not already locked.

      ~ReaderWriterLockGuard();
        // Destroy this object.
};

inline
ReaderWriterLockGuard::ReaderWriterLockGuard(
    BloombergLP::bcemt_ReaderWriterLock *lock)
: d_lock(lock)
{
    BDE_ASSERT_CPP(lock);
}

inline
ReaderWriterLockGuard::~ReaderWriterLockGuard()
{
    d_lock->unlock();
}

inline std::size_t alignedSize(std::size_t size, std::size_t alignment)
{
    return ((size + alignment -1) / alignment) * alignment;
}

}  // close unnamed namespace

namespace BloombergLP {








                         // =================
                         // struct bcecs_PersistentJournalFileUtil
                         // =================
#ifdef BDES_PLATFORM__OS_WINDOWS
    // window specific implementation 
    
bcecs_PersistentJournalFileUtil::FileDescriptor bcecs_PersistentJournalFileUtil::BAD_FD = INVALID_HANDLE_VALUE; 




bcecs_PersistentJournalFileUtil::FileDescriptor bcecs_PersistentJournalFileUtil::open(
    const char *pathName, 
    bool        isReadWrite, 
    bool        isExisting)
{
    DWORD accessMode = GENERIC_READ | (isReadWrite?  GENERIC_WRITE:  0);
    DWORD creationInfo = isExisting?  OPEN_EXISTING:  CREATE_ALWAYS;

    return CreateFile(pathName, 
                      accessMode, 
                      FILE_SHARE_READ,       // share for reading??
                      NULL,                  // default security
                      creationInfo,          // existing file only
                      FILE_ATTRIBUTE_NORMAL, // normal file
                      NULL);                 // no attr

}

int bcecs_PersistentJournalFileUtil::close(FileDescriptor fd)
{
    return CloseHandle(fd)? 0: -1;
}

int bcecs_PersistentJournalFileUtil::lseek(FileDescriptor fd, int offset, int whence)
{
    switch (whence) {
      case SEEK_SET: whence = FILE_BEGIN; break;
      case SEEK_CUR: whence = FILE_CURRENT; break;
      case SEEK_END: whence = FILE_END; break;
      default: BDE_ASSERT_CPP(0);  break;
    }

    int rc = SetFilePointer(fd, offset, 0, whence);
    return (rc == INVALID_SET_FILE_POINTER)?  -1:  rc;
}

int bcecs_PersistentJournalFileUtil::read(FileDescriptor fd, void *buf, int numBytesToRead)
{
    DWORD n;
    return ReadFile(fd, buf, numBytesToRead, &n, 0)?  n:  -1;
}

int bcecs_PersistentJournalFileUtil::write(FileDescriptor fd, void *buf, int numBytesToWrite)
{

    DWORD n;
    return WriteFile(fd, buf, numBytesToWrite, &n, 0)?  n:  -1;
}

int bcecs_PersistentJournalFileUtil::unlink(const char* fileToRemove)
{
    return DeleteFile(fileToRemove)? 0: -1;
}



bcecs_PersistentJournalFileUtil::FileMapping 
bcecs_PersistentJournalFileUtil::mmap(FileDescriptor fd, int offset, int len, 
                     bool isReadWrite)
{
    FileMapping fm;
    HANDLE hMap;
    DWORD access, protect;
    void *addr;

    if (isReadWrite) {
        access = FILE_MAP_WRITE; // both read and write
        protect = PAGE_READWRITE;
    }
    else { 
        access = FILE_MAP_READ;
        protect = PAGE_READONLY;
    }

    hMap = CreateFileMapping(fd,
                             NULL,
                             protect,
                             0,
                             0,
                             NULL);

	if (hMap == NULL) {
		fm.d_addr = NULL;
		return fm;
	}

    addr = MapViewOfFile(hMap,
                         access,
                         0,
                         offset,
                         len);
    if (addr == NULL) {
        int success = CloseHandle(hMap);
        BDE_ASSERT_CPP(success);

        fm.d_addr = NULL;
		return fm;
    }

    
    fm.d_addr = addr;
    fm.d_mapHandle = hMap;
    return fm;
}

int   bcecs_PersistentJournalFileUtil::munmap(FileMapping fileMapping, int len)
{    
    int success;

    success = UnmapViewOfFile(fileMapping.addr());
    if (!success) return -1;


    success = CloseHandle(fileMapping.d_mapHandle);
    if (!success) return -2;

    return 0;
}



int   bcecs_PersistentJournalFileUtil::msync(char *addr, int len)
{
    return FlushViewOfFile(addr,len)? 0: -1;
}


int   bcecs_PersistentJournalFileUtil::mprotect(char *addr, int len, bool isReadWrite)
{
    return 0;  //  TBD??
}




#else
    // unix specific implementation 

bcecs_PersistentJournalFileUtil::FileDescriptor bcecs_PersistentJournalFileUtil::BAD_FD = -1;

bcecs_PersistentJournalFileUtil::FileDescriptor bcecs_PersistentJournalFileUtil::open(
    const char *pathName, 
    bool        isReadWrite, 
    bool        isExisting)
{
    int oflag = (isReadWrite)?  O_RDWR:  O_RDONLY;

    if (isExisting)
        return ::open(pathName, oflag);

    return ::open(pathName, oflag | O_CREAT | O_TRUNC, 
                  S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
}

int bcecs_PersistentJournalFileUtil::close(FileDescriptor fd)
{
    return ::close(fd);
}

int bcecs_PersistentJournalFileUtil::lseek(FileDescriptor fd, int offset, int whence)
{
    return ::lseek(fd, offset, whence);
}

int bcecs_PersistentJournalFileUtil::read(FileDescriptor fd, void *buf, int numBytesToRead)
{
    return ::read(fd, buf, numBytesToRead);
}

int bcecs_PersistentJournalFileUtil::write(FileDescriptor fd, void *buf, int numBytesToWrite)
{
    return ::write(fd, buf, numBytesToWrite);
}

int bcecs_PersistentJournalFileUtil::unlink(const char* fileToRemove)
{
    return ::unlink(fileToRemove);
}

bcecs_PersistentJournalFileUtil::FileMapping 
bcecs_PersistentJournalFileUtil::mmap(FileDescriptor fd, int offset, int size, 
                     bool isReadWrite)
{
    void *p = ::mmap(0, size,
                     PROT_READ | (isReadWrite ? PROT_WRITE : 0),
                     MAP_SHARED, fd, offset);

    if (p == MAP_FAILED) 
        p = NULL;


    FileMapping fm;
    fm.d_addr = p;
    return fm;
}

int   bcecs_PersistentJournalFileUtil::munmap(FileMapping fileMapping, int len)
{
    return ::munmap((char *) fileMapping.addr(), len);
}

int   bcecs_PersistentJournalFileUtil::msync(char *addr, int len)
{
    return ::msync(addr, len, MS_SYNC); 
}

int   bcecs_PersistentJournalFileUtil::mprotect(char *addr, int len, bool isReadWrite)
{
    int modeFlag = PROT_READ | (isReadWrite?  PROT_WRITE:  0);
    return ::mprotect(addr, len, modeFlag);
}

#endif


#define STREAM_MEMBER(m) #m ": " << m
#define STREAM_MEMBER_L(inst,m) STREAM_MEMBER(inst.m) << std::endl


enum {
    DAY_SHIFT = 24,
    MONTH_SHIFT = 16,
    YEAR_SHIFT = 0,
    
    SECOND_SHIFT = 16,
    MINUTE_SHIFT = 8,
    HOUR_SHIFT = 0,
    
    
    DAY_MASK = 0xff,
    MONTH_MASK = 0xff,
    YEAR_MASK = 0xffff,
    
    SECOND_MASK = 0xff,
    MINUTE_MASK = 0xff,
    HOUR_MASK = 0xff
};


static int dateToInt(const bdet_Date& date)
{
    return (date.day() << DAY_SHIFT)     | 
           (date.month() << MONTH_SHIFT) | 
           (date.year() << YEAR_SHIFT);
}

static int timeToInt(const bdet_Time& time)
{
    return (time.second() << SECOND_SHIFT)     | 
           (time.minute() << MINUTE_SHIFT)       | 
           (time.hour() << HOUR_SHIFT);
}

static bdet_Date intToDate(unsigned int i)
{
    int year = (i >> YEAR_SHIFT) & YEAR_MASK;
    int month = (i >> MONTH_SHIFT) & MONTH_MASK;
    int day = (i >> DAY_SHIFT) & DAY_MASK;


    bdet_Date date;
    date.setYearMonthDay(year, month, day);
    return date;
}

static bdet_Time intToTime(unsigned int i)
{
    int hour = (i >> HOUR_SHIFT) & HOUR_MASK;
    int minute = (i >> MINUTE_SHIFT) & MINUTE_MASK;
    int second = (i >> SECOND_SHIFT) & SECOND_MASK;

    bdet_Time time;
    time.setHour(hour); time.setMinute(minute); time.setSecond(second); 
    return time;
}

struct bcecs_RecordList {
    // doulby linked list of records
    int d_head;
    int d_tail;
    int d_numRecords;
    int d_numBlocks;
    int d_numBytes;
    bcecs_RecordList() 
        : d_head(EOL)
        , d_tail(EOL)
        , d_numRecords(0)
        , d_numBlocks(0)
        , d_numBytes(0)
    {
    }
};

struct bcecs_FreeBlockList {
    // singly linked list of blocks
    int d_head;
    int d_numBlocks;
    bcecs_FreeBlockList() 
        : d_head(EOL)
        , d_numBlocks(0)
    {
    }
};


// record type, also used as index to // TBD rename them
enum {
    CONFIRMED_RECORD = 0,   // block is the head of a confirmed record
    UNCONFIRMED_RECORD = 1, // block is the head of an unconfirmed record
    NONE_RECORD             // block is neither of above
};

struct bcecs_JournalStateHeader {
    bcecs_RecordList     d_recordLists[2]; // indexed by either
                                           // CONFIRMED_RECORD or 

    bcecs_FreeBlockList  d_freeList;

    int            d_highestBlockUsed; // index of highest block used

    bcecs_JournalStateHeader() 
        : d_freeList()
        , d_highestBlockUsed(-1)
    {
    }
};




struct bcecs_PersistentJournalHeader {
    static const char *JOURNAL_MAGIC_WORD;
    enum { FILE_VERSION = 1 };

    int  d_headerSize;
    int  d_fileVersion;
    char d_magicWord[12];
    int  d_optionFlags;
    int  d_alignmentSize;         //
    int  d_blockSize;
    int  d_blocksPerPage;          // number of blocks in each record table
    int  d_maxConfirmedBytesKept;
    int  d_maxConfirmedRecordsKept;
    int  d_userDataLength;
    int  d_creationDate; // TBD make platform neutral
    int  d_creationTime;
    int  d_commitState;

    bcecs_JournalStateHeader d_stateData[2];
    // char UserData[userDataLength + (4 - userDataLength % 4)];
    // JournalBlockPage blockAllocationPage;
    
    bcecs_PersistentJournalHeader(int alignmentSize, 
                                  int blockSize, 
                                  int blocksPerPage, 
                                  int maxConfirmedBytesKept, 
                                  int maxConfirmedRecordsKept, 
                                  int userDataLength)
        : d_headerSize(-1) // to be set
        , d_fileVersion(FILE_VERSION)
        // , d_magicWord[12]()
        , d_optionFlags(-1)
        , d_alignmentSize(alignmentSize)
        , d_blockSize(blockSize)
        , d_blocksPerPage(blocksPerPage)
        , d_maxConfirmedBytesKept(maxConfirmedBytesKept)
        , d_maxConfirmedRecordsKept(maxConfirmedRecordsKept)
        , d_userDataLength(userDataLength)
        , d_creationDate() // to be set
        , d_creationTime() // to be set
        , d_commitState(STABLE)
        // , d_stateData[2]()
    {
        d_headerSize = alignedSize((sizeof(*this) + userDataLength),
                                          alignmentSize);

        std::memcpy(d_magicWord, JOURNAL_MAGIC_WORD,
                    sizeof(d_magicWord));


        bdet_Datetime dt = bdetu_SystemTime::nowAsDatetimeGMT();
        d_creationDate = dateToInt(dt.date());
        d_creationTime = timeToInt(dt.time());
    }
    bcecs_PersistentJournalHeader() 
    {
    }

};
const char *bcecs_PersistentJournalHeader::JOURNAL_MAGIC_WORD = "A3kdh*(UNA11";

struct JournalBlockLinkage {
    int   d_recordType; // either CONFIRMED_RECORD, UNCONFIRMED_ or NONE_
    int   d_nextBlock;
    int   d_nextRecord;
    int   d_prevRecord;
};

struct bcecs_JournalRecordHeader {
    JournalBlockLinkage d_links[2];

    int   d_recordId;
    int   d_recordLength;     //
    int   d_creationDate;     //
    int   d_creationTime;     //
    int   d_confirmationDate; //
    int   d_confirmationTime; //
};

struct bcecs_JournalPageTable {
    int                 d_tableSize;
    bcecs_JournalRecordHeader d_records[1];
};



std::ostream& operator<< (std::ostream&     stream,
                          const bcecs_RecordList& rhs)
{
    stream
        << std::endl
        << STREAM_MEMBER_L(rhs, d_head            )
        << STREAM_MEMBER_L(rhs, d_tail            )
        << STREAM_MEMBER_L(rhs, d_numRecords            )
        << STREAM_MEMBER_L(rhs, d_numBlocks            )
        << STREAM_MEMBER_L(rhs, d_numBytes            )
        << std::endl;
    return stream;
}


std::ostream& operator<< (std::ostream&     stream,
                          const bcecs_FreeBlockList& rhs)
{
    stream
        << std::endl
        << STREAM_MEMBER_L(rhs, d_head            )
        << STREAM_MEMBER_L(rhs, d_numBlocks            )
        << std::endl;
    return stream;
}


std::ostream& operator<< (std::ostream&             stream,
                          const bcecs_JournalStateHeader& rhs)
{
    stream
        << STREAM_MEMBER_L(rhs,d_recordLists[CONFIRMED_RECORD])
        << STREAM_MEMBER_L(rhs,d_recordLists[UNCONFIRMED_RECORD])
        << STREAM_MEMBER_L(rhs,d_freeList)
        << STREAM_MEMBER_L(rhs,d_highestBlockUsed)
        << std::endl;
    return stream;
}

std::ostream& operator<< (std::ostream&                        stream,
                          const bcecs_PersistentJournalHeader& rhs)
{
    stream
        << STREAM_MEMBER_L(rhs,d_headerSize)
        << STREAM_MEMBER_L(rhs,d_fileVersion)
        << STREAM_MEMBER_L(rhs,d_magicWord[12])
        << STREAM_MEMBER_L(rhs,d_optionFlags)
        << STREAM_MEMBER_L(rhs,d_alignmentSize)
        << STREAM_MEMBER_L(rhs,d_blockSize)
        << STREAM_MEMBER_L(rhs,d_blocksPerPage)
        << STREAM_MEMBER_L(rhs,d_maxConfirmedBytesKept)
        << STREAM_MEMBER_L(rhs,d_maxConfirmedRecordsKept)
        << STREAM_MEMBER_L(rhs,d_userDataLength)
        << STREAM_MEMBER_L(rhs,d_creationDate)
        << STREAM_MEMBER_L(rhs,d_creationTime)
        << STREAM_MEMBER_L(rhs,d_commitState)
        << STREAM_MEMBER_L(rhs,d_stateData[0])
        << STREAM_MEMBER_L(rhs,d_stateData[1])
        << std::endl;
    return stream;
}

class bcecs_ProtectionModeGuard {
    bcecs_PersistentJournal *d_journal_p;
  public:
    // CREATORS
    bcecs_ProtectionModeGuard(bcecs_PersistentJournal *journal)
        : d_journal_p(journal)
    {
        if (d_journal_p->protectionMode() == 
                               bcecs_PersistentJournal::PROTECTED) 
        {
            d_journal_p->setProtection(true);
        }
    }

    ~bcecs_ProtectionModeGuard()
    {
        if (d_journal_p->protectionMode() == 
                               bcecs_PersistentJournal::PROTECTED) 
        {
            d_journal_p->setProtection(false);
        }
    }
};

class bcecs_JournalTransactionGuard {
    bcecs_PersistentJournal *d_journal_p;

  public:
    bcecs_JournalTransactionGuard(bcecs_PersistentJournal *journal)
        : d_journal_p(journal)
    {
        d_journal_p->beginTransaction();
    }

    ~bcecs_JournalTransactionGuard()
    {
        if (d_journal_p) abort();
    }

    int commit()
    {
        int res = d_journal_p->commitTransaction();
        if (d_journal_p->d_commitMode ==
                           bcecs_PersistentJournal::EXPLICIT_COMMIT)
        {
            d_journal_p->commitImpl();
        }
        d_journal_p = 0;
        return res;
    }

    void abort()
    {
        d_journal_p->abortTransaction();
        d_journal_p = 0;
    }
};



static int ceiling(int x, int y) {
    return (x + y-1)/y;
}

static int chunkSize(const bcecs_PersistentJournalHeader& header)
{
    const int pageTableSize = sizeof(bcecs_JournalPageTable) +
        sizeof(bcecs_JournalRecordHeader) * (header.d_blocksPerPage-1);

    const int pageDataSize  = header.d_blockSize * header.d_blocksPerPage;
    const int chunkSize     = alignedSize(pageTableSize + pageDataSize,
                                          header.d_alignmentSize);

    return chunkSize;
}

static int calcPageOffset(int pageIdx,
                          const bcecs_PersistentJournalHeader& header)
{
    return header.d_headerSize + pageIdx * chunkSize(header);
}

static int calcBlockOffset(int                                  blockIndex,
                           const bcecs_PersistentJournalHeader& header)
{
    const int pageTableSize = sizeof(bcecs_JournalPageTable) +
         sizeof(bcecs_JournalRecordHeader) * (header.d_blocksPerPage-1);
    const int pageDataSize  = header.d_blockSize * header.d_blocksPerPage;
    const int chunkSize     = alignedSize(pageTableSize + pageDataSize,
                                          header.d_alignmentSize);

    return header.d_headerSize +
           (chunkSize * (blockIndex/header.d_blocksPerPage)) + pageTableSize +
           header.d_blockSize * (blockIndex % header.d_blocksPerPage);
}

static void initPageTable(bcecs_JournalPageTable *table,
                          int blocksPerPage,
                          int pageIdx)
{
    std::memset(table, 0, sizeof(*table) +
                (sizeof(bcecs_JournalRecordHeader) * blocksPerPage-1));
    table->d_tableSize = blocksPerPage;

    int id = pageIdx * blocksPerPage;
    for (int i=0; i<blocksPerPage;++i, ++id) {
        table->d_records[i].d_recordId = id;
        table->d_records[i].d_recordId = id;

        table->d_records[i].d_links[0].d_nextBlock = id + 1;
        table->d_records[i].d_links[1].d_nextBlock = id + 1;

        table->d_records[i].d_links[0].d_recordType = NONE_RECORD;
        table->d_records[i].d_links[1].d_recordType = NONE_RECORD;
    }
    table->d_records[blocksPerPage-1].d_links[0].d_nextBlock = EOL;
    table->d_records[blocksPerPage-1].d_links[1].d_nextBlock = EOL;
}

static int initializeJournal(bcecs_PersistentJournalFileUtil::FileDescriptor fd,
                             int blockSize,
                             int blocksPerPage,
                             int userDataSize,
                             int alignmentSize,
                             int confirmedRecordsKept,
                             int confirmedBytesKept)
{
    bcecs_PersistentJournalHeader header(alignmentSize, 
                                         blockSize, 
                                         blocksPerPage, 
                                         confirmedBytesKept, 
                                         confirmedRecordsKept, 
                                         userDataSize);

    if (sizeof(header) != bcecs_PersistentJournalFileUtil::write(fd, &header, sizeof(header)))
        return bcecs_PersistentJournal::IO_ERROR;

    if (-1 == bcecs_PersistentJournalFileUtil::lseek(fd, header.d_headerSize-1, SEEK_SET) ||
        1 != bcecs_PersistentJournalFileUtil::write(fd, (char *) "", 1)) {
        return bcecs_PersistentJournal::IO_ERROR;
    }

    return 0;
}

static bcecs_PersistentJournalFileUtil::FileMapping mapPageData(
    bcecs_PersistentJournalFileUtil::FileDescriptor           fd,
    int                                  pageIdx,
    const bcecs_PersistentJournalHeader& header,
    bool                                 readWrite)
{


    int pageOffset = calcPageOffset(pageIdx, header);
    int endOfPage = calcPageOffset(pageIdx+1, header);

    BDE_ASSERT_CPP(endOfPage > pageOffset);

    int mapSize = endOfPage - pageOffset;

    return bcecs_PersistentJournalFileUtil::mmap(fd, pageOffset, mapSize, readWrite);
}


                        // =============================
                        // class bcecs_PersistentJournal
                        // =============================


// PUBLIC STATIC DATA
bcecs_PersistentJournal::FileDescriptor
bcecs_PersistentJournal::BAD_FD = bcecs_PersistentJournalFileUtil::BAD_FD;


// CREATORS
bcecs_PersistentJournal::bcecs_PersistentJournal(CommitMode commitMode,
                                                 bdema_Allocator *allocator)
: d_allocator_p(allocator)
, d_maxFileSize(-1)
, d_mode(READ_ONLY)
, d_commitMode(commitMode)
, d_protectionMode(UNPROTECTED)
, d_fd(bcecs_PersistentJournalFileUtil::BAD_FD)
, d_workIndex(-1)
, d_header()
, d_pageTables(allocator)
, d_dirtyRecords() // TBD
    , d_dirtyPageIndices() // TBD
, d_lock()
{
    d_header.d_addr = 0;
}

bcecs_PersistentJournal::~bcecs_PersistentJournal()
{
    int rc = this->close();
    BDE_ASSERT_CPP(rc == 0);
}


// PRIVATE MANIPULATORS
bcecs_JournalStateHeader *bcecs_PersistentJournal::stateHeader()
{
    return &getHeader()->d_stateData[d_workIndex];
}

const bcecs_JournalStateHeader *bcecs_PersistentJournal::stateHeader() const
{
    return &getHeader()->d_stateData[d_workIndex];
}


bcecs_JournalRecordHeader *bcecs_PersistentJournal::findRecordForUpdate(
    int recordId)
{
    bcecs_JournalRecordHeader *rec = findRecord(recordId);
    if (rec) { 
        d_dirtyRecords.insert(rec);
        int pageIdx = recordId/blocksPerPage();
        d_dirtyPageIndices.insert(pageIdx);
    }
    return rec;
}

bcecs_JournalRecordHeader *bcecs_PersistentJournal::findRecord(int recordId)
{
    const bcecs_PersistentJournal *ths = this;
    return const_cast<bcecs_JournalRecordHeader *> (ths->findRecord(recordId));
}

const bcecs_JournalRecordHeader*
bcecs_PersistentJournal::findRecord(int recordId) const
{
    if (recordId < 0) {
        return NULL;
    }
    int pageIdx = recordId / blocksPerPage();

    if (pageIdx >= d_pageTables.size())
        return NULL;

    bcecs_JournalPageTable *table = 
                 (bcecs_JournalPageTable *) d_pageTables[pageIdx].first.addr();
    const bcecs_JournalRecordHeader *rec = 
        &table->d_records[recordId % blocksPerPage()];

    if(rec->d_recordId != recordId)
        return NULL;
    return rec;
}




char *bcecs_PersistentJournal::getDataBuffer(int index)
{
    int pageIdx   = index/blocksPerPage();

    if ((pageIdx < 0) || ((unsigned)pageIdx >= d_pageTables.size()))
        return 0;

    return d_pageTables[pageIdx].second +
                      (index % blocksPerPage()) * blockSize();
}


const char *bcecs_PersistentJournal::getDataBuffer(int index) const
{
    int pageIdx   = index/blocksPerPage();

    if ((pageIdx < 0) || ((unsigned)pageIdx >= d_pageTables.size()))
        return 0;

    return d_pageTables[pageIdx].second +
                      (index % blocksPerPage()) * blockSize();
}

int bcecs_PersistentJournal::setProtection(bool readWrite)
{
    int rc;
    char *addr;

    // pages
    int size = chunkSize(*getHeader());
    for (int i = 0; i < d_pageTables.size(); ++i) {
        addr = (char *) d_pageTables[i].first.addr();
        rc = bcecs_PersistentJournalFileUtil::mprotect(addr, size, readWrite);
        if (rc) return PROTECTION_FAILURE;
    }


    // header
    addr = (char *) getHeader();
    size = getHeader()->d_headerSize;
    rc = bcecs_PersistentJournalFileUtil::mprotect(addr, size, readWrite);
    if (rc) return PROTECTION_FAILURE;

    return 0;
}

void bcecs_PersistentJournal::beginTransaction()
{
    BDE_ASSERT_CPP(getHeader()->d_commitState == STABLE);

    getHeader()->d_commitState = MIDDLE_OF_TRASACTION;
}

void bcecs_PersistentJournal::dirtySync(int srcIdx)
{
    const int dstIdx = srcIdx ^ 1;

    typedef std::set<bcecs_JournalRecordHeader*>::iterator It;
    for (It itr=d_dirtyRecords.begin();
         itr != d_dirtyRecords.end(); ++itr) {
        std::memcpy(&(*itr)->d_links[dstIdx], &(*itr)->d_links[srcIdx],
                    sizeof((*itr)->d_links[0]));
    }

    std::memcpy(&getHeader()->d_stateData[dstIdx],
                &getHeader()->d_stateData[srcIdx],
                sizeof(getHeader()->d_stateData[0]));

    d_dirtyRecords.clear();
}

int bcecs_PersistentJournal::commitTransaction()
{
    BDE_ASSERT_CPP(getHeader()->d_commitState == MIDDLE_OF_TRASACTION);

    getHeader()->d_commitState = MIDDLE_OF_SYNC;
    dirtySync(d_workIndex);
    getHeader()->d_commitState = STABLE;
    return 0;
}

void bcecs_PersistentJournal::abortTransaction()
{
    BDE_ASSERT_CPP(getHeader()->d_commitState == MIDDLE_OF_TRASACTION);

    dirtySync(d_workIndex ^ 1);
    while ((unsigned)numPageTables() != d_pageTables.size()) { // to accomodate
                                                      // failure during growing
        BDE_ASSERT_CPP(d_pageTables.size() > (unsigned)numPageTables());

        int rc = bcecs_PersistentJournalFileUtil::munmap(
                          d_pageTables.back().first, chunkSize(*getHeader()));
        BDE_ASSERT_CPP(rc == 0);

        d_pageTables.pop_back();
    }
    getHeader()->d_commitState = STABLE;
}

char *bcecs_PersistentJournal::userData()
{
    return (char *) getHeader() + sizeof(bcecs_PersistentJournalHeader);
}

int bcecs_PersistentJournal::setProtectionMode(ProtectionMode newMode)
{
    d_lock.lockWrite();
    ReaderWriterLockGuard guard(&d_lock);

    ProtectionMode oldMode = d_protectionMode;
    d_protectionMode = newMode;

    if (oldMode != newMode && bcecs_PersistentJournalFileUtil::BAD_FD != d_fd) {
        int rc = setProtection(newMode == UNPROTECTED);
        if (rc) return rc;
    }
    return 0;
}

bcecs_PersistentJournalFileUtil::FileDescriptor bcecs_PersistentJournal::getFd()
{
    return d_fd;
}    



// PRIVATE ACCESSORS
bool bcecs_PersistentJournal::isValidMaxFileSize(int value) const
{
    if (value == -1) return true;
    if (getHeader()) { // is open
        int minSizeForOneRecord = 
            getHeader()->d_headerSize + chunkSize(*getHeader());
        int currentSize = getHeader()->d_headerSize + 
            chunkSize(*getHeader()) * numPageTables();

        if ((value<minSizeForOneRecord) || (value<currentSize))
            return false;
    }
    
    return true;
}

void bcecs_PersistentJournal::fillFreeBlocks(std::vector<int> *buf) const
{
    const bcecs_JournalStateHeader *state = stateHeader();
    for (const bcecs_JournalRecordHeader* record = 
             findRecord(state->d_freeList.d_head);
         record;
         record = findRecord(record->d_links[d_workIndex].d_nextBlock))
    {
        int id         = record->d_recordId;
        buf->push_back(id);
    }
    
}

void bcecs_PersistentJournal::assertInvariantsForFreeList(
    std::set<int> *blockSet) const
{
    const bcecs_JournalStateHeader *state = stateHeader();

    int maxBlockIndex = numPageTables() * blocksPerPage() - 1;
    int numBlk = 0;
    
    const bcecs_JournalRecordHeader* record = 
        findRecord(state->d_freeList.d_head);
    while(record) {
        BDE_ASSERT_CPP(std::memcmp(&record->d_links[0], 
                                   &record->d_links[1], 
                                   sizeof(record->d_links[0])) == 0);
        int type       = record->d_links[d_workIndex].d_recordType;
        int id         = record->d_recordId;
        ++numBlk;

        BDE_ASSERT_CPP(type == NONE_RECORD);


        BDE_ASSERT_CPP(id <= maxBlockIndex);


        BDE_ASSERT_CPP(blockSet->find(id)==blockSet->end());
        blockSet->insert(id);

        record = findRecord(record->d_links[d_workIndex].d_nextBlock);
    }
    BDE_ASSERT_CPP(numBlk == state->d_freeList.d_numBlocks);
}

// return numBlk in this record
int bcecs_PersistentJournal::assertInvariantsForRecord(
    const bcecs_JournalRecordHeader* record,
    int recType,
    std::set<int> *blockSet) const
{
    // const bcecs_JournalStateHeader *state = stateHeader();

    int recLen = record->d_recordLength;
    int maxBlockIndex = highestBlockUsed();
    int numBlk = 0;
    
    int first = 1;
    while(record) {
        BDE_ASSERT_CPP(std::memcmp(&record->d_links[0], 
                                   &record->d_links[1], 
                                   sizeof(record->d_links[0])) == 0);
        int type       = record->d_links[d_workIndex].d_recordType;
        int id         = record->d_recordId;
        ++numBlk;

        if (first) {
            BDE_ASSERT_CPP(type == recType);
            first = 0;
        }
        else {
            BDE_ASSERT_CPP(type == NONE_RECORD);
        }

        BDE_ASSERT_CPP(id <= maxBlockIndex);


        BDE_ASSERT_CPP(blockSet->find(id) == blockSet->end());
        blockSet->insert(id);

        record = findRecord(record->d_links[d_workIndex].d_nextBlock);
    }

    BDE_ASSERT_CPP(numBlk == ceiling(recLen, blockSize()));
    return numBlk;
}



void bcecs_PersistentJournal::assertInvariantsForRecordList(
    int recType, std::set<int> *blockSet) const
{
    const bcecs_JournalStateHeader *state = stateHeader();

    int numRec = 0;
    int numBlk = 0;
    int numByt = 0;

    const bcecs_JournalRecordHeader* record = 
        findRecord(state->d_recordLists[recType].d_head);


    std::vector<int> vec1;
    std::vector<int> vec2;
    while(record) {
        int id         = record->d_recordId;

        ++numRec;
        numBlk += assertInvariantsForRecord(record, recType, blockSet);
        numByt += record->d_recordLength;

        vec1.push_back(id);
        record = findRecord(record->d_links[d_workIndex].d_nextRecord);
    }

    BDE_ASSERT_CPP(numRec == state->d_recordLists[recType].d_numRecords);
    BDE_ASSERT_CPP(numBlk == state->d_recordLists[recType].d_numBlocks);
    BDE_ASSERT_CPP(numByt == state->d_recordLists[recType].d_numBytes);


    BDE_ASSERT_CPP(numByt >= numBlk);
    BDE_ASSERT_CPP(numBlk >= numRec);
    BDE_ASSERT_CPP(numRec >= 0);


    if (recType == CONFIRMED_RECORD) {
        BDE_ASSERT_CPP(numRec == numConfirmedRecords());
        BDE_ASSERT_CPP(numBlk == numConfirmedBlocks());
        BDE_ASSERT_CPP(numByt == numConfirmedBytes());

        BDE_ASSERT_CPP(
            state->d_recordLists[recType].d_head == confirmedListHead());

        BDE_ASSERT_CPP(
            state->d_recordLists[recType].d_tail == confirmedListTail());
    }
    else if (recType == UNCONFIRMED_RECORD) {

        BDE_ASSERT_CPP(numRec == numUnconfirmedRecords());
        BDE_ASSERT_CPP(numBlk == numUnconfirmedBlocks());
        BDE_ASSERT_CPP(numByt == numUnconfirmedBytes());

        BDE_ASSERT_CPP(
            state->d_recordLists[recType].d_head == unconfirmedListHead());

        BDE_ASSERT_CPP(
            state->d_recordLists[recType].d_tail == unconfirmedListTail());
    }
    else {
        BDE_ASSERT_CPP(0);
    }


    // tracerse in reverse
    for (const bcecs_JournalRecordHeader* record = 
             findRecord(state->d_recordLists[recType].d_tail);
         record;
         record = findRecord(record->d_links[d_workIndex].d_prevRecord))
    {
        int id         = record->d_recordId;
        vec2.push_back(id);
    }
    BDE_ASSERT_CPP(vec1.size() == vec2.size());
    int size = vec1.size();
    for (int i = 0; i < size; ++i) {
        BDE_ASSERT_CPP(vec1[i] == vec2[size-i-1]);
    }



}
#ifdef BDE_BUILD_TARGET_SAFE

void bcecs_PersistentJournal::assertInvariants() const
{
    if (d_fd == bcecs_PersistentJournalFileUtil::BAD_FD) {
        BDE_ASSERT_CPP(getHeader() == NULL);
        BDE_ASSERT_CPP(d_pageTables.size() == 0);
        BDE_ASSERT_CPP(d_dirtyPageIndices.size() == 0);
        return;
    }

    BDE_ASSERT_CPP((d_mode == READ_ONLY) || (d_mode == READ_WRITE));
    BDE_ASSERT_CPP((d_commitMode == EXPLICIT_COMMIT) || 
                   (d_commitMode == IMPLICIT_COMMIT));

    BDE_ASSERT_CPP(d_dirtyRecords.size() == 0);
    BDE_ASSERT_CPP((d_workIndex == 1) || (d_workIndex == 0));


    BDE_ASSERT_CPP(d_fd != bcecs_PersistentJournalFileUtil::BAD_FD);
    BDE_ASSERT_CPP(getHeader() != NULL);

    std::set<int> blockSet;
    assertInvariantsForFreeList(&blockSet);

    assertInvariantsForRecordList(UNCONFIRMED_RECORD, &blockSet);
    assertInvariantsForRecordList(CONFIRMED_RECORD, &blockSet);


    BDE_ASSERT_CPP(d_pageTables.size() == numPageTables());
    int numTotBlks = d_pageTables.size() * blocksPerPage();
    BDE_ASSERT_CPP(numTotBlks == blockSet.size());




    
    BDE_ASSERT_CPP(numTotBlks == 
                   numFreeBlocks() + 
                   numConfirmedBlocks() + 
                   numUnconfirmedBlocks());

    BDE_ASSERT_CPP(highestBlockUsed() <= numTotBlks);

    BDE_ASSERT_CPP(
        (d_workIndex == 1) || 
        ((d_mode == READ_ONLY) && (getHeader()->d_commitState == 
                                   MIDDLE_OF_TRASACTION)));


    BDE_ASSERT_CPP((d_commitMode == IMPLICIT_COMMIT) ||
                   (d_dirtyPageIndices.size() == 0));



    if (maxConfirmedRecordsKept() != -1) {
        BDE_ASSERT_CPP(numConfirmedRecords() <= maxConfirmedRecordsKept());
    }
    if (maxConfirmedBytesKept() != -1) {
        BDE_ASSERT_CPP(numConfirmedBytes() <= maxConfirmedBytesKept());
    }


    typedef std::set<int>::iterator It;
    for (It itr=d_dirtyPageIndices.begin();
         itr != d_dirtyPageIndices.end(); ++itr) 
    {
        int pageIdx = *itr;
        
        BDE_ASSERT_CPP(pageIdx >= 0);
        BDE_ASSERT_CPP(pageIdx < numPageTables());
    }
    

    BDE_ASSERT_CPP(getHeader()->d_commitState == STABLE);


    BDE_ASSERT_CPP(std::memcmp(&getHeader()->d_stateData[0],
                &getHeader()->d_stateData[1],
                sizeof(getHeader()->d_stateData[0])) == 0);



}

#else  // BDE_BUILD_TARGET_SAFE

inline
void bcecs_PersistentJournal::assertInvariants() const
{
}

#endif  // BDE_BUILD_TARGET_SAFE







int bcecs_PersistentJournal::numFreeBlocks() const
{
    const bcecs_JournalStateHeader *state = stateHeader();
    return state->d_freeList.d_numBlocks;
}

int bcecs_PersistentJournal::freeListHead()
{
    const bcecs_JournalStateHeader *state = stateHeader();
    return state->d_freeList.d_head;
}



int bcecs_PersistentJournal::confirmedListHead() const
{
    const bcecs_JournalStateHeader *state = stateHeader();
    return state->d_recordLists[CONFIRMED_RECORD].d_head;
}


int bcecs_PersistentJournal::confirmedListTail() const
{
    const bcecs_JournalStateHeader *state = stateHeader();
    return state->d_recordLists[CONFIRMED_RECORD].d_tail;
}


int bcecs_PersistentJournal::numConfirmedBlocks() const
{
    const bcecs_JournalStateHeader *state = stateHeader();
    return state->d_recordLists[CONFIRMED_RECORD].d_numBlocks;
}


int bcecs_PersistentJournal::numConfirmedBytes() const
{
    const bcecs_JournalStateHeader *state = stateHeader();
    return state->d_recordLists[CONFIRMED_RECORD].d_numBytes;
}





int bcecs_PersistentJournal::unconfirmedListHead() const
{
    const bcecs_JournalStateHeader *state = stateHeader();
    return state->d_recordLists[UNCONFIRMED_RECORD].d_head;
}


int bcecs_PersistentJournal::unconfirmedListTail() const
{
    const bcecs_JournalStateHeader *state = stateHeader();
    return state->d_recordLists[UNCONFIRMED_RECORD].d_tail;
}


int bcecs_PersistentJournal::numUnconfirmedBlocks() const
{
    const bcecs_JournalStateHeader *state = stateHeader();
    return state->d_recordLists[UNCONFIRMED_RECORD].d_numBlocks;
}


int bcecs_PersistentJournal::numUnconfirmedBytes() const
{
    const bcecs_JournalStateHeader *state = stateHeader();
    return state->d_recordLists[UNCONFIRMED_RECORD].d_numBytes;
}





    
int bcecs_PersistentJournal::numPageTables() const
{
    if (highestBlockUsed() == -1) return 0;

    int highestPageIndex = highestBlockUsed() / blocksPerPage();
    return highestPageIndex + 1;
}

int bcecs_PersistentJournal::highestBlockUsed() const
{
    const bcecs_JournalStateHeader *state = stateHeader();
    return state->d_highestBlockUsed;
}



int bcecs_PersistentJournal::openJournal(bcecs_PersistentJournalFileUtil::FileDescriptor fd, 
                                         Mode mode)
{
    int res;
    d_mode                    = mode;
    if (bcecs_PersistentJournalFileUtil::BAD_FD != d_fd) return BAD_STATE;

    int fileSize = bcecs_PersistentJournalFileUtil::lseek(fd, 0, SEEK_END);

    if (-1 == fileSize) return IO_ERROR;
    if (-1 == bcecs_PersistentJournalFileUtil::lseek(fd, 0, SEEK_SET)) {
        return IO_ERROR;
    }

    if (fileSize < (int) sizeof(bcecs_PersistentJournalHeader)) {
        return INVALID_FILE;
    }

    bcecs_PersistentJournalHeader header;
    if (sizeof(header) != bcecs_PersistentJournalFileUtil::read(
                                                fd, &header, sizeof(header))) {
        return IO_ERROR;
    }

    if ((unsigned)header.d_headerSize < sizeof(bcecs_PersistentJournalHeader)
      || header.d_headerSize > fileSize) {
        return INVALID_FILE;
    }

    if (std::memcmp(header.d_magicWord, 
                    bcecs_PersistentJournalHeader::JOURNAL_MAGIC_WORD,
                    sizeof(header.d_magicWord))) {
        return INVALID_FILE;
    }



    if (header.d_fileVersion > 1) return UNSUPPORTED_VERSION;

    if (header.d_blockSize <  32) return INVALID_FILE;



    // d_workIndex
    d_workIndex = 1;
    if (mode == READ_ONLY && header.d_commitState == MIDDLE_OF_TRASACTION)
        d_workIndex = 0;

    // d_header
    d_header = 
        bcecs_PersistentJournalFileUtil::mmap(fd, 0, header.d_headerSize, mode == READ_WRITE);

    if (!getHeader()) {
        return MEM_ERROR;
    }


    bcecs_JournalStateHeader *state = stateHeader();


    if (numFreeBlocks()  < 0 || numUnconfirmedRecords() < 0 ||
        numConfirmedRecords() < 0 || userDataLength() < 0)
        return INVALID_FILE;

    int expectedMinFileSize = 
        calcBlockOffset(state->d_highestBlockUsed, header) + blockSize();
    if (state->d_highestBlockUsed != -1 &&
        fileSize < expectedMinFileSize) {
        return INVALID_FILE;
    }


    if (READ_WRITE == mode) { // accomodate for truncation
        int mapSize = calcPageOffset(numPageTables(), header);
        if (fileSize < mapSize) {
            if (-1 == bcecs_PersistentJournalFileUtil::lseek(fd, mapSize, SEEK_SET)) 
                return IO_ERROR;
            if (1 != bcecs_PersistentJournalFileUtil::write(fd, (char *)"", 1)) return IO_ERROR;
        }
    }


    // d_pageTables
    d_pageTables.clear();
    int numPageTbles = numPageTables();
    for (int i=0; i<numPageTbles; ++i) {
        FileMapping pageFm = mapPageData(fd, i, header,
                                                         READ_WRITE == mode);


        if (!pageFm.addr()) return MEM_ERROR;
        char *data = ((char*)pageFm.addr()) + sizeof(bcecs_JournalPageTable) +
                     sizeof(bcecs_JournalRecordHeader) *
                     (getHeader()->d_blocksPerPage-1);
        d_pageTables.push_back(std::make_pair(pageFm, data));
    }

    // sync
    sync();

    BDE_ASSERT_CPP(READ_ONLY == mode || getHeader()->d_commitState == STABLE);

    d_fd                      = fd;

    if (protectionMode() == PROTECTED) {
        res = setProtection(false);
        if (res) return res;
    }

    if (!isValidMaxFileSize(d_maxFileSize)) {
        return INVALID_MAXFILESIZE;
    }

    return 0;
}

void bcecs_PersistentJournal::sync() {
    if (READ_ONLY == d_mode || getHeader()->d_commitState == STABLE)
        return;

    int srcIdx;
    if (getHeader()->d_commitState == MIDDLE_OF_TRASACTION)
        srcIdx = 0;
    else if (getHeader()->d_commitState == MIDDLE_OF_SYNC)
        srcIdx = 1;
    else
        BDE_ASSERT_CPP(0);

    int dstIdx = srcIdx ^ 1;

    for(unsigned int i=0; i< d_pageTables.size(); ++i) {
        bcecs_JournalPageTable *page = 
            (bcecs_JournalPageTable *) d_pageTables[i].first.addr();
        for (int j=0; j < getHeader()->d_blocksPerPage; ++j) {
            std::memcpy(&page->d_records[j].d_links[dstIdx],
                        &page->d_records[j].d_links[srcIdx],
                        sizeof(page->d_records[j].d_links[1]));
        }
    }

    std::memcpy(&getHeader()->d_stateData[dstIdx],
                &getHeader()->d_stateData[srcIdx],
                sizeof(getHeader()->d_stateData[0]));

    getHeader()->d_commitState = STABLE;
}


// MANIPULATORS
int bcecs_PersistentJournal::openJournal(const char *file,
                                         Mode        mode,
                                         bool        createIfNotFound,
                                         int         userDataSize,
                                         int         confirmedRecordsKept,
                                         int         confirmedBytesKept,
                                         int         blockSize,
                                         int         blocksPerPage,
                                         int         alignmentSize)
{

    int res =0;

    if (bcecs_PersistentJournalFileUtil::BAD_FD != d_fd) return BAD_STATE;

    // Ignore createIfNotFound in READ_ONLY mode
    if (READ_ONLY == mode) createIfNotFound = false;

    bool newFile = false;

    d_lock.lockWrite();
    ReaderWriterLockGuard guard(&d_lock);

    bcecs_PersistentJournalFileUtil::FileDescriptor fd;


    if (bcecs_PersistentJournalFileUtil::BAD_FD == (fd = bcecs_PersistentJournalFileUtil::open(file, 
                                                     mode == READ_WRITE, 
                                                     true))) 
    {
        if (!createIfNotFound) return FILE_NOT_EXISTS;


        fd = bcecs_PersistentJournalFileUtil::open(file, mode == READ_WRITE, false);

        if (bcecs_PersistentJournalFileUtil::BAD_FD == fd) return FILE_CREATION_FAILED;

        newFile = true;
        if ((res = initializeJournal(fd, blockSize,
                              blocksPerPage,
                              userDataSize,
                              alignmentSize,
                              confirmedRecordsKept,
                              confirmedBytesKept)))
        {
            bcecs_PersistentJournalFileUtil::close(fd);
            return res;
        }
    }
    BDE_ASSERT_CPP(fd != bcecs_PersistentJournalFileUtil::BAD_FD);
    res = openJournal(fd, mode);
    if (res) bcecs_PersistentJournalFileUtil::close(fd);

    return res;
}

int bcecs_PersistentJournal::setMaxFileSize(int value)
{
    BDE_ASSERT_CPP(value >= -1);
    if (!isValidMaxFileSize(value)) return INVALID_MAXFILESIZE;

    d_maxFileSize = value;
    return 0;
}



int bcecs_PersistentJournal::close()
{
    int res = 0;
    d_lock.lockWrite();
    ReaderWriterLockGuard guard(&d_lock); assertInvariants();
    

    if (bcecs_PersistentJournalFileUtil::BAD_FD != d_fd) {
        // need to fix as mapping partial pages (for READ_ONLY mode) is yet
        // to be supported.
        // 
        // if (READ_WRITE == d_mode && highestBlockUsed() != -1) {
        //     int highestOffsetUsed = calcBlockOffset(highestBlockUsed(),
        //     *getHeader()) + blockSize();
        //     
        //     ftruncate(d_fd, highestOffsetUsed);
        // }
    
        // char *addr;
        int size;

        // pages
        size = chunkSize(*getHeader());
        for (unsigned int i = 0; i < d_pageTables.size(); ++i) {
            int rc = bcecs_PersistentJournalFileUtil::munmap(d_pageTables[i].first, size);
            if (rc) res = rc;
        }
        d_pageTables.clear();
        
        // header
        size = getHeader()->d_headerSize;
        int rc = bcecs_PersistentJournalFileUtil::munmap(d_header, size);
        if (rc) res = rc;
        d_header.d_addr = NULL;        

        d_dirtyPageIndices.clear();
        rc = bcecs_PersistentJournalFileUtil::close(d_fd);
        BDE_ASSERT_CPP(rc == 0);
        d_fd = bcecs_PersistentJournalFileUtil::BAD_FD;
    }

    return res;
}



int bcecs_PersistentJournal::unlinkRecordFromRecordList(
    bcecs_RecordList           *list, 
    bcecs_JournalRecordHeader* record)
{
    BDE_ASSERT_CPP(record);
    BDE_ASSERT_CPP(list);

    bcecs_JournalRecordHeader *prevRec=0, *nextRec=0;

    const int recIndex = record->d_recordId;

    if (record->d_links[d_workIndex].d_prevRecord != EOL) {
        prevRec = 
            findRecordForUpdate(record->d_links[d_workIndex].d_prevRecord);
        if (!prevRec) return INTERNAL_ERROR;
    }

    if (record->d_links[d_workIndex].d_nextRecord != EOL) {
        nextRec = 
            findRecordForUpdate(record->d_links[d_workIndex].d_nextRecord);
        if (!nextRec) return INTERNAL_ERROR;
    }


    if (prevRec) {
        prevRec->d_links[d_workIndex].d_nextRecord = 
            record->d_links[d_workIndex].d_nextRecord;
    }
    else {
        if (list->d_head != recIndex) return INTERNAL_ERROR;
        list->d_head = record->d_links[d_workIndex].d_nextRecord;
    }
    record->d_links[d_workIndex].d_nextRecord = EOL;


    if (nextRec) {
        nextRec->d_links[d_workIndex].d_prevRecord = 
            record->d_links[d_workIndex].d_prevRecord;
    }
    else {
        if (list->d_tail != recIndex) return INTERNAL_ERROR;
        list->d_tail = record->d_links[d_workIndex].d_prevRecord;
    }
    record->d_links[d_workIndex].d_prevRecord = EOL;

    // udpate counters
    list->d_numBlocks  -= ceiling(record->d_recordLength, blockSize());
    list->d_numRecords -= 1;
    list->d_numBytes   -= record->d_recordLength;

    return 0;
}


int bcecs_PersistentJournal::pushBackRecordToRecordList(
    bcecs_RecordList           *list, 
    bcecs_JournalRecordHeader  *record)
{
    const int recId = record->d_recordId;
    
    record->d_links[d_workIndex].d_nextRecord = EOL;
    record->d_links[d_workIndex].d_prevRecord = list->d_tail;


    // bcecs_JournalRecordHeader *tailRec = 0;
    if (list->d_tail != EOL) {
        bcecs_JournalRecordHeader *tailRec= findRecordForUpdate(list->d_tail);
        if (!tailRec) return INTERNAL_ERROR;
        tailRec->d_links[d_workIndex].d_nextRecord = recId;
    }
    else {
        if (EOL != list->d_head) return INTERNAL_ERROR;
        list->d_head = recId;
    }

    list->d_tail = recId;

    // udpate counters
    list->d_numBlocks  += ceiling(record->d_recordLength, blockSize());
    list->d_numRecords += 1;
    list->d_numBytes   += record->d_recordLength;

    return 0;
}

int bcecs_PersistentJournal::getTailBlock(
    bcecs_JournalRecordHeader** result, bcecs_JournalRecordHeader* record)
{
    while(record->d_links[d_workIndex].d_nextBlock != EOL) {
        record = findRecord(record->d_links[d_workIndex].d_nextBlock);
        if (!record) return INTERNAL_ERROR;
    }

    *result = record;
    return 0;
}


int bcecs_PersistentJournal::commitImpl()
{
    int rc;

    // commit header
    char *addr = (char *) getHeader();
    int size = getHeader()->d_headerSize;
    rc = bcecs_PersistentJournalFileUtil::msync(addr, size);
    if (rc) return SYNC_FAILURE;

    // commit dirty pages
    typedef std::set<int>::iterator It;
    for (It itr=d_dirtyPageIndices.begin();
         itr != d_dirtyPageIndices.end(); ++itr) 
    {
        int pageIdx = *itr;
        char *addr = (char *) d_pageTables[pageIdx].first.addr();
        int    len = chunkSize(*getHeader());

        rc = bcecs_PersistentJournalFileUtil::msync(addr, len);
        if (rc) return SYNC_FAILURE;
        
    }
    d_dirtyPageIndices.clear();
    return 0;

}

int bcecs_PersistentJournal::reserveBlocks(int numBlocks) {
    bcecs_JournalStateHeader *state = stateHeader();
    (void) state;  // Value is not used
    while (numFreeBlocks() < numBlocks) {
        int rc = growJournal();
        if (rc) return rc;
    }
    return 0;
}




int bcecs_PersistentJournal::allocateBlocks(
    bcecs_JournalRecordHeader **recPP,
    bcecs_FreeBlockList *list,
    int numBlocks,
    int *highestBlockUsed)
{
    int res;

    // grow if needed
    res = reserveBlocks(numBlocks); 
    if (res) return res;

    BDE_ASSERT_CPP(list->d_numBlocks >= numBlocks);

    *recPP = findRecordForUpdate(list->d_head);  

    const bcecs_JournalRecordHeader* record;

    // find the tail block of retuned record
    *highestBlockUsed = -1;
    int recId = list->d_head;
    for (int i = 1; i < numBlocks; ++i) {
        *highestBlockUsed = std::max(*highestBlockUsed, recId);
        record = findRecord(recId);
        if (!record) return INTERNAL_ERROR;
        
        recId = record->d_links[d_workIndex].d_nextBlock;
    }

    *highestBlockUsed = std::max(*highestBlockUsed, recId);
    bcecs_JournalRecordHeader* rec = findRecordForUpdate(recId);



    list->d_head = rec->d_links[d_workIndex].d_nextBlock;

    rec->d_links[d_workIndex].d_nextBlock = EOL;
    list->d_numBlocks -= numBlocks;
    return 0;
}



int bcecs_PersistentJournal::freeBlocks(
    bcecs_FreeBlockList *list, bcecs_JournalRecordHeader* recordHead)
{

    bcecs_JournalRecordHeader *recordTail;

    int rc = getTailBlock(&recordTail, recordHead);
    if (rc) return rc;

    return freeBlocks(list, 
                      recordHead, 
                      recordTail, 
                      ceiling(recordHead->d_recordLength, blockSize()));

}

int bcecs_PersistentJournal::freeBlocks(
    bcecs_FreeBlockList *list,
    bcecs_JournalRecordHeader* recordHead, 
    bcecs_JournalRecordHeader* recordTail,
    int numBlocks)
{
    // TBD clean up
    d_dirtyRecords.insert(recordTail); 
    d_dirtyPageIndices.insert(recordTail->d_recordId/blocksPerPage()); 

    recordTail->d_links[d_workIndex].d_nextBlock = list->d_head;
    list->d_head = recordHead->d_recordId;
    list->d_numBlocks += numBlocks;
    return 0;
}


int bcecs_PersistentJournal::confirmRecord(RecordHandle handle)
{
// validate handle, unlink from confirmed list, update record header, put in
// confirmed list, shrink the confirm list if it becomes bigger than its limit
    
    d_lock.lockWrite();
    ReaderWriterLockGuard guard(&d_lock); assertInvariants();
    bcecs_ProtectionModeGuard pmGuard(this);
    bcecs_JournalTransactionGuard tran(this);
    
    bcecs_JournalRecordHeader *record;
    int res;
    
    bcecs_JournalStateHeader *state = stateHeader();
    
    record = findRecordForUpdate(handle);
    if (!record)  return INVALID_RECORD_HANDLE;

    
    int recType = record->d_links[d_workIndex].d_recordType;
    if (recType != UNCONFIRMED_RECORD)
        return INVALID_RECORD_HANDLE;
    
    
    res = unlinkRecordFromRecordList(
        &state->d_recordLists[UNCONFIRMED_RECORD],
                                     record);
    if (res) return res;
    
    
    record->d_links[d_workIndex].d_recordType = CONFIRMED_RECORD;
    
    bdet_Datetime dt = bdetu_SystemTime::nowAsDatetimeGMT();
    record->d_confirmationDate = dateToInt(dt.date());
    record->d_confirmationTime = timeToInt(dt.time());
    
    res = pushBackRecordToRecordList(
        &state->d_recordLists[CONFIRMED_RECORD],
                                     record);
    if (res) return res;
    
    
    while ((maxConfirmedRecordsKept() != -1 && 
            numConfirmedRecords() > maxConfirmedRecordsKept()) 
           ||
           (maxConfirmedBytesKept() != -1 && 
            numConfirmedBytes() > maxConfirmedBytesKept())) 
    {
        int res = removeRecordImpl(confirmedListHead());
        if (res) return res;
    }

    tran.commit();
    return 0;
}


int bcecs_PersistentJournal::commit()
{
    d_lock.lockWrite();
    ReaderWriterLockGuard guard(&d_lock); assertInvariants();

    return commitImpl();
}

int bcecs_PersistentJournal::removeRecord(RecordHandle handle)
{
    d_lock.lockWrite();
    ReaderWriterLockGuard guard(&d_lock); assertInvariants();
    bcecs_ProtectionModeGuard pmGuard(this);
    bcecs_JournalTransactionGuard tran(this);

    int rc = removeRecordImpl(handle);
    if (rc) return rc;

    tran.commit();
    return 0;

}

int bcecs_PersistentJournal::removeRecordImpl(RecordHandle handle)
{
    bcecs_JournalRecordHeader *record;
    int res;

    bcecs_JournalStateHeader *state = stateHeader();
    
    record = findRecordForUpdate(handle);
    if (!record)  return INVALID_RECORD_HANDLE;
    
    int recType = record->d_links[d_workIndex].d_recordType;
    if (recType != CONFIRMED_RECORD && recType != UNCONFIRMED_RECORD)
        return INVALID_RECORD_HANDLE;

    res = unlinkRecordFromRecordList(
        &state->d_recordLists[recType], record);
    if (res) return res;

    record->d_links[d_workIndex].d_recordType = NONE_RECORD;

    return freeBlocks(&state->d_freeList, record);
}





int bcecs_PersistentJournal::addRecord(RecordHandle *handleBuffer,
                                       const bcema_Blob& data)
{
    return addRecord(handleBuffer, true, data, 0, -1);
}

int bcecs_PersistentJournal::addRecord(RecordHandle *handleBuffer,
                                       const char *data,
                                       int length)
{
    return addRecord(handleBuffer, false, bcema_Blob(), data, length);
}

int bcecs_PersistentJournal::addRecord(RecordHandle *handleBuffer,
                                       bool isBlob,
                                       const bcema_Blob& blob,
                                       const char *buf,
                                       int bufLength)
{
// allocate record, update record (copy data, mark, datetime etc), put in
// unconfirmed list

    int length = (isBlob)?  blob.length():  bufLength;
    BDE_ASSERT_CPP(length > 0);
    BDE_ASSERT_CPP(READ_WRITE == d_mode);

    if (bcecs_PersistentJournalFileUtil::BAD_FD == d_fd) return BAD_STATE;
    if (READ_WRITE != d_mode) return -2;

    int res;


    bcecs_JournalStateHeader *state = stateHeader();

    d_lock.lockWrite();
    ReaderWriterLockGuard guard(&d_lock); assertInvariants();
    bcecs_ProtectionModeGuard pmGuard(this);
    bcecs_JournalTransactionGuard tran(this);

    // check if record accomodable 
    if (d_maxFileSize != -1) { 
        if (length > maxRecordSizeForEmptyJournal())
            return MAXRECORDSIZEFOREMPTYJOURNAL_VILOATION;
        
        if (length > maxRecordSize())
            return MAXRECORDSIZE_VILOATION;
    }

    int numBlocksNeeded = (length + (blockSize() - 1)) / blockSize();
    int maxBlkIdx;
    bcecs_JournalRecordHeader *record;
    res = allocateBlocks(&record,
                         &state->d_freeList,
                         numBlocksNeeded, 
                         &maxBlkIdx);

    if (res) return res;
    state->d_highestBlockUsed = std::max(state->d_highestBlockUsed, maxBlkIdx);

    if (isBlob) 
        res = copyToRecord(record, blob);
    else 
        res = copyToRecord(record, buf, length);
    if (res) return res;

    // update d_dirtyPageIndice
    for (const bcecs_JournalRecordHeader *r = record; r; 
           r = findRecord(r->d_links[d_workIndex].d_nextBlock))
    {
        d_dirtyPageIndices.insert(r->d_recordId/blocksPerPage());
    }

    
    record->d_links[d_workIndex].d_recordType = UNCONFIRMED_RECORD;

    bdet_Datetime dt = bdetu_SystemTime::nowAsDatetimeGMT();
    record->d_creationDate = dateToInt(dt.date());
    record->d_creationTime = timeToInt(dt.time());


    res = pushBackRecordToRecordList(
        &state->d_recordLists[UNCONFIRMED_RECORD],
                                     record);
    if (res) return res;

    *handleBuffer = record->d_recordId;

    tran.commit();

    return 0;
}

int bcecs_PersistentJournal::copyFromRecord(
    char *buf, int bufLen, const bcecs_JournalRecordHeader *record) const
{
    int numCharsToCopy = std::min(record->d_recordLength, bufLen);
    int result = numCharsToCopy;

    while (1) {
        int cpyLen = std::min(numCharsToCopy, blockSize());

        std::memcpy(buf, getDataBuffer(record->d_recordId), cpyLen);
        buf            += cpyLen;
        numCharsToCopy -= cpyLen;
        if (numCharsToCopy == 0)
            break;
        else
            BDE_ASSERT_CPP(cpyLen == blockSize());

        record = findRecord(record->d_links[d_workIndex].d_nextBlock);
        if (!record) return INTERNAL_ERROR;
    }
    return result;
}

int bcecs_PersistentJournal::copyToRecord(
    bcecs_JournalRecordHeader **record, int *offset,
    const char *buf, int bufLen)
{
    BDE_ASSERT_CPP(bufLen >= 0);

    while (bufLen > 0) {
        if (*offset == blockSize()) {
            bcecs_JournalRecordHeader *nextRec =
                findRecord((*record)->d_links[d_workIndex].d_nextBlock);
            if (!nextRec) return INTERNAL_ERROR;
            *record = nextRec;
            *offset = 0;
        }
        int cpyLen = std::min(bufLen, blockSize() - *offset);

        std::memcpy(getDataBuffer((*record)->d_recordId) + *offset,
                    buf,
                    cpyLen);

        bufLen   -= cpyLen;
        buf      += cpyLen;
        *offset  += cpyLen;
    }
    return 0;
}


int bcecs_PersistentJournal::copyToRecord(
    bcecs_JournalRecordHeader *record, const bcema_Blob& data)
{

    record->d_recordLength   = data.length();

    bcecs_JournalRecordHeader *rec = record;
    int offset = 0;
    int remainingBytes = data.length();

    for (int i = 0; remainingBytes > 0; ++i) {
        BDE_ASSERT_CPP(i < data.numBuffers());

        const bcema_BlobBuffer& blobBuf = data.buffer(i);
        const char *buf = blobBuf.data();
        int bufLen = std::min(blobBuf.size(), remainingBytes);
        int rc = copyToRecord(&rec, &offset, buf, bufLen);
        if (rc) return rc;

        remainingBytes -= bufLen;
    }
    return 0;
}

int bcecs_PersistentJournal::copyToRecord(
    bcecs_JournalRecordHeader *record,
    const char *data, int length)
{
    record->d_recordLength   = length  ;

    int offset = 0;
    bcecs_JournalRecordHeader *rec = record;

    return copyToRecord(&rec, &offset, data, length);
}


int bcecs_PersistentJournal::growJournal()
{
    bcecs_JournalStateHeader  *state  = stateHeader();

    int numPageTbls = d_pageTables.size(); // can't use numPageTables() here
    int endOfPage = calcPageOffset(numPageTbls+1, *getHeader());

    int res = bcecs_PersistentJournalFileUtil::lseek(d_fd, endOfPage, SEEK_SET);


    if (-1 == res || 1 != bcecs_PersistentJournalFileUtil::write(d_fd, (char*) "", 1)) 
        return IO_ERROR;

    FileMapping fm =
        mapPageData(d_fd,
                    numPageTbls,
                    *getHeader(),
                    READ_WRITE == d_mode);

    bcecs_JournalPageTable *table = (bcecs_JournalPageTable *) fm.addr();
    if (!table) return MEM_ERROR;

    initPageTable(table, blocksPerPage(), numPageTbls);
    char *data = ((char*)table) + sizeof(bcecs_JournalPageTable) +
        sizeof(bcecs_JournalRecordHeader) * (getHeader()->d_blocksPerPage-1);

    d_pageTables.push_back(std::make_pair(fm, data));



    bcecs_JournalRecordHeader *recHead =
        findRecord(table->d_records[0].d_recordId);
    bcecs_JournalRecordHeader *recTail =
        findRecord(table->d_records[blocksPerPage()-1].d_recordId);



    return freeBlocks(&state->d_freeList, recHead, recTail, blocksPerPage());
}

bcecs_PersistentJournal::CommitMode bcecs_PersistentJournal::commitMode() const
{
    return d_commitMode;
}

bcecs_PersistentJournal::Mode bcecs_PersistentJournal::mode() const
{
    return d_mode;
}

std::size_t bcecs_PersistentJournal::userDataLength() const
{
    return getHeader()->d_userDataLength;
}

bcecs_PersistentJournal::ProtectionMode 
bcecs_PersistentJournal::protectionMode() const {
    return d_protectionMode;
}

bcecs_PersistentJournalIterator
bcecs_PersistentJournal::firstConfirmedRecord() const
{
    return bcecs_PersistentJournalIterator(this,
                                           confirmedListHead(),
                                           d_allocator_p);
}

bcecs_PersistentJournalIterator 
bcecs_PersistentJournal::lastConfirmedRecord() const
{
    return bcecs_PersistentJournalIterator(this,
                                           confirmedListTail(),
                                           d_allocator_p);
}

bcecs_PersistentJournalIterator
bcecs_PersistentJournal::firstUnconfirmedRecord()  const
{
    return bcecs_PersistentJournalIterator(this,
                                           unconfirmedListHead(),
                                           d_allocator_p);
}

bcecs_PersistentJournalIterator
bcecs_PersistentJournal::lastUnconfirmedRecord()  const
{
    return bcecs_PersistentJournalIterator(this,
                                           unconfirmedListTail(),
                                           d_allocator_p);
}

bdet_Datetime bcecs_PersistentJournal::journalCreationDateTime() const
{
    return bdet_Datetime(intToDate(getHeader()->d_creationDate),
                         intToTime(getHeader()->d_creationTime));
}


int bcecs_PersistentJournal::blockSize() const
{
    return getHeader()->d_blockSize;
}

int bcecs_PersistentJournal::blocksPerPage() const
{
    return getHeader()->d_blocksPerPage;
}

int bcecs_PersistentJournal::alignmentSize() const
{
    return getHeader()->d_alignmentSize;
}

int bcecs_PersistentJournal::maxConfirmedRecordsKept() const
{
    return getHeader()->d_maxConfirmedRecordsKept;
}

int bcecs_PersistentJournal::maxConfirmedBytesKept() const
{
    return getHeader()->d_maxConfirmedBytesKept;
}

int bcecs_PersistentJournal::numUnconfirmedRecords() const
{
    const bcecs_JournalStateHeader *state = stateHeader();
    return state->d_recordLists[UNCONFIRMED_RECORD].d_numRecords;
}

int bcecs_PersistentJournal::numConfirmedRecords() const
{
    const bcecs_JournalStateHeader *state = stateHeader();
    return state->d_recordLists[CONFIRMED_RECORD].d_numRecords;
}

const char *bcecs_PersistentJournal::userData() const
{
    return (char *) getHeader() + sizeof(bcecs_PersistentJournalHeader);
}


int bcecs_PersistentJournal::maxRecordSize() const
{
    BDE_ASSERT_CPP(getHeader());
    if (d_maxFileSize == -1) return -1;
    
    int maxPagesEver = (d_maxFileSize - getHeader()->d_headerSize) / 
                                                 chunkSize(*getHeader());


    int maxBlocks = numFreeBlocks() + 
        (maxPagesEver-numPageTables()) * blocksPerPage();

    return maxBlocks * blockSize();
}

int bcecs_PersistentJournal::maxRecordSizeForEmptyJournal() const
{
    BDE_ASSERT_CPP(getHeader());
    if (d_maxFileSize == -1) return -1;

    int maxPagesEver = (d_maxFileSize - getHeader()->d_headerSize) / 
                                                 chunkSize(*getHeader());

    return maxPagesEver * blocksPerPage() * blockSize();
}

int bcecs_PersistentJournal::maxFileSize() const
{
    return d_maxFileSize;
}

std::ostream& bcecs_PersistentJournal::printHeader(std::ostream& stream) const
{
    stream << *getHeader() << std::endl << std::endl;

    return stream;
}


                    // =====================================
                    // class bcecs_PersistentJournalIterator
                    // =====================================

// CREATORS
bcecs_PersistentJournalIterator::bcecs_PersistentJournalIterator(
    const bcecs_PersistentJournalIterator& src,
    bdema_Allocator *allocator)
: d_allocator_p(allocator)
, d_journal_p(src.d_journal_p)
, d_current_p(src.d_current_p)
, d_data(src.d_data, allocator)
{
    if (d_journal_p)
        d_journal_p->d_lock.lockRead();
}

bcecs_PersistentJournalIterator::bcecs_PersistentJournalIterator(
    const bcecs_PersistentJournal *journal,
    bcecs_PersistentJournal::RecordHandle position,
    bdema_Allocator *allocator)
    : d_allocator_p(allocator)
    , d_journal_p(journal)
    , d_current_p(0)
    , d_data(allocator)
{
    if (d_journal_p) {
        d_journal_p->d_lock.lockRead();
        d_current_p = d_journal_p->findRecord(position);
    }
}

bcecs_PersistentJournalIterator::~bcecs_PersistentJournalIterator()
{
    if (d_journal_p)
        d_journal_p->d_lock.unlock();
}


// MANIPULATORS
bcecs_PersistentJournalIterator& bcecs_PersistentJournalIterator::operator=(
                       const bcecs_PersistentJournalIterator& src)
{
    if (&src == this) return *this;

    if (d_journal_p)
        d_journal_p->d_lock.unlock();



    d_journal_p = src.d_journal_p;
    if (d_journal_p)
        d_journal_p->d_lock.lockRead();
    d_current_p = src.d_current_p;
    d_data.removeAll();

    return *this;

}

bcecs_PersistentJournalIterator& bcecs_PersistentJournalIterator::operator++()
{
    BDE_ASSERT_CPP(d_journal_p && d_current_p);
    d_data.removeAll();
    if (d_current_p) {
        int nextRecId =
                   d_current_p->d_links[d_journal_p->d_workIndex].d_nextRecord;

        if (nextRecId == EOL)
            d_current_p = NULL;
        else
            d_current_p = d_journal_p->findRecord(nextRecId);
    }
    return *this;
}

bcecs_PersistentJournalIterator& bcecs_PersistentJournalIterator::operator--()
{
    BDE_ASSERT_CPP(d_journal_p && d_current_p);
    d_data.removeAll();
    if (d_current_p) {
        int prevRecId =
                   d_current_p->d_links[d_journal_p->d_workIndex].d_prevRecord;

        if (prevRecId == EOL)
            d_current_p = NULL;
        else
            d_current_p = d_journal_p->findRecord(prevRecId);
    }
    return *this;
}



// ACCESSORS
bcecs_PersistentJournalIterator::RecordHandle
bcecs_PersistentJournalIterator::operator*() const
{
    BDE_ASSERT_CPP(d_journal_p && d_current_p);
    return d_current_p->d_recordId;
}

bdet_Datetime bcecs_PersistentJournalIterator::creationDateTime() const
{
    BDE_ASSERT_CPP(d_journal_p && d_current_p);
    return bdet_Datetime(intToDate(d_current_p->d_creationDate),
                         intToTime(d_current_p->d_creationTime));
}

bdet_Datetime bcecs_PersistentJournalIterator::confirmationDateTime() const
{
    BDE_ASSERT_CPP(d_journal_p && d_current_p);
    return bdet_Datetime(intToDate(d_current_p->d_confirmationDate),
                         intToTime(d_current_p->d_confirmationTime));
}

int bcecs_PersistentJournalIterator::recordLength() const
{
    BDE_ASSERT_CPP(d_journal_p && d_current_p);
    return d_current_p->d_recordLength;
}

const bcema_Blob& bcecs_PersistentJournalIterator::data() const
{
    BDE_ASSERT_CPP(d_journal_p && d_current_p);
    if (!d_data.length()) {
        if (!d_blobBufferSP.ptr()) {
            d_blobBufferSP.load((void*)this,
                                bcema_SharedPtrNilDeleter(),
                                0);
        }
        const bcecs_JournalRecordHeader *rec = d_current_p;
        int   len  = rec->d_recordLength;
        int   savedLen  = len;

        while (rec && len) {
            int cpyLen = std::min(d_journal_p->blockSize(), len);
            const char *buf   = d_journal_p->getDataBuffer(rec->d_recordId);

            bcema_BlobBuffer bb(
                bcema_SharedPtr<char>(d_blobBufferSP, 
                                      const_cast<char *> (buf)),
                cpyLen);

            d_data.appendBuffer(bb);

            len -= cpyLen;

            rec = d_journal_p->findRecord(
                rec->d_links[d_journal_p->d_workIndex].d_nextBlock);
        }

        BDE_ASSERT_CPP(len == 0);
    d_data.setLength(savedLen);
    }
    return d_data;

}


int bcecs_PersistentJournalIterator::data(char *buf, int bufLen) const
{
    BDE_ASSERT_CPP(d_journal_p && d_current_p);
    return d_journal_p->copyFromRecord(buf, bufLen, d_current_p);
}

bcecs_PersistentJournalIterator::operator bool() const
{
    return d_journal_p && d_current_p;
}

bool bcecs_PersistentJournalIterator::isConfirmed() const
{
    BDE_ASSERT_CPP(d_journal_p && d_current_p);
    int recType = d_current_p->d_links[d_journal_p->d_workIndex].d_recordType;
    return recType == CONFIRMED_RECORD;

}
}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2006
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------


