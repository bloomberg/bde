// bdef_function.t.cpp             -*-C++-*-

#include <bdef_function.h>
#include <bdef_memfn.h>                         // for testing only

#include <bdema_bufferedsequentialallocator.h>  // for testing only

#include <bslalg_typetraits.h>                  // for testing only
#include <bslma_defaultallocatorguard.h>        // for testing only
#include <bslma_testallocator.h>                // for testing only
#include <bslma_testallocatorexception.h>       // for testing only
#include <bslmf_if.h>                           // for testing only

#include <bsls_alignmentutil.h>                 // for testing only
#include <bsls_platform.h>                      // for testing only
#include <bsls_stopwatch.h>

#include <bsl_iostream.h>
#include <bsl_map.h>
#include <bsl_string.h>

#include <bsl_c_stdlib.h>     // atoi()

#if defined(BSLS_PLATFORM__CMP_MSVC)
#pragma warning(disable: 4355) // we often use 'this' in member-initializers
#endif

using namespace BloombergLP;
using namespace bsl;  // automatically added by script


//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The 'bdef_Function' class owns the representation of an invocable, which is
// either a pointer to a function, a function object (if small enough to fit in
// the footprint of the 'bdef_Function'), or the address of a function object
// (also owned) if larger.  Our main objective is to make sure that the
// function object can be initialized with the different kinds of invocable and
// forwards invocation arguments properly.  Additionally, we wish to test
// thoroughly the (complex) internal mechanisms in the representation portion.
// In particular, the representation plays trick in order to do "type erasure"
// of the invocable:  since it does a good deal of creation/destruction if
// invocable types differ, but attempts to optimize those into assignment when
// the invocable type remains the same, there is a concern about leaking
// memory, properly forwarding allocator types, respecting the type of the
// invocable (despite the type erasure), etc.
//
// Although 'bdef_Function' is not value-semantic (because invocables are not
// required to possess equality comparison), we can make it so in the limited
// framework of this test driver.  This lets us reuse the more rigorous
// organization of test drivers for value-semantic test types, and in
// particular will address all the issues pertaining to the type-erased
// representation.  This, however, implies implementing the equality comparison
// operator within this test driver, which in turn requires all the invocable
// types in this test driver to be registered with a central authority.  We do
// so in the generating function.  The primary manipulators for 'bdef_Function'
// are the default constructor (with or without allocator), the
// 'operator=<FUNC>' assignment operator template, and the 'clear' method.  We
// have chosen for basic accessors the 'operator bool' conversion operator,
// invocation operator (of signature compatible with the type), and 'isInPlace'
// method.
//
// For the value-semantic portion of this driver, we will use a single
// 'PROTOTYPE' that takes an 'int' and returns an 'int'.  We will use for
// values invocables of different types, ordered by "rank", defined in
// increasing order as follows:
//   - rank 0: free function pointer
//   - rank 1: small function object (guaranteed to be represented in-place),
//             further subdivided in:
//                rank 1a: with pointer semantics
//                rank 1b: without pointer semantics
//   - rank 2: large function object (guaranteed to be represented
//             out-of-place), further subdivided in:
//                rank 2a: with pointer semantics
//                rank 2b: without pointer semantics
//-----------------------------------------------------------------------------
///bdef_FunctionUtil
///-----------------
// [ 1] bdef_FunctionUtil::MAX_INPLACE_OBJECT_SIZE
// [ 1] bdef_FunctionUtil::IsInplace
//
///bdef_Function
///-------------
// [ 2] bdef_Function();
// [11] bdef_Function<FUNC_OR_ALLOC>(const FUNC_OR_ALLOC& funcOrAlloc);
// [11] bdef_Function<FUNC>(const FUNC& func, bslma_Allocator *allocator);
// [ 7] bdef_Function(const bdef_Function& original);
// [ 9] bdef_Function::operator=(const bdef_Function& rhs);
// [ 2] bdef_Function::operator=(FUNC const&);
// [ 2] bdef_Function::clear();
// [12] bdef_Function::swap(bdef_Function<PROTOTYPE>&);
// [12] bdef_Function::load(FUNC, bslma_Allocator*);
// [12] bdef_Function::transferTo(bdef_Function<PROTOTYPE>*);
// [ 4] bdef_Function::operator()(...) const;
// [ 4] bdef_Function::operator bool() const;
// [ 4] bdef_Function::isInplace() const;
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] PRIMITIVE GENERATOR FUNCTION gg
// [ 6] EQUALITY COMPARISON
// [ 7] COPY CONSTRUCTOR
// [ 8] GENERATOR FUNCTION gg
// [13] BINDING TO A FUNCTION POINTER
// [14] BINDING TO A LARGE FUNCTION OBJECT
// [15] BINDING TO A SMALL FUNCTION OBJECT
// [16] BINDING TO A LARGE BITWISE-COPYABLE FUNCTION OBJECT
// [17] BINDING TO A SMALL BITWISE-COPYABLE FUNCTION OBJECT
// [18] BINDING TO A FUNCTION OBJECT WITH PTR SEMANTICS
// [19] USAGE EXAMPLE
//-----------------------------------------------------------------------------
// Note: In order to test the version of this component in BDE 1.15 and prior,
// define the macro BDEF_FUNCTION_PRIOR_1_16 (uncomment below):
//..
// #define BDEF_FUNCTION_PRIOR_1_16
//..

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
static int testStatus = 0;

namespace {

void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ cout << "\t" << flush;             // current Line number


//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef int (*Proto)(int);  // unless specified, default prototype

typedef bdef_Function<Proto> Obj;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR VALUE-SEMANTICS TESTING
//-----------------------------------------------------------------------------

                          // ===================
                          // class Function_Rep2
                          // ===================

struct Function_Rep2 {
    // This 'struct' *MUST* have the same layout as the component-private
    // 'bdef_Function_Rep' class.

    // DATA
    bdef_Function_Rep::ArenaType  d_arena;
    bdef_Function_Rep::Manager    d_manager_p;
    bslma_Allocator              *d_allocator_p;
};

                           // ================
                           // struct Function2
                           // ================

struct Function2 {
    // This 'struct' *MUST* have the same layout as the 'bdef_Function' class
    // template.

    Function_Rep2      d_rep;
    void               (*d_invoker_p)();
};

                       // =============================
                       // struct EqualityComparisonUtil
                       // =============================

struct EqualityComparisonUtil {

    // TYPES
     typedef bool (*EqualityManager)(const Function_Rep2 *lhs,
                                     const Function_Rep2 *rhs);
         // 'EqualityManager' is an alias for a boolean predicate comparing two
         // instances of 'Function_Rep2' passed by address.

  private:
    // CLASS DATA
    static bsl::map<bdef_Function_Rep::Manager, EqualityManager> d_managerMap;

  public:
    // CLASS METHODS
    template <class FUNC>
    static
    bool inplaceEqualityManager(const Function_Rep2 *lhs,
                                const Function_Rep2 *rhs)
        // Compare the invocable of the parameterized 'FUNC' type stored in the
        // specified 'lhs' and 'rhs' in-place representations.
    {
        return *(const FUNC *)&lhs->d_arena == *(const FUNC *)&rhs->d_arena;
    }

    template <class FUNC>
    static
    bool outofplaceEqualityManager(const Function_Rep2 *lhs,
                                   const Function_Rep2 *rhs)
        // Compare the invocable of the parameterized 'FUNC' type stored in the
        // specified 'lhs' and 'rhs' out-of-place representations.
    {
        return *(const FUNC *)lhs->d_arena.d_object_p ==
                                        *(const FUNC *)rhs->d_arena.d_object_p;
    }

    template <class FUNC>
    static int registerEqualityManager(const FUNC& = FUNC())
        // Register the parameterized 'FUNC' type, so that comparisons of
        // 'bdef_Function' objects storing instances of 'FUNC' might produce
        // the same result as comparing the stored 'FUNC' instances.
    {
        bdef_Function_Rep::Manager manager;
#ifndef BDE_FUNCTION_PRIOR_1_16
        enum {
            SIZE         = sizeof(FUNC)
          , BITWISE_MOVE = bslalg_HasTrait<FUNC,
                                        bslalg_TypeTraitBitwiseCopyable>::VALUE
          , BITWISE_COPY = bslalg_HasTrait<FUNC,
                                        bslalg_TypeTraitBitwiseCopyable>::VALUE
        };
        if (SIZE <= sizeof(bdef_Function_Rep::ArenaType)) {
            if (BITWISE_COPY) {
                manager = &bdef_Function_RepUtil::
                                      inplaceBitwiseCopyableManager<(int)SIZE>;
            } else if (BITWISE_MOVE) {
                manager = &bdef_Function_RepUtil::
                                      inplaceBitwiseMoveableManager<FUNC>;
            } else {
                manager = &bdef_Function_RepUtil::
                                      inplaceManager<FUNC>;
            }
            d_managerMap[manager] = &inplaceEqualityManager<FUNC>;
        }
        else {
            if (BITWISE_COPY) {
                manager = &bdef_Function_RepUtil::
                                   outofplaceBitwiseCopyableManager<(int)SIZE>;
            } else {
                manager = &bdef_Function_RepUtil::outofplaceManager<FUNC>;
            }
            d_managerMap[manager] = &outofplaceEqualityManager<FUNC>;
        }
#else
        manager = &bdef_Function_RepImpl<FUNC>::manager;
        if (sizeof(FUNC) <= sizeof(bdef_Function_Rep::ArenaType)) {
            d_managerMap[manager] = &inplaceEqualityManager<FUNC>;
        }
        else {
            d_managerMap[manager] = &outofplaceEqualityManager<FUNC>;
        }
#endif
        return 1;
    }

    static EqualityManager equalityManager(bdef_Function_Rep::Manager manager)
        // Return the equality comparison (manager) for two instances of
        // 'Function_Rep2' having the specified 'manager'.
    {
        return d_managerMap[manager];
    }
};

// CLASS DATA
bsl::map<bdef_Function_Rep::Manager, EqualityComparisonUtil::EqualityManager>
EqualityComparisonUtil::d_managerMap;

                         // ==========================
                         // bdef_Function 'operator=='
                         // ==========================

template <class PROTOTYPE>
bool operator==(const bdef_Function<PROTOTYPE>& lhs,
                const bdef_Function<PROTOTYPE>& rhs)
    // Return 'true' if the specified 'lhs' function object has the same value
    // as the specified 'rhs' function object.  Two function objects have the
    // same value if they store the same invocable (of the same type).
{
    const Function2& lhs2 = reinterpret_cast<const Function2&>(lhs);
    const Function2& rhs2 = reinterpret_cast<const Function2&>(rhs);

    if (lhs2.d_invoker_p != rhs2.d_invoker_p) {
        return false;
    }

    if (!lhs2.d_invoker_p) {
        return true;
    }

    if (lhs2.d_rep.d_manager_p != rhs2.d_rep.d_manager_p) {
        // ASSERT(lhs2.d_rep.d_manager_p == rhs2.d_rep.d_manager_p);
        return false;
    }

    if (!lhs2.d_rep.d_manager_p) {
        return lhs2.d_rep.d_arena.d_func_p == rhs2.d_rep.d_arena.d_func_p;
    }

    EqualityComparisonUtil::EqualityManager eqManager =
               EqualityComparisonUtil::equalityManager(lhs2.d_rep.d_manager_p);
    ASSERT(eqManager);

    return (*eqManager)(&lhs2.d_rep, &rhs2.d_rep);
}

template <class PROTOTYPE>
bool operator!=(const bdef_Function<PROTOTYPE>& lhs,
                const bdef_Function<PROTOTYPE>& rhs)
    // Return 'true' if the specified 'lhs' function object does not have the
    // same value as the specified 'rhs' function object.  Two function objects
    // do not have the same value if they store invocables of the different
    // type, or the invocable instances they store have different values.
{
    return !(lhs == rhs);
}

//=============================================================================
//                     GENERATED TYPES AND VALUES FOR TESTING
//-----------------------------------------------------------------------------

                               // =============
                               // return values
                               // =============

const int DEFAULT_VALUE = -1;
const int VF = 'F', VG = 'G', VH = 'H';
const int VU = 'U', VV = 'V', VW = 'W';
const int VS = 'S', VT = 'T';
const int VP = 'P', VQ = 'Q';
const int VL = 'L', VM = 'M';

                          // =======================
                          // function pointer values
                          // =======================

static int lastArgF = DEFAULT_VALUE;
static int lastArgG = DEFAULT_VALUE;
static int lastArgH = DEFAULT_VALUE;

int functionF(int value)
{
    lastArgF = value;
    return VF;
}

int functionG(int value)
{
    lastArgG = value;
    return VG;
}

int functionH(int value)
{
    lastArgH = value;
    return VH;
}

                            // ==================
                            // fussy object class
                            // ==================

template <int ID>
class Fussy {
    // This class protects the integrity of the classes that derive from it.
    // The intent is to detect double creations or destructions, when a
    // different type, or an instance of this same type, accidentally
    // overwrites this object.

    // DATA
    int    d_id;
    Fussy *d_this;

  public:
    // CREATORS
    Fussy()
    : d_id(ID)
    , d_this(this)
    {}

    Fussy(const Fussy&)
    : d_id(ID)
    , d_this(this)
    {
    }

    ~Fussy()
    {
        ASSERT(ID == d_id);
        ASSERT(this == d_this);
        d_id   = 0;
        d_this = 0;
    }

    // MANIPULATORS
    Fussy& operator=(const Fussy& rhs)
    {
        // Do nothing, on purpose.  In particular, we don't want to overwrite
        // 'd_this'.

        return *this;
    }
};
                            // ====================
                            // small object classes
                            // ====================

struct FunctorU : Fussy<VU> {
    // This 'struct' returns 'VU' when invoked.

    // CLASS DATA
    static int s_lastArg;

    // DATA
    int d_value;

    // MANIPULATORS
    int operator()(int value) {
        s_lastArg = value;
        return VU;
    }
};

int FunctorU::s_lastArg = DEFAULT_VALUE;

bool operator==(const FunctorU& lhs, const FunctorU& rhs)
{
    return lhs.d_value == rhs.d_value;
}

struct FunctorV {
    // This 'struct' returns 'VV' when invoked.

    // CLASS DATA
    static int s_lastArg;

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(FunctorV, bslalg_TypeTraitBitwiseCopyable);

    // DATA
    int d_value;

    // MANIPULATORS
    int operator()(int value) {
        s_lastArg = value;
        return VV;
    }
};

int FunctorV::s_lastArg = DEFAULT_VALUE;

bool operator==(const FunctorV& lhs, const FunctorV& rhs)
{
    return lhs.d_value == rhs.d_value;
}

struct FunctorW {
    // This 'struct' returns 'VW' when invoked.

    // CLASS DATA
    static int s_lastArg;

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(FunctorW, bslalg_TypeTraitBitwiseMoveable);

    // DATA
    int d_value;

    // MANIPULATORS
    int operator()(int value) {
        s_lastArg = value;
        return VW;
    }
};

int FunctorW::s_lastArg = DEFAULT_VALUE;

bool operator==(const FunctorW& lhs, const FunctorW& rhs)
{
    return lhs.d_value == rhs.d_value;
}

struct FunctorS : Fussy<VS> {
    // This 'struct' returns 'VS' when invoked.

    // CLASS DATA
    static int s_lastArg;

    // DATA
    int d_value;

    // MANIPULATORS
    int operator()(int value) {
        s_lastArg = value;
        return VS;
    }
};

int FunctorS::s_lastArg = DEFAULT_VALUE;

bool operator==(const FunctorS& lhs, const FunctorS& rhs)
{
    return lhs.d_value == rhs.d_value;
}

struct FunctorT {
    // This 'struct' returns 'VT' when invoked.

    // CLASS DATA
    static int s_lastArg;

    // DATA
    int d_value;

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(FunctorT, bslalg_TypeTraitBitwiseCopyable);

    // MANIPULATORS
    int operator()(int value) {
        s_lastArg = value;
        return VT;
    }
};

int FunctorT::s_lastArg = DEFAULT_VALUE;

bool operator==(const FunctorT& lhs, const FunctorT& rhs)
{
    return lhs.d_value == rhs.d_value;
}

                            // ====================
                            // large object classes
                            // ====================

struct FunctorP : Fussy<VP> {
    // This 'struct' returns 'VP' when invoked.

    // CLASS DATA
    static int s_lastArg;

    // DATA
    int  d_value;
    char d_padding[bdef_FunctionUtil::MAX_INPLACE_OBJECT_SIZE];

    // MANIPULATORS
    int operator()(int value) {
        s_lastArg = value;
        return VP;
    }
};

int FunctorP::s_lastArg = DEFAULT_VALUE;

bool operator==(const FunctorP& lhs, const FunctorP& rhs)
{
    return lhs.d_value == rhs.d_value;
}

struct FunctorQ {
    // This 'struct' returns 'VQ' when invoked.

    // CLASS DATA
    static int s_lastArg;

    // DATA
    int  d_value;
    char d_padding[bdef_FunctionUtil::MAX_INPLACE_OBJECT_SIZE];

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(FunctorQ, bslalg_TypeTraitBitwiseCopyable);

    // MANIPULATORS
    int operator()(int value) {
        s_lastArg = value;
        return VQ;
    }
};

int FunctorQ::s_lastArg = DEFAULT_VALUE;

bool operator==(const FunctorQ& lhs, const FunctorQ& rhs)
{
    return lhs.d_value == rhs.d_value;
}

struct FunctorL : Fussy<VL> {
    // This 'struct' returns 'VL' when invoked.

    // CLASS DATA
    static int s_lastArg;

    // DATA
    int  d_value;
    char d_padding[bdef_FunctionUtil::MAX_INPLACE_OBJECT_SIZE];

    // MANIPULATORS
    int operator()(int value) {
        s_lastArg = value;
        return VL;
    }
};

int FunctorL::s_lastArg = DEFAULT_VALUE;

bool operator==(const FunctorL& lhs, const FunctorL& rhs)
{
    return lhs.d_value == rhs.d_value;
}

struct FunctorM {
    // This 'struct' returns 'VM' when invoked.

    // CLASS DATA
    static int s_lastArg;

    // DATA
    int  d_value;
    char d_padding[bdef_FunctionUtil::MAX_INPLACE_OBJECT_SIZE];

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(FunctorM, bslalg_TypeTraitBitwiseCopyable);

    // MANIPULATORS
    int operator()(int value) {
        s_lastArg = value;
        return VM;
    }
};

int FunctorM::s_lastArg = DEFAULT_VALUE;

bool operator==(const FunctorM& lhs, const FunctorM& rhs)
{
    return lhs.d_value == rhs.d_value;
}

                        // ============================
                        // class PointerSemanticWrapper
                        // ============================

template <class FUNC>
class PointerSemanticWrapper {

    // PRIVATE TYPES
    enum {
        BITWISE = bslalg_HasTrait<FUNC, bslalg_TypeTraitBitwiseCopyable>::VALUE
    };
    typedef typename bslmf_If<BITWISE,
                              bslalg_TypeTraitBitwiseCopyable,
                              bslalg_TypeTraitNil>::Type    PassThroughBitwise;

    // DATA
    FUNC d_object;

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS2(PointerSemanticWrapper,
                                  bslalg_TypeTraitHasPointerSemantics,
                                  PassThroughBitwise);

    // CREATORS
    PointerSemanticWrapper() : d_object() {}
    PointerSemanticWrapper(const FUNC& func) : d_object(func) {}

    // MANIPULATORS
    FUNC& operator *() { return d_object; }

    // ACCESSORS
    const FUNC& operator *() const { return d_object; }
};

// FREE OPERATORS
template <class FUNC>
bool operator==(const PointerSemanticWrapper<FUNC>& lhs,
                const PointerSemanticWrapper<FUNC>& rhs)
{
    return *lhs == *rhs;
}

template <class FUNC>
bool operator!=(const PointerSemanticWrapper<FUNC>& lhs,
                const PointerSemanticWrapper<FUNC>& rhs)
{
    return !(lhs == rhs);
}

// HELPER FUNCTION
template <class FUNC>
PointerSemanticWrapper<FUNC> pointerWrapper(const FUNC& func)
{
    return PointerSemanticWrapper<FUNC>(func);
}

                        // =======================
                        // class RawPointerWrapper
                        // =======================

template <class FUNC>
class RawPointerWrapper {

    // DATA
    FUNC *d_object_p;  // held, not owned

    // FRIENDS
    template <class FUNC2>
    friend bool operator==(const RawPointerWrapper<FUNC2>& lhs,
                           const RawPointerWrapper<FUNC2>& rhs);

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(RawPointerWrapper,
                                 bslalg_TypeTraitHasPointerSemantics);

    // CREATORS
    RawPointerWrapper() : d_object_p(0) {}
    RawPointerWrapper(FUNC *funcPtr) : d_object_p(funcPtr) {}

    // ACCESSORS
    FUNC& operator *() const { return *d_object_p; }
};

// FREE OPERATORS
template <class FUNC>
bool operator==(const RawPointerWrapper<FUNC>& lhs,
                const RawPointerWrapper<FUNC>& rhs)
{
    return lhs.d_object_p == rhs.d_object_p;
}

template <class FUNC>
bool operator!=(const RawPointerWrapper<FUNC>& lhs,
                const RawPointerWrapper<FUNC>& rhs)
{
    return !(lhs == rhs);
}

// HELPER FUNCTION
template <class FUNC>
RawPointerWrapper<FUNC> rawPointerWrapper(FUNC *funcPtr)
{
    return RawPointerWrapper<FUNC>(funcPtr);
}

