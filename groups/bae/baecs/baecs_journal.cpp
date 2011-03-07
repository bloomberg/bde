// baecs_journal.cpp                                                  -*-C++-*-
#include <baecs_journal.h>
#include <baecs_journalpageheader.h>
#include <bael_log.h>

#include <bcema_blobutil.h>
#include <bcema_sharedptr.h>

#include <bdede_crc32.h>
#include <bdef_bind.h>
#include <bdesu_fileutil.h>
#include <bdesu_memoryutil.h>
#include <bdet_timeinterval.h>
#include <bdetu_epoch.h>
#include <bdetu_systemtime.h>
#include <bdeu_print.h>
#include <bdeu_printmethods.h>
#include <bdeut_bigendian.h>

#include <bslmf_assert.h>
#include <bsls_assert.h>

#include <bsl_iomanip.h>

#include <bsl_cstdio.h>
#include <bsl_cstring.h>
#include <bsl_c_string.h>
#include <bsl_c_stdio.h>
#include <bsl_c_stdlib.h>

/*
    IMPLEMENTATION NOTES
    --------------------

    The persistent journal is implemented as a collection of data *pages*
    subdivided into *blocks*.  A record is stored as a singly linked list of
    *record segments*, each segment being a singly linked list of blocks
    allocated on the same page.  As the page size is usually much larger than
    an average record size, most records have just one segment.  A record
    having more than one segment is called an *extended record*.  The first
    segment of the record is called *head segment*, and a segment which is not
    the last is called an *extended segment*.  The first block of the head
    segment is called *head block*.

    Information on block linkage within segments is stored in a *page header*.
    The blocks which are not allocated (i.e., the ones not belonging to record
    segments) are also linked together in two free block lists: *released
    list* and *available list*.  Released list is used for transaction
    support, this will be described later.  Every block on a page must belong
    either to one of the segments, to the released list, or to the available
    list.

    Blocks are numbered sequentially within the journal, so you can easily
    find the page index and the block index within a page by dividing the
    global block index by the number of blocks per page.  The index of the
    first block in a record is used as a *record handle*.

    Records in the journal are linked together in two linked lists: list of
    confirmed records and list of unconfirmed records.  The page header stores
    the record handle of the next and previous record for every head record
    block, so you never need to access the actual data to iterate over the
    records.  For non-head blocks these fields are not used.

    The first block of every record segment holds a *segment header*.  The
    header contains the record size (actually, the size not including the
    previous segments, so the exact size is stored only in the head segment),
    and the index of the head block of the next segment, if any.  It also
    contains the record handle (which may help in data recovery if the journal
    gets corrupted), and data checksum (crc32 of the whole record, in head
    segment header only, in paranoid mode only).

    If the number of available blocks on a page raises above a certain
    threshold, it DIES? TBD.
*/

namespace BloombergLP {

namespace {

const char LOG_CATEGORY[] = "BAECS.JOURNAL";

unsigned static calculateChecksum(const bcema_Blob *blob,
                                  void             *data,
                                  unsigned          size)
{
    bdede_Crc32 checksum;
    if (blob) {
        int i;
        for (i = 0; i < blob->numDataBuffers() - 1; ++i) {
            checksum.update(blob->buffer(i).data(), blob->buffer(i).size());
        }
        if (blob->numDataBuffers() > 0) {
            checksum.update(blob->buffer(i).data(),
                            blob->lastDataBufferLength());
        }
    }
    else {
        checksum.update(data, size);
    }
    return checksum.checksum();
}

inline
bsls_PlatformUtil::Int64 nowAsInt64GMT()
{
    bdet_TimeInterval now;
    bdetu_SystemTime::loadSystemTimeDefault(&now);
    return now.seconds() * 1000000000 + now.nanoseconds();
}

inline
bdet_Datetime int64ToDatetime(bsls_PlatformUtil::Int64 ns)
{
    bdet_Datetime result = bdetu_Epoch::epoch();
    result.addMilliseconds(ns / 1000000);
    return result;
}

struct PrintSize {
    // TBD: it would be nice to move this to BDE
    bdesu_FileUtil::Offset d_size;
    PrintSize(bdesu_FileUtil::Offset size)
        : d_size(size)
    {};
};

inline
bsl::ostream& operator<< (bsl::ostream& stream, const PrintSize& rhs)
{
    double size = rhs.d_size;
    if (size<=0) {
        stream << "-";
        size = -size;
    };
    const char* suffix = "";
    if (size >= 1024) { suffix = "Ki"; size/=1024; }
    if (size >= 1024) { suffix = "Mi"; size/=1024; }
    if (size >= 1024) { suffix = "Gi"; size/=1024; }
    if (size >= 1024) { suffix = "Ti"; size/=1024; }
    return stream << size << suffix << "B";
}

}

#define J_ASSERT(cond) { if(!(cond)) { \
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY); \
    BAEL_LOG_ERROR << d_filename << " [ASSERTION FAILED: " << #cond << "]" \
                   << BAEL_LOG_END; \
    return -1; }  }

#define J_ASSERT1(cond, v1) { if(!(cond)) { \
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY); \
    BAEL_LOG_ERROR << d_filename << "[ASSERTION FAILED: " << #cond \
                   << "|" << #v1 << " == " << (v1) << "]" \
                   << BAEL_LOG_END; \
    return -1; }  }

#define J_ASSERT2(cond, v1, v2) { if(!(cond)) { \
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY); \
    BAEL_LOG_ERROR << d_filename << "[ASSERTION FAILED: " << #cond \
                   << "|" << #v1 << " == " << (v1) << "]" \
                   << "|" << #v2 << " == " << (v2) << "]" \
                   << BAEL_LOG_END; \
    return -1; }  }

// ============================================================================

                      // =======================================
                      // class baecs_Journal_PageHeaderReadGuard
                      // =======================================

class baecs_Journal_PageHeaderReadGuard {
    // DATA
    const baecs_Journal           *d_journal_p;
    unsigned                       d_page;
    const baecs_JournalPageHeader *d_header_p;

    // NOT IMPLEMENTED
    baecs_Journal_PageHeaderReadGuard(
                                     const baecs_Journal_PageHeaderReadGuard&);
    baecs_Journal_PageHeaderReadGuard& operator=(
                                     const baecs_Journal_PageHeaderReadGuard&);
  public:
    // CREATORS
    baecs_Journal_PageHeaderReadGuard(const baecs_Journal *journal,
                                      unsigned             page);
    ~baecs_Journal_PageHeaderReadGuard();

    // ACCESSORS
    const baecs_JournalPageHeader *operator->() const;
    operator bool() const;
};

// CREATORS
baecs_Journal_PageHeaderReadGuard::baecs_Journal_PageHeaderReadGuard(
                                                  const baecs_Journal *journal,
                                                  unsigned             page)
: d_journal_p(journal)
, d_page(page)
, d_header_p(journal->getPageHeader(page, false))
{
}

baecs_Journal_PageHeaderReadGuard::~baecs_Journal_PageHeaderReadGuard()
{
    d_journal_p->releasePageHeader(d_page);
}

// ACCESSORS
inline
baecs_Journal_PageHeaderReadGuard::operator bool() const
{
   return d_header_p;
}

inline
const baecs_JournalPageHeader*
baecs_Journal_PageHeaderReadGuard::operator->() const
{
    return d_header_p;
}

// ============================================================================

                      // ========================================
                      // class baecs_Journal_PageHeaderWriteGuard
                      // ========================================

class baecs_Journal_PageHeaderWriteGuard {
    // DATA
    baecs_Journal           *d_journal_p;
    unsigned                 d_page;
    baecs_JournalPageHeader *d_header_p;

  public:
    // CREATORS
    baecs_Journal_PageHeaderWriteGuard(baecs_Journal *journal,
                                       unsigned       page);
    ~baecs_Journal_PageHeaderWriteGuard();

    // ACCESSORS
    baecs_JournalPageHeader *operator->() const;
    operator bool() const;
};

// CREATORS
baecs_Journal_PageHeaderWriteGuard::baecs_Journal_PageHeaderWriteGuard(
                                                        baecs_Journal *journal,
                                                        unsigned       page)
: d_journal_p(journal)
, d_page(page)
, d_header_p(journal->getPageHeader(page, true))
{
}

// ACCESSORS
inline
baecs_Journal_PageHeaderWriteGuard::operator bool() const
{
   return d_header_p;
}

baecs_Journal_PageHeaderWriteGuard::~baecs_Journal_PageHeaderWriteGuard()
{
    d_journal_p->releasePageHeader(d_page);
}

inline
baecs_JournalPageHeader*
baecs_Journal_PageHeaderWriteGuard::operator->() const
{
    return d_header_p;
}

// ============================================================================

                      // =============================
                      // class baecs_Journal_LockGuard
                      // =============================

class baecs_Journal_LockGuard {
    // This class provides a read/write lock guard specialized for journal
    // objects.

    // DATA
    baecs_Journal *d_journal_p;  // guarded journal

    // NOT IMPLEMENTED
    baecs_Journal_LockGuard(const baecs_Journal_LockGuard&);
    baecs_Journal_LockGuard& operator=(const baecs_Journal_LockGuard&);

  public:
    // CREATORS
    baecs_Journal_LockGuard(baecs_Journal *journal, bool write = false);
        // Create a lock guard for the specified 'journal' and
        // lock 'journal' for writing if write==true, and for reading
        // otherwise.

    ~baecs_Journal_LockGuard();
        // Unlock the underlying journal and destroy this lock guard.

    // MANIPULATORS
    void relock(bool write = false);
        // Unlock the underlying journal and relock it for reading/writing
        // as specified.
};

// CREATORS
inline
baecs_Journal_LockGuard::baecs_Journal_LockGuard(baecs_Journal *journal,
                                                 bool           write)
: d_journal_p(journal)
{
    BSLS_ASSERT(journal);
    BSLS_ASSERT(d_journal_p->d_fd != bdesu_FileUtil::INVALID_FD);
    BSLS_ASSERT(d_journal_p->d_mode & baecs_Journal::BAECS_READWRITE);

    if (write) {
        d_journal_p->d_lock.lockWrite();
    }
    else {
        d_journal_p->d_lock.lockRead();
    }
}

inline
baecs_Journal_LockGuard::~baecs_Journal_LockGuard()
{
    d_journal_p->d_lock.unlock();
}

// MANIPULATORS
inline
void baecs_Journal_LockGuard::relock(bool write)
{
    d_journal_p->d_lock.unlock();
    if (write) {
        d_journal_p->d_lock.lockWrite();
    }
    else {
        d_journal_p->d_lock.lockRead();
    }
}

