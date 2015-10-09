// balst_stacktracetestallocator.h                                    -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALST_STACKTRACETESTALLOCATOR
#define INCLUDED_BALST_STACKTRACETESTALLOCATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a test allocator that reports the call stack for leaks.
//
//@CLASSES:
//  balst::StackTraceTestAllocator: allocator that reports call stack for leaks
//
//@DESCRIPTION: This component provides an instrumented allocator,
// 'balst::StackTraceTestAllocator', that implements the
// 'bdlma::ManagedAllocator' protocol.  An object of this type records the call
// stack for each allocation performed, and can report, either using the
// 'reportBlocksInUse' method or implicitly at destruction, the call stack
// associated with every allocated block that has not (yet) been freed.  It is
// optionally supplied a 'bslma::Allocator' at construction that it uses to
// allocate memory.
//..
//                    ,------------------------------.
//                   ( balst::StackTraceTestAllocator  )
//                    `------------------------------'
//                                    |    ctor/dtor
//                                    |    numBlocksInUse
//                                    |    reportBlocksInUse
//                                    |    setFailureHandler
//                                    V
//                         ,----------------------.
//                        ( bdlma::ManagedAllocator)
//                         `----------------------'
//                                    |    release
//                                    V
//                             ,----------------.
//                            ( bslma::Allocator )
//                             `----------------'
//                                         allocate
//                                         deallocate
//..
// Note that allocation using a 'balst::StackTraceTestAllocator' is
// deliberately incompatible with the default global 'new', 'malloc', 'delete',
// and 'free'.  Using 'delete' or 'free' to free memory supplied by this
// allocator will corrupt the dynamic memory manager and also cause a memory
// leak (and will be reported by purify as freeing mismatched memory, freeing
// unallocated memory, or as a memory leak).  Using 'deallocate' to free memory
// supplied by global 'new' or 'malloc' will immediately cause an error to be
// reported to the associated 'ostream' and the abort handler (which can be
// configured to be a no-op) called.
//
///Overhead / Efficiency
///---------------------
// There is some overhead to using this allocator.  It's is slower than
// 'bslma::NewDeleteAllocator' and uses more memory.  It is, however,
// comparable in speed and memory usage to 'bslma::TestAllocator'.  The stack
// trace stored for each allocation is stack pointers only, which are compact
// and quick to obtain.  Actual resolving of the stack pointer to subroutine
// names and, on some platforms, source file names and line numbers, is
// expensive but doesn't happen during allocation or deallocation and is put
// off until a memory leak report is being generated.
//
// Note that the overhead increases and efficiency decreases as the
// 'numRecordedFrames' argument to the constructor is increased.
//
///Failure Handler
///---------------
// An object of type 'balst::StackTraceTestAllocator' always has a failure
// handler associated with it.  This a a configurable 'bdef::Functton' object
// that will be called if any error condition is detected, after the error
// condition is reported.  By default, it is set to
// 'balst::StackTraceTestAllocator::failAbort' which calls 'abort', but it may
// be set by 'setFailureHandler' to another routine.  If the client does not
// want a core dump to occur, it is recommended they do:
//..
//    stackTraceTestAllocator.setFailurehandler(
//                                  &balst::StackTraceTestAllocator::failNoop);
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
// In this example, we will define a class 'ShipsCrew' that does something,
// but leaks memory, and then we will demonstrate the use of the stack trace
// test allocator to locate the leak.
//
// First, we define 'ShipsCrew', a class that will read the names of a ship's
// crew from a file at construction, and make the results available through
// accessors:
//..
//  struct ShipsCrew {
//      // This struct will, at construction, read and parse a file describing
//      // the names of the crew of a ship.
//
//    private:
//      // PRIVATE TYPES
//      struct CharStrLess {
//          // Functor to compare two 'const char *'s in alphabetical order.
//
//          bool operator()(const char *a, const char *b) const
//          {
//              return bsl::strcmp(a, b) < 0;
//          }
//      };
//
//      typedef bsl::set<const char *, CharStrLess> NameSet;
//
//      // DATA
//      const char       *d_captain;
//      const char       *d_firstMate;
//      const char       *d_cook;
//      NameSet           d_sailors;
//      bslma::Allocator *d_allocator_p;
//
//    private:
//      // PRIVATE MANIPULATORS
//      void addSailor(const bsl::string& name);
//          // Add the specified 'name' to the set of sailor's names.
//          // Redundant names are ignored.
//
//      const char *copy(const bsl::string& str);
//          // Allocate memory for a copy of the specified 'str' as a char
//          // array, copy the contents of 'str' into it, and return a pointer
//          // to the new copy.
//
//      void free(const char **str);
//          // If '0 != str', deallocate '*str' using the allocator associated
//          // with this object and set '*str' to 0, otherwise do nothing.  The
//          // behavior is undefined if '0 == str'.
//
//      void setCaptain(const bsl::string& name);
//          // Set the name of the ship's captain to the specified 'name'.
//
//      void setCook(const bsl::string& name);
//          // Set the name of the ship's cook to the specified 'name'.
//
//      void setFirstMate(const bsl::string& name);
//          // Set the name of the ship's first mate to the specified 'name'.
//
//    public:
//      // CREATORS
//      explicit
//      ShipsCrew(const char        *crewFileName,
//                bslma::Allocator *basicAllocator = 0);
//          // Read the names of the ship's crew in from the file with the
//          // specified name 'crewFileName'.
//
//      ~ShipsCrew();
//          // Destroy this object and free memory.
//
//      // ACCESSORS
//      const char *captain();
//          // Return the captain's name.
//
//      const char *cook();
//          // Return the cook's name.
//
//      const char *firstMate();
//          // Return the first mate's name.
//
//      const char *firstSailor();
//          // Return the name of the sailor whose name is alphabetically the
//          // first in the list.
//
//      const char *nextSailor(const char *currentSailor);
//          // Return the next sailor alphabetically after the specified
//          // 'currentSailor', or 0 if 'currentSailor' is the last in the list
//          // or not found.  The behavior is undefined if
//          // '0 == currentSailor'.
//  };
//..
// Then, we implement the private manipulators of the class:
//..
// PRIVATE MANIPULATORS
//  void ShipsCrew::addSailor(const bsl::string& name)
//  {
//      if (!d_sailors.count(name.c_str())) {
//          d_sailors.insert(copy(name));
//      }
//  }
//
//  const char *ShipsCrew::copy(const bsl::string& str)
//  {
//      char *result = (char *) d_allocator_p->allocate(str.length() + 1);
//      bsl::strcpy(result, str.c_str());
//      return result;
//  }
//
//  void ShipsCrew::free(const char **str)
//  {
//      assert(str);
//
//      if (*str) {
//          d_allocator_p->deallocate(const_cast<char *>(*str));
//          *str = 0;
//      }
//  }
//
//  void ShipsCrew::setCaptain(const bsl::string& name)
//  {
//      free(&d_captain);
//
//      d_captain = copy(name);
//  }
//
//  void ShipsCrew::setCook(const bsl::string& name)
//  {
//      free(&d_cook);
//
//      d_cook = copy(name);
//  }
//
//  void ShipsCrew::setFirstMate(const bsl::string& name)
//  {
//      free(&d_firstMate);
//
//      d_firstMate = copy(name);
//  }
//..
// Next, we implement the creators:
//..
//  // CREATORS
//  ShipsCrew::ShipsCrew(const char       *crewFileName,
//                       bslma::Allocator *basicAllocator)
//  : d_captain(0)
//  , d_firstMate(0)
//  , d_cook(0)
//  , d_sailors(    bslma::Default::allocator(basicAllocator))
//  , d_allocator_p(bslma::Default::allocator(basicAllocator))
//  {
//      bsl::ifstream input(crewFileName);
//      BSLS_ASSERT(!input.eof() && !input.bad());
//
//      bsl::string line(d_allocator_p);
//
//      while (!input.bad() && !input.eof()) {
//          bsl::getline(input, line);
//
//          bsl::size_t colon = line.find(':');
//          if (bsl::string::npos != colon) {
//              const bsl::string& field = line.substr(0, colon);
//              const bsl::string& name  = line.substr(colon + 1);
//
//              if      (0 == bdlb::String::lowerCaseCmp(field, "captain")) {
//                  setCaptain(name);
//              }
//              else if (0 == bdlb::String::lowerCaseCmp(field, "first mate")){
//                  setFirstMate(name);
//              }
//              else if (0 == bdlb::String::lowerCaseCmp(field, "cook")) {
//                  setCook(name);
//              }
//              else if (0 == bdlb::String::lowerCaseCmp(field, "sailor")) {
//                  addSailor(name);
//              }
//              else {
//                  cerr << "Unrecognized field '" << field << "' in line '" <<
//                                                               line << "'\n";
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
//      free(&d_captain);
//      free(&d_firstMate);
//
//      // Note that deallocating the strings will invalidate 'd_sailors' --
//      // any manipulation of 'd_sailors' other than destruction after this
//      // would lead to undefined behavior.
//
//      const NameSet::iterator end = d_sailors.end();
//      for (NameSet::iterator it = d_sailors.begin(); end != it; ++it) {
//          d_allocator_p->deallocate(const_cast<char *>(*it));
//      }
//  }
//..
// Then, we implement the public accessors:
//..
// ACCESSORS
//  const char *ShipsCrew::captain()
//  {
//      return d_captain ? d_captain : "";
//  }
//
//  const char *ShipsCrew::cook()
//  {
//      return d_cook ? d_cook : "";
//  }
//
//  const char *ShipsCrew::firstMate()
//  {
//      return d_firstMate ? d_firstMate : "";
//  }
//
//  const char *ShipsCrew::firstSailor()
//  {
//      NameSet::iterator it = d_sailors.begin();
//      return d_sailors.end() == it ? 0 : *it;
//  }
//
//  const char *ShipsCrew::nextSailor(const char *currentSailor)
//  {
//      assert(currentSailor);
//      NameSet::iterator it = d_sailors.find(currentSailor);
//      if (d_sailors.end() != it) {
//          ++it;
//      }
//      return d_sailors.end() == it ? 0 : *it;
//  }
//..
// Next, in 'main', we create our file './shipscrew.txt' describing the crew of
// the ship.  Note that the order of crew members is not important:
//..
//      {
//          bsl::ofstream outFile("shipscrew.txt");
//
//          outFile << "sailor:Mitch Sandler\n"
//                  << "sailor:Ben Lampert\n"
//                  << "cook:Bob Jones\n"
//                  << "captain:Steve Miller\n"
//                  << "sailor:Daniel Smith\n"
//                  << "first mate:Sally Chandler\n"
//                  << "sailor:Joe Owens\n";
//      }
//..
// Then, we set up a test case to test our 'ShipsCrew' class.  We do not use
// the stack trace test allocator yet, we just use a 'bslma::TestAllocator' to
// get memory usage statistics and determine whether any leakage occurred.
//..
//      {
//          bslma::TestAllocator ta("Bslma Test Allocator");
//          bslma::TestAllocatorMonitor tam(&ta);
//
//          {
//              ShipsCrew crew("shipscrew.txt", &ta);
//              assert(tam.isInUseUp());
//              assert(tam.isTotalUp());
//
//              if (verbose) {
//                  cout << "Captain: "  << crew.captain() <<
//                      "\nFirst Mate: " << crew.firstMate() <<
//                      "\nCook: "       << crew.cook() << endl;
//                  for (const char *sailor = crew.firstSailor(); sailor;
//                                          sailor = crew.nextSailor(sailor)) {
//                      cout << "Sailor: " << sailor << endl;
//                  }
//              }
//          }
//
//          int bytesLeaked = ta.numBytesInUse();
//          if (bytesLeaked > 0) {
//              cout << bytesLeaked << " bytes of memory were leaked!\n";
//          }
//      }
//..
// The program generates the following output in non-verbose mode, telling us
// that one segment of 10 bytes was leaked:
//..
//  10 bytes of memory were leaked!
//  MEMORY_LEAK from Bslma Test Allocator:
//  Number of blocks in use = 1
//  Number of bytes in use = 10
//..
// Next, we would like to use stack trace test allocator to tell us WHERE the
// memory leak is, but we have a problem: our test case not only uses
// 'bslma::TestAllocator', but it calls the 'numBytesInUse' accessor, which is
// not available from stack trace test allocator.  We are also using
// 'bslma::TestAllocatorMonitor', which will only work with
// 'bslma::TestAllocator'.  So if we were to just substitute the stack trace
// test allocator for the bslma test allocator, it would break our test case in
// several ways.  To instrument our test with a minimal change to the code, we
// create a stack test test allocator and feed that allocator to the
// constructor to bslma test allocator.  The rest of our example will now work
// without modification.  (Note that it is important to call
// 'ta.setNoAbort(true)' when we use this method, otherwise the bslma test
// allocator will bomb out before the destructor for 'stta' is able to generate
// its report).
//..
//      {
//          balst::StackTraceTestAllocator stta;
//          stta.setName("stta");
//          stta.setFailureHandler(&stta.failNoop);
//
//          bslma::TestAllocator ta("Bslma Test Allocator", &stta);
//          ta.setNoAbort(true);
//
//          // the rest of the test case after this is totally unchanged
//
//          bslma::TestAllocatorMonitor tam(&ta);
//
//          {
//              ShipsCrew crew("shipscrew.txt", &ta);
//              assert(tam.isInUseUp());
//              assert(tam.isTotalUp());
//
//              if (verbose) {
//                  cout << "Captain: "  << crew.captain() <<
//                      "\nFirst Mate: " << crew.firstMate() <<
//                      "\nCook: "       << crew.cook() << endl;
//                  for (const char *sailor = crew.firstSailor(); sailor;
//                                          sailor = crew.nextSailor(sailor)) {
//                      cout << "Sailor: " << sailor << endl;
//                  }
//              }
//          }
//
//          int bytesLeaked = ta.numBytesInUse();
//          if (bytesLeaked > 0) {
//              cout << bytesLeaked << " bytes of memory were leaked!\n";
//          }
//      }
//..
// Now, this generates the following report:
//..
//  10 bytes of memory were leaked!
//  MEMORY_LEAK from Bslma Test Allocator:
//    Number of blocks in use = 1
//     Number of bytes in use = 10
//  ===========================================================================
//  Error: memory leaked:
//  1 block(s) in allocator 'stta' in use.
//  Block(s) allocated from 1 trace(s).
//  ---------------------------------------------------------------------------
//  Allocation trace 1, 1 block(s) in use.
//  Stack trace at allocation time:
//  (0): BloombergLP::balst::StackTraceTestAllocator::allocate(int)+0x17d at 0x
//  805e741 in balst_stacktracetestallocator.t.dbg_exc_mt
//  (1): BloombergLP::bslma::TestAllocator::allocate(int)+0x12c at 0x8077398 in
//   balst_stacktracetestallocator.t.dbg_exc_mt
//  (2): ShipsCrew::copy(bsl::basic_string<char, std::char_traits<char>, bsl::a
//  llocator<char> > const&)+0x31 at 0x804c3db in balst_stacktracetestallocator
//  .t.dbg_exc_mt
//  (3): ShipsCrew::setCook(bsl::basic_string<char, std::char_traits<char>, bsl
//  ::allocator<char> > const&)+0x2d at 0x804c4c1 in balst_stacktracetestalloca
//  tor.t.dbg_exc_mt
//  (4): ShipsCrew::ShipsCrew(char const*, BloombergLP::bslma::Allocator*)+0x23
//  4 at 0x804c738 in balst_stacktracetestallocator.t.dbg_exc_mt
//  (5): main+0x53c at 0x804d55e in balst_stacktracetestallocator.t.dbg_exc_mt
//  (6): __libc_start_main+0xdc at 0x182e9c in /lib/libc.so.6
//  (7): --unknown-- at 0x804c1d1 in balst_stacktracetestallocator.t.dbg_exc_mt
//..
// Finally, Inspection shows that frame (3) of the stack trace from the
// allocation of the leaked segment was from 'ShipsCrew::setCook'.  Inspection
// of the code shows that we neglected to free 'd_cook' in the destructor and
// we can now easily fix our leak.

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BSLMT_MUTEX
#include <bslmt_mutex.h>
#endif

