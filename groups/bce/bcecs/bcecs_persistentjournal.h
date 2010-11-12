// bcecs_persistentjournal.h                  -*-C++-*-
#ifndef INCLUDED_BCECS_PERSISTENTJOURNAL
#define INCLUDED_BCECS_PERSISTENTJOURNAL

//@PURPOSE: Provide a persistent journal
//
//@CLASSES:
// bcecs_PersistentJournal: file-based persistent journal
// bcecs_PersistentJournalIterator: record iterator
//
//@AUTHOR: Ujjwal Bhoota (ubhoota), Ilougino Rocha (irocha1)
//
//@SEE_ALSO: bcema_blob
//
//@DESCRIPTION:
// This component provides thread-safe, persistent journal of arbitrary sized
// records. 'addRecord' adds a record to the unconfirmed list.  'confirmRecord'
// moves the specifed unconfirmed record to the confirmed list.  A confirmed or
// unconfirmed record can be removed using removeRecord.  Removed records are
// put on free list to be reused.  See also the 'maxConfirmedRecordsKept'
// parameter in 'openJournal'.
// Records can be iterated using 'bcecs_PersistentJournalIterator' (see
// Usage).  
//
///Usage
///-----
///Iterating over unconfirmed record, from oldest to latest:
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//    typedef bcecs_PersistentJournalIterator It;
//    for (It it = journal.firstUnconfirmedRecord(); it; ++it) {
//        // use 'it'
//    }
//    
///Iterating over unconfirmed record, from latest to oldest:
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//    typedef bcecs_PersistentJournalIterator It;
//    for (It it = journal.lastUnconfirmedRecord(); it; --it) {
//        // use 'it'
//    }
//    
//    
///Iterating over confirmed record, from oldest to latest:
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//    typedef bcecs_PersistentJournalIterator It;
//    for (It it = journal.firstConfirmedRecord(); it; ++it) {
//        // use 'it'
//    }
//    
///Iterating over confirmed record, from latest to oldest:
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//    typedef bcecs_PersistentJournalIterator It;
//    for (It it = journal.lastConfirmedRecord(); it; --it) {
//        // use 'it'
//    }
//
///Iterator usage warning:
///- - - - - - - - - - - -
// Note that the associated journal is (read)locked when the iterator is
// constructed and is unlocked only when the iterator is destructed.  This
// means that until the iterator is not destructed, all the threads trying to
// modify the journal will remain blocked (however multiple threads can still
// concurrently read the journal).  Also,  modifying journal from within the
// scope of an iterator will result in deadlock.
//
///'maxFileSize' parameter:
///------------------------
// It is guranteed that file will never be grown beyond the value specified by
// this parameter.  If the value of this parameter is -1, than it is ignored.
// By default the value is set to be -1.  This parameter also put a limit on
// record size (see 'maxRecordSize()' and 'maxRecordSizeForEmptyJournal()').


#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BDES_PLATFORM
#include <bdes_platform.h>
#endif


#ifdef BDES_PLATFORM__OS_WINDOWS
    #ifndef INCLUDED_WINDOWS_H
    #define INCLUDED_WINDOWS_H
    #include <windows.h>
    #endif

    #ifndef INCLUDED_IO_H
    #define INCLUDED_IO_H
    #include <io.h>
    #endif
#endif



#ifndef INCLUDED_BCEMA_BLOB
#include <bcema_blob.h>
#endif

#ifndef INCLUDED_BCEMT_READERWRITERLOCK
#include <bcemt_readerwriterlock.h>
#endif

#ifndef INCLUDED_BDES_ASSERT
#include <bdes_assert.h>
#endif

#ifndef INCLUDED_BDET_DATETIME
#include <bdet_datetime.h>
#endif

#ifndef INCLUDED_IOSFWD
#include <iosfwd>
#define INCLUDED_IOSFWD
#endif

#ifndef INCLUDED_SET
#include <set>
#define INCLUDED_SET
#endif

#ifndef INCLUDED_VECTOR
#include <vector>
#define INCLUDED_VECTOR
#endif

#ifndef INCLUDED_STRING
#include <string>
#define INCLUDED_STRING
#endif