// ============================================================================

                      // =================================
                      // class baecs_Journal_ReadLockGuard
                      // =================================

class baecs_Journal_ReadLockGuard {
    // This class provides a write lock guard specialized for journal objects.

    // DATA
    const baecs_Journal *d_journal_p;  // guarded journal

    // NOT IMPLEMENTED
    baecs_Journal_ReadLockGuard(const baecs_Journal_ReadLockGuard&);
    baecs_Journal_ReadLockGuard& operator=(const baecs_Journal_ReadLockGuard&);

  public:
    // CREATORS
    baecs_Journal_ReadLockGuard(const baecs_Journal *journal);
        // Create a lock guard for the specified 'journal' and
        // lock 'journal' for reading

    ~baecs_Journal_ReadLockGuard();
        // Unlock the underlying journal and destroy this lock guard.

    // MANIPULATORS
    void relock(bool write = false);
};

// CREATORS
inline
baecs_Journal_ReadLockGuard::baecs_Journal_ReadLockGuard(
                                                  const baecs_Journal *journal)
: d_journal_p(journal)
{
    BSLS_ASSERT(d_journal_p);
    BSLS_ASSERT(d_journal_p->d_fd != bdesu_FileUtil::INVALID_FD);
    d_journal_p->d_lock.lockRead();
}

// MANIPULATORS
inline
baecs_Journal_ReadLockGuard::~baecs_Journal_ReadLockGuard()
{
    d_journal_p->d_lock.unlock();
}

// ===========================================================================
BSLMF_ASSERT(sizeof(baecs_JournalHeader::OnDisk)        == 256);
// ============================================================================

                      // ==================================
                      // struct baecs_Journal_SegmentHeader
                      // ==================================

struct baecs_Journal_SegmentHeader {

    // DATA
    bsls_PlatformUtil::Int64  d_timestamp;    // timestamp of db update
                                              // for a head record it also
                                              // indicates creation time

    bdeut_BigEndianUint32     d_reserved1;    // not in use now
    bdeut_BigEndianUint32     d_headSegment;
    bdeut_BigEndianUint32     d_nextSegment;

    bdeut_BigEndianUint32     d_size;         // size of the record in
                                              // bytes

    bdeut_BigEndianUint32     d_checksum;     // for head only, debug mode
                                              // only
    bdeut_BigEndianUint32     d_reserved2;
};

BSLMF_ASSERT(sizeof(baecs_Journal_SegmentHeader) == 32);

// ============================================================================

                      // ============================
                      // class baecs_Journal_BlockRef
                      // ============================

class baecs_Journal_BlockRef {

    // DATA
    const baecs_Journal                  *d_journal_p;
    unsigned                              d_page;
    baecs_JournalPageHeader::BlockHeader *d_blockHeader;

  public:
    // CREATORS
    baecs_Journal_BlockRef(const baecs_Journal *journal,
                           unsigned             handle,
                           bool                 markDirty)
          : d_journal_p (journal)
          , d_page(0)
          , d_blockHeader(NULL)
    {
        BSLS_ASSERT(handle != baecs_Journal::BAECS_INVALID_RECORD_HANDLE);
        unsigned block;
        d_journal_p->handle2PageBlock(&d_page, &block, handle);
        BSLS_ASSERT(block != baecs_JournalPageHeader::BAECS_INDEX_NONE);

        baecs_JournalPageHeader *pageHeader =
                journal->getPageHeader(d_page, markDirty);
        d_blockHeader = pageHeader->block(block);
    }

    ~baecs_Journal_BlockRef()
    {
        d_journal_p->releasePageHeader(d_page);
    }

    // MANIPULATORS
    baecs_JournalPageHeader::BlockHeader* operator->()
    {
        return d_blockHeader;
    }

    // ACCESSORS
    const baecs_JournalPageHeader::BlockHeader* operator->() const
    {
        return d_blockHeader;
    }
};

BSLMF_ASSERT(sizeof(baecs_JournalPageHeader) == 112);

// ============================================================================

                        // -------------------
                        // class baecs_Journal
                        // -------------------

// CREATORS
baecs_Journal::baecs_Journal(baecs_MappingManager *mappingManager,
                             bslma_Allocator      *allocator)
: d_mappingManager_p(mappingManager)
, d_fd(bdesu_FileUtil::INVALID_FD)
, d_pageSetHeaderHandles(bslma_Default::allocator(allocator))
, d_pageHandles(bslma_Default::allocator(allocator))
, d_dirtyListHandle(mappingManager->createDirtyList())
, d_diskSpaceWarningThreshold(0)
, d_diskSpaceErrorThreshold(0)
, d_poolAllocator(bslma_Default::allocator(allocator))
, d_allocator_p(bslma_Default::allocator(allocator))
{
    BSLS_ASSERT(d_mappingManager_p != NULL);
    BSLS_ASSERT(d_dirtyListHandle);
    BSLS_ASSERT(BAECS_NUM_PRIORITIES <=
                                      d_mappingManager_p->numPriorityLevels());
}

baecs_Journal::~baecs_Journal()
{
    if (d_fd != bdesu_FileUtil::INVALID_FD) {
        close();
    }
    d_mappingManager_p->deleteDirtyList(d_dirtyListHandle);
}

// MANIPULATORS
int baecs_Journal::openImpl(const char                   *filename,
                            unsigned                      mode)
{
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);
    char* senvmode = getenv("BAECS_JOURNAL_FORCEMODE");
    if (senvmode) {
        int envmode = atoi(senvmode);
        BAEL_LOG_ERROR
            << "FORCED MODE " << envmode << "(old mode " << mode
            << ", new mode " << (mode | envmode) << ")"
            << BAEL_LOG_END;
        mode |= envmode;
    }
    bool isReadWrite = (mode & BAECS_READWRITE) != 0;
    FileDescriptor fd = bdesu_FileUtil::open(filename, isReadWrite, true);
    if (fd == bdesu_FileUtil::INVALID_FD) {
        return BAECS_FILE_NOT_FOUND_ERROR;
    }
    if (0 != bdesu_FileUtil::tryLock(fd, isReadWrite)) {
        BAEL_LOG_ERROR << "Cannot open " << filename
            << ": cannot lock the file"
            << BAEL_LOG_END;
        bdesu_FileUtil::close(fd);
        return BAECS_UNABLE_TO_LOCK_ERROR;
    }

    struct {
        // TBD: move to header component.
        bdeut_BigEndianUint32    d_magic;
        bdeut_BigEndianUint32    d_version;
        bdeut_BigEndianUint32    d_headerSize;
    } data;
    int n = bdesu_FileUtil::read(fd, &data, sizeof(data));

    if (n != sizeof(data)) {
        BAEL_LOG_ERROR << "Cannot open " << filename
            << ": failed to read header (" << sizeof(data)
            << "bytes)"
            << BAEL_LOG_END;
        bdesu_FileUtil::close(fd);
        return BAECS_IO_ERROR;
    }
    if (data.d_magic != baecs_JournalHeader::BAECS_JOURNAL_MAGIC_NUMBER) {
        BAEL_LOG_ERROR << "Cannot open " << filename
            << ": magic number mismatch (found:"
            << bsl::hex << data.d_magic << " expected:"
            << bsl::hex << baecs_JournalHeader::BAECS_JOURNAL_MAGIC_NUMBER
            << ")"
            << BAEL_LOG_END;
        bdesu_FileUtil::close(fd);
        return BAECS_FORMAT_ERROR;
    }
    if (data.d_version != baecs_JournalHeader::BAECS_JOURNAL_VERSION_CURRENT) {
        BAEL_LOG_ERROR << "Cannot open " << filename
            << ": unsupported journal version found: "
            << bsl::hex << data.d_version
            << BAEL_LOG_END;
        bdesu_FileUtil::close(fd);
        return BAECS_UNSUPPORTED_VERSION_ERROR;
    }
    unsigned headerSize = data.d_headerSize;
    unsigned alignment = (unsigned)bdesu_MemoryUtil::pageSize();
    if ((headerSize/alignment)*alignment != headerSize) {
        BAEL_LOG_ERROR << "Cannot open " << filename
            << ": header size " << headerSize << " is not supported "
               "on this platform, must be a multiple of " << alignment
            << BAEL_LOG_END;
        bdesu_FileUtil::close(fd);
        return BAECS_ALIGNMENT_ERROR;
    }

    d_headerHandle = d_mappingManager_p->addPage(fd,
                                                 0,
                                                 headerSize,
                                                 isReadWrite,
                                                 BAECS_PRIORITY_METADATA);

    d_headerPage = d_mappingManager_p->usePage(d_headerHandle);

    if (!d_headerPage)
    {
        d_mappingManager_p->removePage(d_headerHandle);
        d_headerHandle = baecs_MappingManager::INVALID_HANDLE;
        BAEL_LOG_ERROR << "Cannot open " << filename
                       << ": " << errorMessage(BAECS_MMAP_ERROR)
                       << BAEL_LOG_END;
        bdesu_FileUtil::close(fd);
        return BAECS_MMAP_ERROR;
    }
    d_header.setPersistentStore(d_headerPage);

    if ((unsigned) d_header.alignment() < alignment) {
        BAEL_LOG_ERROR << "Cannot open " << filename
            << ": journal alignment of " << d_header.alignment()
            << " is not supported on this platform, must be a multiple of "
            << alignment
            << BAEL_LOG_END;
        d_mappingManager_p->removePage(d_headerHandle);
        d_headerHandle = baecs_MappingManager::INVALID_HANDLE;
        bdesu_FileUtil::close(fd);
        return BAECS_ALIGNMENT_ERROR;
    }

    BAEL_LOG_TRACE << "Header = " << d_header << BAEL_LOG_END;

    if (d_header.recoverTransaction(mode & BAECS_SAFE)) { // TRT
        BAEL_LOG_ERROR << "Cannot open " << filename
            << ": failed to recover transaction"
            << BAEL_LOG_END;
        d_mappingManager_p->removePage(d_headerHandle);
        d_headerHandle = baecs_MappingManager::INVALID_HANDLE;
        bdesu_FileUtil::close(fd);
        return BAECS_VALIDATION_ERROR;
    }

    d_fd       = fd;
    d_mode     = mode;
    d_filename = filename;
    int rc     = growJournal(false);

    if (rc) { // TRT
        BAEL_LOG_ERROR << "Cannot open " << filename
            << ": growJournal failed"
            << BAEL_LOG_END;
        d_mappingManager_p->removePage(d_headerHandle);
        d_headerHandle = baecs_MappingManager::INVALID_HANDLE;
        bdesu_FileUtil::close(fd);
        return BAECS_VALIDATION_ERROR;
    }

    // Validate the journal:
    // set the mode to read-only, revert after validation
    // validate the journal in non-verbose mode.
    d_mode = BAECS_READONLY;
    if (validateImpl(false)) {
        BAEL_LOG_ERROR << "Validation of " << d_filename << " failed."
                       << BAEL_LOG_END;
        // Close the journal so it can be properly reopened.
        closeImpl();
        return BAECS_VALIDATION_ERROR;
    }
    d_mode = mode;

    if (d_mode & BAECS_SAFE) {
        beginTransaction();
    }
    return 0;
}