#ifndef INCLUDED_BDLMA_MANAGEDALLOCATOR
#include <bdlma_managedallocator.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSL_CSTDDEF
#include <bsl_cstddef.h>
#endif

#ifndef INCLUDED_BSL_FUNCTIONAL
#include <bsl_functional.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

namespace balst {
                       // =============================
                       // class StackTraceTestAllocator
                       // =============================

class StackTraceTestAllocator : public bdlma::ManagedAllocator {
    // This class defines a concrete "test" allocator mechanism that implements
    // the 'bdlma::ManagedAllocator' protocol, and provides instrumentation to
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
    // 'bslma::Allocator' protocol.

  public:
    // PUBLIC TYPES
    typedef bsl::function<void()> FailureHandler;
                // Type of functor called by this object to handle failures.
                // Note that this can be set and accessed using th
                // 'setFailureHandler' and 'failureHandler' methods
                // respectively.

  private:
    // PRIVATE TYPES
    enum AllocatorMagic { k_STACK_TRACE_TEST_ALLOCATOR_MAGIC = 1335775331 };

    struct BlockHeader;                            // information stored in
                                                   // each block (defined in
                                                   // .cpp)

    // DATA
    AllocatorMagic            d_magic;             // magic # to identify type
                                                   // of memory allocator