#ifndef INCLUDED_CSTDDEF
#include <cstddef>
#define INCLUDED_CSTDDEF
#endif


namespace BloombergLP {
// FORWARD DECLRATIONS
class bdema_Allocator;
class bcecs_JournalTransactionGuard;
class bcecs_PersistentJournal;
struct bcecs_PersistentJournalHeader;
struct bcecs_JournalBlockHeader;
struct bcecs_JournalRecordHeader;
struct bcecs_JournalDataBuffer;
struct bcecs_JournalStateHeader;
struct bcecs_JournalPageTable;
struct bcecs_RecordList;
struct bcecs_FreeBlockList;

                         // =====================
                         // struct bcecs_PersistentJournalFileUtil
                         // =====================

struct bcecs_PersistentJournalFileUtil { // platform independent interface
    // exports a type 'FileDescriptor' and associated functions.  

    // TYPES
#ifdef BDES_PLATFORM__OS_WINDOWS
    typedef HANDLE FileDescriptor; 
#else
    typedef int FileDescriptor;
#endif
    
    struct FileMapping {
        // mmap returns this and munmap accepts this
        
        void   *d_addr;
#ifdef BDES_PLATFORM__OS_WINDOWS
        HANDLE d_mapHandle;
#endif
        void *addr() { return d_addr;} // mapped address    
        void *addr() const {return d_addr;} // mapped address
    };


    // STATIC DATA
    static FileDescriptor BAD_FD; 


    // STATIC METHODS
    static FileDescriptor open(const char     *pathName, 
                               bool            isReadWrite, 
                               bool            isExisting);
        // return 'BAD_FD' on error.  If 'isExisting' is true, than only
        // existing file is opened.  If 'isExisting' is false, than 
        // a new file is created, overwriting any existing file.  

    static int close(FileDescriptor fd);

    static int lseek(FileDescriptor fd, int offset, int whence);

    static int read(FileDescriptor fd, void *buf, int numBytesToRead);
    static int write(FileDescriptor fd, void *buf, int numBytesToWrite);

    static int unlink(const char* fileToRemove);


    static FileMapping mmap(FileDescriptor fd, int offset, int size, 
                            bool isReadWrite);
    static int   munmap(FileMapping mapping, int len);
    static int   msync(char *addr, int len);
    static int   mprotect(char *addr, int len, bool isReadWrite);
};


                    // =====================================
                    // class bcecs_PersistentJournalIterator
                    // =====================================

class bcecs_PersistentJournalIterator {
    // This class is used to iterate over all confirmed or unconfirmed records
    // (see Usage) in either direction.  Also see the 'iterateor usage
    // warning'.


    mutable bdema_Allocator              *d_allocator_p;
    const bcecs_PersistentJournal        *d_journal_p;
    const bcecs_JournalRecordHeader            *d_current_p;
    mutable bcema_SharedPtr<void>        d_blobBufferSP; // dummy
    mutable bcema_Blob                   d_data; // valid only for a
                                                 // particular iteration
  public:
    typedef int RecordHandle;

  public:
    // CREATORS 
    bcecs_PersistentJournalIterator(const bcecs_PersistentJournalIterator &src,
                                    bdema_Allocator *allocator=0);

    bcecs_PersistentJournalIterator(const bcecs_PersistentJournal *journal,
                                    RecordHandle position,
                                    bdema_Allocator *allocator=0);
        // Create an iterator poininting to the specified 'position'.  

    ~bcecs_PersistentJournalIterator();


    // MANIPULATORS
    bcecs_PersistentJournalIterator& operator=(
                       const bcecs_PersistentJournalIterator&);

    bcecs_PersistentJournalIterator& operator++();
        // Increment the iterator so that it points to the next (newer)
        // record.  If this is the latest record added to its list,
        // set the iterator to be invalid.  It is an undefined behaviour to
        // invoke any method (except 'operator bool ()') on a invalid iterator.

    bcecs_PersistentJournalIterator& operator--();
        // Decrement the iterator so that it points to the previous (older)
        // record.  If this is the oldest record added to its list,
        // set the iterator to be invalid.  It is an undefined behaviour to
        // invoke any method (except 'operator bool ()') on a invalid iterator.