int baecs_Journal::open(const char *filename,
                        unsigned    mode)
{
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);
    if (d_fd != bdesu_FileUtil::INVALID_FD) {
        BAEL_LOG_ERROR << "Cannot open " << filename
                       << ": journal is already open"
                       << BAEL_LOG_END;
        return BAECS_INVALID_STATE_ERROR;
    }

    // 'openImpl' logs all errors except "file not found", so we must log
    // this error here.
    int rc = openImpl(filename, mode);
    if (rc == BAECS_FILE_NOT_FOUND_ERROR) {
        BAEL_LOG_ERROR << "Cannot open " << filename
                       << ": file not found"
                       << BAEL_LOG_END;
    }

    if (rc) {
        return rc;
    }

    if (d_mode & BAECS_PARANOID) {
        d_debugRecordMap.clear();
        for (RecordHandle h  = firstUnconfirmedRecord();
                         h != BAECS_INVALID_RECORD_HANDLE;
                         h  = nextUnconfirmedRecord(h)) {

            bsl::pair<DebugRecordMap::iterator, bool> rc =
                d_debugRecordMap.insert(bsl::make_pair(h, 0));
            if (!rc.second) {
                BAEL_LOG_ERROR << "duplicate record " << h
                               << " when opening " << filename
                               << BAEL_LOG_END;
                 BSLS_ASSERT(0);
            }
        }

        for (RecordHandle h  = firstConfirmedRecord();
                         h != BAECS_INVALID_RECORD_HANDLE;
                         h  = nextConfirmedRecord(h)) {

            bsl::pair<DebugRecordMap::iterator, bool> rc =
                d_debugRecordMap.insert(
                    bsl::make_pair(h, (int)BAECS_RECORD_CONFIRMED));
            if (!rc.second) {
                BAEL_LOG_ERROR << "duplicate record " << h
                    << " when opening " << filename
                    << BAEL_LOG_END;
                 BSLS_ASSERT(0);
            }
        }
    }
    return 0;
}

int
baecs_Journal::create(
        const char                     *filename,
        unsigned                        mode,
        unsigned                        userDataSize,
        const baecs_JournalParameters&  param)
{

    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);
    enum {
        MIN_ALIGNMENT = 65536
    };

    if (d_fd != bdesu_FileUtil::INVALID_FD) {
        BAEL_LOG_ERROR << "Cannot open " << filename
            << ": journal is already open"
            << BAEL_LOG_END;
        return BAECS_INVALID_STATE_ERROR;
    }

    char* senvmode = getenv("BAECS_JOURNAL_FORCEMODE");

    if (senvmode) {
        int envmode = atoi(senvmode);
        BAEL_LOG_ERROR
            << "FORCED MODE " << envmode << "(old mode " << mode
            << ", new mode " << (mode | envmode) << ")"
            << BAEL_LOG_END;
        mode |= envmode;
    }

    bool isReadWrite = (mode & BAECS_READWRITE) != 0;
    int rc = openImpl(filename, mode);
    if (rc != BAECS_FILE_NOT_FOUND_ERROR) {
        // this condition includes all errors except file not found
        // AND SUCCESS.  errors were logged already.
        return rc;
    }

    if (!(mode & BAECS_READWRITE)) {
        BAEL_LOG_ERROR << "Cannot open " << filename
            << ": file not found"
            << BAEL_LOG_END;
        return BAECS_FILE_NOT_FOUND_ERROR;
    }

    if (param.blockSize() < (int) sizeof(baecs_Journal_SegmentHeader)) {
        BAEL_LOG_ERROR << "Cannot open " << filename
            << ": invalid block size specified, must be at least "
            << sizeof(baecs_Journal_SegmentHeader)
            << BAEL_LOG_END;
        return BAECS_INVALID_PARAMETERS_ERROR;
    }

    FileDescriptor fd = bdesu_FileUtil::open(filename, true, false);
    if (fd == bdesu_FileUtil::INVALID_FD) {
        BAEL_LOG_ERROR << "Cannot open " << filename
                       << ": cannot create file"
                       << BAEL_LOG_END;
        return BAECS_IO_ERROR;
    }

    if (0 != bdesu_FileUtil::tryLock(fd, isReadWrite)) {
        BAEL_LOG_ERROR << "Cannot open " << filename
            << ": cannot lock the file"
            << BAEL_LOG_END;
        bdesu_FileUtil::close(fd);
        return BAECS_UNABLE_TO_LOCK_ERROR;
    }

    d_filename          = filename;
    unsigned headerSize = sizeof(baecs_JournalHeader::OnDisk) + userDataSize;
    unsigned alignment  = bsl::max(bdesu_MemoryUtil::pageSize(),
                                   (int)MIN_ALIGNMENT);

    headerSize = (headerSize + alignment - 1) & ~(alignment - 1);

    unsigned pageHeaderSize =
            baecs_JournalPageHeader::getSize(param.blocksPerPage());
    pageHeaderSize = (pageHeaderSize + alignment - 1) & ~(alignment - 1);
    unsigned pageDataSize =
            (param.blocksPerPage() * param.blockSize() + alignment - 1)
                                                            & ~(alignment - 1);
    rc = growFile(fd, headerSize, (mode & BAECS_RESERVE) != 0);

    if (rc) {
        return rc; // errors are already logged
    }

    d_headerHandle = d_mappingManager_p->addPage(fd,
                                                 0,
                                                 headerSize,
                                                 true,
                                                 BAECS_PRIORITY_METADATA);
    d_headerPage = d_mappingManager_p->usePage(d_headerHandle);

    if (!d_headerPage) {
        d_mappingManager_p->removePage(d_headerHandle);
        BAEL_LOG_ERROR << "Cannot open " << filename
                       << ": " << errorMessage(BAECS_MMAP_ERROR)
                       << BAEL_LOG_END;
        bdesu_FileUtil::close(fd);
        return BAECS_MMAP_ERROR;
    }
    d_header.setPersistentStore(d_headerPage);

    d_header.init(headerSize, pageHeaderSize, pageDataSize, alignment,
                     userDataSize, param);

    if (!(mode & BAECS_SAFE)) {
        // If SAFE mode is not used, mark the current transaction as committed
        // in advance.
        d_header.commitCurrentTransaction();
    }

    d_mode = mode;
    d_fd   = fd;

    return 0;
}

int baecs_Journal::closeImpl()
{
    for (unsigned i = 0; i < d_pageSetHeaderHandles.size(); i++) {
        d_mappingManager_p->removePage(d_pageSetHeaderHandles[i]);
    }
    d_pageSetHeaderHandles.clear();
    for (unsigned i = 0; i < d_pageHandles.size(); i++) {
        d_mappingManager_p->removePage(d_pageHandles[i]);
    }
    d_pageHandles.clear();
    d_pageWorkIndexes.clear();

    d_mappingManager_p->removePage(d_headerHandle);
    // TBD d_header_p = 0;
    d_debugRecordMap.clear();

    bdesu_FileUtil::close(d_fd);
    d_fd = bdesu_FileUtil::INVALID_FD;
    return 0;
}

int baecs_Journal::close()
{
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);

    if (d_fd == bdesu_FileUtil::INVALID_FD) {
        return 0;
    }

    if (d_mode & BAECS_SAFE) {
        commitImpl();
    }

    return closeImpl();
}

int baecs_Journal::growFile(FileDescriptor         fd,
                            bdesu_FileUtil::Offset size,
                            bool                   )
{
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);
    // WRITE LOCK REQUIRED

    BAEL_LOG_DEBUG << "Growing journal " << d_filename << " to "
        << size << " bytes (RESERVED)."
        << BAEL_LOG_END;

    Offset currentSize = bdesu_FileUtil::seek(
        fd, 0, SEEK_END);
    // Double check below for the case when Offset is unsigned.
    if (currentSize != -1 && currentSize >= size) {
        d_fileSize = currentSize;
        return 0;
    }

    bdesu_FileUtil::Offset fileSizeLimit = bdesu_FileUtil::getFileSizeLimit();
    if (fileSizeLimit < 0) {
        fileSizeLimit = bdesu_FileUtil::OFFSET_MAX;
    }

    bdesu_FileUtil::Offset availableSpace =
                                         bdesu_FileUtil::getAvailableSpace(fd);
    if (availableSpace < 0) {
        availableSpace = bdesu_FileUtil::OFFSET_MAX;
    }

    if (currentSize + availableSpace < size) {
        BAEL_LOG_FATAL << bsl::fixed << bsl::setprecision(1)
            << "REFUSING TO GROW " << d_filename
            << ": OUT OF DISK SPACE"
            << " (current size: " << PrintSize(currentSize)
            << ", requested size: " << PrintSize(size)
            << ", avail space: " << PrintSize(availableSpace)
            << ")"
            << BAEL_LOG_END;
        return BAECS_IO_ERROR;
    }

    if (fileSizeLimit < size) {
        BAEL_LOG_FATAL << bsl::fixed << bsl::setprecision(1)
            << "REFUSING TO GROW " << d_filename
            << ": ULIMIT REACHED"
            << " (requested size: " << PrintSize(size)
            << ", ulimit: " << PrintSize(fileSizeLimit)
            << ")"
            << BAEL_LOG_END;
        return BAECS_IO_ERROR;
    }

    bdesu_FileUtil::Offset limit = availableSpace;
    if (fileSizeLimit > currentSize) {
        limit = bsl::min(limit, fileSizeLimit - currentSize);
    }

    if (limit < d_diskSpaceWarningThreshold) {
        bael_Severity::Level level =
                limit < d_diskSpaceErrorThreshold ?
            bael_Severity::BAEL_FATAL : bael_Severity::BAEL_ERROR;

        BAEL_LOG_STREAM(level) << bsl::fixed << bsl::setprecision(1)
            << d_filename
            << " HAS ONLY "               << PrintSize(limit)
            << " TO GROW! New size: "     << PrintSize(size)
            << ", ulimit: "               << PrintSize(fileSizeLimit)
            << ", available disk space: " << PrintSize(availableSpace)
            << BAEL_LOG_END;
    }

    // Reserve is always forced now
    int rc = bdesu_FileUtil::grow(fd, size, true);
    if (rc) {
        BAEL_LOG_ERROR << "Unable to grow the journal "
            << d_filename << " to "
            << size << " bytes: rc = " << rc
            << BAEL_LOG_END;
        return BAECS_IO_ERROR;
    }
    d_fileSize = size;
    return 0;
}

