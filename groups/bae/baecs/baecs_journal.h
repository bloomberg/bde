// baecs_journal.h                                                    -*-C++-*-
#ifndef INCLUDED_BAECS_JOURNAL
#define INCLUDED_BAECS_JOURNAL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a file-backed persistent journal.
//
//@CLASSES:
//  baecs_Journal: persistent journal backed by a file
//
//@AUTHOR: Oleg Semenov (osemenov), Andrei Basov (abasov)
//
//@DESCRIPTION: This component provides a file-backed persistent journal of
// variable length records.  Records can be added (i.e., stored) into a
// journal, deleted from a journal, and "confirmed" in a journal.  "Confirming"
// a record keeps the record in the journal while allowing the user to
// distinguish between "processed" and "not processed" records on disk.  The
// underlying journal file can grow arbitrarily large up to 2TB, even for
// 32-bit processes.  The 'baecs_Journal' class maps pages automatically as
// needed using an externally-pluggable mapping manager (see the
// 'baecs_mappingmanager' component).  The mapping manager can be shared
// between different journals thus guaranteeing that only one global limit
// exists on the number of pages mapped into the process space.  On the other
// end of the spectrum, each journal can have its own mapping manager if
// necessary.
//
// The journal supports a set of operating modes which allow the user to choose
// an acceptable compromise between integrity guarantees and performance for a
// particular use case.  The operating mode is defined by a set of bitwise-ORed
// mode flags, passed to 'open' and 'create' as the second argument.  The
// following mode flags are supported:
//
// 'BAECS_READWRITE': allow write access to the journal.  Note that the
// 'create' method requires the 'BAECS_READWRITE' mode flag to be set.
// 'BAECS_READWRITE' is the only mode flag which is enabled by default when no
// mode argument is passed to 'open' or 'create'.
//
// 'BAECS_READONLY' (default unless 'BAECS_READWRITE' is set): disallow write
// access to the journal.  The journal file is opened read-only and cannot be
// modified.
//
// 'BAECS_SAFE' (ignored unless 'BAECS_READWRITE' is set): enable transactional
// mode providing atomicity, consistency, and durability guarantees (isolation
// does not apply because access to the journal file is always exclusive).  If
// this mode is enabled, updates to the on-disk journal state are performed in
// a sequence of separate explicitly-committed transactions.  No modifications
// to on-disk journal state are made until a commit operation.  During a commit
// operation, the journal file is atomically and durably synchronized with
// in-memory journal state, after which a new transaction is started.  A
// transaction may be rolled back using the 'rollback' method.  Upon invocation
// of 'rollback', all modifications made to the journal since the last commit
// operation are undone.  A commit operation is automatically performed when
// the journal is closed.  Note that a commit operation has to physically
// synchronize the journal state with the disk media which is a relatively slow
// operation.
//
// 'BAECS_FAST' (default unless 'BAECS_SAFE' is set): do not enable
// transactional mode and rely on the operating system to synchronize the
// journal file with in-memory journal state.  In case of a hardware crash,
// or, in rare cases, in case of a process crash, some updates made to the
// journal may be lost, or the journal file may be left in an inconsistent
// state.
//
// 'BAECS_AUTO_COMMIT' (ignored unless 'BAECS_SAFE' is set): make every update
// to the journal (i.e., every invocation of 'addRecord', 'confirmRecord',
// 'removeRecord', 'setRecordAttributes', and 'alterRecordAttributes') a
// separate automatically-committed transaction.  Note that depending on a
// particular use case, particular performance requirements, and performance of
// the underlying disk media committing every journal update to disk might be
// prohibitively expensive.
//
// 'BAECS_MANUAL_COMMIT' (default unless 'BAECS_AUTO_COMMIT' is set): do not
// perform any automatic commit operations.  In this mode the user has to
// manually invoke the 'commit' method or close the journal to update the
// on-disk journal state.
//
// 'BAECS_PARANOID': perform extra consistency checks.  This method is for
// testing only.
//
// 'BAECS_RESERVE': this mode flag is deprecated.  Currently, it is always
// enabled.
//
// 'BAECS_READWRITE' is used when no mode flags are passed to the 'open' or
// 'create' methods.
//
// Each record in the journal has a mutable 16-bit attribute value (while
// the record data itself is immutable).  Updating this value is much more
// efficient than fetching the record data, modifying it, and adding the
// record to the journal again.  In case of large journal sizes, fetching
// this value may also be more efficient than fetching the record data, as
// it is stored in journal metadata, which is less likely to be unmapped or
// paged out than record data.  In case transactional mode is enabled,
// attribute updates provide the same transactional guarantees as any other
// updates of the journal.
//
// The journal optionally includes an area reserved for arbitrary user data.
// This is mapped memory which is flushed to disk by calling 'commit' or when
// the journal is closed.  Rollback and transactional guarantees are not
// available for this area.
//
///Thread-Safety
///-------------
// 'baecs_Journal' is thread-safe and thread-aware; that is, multiple threads
// may use their own journal objects or may concurrently use the same object,
// with certain natural limitations:
//
// * The 'open', 'create', and 'close' methods are not thread-safe and should
// not be called concurrently with any other methods.  You can access a
// 'baecs_Journal' object concurrently from multiple threads only after the
// call to 'open' or 'create' returns, and before 'close' is called.
//
// * You can access a record concurrently from multiple threads only after the
// call to the 'addRecord' method returns, and before the 'removeRecord' method
// is called.  The behavior of accessing a record and removing it at the same
// time is undefined.
//
///Process Safety
///--------------
// Multiple processes can safely open the same journal file for reading
// concurrently.  Only one process can open a journal for writing, and
// subsequent attempts to open the same journal for reading or writing will
// fail.  The journal relies on the operating system to provide an exclusive
// lock on the journal file.  Note that if the operating system is unable to
// provide properly functioning exclusive locks on a particular file system,
// like sometimes happens with network file systems, the integrity of the
// journal state on disk cannot be guaranteed in case of simultaneous access.
//
// As a particular example, users should remember that Unix-like operating
// systems do not provide file access exclusion between threads in the same
// process.  This means that attempts to open the same journal file for writing
// from different threads of the same process may succeed.  The behavior of
// such access is undefined.  Users should not attempt to open the same file
// multiple times from the same process.  The file should be opened once, and
// the 'baecs_Journal' object should be shared between the threads instead.
//
///Usage
///-----
// First, let's create a mapping manager.  For this example we'll assume that
// at most 100MB can be mapped into the process space by persistent journals.
// Note that you have to pass the number of supported page priorities required
// by 'baecs_Journal' as the second argument to the constructor of the mapping
// manager:
//..
//  enum { MAPPING_LIMIT = (1 << 20) * 100 };  // 100 MB
//  baecs_MappingManager mappingManager(MAPPING_LIMIT,
//                                      baecs_Journal::BAECS_NUM_PRIORITIES);
//..
// Secondly, create a journal object:
//..
//  bcema_TestAllocator tA;  // must use a thread-safe allocator
//  baecs_Journal mX(&mappingManager, &ta);
//..
// Now, generate a temporary file name and create the journal file.  We are
// going to use safe mode with automatic commit in this example.  You could
// also try modifying this usage example to use fast mode or safe mode with
// manual commit, and observing the difference in performance.
//..
//  char filename[64];
//  bsl::tmpnam_r(filename);
//
//  if (mX.create(filename, baecs_Journal::BAECS_READWRITE |
//                          baecs_Journal::BAECS_SAFE |
//                          baecs_Journal::BAECS_AUTO_COMMIT)) {
//      bsl::printf("Can't open journal %s: %d\n", filename, errno);
//      exit(-1);
//  }
//..
// Now add a large number of records to the journal:
//..
//  enum { NUM_RECORDS = 25000; }
//  for (int i = 0; i < NUM_RECORDS; ++i) {
//     enum { MAX_RECORD_SIZE = 13243 };
//     char record[MAX_RECORD_SIZE];
//     baecs_Journal::RecordHandle handle;
//     int rc = mX.addRecord(&handle, record, MAX_RECORD_SIZE - 1);
//     assert(0 == rc);
//     assert(baecs_Journal::BAECS_INVALID_RECORD_HANDLE != handle);
//  }
//..
// Verify the number of unconfirmed records in the journal:
//..
//  assert(NUM_RECORDS == mX.numUnconfirmedRecords());
//  assert(          0 == mX.numConfirmedRecords());
//..
// Iterate over the journal and verify the sizes of the records:
//..
//  baecs_Journal::RecordHandle handle;
//
//  baecs_Journal::RecordHandle handle = mX.firstUnconfirmedRecord();
//  while (handle != baecs_Journal::BAECS_INVALID_RECORD_HANDLE) {
//     int size = mX.getRecordLength(handle);
//     ASSERT(MAX_RECORD_SIZE - 1 == size);
//     handle = mX.nextUnconfirmedRecord(handle);
//  }
//..
// Confirm every record in the journal:
//..
//  handle = mX.firstUnconfirmedRecord();
//  while (handle != baecs_Journal::BAECS_INVALID_RECORD_HANDLE) {
//     baecs_Journal::RecordHandle next = mX.nextUnconfirmedRecord(handle);
//     mX.confirmRecord(handle);
//     handle = next;
//  }
//..
// Verify the number of confirmed records in the journal:
//..
//  assert(          0 == mX.numUnconfirmedRecords());
//  assert(NUM_RECORDS == mX.numConfirmedRecords());
//..
// Close the journal and delete the underlying file:
//..
//  mX.close();
//  bdesu_FileUtil::remove(filename);
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAECS_JOURNALHEADER
#include <baecs_journalheader.h>
#endif

