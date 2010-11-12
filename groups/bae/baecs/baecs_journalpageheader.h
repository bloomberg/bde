// baecs_journalpageheader.h                                          -*-C++-*-
#ifndef INCLUDED_BAECS_JOURNALPAGEHEADER
#define INCLUDED_BAECS_JOURNALPAGEHEADER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a container for the header information of a journal page.
//
//@CLASSES:
//  baecs_JournalPageHeader: header information for a page in a journal
//
//@SEE_ALSO: baecs_journalheader, baecs_journalparameters, baecs_journal
//
//@AUTHOR: Oleg Semenov (osemenov), Andrei Basov (abasov)
//
//@DESCRIPTION: This component defines a class, 'baecs_JournalPageHeader', that
// contains header information for a journal page.
//
///Usage
///-----
// This component is meant to used by higher level components in the 'baecs'
// package and should not be used directly by client code.  Therefore, a usage
// example is not provided.

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BDEUT_BIGENDIAN
#include <bdeut_bigendian.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>    // TBD DEPRECATED
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_OSTREAM
#include <bsl_ostream.h>
#endif

namespace BloombergLP {

                   // ===========================================
                   // class baecs_JournalPageHeader_TransactionId
                   // ===========================================

class baecs_JournalPageHeader_TransactionId {
    // TBD class-level doc

    // DATA
    bdeut_BigEndianInt64 d_data[4];
    // Four copies of the transaction id are stored to make the update atomic.
    // Assuming the data in a block on disk is updated linearly (but not
    // necessarily in any particular direction), and assuming all four
    // values are kept in the same block, we can be sure that either the
    // first or the last pair will be consistent regardless of what part of
    // the update has completed.

    // FRIENDS
    friend bsl::ostream& operator<<(
                                 bsl::ostream&,
                                 const baecs_JournalPageHeader_TransactionId&);

  public:
    // MANIPULATORS
    void setValue(const bsls_Types::Int64& rhs)
    {
        // TBD: Should we be concerned with the reordering of writes?
        /*
        *(volatile bdeut_BigEndianInt64*)(d_data+0) = rhs;
        *(volatile bdeut_BigEndianInt64*)(d_data+1) = rhs;
        *(volatile bdeut_BigEndianInt64*)(d_data+2) = rhs;
        *(volatile bdeut_BigEndianInt64*)(d_data+3) = rhs;
        */
        d_data[0] = rhs;
        d_data[1] = rhs;
        d_data[2] = rhs;
        d_data[3] = rhs;
        // full barrier - provided by d_workIndexLock
    }

    // ACCESSORS
    int getValue(bsls_Types::Int64 *outValue) const
    {
        if (d_data[0] == d_data[1]) {
            *outValue = d_data[0];
            return 0;
        } else if (d_data[2] == d_data[3]) {
            *outValue = d_data[2];
            return 0;
        } else {
            return 1;
        }
    }
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&                                stream,
                         const baecs_JournalPageHeader_TransactionId& rhs);

                         // =============================
                         // class baecs_JournalPageHeader
                         // =============================

class baecs_JournalPageHeader {
    // TBD doc

  public:
    // TYPES
    enum {
        BAECS_INVALID_RECORD_HANDLE = 0xFFFFFFFF
    };

    enum {
        BAECS_ATTRIBUTES_SHIFT    = 16,
        BAECS_ATTRIBUTES_MASK     = 0xFFFF0000,
        BAECS_NEXT_BLOCK_SHIFT    = 3,
        BAECS_NEXT_BLOCK_MASK     = 0x0000FFF8,
        BAECS_CONFIRMED_SHIFT     = 2,
        BAECS_CONFIRMED_MASK      = 0x00000004,
        BAECS_EXTENDED_SHIFT      = 1,
        BAECS_EXTENDED_MASK       = 0x00000002,
        BAECS_HEAD_SHIFT          = 0,
        BAECS_HEAD_MASK           = 0x00000001,
        BAECS_INDEX_NONE          =
                               BAECS_NEXT_BLOCK_MASK >> BAECS_NEXT_BLOCK_SHIFT,
        BAECS_MAX_BLOCKS_PER_PAGE = BAECS_INDEX_NONE
    };

    class BlockHeader {
        // TBD doc

        // DATA
        bdeut_BigEndianUint32 d_nextRecord;
        bdeut_BigEndianUint32 d_prevRecord;
        bdeut_BigEndianUint32 d_extraData;

      public:
        // MANIPULATORS
        void init();
        void setNextRecord(unsigned nextRecord);
        void setPrevRecord(unsigned prevRecord);
        void setAttributes(unsigned attributes);
        void setNextBlock(unsigned nextBlock);
        void setConfirmed(bool isConfirmed);
        void setExtended(bool isExtended);
        void setHead(bool isHead);

