// baesu_stacktracetestallocator.h                                    -*-C++-*-
#ifndef INCLUDED_BAESU_STACKTRACETESTALLOCATOR
#define INCLUDED_BAESU_STACKTRACETESTALLOCATOR

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide an allocator that reports the call stack for leaks.
//
//@CLASSES:
//   baesu_StackTraceTestAllocator: reports the call stack for memory leaks.
//
//@AUTHOR: Bill Chapman (bchapman2)
//
//@DESCRIPTION: This component provides an instrumented allocator,
// 'baesu_StackTraceTestAllocator' that implements the
// 'bslma::ManagedAllocator' protocol.  An object of this type records the call
// stack for each allocation performed, and can report, either using the
// 'reportBlocksInUse' method or implicitly at destruction, the call stack
// associated with every allocated block that has not (yet) been freed.  It is
// optionally supplied a 'bslma::Allocator' at construction that it uses to
// allocator memory.
//..
//                    ,------------------------------.
//                   ( baesu_StackTraceTestAllocator  )
//                    `------------------------------'
//                                    |    ctor/dtor
//                                    |    numBlocksInUse
//                                    |    reportBlocksInUse
//                                    |    setNoAbort
//                                    V
//                         ,-----------------------.
//                        ( bslma::ManagedAllocator )
//                         `-----------------------'
//                                    |    release
//                                    V
//                             ,----------------.
//                            ( bslma::Allocator )
//                             `----------------'
//                                         allocate
//                                         deallocate
//..
// Note that allocation using a 'baesu_StackTraceTestAllocator' is deliberately
// incompatible with the default global 'new', 'malloc', 'delete', and 'free'.
// Using 'delete' or 'free' to free memory supplied by this allocator will
// corrupt the dynamic memory manager and also cause a memory leak (and will be
// reported by purify as freeing mismatched memory, freeing unallocated memory,
// and as a memory leak).  Using 'deallocate' to free memory supplied by global
// 'new' or 'malloc' will immediately cause an error to be reported to the
// associated 'ostream' and, unless the 'noAbort' flag is set, an abort (core
// dump).
//
///No-Abort Mode
///-------------
// A 'baesu_StackTraceTestAllocator' object can be put into 'noAbort' mode.  If
// 'true', the stack trace test allocator won't abort if it detects user
// errors, though it will still abort if it detects a consistency error within
// its own code.  The 'noAbort' flag is 'false' at construction, and can be set
// afterward with the 'setNoAbort' manipulator.
//
///Usage
///-----
// Suppose we want a 'struct' that will keep track of a ship's crew.  Upon
// creation, it will parse a text file whose name is passed to the constructor
// containing the names and ranks of the crew members of the ship, and the
// members names will be public data members of the class.
//
// First, we define the class:
//..
//  struct ShipsCrew {
//      // This struct will, at construction, read and parse a file describing
//      // the names of the crew of a ship.
//
//    private:
//      // PRIVATE TYPES
//      struct Less {
//          // Functor to compare two 'const char *'s
//
//          bool operator()(const char *a, const char *b) const
//          {
//              return bsl::strcmp(a, b) < 0;
//          }
//      };
//
//      typedef bsl::set<const char *, Less> NameSet;
//
//    public:
//      // PUBLIC DATA
//      const char       *d_captain;
//      const char       *d_firstMate;
//      const char       *d_cook;
//      NameSet           d_sailors;
//
//    private:
//      // PRIVATE MANIPULATORS
//      void addSailor(const bsl::string& name);
//          // Add the specified 'name' to the set of sailor's names.  Check
//          // for redundancy.
//
//      const char *copy(const bsl::string& str);
//          // Allocate memory for a copy of the specified 'str' as a char
//          // array, copy the contents of 'str' into it, and return a pointer
//          // to the array.
//
//      void setCaptain(const bsl::string& name);
//          // Set the name of the ship's captain to the specified 'name', but
//          // only if the captain's name was not already set.
//
//      void setCook(const bsl::string& name);
//          // Set the name of the ship's cook to the specified 'name', but
//          // only if the captain's name was not already set.
//
//      void setFirstMate(const bsl::string& name);
//          // Set the name of the ship's first mate to the specified 'name',
//          // but only if the captain's name was not already set.
//
//    public:
//      // CREATORS
//      explicit
//      ShipsCrew(const char *crewFileName);
//          // Read the names of the ship's crew in from the file with the
//          // specified name 'crewFileName'.
//
//      ~ShipsCrew();
//          // Destroy this object and free memory.
//  };
//
//  // PRIVATE MANIPULATORS
//  void ShipsCrew::addSailor(const bsl::string& name)
//  {
//      BSLS_ASSERT(! d_sailors.count(name.c_str()));
//
//      d_sailors.insert(copy(name));
//  }
//
//  const char *ShipsCrew::copy(const bsl::string& str)
//  {
//      return BloombergLP::bdeu_String::copy(str,
//                                         bslma::Default::defaultAllocator());
//  }
//
//  void ShipsCrew::setCaptain(const bsl::string& name)
//  {
//      BSLS_ASSERT(! d_captain);
//
//      d_captain = copy(name);
//  }
//
//  void ShipsCrew::setCook(const bsl::string& name)
//  {
//      BSLS_ASSERT(! d_cook);
//
//      d_cook = copy(name);   // This was line 231 when this test case was
//                             // written
//  }
//
//  void ShipsCrew::setFirstMate(const bsl::string& name)
//  {
//      BSLS_ASSERT(! d_firstMate);
//
//      d_firstMate = copy(name);
//  }
//
//  // CREATORS
//  ShipsCrew::ShipsCrew(const char *crewFileName)
//  : d_captain(0)
//  , d_firstMate(0)
//  , d_cook(0)
//  , d_sailors()
//  {
//      typedef BloombergLP::bdeu_String String;
//
//      bsl::ifstream input(crewFileName);
//      BSLS_ASSERT(!input.eof() && !input.bad());
//
//      while (!input.bad() && !input.eof()) {
//          bsl::string line;
//          bsl::getline(input, line);
//
//          bsl::size_t colon = line.find(':');
//          if (bsl::string::npos != colon) {
//              bsl::string field = line.substr(0, colon);
//              bsl::string name  = line.substr(colon + 1);
//
//              if (0 == String::lowerCaseCmp(field, "captain")) {
//                  setCaptain(name);
//              }
//              else if (0 == String::lowerCaseCmp(field, "first mate")) {
//                  setFirstMate(name);
//              }
//              else if (0 == String::lowerCaseCmp(field, "cook")) {
//                  setCook(name);
//              }
//              else if (0 == String::lowerCaseCmp(field, "sailor")) {
//                  addSailor(name);
//              }
//              else {
//                  cerr << "Unrecognized field '" << field << "'\n";
//              }
//          }
//          else if (!line.empty()) {
//              cerr << "Garbled line '" << line << "'\n";
//          }
//      }
//  }
//
//  ShipsCrew::~ShipsCrew()
//  {
//      bslma::Allocator *da = bslma::Default::defaultAllocator();
//
//      da->deallocate(const_cast<char *>(d_captain));
//      da->deallocate(const_cast<char *>(d_firstMate));
//
//      // Note that deallocating the strings will invalidate 'd_sailors' --
//      // any manipulation of 'd_sailors' other than destruction after this
//      // would lead to undefined behavior.
//
//      const NameSet::iterator end = d_sailors.end();
//      for (NameSet::iterator it = d_sailors.begin(); end != it; ++it) {
//          da->deallocate(const_cast<char *>(*it));
//      }
//  }
//..
// Then, we define the free function 'getCaptain' which will create and load
// a 'ShipsCrew' object and return the captain's name:
//..
//  bsl::string getCaptain(const char *fileName)
//  {
//      ShipsCrew crew(fileName);
//
//      return crew.d_captain ? crew.d_captain : "";
//  }
//..
// Next, we install a stack trace test allocator as the default allocator.  We
// set the failure handler to 'failureHandlerNoop' to prevent it from aborting
// if it finds any memory leaks:
//..
//  {
//      baesu_StackTraceTestAllocator ta("Test Allocator");
//      ta.setFailureHandler(&Obj::failureHandlerNoop);
//      bslma::DefaultAllocatorGuard guard(&ta);
//..
// Then, we call our 'getCaptain' function to find the captain's name:
//..
//     bsl::string captain = getCaptain("shipscrew.txt");
//
//      cout << "The captain is: " << captain << bsl::endl;
//..
// Now, we close the block, destroying 'ta', the stack trace test allocator,
// which causes it to report memory leakss to 'cout', and we read the report of
// memory leaks written to 'cout' when 'ta' was destroyed:
//..
//  }
//
// ====================================================================
// Error: memory leaked:
// 1 segment(s) in allocator 'Test Allocator' in use.
// Segment(s) allocated from 1 trace(s).
// --------------------------------------------------------------------
// Allocation trace 1, 1 segment(s) in use.
// Stack trace at allocation time:
// (0): BloombergLP::baesu_StackTraceTestAllocator::.allocate(long)+
//      0x2fc at 0x100007b64 source:baesu_stacktracetestallocator.cpp:
//      335 in baesu_stacktracetestallocator.t.d
// (1): BloombergLP::bdeu_String::.copy(const char*,int,BloombergLP::
//      bslma::Allocator*)+0xbc at 0x10001893c source:bdeu_string.cpp:
//      96 in baesu_stacktracetestallocator.t.d
// (2): ShipsCrew::.copy(const bsl::basic_string<char,std::char_traits<
//      char>,bsl::allocator<char> >&)+0x8c at 0x1000232c4 source:
//      baesu_stacktracetestallocator.t.cpp:610 in
//      baesu_stacktracetestallocator.t.d
// (3): ShipsCrew::.setCook(const bsl::basic_string<char,std::
//      char_traits<char>,bsl::allocator<char> >&)+0x54 at 0x1000233e4
//      source:baesu_stacktracetestallocator.t.cpp:232 in
//      baesu_stacktracetestallocator.t.d
// (4): ShipsCrew::.__ct(const char*)+0x430 at 0x1000214c8
//      source:baesu_stacktracetestallocator.t.cpp:272 in
//      baesu_stacktracetestallocator.t.d
// (5): .getCaptain(const char*)+0x44 at 0x100020704 source:
//      baesu_stacktracetestallocator.t.cpp:306 in
//      baesu_stacktracetestallocator.t.d
// (6): .main+0x278 at 0x100000ab0 source:
//      baesu_stacktracetestallocator.t.cpp:727 in
//      baesu_stacktracetestallocator.t.d
// (7): .__start+0x74 at 0x1000002fc source:crt0_64.s in
//      baesu_stacktracetestallocator.t.d
//..
// Finally, we see that the leaked memory was in the 'setCook' method line 232
// (line numbers are generally the line after the subroutine call in question
// -- the call to 'copy' was on line 231).  The destructor neglected to
// deallocate the cook's name.
//
// Note the following:
//: o 'ta's destructor freed all the leaked memory, because
//:   'setFailureHandler(&Obj::failureHandlerNoop)' had been called.  If the
//:   default value, '&Obj::failureHandlerAbort', had been left in place, the
//:   destructor would have core dumped before freeing the leaked memory.
//: o Output will vary by platform.  Not all platforms support line number
//:   information and demangling.  This report was generated on AIX, and a
//:   couple of AIX quirks are visible -- identifiers have a '.' prepended, and
//:   the constructor name got converted to '__ct'.

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCEMT_MUTEX
#include <bcemt_mutex.h>
#endif