#ifndef INCLUDED_BAECS_JOURNALPARAMETERS
#include <baecs_journalparameters.h>
#endif

#ifndef INCLUDED_BAECS_MAPPINGMANAGER
#include <baecs_mappingmanager.h>
#endif

#ifndef INCLUDED_BCEMA_BLOB
#include <bcema_blob.h>
#endif

#ifndef INCLUDED_BCEMA_POOLALLOCATOR
#include <bcema_poolallocator.h>
#endif

#ifndef INCLUDED_BCEMT_RWMUTEX
#include <bcemt_rwmutex.h>
#endif

#ifndef INCLUDED_BDESU_FILEUTIL
#include <bdesu_fileutil.h>
#endif

#ifndef INCLUDED_BDET_DATETIME
#include <bdet_datetime.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>
#endif

#ifndef INCLUDED_BSL_HASH_MAP
#include <bsl_hash_map.h>
#endif

#ifndef INCLUDED_BSL_LIST
#include <bsl_list.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#ifndef INCLUDED_BSLFWD_BSLMA_ALLOCATOR
#include <bslfwd_bslma_allocator.h>
#endif

namespace BloombergLP {

class bcema_Blob;

class baecs_Journal;

class baecs_Journal_PageHeaderReadGuard;
class baecs_Journal_PageHeaderWriteGuard;
class baecs_JournalLockGuard;
class baecs_JournalReadLockGuard;
class baecs_JournalPageHeader;
class baecs_Journal_BlockRef;

struct baecs_Journal_SegmentHeader;

