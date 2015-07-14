// balj_journalheader.h                                              -*-C++-*-
#ifndef INCLUDED_BALJ_JOURNALHEADER
#define INCLUDED_BALJ_JOURNALHEADER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a container for the header information of a journal.
//
//@CLASSES:
//  balj::JournalHeader: container for header information of a journal
//  balj::JournalHeaderPageList: list of pages for a journal header
//  balj::JournalHeaderRecordList: list of records for a journal header
//
//@SEE_ALSO: balj_journalpageheader, balj_journalparameters, balj_journal
//
//@AUTHOR: Oleg Semenov (osemenov), Andrei Basov (abasov)
//
//@DESCRIPTION: This component defines a class, 'balj::JournalHeader', that
// contains header information for a 'balj_Journal' object.
//
///Usage
///-----
// This component is meant to used by higher level components in the 'baecs'
// package and should not be used directly by client code.  Therefore, a usage
// example is not provided.

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALJ_JOURNALPAGEHEADER
#include <balj_journalpageheader.h>
#endif

#ifndef INCLUDED_BALJ_JOURNALPARAMETERS
#include <balj_journalparameters.h>
#endif

#ifndef INCLUDED_BDLSU_XXXFILEUTIL
#include <bdlsu_xxxfileutil.h>
#endif

#ifndef INCLUDED_BDLB_BIGENDIAN
#include <bdlb_bigendian.h>
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

namespace BloombergLP {

namespace balj {
                        // =================================
                        // class JournalHeaderPageList
                        // =================================

class JournalHeaderPageList {

  public:
    // TYPES
    enum {
        BAECS_INVALID_PAGE = 0xFFFFFFFF
    };

    // DATA
    bdlb::BigEndianUint32 d_firstPage;
    bdlb::BigEndianUint32 d_last;
    bdlb::BigEndianUint32 d_numElements;

    // MANIPULATORS
    void init();

    unsigned addPage(unsigned page);
        // Add the specified 'page' to this page list, and return the number of
        // pages in the list following the addition.
};

                        // ===================================
                        // class JournalHeaderRecordList
                        // ===================================

struct JournalHeaderRecordList {

    // DATA
    bdlb::BigEndianUint32 d_first;
    bdlb::BigEndianUint32 d_last;
    bdlb::BigEndianUint32 d_numElements;

    // MANIPULATORS
    void init();
        // TBD doc
};

                        // =========================
                        // class JournalHeader
                        // =========================

class JournalHeader {
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
        JournalPageHeader_TransactionId d_committedTransactionId;
        bdlb::BigEndianUint32                 d_numPages;
        bdlb::BigEndianUint32                 d_reserved;
        bdlb::BigEndianInt64                  d_modificationTime;
        JournalHeaderRecordList         d_confirmedRecords;
        JournalHeaderRecordList         d_unconfirmedRecords;
        JournalHeaderPageList           d_fillPages;
        JournalHeaderPageList           d_preFillPages;

        // CLASS METHODS
        static void copyState(HeaderState        *destination,
                              const HeaderState&  source);
    };

    struct OnDisk {
        enum {
            BAECS_NUM_STATES = 2
        };

        // immutable part
        bdlb::BigEndianUint32 d_magic;
        bdlb::BigEndianUint32 d_version;
        bdlb::BigEndianUint32 d_headerSize;
        bdlb::BigEndianUint32 d_pagesPerSet;
        bdlb::BigEndianUint32 d_blocksPerPage;
        bdlb::BigEndianUint32 d_blockSize;
        bdlb::BigEndianUint32 d_freeBlockThreshold;
        bdlb::BigEndianUint32 d_pageHeaderSize;
        bdlb::BigEndianUint32 d_pageDataSize;
        bdlb::BigEndianUint32 d_alignment;
        bdlb::BigEndianInt64  d_creationTime;
        bdlb::BigEndianUint32 d_userDataSize;
        bdlb::BigEndianUint32 d_reserve;
        HeaderState           d_state[BAECS_NUM_STATES];
        char                  d_userData[8];
    };