#ifndef INCLUDED_BSLMA_MANAGEDALLOCATOR
#include <bslma_managedallocator.h>
#endif

#ifndef INCLUDED_BSLMF_ASSERT
#include <bslmf_assert.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSL_HASH_SET
#include <bsl_hash_set.h>
#endif

#ifndef INCLUDED_BSL_IOSTREAM
#include <bsl_iostream.h>
#endif

namespace BloombergLP {

                        // =========================
                        // class bcema_TestAllocator
                        // =========================

class baesu_StackTraceTestAllocator : public bslma::ManagedAllocator {
    // This class defines a concrete "test" allocator mechanism that implements
    // the 'bslma::ManagedAllocator' protocol, and provides instrumentation to
    // track the set of all segments allocted by this allocator that have yet
    // to be freed.  At any time it can produce a report about such segments,
    // listing for each place that any unfreed segments were allocated
    //: o the number of unfreed segments allocated at that place
    //: o the stack trace at that place
    // The allocator will also detect redundant frees of the same segment, and
    // frees by the wrong allocator.  The client can choose whether such
    // violations are handled by a core dump, or merely a report being written.
    //
    // Note that, unlike many other allocators, this allocator does DOES NOT
    // rely on the currently installed default allocator (see 'bslma_default')
    // at all, but instead -- by default -- uses 'MallocFreeAllocator'
    // singleton, which in turn calls the C Standard Library functions 'malloc'
    // and 'free' as needed.  Clients may, however, override this allocator by
    // supplying (at construction) any other allocator implementing the
    // 'Allocator' protocol.

