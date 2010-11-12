// baecs_journalheader.h                                              -*-C++-*-
#ifndef INCLUDED_BAECS_JOURNALHEADER
#define INCLUDED_BAECS_JOURNALHEADER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a container for the header information of a journal.
//
//@CLASSES:
//  baecs_JournalHeader: container for header information of a journal
//  baecs_JournalHeaderPageList: list of pages for a journal header
//  baecs_JournalHeaderRecordList: list of records for a journal header
//
//@SEE_ALSO: baecs_journalpageheader, baecs_journalparameters, baecs_journal
//
//@AUTHOR: Oleg Semenov (osemenov), Andrei Basov (abasov)
//
//@DESCRIPTION: This component defines a class, 'baecs_JournalHeader', that
// contains header information for a 'baecs_Journal' object.
//
///Usage
///-----
// This component is meant to used by higher level components in the 'baecs'
// package and should not be used directly by client code.  Therefore, a usage
// example is not provided.

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAECS_JOURNALPAGEHEADER
#include <baecs_journalpageheader.h>
#endif

#ifndef INCLUDED_BAECS_JOURNALPARAMETERS
#include <baecs_journalparameters.h>
#endif

#ifndef INCLUDED_BDESU_FILEUTIL
#include <bdesu_fileutil.h>
#endif

#ifndef INCLUDED_BDEUT_BIGENDIAN
#include <bdeut_bigendian.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_IOSTREAM
#include <bsl_iostream.h>
#endif

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>    // TBD DEPRECATED
#endif
#endif

namespace BloombergLP {

                        // =================================
                        // class baecs_JournalHeaderPageList
                        // =================================

class baecs_JournalHeaderPageList {

  public:
    // TYPES
    enum {
        BAECS_INVALID_PAGE = 0xFFFFFFFF
    };

    // DATA
    bdeut_BigEndianUint32 d_firstPage;
    bdeut_BigEndianUint32 d_last;
    bdeut_BigEndianUint32 d_numElements;

    // MANIPULATORS
    void init();

    unsigned addPage(unsigned page);
        // Add the specified 'page' to this page list, and return the number of
        // pages in the list following the addition.
};

                        // ===================================
                        // class baecs_JournalHeaderRecordList
                        // ===================================

struct baecs_JournalHeaderRecordList {

    // DATA
    bdeut_BigEndianUint32 d_first;
    bdeut_BigEndianUint32 d_last;
    bdeut_BigEndianUint32 d_numElements;

    // MANIPULATORS
    void init();
        // TBD doc
};

                        // =========================
                        // class baecs_JournalHeader
                        // =========================

class baecs_JournalHeader {
    // TBD doc

  public:
    // TYPES
    enum {
        BAECS_INVALID_RECORD_HANDLE = 0xFFFFFFFF
    };

    enum {
        BAECS_JOURNAL_MAGIC_NUMBER    = 0x4A524E4C,
        BAECS_JOURNAL_VERSION_CURRENT = 1
    };

    struct HeaderState {

        // DATA
        baecs_JournalPageHeader_TransactionId d_committedTransactionId;
        bdeut_BigEndianUint32                 d_numPages;
        bdeut_BigEndianUint32                 d_reserved;
        bdeut_BigEndianInt64                  d_modificationTime;
        baecs_JournalHeaderRecordList         d_confirmedRecords;
        baecs_JournalHeaderRecordList         d_unconfirmedRecords;
        baecs_JournalHeaderPageList           d_fillPages;
        baecs_JournalHeaderPageList           d_preFillPages;

        // CLASS METHODS
        static void copyState(HeaderState        *destination,
                              const HeaderState&  source);
    };

    struct OnDisk {
        enum {
            BAECS_NUM_STATES = 2
        };

        // immutable part
        bdeut_BigEndianUint32 d_magic;
        bdeut_BigEndianUint32 d_version;
        bdeut_BigEndianUint32 d_headerSize;
        bdeut_BigEndianUint32 d_pagesPerSet;
        bdeut_BigEndianUint32 d_blocksPerPage;
        bdeut_BigEndianUint32 d_blockSize;
        bdeut_BigEndianUint32 d_freeBlockThreshold;
        bdeut_BigEndianUint32 d_pageHeaderSize;
        bdeut_BigEndianUint32 d_pageDataSize;
        bdeut_BigEndianUint32 d_alignment;
        bdeut_BigEndianInt64  d_creationTime;
        bdeut_BigEndianUint32 d_userDataSize;
        bdeut_BigEndianUint32 d_reserve;
        HeaderState           d_state[BAECS_NUM_STATES];
        char                  d_userData[8];
    };

