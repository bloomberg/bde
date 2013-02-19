// baesu_stacktracetestallocator.h                                    -*-C++-*-
#ifndef INCLUDED_BAESU_STACKTRACETESTALLOCATOR
#define INCLUDED_BAESU_STACKTRACETESTALLOCATOR

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a test allocator that reports the call stack for leaks.
//
//@CLASSES:
//   baesu_StackTraceTestAllocator: allocator that reports call stack for leaks
//
//@AUTHOR: Bill Chapman (bchapman2)
//
//@DESCRIPTION: This component provides an instrumented allocator,
// 'baesu_StackTraceTestAllocator', that implements the
// 'bdema_ManagedAllocator' protocol.  An object of this type records the call
// stack for each allocation performed, and can report, either using the
// 'reportBlocksInUse' method or implicitly at destruction, the call stack
// associated with every allocated block that has not (yet) been freed.  It is
// optionally supplied a 'bslma::Allocator' at construction that it uses to
// allocate memory.
//..
//                    ,------------------------------.
//                   ( baesu_StackTraceTestAllocator  )
//                    `------------------------------'
//                                    |    ctor/dtor
//                                    |    numBlocksInUse
//                                    |    reportBlocksInUse
//                                    |    setFailureHandler
//                                    V
//                         ,----------------------.
//                        ( bdema_ManagedAllocator)
//                         `----------------------'
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
// or as a memory leak).  Using 'deallocate' to free memory supplied by global
// 'new' or 'malloc' will immediately cause an error to be reported to the
// associated 'ostream' and the abort handler (which can be configured to be
// a no-op) called.
//
///Overhead / Efficiency
///---------------------
// There is some overhead to using this allocator.  It's is slower than
// 'bslma::NewDeleteAllocator' and uses more memory.  It is, however,
// comparable in speed and memory usage to 'bslma::TestAllocator'.  The stack
// trace stored for each allocation is stack pointers only, which are compact
// and quick to obtain.  Actual respolving of the stack pointer to subroutine
// names and, on some platforms, source file names and line numbers, is
// expensive but doesn't happen during allocation or deallocation and is put
// off until a memory leak report is being generated.
//
// Note that the overhead increases and efficiency decreases as the
// 'numRecordedFrames' argument to the constructor is increased.
//
///FailureHandler
///--------------
// An object of type 'baesu_StackTraceTestAllocator' always has a failuer
// handler associated with it.  This a a configurable subroutine that will be
// called if any error condition is detected, after the error condition is
// reported.  By default, it is set to
// 'baesu_StackTraceTestAllocator::failureHandlerAbort' which calls 'abort',
// but it may be set by 'setFailureHandler' to another routine.  If the client
// does not want a core dump to occur, it is recommended they do:
//..
//    stackTraceTestAllocator.setFailurehandler(
//                         &baesu_StackTraceTestAllocator::failureHandlerNoop);
//..
// The stack trace test allocator is prepared for the failure handler to
// return, throw (provided the client will catch the exception) or longjmp
// without undefined behavior.
//
// If a memory is found to be outstanding during destruction, that is
// considered a memory leak and a report is written.  After the report, the
// failure handler is called, and if the failure handler returns, the leaked
// memory is then released.  This means that if the failure handler throws or
// longjmps in this case, the leaked memory will not be freed, and there will
// be no way to release it afterward since the allocator will no longer exist.
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

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BCEMT_MUTEX
#include <bcemt_mutex.h>
#endif

#ifndef INCLUDED_BDEMA_MANAGEDALLOCATOR
#include <bdema_managedallocator.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

                      // ===================================
                      // class baesu_StackTraceTestAllocator
                      // ===================================

