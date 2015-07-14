// balj_journalparameters.h                                          -*-C++-*-
#ifndef INCLUDED_BALJ_JOURNALPARAMETERS
#define INCLUDED_BALJ_JOURNALPARAMETERS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a container for the file parameters of a journal.
//
//@CLASSES:
//  balj::JournalParameters: container of journal file parameters
//
//@SEE_ALSO: balj_journalheader, balj_journalpageheader, balj_journal
//
//@AUTHOR: Oleg Semenov (osemenov), Andrei Basov (abasov)
//
//@DESCRIPTION: This component defines a class, 'balj::JournalParameters', that
// describes the file structure used by a 'balj_Journal' object.
//
///Usage
///-----
// TBD

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

namespace balj {
                        // =============================
                        // class JournalParameters
                        // =============================

class JournalParameters {
    // This *value* *semantic* class represents the file structure used by a
    // 'balj_Journal' object.

    // DATA
    int d_blockSize;
    int d_blocksPerPage;
    int d_pagesPerSet;
    int d_freeBlockThreshold;  // interpreted as a percentage

  public:
    // CONSTANTS
    static const int DEFAULT_BLOCK_SIZE;
        // default value of "BlockSize" attribute

    static const int DEFAULT_BLOCKS_PER_PAGE;
        // default value of "BlocksPerPage" attribute

    static const int DEFAULT_PAGES_PER_SET;
        // default value of "PagesPerSet" attribute

    static const int DEFAULT_FREE_BLOCK_THRESHOLD;
        // default value of "FreeBlockThreshold" attribute

  public:
    // CREATORS
    JournalParameters();
        // Create an object of type 'JournalParameters' having the
        // default value.

    JournalParameters(const JournalParameters& original);
        // Create an object of type 'JournalParameters' having the value
        // of the specified 'original' object.

    // ~JournalParameters();
        // Destroy this object.  Note that this trivial destructor is generated
        // by the compiler.

    // MANIPULATORS
    JournalParameters& operator=(const JournalParameters& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference to this modifiable object.

    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    int& blockSize();
        // Return a reference to the modifiable "BlockSize" attribute of this
        // object.

    int& blocksPerPage();
        // Return a reference to the modifiable "BlocksPerPage" attribute of
        // this object.

    int& pagesPerSet();
        // Return a reference to the modifiable "PagesPerSet" attribute of this
        // object.

    int& freeBlockThreshold();
        // Return a reference to the modifiable "FreeBlockThreshold" attribute
        // of this object.

    // ACCESSORS
    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the
        // optionally specified indentation 'level' and return a reference to
        // the modifiable 'stream'.  If 'level' is specified, optionally
        // specify 'spacesPerLevel', the number of spaces per indentation level
        // for this and all of its nested objects.  Each line is indented by
        // the absolute value of 'level * spacesPerLevel'.  If 'level' is
        // negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, suppress line breaks and format the
        // entire output on one line.  If 'stream' is initially invalid, this
        // operation has no effect.  Note that a trailing newline is provided
        // in multiline mode only.

    const int& blockSize() const;
        // Return a reference to the non-modifiable "BlockSize" attribute of
        // this object.

    const int& blocksPerPage() const;
        // Return a reference to the non-modifiable "BlocksPerPage" attribute
        // of this object.

    const int& pagesPerSet() const;
        // Return a reference to the non-modifiable "PagesPerSet" attribute of
        // this object.

    const int& freeBlockThreshold() const;
        // Return a reference to the non-modifiable "FreeBlockThreshold"
        // attribute of this object.
};

// FREE OPERATORS
bool operator==(const JournalParameters& lhs,
                const JournalParameters& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

bool operator!=(const JournalParameters& lhs,
                const JournalParameters& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

bsl::ostream& operator<<(bsl::ostream&                  stream,
                         const JournalParameters& rhs);
    // Format the specified 'rhs' to the specified output 'stream', and
    // return a reference to the modifiable 'stream'.

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        // -----------------------------
                        // class JournalParameters
                        // -----------------------------
// CREATORS
inline
JournalParameters::JournalParameters()
: d_blockSize(DEFAULT_BLOCK_SIZE)
, d_blocksPerPage(DEFAULT_BLOCKS_PER_PAGE)
, d_pagesPerSet(DEFAULT_PAGES_PER_SET)
, d_freeBlockThreshold(DEFAULT_FREE_BLOCK_THRESHOLD)
{
}

inline
JournalParameters::JournalParameters(
                                       const JournalParameters& original)
: d_blockSize(original.d_blockSize)
, d_blocksPerPage(original.d_blocksPerPage)
, d_pagesPerSet(original.d_pagesPerSet)
, d_freeBlockThreshold(original.d_freeBlockThreshold)
{
}

// MANIPULATORS
inline
JournalParameters&
JournalParameters::operator=(const JournalParameters& rhs)
{
    if (this != &rhs) {
        d_blockSize          = rhs.d_blockSize;
        d_blocksPerPage      = rhs.d_blocksPerPage;
        d_pagesPerSet        = rhs.d_pagesPerSet;
        d_freeBlockThreshold = rhs.d_freeBlockThreshold;
    }
    return *this;
}

inline
void JournalParameters::reset()
{
    d_blockSize          = DEFAULT_BLOCK_SIZE;
    d_blocksPerPage      = DEFAULT_BLOCKS_PER_PAGE;
    d_pagesPerSet        = DEFAULT_PAGES_PER_SET;
    d_freeBlockThreshold = DEFAULT_FREE_BLOCK_THRESHOLD;
}

inline
int& JournalParameters::blockSize()
{
    return d_blockSize;
}

inline
int& JournalParameters::blocksPerPage()
{
    return d_blocksPerPage;
}

inline
int& JournalParameters::pagesPerSet()
{
    return d_pagesPerSet;
}

inline
int& JournalParameters::freeBlockThreshold()
{
    return d_freeBlockThreshold;
}

// ACCESSORS
inline
const int& JournalParameters::blockSize() const
{
    return d_blockSize;
}

inline
const int& JournalParameters::blocksPerPage() const
{
    return d_blocksPerPage;
}

inline
const int& JournalParameters::pagesPerSet() const
{
    return d_pagesPerSet;
}

inline
const int& JournalParameters::freeBlockThreshold() const
{
    return d_freeBlockThreshold;
}
}  // close package namespace

// FREE OPERATORS
inline
bool balj::operator==(const JournalParameters& lhs,
                const JournalParameters& rhs)
{
    return lhs.blockSize()          == rhs.blockSize()
        && lhs.blocksPerPage()      == rhs.blocksPerPage()
        && lhs.pagesPerSet()        == rhs.pagesPerSet()
        && lhs.freeBlockThreshold() == rhs.freeBlockThreshold();
}

inline
bool balj::operator!=(const JournalParameters& lhs,
                const JournalParameters& rhs)
{
    return lhs.blockSize()          != rhs.blockSize()
        || lhs.blocksPerPage()      != rhs.blocksPerPage()
        || lhs.pagesPerSet()        != rhs.pagesPerSet()
        || lhs.freeBlockThreshold() != rhs.freeBlockThreshold();
}

inline
bsl::ostream& balj::operator<<(bsl::ostream&                  stream,
                         const JournalParameters& rhs)
{
    return rhs.print(stream, 0, -1);
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