  private:
    // DATA
    bsls::Types::Int64  d_currentTransactionId;
    bsls::Types::Int64  d_committedTransactionId;
    int                 d_activeStateIndex;     // must be in writable memory
    OnDisk             *d_persistent_p;         // could be in read-only memory

  public:
    // CREATORS
    JournalHeader();
    ~JournalHeader();

    // MANIPULATORS
    void addPages(int numPages);

    void init(int                            headerSize,
              int                            pageHeaderSize,
              int                            pageDataSize,
              int                            alignment,
              int                            userDataSize,
              const JournalParameters& param);

    void commitActiveState();
    void commitCurrentTransaction();
    void rollback();
    int recoverTransaction(bool offset);

    JournalHeaderPageList& fillPages();
    JournalHeaderPageList& preFillPages();

    JournalHeaderRecordList& confirmedRecords();
    JournalHeaderRecordList& unconfirmedRecords();

    char *userData();

    void setPersistentStore(void *store);

    // ACCESSORS
    bsl::ostream& print(bsl::ostream& stream) const;

    int freeBlockThreshold() const;
    const char *userData() const;
    int userDataSize() const;

    const JournalHeaderPageList& fillPages() const;
    const JournalHeaderPageList& preFillPages() const;
    const JournalHeaderRecordList& confirmedRecords() const;
    const JournalHeaderRecordList& unconfirmedRecords() const;

    int alignment() const;
    int blockSize() const;
    int blocksPerPage() const;

    bdlsu::FileUtil::Offset calculateFileSize(int numPages);
    bsls::Types::Int64 creationTime() const;
    bsls::Types::Int64 currentTransactionId() const;
    bsls::Types::Int64 committedTransactionId() const;
    int headerSize() const;
    bsls::Types::Int64 modificationTime() const;
    int numPages() const;
    int numPageSets(int numPages) const;

    int pageHeaderSize() const;
    int pageDataSize() const;
    int pagesPerSet() const;
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&        stream,
                         const JournalHeader& header);
    // Format the specified 'header' to the specified 'stream', and return
    // a reference to the modifiable 'stream'.

// ----------------------------------------------------------------------------
//                      INLINE FUNCTION DEFINITIONS
// ----------------------------------------------------------------------------

                        // ---------------------------------
                        // class JournalHeaderPageList
                        // ---------------------------------

// MANIPULATORS
inline
void JournalHeaderPageList::init()
{
    d_firstPage   = BAECS_INVALID_PAGE;
    d_last        = BAECS_INVALID_PAGE;
    d_numElements = 0;
}

                        // -----------------------------------
                        // class JournalHeaderRecordList
                        // -----------------------------------

// MANIPULATORS
inline
void JournalHeaderRecordList::init()
{
    d_first       = JournalHeader::BAECS_INVALID_RECORD_HANDLE;
    d_last        = JournalHeader::BAECS_INVALID_RECORD_HANDLE;
    d_numElements = 0;
}