  private:
    // DATA
    bsls_Types::Int64  d_currentTransactionId;
    bsls_Types::Int64  d_committedTransactionId;
    int                d_activeStateIndex;      // must be in writable memory
    OnDisk            *d_persistent_p;          // could be in read-only memory

  public:
    // CREATORS
    baecs_JournalHeader();
    ~baecs_JournalHeader();

    // MANIPULATORS
    void addPages(int numPages);

    void init(int                            headerSize,
              int                            pageHeaderSize,
              int                            pageDataSize,
              int                            alignment,
              int                            userDataSize,
              const baecs_JournalParameters& param);

    void commitActiveState();
    void commitCurrentTransaction();
    void rollback();
    int recoverTransaction(bool offset);

    baecs_JournalHeaderPageList& fillPages();
    baecs_JournalHeaderPageList& preFillPages();

    baecs_JournalHeaderRecordList& confirmedRecords();
    baecs_JournalHeaderRecordList& unconfirmedRecords();

    char *userData();

    void setPersistentStore(void *store);

    // ACCESSORS
    bsl::ostream& print(bsl::ostream& stream) const;

    int freeBlockThreshold() const;
    const char *userData() const;
    int userDataSize() const;

    const baecs_JournalHeaderPageList& fillPages() const;
    const baecs_JournalHeaderPageList& preFillPages() const;
    const baecs_JournalHeaderRecordList& confirmedRecords() const;
    const baecs_JournalHeaderRecordList& unconfirmedRecords() const;

    int alignment() const;
    int blockSize() const;
    int blocksPerPage() const;

    bdesu_FileUtil::Offset calculateFileSize(int numPages);
    bsls_Types::Int64 creationTime() const;
    bsls_Types::Int64 currentTransactionId() const;
    bsls_Types::Int64 committedTransactionId() const;
    int headerSize() const;
    bsls_Types::Int64 modificationTime() const;
    int numPages() const;
    int numPageSets(int numPages) const;

    int pageHeaderSize() const;
    int pageDataSize() const;
    int pagesPerSet() const;
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&              stream,
                         const baecs_JournalHeader& header);
    // Format the specified 'header' to the specified 'stream', and return
    // a reference to the modifiable 'stream'.

// ----------------------------------------------------------------------------
//                      INLINE FUNCTION DEFINITIONS
// ----------------------------------------------------------------------------

                        // ---------------------------------
                        // class baecs_JournalHeaderPageList
                        // ---------------------------------

// MANIPULATORS
inline
void baecs_JournalHeaderPageList::init()
{
    d_firstPage   = BAECS_INVALID_PAGE;
    d_last        = BAECS_INVALID_PAGE;
    d_numElements = 0;
}

                        // -----------------------------------
                        // class baecs_JournalHeaderRecordList
                        // -----------------------------------

// MANIPULATORS
inline
void baecs_JournalHeaderRecordList::init()
{
    d_first       = baecs_JournalHeader::BAECS_INVALID_RECORD_HANDLE;
    d_last        = baecs_JournalHeader::BAECS_INVALID_RECORD_HANDLE;
    d_numElements = 0;
}