  public:
    // PUBLIC TYPES
    typedef void (*FailureHandler)();
        // Type of function called by this object to handle failures.  This
        // allocator is guaranteed to cope, without undefined behavior,
        // properly if the failure handler returns, throws, or longjmp's.

  private:
    // PRIVATE TYPES
    enum AllocatorMagic { STACK_TRACE_TEST_ALLOCATOR_MAGIC = 1335775331 };

    struct SegmentHeader;                       // information stored in each
                                                // segment

    // DATA
    AllocatorMagic            d_magic;          // magic # to identify type of
                                                // memory allocator

    volatile int              d_numBlocksInUse; // number of allocated blocks
                                                // currently unfreed

    SegmentHeader            *d_segments;       // list of allocated, unfreed
                                                // segments

    mutable bcemt_Mutex       d_mutex;          // mutex used to synchronize
                                                // access to this object

    const char               *d_name;           // name of this allocator
                                                // (held, not owned)

    volatile FailureHandler  d_failureHandler;  // function we are to call to
                                                // abort.

    const int                 d_maxRecordedFrames; // max number of stack trace
                                                   // frames to store in each
                                                   // segment.  May be larger
                                                   // than the number of frames
                                                   // requested to the c'tor
                                                   // due to ignored frames.

    const int                 d_traceBufferLength; // length of area for
                                                   // storing stack traces.
                                                   // May be larger than
                                                   // 'd_maxRecordedFrames' due
                                                   // to alignment
                                                   // considerations.