bdesu_FileUtil::Offset baecs_Journal::getFileSize(FileDescriptor fd)
{
    bdesu_FileUtil::Offset result =  bdesu_FileUtil::seek(fd, 0, SEEK_END);
    return result;
}

int baecs_Journal::growJournal(bool init)
{
    // WRITE LOCK REQUIRED
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);
    int numPages = d_header.numPages();
    if (!numPages) return 0;
    BSLS_ASSERT(getFileSize(d_fd) >= d_header.calculateFileSize(numPages));

    unsigned pageSets = d_header.numPageSets(numPages);

    while (pageSets > d_pageSetHeaderHandles.size()) {
        // TBD: Move calculations to header?
        unsigned size = d_header.pageHeaderSize()
            * d_header.pagesPerSet() * 2;

        bsl::size_t n = d_pageSetHeaderHandles.size();

        bdesu_FileUtil::Offset offset =
            d_header.headerSize() +
           (size + (bdesu_FileUtil::Offset)(d_header.pagesPerSet())
            * d_header.pageDataSize()) * n;

        BSLS_ASSERT(getFileSize(d_fd) >= size + offset);
        baecs_MappingManager::Handle setHandle =
            d_mappingManager_p->addPage(d_fd,
                                        offset,
                                        size,
                                        (d_mode & BAECS_READWRITE) != 0,
                                        BAECS_PRIORITY_METADATA,
                                        d_dirtyListHandle);
        d_pageSetHeaderHandles.push_back(setHandle);
    }


    while(numPages > d_pageWorkIndexes.size()) {

        int page = d_pageWorkIndexes.size();
        int pageSet = page / d_header.pagesPerSet();
        int pageInPageset = page % d_header.pagesPerSet();

        BSLS_ASSERT(pageSet < d_pageSetHeaderHandles.size());

        char *data = d_mappingManager_p->usePage(
             d_pageSetHeaderHandles[pageSet], (d_mode & BAECS_READWRITE) != 0);

        baecs_JournalPageHeader *ph[2];
        ph[0] = reinterpret_cast<baecs_JournalPageHeader *>
                ((void *)
                   (data + (pageInPageset * 2 + 0)*d_header.pageHeaderSize()));
        ph[1] = reinterpret_cast<baecs_JournalPageHeader *>
                ((void *)
                   (data + (pageInPageset * 2 + 1)*d_header.pageHeaderSize()));

        if (init) {
            ph[0]->init(d_header.blocksPerPage(),
                        d_header.currentTransactionId());

            ph[1]->init(d_header.blocksPerPage(), 0);
            d_pageWorkIndexes.push_back(0);
        }
        else {
            bsls_PlatformUtil::Int64 tid[2];
            if (ph[0]->getTransactionId(tid+0)) {
                return 1;
            }
            if (ph[1]->getTransactionId(tid+1)) {
                return 1;
            }

            int index = 0;
            if (tid[1 - index] > tid[index]) {
                index = 1 - index;
            }

            if (tid[index] > d_header.committedTransactionId()) {
                // We must set the tid on an invalid PH to 0 to prevent it
                // from being picked up in the next transaction.  This
                // change will be synced in commitImpl before actual
                // commit.
                if ((d_mode & BAECS_READWRITE) && (d_mode & BAECS_SAFE)) {
                        ph[index]->setTransactionId(0);
                }
                index = 1 - index;
            }
            BAEL_LOG_TRACE << "Grow: page "
              << d_pageWorkIndexes.size()
              << " tid[0]=" << tid[0] << " tid[1]=" << tid[1] << " wi="
              << index << BAEL_LOG_END;
            d_pageWorkIndexes.push_back(index);
        }

        d_mappingManager_p->releasePage(d_pageSetHeaderHandles[pageSet]);
    }

    unsigned pageSetHeaderSize =
                        d_header.pageHeaderSize() * d_header.pagesPerSet() * 2;

    while (numPages > (int) d_pageHandles.size()) {
        bsl::size_t n = d_pageHandles.size();
        bsl::size_t nPageSet = n / d_header.pagesPerSet();
        bdesu_FileUtil::Offset offset =
                     d_header.headerSize() +
                     (bdesu_FileUtil::Offset)(nPageSet+1) * pageSetHeaderSize +
                     (bdesu_FileUtil::Offset)(n) * d_header.pageDataSize();

        BSLS_ASSERT(getFileSize(d_fd) >= d_header.pageDataSize() + offset);

        PageHandle handle =
                   d_mappingManager_p->addPage(d_fd,
                                               offset,
                                               d_header.pageDataSize(),
                                               (d_mode & BAECS_READWRITE) != 0,
                                               BAECS_PRIORITY_DATA,
                                               d_dirtyListHandle);
        d_pageHandles.push_back(handle);
    }

    BSLS_ASSERT(d_pageWorkIndexes.size()
                <= d_pageSetHeaderHandles.size() * d_header.pagesPerSet());
    BSLS_ASSERT(d_pageWorkIndexes.size() + d_header.pagesPerSet()
                > d_pageSetHeaderHandles.size() * d_header.pagesPerSet());
    return 0;
}

// ACCESSORS
baecs_JournalPageHeader*
baecs_Journal::getPageHeader(unsigned page, bool makeDirty) const
{
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);
    BSLS_ASSERT(!makeDirty || (d_mode & BAECS_READWRITE));

    if (!(d_mode & BAECS_SAFE)) {
        makeDirty = false;
    }

    unsigned pageSet       = page / d_header.pagesPerSet();
    unsigned pageInPageset = page % d_header.pagesPerSet();

    BSLS_ASSERT(pageSet < d_pageSetHeaderHandles.size());

    char* data = d_mappingManager_p->usePage(
                                   d_pageSetHeaderHandles[pageSet], makeDirty);

    baecs_JournalPageHeader* ph[2];

    ph[0] = reinterpret_cast<baecs_JournalPageHeader*>
            ((void *)(data + (pageInPageset * 2) * d_header.pageHeaderSize()));
    ph[1] = reinterpret_cast<baecs_JournalPageHeader*>
            ((void *)(data + (pageInPageset * 2 + 1)
                                                 * d_header.pageHeaderSize()));

    BSLS_ASSERT(page < d_pageWorkIndexes.size());
    int index = d_pageWorkIndexes[page];

    if (d_mode & BAECS_SAFE) {
        // TBD: think of how we can get rid of this lock?
        bcemt_LockGuard<bcemt_Mutex> lGuard(&d_workIndexLock);

        bsls_PlatformUtil::Int64 ctid;
        if (ph[index]->getTransactionId(&ctid)) {
            BSLS_ASSERT(0); // this is not supposed to fail, transaction IDs
            // were recovered in growJournal
            return 0;
        }

        if (ctid != d_header.currentTransactionId()) {

            // TBD: why can't we just use pageWorkIndexes[i] instead of
            // selecting the valid copy again?
            bsls_PlatformUtil::Int64 tid[2];
            if (ph[0]->getTransactionId(tid+0)) {
                BSLS_ASSERT(0); // this is not supposed to fail,
                // transaction IDs were recovered in growJournal
                return 0;

            }
            if (ph[1]->getTransactionId(tid+1)) {
                BSLS_ASSERT(0); // this is not supposed to fail,
                // transaction IDs were recovered in growJournal
                return 0;
            }
            index = 0;
            if (tid[1 - index] > tid[index]) {
                index = 1 - index;
            }
            if (tid[index] > d_header.committedTransactionId()) {
                index = 1 - index;
            }
            BSLS_ASSERT(tid[index] <= d_header.committedTransactionId());
            index = 1 - index;
            d_pageWorkIndexes[page] = index;

            BAEL_LOG_TRACE << "Page " << page << ", current transaction "
                           << d_header.currentTransactionId()
                           << ", copying to " << ph[index]
                           << " (index " << index << ", tid "
                           << tid[index]
                           << ") from " << ph[1-index]
                           << " (index " << index << ", tid "
                           << tid[1-index]
                           << ") " << d_header.pageHeaderSize()
                           << " bytes" << BAEL_LOG_END;

            ph[index]->copyFrom(*(ph[1-index]),
                                d_header.currentTransactionId());

            // Purge the released list in the page header.
            ph[index]->purge();
        }
    }
    return ph[index];
}

char* baecs_Journal::getPageData(unsigned page, bool makeDirty) const
{
    BSLS_ASSERT(page < d_pageHandles.size());
    BSLS_ASSERT(!makeDirty || (d_mode & BAECS_READWRITE));
    if (!(d_mode & BAECS_SAFE)) makeDirty = false;
    return d_mappingManager_p->usePage(d_pageHandles[page], makeDirty);
}

void baecs_Journal::releasePageData(unsigned page) const
{
    BSLS_ASSERT(page < d_pageHandles.size());
    d_mappingManager_p->releasePage(d_pageHandles[page]);
}

void baecs_Journal::releasePageHeader(unsigned page) const
{
    unsigned pageSet = page / d_header.pagesPerSet();
    BSLS_ASSERT(pageSet < d_pageSetHeaderHandles.size());
    d_mappingManager_p->releasePage(d_pageSetHeaderHandles[pageSet]);
}

baecs_Journal_SegmentHeader*
baecs_Journal::segmentHeaderForBlock(unsigned page,
                                     unsigned block,
                                     bool     isDirty) const
{
    char *data = getPageData(page, isDirty);
    return reinterpret_cast<baecs_Journal_SegmentHeader*>
                               ((void *)(data + block * d_header.blockSize()));
}

