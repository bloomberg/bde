// bdea_array.t.cpp     -*-C++-*-

#include <bdea_array.h>

#include <bdes_platformutil.h>                  // for testing only
#include <bdema_bufferallocator.h>             // for testing only
#include <bdema_testallocator.h>               // for testing only
#include <bdema_testallocatorexception.h>      // for testing only
#include <bdex_testoutstream.h>                // for testing only
#include <bdex_testinstream.h>                 // for testing only
#include <bdex_testinstreamexception.h>        // for testing only
#include <bdeimp_fussy.h>                      // for testing only

#include <new>         // placement syntax
#include <cstring>     // strlen(), memset(), memcpy(), memcmp()
#include <cstdlib>     // atoi()
#include <iostream>
#include <strstream>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This test plan follows the standard approach for components implementing
// value-semantic containers.  We have chosen as *primary* *manipulators* the
// 'append' and 'removeAll' methods to be used by the generator functions
// 'g' and 'gg'.  Additional helper functions are provided to facilitate
// perturbation of internal state (e.g., capacity).  Note that each
// manipulator must support aliasing, and those that perform memory allocation
// must be tested for exception neutrality via the 'bdema_testallocator'
// component.  Exception neutrality involving streaming is verified using
// 'bdex_testinstream' (and 'bdex_testoutstream').
//
// Note that places where test drivers in this family are likely to require
// adjustment are indicated by the tag: "ADJ".
//
//-----------------------------------------------------------------------------
// [ 2] bdea_Array<T>(bdema_Allocator *ba = 0);
// [11] bdea_Array<T>(const Explicit& iLen, bdema_Allocator *ba = 0);
// [11] bdea_Array<T>(int iLen, const bdet_String& iVal, *ba = 0);
// [16] bdea_Array<T>(const InitialCapacity& ne, *ba = 0);
// [11] bdea_Array<T>(const bdet_String *sa, int ne, *ba = 0);
// [ 7] bdea_Array<T>(const bdea_Array<T>& original, *ba = 0);
// [ 2] ~bdea_Array<T>();
// [ 9] bdea_Array<T>& operator=(const bdea_Array<T>& rhs);
// [14] int& operator[](int index);
// [13] void append(bdet_String & item);
// [13] void append(const bdea_Array<T>& sa);
// [13] void append(const bdea_Array<T>& sa, int si, int ne);
// [18] T *data();
// [13] void insert(int di, const bdet_String& item);
// [13] void insert(int di, const bdea_Array<T>& sa);
// [13] void insert(int di, const bdea_Array<T>& sa, int si, int ne);
// [13] void remove(int index);
// [13] void remove(int index, int ne);
// [ 2] void removeAll();
// [14] void replace(int di, const bdet_String& item);
// [14] void replace(int di, const bdea_Array<T>& sa, int si, int ne);
// [16] void reserveCapacityRaw(int ne);
// [16] void reserveCapacity(int ne);
// [12] void setLength(int newLength);
// [12] void setLength(int newLength, const bdet_String& iVal);
// [10] bdex_InStream& streamIn(bdex_InStream& stream);
// [15] void swap(int index1, int index2);
// [ 4] const int& operator[](int index) const;
// [18] const T *data() const;
// [ 4] int length() const;
// [15] ostream& print(ostream& stream, int level, int spl);
// [10] bdex_OutStream& streamOut(bdex_OutStream& stream) const;
//
// [ 6] operator==(const bdea_Array<T>&, const bdea_Array<T>&);
// [ 6] operator!=(const bdea_Array<T>&, const bdea_Array<T>&);
// [ 5] operator<<(ostream&, const bdea_Array<T>&);
// [10] operator>>(bdex_InStream&, bdea_Array<T>&);
// [10] operator<<(bdex_OutStream&, const bdea_Array<T>&);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [17] USAGE EXAMPLE
// [ 2] BOOTSTRAP: void append(bdet_String & item); // no aliasing
// [ 3] CONCERN: Is the internal memory organization behaving as intended?
//
// [ 3] void stretch(Obj *object, int size, const Element & elem);
// [ 3] void stretchRemoveAll(Obj *object, int size, const Element & elem);
// [ 3] int ggg<T>(bdea_Array<T> *o, const char *s, const T *v, int vF = 1);
// [ 3] bdea_Array<T>& gg<T>(bdea_Array<T> *o, const char *s, const T *v);
// [ 8] bdea_Array<T> g<T>(const char *spec, const T *values);

//=============================================================================
//                     STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

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

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  STANDARD BDEMA EXCEPTION TEST MACROS
//-----------------------------------------------------------------------------

#ifdef BDE_BUILD_TARGET_EXC
#define BEGIN_BDEMA_EXCEPTION_TEST {                                       \
    {                                                                      \
        static int firstTime = 1;                                          \
        if (veryVerbose && firstTime) cout <<                              \
            "### BDEMA EXCEPTION TEST -- (ENABLED) --" << endl;            \
        firstTime = 0;                                                     \
    }                                                                      \
    if (veryVeryVerbose) cout <<                                           \
        "### Begin bdema exception test." << endl;                         \
    int bdemaExceptionCounter = 0;                                         \
    static int bdemaExceptionLimit = 100;                                  \
    testAllocator.setAllocationLimit(bdemaExceptionCounter);               \
    do {                                                                   \
        try {

#define END_BDEMA_EXCEPTION_TEST                                           \
        } catch (bdema_TestAllocatorException& e) {                        \
            if (veryVerbose && bdemaExceptionLimit || veryVeryVerbose) {   \
                --bdemaExceptionLimit;                                     \
                cout << "(*** " <<                                         \
                bdemaExceptionCounter << ')';                              \
                if (veryVeryVerbose) { cout << " BEDMA_EXCEPTION: "        \
                    << "alloc limit = " << bdemaExceptionCounter << ", "   \
                    << "last alloc size = " << e.numBytes();               \
                }                                                          \
                else if (0 == bdemaExceptionLimit) {                       \
                     cout << " [ Note: 'bdemaExceptionLimit' reached. ]";  \
                }                                                          \
                cout << endl;                                              \
            }                                                              \
            testAllocator.setAllocationLimit(++bdemaExceptionCounter);     \
            continue;                                                      \
        }                                                                  \
        testAllocator.setAllocationLimit(-1);                              \
        break;                                                             \
    } while (1);                                                           \
    if (veryVeryVerbose) cout <<                                           \
        "### End bdema exception test." << endl;                           \
}
#else
#define BEGIN_BDEMA_EXCEPTION_TEST                                         \
{                                                                          \
    static int firstTime = 1;                                              \
    if (verbose && firstTime) { cout <<                                    \
        "### BDEMA EXCEPTION TEST -- (NOT ENABLED) --" << endl;            \
        firstTime = 0;                                                     \
    }                                                                      \
}
#define END_BDEMA_EXCEPTION_TEST
#endif

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
//
//   Objects with the BitwiseCopyable trait will always be initialized
//   with the default constructor or a bitwise copy.  The array never
//   invokes the copy constructor or the destructor.
//
//   Objects with the BitwiseMoveable trait will always be moved
//   with a bitwise copy or move.  Moving objects will never use the
//   copy constructor with the destructor.
//
//   The static member functions getCopyCtorCount() and getDtorCount()
//   return the number of copy ctors or dtors run for this class.
//
//   The static member function verify Algorithm uses the copy constructor
//   and destructor counts to verify that the most optimal algorithm is
//   being applied based on the the copyable and moveable traits.
//   This function returns 1 if the copy ctor and dtor counts are appropriate.
//

namespace BloombergLP {

static void detectFussy (const bdeimp_Fussy *current,
                         const bdeimp_Fussy *cached)
{
    // Callback function from the fussy object.  Called when the fussy
    // object detects that an object that cannot be moved using a
    // bitwise move (e.g. memmove) was in fact moved.

    cout << "Object that cannot be moved was moved\n";
    ASSERT(current == cached);
}

//-----------------------------------------------------------------------------
//
//     Test classes:
//       TestAllocCopy      uses bdema_allocator           bitwise copyable
//       TestAllocMove      uses bdema_allocator           bitwise moveable
//       TestAllocNoMove    uses bdema_allocator           not bitwise moveable
//       TestNoAllocCopy    does not use bdema_allocator   bitwise copyable
//       TestNoAllocMove    does not use bdema_allocator   bitwise moveable
//       TestNoAllocNoMove  does not use bdema_allocator   not bitwise moveable
//
//
//     Test classes with an allocator.
//     -------------------------------
//
// Supports the bdema_allocator.  This class does not have the BitwiseCopyable
// or BitwiseMoveable traits.
//
// A member variable of type bdeimp_Fussy is used to detect if the object
// is moved using a bitwise move.
//
class TestAllocNoMove {
  private:
    static int copyCtorCt;               // count of calls to copy ctor
    static int dtorCt;                   // count of calls to dtor

    int              value;              // a "unique" value for the object
    int             *pointer;            // allocate an integer
    bdeimp_Fussy     fussy;              // detects moving the object
    bdema_Allocator *d_allocator_p;

  public:

    static int getCopyCtorCt() { return copyCtorCt; }
    static int getDtorCt() { return dtorCt; }
    static int verifyAlgorithm(int numCtor, int numDtor, int numInserts,
         int numRemoves);
    static int maxSupportedBdexVersion() { return 1; }

    TestAllocNoMove(int ivalue, bdema_Allocator *basicAllocator=0);
    TestAllocNoMove(const TestAllocNoMove& old,
                    bdema_Allocator *basicAllocator=0);
    TestAllocNoMove(bdema_Allocator *basicAllocator=0);
    ~TestAllocNoMove();

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version)
    {
        stream >> value;
        return stream;
    }

    template <class STREAM>
    STREAM& streamInRaw(STREAM& stream, int version)
    {
        return bdexStreamIn(stream, version);
    }

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const
    {
        stream << value;
        return stream;
    }

    template <class STREAM>
    STREAM& streamOutRaw(STREAM& stream, int version) const
    {
        return bdexStreamOut(stream, version);
    }

    TestAllocNoMove& operator=(const TestAllocNoMove&);
    friend int operator==(const TestAllocNoMove&, const TestAllocNoMove&);
    friend ostream& operator<<(ostream& os, const TestAllocNoMove&);
};

TestAllocNoMove::TestAllocNoMove(bdema_Allocator *basicAllocator)
    : value(0)
    , fussy(detectFussy)
    , d_allocator_p(bdema_Default::allocator(basicAllocator))
{
    pointer = (int *) d_allocator_p->allocate(sizeof(int));
}

TestAllocNoMove::TestAllocNoMove(const TestAllocNoMove& old,
                                 bdema_Allocator *basicAllocator)
    : value(old.value)
    , fussy(detectFussy)
    , d_allocator_p(bdema_Default::allocator(basicAllocator))
{
    ++copyCtorCt;
    pointer = (int *) d_allocator_p->allocate(sizeof(int));
}

TestAllocNoMove::TestAllocNoMove(int ivalue, bdema_Allocator *basicAllocator)
    : value(ivalue)
    , fussy(detectFussy)
    , d_allocator_p(bdema_Default::allocator(basicAllocator))
{
    pointer = (int *) d_allocator_p->allocate(sizeof(int));
}

TestAllocNoMove::~TestAllocNoMove()
{
    ++dtorCt;
    d_allocator_p->deallocate(pointer);
}

TestAllocNoMove& TestAllocNoMove::operator=(const TestAllocNoMove& item)
{
    value = item.value;
    return *this;
}

int operator==(const TestAllocNoMove& lhs, const TestAllocNoMove& rhs)
{
    return lhs.value == rhs.value;
}

ostream& operator<<(ostream& os, const TestAllocNoMove& rhs)
{
    return os << rhs.value;
}

int TestAllocNoMove::copyCtorCt = 0;
int TestAllocNoMove::dtorCt = 0;

inline
int TestAllocNoMove::verifyAlgorithm(int numCtor,
                                     int numDtor,
                                     int numInserts,
                                     int numRemoves)
{
    // Non-moveable objects are always OK with most conservative algorithm
    return 1;
}

//
// Supports the bdema_allocator.  This class does not have the BitwiseCopyable
// trait.  The class does have the BitwiseMoveable traits.
//
class TestAllocMove {
  private:
    static int copyCtorCt;
    static int dtorCt;

    int              value;              // a "unique" value for the object
    int             *pointer;            // allocate an integer
    bdema_Allocator *d_allocator_p;

  public:

    static int getCopyCtorCt() { return copyCtorCt; }
    static int getDtorCt() { return dtorCt; }
    static int verifyAlgorithm(int numCtor, int numDtor, int numInserts,
         int numRemoves);

    TestAllocMove(int ivalue, bdema_Allocator *basicAllocator=0);
    TestAllocMove(const TestAllocMove& old, bdema_Allocator *basicAllocator=0);
    TestAllocMove(bdema_Allocator *basicAllocator=0);
    ~TestAllocMove();

    TestAllocMove& operator=(const TestAllocMove&);
    friend int operator==(const TestAllocMove&, const TestAllocMove&);
    friend ostream& operator<<(ostream& os, const TestAllocMove&);
};

template <> struct bdemf_IsBitwiseMoveable<TestAllocMove>
{
    enum { VALUE = 1 };
};

TestAllocMove::TestAllocMove(bdema_Allocator *basicAllocator)
: value(0)
, d_allocator_p(bdema_Default::allocator(basicAllocator))
{
    pointer = (int *) d_allocator_p->allocate(sizeof(int));
}

TestAllocMove::TestAllocMove(const TestAllocMove& old,
                                 bdema_Allocator *basicAllocator)
: value(old.value)
, d_allocator_p(bdema_Default::allocator(basicAllocator))
{
    ++copyCtorCt;
    pointer = (int *) d_allocator_p->allocate(sizeof(int));
}

TestAllocMove::TestAllocMove(int ivalue, bdema_Allocator *basicAllocator)
: value(ivalue)
, d_allocator_p(bdema_Default::allocator(basicAllocator))
{
    pointer = (int *) d_allocator_p->allocate(sizeof(int));
}

TestAllocMove::~TestAllocMove()
{
    ++dtorCt;
    d_allocator_p->deallocate(pointer);
}

TestAllocMove& TestAllocMove::operator=(const TestAllocMove& item)
{
    value = item.value;
    return *this;
}

int operator==(const TestAllocMove& lhs, const TestAllocMove& rhs)
{
    return lhs.value == rhs.value;
}

ostream& operator<<(ostream& os, const TestAllocMove& rhs)
{
    return os << rhs.value;
}

int TestAllocMove::copyCtorCt = 0;
int TestAllocMove::dtorCt = 0;

inline
int TestAllocMove::verifyAlgorithm(int numCtor,    // number of copy ctors
                                   int numDtor,    // number of dtor
                                   int numInserts, // elements inserted
                                   int numRemoves) // elements removed
{
    if ((copyCtorCt - numCtor) > numInserts) return 0;
    if ((dtorCt - numDtor) > (numInserts + numRemoves)) return 0;

    return 1;
}

//
// Supports the bdema_allocator.  This class does have the BitwiseCopyable
// trait which implies the BitwiseMoveable trait.
//
class TestAllocCopy {
  private:
    static int copyCtorCt;
    static int dtorCt;

    int              value;              // a "unique" value for the object
    bdema_Allocator *d_allocator_p;

  public:
    static int getCopyCtorCt() { return copyCtorCt; }
    static int getDtorCt() { return dtorCt; }
    static int verifyAlgorithm(int numCtor, int numDtor, int numInserts,
         int numRemoves);
    TestAllocCopy(int ivalue, bdema_Allocator *basicAllocator=0);
    TestAllocCopy(const TestAllocCopy& old,
                    bdema_Allocator *basicAllocator=0);
    TestAllocCopy(bdema_Allocator *basicAllocator=0);
    ~TestAllocCopy();

    TestAllocCopy& operator=(const TestAllocCopy&);
    friend int operator==(const TestAllocCopy&, const TestAllocCopy&);
    friend ostream& operator<<(ostream& os, const TestAllocCopy&);
};

template <> struct bdemf_IsBitwiseCopyable<TestAllocCopy>
{
    enum { VALUE = 1 };
};

TestAllocCopy::TestAllocCopy(bdema_Allocator *basicAllocator)
    : value(0)
    , d_allocator_p(bdema_Default::allocator(basicAllocator))
{
}

TestAllocCopy::TestAllocCopy(const TestAllocCopy& old,
                             bdema_Allocator     *basicAllocator)
: value(old.value)
, d_allocator_p(bdema_Default::allocator(basicAllocator))
{
    ++copyCtorCt;
}

TestAllocCopy::TestAllocCopy(int ivalue, bdema_Allocator *basicAllocator)
: value(ivalue)
, d_allocator_p(bdema_Default::allocator(basicAllocator))
{
}

TestAllocCopy::~TestAllocCopy()
{
    ++dtorCt;
}

TestAllocCopy& TestAllocCopy::operator=(const TestAllocCopy& item)
{
    value = item.value;
    return *this;
}

int operator==(const TestAllocCopy& lhs, const TestAllocCopy& rhs)
{
    return lhs.value == rhs.value;
}

ostream& operator<<(ostream& os, const TestAllocCopy& rhs)
{
    return os << rhs.value;
}

int TestAllocCopy::copyCtorCt = 0;
int TestAllocCopy::dtorCt = 0;

inline
int TestAllocCopy::verifyAlgorithm(int numCtor,    // number of copy ctors
                                   int numDtor,    // number of dtor
                                   int numInserts,
                                   int numRemoves)
{
    // No copy constructor or destructor calls are appropriate as part
    // of array operations.

    if ((copyCtorCt - numCtor) > 0) return 0;
    if ((dtorCt - numDtor) > 0) return 0;

    return 1;
}

//=============================================================================
//
//     Test class without an allocator.
//     --------------------------------
//
// Does not support the bdema_allocator.  This class does not have the
// BitwiseCopyable or BitwiseMoveable traits.
//
class TestNoAllocNoMove {
  private:
    static int copyCtorCt;
    static int dtorCt;

    int              value;              // a "unique" value for the object
    int             *pointer;            // allocate an integer
    bdeimp_Fussy     fussy;              // detects moving the object

  public:
    static int getCopyCtorCt() { return copyCtorCt; }
    static int getDtorCt() { return dtorCt; }
    static int verifyAlgorithm(int numCtor, int numDtor, int numInserts,
         int numRemoves);
    TestNoAllocNoMove(int ivalue);
    TestNoAllocNoMove(const TestNoAllocNoMove& old);
    TestNoAllocNoMove();
    ~TestNoAllocNoMove();

    TestNoAllocNoMove& operator=(const TestNoAllocNoMove&);
    friend int operator==(const TestNoAllocNoMove&, const TestNoAllocNoMove&);
    friend ostream& operator<<(ostream& os, const TestNoAllocNoMove&);
};

inline TestNoAllocNoMove::TestNoAllocNoMove()
: value(0)
, fussy(detectFussy)
{
    pointer = new int;
}

inline TestNoAllocNoMove::TestNoAllocNoMove(const TestNoAllocNoMove& old)
: value(old.value)
, fussy(detectFussy)
{
    ++copyCtorCt;
    pointer = new int;
}

inline TestNoAllocNoMove::TestNoAllocNoMove(int ivalue)
: value(ivalue)
, fussy(detectFussy)
{
    pointer = new int;
}


inline TestNoAllocNoMove::~TestNoAllocNoMove()
{
    ++dtorCt;
    delete pointer;
}

inline TestNoAllocNoMove& TestNoAllocNoMove::operator=(
      const TestNoAllocNoMove& item)
{
    value = item.value;
    return *this;
}

inline
int operator==(const TestNoAllocNoMove& lhs, const TestNoAllocNoMove& rhs)
{
    return lhs.value == rhs.value;
}

ostream& operator<<(ostream& os, const TestNoAllocNoMove& rhs)
{
    return os << rhs.value;
}

int TestNoAllocNoMove::copyCtorCt = 0;
int TestNoAllocNoMove::dtorCt = 0;

inline
int TestNoAllocNoMove::verifyAlgorithm(int numCtor,
                                     int numDtor,
                                     int numInserts,
                                     int numRemoves)
{
    // Non-moveable objects are always OK with most conservative algorithm
    return 1;
}


//
// Does not support the bdema_allocator.  This class does not have the
// BitwiseCopyable trait.  The class does have the BitwiseMoveable traits.
//
class TestNoAllocMove {
  private:
    static int copyCtorCt;
    static int dtorCt;

    int  value;              // a "unique" value for the object
    int *pointer;            // allocate an integer

  public:

    static int getCopyCtorCt() { return copyCtorCt; }
    static int getDtorCt() { return dtorCt; }
    static int verifyAlgorithm(int numCtor, int numDtor, int numInserts,
         int numRemoves);
    TestNoAllocMove(int ivalue);
    TestNoAllocMove(const TestNoAllocMove& old);
    TestNoAllocMove();
    ~TestNoAllocMove();

    TestNoAllocMove& operator=(const TestNoAllocMove&);
    friend int operator==(const TestNoAllocMove&, const TestNoAllocMove&);

    friend ostream& operator<<(ostream& os, const TestNoAllocMove&);
};

template <> struct bdemf_IsBitwiseMoveable<TestNoAllocMove>
{
    enum { VALUE = 1 };
};

TestNoAllocMove::TestNoAllocMove()
: value(0)
{
    pointer = new int;
}

TestNoAllocMove::TestNoAllocMove(const TestNoAllocMove& old)
: value(old.value)
{
    ++copyCtorCt;
    pointer = new int;
}

TestNoAllocMove::TestNoAllocMove(int ivalue)
: value(ivalue)
{
    pointer = new int;
}

TestNoAllocMove::~TestNoAllocMove()
{
    ++dtorCt;
    delete pointer;
}

TestNoAllocMove& TestNoAllocMove::operator=(const TestNoAllocMove& item)
{
    value = item.value;
    return *this;
}

int operator==(const TestNoAllocMove& lhs, const TestNoAllocMove& rhs)
{
    return lhs.value == rhs.value;
}

ostream& operator<<(ostream& os, const TestNoAllocMove& rhs)
{
    return os << rhs.value;
}

int TestNoAllocMove::copyCtorCt = 0;
int TestNoAllocMove::dtorCt = 0;

inline
int TestNoAllocMove::verifyAlgorithm(int numCtor,    // number of copy ctors
                                     int numDtor,    // number of dtor
                                     int numInserts,
                                     int numRemoves)
{
    if ((copyCtorCt - numCtor) > numInserts) return 0;
    if ((dtorCt - numDtor) > (numInserts + numRemoves)) return 0;

    return 1;
}

//
// Does not support the bdema_allocator.  This class does have the
// BitwiseCopyable trait which implies the BitwiseMoveable trait.
//
class TestNoAllocCopy {
  private:
    static int copyCtorCt;
    static int dtorCt;

    int              value;              // a "unique" value for the object

  public:

    static int getCopyCtorCt() { return copyCtorCt; }
    static int getDtorCt() { return dtorCt; }
    static int verifyAlgorithm(int numCtor, int numDtor, int numInserts,
         int numRemoves);
    TestNoAllocCopy(int ivalue);
    TestNoAllocCopy(const TestNoAllocCopy& old);
    TestNoAllocCopy();
    ~TestNoAllocCopy();

    TestNoAllocCopy& operator=(const TestNoAllocCopy&);
    friend int operator==(const TestNoAllocCopy&, const TestNoAllocCopy&);
    friend ostream& operator<<(ostream& os, const TestNoAllocCopy&);
};

template <> struct bdemf_IsBitwiseCopyable<TestNoAllocCopy>
{
    enum { VALUE = 1 };
};

inline
TestNoAllocCopy::TestNoAllocCopy()
: value(0)
{
}

inline
TestNoAllocCopy::TestNoAllocCopy(const TestNoAllocCopy& old)
: value(old.value)
{
    ++copyCtorCt;
}

inline
TestNoAllocCopy::TestNoAllocCopy(int ivalue)
: value(ivalue)
{
}

inline
TestNoAllocCopy::~TestNoAllocCopy()
{
    ++dtorCt;
}

inline
TestNoAllocCopy& TestNoAllocCopy::operator=(const TestNoAllocCopy& item)
{
    value = item.value;
    return *this;
}

inline
int operator==(const TestNoAllocCopy& lhs, const TestNoAllocCopy& rhs)
{
    return lhs.value == rhs.value;
}

ostream& operator<<(ostream& os, const TestNoAllocCopy& rhs)
{
    return os << rhs.value;
}

int TestNoAllocCopy::copyCtorCt = 0;
int TestNoAllocCopy::dtorCt = 0;

inline
int TestNoAllocCopy::verifyAlgorithm(int numCtor,    // number of copy ctors
                                     int numDtor,    // number of dtor
                                     int numInserts,
                                     int numRemoves)
{
    if ((copyCtorCt - numCtor) > 0) return 0;
    if ((dtorCt - numDtor) > 0) return 0;

    return 1;
}

//=============================================================================
//
//      Structure and Class used as part of Usage examples
//
struct SampleStruct {
    int x;
    friend ostream& operator<<(ostream& os, const SampleStruct &v);
};
ostream& operator<<(ostream& os, const SampleStruct &v) { return os << v.x; }

template <> struct bdemf_IsBitwiseCopyable<SampleStruct>
{
    enum { VALUE = 1 };
};

class SampleClass {
    int  x;
    int *y;
    bdema_Allocator *d_allocator_p;
  public:
    SampleClass(int value, bdema_Allocator *basicAllocator = 0);
    SampleClass(const SampleClass& original,
                bdema_Allocator *basicAllocator = 0);
    SampleClass(bdema_Allocator *basicAllocator = 0);
    ~SampleClass();

    SampleClass& operator=(const SampleClass&);

    friend int operator==(const SampleClass& lhs, const SampleClass& rhs);
    friend ostream& operator<<(ostream& os, const SampleClass&);
};

template <> struct bdemf_IsBitwiseMoveable<SampleClass>
{
    enum { VALUE = 1 };
};

SampleClass::SampleClass(int value, bdema_Allocator *basicAllocator)
: x(value)
, d_allocator_p(bdema_Default::allocator(basicAllocator))
{
    y = (int *) d_allocator_p->allocate(sizeof(int));
}

SampleClass::SampleClass(const SampleClass& original,
                           bdema_Allocator *basicAllocator)
: x(original.x)
, d_allocator_p(bdema_Default::allocator(basicAllocator))
{
    y = (int *) d_allocator_p->allocate(sizeof(int));
}

SampleClass::SampleClass(bdema_Allocator *basicAllocator)
: x(0)
, d_allocator_p(bdema_Default::allocator(basicAllocator))
{
    y = (int *) d_allocator_p->allocate(sizeof(int));
}

SampleClass::~SampleClass()
{
    d_allocator_p->deallocate(y);
}

inline
SampleClass& SampleClass::operator=(const SampleClass& item)
{
    x = item.x;
    return *this;
}

ostream& operator<<(ostream& os, const SampleClass& rhs)
{
    return os << rhs.x;
}

inline
int operator==(const SampleClass& lhs, const SampleClass& rhs)
{
    return lhs.x == rhs.x;
}

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

template <class T>
class testDriver
{
   public:
    static int getValues(const T **values);
    static void stretch(bdea_Array<T> *object, int size, const T& elem);
    static void stretchRemoveAll(bdea_Array<T> *object, int size,
                                 const T& elem);

    static int ggg(bdea_Array<T> *object,
                   const char *spec, int verboseFlag = 1);
    static bdea_Array<T>& gg(bdea_Array<T> *object, const char *spec);
    static bdea_Array<T> g(const char *spec);
};

template <class T>
int testDriver<T>::getValues(const T **values)
{
    static const T VALUES[] = {
            1, 2, 3, 4, 5
    }; // avoid DEFAULT_VALUE

    const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

    *values = VALUES;
    return NUM_VALUES;
}

template <class T>
void testDriver<T>::stretch(bdea_Array<T> *object, int size, const T& elem)
   // Using only primary manipulators, extend the length of the specified
   // 'object' by the specified size.  The resulting value is not specified.
   // The behavior is undefined unless 0 <= size.
{
    ASSERT(object);
    ASSERT(0 <= size);
    for (int i = 0; i < size; ++i) {
        object->append(elem);
    }
    ASSERT(object->length() >= size);
}

template <class T>
void testDriver<T>::stretchRemoveAll(bdea_Array<T> *object, int size,
                                     const T& elem)
   // Using only primary manipulators, extend the capacity of the specified
   // 'object' to (at least) the specified size; then remove all elements
   // leaving 'object' empty.  The behavior is undefined unless 0 <= size.
{
    ASSERT(object);
    ASSERT(0 <= size);
    stretch(object, size, elem);
    object->removeAll();
    ASSERT(0 == object->length());
}

template <class T, class STREAM>
void putArrayStream (STREAM& out, T *array, int length)
    // Output the array 'array' of elements with length 'length' to stream
    // 'stream'.
{
    for (int i = 0; i < length; ++i) {
        array[i].bdexStreamOut(out, 1);
    }
}

}

//=============================================================================
//              GENERATOR FUNCTIONS 'g' AND 'gg' FOR TESTING
//-----------------------------------------------------------------------------
// The following functions interpret the given 'spec' in order from left to
// right to configure the object according to a custom language.  Uppercase
// letters [A .. E] correspond to arbitrary (but unique) int values to be
// appended to the 'bdea_Array<T>' object.  A tilde ('~') indicates that
// the logical (but not necessarily physical) state of the object is to be set
// to its initial, empty state (via the 'removeAll' method).
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
// <ITEM>       ::= <ELEMENT> | <REMOVE_ALL>
//
// <ELEMENT>    ::= 'A' | 'B' | 'C' | 'D' | 'E'
//                                      // unique but otherwise arbitrary
// <REMOVE_ALL> ::= '~'
//
// Spec String  Description
// -----------  ---------------------------------------------------------------
// ""           Has no effect; leaves the object empty.
// "A"          Append the value corresponding to A.
// "AA"         Append two values both corresponding to A.
// "ABC"        Append three values corresponding to A, B and C.
// "ABC~"       Append three values corresponding to A, B and C and then
//              remove all the elements (set array length to 0).  Note that
//              this spec yields an object that is logically equivalent
//              (but not necessarily identical internally) to one
//              yielded by ("").
// "ABC~DE"     Append three values corresponding to A, B, and C; empty
//              the object; and append values corresponding to D and E.
//
//-----------------------------------------------------------------------------