    bsl::ostream             *d_ostream;        // stream to which reports are
                                                // to be written.  Held, not
                                                // owned.

    bool                      d_demangleFlag;   // if 'true' demangling of
                                                // symbol names is attempted

    bslma::Allocator         *d_allocator_p;    // held, not owned

  private:
    // NOT IMPLEMENTED
    baesu_StackTraceTestAllocator(const baesu_StackTraceTestAllocator&);
    baesu_StackTraceTestAllocator& operator=(
                                         const baesu_StackTraceTestAllocator&);

  private:
    // PRIVATE ACCESSORS
    int preDeallocateCheckSegmentHeader(
                                     const SegmentHeader *segmentHdr) const;
        // Perform sanity checks on segment header, reporting any
        // irregularities to '*d_ostream' Return 0 if no irregularities are
        // found, and a non-zero value otherwise.

  public:
    // CLASS METHODS
    static void failureHandlerAbort();
        // Calls 'bsl::abort()', 'd_failureHanlder' is initialized to this
        // value by all constructors.  Note that in ALL failure situations,
        // errors or warnings will be written to the 'ostream' associated with
        // this object prior to the failure handler call.

    static void failureHandlerNoop();
        // Does nothing.  'setFailureHandler' may be called with this function,
        // in which case this allocator object, when a faliure occurs, will
        // recover rather than abort.  Note that in ALL failure situations,
        // errors or warnings will be written to the 'ostream' associated with
        // this object prior to the failure handler call.