baecs_JournalPageHeader*
baecs_Journal::getAvailablePageHeader(unsigned *page, unsigned numBlocks)
{
    // Locking : TBD
    //   1) Check the fill list first.
    //      - If the first page doesn't have enough blocks remove it
    //        from the fill list.
    //

    baecs_JournalHeaderPageList& fillPages = d_header.fillPages();
    while (fillPages.d_firstPage != BAECS_INVALID_RECORD_HANDLE) {

       BSLS_ASSERT(0 != fillPages.d_numElements);
       *page = fillPages.d_firstPage;
       baecs_JournalPageHeader *ph = getPageHeader(*page, true);

       // Released list is already purged at this point if necessary.
       unsigned numAvailableBlocks = ph->numAvailableBlocks();
       if (numAvailableBlocks <= numBlocks) {
           // remove this page from the

           fillPages.d_firstPage = ph->nextPage();
           fillPages.d_numElements = fillPages.d_numElements - 1;;

           ph->setNextPage(BAECS_INVALID_RECORD_HANDLE);
           ph->setIsOnFillList(false);
           if (fillPages.d_firstPage == BAECS_INVALID_RECORD_HANDLE) {
               BSLS_ASSERT(fillPages.d_numElements == 0);
               BSLS_ASSERT(*page == fillPages.d_last);
               fillPages.d_last = BAECS_INVALID_RECORD_HANDLE;
           }
       }

       if (numAvailableBlocks > 0) {
           // An empty page can end up on a fill list when a new
           // page is added and allocated immediately.
           // It will be purged on the next call to 'getAvailablePageHeader'.
           return ph;
       }
       else {
           releasePageHeader(*page);
       }
   }
   BSLS_ASSERT(fillPages.d_numElements == 0);

   *page = d_header.numPages();
   int rc = growFile(d_fd, d_header.calculateFileSize(*page+1),
                     0 != (d_mode & BAECS_RESERVE));

   if (0 != rc) {
       return 0;
   }

   d_header.addPages(1);
   rc = growJournal(true);
   if (0 != rc) {
       return 0;
   }

   baecs_JournalPageHeader *ph = getPageHeader(*page, true);

   addPageToList(&d_header.fillPages(), *page);
   ph->setIsOnFillList(true);
   BSLS_ASSERT(ph->numAvailableBlocks() > 0);

   return ph;
}

void baecs_Journal::beginTransaction()
{
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);

    // Move 'pre-fill' pages to 'fill' page list (and empty 'pre-fill' list).

    BAEL_LOG_DEBUG << "Started tid=" << d_header.currentTransactionId()
                   << ", committed tid=" << d_header.committedTransactionId()
                   << BAEL_LOG_END;

    baecs_JournalHeaderPageList &fillPages = d_header.fillPages();
    baecs_JournalHeaderPageList &preFillPages = d_header.preFillPages();

    if (preFillPages.d_firstPage != BAECS_INVALID_RECORD_HANDLE) {
        if (fillPages.d_firstPage != BAECS_INVALID_RECORD_HANDLE) {
            BSLS_ASSERT(fillPages.d_last !=
                           BAECS_INVALID_RECORD_HANDLE);

            baecs_Journal_PageHeaderWriteGuard lastFillPage(
                    this, fillPages.d_last);
            lastFillPage->setNextPage(preFillPages.d_firstPage);
            BAEL_LOG_TRACE << "beginTransaction: Adding preFill pages to "
                "non-empty fill page list." << BAEL_LOG_END;
        }
        else {
            fillPages.d_firstPage = preFillPages.d_firstPage;
            BAEL_LOG_TRACE << "BeginTransation: fill list is empty."
                           << BAEL_LOG_END;
        }
        fillPages.d_last = preFillPages.d_last;
        fillPages.d_numElements =
              (unsigned)(fillPages.d_numElements) + preFillPages.d_numElements;

        BAEL_LOG_TRACE << "beginTransaction: first, next, last = ("
                       << fillPages.d_firstPage << ",";

        // This initialization is inside "if" block defined by the trace above.
        baecs_Journal_PageHeaderReadGuard tmpNext
           (this, fillPages.d_firstPage);

        BAEL_STREAM << (tmpNext ?
                        tmpNext->nextPage() :
                        BAECS_INVALID_RECORD_HANDLE) << ","
                    << fillPages.d_last << ")" << BAEL_LOG_END;

        preFillPages.init();
    }
}

int baecs_Journal::commitImpl()
{
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);

    // WRITE LOCK REQUIRED
    // ALL PAGES (HEADERS/DATA) MUST BE RELEASED

    // Flush the data to disk.
    d_mappingManager_p->flushDirtyList(d_dirtyListHandle, true);

    // Mark the transaction as committed and flush the header copy the current
    // state from the committed one, not including the transaction id.  Flip
    // the workindex and increment the current transaction id.

    d_header.commitCurrentTransaction();
    d_header.commitActiveState();

    int rc = bdesu_FileUtil::sync((char*)d_headerPage,
                                  d_header.headerSize(),
                                  true);

    if (0 != rc) {
        BAEL_LOG_FATAL << "Failed to sync header page to disk for "
            << d_filename << ": rc = " << rc << " (" << (void*)d_headerPage
            << ", " << d_header.headerSize() << ")." << BAEL_LOG_END;
    }

    if (d_mode & BAECS_PARANOID) {
        for (DebugRecordMap::iterator p = d_debugRecordMap.begin();
            p != d_debugRecordMap.end(); ++p)
        {
            p->second |= BAECS_RECORD_COMMITTED;
        }
    }

    beginTransaction();
    return 0;
}

int baecs_Journal::rollbackImpl()
{
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);
    // WRITE LOCK REQUIRED
    // ALL PAGES (HEADERS/DATA) MUST BE RELEASED

    d_mappingManager_p->clearDirtyList(d_dirtyListHandle);

    BAEL_LOG_TRACE << "Rolling back.  Header =  " << d_header << BAEL_LOG_END;

    // Discard the current state, copying it from the opposite one.
    d_header.rollback();

    // Bumped transaction id guarantees that any metadata changes made during
    // the transaction will be discarded and re-copied from the opposite state.
    // Data changes are safe if the released blocks are not reused within the
    // same transaction.  This is guaranteed by the release/purge mechanism in
    // the page header.

    if (d_mode & BAECS_PARANOID) {
        DebugRecordMap::iterator p = d_debugRecordMap.begin();
        while (p != d_debugRecordMap.end()) {
            DebugRecordMap::iterator curr = p;
            ++p;
            if (!(curr->second & BAECS_RECORD_COMMITTED)) {
                d_debugRecordMap.erase(curr);
            }
        }
    }

    beginTransaction();
    return 0;
}

int baecs_Journal::commit()
{
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);
    baecs_Journal_LockGuard guard(this, true);
    BSLS_ASSERT(d_mode & BAECS_SAFE);

    if (d_mode & BAECS_AUTO_COMMIT) {
        return 0;
    }
    return commitImpl();
}

int baecs_Journal::rollback()
{
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);
    baecs_Journal_LockGuard guard(this, true);
    BSLS_ASSERT(d_mode & BAECS_SAFE);
    if (d_mode & BAECS_AUTO_COMMIT) {
        BAEL_LOG_ERROR << "Unable to rollback in autocommit mode: "
                       << d_filename
                       << BAEL_LOG_END;
        return BAECS_UNABLE_TO_ROLLBACK_ERROR;
    }
    return rollbackImpl();
}

void baecs_Journal::addRecordToList(baecs_JournalHeaderRecordList *list,
                                    unsigned                       handle)
{
    // WRITE LOCK REQUIRED
    baecs_Journal_BlockRef current(this, handle, true);

    current->setPrevRecord(list->d_last);
    current->setNextRecord(baecs_Journal::BAECS_INVALID_RECORD_HANDLE);
    if (list->d_last != baecs_Journal::BAECS_INVALID_RECORD_HANDLE) {
        baecs_Journal_BlockRef last(this, list->d_last, true);
        last->setNextRecord(handle);
    }
    else {
        list->d_first = handle;
    }
    list->d_last = handle;
    list->d_numElements = list->d_numElements + 1;
}

void baecs_Journal::removeRecordFromList(baecs_JournalHeaderRecordList *list,
                                         unsigned                       handle)
{
    // WRITE LOCK REQUIRED
    baecs_Journal_BlockRef current(this, handle, true);

    unsigned prevRecord = current->prevRecord();
    unsigned nextRecord = current->nextRecord();

    if (list->d_first == handle) {
        list->d_first = nextRecord;
    }

    if (list->d_last == handle) {
        list->d_last = prevRecord;
    }

    if (prevRecord != baecs_Journal::BAECS_INVALID_RECORD_HANDLE) {
        baecs_Journal_BlockRef prev(this, prevRecord, true);
        prev->setNextRecord(nextRecord);
    }

    if (nextRecord != baecs_Journal::BAECS_INVALID_RECORD_HANDLE) {
        baecs_Journal_BlockRef next(this, nextRecord, true);
        next->setPrevRecord(prevRecord);
    }
    current->setPrevRecord(0xDEADBEEF);
    current->setNextRecord(0xDEADBEEF);
    list->d_numElements = list->d_numElements - 1;
}