    // ACCESSORS
    RecordHandle operator*() const;
        // Return the record handle associated with this iterator.  It is an
        // undefined behaviour to invoke this method on an invalid
        // iterator. (see '++' and '--')

    bdet_Datetime     creationDateTime() const;
        // Return creation time of the record associated with this iterator.  
        // It is an undefined behaviour to invoke this method on an invalid 
        // iterator. (see '++' and '--')

    bdet_Datetime     confirmationDateTime() const;
        // Return confirmation time associated with this iterator.  It is an
        // undefined behaviour to invoke this method on an invalid iterator. 
        // (see '++' and '--').  If this record has not yet been confirmed, 
        // return an arbitrary value.  

    int               recordLength() const;
        // Return the length of the record associated with this iterator.  It
        // is an undefined behaviour to invoke this method on an invalid
        // iterator. (see '++' and '--')

    const bcema_Blob& data() const;
        // Return the data of the record associated with this iterator.  It is 
        // an undefined behaviour to invoke this method on an invalid 
        // iterator. (see '++' and '--')
        // *NOTE*: the returnd blob contents are valid only for the duration 
        // of current iteration; specifically, a ++ or -- will invalidate the
        // blob contents.  However the underlysing shared ptrs are valid even
        // after the iterator is destroyed; they will become invalide when
        // either the record is removed or the journal is closed.  

    int data(char *buf, int bufLen) const;
        // Fill the specified 'buf' with the data of the record 
        // associated with this iterator.  Write at most 'bufLen' chars and
        // return the number of the chars written.  It is an
        // undefined behaviour to invoke this method on an invalid iterator. 
        // (see '++' and '--')

    operator bool () const;
        // Return true if this record is valid, and false otherwise.  

    bool isConfirmed() const;
        // return true if the iterator points to a confirmed record and false
        // otherwise.
};



                        // =============================
                        // class bcecs_PersistentJournal
                        // =============================

class bcecs_PersistentJournal {
    // 'bcecs_PersistentJournal' is a thread-safe, persistent journal of
    // arbitrary sized records.  'addRecord' adds a record to the unconfirmed
    // list. 'confirmRecord' moves the specifed unconfirmed record to the
    // confirmed list.  A confirmed or unconfirmed record can be removed using
    // 'removeRecord'.  Removed records are put into free list to be reused.  

  public:
    // PUBLIC TYPES
    typedef bcecs_PersistentJournalFileUtil::FileDescriptor FileDescriptor; 

    typedef int RecordHandle; // handle for a record

    enum Mode {
        READ_ONLY  = 1, // only read allowed
        READ_WRITE = 3  // both read and write allowed
    };

    enum CommitMode {
          IMPLICIT_COMMIT = 1  // Update the journal but allow the system to
                               // decide when to synchronize the file with the
                               // disk

        , EXPLICIT_COMMIT = 2  // Explicity write all changes to disk before
                               // returning from all modifying functions
                               // (addRecord, confirmRecord, removeRecord).
                               // Note that it could be expensive.  
                               // Note that implict commits (by system) can
                               // still occur between explict commits.
    };

    enum ProtectionMode {
        PROTECTED,             // keep the mapped memory write protected to
                               // prevent journal corruption.  On each
                               // modifying call (like 'addRecord',
                               // 'removeRecord' and 'confirmRecord', the
                               // memory is unprotected for the duration of
                               // the call.  Note that having this mode might
                               // have some performance penalty on these calls.

        UNPROTECTED            // keep the memory unprotected.
    };
        // The default protection mode is 'UNPROTECTED'.

    enum Defaults {
          DEFAULT_BLOCK_SIZE      = 1024
        , DEFAULT_BLOCKS_PER_PAGE = 1024
#ifdef BDES_PLATFORM__OS_WINDOWS
        , DEFAULT_ALIGNMENT_SIZE  = 64*1024    
#else
        , DEFAULT_ALIGNMENT_SIZE  = 8*1024
#endif

    };