                     // ===================
                     // class baecs_Journal
                     // ===================

class baecs_Journal {
    // TBD doc

    // FRIENDS
    friend class baecs_Journal_PageHeaderReadGuard;
    friend class baecs_Journal_PageHeaderWriteGuard;
    friend class baecs_Journal_LockGuard;
    friend class baecs_Journal_ReadLockGuard;
    friend class baecs_Journal_BlockRef;

    typedef baecs_MappingManager::Handle PageHandle;

  public:
    typedef unsigned RecordHandle;  // typedef for the record handle

    enum {
        BAECS_INVALID_RECORD_HANDLE = 0xFFFFFFFF
#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
      , INVALID_RECORD_HANDLE = BAECS_INVALID_RECORD_HANDLE
#endif
    };

    enum {
        BAECS_READONLY           = 0,  // no write access
        BAECS_READWRITE          = 1,  // write access
        BAECS_FAST               = 0,  // no transactions
        BAECS_SAFE               = 2,  // transaction support;
                                       // needs BAECS_READWRITE
        BAECS_MANUAL_COMMIT      = 0,  // user calls commit manually
        BAECS_AUTO_COMMIT        = 4,  // every add/remove/confirm commits
        BAECS_PARANOID           = 8,  // extra checks (for testing only)
        BAECS_RESERVE            = 16  // preallocate disk space
                                       // (deprecated, now always enabled)
#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
      , MODE_READONLY      = BAECS_READONLY
      , MODE_READWRITE     = BAECS_READWRITE
      , MODE_FAST          = BAECS_FAST
      , MODE_SAFE          = BAECS_SAFE
      , MODE_MANUAL_COMMIT = BAECS_MANUAL_COMMIT
      , MODE_AUTO_COMMIT   = BAECS_AUTO_COMMIT
      , MODE_PARANOID      = BAECS_PARANOID
      , MODE_RESERVE       = BAECS_RESERVE
#endif
    };