int baecs_Journal::addRecord(unsigned         *outHandle,
                             const bcema_Blob *blob,
                             void*             data,
                             unsigned          size,
                             unsigned          attributes)
{
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);
    baecs_Journal_LockGuard guard(this, true);

    char *inP = 0;          // input data pointer
    char *outP = 0;         // output data pointer
    unsigned inBufSize = 0;      // space remaining in input buffer
    unsigned outBufSize = 0;     // space remaining in output buffer
    unsigned currentBlobBuf = 0; // current buffer of the blob
    unsigned dataSize = blob ? blob->length() : size;
    unsigned dataRemaining = dataSize;   // total input data remaining
    unsigned prevPage = BAECS_INVALID_RECORD_HANDLE;
    unsigned headHandle = BAECS_INVALID_RECORD_HANDLE;
    unsigned handle     = BAECS_INVALID_RECORD_HANDLE;
    baecs_Journal_SegmentHeader* prevSegmentHeader = 0;

    unsigned originalDataSize = dataSize;

    unsigned checksum = 0;
    if (d_mode & BAECS_PARANOID) {
        checksum = calculateChecksum(blob, data, size);
    }

    do {
        // Get the write lock & allocate blocks
        // switch the current page if necessary.
        unsigned numBlocks = (dataRemaining
                              + sizeof(baecs_Journal_SegmentHeader)
                              + d_header.blockSize() - 1)
                             / d_header.blockSize();
        unsigned thisPage = 0;
        baecs_JournalPageHeader* thisPageHeader_p =
            getAvailablePageHeader(&thisPage, numBlocks);
        if (!thisPageHeader_p) {
            BAEL_LOG_ERROR << d_filename
                << " : getAvailablePageHeader failed (out of disk space?)"
                << BAEL_LOG_END;
            // Probably we're out of disk space.  Remove the partially added
            // record and return an error.
            if (headHandle != BAECS_INVALID_RECORD_HANDLE) {
                removeRecordImpl(headHandle);
            }
            *outHandle = BAECS_INVALID_RECORD_HANDLE;
            return BAECS_IO_ERROR;
        }

        numBlocks = bsl::min(numBlocks,
                             thisPageHeader_p->numAvailableBlocks());
        BSLS_ASSERT(numBlocks > 0);
        unsigned block = thisPageHeader_p->allocate(numBlocks, attributes);
        BSLS_ASSERT(block != baecs_JournalPageHeader::BAECS_INDEX_NONE);

        if (dataRemaining + sizeof(baecs_Journal_SegmentHeader) >
            numBlocks * d_header.blockSize()) {
            thisPageHeader_p->block(block)->setExtended(true);
        }

        if (headHandle == BAECS_INVALID_RECORD_HANDLE) {
            thisPageHeader_p->block(block)->setHead(true);
        }

        handle = thisPage * d_header.blocksPerPage() + block;
        BAEL_LOG_TRACE << "Allocated " << numBlocks
            << " blocks at handle " << handle
            << ", head " << headHandle
            << BAEL_LOG_END;

        if (headHandle == BAECS_INVALID_RECORD_HANDLE) {
            addRecordToList(&d_header.unconfirmedRecords(), handle);
            if (d_mode & BAECS_PARANOID) {
                bsl::pair<DebugRecordMap::iterator, bool> rc =
                    d_debugRecordMap.insert(bsl::make_pair(handle, 0));
                if (!rc.second) {
                    BAEL_LOG_ERROR << "duplicate record handle allocated "
                        "in addRecord()" << BAEL_LOG_END;
                    BSLS_ASSERT(0);
                }
            }
        }

        // Now fill the blocks with data.  This does not have to be under lock.
        guard.relock(false);
        if (headHandle == BAECS_INVALID_RECORD_HANDLE) {
            headHandle = handle;
        }
        else {
            BSLS_ASSERT(prevSegmentHeader);
            BSLS_ASSERT(prevPage != BAECS_INVALID_RECORD_HANDLE);
            prevSegmentHeader->d_nextSegment = handle;
            releasePageData(prevPage);
        }

        // TBD: get a "segment" header for a block
#if 1
        char* pageData = getPageData(thisPage, true);
        baecs_Journal_SegmentHeader* segmentHeader =
            reinterpret_cast<baecs_Journal_SegmentHeader*>
                           ((void *)(pageData + block * d_header.blockSize()));
#else
        baecs_Journal_SegmentHeader* segmentHeader =
                                  segmentHeaderForBlock(thisPage, block, true);
        char *blockData = (char*)segmentHeader;
#endif

        segmentHeader->d_timestamp = nowAsInt64GMT(); // TBD
        segmentHeader->d_headSegment = headHandle;
        segmentHeader->d_nextSegment = BAECS_INVALID_RECORD_HANDLE;
        segmentHeader->d_size = dataRemaining;
        segmentHeader->d_checksum = checksum;
        prevSegmentHeader = segmentHeader;

        outP = reinterpret_cast<char*>(segmentHeader+1);
        outBufSize = d_header.blockSize()
                     - sizeof(baecs_Journal_SegmentHeader);
        --numBlocks;
        prevPage = thisPage;

        while (dataRemaining) {

            if (!outBufSize) {
                if (!numBlocks) {
                    break;
                }
                --numBlocks;
                block = thisPageHeader_p->block(block)->nextBlock();
                BSLS_ASSERT(block !=
                                    baecs_JournalPageHeader::BAECS_INDEX_NONE);
                // Logical: get offset for block
                outP = pageData + block * d_header.blockSize();
                outBufSize = d_header.blockSize();
            }
            if (!inBufSize) {
                if (blob) {
                    BSLS_ASSERT((int)currentBlobBuf < blob->numDataBuffers());
                    inP  = blob->buffer(currentBlobBuf).data();
                    inBufSize = bsl::min(
                        (unsigned)blob->buffer(currentBlobBuf).size(),
                        dataRemaining);
                    ++currentBlobBuf;
                }
                else {
                    inP = (char*)data;
                    inBufSize = size;
                }
            }

            unsigned numBytes = bsl::min(inBufSize, outBufSize);
            bsl::memcpy(outP, inP, numBytes);
            dataRemaining -= numBytes;
            inBufSize     -= numBytes;
            outBufSize    -= numBytes;
            outP          += numBytes;
            inP           += numBytes;
        }
        releasePageHeader(thisPage);
        guard.relock(true);
    } while (dataRemaining);

    if (prevPage != BAECS_INVALID_RECORD_HANDLE) {
        releasePageData(prevPage);
    }

    *outHandle = headHandle;
    BSLS_ASSERT(*outHandle != BAECS_INVALID_RECORD_HANDLE);

    if (d_mode & BAECS_AUTO_COMMIT) {
        int rc = commitImpl();
        BSLS_ASSERT(rc == 0); // TBD
    }

    BAEL_LOG_TRACE << "added record " << *outHandle << "("
        << originalDataSize << " bytes)"
        << BAEL_LOG_END;

    if (d_mode & BAECS_PARANOID) {
        guard.relock(false);
        bcema_Blob tmpblob;
        // getRecordData verifies the checksum
        getRecordData(&tmpblob, *outHandle);
        BAEL_LOG_TRACE << "added record " << *outHandle << "("
            << tmpblob.length() << " bytes)\n"
            << bcema_BlobUtilHexDumper(&tmpblob) << BAEL_LOG_END;
    }
    return 0;
}

void baecs_Journal::removeRecordImpl(unsigned headHandle)
{
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);
    unsigned handle = headHandle;
    // It could be more efficient to write-lock the journal
    // just for actual removal, but the only case when it would
    // make a significant difference is the extended record
    // case, and extended records are rare.

    while (handle != BAECS_INVALID_RECORD_HANDLE) {
        unsigned page, block;
        handle2PageBlock(&page, &block, handle);
        BSLS_ASSERT(page < (unsigned) d_header.numPages());

        baecs_Journal_PageHeaderWriteGuard pageHeader(this, page);

        baecs_JournalPageHeader::BlockHeader* bh = pageHeader->block(block);
        unsigned nextHandle;
        if (bh->isExtended()) {
            nextHandle = segmentHeaderForBlock(page,
                                               block,
                                               false)->d_nextSegment;
            releasePageData(page);
        }
        else {
            nextHandle = BAECS_INVALID_RECORD_HANDLE;
        }

        if (bh->isHead()) {
            if (bh->isConfirmed()) {
                removeRecordFromList(&d_header.confirmedRecords(), handle);
            } else {
                removeRecordFromList(&d_header.unconfirmedRecords(), handle);
            }
        }

        int rc;
        if (d_mode & BAECS_SAFE) {
            rc = pageHeader->release(block);
        }
        else {
            rc = pageHeader->deallocate(block);
        }

        if (rc) {
            BAEL_LOG_ERROR << d_filename << ":failed to release segment "
                           << handle << ", block " << block << ", "
                           << rc << " blocks long.  dumping page "
                           << page << ":\n";
            pageHeader->print(BAEL_STREAM);
            BAEL_STREAM << BAEL_LOG_END;
            BSLS_ASSERT(0);
        }

        // In FAST mode, if the page has a sufficient number of free blocks,
        // we can put it on FILL list.
        // In SAFE mode, we need to be able to roll back any changes, so
        // the page is put on PRE_FILL list, which is appended to FILL list
        // when the transaction is committed.

        if (!pageHeader->isOnFillList() &&
            pageHeader->numFreeBlocks()*100/pageHeader->numBlocks()
                                  > (unsigned) d_header.freeBlockThreshold()) {

            BSLS_ASSERT(pageHeader->nextPage() == BAECS_INVALID_RECORD_HANDLE);
            baecs_JournalHeaderPageList* list = d_mode & BAECS_SAFE
                                            ? &(d_header.preFillPages())
                                            : &(d_header.fillPages());

            BSLS_ASSERT(pageHeader->numFreeBlocks() > 0);
            addPageToList(list, page);
            pageHeader->setIsOnFillList(true);
        }
        handle = nextHandle;
    }
}

int baecs_Journal::removeRecord(unsigned headHandle)
{
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);
    if (d_mode & BAECS_PARANOID) {
        bcema_Blob tmpblob;
        // getRecordData verifies the checksum before removal
        getRecordData(&tmpblob, headHandle);
    }

    baecs_Journal_LockGuard guard(this, true);

    if (d_mode & BAECS_PARANOID) {
        DebugRecordMap::iterator p = d_debugRecordMap.find(headHandle);
        if (p == d_debugRecordMap.end()) {
            BAEL_LOG_ERROR << "Removing a non-existent record: " << headHandle
                    << BAEL_LOG_END;
            BSLS_ASSERT(0);
        }
        BSLS_ASSERT(p->first == headHandle);
        d_debugRecordMap.erase(p);
    }

    removeRecordImpl(headHandle);

    if (d_mode & BAECS_AUTO_COMMIT) {
        int rc = commitImpl();
        BSLS_ASSERT(rc == 0); // TBD
    }

    if (d_mode & BAECS_PARANOID) {
        BAEL_LOG_TRACE << "removed record" << headHandle << BAEL_LOG_END;
    }

    return 0;
}

void baecs_Journal::addPageToList(baecs_JournalHeaderPageList *list,
                                  unsigned                     page)
{
    // WRITE LOCK REQUIRED
    BSLS_ASSERT(list);
    unsigned prev = list->addPage(page);
    if (prev != baecs_JournalHeaderPageList::BAECS_INVALID_PAGE) {
        baecs_Journal_PageHeaderWriteGuard last(this, prev);
        last->setNextPage(page);
    }
}

int baecs_Journal::confirmRecord(unsigned handle)
{
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);
    baecs_Journal_LockGuard guard(this, true);
    unsigned page, first;
    handle2PageBlock(&page, &first, handle);
    BSLS_ASSERT(page < (unsigned) d_header.numPages());

    baecs_Journal_PageHeaderWriteGuard pageHeader(this, page);
    bool isConfirmed = pageHeader->block(first)->isConfirmed();
    if (d_mode & BAECS_PARANOID) {
        DebugRecordMap::iterator p = d_debugRecordMap.find(handle);
        if (p == d_debugRecordMap.end()) {
            BAEL_LOG_ERROR << "Confirming a non-existent record: " << handle
                << BAEL_LOG_END;
            BSLS_ASSERT(0);
        }
        BSLS_ASSERT(p->first == handle);
        if ((p->second & BAECS_RECORD_CONFIRMED) != isConfirmed) {
            BAEL_LOG_ERROR << "Inconsistent CONFIRMED flag in record: "
                << handle << BAEL_LOG_END;
            BSLS_ASSERT(0);
        }
        p->second = 1;
    }

    if (!isConfirmed) {
        removeRecordFromList(&d_header.unconfirmedRecords(), handle);
        addRecordToList(&d_header.confirmedRecords(), handle);

        pageHeader->block(first)->setConfirmed(true);
        if (d_mode & BAECS_AUTO_COMMIT) {
            int rc = commitImpl();
            BSLS_ASSERT(rc == 0); // TBD
        }
        if (d_mode & BAECS_PARANOID) {
            BAEL_LOG_TRACE << "Confirmed record" << handle << BAEL_LOG_END;
        }
    }
    return 0;
}