        // ACCESSORS
        unsigned nextRecord() const;
        unsigned prevRecord() const;
        unsigned attributes() const;
        unsigned nextBlock() const;
        bool isConfirmed() const;
        bool isExtended() const;
        bool isHead() const;
    };

  private:
    // DATA
    baecs_JournalPageHeader_TransactionId d_transactionId;
    bdeut_BigEndianUint32                 d_numBlocks;
    bdeut_BigEndianUint32                 d_numAvailableBlocks;
    bdeut_BigEndianUint32                 d_numReleasedBlocks;
    bdeut_BigEndianUint32                 d_firstAvailableBlock;
    bdeut_BigEndianUint32                 d_firstReleasedBlock;
    bdeut_BigEndianUint32                 d_lastReleasedBlock;
    bdeut_BigEndianUint32                 d_isOnFillList;
    bdeut_BigEndianUint32                 d_nextPage;
    BlockHeader                           d_blocks[4];

  public:
    // CLASS METHODS
    static unsigned getSize(unsigned numBlocks);
        // Return the number of bytes required to store a
        // 'baecs_JournalPageHeader' with the specified 'numBlocks' in memory.

    // MANIPULATORS
    void copyFrom(const baecs_JournalPageHeader& other,
                  const bsls_Types::Int64&       transactionId);

    void init(unsigned                 numBlocks,
              const bsls_Types::Int64& transactionId);
        // Initialize this 'baecs_JournalPageHeader' structure having the
        // specified 'numBlocks', placing all blocks on the available list,
        // and setting the current transaction id to the specified
        // 'transactionId'.

    void setIsOnFillList(bool isOnFillList)
    {
        d_isOnFillList = (unsigned)isOnFillList;
    }

    unsigned allocate(unsigned numBlocks, unsigned attributes);
        // Allocate a list of the specified 'numBlocks' from the available
        // list of this 'baecs_JournalPageHeader', assigning the specified
        // 'attributes' to the head of the list, and setting flags to zero.
        // Returns the index of the head.  The behavior is undefined unless
        // 'numBlocks' is less or equal than the the number of available
        // blocks.

    int release(unsigned first);
        // Free the list of blocks (previously allocated with 'allocate'),
        // placing the blocks on the released list.  The behavior is undefined
        // unless 'first' is a valid block index previously returned from
        // 'allocate'.  Note that the blocks on the released list do not
        // become available for allocation until a call to 'purge'.

    int deallocate(unsigned first);
        // Free the list of blocks previously allocated with 'allocate',
        // placing the blocks directly on an available list.  The behavior is
        // undefined unless 'first' is a valid block index previously returned
        // from 'allocate'.

    int purge();
        // Moves the contents of the released list to the available list,
        // making the blocks available for allocation.

    int markFreeListsForValidation(unsigned *map,
                                   bool      verbose,
                                   unsigned  offset) const;
    int markBlockListForValidation(unsigned  index,
                                   unsigned  numBlocks,
                                   unsigned  blockType,
                                   unsigned *map,
                                   bool      verbose,
                                   unsigned  offset) const;

    void setNextPage(unsigned nextPage)
    {
        d_nextPage = nextPage;
    }

    void setTransactionId(const bsls_Types::Int64& id);

    // TBD
    BlockHeader *block(int n)
    {
        return d_blocks + n;
    }

    const BlockHeader *block(int n) const
    {
        return d_blocks + n;
    }

    bool isOnFillList() const;

    unsigned nextPage() const;
    unsigned numBlocks() const;
    unsigned numFreeBlocks() const;
    unsigned numAvailableBlocks() const;
    int getTransactionId(bsls_Types::Int64 *outValue) const;

    void print(bsl::ostream& stream) const;
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&                               stream,
                         const baecs_JournalPageHeader::BlockHeader& rhs);
    // TBD doc

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                   // -------------------------------------------
                   // class baecs_JournalPageHeader_TransactionId
                   // -------------------------------------------

// FREE OPERATORS
inline
bsl::ostream& operator<<(bsl::ostream&                                stream,
                         const baecs_JournalPageHeader_TransactionId& rhs)
{
   return stream << "["
                 << rhs.d_data[0] << ","
                 << rhs.d_data[1] << ","
                 << rhs.d_data[2] << ","
                 << rhs.d_data[3] << "]";
}