    enum {
        BAECS_PRIORITY_DATA     = 0,
        BAECS_PRIORITY_METADATA = 1,
        BAECS_NUM_PRIORITIES    = BAECS_PRIORITY_METADATA + 1
#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
      , PRIORITY_DATA     = BAECS_PRIORITY_DATA
      , PRIORITY_METADATA = BAECS_PRIORITY_METADATA
      , NUM_PRIORITIES    = BAECS_NUM_PRIORITIES
#endif
    };

    /*
        valid modes:
        READONLY
        READWRITE | FAST
        READWRITE | SAFE | MANUAL_COMMIT
        READWRITE | SAFE | AUTO_COMMIT
    */

    enum {
        BAECS_WRITE_ACCESS_REQUIRED_ERROR     = -1,
        BAECS_IO_ERROR                        = -2,
        BAECS_MMAP_ERROR                      = -3,
        BAECS_UNABLE_TO_ROLLBACK_ERROR        = -4,
        BAECS_FILE_NOT_FOUND_ERROR            = -5,
        BAECS_FORMAT_ERROR                    = -6,
        BAECS_ALIGNMENT_ERROR                 = -7,
        BAECS_UNSUPPORTED_VERSION_ERROR       = -8,
        BAECS_INVALID_STATE_ERROR             = -9,
        BAECS_INVALID_PARAMETERS_ERROR        = -10,
        BAECS_INVALID_HANDLE_ERROR            = -11,
        BAECS_VALIDATION_ERROR                = -12,
        BAECS_UNABLE_TO_LOCK_ERROR            = -13
#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
      , WRITE_ACCESS_REQUIRED_ERROR = BAECS_WRITE_ACCESS_REQUIRED_ERROR
      , IO_ERROR                    = BAECS_IO_ERROR
      , MMAP_ERROR                  = BAECS_MMAP_ERROR
      , UNABLE_TO_ROLLBACK_ERROR    = BAECS_UNABLE_TO_ROLLBACK_ERROR
      , FILE_NOT_FOUND_ERROR        = BAECS_FILE_NOT_FOUND_ERROR
      , FORMAT_ERROR                = BAECS_FORMAT_ERROR
      , ALIGNMENT_ERROR             = BAECS_ALIGNMENT_ERROR
      , UNSUPPORTED_VERSION_ERROR   = BAECS_UNSUPPORTED_VERSION_ERROR
      , INVALID_STATE_ERROR         = BAECS_INVALID_STATE_ERROR
      , INVALID_PARAMETERS_ERROR    = BAECS_INVALID_PARAMETERS_ERROR
      , INVALID_HANDLE_ERROR        = BAECS_INVALID_HANDLE_ERROR
      , VALIDATION_ERROR            = BAECS_VALIDATION_ERROR
      , UNABLE_TO_LOCK_ERROR        = BAECS_UNABLE_TO_LOCK_ERROR
#endif
    };