int baecs_Journal::getRecordData(bcema_Blob *blob,
                                 char       *buf,
                                 unsigned    bufSize,
                                 unsigned    headHandle) const
{
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);
    unsigned handle = headHandle;
    BSLS_ASSERT(handle != BAECS_INVALID_RECORD_HANDLE);
    baecs_Journal_ReadLockGuard guard(this);

    if (d_mode & BAECS_PARANOID) {
       DebugRecordMap::const_iterator p = d_debugRecordMap.find(handle);
       BSLS_ASSERT(p != d_debugRecordMap.end());
       BSLS_ASSERT(p->first == handle);
    }

    if (blob) {
        blob->removeAll();
    }
    unsigned numBytesCopied = 0;
    unsigned recordSize = 0;
    unsigned checksum = 0;

    while (handle != BAECS_INVALID_RECORD_HANDLE && (blob || bufSize)) {
        unsigned page, block;
        handle2PageBlock(&page, &block, handle);
        BSLS_ASSERT(page < (unsigned) d_header.numPages());
        BSLS_ASSERT(block != baecs_JournalPageHeader::BAECS_INDEX_NONE);
        baecs_Journal_PageHeaderReadGuard pageHeader(this, page);

        // TBD: Get a segment header for a block
        char* pageData = getPageData(page, false);
        baecs_Journal_SegmentHeader* segmentHeader =
            reinterpret_cast<baecs_Journal_SegmentHeader*>
                           ((void *)(pageData + block * d_header.blockSize()));

        unsigned segmentSize = segmentHeader->d_size;

        char *data = reinterpret_cast<char*>(segmentHeader + 1);

        unsigned dataSize = bsl::min(unsigned(d_header.blockSize()
                          - sizeof(baecs_Journal_SegmentHeader)), segmentSize);
        if (pageHeader->block(block)->isHead()) {
            recordSize = segmentSize;
            checksum = segmentHeader->d_checksum;
        }

        if (pageHeader->block(block)->isExtended()) {
            handle = segmentHeader->d_nextSegment;
        }
        else {
            handle = BAECS_INVALID_RECORD_HANDLE;
        }

        for (; block != baecs_JournalPageHeader::BAECS_INDEX_NONE
                  && segmentSize
            && (blob || bufSize);
            // TBD: Move calculation to pageheader
            data = pageData + block * d_header.blockSize(),
              dataSize = bsl::min((unsigned)d_header.blockSize(), segmentSize))
        {
            // Skip the set of consecutive blocks -- append them as one
            // BlobBuffer or one memcpy.
            unsigned firstBlock = block;
            while (pageHeader->block(block)->nextBlock()
                   != baecs_JournalPageHeader::BAECS_INDEX_NONE &&
                   pageHeader->block(block)->nextBlock() == block + 1) {

                dataSize += d_header.blockSize();
                block = pageHeader->block(block)->nextBlock();
            }

            dataSize = bsl::min(segmentSize, dataSize);

            if (blob) {
                getPageData(page, false);
                bcema_BlobBuffer bb(
                    bcema_SharedPtr<char>(
                        data,
                        bdef_BindUtil::bind(
                            &baecs_MappingManager::releasePage,
                            d_mappingManager_p,
                            d_pageHandles[page]),
                        &d_poolAllocator),
                    dataSize);

                blob->appendBuffer(bb);
                numBytesCopied += dataSize;

                BSLS_ASSERT (segmentSize >= dataSize);
                segmentSize -= dataSize;

                BAEL_LOG_TRACE << "Record " << headHandle << ": appending "
                    << dataSize << "(b " << firstBlock << ") bytes from "
                    << (void*)data
                    << BAEL_LOG_END;
            }
            else {
                unsigned numBytes = bsl::min(bufSize, dataSize);
                BAEL_LOG_TRACE << "Record " << headHandle << ":copying "
                    << numBytes << "(b " << block << ") bytes from "
                    << (void*)data
                    << BAEL_LOG_END;
                bsl::memcpy(buf+numBytesCopied, data, numBytes);
                numBytesCopied += numBytes;
                bufSize -= numBytes;
                segmentSize -= numBytes;
            }
            block = pageHeader->block(block)->nextBlock();
        }
        releasePageData(page);
    }

    if (blob) {
        blob->setLength(numBytesCopied);
    }

    if (d_mode & BAECS_PARANOID) {
        if (blob) {
            BAEL_LOG_TRACE << "Got data for record " << headHandle << "("
            << blob->length() << " bytes)\n"
            << bcema_BlobUtilHexDumper(blob) << BAEL_LOG_END;
        } else {
            BAEL_LOG_TRACE << "Got data for record " << headHandle << "("
            << numBytesCopied << " bytes)\n"
            << bdeu_PrintStringHexDumper(buf, numBytesCopied) << BAEL_LOG_END;
        }
        if (numBytesCopied == recordSize) {
            BSLS_ASSERT(calculateChecksum(
                    blob, buf, numBytesCopied) == checksum);
        }
    }
    return recordSize;
}

unsigned baecs_Journal::getRecordLength(RecordHandle handle) const
{
    baecs_Journal_ReadLockGuard guard(this);
    unsigned page, block;
    handle2PageBlock(&page, &block, handle);

    BSLS_ASSERT(page < (unsigned) d_header.numPages());

    baecs_Journal_PageHeaderReadGuard pageHeader(this, page);
    BSLS_ASSERT(pageHeader->block(block)->isHead());

    unsigned recordSize = segmentHeaderForBlock(page, block, false)->d_size;

    releasePageData(page);  // TBD: page guard?
    return recordSize;
}

bdet_Datetime
baecs_Journal::getRecordCreationDatetime(RecordHandle handle) const
{
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);
    baecs_Journal_ReadLockGuard guard(this);

    unsigned page, block;
    handle2PageBlock(&page, &block, handle);

    BSLS_ASSERT(handle != BAECS_INVALID_RECORD_HANDLE);
    if (page >= (unsigned) d_header.numPages()) {
        BAEL_LOG_ERROR << "Invalid handle "
                       << handle
                       << " passed to getRecordCreationDatetime()"
                       << BAEL_LOG_END;
        return bdet_Datetime();
    }

    baecs_Journal_PageHeaderReadGuard pageHeader(this, page);
    BSLS_ASSERT(pageHeader->block(block)->isHead());

    bdet_Datetime result =
        int64ToDatetime(segmentHeaderForBlock(page,
                                              block,
                                              false)->d_timestamp);
    releasePageData(page);
    return result;
}

bool baecs_Journal::isConfirmedRecord(
        baecs_Journal::RecordHandle handle) const
{
    baecs_Journal_ReadLockGuard guard(this);
    baecs_Journal_BlockRef head(this, handle, false);

    BSLS_ASSERT(head->isHead());
    return head->isConfirmed();
}

bool baecs_Journal::isUnconfirmedRecord(
        baecs_Journal::RecordHandle handle) const
{
    return !isConfirmedRecord(handle);
}

bsls_PlatformUtil::Int64 baecs_Journal::getFileSize() const
{
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);
    baecs_Journal_ReadLockGuard guard(this);
    return d_fileSize;
}

baecs_Journal::RecordHandle baecs_Journal::firstConfirmedRecord() const
{
    baecs_Journal_ReadLockGuard guard(this);
    return d_header.confirmedRecords().d_first;
}

baecs_Journal::RecordHandle baecs_Journal::firstUnconfirmedRecord() const
{
    baecs_Journal_ReadLockGuard guard(this);
    return d_header.unconfirmedRecords().d_first;
}

baecs_Journal::RecordHandle
baecs_Journal::nextRecord(RecordHandle handle) const
{
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);
    baecs_Journal_ReadLockGuard guard(this);
    unsigned page, block;
    handle2PageBlock(&page, &block, handle);

    if (page >= (unsigned) d_header.numPages()) {
        BAEL_LOG_ERROR << "Invalid handle " << handle
                       << " in nextRecord()."
                       << BAEL_LOG_END;
        return BAECS_INVALID_RECORD_HANDLE;
    }

    baecs_Journal_BlockRef current(this, handle, false);
    return current->nextRecord();
}

baecs_Journal::RecordHandle baecs_Journal::nextUnconfirmedRecord(
        RecordHandle handle) const
{
    return nextRecord(handle);
}

baecs_Journal::RecordHandle baecs_Journal::nextConfirmedRecord(
        RecordHandle handle) const
{
    return nextRecord(handle);
}

char* baecs_Journal::userData()
{
    baecs_Journal_ReadLockGuard guard(this);
    return d_header.userData();
}

const char*
baecs_Journal::userData() const
{
    baecs_Journal_ReadLockGuard guard(this);
    return d_header.userData();
}

unsigned
baecs_Journal::userDataSize() const
{
    baecs_Journal_ReadLockGuard guard(this);
    return d_header.userDataSize();
}

int baecs_Journal::alterRecordAttributes(unsigned              *outAttributes,
                                         RecordHandle           handle,
                                         unsigned               andMask,
                                         unsigned               orMask)
{
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);
    baecs_Journal_LockGuard guard(this, true);

    baecs_Journal_BlockRef head(this, handle, true);

    unsigned newAttributes = (head->attributes() & andMask) | orMask;
    head->setAttributes(newAttributes);
    if (outAttributes) {
        *outAttributes = newAttributes;
    }

    if (d_mode & BAECS_AUTO_COMMIT) {
        int rc = commitImpl();
        BSLS_ASSERT(rc == 0); // TBD
    }

    return 0;
}

unsigned baecs_Journal::getRecordAttributes(RecordHandle handle) const
{
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);
    baecs_Journal_ReadLockGuard guard(this);
    baecs_Journal_BlockRef current(this, handle, false);
    return current->attributes();
}