    enum ErrorCodes {
        NONE_ERROR            = 0
        , IO_ERROR            = 1
        , MEM_ERROR           = 2 // mmap failed
        , INVALID_FILE        = 3
        , INTERNAL_ERROR      = 4
        , BAD_STATE           = 5
        , FULL                = 6
        , NOT_FOUND           = 7
        , UNSUPPORTED_VERSION = 8
        , INVALID_RECORD_HANDLE  = 9 
        , FILE_NOT_EXISTS = 10
        , FILE_CREATION_FAILED = 11
        , PROTECTION_FAILURE = 12    // mprotect failed


        , INVALID_MAXFILESIZE = 13 

        , MAXRECORDSIZEFOREMPTYJOURNAL_VILOATION = 14 
                                     // see 'maxRecordSizeForEmptyJournal'


        , MAXRECORDSIZE_VILOATION = 15 // see 'maxRecordSize'

        , UNMAP_FAILURE = 16 // munmap failed
        , SYNC_FAILURE = 16 // msync failed
    };


  private:
    typedef bcecs_PersistentJournalFileUtil::FileMapping FileMapping;

    // PUBLIC STATIC DATA
  public:
    static FileDescriptor BAD_FD;

  private:
    bdema_Allocator                                   *d_allocator_p;
    int                                                d_maxFileSize;
    Mode                                               d_mode;
    CommitMode                                         d_commitMode;
    ProtectionMode                                     d_protectionMode;
    FileDescriptor                                     d_fd;
    int                                                d_workIndex;
    FileMapping                                        d_header;
    std::vector<std::pair<FileMapping, char*> >        d_pageTables;
    std::set<bcecs_JournalRecordHeader*>               d_dirtyRecords; 
    std::set<int>                                      d_dirtyPageIndices; 
    mutable bcemt_ReaderWriterLock                     d_lock;



    // PRIVATE METHODS
  private:

    const bcecs_PersistentJournalHeader* getHeader() const; // convinient 
    bcecs_PersistentJournalHeader* getHeader(); // convinient 

    int setProtection(bool readWrite);

    // transaction related functions 
    void beginTransaction();
    int  commitTransaction();
    void abortTransaction();


    // 'bcecs_RecordList' related functions.
    int pushBackRecordToRecordList(
        bcecs_RecordList           *list, 
        bcecs_JournalRecordHeader  *record);
    int unlinkRecordFromRecordList(
        bcecs_RecordList           *list, 
        bcecs_JournalRecordHeader* record);
    

    // 'bcecs_FreeBlockList' related functions
    int allocateBlocks(bcecs_JournalRecordHeader **result,
                       bcecs_FreeBlockList *list,
                       int numBlocks,
                       int *highestBlockIndexBuf); 
    // allocate 'numBlocks'.  load 'highestBlockIndexBuf'.  grow as
    // necessory.    
    int freeBlocks(bcecs_FreeBlockList *list, 
                   bcecs_JournalRecordHeader* recordHead);
    int freeBlocks(
        bcecs_FreeBlockList *list,
        bcecs_JournalRecordHeader* recordHead, 
        bcecs_JournalRecordHeader* recordTail,
        int numBlocks);


    // copy from or to record functions 
    int copyToRecord(
        bcecs_JournalRecordHeader* record, const bcema_Blob &data);
    int copyToRecord(bcecs_JournalRecordHeader *record,
                      const char *data, int length);
    int copyToRecord(
        bcecs_JournalRecordHeader** record, int* offset,
        const char* buf, int bufLen);
    int copyFromRecord(char *buf, int bufLen, 
                       const bcecs_JournalRecordHeader* record) const;



    // assert functions
    void assertInvariants() const;
    void assertInvariantsForFreeList(std::set<int> *blockSet) const;
                                     
    void assertInvariantsForRecordList(
        int recType, std::set<int> *blockSet) const;
    int assertInvariantsForRecord(
        const bcecs_JournalRecordHeader* record,
        int recType,
        std::set<int> *blockSet) const;

    // other helper functions
    int commitImpl();
        // commit without lock

    int getTailBlock(bcecs_JournalRecordHeader** result,
                     bcecs_JournalRecordHeader* recordHead);
    int reserveBlocks(int numBlocks);

    int removeRecordImpl(RecordHandle handle);
    // remove without putting any locks or transactions guards.
    
    int addRecord(RecordHandle *handleBuffer,
                  bool isBlob,
                  const bcema_Blob& blob,
                  const char *buf,
                  int bufLength); 