  private:
    enum {
        BAECS_RECORD_CONFIRMED = 0x01,
        BAECS_RECORD_COMMITTED = 0x02
    };

    typedef bdesu_FileUtil::FileDescriptor FileDescriptor;
    typedef bdesu_FileUtil::Offset         Offset;

    baecs_MappingManager     *d_mappingManager_p;

    typedef bsl::hash_map<RecordHandle, int> DebugRecordMap;
    DebugRecordMap            d_debugRecordMap;

    unsigned                  d_mode;

    bdesu_FileUtil::Offset    d_fileSize;

    FileDescriptor            d_fd;

    bsl::string               d_filename;

    baecs_JournalHeader       d_header;
    void                     *d_headerPage;

    PageHandle                d_headerHandle;

    bsl::vector<PageHandle>   d_pageSetHeaderHandles;
    bsl::vector<PageHandle>   d_pageHandles;
    mutable bsl::vector<int>  d_pageWorkIndexes;
    mutable bcemt_Mutex       d_workIndexLock;

    mutable bcemt_RWMutex     d_lock;

    baecs_MappingManager::PageListHandle
                              d_dirtyListHandle;

    bdesu_FileUtil::Offset    d_diskSpaceWarningThreshold;

    bdesu_FileUtil::Offset    d_diskSpaceErrorThreshold;

    mutable bcema_PoolAllocator d_poolAllocator;
    bslma_Allocator          *d_allocator_p;

    // private methods

    baecs_JournalPageHeader*
          getPageHeader(unsigned page, bool makeDirty) const;
    void releasePageHeader(unsigned page) const;

    char *getPageData(unsigned page, bool makeDirty) const;
    baecs_Journal_SegmentHeader* segmentHeaderForBlock(unsigned page,
                                                       unsigned block,
                                                       bool     isDirty) const;
    void releasePageData(unsigned page) const;

    int growJournal(bool init);

    int growFile(FileDescriptor         fd,
                 bdesu_FileUtil::Offset size,
                 bool                   reserve);
        // Grow the file with the specified 'fd' file descriptor to the
        // specified 'size'.  Return 0 on success and a non-zero value
        // otherwise.  The behavior is undefined unless 'fd' refers to
        // a valid file descriptor.  If this method is successful,
        // 'd_fileSize' >= 'size'.
        // WARNING: This method MUST BE called under WRITE lock.
    bdesu_FileUtil::Offset getFileSize(FileDescriptor fd);

    baecs_JournalPageHeader* getAvailablePageHeader(
                                      unsigned *page, unsigned numPages);

    void addRecordToList(baecs_JournalHeaderRecordList* list, unsigned handle);
    void removeRecordFromList(baecs_JournalHeaderRecordList* list,
                              unsigned handle);

    void addPageToList(baecs_JournalHeaderPageList* list, unsigned page);
    int markPageListForValidation(
                  const baecs_JournalHeaderPageList* list,
                  unsigned* map,
                  int index,
                  bool verbose) const;

    void handle2PageBlock(unsigned *page, unsigned *block,
                          RecordHandle handle) const
    {
        BSLS_ASSERT_SAFE(BAECS_INVALID_RECORD_HANDLE != handle);
        *page = handle / d_header.blocksPerPage();
        *block = handle % d_header.blocksPerPage();

    }

    int openImpl(const char* filename, unsigned mode);

    int closeImpl();

    int commitImpl();
    int rollbackImpl();
    void beginTransaction();

    void removeRecordImpl(unsigned headHandle);

    int markRecordForValidation(
                  RecordHandle handle,
                  unsigned* map,
                  bool verbose) const;

    int validateImpl(bool verbose) const;

    int addRecord(RecordHandle         *record,
                  const bcema_Blob     *blob,
                  void*                 data,
                  unsigned              size,
                  unsigned              attributes);

    int getRecordData(bcema_Blob    *blob,
                      char          *buf,
                      unsigned       bufSize,
                      RecordHandle   handle) const;