class baesu_StackTraceTestAllocator : public bdema_ManagedAllocator {
    // This class defines a concrete "test" allocator mechanism that implements
    // the 'bdema_ManagedAllocator' protocol, and provides instrumentation to
    // track the set of all blocks allocated by this allocator that have yet to
    // be freed.  At any time it can produce a report about such blocks,
    // listing for each place that any unfreed blocks were allocated
    //: o the number of unfreed blocks allocated at that place
    //: o the stack trace at that place
    // The allocator will also detect redundant frees of the same block, and
    // frees by the wrong allocator.  The client can choose whether such
    // violations are handled by a core dump, or merely a report being written.
    //
    // Note that, unlike many other allocators, this allocator does DOES NOT
    // rely on the currently installed default allocator (see 'bslma_default')
    // at all, but instead -- by default -- uses 'MallocFreeAllocator'
    // singleton, which in turn calls the C Standard Library functions 'malloc'
    // and 'free' as needed.  Clients may, however, override this allocator by
    // supplying (at construction) any other allocator implementing the
    // 'bslam::Allocator' protocol.

  public:
    // PUBLIC TYPES
    typedef void (*FailureHandler)();
        // Type of function called by this object to handle failures.  The
        // failure handler is allowed to return, throw an exception, abort,
        // or 'longjmp'.

  private:
    // PRIVATE TYPES
    enum AllocatorMagic { STACK_TRACE_TEST_ALLOCATOR_MAGIC = 1335775331 };

    struct BlockHeader;                            // information stored in
                                                   // each block

    // DATA
    AllocatorMagic            d_magic;             // magic # to identify type
                                                   // of memory allocator

    volatile int              d_numBlocksInUse;    // number of allocated
                                                   // blocks currently unfreed

    BlockHeader              *d_blocks;            // list of allocated,
                                                   // unfreed blocks

    mutable bcemt_Mutex       d_mutex;             // mutex used to synchronize
                                                   // access to this object

    const char               *d_name;              // name of this allocator

    volatile FailureHandler   d_failureHandler;    // function we are to call
                                                   // on errors.  The default
                                                   // handler will call
                                                   // 'abort'.

    const int                 d_maxRecordedFrames; // max number of stack trace
                                                   // frames to store in each
                                                   // block; may be larger than
                                                   // the number of frames
                                                   // requested to the ctor due
                                                   // to ignored frames

    const int                 d_traceBufferLength; // length, in pointers, of
                                                   // area for storing stack
                                                   // traces; may be larger
                                                   // than
                                                   // 'd_maxRecordedFrames' due
                                                   // to alignment
                                                   // considerations

    bsl::ostream             *d_ostream;           // stream to which
                                                   // diagnostics are to be
                                                   // written; held, not owned

    bool                      d_demangleFlag;      // if 'true', demangling of
                                                   // symbol names is attempted

    bslma::Allocator         *d_allocator_p;       // held, not owned

  private:
    // NOT IMPLEMENTED
    baesu_StackTraceTestAllocator(const baesu_StackTraceTestAllocator&);
    baesu_StackTraceTestAllocator& operator=(
                                         const baesu_StackTraceTestAllocator&);

  private:
    // PRIVATE ACCESSORS
    int checkBlockHeader(const BlockHeader *blockHdr) const;
        // Return 0 if the block specified by 'blockHdr' was allocated with
        // this allocator, is not corrupted, and has not yet been freed; report
        // diagnostics to '*d_ostream' and return a non-zero value otherwise.

  public:
    // CLASS METHODS
    static void failAbort();
        // Calls 'bsl::abort()', 'd_failureHanlder' is initialized to this
        // value by all constructors.  Note that in ALL failure situations,
        // errors or warnings will be written to the 'ostream' associated with
        // this object prior to the failure handler call.

    static void failNoop();
        // Does nothing.  'setFailureHandler' may be called with this function,
        // in which case this allocator object, when a faliure occurs, will
        // recover rather than abort.  Note that in ALL failure situations,
        // errors or warnings will be written to the 'ostream' associated with
        // this object prior to the failure handler call.