                        // -------------------------
                        // class baecs_JournalHeader
                        // -------------------------

// MANIPULATORS
inline
void baecs_JournalHeader::commitActiveState()
{
    d_activeStateIndex = 1 - d_activeStateIndex;
    HeaderState::copyState(&d_persistent_p->d_state[d_activeStateIndex],
                           d_persistent_p->d_state[1-d_activeStateIndex]);
    d_committedTransactionId = d_currentTransactionId;
    ++d_currentTransactionId;
}

inline
void baecs_JournalHeader::rollback()
{
    HeaderState::copyState(&d_persistent_p->d_state[d_activeStateIndex],
                           d_persistent_p->d_state[1-d_activeStateIndex]);
    ++d_currentTransactionId;
}

inline
void baecs_JournalHeader::setPersistentStore(void *store)
{
    d_persistent_p = (OnDisk*) store;
}

inline
baecs_JournalHeaderPageList& baecs_JournalHeader::fillPages()
{
    return d_persistent_p->d_state[d_activeStateIndex].d_fillPages;
}

inline
baecs_JournalHeaderPageList& baecs_JournalHeader::preFillPages()
{
    return d_persistent_p->d_state[d_activeStateIndex].d_preFillPages;
}

inline
baecs_JournalHeaderRecordList& baecs_JournalHeader::confirmedRecords()
{
    return d_persistent_p->d_state[d_activeStateIndex].d_confirmedRecords;
}
inline
baecs_JournalHeaderRecordList& baecs_JournalHeader::unconfirmedRecords()
{
    return d_persistent_p->d_state[d_activeStateIndex].d_unconfirmedRecords;
}

inline
char *baecs_JournalHeader::userData()
{
    return d_persistent_p->d_userData;
}

inline
void baecs_JournalHeader::addPages(int numPages)
{
    BSLS_ASSERT_SAFE(0 < numPages);

    d_persistent_p->d_state[d_activeStateIndex].d_numPages =
            d_persistent_p->d_state[d_activeStateIndex].d_numPages + numPages;
}

// ACCESSORS
inline
int baecs_JournalHeader::numPages() const
{
    return d_persistent_p->d_state[d_activeStateIndex].d_numPages;
}

inline
int baecs_JournalHeader::numPageSets(int numPages) const
{
    return (numPages + d_persistent_p->d_pagesPerSet - 1)
        / d_persistent_p->d_pagesPerSet;
}

inline
int baecs_JournalHeader::freeBlockThreshold() const
{
    return d_persistent_p->d_freeBlockThreshold;
}

inline
const char *baecs_JournalHeader::userData() const
{
    return d_persistent_p->d_userData;
}
inline
int baecs_JournalHeader::userDataSize() const
{
    return d_persistent_p->d_userDataSize;
}

inline
const baecs_JournalHeaderPageList& baecs_JournalHeader::fillPages() const
{
    return d_persistent_p->d_state[d_activeStateIndex].d_fillPages;
}

inline
const baecs_JournalHeaderPageList& baecs_JournalHeader::preFillPages() const
{
    return d_persistent_p->d_state[d_activeStateIndex].d_preFillPages;
}

inline
const baecs_JournalHeaderRecordList&
                                  baecs_JournalHeader::confirmedRecords() const
{
    return d_persistent_p->d_state[d_activeStateIndex].d_confirmedRecords;
}

inline
const baecs_JournalHeaderRecordList&
                                baecs_JournalHeader::unconfirmedRecords() const
{
    return d_persistent_p->d_state[d_activeStateIndex].d_unconfirmedRecords;
}

inline
bsls_Types::Int64 baecs_JournalHeader::modificationTime() const
{
    return d_persistent_p->d_state[d_activeStateIndex].d_modificationTime;
}

inline
bsls_Types::Int64 baecs_JournalHeader::creationTime() const
{
    return d_persistent_p->d_creationTime;
}

inline
int baecs_JournalHeader::pagesPerSet() const
{
    return d_persistent_p->d_pagesPerSet;
}

inline
int baecs_JournalHeader::alignment() const
{
    return d_persistent_p->d_alignment;
}

inline
int baecs_JournalHeader::blockSize() const
{
    return d_persistent_p->d_blockSize;
}

inline
int baecs_JournalHeader::blocksPerPage() const
{
    return d_persistent_p->d_blocksPerPage;
}

inline
int baecs_JournalHeader::pageHeaderSize() const
{
    return d_persistent_p->d_pageHeaderSize;
}

inline
int baecs_JournalHeader::pageDataSize() const
{
    return d_persistent_p->d_pageDataSize;
}

inline
int baecs_JournalHeader::headerSize() const
{
    return d_persistent_p->d_headerSize;
}

inline
bdesu_FileUtil::Offset baecs_JournalHeader::calculateFileSize(int numPages)
{
    bdesu_FileUtil::Offset result =  d_persistent_p->d_headerSize
        + (bdesu_FileUtil::Offset) numPageSets(numPages)
        * d_persistent_p->d_pageHeaderSize * d_persistent_p->d_pagesPerSet * 2
        + (bdesu_FileUtil::Offset) numPages
        * d_persistent_p->d_pageDataSize;
    return result;
}

inline
bsls_Types::Int64 baecs_JournalHeader::currentTransactionId() const
{
    return d_currentTransactionId;
}

inline
bsls_Types::Int64 baecs_JournalHeader::committedTransactionId() const
{
    return d_committedTransactionId;
}

inline
bsl::ostream& operator<<(bsl::ostream&              stream,
                         const baecs_JournalHeader& header)
{
    return header.print(stream);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