    volatile int              d_numBlocksInUse;    // number of allocated
                                                   // blocks currently unfreed

    BlockHeader              *d_blocks;            // list of allocated,
                                                   // unfreed blocks

    mutable bslmt::Mutex      d_mutex;             // mutex used to synchronize
                                                   // access to this object

    const char               *d_name;              // name of this allocator

    FailureHandler            d_failureHandler;    // function we are to call
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
    StackTraceTestAllocator(const StackTraceTestAllocator&);
    StackTraceTestAllocator& operator=(
                                         const StackTraceTestAllocator&);

  private:
    // PRIVATE ACCESSORS
    int checkBlockHeader(const BlockHeader *blockHdr) const;
        // Return 0 if the block specified by 'blockHdr' was allocated with
        // this allocator, is not corrupted, and has not yet been freed; report
        // diagnostics to '*d_ostream' and return a non-zero value otherwise.

  public:
    // CLASS METHODS
    static void failAbort();
        // Calls 'bsl::abort()', 'd_failureHandler' is initialized to this
        // value by all constructors.  Note that in ALL failure situations,
        // errors or warnings will be written to the 'ostream' associated with
        // this object prior to the failure handler call.

    static void failNoop();
        // Does nothing.  'setFailureHandler' may be called with this function,
        // in which case this allocator object, when a failure occurs, will
        // recover rather than abort.  Note that in ALL failure situations,
        // errors or warnings will be written to the 'ostream' associated with
        // this object prior to the failure handler call.

