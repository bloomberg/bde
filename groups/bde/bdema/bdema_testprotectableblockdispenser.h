// bdema_testprotectableblockdispenser.h   -*-C++-*-
#ifndef INCLUDED_BDEMA_TESTPROTECTABLEBLOCKDISPENSER
#define INCLUDED_BDEMA_TESTPROTECTABLEBLOCKDISPENSER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a concrete protectable block dispenser for testing.
//
//@CLASSES:
// bdema_TestProtectableBlockDispenser: concrete test dispenser implementation
//
//@AUTHOR: Henry Verschell (hverschell)
//
//@DESCRIPTION: This component implements the
// 'bdema_ProtectableBlockDispenser' protocol with a test implementation
// that tracks memory allocation and memory protection state.  The test
// implementation simulates hardware memory protection by storing a checksum of
// the block of memory when it is protected and validates the stored
// checksum when the block is unprotected again.  If exceptions are enabled,
// this dispenser can be configured to throw an exception after the number of
// allocation requests exceeds some specified limit.  This dispenser also
// maintains a count of: (1) the number of outstanding blocks (and bytes) that
// are currently in use, (2) the cumulative number of blocks (and bytes) that
// have been allocated, (3) the maximum number of blocks (and bytes) that have
// been in use at any one time, (4) and the number of blocks in use that are
// currently in a protected state.  A 'print' method is provided to format
// these values to 'stdout':
//..
//            ( bdema_TestProtectableBlockDispenser )
//                               |          ctor
//                               |          numBlocksInUse/numBytesInUse
//                               |          numBlocksMax/numBytesMax
//                               |          numBlocksTotal/numBytesTotal
//                               |          numBlocksProtected
//                               |          numMismatches/status
//                               |          setQuiet/isQuiet
//                               |          setVerbose/isVerbose
//                               |          setAllocationLimit/allocationLimit
//                               V
//              ( bdema_ProtectableBlockDispenser )
//                                          dtor
//                                          allocate
//                                          deallocate
//                                          protect
//                                          unprotect
//..
// Note that allocation using this component is deliberately incompatible with
// the default global 'new' ('malloc') and 'delete' ('free').  Using 'delete'
// to free memory supplied by this dispenser will corrupt the dynamic memory
// manager and also cause a memory leak (and will be reported by 'purify' as
// freeing mismatched memory).  Using 'deallocate' to free memory supplied by
// global 'new' will almost always abort immediately (unless quiet mode is set
// for the purpose of testing this component itself).
//
///MODES
///-----
// The test dispenser's behavior is controlled by three basic *mode* flags:
//
// VERBOSE MODE: (Default 0) Specifies that each allocation and deallocation
// should be printed to standard output.  In verbose mode, all state variables
// will be displayed at destruction.
//
// QUIET MODE: (Default 0) Specifies that mismatched memory and memory leaks
// should *not* be reported, and should not cause the process to terminate when
// detected.  Note that this mode is used primarily for testing the test
// dispenser itself; behavior that would otherwise abort instead quietly
// increments the 'numMismatches' counter.
//
// NO-ABORT MODE: (Default 0) Specifies that all abort statements are replaced
// by return statements without suppressing diagnostics.  Although the internal
// state values are independent, Quiet Mode implies the behavior of No-Abort
// mode in all cases.  Note that this mode is used primarily for visual
// inspection of unusual error diagnostics in this component's test driver (in
// verbose mode only).
//
// Taking the default mode settings, memory allocation/deallocation will not be
// displayed individually, but in the event of a memory error (a mismatched
// deallocation, memory leak, or a modification of protected memory) the
// problem will be announced, any relevant state of the object will be
// displayed, and the program will abort.
//
///ALLOCATION LIMIT
///----------------
// If exceptions are enabled at compile time, the test dispenser can be
// configured to throw an exception after a specified number of allocation
// requests is exceeded.  If the allocation limit is less than zero (default),
// then the dispenser never throws an exception.  Note that a non-negative
// allocation limit is decremented after each allocation attempt, and throws
// only when the current allocation limit transitions from 0 to -1; no
// additional exceptions will be thrown until the allocation limit is again
// reset to a non-negative value.
//
///Usage
///-----
// The 'bdema_TestProtectableBlockDispenser' class defined in this component
// can be used to test objects that take a 'bdema_ProtectableBlockDispenser'
// (e.g., at construction).  The following code example illustrates how to
// verify that an object under test (e.g., 'IntegerStack') is exception
// neutral.  Note that the source for the example 'IntegerStack' can be found
// in the usage example from 'bdema_protectableblockdispenser':
//..
//  // IntegerStack.t.cpp
//
//  #include <IntegerStack.h>  // from bdema_protectableblockdispenser.h
//  #include <bdema_testprotectableblockdispenser.h>
//  #include <bdema_testallocatorexception.h>
//..
// Specify a set of macros that will be used to test the exception neutrality
// of the components under test.  Note that "\$" must be replaced by "\" in
// the preprocessor macro definitions that follow.  The "$" symbols are
// present in this header file to avoid a diagnostic elicited by some
// compilers (e.g., "warning: multi-line comment").
//..
// #ifdef BDE_BUILD_TARGET_EXC
// #define BEGIN_BDEMA_EXCEPTION_TEST {                                      \$
//     {                                                                     \$
//         static int firstTime = 1;                                         \$
//         if (veryVerbose && firstTime) cout <<                             \$
//             "### BDEMA EXCEPTION TEST -- (ENABLED) --" << endl;           \$
//         firstTime = 0;                                                    \$
//     }                                                                     \$
//     if (veryVeryVerbose) cout <<                                          \$
//         "### Begin bdema exception test." << endl;                        \$
//     int bdemaExceptionCounter = 0;                                        \$
//     static int bdemaExceptionLimit = 100;                                 \$
//     testDispenser.setAllocationLimit(bdemaExceptionCounter);              \$
//     do {                                                                  \$
//         try {
//
// #define END_BDEMA_EXCEPTION_TEST                                          \$
//         } catch (bslma_TestAllocatorException& e) {                       \$
//             if (veryVerbose && bdemaExceptionLimit || veryVeryVerbose) {  \$
//                 --bdemaExceptionLimit;                                    \$
//                 cout << "(*** " << bdemaExceptionCounter << ')';          \$
//                 if (veryVeryVerbose) { cout << " BDEMA_EXCEPTION: "       \$
//                     << "alloc limit = " << bdemaExceptionCounter << ", "  \$
//                     << "last alloc size = " << e.numBytes();              \$
//                 }                                                         \$
//                 else if (0 == bdemaExceptionLimit) {                      \$
//                     cout << " [ Note: 'bdemaExceptionLimit' reached. ]";  \$
//                 }                                                         \$
//                 cout << endl;                                             \$
//             }                                                             \$
//             testDispenser.setAllocationLimit(++bdemaExceptionCounter);    \$
//             continue;                                                     \$
//         }                                                                 \$
//         testDispenser.setAllocationLimit(-1);                             \$
//         break;                                                            \$
//     } while (1);                                                          \$
//     if (veryVeryVerbose) cout <<                                          \$
//         "### End bdema exception test." << endl;                          \$
// }
// #else
// #define BEGIN_BDEMA_EXCEPTION_TEST                                        \$
// {                                                                         \$
//     static int firstTime = 1;                                             \$
//     if (verbose && firstTime) { cout <<                                   \$
//         "### BDEMA EXCEPTION TEST -- (NOT ENABLED) --" << endl;           \$
//         firstTime = 0;                                                    \$
//     }                                                                     \$
// }
// #define END_BDEMA_EXCEPTION_TEST
// #endif
//..
// Use the macros to define a test for exceptions:
//..
//  int main(int argc, char *argv[])
//  {
//      int test = argc > 1 ? atoi(argv[1]) : 0;
//      int verbose = argc > 2;
//      int veryVerbose = argc > 3;
//      int veryVeryVerbose = argc > 4;
//
//      bdema_TestProtectableBlockDispenser testDispenser(64, veryVeryVerbose);
//
//      switch (test) { case 0:
//
//        // ...
//        case 7: {
//          typedef short Element;
//          const Element VALUES[] = { 1, 2, 3, 4, -5 };
//          const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
//
//          const Element &V0 = VALUES[0],
//                        &V1 = VALUES[1],
//                        &V2 = VALUES[2],
//                        &V3 = VALUES[3],
//                        &V4 = VALUES[4];
//
//          struct {
//              int   d_line;
//              int   d_numElem;
//              short d_exp[NUM_VALUES];
//          } DATA[] = {
//              { L_, 0, { 0 } },
//              { L_, 1, { V0 } },
//              { L_, 5, { V0, V1, V2, V3, V4 } }
//          };
//
//          const int NUM_TEST = sizeof DATA / sizeof *DATA;
//
//          for (int ti = 0; ti < NUM_TEST; ++ti) {
//              const int    LINE     = DATA[ti].d_line;
//              const int    NUM_ELEM = DATA[ti].d_numElem;
//              const short *EXP      = DATA[ti].d_exp;
//
//              BEGIN_BDEMA_EXCEPTION_TEST {
//                  IntegerStack mA(1, &testDispenser);
//                  const IntegerStack& A = mA;
//                  for (int testI = 0; testI < NUM_ELEM; ++testI) {
//                      for (int ei = 0; ei <= testI; ++ei) {
//                          mA.push(VALUES[ei]);
//                      }
//                      for (int ei = testI; ei >= 0; --ei) {
//                          LOOP2_ASSERT(testI, ei, VALUES[ei] == mA.pop());
//                      }
//                  }
//              } END_BDEMA_EXCEPTION_TEST
//          }
//
//          if (veryVerbose) {
//              testDispenser.print();
//          }
//        } break;
//
//        // ...
//
//      }
//
//      // ...
//  }
//..
// Note that the 'BDE_BUILD_TARGET_EXC' macro is defined at compile-time to
// indicate that exceptions are enabled.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEMA_MEMORYBLOCKDESCRIPTOR
#include <bdema_memoryblockdescriptor.h>
#endif