                        // =========================
                        // class RawBCPointerWrapper
                        // =========================

template <class FUNC>
struct RawBCPointerWrapper : public RawPointerWrapper<FUNC> {

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS2(RawBCPointerWrapper,
                                  bslalg_TypeTraitHasPointerSemantics,
                                  bslalg_TypeTraitBitwiseCopyable);

    // CREATORS
    RawBCPointerWrapper() : RawPointerWrapper<FUNC>() {}
    RawBCPointerWrapper(FUNC *funcPtr) : RawPointerWrapper<FUNC>(funcPtr) {}
};

// HELPER FUNCTION
template <class FUNC>
RawBCPointerWrapper<FUNC> rawBCPointerWrapper(FUNC *funcPtr)
{
    return RawBCPointerWrapper<FUNC>(funcPtr);
}

                            // ====================
                            // function testBinding
                            // ====================

int testBinding(const Obj& functor)
{
    return functor(1);
}

//=============================================================================
//              GENERATOR FUNCTIONS 'g' AND 'gg' FOR TESTING
//-----------------------------------------------------------------------------
// The following functions interpret the given 'spec' in order from left to
// right to configure the object according to a custom language.  Selected
// uppercase letters in [A .. Z] correspond to arbitrary (but unique) functor
// types (and for some types, values in [0 .. 9]) to be assigned to the
// 'bdef_Function<PROTOTYPE>' object.  A tilde ('~') indicates that the logical
// (but not necessarily physical) state of the object is to be set to its
// initial, empty state (via the 'clear' method).  For readability, allow
// spaces or semicolons to separate elements.
//
// LANGUAGE SPECIFICATION:
// -----------------------
//
// <SPEC>       ::= <EMPTY>   | <LIST>
//
// <EMPTY>      ::=
//
// <LIST>       ::= <ITEM>    | <ITEM><LIST>
//
// <ITEM>       ::= <POINTER> | <ELEMENT><VALUE> | <REMOVE_ALL>
//
// <POINTER>    ::= 'F' | 'G' | 'H'          // function pointers
//
// <ELEMENT>    ::= 'U' | 'V' | 'S' | 'T' |  // small function objects
//                  'P' | 'Q' | 'L' | 'M' |  // large function objects
//
// <VALUE>      ::= '0' .. '9'
//
// <REMOVE_ALL> ::= '~'
//
// Spec String  Description
// -----------  ---------------------------------------------------------------
// ""           Has no effect; leaves the object empty.
// "F"          Assigns the value corresponding to F.
// "FF"         Assigns two values both corresponding to F.
// "FGH"        Assigns three values corresponding to F, G and H in that order.
// "U1V2F~"     Assign three values corresponding to U(1), V(2) and F and then
//              remove all the elements (set function pointer to 0).  Note that
//              this spec yields an object that is logically equivalent
//              (but not necessarily identical internally) to one
//              yielded by ("").
// "U1V2F~P3L4" Assign three values corresponding to U(1), V(2), and F; empty
//              the object; and assign values corresponding to P(3) and L(4).
//
//-----------------------------------------------------------------------------

                            // ====================
                            // generating functions
                            // ====================

int getValue(int *i, const char *spec, int verboseFlag)
{
    ++(*i);
    if (spec[*i]) {
        if ('0' <= spec[*i] && spec[*i] <= '9') {
            return (int)(spec[*i] - '0');
        }
        else {
            if (verboseFlag) {
                cout << "Error, bad object value ('" << spec[*i] << "')"
                     << " in spec \"" << spec << "\""
                     << " at position " << *i << '.' << endl;
            }
            return -*i;
        }
    }

    if (verboseFlag) {
        cout << "Error, premature end of line"
             << " in spec \"" << spec << "\""
             << " at position " << *i << '.' << endl;
    }
    return -*i;
}

template <class PROTOTYPE>
int
ggg(bdef_Function<PROTOTYPE> *object, const char *spec, int verboseFlag = 0)
    // Configure the specified 'object' according to the specified 'spec',
    // using only the primary manipulator function 'operator=<FUNC>' and
    // white-box manipulator 'clear'.  Optionally specify a zero 'verboseFlag'
    // to suppress 'spec' syntax error messages.  Return the index of the first
    // invalid character, and a negative value otherwise.  Note that this
    // function is used to implement 'gg' as well as allow for verification of
    // syntax error detection.
{
    enum { SUCCESS = -1 };
    for (int i = 0; spec[i]; ++i) {
        if ('A' <= spec[i] && spec[i] <= 'Z') {
            switch (spec[i]) {
              case 'F': {  // rank 0, function ptr
                *object = &functionF;
                // EqualityComparisonUtil::registerEqualityManager(&functionF);
              } break;
              case 'G': {  // rank 0, function ptr
                *object = &functionG;
                // EqualityComparisonUtil::registerEqualityManager(&functionG);
              } break;
              case 'H': {  // rank 0, function ptr
                *object = &functionH;
                // EqualityComparisonUtil::registerEqualityManager(&functionH);
              } break;
              case 'U': {  // rank 1a, small object, pointer semantics
                FunctorU mU;
                int value = getValue(&i, spec, verboseFlag);
                if (value < 0) {
                    return i;
                }
                mU.d_value = value;
                *object = pointerWrapper(mU);
                EqualityComparisonUtil::registerEqualityManager(
                                                           pointerWrapper(mU));
              } break;
              case 'V': {  // rank 1a, small object, pointer semantics
                FunctorV mV;
                int value = getValue(&i, spec, verboseFlag);
                if (value < 0) {
                    return i;
                }
                mV.d_value = value;
                *object = pointerWrapper(mV);
                EqualityComparisonUtil::registerEqualityManager(
                                                           pointerWrapper(mV));
              } break;
              case 'W': {  // rank 1a, small object, pointer semantics
                FunctorW mW;
                int value = getValue(&i, spec, verboseFlag);
                if (value < 0) {
                    return i;
                }
                mW.d_value = value;
                *object = pointerWrapper(mW);
                EqualityComparisonUtil::registerEqualityManager(
                                                           pointerWrapper(mW));
              } break;
              case 'S': {  // rank 1b, small object, no pointer semantics
                FunctorS mS; const FunctorS& S = mS;
                int value = getValue(&i, spec, verboseFlag);
                if (value < 0) {
                    return i;
                }
                mS.d_value = value;
                *object = S;
                EqualityComparisonUtil::registerEqualityManager(mS);
              } break;
              case 'T': {  // rank 1b, small object, no pointer semantics
                FunctorT mT; const FunctorT& T = mT;
                int value = getValue(&i, spec, verboseFlag);
                if (value < 0) {
                    return i;
                }
                mT.d_value = value;
                *object = T;
                EqualityComparisonUtil::registerEqualityManager(mT);
              } break;
              case 'P': {  // rank 2a, large object, pointer semantics
                FunctorP mP;
                int value = getValue(&i, spec, verboseFlag);
                if (value < 0) {
                    return i;
                }
                mP.d_value = value;
                *object = pointerWrapper(mP);
                EqualityComparisonUtil::registerEqualityManager(
                                                           pointerWrapper(mP));
              } break;
              case 'Q': {  // rank 2a, large object, pointer semantics
                FunctorQ mQ;
                int value = getValue(&i, spec, verboseFlag);
                if (value < 0) {
                    return i;
                }
                mQ.d_value = value;
                *object = pointerWrapper(mQ);
                EqualityComparisonUtil::registerEqualityManager(
                                                           pointerWrapper(mQ));
              } break;
              case 'L': {  // rank 2b, large object, no pointer semantics
                FunctorL mL; const FunctorL& L = mL;
                int value = getValue(&i, spec, verboseFlag);
                if (value < 0) {
                    return i;
                }
                mL.d_value = value;
                *object = L;
                EqualityComparisonUtil::registerEqualityManager(L);
              } break;
              case 'M': {  // rank 2b, large object, no pointer semantics
                FunctorM mM; const FunctorM& M = mM;
                int value = getValue(&i, spec, verboseFlag);
                if (value < 0) {
                    return i;
                }
                mM.d_value = value;
                *object = M;
                EqualityComparisonUtil::registerEqualityManager(M);
              } break;
              default:
                if (verboseFlag) {
                    cout << "Error, bad function object ('" << spec[i] << "')"
                         << " in spec \"" << spec << "\""
                         << " at position " << i << '.' << endl;
                }
                return i;  // Discontinue processing this spec.
            }
        }
        else if ('~' == spec[i]) {
            object->clear();
        }
        else if (' ' == spec[i] || ';' == spec[i]) {
            // do nothing, allowed for readability
        }
        else {
            if (verboseFlag) {
                cout << "Error, bad character ('" << spec[i] << "') in spec \""
                     << spec << "\" at position " << i << '.' << endl;
            }
            return i;  // Discontinue processing this spec.
        }
   }
   return SUCCESS;
}

template <class PROTOTYPE>
bdef_Function<PROTOTYPE>& gg(bdef_Function<PROTOTYPE> *object,
                             const char               *spec)
    // Return, by reference, the specified object with its value adjusted
    // according to the specified 'spec'.
{
    ASSERT(ggg(object, spec) < 0);
    return *object;
}

template <class PROTOTYPE>
bdef_Function<PROTOTYPE> g(const char *spec)
    // Return, by value, a new object corresponding to the specified 'spec'.
{
    bdef_Function<PROTOTYPE> object((bslma_Allocator *)0);
    return gg(&object, spec);
}

template <class PROTOTYPE>
void stretchRemoveAll(bdef_Function<PROTOTYPE> *object, const char *perturb)
{
    ASSERT(ggg(object, perturb) < 0);
    object->clear();
}

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

void add5(int* value)
{
    (*value) += 5;
}

void subtract7(int* value)
{
    (*value) -= 7;
}

                            // ===================
                            // functions freeFunc*
                            // ===================

int freeFunc0()
{
    return 0;
}

int freeFunc1(int v1)
{
    ASSERT(1==v1);
    return 1;
}

int freeFunc2(int v1, int v2 )
{
    ASSERT(1==v1);
    ASSERT(2==v2);
    return 2;
}

int freeFunc3(int v1, int v2, int v3 )
{
    ASSERT(1==v1);
    ASSERT(2==v2);
    ASSERT(3==v3);
    return 3;
}

int freeFunc4(int v1, int v2, int v3, int v4 )
{
    ASSERT(1==v1);
    ASSERT(2==v2);
    ASSERT(3==v3);
    ASSERT(4==v4);
    return 4;
}

int freeFunc5(int v1, int v2, int v3, int v4, int v5 )
{
    ASSERT(1==v1);
    ASSERT(2==v2);
    ASSERT(3==v3);
    ASSERT(4==v4);
    ASSERT(5==v5);
    return 5;
}

int freeFunc6(int v1, int v2, int v3, int v4, int v5, int v6 )
{
    ASSERT(1==v1);
    ASSERT(2==v2);
    ASSERT(3==v3);
    ASSERT(4==v4);
    ASSERT(5==v5);
    ASSERT(6==v6);
    return 6;
}

int freeFunc7(int v1, int v2, int v3, int v4, int v5, int v6, int v7 )
{
    ASSERT(1==v1);
    ASSERT(2==v2);
    ASSERT(3==v3);
    ASSERT(4==v4);
    ASSERT(5==v5);
    ASSERT(6==v6);
    ASSERT(7==v7);
    return 7;
}

int freeFunc8(int v1, int v2, int v3, int v4, int v5, int v6, int v7, int v8 )
{
    ASSERT(1==v1);
    ASSERT(2==v2);
    ASSERT(3==v3);
    ASSERT(4==v4);
    ASSERT(5==v5);
    ASSERT(6==v6);
    ASSERT(7==v7);
    ASSERT(8==v8);
    return 8;
}

int freeFunc9(int v1, int v2, int v3, int v4, int v5, int v6, int v7, int v8,
              int v9)
{
    ASSERT(1==v1);
    ASSERT(2==v2);
    ASSERT(3==v3);
    ASSERT(4==v4);
    ASSERT(5==v5);
    ASSERT(6==v6);
    ASSERT(7==v7);
    ASSERT(8==v8);
    ASSERT(9==v9);
    return 9;
}

int freeFunc10(int v1, int v2, int v3, int v4, int v5, int v6, int v7, int v8,
               int v9, int v10)
{
    ASSERT(1==v1);
    ASSERT(2==v2);
    ASSERT(3==v3);
    ASSERT(4==v4);
    ASSERT(5==v5);
    ASSERT(6==v6);
    ASSERT(7==v7);
    ASSERT(8==v8);
    ASSERT(9==v9);
    ASSERT(10==v10);
    return 10;
}

int freeFunc11(int v1, int v2, int v3, int v4, int v5, int v6, int v7, int v8,
               int v9, int v10, int v11)
{
    ASSERT(1==v1);
    ASSERT(2==v2);
    ASSERT(3==v3);
    ASSERT(4==v4);
    ASSERT(5==v5);
    ASSERT(6==v6);
    ASSERT(7==v7);
    ASSERT(8==v8);
    ASSERT(9==v9);
    ASSERT(10==v10);
    ASSERT(11==v11);
    return 11;
}

int freeFunc12(int v1, int v2, int v3, int v4, int v5, int v6, int v7, int v8,
               int v9, int v10, int v11, int v12)
{
    ASSERT(1==v1);
    ASSERT(2==v2);
    ASSERT(3==v3);
    ASSERT(4==v4);
    ASSERT(5==v5);
    ASSERT(6==v6);
    ASSERT(7==v7);
    ASSERT(8==v8);
    ASSERT(9==v9);
    ASSERT(10==v10);
    ASSERT(11==v11);
    ASSERT(12==v12);
    return 12;
}

int freeFunc13(int v1, int v2, int v3, int v4, int v5, int v6, int v7, int v8,
               int v9, int v10, int v11, int v12, int v13)
{
    ASSERT(1==v1);
    ASSERT(2==v2);
    ASSERT(3==v3);
    ASSERT(4==v4);
    ASSERT(5==v5);
    ASSERT(6==v6);
    ASSERT(7==v7);
    ASSERT(8==v8);
    ASSERT(9==v9);
    ASSERT(10==v10);
    ASSERT(11==v11);
    ASSERT(12==v12);
    ASSERT(13==v13);
    return 13;
}

int freeFunc14(int v1, int v2, int v3, int v4, int v5, int v6, int v7, int v8,
               int v9, int v10, int v11, int v12, int v13, int v14)
{
    ASSERT(1==v1);
    ASSERT(2==v2);
    ASSERT(3==v3);
    ASSERT(4==v4);
    ASSERT(5==v5);
    ASSERT(6==v6);
    ASSERT(7==v7);
    ASSERT(8==v8);
    ASSERT(9==v9);
    ASSERT(10==v10);
    ASSERT(11==v11);
    ASSERT(12==v12);
    ASSERT(13==v13);
    ASSERT(14==v14);
    return 14;
}

                           // ======================
                           // class MyFunctionObject
                           // ======================

class MyFunctionObject {

    // DATA
    int d_v1;
    int d_v2;
    int d_v3;
    int d_v4;
    int d_v5;
    int d_v6;
    int d_v7;
    int d_v8;
    int d_v9;
    int d_v10;
    int d_v11;
    int d_v12;
    int d_v13;
    int d_v14;

    // For making sure this object is not in place, used in case 1.
    int d_padding[bdef_FunctionUtil::MAX_INPLACE_OBJECT_SIZE];

  public:
    // CREATORS
    // None on purpose.

    // MANIPULATORS
    int operator()()
    {
        return 0;
    }

    int operator()(int v1)
    {
        ASSERT(1==v1);
        return 1;
    }

    int operator()(int v1, int v2 )
    {
        ASSERT(1==v1);
        ASSERT(2==v2);
        return 2;
    }

    int operator()(int v1, int v2, int v3 )
    {
        ASSERT(1==v1);
        ASSERT(2==v2);
        ASSERT(3==v3);
        return 3;
    }

    int operator()(int v1, int v2, int v3, int v4 )
    {
        ASSERT(1==v1);
        ASSERT(2==v2);
        ASSERT(3==v3);
        ASSERT(4==v4);
        return 4;
    }

    int operator()(int v1, int v2, int v3, int v4, int v5 )
    {
        ASSERT(1==v1);
        ASSERT(2==v2);
        ASSERT(3==v3);
        ASSERT(4==v4);
        ASSERT(5==v5);
        return 5;
    }

    int operator()(int v1, int v2, int v3, int v4, int v5,
                   int v6 )
    {
        ASSERT(1==v1);
        ASSERT(2==v2);
        ASSERT(3==v3);
        ASSERT(4==v4);
        ASSERT(5==v5);
        ASSERT(6==v6);
        return 6;
    }

    int operator()(int v1, int v2, int v3, int v4, int v5,
                   int v6, int v7 )
    {
        ASSERT(1==v1);
        ASSERT(2==v2);
        ASSERT(3==v3);
        ASSERT(4==v4);
        ASSERT(5==v5);
        ASSERT(6==v6);
        ASSERT(7==v7);
        return 7;
    }

    int operator()(int v1, int v2, int v3, int v4, int v5,
                   int v6, int v7, int v8 )
    {
        ASSERT(1==v1);
        ASSERT(2==v2);
        ASSERT(3==v3);
        ASSERT(4==v4);
        ASSERT(5==v5);
        ASSERT(6==v6);
        ASSERT(7==v7);
        ASSERT(8==v8);
        return 8;
    }

    int operator()(int v1, int v2, int v3, int v4, int v5,
                                  int v6, int v7, int v8, int v9)
    {
        ASSERT(1==v1);
        ASSERT(2==v2);
        ASSERT(3==v3);
        ASSERT(4==v4);
        ASSERT(5==v5);
        ASSERT(6==v6);
        ASSERT(7==v7);
        ASSERT(8==v8);
        ASSERT(9==v9);
        return 9;
    }

    int operator()(int v1, int v2, int v3, int v4, int v5,
                                   int v6, int v7, int v8,int v9, int v10)
    {
        ASSERT(1==v1);
        ASSERT(2==v2);
        ASSERT(3==v3);
        ASSERT(4==v4);
        ASSERT(5==v5);
        ASSERT(6==v6);
        ASSERT(7==v7);
        ASSERT(8==v8);
        ASSERT(9==v9);
        ASSERT(10==v10);
        return 10;
    }

    int operator()(int v1, int v2, int v3, int v4, int v5, int v6, int v7,
                   int v8, int v9, int v10, int v11)
    {
        ASSERT(1==v1);
        ASSERT(2==v2);
        ASSERT(3==v3);
        ASSERT(4==v4);
        ASSERT(5==v5);
        ASSERT(6==v6);
        ASSERT(7==v7);
        ASSERT(8==v8);
        ASSERT(9==v9);
        ASSERT(10==v10);
        ASSERT(11==v11);
        return 11;
    }

    int operator()(int v1, int v2, int v3, int v4, int v5, int v6, int v7,
                   int v8, int v9, int v10, int v11, int v12)
    {
        ASSERT(1==v1);
        ASSERT(2==v2);
        ASSERT(3==v3);
        ASSERT(4==v4);
        ASSERT(5==v5);
        ASSERT(6==v6);
        ASSERT(7==v7);
        ASSERT(8==v8);
        ASSERT(9==v9);
        ASSERT(10==v10);
        ASSERT(11==v11);
        ASSERT(12==v12);
        return 12;
    }

    int operator()(int v1, int v2, int v3, int v4, int v5, int v6, int v7,
                   int v8, int v9, int v10, int v11, int v12, int v13)
    {
        ASSERT(1==v1);
        ASSERT(2==v2);
        ASSERT(3==v3);
        ASSERT(4==v4);
        ASSERT(5==v5);
        ASSERT(6==v6);
        ASSERT(7==v7);
        ASSERT(8==v8);
        ASSERT(9==v9);
        ASSERT(10==v10);
        ASSERT(11==v11);
        ASSERT(12==v12);
        ASSERT(13==v13);
        return 13;
    }

    int operator()(int v1, int v2, int v3, int v4, int v5, int v6, int v7,
                   int v8, int v9, int v10, int v11, int v12, int v13, int v14)
    {
        ASSERT(1==v1);
        ASSERT(2==v2);
        ASSERT(3==v3);
        ASSERT(4==v4);
        ASSERT(5==v5);
        ASSERT(6==v6);
        ASSERT(7==v7);
        ASSERT(8==v8);
        ASSERT(9==v9);
        ASSERT(10==v10);
        ASSERT(11==v11);
        ASSERT(12==v12);
        ASSERT(13==v13);
        ASSERT(14==v14);
        return 14;
    }

    int memFunc0() {
        return 0;
    }
};

typedef PointerSemanticWrapper<MyFunctionObject> TestWrapper;

struct MyBCFunctionObject : public MyFunctionObject {

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(MyBCFunctionObject,
                                 bslalg_TypeTraitBitwiseCopyable);
};

                           // ==========================
                           // class MySizeFunctionObject
                           // ==========================

template <int SIZE>
struct MySizeFunctionObject {

    // DATA
    char padding[SIZE];