    // CREATORS
    explicit
    StackTraceTestAllocator(bslma::Allocator *basicAllocator = 0);
    explicit
    StackTraceTestAllocator(int               numRecordedFrames,
                                  bslma::Allocator *basicAllocator = 0);
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

    virtual ~StackTraceTestAllocator();
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
        // of the flag is 'true'.  However the flag is ignored on some
        // platforms; demangling never happens on some platforms and always
        // happens on others.

    void setFailureHandler(const FailureHandler& func);
        // Set the failure handler associated with this allocator object to the
        // specified 'func'.  Upon construction, the function 'failAbort' is
        // associated with this object by default.  Note that 'func' will be
        // called by this object's destructor if memory is leaked, so it is
        // important that it not throw.  Note that in ALL failure situations,
        // errors or warnings will be written to the 'ostream' associated with
        // this object prior to the call to the failure handler.

    void setName(const char * name);
        // Set the name of this allocator to the specified 'name'.  If
        // 'setName' is never called, the name of the allocator is "<unnamed>".
        // Note that the lifetime of 'name' must exceed the lifetime of this
        // object.

    void setOstream(bsl::ostream *ostream);
        // Set the stream to which diagnostics will be written to the specified
        // 'ostream'.  If 'setOstream' is never called, diagnostics will be
        // written to 'bsl::cerr'.

    // ACCESSORS
    const FailureHandler& failureHandler() const;
        // Return a reference to the function that will be called when a
        // failure is observered.

    bsl::size_t numBlocksInUse() const;
        // Return the number of blocks currently allocated from this object.

    void reportBlocksInUse(bsl::ostream *ostream = 0) const;
        // Write a report to the specified 'ostream', reporting the unique
        // call-stacks for each block that has been allocated and has not yet
        // been freed.  If 'ostream' is not specified, the value of 'ostream'
        // passed to the last call to 'setOstream' will be used.  If
        // 'setOstream' was never called, 'bsl::cerr' will be used.
};

                       // -----------------------------
                       // class StackTraceTestAllocator
                       // -----------------------------

inline
const StackTraceTestAllocator::FailureHandler&
StackTraceTestAllocator::failureHandler() const
{
    return d_failureHandler;
}

inline
bsl::size_t StackTraceTestAllocator::numBlocksInUse() const
{
    return d_numBlocksInUse;
}
}  // close package namespace

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