    // CREATORS
    explicit
    baesu_StackTraceTestAllocator(bslma_Allocator *basicAllocator = 0);
    explicit
    baesu_StackTraceTestAllocator(int              numRecordedFrames,
                                  bslma_Allocator *basicAllocator = 0);
        // Create a test allocator.  Optionally specify 'numRecordedFrames',
        // the number of stack trace frame pointers to be saved for every
        // allocation.  Specifying a larger value of 'numRecordedFrames' means
        // that stack traces, when given, will be more complete, but will also
        // mean that both more CPU time and more memory per allocation will be
        // consumed.  If 'numRecordedFrames' is not specified, a value of '12'
        // will be assumed.  Optionally specify 'basicAllocator', the allocator
        // from which memory will be provided.  If 'basicAllocator' is 0, the
        // 'MallocFreeAllocator' singleton is used.  Associate 'bsl::cerr' with
        // this object for error diagnostic output, which may be changed by
        // calling the 'setOstream' manipulator.  Set the
        // 'demanglingPreferringFlag' attribute to 'true', which may be changed
        // using the 'setDemanglingPreferredFlag' manipulator.  The behavior is
        // undefined if 'numRecordedFrames < 2'.

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
        // the allocator supplied at construction and record the returned block
        // in order to be able to report leaked blocks upon destruction.

    virtual void deallocate(void *address);
        // Return the memory block at the specified 'address' back to this
        // allocator.  If 'address' is 0, this function has no effect.
        // Otherwise, if the memory at 'address' is consistent with being
        // allocated from this test allocator, deallocate it using the
        // underlying allocator and delete it from the data structures keeping
        // track of blocks in use'.  If 'address' is not zero and is not the
        // address of a block allocated with this allocator (or if it is being
        // deallocated a second time), write an error message and call the
        // failure handler.

    virtual void release();
        // Deallocate all memory held by this allocator.

    void setDemanglingPreferredFlag(bool value);
        // Set the 'demanglingPreferredFlag' attribute, which is used to
        // determine whether demangling of symbols is to be attempted when
        // generating diagnostics, to the specified 'value'.  The default value
        // of the flag is 'true'.  However the flag is ingored on some
        // platforms; demangling never happens on some platforms and always
        // happens on others.

    void setFailureHandler(FailureHandler func);
        // Set the failure handler associated with this allocator object to the
        // specified 'func'.  Upon construction, the function
        // 'failureHandlerAbort' is associated with this object by default.
        // Note that 'func' will be called by this object's destructor if
        // memory is leaked, so it is important that it not throw.  Note that
        // in ALL failure situations, errors or warnings will be written to the
        // 'ostream' associated with this object prior to the call to the
        // failure handler.

    void setName(const char * name);
        // Set the name of this allocator to the specified 'name'.  If
        // 'setName' is never called, the name of the allocator is "<unnamed>".
        // Note that the lifetime of 'name' must exceed the lifetime of this
        // object.

    void setOstream(bsl::ostream *ostream);
        // Set the stream to which diagnostics will be written to the specified
        // 'ostream'.  If 'setOstream' is never called, diagnositcs will be
        // written to 'bsl::cerr'.

    // ACCESSORS
    FailureHandler failureHandler() const;
        // Return the pointer to the function that will be called to facilitate
        // this allocator object's handling of failure.

    bsl::size_t numBlocksInUse() const;
        // Return the number of blocks currently allocated from this object.

    void reportBlocksInUse(bsl::ostream *ostream = 0) const;
        // Write a report to the specified 'ostream', reporting the unique
        // call-stacks for each block that has been allocated and has not yet
        // been freed.  If 'ostream' is not specified, the value of 'ostream'
        // passed to the last call to 'setOstream' will be used.  If
        // 'setOstream' was never called, 'bsl::cerr' will be used.
};

                      // -----------------------------------
                      // class baesu_StackTraceTestAllocator
                      // -----------------------------------

inline
baesu_StackTraceTestAllocator::FailureHandler
baesu_StackTraceTestAllocator::failureHandler() const
{
    return d_failureHandler;
}

inline
bsl::size_t baesu_StackTraceTestAllocator::numBlocksInUse() const
{
    return d_numBlocksInUse;
}

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