    // MANIPULATORS
    int operator()() {
        return SIZE;
    }
};

extern "C" int externCIdentityFunction(int value)
{
    return value;
}

//=============================================================================
//                 USAGE EXAMPLE CLASSES AND FUNCTIONS
//-----------------------------------------------------------------------------
namespace TEST_CASE_BASSVC {

struct SharedPtr {};
namespace bassvc {
    struct ServiceManifest {};
}
struct Aggregate {
    int d_value;
};

extern "C"
int oldsvc_Entry__createService(
                SharedPtr               *requestRouter,
                bassvc::ServiceManifest *manifest,
                const Aggregate&         configuration,
                bslma_Allocator         *basicAllocator)
{
    return configuration.d_value;
}

struct InProcessServiceManager {

    typedef bdef_Function<int (*)(SharedPtr               *requestRouter,
                                  bassvc::ServiceManifest *manifest,
                                  const Aggregate&         configuration,
                                  bslma_Allocator         *basicAllocator)>
                                                     CreateBlobServiceCallback;

    int registerBlobRouter(const char                       *name,
                           int                               serviceId,
                           const CreateBlobServiceCallback&  createServiceCb)
    {
        SharedPtr               requestRouter;
        bassvc::ServiceManifest manifest;
        Aggregate               configuration;
        bslma_TestAllocator     ta;
        configuration.d_value = 0;
        ASSERT(0   == createServiceCb(&requestRouter,
                                      &manifest,
                                      configuration,
                                      &ta));
        configuration.d_value = 123;
        ASSERT(123 == createServiceCb(&requestRouter,
                                      &manifest,
                                      configuration,
                                      &ta));
        return 0;
    }
};

}  // namespace  TEST_CASE_BASSVC
//=============================================================================
//                 USAGE EXAMPLE CLASSES AND FUNCTIONS
//-----------------------------------------------------------------------------
namespace USAGE_EXAMPLE_NAMESPACE {

///Declaring a Functor
///-------------------
// The 'bdef_Function' functor takes a single c-style function pointer template
// argument that describes the prototype of the functor.  For example, the
// declaration of a functor that return a 'void' result and accept no
// arguments, would look as follows:
//..
    bdef_Function< void (*)() > voidFunctor;
//..
// to declare a functor the returns 'int' and accepts two 'double' arguments:
//..
    bdef_Function< int (*)(double, double) > intFunctor;
//..
//
///Initializing a functor
///----------------------
// That is, for any object 'F',
// that the expression "F(X,Y)" is legal, then F can be represented by
// any functor that accepts two arguments of compatible 'X', and 'Y'
// respectively.  For example, given the following functor 'FUNC':
//..
    typedef bdef_Function<void(*)(int, const char*)> FUNC;
//..
// Any of the following object my be assigned to an invoked through a 'FUNC'
// object:
//..
    void freeFunction(int, const char*) {}

    int freeFunctionNonVoid(int, const char*) { return 0; }

    void freeFunctionCompatibleArgs(double, const char*) {}

    class FunctionObject {
        // ....
      public:
        void operator() (int , const char*) {}
    };

    class FunctionObject2 {
        // ....
      public:
        void operator() (int , bsl::string) {}
    };

    void usageExampleTest1()
    {
       FUNC func;

       func = &freeFunction;
       func(100, "It works");

       func = &freeFunctionNonVoid;
       func(100, "Still works - 'int' result discarded");

       func = &freeFunctionCompatibleArgs;
       func(100, "Works too - 100 promoted to (double)100.0");

       FunctionObject o;
       func = o;
       func(100, "This works too!!!!!");

       FunctionObject2 o2;
       func = o2;
       func(100, "This also works - 'bsl::string' temporary created!");
    }
//..
// Note that member function pointers cannot be directly invoked by a functor
// since for a given function pointer 'p' to a member function on the object
// 'o' that accepts two arguments X, and Y, the invocation syntax is
// 'o.*p(X,Y)' which is syntactically different for 'p(X,Y)'.  The 'bdef_MemFn'
// component provides an adapter to allow member function pointers to be
// invoked through the functors.  For example:
//..
    class MyClass {
      public:
        void myMember(int , const char*) {}
    };

    void usageExampleTest2()
    {
        MyClass o;
        FUNC    func;

        // func = &MyClass::myMember;  //  ILLEGAL!!!!
        // func(100, "Can't work -- On which instance anyway???");

        func = bdef_MemFnUtil::memFn(&MyClass::myMember, &o); // LEGAL
        func(100, "This works -- on instance 'o'");
    }
//..
// Note that static member functions can be directly invoked through functors
// since they can be syntactically invoked in the same manner as free
// functions.
//
///Invoking the functor
///--------------------
// Once initialized, functors can be invoked in the same way as regular
// function pointers.  For example, to invoke the following functor, simply
// call it with the appropriate arguments:
//..
    int dummy(double, double) {
        return 0;
    }

    void usageExampleTest3() {
        bdef_Function< int(*)(double,double) > func;
        // initialize 'func'
        func = &dummy;
        // Now call func
        int result = func(3.14, 100.3);
        (void)result; // warning
    }
//..
//
///Using functors in place of virtual functions
///--------------------------------------------
// For small interfaces, functors can often be used in place of virtual
// functions, reducing the need for inheritance.  Take for example the
// following class that reads data from a stream, and invokes a user-provided
// object for each message extracted from the stream:
//..
    struct Message {
        // ...  // message body
        operator const char*() const { return ""; }
    };

    class MessageHandler {
      public:
        virtual ~MessageHandler() {}
        virtual void processMessage(Message const &msg) {}
    };

    class MessageProcessor {
        MessageHandler *d_handler_p;
        int extractMessage(Message *messageBuffer, bsl::istream &stream);
      public:
        MessageProcessor(MessageHandler *handler);

        void processMessages(bsl::istream &stream);
    };

    int MessageProcessor::extractMessage(Message *messageBuffer,
                                         bsl::istream &stream)
    {
        // Read data from the stream and fill 'messageBuffer'
        // ...
        return 1;
    }

    class PrintOnlyHandler : public MessageHandler {
      public:
        virtual ~PrintOnlyHandler() {
        }
        void processMessage(Message const&msg)
        {
           bsl::cout << msg << bsl::endl;
        }
    };

    class ActualHandler : public MessageHandler {
      public:
        virtual ~ActualHandler() {
        }
        void processMessage(Message const&msg)
        {
           // Do some stuff!!
        }
    };

    MessageProcessor::MessageProcessor(MessageHandler *handler)
    : d_handler_p(handler)
    {
    }

    void MessageProcessor::processMessages(bsl::istream &stream)
    {
        int ret;
        do {
            Message msg;
            ret = extractMessage(&msg, stream);
            if (!ret) {
                d_handler_p->processMessage(msg);
            }
        } while (!ret);
    }

    void processMessages(bool printOnly, bsl::istream &stream)
    {
        MessageHandler *handler;
        if (printOnly) {
           handler = new PrintOnlyHandler();
        }
        else {
           handler = new ActualHandler();
        }
        MessageProcessor processor(handler);
        processor.processMessages(stream);
    }
//..
// In the above example, 'MessageProcessor' requires that a concrete instance
// of the 'MessageHandler' protocol must be provided in order to process the
// messages.  This means that regardless of how trivial the implementation is,
// a class must be derived to implement that 'processMessage' function.  That
// also means that the class must be instantiated in order to provide it to
// 'MessageProcessor', possibly requiring memory allocation .  In the following
// alternate implementation, the 'MessageHandler' protocol has been replace
// with a single functor.
//..
    class MessageProcessor2 {
      public:
        typedef bdef_Function<void(*)(Message const&)> MessageHandler;
      private:
        MessageHandler d_handler;
        int extractMessage(Message *messageBuffer, bsl::istream &stream);
      public:
        MessageProcessor2(MessageHandler handler);

        void processMessages(bsl::istream &stream);
    };

    MessageProcessor2::MessageProcessor2(MessageHandler handler)
    : d_handler(handler)
    {
    }

    int MessageProcessor2::extractMessage(Message *messageBuffer,
                                          bsl::istream &stream)
    {
        return 1;
    }

    void MessageProcessor2::processMessages(bsl::istream &stream)
    {
        int ret;
        do {
            Message msg;
            ret = extractMessage(&msg, stream);
            if (!ret) d_handler(msg);
        } while (!ret);
    }

    void processMessage2(const Message &)
    {
       // Do some stuff!!
    }

    void printMessage2(const Message &msg)
    {
        bsl::cout << msg << bsl::endl;
    }

    void processMessages2(bool printOnly, bsl::istream &stream)
    {
        MessageProcessor2::MessageHandler handler;
        handler = printOnly ? &printMessage2 : &processMessage2;
        MessageProcessor2 processor(handler);
        processor.processMessages(stream);
    }
//..

} // closing namespace USAGE_EXAMPLE_NAMESPACE

void testCase19(int argc)
{
    // --------------------------------------------------------------------
    // TESTING BINDING TO A 'bdef_Function' ARGUMENT IMPLICITLY
    //
    // Concern: That a function object can be implicitly converted to a
    //   'bdef_Function' instance that can be invoked and dispatch the call
    //   properly.
    //
    // Plan:
    //   Assign various function objects to a 'bdef_Function' object as the
    //   argument of a function and verify that the proper function has
    //   been called.
    //
    // Testing:
    //   BINDING TO A 'bdef_Function' ARGUMENT IMPLICITLY
    // --------------------------------------------------------------------

    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    bslma_TestAllocator ta(veryVeryVerbose);
    int numAllocations;
    int numDeallocations;

    bslma_TestAllocator& testAllocator = ta;  // for BSLMA_EXCEPTION_TEST

    if (verbose)
       cout << "\nTesting binding to a 'bdef_Function' implicitly"
            << "\n===============================================" << endl;

    static const char *SPECS[] = {
        "",  "F", "G", "H",
        "U0", "U5", "U9", "V0", "V5", "V9", "W0", "W5", "W9",
        "S0", "S5", "S9", "T0", "T5", "T9", "P0", "P5", "P9",
        "Q0", "Q5", "Q9", "L0", "L5", "L9", "M0", "M5", "M9",
    0}; // Null string required as last element.

    if (verbose) cout << "Testing exhaustively.\n";
    {
        for (int ui = 1; SPECS[ui]; ++ui) {
            const char *const U_SPEC = SPECS[ui];

            if (verbose) {
                cout << "\tFor lhs objects of spec ";
                P(U_SPEC);
            }

            const Obj UU = g<Proto>(U_SPEC);  // control

            int result = -1;

            // Unfortunately, there is no way to test this in a loop
            // because we must instantiate the ctor with various 'FUNC'
            // template parameters.  A 'switch' statement will allow to
            // reuse the rest of the test loop infrastructure.  Since the
            // test must be performed inside the 'case' block, we use a
            // macro to avoid repetition:

            switch (strlen(U_SPEC)) {
              case 0: {
                ASSERT(0);
                result = testBinding((Proto)0);  // undefined behavior
              } break;
              case 1: {
                switch (U_SPEC[0]) {  // test here
                  case 'F': {
                    ASSERT(0 == strcmp(U_SPEC, "F"));
                    result = testBinding(&functionF);
                  } break;
                  case 'G': {
                    ASSERT(0 == strcmp(U_SPEC, "G"));
                    result = testBinding(&functionG);
                  } break;
                  case 'H': {
                    ASSERT(0 == strcmp(U_SPEC, "H"));
                    result = testBinding(&functionH);
                  } break;
                  default: ASSERT(0);
                }
              } break;
              case 2: {
                ASSERT('0' <= U_SPEC[1] && U_SPEC[1] <= '9');
                const int N = U_SPEC[1] - '0';

                switch (U_SPEC[0]) {  // test here
                  case 'U': {
                    FunctorU fU; fU.d_value = N;
                    result = testBinding(pointerWrapper(fU));
                  } break;
                  case 'V': {
                    FunctorV fV; fV.d_value = N;
                    result = testBinding(pointerWrapper(fV));
                  } break;
                  case 'W': {
                    FunctorW fW; fW.d_value = N;
                    result = testBinding(pointerWrapper(fW));
                  } break;
                  case 'S': {
                    FunctorS fS; fS.d_value = N;
                    result = testBinding(fS);
                  } break;
                  case 'T': {
                    FunctorT fT; fT.d_value = N;
                    result = testBinding(fT);
                  } break;
                  case 'P': {
                    FunctorP fP; fP.d_value = N;
                    result = testBinding(pointerWrapper(fP));
                  } break;
                  case 'Q': {
                    FunctorQ fQ; fQ.d_value = N;
                    result = testBinding(pointerWrapper(fQ));
                  } break;
                  case 'L': {
                    FunctorL fL; fL.d_value = N;
                    result = testBinding(fL);
                  } break;
                  case 'M': {
                    FunctorM fM; fM.d_value = N;
                    result = testBinding(fM);
                  } break;
                  default: ASSERT(0);
                }
              } break;
              default: ASSERT(0);
            }


            LOOP2_ASSERT(ui, result, UU(1) == result);
        }
    }

    if (verbose) cout << "Testing bassvc test case.\n";
    {
        // Inexplicably, at some point (bdef_function.h version dev/62),
        // this code would not compile in bassvc_inprocessservicemanager,
        // so it is here extracted with bogus types.  It would compile on
        // all platforms but create an internal compiler error with the AIX
        // xlC compiler.  Versions dev/63 seem to remove the error.

        using namespace TEST_CASE_BASSVC;

        SharedPtr               *requestRouter = 0;
        bassvc::ServiceManifest *manifest = 0;
        Aggregate                configuration;
        configuration.d_value = 123;

        int result = oldsvc_Entry__createService(requestRouter,
                                                 manifest,
                                                 configuration,
                                                 &ta);
        ASSERT(123 == result);

        typedef int (*FUNC)(SharedPtr               *requestRouter,
                            bassvc::ServiceManifest *manifest,
                            const Aggregate&         configuration,
                            bslma_Allocator         *basicAllocator);

        enum {
            IS_IN_PLACE           =
                                  bdef_FunctionUtil::IsInplace<FUNC>::VALUE
          , HAS_POINTER_SEMANTICS =
                bslalg_HasTrait<FUNC,
                                bslalg_TypeTraitHasPointerSemantics>::VALUE
          , INVOKER_TAG           = bslmf_IsFunctionPointer<FUNC>::VALUE
                                  ? bdef_Function_Rep::IS_FUNCTION_POINTER
                                  : IS_IN_PLACE ? (HAS_POINTER_SEMANTICS
            ? bdef_Function_Rep::IS_IN_PLACE_WITH_POINTER_SEMANTICS
            : bdef_Function_Rep::IS_IN_PLACE_WITHOUT_POINTER_SEMANTICS)
                                                : (HAS_POINTER_SEMANTICS
            ? bdef_Function_Rep::IS_OUT_OF_PLACE_WITH_POINTER_SEMANTICS
            : bdef_Function_Rep::IS_OUT_OF_PLACE_WITHOUT_POINTER_SEMANTICS)
        };

        if (verbose) { T_ P(INVOKER_TAG); }

        const char *name      = "myService";
        int         serviceId = 12345;
        InProcessServiceManager mX;

#ifndef BSLS_PLATFORM__CMP_IBM
        result = mX.registerBlobRouter(name,
                                       serviceId,
                                       &oldsvc_Entry__createService);
        ASSERT(0 == result);
#endif
    }
}

struct FunctorNop {
    void operator()() {}
};

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bslma_TestAllocator ta(veryVeryVerbose);
    int numAllocations;
    int numDeallocations;

    bslma_TestAllocator& testAllocator = ta;  // for BSLMA_EXCEPTION_TEST