                        // -------------------------
                        // class JournalHeader
                        // -------------------------

// MANIPULATORS
inline
void JournalHeader::commitActiveState()
{
    d_activeStateIndex = 1 - d_activeStateIndex;
    HeaderState::copyState(&d_persistent_p->d_state[d_activeStateIndex],
                           d_persistent_p->d_state[1-d_activeStateIndex]);
    d_committedTransactionId = d_currentTransactionId;
    ++d_currentTransactionId;
}

inline
void JournalHeader::rollback()
{
    HeaderState::copyState(&d_persistent_p->d_state[d_activeStateIndex],
                           d_persistent_p->d_state[1-d_activeStateIndex]);
    ++d_currentTransactionId;
}

inline
void JournalHeader::setPersistentStore(void *store)
{
    d_persistent_p = (OnDisk*) store;
}

inline
JournalHeaderPageList& JournalHeader::fillPages()
{
    return d_persistent_p->d_state[d_activeStateIndex].d_fillPages;
}

inline
JournalHeaderPageList& JournalHeader::preFillPages()
{
    return d_persistent_p->d_state[d_activeStateIndex].d_preFillPages;
}

inline
JournalHeaderRecordList& JournalHeader::confirmedRecords()
{
    return d_persistent_p->d_state[d_activeStateIndex].d_confirmedRecords;
}
inline
JournalHeaderRecordList& JournalHeader::unconfirmedRecords()
{
    return d_persistent_p->d_state[d_activeStateIndex].d_unconfirmedRecords;
}

inline
char *JournalHeader::userData()
{
    return d_persistent_p->d_userData;
}

inline
void JournalHeader::addPages(int numPages)
{
    BSLS_ASSERT_SAFE(0 < numPages);

    d_persistent_p->d_state[d_activeStateIndex].d_numPages =
            d_persistent_p->d_state[d_activeStateIndex].d_numPages + numPages;
}

// ACCESSORS
inline
int JournalHeader::numPages() const
{
    return d_persistent_p->d_state[d_activeStateIndex].d_numPages;
}

inline
int JournalHeader::numPageSets(int numPages) const
{
    return (numPages + d_persistent_p->d_pagesPerSet - 1)
        / d_persistent_p->d_pagesPerSet;
}

inline
int JournalHeader::freeBlockThreshold() const
{
    return d_persistent_p->d_freeBlockThreshold;
}

inline
const char *JournalHeader::userData() const
{
    return d_persistent_p->d_userData;
}
inline
int JournalHeader::userDataSize() const
{
    return d_persistent_p->d_userDataSize;
}

inline
const JournalHeaderPageList& JournalHeader::fillPages() const
{
    return d_persistent_p->d_state[d_activeStateIndex].d_fillPages;
}

inline
const JournalHeaderPageList& JournalHeader::preFillPages() const
{
    return d_persistent_p->d_state[d_activeStateIndex].d_preFillPages;
}

inline
const JournalHeaderRecordList&
                                  JournalHeader::confirmedRecords() const
{
    return d_persistent_p->d_state[d_activeStateIndex].d_confirmedRecords;
}

inline
const JournalHeaderRecordList&
                                JournalHeader::unconfirmedRecords() const
{
    return d_persistent_p->d_state[d_activeStateIndex].d_unconfirmedRecords;
}

inline
bsls::Types::Int64 JournalHeader::modificationTime() const
{
    return d_persistent_p->d_state[d_activeStateIndex].d_modificationTime;
}

inline
bsls::Types::Int64 JournalHeader::creationTime() const
{
    return d_persistent_p->d_creationTime;
}

inline
int JournalHeader::pagesPerSet() const
{
    return d_persistent_p->d_pagesPerSet;
}

inline
int JournalHeader::alignment() const
{
    return d_persistent_p->d_alignment;
}

inline
int JournalHeader::blockSize() const
{
    return d_persistent_p->d_blockSize;
}

inline
int JournalHeader::blocksPerPage() const
{
    return d_persistent_p->d_blocksPerPage;
}

inline
int JournalHeader::pageHeaderSize() const
{
    return d_persistent_p->d_pageHeaderSize;
}

inline
int JournalHeader::pageDataSize() const
{
    return d_persistent_p->d_pageDataSize;
}

inline
int JournalHeader::headerSize() const
{
    return d_persistent_p->d_headerSize;
}

inline
bdlsu::FileUtil::Offset JournalHeader::calculateFileSize(int numPages)
{
    bdlsu::FileUtil::Offset result =  d_persistent_p->d_headerSize
        + (bdlsu::FileUtil::Offset) numPageSets(numPages)
        * d_persistent_p->d_pageHeaderSize * d_persistent_p->d_pagesPerSet * 2
        + (bdlsu::FileUtil::Offset) numPages
        * d_persistent_p->d_pageDataSize;
    return result;
}

inline
bsls::Types::Int64 JournalHeader::currentTransactionId() const
{
    return d_currentTransactionId;
}

inline
bsls::Types::Int64 JournalHeader::committedTransactionId() const
{
    return d_committedTransactionId;
}
}  // close package namespace

inline
bsl::ostream& balj::operator<<(bsl::ostream&        stream,
                                const JournalHeader& header)
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