#ifndef INCLUDED_BDEMA_PROTECTABLEBLOCKDISPENSER
#include <bdema_protectableblockdispenser.h>
#endif

namespace BloombergLP {

                  // =========================================
                  // class bdema_TestProtectableBlockDispenser
                  // =========================================

class bdema_TestProtectableBlockDispenser :
                                       public bdema_ProtectableBlockDispenser {
    // This 'class' defines a dispenser that DOES NOT USE global operators
    // 'new' or 'delete' at all, but instead uses 'malloc' and 'free'
    // directly.  This dispenser simulates hardware memory protection by
    // storing a checksum of the block of memory when it is protected and
    // validates the stored checksum when the block is unprotected again.
    // This dispenser class also tracks the number of blocks (and bytes) that
    // are currently in use from this dispenser, the cumulative number of
    // blocks (and bytes) that have been allocated by this dispenser, the
    // maximum number of blocks (and bytes) that have ever been in use from
    // this dispenser at any one time, and the current number of blocks
    // allocated from this dispenser that are in a protected state.  Each
    // individual block returned by this test dispenser has a header that is
    // intended to be accessed only by this test dispenser.  The block header
    // maintains the current state of the memory block.  Note that the number
    // of bytes in use is defined to be the number requested (e.g., if 5 bytes
    // are requested and, for whatever reason, 8 are returned, only 5 bytes are
    // considered to be in use).

    // DATA
    const int d_pageSize;          // system page size to simulate
    int d_numBlocksInUse;          // number of blocks currently allocated
    int d_numBytesInUse;           // number of bytes currently allocated

    int d_numBlocksMax;            // max # of blocks ever used at any one time
    int d_numBytesMax;             // max # of bytes ever used at any one time

    int d_numBlocksTotal;          // cumulative blocks ever requested
    int d_numBytesTotal;           // cumulative bytes ever requested

    int d_numErrors;               // number of invalid memory operations
    int d_allocationLimit;         // # of alloc's before throwing an exception

    int d_lastAllocateNumBytes;    // size (bytes) of last allocation request
    int d_lastDeallocateNumBytes;  // size (bytes) of last deallocated memory

    void *d_lastAllocateAddress;   // memory address of last allocated memory
    void *d_lastDeallocateAddress; // memory address of last deallocated memory

    int d_numAllocation;           // cumulative # of allocation requests
    int d_numDeallocation;         // cumulative # of deallocation requests

    bool d_noAbortFlag;            // whether to suppress the abort on error
    bool d_quietFlag;              // whether to suppress reporting hard errors
    bool d_verboseFlag;            // whether to write operations to 'stdout'
    int  d_numBlocksProtected;     // number of currently protected blocks

    // NOT IMPLEMENTED
    bdema_TestProtectableBlockDispenser(
                                   const bdema_TestProtectableBlockDispenser&);
    bdema_TestProtectableBlockDispenser& operator=(
                                   const bdema_TestProtectableBlockDispenser&);
  public:
    // PUBLIC TYPES
    enum {
        BDEMA_DEFAULT_PAGE_SIZE = 128  // default simulated page size
    };

    // CREATORS
    explicit bdema_TestProtectableBlockDispenser(int  pageSize,
                                                 bool verboseFlag = false);
        // Create a test dispenser that simulates a system configured with the
        // specified 'pageSize'.  Blocks allocated by this test dispenser will
        // have sizes that are multiples of 'pageSize'.  The optionally
        // specified 'verboseFlag', if 'true', indicates this test dispenser
        // should write allocation/deallocation messages to 'stdout'.  By
        // default, no messages are displayed.

    ~bdema_TestProtectableBlockDispenser();
        // Destroy this test dispenser.  Note that the behavior of destroying a
        // test dispenser while memory is allocated from it is not specified.
        // (Unless you *know* that it is valid to do so, don't!)
        //
        // For this concrete implementation, destroying this test dispenser has
        // no effect on allocated memory (i.e., memory leaks will be detected
        // by 'purify').  In verbose mode, print all contained state values of
        // this test dispenser to 'stdout'.  Except in quiet mode, report any
        // memory leaks or mismatched deallocations to 'stdout'.  Abort if not
        // in quiet mode unless both 'numBytesInUse' and 'numBlocksInUse'
        // return 0.

    // MANIPULATORS
    virtual bdema_MemoryBlockDescriptor allocate(size_type size);
        // Allocate a block of at least the specified 'numBytes' and return a
        // descriptor of the block (containing the address and actual size of
        // the allocated memory).  If 'numBytes' is 0, a null descriptor is
        // returned with no other effect.  If this test dispenser cannot return
        // the requested number of bytes, it will throw a 'bsl::bad_alloc'
        // exception in an exception-enabled build, or else abort the program
        // in a non-exception build.  Note that the alignment of the address
        // returned conforms to the platform requirement for any object of the
        // specified 'numBytes'.
        //
        // For this concrete implementation, if 0 is supplied, 'malloc' is not
        // called and this operation has no effect other than to return a null
        // block descriptor.  If 'size >= 1' allocate and return a block whose
        // 'size' is the nearest multiple of 'pageSize'.  Increase the number
        // of currently allocated blocks by 1 and the number of currently
        // allocated bytes by the actual number of bytes allocated.  Update all
        // other fields accordingly.

    virtual void deallocate(const bdema_MemoryBlockDescriptor& block);
        // Return the memory described by the specified 'block' to this test
        // dispenser.  If 'block' is a null descriptor, then this function has
        // no effect.  The behavior is undefined unless 'block' was returned by
        // this test dispenser, has not since been deallocated, and is
        // currently in an unprotected state.
        //
        // For this concrete implementation and if 'block' is determined to be
        // valid and non-null, decrease the number of currently allocated
        // blocks by 1 and the number of currently allocated bytes by the
        // number of bytes allocated for this block.  If the value of the
        // 'block' does not match a block that was created by this test
        // dispenser, or if the 'block' is currently in a protected state,
        // increment the number of mismatches, and, unless in quiet mode,
        // immediately report the details of the mismatch to 'stdout' (e.g., as
        // an 'bsl::hex' memory dump) and abort.

    virtual int protect(const bdema_MemoryBlockDescriptor& block);
        // Set the access protection for the specified 'block' to be READ-ONLY.
        // Return 0 on success and a non-zero value otherwise.  This method has
        // no effect if 'block' is a null descriptor or was already protected;
        // concrete implementations of the 'bdema_ProtectableBlockDispenser'
        // protocol may differ in the status value that is returned in those
        // two cases.  The behavior is undefined unless 'block' was returned
        // from a call to 'allocate' on this test dispenser and has not since
        // been deallocated.
        //
        // For this concrete implementation, hardware memory protection is
        // simulated by storing a checksum of the 'block' when the memory is
        // protected and validating the checksum when the block is unprotected
        // If the 'block' is determined to be valid, internally mark this
        // 'block' as having been protected and record a checksum for the
        // memory.  If the value of the 'block' does not match a block that
        // was created by this test dispenser, increment the number of
        // mismatches, and, unless in quiet mode, immediately report the
        // details of the mismatch to 'stdout' (e.g., as an 'bsl::hex' memory
        // dump) and abort.

    virtual int unprotect(const bdema_MemoryBlockDescriptor& block);
        // Set the access protection for the specified 'block' to be
        // READ-WRITE.  Return 0 on success and a non-zero value otherwise.
        // This method has no effect if 'block' is a null descriptor or was
        // already unprotected; concrete implementations of the
        // 'bdema_ProtectableBlockDispenser' protocol may differ in the status
        // value that is returned in those two cases.  The behavior is
        // undefined unless 'block' was returned from a call to 'allocate' on
        // this test dispenser and has not since been deallocated.
        //
        // For this concrete implementation, hardware memory protection is
        // simulated by storing a checksum of the 'block' when the memory is
        // protected and validating the checksum when the block is unprotected
        // If the 'block' is determined to be valid, internally mark this
        // 'block' as having been unprotected and validate the stored checksum
        // for the 'block'.  If the value of the 'block' does not match a block
        // that was created by this test dispenser, or if the stored checksum
        // doesn't match the current block, increment the number of mismatches,
        // and, unless in quiet mode, immediately report the details of the
        // mismatch to 'stdout' (e.g., as an 'bsl::hex' memory dump) and abort.

    void setNoAbort(bool flagValue);
        // Set the internal no-abort mode to the specified 'flagValue'.  A
        // 'true' value suppresses aborting on fatal errors and simply returns.
        // Diagnostics are left unaffected.  Note that this function is
        // primarily to enable visual testing of bizarre error messages
        // generated by this component.

    void setQuiet(bool flagValue);
        // Set the internal quiet mode to the specified 'flagValue'.  The
        // default mode is *not* quiet.  If 'quietFlag' is 'true', mismatched
        // allocation and memory leak messages will not be displayed to
        // 'stdout' and the process will not abort as a result of either of
        // these conditions.  Note that this function is primarily to enable
        // testing of this component; situations that would otherwise abort,
        // now quietly increment the 'numMismatches' counter.

    void setVerbose(bool flagValue);
        // Set the internal verbose mode to the specified 'flagValue'.  The
        // default mode is *not* verbose.  If 'verboseFlag' is 'false',
        // allocation/deallocation messages will be displayed to 'stdout'.

    void setAllocationLimit(int limit);
        // Set the number of valid allocation requests before an exception is
        // to be thrown to the specified 'limit'.  If 'limit' is less than 0,
        // no exception will be thrown.  By default, no exception is scheduled.

    // ACCESSORS
    bool isNoAbort() const;
        // Return 'true' if this test dispenser is currently in no-abort mode,
        // and 'false' otherwise.  In no-abort mode all diagnostic messages are
        // printed, but all aborts are replaced by return statements.  Note
        // that quiet mode implies no-abort mode.

    bool isQuiet() const;
        // Return 'true' if this test dispenser is currently in quiet mode, and
        // 'false' otherwise.  In quiet mode, mismatched deallocation messages
        // and memory leaks will not be displayed to 'stdout' and will not
        // cause the program to abort.

    bool isVerbose() const;
        // Return 'true' if this test dispenser is currently in verbose mode,
        // and 'false'  otherwise.  In verbose mode, allocation/deallocation
        // messages will be displayed to 'stdout'.

    int numBytesInUse() const;
        // Return the number of bytes currently in use from this test
        // dispenser, where "in use" means the number of bytes explicitly
        // requested.

    int numBlocksInUse() const;
        // Return the number of blocks currently in use from this test
        // dispenser.

    int numBytesMax() const;
        // Return the largest number of bytes ever in use from this test
        // dispenser at any one time (i.e., the "high water mark").  Note that
        // 'numBytesMax() <= numBytesTotal()'.

    int numBlocksMax() const;
        // Return the largest number of blocks ever in use from this test
        // dispenser at any one time (i.e., the "high water mark").  Note that
        // 'numBlocksMax() <= numBlocksTotal()'.

    int numBytesTotal() const;
        // Return the cumulative number of bytes ever allocated from this test
        // dispenser.  Note that 'numBytesTotal() >= numBytesMax()'.

    int numBlocksTotal() const;
        // Return the cumulative number of blocks ever allocated from this test
        // dispenser.  Note that 'numBlocksTotal() >= numBlocksMax()'.

    int numErrors() const;
        // Return the number of memory errors (i.e., a mismatched deallocation,
        // memory leak, or a modification of protected memory) that have
        // occurred since this test dispenser was created.

    int status() const;
        // Return 0 on success, and a non-zero otherwise.  If there have been
        // any errors detected, return the positive number of detected errors;
        // otherwise, if the number of blocks and bytes are not both 0, return
        // a negative number.  Note that this function is used to define the
        // criteria for an abort at destruction if quiet mode has not been
        // set.

    int allocationLimit() const;
        // Return the current number of allocation requests left before an
        // exception is thrown.  A negative value indicates that no exception
        // is scheduled.

    int lastAllocateNumBytes() const;
        // Return the number of bytes of the most recent memory request.  The
        // behavior is undefined if no such request has occurred.  Note that
        // this number is always recorded regardless of the validity of the
        // request.

    int lastDeallocateNumBytes() const;
        // Return the number of bytes of the most recent memory deallocation
        // request.  Return 0 if the deallocation was performed on a null
        // block descriptor or the deallocation request was invalid (e.g.,
        // an attempt to deallocate memory not allocated through this test
        // dispenser).  The behavior is undefined if no such request has
        // occurred.

    void *lastAllocateAddress() const;
        // Return the allocated memory address of the most recent memory
        // request.  Return 0 if the request was for 0 bytes or if the request
        // was invalid (e.g., allocate non-positive number of bytes).  The
        // behavior is undefined if no such request has occurred.

    void *lastDeallocateAddress() const;
        // Return the memory address of the last memory deallocation request.
        // Note that the address is always recorded regardless of the validity
        // of the request.  The behavior is undefined if no such request has
        // occurred.

    int numAllocations() const;
        // Return the cumulative number of allocation requests.  Note that this
        // number is incremented for every 'allocate' invocation, regardless of
        // the validity of the request.

    int numDeallocations() const;
        // Return the cumulative number of deallocation requests.  Note that
        // this number is incremented for every 'deallocate' invocation,
        // regardless of the validity of the request.

    void print() const;
        // Write the accumulated state information held in this test dispenser
        // to 'bsl::stdout' in some reasonable (multi-line) format.

    int pageSize() const;
        // Return the page size (supplied at construction) used by this test
        // dispenser.

    int minimumBlockSize() const;
        // Return the minimum size of a block returned by this test dispenser.
        // Note that this value indicates the size of the block that would be
        // returned if 'allocate(1)' were called.
        //
        // For this implementation the minimum block size is the page size
        // specified at construction.

    bool isProtected(const bdema_MemoryBlockDescriptor& block) const;
        // If the specified 'block' matches a block returned by this test
        // dispenser, then return 'true' if the 'block' is in a protected state
        // and 'false' otherwise.  If the 'block' does not match a block that
        // was created by this test dispenser, report the error to 'stdout'
        // (unless this test dispenser is in quiet mode) and abort the task
        // (unless this test dispenser is in no-abort mode).

    int numBlocksProtected() const;
        // Return the number of blocks created by this test dispenser that are
        // currently in a protected state.
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                  // -----------------------------------------
                  // class bdema_TestProtectableBlockDispenser
                  // -----------------------------------------

// MANIPULATORS
inline
void bdema_TestProtectableBlockDispenser::setNoAbort(bool flagValue)
{
    d_noAbortFlag = flagValue;
}

inline
void bdema_TestProtectableBlockDispenser::setQuiet(bool flagValue)
{
    d_quietFlag = flagValue;
}

inline
void bdema_TestProtectableBlockDispenser::setVerbose(bool flagValue)
{
    d_verboseFlag = flagValue;
}

inline
void bdema_TestProtectableBlockDispenser::setAllocationLimit(int numAlloc)
{
    d_allocationLimit = numAlloc;
}

// ACCESSORS
inline
bool bdema_TestProtectableBlockDispenser::isNoAbort() const
{
    return d_noAbortFlag;
}

inline
bool bdema_TestProtectableBlockDispenser::isQuiet() const
{
    return d_quietFlag;
}

inline
bool bdema_TestProtectableBlockDispenser::isVerbose() const
{
    return d_verboseFlag;
}

inline
int bdema_TestProtectableBlockDispenser::numBlocksInUse() const
{
    return d_numBlocksInUse;
}

inline
int bdema_TestProtectableBlockDispenser::numBytesInUse() const
{
    return d_numBytesInUse;
}

inline
int bdema_TestProtectableBlockDispenser::numBlocksMax() const
{
    return d_numBlocksMax;
}

inline
int bdema_TestProtectableBlockDispenser::numBytesMax() const
{
    return d_numBytesMax;
}

inline
int bdema_TestProtectableBlockDispenser::numBlocksTotal() const
{
    return d_numBlocksTotal;
}

inline
int bdema_TestProtectableBlockDispenser::numBytesTotal() const
{
    return d_numBytesTotal;
}

inline
int bdema_TestProtectableBlockDispenser::numErrors() const
{
    return d_numErrors;
}

inline
int bdema_TestProtectableBlockDispenser::allocationLimit() const
{
    return d_allocationLimit;
}

inline
int bdema_TestProtectableBlockDispenser::lastAllocateNumBytes() const
{
    return d_lastAllocateNumBytes;
}

inline
int bdema_TestProtectableBlockDispenser::lastDeallocateNumBytes() const
{
    return d_lastDeallocateNumBytes;
}

inline
void *bdema_TestProtectableBlockDispenser::lastAllocateAddress() const
{
    return d_lastAllocateAddress;
}

inline
void *bdema_TestProtectableBlockDispenser::lastDeallocateAddress() const
{
    return d_lastDeallocateAddress;
}

inline
int bdema_TestProtectableBlockDispenser::numAllocations() const
{
    return d_numAllocation;
}

inline
int bdema_TestProtectableBlockDispenser::numDeallocations() const
{
    return d_numDeallocation;
}

inline
int bdema_TestProtectableBlockDispenser::pageSize() const
{
    return d_pageSize;
}

inline
int bdema_TestProtectableBlockDispenser::numBlocksProtected() const
{
    return d_numBlocksProtected;
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