    int  growJournal();

    void dirtySync(int dstIndex);
    void sync(); 
    bcecs_JournalStateHeader* stateHeader();
    const bcecs_JournalStateHeader* stateHeader() const;
    bcecs_JournalRecordHeader* findRecord(int recordIndex);
    bcecs_JournalRecordHeader* findRecordForUpdate(int recordIndex);
    const bcecs_JournalRecordHeader* findRecord(int recordIndex) const;
    char* getDataBuffer(int index);
    const char* getDataBuffer(int index) const;
    bool isValidMaxFileSize(int value) const;
    void fillFreeBlocks(std::vector<int> *buf) const;

    int numFreeBlocks() const;
    int freeListHead();
    
    int confirmedListHead() const;
    int confirmedListTail() const;
    int numConfirmedBlocks() const;
    int numConfirmedBytes() const;
    
    int unconfirmedListHead() const;
    int unconfirmedListTail() const;
    int numUnconfirmedBlocks() const;
    int numUnconfirmedBytes() const;
    
    int numPageTables() const;
    int highestBlockUsed() const;
    
    // not implemented
    bcecs_PersistentJournal(const bcecs_PersistentJournal&);
    bcecs_PersistentJournal& operator=(const bcecs_PersistentJournal&);

    // friends
    friend class bcecs_JournalTransactionGuard;
    friend class bcecs_ProtectionModeGuard;
    friend class bcecs_PersistentJournalIterator;

    friend void bcecs_PersistentJournalTest(
        void *info, 
        const bcecs_PersistentJournal *o); // for testing only

  public:
    // CREATORS
    explicit bcecs_PersistentJournal(
        CommitMode       commitMode = IMPLICIT_COMMIT,
        bdema_Allocator *allocator  = 0);

    ~bcecs_PersistentJournal();

    // MANIPULATORS
    int openJournal(FileDescriptor fd, Mode mode);
        // Open the journal using the specified 'fd' and 'mode'.  Return 0 on
        // success.  

    int openJournal(const char *file,
                    Mode        mode,
                    bool        createIfNotFound = true,
                    int         userDataSize=0,

                    int         maxConfirmedRecordsKept=0, // -1 to ignore the
                                                           // limit

                    int         maxConfirmedBytesKept=0, // -1 to ignore the
                                                         // limit

                    int         blockSize=DEFAULT_BLOCK_SIZE,
                    int         blocksPerPage=DEFAULT_BLOCKS_PER_PAGE,
                    int         alignmentSize=DEFAULT_ALIGNMENT_SIZE);

        // Open the journal using the specified 'file'.  If a file already
        // exists than it must be a valid journal file.  If file does not
        // exists and 'createIfNotFound' is true than a new journal file is
        // created.          
        //
        // Internally, memory is mapped in chunks (called pages).  a page 
        // consists of data headers followed by data blocks.  A record is
        // consists of one or more blocks.  
        //
        // At any time, the size of confirmed list is limited 
        // by 'maxConfirmedRecordsKept' and 'maxConfirmedBytesKept'.
        // If the invocation to
        // 'confirmRecord' cause above limits to exceed, then 
        // the oldest confirmed records are removed to keep these limits.
        // 
        // Return 0 on success.

    int setMaxFileSize(int value);
        // Set the value of 'maxFileSize' parameter to the specified 'value'.
        // (see section 'maxFileSize' parameter).  Return 0 on success.  

    int close();
        // Close this journal.  

    int addRecord(RecordHandle *recordHandleBuffer,  // OUTPUT
                  const bcema_Blob &data);
    int addRecord(RecordHandle *handleBuffer,
                  const char *data,
                  int length);
        // Load the specified 'recordHandleBuffer' with
        // the RecordHandle, that can subsequently be used to refer the
        // specified record (for example, in 'confirmRecord' or
        // 'removeRecordImpl') .  Put this record at the end of the list of
        // unconfirmed records.  
        // The behaviour is undefined if data length is non-positive.  
        // Return 0 on success.  

    int commit();
        // Force sync all the modified record to hard disk.  return 0 on
        // success.  Note that this could be expensive, and a fairly
        // reasonable alternative could be relying on system to do that (by
        // specifying IMPLICIT_CIMMIT in the constructor).  