template <class T>
int testDriver<T>::ggg(bdea_Array<T> *object,
        const char    *spec,
        int            verboseFlag)
    // Configure the specified 'object' according to the specified 'spec',
    // using only the primary manipulator function 'append' and white-box
    // manipulator 'removeAll'.  Optionally specify a zero 'verboseFlag' to
    // suppress 'spec' syntax error messages.  Return the index of the first
    // invalid character, and a negative value otherwise.  Note that this
    // function is used to implement 'gg' as well as allow for verification of
    // syntax error detection.
{
    const T *VALUES;
    getValues(&VALUES);
    enum { SUCCESS = -1 };
    for (int i = 0; spec[i]; ++i) {
        if ('A' <= spec[i] && spec[i] <= 'E') {
            object->append(VALUES[spec[i] - 'A']);
        }
        else if ('~' == spec[i]) {
            object->removeAll();
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

template <class T>
bdea_Array<T>& testDriver<T>::gg(bdea_Array<T> *object,
                  const char    *spec)
    // Return, by reference, the specified object with its value adjusted
    // according to the specified 'spec'.
{
    ASSERT(ggg(object, spec) < 0);
    return *object;
}

template <class T>
bdea_Array<T> testDriver<T>::g(const char *spec)
    // Return, by value, a new object corresponding to the specified 'spec'.
{
    bdea_Array<T> object((bdema_Allocator *)0);
    return gg(&object, spec);
}

template <class T>
class EqualityTester {
private:
        const T   *A;
        const T   *B;
public:
        EqualityTester(const T *a, const T *b);
        ~EqualityTester();
};

// CREATORS
template <class T>
inline EqualityTester<T>::EqualityTester(const T *a, const T *b)
: A(a)
, B(b)
{
}

template <class T>
inline EqualityTester<T>::~EqualityTester()
{
        ASSERT(*A == *B);
}

//=============================================================================
//                             Structures for Test Data
//-----------------------------------------------------------------------------

struct InsertTable {
    int         d_lineNum;  // source line number
    const char *d_daSpec;   // initial (destination) array
    int         d_di;       // index at which to insert into da
    const char *d_saSpec;   // source array
    int         d_si;       // index at which to insert from sa
    int         d_ne;       // number of elements to insert
    const char *d_expSpec;  // expected array value
};

struct InsertAliasTable {
    int         d_lineNum;  // source line number
    const char *d_xSpec;    // initial array (= srcArray)
    int         d_di;       // index at which to insert into x
    int         d_si;       // index at which to insert from x
    int         d_ne;       // number of elements to insert
    const char *d_expSpec;  // expected array value
};

struct ReplaceTable {
    int         d_lineNum;  // source line number
    const char *d_daSpec;   // initial (destination) array
    int         d_di;       // index at which to replace into da
    const char *d_saSpec;   // source array
    int         d_si;       // index at which to replace from sa
    int         d_ne;       // number of elements to replace
    const char *d_expSpec;  // expected array value
};

struct ReplaceAliasTable {
    int         d_lineNum;  // source line number
    const char *d_xSpec;    // initial array (= srcArray)
    int         d_di;       // index at which to insert into x
    int         d_si;       // index at which to insert from x
    int         d_ne;       // number of elements to insert
    const char *d_expSpec;  // expected array value
};

struct ReserveCapacityTable {
    int         d_lineNum;  // source line number
    const char *d_x;        // initial value
    int         d_ne;       // total number of elements to reserve
    int         d_alloc;    // number of elements allocated
};

//=============================================================================
//                             class TestCases
//-----------------------------------------------------------------------------

template <class Element>
struct TestCases {
    static void testcase2(
     int& testStatus, int argc, char *argv[],
     int verbose, int veryVerbose, int veryVeryVerbose,
     int hasInternalAllocation, bdema_TestAllocator& testAllocator);

    static void testcase7(
     int& testStatus, int argc, char *argv[],
     int verbose, int veryVerbose, int veryVeryVerbose, int hasAllocator,
     bdema_TestAllocator& testAllocator);

    static void testcase9(
     int& testStatus, int argc, char *argv[],
     int verbose, int veryVerbose, int veryVeryVerbose,
     bdema_TestAllocator& testAllocator);

    static void testcase11(
     int& testStatus, int argc, char *argv[],
     int verbose, int veryVerbose, int veryVeryVerbose,
     bdema_TestAllocator& testAllocator);

    static void testcase12(
     int& testStatus, int argc, char *argv[],
     int verbose, int veryVerbose, int veryVeryVerbose,
     bdema_TestAllocator& testAllocator);

    static void testcase13Insert(
     int& testStatus, int argc, char *argv[],
     int verbose, int veryVerbose, int veryVeryVerbose,
     const InsertTable *data, int numData,
     int hasInternalAllocation, bdema_TestAllocator& testAllocator);

    static void testcase13InsertAlias(
     int& testStatus, int argc, char *argv[],
     int verbose, int veryVerbose, int veryVeryVerbose,
     const InsertAliasTable *data, int numData,
     int hasInternalAllocation, bdema_TestAllocator& testAllocator);

    static void testcase14(
     int& testStatus, int argc, char *argv[],
     int verbose, int veryVerbose, int veryVeryVerbose,
     const ReplaceTable *data, int numData,
     int hasInternalAllocation, bdema_TestAllocator& testAllocator);

    static void testcase14Alias(
     int& testStatus, int argc, char *argv[],
     int verbose, int veryVerbose, int veryVeryVerbose,
     const ReplaceAliasTable *data, int numData,
     bdema_TestAllocator& testAllocator);

    static void testcase15(
     int& testStatus, int argc, char *argv[],
     int verbose, int veryVerbose, int veryVeryVerbose,
     bdema_TestAllocator& testAllocator);

    static void testcase16(
     int& testStatus, int argc, char *argv[],
     int verbose, int veryVerbose, int veryVeryVerbose,
     int hasInternalAllocation, bdema_TestAllocator& testAllocator);

    static void testcase16ReserveCapacity(
     int& testStatus, int argc, char *argv[],
     int verbose, int veryVerbose, int veryVeryVerbose,
     const struct ReserveCapacityTable *data, int numData,
     int hasInternalAllocation, bdema_TestAllocator& testAllocator);

    static void testcase18(
     int& testStatus, int argc, char *argv[],
     int verbose, int veryVerbose, int veryVeryVerbose,
     bdema_TestAllocator& testAllocator);
};

//=============================================================================
//                             Test Case 2
//-----------------------------------------------------------------------------

template <class Element>
void TestCases<Element>::testcase2(
     int& testStatus, int argc, char *argv[],
     int verbose, int veryVerbose, int veryVeryVerbose,
     int hasInternalAllocation, bdema_TestAllocator& testAllocator)
{
      typedef bdea_Array<Element> Obj;
      const Element *VALUES;
      int NUM_VALUES = testDriver<Element>::getValues(&VALUES);

        if (verbose) cout << "\nTesting default ctor (thoroughly)." << endl;

        if (verbose) cout << "\tTesting ctor with no parameters." << endl;
        {
            const Obj X;
            if (veryVerbose) { cout << "\t\t"; P(X); }
            ASSERT(0 == X.length());
        }

        if (verbose) cout << "\nTesting 'append' with default ctor." << endl;
        {
            if (verbose) cout << "\tOn an object of initial length 0." << endl;
            Obj mX;  const Obj& X = mX;
            if (veryVerbose) { cout << "\t\t"; P(X); }

            mX.append(VALUES[0]);
            if (veryVerbose) { cout << "\t\t"; P(X); }
            ASSERT(1 == X.length());
            ASSERT(VALUES[0] == X[0]);
        }
        {
            if (verbose) cout << "\tOn an object of initial length 1." << endl;
            Obj mX;  const Obj& X = mX;
            if (veryVerbose) { cout << "\t\t"; P(X); }
            mX.append(VALUES[0]);

            if (veryVerbose) { cout << "\t\t"; P(X); }
            mX.append(VALUES[1]);
            if (veryVerbose) { cout << "\t\t"; P(X); }
            ASSERT(2 == X.length());
            ASSERT(VALUES[0] == X[0]);
            ASSERT(VALUES[1] == X[1]);
        }

        if (verbose) cout << "\tWithout passing in an allocator." << endl;
        {
            const Obj X((bdema_Allocator *)0);
            if (veryVerbose) { cout << "\t\t"; P(X); }
            ASSERT(0 == X.length());
        }
        if (verbose) cout << "\tPassing in an allocator." << endl;

        if (verbose) cout << "\t\tWith no exceptions." << endl;
        {
            const Obj X(&testAllocator);
            if (veryVerbose) { cout << "\t\t"; P(X); }
            ASSERT(0 == X.length());
        }

        if (verbose) cout << "\t\tWith exceptions." << endl;
        {
          BEGIN_BDEMA_EXCEPTION_TEST {
            if (veryVerbose) cout <<
                "\tTesting Exceptions In Default Ctor" << endl;
            const Obj X(&testAllocator);
            if (veryVerbose) { cout << "\t\t"; P(X); }
            ASSERT(0 == X.length());
          } END_BDEMA_EXCEPTION_TEST
        }

        if (verbose) cout << "\tIn place using a buffer allocator." << endl;
        {
            char memory[8192];
            bdema_BufferAllocator a(memory, sizeof memory);
            void *doNotDelete = new(a.allocate(sizeof(Obj))) Obj(&a);
            ASSERT(doNotDelete);

            // No destructor is called; will produce memory leak in purify
            // if internal allocators are not hooked up properly.
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'append' (bootstrap)." << endl;
        {
            if (verbose) cout << "\tOn an object of initial length 0." << endl;
            Obj mX(&testAllocator);  const Obj& X = mX;

            const int BB = testAllocator.numBlocksTotal();
            const int B  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\tBEFORE: "; P_(B); P(X); }
            mX.append(VALUES[0]);
            const int AA = testAllocator.numBlocksTotal();
            const int A  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\t AFTER: "; P_(A); P(X); }
            if (!hasInternalAllocation) {
                ASSERT(BB + 0 == AA);
                ASSERT(B  + 0 == A);
            }
            ASSERT(1 == X.length());
            ASSERT(VALUES[0] == X[0]);
        }
        {
            if (verbose) cout << "\tOn an object of initial length 1." << endl;
            Obj mX(&testAllocator);  const Obj& X = mX;
            mX.append(VALUES[0]);

            const int BB = testAllocator.numBlocksTotal();
            const int B  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\tBEFORE: "; P_(B); P(X); }
            mX.append(VALUES[1]);
            const int AA = testAllocator.numBlocksTotal();
            const int A  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\t AFTER: "; P_(A); P(X); }
            ASSERT(2 == X.length());
            if (!hasInternalAllocation) {
                ASSERT(BB + 1 == AA);
                ASSERT(B  - 0 == A);
            }
            ASSERT(VALUES[0] == X[0]);
            ASSERT(VALUES[1] == X[1]);
        }
        {
            if (verbose) cout << "\tOn an object of initial length 2." << endl;
            Obj mX(&testAllocator);  const Obj& X = mX;
            mX.append(VALUES[0]); mX.append(VALUES[1]);

            const int BB = testAllocator.numBlocksTotal();
            const int B  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\tBEFORE: "; P_(B); P(X); }
            mX.append(VALUES[2]);
            const int AA = testAllocator.numBlocksTotal();
            const int A  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\t AFTER: "; P_(A); P(X); }
            if (!hasInternalAllocation) {
                 ASSERT(BB + 1 == AA);
                 ASSERT(B  - 0 == A);
            }
            ASSERT(3 == X.length());
            ASSERT(VALUES[0] == X[0]);
            ASSERT(VALUES[1] == X[1]);
            ASSERT(VALUES[2] == X[2]);
        }
        {
            if (verbose) cout << "\tOn an object of initial length 3." << endl;
            Obj mX(&testAllocator);  const Obj& X = mX;
            mX.append(VALUES[0]); mX.append(VALUES[1]); mX.append(VALUES[2]);

            const int BB = testAllocator.numBlocksTotal();
            const int B  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\tBEFORE: "; P_(B); P(X); }
            mX.append(VALUES[3]);
            const int AA = testAllocator.numBlocksTotal();
            const int A  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\t AFTER: "; P_(A); P(X); }
            if (!hasInternalAllocation) {
                ASSERT(BB + 0 == AA);
                ASSERT(B  - 0 == A);
            }
            ASSERT(4 == X.length());
            ASSERT(VALUES[0] == X[0]);
            ASSERT(VALUES[1] == X[1]);
            ASSERT(VALUES[2] == X[2]);
            ASSERT(VALUES[3] == X[3]);
        }
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'removeAll'." << endl;
        {
            if (verbose) cout << "\tOn an object of initial length 0." << endl;
            Obj mX(&testAllocator);  const Obj& X = mX;
            ASSERT(0 == X.length());

            const int BB = testAllocator.numBlocksTotal();
            const int B = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\tBEFORE: "; P_(B); P(X); }
            mX.removeAll();
            const int AA = testAllocator.numBlocksTotal();
            const int A = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\t AFTER: "; P_(A); P(X); }
            ASSERT(BB == AA);   // always
            ASSERT(B - 0 == A); // ADJUST
            ASSERT(0 == X.length());
        }
        {
            if (verbose) cout << "\tOn an object of initial length 1." << endl;
            Obj mX(&testAllocator);  const Obj& X = mX;
            mX.append(VALUES[0]);
            ASSERT(1 == X.length());

            const int BB = testAllocator.numBlocksTotal();
            const int B = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\tBEFORE: "; P_(B); P(X); }
            mX.removeAll();
            const int AA = testAllocator.numBlocksTotal();
            const int A = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\t AFTER: "; P_(A); P(X); }
            ASSERT(BB == AA);    // always
            if (!hasInternalAllocation) {
                ASSERT(B - 0 == A);
            }
            ASSERT(0 == X.length());
        }
        {
            if (verbose) cout << "\tOn an object of initial length 2." << endl;
            Obj mX(&testAllocator);  const Obj& X = mX;
            mX.append(VALUES[0]); mX.append(VALUES[1]);
            ASSERT(2 == X.length());

            const int BB = testAllocator.numBlocksTotal();
            const int B = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\tBEFORE: "; P_(B); P(X); }
            mX.removeAll();
            const int AA = testAllocator.numBlocksTotal();
            const int A = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\t AFTER: "; P_(A); P(X); }
            ASSERT(BB == AA);    // always
            if (!hasInternalAllocation) {
                  ASSERT(B - 0 == A);
            }
            ASSERT(0 == X.length());
        }
        {
            if (verbose) cout << "\tOn an object of initial length 3." << endl;
            Obj mX(&testAllocator);  const Obj& X = mX;
            mX.append(VALUES[0]); mX.append(VALUES[1]); mX.append(VALUES[2]);
            ASSERT(3 == X.length());

            const int BB = testAllocator.numBlocksTotal();
            const int B = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\tBEFORE: "; P_(B); P(X); }
            mX.removeAll();
            const int AA = testAllocator.numBlocksTotal();
            const int A = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\t AFTER: "; P_(A); P(X); }
            ASSERT(BB == AA);    // always
            if (!hasInternalAllocation) {
                ASSERT(B - 0 == A);
            }
            ASSERT(0 == X.length());
        }

        // --------------------------------------------------------------------

        if (verbose) cout <<
          "\nTesting the destructor and exception neutrality." << endl;

        if (verbose) cout << "\tWith 'append' only" << endl;
        {
            // For each lengths i up to some modest limit:
            //    1. create an instance
            //    2. append { VALUES[0], VALUES[1], VALUES[2], VALUES[3],
            //       VALUES[4], VALUES[0], ... }  up to length i
            //    3. verify initial length and contents
            //    4. allow the instance to leave scope

            const int NUM_TRIALS = 10;
            for (int i = 0; i < NUM_TRIALS; ++i) { // i is the length
                if (verbose) cout <<
                    "\t\tOn an object of length " << i << '.' << endl;

              BEGIN_BDEMA_EXCEPTION_TEST {
                int k; // loop index

                Obj mX(&testAllocator);  const Obj& X = mX;     // 1.
                for (k = 0; k < i; ++k) {                               // 2.
                    mX.append(VALUES[k % NUM_VALUES]);
                }

                LOOP_ASSERT(i, i == X.length());                        // 3.
                for (k = 0; k < i; ++k) {
                    LOOP2_ASSERT(i, k, VALUES[k % NUM_VALUES] == X[k]);
                }

              } END_BDEMA_EXCEPTION_TEST                                // 4.
            }
        }

        if (verbose) cout << "\tWith 'append' and 'removeAll'" << endl;
        {
            // For each pair of lengths (i, j) up to some modest limit:
            //    1. create an instance
            //    2. append VALUES[0] values up to a length of i
            //    3. verify initial length and contents
            //    4. removeAll contents from instance
            //    5. verify length is 0
            //    6. append { VALUES[0], VALUES[1], VALUES[2], VALUES[3],
            //       VALUES[4], VALUES[0], ... }  up to length j
            //    7. verify new length and contents
            //    8. allow the instance to leave scope

            const int NUM_TRIALS = 10;
            for (int i = 0; i < NUM_TRIALS; ++i) { // i is first length
                if (verbose) cout <<
                    "\t\tOn an object of initial length " << i << '.' << endl;

                for (int j = 0; j < NUM_TRIALS; ++j) { // j is second length
                    if (veryVerbose) cout <<
                        "\t\t\tAnd with final length " << j << '.' << endl;

                  BEGIN_BDEMA_EXCEPTION_TEST {
                    int k; // loop index

                    Obj mX(&testAllocator);  const Obj& X = mX; // 1.
                    for (k = 0; k < i; ++k) {                           // 2.
                        mX.append(VALUES[0]);
                    }

                    LOOP2_ASSERT(i, j, i == X.length());                // 3.
                    for (k = 0; k < i; ++k) {
                        LOOP3_ASSERT(i, j, k, VALUES[0] == X[k]);
                    }

                    mX.removeAll();                                     // 4.
                    LOOP2_ASSERT(i, j, 0 == X.length());                // 5.

                    for (k = 0; k < j; ++k) {                           // 6.
                        mX.append(VALUES[k % NUM_VALUES]);
                    }

                    LOOP2_ASSERT(i, j, j == X.length());                // 7.
                    for (k = 0; k < j; ++k) {
                        LOOP3_ASSERT(i, j, k, VALUES[k % NUM_VALUES] == X[k]);
                    }
                  } END_BDEMA_EXCEPTION_TEST                            // 8.
                }
            }
        }
}

//=============================================================================
//                             Test Case 7
//-----------------------------------------------------------------------------

template <class Element>
void TestCases<Element>::testcase7(
     int& testStatus, int argc, char *argv[],
     int verbose, int veryVerbose, int veryVeryVerbose, int hasAllocator,
     bdema_TestAllocator& testAllocator)
{
      typedef bdea_Array<Element> Obj;
      const Element *VALUES;
      testDriver<Element>::getValues(&VALUES);

        if (verbose) cout <<
            "\nCopy construct values with varied representations." << endl;
        {
            static const char *SPECS[] = { // len: 0 - 5, 7, 8, 9, 15, 16, 17
                "",      "A",      "BC",     "CDE",    "DEAB",   "EABCD",
                "ABCDEAB",         "ABCDEABC",         "ABCDEABCD",
                "ABCDEABCDEABCDE", "ABCDEABCDEABCDEA", "ABCDEABCDEABCDEAB",
            0}; // Null string required as last element.

            static const int EXTEND[] = {
                0, 1, 2, 3, 4, 5, 7, 8, 9, 15, 16, 17
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC = SPECS[ti];
                const int curLen = (int) strlen(SPEC);

                if (verbose) {
                    cout << "\tFor an object of length " << curLen << ":\t";
                    P(SPEC);
                }

                LOOP_ASSERT(SPEC, oldLen < curLen); // strictly increasing
                oldLen = curLen;

                // Create control object w.
                Obj mW(&testAllocator); testDriver<Element>::gg(&mW, SPEC);
                const Obj& W = mW;

                LOOP_ASSERT(ti, curLen == W.length()); // same lengths
                if (veryVerbose) { cout << "\t"; P(W); }

                // Stretch capacity of x object by different amounts.

                for (int ei = 0; ei < NUM_EXTEND; ++ei) {
                    const int N = EXTEND[ei];
                    if (veryVerbose) { cout << "\t\t"; P(N) }

                    Obj *pX = new Obj(&testAllocator);
                    Obj& mX = *pX;
                    testDriver<Element>::stretchRemoveAll(&mX, N, VALUES[0]);
                    const Obj& X = mX;  testDriver<Element>::gg(&mX, SPEC);
                    if (veryVerbose) { cout << "\t\t"; P(X); }

                    {                                   // No allocator.
                        const Obj Y0(X);
                        if (veryVerbose) { cout << "\t\t\t"; P(Y0); }
                        LOOP2_ASSERT(SPEC, N, W == Y0);
                        LOOP2_ASSERT(SPEC, N, W == X);
                    }

                    {                                   // Null allocator.
                        const Obj Y1(X, (bdema_Allocator *) 0);
                        if (veryVerbose) { cout << "\t\t\t"; P(Y1); }
                        LOOP2_ASSERT(SPEC, N, W == Y1);
                        LOOP2_ASSERT(SPEC, N, W == X);
                    }

                    BEGIN_BDEMA_EXCEPTION_TEST {    // Test allocator
                        const int NUM_CTOR = Element::getCopyCtorCt();
                        const int NUM_DTOR = Element::getDtorCt();
                        const Obj Y2(X, &testAllocator);
                        ASSERT(Element::verifyAlgorithm(NUM_CTOR, NUM_DTOR,
                               X.length(), 0));
                        if (veryVerbose) { cout << "\t\t\t"; P(Y2); }
                        LOOP2_ASSERT(SPEC, N, W == Y2);
                        LOOP2_ASSERT(SPEC, N, W == X);
                    } END_BDEMA_EXCEPTION_TEST

                    if (hasAllocator) {            // Buffer Allocator.
                        char memory[8192];
                        bdema_BufferAllocator a(memory, sizeof memory);
                        Obj *Y = new(a.allocate(sizeof(Obj)))
                           Obj(X, &a);
                        if (veryVerbose) { cout << "\t\t\t"; P(*Y); }
                        LOOP2_ASSERT(SPEC, N, W == *Y);
                        LOOP2_ASSERT(SPEC, N, W == X);
                    }

                    {                             // with 'original' destroyed
                        const Obj Y2(X, &testAllocator);
                        delete pX;
                        if (veryVerbose) { cout << "\t\t\t"; P(Y2); }
                        LOOP2_ASSERT(SPEC, N, W == Y2);
                    }
                }
            }
        }
}

//=============================================================================
//                             Test Case 9
//-----------------------------------------------------------------------------

template <class Element>
void TestCases<Element>::testcase9(
     int& testStatus, int argc, char *argv[],
     int verbose, int veryVerbose, int veryVeryVerbose,
     bdema_TestAllocator& testAllocator)
{
    typedef bdea_Array<Element> Obj;
    const Element              *VALUES;
    testDriver<Element>::getValues(&VALUES);

    if (verbose) cout <<
        "\nAssign cross product of values with varied representations. \n"
                      << "No Exceptions"
                      << endl;
    {
            static const char *SPECS[] = { // len: 0 - 4, 8, 9,
                "",        "A",    "BC",     "CDE",    "DEAB",
                "CBAEDCBA",         "EDCBAEDCB",
            0}; // Null string required as last element.

            static const int EXTEND[] = {
                0, 1, 2, 3, 4, 5, 7, 8, 9,
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            {
                int uOldLen = -1;
                for (int ui = 0; SPECS[ui]; ++ui) {
                const char *const U_SPEC = SPECS[ui];
                const int uLen = (int) strlen(U_SPEC);

                if (verbose) {
                    cout << "\tFor lhs objects of length " << uLen << ":\t";
                    P(U_SPEC);
                }

                LOOP_ASSERT(U_SPEC, uOldLen < uLen);  // strictly increasing
                uOldLen = uLen;

                const Obj UU = testDriver<Element>::g(U_SPEC);  // control
                LOOP_ASSERT(ui, uLen == UU.length());   // same lengths

                // int vOldLen = -1;
                for (int vi = 0; SPECS[vi]; ++vi) {
                    const char *const V_SPEC = SPECS[vi];
                    const int vLen = (int) strlen(V_SPEC);

                    if (veryVerbose) {
                        cout << "\t\tFor rhs objects of length " << vLen
                                                                 << ":\t";
                        P(V_SPEC);
                    }

                    const Obj VV = testDriver<Element>::g(V_SPEC); // control

                    const int Z = ui == vi; // flag indicating same values

                    for (int uj = 0; uj < NUM_EXTEND; ++uj) {
                        const int U_N = EXTEND[uj];
                        for (int vj = 0; vj < NUM_EXTEND; ++vj) {
                            const int V_N = EXTEND[vj];

                            Obj mU(&testAllocator);
                            testDriver<Element>::stretchRemoveAll(&mU, U_N,
                                                                  VALUES[0]);
                            const Obj& U = mU;
                            testDriver<Element>::gg(&mU, U_SPEC);
                            {
                            //--^
                            Obj mV(&testAllocator);
                            testDriver<Element>::stretchRemoveAll(&mV, V_N,
                                                                  VALUES[0]);
                            const Obj& V = mV;
                            testDriver<Element>::gg(&mV, V_SPEC);

                            static int firstFew = 2 * NUM_EXTEND * NUM_EXTEND;
                            if (veryVeryVerbose||veryVerbose && firstFew > 0) {
                                cout << "\t| "; P_(U_N); P_(V_N); P_(U); P(V);
                                --firstFew;
                            }

                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, UU == U);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == V);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, Z==(V==U));

                            const int NUM_CTOR = Element::getCopyCtorCt();
                            const int NUM_DTOR = Element::getDtorCt();
                            const int OLD_LENGTH = mU.length();

                            mU = V; // test assignment here

                            ASSERT(Element::verifyAlgorithm(NUM_CTOR, NUM_DTOR,
                               V.length(), OLD_LENGTH));

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
        }

        if (verbose) cout <<
            "\nAssign cross product of values with varied representations. \n"
                        << "With Exceptions"
                        << endl;
        {
            static const char *SPECS[] = { // len: 0-2, 4, 9,
                "",        "A",    "BC",     "DEAB",
                "EDCBAEDCB",
            0}; // Null string required as last element.

            static const int EXTEND[] = {
                0, 1, 3, 5,
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            int iterationModulus = argc > 2 ? atoi(argv[2]) : 0;
            if (iterationModulus <= 0) iterationModulus = 11;
            int iteration = 0;
            iterationModulus = 1;

            {
              int uOldLen = -1;
              for (int ui = 0; SPECS[ui]; ++ui) {
                const char *const U_SPEC = SPECS[ui];
                const int uLen = (int) strlen(U_SPEC);

                if (verbose) {
                    cout << "\tFor lhs objects of length " << uLen << ":\t";
                    P(U_SPEC);
                }

                LOOP_ASSERT(U_SPEC, uOldLen < uLen);    // strictly increasing
                uOldLen = uLen;

                const Obj UU = testDriver<Element>::g(U_SPEC);  // control
                LOOP_ASSERT(ui, uLen == UU.length());   // same lengths

                // int vOldLen = -1;
                for (int vi = 0; SPECS[vi]; ++vi) {
                    const char *const V_SPEC = SPECS[vi];
                    const int vLen = (int) strlen(V_SPEC);

                    if (veryVerbose) {
                        cout << "\t\tFor rhs objects of length " << vLen
                                                                 << ":\t";
                        P(V_SPEC);
                    }

                    // control
                    const Obj VV = testDriver<Element>::g(V_SPEC);

                    for (int uj = 0; uj < NUM_EXTEND; ++uj) {
                        const int U_N = EXTEND[uj];
                        for (int vj = 0; vj < NUM_EXTEND; ++vj) {
                            const int V_N = EXTEND[vj];

                          if (iteration % iterationModulus == 0)
                          {
                          BEGIN_BDEMA_EXCEPTION_TEST {
                            const int AL = testAllocator.allocationLimit();
                            testAllocator.setAllocationLimit(-1);
                            Obj mU(&testAllocator);
                            testDriver<Element>::stretchRemoveAll(&mU, U_N,
                                                                  VALUES[0]);
                            const Obj& U = mU;
                            testDriver<Element>::gg(&mU, U_SPEC);
                            {
                            //--^
                            Obj mV(&testAllocator);
                            testDriver<Element>::stretchRemoveAll(&mV, V_N,
                                                                  VALUES[0]);
                            const Obj& V = mV;
                            testDriver<Element>::gg(&mV, V_SPEC);

                            static int firstFew = 2 * NUM_EXTEND * NUM_EXTEND;
                            if (veryVeryVerbose||veryVerbose && firstFew > 0) {
                                cout << "\t| "; P_(U_N); P_(V_N); P_(U); P(V);
                                --firstFew;
                            }

                            testAllocator.setAllocationLimit(AL);
                            mU = V; // test assignment here

                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == U);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == V);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N,  V == U);
                            //--v
                            }
                            // 'mV' (and therefore 'V') now out of scope
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == U);
                          } END_BDEMA_EXCEPTION_TEST

                          }
                          ++iteration;
                        }
                    }
                }
            }
            }
        }

        if (verbose) cout << "\nTesting self assignment (Aliasing)." << endl;
        {
            static const char *SPECS[] = { // len: 0 - 5, 7, 8, 9, 15, 16, 17
                "",      "A",      "BC",     "CDE",    "DEAB",   "EABCD",
                "ABCDEAB",         "ABCDEABC",         "ABCDEABCD",
                "ABCDEABCDEABCDE", "ABCDEABCDEABCDEA", "ABCDEABCDEABCDEAB",
            0}; // Null string required as last element.

            static const int EXTEND[] = {
                0, 1, 2, 3, 4, 5, 7, 8, 9, 15, 16, 17
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC = SPECS[ti];
                const int curLen = (int) strlen(SPEC);

                if (verbose) {
                    cout << "\tFor an object of length " << curLen << ":\t";
                    P(SPEC);
                }
                LOOP_ASSERT(SPEC, oldLen < curLen);  // strictly increasing
                oldLen = curLen;

                // control
                const Obj X = testDriver<Element>::g(SPEC);
                LOOP_ASSERT(ti, curLen == X.length());  // same lengths

                for (int tj = 0; tj < NUM_EXTEND; ++tj) {
                  BEGIN_BDEMA_EXCEPTION_TEST {
                    const int AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);

                    const int N = EXTEND[tj];
                    Obj mY(&testAllocator);
                    testDriver<Element>::stretchRemoveAll(&mY, N, VALUES[0]);
                    const Obj& Y = mY;
                    testDriver<Element>::gg(&mY, SPEC);

                    if (veryVerbose) { cout << "\t\t"; P_(N); P(Y); }

                    LOOP2_ASSERT(SPEC, N, Y == Y);
                    LOOP2_ASSERT(SPEC, N, X == Y);

                    testAllocator.setAllocationLimit(AL);
                    mY = Y; // test assignment here

                    LOOP2_ASSERT(SPEC, N, Y == Y);
                    LOOP2_ASSERT(SPEC, N, X == Y);

                  } END_BDEMA_EXCEPTION_TEST
                }
            }
        }
}

//=============================================================================
//                             Test Case 11
//-----------------------------------------------------------------------------

template <class Element>
void TestCases<Element>::testcase11(
     int& testStatus, int argc, char *argv[],
     int verbose, int veryVerbose, int veryVeryVerbose,
     bdema_TestAllocator& testAllocator)
{
    typedef bdea_Array<Element> Obj;
    const Element *VALUES;
    testDriver<Element>::getValues(&VALUES);

    if (verbose) cout << "\nTesting initial-length ctor with unspecified "
                     "(default) initial value." << endl;

    if (verbose) cout << "\tWithout passing in an allocator." << endl;
    {
        const int DEFAULT_VALUE = 0;
        const int MAX_LENGTH = 10;
        for (int length = 0; length <= MAX_LENGTH; ++length) {
            if (verbose) P(length);
            Obj mX(length);  const Obj& X = mX;
            if (veryVerbose) P(X);
            LOOP_ASSERT(length, length == X.length());
            for (int i = 0; i < length; ++i) {
                LOOP2_ASSERT(length, i, DEFAULT_VALUE == X[i]);
            }
        }
    }

    if (verbose) cout << "\tPassing in an allocator." << endl;

    if (verbose) cout << "\t\tWith no exceptions." << endl;
    {
        const int DEFAULT_VALUE = 0;
        const int MAX_LENGTH = 10;
        for (int length = 0; length <= MAX_LENGTH; ++length) {
            if (verbose) P(length);
            Obj mX(length, &testAllocator);
            const Obj& X = mX;
            if (veryVerbose) P(X);
            LOOP_ASSERT(length, length == X.length());
            for (int i = 0; i < length; ++i) {
                LOOP2_ASSERT(length, i, DEFAULT_VALUE == X[i]);
            }
        }
    }

    if (verbose) cout << "\t\tWith exceptions." << endl;
    {
        const int DEFAULT_VALUE = 0;
        const int MAX_LENGTH = 10;
        for (int length = 0; length <= MAX_LENGTH; ++length) {
          BEGIN_BDEMA_EXCEPTION_TEST {
            if (verbose) P(length);
            Obj mX(length, &testAllocator);
            const Obj& X = mX;
            if (veryVerbose) P(X);
            LOOP_ASSERT(length, length == X.length());
            for (int i = 0; i < length; ++i) {
                LOOP2_ASSERT(length, i, DEFAULT_VALUE == X[i]);
            }
          } END_BDEMA_EXCEPTION_TEST
        }
    }

    if (verbose) cout << "\tIn place using a buffer allocator." << endl;
    {
        char memory[8192];
        bdema_BufferAllocator a(memory, sizeof memory);
        const int DEFAULT_VALUE = 0;
        const int MAX_LENGTH = 10;
        for (int length = 0; length <= MAX_LENGTH; ++length) {
            if (verbose) P(length);
            Obj *doNotDelete =
                new(a.allocate(sizeof(Obj))) Obj(length, &a);
            Obj& mX = *doNotDelete;  const Obj& X = mX;
            if (veryVerbose) P(X);
            LOOP_ASSERT(length, length == X.length());
            for (int i = 0; i < length; ++i) {
                LOOP2_ASSERT(length, i, DEFAULT_VALUE == X[i]);
            }
        }

         // No destructor is called; will produce memory leak in purify
         // if internal allocators are not hooked up properly.
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (verbose) cout <<
        "\nTesting initial-length ctor with user-specified initial value."
                                                                   << endl;
    if (verbose) cout << "\tWithout passing in an allocator." << endl;
    {
        const int VALUE = 99;
        const int MAX_LENGTH = 10;
        for (int length = 0; length <= MAX_LENGTH; ++length) {
            if (verbose) P(length);
            Obj mX(length, VALUE);  const Obj& X = mX;
            if (veryVerbose) P(X);
            LOOP_ASSERT(length, length == X.length());
            for (int i = 0; i < length; ++i) {
                LOOP2_ASSERT(length, i, VALUE == X[i]);
            }
        }
    }

    if (verbose) cout << "\tPassing in an allocator." << endl;

    if (verbose) cout << "\t\tWith no exceptions." << endl;
    {
        const int VALUE = 101;
        const int MAX_LENGTH = 10;
        for (int length = 0; length <= MAX_LENGTH; ++length) {
            if (verbose) P(length);
            Obj mX(length, VALUE, &testAllocator);
            const Obj& X = mX;
            if (veryVerbose) P(X);
            LOOP_ASSERT(length, length == X.length());
            for (int i = 0; i < length; ++i) {
                LOOP2_ASSERT(length, i, VALUE == X[i]);
            }
        }
    }

    if (verbose) cout << "\t\tWith exceptions." << endl;
    {
        const int VALUE = 103;
        const int MAX_LENGTH = 10;
        for (int length = 0; length <= MAX_LENGTH; ++length) {
          BEGIN_BDEMA_EXCEPTION_TEST {
            if (verbose) P(length);
            Obj mX(length, VALUE, &testAllocator);
            const Obj& X = mX;
            if (veryVerbose) P(X);
            LOOP_ASSERT(length, length == X.length());
            for (int i = 0; i < length; ++i) {
                LOOP2_ASSERT(length, i, VALUE == X[i]);
            }
          } END_BDEMA_EXCEPTION_TEST
        }
    }

    if (verbose) cout << "\tIn place using a buffer allocator." << endl;
    {
        char memory[8192];
        bdema_BufferAllocator a(memory, sizeof memory);
        const int VALUE = 104;
        const int MAX_LENGTH = 10;
        for (int length = 0; length <= MAX_LENGTH; ++length) {
            if (verbose) P(length);
            Obj *doNotDelete =
                new(a.allocate(sizeof(Obj)))
                    Obj(length, VALUE, &a);
            Obj& mX = *doNotDelete;  const Obj& X = mX;
            if (veryVerbose) P(X);
            LOOP_ASSERT(length, length == X.length());
            for (int i = 0; i < length; ++i) {
                LOOP2_ASSERT(length, i, VALUE == X[i]);
            }
        }
        // No destructor is called; will produce memory leak in purify
        // if internal allocators are not hooked up properly.
    }
}

//=============================================================================
//                             Test Case 12
//-----------------------------------------------------------------------------

template <class Element>
void TestCases<Element>::testcase12(
     int& testStatus, int argc, char *argv[],
     int verbose, int veryVerbose, int veryVeryVerbose,
     bdema_TestAllocator& testAllocator)
{
    typedef bdea_Array<Element> Obj;
    const Element *VALUES;
    testDriver<Element>::getValues(&VALUES);

        if (verbose) cout << "\nTesting 'setLength(int)'" << endl;
        {
            const int lengths[] = { 0, 1, 2, 3, 4, 5, 7, 8, 9, 15, 16, 17 };
            const int NUM_TESTS = sizeof lengths / sizeof lengths[0];
            const int I_VALUE       = 22;
            const int DEFAULT_VALUE = 0;
            for (int i = 0; i < NUM_TESTS; ++i) {
                const int a1 = lengths[i];
                if (verbose) { cout << "\t"; P(a1); }
                for (int j = 0; j < NUM_TESTS; ++j) {
                    const int a2 = lengths[j];
                    if (veryVerbose) { cout << "\t\t"; P(a2); }
                  BEGIN_BDEMA_EXCEPTION_TEST {
                    const int AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);
                    Obj mX(a1, I_VALUE, &testAllocator);
                    testAllocator.setAllocationLimit(AL);
                    const Obj& X = mX;
                    if (veryVerbose) P(X);
                    LOOP2_ASSERT(i, j, a1 == X.length());
                    mX.setLength(a2);
                    if (veryVerbose) P(X);
                    LOOP2_ASSERT(i, j, a2 == X.length());
                    for (int k = 0; k < a2; ++k) {
                        if (k < a1) {
                            LOOP3_ASSERT(i, j, k, I_VALUE == X[k]);
                        }
                        else {
                            LOOP3_ASSERT(i, j, k, DEFAULT_VALUE == X[k]);
                        }
                    }
                  } END_BDEMA_EXCEPTION_TEST
                }
            }
        }

        if (verbose) cout << "\nTesting 'setLength(int, int)'" << endl;
        {
            const int lengths[] = { 0, 1, 2, 3, 4, 5, 7, 8, 9, 15, 16, 17 };
            const int NUM_TESTS = sizeof lengths / sizeof lengths[0];
            const int I_VALUE = 105;
            const int F_VALUE = 107;
            for (int i = 0; i < NUM_TESTS; ++i) {
                const int a1 = lengths[i];
                if (verbose) { cout << "\t"; P(a1); }
                for (int j = 0; j < NUM_TESTS; ++j) {
                    const int a2 = lengths[j];
                    if (veryVerbose) { cout << "\t\t"; P(a2); }
                  BEGIN_BDEMA_EXCEPTION_TEST {
                    const int AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);
                    Obj mX(a1, I_VALUE, &testAllocator);
                    testAllocator.setAllocationLimit(AL);
                    const Obj& X = mX;
                    if (veryVerbose) P(X);
                    LOOP2_ASSERT(i, j, a1 == X.length());
                    mX.setLength(a2, F_VALUE);
                    if (veryVerbose) P(X);
                    LOOP2_ASSERT(i, j, a2 == X.length());
                    for (int k = 0; k < a2; ++k) {
                        if (k < a1) {
                            LOOP3_ASSERT(i, j, k, I_VALUE == X[k]);
                        }
                        else {
                            LOOP3_ASSERT(i, j, k, F_VALUE == X[k]);
                        }
                    }
                  } END_BDEMA_EXCEPTION_TEST
                }
            }
        }
}

//=============================================================================
//                             Test Case 13
//-----------------------------------------------------------------------------

template <class Element>
void TestCases<Element>::testcase13Insert(
     int& testStatus, int argc, char *argv[],
     int verbose, int veryVerbose, int veryVeryVerbose,
     const InsertTable *data, int numData,
     int hasInternalAllocation, bdema_TestAllocator& testAllocator)
{
      typedef bdea_Array<Element> Obj;
      const Element *VALUES;
      testDriver<Element>::getValues(&VALUES);

      int oldDepth = -1;
      for (int ti = 0; ti < numData; ++ti) {
        const int   LINE   = data[ti].d_lineNum;
        const char *D_SPEC = data[ti].d_daSpec;
        const int   DI     = data[ti].d_di;
        const char *S_SPEC = data[ti].d_saSpec;
        const int   SI     = data[ti].d_si;
        const int   NE     = data[ti].d_ne;
        const char *E_SPEC = data[ti].d_expSpec;

        const int   DEPTH  = (int) strlen(D_SPEC) + (int) strlen(S_SPEC);
        if (DEPTH > oldDepth) {
            oldDepth = DEPTH;
            if (verbose) { cout << '\t';  P(DEPTH); }
        }

        Obj DD(testDriver<Element>::g(D_SPEC)); // control for destination
        Obj SS(testDriver<Element>::g(S_SPEC)); // control for source
        Obj EE(testDriver<Element>::g(E_SPEC)); // control for expected value

        if (veryVerbose) {
            cout << "\t  =================================="
                        "==================================" << endl;
            cout << "\t  "; P_(D_SPEC); P_(DI); P_(S_SPEC);
                                        P_(SI); P_(NE); P(E_SPEC);
            cout << "\t\t"; P(DD);
            cout << "\t\t"; P(SS);
            cout << "\t\t"; P(EE);

            cout << "\t\t\t---------- BLACK BOX ----------" << endl;
        }

        if (veryVerbose) cout << "\t\tinsert(di, sa, si, ne)" << endl;
        {
          BEGIN_BDEMA_EXCEPTION_TEST {
            Obj x(DD, &testAllocator);  const Obj &X = x;
            {
                Obj s(SS, &testAllocator);  const Obj &S = s;
                if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                x.insert(DI, s, SI, NE);        // source non-'const'
                if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                LOOP_ASSERT(LINE, EE == X);
                LOOP_ASSERT(LINE, SS == S);     // source unchanged?
            }
            LOOP_ASSERT(LINE, EE == X);  // source is out of scope
          } END_BDEMA_EXCEPTION_TEST
        }

        if (veryVerbose) cout << "\t\tinsert(di, sa)" << endl;
        if ((int) strlen(S_SPEC) == NE) {
          BEGIN_BDEMA_EXCEPTION_TEST {
            Obj x(DD, &testAllocator);  const Obj &X = x;
            {
                Obj s(SS, &testAllocator);  const Obj &S = s;
                if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                x.insert(DI, s);                // source non-'const'
                if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                LOOP_ASSERT(LINE, EE == X);
                LOOP_ASSERT(LINE, SS == S);     // source unchanged?
            }
            LOOP_ASSERT(LINE, EE == X);  // source is out of scope
          } END_BDEMA_EXCEPTION_TEST
        }

        if (veryVerbose) cout << "\t\tinsert(di, item)" << endl;
        if (1 == NE) {
          BEGIN_BDEMA_EXCEPTION_TEST {
            Obj x(DD, &testAllocator);  const Obj &X = x;
            {
                Obj s(SS, &testAllocator);  const Obj &S = s;
                if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                x.insert(DI, s[SI]);            // source non-'const'
                if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                LOOP_ASSERT(LINE, EE == X);
                LOOP_ASSERT(LINE, SS == S);     // source unchanged?
            }
            LOOP_ASSERT(LINE, EE == X);  // source is out of scope
          } END_BDEMA_EXCEPTION_TEST
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (veryVerbose) cout << "\t\tappend(sa, si, ne)" << endl;
        if ((int) strlen(D_SPEC) == DI) {
          BEGIN_BDEMA_EXCEPTION_TEST {
            Obj x(DD, &testAllocator);  const Obj &X = x;
            {
                Obj s(SS, &testAllocator);  const Obj &S = s;
                if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                x.append(s, SI, NE);            // source non-'const'
                if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                LOOP_ASSERT(LINE, EE == X);
                LOOP_ASSERT(LINE, SS == S);     // source unchanged?
            }
            LOOP_ASSERT(LINE, EE == X);  // source is out of scope
          } END_BDEMA_EXCEPTION_TEST
        }

        if (veryVerbose) cout << "\t\tappend(sa)" << endl;
        if ((int) strlen(D_SPEC) == DI && (int) strlen(S_SPEC) == NE) {
          BEGIN_BDEMA_EXCEPTION_TEST {
            Obj x(DD, &testAllocator);  const Obj &X = x;
            {
                Obj s(SS, &testAllocator);  const Obj &S = s;
                if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                x.append(s);                    // source non-'const'
                if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                LOOP_ASSERT(LINE, EE == X);
                LOOP_ASSERT(LINE, SS == S);     // source unchanged?
            }
            LOOP_ASSERT(LINE, EE == X);  // source is out of scope
          } END_BDEMA_EXCEPTION_TEST
        }

        if (veryVerbose) cout << "\t\tappend(item)" << endl;
        if ((int) strlen(D_SPEC) == DI && 1 == NE) {
          BEGIN_BDEMA_EXCEPTION_TEST {
            Obj x(DD, &testAllocator);  const Obj &X = x;
            {
                Obj s(SS, &testAllocator);  const Obj &S = s;
                if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                x.append(s[SI]);                // source non-'const'
                if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                LOOP_ASSERT(LINE, EE == X);
                LOOP_ASSERT(LINE, SS == S);     // source unchanged?
            }
            LOOP_ASSERT(LINE, EE == X);  // source is out of scope
          } END_BDEMA_EXCEPTION_TEST
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (veryVerbose) cout << "\t\tremove(index, ne)" << endl;
        {
          BEGIN_BDEMA_EXCEPTION_TEST {  // Note specs are switched.
            Obj x(EE, &testAllocator);  const Obj &X = x;
            if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
            x.remove(DI, NE);
            if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
            LOOP_ASSERT(LINE, DD == X);
          } END_BDEMA_EXCEPTION_TEST
        }

        if (veryVerbose) cout << "\t\tremove(index)" << endl;
        if (1 == NE) {
          BEGIN_BDEMA_EXCEPTION_TEST {  // Note specs are switched
            Obj x(EE, &testAllocator);  const Obj &X = x;
            if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
            x.remove(DI);
            if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
            LOOP_ASSERT(LINE, DD == X);
          } END_BDEMA_EXCEPTION_TEST
        }

        if (veryVerbose) {
            cout << "\t\t\t---------- WHITE BOX ----------" << endl;
        }

        Obj x(&testAllocator);  const Obj &X = x;

        const int STRETCH_SIZE = 50;

        testDriver<Element>::stretchRemoveAll(&x, STRETCH_SIZE, VALUES[0]);

        const int NUM_BLOCKS = testAllocator.numBlocksTotal();
        const int NUM_BYTES  = testAllocator.numBytesInUse();

        if (veryVerbose) cout << "\t\tinsert(di, sa, si, ne)" << endl;
        {
            x.removeAll();  testDriver<Element>::gg(&x, D_SPEC);
            if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
            const int NUM_CTOR = Element::getCopyCtorCt();
            const int NUM_DTOR = Element::getDtorCt();
            x.insert(DI, SS, SI, NE);

            ASSERT(Element::verifyAlgorithm(NUM_CTOR, NUM_DTOR, NE, 0));
            if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
            LOOP_ASSERT(LINE, EE == X);
        }

        if (veryVerbose) cout << "\t\tinsert(di, sa)" << endl;
        if ((int) strlen(S_SPEC) == NE) {
            x.removeAll();  testDriver<Element>::gg(&x, D_SPEC);
            if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
            const int NUM_CTOR = Element::getCopyCtorCt();
            const int NUM_DTOR = Element::getDtorCt();
            x.insert(DI, SS);
            ASSERT(Element::verifyAlgorithm(NUM_CTOR, NUM_DTOR, NE, 0));
            if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
            LOOP_ASSERT(LINE, EE == X);
        }

        if (veryVerbose) cout << "\t\tinsert(di, item)" << endl;
        if (1 == NE) {
            x.removeAll();  testDriver<Element>::gg(&x, D_SPEC);
            if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
            const int NUM_CTOR = Element::getCopyCtorCt();
            const int NUM_DTOR = Element::getDtorCt();
            x.insert(DI, SS[SI]);
            ASSERT(Element::verifyAlgorithm(NUM_CTOR, NUM_DTOR, NE, 0));
            if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
            LOOP_ASSERT(LINE, EE == X);
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (veryVerbose) cout << "\t\tappend(sa, si, ne)" << endl;
        if ((int) strlen(D_SPEC) == DI) {
            x.removeAll();  testDriver<Element>::gg(&x, D_SPEC);
            if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
            const int NUM_CTOR = Element::getCopyCtorCt();
            const int NUM_DTOR = Element::getDtorCt();
            x.append(SS, SI, NE);
            ASSERT(Element::verifyAlgorithm(NUM_CTOR, NUM_DTOR, NE, 0));
            if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
            LOOP_ASSERT(LINE, EE == X);
        }

        if (veryVerbose) cout << "\t\tappend(sa)" << endl;
        if ((int) strlen(D_SPEC) == DI && (int) strlen(S_SPEC) == NE) {
            x.removeAll();  testDriver<Element>::gg(&x, D_SPEC);
            if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
            const int NUM_CTOR = Element::getCopyCtorCt();
            const int NUM_DTOR = Element::getDtorCt();
            x.append(SS);
            ASSERT(Element::verifyAlgorithm(NUM_CTOR, NUM_DTOR, NE, 0));
            if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
            LOOP_ASSERT(LINE, EE == X);
        }

        if (veryVerbose) cout << "\t\tappend(item)" << endl;
        if ((int) strlen(D_SPEC) == DI && 1 == NE) {
            x.removeAll();  testDriver<Element>::gg(&x, D_SPEC);
            if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
            const int NUM_CTOR = Element::getCopyCtorCt();
            const int NUM_DTOR = Element::getDtorCt();
            x.append(SS[SI]);
            ASSERT(Element::verifyAlgorithm(NUM_CTOR, NUM_DTOR, NE, 0));
            if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
            LOOP_ASSERT(LINE, EE == X);
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (veryVerbose) cout << "\t\tremove(index, ne)" << endl;
        {
            x.removeAll();
            testDriver<Element>::gg(&x, E_SPEC); // Note: specs switched!
            if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
            const int NUM_CTOR = Element::getCopyCtorCt();
            const int NUM_DTOR = Element::getDtorCt();
            x.remove(DI, NE);
            ASSERT(Element::verifyAlgorithm(NUM_CTOR, NUM_DTOR, 0, NE));
            if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
            LOOP_ASSERT(LINE, DD == X);
        }

        if (veryVerbose) cout << "\t\tremove(index)" << endl;
        if (1 == NE) {
            x.removeAll();
            testDriver<Element>::gg(&x, E_SPEC);  // Note: specs switched!
            if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
            const int NUM_CTOR = Element::getCopyCtorCt();
            const int NUM_DTOR = Element::getDtorCt();
            x.remove(DI);
            ASSERT(Element::verifyAlgorithm(NUM_CTOR, NUM_DTOR, 0, NE));
            if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
            LOOP_ASSERT(LINE, DD == X);
        }

        if (!hasInternalAllocation) {
            LOOP_ASSERT(LINE,
                        NUM_BLOCKS ==testAllocator.numBlocksTotal());
            LOOP_ASSERT(LINE,
                        NUM_BYTES == testAllocator.numBytesInUse());
        }
      }
}

template <class Element>
void TestCases<Element>::testcase13InsertAlias(
     int& testStatus, int argc, char *argv[],
     int verbose, int veryVerbose, int veryVeryVerbose,
     const InsertAliasTable *data, int numData,
     int hasInternalAllocation, bdema_TestAllocator& testAllocator)
{
    typedef bdea_Array<Element> Obj;
    const Element              *VALUES;
    testDriver<Element>::getValues(&VALUES);

    {
      int oldDepth = -1;
      for (int ti = 0; ti < numData; ++ti) {
        const int   LINE   = data[ti].d_lineNum;
        const char *X_SPEC = data[ti].d_xSpec;
        const int   DI     = data[ti].d_di;
        const int   SI     = data[ti].d_si;
        const int   NE     = data[ti].d_ne;
        const char *E_SPEC = data[ti].d_expSpec;

        const int   DEPTH  = (int) strlen(X_SPEC);
        if (DEPTH > oldDepth) {
            oldDepth = DEPTH;
            if (verbose) { cout << '\t';  P(DEPTH); }
        }

        // control for destination
        Obj DD(testDriver<Element>::g(X_SPEC));

        // control for expected value
        Obj EE(testDriver<Element>::g(E_SPEC));

        if (veryVerbose) {
            cout << "\t  =================================="
                        "==================================" << endl;
            cout << "\t  "; P_(X_SPEC); P_(DI);
                                        P_(SI); P_(NE); P(E_SPEC);
            cout << "\t\t"; P(DD);
            cout << "\t\t"; P(EE);


            cout << "\t\t\t---------- BLACK BOX ----------" << endl;
        }

        // CONTROL: ensure same table result as non-alias case.
        {
            Obj x(DD, &testAllocator);  const Obj &X = x;
            Obj x2(DD, &testAllocator);
            const Obj &X2 = x2; // control
            x2.insert(DI, X, SI, NE);
            LOOP_ASSERT(LINE, EE == X2);
        }

        if (veryVerbose) cout << "\t\tinsert(di, sa, si, ne)" << endl;
        {
          BEGIN_BDEMA_EXCEPTION_TEST {
            Obj x(DD, &testAllocator);  const Obj &X = x;
            if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
            x.insert(DI, X, SI, NE);
            if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
            LOOP_ASSERT(LINE, EE == X);
          } END_BDEMA_EXCEPTION_TEST
        }

        if (veryVerbose) cout << "\t\tinsert(di, sa)" << endl;
        if ((int) strlen(X_SPEC) == NE) {
          BEGIN_BDEMA_EXCEPTION_TEST {
            Obj x(DD, &testAllocator);  const Obj &X = x;
            if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
            x.insert(DI, X);
            if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
            LOOP_ASSERT(LINE, EE == X);
          } END_BDEMA_EXCEPTION_TEST
        }

        if (veryVerbose) cout << "\t\tinsert(di, item)" << endl;
        if (1 == NE) {
          BEGIN_BDEMA_EXCEPTION_TEST {
            Obj x(DD, &testAllocator);  const Obj &X = x;
            if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
            x.insert(DI, X[SI]);
            if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
            LOOP_ASSERT(LINE, EE == X);
          } END_BDEMA_EXCEPTION_TEST
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (veryVerbose) cout << "\t\tappend(sa, si, ne)" << endl;
        if ((int) strlen(X_SPEC) == DI) {
          BEGIN_BDEMA_EXCEPTION_TEST {
            Obj x(DD, &testAllocator);  const Obj &X = x;
            if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
            x.append(X, SI, NE);
            if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
            LOOP_ASSERT(LINE, EE == X);
          } END_BDEMA_EXCEPTION_TEST
        }

        if (veryVerbose) cout << "\t\tappend(sa)" << endl;
        if ((int) strlen(X_SPEC) == DI && (int) strlen(X_SPEC) == NE) {
          BEGIN_BDEMA_EXCEPTION_TEST {
            Obj x(DD, &testAllocator);  const Obj &X = x;
            if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
            x.append(X);
            if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
            LOOP_ASSERT(LINE, EE == X);
          } END_BDEMA_EXCEPTION_TEST
        }

        if (veryVerbose) cout << "\t\tappend(item)" << endl;
        if ((int) strlen(X_SPEC) == DI && 1 == NE) {
          BEGIN_BDEMA_EXCEPTION_TEST {
            Obj x(DD, &testAllocator);  const Obj &X = x;
            if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
            x.append(X[SI]);
            if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
            LOOP_ASSERT(LINE, EE == X);
          } END_BDEMA_EXCEPTION_TEST
        }

        if (veryVerbose) {
            cout << "\t\t\t---------- WHITE BOX ----------" << endl;
        }

        Obj x(&testAllocator);  const Obj &X = x;

        const int STRETCH_SIZE = 50;

        testDriver<Element>::stretchRemoveAll(&x, STRETCH_SIZE, VALUES[0]);

        const int NUM_BLOCKS = testAllocator.numBlocksTotal();
        const int NUM_BYTES  = testAllocator.numBytesInUse();

        if (veryVerbose) cout << "\t\tinsert(di, sa, si, ne)" << endl;
        {
            x.removeAll();  testDriver<Element>::gg(&x, X_SPEC);
            if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
            const int NUM_CTOR = Element::getCopyCtorCt();
            const int NUM_DTOR = Element::getDtorCt();
            x.insert(DI, X, SI, NE);
            ASSERT(Element::verifyAlgorithm(NUM_CTOR, NUM_DTOR, NE, 0));
            if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
            LOOP_ASSERT(LINE, EE == X);
        }

        if (veryVerbose) cout << "\t\tinsert(di, sa)" << endl;
        if ((int) strlen(X_SPEC) == NE) {
            x.removeAll();  testDriver<Element>::gg(&x, X_SPEC);
            if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
            const int NUM_CTOR = Element::getCopyCtorCt();
            const int NUM_DTOR = Element::getDtorCt();
            x.insert(DI, X);
            ASSERT(Element::verifyAlgorithm(NUM_CTOR, NUM_DTOR, NE, 0));
            if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
            LOOP_ASSERT(LINE, EE == X);
        }

        if (veryVerbose) cout << "\t\tinsert(di, item)" << endl;
        if (1 == NE) {
            x.removeAll();  testDriver<Element>::gg(&x, X_SPEC);
            if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
            const int NUM_CTOR = Element::getCopyCtorCt();
            const int NUM_DTOR = Element::getDtorCt();
            x.insert(DI, X[SI]);
            ASSERT(Element::verifyAlgorithm(NUM_CTOR, NUM_DTOR, NE, 0));
            if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
            LOOP_ASSERT(LINE, EE == X);
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (veryVerbose) cout << "\t\tappend(sa, si, ne)" << endl;
        if ((int) strlen(X_SPEC) == DI) {
            x.removeAll();  testDriver<Element>::gg(&x, X_SPEC);
            if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
            const int NUM_CTOR = Element::getCopyCtorCt();
            const int NUM_DTOR = Element::getDtorCt();
            x.append(X, SI, NE);
            ASSERT(Element::verifyAlgorithm(NUM_CTOR, NUM_DTOR, NE, 0));
            if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
            LOOP_ASSERT(LINE, EE == X);
        }

        if (veryVerbose) cout << "\t\tappend(sa)" << endl;
        if ((int) strlen(X_SPEC) == DI && (int) strlen(X_SPEC) == NE) {
            x.removeAll();  testDriver<Element>::gg(&x, X_SPEC);
            if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
            const int NUM_CTOR = Element::getCopyCtorCt();
            const int NUM_DTOR = Element::getDtorCt();
            x.append(X);
            ASSERT(Element::verifyAlgorithm(NUM_CTOR, NUM_DTOR, NE, 0));
            if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
            LOOP_ASSERT(LINE, EE == X);
        }

        if (veryVerbose) cout << "\t\tappend(item)" << endl;
        if ((int) strlen(X_SPEC) == DI && 1 == NE) {
            x.removeAll();  testDriver<Element>::gg(&x, X_SPEC);
            if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
            const int NUM_CTOR = Element::getCopyCtorCt();
            const int NUM_DTOR = Element::getDtorCt();
            x.append(X[SI]);
            ASSERT(Element::verifyAlgorithm(NUM_CTOR, NUM_DTOR, NE, 0));
            if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
            LOOP_ASSERT(LINE, EE == X);
        }

        if (!hasInternalAllocation) {
            LOOP_ASSERT(LINE,
                        NUM_BLOCKS ==testAllocator.numBlocksTotal());
            LOOP_ASSERT(LINE,
                        NUM_BYTES == testAllocator.numBytesInUse());
        }
      }

    }

}

//=============================================================================
//                             Test Case 14
//-----------------------------------------------------------------------------

template <class Element>
void TestCases<Element>::testcase14(
     int& testStatus, int argc, char *argv[],
     int verbose, int veryVerbose, int veryVeryVerbose,
     const ReplaceTable *data, int numData,
     int hasInternalAllocation, bdema_TestAllocator& testAllocator)
{
    typedef bdea_Array<Element> Obj;
    const Element *VALUES;
    testDriver<Element>::getValues(&VALUES);

            {
              int oldDepth = -1;
              for (int ti = 0; ti < numData; ++ti) {
                const int   LINE   = data[ti].d_lineNum;
                const char *D_SPEC = data[ti].d_daSpec;
                const int   DI     = data[ti].d_di;
                const char *S_SPEC = data[ti].d_saSpec;
                const int   SI     = data[ti].d_si;
                const int   NE     = data[ti].d_ne;
                const char *E_SPEC = data[ti].d_expSpec;

                const int   DEPTH  = (int) strlen(D_SPEC)
                                     + (int) strlen(S_SPEC);
                if (DEPTH > oldDepth) {
                    oldDepth = DEPTH;
                    if (verbose) { cout << '\t';  P(DEPTH); }
                }

                // control for destination
                Obj DD(testDriver<Element>::g(D_SPEC));
                // control for source
                Obj SS(testDriver<Element>::g(S_SPEC));
                // control for expected value
                Obj EE(testDriver<Element>::g(E_SPEC));

                if (veryVerbose) {
                    cout << "\t  =================================="
                                "==================================" << endl;
                    cout << "\t  "; P_(D_SPEC); P_(DI); P_(S_SPEC);
                                                P_(SI); P_(NE); P(E_SPEC);
                    cout << "\t\t"; P(DD);
                    cout << "\t\t"; P(SS);
                    cout << "\t\t"; P(EE);

                    cout << "\t\t\t---------- BLACK BOX ----------" << endl;
                }

                if (veryVerbose) cout << "\t\treplace(di, sa, si, ne)" << endl;
                {
                  BEGIN_BDEMA_EXCEPTION_TEST {
                    Obj x(DD, &testAllocator);  const Obj& X = x;
                    {
                        Obj s(SS, &testAllocator);  const Obj &S = s;
                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                        x.replace(DI, s, SI, NE);       // source non-'const'
                        if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                        LOOP_ASSERT(LINE, EE == X);
                        LOOP_ASSERT(LINE, SS == S);     // source unchanged?
                    }
                    LOOP_ASSERT(LINE, EE == X);  // source is out of scope
                  } END_BDEMA_EXCEPTION_TEST
                }

                if (veryVerbose) cout << "\t\treplace(di, item)" << endl;
                if (1 == NE) {
                  BEGIN_BDEMA_EXCEPTION_TEST {
                    Obj x(DD, &testAllocator);  const Obj &X = x;
                    {
                        Obj s(SS, &testAllocator);  const Obj &S = s;
                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                        x.replace(DI, s[SI]);           // source non-'const'
                        if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                        LOOP_ASSERT(LINE, EE == X);
                        LOOP_ASSERT(LINE, SS == S);     // source unchanged?
                    }
                    LOOP_ASSERT(LINE, EE == X);  // source is out of scope
                  } END_BDEMA_EXCEPTION_TEST
                }

                if (veryVerbose) cout << "\t\toperator[](index)" << endl;
                if (1 == NE) {
                  BEGIN_BDEMA_EXCEPTION_TEST {
                    Obj x(DD, &testAllocator);  const Obj &X = x;
                    {
                        Obj s(SS, &testAllocator);  const Obj &S = s;
                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                        x[DI] = s[SI];                  // source non-'const'
                        if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                        LOOP_ASSERT(LINE, EE == X);
                        LOOP_ASSERT(LINE, SS == S);     // source unchanged?
                    }
                    LOOP_ASSERT(LINE, EE == X);  // source is out of scope
                  } END_BDEMA_EXCEPTION_TEST
                }


                if (veryVerbose) {
                    cout << "\t\t\t---------- WHITE BOX ----------" << endl;
                }

                Obj x(&testAllocator);  const Obj &X = x;

                const int STRETCH_SIZE = 50;

                testDriver<Element>::stretchRemoveAll(&x, STRETCH_SIZE,
                                                      VALUES[0]);

                const int NUM_BLOCKS = testAllocator.numBlocksTotal();
                const int NUM_BYTES  = testAllocator.numBytesInUse();

                if (veryVerbose) cout << "\t\treplace(di, sa, si, ne)" << endl;
                {
                    x.removeAll();  testDriver<Element>::gg(&x, D_SPEC);
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    const int NUM_CTOR = Element::getCopyCtorCt();
                    const int NUM_DTOR = Element::getDtorCt();
                    x.replace(DI, SS, SI, NE);
                    ASSERT(Element::verifyAlgorithm(NUM_CTOR, NUM_DTOR,
                                                    NE, 0));
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                }

                if (veryVerbose) cout << "\t\treplace(di, item)" << endl;
                if (1 == NE) {
                    x.removeAll();  testDriver<Element>::gg(&x, D_SPEC);
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    const int NUM_CTOR = Element::getCopyCtorCt();
                    const int NUM_DTOR = Element::getDtorCt();
                    x.replace(DI, SS[SI]);
                    ASSERT(Element::verifyAlgorithm(NUM_CTOR, NUM_DTOR,
                                                    NE, 0));
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                }

                if (veryVerbose) cout << "\t\toperator[](int index)" << endl;
                if (1 == NE) {
                    x.removeAll();  testDriver<Element>::gg(&x, D_SPEC);
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    x[DI] = SS[SI];
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                }

                if (!hasInternalAllocation) {
                  LOOP_ASSERT(LINE,
                              NUM_BLOCKS ==testAllocator.numBlocksTotal());
                  LOOP_ASSERT(LINE,
                              NUM_BYTES == testAllocator.numBytesInUse());
                }
              }
            }
}

template <class Element>
void TestCases<Element>::testcase14Alias(
     int& testStatus, int argc, char *argv[],
     int verbose, int veryVerbose, int veryVeryVerbose,
     const ReplaceAliasTable *data, int numData,
     bdema_TestAllocator& testAllocator)
{
      typedef bdea_Array<Element> Obj;
      const Element *VALUES;
      testDriver<Element>::getValues(&VALUES);

            {

              int oldDepth = -1;
              for (int ti = 0; ti < numData ; ++ti) {
                const int   LINE   = data[ti].d_lineNum;
                const char *X_SPEC = data[ti].d_xSpec;
                const int   DI     = data[ti].d_di;
                const int   SI     = data[ti].d_si;
                const int   NE     = data[ti].d_ne;
                const char *E_SPEC = data[ti].d_expSpec;

                const int   DEPTH  = (int) strlen(X_SPEC);
                if (DEPTH > oldDepth) {
                    oldDepth = DEPTH;
                    if (verbose) { cout << '\t';  P(DEPTH); }
                }

                // control for destination
                Obj DD(testDriver<Element>::g(X_SPEC));
                // control for expected value
                Obj EE(testDriver<Element>::g(E_SPEC));

                if (veryVerbose) {
                    cout << "\t  =================================="
                                "==================================" << endl;
                    cout << "\t  "; P_(X_SPEC); P_(DI);
                                                P_(SI); P_(NE); P(E_SPEC);
                    cout << "\t\t"; P(DD);
                    cout << "\t\t"; P(EE);


                    cout << "\t\t\t---------- BLACK BOX ----------" << endl;
                }

                // CONTROL: ensure same table result as non-alias case.
                {
                    Obj x(DD, &testAllocator); const Obj &X = x;
                    Obj x2(DD, &testAllocator);
                    const Obj &X2 = x2; // control
                    x2.replace(DI, X, SI, NE);
                    LOOP_ASSERT(LINE, EE == X2);
                }

                if (veryVerbose) cout << "\t\treplace(di, sa, si, ne)" << endl;
                {
                  BEGIN_BDEMA_EXCEPTION_TEST {
                    Obj x(DD, &testAllocator);  const Obj &X = x;
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    x.replace(DI, X, SI, NE);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                  } END_BDEMA_EXCEPTION_TEST
                }

                if (veryVerbose) cout << "\t\treplace(di, item)" << endl;
                if (1 == NE) {
                  BEGIN_BDEMA_EXCEPTION_TEST {
                    Obj x(DD, &testAllocator);  const Obj &X = x;
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    x.replace(DI, X[SI]);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                  } END_BDEMA_EXCEPTION_TEST
                }

                if (veryVerbose) cout << "\t\toperator[](index)" << endl;
                if (1 == NE) {
                  BEGIN_BDEMA_EXCEPTION_TEST {
                    Obj x(DD, &testAllocator);  const Obj &X = x;
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    x[DI] = X[SI];
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                  } END_BDEMA_EXCEPTION_TEST
                }

                if (veryVerbose) {
                    cout << "\t\t\t---------- WHITE BOX ----------" << endl;
                }

                Obj x(&testAllocator);  const Obj &X = x;

                const int STRETCH_SIZE = 50;

                testDriver<Element>::stretchRemoveAll(&x, STRETCH_SIZE,
                                                      VALUES[0]);

                if (veryVerbose) cout << "\t\treplace(di, sa, si, ne)" << endl;
                {
                    x.removeAll();  testDriver<Element>::gg(&x, X_SPEC);
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    x.replace(DI, X, SI, NE);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                }

                if (veryVerbose) cout << "\t\treplace(di, item)" << endl;
                if (1 == NE) {
                    x.removeAll();  testDriver<Element>::gg(&x, X_SPEC);
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    x.replace(DI, X[SI]);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                }

                if (veryVerbose) cout << "\t\toperator[](item)" << endl;
                if (1 == NE) {
                    x.removeAll();  testDriver<Element>::gg(&x, X_SPEC);
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    x[DI] = X[SI];
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                }
              }
            }
}

//=============================================================================
//                             Test Case 15
//-----------------------------------------------------------------------------

template <class Element>
void TestCases<Element>::testcase15(
     int& testStatus, int argc, char *argv[],
     int verbose, int veryVerbose, int veryVeryVerbose,
     bdema_TestAllocator& testAllocator)
{
      typedef bdea_Array<Element> Obj;
      const Element *VALUES;
      testDriver<Element>::getValues(&VALUES);

        if (verbose) cout << "\nTesting swap(index1, index2)" << endl;

        const int NUM_TRIALS = 10;

        Obj mX(&testAllocator);  const Obj& X = mX;  // control

        for (int iLen = 0; iLen < NUM_TRIALS; ++iLen) { // iLen: initial length
            if (verbose) { cout << "\t"; P_(iLen); P(X); }
            for (int pos1 = 0; pos1 < iLen; ++pos1) { // for each position
                for (int pos2 = 0; pos2 < iLen; ++pos2) { // for each position

                  BEGIN_BDEMA_EXCEPTION_TEST {
                    Obj mT(X, &testAllocator);
                    const Obj& T = mT;                  // object under test
                    LOOP3_ASSERT(iLen, pos1, pos2, X == T);

                    // element values distinct from initial
                    mT[pos1] = VALUES[3];
                    mT[pos2] = VALUES[4];

                    if (veryVerbose) { cout << "\t\t";  P_(pos1);
                                       P_(pos2);        P_(T);
                                       cout << " before swap" << endl; }
                    const int NUM_CTOR = Element::getCopyCtorCt();
                    const int NUM_DTOR = Element::getDtorCt();
                    mT.swap(pos1, pos2);
                    ASSERT(Element::verifyAlgorithm(NUM_CTOR, NUM_DTOR, 0, 0));
                    if (veryVerbose) { cout << "\t\t";  P_(pos1);
                                       P_(pos2);        P_(T);
                                       cout << " after swap" << endl;  }

                    LOOP3_ASSERT(iLen, pos1, pos2, VALUES[4] == T[pos1]);

                    const Element& VX = pos1 == pos2 ? VALUES[4] : VALUES[3];
                    LOOP3_ASSERT(iLen, pos1, pos2, VX == T[pos2]);

                    for (int i = 0; i < iLen; ++i) {
                        if (i == pos1 || i == pos2) continue;
                        LOOP4_ASSERT(iLen, pos1, pos2, i,
                                     VALUES[i % 3] == T[i]);
                    }
                  } END_BDEMA_EXCEPTION_TEST
                }
            }
            if (veryVerbose) cout <<
                "\t--------------------------------------------------" << endl;
            mX.append(VALUES[iLen % 3]);  // Extend control with cyclic values.
        }
}

//=============================================================================
//                             Test Case 16
//-----------------------------------------------------------------------------

template <class Element>
void TestCases<Element>::testcase16(
     int& testStatus, int argc, char *argv[],
     int verbose, int veryVerbose, int veryVeryVerbose,
     int hasInternalAllocation, bdema_TestAllocator& testAllocator)
{
      typedef bdea_Array<Element> Obj;
      const Element *VALUES;
      testDriver<Element>::getValues(&VALUES);

        if (verbose) cout <<
            "\nTesting 'bdea_Array<T>(capacity, ba)' Constructor" << endl;
        if (verbose) cout << "\twith a 'bdema_TestAllocator':" << endl;
        {
            const Obj W(&testAllocator);  // control value
            const int MAX_NUM_ELEMS = 9;
            for (int ne = 0; ne <= MAX_NUM_ELEMS; ++ne) {
                if (veryVerbose) { cout << "\t\t"; P(ne) }
              BEGIN_BDEMA_EXCEPTION_TEST {
                const typename Obj::InitialCapacity NE(ne);
                Obj mX(NE, &testAllocator);  const Obj &X = mX;
                LOOP_ASSERT(ne, W == X);
                const int NUM_BLOCKS = testAllocator.numBlocksTotal();
                const int NUM_BYTES  = testAllocator.numBytesInUse();
                if (veryVerbose) P_(X);
                for (int i = 0; i < ne; ++i) {
                    mX.append(VALUES[0]);
                }
                if (veryVerbose) P(X);
                if (!hasInternalAllocation) {
                    LOOP_ASSERT(ne,
                                NUM_BLOCKS == testAllocator.numBlocksTotal());
                    LOOP_ASSERT(ne,
                                NUM_BYTES  == testAllocator.numBytesInUse());
                }
              } END_BDEMA_EXCEPTION_TEST
            }
        }

        if (verbose) cout << "\twith no allocator (exercise only):" << endl;
        {
            const Obj W(&testAllocator);  // control value
            const int MAX_NUM_ELEMS = 9;
            for (int ne = 0; ne <= MAX_NUM_ELEMS; ++ne) {
                if (veryVerbose) { cout << "\t\t"; P(ne) }
                const typename Obj::InitialCapacity NE(ne);
                Obj mX(NE); const Obj &X = mX;
                LOOP_ASSERT(ne, W == X);
                if (veryVerbose) P_(X);
                for (int i = 0; i < ne; ++i) {
                    mX.append(VALUES[0]);
                }
                if (veryVerbose) P(X);
            }
        }

        if (verbose)
            cout << "\twith a buffer allocator (exercise only):" << endl;
        {
            char memory[8192];
            bdema_BufferAllocator a(memory, sizeof memory);
            const Obj W(&testAllocator);  // control value
            const int MAX_NUM_ELEMS = 9;
            for (int ne = 0; ne <= MAX_NUM_ELEMS; ++ne) {
                if (veryVerbose) { cout << "\t\t"; P(ne) }
                const typename Obj::InitialCapacity NE(ne);
                Obj *doNotDelete =
                    new(a.allocate(sizeof(Obj))) Obj(NE, &a);
                Obj &mX = *doNotDelete;  const Obj &X = mX;
                LOOP_ASSERT(ne, W == X);
                if (veryVerbose) P_(X);
                for (int i = 0; i < ne; ++i) {
                    mX.append(VALUES[0]);
                }
                if (veryVerbose) P(X);
            }
             // No destructor is called; will produce memory leak in purify
             // if internal allocators are not hooked up properly.
        }
}

template <class Element>
void TestCases<Element>::testcase16ReserveCapacity(
     int& testStatus, int argc, char *argv[],
     int verbose, int veryVerbose, int veryVeryVerbose,
     const struct ReserveCapacityTable *data, int numData,
     int hasInternalAllocation, bdema_TestAllocator& testAllocator)
{
      typedef bdea_Array<Element> Obj;
      const Element *VALUES;
      testDriver<Element>::getValues(&VALUES);

      if (verbose)
            cout << "\nTesting the 'reserveCapacityRaw' method" << endl;
      {
              for (int ti = 0; ti < numData ; ++ti) {
                BEGIN_BDEMA_EXCEPTION_TEST {
                  const int   LINE = data[ti].d_lineNum;
                  const char *SPEC = data[ti].d_x;
                  const int   NE   = data[ti].d_ne;
                  if (veryVerbose) { cout << "\t\t"; P_(SPEC); P(NE); }

                  const Obj W(testDriver<Element>::g(SPEC), &testAllocator);
                  Obj mX(W, &testAllocator);  const Obj &X = mX;
                  mX.reserveCapacityRaw(NE);
                  LOOP_ASSERT(LINE, W == X);
                  const int NUM_BLOCKS = testAllocator.numBlocksTotal();
                  const int NUM_BYTES  = testAllocator.numBytesInUse();
                  if (veryVerbose) P_(X);
                  for (int i = X.length(); i < NE; ++i) {
                      mX.append(VALUES[0]);
                  }
                  if (veryVerbose) P(X);
                  if (!hasInternalAllocation) {
                      LOOP_ASSERT(LINE,
                                  NUM_BLOCKS== testAllocator.numBlocksTotal());
                      LOOP_ASSERT(LINE,
                                  NUM_BYTES == testAllocator.numBytesInUse());
                  }
              } END_BDEMA_EXCEPTION_TEST
            }
      }

      if (verbose)
            cout << "\nTesting the 'reserveCapacity' method" << endl;
      {
              for (int ti = 0; ti < numData ; ++ti) {
                BEGIN_BDEMA_EXCEPTION_TEST {
                  const int   LINE   = data[ti].d_lineNum;
                  const char *SPEC   = data[ti].d_x;
                  const int   NE     = data[ti].d_ne;
                  const int   ALLOC  = data[ti].d_alloc;
                  if (veryVerbose) { cout << "\t\t"; P_(SPEC); P(NE); }

                  const Obj W(testDriver<Element>::g(SPEC), &testAllocator);
                  Obj mX(W, &testAllocator);  const Obj &X = mX;
                  const int BT = testAllocator.numBytesTotal();
                  {
                      // Verify that X doesn't change after exceptions in
                      // reserveCapacity by comparing it to the control W.

                      EqualityTester<Obj> chX(&X, &W);

                      mX.reserveCapacity(NE);
                  }
                  LOOP_ASSERT(LINE, W == X);

                  // Memory allocated should be as specified.
                  if (!hasInternalAllocation) {
                      ASSERT(ALLOC
                        == (testAllocator.numBytesTotal() - BT)
                        / (int)sizeof(Element))
                  }

                  const int NUM_BLOCKS = testAllocator.numBlocksTotal();
                  const int NUM_BYTES  = testAllocator.numBytesInUse();

                  if (veryVerbose) P_(X);
                  for (int i = X.length(); i < NE; ++i) {
                      mX.append(VALUES[0]);
                  }
                  if (veryVerbose) P(X);
                  if (!hasInternalAllocation) {
                      LOOP_ASSERT(LINE,
                                  NUM_BLOCKS== testAllocator.numBlocksTotal());
                      LOOP_ASSERT(LINE,
                                  NUM_BYTES == testAllocator.numBytesInUse());
                  }
              } END_BDEMA_EXCEPTION_TEST
            }
     }
}

//=============================================================================
//                             Test Case 18
//-----------------------------------------------------------------------------

template <class Element>
void TestCases<Element>::testcase18(
     int& testStatus, int argc, char *argv[],
     int verbose, int veryVerbose, int veryVeryVerbose,
     bdema_TestAllocator& testAllocator)
{
      typedef bdea_Array<Element> Obj;
      const Element *VALUES;
      int NUM_VALUES = testDriver<Element>::getValues(&VALUES);

      Obj mX(&testAllocator);  const Obj& X = mX;  // object under test

      if (verbose) cout << "\tVerify X.data() + i same as &X[i]." <<endl;

      const int NUM_TRIALS = 10;
      for (int iLen = 0; iLen < NUM_TRIALS; ++iLen) {
          if (verbose) { cout << "\t\t"; P_(iLen); P(X); }
          for (int i = 0; i < iLen; ++i) {
              if (veryVerbose) { cout << "\t\t\t"; P(i); }
              LOOP2_ASSERT(iLen, i,    &X[i] == X.data() + i);
              LOOP2_ASSERT(iLen, i, X.data() == mX.data());
          }
          const Element TEST_VALUE = VALUES[iLen % NUM_VALUES];
          mX.append(TEST_VALUE);  // Extend length of object under test.
      }

      if (verbose) cout <<
             "\tVerify const/non-const 'data()' return correct element access."
                        << endl;
      Element *dummy1 = mX.data();       // data in modifiable object
      const Element *dummy2 = X.data();  // data in non-modifiable object
      ASSERT(dummy1 && dummy2);

      Element *(Obj::*f1)() = &Obj::data;              // non-const method
      const Element *(Obj::*f2)() const = &Obj::data;  // const method
      ASSERT(f1 && f2);
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

#if defined(BDES_PLATFORM__CMP_EPC)
    // The EPC C++ compiler on DGUX has a bug in the constructor for
    // ostrstream that take abuffer and a length.  The constructor will
    // destroy the last byte of the variable immediately preceeding the
    // buffer used to hold the stream.  If the parameter specifying the
    // buffer passed to the constructor is 1 less than the size of the buffer
    // stack integrity is maintained.
    const int epcBug = 1;
#else
    const int epcBug = 0;
#endif

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    bdema_TestAllocator testAllocator(veryVeryVerbose);

    switch (test) { case 0:  // Zero is always the leading case.
      case 18: {
        // --------------------------------------------------------------------
        // TESTING INTERNAL DATA ACCESS METHODS
        //   The concerns are as follows:
        //     - Data can be called on both 'const' and non-'const' objects.
        //     - The address returned for both is identical to &array[0].
        //     - When called on a 'const' object, the value returned is 'const'
        //       (plan: pass to function with 'const' signatures as proof).
        //     - When called on a non-'const' object, the value returned is
        //       non-'const'  (plan: use it to modify contents as proof).
        //
        // Plan:
        //   For each object X in a set of 'bdea_Array<T>' objects ordered
        //   by increasing length L:
        //     For each valid index position P:
        //        Assert X.data() + P == &X[P] for 'const' and non-'const'
        //        methods.
        //
        //   Ensure (at compile time) that the non-const data() method returns
        //   the address of a modifiable element, while the const data() method
        //   return the address of a non-modifiable element.
        //
        // Testing:
        //   T *data();
        //   const T *data() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Internal Data Access Methods" << endl
                          << "====================================" << endl;

        if (verbose) cout <<
            "\nTesting const and non-const 'data()' methods." << endl;

        if (verbose) cout <<
              "\nTesting without allocator, copyable.\n";

        TestCases<TestNoAllocCopy>::testcase18(testStatus, argc,
               argv, verbose, veryVerbose, veryVeryVerbose, testAllocator);

        if (verbose) cout <<
              "\nTesting without allocator, moveable.\n";

        TestCases<TestNoAllocMove>::testcase18(testStatus, argc,
               argv, verbose, veryVerbose, veryVeryVerbose, testAllocator);

        if (verbose) cout <<
              "\nTesting without allocator, not moveable.\n";

        TestCases<TestNoAllocNoMove>::testcase18(testStatus, argc,
               argv, verbose, veryVerbose, veryVeryVerbose, testAllocator);

        if (verbose) cout <<
              "\nTesting with allocator, copyable.\n";

        TestCases<TestAllocCopy>::testcase18(testStatus, argc,
               argv, verbose, veryVerbose, veryVeryVerbose, testAllocator);

        if (verbose) cout <<
              "\nTesting with allocator, moveable.\n";

        TestCases<TestAllocMove>::testcase18(testStatus, argc,
               argv, verbose, veryVerbose, veryVeryVerbose, testAllocator);

        if (verbose) cout <<
              "\nTesting with allocator, not moveable.\n";

        TestCases<TestAllocNoMove>::testcase18(testStatus, argc,
               argv, verbose, veryVerbose, veryVeryVerbose, testAllocator);

      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Usage Example" << endl
                          << "=====================" << endl;

        if (verbose) cout << "\nUsage Example 1" << endl;
        {
          // Template types must be defined at the global level.
          // struct SampleStruct {
          //    int x;
          //};

          const SampleStruct E1 = { 1 };
          const SampleStruct E2 = { 2 };

          bdea_Array<SampleStruct> a(&testAllocator); ASSERT( 0 == a.length());


          a.append(E1);                   ASSERT( 1 == a.length());
                                          ASSERT(E1.x == a[0].x);

          a.append(E2);                   ASSERT( 2 == a.length());
                                          ASSERT(E1.x == a[0].x);
                                          ASSERT(E2.x == a[1].x);

          const SampleStruct E3 = { 3 };

          a[0] = E3;                      ASSERT( 2 == a.length());
                                          ASSERT(E3.x == a[0].x);
                                          ASSERT(E2.x == a[1].x);

          const SampleStruct E4 = { 4 };

          a.insert(1, E4);                ASSERT( 3 == a.length());
                                          ASSERT(E3.x == a[0].x);
                                          ASSERT(E4.x == a[1].x);
                                          ASSERT(E2.x == a[2].x);

          char buf[100];  memset(buf, 0xff, sizeof buf);  // Scribble on buf.
          ostrstream out(buf, sizeof buf - epcBug);

          out << '[';
          int len = a.length();
          for (int i = 0; i < len; ++i) {
              out << ' ' << a[i].x;
          }
          out << " ]" << endl;
          out << ends;  // Terminate the in-core string.

          const char *const EXPECTED = "[ 3 4 2 ]\n";

          if (veryVerbose) {
              cout << "EXPECTED: " << EXPECTED;
              cout << "  ACTUAL: " << buf;
          }
          ASSERT(0 == strcmp(EXPECTED, buf));

          a.remove(2);                    ASSERT( 2 == a.length());
                                          ASSERT(E3.x == a[0].x);
                                          ASSERT(E4.x == a[1].x);

          a.remove(0);                    ASSERT( 1 == a.length());
                                          ASSERT(E4.x == a[0].x);

          a.remove(0);                    ASSERT( 0 == a.length());

        }

        if (verbose) cout << "\nUsage Example 2" << endl;
        {
          // SampleClass is defined at global level.

          const SampleClass E1 =  1;
          const SampleClass E2 =  2;

          bdea_Array<SampleClass> a(&testAllocator);  ASSERT( 0 == a.length());

          a.append(E1);                   ASSERT( 1 == a.length());
                                          ASSERT(E1 == a[0]);

          a.append(E2);                   ASSERT( 2 == a.length());
                                          ASSERT(E1 == a[0]);
                                          ASSERT(E2 == a[1]);

          const SampleClass E3 =  3;

          a[0] = E3;                      ASSERT( 2 == a.length());
                                          ASSERT(E3 == a[0]);
                                          ASSERT(E2 == a[1]);

          const SampleClass E4 =  4;

          a.insert(1, E4);                ASSERT( 3 == a.length());
                                          ASSERT(E3 == a[0]);
                                          ASSERT(E4 == a[1]);
                                          ASSERT(E2 == a[2]);

          char buf[100];  memset(buf, 0xff, sizeof buf);  // Scribble on buf.
//        ostrstream out(buf, sizeof buf);   *** MISMATCH caused by EPC bug ***
          ostrstream out(buf, sizeof buf - epcBug);      // *** EPC bug fix ***
          out << '[';
          int len = a.length();
          for (int i = 0; i < len; ++i) {
              out << ' ' << a[i];
          }
          out << " ]" << endl;
          out << ends;  // Terminate the in-core string.

          const char *const EXPECTED = "[ 3 4 2 ]\n";

          if (veryVerbose) {
              cout << "EXPECTED: " << EXPECTED;
              cout << "  ACTUAL: " << buf;
          }
          ASSERT(0 == strcmp(EXPECTED, buf));

          a.remove(2);                    ASSERT( 2 == a.length());
                                          ASSERT(E3 == a[0]);
                                          ASSERT(E4 == a[1]);

          a.remove(0);                    ASSERT( 1 == a.length());
                                          ASSERT(E4 == a[0]);

          a.remove(0);                    ASSERT( 0 == a.length());
        }
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING CAPACITY-RESERVING CONSTRUCTOR AND METHOD
        //   The concerns are as follows:
        //    1. capacity-reserving constructor:
        //       a. The initial value is correct (empty).
        //       b. The initial capacity is correct.
        //       c. The constructor is exception neutral w.r.t. allocation.
        //       d. The internal memory management system is hooked up properly
        //          so that *all* internally allocated memory draws from a
        //          user-supplied allocator whenever one is specified.
        //    2. 'reserveCapacityRaw' method:
        //       a. The resulting value is correct (unchanged).
        //       b. The resulting capacity is correct (not less than initial).
        //       c. The method is exception neutral w.r.t. allocation.
        //    3. 'reserveCapacity' method:
        //       a. The resulting value is correct (unchanged).
        //       b. The resulting capacity is correct (not less than initial).
        //       c. The method is exception neutral w.r.t. allocation.
        //       d. The resulting value is unchanged in the event of
        //          exceptions.
        //       e. The amount of memory allocated is current * 2^N
        //
        // Plan:
        //   In a loop, use the capacity-reserving constructor to create empty
        //   objects with increasing initial capacity.  Verify that each object
        //   has the same value as a control default object.  Then, append as
        //   many values as the requested initial capacity, and use
        //   'bdema_TestAllocator' to verify that no additional allocations
        //   have occurred.  Perform each test in the standard 'bdema'
        //   exception-testing macro block.
        //
        //   Repeat the constructor test initially specifying no allocator and
        //   again, specifying a static buffer allocator.  These tests (without
        //   specifying a 'bdema_TestAllocator') cannot confirm correct
        //   capacity-reserving behavior, but can test for rudimentary correct
        //   object behavior via the destructor and Purify, and, in
        //   'veryVerbose' mode, via the print statements.
        //
        //   To test 'reserveCapacity', specify a table of initial object
        //   values and subsequent capacities to reserve.  Construct each
        //   tabulated value, call 'reserveCapacity' with the tabulated number
        //   of elements, and confirm that the test object has the same value
        //   as a separately constructed control object.  Then, append as many
        //   values as required to bring the test object's length to the
        //   specified number of elements, and use 'bdema_TestAllocator' to
        //   verify that no additional allocations have occurred.  Perform each
        //   test in the standard 'bdema' exception-testing macro block.
        //
        // Testing:
        //   bdea_DoubleArray(const InitialCapacity& ne, *ba = 0);
        //   void reserveCapacityRaw(int ne);
        //   void reserveCapacity(int ne);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "Testing Capacity Reserving Constructor and Methods" << endl
            << "==================================================" << endl;

        if (verbose) cout <<
          "\nTesting with allocator, Copyable.\n";

        TestCases<TestAllocCopy>::testcase16(testStatus, argc,
               argv, verbose, veryVerbose, veryVeryVerbose, 0, testAllocator);

        if (verbose) cout <<
          "\nTesting with allocator, Not Moveable.\n";

        TestCases<TestAllocNoMove>::testcase16(testStatus, argc,
               argv, verbose, veryVerbose, veryVeryVerbose, 1, testAllocator);

        if (verbose) cout << "\nTesting the 'reserveCapacity' method" << endl;
        {
            static const struct ReserveCapacityTable DATA[] = {
                //line  initialValue   numElements  Allocated
                //----  ------------   -----------  --------
                { L_,   "",              0,            0       },
                { L_,   "",              1,            0       },
                { L_,   "",              2,            2       },
                { L_,   "",              3,            4       },
                { L_,   "",              4,            4       },
                { L_,   "",              5,            8       },
                { L_,   "",              15,           16      },
                { L_,   "",              16,           16      },
                { L_,   "",              17,           32      },

                { L_,   "A",             0,            0       },
                { L_,   "A",             1,            0       },
                { L_,   "A",             2,            2       },
                { L_,   "A",             3,            4       },
                { L_,   "A",             4,            4       },
                { L_,   "A",             5,            8       },
                { L_,   "A",             15,           16      },
                { L_,   "A",             16,           16      },
                { L_,   "A",             17,           32      },

                { L_,   "AB",            0,            0       },
                { L_,   "AB",            1,            0       },
                { L_,   "AB",            2,            0       },
                { L_,   "AB",            3,            4       },
                { L_,   "AB",            4,            4       },
                { L_,   "AB",            5,            8       },
                { L_,   "AB",            15,           16      },
                { L_,   "AB",            16,           16      },
                { L_,   "AB",            17,           32      },

                { L_,   "ABCDE",         0,            0       },
                { L_,   "ABCDE",         1,            0       },
                { L_,   "ABCDE",         2,            0       },
                { L_,   "ABCDE",         3,            0       },
                { L_,   "ABCDE",         4,            0       },
                { L_,   "ABCDE",         5,            0       },
                { L_,   "ABCDE",         15,           16      },
                { L_,   "ABCDE",         16,           16      },
                { L_,   "ABCDE",         17,           32      },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            if (verbose) cout <<
              "\nTesting with allocator, copyable.\n";

            TestCases<TestAllocCopy>::testcase16ReserveCapacity(testStatus,
                                                                argc,
               argv, verbose, veryVerbose, veryVeryVerbose,
               DATA, NUM_DATA, 0, testAllocator);

            if (verbose) cout <<
              "\nTesting with allocator, not moveable.\n";

            TestCases<TestAllocNoMove>::testcase16ReserveCapacity(testStatus,
               argc, argv, verbose, veryVerbose, veryVeryVerbose,
               DATA, NUM_DATA, 1, testAllocator);
         }


      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING SWAP METHOD:
        //   We are concerned that, for an object of any length, 'swap' must
        //   exchange the values at any valid pair of index positions while
        //   leaving all other elements unaffected.  Note that, upon inspecting
        //   the implementation, we are explicitly not concerned about specific
        //   element values (i.e., a single pair of test values is sufficient,
        //   and cannot mask a "stuck at" error).
        // Concerns:
        //   1. Exceptions can be thrown in the swap method.
        //   2. The internal functionality varies according to which
        //      bitwise copy/move trait is applied.
        //
        // Plan:
        //   For each object X in a set of objects ordered by increasing length
        //   L containing uniform values, VALUES[0].
        //     For each valid index position P1 in [0 .. L-1]:
        //       For each valid index position P2 in [0 .. L-1]:
        //         1.  Create a test object T from X using the copy ctor.
        //         2.  Replace the element at P1 with VALUES[1] and at P2 with
        //             VALUES[2].
        //         3.  Swap these elements in T.
        //         4.  Verify that
        //               (i)     VALUES[2] == T[P1]     always
        //         5.  Verify that
        //               (ii)    VALUES[1] == T[P2]     if (P1 != P2)
        //                       VALUES[2] == T[P2]     if (P1 == P2)
        //         6.  For each index position, i, in [0 .. L-1] verify that:
        //               (iii)   VALUES[0] == T[i]      if (P1 != i && P2 != i)
        //
        //   To address concern 2, all these tests are performed on user
        //   defined types:
        //          With allocator, moveable
        //          With allocator, not moveable
        //
        // Testing:
        //   void swap(int index1, int index2);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'swap' Method" << endl
                          << "=====================" << endl;


        if (verbose) cout <<
          "\nTesting with allocator, not moveable.\n";

        TestCases<TestAllocNoMove>::testcase15(testStatus, argc, argv,
              verbose, veryVerbose, veryVeryVerbose, testAllocator);

        if (verbose) cout <<
          "\nTesting with allocator, moveable.\n";

        TestCases<TestAllocMove>::testcase15(testStatus, argc, argv,
              verbose, veryVerbose, veryVeryVerbose, testAllocator);

      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING REPLACE AND 'operator[]'
        //   Concerns:
        //     For the 'replace' method, the following properties must hold:
        //       1. The source is left unaffected (apart from aliasing).
        //       2. The subsequent existence of the source has no effect on the
        //          result object (apart from aliasing).
        //       3. The function is alias safe.
        //       4. The function preserves object invariants.
        //       5. The function is exception safe.
        //       6. The internal functionality varies according to which
        //          bitwise copy/move trait is applied.
        //
        //     Although the replace methods do not change the length of the
        //     array, it calls the 'bdet_String' assignment operator that may
        //     allocate memory and therefore generate exceptions.
        //
        //     For 'operator[]', the method must be able to modify its indexed
        //     element when used as an lvalue, but must not modify its indexed
        //     element when used as an rvalue.
        //
        // Plan:
        //   Use the enumeration technique to a depth of 5 for both the normal
        //   and alias cases.  Data is tabulated explicitly for the 'replace'
        //   method that takes a range from a source array (or itself, for the
        //   aliasing test); the "scalar" 'replace' test selects a subset of
        //   the table by testing 'if (1 == NE)' where 'NE' is the tabulated
        //   number of elements to replace.  'operator[]' is also tested using
        //   the scalar 'replace' data, but using explicit assignment to
        //   achieve the "expected" result.
        //     - In the "canonical state" (black-box) tests, we confirm that
        //       the source is unmodified by the method call, and that its
        //       subsequent destruction has no effect on the destination
        //       object.
        //     - In all cases we want to make sure that after the application
        //       of the operation, the object is allowed to go out of scope
        //       directly to enable the destructor to assert object invariants.
        //     - Each object constructed should be wrapped in separate
        //       BDEMA test assert macros and use gg as an optimization.
        //
        //   To address concern 6, all these tests are performed on user
        //   defined types:
        //          With allocator, copyable
        //          With allocator, not moveable
        //
        // Testing:
        //   void replace(int di, bdet_String& item);
        //   void replace(int di, const bdea_Array<T>& sa, int si, int ne);
        //   int& operator[](int index);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                << "Testing 'replace' and 'operator[]'" << endl
                << "==================================" << endl;

        if (verbose) cout <<
            "\nTesting replace(di, sa, si, ne) et. al. (no aliasing)" << endl;
        {
            static const struct ReplaceTable DATA[] = {
                //line  d-array di   s-array si  ne  expected
                //----  ------- --   ------- --  --  --------   Depth = 0
                { L_,   "",      0,  "",      0,  0, ""      },

                //line  d-array di   s-array si  ne  expected
                //----  ------- --   ------- --  --  --------   Depth = 1
                { L_,   "A",     0,  "",      0,  0, "A"     },
                { L_,   "A",     1,  "",      0,  0, "A"     },

                { L_,   "",      0,  "B",     0,  0, ""      },
                { L_,   "",      0,  "B",     1,  0, ""      },

                //line  d-array di   s-array si  ne  expected
                //----  ------- --   ------- --  --  --------   Depth = 2
                { L_,   "AB",    0,  "",      0,  0, "AB"    },
                { L_,   "AB",    1,  "",      0,  0, "AB"    },
                { L_,   "AB",    2,  "",      0,  0, "AB"    },

                { L_,   "A",     0,  "B",     0,  0, "A"     },
                { L_,   "A",     0,  "B",     0,  1, "B"     },
                { L_,   "A",     0,  "B",     1,  0, "A"     },
                { L_,   "A",     1,  "B",     0,  0, "A"     },
                { L_,   "A",     1,  "B",     1,  0, "A"     },

                { L_,   "",      0,  "AB",    0,  0, ""      },
                { L_,   "",      0,  "AB",    1,  0, ""      },
                { L_,   "",      0,  "AB",    2,  0, ""      },

                //line  d-array di   s-array si  ne  expected
                //----  ------- --   ------- --  --  --------   Depth = 3
                { L_,   "ABC",   0,  "",      0,  0, "ABC"   },
                { L_,   "ABC",   1,  "",      0,  0, "ABC"   },
                { L_,   "ABC",   2,  "",      0,  0, "ABC"   },
                { L_,   "ABC",   3,  "",      0,  0, "ABC"   },

                { L_,   "AB",    0,  "C",     0,  0, "AB"    },
                { L_,   "AB",    0,  "C",     0,  1, "CB"    },
                { L_,   "AB",    0,  "C",     1,  0, "AB"    },
                { L_,   "AB",    1,  "C",     0,  0, "AB"    },
                { L_,   "AB",    1,  "C",     0,  1, "AC"    },
                { L_,   "AB",    1,  "C",     1,  0, "AB"    },
                { L_,   "AB",    2,  "C",     0,  0, "AB"    },
                { L_,   "AB",    2,  "C",     1,  0, "AB"    },

                { L_,   "A",     0,  "BC",    0,  0, "A"     },
                { L_,   "A",     0,  "BC",    0,  1, "B"     },
                { L_,   "A",     0,  "BC",    1,  0, "A"     },
                { L_,   "A",     0,  "BC",    1,  1, "C"     },
                { L_,   "A",     0,  "BC",    2,  0, "A"     },
                { L_,   "A",     1,  "BC",    0,  0, "A"     },
                { L_,   "A",     1,  "BC",    1,  0, "A"     },
                { L_,   "A",     1,  "BC",    2,  0, "A"     },

                { L_,   "",      0,  "ABC",   0,  0, ""      },
                { L_,   "",      0,  "ABC",   1,  0, ""      },
                { L_,   "",      0,  "ABC",   2,  0, ""      },
                { L_,   "",      0,  "ABC",   3,  0, ""      },

                //line  d-array di   s-array si  ne  expected
                //----  ------- --   ------- --  --  --------   Depth = 4
                { L_,   "ABCD",  0,  "",      0,  0, "ABCD"  },
                { L_,   "ABCD",  1,  "",      0,  0, "ABCD"  },
                { L_,   "ABCD",  2,  "",      0,  0, "ABCD"  },
                { L_,   "ABCD",  3,  "",      0,  0, "ABCD"  },
                { L_,   "ABCD",  4,  "",      0,  0, "ABCD"  },

                { L_,   "ABC",   0,  "D",     0,  0, "ABC"   },
                { L_,   "ABC",   0,  "D",     0,  1, "DBC"   },
                { L_,   "ABC",   0,  "D",     1,  0, "ABC"   },
                { L_,   "ABC",   1,  "D",     0,  0, "ABC"   },
                { L_,   "ABC",   1,  "D",     0,  1, "ADC"   },
                { L_,   "ABC",   1,  "D",     1,  0, "ABC"   },
                { L_,   "ABC",   2,  "D",     0,  0, "ABC"   },
                { L_,   "ABC",   2,  "D",     0,  1, "ABD"   },
                { L_,   "ABC",   2,  "D",     1,  0, "ABC"   },
                { L_,   "ABC",   3,  "D",     0,  0, "ABC"   },
                { L_,   "ABC",   3,  "D",     1,  0, "ABC"   },

                { L_,   "AB",    0,  "CD",    0,  0, "AB"    },
                { L_,   "AB",    0,  "CD",    0,  1, "CB"    },
                { L_,   "AB",    0,  "CD",    0,  2, "CD"    },
                { L_,   "AB",    0,  "CD",    1,  0, "AB"    },
                { L_,   "AB",    0,  "CD",    1,  1, "DB"    },
                { L_,   "AB",    0,  "CD",    2,  0, "AB"    },
                { L_,   "AB",    1,  "CD",    0,  0, "AB"    },
                { L_,   "AB",    1,  "CD",    0,  1, "AC"    },
                { L_,   "AB",    1,  "CD",    1,  0, "AB"    },
                { L_,   "AB",    1,  "CD",    1,  1, "AD"    },
                { L_,   "AB",    1,  "CD",    2,  0, "AB"    },
                { L_,   "AB",    2,  "CD",    0,  0, "AB"    },
                { L_,   "AB",    2,  "CD",    1,  0, "AB"    },
                { L_,   "AB",    2,  "CD",    2,  0, "AB"    },

                { L_,   "A",     0,  "BCD",   0,  0, "A"     },
                { L_,   "A",     0,  "BCD",   0,  1, "B"     },
                { L_,   "A",     0,  "BCD",   1,  0, "A"     },
                { L_,   "A",     0,  "BCD",   1,  1, "C"     },
                { L_,   "A",     0,  "BCD",   2,  0, "A"     },
                { L_,   "A",     0,  "BCD",   2,  1, "D"     },
                { L_,   "A",     0,  "BCD",   3,  0, "A"     },
                { L_,   "A",     1,  "BCD",   0,  0, "A"     },
                { L_,   "A",     1,  "BCD",   1,  0, "A"     },
                { L_,   "A",     1,  "BCD",   2,  0, "A"     },
                { L_,   "A",     1,  "BCD",   3,  0, "A"     },

                { L_,   "",      0,  "ABCD",  0,  0, ""      },
                { L_,   "",      0,  "ABCD",  1,  0, ""      },
                { L_,   "",      0,  "ABCD",  2,  0, ""      },
                { L_,   "",      0,  "ABCD",  3,  0, ""      },
                { L_,   "",      0,  "ABCD",  4,  0, ""      },

                //line  d-array di   s-array si  ne  expected
                //----  ------- --   ------- --  --  --------   Depth = 5
                { L_,   "ABCDE", 0,  "",      0,  0, "ABCDE" },
                { L_,   "ABCDE", 1,  "",      0,  0, "ABCDE" },
                { L_,   "ABCDE", 2,  "",      0,  0, "ABCDE" },
                { L_,   "ABCDE", 3,  "",      0,  0, "ABCDE" },
                { L_,   "ABCDE", 4,  "",      0,  0, "ABCDE" },
                { L_,   "ABCDE", 5,  "",      0,  0, "ABCDE" },

                { L_,   "ABCD",  0,  "E",     0,  0, "ABCD" },
                { L_,   "ABCD",  0,  "E",     0,  1, "EBCD" },
                { L_,   "ABCD",  0,  "E",     1,  0, "ABCD" },
                { L_,   "ABCD",  1,  "E",     0,  0, "ABCD" },
                { L_,   "ABCD",  1,  "E",     0,  1, "AECD" },
                { L_,   "ABCD",  1,  "E",     1,  0, "ABCD" },
                { L_,   "ABCD",  2,  "E",     0,  0, "ABCD" },
                { L_,   "ABCD",  2,  "E",     0,  1, "ABED" },
                { L_,   "ABCD",  2,  "E",     1,  0, "ABCD" },
                { L_,   "ABCD",  3,  "E",     0,  0, "ABCD" },
                { L_,   "ABCD",  3,  "E",     0,  1, "ABCE" },
                { L_,   "ABCD",  3,  "E",     1,  0, "ABCD" },
                { L_,   "ABCD",  4,  "E",     0,  0, "ABCD" },
                { L_,   "ABCD",  4,  "E",     1,  0, "ABCD" },

                { L_,   "ABC",   0,  "DE",    0,  0, "ABC"   },
                { L_,   "ABC",   0,  "DE",    0,  1, "DBC"   },
                { L_,   "ABC",   0,  "DE",    0,  2, "DEC"   },
                { L_,   "ABC",   0,  "DE",    1,  0, "ABC"   },
                { L_,   "ABC",   0,  "DE",    1,  1, "EBC"   },
                { L_,   "ABC",   0,  "DE",    2,  0, "ABC"   },
                { L_,   "ABC",   1,  "DE",    0,  0, "ABC"   },
                { L_,   "ABC",   1,  "DE",    0,  1, "ADC"   },
                { L_,   "ABC",   1,  "DE",    0,  2, "ADE"   },
                { L_,   "ABC",   1,  "DE",    1,  0, "ABC"   },
                { L_,   "ABC",   1,  "DE",    1,  1, "AEC"   },
                { L_,   "ABC",   1,  "DE",    2,  0, "ABC"   },
                { L_,   "ABC",   2,  "DE",    0,  0, "ABC"   },
                { L_,   "ABC",   2,  "DE",    0,  1, "ABD"   },
                { L_,   "ABC",   2,  "DE",    1,  0, "ABC"   },
                { L_,   "ABC",   2,  "DE",    1,  1, "ABE"   },
                { L_,   "ABC",   2,  "DE",    2,  0, "ABC"   },
                { L_,   "ABC",   3,  "DE",    0,  0, "ABC"   },
                { L_,   "ABC",   3,  "DE",    1,  0, "ABC"   },
                { L_,   "ABC",   3,  "DE",    2,  0, "ABC"   },

                { L_,   "AB",    0,  "CDE",   0,  0, "AB"    },
                { L_,   "AB",    0,  "CDE",   0,  1, "CB"    },
                { L_,   "AB",    0,  "CDE",   0,  2, "CD"    },
                { L_,   "AB",    0,  "CDE",   1,  0, "AB"    },
                { L_,   "AB",    0,  "CDE",   1,  1, "DB"    },
                { L_,   "AB",    0,  "CDE",   2,  0, "AB"    },
                { L_,   "AB",    0,  "CDE",   2,  1, "EB"    },
                { L_,   "AB",    0,  "CDE",   3,  0, "AB"    },
                { L_,   "AB",    1,  "CDE",   0,  0, "AB"    },
                { L_,   "AB",    1,  "CDE",   0,  1, "AC"    },
                { L_,   "AB",    1,  "CDE",   1,  0, "AB"    },
                { L_,   "AB",    1,  "CDE",   1,  1, "AD"    },
                { L_,   "AB",    1,  "CDE",   2,  0, "AB"    },
                { L_,   "AB",    1,  "CDE",   2,  1, "AE"    },
                { L_,   "AB",    1,  "CDE",   3,  0, "AB"    },
                { L_,   "AB",    2,  "CDE",   0,  0, "AB"    },
                { L_,   "AB",    2,  "CDE",   1,  0, "AB"    },
                { L_,   "AB",    2,  "CDE",   2,  0, "AB"    },
                { L_,   "AB",    2,  "CDE",   3,  0, "AB"    },

                { L_,   "A",     0,  "BCDE",  0,  0, "A"     },
                { L_,   "A",     0,  "BCDE",  0,  1, "B"     },
                { L_,   "A",     0,  "BCDE",  1,  0, "A"     },
                { L_,   "A",     0,  "BCDE",  1,  1, "C"     },
                { L_,   "A",     0,  "BCDE",  2,  0, "A"     },
                { L_,   "A",     0,  "BCDE",  2,  1, "D"     },
                { L_,   "A",     0,  "BCDE",  3,  0, "A"     },
                { L_,   "A",     0,  "BCDE",  3,  1, "E"     },
                { L_,   "A",     0,  "BCDE",  4,  0, "A"     },
                { L_,   "A",     1,  "BCDE",  0,  0, "A"     },
                { L_,   "A",     1,  "BCDE",  1,  0, "A"     },
                { L_,   "A",     1,  "BCDE",  2,  0, "A"     },
                { L_,   "A",     1,  "BCDE",  3,  0, "A"     },
                { L_,   "A",     1,  "BCDE",  4,  0, "A"     },

                { L_,   "",      0,  "ABCDE", 0,  0, ""      },
                { L_,   "",      0,  "ABCDE", 1,  0, ""      },
                { L_,   "",      0,  "ABCDE", 2,  0, ""      },
                { L_,   "",      0,  "ABCDE", 3,  0, ""      },
                { L_,   "",      0,  "ABCDE", 4,  0, ""      },
                { L_,   "",      0,  "ABCDE", 5,  0, ""      },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            if (verbose) cout <<
              "\nTesting with allocator, copyable.\n";

            TestCases<TestAllocCopy>::testcase14(testStatus, argc,
               argv, verbose, veryVerbose, veryVeryVerbose,
               DATA, NUM_DATA, 0, testAllocator);

            if (verbose) cout <<
              "\nTesting with allocator, not moveable.\n";

            TestCases<TestAllocNoMove>::testcase14(testStatus, argc,
               argv, verbose, veryVerbose, veryVeryVerbose,
               DATA, NUM_DATA, 1, testAllocator);
        }

        //---------------------------------------------------------------------

        if (verbose) cout <<
            "\nTesting x.replace(di, sa, si, ne) et. al. (aliasing)" << endl;
        {
            static const struct ReplaceAliasTable DATA[] = {
                //line  x-array di   si  ne  expected
                //----  ------- --   --  --  --------   Depth = 0
                { L_,   "",      0,  0,  0,  ""      },

                //line  x-array di   si  ne  expected
                //----  ------- --   --  --  --------   Depth = 1
                { L_,   "A",     0,  0,  0,  "A"     },
                { L_,   "A",     0,  0,  1,  "A"     },
                { L_,   "A",     0,  1,  0,  "A"     },

                { L_,   "A",     1,  0,  0,  "A"     },
                { L_,   "A",     1,  1,  0,  "A"     },

                //line  x-array di   si  ne  expected
                //----  ------- --   --  --  --------   Depth = 2
                { L_,   "AB",    0,  0,  0,  "AB"    },
                { L_,   "AB",    0,  0,  1,  "AB"    },
                { L_,   "AB",    0,  0,  2,  "AB"    },
                { L_,   "AB",    0,  1,  0,  "AB"    },
                { L_,   "AB",    0,  1,  1,  "BB"    },
                { L_,   "AB",    0,  2,  0,  "AB"    },

                { L_,   "AB",    1,  0,  0,  "AB"    },
                { L_,   "AB",    1,  0,  1,  "AA"    },
                { L_,   "AB",    1,  1,  0,  "AB"    },
                { L_,   "AB",    1,  1,  1,  "AB"    },
                { L_,   "AB",    1,  2,  0,  "AB"    },

                { L_,   "AB",    2,  0,  0,  "AB"    },
                { L_,   "AB",    2,  1,  0,  "AB"    },
                { L_,   "AB",    2,  2,  0,  "AB"    },

                //line  x-array di   si  ne  expected
                //----  ------- --   --  --  --------   Depth = 3
                { L_,   "ABC",   0,  0,  0,  "ABC"   },
                { L_,   "ABC",   0,  0,  1,  "ABC"   },
                { L_,   "ABC",   0,  0,  2,  "ABC"   },
                { L_,   "ABC",   0,  0,  3,  "ABC"   },
                { L_,   "ABC",   0,  1,  0,  "ABC"   },
                { L_,   "ABC",   0,  1,  1,  "BBC"   },
                { L_,   "ABC",   0,  1,  2,  "BCC"   },
                { L_,   "ABC",   0,  2,  0,  "ABC"   },
                { L_,   "ABC",   0,  2,  1,  "CBC"   },
                { L_,   "ABC",   0,  3,  0,  "ABC"   },

                { L_,   "ABC",   1,  0,  0,  "ABC"   },
                { L_,   "ABC",   1,  0,  1,  "AAC"   },
                { L_,   "ABC",   1,  0,  2,  "AAB"   },
                { L_,   "ABC",   1,  1,  0,  "ABC"   },
                { L_,   "ABC",   1,  1,  1,  "ABC"   },
                { L_,   "ABC",   1,  1,  2,  "ABC"   },
                { L_,   "ABC",   1,  2,  0,  "ABC"   },
                { L_,   "ABC",   1,  2,  1,  "ACC"   },
                { L_,   "ABC",   1,  3,  0,  "ABC"   },

                { L_,   "ABC",   2,  0,  0,  "ABC"   },
                { L_,   "ABC",   2,  0,  1,  "ABA"   },
                { L_,   "ABC",   2,  1,  0,  "ABC"   },
                { L_,   "ABC",   2,  1,  1,  "ABB"   },
                { L_,   "ABC",   2,  2,  0,  "ABC"   },
                { L_,   "ABC",   2,  2,  1,  "ABC"   },
                { L_,   "ABC",   2,  3,  0,  "ABC"   },

                { L_,   "ABC",   3,  0,  0,  "ABC"   },
                { L_,   "ABC",   3,  1,  0,  "ABC"   },
                { L_,   "ABC",   3,  2,  0,  "ABC"   },
                { L_,   "ABC",   3,  3,  0,  "ABC"   },

                //line  x-array di   si  ne  expected
                //----  ------- --   --  --  --------   Depth = 4
                { L_,   "ABCD",  0,  0,  0,  "ABCD"  },
                { L_,   "ABCD",  0,  0,  1,  "ABCD"  },
                { L_,   "ABCD",  0,  0,  2,  "ABCD"  },
                { L_,   "ABCD",  0,  0,  3,  "ABCD"  },
                { L_,   "ABCD",  0,  0,  4,  "ABCD"  },
                { L_,   "ABCD",  0,  1,  0,  "ABCD"  },
                { L_,   "ABCD",  0,  1,  1,  "BBCD"  },
                { L_,   "ABCD",  0,  1,  2,  "BCCD"  },
                { L_,   "ABCD",  0,  1,  3,  "BCDD"  },
                { L_,   "ABCD",  0,  2,  0,  "ABCD"  },
                { L_,   "ABCD",  0,  2,  1,  "CBCD"  },
                { L_,   "ABCD",  0,  2,  2,  "CDCD"  },
                { L_,   "ABCD",  0,  3,  0,  "ABCD"  },
                { L_,   "ABCD",  0,  3,  1,  "DBCD"  },
                { L_,   "ABCD",  0,  4,  0,  "ABCD"  },

                { L_,   "ABCD",  1,  0,  0,  "ABCD"  },
                { L_,   "ABCD",  1,  0,  1,  "AACD"  },
                { L_,   "ABCD",  1,  0,  2,  "AABD"  },
                { L_,   "ABCD",  1,  0,  3,  "AABC"  },
                { L_,   "ABCD",  1,  1,  0,  "ABCD"  },
                { L_,   "ABCD",  1,  1,  1,  "ABCD"  },
                { L_,   "ABCD",  1,  1,  2,  "ABCD"  },
                { L_,   "ABCD",  1,  1,  3,  "ABCD"  },
                { L_,   "ABCD",  1,  2,  0,  "ABCD"  },
                { L_,   "ABCD",  1,  2,  1,  "ACCD"  },
                { L_,   "ABCD",  1,  2,  2,  "ACDD"  },
                { L_,   "ABCD",  1,  3,  0,  "ABCD"  },
                { L_,   "ABCD",  1,  3,  1,  "ADCD"  },
                { L_,   "ABCD",  1,  4,  0,  "ABCD"  },

                { L_,   "ABCD",  2,  0,  0,  "ABCD"  },
                { L_,   "ABCD",  2,  0,  1,  "ABAD"  },
                { L_,   "ABCD",  2,  0,  2,  "ABAB"  },
                { L_,   "ABCD",  2,  1,  0,  "ABCD"  },
                { L_,   "ABCD",  2,  1,  1,  "ABBD"  },
                { L_,   "ABCD",  2,  1,  2,  "ABBC"  },
                { L_,   "ABCD",  2,  2,  0,  "ABCD"  },
                { L_,   "ABCD",  2,  2,  1,  "ABCD"  },
                { L_,   "ABCD",  2,  2,  2,  "ABCD"  },
                { L_,   "ABCD",  2,  3,  0,  "ABCD"  },
                { L_,   "ABCD",  2,  3,  1,  "ABDD"  },
                { L_,   "ABCD",  2,  4,  0,  "ABCD"  },

                { L_,   "ABCD",  3,  0,  0,  "ABCD"  },
                { L_,   "ABCD",  3,  0,  1,  "ABCA"  },
                { L_,   "ABCD",  3,  1,  0,  "ABCD"  },
                { L_,   "ABCD",  3,  1,  1,  "ABCB"  },
                { L_,   "ABCD",  3,  2,  0,  "ABCD"  },
                { L_,   "ABCD",  3,  2,  1,  "ABCC"  },
                { L_,   "ABCD",  3,  3,  0,  "ABCD"  },
                { L_,   "ABCD",  3,  3,  1,  "ABCD"  },
                { L_,   "ABCD",  3,  4,  0,  "ABCD"  },

                { L_,   "ABCD",  4,  0,  0,  "ABCD"  },
                { L_,   "ABCD",  4,  1,  0,  "ABCD"  },
                { L_,   "ABCD",  4,  2,  0,  "ABCD"  },
                { L_,   "ABCD",  4,  3,  0,  "ABCD"  },
                { L_,   "ABCD",  4,  4,  0,  "ABCD"  },

                //line  x-array di   si  ne  expected
                //----  ------- --   --  --  --------   Depth = 5
                { L_,   "ABCDE", 0,  0,  0,  "ABCDE" },
                { L_,   "ABCDE", 0,  0,  1,  "ABCDE" },
                { L_,   "ABCDE", 0,  0,  2,  "ABCDE" },
                { L_,   "ABCDE", 0,  0,  3,  "ABCDE" },
                { L_,   "ABCDE", 0,  0,  4,  "ABCDE" },
                { L_,   "ABCDE", 0,  0,  5,  "ABCDE" },
                { L_,   "ABCDE", 0,  1,  0,  "ABCDE" },
                { L_,   "ABCDE", 0,  1,  1,  "BBCDE" },
                { L_,   "ABCDE", 0,  1,  2,  "BCCDE" },
                { L_,   "ABCDE", 0,  1,  3,  "BCDDE" },
                { L_,   "ABCDE", 0,  1,  4,  "BCDEE" },
                { L_,   "ABCDE", 0,  2,  0,  "ABCDE" },
                { L_,   "ABCDE", 0,  2,  1,  "CBCDE" },
                { L_,   "ABCDE", 0,  2,  2,  "CDCDE" },
                { L_,   "ABCDE", 0,  2,  3,  "CDEDE" },
                { L_,   "ABCDE", 0,  3,  0,  "ABCDE" },
                { L_,   "ABCDE", 0,  3,  1,  "DBCDE" },
                { L_,   "ABCDE", 0,  3,  2,  "DECDE" },
                { L_,   "ABCDE", 0,  4,  0,  "ABCDE" },
                { L_,   "ABCDE", 0,  4,  1,  "EBCDE" },
                { L_,   "ABCDE", 0,  5,  0,  "ABCDE" },

                { L_,   "ABCDE", 1,  0,  0,  "ABCDE" },
                { L_,   "ABCDE", 1,  0,  1,  "AACDE" },
                { L_,   "ABCDE", 1,  0,  2,  "AABDE" },
                { L_,   "ABCDE", 1,  0,  3,  "AABCE" },
                { L_,   "ABCDE", 1,  0,  4,  "AABCD" },
                { L_,   "ABCDE", 1,  1,  0,  "ABCDE" },
                { L_,   "ABCDE", 1,  1,  1,  "ABCDE" },
                { L_,   "ABCDE", 1,  1,  2,  "ABCDE" },
                { L_,   "ABCDE", 1,  1,  3,  "ABCDE" },
                { L_,   "ABCDE", 1,  1,  4,  "ABCDE" },
                { L_,   "ABCDE", 1,  2,  0,  "ABCDE" },
                { L_,   "ABCDE", 1,  2,  1,  "ACCDE" },
                { L_,   "ABCDE", 1,  2,  2,  "ACDDE" },
                { L_,   "ABCDE", 1,  2,  3,  "ACDEE" },
                { L_,   "ABCDE", 1,  3,  0,  "ABCDE" },
                { L_,   "ABCDE", 1,  3,  1,  "ADCDE" },
                { L_,   "ABCDE", 1,  3,  2,  "ADEDE" },
                { L_,   "ABCDE", 1,  4,  0,  "ABCDE" },
                { L_,   "ABCDE", 1,  4,  1,  "AECDE" },
                { L_,   "ABCDE", 1,  5,  0,  "ABCDE" },

                { L_,   "ABCDE", 2,  0,  0,  "ABCDE" },
                { L_,   "ABCDE", 2,  0,  1,  "ABADE" },
                { L_,   "ABCDE", 2,  0,  2,  "ABABE" },
                { L_,   "ABCDE", 2,  0,  3,  "ABABC" },
                { L_,   "ABCDE", 2,  1,  0,  "ABCDE" },
                { L_,   "ABCDE", 2,  1,  1,  "ABBDE" },
                { L_,   "ABCDE", 2,  1,  2,  "ABBCE" },
                { L_,   "ABCDE", 2,  1,  3,  "ABBCD" },
                { L_,   "ABCDE", 2,  2,  0,  "ABCDE" },
                { L_,   "ABCDE", 2,  2,  1,  "ABCDE" },
                { L_,   "ABCDE", 2,  2,  2,  "ABCDE" },
                { L_,   "ABCDE", 2,  2,  3,  "ABCDE" },
                { L_,   "ABCDE", 2,  3,  0,  "ABCDE" },
                { L_,   "ABCDE", 2,  3,  1,  "ABDDE" },
                { L_,   "ABCDE", 2,  3,  2,  "ABDEE" },
                { L_,   "ABCDE", 2,  4,  0,  "ABCDE" },
                { L_,   "ABCDE", 2,  4,  1,  "ABEDE" },
                { L_,   "ABCDE", 2,  5,  0,  "ABCDE" },

                { L_,   "ABCDE", 3,  0,  0,  "ABCDE" },
                { L_,   "ABCDE", 3,  0,  1,  "ABCAE" },
                { L_,   "ABCDE", 3,  0,  2,  "ABCAB" },
                { L_,   "ABCDE", 3,  1,  0,  "ABCDE" },
                { L_,   "ABCDE", 3,  1,  1,  "ABCBE" },
                { L_,   "ABCDE", 3,  1,  2,  "ABCBC" },
                { L_,   "ABCDE", 3,  2,  0,  "ABCDE" },
                { L_,   "ABCDE", 3,  2,  1,  "ABCCE" },
                { L_,   "ABCDE", 3,  2,  2,  "ABCCD" },
                { L_,   "ABCDE", 3,  3,  0,  "ABCDE" },
                { L_,   "ABCDE", 3,  3,  1,  "ABCDE" },
                { L_,   "ABCDE", 3,  3,  2,  "ABCDE" },
                { L_,   "ABCDE", 3,  4,  0,  "ABCDE" },
                { L_,   "ABCDE", 3,  4,  1,  "ABCEE" },
                { L_,   "ABCDE", 3,  5,  0,  "ABCDE" },

                { L_,   "ABCDE", 4,  0,  0,  "ABCDE" },
                { L_,   "ABCDE", 4,  0,  1,  "ABCDA" },
                { L_,   "ABCDE", 4,  1,  0,  "ABCDE" },
                { L_,   "ABCDE", 4,  1,  1,  "ABCDB" },
                { L_,   "ABCDE", 4,  2,  0,  "ABCDE" },
                { L_,   "ABCDE", 4,  2,  1,  "ABCDC" },
                { L_,   "ABCDE", 4,  3,  0,  "ABCDE" },
                { L_,   "ABCDE", 4,  3,  1,  "ABCDD" },
                { L_,   "ABCDE", 4,  4,  0,  "ABCDE" },
                { L_,   "ABCDE", 4,  4,  1,  "ABCDE" },
                { L_,   "ABCDE", 4,  5,  0,  "ABCDE" },

                { L_,   "ABCDE", 5,  0,  0,  "ABCDE" },
                { L_,   "ABCDE", 5,  1,  0,  "ABCDE" },
                { L_,   "ABCDE", 5,  2,  0,  "ABCDE" },
                { L_,   "ABCDE", 5,  3,  0,  "ABCDE" },
                { L_,   "ABCDE", 5,  4,  0,  "ABCDE" },
                { L_,   "ABCDE", 5,  5,  0,  "ABCDE" },

            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            if (verbose) cout <<
              "\nTesting with allocator, copyable.\n";

            TestCases<TestAllocCopy>::testcase14Alias(testStatus, argc,
                  argv, verbose, veryVerbose, veryVeryVerbose,
                  DATA, NUM_DATA, testAllocator);

            if (verbose) cout <<
              "\nTesting with allocator, not moveable.\n";

            TestCases<TestAllocNoMove>::testcase14Alias(testStatus, argc,
                  argv, verbose, veryVerbose, veryVeryVerbose,
                  DATA, NUM_DATA, testAllocator);
         }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING APPEND, INSERT, REMOVE
        //   Concerns:
        //     For the 'append' and 'insert' methods, the following properties
        //     must hold:
        //       1. The source is left unaffected (apart from aliasing).
        //       2. The subsequent existing of the source has no effect on the
        //          result object (apart from aliasing).
        //       3. The function is alias safe.
        //       4. The function is exception neutral (w.r.t. allocation).
        //       5. The function preserves object invariants.
        //       6. The function is independent of internal representation.
        //       7. The internal functionality varies according to which
        //          bitwise copy/move trait is applied.
        //     Note that all (contingent) reallocations occur strictly before
        //     the essential implementation of each method.  Therefore,
        //     concerns 1, 2, and 4 above are valid for objects in the
        //     "canonical state", but need not be repeated when concern 6
        //     ("white-box test") is addressed.
        //
        //     For the 'remove' methods, the concerns are simply to cover the
        //     full range of possible indices and numbers of elements.
        //
        // Plan:
        //   Use the enumeration technique to a depth of 5 for both the normal
        //   and alias cases.  Data is tabulated explicitly for the 'insert'
        //   method that takes a range from a source array (or itself, for the
        //   aliasing test); other methods are tested using a subset of the
        //   full test vector table.  In particular, the 'append' methods use
        //   data where the destination index equals the destination length
        //   ((int) strlen(D_SPEC) == DI).  All methods using the entire source
        //   object use test data where the source length equals the number of
        //   elements ((int) strlen(S_SPEC) == NE), while the "scalar" methods
        //   use data where the number of elements equals 1 (1 == NE).  In
        //   addition, the 'remove' methods switch the "d-array" and "expected"
        //   values from the 'insert' table.
        //     - In the "canonical state" (black-box) tests, we confirm that
        //       the source is unmodified by the method call, and that its
        //       subsequent destruction has no effect on the destination
        //       object.
        //     - In all cases we want to make sure that after the application
        //       of the operation, the object is allowed to go out of scope
        //       directly to enable the destructor to assert object invariants.
        //     - Each object constructed should be wrapped in separate
        //       BDEMA test assert macros and use gg as an optimization.
        //
        //   To address concern 7, all these tests are performed on user
        //   defined types:
        //          With allocator, copyable
        //          With allocator, moveable
        //          With allocator, not moveable
        //
        // Testing:
        //   void append(bdet_String& item);
        //   void append(const bdea_Array<T>& sa);
        //   void append(const bdea_Array<T>& sa, int si, int ne);
        //
        //   void insert(int di, bdet_String& item);
        //   void insert(int di, const bdea_Array<T>& sa);
        //   void insert(int di, const bdea_Array<T>& sa, int si, int ne);
        //
        //   void remove(int index);
        //   void remove(int index, int ne);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                << "Testing 'append', 'insert', and 'remove'" << endl
                << "========================================" << endl;

        if (verbose) cout <<
            "\nTesting x.insert(di, sa, si, ne) et. al. (no aliasing)" << endl;
        {
            static const struct InsertTable DATA[] = {
                //line  d-array di   s-array si  ne  expected
                //----  ------- --   ------- --  --  --------   Depth = 0
                { L_,   "",      0,  "",      0,  0, ""      },

                //line  d-array di   s-array si  ne  expected
                //----  ------- --   ------- --  --  --------   Depth = 1
                { L_,   "A",     0,  "",      0,  0, "A"     },
                { L_,   "A",     1,  "",      0,  0, "A"     },

                { L_,   "",      0,  "A",     0,  0, ""      },
                { L_,   "",      0,  "A",     0,  1, "A"     },
                { L_,   "",      0,  "A",     1,  0, ""      },

                //line  d-array di   s-array si  ne  expected
                //----  ------- --   ------- --  --  --------   Depth = 2
                { L_,   "AB",    0,  "",      0,  0, "AB"    },
                { L_,   "AB",    1,  "",      0,  0, "AB"    },
                { L_,   "AB",    2,  "",      0,  0, "AB"    },

                { L_,   "A",     0,  "B",     0,  0, "A"     },
                { L_,   "A",     0,  "B",     0,  1, "BA"    },
                { L_,   "A",     0,  "B",     1,  0, "A"     },
                { L_,   "A",     1,  "B",     0,  0, "A"     },
                { L_,   "A",     1,  "B",     0,  1, "AB"    },
                { L_,   "A",     1,  "B",     1,  0, "A"     },

                { L_,   "",      0,  "AB",    0,  0, ""      },
                { L_,   "",      0,  "AB",    0,  1, "A"     },
                { L_,   "",      0,  "AB",    0,  2, "AB"    },
                { L_,   "",      0,  "AB",    1,  0, ""      },
                { L_,   "",      0,  "AB",    1,  1, "B"     },
                { L_,   "",      0,  "AB",    2,  0, ""      },

                //line  d-array di   s-array si  ne  expected
                //----  ------- --   ------- --  --  --------   Depth = 3
                { L_,   "ABC",   0,  "",      0,  0, "ABC"   },
                { L_,   "ABC",   1,  "",      0,  0, "ABC"   },
                { L_,   "ABC",   2,  "",      0,  0, "ABC"   },
                { L_,   "ABC",   3,  "",      0,  0, "ABC"   },

                { L_,   "AB",    0,  "C",     0,  0, "AB"    },
                { L_,   "AB",    0,  "C",     0,  1, "CAB"   },
                { L_,   "AB",    0,  "C",     1,  0, "AB"    },
                { L_,   "AB",    1,  "C",     0,  0, "AB"    },
                { L_,   "AB",    1,  "C",     0,  1, "ACB"   },
                { L_,   "AB",    1,  "C",     1,  0, "AB"    },
                { L_,   "AB",    2,  "C",     0,  0, "AB"    },
                { L_,   "AB",    2,  "C",     0,  1, "ABC"   },
                { L_,   "AB",    2,  "C",     1,  0, "AB"    },

                { L_,   "A",     0,  "BC",    0,  0, "A"     },
                { L_,   "A",     0,  "BC",    0,  1, "BA"    },
                { L_,   "A",     0,  "BC",    0,  2, "BCA"   },
                { L_,   "A",     0,  "BC",    1,  0, "A"     },
                { L_,   "A",     0,  "BC",    1,  1, "CA"    },
                { L_,   "A",     0,  "BC",    2,  0, "A"     },
                { L_,   "A",     1,  "BC",    0,  0, "A"     },
                { L_,   "A",     1,  "BC",    0,  1, "AB"    },
                { L_,   "A",     1,  "BC",    0,  2, "ABC"   },
                { L_,   "A",     1,  "BC",    1,  0, "A"     },
                { L_,   "A",     1,  "BC",    1,  1, "AC"    },
                { L_,   "A",     1,  "BC",    2,  0, "A"     },

                { L_,   "",      0,  "ABC",   0,  0, ""      },
                { L_,   "",      0,  "ABC",   0,  1, "A"     },
                { L_,   "",      0,  "ABC",   0,  2, "AB"    },
                { L_,   "",      0,  "ABC",   0,  3, "ABC"   },
                { L_,   "",      0,  "ABC",   1,  0, ""      },
                { L_,   "",      0,  "ABC",   1,  1, "B"     },
                { L_,   "",      0,  "ABC",   1,  2, "BC"    },
                { L_,   "",      0,  "ABC",   2,  0, ""      },
                { L_,   "",      0,  "ABC",   2,  1, "C"     },
                { L_,   "",      0,  "ABC",   3,  0, ""      },

                //line  d-array di   s-array si  ne  expected
                //----  ------- --   ------- --  --  --------   Depth = 4
                { L_,   "ABCD",  0,  "",      0,  0, "ABCD"  },
                { L_,   "ABCD",  1,  "",      0,  0, "ABCD"  },
                { L_,   "ABCD",  2,  "",      0,  0, "ABCD"  },
                { L_,   "ABCD",  3,  "",      0,  0, "ABCD"  },
                { L_,   "ABCD",  4,  "",      0,  0, "ABCD"  },

                { L_,   "ABC",   0,  "D",     0,  0, "ABC"   },
                { L_,   "ABC",   0,  "D",     0,  1, "DABC"  },
                { L_,   "ABC",   0,  "D",     1,  0, "ABC"   },
                { L_,   "ABC",   1,  "D",     0,  0, "ABC"   },
                { L_,   "ABC",   1,  "D",     0,  1, "ADBC"  },
                { L_,   "ABC",   1,  "D",     1,  0, "ABC"   },
                { L_,   "ABC",   2,  "D",     0,  0, "ABC"   },
                { L_,   "ABC",   2,  "D",     0,  1, "ABDC"  },
                { L_,   "ABC",   2,  "D",     1,  0, "ABC"   },
                { L_,   "ABC",   3,  "D",     0,  0, "ABC"   },
                { L_,   "ABC",   3,  "D",     0,  1, "ABCD"  },
                { L_,   "ABC",   3,  "D",     1,  0, "ABC"   },

                { L_,   "AB",    0,  "CD",    0,  0, "AB"    },
                { L_,   "AB",    0,  "CD",    0,  1, "CAB"   },
                { L_,   "AB",    0,  "CD",    0,  2, "CDAB"  },
                { L_,   "AB",    0,  "CD",    1,  0, "AB"    },
                { L_,   "AB",    0,  "CD",    1,  1, "DAB"   },
                { L_,   "AB",    0,  "CD",    2,  0, "AB"    },
                { L_,   "AB",    1,  "CD",    0,  0, "AB"    },
                { L_,   "AB",    1,  "CD",    0,  1, "ACB"   },
                { L_,   "AB",    1,  "CD",    0,  2, "ACDB"  },
                { L_,   "AB",    1,  "CD",    1,  0, "AB"    },
                { L_,   "AB",    1,  "CD",    1,  1, "ADB"   },
                { L_,   "AB",    1,  "CD",    2,  0, "AB"    },
                { L_,   "AB",    2,  "CD",    0,  0, "AB"    },
                { L_,   "AB",    2,  "CD",    0,  1, "ABC"   },
                { L_,   "AB",    2,  "CD",    0,  2, "ABCD"  },
                { L_,   "AB",    2,  "CD",    1,  0, "AB"    },
                { L_,   "AB",    2,  "CD",    1,  1, "ABD"   },
                { L_,   "AB",    2,  "CD",    2,  0, "AB"    },

                { L_,   "A",     0,  "BCD",   0,  0, "A"     },
                { L_,   "A",     0,  "BCD",   0,  1, "BA"    },
                { L_,   "A",     0,  "BCD",   0,  2, "BCA"   },
                { L_,   "A",     0,  "BCD",   0,  3, "BCDA"  },
                { L_,   "A",     0,  "BCD",   1,  0, "A"     },
                { L_,   "A",     0,  "BCD",   1,  1, "CA"    },
                { L_,   "A",     0,  "BCD",   1,  2, "CDA"   },
                { L_,   "A",     0,  "BCD",   2,  0, "A"     },
                { L_,   "A",     0,  "BCD",   2,  1, "DA"    },
                { L_,   "A",     0,  "BCD",   3,  0, "A"     },
                { L_,   "A",     1,  "BCD",   0,  0, "A"     },
                { L_,   "A",     1,  "BCD",   0,  1, "AB"    },
                { L_,   "A",     1,  "BCD",   0,  2, "ABC"   },
                { L_,   "A",     1,  "BCD",   0,  3, "ABCD"  },
                { L_,   "A",     1,  "BCD",   1,  0, "A"     },
                { L_,   "A",     1,  "BCD",   1,  1, "AC"    },
                { L_,   "A",     1,  "BCD",   1,  2, "ACD"   },
                { L_,   "A",     1,  "BCD",   2,  0, "A"     },
                { L_,   "A",     1,  "BCD",   2,  1, "AD"    },
                { L_,   "A",     1,  "BCD",   3,  0, "A"     },

                { L_,   "",      0,  "ABCD",  0,  0, ""      },
                { L_,   "",      0,  "ABCD",  0,  1, "A"     },
                { L_,   "",      0,  "ABCD",  0,  2, "AB"    },
                { L_,   "",      0,  "ABCD",  0,  3, "ABC"   },
                { L_,   "",      0,  "ABCD",  0,  4, "ABCD"  },
                { L_,   "",      0,  "ABCD",  1,  0, ""      },
                { L_,   "",      0,  "ABCD",  1,  1, "B"     },
                { L_,   "",      0,  "ABCD",  1,  2, "BC"    },
                { L_,   "",      0,  "ABCD",  1,  3, "BCD"   },
                { L_,   "",      0,  "ABCD",  2,  0, ""      },
                { L_,   "",      0,  "ABCD",  2,  1, "C"     },
                { L_,   "",      0,  "ABCD",  2,  2, "CD"    },
                { L_,   "",      0,  "ABCD",  3,  0, ""      },
                { L_,   "",      0,  "ABCD",  3,  1, "D"     },
                { L_,   "",      0,  "ABCD",  4,  0, ""      },

                //line  d-array di   s-array si  ne  expected
                //----  ------- --   ------- --  --  --------   Depth = 5
                { L_,   "ABCDE", 0,  "",      0,  0, "ABCDE" },
                { L_,   "ABCDE", 1,  "",      0,  0, "ABCDE" },
                { L_,   "ABCDE", 2,  "",      0,  0, "ABCDE" },
                { L_,   "ABCDE", 3,  "",      0,  0, "ABCDE" },
                { L_,   "ABCDE", 4,  "",      0,  0, "ABCDE" },
                { L_,   "ABCDE", 5,  "",      0,  0, "ABCDE" },

                { L_,   "ABCD",  0,  "E",     0,  0, "ABCD"  },
                { L_,   "ABCD",  0,  "E",     0,  1, "EABCD" },
                { L_,   "ABCD",  0,  "E",     1,  0, "ABCD"  },
                { L_,   "ABCD",  1,  "E",     0,  0, "ABCD"  },
                { L_,   "ABCD",  1,  "E",     0,  1, "AEBCD" },
                { L_,   "ABCD",  1,  "E",     1,  0, "ABCD"  },
                { L_,   "ABCD",  2,  "E",     0,  0, "ABCD"  },
                { L_,   "ABCD",  2,  "E",     0,  1, "ABECD" },
                { L_,   "ABCD",  2,  "E",     1,  0, "ABCD"  },
                { L_,   "ABCD",  3,  "E",     0,  0, "ABCD"  },
                { L_,   "ABCD",  3,  "E",     0,  1, "ABCED" },
                { L_,   "ABCD",  3,  "E",     1,  0, "ABCD"  },
                { L_,   "ABCD",  4,  "E",     0,  0, "ABCD"  },
                { L_,   "ABCD",  4,  "E",     0,  1, "ABCDE" },
                { L_,   "ABCD",  4,  "E",     1,  0, "ABCD"  },

                { L_,   "ABC",   0,  "DE",    0,  0, "ABC"   },
                { L_,   "ABC",   0,  "DE",    0,  1, "DABC"  },
                { L_,   "ABC",   0,  "DE",    0,  2, "DEABC" },
                { L_,   "ABC",   0,  "DE",    1,  0, "ABC"   },
                { L_,   "ABC",   0,  "DE",    1,  1, "EABC"  },
                { L_,   "ABC",   0,  "DE",    2,  0, "ABC"   },
                { L_,   "ABC",   1,  "DE",    0,  0, "ABC"   },
                { L_,   "ABC",   1,  "DE",    0,  1, "ADBC"  },
                { L_,   "ABC",   1,  "DE",    0,  2, "ADEBC" },
                { L_,   "ABC",   1,  "DE",    1,  0, "ABC"   },
                { L_,   "ABC",   1,  "DE",    1,  1, "AEBC"  },
                { L_,   "ABC",   1,  "DE",    2,  0, "ABC"   },
                { L_,   "ABC",   2,  "DE",    0,  0, "ABC"   },
                { L_,   "ABC",   2,  "DE",    0,  1, "ABDC"  },
                { L_,   "ABC",   2,  "DE",    0,  2, "ABDEC" },
                { L_,   "ABC",   2,  "DE",    1,  0, "ABC"   },
                { L_,   "ABC",   2,  "DE",    1,  1, "ABEC"  },
                { L_,   "ABC",   2,  "DE",    2,  0, "ABC"   },
                { L_,   "ABC",   3,  "DE",    0,  0, "ABC"   },
                { L_,   "ABC",   3,  "DE",    0,  1, "ABCD"  },
                { L_,   "ABC",   3,  "DE",    0,  2, "ABCDE" },
                { L_,   "ABC",   3,  "DE",    1,  0, "ABC"   },
                { L_,   "ABC",   3,  "DE",    1,  1, "ABCE"  },
                { L_,   "ABC",   3,  "DE",    2,  0, "ABC"   },

                { L_,   "AB",    0,  "CDE",   0,  0, "AB"    },
                { L_,   "AB",    0,  "CDE",   0,  1, "CAB"   },
                { L_,   "AB",    0,  "CDE",   0,  2, "CDAB"  },
                { L_,   "AB",    0,  "CDE",   0,  3, "CDEAB" },
                { L_,   "AB",    0,  "CDE",   1,  0, "AB"    },
                { L_,   "AB",    0,  "CDE",   1,  1, "DAB"   },
                { L_,   "AB",    0,  "CDE",   1,  2, "DEAB"  },
                { L_,   "AB",    0,  "CDE",   2,  0, "AB"    },
                { L_,   "AB",    0,  "CDE",   2,  1, "EAB"   },
                { L_,   "AB",    0,  "CDE",   3,  0, "AB"    },
                { L_,   "AB",    1,  "CDE",   0,  0, "AB"    },
                { L_,   "AB",    1,  "CDE",   0,  1, "ACB"   },
                { L_,   "AB",    1,  "CDE",   0,  2, "ACDB"  },
                { L_,   "AB",    1,  "CDE",   0,  3, "ACDEB" },
                { L_,   "AB",    1,  "CDE",   1,  0, "AB"    },
                { L_,   "AB",    1,  "CDE",   1,  1, "ADB"   },
                { L_,   "AB",    1,  "CDE",   1,  2, "ADEB"  },
                { L_,   "AB",    1,  "CDE",   2,  0, "AB"    },
                { L_,   "AB",    1,  "CDE",   2,  1, "AEB"   },
                { L_,   "AB",    1,  "CDE",   3,  0, "AB"    },
                { L_,   "AB",    2,  "CDE",   0,  0, "AB"    },
                { L_,   "AB",    2,  "CDE",   0,  1, "ABC"   },
                { L_,   "AB",    2,  "CDE",   0,  2, "ABCD"  },
                { L_,   "AB",    2,  "CDE",   0,  3, "ABCDE" },
                { L_,   "AB",    2,  "CDE",   1,  0, "AB"    },
                { L_,   "AB",    2,  "CDE",   1,  1, "ABD"   },
                { L_,   "AB",    2,  "CDE",   1,  2, "ABDE"  },
                { L_,   "AB",    2,  "CDE",   2,  0, "AB"    },
                { L_,   "AB",    2,  "CDE",   2,  1, "ABE"   },
                { L_,   "AB",    2,  "CDE",   3,  0, "AB"    },

                { L_,   "A",     0,  "BCDE",  0,  0, "A"     },
                { L_,   "A",     0,  "BCDE",  0,  1, "BA"    },
                { L_,   "A",     0,  "BCDE",  0,  2, "BCA"   },
                { L_,   "A",     0,  "BCDE",  0,  3, "BCDA"  },
                { L_,   "A",     0,  "BCDE",  1,  0, "A"     },
                { L_,   "A",     0,  "BCDE",  1,  1, "CA"    },
                { L_,   "A",     0,  "BCDE",  1,  2, "CDA"   },
                { L_,   "A",     0,  "BCDE",  2,  0, "A"     },
                { L_,   "A",     0,  "BCDE",  2,  1, "DA"    },
                { L_,   "A",     0,  "BCDE",  3,  0, "A"     },
                { L_,   "A",     1,  "BCDE",  0,  0, "A"     },
                { L_,   "A",     1,  "BCDE",  0,  1, "AB"    },
                { L_,   "A",     1,  "BCDE",  0,  2, "ABC"   },
                { L_,   "A",     1,  "BCDE",  0,  3, "ABCD"  },
                { L_,   "A",     1,  "BCDE",  1,  0, "A"     },
                { L_,   "A",     1,  "BCDE",  1,  1, "AC"    },
                { L_,   "A",     1,  "BCDE",  1,  2, "ACD"   },
                { L_,   "A",     1,  "BCDE",  2,  0, "A"     },
                { L_,   "A",     1,  "BCDE",  2,  1, "AD"    },
                { L_,   "A",     1,  "BCDE",  3,  0, "A"     },

                { L_,   "",      0,  "ABCDE", 0,  0, ""      },
                { L_,   "",      0,  "ABCDE", 0,  1, "A"     },
                { L_,   "",      0,  "ABCDE", 0,  2, "AB"    },
                { L_,   "",      0,  "ABCDE", 0,  3, "ABC"   },
                { L_,   "",      0,  "ABCDE", 0,  4, "ABCD"  },
                { L_,   "",      0,  "ABCDE", 0,  5, "ABCDE" },
                { L_,   "",      0,  "ABCDE", 1,  0, ""      },
                { L_,   "",      0,  "ABCDE", 1,  1, "B"     },
                { L_,   "",      0,  "ABCDE", 1,  2, "BC"    },
                { L_,   "",      0,  "ABCDE", 1,  3, "BCD"   },
                { L_,   "",      0,  "ABCDE", 1,  4, "BCDE"  },
                { L_,   "",      0,  "ABCDE", 2,  0, ""      },
                { L_,   "",      0,  "ABCDE", 2,  1, "C"     },
                { L_,   "",      0,  "ABCDE", 2,  2, "CD"    },
                { L_,   "",      0,  "ABCDE", 2,  3, "CDE"   },
                { L_,   "",      0,  "ABCDE", 3,  0, ""      },
                { L_,   "",      0,  "ABCDE", 3,  1, "D"     },
                { L_,   "",      0,  "ABCDE", 3,  2, "DE"    },
                { L_,   "",      0,  "ABCDE", 4,  0, ""      },
                { L_,   "",      0,  "ABCDE", 4,  1, "E"     },
                { L_,   "",      0,  "ABCDE", 5,  0, ""      },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            {
              if (verbose) cout <<
                "\nTesting with allocator, copyable.\n";

              TestCases<TestAllocCopy>::testcase13Insert(testStatus, argc,
                     argv, verbose, veryVerbose, veryVeryVerbose, DATA,
                     NUM_DATA, 0, testAllocator);

              if (verbose) cout <<
                "\nTesting with allocator, moveable.\n";

              TestCases<TestAllocMove>::testcase13Insert(testStatus, argc,
                     argv, verbose, veryVerbose, veryVeryVerbose, DATA,
                     NUM_DATA, 1, testAllocator);

              if (verbose) cout <<
                "\nTesting with allocator, not moveable.\n";

              TestCases<TestAllocNoMove>::testcase13Insert(testStatus, argc,
                     argv, verbose, veryVerbose, veryVeryVerbose, DATA,
                     NUM_DATA, 1, testAllocator);

            }
        }

        //---------------------------------------------------------------------

        if (verbose) cout <<
            "\nTesting x.insert(di, sa, si, ne) et. al. (aliasing)" << endl;
        {
            static const struct InsertAliasTable DATA[] = {
                //line  x-array di   si  ne  expected
                //----  ------- --   --  --  --------   Depth = 0
                { L_,   "",      0,   0,  0,  ""      },

                //line  x-array di   si  ne  expected
                //----  ------- --   --  --  --------   Depth = 1
                { L_,   "A",     0,   0,  0,  "A"     },
                { L_,   "A",     0,   0,  1,  "AA"    },
                { L_,   "A",     0,   1,  0,  "A"     },

                { L_,   "A",     1,   0,  0,  "A"     },
                { L_,   "A",     1,   0,  1,  "AA"    },
                { L_,   "A",     1,   1,  0,  "A"     },

                //line  x-array di   si  ne  expected
                //----  ------- --   --  --  --------   Depth = 2
                { L_,   "AB",    0,   0,  0,  "AB"    },
                { L_,   "AB",    0,   0,  1,  "AAB"   },
                { L_,   "AB",    0,   0,  2,  "ABAB"  },
                { L_,   "AB",    0,   1,  0,  "AB"    },
                { L_,   "AB",    0,   1,  1,  "BAB"   },
                { L_,   "AB",    0,   2,  0,  "AB"    },

                { L_,   "AB",    1,   0,  0,  "AB"    },
                { L_,   "AB",    1,   0,  1,  "AAB"   },
                { L_,   "AB",    1,   0,  2,  "AABB"  },
                { L_,   "AB",    1,   1,  0,  "AB"    },
                { L_,   "AB",    1,   1,  1,  "ABB"   },
                { L_,   "AB",    1,   2,  0,  "AB"    },

                { L_,   "AB",    2,   0,  0,  "AB"    },
                { L_,   "AB",    2,   0,  1,  "ABA"   },
                { L_,   "AB",    2,   0,  2,  "ABAB"  },
                { L_,   "AB",    2,   1,  0,  "AB"    },
                { L_,   "AB",    2,   1,  1,  "ABB"   },
                { L_,   "AB",    2,   2,  0,  "AB"    },

                //line  x-array di   si  ne  expected
                //----  ------- --   --  --  --------   Depth = 3
                { L_,   "ABC",   0,   0,  0,  "ABC"     },
                { L_,   "ABC",   0,   0,  1,  "AABC"    },
                { L_,   "ABC",   0,   0,  2,  "ABABC"   },
                { L_,   "ABC",   0,   0,  3,  "ABCABC"  },
                { L_,   "ABC",   0,   1,  0,  "ABC"     },
                { L_,   "ABC",   0,   1,  1,  "BABC"    },
                { L_,   "ABC",   0,   1,  2,  "BCABC"   },
                { L_,   "ABC",   0,   2,  0,  "ABC"     },
                { L_,   "ABC",   0,   2,  1,  "CABC"    },
                { L_,   "ABC",   0,   3,  0,  "ABC"     },

                { L_,   "ABC",   1,   0,  0,  "ABC"     },
                { L_,   "ABC",   1,   0,  1,  "AABC"    },
                { L_,   "ABC",   1,   0,  2,  "AABBC"   },
                { L_,   "ABC",   1,   0,  3,  "AABCBC"  },
                { L_,   "ABC",   1,   1,  0,  "ABC"     },
                { L_,   "ABC",   1,   1,  1,  "ABBC"    },
                { L_,   "ABC",   1,   1,  2,  "ABCBC"   },
                { L_,   "ABC",   1,   2,  0,  "ABC"     },
                { L_,   "ABC",   1,   2,  1,  "ACBC"    },
                { L_,   "ABC",   1,   3,  0,  "ABC"     },

                { L_,   "ABC",   2,   0,  0,  "ABC"     },
                { L_,   "ABC",   2,   0,  1,  "ABAC"    },
                { L_,   "ABC",   2,   0,  2,  "ABABC"   },
                { L_,   "ABC",   2,   0,  3,  "ABABCC"  },
                { L_,   "ABC",   2,   1,  0,  "ABC"     },
                { L_,   "ABC",   2,   1,  1,  "ABBC"    },
                { L_,   "ABC",   2,   1,  2,  "ABBCC"   },
                { L_,   "ABC",   2,   2,  0,  "ABC"     },
                { L_,   "ABC",   2,   2,  1,  "ABCC"    },
                { L_,   "ABC",   2,   3,  0,  "ABC"     },

                { L_,   "ABC",   3,   0,  0,  "ABC"     },
                { L_,   "ABC",   3,   0,  1,  "ABCA"    },
                { L_,   "ABC",   3,   0,  2,  "ABCAB"   },
                { L_,   "ABC",   3,   0,  3,  "ABCABC"  },
                { L_,   "ABC",   3,   1,  0,  "ABC"     },
                { L_,   "ABC",   3,   1,  1,  "ABCB"    },
                { L_,   "ABC",   3,   1,  2,  "ABCBC"   },
                { L_,   "ABC",   3,   2,  0,  "ABC"     },
                { L_,   "ABC",   3,   2,  1,  "ABCC"    },
                { L_,   "ABC",   3,   3,  0,  "ABC"     },

                //line  x-array di   si  ne  expected
                //----  ------- --   --  --  --------   Depth = 4
                { L_,   "ABCD",  0,   0,  0,  "ABCD"      },
                { L_,   "ABCD",  0,   0,  1,  "AABCD"     },
                { L_,   "ABCD",  0,   0,  2,  "ABABCD"    },
                { L_,   "ABCD",  0,   0,  3,  "ABCABCD"   },
                { L_,   "ABCD",  0,   0,  4,  "ABCDABCD"  },
                { L_,   "ABCD",  0,   1,  0,  "ABCD"      },
                { L_,   "ABCD",  0,   1,  1,  "BABCD"     },
                { L_,   "ABCD",  0,   1,  2,  "BCABCD"    },
                { L_,   "ABCD",  0,   1,  3,  "BCDABCD"   },
                { L_,   "ABCD",  0,   2,  0,  "ABCD"      },
                { L_,   "ABCD",  0,   2,  1,  "CABCD"     },
                { L_,   "ABCD",  0,   2,  2,  "CDABCD"    },
                { L_,   "ABCD",  0,   3,  0,  "ABCD"      },
                { L_,   "ABCD",  0,   3,  1,  "DABCD"     },
                { L_,   "ABCD",  0,   4,  0,  "ABCD"      },

                { L_,   "ABCD",  1,   0,  0,  "ABCD"      },
                { L_,   "ABCD",  1,   0,  1,  "AABCD"     },
                { L_,   "ABCD",  1,   0,  2,  "AABBCD"    },
                { L_,   "ABCD",  1,   0,  3,  "AABCBCD"   },
                { L_,   "ABCD",  1,   0,  4,  "AABCDBCD"  },
                { L_,   "ABCD",  1,   1,  0,  "ABCD"      },
                { L_,   "ABCD",  1,   1,  1,  "ABBCD"     },
                { L_,   "ABCD",  1,   1,  2,  "ABCBCD"    },
                { L_,   "ABCD",  1,   1,  3,  "ABCDBCD"   },
                { L_,   "ABCD",  1,   2,  0,  "ABCD"      },
                { L_,   "ABCD",  1,   2,  1,  "ACBCD"     },
                { L_,   "ABCD",  1,   2,  2,  "ACDBCD"    },
                { L_,   "ABCD",  1,   3,  0,  "ABCD"      },
                { L_,   "ABCD",  1,   3,  1,  "ADBCD"     },
                { L_,   "ABCD",  1,   4,  0,  "ABCD"      },

                { L_,   "ABCD",  2,   0,  0,  "ABCD"      },
                { L_,   "ABCD",  2,   0,  1,  "ABACD"     },
                { L_,   "ABCD",  2,   0,  2,  "ABABCD"    },
                { L_,   "ABCD",  2,   0,  3,  "ABABCCD"   },
                { L_,   "ABCD",  2,   0,  4,  "ABABCDCD"  },
                { L_,   "ABCD",  2,   1,  0,  "ABCD"      },
                { L_,   "ABCD",  2,   1,  1,  "ABBCD"     },
                { L_,   "ABCD",  2,   1,  2,  "ABBCCD"    },
                { L_,   "ABCD",  2,   1,  3,  "ABBCDCD"   },
                { L_,   "ABCD",  2,   2,  0,  "ABCD"      },
                { L_,   "ABCD",  2,   2,  1,  "ABCCD"     },
                { L_,   "ABCD",  2,   2,  2,  "ABCDCD"    },
                { L_,   "ABCD",  2,   3,  0,  "ABCD"      },
                { L_,   "ABCD",  2,   3,  1,  "ABDCD"     },
                { L_,   "ABCD",  2,   4,  0,  "ABCD"      },

                { L_,   "ABCD",  3,   0,  0,  "ABCD"      },
                { L_,   "ABCD",  3,   0,  1,  "ABCAD"     },
                { L_,   "ABCD",  3,   0,  2,  "ABCABD"    },
                { L_,   "ABCD",  3,   0,  3,  "ABCABCD"   },
                { L_,   "ABCD",  3,   0,  4,  "ABCABCDD"  },
                { L_,   "ABCD",  3,   1,  0,  "ABCD"      },
                { L_,   "ABCD",  3,   1,  1,  "ABCBD"     },
                { L_,   "ABCD",  3,   1,  2,  "ABCBCD"    },
                { L_,   "ABCD",  3,   1,  3,  "ABCBCDD"   },
                { L_,   "ABCD",  3,   2,  0,  "ABCD"      },
                { L_,   "ABCD",  3,   2,  1,  "ABCCD"     },
                { L_,   "ABCD",  3,   2,  2,  "ABCCDD"    },
                { L_,   "ABCD",  3,   3,  0,  "ABCD"      },
                { L_,   "ABCD",  3,   3,  1,  "ABCDD"     },
                { L_,   "ABCD",  3,   4,  0,  "ABCD"      },

                { L_,   "ABCD",  4,   0,  0,  "ABCD"      },
                { L_,   "ABCD",  4,   0,  1,  "ABCDA"     },
                { L_,   "ABCD",  4,   0,  2,  "ABCDAB"    },
                { L_,   "ABCD",  4,   0,  3,  "ABCDABC"   },
                { L_,   "ABCD",  4,   0,  4,  "ABCDABCD"  },
                { L_,   "ABCD",  4,   1,  0,  "ABCD"      },
                { L_,   "ABCD",  4,   1,  1,  "ABCDB"     },
                { L_,   "ABCD",  4,   1,  2,  "ABCDBC"    },
                { L_,   "ABCD",  4,   1,  3,  "ABCDBCD"   },
                { L_,   "ABCD",  4,   2,  0,  "ABCD"      },
                { L_,   "ABCD",  4,   2,  1,  "ABCDC"     },
                { L_,   "ABCD",  4,   2,  2,  "ABCDCD"    },
                { L_,   "ABCD",  4,   3,  0,  "ABCD"      },
                { L_,   "ABCD",  4,   3,  1,  "ABCDD"     },
                { L_,   "ABCD",  4,   4,  0,  "ABCD"      },

                //line  x-array di   si  ne  expected
                //----  ------- --   --  --  --------   Depth = 5
                { L_,   "ABCDE", 0,   0,  0,  "ABCDE"      },
                { L_,   "ABCDE", 0,   0,  1,  "AABCDE"     },
                { L_,   "ABCDE", 0,   0,  2,  "ABABCDE"    },
                { L_,   "ABCDE", 0,   0,  3,  "ABCABCDE"   },
                { L_,   "ABCDE", 0,   0,  4,  "ABCDABCDE"  },
                { L_,   "ABCDE", 0,   0,  5,  "ABCDEABCDE" },
                { L_,   "ABCDE", 0,   1,  0,  "ABCDE"      },
                { L_,   "ABCDE", 0,   1,  1,  "BABCDE"     },
                { L_,   "ABCDE", 0,   1,  2,  "BCABCDE"    },
                { L_,   "ABCDE", 0,   1,  3,  "BCDABCDE"   },
                { L_,   "ABCDE", 0,   1,  4,  "BCDEABCDE"  },
                { L_,   "ABCDE", 0,   2,  0,  "ABCDE"      },
                { L_,   "ABCDE", 0,   2,  1,  "CABCDE"     },
                { L_,   "ABCDE", 0,   2,  2,  "CDABCDE"    },
                { L_,   "ABCDE", 0,   2,  3,  "CDEABCDE"   },
                { L_,   "ABCDE", 0,   3,  0,  "ABCDE"      },
                { L_,   "ABCDE", 0,   3,  1,  "DABCDE"     },
                { L_,   "ABCDE", 0,   3,  2,  "DEABCDE"    },
                { L_,   "ABCDE", 0,   4,  0,  "ABCDE"      },
                { L_,   "ABCDE", 0,   4,  1,  "EABCDE"     },
                { L_,   "ABCDE", 0,   5,  0,  "ABCDE"      },

                { L_,   "ABCDE", 1,   0,  0,  "ABCDE"      },
                { L_,   "ABCDE", 1,   0,  1,  "AABCDE"     },
                { L_,   "ABCDE", 1,   0,  2,  "AABBCDE"    },
                { L_,   "ABCDE", 1,   0,  3,  "AABCBCDE"   },
                { L_,   "ABCDE", 1,   0,  4,  "AABCDBCDE"  },
                { L_,   "ABCDE", 1,   0,  5,  "AABCDEBCDE" },
                { L_,   "ABCDE", 1,   1,  0,  "ABCDE"      },
                { L_,   "ABCDE", 1,   1,  1,  "ABBCDE"     },
                { L_,   "ABCDE", 1,   1,  2,  "ABCBCDE"    },
                { L_,   "ABCDE", 1,   1,  3,  "ABCDBCDE"   },
                { L_,   "ABCDE", 1,   1,  4,  "ABCDEBCDE"  },
                { L_,   "ABCDE", 1,   2,  0,  "ABCDE"      },
                { L_,   "ABCDE", 1,   2,  1,  "ACBCDE"     },
                { L_,   "ABCDE", 1,   2,  2,  "ACDBCDE"    },
                { L_,   "ABCDE", 1,   2,  3,  "ACDEBCDE"   },
                { L_,   "ABCDE", 1,   3,  0,  "ABCDE"      },
                { L_,   "ABCDE", 1,   3,  1,  "ADBCDE"     },
                { L_,   "ABCDE", 1,   3,  2,  "ADEBCDE"    },
                { L_,   "ABCDE", 1,   4,  0,  "ABCDE"      },
                { L_,   "ABCDE", 1,   4,  1,  "AEBCDE"     },
                { L_,   "ABCDE", 1,   5,  0,  "ABCDE"      },

                { L_,   "ABCDE", 2,   0,  0,  "ABCDE"      },
                { L_,   "ABCDE", 2,   0,  1,  "ABACDE"     },
                { L_,   "ABCDE", 2,   0,  2,  "ABABCDE"    },
                { L_,   "ABCDE", 2,   0,  3,  "ABABCCDE"   },
                { L_,   "ABCDE", 2,   0,  4,  "ABABCDCDE"  },
                { L_,   "ABCDE", 2,   0,  5,  "ABABCDECDE" },
                { L_,   "ABCDE", 2,   1,  0,  "ABCDE"      },
                { L_,   "ABCDE", 2,   1,  1,  "ABBCDE"     },
                { L_,   "ABCDE", 2,   1,  2,  "ABBCCDE"    },
                { L_,   "ABCDE", 2,   1,  3,  "ABBCDCDE"   },
                { L_,   "ABCDE", 2,   1,  4,  "ABBCDECDE"  },
                { L_,   "ABCDE", 2,   2,  0,  "ABCDE"      },
                { L_,   "ABCDE", 2,   2,  1,  "ABCCDE"     },
                { L_,   "ABCDE", 2,   2,  2,  "ABCDCDE"    },
                { L_,   "ABCDE", 2,   2,  3,  "ABCDECDE"   },
                { L_,   "ABCDE", 2,   3,  0,  "ABCDE"      },
                { L_,   "ABCDE", 2,   3,  1,  "ABDCDE"     },
                { L_,   "ABCDE", 2,   3,  2,  "ABDECDE"    },
                { L_,   "ABCDE", 2,   4,  0,  "ABCDE"      },
                { L_,   "ABCDE", 2,   4,  1,  "ABECDE"     },
                { L_,   "ABCDE", 2,   5,  0,  "ABCDE"      },

                { L_,   "ABCDE", 3,   0,  0,  "ABCDE"      },
                { L_,   "ABCDE", 3,   0,  1,  "ABCADE"     },
                { L_,   "ABCDE", 3,   0,  2,  "ABCABDE"    },
                { L_,   "ABCDE", 3,   0,  3,  "ABCABCDE"   },
                { L_,   "ABCDE", 3,   0,  4,  "ABCABCDDE"  },
                { L_,   "ABCDE", 3,   0,  5,  "ABCABCDEDE" },
                { L_,   "ABCDE", 3,   1,  0,  "ABCDE"      },
                { L_,   "ABCDE", 3,   1,  1,  "ABCBDE"     },
                { L_,   "ABCDE", 3,   1,  2,  "ABCBCDE"    },
                { L_,   "ABCDE", 3,   1,  3,  "ABCBCDDE"   },
                { L_,   "ABCDE", 3,   1,  4,  "ABCBCDEDE"  },
                { L_,   "ABCDE", 3,   2,  0,  "ABCDE"      },
                { L_,   "ABCDE", 3,   2,  1,  "ABCCDE"     },
                { L_,   "ABCDE", 3,   2,  2,  "ABCCDDE"    },
                { L_,   "ABCDE", 3,   2,  3,  "ABCCDEDE"   },
                { L_,   "ABCDE", 3,   3,  0,  "ABCDE"      },
                { L_,   "ABCDE", 3,   3,  1,  "ABCDDE"     },
                { L_,   "ABCDE", 3,   3,  2,  "ABCDEDE"    },
                { L_,   "ABCDE", 3,   4,  0,  "ABCDE"      },
                { L_,   "ABCDE", 3,   4,  1,  "ABCEDE"     },
                { L_,   "ABCDE", 3,   5,  0,  "ABCDE"      },

                { L_,   "ABCDE", 4,   0,  0,  "ABCDE"      },
                { L_,   "ABCDE", 4,   0,  1,  "ABCDAE"     },
                { L_,   "ABCDE", 4,   0,  2,  "ABCDABE"    },
                { L_,   "ABCDE", 4,   0,  3,  "ABCDABCE"   },
                { L_,   "ABCDE", 4,   0,  4,  "ABCDABCDE"  },
                { L_,   "ABCDE", 4,   0,  5,  "ABCDABCDEE" },
                { L_,   "ABCDE", 4,   1,  0,  "ABCDE"      },
                { L_,   "ABCDE", 4,   1,  1,  "ABCDBE"     },
                { L_,   "ABCDE", 4,   1,  2,  "ABCDBCE"    },
                { L_,   "ABCDE", 4,   1,  3,  "ABCDBCDE"   },
                { L_,   "ABCDE", 4,   1,  4,  "ABCDBCDEE"  },
                { L_,   "ABCDE", 4,   2,  0,  "ABCDE"      },
                { L_,   "ABCDE", 4,   2,  1,  "ABCDCE"     },
                { L_,   "ABCDE", 4,   2,  2,  "ABCDCDE"    },
                { L_,   "ABCDE", 4,   2,  3,  "ABCDCDEE"   },
                { L_,   "ABCDE", 4,   3,  0,  "ABCDE"      },
                { L_,   "ABCDE", 4,   3,  1,  "ABCDDE"     },
                { L_,   "ABCDE", 4,   3,  2,  "ABCDDEE"    },
                { L_,   "ABCDE", 4,   4,  0,  "ABCDE"      },
                { L_,   "ABCDE", 4,   4,  1,  "ABCDEE"     },
                { L_,   "ABCDE", 4,   5,  0,  "ABCDE"      },

                { L_,   "ABCDE", 5,   0,  0,  "ABCDE"      },
                { L_,   "ABCDE", 5,   0,  1,  "ABCDEA"     },
                { L_,   "ABCDE", 5,   0,  2,  "ABCDEAB"    },
                { L_,   "ABCDE", 5,   0,  3,  "ABCDEABC"   },
                { L_,   "ABCDE", 5,   0,  4,  "ABCDEABCD"  },
                { L_,   "ABCDE", 5,   0,  5,  "ABCDEABCDE" },
                { L_,   "ABCDE", 5,   1,  0,  "ABCDE"      },
                { L_,   "ABCDE", 5,   1,  1,  "ABCDEB"     },
                { L_,   "ABCDE", 5,   1,  2,  "ABCDEBC"    },
                { L_,   "ABCDE", 5,   1,  3,  "ABCDEBCD"   },
                { L_,   "ABCDE", 5,   1,  4,  "ABCDEBCDE"  },
                { L_,   "ABCDE", 5,   2,  0,  "ABCDE"      },
                { L_,   "ABCDE", 5,   2,  1,  "ABCDEC"     },
                { L_,   "ABCDE", 5,   2,  2,  "ABCDECD"    },
                { L_,   "ABCDE", 5,   2,  3,  "ABCDECDE"   },
                { L_,   "ABCDE", 5,   3,  0,  "ABCDE"      },
                { L_,   "ABCDE", 5,   3,  1,  "ABCDED"     },
                { L_,   "ABCDE", 5,   3,  2,  "ABCDEDE"    },
                { L_,   "ABCDE", 5,   4,  0,  "ABCDE"      },
                { L_,   "ABCDE", 5,   4,  1,  "ABCDEE"     },
                { L_,   "ABCDE", 5,   5,  0,  "ABCDE"      },

            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            if (verbose) cout <<
                     "\nTesting with allocator, copyable.\n";

            TestCases<TestAllocCopy>::testcase13InsertAlias(testStatus, argc,
                     argv, verbose, veryVerbose, veryVeryVerbose, DATA,
                     NUM_DATA, 0, testAllocator);

            if (verbose) cout <<
                     "\nTesting with allocator, not moveable.\n";

            TestCases<TestAllocNoMove>::testcase13InsertAlias(testStatus, argc,
                     argv, verbose, veryVerbose, veryVeryVerbose, DATA,
                     NUM_DATA, 1, testAllocator);
        }

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING SET-LENGTH FUNCTIONS:
        //   1. We have the following concerns:
        //    - The resulting length is correct.
        //    - The resulting element values are correct when:
        //        new length <  initial length
        //        new length == initial length
        //        new length >  initial length
        //   2. We are also concerned that the test data include sufficient
        //   differences in initial and final length that resizing is
        //   guaranteed to occur.  Beyond that, no explicit "white box" test is
        //   required.
        //
        //   3. The set-length functions internal functionality vary
        //   according to which bitwise copy/move trait is applied.
        //
        //   4. setLength must be tested for ascending and descending lengths
        //   because setLength may run the destructor on elements no longer
        //   needed.
        //
        // Plan:
        //   Specify a set A of lengths.  For each a1 in A construct an object
        //   x of length a1 with each element in x initialized to an arbitrary
        //   but known value V.  For each a2 in A use the 'setLength' method
        //   under test to set the length of x and potentially remove or set
        //   element values as per the method's contract.  Use the basic
        //   accessors to verify the length and element values of the modified
        //   object x.
        //
        //   To address concern 4, all these tests are performed on user
        //   defined types:
        //          With allocator, copyable
        //          With allocator, moveable
        //          With allocator, not moveable
        //
        // Testing:
        //   void setLength(int newLength);
        //   void setLength(int newLength, const bdet_String& iVal);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Set-Length Functions" << endl
                          << "============================" << endl;

        if (verbose) cout <<
              "\nTesting with allocator, copyable.\n";

        TestCases<TestAllocCopy>::testcase12(testStatus, argc,
               argv, verbose, veryVerbose, veryVeryVerbose, testAllocator);

        if (verbose) cout <<
              "\nTesting with allocator, not moveable.\n";

        TestCases<TestAllocNoMove>::testcase12(testStatus, argc,
               argv, verbose, veryVerbose, veryVeryVerbose, testAllocator);

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING INITIAL-LENGTH CONSTRUCTORS:
        //   We have the following concerns:
        //    1. The initial value is correct.
        //    2. The constructor is exception neutral w.r.t. memory allocation.
        //    3. The internal memory management system is hooked up properly
        //       so that *all* internally allocated memory draws from a
        //       user-supplied allocator whenever one is specified.
        //    4. The initial length constructors' internal functionality vary
        //       according to which bitwise copy trait is applied.
        //
        // Plan:
        //   For each constructor we will create objects
        //    - With and without passing in an allocator.
        //    - In the presence of exceptions during memory allocations using
        //        a 'bdema_TestAllocator' and varying its *allocation* *limit*.
        //    - Where the object is constructed entirely in static memory
        //        (using a 'bdema_BufferAllocator') and never destroyed.
        //   and use basic accessors to verify
        //      - length
        //      - element value at each index position { 0 .. length - 1 }.
        //
        //   To address concern 4, all these tests are performed on user
        //   defined types:
        //          With allocator, copyable
        //          With allocator, moveable
        //
        // Testing:
        //   bdea_Array<T>(const Explicit& iLen, *ba = 0);
        //   bdea_Array<T>(int iLen, const bdet_String& iVal, *ba = 0);
        //   bdea_Array<T>(const int *sa, int ne, *ba = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Initial-Length Constructor" << endl
                          << "==================================" << endl;

        if (verbose) cout <<
              "\nTesting with allocator, copyable.\n";

        TestCases<TestAllocCopy>::testcase11(testStatus, argc,
               argv, verbose, veryVerbose, veryVeryVerbose, testAllocator);

        if (verbose) cout <<
              "\nTesting with allocator, moveable.\n";

        TestCases<TestAllocMove>::testcase11(testStatus, argc,
               argv, verbose, veryVerbose, veryVeryVerbose, testAllocator);


      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING STREAMING FUNCTIONALITY:
        //   1. The (free) streaming operators '<<' and '>>' are implemented
        //      using the respective free functions 'bdea_Array_streamOut' and
        //      'bdea_Array_streamIn'.
        //   2. Streaming must be neutral to exceptions thrown as a result of
        //      either allocating memory or streaming in values.
        //   3. Ensure that streaming works under the following conditions:
        //       VALID - may contain any sequence of valid values.
        //       EMPTY - valid, but contains no data.
        //       INVALID - may or may not be empty.
        //       INCOMPLETE - the stream is truncated, but otherwise valid.
        //       CORRUPTED - the data contains explicitly inconsistent fields.
        //
        //   Plan:
        //     First perform a trivial direct (breathing) test of the
        //     'outStream' and 'inStream' methods (to address concern 1).  Note
        //     that the rest of the testing will use the stream operators.
        //
        //     Next, specify a set S of unique object values with substantial
        //     and varied differences, ordered by increasing length.  For each
        //     value in S, construct an object x along with a sequence of
        //     similarly constructed duplicates x1, x2, ..., xN.  Attempt to
        //     affect every aspect of white-box state by altering each xi in
        //     a unique way.  Let the union of all such objects be the set T.
        //
        //   VALID STREAMS (and exceptions)
        //      Using all combinations of (u, v) in T X T, stream-out the value
        //      of u into a buffer and stream it back into (an independent
        //      instance of) v, and assert that u == v.
        //
        //   EMPTY AND INVALID STREAMS
        //     For each x in T, attempt to stream into (a temporary copy of) x
        //     from an empty and then invalid stream.  Verify after each try
        //     that the object is unchanged and that the stream is invalid.
        //
        //   INCOMPLETE (BUT OTHERWISE VALID) DATA
        //     Write 3 distinct objects to an output stream buffer of total
        //     length N.  For each partial stream length from 0 to N - 1,
        //     construct a truncated input stream and attempt to read into
        //     objects initialized with distinct values.  Verify values of
        //     objects that are either successfully modified or left entirely
        //     unmodified,  and that the stream became invalid immediately
        //     after the first incomplete read.  Finally ensure that each
        //     object streamed into is in some valid state by creating a copy
        //     and then assigning a known value to that copy; allow the
        //     original instance to leave scope without further modification,
        //     so that the destructor asserts internal object invariants
        //     appropriately.
        //
        //   CORRUPTED DATA
        //     We will assume that the incomplete test fail every field,
        //     including a char (multi-byte representation) hence we need
        //     only to produce values that are inconsistent with a valid
        //     value and verify that they are detected.  Use the underlying
        //     stream package to simulate an instance of a typical valid
        //     (control) stream and verify that it can be streamed in
        //     successfully.  Then for each data field in the stream (beginning
        //     with the version number), provide one or more similar tests with
        //     that data field corrupted.  After each test, verify that the
        //     object is in some valid state after streaming, and that the
        //     input stream has gone invalid.
        //
        // TEST PERFORMANCE
        //     since a full test with exceptions using purify can take
        //     of the order of 20 minutes, not every permutation is tested
        //     for every test.
        //     Every N'th permutation is tested where N is a prime number.
        //     By default N is 11.  This may be changed at run time through
        //     an additional parameter.
        //
        // Testing:
        //   operator>>(bdex_InStream&, bdea_Array<T>&);
        //   operator<<(bdex_OutStream&, const bdea_Array<T>&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Streaming Functionality" << endl
                          << "===============================" << endl;

        typedef bdea_Array<TestAllocNoMove> Obj;
        typedef TestAllocNoMove Element;
        const Element *VALUES;
        testDriver<Element>::getValues(&VALUES);

        if (verbose) cout <<
            "\nTesting stream operators ('<<' and '>>')." << endl;

        if (verbose)
           cout << "\tOn valid, non-empty stream data.  Some permutations."
           << endl;
        {
            // Since every permutation with exceptions enabled can be
            // lengthy, only some permutations are tested.  This can
            // be overridden on the command line.  Prime numbers > 5 are
            // recommended.  Specify 1 to test every permutation.
            static const char *SPECS[] = { // len: 0 - 5,
                "",       "A",      "BC",     "CDE",    "DEAB",   "EABCD",
            0}; // Null string required as last element.

            static const int EXTEND[] = {
                0, 1, 2, 3, 4, 5,
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            int iterationModulus = argc > 2 ? atoi(argv[2]) : 0;
            if (iterationModulus <= 0) iterationModulus = 11;
            if (verbose) P(iterationModulus);

            int iteration = 0;

            int uOldLen = -1;
            for (int ui = 0; SPECS[ui]; ++ui) {
                const char *const U_SPEC = SPECS[ui];
                const int uLen = (int) strlen(U_SPEC);

                if (verbose) {
                    cout << "\t\tFor source objects of length "
                                                        << uLen << ":\t";
                    P(U_SPEC);
                }

                LOOP_ASSERT(U_SPEC, uOldLen < uLen);  // strictly increasing
                uOldLen = uLen;

                const Obj UU = testDriver<Element>::g(U_SPEC);      // control
                LOOP_ASSERT(ui, uLen == UU.length());         // same lengths

                for (int uj = 0; uj < NUM_EXTEND; ++uj) {
                    const int U_N = EXTEND[uj];

                    Obj mU(&testAllocator);
                    testDriver<Element>::stretchRemoveAll(&mU, U_N, VALUES[0]);
                    const Obj& U = mU; testDriver<Element>::gg(&mU, U_SPEC);

                    bdex_TestOutStream out;

                    out << U;        // testing stream-out operator here

                    const char *const OD  = out.data();
                    const int         LOD = out.length();

                    // Must reset stream for each iteration of inner loop.
                    bdex_TestInStream testInStream(OD, LOD);
                    LOOP2_ASSERT(U_SPEC, U_N, testInStream);
                    LOOP2_ASSERT(U_SPEC, U_N, !testInStream.isEmpty());

                    for (int vi = 0; SPECS[vi]; ++vi) {
                        const char *const V_SPEC = SPECS[vi];
                        const int vLen = (int) strlen(V_SPEC);

                        const Obj VV =
                              testDriver<Element>::g(V_SPEC);  // control

                        if (0 == uj && veryVerbose || veryVeryVerbose) {
                            cout << "\t\t\tFor destination objects of length "
                                                        << vLen << ":\t";
                            P(V_SPEC);
                        }

                        for (int vj = 0; vj < NUM_EXTEND; ++vj) {
                          if (iteration % iterationModulus == 0)
                          {
                            // run test only on certain iterations.

                          BEGIN_BDEMA_EXCEPTION_TEST {
                          BEGIN_BDEX_EXCEPTION_TEST {
                            testInStream.reset();
                            const int V_N = EXTEND[vj];
                            const int AL = testAllocator.allocationLimit();
                            testAllocator.setAllocationLimit(-1);
            //--------------^
            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, testInStream);
            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, !testInStream.isEmpty());
            //--------------v
                            Obj mV(&testAllocator);
                            testDriver<Element>::stretchRemoveAll(&mV, V_N,
                                                                  VALUES[0]);
                            const Obj& V = mV;
                            testDriver<Element>::gg(&mV, V_SPEC);

                            static int firstFew = 2 * NUM_EXTEND * NUM_EXTEND;
                            if (veryVeryVerbose||veryVerbose && firstFew > 0) {
                                cout << "\t |"; P_(U_N); P_(V_N); P_(U); P(V);
                                --firstFew;
                            }

                            testAllocator.setAllocationLimit(AL);
                            testInStream >> mV; // test stream-in operator here

                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, UU == U);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, UU == V);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N,  U == V);

                          } END_BDEX_EXCEPTION_TEST
                          } END_BDEMA_EXCEPTION_TEST
                          }
                          else {
                                if (veryVeryVerbose) cout << "skipped.\n";
                          }
                          ++iteration;
                    }
                }
            }
        }

        if (verbose) cout <<
             "\tOn valid, non-empty stream data.  All permutations." << endl;
        {
            static const char *SPECS[] = { // len: 0 - 5,
                "",       "A",      "BC",     "CDE",    "DEAB",   "EABCD",
            0}; // Null string required as last element.

            static const int EXTEND[] = {
                0, 1, 2, 3, 4, 5,
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            int uOldLen = -1;
            for (int ui = 0; SPECS[ui]; ++ui) {
                const char *const U_SPEC = SPECS[ui];
                const int uLen = (int) strlen(U_SPEC);

                if (verbose) {
                    cout << "\t\tFor source objects of length "
                                                        << uLen << ":\t";
                    P(U_SPEC);
                }

                LOOP_ASSERT(U_SPEC, uOldLen < uLen);  // strictly increasing
                uOldLen = uLen;

                const Obj UU = testDriver<Element>::g(U_SPEC); // control
                LOOP_ASSERT(ui, uLen == UU.length());    // same lengths

                for (int uj = 0; uj < NUM_EXTEND; ++uj) {
                    const int U_N = EXTEND[uj];

                    Obj mU(&testAllocator);
                    testDriver<Element>::stretchRemoveAll(&mU, U_N, VALUES[0]);

                    const Obj& U = mU;
                    testDriver<Element>::gg(&mU, U_SPEC);

                    bdex_TestOutStream out;

                    out << U;   // testing stream-out operator here

                    const char *const OD  = out.data();
                    const int         LOD = out.length();

                    // Must reset stream for each iteration of inner loop.
                    bdex_TestInStream testInStream(OD, LOD);
                    LOOP2_ASSERT(U_SPEC, U_N, testInStream);
                    LOOP2_ASSERT(U_SPEC, U_N, !testInStream.isEmpty());

                    for (int vi = 0; SPECS[vi]; ++vi) {
                        const char *const V_SPEC = SPECS[vi];
                        const int vLen = (int) strlen(V_SPEC);

                        const Obj VV =
                              testDriver<Element>::g(V_SPEC); // control

                        if (0 == uj && veryVerbose || veryVeryVerbose) {
                            cout << "\t\t\tFor destination objects of length "
                                                        << vLen << ":\t";
                            P(V_SPEC);
                        }

                        const int Z = ui == vi; // flag indicating same values

                        for (int vj = 0; vj < NUM_EXTEND; ++vj) {
                          { testInStream.reset();
                            const int V_N = EXTEND[vj];
            //--------------^
            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, testInStream);
            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, !testInStream.isEmpty());
            //--------------v
                            Obj mV(&testAllocator);
                            testDriver<Element>::stretchRemoveAll(&mV, V_N,
                                                                  VALUES[0]);
                            const Obj& V = mV;
                            testDriver<Element>::gg(&mV, V_SPEC);

                            static int firstFew = 2 * NUM_EXTEND * NUM_EXTEND;
                            if (veryVeryVerbose||veryVerbose && firstFew > 0) {
                                cout << "\t |"; P_(U_N); P_(V_N); P_(U); P(V);
                                --firstFew;
                            }

                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, UU == U);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == V);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, Z==(U==V));

                            testInStream >> mV; // test stream-in operator here

                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, UU == U);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, UU == V);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N,  U == V);

                          }
                          }
                        }
                    }
                }
            }
        }

        if (verbose) cout << "\tOn empty and invalid streams." << endl;
        {
            static const char *SPECS[] = { // len: 0 - 5, 7, 8, 9, 15, 16, 17
                "",      "A",      "BC",     "CDE",    "DEAB",   "EABCD",
                "ABCDEAB",         "ABCDEABC",         "ABCDEABCD",
                "ABCDEABCDEABCDE", "ABCDEABCDEABCDEA", "ABCDEABCDEABCDEAB",
            0}; // Null string required as last element.

            static const int EXTEND[] = {
                0, 1, 2, 3, 4, 5, 7, 8, 9, 15, 16, 17
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            bdex_TestInStream testInStream("", 0);

            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC = SPECS[ti];
                const int curLen = (int) strlen(SPEC);

                if (verbose) cout << "\t\tFor objects of length "
                                                    << curLen << '.' << endl;
                LOOP_ASSERT(SPEC, oldLen < curLen); // strictly increasing
                oldLen = curLen;

                if (veryVerbose) { cout << "\t\t\t"; P(SPEC); }

                // Create control object X.

                Obj mX(&testAllocator); testDriver<Element>::gg(&mX, SPEC);
                const Obj& X = mX;

                LOOP_ASSERT(ti, curLen == X.length());  // same lengths

                for (int ei = 0; ei < NUM_EXTEND; ++ei) {
                  BEGIN_BDEMA_EXCEPTION_TEST {
                  BEGIN_BDEX_EXCEPTION_TEST {
                    testInStream.reset();

                    const int N = EXTEND[ei];

                    if (veryVerbose) { cout << "\t\t\t\t"; P(N); }

                    const int AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);

                    Obj t(&testAllocator);
                    testDriver<Element>::gg(&t, SPEC);
                    testDriver<Element>::stretchRemoveAll(&t, N, VALUES[0]);
                    testDriver<Element>::gg(&t, SPEC);

                    testAllocator.setAllocationLimit(AL);

                  // Ensure that reading from an empty or invalid input stream
                  // leaves the stream invalid and the target object unchanged.

                                        LOOP2_ASSERT(ti, ei, testInStream);
                                        LOOP2_ASSERT(ti, ei, X == t);

                    testInStream >> t;  LOOP2_ASSERT(ti, ei, !testInStream);
                                        LOOP2_ASSERT(ti, ei, X == t);

                    testInStream >> t;  LOOP2_ASSERT(ti, ei, !testInStream);
                                        LOOP2_ASSERT(ti, ei, X == t);

                  } END_BDEX_EXCEPTION_TEST
                  } END_BDEMA_EXCEPTION_TEST
                }
            }
        }

        if (verbose) cout <<
            "\tOn incomplete (but otherwise valid) data.  Some permutations."
            << endl;
        {
            const Obj X1 = testDriver<Element>::g("CDCD"),
                      Y1 = testDriver<Element>::g("BB"),
                      Z1 = testDriver<Element>::g("ABCDE");
            const Obj X2 = testDriver<Element>::g("ADE"),
                      Y2 = testDriver<Element>::g("CABDE"),
                      Z2 = testDriver<Element>::g("B");
            const Obj X3 = testDriver<Element>::g("DEEDDE"),
                      Y3 = testDriver<Element>::g("C"),
                      Z3 = testDriver<Element>::g("DBED");

            bdex_TestOutStream out;
            out << Y1;  const int LOD1 = out.length();
            out << Y2;  const int LOD2 = out.length();
            out << Y3;  const int LOD  = out.length();
            const char *const OD = out.data();
            if (veryVerbose) P(LOD);

            int iterationModulus = argc > 2 ? atoi(argv[2]) : 0;
            if (iterationModulus <= 0) iterationModulus = 11;
            if (verbose) P(iterationModulus);

            int iteration = 0;

            for (int i = 0; i < LOD; ++i) {
              bdex_TestInStream testInStream(OD, i);
              bdex_TestInStream& in = testInStream;
              LOOP_ASSERT(i, in);  LOOP_ASSERT(i, !i == in.isEmpty());

              if (veryVerbose) { cout << "\t\t"; P(i); }
              if (iteration % iterationModulus == 0) {
                // run test only on certain iterations.

                LOOP_ASSERT(i, in);  LOOP_ASSERT(i, !i == in.isEmpty());

                Obj t1(X1, &testAllocator),
                    t2(X2, &testAllocator),
                    t3(X3, &testAllocator);

                if (i < LOD1) {
                  BEGIN_BDEMA_EXCEPTION_TEST {
                  BEGIN_BDEX_EXCEPTION_TEST { in.reset();
                    const int AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);
                    LOOP_ASSERT(i, in);  LOOP_ASSERT(i, !i == in.isEmpty());
                    t1 = X1;
                    t2 = X2;
                    t3 = X3;

                    testAllocator.setAllocationLimit(AL);
                    in >> t1; LOOP_ASSERT(i, !in);
                                        if (0 == i) LOOP_ASSERT(i, X1 == t1);
                    in >> t2; LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, X2 == t2);
                    in >> t3; LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, X3 == t3);
                  } END_BDEX_EXCEPTION_TEST
                  } END_BDEMA_EXCEPTION_TEST
                }
                else if (i < LOD2) {
                  BEGIN_BDEMA_EXCEPTION_TEST {
                  BEGIN_BDEX_EXCEPTION_TEST { in.reset();
                    const int AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);
                    LOOP_ASSERT(i, in);  LOOP_ASSERT(i, !i == in.isEmpty());
                    t1 = X1;
                    t2 = X2;
                    t3 = X3;

                    testAllocator.setAllocationLimit(AL);
                    in >> t1; LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, Y1 == t1);
                    in >> t2; LOOP_ASSERT(i, !in);
                                     if (LOD1 == i) LOOP_ASSERT(i, X2 == t2);
                    in >> t3; LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, X3 == t3);
                  } END_BDEX_EXCEPTION_TEST
                  } END_BDEMA_EXCEPTION_TEST
                }
                else {
                  BEGIN_BDEMA_EXCEPTION_TEST {
                  BEGIN_BDEX_EXCEPTION_TEST { in.reset();
                    const int AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);
                    LOOP_ASSERT(i, in);  LOOP_ASSERT(i, !i == in.isEmpty());
                    t1 = X1;
                    t2 = X2;
                    t3 = X3;

                    testAllocator.setAllocationLimit(AL);
                    in >> t1; LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, Y1 == t1);
                    in >> t2; LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, Y2 == t2);
                    in >> t3; LOOP_ASSERT(i, !in);
                                     if (LOD2 == i) LOOP_ASSERT(i, X3 == t3);
                  } END_BDEX_EXCEPTION_TEST
                  } END_BDEMA_EXCEPTION_TEST
                }

                Obj w1(t1), w2(t2), w3(t3);  // make copies to be sure we can

                                LOOP_ASSERT(i, Z1 != w1);
                w1 = Z1;        LOOP_ASSERT(i, Z1 == w1);

                                LOOP_ASSERT(i, Z2 != w2);
                w2 = Z2;        LOOP_ASSERT(i, Z2 == w2);

                                LOOP_ASSERT(i, Z3 != w3);
                w3 = Z3;        LOOP_ASSERT(i, Z3 == w3);

                }
                ++iteration;
            }
        }

        if (verbose) cout <<
            "\tOn incomplete (but otherwise valid) data.  All permutations."
            << endl;
        {
            const Obj X1 = testDriver<Element>::g("CDCD"),
                      Y1 = testDriver<Element>::g("BB"),
                      Z1 = testDriver<Element>::g("ABCDE");
            const Obj X2 = testDriver<Element>::g("ADE"),
                      Y2 = testDriver<Element>::g("CABDE"),
                      Z2 = testDriver<Element>::g("B");
            const Obj X3 = testDriver<Element>::g("DEEDDE"),
                      Y3 = testDriver<Element>::g("C"),
                      Z3 = testDriver<Element>::g("DBED");

            bdex_TestOutStream out;
            out << Y1;  const int LOD1 = out.length();
            out << Y2;  const int LOD2 = out.length();
            out << Y3;  const int LOD  = out.length();
            const char *const OD = out.data();
            if (veryVerbose) P(LOD);

            for (int i = 0; i < LOD; ++i) {
                bdex_TestInStream testInStream(OD, i);
                bdex_TestInStream& in = testInStream;
                LOOP_ASSERT(i, in);  LOOP_ASSERT(i, !i == in.isEmpty());

                if (veryVerbose) { cout << "\t\t"; P(i); }

                in.reset();

                LOOP_ASSERT(i, in);  LOOP_ASSERT(i, !i == in.isEmpty());

                Obj t1(X1, &testAllocator),
                    t2(X2, &testAllocator),
                    t3(X3, &testAllocator);

                if (i < LOD1) {
                    in >> t1; LOOP_ASSERT(i, !in);
                                        if (0 == i) LOOP_ASSERT(i, X1 == t1);
                    in >> t2; LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, X2 == t2);
                    in >> t3; LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, X3 == t3);
                }
                else if (i < LOD2) {
                    in >> t1; LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, Y1 == t1);
                    in >> t2; LOOP_ASSERT(i, !in);
                                     if (LOD1 == i) LOOP_ASSERT(i, X2 == t2);
                    in >> t3; LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, X3 == t3);
                }
                else {
                    in >> t1; LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, Y1 == t1);
                    in >> t2; LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, Y2 == t2);
                    in >> t3; LOOP_ASSERT(i, !in);
                                     if (LOD2 == i) LOOP_ASSERT(i, X3 == t3);
                }

                Obj w1(t1), w2(t2), w3(t3);  // make copies to be sure we can

                                LOOP_ASSERT(i, Z1 != w1);
                w1 = Z1;        LOOP_ASSERT(i, Z1 == w1);

                                LOOP_ASSERT(i, Z2 != w2);
                w2 = Z2;        LOOP_ASSERT(i, Z2 == w2);

                                LOOP_ASSERT(i, Z3 != w3);
                w3 = Z3;        LOOP_ASSERT(i, Z3 == w3);

            }
        }

        if (verbose) cout << "\tOn corrupted data." << endl;

        const Obj W = testDriver<Element>::g("");            // default value
        const Obj X = testDriver<Element>::g("ABCDE");       // original value
        const Obj Y = testDriver<Element>::g("DCB");         // new value

        enum { A_LEN = 3 };
        Element a[A_LEN];
        a[0] = VALUES[3];  a[1] = VALUES[2];  a[2] = VALUES[1];

        if (verbose) cout << "\t\tGood stream (for control)." << endl;
        {
            const char version = 1;
            const int length   = 3;

            bdex_TestOutStream out;
            out.putVersion(version);
            out.putLength(length);
            putArrayStream(out, a, A_LEN);
            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);   ASSERT(W != t);    ASSERT(X == t);      ASSERT(Y != t);
            bdex_TestInStream in(OD, LOD); ASSERT(in); in >> t; ASSERT(in);
                        ASSERT(W != t);    ASSERT(X != t);      ASSERT(Y == t);
        }
        /* TBD
        if (verbose) cout << "\t\tBad version." << endl;
        {
            const char version = 0; // too small
            const int length   = 3;

            bdex_TestOutStream out;
            out.putVersion(version);
            out.putLength(length);
            putArrayStream(out, a, A_LEN);
            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);   ASSERT(W != t);    ASSERT(X == t);      ASSERT(Y != t);
            bdex_TestInStream in(OD, LOD);
            in.setSuppressVersionCheck(1);
            ASSERT(in); in >> t; ASSERT(!in);
            ASSERT(W != t);    ASSERT(X == t);      ASSERT(Y != t);
        }
        {
            const char version = 2; // too large
            const int length   = 3;

            bdex_TestOutStream out;
            out.putVersion(version);
            out.putLength(length);
            putArrayStream(out, a, A_LEN);
            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);   ASSERT(W != t);    ASSERT(X == t);      ASSERT(Y != t);
            bdex_TestInStream in(OD, LOD); ASSERT(in); in >> t; ASSERT(!in);
                        ASSERT(W != t);    ASSERT(X == t);      ASSERT(Y != t);
        }
        */
        /* TBD
        if (verbose) cout << "\t\tBad length." << endl;
        {
            const char version = 1;
            const int length   = -1; // too small

            bdex_TestOutStream out;
            out.putVersion(version);
            out.putLength(length);
            putArrayStream(out, a, A_LEN);
            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);   ASSERT(W != t);    ASSERT(X == t);      ASSERT(Y != t);
            bdex_TestInStream in(OD, LOD); ASSERT(in); in >> t; ASSERT(!in);
                        ASSERT(W != t);    ASSERT(X == t);      ASSERT(Y != t);
        }
        */

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
        //   separately without  exceptions.
        //
        //   As a separate exercise, we address 4 and 5 by constructing tests
        //   y = y for all y in T.  Using a canonical control X, we will verify
        //   that X == y before and after the assignment, again within
        //   the bdema exception testing apparatus.
        //
        //   To address concern 7, all these tests are performed on user
        //   defined types:
        //          With allocator, copyable
        //          With allocator, moveable
        //          With allocator, not moveable
        //
        // Testing:
        //   bdea_Array<T>& operator=(const bdea_Array<T>& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Assignment Operator" << endl
                          << "===========================" << endl;

        if (verbose) cout <<
              "\nTesting with allocator, copyable.\n";

        TestCases<TestAllocCopy>::testcase9(testStatus, argc,
               argv, verbose, veryVerbose, veryVeryVerbose, testAllocator);

        if (verbose) cout <<
              "\nTesting with allocator, moveable.\n";

        TestCases<TestAllocMove>::testcase9(testStatus, argc,
               argv, verbose, veryVerbose, veryVeryVerbose, testAllocator);

        if (verbose) cout <<
              "\nTesting with allocator, not moveable.\n";

        TestCases<TestAllocNoMove>::testcase9(testStatus, argc,
               argv, verbose, veryVerbose, veryVeryVerbose, testAllocator);

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTION, g:
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
        //   bdea_Array<T> g(const char *spec);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Generator Function 'g'" << endl
                          << "==============================" << endl;

        typedef bdea_Array<TestAllocNoMove> Obj;
        typedef TestAllocNoMove Element;
        const Element *VALUES;
        testDriver<Element>::getValues(&VALUES);

        static const char *SPECS[] = {
            "", "~", "A", "B", "C", "D", "E", "A~B~C~D~E", "ABCDE", "ABC~DE",
        0}; // Null string required as last element.

        if (verbose) cout <<
            "\nCompare values produced by 'g' and 'gg' for various inputs."
                                                                       << endl;
        for (int ti = 0; SPECS[ti]; ++ti) {
            const char *spec = SPECS[ti];
            if (veryVerbose) { P_(ti);  P(spec); }
            Obj mX(&testAllocator);
            testDriver<Element>::gg(&mX, spec);  const Obj& X = mX;
            if (veryVerbose) {
                cout << "\t g = " << testDriver<Element>::g(spec) << endl;
                cout << "\tgg = " << X       << endl;
            }
            const int TOTAL_BLOCKS_BEFORE = testAllocator.numBlocksTotal();
            const int IN_USE_BYTES_BEFORE = testAllocator.numBytesInUse();
            LOOP_ASSERT(ti, X == testDriver<Element>::g(spec));
            const int TOTAL_BLOCKS_AFTER = testAllocator.numBlocksTotal();
            const int IN_USE_BYTES_AFTER = testAllocator.numBytesInUse();
            LOOP_ASSERT(ti, TOTAL_BLOCKS_BEFORE == TOTAL_BLOCKS_AFTER);
            LOOP_ASSERT(ti, IN_USE_BYTES_BEFORE == IN_USE_BYTES_AFTER);
        }

        if (verbose) cout << "\nConfirm return-by-value." << endl;
        {
            const char *spec = "ABCDE";

            // compile-time fact
            ASSERT(sizeof(Obj) == sizeof testDriver<Element>::g(spec));

            Obj x(&testAllocator);                      // runtime tests
            Obj& r1 = testDriver<Element>::gg(&x, spec);
            Obj& r2 = testDriver<Element>::gg(&x, spec);
            const Obj& r3 = testDriver<Element>::g(spec);
            const Obj& r4 = testDriver<Element>::g(spec);
            ASSERT(&r2 == &r1);
            ASSERT(&x  == &r1);
            ASSERT(&r4 != &r3);
            ASSERT(&x  != &r3);
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
        //      according to which bitwise copy/move trait is applied and
        //      whether the object has an allocator.
        //
        // Plan:
        //   To address concerns 1 - 3, specify a set S of object values with
        //   substantial and varied differences, ordered by increasing length.
        //   For each value in S, initialize objects w and x, copy construct y
        //   from x and use 'operator==' to verify that both x and y
        //   subsequently have the same value as w.  Let x go out of scope and
        //   again verify that w == x.  Repeat this test with x having the same
        //   *logical* value, but perturbed so as to have potentially different
        //   internal representations.
        //
        //   To address concern 5, we will perform each of the above tests in
        //   the presence of exceptions during memory allocations using a
        //   'bdema_TestAllocator' and varying its *allocation* *limit*.
        //
        //   To address concern 6, we will repeat the above tests:
        //     - When passing in no allocator.
        //     - When passing in a null pointer: (bdema_Allocator *)0.
        //     - When passing in a test allocator (see concern 5).
        //     - Where the object is constructed entirely in static memory
        //       (using a 'bdema_BufferAllocator') and never destroyed.
        //     - After the (dynamically allocated) source object is
        //       deleted and its footprint erased (see concern 4).
        //
        //   To address concern 7, all these tests are performed on user
        //   defined types:
        //          Without allocator, copyable
        //          Without allocator, not moveable
        //          With allocator, moveable
        //
        // Testing:
        //   bdea_Array<T>(const bdea_Array<T>& original, *ba = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Copy Constructor" << endl
                          << "========================" << endl;

        if (verbose) cout <<
              "\nTesting without allocator, copyable.\n";

        TestCases<TestNoAllocCopy>::testcase7(testStatus, argc,
               argv, verbose, veryVerbose, veryVeryVerbose, 0, testAllocator);

        if (verbose) cout <<
              "\nTesting without allocator, not moveable.\n";

        TestCases<TestNoAllocNoMove>::testcase7(testStatus, argc,
               argv, verbose, veryVerbose, veryVeryVerbose, 0, testAllocator);

        if (verbose) cout <<
              "\nTesting with allocator, moveable.\n";

        TestCases<TestAllocMove>::testcase7(testStatus, argc,
               argv, verbose, veryVerbose, veryVeryVerbose, 1, testAllocator);

      } break;
      case 6: {
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
        //   or subtle differences, ordered by non-decreasing length.  Verify
        //   the correctness of 'operator==' and 'operator!=' (returning either
        //   1 or 0) using all elements (u, v) of the cross product S X S.
        //
        //   Next specify a second set S' containing a representative variety
        //   of (black-box) box values ordered by increasing (logical) length.
        //   For each value in S', construct an object x along with a sequence
        //   of similarly constructed duplicates x1, x2, ..., xN.  Attempt to
        //   affect every aspect of white-box state by altering each xi in a
        //   unique way.  Verify correctness of 'operator==' and 'operator!='
        //   by asserting that each element in { x, x1, x2, ..., xN } is
        //   equivalent to every other element.
        //
        //   The array element chosen 'TestAllocNoMove' has an allocator but
        //   is not bitwise moveable.
        //
        // Testing:
        //   operator==(const bdea_Array<T>&, const bdea_Array<T>&);
        //   operator!=(const bdea_Array<T>&, const bdea_Array<T>&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Equality Operators" << endl
                          << "==========================" << endl;

        typedef bdea_Array<TestAllocNoMove> Obj;
        typedef TestAllocNoMove Element;
        const Element *VALUES;
        testDriver<Element>::getValues(&VALUES);

        if (verbose) cout <<
            "\nCompare each pair of similar values (u, v) in S X S." << endl;
        {
            static const char *SPECS[] = {
                "",
                "A",      "B",
                "AA",     "AB",     "BB",     "BA",
                "AAA",    "BAA",    "ABA",    "AAB",
                "AAAA",   "BAAA",   "ABAA",   "AABA",   "AAAB",
                "AAAAA",  "BAAAA",  "ABAAA",  "AABAA",  "AAABA",  "AAAAB",
                "AAAAAA", "BAAAAA", "AABAAA", "AAABAA", "AAAAAB",
                "AAAAAAA",          "BAAAAAA",          "AAAAABA",
                "AAAAAAAA",         "ABAAAAAA",         "AAAAABAA",
                "AAAAAAAAA",        "AABAAAAAA",        "AAAAABAAA",
                "AAAAAAAAAA",       "AAABAAAAAA",       "AAAAABAAAA",
            0}; // Null string required as last element.

            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const U_SPEC = SPECS[ti];
                const int curLen = (int) strlen(U_SPEC);

                Obj mU(&testAllocator); testDriver<Element>::gg(&mU, U_SPEC);
                const Obj& U = mU;

                LOOP_ASSERT(ti, curLen == U.length()); // same lengths

                if (curLen != oldLen) {
                    if (verbose) cout << "\tUsing lhs objects of length "
                                  << curLen << '.' << endl;
                    LOOP_ASSERT(U_SPEC, oldLen <= curLen); // non-decreasing
                    oldLen = curLen;
                }

                if (veryVerbose) { P_(ti); P_(U_SPEC); P(U); }

                for (int tj = 0; SPECS[tj]; ++tj) {
                    const char *const V_SPEC = SPECS[tj];
                    Obj mV(&testAllocator);
                    testDriver<Element>::gg(&mV, V_SPEC);
                    const Obj& V = mV;

                    if (veryVerbose) { cout << "  "; P_(tj); P_(V_SPEC); P(V);}
                    const int isSame = ti == tj;
                    LOOP2_ASSERT(ti, tj,  isSame == (U == V));
                    LOOP2_ASSERT(ti, tj, !isSame == (U != V));
                }
            }
        }

        if (verbose) cout << "\nCompare objects of equal value having "
                             "potentially different internal state." << endl;
        {
            static const char *SPECS[] = { // len: 0 - 5, 7, 8, 9, 15, 16, 17
                "",      "A",      "AB",     "ABC",    "ABCD",   "ABCDE",
                "ABCDEAB",         "ABCDEABC",         "ABCDEABCD",
                "ABCDEABCDEABCDE", "ABCDEABCDEABCDEA", "ABCDEABCDEABCDEAB",
            0}; // Null string required as last element.

            static const int EXTEND[] = {
                0, 1, 2, 3, 4, 5, 7, 8, 9, 15, 16, 17
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC = SPECS[ti];
                const int curLen = (int) strlen(SPEC);

                if (curLen != oldLen) {
                    if (verbose) cout << "\tUsing objects having (logical) "
                                         "length " << curLen << '.' << endl;
                    LOOP_ASSERT(SPEC, oldLen < curLen); // strictly increasing
                    oldLen = curLen;
                }

                Obj mX(&testAllocator); testDriver<Element>::gg(&mX, SPEC);
                const Obj& X = mX;

                LOOP_ASSERT(ti, curLen == X.length()); // same lengths
                if (veryVerbose) { cout << "\t\t"; P_(ti); P_(SPEC); P(X)}

                for (int u = 0; u < NUM_EXTEND; ++u) {
                    const int U_N = EXTEND[u];
                    Obj mU(&testAllocator);
                    testDriver<Element>::stretchRemoveAll(&mU, U_N, VALUES[0]);
                    const Obj& U = mU;
                    testDriver<Element>::gg(&mU, SPEC);

                    if (veryVerbose) { cout << "\t\t\t"; P_(U_N); P(U)}

                    // compare canonical representation with every variation

                    LOOP2_ASSERT(SPEC, U_N, 1 == (U == X));
                    LOOP2_ASSERT(SPEC, U_N, 1 == (X == U));
                    LOOP2_ASSERT(SPEC, U_N, 0 == (U != X));
                    LOOP2_ASSERT(SPEC, U_N, 0 == (X != U));

                    for (int v = 0; v < NUM_EXTEND; ++v) {
                        const int V_N = EXTEND[v];
                        Obj mV(&testAllocator);
                        testDriver<Element>::stretchRemoveAll(&mV, V_N,
                                                              VALUES[0]);
                        const Obj& V = mV;
                        testDriver<Element>::gg(&mV, SPEC);

                        static int firstFew = 2 * NUM_EXTEND * NUM_EXTEND;
                        if (veryVeryVerbose || veryVerbose && firstFew > 0) {
                            cout << "\t| "; P_(U_N); P_(V_N); P_(U); P(V);
                            --firstFew;
                        }

                        // compare every variation with every other one

                        LOOP3_ASSERT(SPEC, U_N, V_N, 1 == (U == V));
                        LOOP3_ASSERT(SPEC, U_N, V_N, 0 == (U != V));
                    }
                }
            }
        }

      } break;
      case 5: {
        /* TBD
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR:
        //   Since the output operator is layered on basic accessors, it is
        //   sufficient to test only the output *format* (and to ensure that no
        //   additional characters are written past the terminating null).
        //
        // Plan:
        //   For each of a small representative set of object values, ordered
        //   by increasing length, use 'ostrstream' to write that object's
        //   value to two separate character buffers each with different
        //   initial values.  Compare the contents of these buffers with the
        //   literal expected output format and verify that the characters
        //   beyond the null characters are unaffected in both buffers.
        //
        //   The array element chosen 'TestAllocNoMove' has an allocator but
        //   is not bitwise moveable.
        //
        // Testing:
        //   operator<<(ostream&, const bdea_Array<T>&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Output (<<) Operator" << endl
                          << "============================" << endl;

        typedef bdea_Array<TestAllocNoMove> Obj;
        typedef TestAllocNoMove Element;
        const Element *VALUES;
        testDriver<Element>::getValues(&VALUES);

        if (verbose) cout << "\nTesting 'operator<<' (ostream)." << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                const char *d_fmt_p;    // expected output format
            } DATA[] = {
                //line  spec           output format                // ADJUST
                //----  -------------  ---------------------------------
                { L_,   "",            "[ ]"                                 },
                { L_,   "A",           "[ 1 ]"                               },
                { L_,   "BC",          "[ 2 "
                                       "3 ]"                                 },
                { L_,   "ABCDE",       "[ 1 "
                                       "2 "
                                       "3 "
                                       "4 "
                                       "5 ]"                                 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const int SIZE = 1000; // Must be big enough to hold output string.
            const char Z1 = (char) 0xff;  // Used to represent an unset char.
            const char Z2 = 0x00;  // Value 2 used to represent an unset char.

            char mCtrlBuf1[SIZE];  memset(mCtrlBuf1, Z1, SIZE);
            char mCtrlBuf2[SIZE];  memset(mCtrlBuf2, Z2, SIZE);
            const char *CTRL_BUF1 = mCtrlBuf1;
            const char *CTRL_BUF2 = mCtrlBuf2;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA;  ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const char *const FMT  = DATA[ti].d_fmt_p;
                const int curLen = (int) strlen(SPEC);

                char buf1[SIZE + epcBug], buf2[SIZE + epcBug];
                memcpy(buf1, CTRL_BUF1, SIZE); // Preset buf1 to Z1 values.
                memcpy(buf2, CTRL_BUF2, SIZE); // Preset buf2 to Z2 values.

                Obj mX(&testAllocator);
                const Obj& X = testDriver<Element>::gg(&mX, SPEC);
                LOOP_ASSERT(ti, curLen == X.length()); // same lengths

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton objects of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen < curLen); // strictly increasing
                    oldLen = curLen;
                }

                if (verbose) cout << "\t\tSpec = \"" << SPEC << '"' << endl;
                if (veryVerbose) cout << "EXPECTED FORMAT:" << endl<<FMT<<endl;
                ostrstream out1(buf1, SIZE);  out1 << X << ends;
                ostrstream out2(buf2, SIZE);  out2 << X << ends;
                if (veryVerbose) cout << "ACTUAL FORMAT:" << endl<<buf1<<endl;

                const int SZ = (int) strlen(FMT) + 1;
                const int REST = SIZE - SZ;
                LOOP_ASSERT(ti, SZ < SIZE);  // Check buffer is large enough.
                LOOP_ASSERT(ti, Z1 == buf1[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(ti, Z2 == buf2[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(ti, 0 == memcmp(buf1, FMT, SZ));
                LOOP_ASSERT(ti, 0 == memcmp(buf2, FMT, SZ));
                LOOP_ASSERT(ti, 0 == memcmp(buf1 + SZ, CTRL_BUF1 + SZ, REST));
                LOOP_ASSERT(ti, 0 == memcmp(buf2 + SZ, CTRL_BUF2 + SZ, REST));
            }
        }
        */
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING BASIC ACCESSORS:
        //   Having implemented an effective generation mechanism, we now
        //   would like to test thoroughly the basic accessor functions
        //     - length() const
        //     - operator[](int) const
        //   Also, we want to ensure that various internal state
        //   representations for a given value produce identical results.
        //
        // Plan:
        //   Specify a set S of representative object values ordered by
        //   increasing length.  For each value w in S, initialize a newly
        //   constructed object x with w using 'gg' and verify that each basic
        //   accessor returns the expected result.  Reinitialize and repeat
        //   the same test on an existing object y after perturbing y so as to
        //   achieve an internal state representation of w that is potentially
        //   different from that of x.
        //
        //   The array element chosen 'TestAllocNoMove' has an allocator but
        //   is not bitwise moveable.
        //
        // Testing:
        //   int length() const;
        //   const int& operator[](int index) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Basic Accessors" << endl
                          << "=======================" << endl;

        typedef bdea_Array<TestAllocNoMove> Obj;
        typedef TestAllocNoMove Element;
        const Element *VALUES;
        testDriver<Element>::getValues(&VALUES);

        if (verbose) cout << "\nTesting 'length' & 'operator[]'" << endl;
        {
            const int SZ = 10;
            static const struct {
                int         d_lineNum;          // source line number
                const char *d_spec_p;           // specification string
                int         d_length;           // expected length
                int         d_elements[SZ];     // expected element values
                                                // > 0
            } DATA[] = {
                //line  spec            length  elements
                //----  --------------  ------  ------------------------
                { L_,   "",             0,      { 0 }                   },
                { L_,   "A",            1,      { 1 }                   },
                { L_,   "B",            1,      { 2 }                   },
                { L_,   "AB",           2,      { 1, 2 }                },
                { L_,   "BC",           2,      { 2, 3 }                },
                { L_,   "BCA",          3,      { 2, 3, 1 }             },
                { L_,   "CAB",          3,      { 3, 1, 2 }             },
                { L_,   "CDAB",         4,      { 3, 4, 1, 2 }          },
                { L_,   "DABC",         4,      { 4, 1, 2, 3 }          },
                { L_,   "ABCDE",        5,      { 1, 2, 3, 4, 5 }       },
                { L_,   "EDCBA",        5,      { 5, 4, 3, 2, 1 }       },
                { L_,   "ABCDEAB",      7,      { 1, 2, 3, 4, 5,
                                                  1, 2 }                },
                { L_,   "BACDEABC",     8,      { 2, 1, 3, 4, 5,
                                                  1, 2, 3 }             },
                { L_,   "CBADEABCD",    9,      { 3, 2, 1, 4, 5,
                                                  1, 2, 3, 4 }          },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            Obj mY(&testAllocator);  // object with extended internal capacity
            const int EXTEND = 50;
            testDriver<Element>::stretch(&mY, EXTEND, VALUES[0]);
            ASSERT(mY.length());
            if (veryVerbose) cout << "\tEXTEND = " << EXTEND << endl;

            int oldLen= -1;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int LENGTH       = DATA[ti].d_length;
                const int *const e = DATA[ti].d_elements;
                const int curLen = LENGTH;

                Obj mX(&testAllocator);

                // canonical organization
                const Obj& X = testDriver<Element>::gg(&mX, SPEC);
                mY.removeAll();
                // has extended capacity
                const Obj& Y = testDriver<Element>::gg(&mY, SPEC);

                LOOP_ASSERT(ti, curLen == X.length()); // same lengths

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton objects of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                if (verbose) cout << "\t\tSpec = \"" << SPEC << '"' << endl;
                if (veryVerbose) { cout << "\t\t\t"; P(X);
                                   cout << "\t\t\t"; P(Y); }

                LOOP_ASSERT(LINE, LENGTH == X.length());
                LOOP_ASSERT(LINE, LENGTH == Y.length());
                int i;
                for (i = 0; i < LENGTH; ++i) {
                    LOOP2_ASSERT(LINE, i, e[i] > 0);
                    LOOP2_ASSERT(LINE, i, VALUES[e[i] - 1] == X[i]);
                    LOOP2_ASSERT(LINE, i, VALUES[e[i] - 1] == Y[i]);
                }
                for (; i < SZ; ++i) {
                    LOOP2_ASSERT(LINE, i, 0 == e[i]);
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
        //    - 'stretch'          Tested separately to observe stretch occurs.
        //    - 'stretchRemoveAll' Deliberately implemented using 'stretch'.
        //
        //   Finally we want to make sure that we can rationalize the internal
        //   memory management with respect to the primary manipulators (i.e.,
        //   precisely when new blocks are allocated and deallocated).
        //
        // Plan:
        //   For each of an enumerated sequence of 'spec' values, ordered by
        //   increasing 'spec' length, use the primitive generator function
        //   'gg' to set the state of a newly created object.  Verify that 'gg'
        //   returns a valid reference to the modified argument object and,
        //   using basic accessors, that the value of the object is as
        //   expected.  Repeat the test for a longer 'spec' generated by
        //   prepending a string ending in a '~' character (denoting
        //   'removeAll').  Note that we are testing the parser only; the
        //   primary manipulators are already assumed to work.
        //
        //   To verify that the stretching functions work as expected (and to
        //   cross-check that internal memory is being managed as intended),
        //   create a depth-ordered enumeration of initial values and sizes by
        //   which to extend the initial value.  Record as expected values the
        //   total number of memory blocks allocated during the first and
        //   second modifications of each object.  For each test vector,
        //   construct two identical objects X and Y and bring each to the
        //   initial state.  Assert that the memory allocation for the two
        //   operations are identical and consistent with the first expected
        //   value.  Next apply the 'stretch' and 'stretchRemoveAll' functions
        //   to X and Y (respectively) and again compare the memory allocation
        //   characteristics for the two functions.  Note that we will track
        //   the *total* number of *blocks* allocated as well as the *current*
        //   number of *bytes* in use -- this to measure different aspects of
        //   operation while remaining insensitive to the array 'Element' size.
        //
        //   In order to be able to measure the internal memory used by the
        //   array, the array element chosen 'TestAllocCopy' is an object that
        //   does not allocate internal memory.
        //
        // Testing:
        //   bdea_Array<T>& gg<T>(bdea_Array<T>* object, const char *spec);
        //   int ggg<T>(bdea_Array<T> *object, const char *spec, int vF = 1);
        //   void stretch(Obj *object, int size);
        //   void stretchRemoveAll(Obj *object, int size);
        //   CONCERN: Is the internal memory organization behaving as intended?
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "Testing Primitive Generator Function 'gg'" << endl
                 << "=========================================" << endl;

        typedef bdea_Array<TestAllocCopy> Obj;
        typedef TestAllocCopy Element;
        const Element *VALUES;
        testDriver<Element>::getValues(&VALUES);

        if (verbose) cout << "\nTesting generator on valid specs." << endl;
        {
            const int SZ = 10;
            static const struct {
                int         d_lineNum;          // source line number
                const char *d_spec_p;           // specification string
                int         d_length;           // expected length
                int         d_elements[SZ];     // expected element values
                                                // > 0
            } DATA[] = {
                //line  spec            length  elements
                //----  --------------  ------  ------------------------
                { L_,   "",             0,      { 0 }                   },

                { L_,   "A",            1,      { 1 }                   },
                { L_,   "B",            1,      { 2 }                   },
                { L_,   "~",            0,      { 0 }                   },

                { L_,   "CD",           2,      { 3, 4 }                },
                { L_,   "E~",           0,      { 0 }                   },
                { L_,   "~E",           1,      { 5 }                   },
                { L_,   "~~",           0,      { 0 }                   },

                { L_,   "ABC",          3,      { 1, 2, 3 }             },
                { L_,   "~BC",          2,      { 2, 3 }                },
                { L_,   "A~C",          1,      { 3 }                   },
                { L_,   "AB~",          0,      { 0 }                   },
                { L_,   "~~C",          1,      { 3 }                   },
                { L_,   "~B~",          0,      { 0 }                   },
                { L_,   "A~~",          0,      { 0 }                   },
                { L_,   "~~~",          0,      { 0 }                   },

                { L_,   "ABCD",         4,      { 1, 2, 3, 4 }  },
                { L_,   "~BCD",         3,      { 2, 3, 4 }             },
                { L_,   "A~CD",         2,      { 3, 4 }                },
                { L_,   "AB~D",         1,      { 4 }                   },
                { L_,   "ABC~",         0,      { 0 }                   },

                { L_,   "ABCDE",        5,      { 1, 2, 3, 4, 5 }       },
                { L_,   "~BCDE",        4,      { 2, 3, 4, 5 }  },
                { L_,   "AB~DE",        2,      { 4, 5 }                },
                { L_,   "ABCD~",        0,      { 0 }                   },
                { L_,   "A~C~E",        1,      { 5 }                   },
                { L_,   "~B~D~",        0,      { 0 }                   },

                { L_,   "~CBA~~ABCDE",  5,      { 1, 2, 3, 4, 5 }       },

                { L_,   "ABCDE~CDEC~E", 1,      { 5 }                   },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int LENGTH       = DATA[ti].d_length;
                const int *const e = DATA[ti].d_elements;
                const int curLen = (int) strlen(SPEC);

                Obj mX(&testAllocator);
                const Obj& X =
                     testDriver<Element>::gg(&mX, SPEC);  // original spec

                static const char *const MORE_SPEC = "~ABCDEABCDEABCDEABCDE~";
                char buf[100]; strcpy(buf, MORE_SPEC); strcat(buf, SPEC);

                Obj mY(&testAllocator);
                const Obj& Y =
                     testDriver<Element>::gg(&mY, buf);   // extended spec

                if (curLen != oldLen) {
                    if (verbose) cout << "\tof length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                if (veryVerbose) {
                    cout << "\t\t   Spec = \"" << SPEC << '"' << endl;
                    cout << "\t\tBigSpec = \"" << buf << '"' << endl;
                    cout << "\t\t\t"; P(X);
                    cout << "\t\t\t"; P(Y);
                }

                LOOP_ASSERT(LINE, LENGTH == X.length());
                LOOP_ASSERT(LINE, LENGTH == Y.length());
                for (int i = 0; i < LENGTH; ++i) {
                    LOOP2_ASSERT(LINE, i, e[i] > 0);
                    LOOP2_ASSERT(LINE, i, VALUES[e[i] - 1] == X[i]);
                    LOOP2_ASSERT(LINE, i, VALUES[e[i] - 1] == Y[i]);
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
                //line  spec            index
                //----  -------------   -----
                { L_,   "",             -1,     }, // control

                { L_,   "~",            -1,     }, // control
                { L_,   " ",             0,     },
                { L_,   ".",             0,     },
                { L_,   "F",             0,     },

                { L_,   "AE",           -1,     }, // control
                { L_,   "aE",            0,     },
                { L_,   "Ae",            1,     },
                { L_,   ".~",            0,     },
                { L_,   "~!",            1,     },
                { L_,   "  ",            0,     },

                { L_,   "ABC",          -1,     }, // control
                { L_,   " BC",           0,     },
                { L_,   "A C",           1,     },
                { L_,   "AB ",           2,     },
                { L_,   "?#:",           0,     },
                { L_,   "   ",           0,     },

                { L_,   "ABCDE",        -1,     }, // control
                { L_,   "aBCDE",         0,     },
                { L_,   "ABcDE",         2,     },
                { L_,   "ABCDe",         4,     },
                { L_,   "AbCdE",         1,     },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int INDEX        = DATA[ti].d_index;
                const int curLen       = (int) strlen(SPEC);

                Obj mX(&testAllocator);

                if (curLen != oldLen) {
                    if (verbose) cout << "\tof length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                if (veryVerbose) cout <<
                    "\t\tSpec = \"" << SPEC << '"' << endl;

                int result = testDriver<Element>::ggg(&mX, SPEC, veryVerbose);

                LOOP_ASSERT(LINE, INDEX == result);
            }
        }

        if (verbose) cout <<
            "\nTesting 'stretch' and 'stretchRemoveAll'." << endl;
        {
            static const struct {
                int         d_lineNum;       // source line number
                const char *d_spec_p;        // specification string
                int         d_size;          // amount to grow (also length)
                int         d_firstResize;   // total blocks allocated
                int         d_secondResize;  // total blocks allocated

                // Note: total blocks (first/second Resize) and whether or not
                // 'removeAll' deallocates memory depends on 'Element' type.

            } DATA[] = {
                //line  spec            size    firstResize     secondResize
                //----  -------------   ----    -----------     ------------
                { L_,   "",             0,      0,              0       },

                { L_,   "",             1,      0,              0       },
                { L_,   "A",            0,      0,              0       },

                { L_,   "",             2,      0,              1       },
                { L_,   "A",            1,      0,              1       },
                { L_,   "AB",           0,      1,              0       },

                { L_,   "",             3,      0,              2       },
                { L_,   "A",            2,      0,              2       },
                { L_,   "AB",           1,      1,              1       },
                { L_,   "ABC",          0,      2,              0       },

                { L_,   "",             4,      0,              2       },
                { L_,   "A",            3,      0,              2       },
                { L_,   "AB",           2,      1,              1       },
                { L_,   "ABC",          1,      2,              0       },
                { L_,   "ABCD",         0,      2,              0       },

                { L_,   "",             5,      0,              3       },
                { L_,   "A",            4,      0,              3       },
                { L_,   "AB",           3,      1,              2       },
                { L_,   "ABC",          2,      2,              1       },
                { L_,   "ABCD",         1,      2,              1       },
                { L_,   "ABCDE",        0,      3,              0       },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldDepth = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int size         = DATA[ti].d_size;
                const int firstResize  = DATA[ti].d_firstResize;
                const int secondResize = DATA[ti].d_secondResize;
                const int curLen       = (int) strlen(SPEC);
                const int curDepth     = curLen + size;

                Obj mX(&testAllocator);
                const Obj& X = mX;
                Obj mY(&testAllocator);
                const Obj& Y = mY;

                if (curDepth != oldDepth) {
                    if (verbose) cout << "\ton test vectors of depth "
                                      << curDepth << '.' << endl;
                    LOOP_ASSERT(LINE, oldDepth <= curDepth); // non-decreasing
                    oldDepth = curDepth;
                }

                if (veryVerbose) {
                    cout << "\t\t"; P_(SPEC); P(size);
                    P_(firstResize); P_(secondResize);
                    P_(curLen);      P(curDepth);
                }

                // Create identical objects using the gg function.
                {
                    int blocks1A = testAllocator.numBlocksTotal();
                    int bytes1A = testAllocator.numBytesInUse();

                    testDriver<Element>::gg(&mX, SPEC);

                    int blocks2A = testAllocator.numBlocksTotal();
                    int bytes2A = testAllocator.numBytesInUse();

                    testDriver<Element>::gg(&mY, SPEC);

                    int blocks3A = testAllocator.numBlocksTotal();
                    int bytes3A = testAllocator.numBytesInUse();

                    int blocks12A = blocks2A - blocks1A;
                    int bytes12A = bytes2A - bytes1A;

                    int blocks23A = blocks3A - blocks2A;
                    int bytes23A = bytes3A - bytes2A;

                    if (veryVerbose) { P_(bytes12A);  P_(bytes23A);
                                       P_(blocks12A); P(blocks23A); }

                    LOOP_ASSERT(LINE, curLen == X.length()); // same lengths
                    LOOP_ASSERT(LINE, curLen == Y.length()); // same lengths

                    LOOP_ASSERT(LINE, firstResize == blocks12A);

                    LOOP_ASSERT(LINE, blocks12A == blocks23A);
                    LOOP_ASSERT(LINE, bytes12A == bytes23A);
                }

                // Apply both functions under test to the respective objects.
                {

                    int blocks1B = testAllocator.numBlocksTotal();
                    int bytes1B = testAllocator.numBytesInUse();

                    testDriver<Element>::stretch(&mX, size, VALUES[0]);

                    int blocks2B = testAllocator.numBlocksTotal();
                    int bytes2B = testAllocator.numBytesInUse();

                    testDriver<Element>::stretchRemoveAll(&mY, size,
                                                          VALUES[0]);

                    int blocks3B = testAllocator.numBlocksTotal();
                    int bytes3B = testAllocator.numBytesInUse();

                    int blocks12B = blocks2B - blocks1B;
                    int bytes12B = bytes2B - bytes1B;

                    int blocks23B = blocks3B - blocks2B;
                    int bytes23B = bytes3B - bytes2B;

                    if (veryVerbose) { P_(bytes12B);  P_(bytes23B);
                                       P_(blocks12B); P(blocks23B); }

                    LOOP_ASSERT(LINE, curDepth == X.length());
                    LOOP_ASSERT(LINE,        0 == Y.length());

                    LOOP_ASSERT(LINE, secondResize == blocks12B);

                    LOOP_ASSERT(LINE, blocks12B == blocks23B); // Always true.

                    LOOP_ASSERT(LINE, bytes12B == bytes23B);   // True for POD;
                }                                              // else > or >=.
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS (BOOTSTRAP):
        //   The basic concern is that the default constructor, the destructor,
        //   and, under normal conditions (i.e., no aliasing), the primary
        //   manipulators
        //      - append                (black-box)
        //      - removeAll             (white-box)
        //   operate as expected.  We have the following specific concerns:
        //    1. The default Constructor
        //        1a. creates the correct initial value.
        //        1b. is exception neutral with respect to memory allocation.
        //        1c. has the internal memory management system hooked up
        //              properly so that *all* internally allocated memory
        //              draws from the same user-supplied allocator whenever
        //              one is specified.
        //        1d. operates correctly with different user defined types
        //              including those with bitwise copyable or bitwise
        //              moveable trait and those with and without bdema
        //              allocators.
        //
        //    2. The destructor properly deallocates all allocated memory to
        //         its corresponding allocator from any attainable state.
        //    3. 'append'
        //        3a. produces the expected value.
        //        3b. increases capacity as needed.
        //        3c. maintains valid internal state.
        //        3d. is exception neutral with respect to memory allocation.
        //    4. 'removeAll'
        //        4a. produces the expected value (empty).
        //        4b. properly destroys each contained element value.
        //        4c. maintains valid internal state.
        //        4d. does not allocate memory.
        //
        // Plan:
        //   To address concerns 1a - 1c, create an object using the default
        //   constructor:
        //    - With and without passing in an allocator.
        //    - In the presence of exceptions during memory allocations using
        //        a 'bdema_TestAllocator' and varying its *allocation* *limit*.
        //    - Where the object is constructed entirely in static memory
        //        (using a 'bdema_BufferAllocator') and never destroyed.
        //
        //   To address concerns 3a - 3c, construct a series of independent
        //   objects, ordered by increasing length.  In each test, allow the
        //   object to leave scope without further modification, so that the
        //   destructor asserts internal object invariants appropriately.
        //   After the final append operation in each test, use the (untested)
        //   basic accessors to cross-check the value of the object
        //   and the 'bdema_TestAllocator' to confirm whether a resize has
        //   occurred.
        //
        //   To address concerns 4a-4c, construct a similar test, replacing
        //   'append' with 'removeAll'; this time, however, use the test
        //   allocator to record *numBlocksInUse* rather than *numBlocksTotal*.
        //
        //   To address concerns 2, 3d, 4d, create a small "area" test that
        //   exercises the construction and destruction of objects of various
        //   lengths and capacities in the presence of memory allocation
        //   exceptions.  Two separate tests will be performed.
        //
        //   Let S be the sequence of integers { 0 .. N - 1 }.
        //      (1) for each i in S, use the default constructor and 'append'
        //          to create an instance of length i, confirm its value (using
        //           basic accessors), and let it leave scope.
        //      (2) for each (i, j) in S X S, use 'append' to create an
        //          instance of length i, use 'removeAll' to erase its value
        //          and confirm (with 'length'), use append to set the instance
        //          to a value of length j, verify the value, and allow the
        //          instance to leave scope.
        //
        //   The first test acts as a "control" in that 'removeAll' is not
        //   called; if only the second test produces an error, we know that
        //   'removeAll' is to blame.  We will rely on 'bdema_TestAllocator'
        //   and purify to address concern 2, and on the object invariant
        //   assertions in the destructor to address concerns 3d and 4d.
        //
        //   To address concern 1d, all these tests are performed on user
        //   defined types:
        //          Without allocator, not moveable
        //          With allocator, copyable
        //          With allocator, moveable
        //          With allocator, not moveable
        //
        // Testing:
        //   bdea_Array<T>(bdema_Allocator *ba);
        //   ~bdea_Array<T>();
        //   BOOTSTRAP: void append(bdet_String& item); // no aliasing
        //   void removeAll();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Primary Manipulators" << endl
                          << "============================" << endl;

        if (verbose) cout <<
              "\nTesting without allocator, not moveable.\n";

        TestCases<TestNoAllocNoMove>::testcase2(testStatus, argc,
               argv, verbose, veryVerbose, veryVeryVerbose, 1, testAllocator);

        if (verbose) cout <<
              "\nTesting with allocator, copyable.\n";

        TestCases<TestAllocCopy>::testcase2(testStatus, argc,
               argv, verbose, veryVerbose, veryVeryVerbose, 0, testAllocator);

        if (verbose) cout <<
              "\nTesting with allocator, moveable.\n";

        TestCases<TestAllocMove>::testcase2(testStatus, argc,
               argv, verbose, veryVerbose, veryVeryVerbose, 1, testAllocator);

        if (verbose) cout <<
              "\nTesting with allocator, not moveable.\n";

        TestCases<TestAllocNoMove>::testcase2(testStatus, argc,
               argv, verbose, veryVerbose, veryVeryVerbose, 1, testAllocator);

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   We want to exercise basic value-semantic functionality.  In
        //   particular we want to demonstrate a base-line level of correct
        //   operation of the following methods and operators:
        //      - default and copy constructors (and also the destructor)
        //      - the assignment operator (including aliasing)
        //      - equality operators: 'operator==' and 'operator!='
        //      - the output operator: 'operator<<'
        //      - primary manipulators: 'append' and 'removeAll' methods
        //      - basic accessors: 'length' and 'operator[]'
        //   In addition we would like to exercise objects with potentially
        //   different internal organizations representing the same value.
        //
        // Plan:
        //   Create four objects using both the default and copy constructors.
        //   The array element is a user defined type with an allocator
        //   but not bitwise moveable.  Exercise these objects using primary
        //   manipulators, basic accessors, equality operators, and the
        //   assignment operator.  Invoke the primary manipulator [1&5], copy
        //   constructor [2&8], and assignment operator [9&10] in situations
        //   where the internal data (i) does *not* and (ii) *does* have to
        //   resize.  Try aliasing with assignment for a non-empty instance
        //   [11] and allow the result to leave scope, enabling the destructor
        //   to assert internal object invariants.  Display object values
        //   frequently in verbose mode:
        //
        // 1. Create an object x1 (default ctor).       { x1: }
        // 2. Create a second object x2 (copy from x1). { x1: x2: }
        // 3. Append an element value A to x1).         { x1:A x2: }
        // 4. Append the same element value A to x2).   { x1:A x2:A }
        // 5. Append another element value B to x2).    { x1:A x2:AB }
        // 6. Remove all elements from x1.              { x1: x2:AB }
        // 7. Create a third object x3 (default ctor).  { x1: x2:AB x3: }
        // 8. Create a fourth object x4 (copy of x2).   { x1: x2:AB x3: x4:AB }
        // 9. Assign x2 = x1 (non-empty becomes empty). { x1: x2: x3: x4:AB }
        // 10. Assign x3 = x4 (empty becomes non-empty).{ x1: x2: x3:AB x4:AB }
        // 11. Assign x4 = x4 (aliasing).               { x1: x2: x3:AB x4:AB }
        //
        // Testing:
        //   This "test" *exercises* basic functionality, but *tests* nothing.

        // --------------------------------------------------------------------

        typedef bdea_Array<TestAllocNoMove> Obj;
        typedef TestAllocNoMove Element;
        const Element *VALUES;
        testDriver<Element>::getValues(&VALUES);

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 1. Create an object x1 (default ctor)."
                             "\t\t\t{ x1: }" << endl;
        Obj mX1(&testAllocator);  const Obj& X1 = mX1;
        if (verbose) { cout << '\t';  P(X1); }

        if (verbose) cout <<
            "\ta. Check initial state of x1." << endl;
        ASSERT(0 == X1.length());

        if (verbose) cout <<
            "\tb. Try equality operators: x1 <op> x1." << endl;
        ASSERT(X1 == X1 == 1);          ASSERT(X1 != X1 == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 2. Create a second object x2 (copy from x1)."
                             "\t\t{ x1: x2: }" << endl;
        Obj mX2(X1, &testAllocator);  const Obj& X2 = mX2;
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout <<
            "\ta. Check the initial state of x2." << endl;
        ASSERT(0 == X2.length());

        if (verbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT(X2 == X1 == 1);          ASSERT(X2 != X1 == 0);
        ASSERT(X2 == X2 == 1);          ASSERT(X2 != X2 == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 3. Append an element value A to x1)."
                             "\t\t\t{ x1:A x2: }" << endl;
        mX1.append(VALUES[0]);
        if (verbose) { cout << '\t';  P(X1); }

        if (verbose) cout <<
            "\ta. Check new state of x1." << endl;
        ASSERT(1 == X1.length());
        ASSERT(VALUES[0] == X1[0]);

        if (verbose) cout <<
            "\tb. Try equality operators: x1 <op> x1, x2." << endl;
        ASSERT(X1 == X1 == 1);          ASSERT(X1 != X1 == 0);
        ASSERT(X1 == X2 == 0);          ASSERT(X1 != X2 == 1);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 4. Append the same element value A to x2)."
                             "\t\t{ x1:A x2:A }" << endl;
        mX2.append(VALUES[0]);
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout <<
            "\ta. Check new state of x2." << endl;
        ASSERT(1 == X2.length());
        ASSERT(VALUES[0] == X2[0]);

        if (verbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT(X2 == X1 == 1);          ASSERT(X2 != X1 == 0);
        ASSERT(X2 == X2 == 1);          ASSERT(X2 != X2 == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 5. Append another element value B to x2)."
                             "\t\t{ x1:A x2:AB }" << endl;
        mX2.append(VALUES[1]);
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout <<
            "\ta. Check new state of x2." << endl;
        ASSERT(2 == X2.length());
        ASSERT(VALUES[0] == X2[0]);
        ASSERT(VALUES[1] == X2[1]);

        if (verbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT(X2 == X1 == 0);          ASSERT(X2 != X1 == 1);
        ASSERT(X2 == X2 == 1);          ASSERT(X2 != X2 == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 6. Remove all elements from x1."
                             "\t\t\t{ x1: x2:AB }" << endl;
        mX1.removeAll();
        if (verbose) { cout << '\t';  P(X1); }

        if (verbose) cout <<
            "\ta. Check new state of x1." << endl;
        ASSERT(0 == X1.length());

        if (verbose) cout <<
            "\tb. Try equality operators: x1 <op> x1, x2." << endl;
        ASSERT(X1 == X1 == 1);          ASSERT(X1 != X1 == 0);
        ASSERT(X1 == X2 == 0);          ASSERT(X1 != X2 == 1);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 7. Create a third object x3 (default ctor)."
                             "\t\t{ x1: x2:AB x3: }" << endl;

        Obj mX3(&testAllocator);  const Obj& X3 = mX3;
        if (verbose) { cout << '\t';  P(X3); }

        if (verbose) cout <<
            "\ta. Check new state of x3." << endl;
        ASSERT(0 == X3.length());

        if (verbose) cout <<
            "\tb. Try equality operators: x3 <op> x1, x2, x3." << endl;
        ASSERT(X3 == X1 == 1);          ASSERT(X3 != X1 == 0);
        ASSERT(X3 == X2 == 0);          ASSERT(X3 != X2 == 1);
        ASSERT(X3 == X3 == 1);          ASSERT(X3 != X3 == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 8. Create a fourth object x4 (copy of x2)."
                             "\t\t{ x1: x2:AB x3: x4:AB }" << endl;

        Obj mX4(X2, &testAllocator);  const Obj& X4 = mX4;
        if (verbose) { cout << '\t';  P(X4); }

        if (verbose) cout <<
            "\ta. Check new state of x4." << endl;

        ASSERT(2 == X4.length());
        ASSERT(VALUES[0] == X4[0]);
        ASSERT(VALUES[1] == X4[1]);

        if (verbose) cout <<
            "\tb. Try equality operators: x4 <op> x1, x2, x3, x4." << endl;
        ASSERT(X4 == X1 == 0);          ASSERT(X4 != X1 == 1);
        ASSERT(X4 == X2 == 1);          ASSERT(X4 != X2 == 0);
        ASSERT(X4 == X3 == 0);          ASSERT(X4 != X3 == 1);
        ASSERT(X4 == X4 == 1);          ASSERT(X4 != X4 == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 9. Assign x2 = x1 (non-empty becomes empty)."
                             "\t\t{ x1: x2: x3: x4:AB }" << endl;

        mX2 = X1;
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout <<
            "\ta. Check new state of x2." << endl;
        ASSERT(0 == X2.length());

        if (verbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(X2 == X1 == 1);          ASSERT(X2 != X1 == 0);
        ASSERT(X2 == X2 == 1);          ASSERT(X2 != X2 == 0);
        ASSERT(X2 == X3 == 1);          ASSERT(X2 != X3 == 0);
        ASSERT(X2 == X4 == 0);          ASSERT(X2 != X4 == 1);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n10. Assign x3 = x4 (empty becomes non-empty)."
                             "\t\t{ x1: x2: x3:AB x4:AB }" << endl;

        mX3 = X4;
        if (verbose) { cout << '\t';  P(X3); }

        if (verbose) cout <<
            "\ta. Check new state of x3." << endl;
        ASSERT(2 == X3.length());
        ASSERT(VALUES[0] == X3[0]);
        ASSERT(VALUES[1] == X3[1]);

        if (verbose) cout <<
            "\tb. Try equality operators: x3 <op> x1, x2, x3, x4." << endl;
        ASSERT(X3 == X1 == 0);          ASSERT(X3 != X1 == 1);
        ASSERT(X3 == X2 == 0);          ASSERT(X3 != X2 == 1);
        ASSERT(X3 == X3 == 1);          ASSERT(X3 != X3 == 0);
        ASSERT(X3 == X4 == 1);          ASSERT(X3 != X4 == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n11. Assign x4 = x4 (aliasing)."
                             "\t\t\t\t{ x1: x2: x3:AB x4:AB }" << endl;

        mX4 = X4;
        if (verbose) { cout << '\t';  P(X4); }

        if (verbose) cout <<
            "\ta. Check new state of x4." << endl;
        ASSERT(2 == X4.length());
        ASSERT(VALUES[0] == X4[0]);
        ASSERT(VALUES[1] == X4[1]);

        if (verbose) cout <<
            "\tb. Try equality operators: x4 <op> x1, x2, x3, x4." << endl;
        ASSERT(X4 == X1 == 0);          ASSERT(X4 != X1 == 1);
        ASSERT(X4 == X2 == 0);          ASSERT(X4 != X2 == 1);
        ASSERT(X4 == X3 == 1);          ASSERT(X4 != X3 == 0);
        ASSERT(X4 == X4 == 1);          ASSERT(X4 != X4 == 0);

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
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