                         // -----------------------------
                         // class baecs_JournalPageHeader
                         // -----------------------------

// ACCESSORS
inline
bool baecs_JournalPageHeader::isOnFillList() const
{
    return d_isOnFillList;
}

inline
unsigned baecs_JournalPageHeader::nextPage() const
{
    return d_nextPage;
}

inline
unsigned baecs_JournalPageHeader::numBlocks() const
{
    return d_numBlocks;
}

inline
unsigned baecs_JournalPageHeader::numFreeBlocks() const
{
    return d_numAvailableBlocks + d_numReleasedBlocks;
}

inline
unsigned baecs_JournalPageHeader::numAvailableBlocks() const
{
    return d_numAvailableBlocks;
}

inline
int baecs_JournalPageHeader::getTransactionId(
                                             bsls_Types::Int64 *outValue) const
{
    return d_transactionId.getValue(outValue);
}

inline
void  baecs_JournalPageHeader::setTransactionId(const bsls_Types::Int64& id)
{
    d_transactionId.setValue(id);
}

                     // ------------------------------------------
                     // class baecs_JournalPageHeader::BlockHeader
                     // ------------------------------------------

// MANIPULATORS
inline
void baecs_JournalPageHeader::BlockHeader::init()
{
    d_nextRecord = 0xBEEFDEAD;
    d_prevRecord = 0xBEEDDEAD;
    d_extraData  = 0;
}

inline
void baecs_JournalPageHeader::BlockHeader::setNextRecord(unsigned nextRecord)
{
    d_nextRecord = nextRecord;
}

inline
void baecs_JournalPageHeader::BlockHeader::setPrevRecord(unsigned prevRecord)
{
    d_prevRecord = prevRecord;
}

inline
void baecs_JournalPageHeader::BlockHeader::setAttributes(unsigned attributes)
{
    d_extraData = (d_extraData & ~BAECS_ATTRIBUTES_MASK)
            | ((attributes << BAECS_ATTRIBUTES_SHIFT) & BAECS_ATTRIBUTES_MASK);
}

inline
void baecs_JournalPageHeader::BlockHeader::setNextBlock(unsigned nextBlock)
{
    d_extraData = (d_extraData & ~BAECS_NEXT_BLOCK_MASK)
             | ((nextBlock << BAECS_NEXT_BLOCK_SHIFT) & BAECS_NEXT_BLOCK_MASK);
}

inline
void baecs_JournalPageHeader::BlockHeader::setConfirmed(bool isConfirmed)
{
    d_extraData = (d_extraData & ~BAECS_CONFIRMED_MASK)
   | (((unsigned)isConfirmed << BAECS_CONFIRMED_SHIFT) & BAECS_CONFIRMED_MASK);
}

inline
void baecs_JournalPageHeader::BlockHeader::setExtended(bool isExtended)
{
    d_extraData = (d_extraData & ~BAECS_EXTENDED_MASK)
      | (((unsigned)isExtended << BAECS_EXTENDED_SHIFT) & BAECS_EXTENDED_MASK);
}

inline
void baecs_JournalPageHeader::BlockHeader::setHead(bool isHead)
{
    d_extraData = (d_extraData & ~BAECS_HEAD_MASK)
                  | (((unsigned)isHead << BAECS_HEAD_SHIFT) & BAECS_HEAD_MASK);
}

// ACCESSORS
inline
unsigned baecs_JournalPageHeader::BlockHeader::nextRecord() const
{
    return d_nextRecord;
}

inline
unsigned baecs_JournalPageHeader::BlockHeader::prevRecord() const
{
    return d_prevRecord;
}

inline
unsigned baecs_JournalPageHeader::BlockHeader::attributes() const
{
    return (d_extraData & BAECS_ATTRIBUTES_MASK) >> BAECS_ATTRIBUTES_SHIFT;
}

inline
unsigned baecs_JournalPageHeader::BlockHeader::nextBlock() const
{
    return (d_extraData & BAECS_NEXT_BLOCK_MASK) >> BAECS_NEXT_BLOCK_SHIFT;
}

inline
bool baecs_JournalPageHeader::BlockHeader::isConfirmed() const
{
    return 0 != (d_extraData & BAECS_CONFIRMED_MASK);
}

inline
bool baecs_JournalPageHeader::BlockHeader::isExtended() const
{
    return 0 != (d_extraData & BAECS_EXTENDED_MASK);
}

inline
bool baecs_JournalPageHeader::BlockHeader::isHead() const
{
    return 0 != (d_extraData & BAECS_HEAD_MASK);
}

// FREE OPERATORS
inline
bsl::ostream& operator<<(bsl::ostream&                               stream,
                         const baecs_JournalPageHeader::BlockHeader& rhs)
{
    return stream << "{"
                  << rhs.nextRecord() << ","
                  << rhs.prevRecord() << ","
                  << rhs.nextBlock()  << ","
                  << "A=" << rhs.attributes() << ","
                  << (rhs.isHead() ?      "H":"")
                  << (rhs.isExtended() ?  "E":"")
                  << (rhs.isConfirmed() ? "C":"")
                  << "}";
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