    RecordHandle nextRecord(RecordHandle handle) const;
        // Return the record following the one with the specified 'handle'.

    // not implemented
    baecs_Journal(const baecs_Journal&);
    baecs_Journal& operator=(const baecs_Journal&);

  public:
    // CREATORS
    baecs_Journal(baecs_MappingManager *mappingManager,
                  bslma_Allocator      *basicAllocator = 0);
        // Create a journal that uses the specified 'mappingManager' for
        // page management.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the
        // currently-installed default allocator is used.

    ~baecs_Journal();
        // Destroy this journal.

    // MANIPULATORS
    int addRecord(RecordHandle         *handle,
                  const bcema_Blob&     record,
                  unsigned              attributes = 0);
    int addRecord(RecordHandle         *handle,
                  void                 *record,
                  unsigned              size,
                  unsigned              attributes = 0);
        // Store the specified 'record' (of the specified 'size', if
        // applicable) to this journal and load the specified 'handle' with
        // the value that can subsequently be used for referring to this data.
        // Return 0 on success and a non-zero error code otherwise.

    int close();
        // Close this journal.  Return 0 on success and a non-zero error
        // code otherwise.

    int commit();
        // Commit the changes made since the last call to this method to
        // physical storage.
        // Return 0 on success and a non-zero error code otherwise.
        // Note that this method is a no-op unless SAFE mode is used.
        // Note that if AUTO_COMMIT mode is used, all changes are committed
        // automatically and there is no need to use this method.

    int rollback();
        // Roll back the changes made since the last call to commit(), except
        // any changes in the reserved user data area.
        // Return 0 on success and non-zero error code otherwise.
        // Note that this method returns an error unless SAFE mode is used.
        // Note that if AUTO_COMMIT mode is used, all changes are committed
        // automatically and rolling back is impossible.

    int confirmRecord(RecordHandle handle);
        // Move the record having the specified 'handle' to the list of
        // "confirmed" records.  Return 0 on success and a non-zero error
        // code otherwise.  The behavior is undefined unless 'handle' is the
        // handle of a record currently stored in this journal and it has not
        // already been confirmed.

    int removeRecord(RecordHandle handle);
        // Delete the record with the specified 'handle' from this journal.
        // Return 0 on success and a non-zero error code otherwise.  The
        // behavior is undefined unless 'handle' is the handle of a record
        // currently stored in the journal.

    int setRecordAttributes(RecordHandle handle, unsigned attributes);
        // Assign the specified 'attributes' to the record with the
        // specified 'handle'.  Return 0 on success and a non-zero error
        // code otherwise.  The behavior is undefined unless 'handle' is the
        // handle of a record currently stored in the journal.

    int alterRecordAttributes(unsigned         *outAttributes,
                              RecordHandle      handle,
                              unsigned          andMask,
                              unsigned          orMask);
        // Atomically alter the attributes of the record with the specified
        // 'handle' according to the specified 'andMask' and 'orMask':
        // 'newAttributes = oldAttributes & andMask | orMask'.  If
        // 'outAttributes' is not zero, load the specified 'outAttributes'
        // with the new value of the record attributes.  Return 0 on success
        // and a non-zero error code otherwise.  The behavior is undefined
        // unless 'handle' is the handle of a record currently stored in the
        // journal.

    int open(const char                   *filename,
             unsigned                      mode = BAECS_READWRITE);
        // Open the journal backed by the specified 'filename' in the
        // optionally-specified 'mode'.  If 'mode' is not specified,
        // 'BAECS_READWRITE' is assumed.  Return 0 on success and a non-zero
        // error code otherwise.

    int create(const char                       *filename,
               unsigned                          mode = BAECS_READWRITE,
               unsigned                          userDataSize = 0,
               const baecs_JournalParameters&    parameters =
                                      baecs_JournalParameters());