    // CREATORS
    explicit
    baesu_StackTraceTestAllocator(
                         bsl::ostream    *ostream                 = &bsl::cout,
                         int              numRecordedFrames       = 12,
                         bool             demanglingPreferredFlag = true,
                         bslma_Allocator *basicAllocator          = 0);
    explicit
    baesu_StackTraceTestAllocator(
                         const char      *name,
                         bsl::ostream    *ostream                 = &bsl::cout,
                         int              numRecordedFrames       = 12,
                         bool             demanglingPreferredFlag = true,
                         bslma_Allocator *basicAllocator          = 0);
        // Create a test allocator.  Optionally specify 'name' as the name of
        // the allocator, to be used in reports and error messages.  Optionally
        // specify 'ostream', the stream to which error messages are to be
        // reported.  If 'ostream' is not specified, 'bsl::cout' is used.
        // Optionally specify 'numRecordedFrames', the number of stack trace
        // frame pointers to be saved for every allocation.  Specifying a
        // larger value of 'numRecordedFrames' means that stack traces, when
        // given, will be more complete, but will also mean that both more cpu
        // time and more memory per allocation will be consumed.  Optionally
        // specify 'demanglingPreferredFlag' to indicate whether demangling is
        // desired.  'demanglingPreferredFlag' signals a preference only, on
        // some platforms demangling is not implemented or not optional, and on
        // those platforms the flag is ignored.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the 'bslma::MallocFreeAllocator' singleton is used, since the client
        // may be trying to avoid using the default allocator.  The behavior is
        // undefined unless 'name', 'ostream', and 'basicAllocator' remain
        // valid for the lifetime of the object, and unless
        // 'numRecordedFrames >= 2'.

    virtual ~baesu_StackTraceTestAllocator();
        // Destroy this allocator.  Report any memory leaks to the 'ostream'
        // that was supplied at construction.  If no memory leaks are observed,
        // nothing is written to the output 'ostream'.  Call the failure
        // handler if 'numBlocksInUse() > 0'.  Note that a report of
        // outstanding memory blocks is written to 'ostream' before the failure
        // handler is called, and if the failure handler returns, all
        // outstanding memory blocks will be released.

    // MANIPULATORS
    virtual void *allocate(size_type size);
        // Return a newly allocated block of memory of the specified positive
        // 'size' (in bytes).  If 'size' is 0, a null pointer is returned with
        // no other other effect.  Otherwise, invoke the 'allocate' method of
        // the allocator supplied at construction and record the returned
        // segment in order to be able to report leaked segments upon
        // destruction.

    virtual void deallocate(void *address);
        // Return the memory block at the specified 'address' back to this
        // allocator.  If 'address' is 0, this function has no effect.
        // Otherwise, if the memory at 'address' is consistent with being
        // allocated from this test allocator, deallocate it using the
        // underlying allocator and delete it from the data structures keeping
        // track of blocks in use'.  If 'address' is not zero and is not the
        // address of a segment allocated with this allocator (or if it is
        // being deallocated a second time), write an error message and call
        // the failure handler.

    virtual void release();
        // Deallocate all memory held by this allocator.

    void setFailureHandler(FailureHandler func);
        // Set the failure handler associated with this allocator object to the
        // specified 'func'.  Upon construction, the function
        // 'failureHandlerAbort' is associated with this object by default.
        // Note that 'func' will be called by this object's destructor if
        // memory is leaked, so it is important that it not throw.  Note that
        // in ALL failure situations, errors or warnings will be written to the
        // 'ostream' associated with this object prior to the failulre handler
        // call.

    // ACCESSORS
    FailureHandler failureHandler() const;
        // Return the pointer to the function that will be called to facilitate
        // this allocator object's handling of failure.

    bsl::size_t numBlocksInUse() const;
        // Return the number of segments that have been allocated and are not
        // yet freed.  Note that this is not guaranteed to execute in constant
        // time.

    void reportBlocksInUse(bsl::ostream *ostream = 0) const;
        // Write a report to the output stream associated with this test
        // allocator, reporting the unique call-stacks for each segment that
        // has been allocated and has not yet been freed.
};

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