    switch (test) { case 0:  // Zero is always the leading case.
      case 21: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.  Also
        //   provide (empty) bodies for functions in order to link
        //   successfully.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Usage Example" << endl
                          << "=====================" << endl;

        using namespace USAGE_EXAMPLE_NAMESPACE;

        usageExampleTest1();
        usageExampleTest2();
        usageExampleTest3();

        processMessages(true, cin);
        processMessages(false, cin);

        processMessages2(true, cin);
        processMessages2(false, cin);

      } break;
      case 20: {
        // --------------------------------------------------------------------
        // TESTING BINDING TO A FUNCTION WITH "C" LINKAGE
        //
        // Concern: That a function object can be implicitly converted to a
        //   'bdef_Function' instance that can be invoked and dispatch the call
        //   properly.
        //
        // Plan:
        //   Assign various function objects to a 'bdef_Function' object as the
        //   argument of a function and verify that the proper function has
        //   been called.
        //
        // Testing:
        //   BINDING TO A 'bdef_Function' ARGUMENT IMPLICITLY
        // --------------------------------------------------------------------

        if (verbose)
           cout << "\nTesting binding to a 'bdef_Function' implicitly"
                << "\n===============================================" << endl;

        const Obj OBJ = &externCIdentityFunction;

        ASSERT(0   == OBJ(0));
        ASSERT(1   == OBJ(1));
        ASSERT(123 == OBJ(123));

      } break;
      case 19: {
        // --------------------------------------------------------------------
        // TESTING BINDING TO A 'bdef_Function' ARGUMENT IMPLICITLY
        //
        // Concern: That a function object can be implicitly converted to a
        //   'bdef_Function' instance that can be invoked and dispatch the call
        //   properly.
        //
        // Plan:
        //   Assign various function objects to a 'bdef_Function' object as the
        //   argument of a function and verify that the proper function has
        //   been called.
        //
        // Testing:
        //   BINDING TO A 'bdef_Function' ARGUMENT IMPLICITLY
        // --------------------------------------------------------------------

        // NOTE: This test case had been moved to function 'testCase19' to
        //       work around an internal compiler error on AIX with compiler
        //       xlC.
        testCase19(argc);

      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING BINDING TO A FUNCTION OBJECT WITH PTR SEMANTICS
        //
        // Concern: That a function object which takes a variable arguments
        //   can be invoked with several signatures and dispatch the call
        //   properly.
        //
        // Plan:
        //   Assign a function object accepting multiple signatures to various
        //   'bdef_Function' objects and verify that the proper function has
        //   been called.
        //
        // Testing:
        //   BINDING TO A FUNCTION OBJECT WITH PTR SEMANTICS
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTesting binding to a function object with ptr semantics"
                 << "\n======================================================="
                 << endl;

          bdef_Function<int (*)(int)> mX;
          bdef_Function<int (*)(int)> const &X = mX;

          TestWrapper wrapper;
          mX = wrapper;
          ASSERT(X);
          ASSERT(1 == X(1));

      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING BINDING TO A SMALL BITWISE-COPYABLE FUNCTION OBJECT
        //
        // Concern: That a function object which takes a variable arguments
        //   can be invoked with several signatures and dispatch the call
        //   properly.
        //
        // Plan:
        //   Assign a small function object accepting multiple signatures to
        //   various 'bdef_Function' objects and verify that the proper
        //   function has been called.
        //
        // Testing:
        //   BINDING TO A SMALL BITWISE-COPYABLE FUNCTION OBJECT
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "Testing binding to a small bitwise-copyable"
                                                    " function object" << endl
                 << "==========================================="
                                                    "================" << endl;

        bdef_Function<int (*)()> f0;
        bdef_Function<int (*)(int)> f1;
        bdef_Function<int (*)(int,int)> f2;
        bdef_Function<int (*)(int,int,int)> f3;
        bdef_Function<int (*)(int,int,int,int)> f4;
        bdef_Function<int (*)(int,int,int,int,int)> f5;
        bdef_Function<int (*)(int,int,int,int,int,int)> f6;
        bdef_Function<int (*)(int,int,int,int,int,int,int)> f7;
        bdef_Function<int (*)(int,int,int,int,int,int,int, int)> f8;
        bdef_Function<int (*)(int,int,int,int,int,int,int, int,int)> f9;
        bdef_Function<int (*)(int,int,int,int,int,int,int, int,int,int)> f10;
        bdef_Function<int (*)(int,int,int,int,int,int,int,
                                                         int,int,int,int)> f11;
        bdef_Function<int (*)(int,int,int,int,int,int,int,
                                                     int,int,int,int,int)> f12;
        bdef_Function<int (*)(int,int,int,int,int,int,int,
                                                 int,int,int,int,int,int)> f13;
        bdef_Function<int (*)(int,int,int,int,int,int,int,
                                             int,int,int,int,int,int,int)> f14;

        MyFunctionObject obj;
        ASSERT(bdef_FunctionUtil::MAX_INPLACE_OBJECT_SIZE >=
                                                  sizeof(MyFunctionObject *));

        ASSERT(!f0);
        f0 = rawBCPointerWrapper(&obj);
        ASSERT(f0);
        ASSERT(0 == f0());

        ASSERT(!f1);
        f1 = rawBCPointerWrapper(&obj);
        ASSERT(f1);
        ASSERT(1 == f1(1));

        ASSERT(!f2);
        f2 = rawBCPointerWrapper(&obj);
        ASSERT(f2);
        ASSERT(2 == f2(1,2));

        ASSERT(!f3);
        f3 = rawBCPointerWrapper(&obj);
        ASSERT(f3);
        ASSERT(3 == f3(1,2,3));

        ASSERT(!f4);
        f4 = rawBCPointerWrapper(&obj);
        ASSERT(f4);
        ASSERT( 4 == f4(1,2,3,4));

        ASSERT(!f5);
        f5 = rawBCPointerWrapper(&obj);
        ASSERT(f5);
        ASSERT( 5 == f5(1,2,3,4,5));

        ASSERT(!f6);
        f6 = rawBCPointerWrapper(&obj);
        ASSERT(f6);
        ASSERT( 6 == f6(1,2,3,4,5,6));

        ASSERT(!f7);
        f7 = rawBCPointerWrapper(&obj);
        ASSERT(f7);
        ASSERT( 7 == f7(1,2,3,4,5,6,7));

        ASSERT(!f8);
        f8 = rawBCPointerWrapper(&obj);
        ASSERT(f8);
        ASSERT( 8 == f8(1,2,3,4,5,6,7,8));

        ASSERT(!f9);
        f9 = rawBCPointerWrapper(&obj);
        ASSERT(f9);
        ASSERT( 9 == f9(1,2,3,4,5,6,7,8,9));

        ASSERT(!f10);
        f10 = rawBCPointerWrapper(&obj);
        ASSERT(f10);
        ASSERT( 10 == f10(1,2,3,4,5,6,7,8,9,10));

        ASSERT(!f11);
        f11 = rawBCPointerWrapper(&obj);
        ASSERT(f11);
        ASSERT( 11 == f11(1,2,3,4,5,6,7,8,9,10,11));

        ASSERT(!f12);
        f12 = rawBCPointerWrapper(&obj);
        ASSERT(f12);
        ASSERT( 12 == f12(1,2,3,4,5,6,7,8,9,10,11,12));

        ASSERT(!f13);
        f13 = rawBCPointerWrapper(&obj);
        ASSERT(f13);
        ASSERT( 13 == f13(1,2,3,4,5,6,7,8,9,10,11,12,13));

        ASSERT(!f14);
        f14 = rawBCPointerWrapper(&obj);
        ASSERT(f14);
        ASSERT( 14 == f14(1,2,3,4,5,6,7,8,9,10,11,12,13,14));

      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING BINDING TO A LARGE BITWISE-COPYABLE FUNCTION OBJECT
        //
        // Concern: That a function object which takes a variable arguments
        //   can be invoked with several signatures and dispatch the call
        //   properly.
        //
        // Plan:
        //   Assign a function object accepting multiple signatures to various
        //   'bdef_Function' objects and verify that the proper function has
        //   been called.
        //
        // Testing:
        //   BINDING TO A LARGE BITWISE-COPYABLE FUNCTION OBJECT
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "Testing binding to a large bitwise-copyable"
                                                    " function object" << endl
                 << "==========================================="
                                                    "================" << endl;

        bdef_Function<int (*)()> f0;
        bdef_Function<int (*)(int)> f1;
        bdef_Function<int (*)(int,int)> f2;
        bdef_Function<int (*)(int,int,int)> f3;
        bdef_Function<int (*)(int,int,int,int)> f4;
        bdef_Function<int (*)(int,int,int,int,int)> f5;
        bdef_Function<int (*)(int,int,int,int,int,int)> f6;
        bdef_Function<int (*)(int,int,int,int,int,int,int)> f7;
        bdef_Function<int (*)(int,int,int,int,int,int,int, int)> f8;
        bdef_Function<int (*)(int,int,int,int,int,int,int, int,int)> f9;
        bdef_Function<int (*)(int,int,int,int,int,int,int, int,int,int)> f10;
        bdef_Function<int (*)(int,int,int,int,int,int,int,
                                                         int,int,int,int)> f11;
        bdef_Function<int (*)(int,int,int,int,int,int,int,
                                                     int,int,int,int,int)> f12;
        bdef_Function<int (*)(int,int,int,int,int,int,int,
                                                 int,int,int,int,int,int)> f13;
        bdef_Function<int (*)(int,int,int,int,int,int,int,
                                             int,int,int,int,int,int,int)> f14;

        MyBCFunctionObject obj; MyBCFunctionObject const &OBJ = obj;
        ASSERT(bdef_FunctionUtil::MAX_INPLACE_OBJECT_SIZE <
                                                   sizeof(MyBCFunctionObject));

        ASSERT(!f0);
        f0 = OBJ;
        ASSERT(f0);
        ASSERT(0 == f0());

        ASSERT(!f1);
        f1 = OBJ;
        ASSERT(f1);
        ASSERT(1 == f1(1));

        ASSERT(!f2);
        f2 = OBJ;
        ASSERT(f2);
        ASSERT(2 == f2(1,2));

        ASSERT(!f3);
        f3 = OBJ;
        ASSERT(f3);
        ASSERT(3 == f3(1,2,3));

        ASSERT(!f4);
        f4 = OBJ;
        ASSERT(f4);
        ASSERT( 4 == f4(1,2,3,4));

        ASSERT(!f5);
        f5 = OBJ;
        ASSERT(f5);
        ASSERT( 5 == f5(1,2,3,4,5));

        ASSERT(!f6);
        f6 = OBJ;
        ASSERT(f6);
        ASSERT( 6 == f6(1,2,3,4,5,6));

        ASSERT(!f7);
        f7 = OBJ;
        ASSERT(f7);
        ASSERT( 7 == f7(1,2,3,4,5,6,7));

        ASSERT(!f8);
        f8 = OBJ;
        ASSERT(f8);
        ASSERT( 8 == f8(1,2,3,4,5,6,7,8));

        ASSERT(!f9);
        f9 = OBJ;
        ASSERT(f9);
        ASSERT( 9 == f9(1,2,3,4,5,6,7,8,9));

        ASSERT(!f10);
        f10 = OBJ;
        ASSERT(f10);
        ASSERT( 10 == f10(1,2,3,4,5,6,7,8,9,10));

        ASSERT(!f11);
        f11 = OBJ;
        ASSERT(f11);
        ASSERT( 11 == f11(1,2,3,4,5,6,7,8,9,10,11));

        ASSERT(!f12);
        f12 = OBJ;
        ASSERT(f12);
        ASSERT( 12 == f12(1,2,3,4,5,6,7,8,9,10,11,12));

        ASSERT(!f13);
        f13 = OBJ;
        ASSERT(f13);
        ASSERT( 13 == f13(1,2,3,4,5,6,7,8,9,10,11,12,13));

        ASSERT(!f14);
        f14 = OBJ;
        ASSERT(f14);
        ASSERT( 14 == f14(1,2,3,4,5,6,7,8,9,10,11,12,13,14));

      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING BINDING TO A SMALL FUNCTION OBJECT
        //
        // Concern: That a function object which takes a variable arguments
        //   can be invoked with several signatures and dispatch the call
        //   properly.
        //
        // Plan:
        //   Assign a small function object accepting multiple signatures to
        //   various 'bdef_Function' objects and verify that the proper
        //   function has been called.
        //
        // Testing:
        //   BINDING TO A SMALL FUNCTION OBJECT
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "Testing binding to a small function object" << endl
                 << "==========================================" << endl;

        bdef_Function<int (*)()> f0;
        bdef_Function<int (*)(int)> f1;
        bdef_Function<int (*)(int,int)> f2;
        bdef_Function<int (*)(int,int,int)> f3;
        bdef_Function<int (*)(int,int,int,int)> f4;
        bdef_Function<int (*)(int,int,int,int,int)> f5;
        bdef_Function<int (*)(int,int,int,int,int,int)> f6;
        bdef_Function<int (*)(int,int,int,int,int,int,int)> f7;
        bdef_Function<int (*)(int,int,int,int,int,int,int, int)> f8;
        bdef_Function<int (*)(int,int,int,int,int,int,int, int,int)> f9;
        bdef_Function<int (*)(int,int,int,int,int,int,int, int,int,int)> f10;
        bdef_Function<int (*)(int,int,int,int,int,int,int,
                                                         int,int,int,int)> f11;
        bdef_Function<int (*)(int,int,int,int,int,int,int,
                                                     int,int,int,int,int)> f12;
        bdef_Function<int (*)(int,int,int,int,int,int,int,
                                                 int,int,int,int,int,int)> f13;
        bdef_Function<int (*)(int,int,int,int,int,int,int,
                                             int,int,int,int,int,int,int)> f14;

        MyFunctionObject obj;
        ASSERT(bdef_FunctionUtil::MAX_INPLACE_OBJECT_SIZE >=
                                                  sizeof(MyFunctionObject *));

        ASSERT(!f0);
        f0 = rawPointerWrapper(&obj);
        ASSERT(f0);
        ASSERT(0 == f0());

        ASSERT(!f1);
        f1 = rawPointerWrapper(&obj);
        ASSERT(f1);
        ASSERT(1 == f1(1));

        ASSERT(!f2);
        f2 = rawPointerWrapper(&obj);
        ASSERT(f2);
        ASSERT(2 == f2(1,2));

        ASSERT(!f3);
        f3 = rawPointerWrapper(&obj);
        ASSERT(f3);
        ASSERT(3 == f3(1,2,3));

        ASSERT(!f4);
        f4 = rawPointerWrapper(&obj);
        ASSERT(f4);
        ASSERT( 4 == f4(1,2,3,4));

        ASSERT(!f5);
        f5 = rawPointerWrapper(&obj);
        ASSERT(f5);
        ASSERT( 5 == f5(1,2,3,4,5));

        ASSERT(!f6);
        f6 = rawPointerWrapper(&obj);
        ASSERT(f6);
        ASSERT( 6 == f6(1,2,3,4,5,6));

        ASSERT(!f7);
        f7 = rawPointerWrapper(&obj);
        ASSERT(f7);
        ASSERT( 7 == f7(1,2,3,4,5,6,7));

        ASSERT(!f8);
        f8 = rawPointerWrapper(&obj);
        ASSERT(f8);
        ASSERT( 8 == f8(1,2,3,4,5,6,7,8));

        ASSERT(!f9);
        f9 = rawPointerWrapper(&obj);
        ASSERT(f9);
        ASSERT( 9 == f9(1,2,3,4,5,6,7,8,9));

        ASSERT(!f10);
        f10 = rawPointerWrapper(&obj);
        ASSERT(f10);
        ASSERT( 10 == f10(1,2,3,4,5,6,7,8,9,10));

        ASSERT(!f11);
        f11 = rawPointerWrapper(&obj);
        ASSERT(f11);
        ASSERT( 11 == f11(1,2,3,4,5,6,7,8,9,10,11));

        ASSERT(!f12);
        f12 = rawPointerWrapper(&obj);
        ASSERT(f12);
        ASSERT( 12 == f12(1,2,3,4,5,6,7,8,9,10,11,12));

        ASSERT(!f13);
        f13 = rawPointerWrapper(&obj);
        ASSERT(f13);
        ASSERT( 13 == f13(1,2,3,4,5,6,7,8,9,10,11,12,13));

        ASSERT(!f14);
        f14 = rawPointerWrapper(&obj);
        ASSERT(f14);
        ASSERT( 14 == f14(1,2,3,4,5,6,7,8,9,10,11,12,13,14));

      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING BINDING TO A LARGE FUNCTION OBJECT
        //
        // Concern: That a function object which takes a variable arguments
        //   can be invoked with several signatures and dispatch the call
        //   properly.
        //
        // Plan:
        //   Assign a function object accepting multiple signatures to various
        //   'bdef_Function' objects and verify that the proper function has
        //   been called.
        //
        // Testing:
        //   BINDING TO A LARGE FUNCTION OBJECT
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "Testing binding to a large function object" << endl
                 << "==========================================" << endl;

        bdef_Function<int (*)()> f0;
        bdef_Function<int (*)(int)> f1;
        bdef_Function<int (*)(int,int)> f2;
        bdef_Function<int (*)(int,int,int)> f3;
        bdef_Function<int (*)(int,int,int,int)> f4;
        bdef_Function<int (*)(int,int,int,int,int)> f5;
        bdef_Function<int (*)(int,int,int,int,int,int)> f6;
        bdef_Function<int (*)(int,int,int,int,int,int,int)> f7;
        bdef_Function<int (*)(int,int,int,int,int,int,int, int)> f8;
        bdef_Function<int (*)(int,int,int,int,int,int,int, int,int)> f9;
        bdef_Function<int (*)(int,int,int,int,int,int,int, int,int,int)> f10;
        bdef_Function<int (*)(int,int,int,int,int,int,int,
                                                         int,int,int,int)> f11;
        bdef_Function<int (*)(int,int,int,int,int,int,int,
                                                     int,int,int,int,int)> f12;
        bdef_Function<int (*)(int,int,int,int,int,int,int,
                                                 int,int,int,int,int,int)> f13;
        bdef_Function<int (*)(int,int,int,int,int,int,int,
                                             int,int,int,int,int,int,int)> f14;

        MyFunctionObject obj; MyFunctionObject const &OBJ = obj;
        ASSERT(bdef_FunctionUtil::MAX_INPLACE_OBJECT_SIZE <
                                                     sizeof(MyFunctionObject));

        ASSERT(!f0);
        f0 = OBJ;
        ASSERT(f0);
        ASSERT(0 == f0());

        ASSERT(!f1);
        f1 = OBJ;
        ASSERT(f1);
        ASSERT(1 == f1(1));

        ASSERT(!f2);
        f2 = OBJ;
        ASSERT(f2);
        ASSERT(2 == f2(1,2));

        ASSERT(!f3);
        f3 = OBJ;
        ASSERT(f3);
        ASSERT(3 == f3(1,2,3));

        ASSERT(!f4);
        f4 = OBJ;
        ASSERT(f4);
        ASSERT( 4 == f4(1,2,3,4));

        ASSERT(!f5);
        f5 = OBJ;
        ASSERT(f5);
        ASSERT( 5 == f5(1,2,3,4,5));

        ASSERT(!f6);
        f6 = OBJ;
        ASSERT(f6);
        ASSERT( 6 == f6(1,2,3,4,5,6));

        ASSERT(!f7);
        f7 = OBJ;
        ASSERT(f7);
        ASSERT( 7 == f7(1,2,3,4,5,6,7));

        ASSERT(!f8);
        f8 = OBJ;
        ASSERT(f8);
        ASSERT( 8 == f8(1,2,3,4,5,6,7,8));

        ASSERT(!f9);
        f9 = OBJ;
        ASSERT(f9);
        ASSERT( 9 == f9(1,2,3,4,5,6,7,8,9));

        ASSERT(!f10);
        f10 = OBJ;
        ASSERT(f10);
        ASSERT( 10 == f10(1,2,3,4,5,6,7,8,9,10));

        ASSERT(!f11);
        f11 = OBJ;
        ASSERT(f11);
        ASSERT( 11 == f11(1,2,3,4,5,6,7,8,9,10,11));

        ASSERT(!f12);
        f12 = OBJ;
        ASSERT(f12);
        ASSERT( 12 == f12(1,2,3,4,5,6,7,8,9,10,11,12));

        ASSERT(!f13);
        f13 = OBJ;
        ASSERT(f13);
        ASSERT( 13 == f13(1,2,3,4,5,6,7,8,9,10,11,12,13));

        ASSERT(!f14);
        f14 = OBJ;
        ASSERT(f14);
        ASSERT( 14 == f14(1,2,3,4,5,6,7,8,9,10,11,12,13,14));

      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING BINDING TO A FUNCTION POINTER
        //
        // Concern: That 'bdef_Function' can be bound to a function pointer.
        //
        // Plan:
        //   Assign function pointers to various 'bdef_Function' objects
        //   accepting the same signature and verify that the proper function
        //   has been called.
        //
        // Testing:
        //   BINDING TO A FUNCTION POINTER
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing binding to function pointer" << endl
                          << "===================================" << endl;

        bdef_Function<int (*)()> f0;
        bdef_Function<int (*)(int)> f1;
        bdef_Function<int (*)(int,int)> f2;
        bdef_Function<int (*)(int,int,int)> f3;
        bdef_Function<int (*)(int,int,int,int)> f4;
        bdef_Function<int (*)(int,int,int,int,int)> f5;
        bdef_Function<int (*)(int,int,int,int,int,int)> f6;
        bdef_Function<int (*)(int,int,int,int,int,int,int)> f7;
        bdef_Function<int (*)(int,int,int,int,int,int,int,
                              int)> f8;
        bdef_Function<int (*)(int,int,int,int,int,int,int,
                              int,int)> f9;
        bdef_Function<int (*)(int,int,int,int,int,int,int,
                              int,int,int)> f10;
        bdef_Function<int (*)(int,int,int,int,int,int,int,
                              int,int,int,int)> f11;
        bdef_Function<int (*)(int,int,int,int,int,int,int,
                              int,int,int,int,int)> f12;
        bdef_Function<int (*)(int,int,int,int,int,int,int,
                              int,int,int,int,int,int)> f13;
        bdef_Function<int (*)(int,int,int,int,int,int,int,
                              int,int,int,int,int,int,int)> f14;

        ASSERT(!f0);
        f0 = &freeFunc0;
        ASSERT(f0);
        ASSERT( 0 == f0());

        ASSERT(!f1);
        f1 = &freeFunc1;
        ASSERT(f1);
        ASSERT( 1 == f1(1));

        ASSERT(!f2);
        f2 = &freeFunc2;
        ASSERT(f2);
        ASSERT( 2 == f2(1,2));

        ASSERT(!f3);
        f3 = &freeFunc3;
        ASSERT(f3);
        ASSERT( 3 == f3(1,2,3));

        ASSERT(!f4);
        f4 = &freeFunc4;
        ASSERT(f4);
        ASSERT( 4 == f4(1,2,3,4));

        ASSERT(!f5);
        f5 = &freeFunc5;
        ASSERT(f5);
        ASSERT( 5 == f5(1,2,3,4,5));

        ASSERT(!f6);
        f6 = &freeFunc6;
        ASSERT(f6);
        ASSERT( 6 == f6(1,2,3,4,5,6));

        ASSERT(!f7);
        f7 = &freeFunc7;
        ASSERT(f7);
        ASSERT( 7 == f7(1,2,3,4,5,6,7));

        ASSERT(!f8);
        f8 = &freeFunc8;
        ASSERT(f8);
        ASSERT( 8 == f8(1,2,3,4,5,6,7,8));

        ASSERT(!f9);
        f9 = &freeFunc9;
        ASSERT(f9);
        ASSERT( 9 == f9(1,2,3,4,5,6,7,8,9));

        ASSERT(!f10);
        f10 = &freeFunc10;
        ASSERT(f10);
        ASSERT( 10 == f10(1,2,3,4,5,6,7,8,9,10));

        ASSERT(!f11);
        f11 = &freeFunc11;
        ASSERT(f11);
        ASSERT( 11 == f11(1,2,3,4,5,6,7,8,9,10,11));

        ASSERT(!f12);
        f12 = &freeFunc12;
        ASSERT(f12);
        ASSERT(12 == f12(1,2,3,4,5,6,7,8,9,10,11,12));

        ASSERT(!f13);
        f13 = &freeFunc13;
        ASSERT(f13);
        ASSERT(13 == f13(1,2,3,4,5,6,7,8,9,10,11,12,13));

        ASSERT(!f14);
        f14 = &freeFunc14;
        ASSERT(f14);
        ASSERT( 14 == f14(1,2,3,4,5,6,7,8,9,10,11,12,13,14));

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // ADDITIONAL MANIPULATORS
        //
        // Concerns:
        //   1. 'load' assigns the target value, and properly disposes of the
        //      previous value without memory leaks; if allocator is specified,
        //      the object's current allocator is replaced by the target
        //      allocator.
        //   2. 'swap' exchanges the values and properly preserves the
        //      allocator of both objects.
        //   3. 'transferTo' moves the source value to the target object,
        //      leaving the source object in an empty state; it properly
        //      disposes of the previous target value without memory leaks; it
        //      properly preserves the allocator of both objects.
        //
        // Plan:
        //   For concerns 1--3, for every source and target invocable
        //   values in a cross-product S x S, initialize the source and apply
        //   the operation to the target, and verify that both the source and
        //   target values have been changed as expected (depending on the
        //   operation).  Repeat each test with source and target objects
        //   initialized with distinct allocators (default, global test
        //   allocator, or another, distinct, test allocator), and assert that
        //   allocators are .
        //
        // Testing:
        //    void load(const FUNC& func, bslma_Allocator *allocator);
        //    void swap(const bdef_Function& rhs);
        //    void transferTo(const bdef_Function *target);
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing additional manipulators" << endl
                          << "===============================" << endl;

        static const char *SPECS[] = {
            "",  "F", "G", "H",
            "U0", "U5", "U9", "V0", "V5", "V9", "W0", "W5", "W9",
            "S0", "S5", "S9", "T0", "T5", "T9", "P0", "P5", "P9",
            "Q0", "Q5", "Q9", "L0", "L5", "L9", "M0", "M5", "M9",
        0}; // Null string required as last element.

        if (verbose) cout << "\nTesting 'load'." << endl;
        {
            bslma_TestAllocator tb(veryVeryVerbose);
            bslma_Allocator *Z[] = { bslma_Default::allocator(0), &ta, &tb };
            const int NUM_ALLOCS = sizeof Z / sizeof *Z;

            for (int ui = 0; SPECS[ui]; ++ui) {
            for (int ua = 0; ua < NUM_ALLOCS; ++ua) {
                const char *const U_SPEC = SPECS[ui];

                if (verbose) {
                    cout << "\tFor lhs objects of spec ";
                    P_(U_SPEC);
                    cout << " with alloc " << ua << endl;
                }

                const Obj UU = g<Proto>(U_SPEC);  // control

                for (int vi = 1; SPECS[vi]; ++vi) {
                    // Note: the behavior is undefined if vi == 0.

                for (int va = 0; va < NUM_ALLOCS; ++va) {
                    const char *const V_SPEC = SPECS[vi];

                    if (verbose) {
                        cout << "\t\tLoading rhs objects of spec ";
                        P_(V_SPEC);
                        cout << " with alloc " << va << endl;

                    }

                    const Obj VV = g<Proto>(V_SPEC);  // control

                    Obj mU(Z[ua]);  const Obj& U = mU;  gg(&mU, U_SPEC);

                    // Unfortunately, there is no way to test this in a loop
                    // because we must instantiate 'load' with various 'FUNC'
                    // template parameters.  A 'switch' statement will allow to
                    // reuse the rest of the test loop infrastructure.

                    switch (strlen(V_SPEC)) {
                      case 0: {
                        ASSERT(0);  // undefined behavior
                        mU.load((Proto)0, Z[va]);
                      } break;
                      case 1: {
                        switch (V_SPEC[0]) {  // test here
                          case 'F':
                            ASSERT(0 == strcmp(V_SPEC, "F"));
                            mU.load(&functionF, Z[va]);
                          break;
                          case 'G':
                            ASSERT(0 == strcmp(V_SPEC, "G"));
                            mU.load(&functionG, Z[va]);
                          break;
                          case 'H':
                            ASSERT(0 == strcmp(V_SPEC, "H"));
                            mU.load(&functionH, Z[va]);
                          break;
                          default: ASSERT(0);
                        }
                      } break;
                      case 2: {
                        ASSERT('0' <= V_SPEC[1] && V_SPEC[1] <= '9');
                        const int N = V_SPEC[1] - '0';

                        switch (V_SPEC[0]) {  // test here
                          case 'U': {
                            FunctorU fU; fU.d_value = N;
                            mU.load(pointerWrapper(fU), Z[va]);
                          } break;
                          case 'V': {
                            FunctorV fV; fV.d_value = N;
                            mU.load(pointerWrapper(fV), Z[va]);
                          } break;
                          case 'W': {
                            FunctorW fW; fW.d_value = N;
                            mU.load(pointerWrapper(fW), Z[va]);
                          } break;
                          case 'S': {
                            FunctorS fS; fS.d_value = N;
                            mU.load(fS, Z[va]);
                          } break;
                          case 'T': {
                            FunctorT fT; fT.d_value = N;
                            mU.load(fT, Z[va]);
                          } break;
                          case 'P': {
                            FunctorP fP; fP.d_value = N;
                            mU.load(pointerWrapper(fP), Z[va]);
                          } break;
                          case 'Q': {
                            FunctorQ fQ; fQ.d_value = N;
                            mU.load(pointerWrapper(fQ), Z[va]);
                          } break;
                          case 'L': {
                            FunctorL fL; fL.d_value = N;
                            mU.load(fL, Z[va]);
                          } break;
                          case 'M': {
                            FunctorM fM; fM.d_value = N;
                            mU.load(fM, Z[va]);
                          } break;
                          default: ASSERT(0);
                        }
                      } break;
                      default: ASSERT(0);
                    }

#ifndef BDE_FUNCTION_PRIOR_1_16
                    if (veryVerbose) {
                        cout << "\t\t\tafter loading: ";
                        if (U.getAllocator() == Z[0]) cout << "ua = 0";
                        if (U.getAllocator() == Z[1]) cout << "ua = ta";
                        if (U.getAllocator() == Z[2]) cout << "ua = tb";
                        cout << endl;
                    }
#endif

                    LOOP4_ASSERT(ui, ua, vi, va, VV == U);
#ifndef BDE_FUNCTION_PRIOR_1_16
                    LOOP4_ASSERT(ui, ua, vi, va, U.getAllocator() == Z[va]);
#endif
                }
                }
                ASSERT(0 == ta.numMismatches());
                ASSERT(0 == ta.numBytesInUse());
                ASSERT(0 == tb.numMismatches());
                ASSERT(0 == tb.numBytesInUse());
            }
            }
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\nTesting 'swap'." << endl;

        if (verbose) cout << "\tUsing default allocator." << endl;
        {
            for (int ui = 0; SPECS[ui]; ++ui) {
                const char *const U_SPEC = SPECS[ui];

                if (verbose) {
                    cout << "\tFor lhs objects of spec ";
                    P(U_SPEC);
                }

                const Obj UU = g<Proto>(U_SPEC);  // control

                for (int vi = 0; SPECS[vi]; ++vi) {
                    const char *const V_SPEC = SPECS[vi];

                    if (verbose) {
                        cout << "\t\tSwapping with rhs objects of spec ";
                        P(V_SPEC);
                    }

                    const Obj VV = g<Proto>(V_SPEC);  // control

                    Obj mU;  const Obj& U = mU;  gg(&mU, U_SPEC);
                    Obj mV;  const Obj& V = mV;  gg(&mV, V_SPEC);

                    mU.swap(mV);  // test here

                    LOOP2_ASSERT(ui, vi, UU == V);
                    LOOP2_ASSERT(ui, vi, VV == U);
                }
            }
        }

        if (verbose) cout << "\tUsing various allocators." << endl;
        {
            bslma_TestAllocator tb(veryVeryVerbose);
            bslma_Allocator *Z[] = { bslma_Default::allocator(0), &ta, &tb };
            const int NUM_ALLOCS = sizeof Z / sizeof *Z;

            for (int ui = 0; SPECS[ui]; ++ui) {
            for (int ua = 0; ua < NUM_ALLOCS; ++ua) {
                const char *const U_SPEC = SPECS[ui];

                if (verbose) {
                    cout << "\tFor lhs objects of spec ";
                    P_(U_SPEC);
                    cout << " with alloc " << ua << endl;
                }

                const Obj UU = g<Proto>(U_SPEC);  // control

                for (int vi = 0; SPECS[vi]; ++vi) {
                for (int va = 0; va < NUM_ALLOCS; ++va) {
                    const char *const V_SPEC = SPECS[vi];

                    if (veryVerbose) {
                        cout << "\t\tSwapping with rhs objects of spec ";
                        P_(V_SPEC);
                        cout << " with alloc " << va << endl;
                    }

                    const Obj VV = g<Proto>(V_SPEC);  // control

                    Obj mU(Z[ua]);  const Obj& U = mU;  gg(&mU, U_SPEC);
                    Obj mV(Z[va]);  const Obj& V = mV;  gg(&mV, V_SPEC);

                    mU.swap(mV);  // test here

#ifndef BDE_FUNCTION_PRIOR_1_16
                    if (veryVerbose) {
                        cout << "\t\t\tafter swapping: ";
                        if (U.getAllocator() == Z[0]) cout << "ua = 0";
                        if (U.getAllocator() == Z[1]) cout << "ua = ta";
                        if (U.getAllocator() == Z[2]) cout << "ua = tb";
                        cout << " and ";
                        if (V.getAllocator() == Z[0]) cout << "va = 0";
                        if (V.getAllocator() == Z[1]) cout << "va = ta";
                        if (V.getAllocator() == Z[2]) cout << "va = tb";
                        cout << endl;
                    }
#endif

                    LOOP4_ASSERT(ui, ua, vi, va, UU == V);
                    LOOP4_ASSERT(ui, ua, vi, va, VV == U);

#ifndef BDE_FUNCTION_PRIOR_1_16
                    LOOP4_ASSERT(ui, ua, vi, va, U.getAllocator() == Z[ua]);
                    LOOP4_ASSERT(ui, ua, vi, va, V.getAllocator() == Z[va]);
#endif
                }
                }
                ASSERT(0 == ta.numMismatches());
                ASSERT(0 == ta.numBytesInUse());
                ASSERT(0 == tb.numMismatches());
                ASSERT(0 == tb.numBytesInUse());
            }
            }
        }

        if (verbose) cout << "\tUsing free function." << endl;

        {
            for (int ui = 0; SPECS[ui]; ++ui) {
                const char *const U_SPEC = SPECS[ui];

                if (verbose) {
                    cout << "\tFor lhs objects of spec ";
                    P(U_SPEC);
                }

                const Obj UU = g<Proto>(U_SPEC);  // control

                for (int vi = 0; SPECS[vi]; ++vi) {
                    const char *const V_SPEC = SPECS[vi];

                    if (verbose) {
                        cout << "\t\tSwapping with rhs objects of spec ";
                        P(V_SPEC);
                    }

                    const Obj VV = g<Proto>(V_SPEC);  // control

                    Obj mU;  const Obj& U = mU;  gg(&mU, U_SPEC);
                    Obj mV;  const Obj& V = mV;  gg(&mV, V_SPEC);

                    swap(mU, mV); // test here

                    LOOP2_ASSERT(ui, vi, UU == V);
                    LOOP2_ASSERT(ui, vi, VV == U);
                }
            }
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\nTesting 'transferTo'." << endl;

        if (verbose) cout << "\tUsing default allocator." << endl;
        {
            for (int ui = 0; SPECS[ui]; ++ui) {
                const char *const U_SPEC = SPECS[ui];

                if (verbose) {
                    cout << "\tFor lhs objects of spec ";
                    P(U_SPEC);
                }

                const Obj UU = g<Proto>(U_SPEC);  // control

                for (int vi = 0; SPECS[vi]; ++vi) {
                    const char *const V_SPEC = SPECS[vi];

                    if (veryVerbose) {
                        cout << "\t\tTransfer to rhs objects of spec ";
                        P(V_SPEC);
                    }

                    const Obj VV = g<Proto>(V_SPEC);  // control

                    Obj mU;  const Obj& U = mU;  gg(&mU, U_SPEC);
                    Obj mV;  const Obj& V = mV;  gg(&mV, V_SPEC);

                    mU.transferTo(&mV);  // test here

                    LOOP2_ASSERT(ui, vi, UU    == V);
                    LOOP2_ASSERT(ui, vi, false == U);
                }
            }
        }

        if (verbose) cout << "\tUsing various allocators." << endl;
        {
            bslma_TestAllocator tb(veryVeryVerbose);
            bslma_Allocator *Z[] = { bslma_Default::allocator(0), &ta, &tb };
            const int NUM_ALLOCS = sizeof Z / sizeof *Z;

            for (int ui = 0; SPECS[ui]; ++ui) {
            for (int ua = 0; ua < NUM_ALLOCS; ++ua) {
                const char *const U_SPEC = SPECS[ui];

                if (verbose) {
                    cout << "\tFor lhs objects of spec ";
                    P_(U_SPEC);
                    cout << " with alloc " << ua << endl;
                }

                const Obj UU = g<Proto>(U_SPEC);  // control

                for (int vi = 0; SPECS[vi]; ++vi) {
                for (int va = 0; va < NUM_ALLOCS; ++va) {
                    const char *const V_SPEC = SPECS[vi];

                    if (veryVerbose) {
                        cout << "\t\tTransfer to rhs objects of spec ";
                        P_(V_SPEC);
                        cout << " with alloc " << va << endl;
                    }

                    const Obj VV = g<Proto>(V_SPEC);  // control

                    Obj mU(Z[ua]);  const Obj& U = mU;  gg(&mU, U_SPEC);
                    Obj mV(Z[va]);  const Obj& V = mV;  gg(&mV, V_SPEC);

                    mU.transferTo(&mV);  // test here

#ifndef BDE_FUNCTION_PRIOR_1_16
                    if (veryVerbose) {
                        cout << "\t\t\tafter transfer: ";
                        if (U.getAllocator() == Z[0]) cout << "ua = 0";
                        if (U.getAllocator() == Z[1]) cout << "ua = 1";
                        if (U.getAllocator() == Z[2]) cout << "ua = 2";
                        cout << " and ";
                        if (V.getAllocator() == Z[0]) cout << "va = 0";
                        if (V.getAllocator() == Z[1]) cout << "va = 1";
                        if (V.getAllocator() == Z[2]) cout << "va = 2";
                        cout << endl;
                    }
#endif

                    LOOP4_ASSERT(ui, ua, vi, va, UU    == V);
                    LOOP4_ASSERT(ui, ua, vi, va, false == U);

#ifndef BDE_FUNCTION_PRIOR_1_16
                    LOOP4_ASSERT(ui, ua, vi, va, U.getAllocator() == Z[ua]);
                    LOOP4_ASSERT(ui, ua, vi, va, V.getAllocator() == Z[va]);
#endif
                }
                }
                ASSERT(0 == ta.numMismatches());
                ASSERT(0 == ta.numBytesInUse());
                ASSERT(0 == tb.numMismatches());
                ASSERT(0 == tb.numBytesInUse());
            }
            }
        }

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING ADDITIONAL CONSTRUCTORS
        //
        // Concerns:
        //   1. That construction with a 'bslma_Allocator *' leaves the
        //      function object empty, but with a function pointer correctly
        //      sets the invocable and uses the default allocator.
        //   2. That value is set identically as default constructor followed
        //      by assignment.
        //   3. That passing in an allocator extra argument correctly sets the
        //      allocator of the function object.
        //
        // Testing:
        //   bdef_Function<FUNC_OR_ALLOC>(const FUNC_OR_ALLOC& funcOrAlloc);
        //   bdef_Function<FUNC>(const FUNC& func, bslma_Allocator *allocator);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Additional Constructors." << endl
                          << "================================" << endl;

        static const char *SPECS[] = {
            "",  "F", "G", "H",
            "U0", "U5", "U9", "V0", "V5", "V9", "W0", "W5", "W9",
            "S0", "S5", "S9", "T0", "T5", "T9", "P0", "P5", "P9",
            "Q0", "Q5", "Q9", "L0", "L5", "L9", "M0", "M5", "M9",
        0}; // Null string required as last element.

        if (verbose) cout << "\nTesting 'bdef_Function<FUNC>'." << endl;

        if (verbose) cout << "\tFrom allocator." << endl;
        {
            const Obj X(&ta);

            ASSERT(!X);
            ASSERT(&ta == X.getAllocator());
        }

        if (verbose) cout << "\tWithout allocators." << endl;
        {
            for (int ui = 1; SPECS[ui]; ++ui) {
                const char *const U_SPEC = SPECS[ui];

                if (verbose) {
                    cout << "\tFor lhs objects of spec ";
                    P(U_SPEC);
                }

                const Obj UU = g<Proto>(U_SPEC);  // control

                // Unfortunately, there is no way to test this in a loop
                // because we must instantiate the ctor with various 'FUNC'
                // template parameters.  A 'switch' statement will allow to
                // reuse the rest of the test loop infrastructure.  Since the
                // test must be performed inside the 'case' block, we use a
                // macro to avoid repetition:

                switch (strlen(U_SPEC)) {
                  case 0: {
                    ASSERT(0);
                    const Obj U((Proto)0);  // undefined behavior
                    LOOP_ASSERT(ui, UU == U);  // will fail
                  } break;
                  case 1: {
                    switch (U_SPEC[0]) {  // test here
                      case 'F': {
                        ASSERT(0 == strcmp(U_SPEC, "F"));
                        const Obj U(&functionF);
                        LOOP_ASSERT(ui, UU == U);
                      } break;
                      case 'G': {
                        ASSERT(0 == strcmp(U_SPEC, "G"));
                        const Obj U(&functionG);
                        LOOP_ASSERT(ui, UU == U);
                      } break;
                      case 'H': {
                        ASSERT(0 == strcmp(U_SPEC, "H"));
                        const Obj U(&functionH);
                        LOOP_ASSERT(ui, UU == U);
                      } break;
                      default: ASSERT(0);
                    }
                  } break;
                  case 2: {
                    ASSERT('0' <= U_SPEC[1] && U_SPEC[1] <= '9');
                    const int N = U_SPEC[1] - '0';

                    switch (U_SPEC[0]) {  // test here
                      case 'U': {
                        FunctorU fU; fU.d_value = N;
                        const Obj U(pointerWrapper(fU));
                        LOOP_ASSERT(ui, UU == U);
                      } break;
                      case 'V': {
                        FunctorV fV; fV.d_value = N;
                        const Obj U(pointerWrapper(fV));
                        LOOP_ASSERT(ui, UU == U);
                      } break;
                      case 'W': {
                        FunctorW fW; fW.d_value = N;
                        const Obj U(pointerWrapper(fW));
                        LOOP_ASSERT(ui, UU == U);
                      } break;
                      case 'S': {
                        FunctorS fS; fS.d_value = N;
                        const Obj U(fS);
                        LOOP_ASSERT(ui, UU == U);
                      } break;
                      case 'T': {
                        FunctorT fT; fT.d_value = N;
                        const Obj U(fT);
                        LOOP_ASSERT(ui, UU == U);
                      } break;
                      case 'P': {
                        FunctorP fP; fP.d_value = N;
                        const Obj U(pointerWrapper(fP));
                        LOOP_ASSERT(ui, UU == U);
                      } break;
                      case 'Q': {
                        FunctorQ fQ; fQ.d_value = N;
                        const Obj U(pointerWrapper(fQ));
                        LOOP_ASSERT(ui, UU == U);
                      } break;
                      case 'L': {
                        FunctorL fL; fL.d_value = N;
                        const Obj U(fL);
                        LOOP_ASSERT(ui, UU == U);
                      } break;
                      case 'M': {
                        FunctorM fM; fM.d_value = N;
                        const Obj U(fM);
                        LOOP_ASSERT(ui, UU == U);
                      } break;
                      default: ASSERT(0);
                    }
                  } break;
                  default: ASSERT(0);
                }
            }
        }

        if (verbose) cout << "\tWith allocators." << endl;
        {
            bslma_TestAllocator tb(veryVeryVerbose);
            bslma_Allocator *Z[] = { 0, &tb };

            bslma_DefaultAllocatorGuard guard(&ta);
            bslma_TestAllocator *ZZ[] = { &ta, &tb };

            const int NUM_ALLOCS = sizeof Z / sizeof *Z;

            for (int ui = 1; SPECS[ui]; ++ui) {
            for (int ua = 0; ua < NUM_ALLOCS; ++ua) {
                const char *const U_SPEC = SPECS[ui];

                if (verbose) {
                    cout << "\tFor lhs objects of spec ";
                    P_(U_SPEC);
                    cout << " with alloc " << ua << endl;
                }

                const Obj UU = g<Proto>(U_SPEC);  // control

                int NA[NUM_ALLOCS];
                for (int va = 0; va < NUM_ALLOCS; ++va) {
                    NA[va] = ZZ[va]->numBlocksTotal();
                }

                // Unfortunately, there is no way to test this in a loop
                // because we must instantiate the ctor with various 'FUNC'
                // template parameters.  A 'switch' statement will allow to
                // reuse the rest of the test loop infrastructure.  Since the
                // test must be performed inside the 'case' block, we use a
                // macro to avoid repetition:

#ifndef BDE_FUNCTION_PRIOR_1_16
    #define BDEF_FUNCTION_TEST_CTOR(U) do {                                   \
                    if (veryVerbose) {                                        \
                        cout << "\t\t\tafter loading: ";                      \
                        if (U.getAllocator() == Z[0]) cout << "ua = 0";       \
                        if (U.getAllocator() == Z[1]) cout << "ua = ta";      \
                        if (U.getAllocator() == Z[2]) cout << "ua = tb";      \
                        cout << endl;                                         \
                    }                                                         \
                    LOOP2_ASSERT(ui, ua, UU == U);                            \
                    LOOP2_ASSERT(ui, ua, U.getAllocator() ==                  \
                                            bslma_Default::allocator(Z[ua])); \
    } while(0)
#else
    #define BDEF_FUNCTION_TEST_CTOR(U) \
                    LOOP4_ASSERT(ui, ua, UU == U)

#endif
                switch (strlen(U_SPEC)) {
                  case 0: {
                    ASSERT(0);
                    const Obj U((Proto)0, Z[ua]);
                    BDEF_FUNCTION_TEST_CTOR(U);
                  } break;
                  case 1: {
                    switch (U_SPEC[0]) {  // test here
                      case 'F': {
                        ASSERT(0 == strcmp(U_SPEC, "F"));
                        const Obj U(&functionF, Z[ua]);
                        BDEF_FUNCTION_TEST_CTOR(U);
                      } break;
                      case 'G': {
                        ASSERT(0 == strcmp(U_SPEC, "G"));
                        const Obj U(&functionG, Z[ua]);
                        BDEF_FUNCTION_TEST_CTOR(U);
                      } break;
                      case 'H': {
                        ASSERT(0 == strcmp(U_SPEC, "H"));
                        const Obj U(&functionH, Z[ua]);
                        BDEF_FUNCTION_TEST_CTOR(U);
                      } break;
                      default: ASSERT(0);
                    }
                  } break;
                  case 2: {
                    ASSERT('0' <= U_SPEC[1] && U_SPEC[1] <= '9');
                    const int N = U_SPEC[1] - '0';

                    switch (U_SPEC[0]) {  // test here
                      case 'U': {
                        FunctorU fU; fU.d_value = N;
                        const Obj U(pointerWrapper(fU), Z[ua]);
                        BDEF_FUNCTION_TEST_CTOR(U);
                      } break;
                      case 'V': {
                        FunctorV fV; fV.d_value = N;
                        const Obj U(pointerWrapper(fV), Z[ua]);
                        BDEF_FUNCTION_TEST_CTOR(U);
                      } break;
                      case 'W': {
                        FunctorW fW; fW.d_value = N;
                        const Obj U(pointerWrapper(fW), Z[ua]);
                        BDEF_FUNCTION_TEST_CTOR(U);
                      } break;
                      case 'S': {
                        FunctorS fS; fS.d_value = N;
                        const Obj U(fS, Z[ua]);
                        BDEF_FUNCTION_TEST_CTOR(U);
                      } break;
                      case 'T': {
                        FunctorT fT; fT.d_value = N;
                        const Obj U(fT, Z[ua]);
                        BDEF_FUNCTION_TEST_CTOR(U);
                      } break;
                      case 'P': {
                        FunctorP fP; fP.d_value = N;
                        const Obj U(pointerWrapper(fP), Z[ua]);
                        BDEF_FUNCTION_TEST_CTOR(U);
                      } break;
                      case 'Q': {
                        FunctorQ fQ; fQ.d_value = N;
                        const Obj U(pointerWrapper(fQ), Z[ua]);
                        BDEF_FUNCTION_TEST_CTOR(U);
                      } break;
                      case 'L': {
                        FunctorL fL; fL.d_value = N;
                        const Obj U(fL, Z[ua]);
                        BDEF_FUNCTION_TEST_CTOR(U);
                      } break;
                      case 'M': {
                        FunctorM fM; fM.d_value = N;
                        const Obj U(fM, Z[ua]);
                        BDEF_FUNCTION_TEST_CTOR(U);
                      } break;
                      default: ASSERT(0);
                    }
                  } break;
                  default: ASSERT(0);
                }
#undef BDEF_FUNCTION_TEST_CTOR

                for (int va = 0; va < NUM_ALLOCS; ++va) {
                    if (va != ua) {
                        ASSERT(NA[va] == ZZ[va]->numBlocksTotal());
                    }
                    else if (!UU.isInplace()) {
                        // Because 'va == ua && !UU.isInplace())', we must have
                        // required allocation.

                        ASSERT(NA[va] <  ZZ[va]->numBlocksTotal());
                    }
                }
            }
            }
            ASSERT(0 == ta.numMismatches());
            ASSERT(0 == ta.numBytesInUse());
            ASSERT(0 == tb.numMismatches());
            ASSERT(0 == tb.numBytesInUse());
        }

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING STREAMING FUNCTIONALITY:
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "No 'bdex' streaming functionality." << endl
                          << "No testing need be performed." << endl;

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR:
        // We have the following concerns:
        //   1.  The value represented by any instance can be assigned to any
        //         other instance regardless of how either value is represented
        //         internally.
        //   2.  The 'rhs' value must not be affected by the operation.
        //   3.  'rhs' going out of scope has no effect on the value of 'lhs'
        //       after the assignment.
        //   4.  Aliasing (x = x): The assignment operator must always work --
        //         even when the lhs and rhs are identically the same object.
        //   5.  The assignment operator must be neutral with respect to memory
        //       allocation exceptions.
        //   6. The copy constructor's internal functionality varies
        //      according to which bitwise copy/move trait is applied.
        //
        // Plan:
        //   Specify a set S of unique object values with substantial and
        //   varied differences, ordered by increasing length.  For each value
        //   in S, construct an object x along with a sequence of similarly
        //   constructed duplicates x1, x2, ..., xN.  Attempt to affect every
        //   aspect of white-box state by altering each xi in a unique way.
        //   Let the union of all such objects be the set T.
        //
        //   To address concerns 1, 2, and 5, construct tests u = v for all
        //   (u, v) in T X T.  Using canonical controls UU and VV, assert
        //   before the assignment that UU == u, VV == v, and v == u iff
        //   VV == UU.  After the assignment, assert that VV == u, VV == v,
        //   and, for grins, that v == u.  Let v go out of scope and confirm
        //   that VV == u.  All of these tests are performed within the 'bdema'
        //   exception testing apparatus.  Since the execution time is lengthy
        //   with exceptions, every permutation is not performed when
        //   exceptions are tested.  Every permutation is also tested
        //   separately without exceptions.
        //
        //   As a separate exercise, we address 4 and 5 by constructing tests
        //   y = y for all y in T.  Using a canonical control X, we will verify
        //   that X == y before and after the assignment, again within
        //   the bdema exception testing apparatus.
        //
        //   To address concern 7, all these tests are performed on user
        //   defined types:
        //          Without allocator
        //          With allocator
        //
        // Testing:
        //   bdef_Function<PROTOTYPE>& operator=(const bdef_Function& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Assignment Operator" << endl
                          << "===========================" << endl;

        static const char *SPECS[] = {
            "",  "F", "G", "H",
            "U0", "U5", "U9", "V0", "V5", "V9", "W0", "W5", "W9",
            "S0", "S5", "S9", "T0", "T5", "T9", "P0", "P5", "P9",
            "Q0", "Q5", "Q9", "L0", "L5", "L9", "M0", "M5", "M9",
        0}; // Null string required as last element.

        const char *const PERTURB[] = {
            "",                         "~",
            "F",                        "F~",
            "FL9",        "F~L9",       "FL9~",      "F~L9~",
            "FL9F",       "F~L9F",      "FL9F~",     "F~L9F~",
            "FU1L2",      "FU1L2",      "FU1L2~",    "FU1~L2~",
            "FL9L1",      "FL9~L1",     "FL9L1~",    "FL9~L1~",
        0}; // Null string required as last element.

        if (verbose) cout <<
            "\nAssign cross product of values with varied representations.\n"
                          << "No Exceptions"
                          << endl;
        {
            for (int ui = 0; SPECS[ui]; ++ui) {
                const char *const U_SPEC = SPECS[ui];

                if (verbose) {
                    cout << "\tFor lhs objects of spec ";
                    P(U_SPEC);
                }

                const Obj UU = g<Proto>(U_SPEC);  // control

                for (int vi = 0; SPECS[vi]; ++vi) {
                    const char *const V_SPEC = SPECS[vi];

                    if (veryVerbose) {
                        cout << "\t\tFor rhs objects of spec ";
                        P(V_SPEC);
                    }

                    const Obj VV = g<Proto>(V_SPEC); // control

                    const int Z = ui == vi; // flag indicating same values

                    for (int uj = 0; PERTURB[uj]; ++uj) {
                        const char *U_N = PERTURB[uj];
                        for (int vj = 0; PERTURB[vj]; ++vj) {
                            const char *V_N = PERTURB[uj];

                            Obj mU(&testAllocator);
                            stretchRemoveAll(&mU, U_N);
                            // perturb mU

                            const Obj& U = mU;  gg(&mU, U_SPEC);
                            {
                            //--^
                            Obj mV(&testAllocator);
                            stretchRemoveAll(&mV, V_N);

                            const Obj& V = mV;  gg(&mV, V_SPEC);

                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, UU == U);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == V);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, Z==(V==U));

                            mU = V; // test assignment here

                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == U);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == V);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N,  V == U);
                            //--v
                            }
                            // 'mV' (and therefore 'V') now out of scope
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == U);
                        }
                    }
                }
            }
        }

        if (verbose) cout <<
            "\nAssign cross product of values with varied representations.\n"
                        << "With Exceptions"
                        << endl;
        {
            int iterationModulus = argc > 2 ? atoi(argv[2]) : 0;
            if (iterationModulus <= 0) iterationModulus = 11;
            int iteration = 0;
            iterationModulus = 1;

            for (int ui = 0; SPECS[ui]; ++ui) {
                const char *const U_SPEC = SPECS[ui];

                if (verbose) {
                    cout << "\tFor lhs objects of spec ";
                    P(U_SPEC);
                }

                const Obj UU = g<Proto>(U_SPEC);  // control

                for (int vi = 0; SPECS[vi]; ++vi) {
                    const char *const V_SPEC = SPECS[vi];

                    // control
                    const Obj VV = g<Proto>(V_SPEC);

                    for (int uj = 0; PERTURB[uj]; ++uj) {
                        const char *U_N = PERTURB[uj];
                        for (int vj = 0; PERTURB[vj]; ++vj) {
                            const char *V_N = PERTURB[vj];

                          if (iteration % iterationModulus == 0)
                          {
                          BEGIN_BSLMA_EXCEPTION_TEST {
                            const int AL = testAllocator.allocationLimit();
                            testAllocator.setAllocationLimit(-1);
                            Obj mU(&testAllocator);
                            stretchRemoveAll(&mU, U_N);
                            const Obj& U = mU;  gg(&mU, U_SPEC);
                            {
                            //--^
                            Obj mV(&testAllocator);
                            stretchRemoveAll(&mV, V_N);
                            const Obj& V = mV;  gg(&mV, V_SPEC);

                            testAllocator.setAllocationLimit(AL);
                            mU = V; // test assignment here

                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == U);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == V);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N,  V == U);
                            //--v
                            }
                            // 'mV' (and therefore 'V') now out of scope
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == U);
                          } END_BSLMA_EXCEPTION_TEST

                          }
                          ++iteration;
                        }
                    }
                }
            }
        }

        if (verbose) cout << "\nTesting self assignment (Aliasing)." << endl;
        {
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC = SPECS[ti];

                if (verbose) {
                    cout << "\tFor an object of spec ";
                    P(SPEC);
                }

                // control
                const Obj X = g<Proto>(SPEC);

                for (int tj = 0; PERTURB[tj]; ++tj) {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    const int AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);

                    const char *N = PERTURB[tj];

                    Obj mY(&testAllocator);
                    stretchRemoveAll(&mY, N);
                    const Obj& Y = mY; gg<Proto>(&mY, SPEC);

                    LOOP2_ASSERT(SPEC, N, Y == Y);
                    LOOP2_ASSERT(SPEC, N, X == Y);

                    testAllocator.setAllocationLimit(AL);
                    mY = Y; // test assignment here

                    LOOP2_ASSERT(SPEC, N, Y == Y);
                    LOOP2_ASSERT(SPEC, N, X == Y);

                  } END_BSLMA_EXCEPTION_TEST
                }
            }
        }

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTION g
        //   Since 'g' is implemented almost entirely using 'gg', we need to
        //   verify only that the arguments are properly forwarded, that 'g'
        //   does not affect the test allocator, and that 'g' returns an
        //   object by value.
        //
        // Plan:
        //   For each SPEC in a short list of specifications, compare the
        //   object returned (by value) from the generator function,
        //   'g(SPEC)' with the value of a newly constructed OBJECT
        //   configured using 'gg(&OBJECT, SPEC)'.  Compare the
        //   results of calling the allocator's 'numBlocksTotal' and
        //   'numBytesInUse' methods before and after calling 'g' in order to
        //   demonstrate that 'g' has no effect on the test allocator.
        //   Finally, use 'sizeof' to confirm that the (temporary) returned by
        //   'g' differs in size from that returned by 'gg'.
        //
        // Testing:
        //   bdef_Function<PROTOTYPE> g(const char *spec);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Generator Function 'g'" << endl
                          << "==============================" << endl;

        static const char *SPECS[] = {
            "",  "F", "G", "H",
            "U0", "U5", "U9", "V0", "V5", "V9", "W0", "W5", "W9",
            "S0", "S5", "S9", "T0", "T5", "T9", "P0", "P5", "P9",
            "Q0", "Q5", "Q9", "L0", "L5", "L9", "M0", "M5", "M9",
        0}; // Null string required as last element.

        if (verbose) cout <<
            "\nCompare values produced by 'g' and 'gg' for various inputs."
                                                                       << endl;
        for (int ti = 0; SPECS[ti]; ++ti) {
            const char *spec = SPECS[ti];
            if (veryVerbose) { P_(ti);  P(spec); }
            Obj mX(&testAllocator);
            const Obj& X = gg(&mX, spec);
            const int TOTAL_BLOCKS_BEFORE = testAllocator.numBlocksTotal();
            const int IN_USE_BYTES_BEFORE = testAllocator.numBytesInUse();
            const Obj Y = g<Proto>(spec);
            LOOP_ASSERT(ti, X == Y);
            const int TOTAL_BLOCKS_AFTER = testAllocator.numBlocksTotal();
            const int IN_USE_BYTES_AFTER = testAllocator.numBytesInUse();
            LOOP_ASSERT(ti, TOTAL_BLOCKS_BEFORE == TOTAL_BLOCKS_AFTER);
            LOOP_ASSERT(ti, IN_USE_BYTES_BEFORE == IN_USE_BYTES_AFTER);
        }

        if (verbose) cout << "\nConfirm return-by-value." << endl;
        {
            const char *spec = "L5";

            // compile-time fact
            ASSERT(sizeof(Obj) == sizeof g<Proto>(spec));

            Obj mX(&testAllocator);                      // runtime tests
            Obj& r1 = gg(&mX, spec);
            Obj& r2 = gg(&mX, spec);
            const Obj& r3 = g<Proto>(spec);
            const Obj& r4 = g<Proto>(spec);
            ASSERT(&r2 == &r1);
            ASSERT(&mX == &r1);
            ASSERT(&r4 != &r3);
            ASSERT(&mX != &r3);
        }

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR:
        //   We have the following concerns:
        //   1. The new object's value is the same as that of the original
        //       object (relying on the previously tested equality operators).
        //   2. All internal representations of a given value can be used to
        //        create a new object of equivalent value.
        //   3. The value of the original object is left unaffected.
        //   4. Subsequent changes in or destruction of the source object have
        //      no effect on the copy-constructed object.
        //   5. The function is exception neutral w.r.t. memory allocation.
        //   6. The object has its internal memory management system hooked up
        //         properly so that *all* internally allocated memory draws
        //         from a user-supplied allocator whenever one is specified.
        //   7. The copy constructor's internal functionality varies
        //      according to whether the object has an allocator.
        //
        // Plan:
        //   To address concerns 1 - 3, specify a set S of object values with
        //   substantial and varied differences, ordered by non-decreasing
        //   rank.
        //   For each value in S, initialize objects w and x, copy construct y
        //   from x and use 'operator==' to verify that both x and y
        //   subsequently have the same value as w.  Let x go out of scope and
        //   again verify that w == y.  Repeat this test with x having the same
        //   *logical* value, but perturbed so as to have potentially different
        //   internal representations.
        //
        //   To address concern 5, we will perform each of the above tests in
        //   the presence of exceptions during memory allocations using a
        //   'bslma_TestAllocator' and varying its *allocation* *limit*.
        //
        //   To address concern 6, we will repeat the above tests:
        //     - When passing in no allocator.
        //     - When passing in a null pointer: (bslma_Allocator *)0.
        //     - When passing in a test allocator (see concern 5).
        //     - Where the object is constructed entirely in static memory
        //       (using a 'bdema_BufferedSequentialAllocator') and never
        //       destroyed.
        //     - After the (dynamically allocated) source object is
        //       deleted and its footprint erased (see concern 4).
        //
        //   To address concern 7, all these tests are performed on user
        //   defined types:
        //          Without allocator
        //          With allocator
        //
        // Testing:
        //   bdef_Function(const bdef_Function& original);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Copy Constructor" << endl
                          << "========================" << endl;

        if (verbose) cout <<
            "\nCopy construct values with varied representations." << endl;
        {
            static const char *SPECS[] = {
                "",  "F", "G", "H",
                "U0", "U5", "U9", "V0", "V5", "V9", "W0", "W5", "W9",
                "S0", "S5", "S9", "T0", "T5", "T9", "P0", "P5", "P9",
                "Q0", "Q5", "Q9", "L0", "L5", "L9", "M0", "M5", "M9",
            0}; // Null string required as last element.

            const char *const PERTURB[] = {
                "",                         "~",
                "F",                        "F~",
                "FL9",        "F~L9",       "FL9~",      "F~L9~",
                "FL9F",       "F~L9F",      "FL9F~",     "F~L9F~",
                "FU1L2",      "FU1L2",      "FU1L2~",    "FU1~L2~",
                "FL9L1",      "FL9~L1",     "FL9L1~",    "FL9~L1~",
                "FL9M8",      "FL9~M8",     "FL9M8~",    "FL9~M8~",
                "FU1L9U1",    "FU1~L9U1",   "FU1L9U1~",  "FU1~L9U1~",
                "FU1L9U3",    "FU1~L9U3",   "FU1L9U3~",  "FU1~L9U3~",
            0}; // Null string required as last element.

            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC = SPECS[ti];

                if (verbose)
                    cout << "\tFor an object of spec " << SPEC << ":\t";

                // Create control object w.

                Obj mW(&testAllocator); gg(&mW, SPEC);
                const Obj& W = mW;

                // Perturb x object by different amounts.

                for (int ei = 0; PERTURB[ei]; ++ei) {
                    if (veryVerbose) { cout << "\t\t"; P(PERTURB[ei]) }

                    if (0 == ti) {
                        const int  LENGTH = strlen(PERTURB[ei]);
                        const char END    = ei ? PERTURB[ei][LENGTH - 1] : '~';
                        if ('~' != END) {
                            // Abort this iteration if perturbing an empty
                            // SPEC with a perturbation not also resulting into
                            // an empty function object.

                            continue;
                        }
                    }

                    Obj *pX = new Obj(&testAllocator);
                    Obj& mX = *pX;

                    char buf[100]; strcpy(buf, PERTURB[ei]); strcat(buf, SPEC);

                    const Obj& X = mX;  gg(&mX, buf);
                    {                                   // No allocator.
                        const Obj Y0(X);
                        LOOP2_ASSERT(SPEC, PERTURB[ei], W == Y0);
                        LOOP2_ASSERT(SPEC, PERTURB[ei], W == X);
                    }

                    {                                   // Null allocator.
                        const Obj Y1(X, (bslma_Allocator *) 0);
                        LOOP2_ASSERT(SPEC, PERTURB[ei], W == Y1);
                        LOOP2_ASSERT(SPEC, PERTURB[ei], W == X);
                    }

                    BEGIN_BSLMA_EXCEPTION_TEST {    // Test allocator
                        const Obj Y2(X, &testAllocator);
                        LOOP2_ASSERT(SPEC, PERTURB[ei], W == Y2);
                        LOOP2_ASSERT(SPEC, PERTURB[ei], W == X);
                    } END_BSLMA_EXCEPTION_TEST

                    {                               // Buffer Allocator.
                        char memory[8192];
                        bdema_BufferedSequentialAllocator a(memory,
                                                            sizeof memory);
                        Obj *Y = new(a.allocate(sizeof(Obj))) Obj(X, &a);
                        LOOP2_ASSERT(SPEC, PERTURB[ei], W == *Y);
                        LOOP2_ASSERT(SPEC, PERTURB[ei], W == X);
                    }

                    {                             // with 'original' destroyed
                        const Obj Y2(X, &testAllocator);
                        delete pX;
                        LOOP2_ASSERT(SPEC, PERTURB[ei], W == Y2);
                    }
                }
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY OPERATORS:
        //   Since 'operators==' is implemented in terms of basic accessors,
        //   it is sufficient to verify only that a difference in value of any
        //   one basic accessor for any two given objects implies inequality.
        //   However, to test that no other internal state information is
        //   being considered, we want also to verify that 'operator==' reports
        //   true when applied to any two objects whose internal
        //   representations may be different yet still represent the same
        //   (logical) value:
        //      - d_size
        //      - the (corresponding) amount of dynamically allocated memory
        //
        //   Note also that both equality operators must return either 1 or 0,
        //   and neither 'lhs' nor 'rhs' value may be modified.
        //
        // Plan:
        //   First specify a set S of unique object values having various minor
        //   or subtle differences, ordered by non-decreasing rank.  Verify
        //   the correctness of 'operator==' and 'operator!=' (returning either
        //   1 or 0) using all elements (u, v) of the cross product S X S.
        //
        //   Next specify a second set S' containing a representative variety
        //   of (black-box) box values ordered by non-decreasing rank.
        //   For each value in S', construct an object x along with a sequence
        //   of similarly constructed duplicates x1, x2, ..., xN.  Attempt to
        //   affect every aspect of white-box state by altering each xi in a
        //   unique way.  Verify correctness of 'operator==' and 'operator!='
        //   by asserting that each element in { x, x1, x2, ..., xN } is
        //   equivalent to every other element.
        //
        // Testing:
        //   operator==(const bdef_Function<T>&, const bdef_Function<T>&);
        //   operator!=(const bdef_Function<T>&, const bdef_Function<T>&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Equality Comparison" << endl
                          << "===========================" << endl;

        const struct {
            int         d_lineNum;    // source line number
            const char *d_spec_p;     // specification string
            int         d_valid;      // function object is valid
            int         d_isInplace;  // function object is in-place
            int         d_exp;        // return value for for invocation
        } DATA[] = {
        // lineNo  Spec                      Valid Inplace  Return value
        // ------  ----                      ----- -------  ------------
            { L_,  "",                          0,   1,     -1 }
          , { L_,  "F",                         1,   1,     VF }
          , { L_,  "G",                         1,   1,     VG }
          , { L_,  "H",                         1,   1,     VH }
          , { L_,  "U0",                        1,   1,     VU }
          , { L_,  "U5",                        1,   1,     VU }
          , { L_,  "U9",                        1,   1,     VU }
          , { L_,  "V0",                        1,   1,     VV }
          , { L_,  "V5",                        1,   1,     VV }
          , { L_,  "V9",                        1,   1,     VV }
          , { L_,  "W0",                        1,   1,     VW }
          , { L_,  "W5",                        1,   1,     VW }
          , { L_,  "W9",                        1,   1,     VW }
          , { L_,  "S0",                        1,   1,     VS }
          , { L_,  "S5",                        1,   1,     VS }
          , { L_,  "S9",                        1,   1,     VS }
          , { L_,  "T0",                        1,   1,     VT }
          , { L_,  "T5",                        1,   1,     VT }
          , { L_,  "T9",                        1,   1,     VT }
          , { L_,  "P0",                        1,   0,     VP }
          , { L_,  "P5",                        1,   0,     VP }
          , { L_,  "P9",                        1,   0,     VP }
          , { L_,  "Q0",                        1,   0,     VQ }
          , { L_,  "Q5",                        1,   0,     VQ }
          , { L_,  "Q9",                        1,   0,     VQ }
          , { L_,  "L0",                        1,   0,     VL }
          , { L_,  "L5",                        1,   0,     VL }
          , { L_,  "L9",                        1,   0,     VL }
          , { L_,  "M0",                        1,   0,     VM }
          , { L_,  "M5",                        1,   0,     VM }
          , { L_,  "M9",                        1,   0,     VM }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int         LINE     = DATA[ti].d_lineNum;
            const char *const SPEC     = DATA[ti].d_spec_p;

            if (veryVerbose) cout <<
                "\t\tSpec = \"" << SPEC << '"' << endl;

            Obj mX(&ta);
            const Obj& X = gg(&mX, SPEC);

            for (int tj = 0; tj < NUM_DATA ; ++tj) {
                const int         MORE_LINE     = DATA[tj].d_lineNum;
                const char *const MORE_SPEC     = DATA[tj].d_spec_p;

            if (veryVerbose) cout <<
                "\t\t\tAgainst spec = \"" << MORE_SPEC << '"' << endl;

                Obj mY(&ta);  const Obj& Y = mY;
                mY.clear();  gg(&mY, MORE_SPEC);

                LOOP4_ASSERT(LINE, MORE_LINE, ti, tj, (ti == tj) == (X == Y));
                LOOP4_ASSERT(LINE, MORE_LINE, ti, tj, (ti == tj) == (Y == X));
                LOOP4_ASSERT(LINE, MORE_LINE, ti, tj, (ti != tj) == (X != Y));
                LOOP4_ASSERT(LINE, MORE_LINE, ti, tj, (ti != tj) == (Y != X));

                static const char *const PREFIX = "~FGHV1T2Q3M4~";
                char buf[100]; strcpy(buf, PREFIX); strcat(buf, SPEC);

                Obj mZ(&ta);
                const Obj& Z = gg(&mZ, buf);

                LOOP4_ASSERT(LINE, MORE_LINE, ti, tj, (ti == tj) == (Y == Z));
                LOOP4_ASSERT(LINE, MORE_LINE, ti, tj, (ti == tj) == (Z == Y));
                LOOP4_ASSERT(LINE, MORE_LINE, ti, tj, (ti != tj) == (Y != Z));
                LOOP4_ASSERT(LINE, MORE_LINE, ti, tj, (ti != tj) == (Z != Y));

                char more_buf[100];
                strcpy(more_buf, PREFIX); strcat(more_buf, MORE_SPEC);

                Obj mW(&ta);
                const Obj& W = gg(&mW, more_buf);

                LOOP4_ASSERT(LINE, MORE_LINE, ti, tj, (ti == tj) == (Z == W));
                LOOP4_ASSERT(LINE, MORE_LINE, ti, tj, (ti == tj) == (W == Z));
                LOOP4_ASSERT(LINE, MORE_LINE, ti, tj, (ti != tj) == (Z != W));
                LOOP4_ASSERT(LINE, MORE_LINE, ti, tj, (ti != tj) == (W != Z));
            }
        }

        const char *const PERTURB[] = {
            "",                         "~",
            "F",                        "F~",
            "FL9",        "F~L9",       "FL9~",      "F~L9~",
            "FL9F",       "F~L9F",      "FL9F~",     "F~L9F~",
            "FU1L2",      "FU1L2",      "FU1L2~",    "FU1~L2~",
            "FL9L1",      "FL9~L1",     "FL9L1~",    "FL9~L1~",
            "FL9M8",      "FL9~M8",     "FL9M8~",    "FL9~M8~",
            "FU1L9U1",    "FU1~L9U1",   "FU1L9U1~",  "FU1~L9U1~",
            "FU1L9U3",    "FU1~L9U3",   "FU1L9U3~",  "FU1~L9U3~",
        0}; // Null string required as last element.

        for (int ti = 1; ti < NUM_DATA ; ++ti) {
            const int         LINE     = DATA[ti].d_lineNum;
            const char *const SPEC     = DATA[ti].d_spec_p;

            if (veryVerbose) cout <<
                "\t\tSpec = \"" << SPEC << '"' << endl;

            for (int i = 0; PERTURB[i]; ++i) {
                for (int j = 0; PERTURB[j]; ++j) {
                    Obj mX(&ta);  const Obj& X = gg(&mX, SPEC);
                    Obj mY(&ta);  const Obj& Y = gg(&mY, SPEC);

                    char buf[100];
                    strcpy(buf, PERTURB[i]); strcat(buf, SPEC);

                    char more_buf[100];
                    strcpy(more_buf, PERTURB[j]); strcat(more_buf, SPEC);

                    LOOP3_ASSERT(LINE, i, j, 1 == (X == Y));
                    LOOP3_ASSERT(LINE, i, j, 1 == (Y == X));
                    LOOP3_ASSERT(LINE, i, j, 0 == (X != Y));
                    LOOP3_ASSERT(LINE, i, j, 0 == (Y != X));
                }
            }
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR:
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "No output (<<) operator." << endl
                          << "No testing need be performed." << endl;

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING BASIC ACCESSORS:
        //   Having implemented an effective generation mechanism, we now
        //   would like to test thoroughly the basic accessor functions
        //     - operator bool() const
        //     - isInPlace() const
        //     - invocation on a const object
        //   Also, we want to ensure that various internal state
        //   representations for a given value produce identical results.
        //
        // Plan:
        //   Specify a set S of representative object values ordered by
        //   non-decreasing ranks.  For each value w in S, initialize a newly
        //   constructed object x with w using 'gg' and verify that each basic
        //   accessor returns the expected result.  Reinitialize and repeat
        //   the same test on an existing object y after perturbing y so as to
        //   achieve an internal state representation of w that is potentially
        //   different from that of x.
        //
        // Testing:
        //   bdef_Function::operator()(...) const;
        //   bdef_Function::operator bool() const;
        //   bdef_Function::isInplace() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Basic Accessors." << endl
                          << "========================" << endl;

        const struct {
            int         d_lineNum;    // source line number
            const char *d_spec_p;     // specification string
            int         d_valid;      // function object is valid
            int         d_isInplace;  // function object is in-place
            int         d_exp;        // return value for for invocation
        } DATA[] = {
        // lineNo  Spec                      Valid Inplace  Return value
        // ------  ----                      ----- -------  ------------
            { L_,  "",                          0,   1,     -1 }
          , { L_,  "F",                         1,   1,     VF }
          , { L_,  "G",                         1,   1,     VG }
          , { L_,  "H",                         1,   1,     VH }
          , { L_,  "U0",                        1,   1,     VU }
          , { L_,  "U5",                        1,   1,     VU }
          , { L_,  "U9",                        1,   1,     VU }
          , { L_,  "V0",                        1,   1,     VV }
          , { L_,  "V5",                        1,   1,     VV }
          , { L_,  "V9",                        1,   1,     VV }
          , { L_,  "W0",                        1,   1,     VW }
          , { L_,  "W5",                        1,   1,     VW }
          , { L_,  "W9",                        1,   1,     VW }
          , { L_,  "S0",                        1,   1,     VS }
          , { L_,  "S5",                        1,   1,     VS }
          , { L_,  "S9",                        1,   1,     VS }
          , { L_,  "T0",                        1,   1,     VT }
          , { L_,  "T5",                        1,   1,     VT }
          , { L_,  "T9",                        1,   1,     VT }
          , { L_,  "P0",                        1,   0,     VP }
          , { L_,  "P5",                        1,   0,     VP }
          , { L_,  "P9",                        1,   0,     VP }
          , { L_,  "Q0",                        1,   0,     VQ }
          , { L_,  "Q5",                        1,   0,     VQ }
          , { L_,  "Q9",                        1,   0,     VQ }
          , { L_,  "L0",                        1,   0,     VL }
          , { L_,  "L5",                        1,   0,     VL }
          , { L_,  "L9",                        1,   0,     VL }
          , { L_,  "M0",                        1,   0,     VM }
          , { L_,  "M5",                        1,   0,     VM }
          , { L_,  "M9",                        1,   0,     VM }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        Obj mY(&ta);  const Obj& Y = mY;

        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int         LINE     = DATA[ti].d_lineNum;
            const char *const SPEC     = DATA[ti].d_spec_p;
            const int         VALID    = DATA[ti].d_valid;
            const int         IN_PLACE = DATA[ti].d_isInplace;
            const int         EXP      = DATA[ti].d_exp;

            if (veryVerbose) cout <<
                "\t\tSpec = \"" << SPEC << '"' << endl;

            Obj mX(&ta);
            const Obj& X = gg(&mX, SPEC);

            if (VALID) {
                LOOP_ASSERT(LINE, X);
                LOOP_ASSERT(LINE, IN_PLACE == X.isInplace());
                LOOP_ASSERT(LINE, EXP == X(1));
            }
            else {
                LOOP_ASSERT(LINE, !X);
                LOOP_ASSERT(LINE, X.isInplace());
            }

            for (int tj = 0; tj < NUM_DATA ; ++tj) {
                const int         MORE_LINE     = DATA[tj].d_lineNum;
                const char *const MORE_SPEC     = DATA[tj].d_spec_p;

                mY.clear();  gg(&mY, MORE_SPEC);
                gg(&mY, SPEC);

                if (ti) {
                    // When !ti, the SPEC is "" and so Y is never overwritten
                    // from MORE_SPEC.

                    if (VALID) {
                        LOOP2_ASSERT(LINE, MORE_LINE, Y);
                        LOOP2_ASSERT(LINE, MORE_LINE,
                                                    IN_PLACE == Y.isInplace());
                        LOOP2_ASSERT(LINE, MORE_LINE, EXP == Y(1));
                    }
                    else {
                        LOOP2_ASSERT(LINE, MORE_LINE, !Y);
                        LOOP2_ASSERT(LINE, MORE_LINE, Y.isInplace());
                    }
                }
            }
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING PRIMITIVE GENERATOR FUNCTION gg
        //   Having demonstrated that our primary manipulators work as expected
        //   under normal conditions, we want to verify (1) that valid
        //   generator syntax produces expected results and (2) that invalid
        //   syntax is detected and reported.
        //
        //   We want also to make trustworthy some additional test helper
        //   functionality that we will use within the first 10 test cases:
        //
        //   Finally we want to make sure that we can rationalize the internal
        //   memory management with respect to the primary manipulators (i.e.,
        //   precisely when new blocks are allocated and deallocated as a
        //   result of in-place and out-of-place representations).
        //
        // Plan:
        //   For each of an enumerated sequence of 'spec' values, ordered by
        //   non-decreasing 'spec' rank, use the primitive generator function
        //   'gg' to set the state of a newly created object.  Verify that 'gg'
        //   returns a valid reference to the modified argument object and,
        //   using basic accessors, that the value of the object is as
        //   expected.  Repeat the test for a longer 'spec' generated by
        //   prepending a string ending in a '~' character (denoting
        //   'clear').  Note that we are testing the parser only; the
        //   primary manipulators are already assumed to work.
        //
        //   In order to be able to measure the internal memory used by the
        //   function wrapper, the functor chosen 'FUNC' is an object that
        //   does not allocate internal memory.
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Basic Accessors." << endl
                          << "========================" << endl;

        if (verbose) cout << "\nTesting generator on valid specs." << endl;
        {
            const struct {
                int         d_lineNum;    // source line number
                const char *d_spec_p;     // specification string
                int         d_valid;      // function object is valid
                int         d_isInplace;  // function object is in-place
                int         d_exp;        // return value for for invocation
            } DATA[] = {
            // lineNo  Spec                      Valid Inplace  Return value
            // ------  ----                      ----- -------  ------------
                // Empty functor
                { L_,  "",                          0,   1,     -1 }

                // Valid functors
              , { L_,  "F",                         1,   1,     VF }
              , { L_,  "G",                         1,   1,     VG }
              , { L_,  "H",                         1,   1,     VH }
              , { L_,  "U0",                        1,   1,     VU }
              , { L_,  "U5",                        1,   1,     VU }
              , { L_,  "U9",                        1,   1,     VU }
              , { L_,  "V0",                        1,   1,     VV }
              , { L_,  "V5",                        1,   1,     VV }
              , { L_,  "V9",                        1,   1,     VV }
              , { L_,  "W0",                        1,   1,     VW }
              , { L_,  "W5",                        1,   1,     VW }
              , { L_,  "W9",                        1,   1,     VW }
              , { L_,  "S0",                        1,   1,     VS }
              , { L_,  "S5",                        1,   1,     VS }
              , { L_,  "S9",                        1,   1,     VS }
              , { L_,  "T0",                        1,   1,     VT }
              , { L_,  "T5",                        1,   1,     VT }
              , { L_,  "T9",                        1,   1,     VT }
              , { L_,  "P0",                        1,   0,     VP }
              , { L_,  "P5",                        1,   0,     VP }
              , { L_,  "P9",                        1,   0,     VP }
              , { L_,  "Q0",                        1,   0,     VQ }
              , { L_,  "Q5",                        1,   0,     VQ }
              , { L_,  "Q9",                        1,   0,     VQ }
              , { L_,  "L0",                        1,   0,     VL }
              , { L_,  "L5",                        1,   0,     VL }
              , { L_,  "L9",                        1,   0,     VL }
              , { L_,  "M0",                        1,   0,     VM }
              , { L_,  "M5",                        1,   0,     VM }
              , { L_,  "M9",                        1,   0,     VM }

              // Multiple assignments
              , { L_,  "FGH",                       1,   1,     VH }
              , { L_,  "FU1S9P2M0",                 1,   0,     VM }

              // Final ~
              , { L_,  "~",                         0,   1,     -1 }
              , { L_,  "F~",                        0,   1,     -1 }
              , { L_,  "L0~",                       0,   1,     -1 }

              // Final ~ with multiple assignments
              , { L_,  "FGH~",                      0,   1,     -1 }
              , { L_,  "FU1S9P2M0~",                0,   1,     -1 }

              // Interspersed ~ with multiple assignments
              , { L_,  "F~GH",                      1,   1,     VH }
              , { L_,  "FU1~S9~P2M0",               1,   0,     VM }

              // Allowed for readability
              , { L_,  "F ~ G H",                   1,   1,     VH }
              , { L_,  "F U1 ~ S9 ~ P2 M0",         1,   0,     VM }
              , { L_,  "F~;G;H",                    1,   1,     VH }
              , { L_,  "F;U1;~;S9~;P2;M0",          1,   0,     VM }

            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int         LINE     = DATA[ti].d_lineNum;
                const char *const SPEC     = DATA[ti].d_spec_p;
                const int         VALID    = DATA[ti].d_valid;
                const int         IN_PLACE = DATA[ti].d_isInplace;
                const int         EXP      = DATA[ti].d_exp;

                if (veryVerbose) cout <<
                    "\t\tSpec = \"" << SPEC << '"' << endl;

                Obj mX(&ta);
                const Obj& X = gg(&mX, SPEC);

                if (VALID) {
                    LOOP_ASSERT(LINE, X);
                    LOOP_ASSERT(LINE, IN_PLACE == X.isInplace());
                    LOOP_ASSERT(LINE, EXP == X(1));
                }
                else {
                    LOOP_ASSERT(LINE, !X);
                    LOOP_ASSERT(LINE, X.isInplace());
                }

                static const char *const MORE_SPEC = "~FGHV1T2Q3M4~";
                char buf[100]; strcpy(buf, MORE_SPEC); strcat(buf, SPEC);

                Obj mY(&ta);
                const Obj& Y = gg(&mY, buf);

                if (VALID) {
                    LOOP_ASSERT(LINE, Y);
                    LOOP_ASSERT(LINE, IN_PLACE == Y.isInplace());
                    LOOP_ASSERT(LINE, EXP == Y(1));
                }
                else {
                    LOOP_ASSERT(LINE, !Y);
                    LOOP_ASSERT(LINE, Y.isInplace());
                }
            }
        }

        if (verbose) cout << "\nTesting generator on invalid specs." << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_index;    // offending character index
            } DATA[] = {
                { L_,   "",             -1     }, // control

                { L_,   "~",            -1     }, // control
                { L_,   "'",             0     },
                { L_,   ".",             0     },
                { L_,   "#",             0     },
                { L_,   "A",             0     },
                { L_,   "B",             0     },
                { L_,   "C",             0     },
                { L_,   "X",             0     },
                { L_,   "Y",             0     },
                { L_,   "Z",             0     },
                { L_,   "1",             0     },
                { L_,   "F1",            1     },
                { L_,   "U12",           2     },

                { L_,   "FG",           -1     }, // control
                { L_,   "U 1",           1     },
                { L_,   "U;1",           1     },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int INDEX        = DATA[ti].d_index;

                if (veryVerbose) cout <<
                    "\t\tSpec = \"" << SPEC << '"' << endl;

                Obj mX(&ta);  // const Obj& X = mX;
                int result = ggg(&mX, SPEC, veryVerbose);

                LOOP_ASSERT(LINE, INDEX == result);

                static const char *const MORE_SPEC = "~FGHV1T2Q3M4~";
                static const int         MORE_LEN  = strlen(MORE_SPEC);
                char buf[100]; strcpy(buf, MORE_SPEC); strcat(buf, SPEC);

                Obj mY(&ta);
                result = ggg(&mY, buf, veryVerbose);

                if (INDEX != -1) {
                    LOOP_ASSERT(LINE, MORE_LEN + INDEX == result);
                }
            }
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS (BOOTSTRAP):
        //   The basic concern is that the default constructor, the destructor,
        //   and, under normal conditions (i.e., no aliasing), the primary
        //   manipulators
        //     - operator=<FUNC>       (black-box)
        //     - clear                 (white-box)
        //   operate as expected.  We have the following specific concerns:
        //     1. The default Constructor
        //         1a. creates the correct initial value.
        //         1b. is exception neutral with respect to memory allocation.
        //         1c. has the internal memory management system hooked up
        //               properly so that *all* internally allocated memory
        //               draws from the same user-supplied allocator whenever
        //               one is specified.
        //         1d. operates correctly with 'FUNC' user defined types
        //               including those with and without 'bdema' allocators.
        //     2. The destructor properly deallocates all allocated memory to
        //          its corresponding allocator from any attainable state.
        //     3. 'operator=<FUNC>'
        //         3a. produces the expected value.
        //         3b. increases capacity as needed.
        //         3c. maintains valid internal state.
        //         3d. is exception neutral with respect to memory allocation.
        //     4. 'clear'
        //         4a. produces the expected value (empty).
        //         4b. properly destroys each contained element value.
        //         4c. maintains valid internal state.
        //         4d. does not allocate memory.
        //
        // Plan:
        //   To address concerns 1a - 1c, create an object using the default
        //   constructor:
        //    - With and without passing in an allocator.
        //    - In the presence of exceptions during memory allocations using
        //        a 'bslma_TestAllocator' and varying its *allocation* *limit*.
        //    - Where the object is constructed entirely in static memory
        //        (using a 'bdema_BufferedSequentialAllocator') and never
        //        destroyed.
        //
        //   To address concerns 3a - 3c, construct a series of independent
        //   objects, ordered by increasing ranks.  In each test, allow the
        //   object to leave scope without further modification, so that the
        //   destructor asserts internal object invariants appropriately.
        //   After the final assignment operation in each test, use the
        //   (untested) basic accessors to cross-check the value of the object
        //   and the 'bslma_TestAllocator' to confirm whether a transition from
        //   in-place to out-of-place has occurred.
        //
        //   To address concerns 4a-4c, construct a similar test, replacing
        //   assignment with 'clear'; this time, however, use the test
        //   allocator to record *numBlocksInUse* rather than *numBlocksTotal*.
        //
        //   To address concerns 2, 3d, 4d, create a small "area" test that
        //   exercises the construction and destruction of objects of various
        //   lengths and capacities in the presence of memory allocation
        //   exceptions.  Two separate tests will be performed.
        //
        //   Let S be a sequence of function invocables of increasing ranks,
        //   all returning their rank as an integer.
        //      (1) for each f in S, use the default constructor and
        //          assignment to create an instance of f, confirm its value
        //          (using basic accessors), and let it leave scope.
        //      (2) for each (f, g) in S x S, use 'operator=<FUNC>' to create
        //          an instance of f, use 'clear' to erase its value
        //          and confirm (with 'operator bool'), use assignment to set
        //          the instance to a value of g, verify the value, and allow
        //          the instance to leave scope.
        //
        //   The first test acts as a "control" in that 'clear' is not
        //   called; if only the second test produces an error, we know that
        //   'clear' is to blame.  We will rely on 'bslma_TestAllocator'
        //   and purify to address concern 2, and on the object invariant
        //   assertions in the destructor to address concerns 3d and 4d.
        //
        //   To address concern 1d, all these tests are performed on user
        //   defined types:
        //          Without allocator
        //          With allocator
        //
        // Testing:
        //   bdef_Function::operator=(FUNC const&);
        //   bdef_Function::clear();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Primary Manipulators." << endl
                          << "=============================" << endl;

        numAllocations = ta.numAllocations();

        if (verbose) cout << "\nTesting default ctor (thoroughly)." << endl;

        if (verbose) cout << "\tTesting ctor with no parameters." << endl;
        {
            const Obj X;
            ASSERT(!X);
        }

        if (verbose) cout << "\tTesting ctor with allocator." << endl;
        {
            const Obj X(&ta);
            ASSERT(!X);
        }
        ASSERT(numAllocations == ta.numAllocations());

        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTesting 'operator=' with default ctor." << endl;
        {
            if (verbose) cout << "\tOn an empty object." << endl;

            Obj mX; const Obj& X = mX;
            ASSERT(!X);

            FunctorV mV; mV.d_value = VV;
            mX = pointerWrapper(mV);

            ASSERT(X);
            ASSERT(X.isInplace());
            ASSERT(VV == X(1));
        }
        {
            Obj mX(&ta); const Obj& X = mX;
            ASSERT(!X);

            FunctorL mL; mL.d_value = VL;
            mX = mL;

            ASSERT(X);
            ASSERT(!X.isInplace());
            ASSERT(VL == X(1));
        }

        if (verbose) cout << "\nPassing in an allocator." << endl
                          << "\tWith no exceptions." << endl;

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            if (verbose) cout << "\t\tOn an empty object." << endl;

            Obj mX(&ta); const Obj& X = mX;
            ASSERT(!X);

            FunctorV mV; mV.d_value = VV;
            mX = pointerWrapper(mV);

            ASSERT(X);
            ASSERT(X.isInplace());
            ASSERT(VV == X(1));
        }
        ASSERT(numAllocations == ta.numAllocations());
        {
            Obj mX(&ta); const Obj& X = mX;
            ASSERT(!X);

            FunctorL mL; mL.d_value = VL;
            mX = mL;

            ASSERT(X);
            ASSERT(!X.isInplace());  ++numAllocations;
            ASSERT(VL == X(1));
        }
        ++numDeallocations;  // destruction when out of scope
        ASSERT(numAllocations   == ta.numAllocations());
        ASSERT(numDeallocations == ta.numDeallocations());
        {
            if (verbose) cout << "\t\tOn a non-empty object." << endl;

            Obj mX(&ta); const Obj& X = mX;
            ASSERT(!X);

            FunctorU mU; mU.d_value = VU;
            mX = mU;
            ASSERT(X);
            ASSERT(X.isInplace());
            ASSERT(VU == X(1));

            FunctorL mL; mL.d_value = VL;
            mX = mL;
            ASSERT(X);
            ASSERT(!X.isInplace());  ++numAllocations;
            ASSERT(VL == X(3));

            FunctorV mV; mV.d_value = VV;
            mX = pointerWrapper(mV);
            ASSERT(X);
            ASSERT(X.isInplace());   ++numDeallocations;
            ASSERT(VV == X(2));

            FunctorP mP; mP.d_value = VP;
            mX = mP;
            ASSERT(X);
            ASSERT(!X.isInplace());  ++numAllocations;
            ASSERT(VP == X(4));
        }
        ++numDeallocations;
        ASSERT(numAllocations - numDeallocations == ta.numAllocations() - ta.numDeallocations());

        if (verbose) cout << "\tIn place using a buffer allocator." << endl;
        {
            char memory[8192];
            bdema_BufferedSequentialAllocator a(memory, sizeof memory);
            void *doNotDelete = new(a.allocate(sizeof(Obj))) Obj(&a);
            ASSERT(doNotDelete);

            Obj& mX = *(Obj *)doNotDelete; const Obj& X = mX;
            ASSERT(!X);

            FunctorL mL; mL.d_value = VL;
            mX = mL;

            ASSERT(X);
            ASSERT(!X.isInplace());
            ASSERT(VL == X(1));

            // No destructor is called; will produce memory leak in purify
            // if internal allocators are not hooked up properly.
        }

        if (verbose) cout << "\tWith exceptions." << endl;
        {
            if (verbose) cout << "\t\tOn an empty object." << endl;

            Obj mX(&ta); const Obj& X = mX;
            ASSERT(!X);

            BEGIN_BSLMA_EXCEPTION_TEST {
                FunctorL mL; mL.d_value = VL;
                mX = mL;

                ASSERT(X);
                ASSERT(!X.isInplace());
                ASSERT(VL == X(1));
            } END_BSLMA_EXCEPTION_TEST
        }
        {
            if (verbose) cout << "\t\tOn a non-empty object." << endl;

            Obj mX(&ta); const Obj& X = mX;
            ASSERT(!X);

            BEGIN_BSLMA_EXCEPTION_TEST {
                FunctorU mU; mU.d_value = VU;
                mX = mU;
                ASSERT(X);
                ASSERT(X.isInplace());
                ASSERT(VU == X(1));

                FunctorL mL; mL.d_value = VL;
                mX = mL;
                ASSERT(X);
                ASSERT(!X.isInplace());
                ASSERT(VL == X(3));

                FunctorV mV; mV.d_value = VV;
                mX = pointerWrapper(mV);
                ASSERT(X);
                ASSERT(X.isInplace());
                ASSERT(VV == X(2));

                FunctorP mP; mP.d_value = VP;
                mX = mP;
                ASSERT(X);
                ASSERT(!X.isInplace());
                ASSERT(VP == X(4));
            } END_BSLMA_EXCEPTION_TEST
        }
        ASSERT(0 == ta.numBlocksInUse());
        ASSERT(0 == ta.numMismatches());

        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTesting 'clear' with default ctor." << endl;
        {
            if (verbose) cout << "\tOn an empty object." << endl;

            Obj mX; const Obj& X = mX;
            ASSERT(!X);

            mX.clear();

            ASSERT(!X);
            ASSERT(X.isInplace());
        }
        {
            if (verbose) cout << "\tOn an non-empty object." << endl;

            Obj mX(&ta); const Obj& X = mX;
            ASSERT(!X);

            FunctorS mS; mS.d_value = VS;
            mX = mS;

            mX.clear();
            ASSERT(!X);
            ASSERT(X.isInplace());

            FunctorL mL; mL.d_value = VL;
            mX = mL;

            mX.clear();
            ASSERT(!X);
            ASSERT(X.isInplace());

            mX = mL;
            ASSERT(X);

            mX.clear();
            ASSERT(!X);
            ASSERT(X.isInplace());
        }

        if (verbose) cout << "\nPassing in an allocator." << endl;
        {
            if (verbose) cout << "\tOn an empty object." << endl;

            Obj mX(&ta); const Obj& X = mX;
            ASSERT(!X);

            mX.clear();

            ASSERT(!X);
            ASSERT(X.isInplace());
        }
        ASSERT(0 == ta.numBlocksInUse());
        {
            if (verbose) cout << "\tOn an non-empty object." << endl;

            Obj mX(&ta); const Obj& X = mX;
            ASSERT(!X);

            FunctorS mS; mS.d_value = VS;
            mX = mS;
            ASSERT(X);
            ASSERT(X.isInplace());
            ASSERT(0 == ta.numBlocksInUse());

            mX.clear();
            ASSERT(!X);
            ASSERT(X.isInplace());
            ASSERT(0 == ta.numBlocksInUse());

            FunctorL mL; mL.d_value = VL;
            mX = mL;
            ASSERT(X);
            ASSERT(!X.isInplace());
            ASSERT(0 < ta.numBlocksInUse());

            mX.clear();
            ASSERT(!X);
            ASSERT(X.isInplace());
            ASSERT(0 == ta.numBlocksInUse());

            mX = mL;
            ASSERT(X);
            ASSERT(!X.isInplace());
            ASSERT(0 < ta.numBlocksInUse());

            mX.clear();
            ASSERT(!X);
            ASSERT(X.isInplace());
            ASSERT(0 == ta.numBlocksInUse());
        }
        ASSERT(0 == ta.numBlocksInUse());

        if (verbose) cout << "\tWith exceptions." << endl;
        {
            if (verbose) cout << "\tOn an non-empty object." << endl;

            Obj mX(&ta); const Obj& X = mX;
            ASSERT(!X);

            BEGIN_BSLMA_EXCEPTION_TEST {
                FunctorS mS; mS.d_value = VS;
                mX = mS;
                ASSERT(X);
                ASSERT(X.isInplace());
                ASSERT(0 == ta.numBlocksInUse());

                mX.clear();
                ASSERT(!X);
                ASSERT(X.isInplace());
                ASSERT(0 == ta.numBlocksInUse());

                FunctorL mL; mL.d_value = VL;
                mX = mL;
                ASSERT(X);
                ASSERT(!X.isInplace());
                ASSERT(0 < ta.numBlocksInUse());

                mX.clear();
                ASSERT(!X);
                ASSERT(X.isInplace());
                ASSERT(0 == ta.numBlocksInUse());

                mX = mL;
                ASSERT(X);
                ASSERT(!X.isInplace());
                ASSERT(0 < ta.numBlocksInUse());

                mX.clear();
                ASSERT(!X);
                ASSERT(X.isInplace());
                ASSERT(0 == ta.numBlocksInUse());
            } END_BSLMA_EXCEPTION_TEST
        }
        ASSERT(0 == ta.numBlocksInUse());

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        // Developer's sandbox
        //
        // Plan:
        //   - Basic value-semantic breathing test:
        //   1. Create an object x1 (init. to VA).   { x1:VA }
        //   2. Create an object x2 (copy from x1).  { x1:VA x2:VA }
        //   3. Set x1 to VB.                        { x1:VB x2:VA }
        //   4. Create an object x3 (default ctor).  { x1:VB x2:VA x3:U }
        //   5. Create an object x4 (copy from x3).  { x1:VB x2:VA x3:U  x4:U }
        //   6. Set x3 to VC.                        { x1:VB x2:VA x3:VC x4:U }
        //   7. Assign x2 = x1.                      { x1:VB x2:VB x3:VC x4:U }
        //   8. Assign x2 = x3.                      { x1:VB x2:VC x3:VC x4:U }
        //   9. Assign x1 = x1 (aliasing).           { x1:VB x2:VB x3:VC x4:U }
        //   - Make sure that a simple void function or member function is
        //     created in-place.
        //   - Make sure that allocator is propagated to function object when
        //     it is copied into the 'bdef_Function' functor.
        //   - Make sure that a simple void function or member function is
        //     created in-place, even with four additional arguments.
        //
        // Testing:
        //   This test exercises basic functionality, but *tests* NOTHING.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        if (verbose) cout << "Value semantic testing.\n";

        Obj VA = &functionF;
        Obj VB = &functionG;
        Obj VC = &functionH;

        EqualityComparisonUtil::registerEqualityManager(&functionF);
        EqualityComparisonUtil::registerEqualityManager(&functionG);
        EqualityComparisonUtil::registerEqualityManager(&functionH);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 1. Create an object x1 (init. to VA)."
                             "\t\t{ x1:VA }" << endl;
        Obj mX1(VA);  const Obj& X1 = mX1;

        if (verbose) cout << "\ta. Check initial state of x1." << endl;
        ASSERT(VF == X1(1));
        ASSERT(1  == lastArgF);

        if (verbose) cout <<
            "\tb. Try equality operators: x1 <op> x1." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 2. Create an object x2 (copy from x1)."
                             "\t\t{ x1:VA x2:VA }" << endl;
        Obj mX2(X1);  const Obj& X2 = mX2;

        if (verbose) cout << "\ta. Check the initial state of x2." << endl;
        ASSERT(VF == X2(2));
        ASSERT(2  == lastArgF);

        if (verbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 3. Set x1 to a new value VB."
                             "\t\t\t{ x1:VB x2:VA }" << endl;
        mX1 = VB;

        if (verbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(VG == X1(3));
        ASSERT(2  == lastArgF);
        ASSERT(3  == lastArgG);

        if (verbose) cout <<
            "\tb. Try equality operators: x1 <op> x1, x2." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 4. Create a default object x3()."
                             "\t\t{ x1:VB x2:VA x3:U }" << endl;
        Obj mX3;  const Obj& X3 = mX3;

        if (verbose) cout << "\ta. Check initial state of x3." << endl;
        ASSERT(!X3);

        if (verbose) cout <<
            "\tb. Try equality operators: x3 <op> x1, x2, x3." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 5. Create an object x4 (copy from x3)."
                             "\t\t{ x1:VA x2:VA x3:U  x4:U }" << endl;
        Obj mX4(X3);  const Obj& X4 = mX4;

        if (verbose) cout << "\ta. Check initial state of x4." << endl;
        ASSERT(!X4);

        if (verbose) cout <<
            "\tb. Try equality operators: x4 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X4 == X1));        ASSERT(1 == (X4 != X1));
        ASSERT(0 == (X4 == X2));        ASSERT(1 == (X4 != X2));
        ASSERT(1 == (X4 == X3));        ASSERT(0 == (X4 != X3));
        ASSERT(1 == (X4 == X4));        ASSERT(0 == (X4 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 6. Set x3 to a new value VC."
                             "\t\t\t{ x1:VB x2:VA x3:VC x4:U }" << endl;
        mX3 = VC;

        if (verbose) cout << "\ta. Check new state of x3." << endl;
        ASSERT(VH == X3(4));
        ASSERT(2  == lastArgF);
        ASSERT(3  == lastArgG);
        ASSERT(4  == lastArgH);

        if (verbose) cout <<
            "\tb. Try equality operators: x3 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));
        ASSERT(0 == (X3 == X4));        ASSERT(1 == (X3 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 7. Assign x2 = x1."
                             "\t\t\t\t{ x1:VB x2:VB x3:VC x4:U }" << endl;
        mX2 = X1;

        if (verbose) cout << "\ta. Check new state of x2." << endl;
        ASSERT(VG == X2(5));
        ASSERT(2  == lastArgF);
        ASSERT(5  == lastArgG);
        ASSERT(4  == lastArgH);

        if (verbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(0 == (X2 == X3));        ASSERT(1 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 8. Assign x2 = x3."
                             "\t\t\t\t{ x1:VB x2:VC x3:VC x4:U }" << endl;
        mX2 = X3;

        if (verbose) cout << "\ta. Check new state of x2." << endl;
        ASSERT(VH == X2(6));
        ASSERT(2  == lastArgF);
        ASSERT(5  == lastArgG);
        ASSERT(6  == lastArgH);

        if (verbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X2 == X1));        ASSERT(1 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(1 == (X2 == X3));        ASSERT(0 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 9. Assign x1 = x1 (aliasing)."
                             "\t\t\t{ x1:VB x2:VC x3:VC x4:U }" << endl;

        mX1 = X1;

        if (verbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(VG == X1(7));
        ASSERT(2  == lastArgF);
        ASSERT(7  == lastArgG);
        ASSERT(6  == lastArgH);

        if (verbose) cout <<
            "\tb. Try equality operators: x1 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT(0 == (X1 == X4));        ASSERT(1 == (X1 != X4));

        // = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

        if (verbose) cout << "\nTesting various types of invocables.\n";

        bdef_Function<int (*)(int)> Z((bslma_Allocator*)0);
        ASSERT((!Z));
        {
            if (verbose) cout << "\tFree function pointer.\n";
            numAllocations = ta.numAllocations();
            numDeallocations = ta.numDeallocations();
            {
                bslma_DefaultAllocatorGuard guard(&ta);
                numAllocations = ta.numAllocations();
                bdef_Function<int (*)()> mX;
                bdef_Function<int (*)()> const &X = mX;

                ASSERT(numAllocations   == ta.numAllocations());
                ASSERT(numDeallocations == ta.numDeallocations());

                typedef int (*FUNC)();
                ASSERT(1 == bdef_FunctionUtil::IsInplace<FUNC>::VALUE);

                ASSERT(!X);
                mX = &freeFunc0;
                ASSERT(X);
                ASSERT(0 == X());
                ASSERT(X.isInplace());
                ASSERT(numAllocations   == ta.numAllocations());
                ASSERT(numDeallocations == ta.numDeallocations());
            }
            ASSERT(numAllocations   == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());

            if (verbose) cout << "\tCustom function object.\n";
            numAllocations = ta.numAllocations();
            numDeallocations = ta.numDeallocations();
            {
                bslma_DefaultAllocatorGuard guard(&ta);
                numAllocations = ta.numAllocations();
                bdef_Function<int (*)()> mX;
                bdef_Function<int (*)()> const &X = mX;

                ASSERT(numAllocations   == ta.numAllocations());
                ASSERT(numDeallocations == ta.numDeallocations());

                typedef MyFunctionObject FUNC;
                FUNC obj; const FUNC& OBJ = obj;
                ASSERT(0 == bdef_FunctionUtil::IsInplace<FUNC>::VALUE);

                ASSERT(!X);
                mX = OBJ;
                ASSERT(X);
                ASSERT(0 == X());
                ASSERT(false == X.isInplace());
                ASSERT(++numAllocations == ta.numAllocations());
                ASSERT(numDeallocations == ta.numDeallocations());
            }
            ASSERT(numAllocations     == ta.numAllocations());
            ASSERT(++numDeallocations == ta.numDeallocations());

            const unsigned SIZE = bdef_FunctionUtil::MAX_INPLACE_OBJECT_SIZE;
            ASSERT(SIZE >= sizeof &MyFunctionObject::memFunc0 +
                                                            4 * sizeof(void*));

            if (verbose) cout << "\tSmall (inplace) function object.\n";
            numAllocations = ta.numAllocations();
            numDeallocations = ta.numDeallocations();
            {
                bslma_DefaultAllocatorGuard guard(&ta);
                numAllocations = ta.numAllocations();
                bdef_Function<int (*)()> mX;
                bdef_Function<int (*)()> const &X = mX;

                ASSERT(numAllocations   == ta.numAllocations());
                ASSERT(numDeallocations == ta.numDeallocations());

                typedef MySizeFunctionObject<SIZE> FUNC;
                FUNC obj; const FUNC& OBJ = obj;
                ASSERT(SIZE == sizeof OBJ);
                ASSERT(1 == bdef_FunctionUtil::IsInplace<FUNC>::VALUE);

                ASSERT(!X);
                mX = OBJ;
                ASSERT(X);
                ASSERT((int)SIZE == X());
                ASSERT(X.isInplace());
                ASSERT(numAllocations   == ta.numAllocations());
                ASSERT(numDeallocations == ta.numDeallocations());
            }
            ASSERT(numAllocations   == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());

            if (verbose) cout << "\tMember function pointer + instance.\n";
            numAllocations = ta.numAllocations();
            numDeallocations = ta.numDeallocations();
            {
                bslma_DefaultAllocatorGuard guard(&ta);
                numAllocations = ta.numAllocations();
                bdef_Function<int (*)()> mX;
                bdef_Function<int (*)()> const &X = mX;

                ASSERT(numAllocations   == ta.numAllocations());
                ASSERT(numDeallocations == ta.numDeallocations());

                MyFunctionObject obj;

                typedef bdef_MemFnInstance<int (MyFunctionObject::*)(),
                                           MyFunctionObject*> FUNC;
                ASSERT(1 == bdef_FunctionUtil::IsInplace<FUNC>::VALUE);

                ASSERT(!X);
                mX = bdef_MemFnUtil::memFn(&MyFunctionObject::memFunc0, &obj);
                ASSERT(X);
                ASSERT(0 == X());
                ASSERT(X.isInplace());
                ASSERT(numAllocations   == ta.numAllocations());
                ASSERT(numDeallocations == ta.numDeallocations());
            }
            ASSERT(numAllocations   == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());

            if (verbose) cout << "\tLarge (not in-place) function object.\n";
            numAllocations = ta.numAllocations();
            numDeallocations = ta.numDeallocations();
            {
                bslma_DefaultAllocatorGuard guard(&ta);
                numAllocations = ta.numAllocations();
                bdef_Function<int (*)()> mX;
                bdef_Function<int (*)()> const &X = mX;

                ASSERT(numAllocations   == ta.numAllocations());
                ASSERT(numDeallocations == ta.numDeallocations());

                typedef MySizeFunctionObject<SIZE + 1> FUNC;
                FUNC obj; const FUNC& OBJ = obj;
                ASSERT(SIZE < sizeof OBJ);
                ASSERT(0 == bdef_FunctionUtil::IsInplace<FUNC>::VALUE);

                ASSERT(!X);
                mX = OBJ;
                ASSERT(X);
                ASSERT((int)SIZE + 1 == X());
                ASSERT(false == X.isInplace());
                ASSERT(++numAllocations == ta.numAllocations());
                ASSERT(numDeallocations == ta.numDeallocations());
            }
            ASSERT(numAllocations     == ta.numAllocations());
            ASSERT(++numDeallocations == ta.numDeallocations());

            if (verbose) {
                cout << "\nDisplaying memory footprints.\n";

                T_ P(sizeof(void*));
                T_ P(sizeof &freeFunc0);
                T_ P(sizeof &MyFunctionObject::memFunc0);
                T_ P(sizeof(bsls_AlignmentUtil::MaxAlignedType));
                T_ P(sizeof(int));
                T_ P(sizeof(bdef_Function<int (*)()>));
                T_ P(bdef_FunctionUtil::MAX_INPLACE_OBJECT_SIZE);
            }
        }

      } break;
      case -1: {
        // --------------------------------------------------------------------
        // TEST SPEED OF CALLING NOP THROUGH 'bdef_Function'
        //
        // Concern:
        //   What is the cpu overhead of calling a 'bdef_Function'?
        //
        // Plan:
        //   Load a 'bdef_Function' object with a nop functor and call it
        //   repeatdly.  Time how long this takes.
        // --------------------------------------------------------------------

        cout << "'bdef_Function' nop benchmark\n"
                "=============================\n";

        int iterations;
        cout << "Enter Iterations: " << flush;
        cin >> iterations;

        bdef_Function<void (*)()> f((FunctorNop()));

        bsls_Stopwatch sw;
        sw.start(true);

        for (int i = iterations; i > 0; --i) {
            f();
        }

        sw.stop();

        double user = sw.accumulatedUserTime() / iterations;
        double wall = sw.accumulatedWallTime() / iterations;

        cout << "User time: " << user << " sec, Wall time: " << wall <<
                                                                      " sec\n";
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }

    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