        // Open the journal with the specified 'filename' using the specified
        // 'mode' and with a reserved user data area of the specified
        // 'userDataSize'.  If the file does not exist, create a new journal
        // backed by the specified 'filename' using the specified 'parameters'.
        // If 'mode' is not specified, 'BAECS_READWRITE' is assumed.  Return 0
        // on success and a non-zero error code otherwise.  Note that if
        // the file exists, 'parameters' and 'userDataSize' are ignored.

    char *userData();
        // Return the address of the modifiable user data.

    const char *userData() const;
        // Return the address of the non-modifiable user data.

    int validate(bool verbose) const;
        // Validate the contents of the journal.  This method is for testing
        // only.

    void setDiskSpaceLoggingThresholds(bdesu_FileUtil::Offset warningThreshold,
                                       bdesu_FileUtil::Offset errorThreshold);
        // Set the minimum available space for journal growth, in bytes,
        // below which this journal object starts producing warning/error
        // messages every time the journal file is grown, to the specified
        // 'warnThreshold' and the specified 'errorThreshold' respectively.
        // Note that the default threshold values are zero, so
        // no logging is performed by default.  Note that the space available
        // for growth is defined both by available disk space and current
        // ulimit settings.

    // ACCESSORS
    unsigned blockSize() const;
        // Return the size of the block.

    unsigned blocksPerPage() const;
        // Return the number of blocks per page.

    const char *filename() const;
        // Return the file name used by this journal.

    unsigned freeBlockThreshold() const;
        // Return the free block threshold.

    unsigned mode() const;
        // Return the mode.

    bdet_Datetime journalCreationDatetime() const;
        // Return the journal creation datetime.

    bdet_Datetime journalModificationDatetime() const;
        // Return the journal modification datetime.

    unsigned pagesPerSet() const;
        // Return the number of pages per set.

    unsigned userDataSize() const;
        // Return the length of the user data.

    bsls_PlatformUtil::Int64 getFileSize() const;
        // Return the current size of the backing file in bytes.

    int recordFragmentation(int *numBlocks, int *numPages,
                            RecordHandle handle) const;
        // Load into the specified 'numBlocks' the number of non-sequential
        // segments occupied by the record having the specified 'handle'.
        // Load into the specified 'numPages' the number of pages occupied
        // by this record.  Return 0 on success and a non-zero error code
        // otherwise.

    int getRecordData(bcema_Blob *blob, RecordHandle handle) const;
        // Load into the specified 'blob' a reference to the data of the
        // record with the specified 'handle'.  Return the number of bytes
        // in the record.  The behavior is undefined unless 'handle' is the
        // handle of a record currently in this journal.  The behavior of
        // modifying the data loaded into the 'blob' is undefined, i.e.,
        // this method may NOT be used to modify the contents of the record,
        // which are immutable except for record attributes.  The behavior
        // of accessing the data loaded into the 'blob' becomes undefined
        // after the record with the specified 'handle' is removed, or the
        // journal is closed.  Note that while the loaded references to
        // record data remain in the specified 'blob', the data pages of the
        // journal which contain the data are pinned in memory and cannot be
        // unmapped even if the mapping limit of the mapping manager used by
        // this journal is reached.  In a 32-bit process pinning too many
        // pages can exhaust process address space.

    int copyRecordData(char *buffer, unsigned size, RecordHandle handle) const;
        // Copy into the specified 'buffer' of the specified 'size' the data
        // of the record with the specified 'handle'.  Return the length of
        // the record.  The behavior is undefined unless 'handle' is the
        // handle of a record currently in this journal.  Note that if the
        // length of the record is larger than 'size', at most 'size' bytes
        // will be copied into 'buffer'.

    unsigned getRecordLength(RecordHandle handle) const;
        // Return the size of the record having the specified 'handle.  The
        // behavior is undefined unless 'handle' is the handle of a record
        // currently in this journal.

    unsigned getRecordAttributes(RecordHandle handle) const;
        // Return the attributes of the record having the specified 'handle' as
        // set by 'addRecord', 'setRecordAttributes' or
        // 'alterRecordAttributes'.