    int removeRecord(RecordHandle recordHandle);
       // Remove the record specified by 'recordHandle' and put in free list
       // for reuse.  Return 0 on success.

    int confirmRecord(RecordHandle recordhandle);
        // Move the record specified by the 'recordhandle' to the confirmed
        // list.  If addition of this record cause confirmed list to go beyond
        // 'maxConfirmedRecordKept' or 'maxConfirmedBytesKept' limits, than 
        // remove enough oldest confirmed records, so as to satisfy these
        // limits.  

    char* userData();
        // Return modifiable access to the user data.

    int setProtectionMode(ProtectionMode protectionMode);
        // Set the protection mode to the specified 'protectionMode' and
        // return 0 on success.  If the specified mode is
        // 'PROTECTED' and currently it is not protected than protect the
        // memory.  If the specified mode is
        // 'UNPROTECTED' and currently it is protected than unprotect the
        // memory.  



    FileDescriptor getFd();
        // Return the file descriptor of the underlying.  If this journal is
        // in closed state than return 'BAD_FD'.  

    // ACCESSORS
    bcecs_PersistentJournalIterator firstConfirmedRecord() const;
        // Return the iterator to the oldest confirmed record.

    bcecs_PersistentJournalIterator lastConfirmedRecord() const;
        // Return the iterator to the latest confirmed record.

    bcecs_PersistentJournalIterator firstUnconfirmedRecord() const;
        // Return the iterator to the oldest unconfirmed record.

    bcecs_PersistentJournalIterator lastUnconfirmedRecord()  const;
        // Return the iterator to the latest unconfirmed record.

    CommitMode commitMode() const;
         // Return the commit mode.  

    Mode mode() const;
        // Return the mode.  

    ProtectionMode protectionMode() const;
        // Return the current protectionMode.

    std::size_t userDataLength() const;
        // Return the user data lengtht.  

    bdet_Datetime journalCreationDateTime() const;
        // Return the journal creation datetime.  

    int blockSize() const;
        // Return the size of the block.

    int blocksPerPage() const;
        // Return the number of blocks per page.  

    int alignmentSize() const;
        // Return the alignment size.  

    int maxConfirmedRecordsKept() const;
        // Return the maximum confirmed records kept.  

    int maxConfirmedBytesKept() const;
        // Return the maximum confirmed bytes kept.  

    int numUnconfirmedRecords() const;
        // Return the number of unconfirmed records.

    int numConfirmedRecords() const;
        // Return the number of confirmed records.

    const char* userData() const;
        // Return non-modifiable access to the user data.  

    int maxRecordSize() const;
        // Return the largest possible size of any record that could be added
        // to this jounal.  This limit is imposed due to 'maxFileSize' limit.
        // A return value of -1 signifies that there is no limit.  Adding a
        // record beyond this size will return 'MAXRECORDSIZE_VILOATION'
        // error.  

    int maxRecordSizeForEmptyJournal() const;
        // Return the largest possible size of any record that could be added
        // to an *empty* jounal.  This limit is imposed due to 'maxFileSize'
        // limit.
        // A return value of -1 signifies that there is no limit.  
        // Adding a
        // record beyond this size will return
        // 'MAXRECORDSIZEFOREMPTYJOURNAL_VILOATION'

        // error.  

    int maxFileSize() const;
        // Return the 'maxFileSize' value.  

    std::ostream& printHeader(std::ostream& stream) const;
        // Used for debugging.  
};


// ===========================================================================
//                      INLINE & TEMPLATE FUNCTION DEFINITIONS
// ===========================================================================

                          // ----------------------
                          // class bcecs_RoundRobin
                          // ----------------------

// PRIVATE
inline const bcecs_PersistentJournalHeader* 
bcecs_PersistentJournal::getHeader() const 
{
    return ((const bcecs_PersistentJournalHeader *) d_header.addr());
}

inline bcecs_PersistentJournalHeader* 
bcecs_PersistentJournal::getHeader() 
{
    return ((bcecs_PersistentJournalHeader *) d_header.addr());
}

}  // close namespace BloombergLP


#endif


// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2006
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