int baecs_Journal::recordFragmentation(int *numBlocks, int *numPages,
                                       RecordHandle handle) const
{
    *numBlocks = 0;
    *numPages = 0;
    RecordHandle h = handle;
    baecs_Journal_ReadLockGuard guard(this);
    while (h != BAECS_INVALID_RECORD_HANDLE)
    {
        unsigned page, firstBlock;
        handle2PageBlock(&page, &firstBlock, h);

        ++*numPages;
        baecs_Journal_PageHeaderReadGuard pageHeader(this, page);
        baecs_Journal_SegmentHeader* segmentHeader
            = segmentHeaderForBlock(page, firstBlock, false);
        if (pageHeader->block(firstBlock)->isExtended())
        {
            h = segmentHeader->d_nextSegment;
        }
        else
        {
            h = BAECS_INVALID_RECORD_HANDLE;
        }

        for (unsigned block = firstBlock;
             block != baecs_JournalPageHeader::BAECS_INDEX_NONE;)
        {
            unsigned next = pageHeader->block(block)->nextBlock();
            if (next != block + 1)
            {
                ++*numBlocks;
            }
            block = next;
        }
        releasePageData(page);
    }
    return 0;
}

int baecs_Journal::markRecordForValidation(RecordHandle  headHandle,
                                           unsigned     *map,
                                           bool          verbose) const
{
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);
    int result = 0;
    unsigned handle = headHandle;
    J_ASSERT1(handle != BAECS_INVALID_RECORD_HANDLE, handle);

    if (verbose) {
        bsl::printf("Record %u: ", headHandle);
        bsl::fflush(stdout);
    }

    while (handle != BAECS_INVALID_RECORD_HANDLE) {
        unsigned page, block;
        handle2PageBlock(&page, &block, handle);

        J_ASSERT1(block != baecs_JournalPageHeader::BAECS_INDEX_NONE, block);

        baecs_Journal_PageHeaderReadGuard pageHeader(this, page);
        bool isHead = pageHeader->block(block)->isHead();
        J_ASSERT1((handle != headHandle) || isHead, isHead);

        if (verbose) {
            if (!isHead) {
                bsl::printf("\t\t");
            }
            bsl::printf("Segment %u", handle);
            bsl::fflush(stdout);
        }

        baecs_Journal_SegmentHeader* segmentHeader
            = segmentHeaderForBlock(page, block, false);
        if (pageHeader->block(block)->isExtended()) {
            handle = segmentHeader->d_nextSegment;
            if (verbose) {
                bsl::printf(" (next is %u)", handle);
            }
        } else {
            handle = BAECS_INVALID_RECORD_HANDLE;
        }

        if (verbose) {
            bsl::printf(": ");
            bsl::fflush(stdout);
        }

        int firstBlock = baecs_JournalPageHeader::BAECS_INDEX_NONE,
            lastBlock = baecs_JournalPageHeader::BAECS_INDEX_NONE;

        while (block != baecs_JournalPageHeader::BAECS_INDEX_NONE)
        {
            int globalIndex = page*d_header.blocksPerPage() + block;

            if (verbose) {
                if (firstBlock == baecs_JournalPageHeader::BAECS_INDEX_NONE)
                {
                    // the beginning of consecutive list of blocks
                    firstBlock = globalIndex;
                    lastBlock = globalIndex - 1;
                }
                if (globalIndex != lastBlock + 1) {
                    if (firstBlock != lastBlock) {
                        bsl::printf(" [%d, %d] ", firstBlock, lastBlock);
                        bsl::fflush(stdout);
                    }
                    else {
                        bsl::printf(" [%d] ", firstBlock);
                        bsl::fflush(stdout);
                    }
                    firstBlock = globalIndex;
                    lastBlock = firstBlock;
                }
                else {
                    lastBlock = globalIndex;
                }
            }

            J_ASSERT2(map[globalIndex] == (unsigned)-1,
                      map[globalIndex],
                      globalIndex);
            map[globalIndex] = headHandle;
            block = pageHeader->block(block)->nextBlock();
        }

        if (verbose) {
            if (firstBlock != lastBlock) {
                bsl::printf(" [%d, %d] \n", firstBlock, lastBlock);
            }
            else {
                bsl::printf(" [%d] \n", firstBlock);
            }
        }

        releasePageData(page);
    }
    return result;
}

int baecs_Journal::markPageListForValidation(
                                    const baecs_JournalHeaderPageList *list,
                                    unsigned                          *pageMap,
                                    int                                index,
                                    bool /*verbose*/) const
{

    int result = 0;
    if (0 == list->d_numElements) {
        J_ASSERT1(list->d_firstPage == BAECS_INVALID_RECORD_HANDLE,
                  list->d_firstPage);
        J_ASSERT1(list->d_last == BAECS_INVALID_RECORD_HANDLE, list->d_last);
        return result;
    }

    unsigned page = list->d_firstPage;
    unsigned numPages = 0;

    J_ASSERT1(list->d_last != baecs_JournalHeaderPageList::BAECS_INVALID_PAGE,
              list->d_last);

    while (1) {
        J_ASSERT1(page != baecs_JournalHeaderPageList::BAECS_INVALID_PAGE,
                  page);
        J_ASSERT2(page < (unsigned) d_header.numPages(),
                  page,
                  d_header.numPages());
        J_ASSERT2(pageMap[page] == (unsigned)-1, page,
                  pageMap[page]);
        J_ASSERT2(numPages < list->d_numElements,
                  numPages,
                  list->d_numElements);
        pageMap[page] = index;
        ++numPages;
        if (page == list->d_last) {
            return result;
        }
        baecs_Journal_PageHeaderReadGuard pageHeader(this, page);
        page = pageHeader->nextPage();
    }
    J_ASSERT2(numPages == list->d_numElements, numPages,
              list->d_numElements);

}

int baecs_Journal::validateImpl(bool verbose) const {
    int result = 0;
    unsigned numPages = d_header.numPages();
    unsigned numBlocks = numPages*d_header.blocksPerPage();

    bsl::vector<unsigned> pageMapVector;
    pageMapVector.resize(d_header.numPages(), (unsigned)-1);
    unsigned *pageMap = &(pageMapVector[0]);
    markPageListForValidation(&d_header.fillPages(), pageMap, 1, verbose);
    markPageListForValidation(&d_header.preFillPages(), pageMap, 2, verbose);

    bsl::vector<unsigned> mapVector;
    mapVector.resize(numBlocks, (unsigned)-1);
    unsigned *map = &(mapVector[0]);
    if (verbose) {
        bsl::printf("Validating journal %s (%d pages).\n",
                    d_filename.c_str(),
                    numPages);
    }

    for (RecordHandle handle = firstUnconfirmedRecord();
        handle != BAECS_INVALID_RECORD_HANDLE;
        handle = nextUnconfirmedRecord(handle)) {
        J_ASSERT(0 == markRecordForValidation(handle, map, verbose));
    }

    for (RecordHandle handle = firstConfirmedRecord();
        handle != BAECS_INVALID_RECORD_HANDLE;
        handle = nextConfirmedRecord(handle)) {
        J_ASSERT(0 == markRecordForValidation(handle, map, verbose));
    }

    for (unsigned page = 0; page < numPages; ++page) {
        baecs_Journal_PageHeaderReadGuard pageHeader(this, page);
        unsigned offset = page * d_header.blocksPerPage();

        J_ASSERT(0 == pageHeader->markFreeListsForValidation(map,
                                                             verbose,
                                                             offset));
    }

    for (unsigned globalIndex = 0; globalIndex < numBlocks; ++globalIndex) {
        J_ASSERT2(map[globalIndex] != (unsigned)-1,
                  map[globalIndex],
                  globalIndex);
    }

    if (verbose) {
        bsl::puts("Validation complete.");
    }
    return result;
}

int baecs_Journal::validate(bool verbose) const
{
    baecs_Journal_ReadLockGuard guard(this);
    return validateImpl(verbose);
}

void baecs_Journal::setDiskSpaceLoggingThresholds(
        bdesu_FileUtil::Offset warningThreshold,
        bdesu_FileUtil::Offset errorThreshold)
{
    d_diskSpaceWarningThreshold = bsl::max(warningThreshold,
                                           errorThreshold);
    d_diskSpaceErrorThreshold = errorThreshold;
}

const char* baecs_Journal::errorMessage(int errorCode)
{
    switch(errorCode) {
        case 0:
            return "no error";
        case BAECS_WRITE_ACCESS_REQUIRED_ERROR:
            return "write access required";
        case BAECS_IO_ERROR:
            return "I/O error";
        case BAECS_MMAP_ERROR:
            return "mmap error";
        case BAECS_UNABLE_TO_ROLLBACK_ERROR:
            return "unable to rollback in current mode";
        case BAECS_FILE_NOT_FOUND_ERROR:
            return "file not found";
        case BAECS_FORMAT_ERROR:
            return "unrecoverable journal format error";
        case BAECS_ALIGNMENT_ERROR:
            return "journal alignment is lower than this platform supports";
        case BAECS_UNSUPPORTED_VERSION_ERROR:
            return "unsupported journal format version";
        case BAECS_INVALID_STATE_ERROR:
            return "invalid state (journal was not opened or closed)";
        case BAECS_INVALID_PARAMETERS_ERROR:
            return "unsupported journal parameters";
        default:
            return "unknown error";
    }
}

unsigned baecs_Journal::blockSize() const
{
    baecs_Journal_ReadLockGuard guard(this);
    return d_header.blockSize();
}

unsigned baecs_Journal::blocksPerPage() const
{
    baecs_Journal_ReadLockGuard guard(this);
    return d_header.blocksPerPage();
}

unsigned baecs_Journal::pagesPerSet() const
{
    baecs_Journal_ReadLockGuard guard(this);
    return d_header.pagesPerSet();
}

unsigned baecs_Journal::freeBlockThreshold() const
{
    baecs_Journal_ReadLockGuard guard(this);
    return d_header.freeBlockThreshold();
}

bdet_Datetime baecs_Journal::journalCreationDatetime() const
{
    baecs_Journal_ReadLockGuard guard(this);
    return int64ToDatetime(d_header.creationTime());
}

bdet_Datetime baecs_Journal::journalModificationDatetime() const
{
    baecs_Journal_ReadLockGuard guard(this);
    return int64ToDatetime(d_header.modificationTime());
}

unsigned baecs_Journal::numConfirmedRecords() const
{
    baecs_Journal_ReadLockGuard guard(this);
    return d_header.confirmedRecords().d_numElements;
}

unsigned baecs_Journal::numUnconfirmedRecords() const
{
    baecs_Journal_ReadLockGuard guard(this);
    return d_header.unconfirmedRecords().d_numElements;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007, 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