    bdet_Datetime getRecordCreationDatetime(RecordHandle handle) const;
        // Return the creation time of the record having the specified
        // 'handle.  The behavior is undefined unless 'handle' is the handle of
        // a record currently in this journal.

    bool isConfirmedRecord(RecordHandle handle) const;
    bool isUnconfirmedRecord(RecordHandle handle) const;
        // Return 'true' if the record with the specified 'handle'
        // is confirmed (or unconfirmed) respectively.  The behavior is
        // undefined unless 'handle' is the handle of a record currently in
        // this journal.

    RecordHandle firstConfirmedRecord() const;
        // Return the record handle for the first confirmed record in this
        // journal or 'BAECS_INVALID_RECORD_HANDLE', if there are no
        // confirmed records.

    RecordHandle firstUnconfirmedRecord() const;
        // Return the record handle for the first unconfirmed record in this
        // journal or 'BAECS_INVALID_RECORD_HANDLE', if there are no
        // confirmed records.

    RecordHandle nextConfirmedRecord(RecordHandle handle) const;
        // Return the record handle of the confirmed record following the
        // confirmed record with the specified 'handle' in this journal.
        // Return 'BAECS_INVALID_RECORD_HANDLE' if there is no such record
        // or if 'handle' is 'BAECS_INVALID_RECORD_HANDLE'.  The behavior is
        // undefined unless 'handle' is 'BAECS_INVALID_RECORD_HANDLE' or the
        // handle of a confirmed record currently in the journal.

    RecordHandle nextUnconfirmedRecord(RecordHandle handle) const;
        // Return the record handle of the unconfirmed record following an
        // unconfirmed record with the specified 'handle' in this journal.
        // Return 'BAECS_INVALID_RECORD_HANDLE' if there is no such record
        // or if 'handle' is 'BAECS_INVALID_RECORD_HANDLE'.  The behavior is
        // undefined unless 'handle' is 'BAECS_INVALID_RECORD_HANDLE' or the
        // handle of an unconfirmed record currently in the journal.  In
        // particular, the behavior is undefined if 'handle' was returned
        // from 'firstUnconfirmedRecord' or 'nextUnconfirmedRecord' and
        // subsequently confirmed.

    unsigned numUnconfirmedRecords() const;
        // Return the number of unconfirmed records in this journal.

    unsigned numConfirmedRecords() const;
        // Return the number of confirmed records in this journal.

    static const char* errorMessage(int errorCode);
        // Return the text of the error message corresponding to the
        // specified 'errorCode' returned from one of the methods of
        // 'baecs_Journal', or a string "unknown error" if the 'errorCode'
        // is not recognized.
};

// ---------------------------------------------------------------------------
//                 INLINE FUNCTION DEFINITIONS
// ---------------------------------------------------------------------------

                        // -------------------
                        // class baecs_Journal
                        // -------------------

inline
int baecs_Journal::addRecord(baecs_Journal::RecordHandle *handle,
                             const bcema_Blob&            record,
                             unsigned                     attributes)
{
    return addRecord(handle, &record, 0, 0, attributes);
}

inline
int baecs_Journal::addRecord(baecs_Journal::RecordHandle *handle,
                             void                        *data,
                             unsigned                     size,
                             unsigned                     attributes)
{
    return addRecord(handle, 0, data, size, attributes);
}

inline
int baecs_Journal::getRecordData(bcema_Blob         *blob,
                                 RecordHandle        handle) const
{
    return getRecordData(blob, 0, 0, handle);
}

inline
int baecs_Journal::copyRecordData(char               *buffer,
                                  unsigned            size,
                                  RecordHandle        handle) const
{
    return getRecordData(0, buffer, size, handle);
}

inline
int baecs_Journal::setRecordAttributes(RecordHandle handle,
                                       unsigned attributes)
{
    return alterRecordAttributes(0, handle, 0, attributes);
}

// ACCESSORS
inline
const char *baecs_Journal::filename() const {
    return d_filename.c_str();
}

inline
unsigned baecs_Journal::mode() const {
    return d_mode;
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
