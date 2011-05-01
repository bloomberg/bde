// bdea_bitarray.t.cpp               -*-C++-*-

#include <bdea_bitarray.h>

#include <bdema_bufferedsequentialallocator.h>  // for testing only

#include <bdex_byteinstream.h>                  // for testing only
#include <bdex_byteoutstream.h>                 // for testing only
#include <bdex_instreamfunctions.h>             // for testing only
#include <bdex_outstreamfunctions.h>            // for testing only
#include <bdex_testinstream.h>                  // for testing only
#include <bdex_testinstreamexception.h>         // for testing only
#include <bdex_testoutstream.h>                 // for testing only

#include <bslma_defaultallocatorguard.h>        // for testing only
#include <bslma_testallocator.h>                // for testing only
#include <bslma_testallocatorexception.h>       // for testing only

#include <bsls_platform.h>                      // for testing only
#include <bsls_platformutil.h>                  // for testing only

#include <bsl_iostream.h>
#include <bsl_new.h>         // placement syntax
#include <bsl_strstream.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // strlen(), memset(), memcpy(), memcmp()

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

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
//-----------------------------------------------------------------------------
// [ 2] bdea_BitArray(bslma_Allocator *ba = 0);
// [11] bdea_BitArray(int iLen, *ba = 0);
// [11] bdea_BitArray(int iLen, bool iVal, *ba = 0);
// [17] bdea_BitArray(const InitialCapacity& ne, *ba = 0);
// [17] bdea_BitArray(const InitialCapacity& ne, int iLen, *ba = 0);
// [17] bdea_BitArray(const InitialCapacity& ne, int iLen, bool val, *ba = 0);
// [ 7] bdea_BitArray(const bdea_BitArray& original, *ba = 0);
// [ 2] ~bdea_BitArray();
// [ 9] bdea_BitArray& operator=(const bdea_BitArray& rhs);
// [ 9] void swap(bdea_BitArray& other);
// [27] bdea_BitArray& operator&=(const bdea_BitArray& rhs);
// [27] bdea_BitArray& operator|=(const bdea_BitArray& rhs);
// [27] bdea_BitArray& operator^=(const bdea_BitArray& rhs);
// [27] bdea_BitArray& operator-=(const bdea_BitArray& rhs);
// [21] bdea_BitArray& operator<<=(int numBits);
// [21] bdea_BitArray& operator>>=(int numBits);
// [22] void andEqual(int index, bool value);
// [26] void andEqual(dstIndex, srcArray, srcIndex, numBits);
// [23] void orEqual(int index, bool value);
// [26] void orEqual(dstIndex, srcArray, srcIndex, numBits);
// [24] void xorEqual(int index, bool value);
// [26] void xorEqual(dstIndex, srcArray, srcIndex, numBits);
// [25] void minusEqual(int index, bool value);
// [26] void minusEqual(dstIndex, srcArray, srcIndex, numBits);
// [20] void rotateLeft(int numBits);
// [20] void rotateRight(int numBits);
// [18] void setAll1();
// [18] void set1(int index);
// [18] void set1(int index, int numBits);
// [18] void setAll0();
// [18] void set0(int index);
// [18] void set0(int index, int numBits);
// [18] void set(int index, bool value);
// [19] void toggleAll();
// [19] void toggle(int index);
// [19] void toggle(int index, int numBits);
// [13] void append(bool value);
// [13] void append(const bdea_BitArray& sa);
// [13] void append(const bdea_BitArray& sa, int si, int ne);
// [13] void insert(int di, bool value);
// [13] void insert(int di, const bdea_BitArray& sa);
// [13] void insert(int di, const bdea_BitArray& sa, int si, int ne);
// [13] void remove(int index);
// [13] void remove(int index, int ne);
// [ 2] void removeAll();
// [14] void replace(int di, bool value);
// [14] void replace(int di, const bdea_BitArray& sa, int si, int ne);
// [17] void reserveCapacity(int ne);
// [17] void reserveCapacityRaw(int ne);
// [12] void setLength(int newLength);
// [12] void setLength(int newLength, bool value);
// [12] void setLengthRaw(int newLength);
// [10] STREAM& bdexStreamIn(STREAM& stream, int version);
// [16] void swap(int index1, int index2);
// [ 4] bool operator[](int index) const;
// [28] int find0AtLargestIndex() const;
// [28] int find0AtLargestIndexGE(int index) const;
// [28] int find0AtLargestIndexGT(int index) const;
// [28] int find0AtLargestIndexLE(int index) const;
// [28] int find0AtLargestIndexLT(int index) const;
// [29] int find0AtSmallestIndex() const;
// [29] int find0AtSmallestIndexGE(int index) const;
// [29] int find0AtSmallestIndexGT(int index) const;
// [29] int find0AtSmallestIndexLE(int index) const;
// [29] int find0AtSmallestIndexLT(int index) const;
// [30] int find1AtLargestIndex() const;
// [30] int find1AtLargestIndexGE(int index) const;
// [30] int find1AtLargestIndexGT(int index) const;
// [30] int find1AtLargestIndexLE(int index) const;
// [30] int find1AtLargestIndexLT(int index) const;
// [31] int find1AtSmallestIndex() const;
// [31] int find1AtSmallestIndexGE(int index) const;
// [31] int find1AtSmallestIndexGT(int index) const;
// [31] int find1AtSmallestIndexLE(int index) const;
// [31] int find1AtSmallestIndexLT(int index) const;
// [ 4] bool isAnySet1() const;
// [ 4] bool isAnySet0() const;
// [ 4] int length() const;
// [ 4] int numSet1() const;
// [ 4] int numSet0() const;
// [ 5] ostream& print(ostream& stream, int level, int spl);
// [10] int maxSupportedBdexVersion() const;
// [10] STREAM& bdexStreamOut(STREAM& stream, version) const;
//
// [ 6] operator==(const bdea_BitArray&, const bdea_BitArray&);
// [ 6] operator!=(const bdea_BitArray&, const bdea_BitArray&);
// [19] operator~(const bdea_BitArray& bitArray);
// [21] operator<<(const bdea_BitArray& bitArray, int numBits);
// [21] operator>>(const bdea_BitArray& bitArray, int numBits);
// [27] operator&(const bdea_BitArray&, const bdea_BitArray&);
// [27] operator|(const bdea_BitArray&, const bdea_BitArray&);
// [27] operator^(const bdea_BitArray&, const bdea_BitArray&);
// [27] operator-(const bdea_BitArray&, const bdea_BitArray&);
// [ 5] operator<<(ostream&, const bdea_BitArray&);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [32] USAGE EXAMPLE
// [ 2] BOOTSTRAP: void append(bool value); // no aliasing
// [ 3] CONCERN: Is the internal memory organization behaving as intended?
//
// [ 3] void stretch(Obj *object, int size);
// [ 3] void stretchRemoveAll(Obj *object, int size);
// [ 3] int ggg(bdea_BitArray *object, const char *spec, int vF = 1);
// [ 3] bdea_BitArray& gg(bdea_BitArray* object, const char *spec);
// [ 8] bdea_BitArray   g(const char *spec);

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
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
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdea_BitArray Obj;
typedef bool Element;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

void stretch(Obj *object, int size)
   // Using only primary manipulators, extend the length of the specified
   // 'object' by the specified size.  The resulting value is not specified.
   // The behavior is undefined unless 0 <= size.
{
    ASSERT(object);
    ASSERT(0 <= size);
    for (int i = 0; i < size; ++i) {
        object->append(0);
    }
    ASSERT(object->length() >= size);
}

void stretchRemoveAll(Obj *object, int size)
   // Using only primary manipulators, extend the capacity of the specified
   // 'object' to (at least) the specified size; then remove all elements
   // leaving 'object' empty.  The behavior is undefined unless 0 <= size.
{
    ASSERT(object);
    ASSERT(0 <= size);
    stretch(object, size);
    object->removeAll();
    ASSERT(0 == object->length());
}

void outerP(const char *leader, const Obj &X)
    // This function is used to avert an uncaught exception on Windows during
    // bdema exception testing.  This can happen, e.g., in test cases with
    // large DATA sets.
{
    cout << leader; P(X);
}

//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

 template <class TYPE>
 class NullableVector {
     // This class implements a sequential container of elements of the
     // parameterized 'TYPE'.

     // DATA
     bsl::vector<TYPE>  d_values;       // data elements
     bdea_BitArray      d_nullFlags;    // 'true' indicates i'th element is
                                        // null
     bslma_Allocator   *d_allocator_p;  // held, but not owned

     // NOT IMPLEMENTED
     NullableVector(const NullableVector& original);
     NullableVector& operator=(const NullableVector& rhs);

   public:
     // CREATORS
     // ...
     NullableVector(int initialLength, bslma_Allocator *basicAllocator = 0);
         // Construct a vector having the specified 'initialLength' null
         // elements.  Optionally specify a 'basicAllocator' used to supply
         // memory.  If 'basicAllocator' is 0, the currently supplied default
         // allocator is used.  the behavior is undefined unless
         // '0 <= initialLength'.

     // ...

     ~NullableVector();
         // Destroy this vector.

     // MANIPULATORS
     TYPE& modifiableElement(int index);
            // Return a reference to the (valid) modifiable element at the
            // specified 'index' in this vector.  The behavior is undefined
            // unless '0 <= index < length()'.  Note that if the element at
            // 'index' is null then the nullness flag is reset and the returned
            // value is the default constructed 'TYPE' object.

     void appendNullElement();
            // Append a null element to this vector.  Note that the appended
            // element will have the same value as a default constructed 'TYPE'
            // object.

        void appendElement(const TYPE& value);
            // Append an element having the specified 'value' to the end of
            // this vector.

        void makeNonNull(int index);
            // Make the element at the specified 'index' in this vector
            // non-null.  The behavior is undefined unless
            // '0 <= index < length()'.

        void makeNull(int index);
            // Make the element at the specified 'index' in this vector null.
            // The behavior is undefined unless '0 <= index < length()'.  Note
            // that the new value of the element will be the default
            // constructed value for 'TYPE'.

        void removeElement(int index);
            // Remove the element at the specified 'index' in this vector.  The
            // behavior is undefined unless '0 <= index < length()'.

        // ACCESSORS
        const TYPE& constElement(int index) const;
            // Return a reference to the non-modifiable element at the
            // specified 'index' in this vector.  The behavior is undefined
            // unless '0 <= index < length()'.  Note that if the element at
            // 'index' is null then the nullness flag is not reset and the
            // returned value is the default constructed 'TYPE' object.

        bool isElementNull(int index) const;
            // Return 'true' if the element at the specified 'index' in this
            // vector is null, and 'false' otherwise.  The behavior is
            // undefined unless '0 <= index < length()'.

        int length() const;
            // Return the number of elements in this vector.

        int numNullElements() const;
            // Return the number of null elements in this vector.

        bool isAnyElementNonNull() const;
            // Return 'true' if any element in this vector is non-null, and
            // 'false' otherwise.

        bool isAnyElementNull() const;
            // Return 'true' if any element in this vector is null, and 'false'
            // otherwise.
    };
//..
// Next we implement, in turn, each of the methods declared above.  Note also
// that, since the example class is a template, all methods would be
// implemented within the header file, regardless of whether or not they are
// (or should be) declared 'inline':
//..
                     // --------------------
                     // class NullableVector
                     // --------------------

     // CREATORS
     template <typename TYPE>
     NullableVector<TYPE>::NullableVector(int              initialLength,
                                          bslma_Allocator *basicAllocator)
     : d_values(initialLength, TYPE(), basicAllocator)
     , d_nullFlags(initialLength, true, basicAllocator)
     , d_allocator_p(bslma_Default::allocator(basicAllocator))
     {
     }

     template <typename TYPE>
     NullableVector<TYPE>::~NullableVector()
     {
         BSLS_ASSERT((int) d_values.size() == d_nullFlags.length());
     }

     // MANIPULATORS
     template <typename TYPE>
     inline
     TYPE& NullableVector<TYPE>::modifiableElement(int index)
     {
         BSLS_ASSERT_SAFE(0 <= index && index < length());
         d_nullFlags.set(index, false);
         return d_values[index];
     }

     template <typename TYPE>
     inline
     void NullableVector<TYPE>::appendElement(const TYPE& value)
     {
         d_values.push_back(value);
         d_nullFlags.append(false);
     }

     template <typename TYPE>
     inline
     void NullableVector<TYPE>::appendNullElement()
     {
         d_values.push_back(TYPE());
         d_nullFlags.append(true);
     }

     template <typename TYPE>
     inline
     void NullableVector<TYPE>::removeElement(int index)
     {
         BSLS_ASSERT_SAFE(0 <= index && index < length());
         d_values.erase(d_values.begin() + index);
         d_nullFlags.remove(index);
     }

     template <typename TYPE>
     inline
     void NullableVector<TYPE>::makeNonNull(int index)
     {
         BSLS_ASSERT_SAFE(0 <= index && index < length());
         d_nullFlags.set(index, false);
     }

     template <typename TYPE>
     inline
     void NullableVector<TYPE>::makeNull(int index)
     {
         BSLS_ASSERT_SAFE(0 <= index && index < length());
         d_values[index] = TYPE();
         d_nullFlags.set(index, true);
     }

     // ACCESSORS
     template <typename TYPE>
     inline
     int NullableVector<TYPE>::length() const
     {
         return d_values.size();
     }

     template <typename TYPE>
     inline
     const TYPE& NullableVector<TYPE>::constElement(int index) const
     {
         BSLS_ASSERT_SAFE(0 <= index && index < length());
         return d_values[index];
     }

     template <typename TYPE>
     inline
     bool NullableVector<TYPE>::isElementNull(int index) const
     {
         BSLS_ASSERT_SAFE(0 <= index && index < length());
         return d_nullFlags[index];
     }

     template <typename TYPE>
     inline
     int NullableVector<TYPE>::numNullElements() const
     {
         return d_nullFlags.numSet1();
     }

     template <typename TYPE>
     inline
     bool NullableVector<TYPE>::isAnyElementNonNull() const
     {
         return d_nullFlags.isAnySet0();
     }

     template <typename TYPE>
     inline
     bool NullableVector<TYPE>::isAnyElementNull() const
     {
         return d_nullFlags.isAnySet1();
     }

//=============================================================================
//              GENERATOR FUNCTIONS 'g' AND 'gg' FOR TESTING
//-----------------------------------------------------------------------------
// The following functions interpret the given 'spec' in order from left to
// right to configure the object according to a custom language.  The numbers
// 0 and 1 correspond to bit values to be appended to the 'bdea_BitArray'
// object.  A tilde ('~') indicates that the logical (but not necessarily
// physical) state of the object is to be set to its initial, empty state (via
// the 'removeAll' method).
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
// <ELEMENT>    ::= '0' | '1'
//
// <REMOVE_ALL> ::= '~'
//
// Spec String  Description
// -----------  ---------------------------------------------------------------
// ""           Has no effect; leaves the object empty.
// "0"          Append the value 0.
// "00"         Append two 0 values.
// "011"        Append three values corresponding to 0, 1, and 1.
// "011~"       Append three values corresponding to 0, 1, and 1 and then
//              remove all the elements (set array length to 0).  Note that
//              this spec yields an object that is logically equivalent
//              (but not necessarily identical internally) to one
//              yielded by ("").
// "011~00"     Append three values corresponding to 0, 1, and 1; empty
//              the object; and append values corresponding to 0 and 0.
//
//-----------------------------------------------------------------------------

int ggg(bdea_BitArray *object, const char *spec, int verboseFlag = 1)
    // Configure the specified 'object' according to the specified 'spec',
    // using only the primary manipulator function 'append' and white-box
    // manipulator 'removeAll'.  Optionally specify a zero 'verboseFlag' to
    // suppress 'spec' syntax error messages.  Return the index of the first
    // invalid character, and a negative value otherwise.  Note that this
    // function is used to implement 'gg' as well as allow for verification of
    // syntax error detection.
{
    enum { SUCCESS = -1 };
    for (int i = 0; spec[i]; ++i) {
        if ('0' == spec[i]) {
            object->append(0);
        }
        else if ('1' == spec[i]) {
            object->append(1);
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

bdea_BitArray& gg(bdea_BitArray *object, const char *spec)
    // Return, by reference, the specified object with its value adjusted
    // according to the specified 'spec'.
{
    ASSERT(ggg(object, spec) < 0);
    return *object;
}

bdea_BitArray g(const char *spec)
    // Return, by value, a new object corresponding to the specified 'spec'.
{
    bdea_BitArray object((bslma_Allocator *)0);
    return gg(&object, spec);
}

class EqualityTester {
  private:
        const Obj   *A;
        const Obj   *B;
  public:
        EqualityTester(const Obj *a, const Obj *b);
        ~EqualityTester();
};

// CREATORS
inline EqualityTester::EqualityTester(const Obj *a, const Obj *b)
: A(a)
, B(b)
{
}

inline EqualityTester::~EqualityTester()
{
        ASSERT(*A == *B);
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

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    bslma_TestAllocator testAllocator(veryVeryVerbose);

    char LONG_SPEC_1[33];
    char LONG_SPEC_2[65];
    char LONG_SPEC_3[97];
    char LONG_SPEC_4[129];
    char LONG_SPEC_5[161];
    char LONG_SPEC_6[193];
    char LONG_SPEC_7[225];
    char LONG_SPEC_8[257];
    char LONG_SPEC_9[289];

    strcpy(LONG_SPEC_1, "00001000111000010011000001001101");

    strcpy(LONG_SPEC_2, LONG_SPEC_1);
    strcat(LONG_SPEC_2, LONG_SPEC_1);

    strcpy(LONG_SPEC_3, LONG_SPEC_2);
    strcat(LONG_SPEC_3, LONG_SPEC_1);

    strcpy(LONG_SPEC_4, LONG_SPEC_3);
    strcat(LONG_SPEC_4, LONG_SPEC_1);

    strcpy(LONG_SPEC_5, LONG_SPEC_4);
    strcat(LONG_SPEC_5, LONG_SPEC_1);

    strcpy(LONG_SPEC_6, LONG_SPEC_5);
    strcat(LONG_SPEC_6, LONG_SPEC_1);

    strcpy(LONG_SPEC_7, LONG_SPEC_6);
    strcat(LONG_SPEC_7, LONG_SPEC_1);

    strcpy(LONG_SPEC_8, LONG_SPEC_7);
    strcat(LONG_SPEC_8, LONG_SPEC_1);

    strcpy(LONG_SPEC_9, LONG_SPEC_8);
    strcat(LONG_SPEC_9, LONG_SPEC_1);

    switch (test) { case 0:  // Zero is always the leading case.
      case 31: {
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

///Usage
///-----
// An efficient implementation of an arbitrary precision bit sequence container
// has myriad applications.  For example, a 'bdea_BitArray' can be used
// effectively as a parallel array of flags indicating some special property,
// such as, 'isNull', 'isBusinessDay', etc.; its use is especially indicated
// when (1) the number of elements of the primary array can grow large, and
// (2) the individual elements do not have the capacity or capability to store
// the information directly.
//
// As a simple example, we'll implement a (heavily elided) value-semantic
// template class, 'NullableVector<TYPE>', that behaves like a
// 'bsl::vector<TYPE>' but additionally allows storing a nullness flag to
// signify that the corresponding element was not specified.  Elements added to
// a 'NullableVector' are null by default, although there are manipulator
// functions that allow appending a non-null element.  Each null element
// stores the default value for 'TYPE'.
//
// Note that this class has a minimal interface (suitable for illustration
// purpose only) that allows users to either append a (non-null) 'TYPE' value
// or a null value.  A real 'NullableVector' class would support a complete set
// of *value* *semantic* operations, including copy construction, assignment,
// equality comparison, 'ostream' printing, and 'bdex' serialization.
//
// We start by defining the interface of 'NullableVector':
//..

    // See class definition is above 'main'.

//..
// Clients of 'NullableVector' can then use this class as follows:
//..
      NullableVector<int> array(0);
      const NullableVector<int>& ARRAY = array;
      const int DEFAULT_INT = 0;
      ASSERT(0       == ARRAY.length());
      ASSERT(0       == ARRAY.numNullElements());
      ASSERT(false   == ARRAY.isAnyElementNonNull());
      ASSERT(false   == ARRAY.isAnyElementNull());

      array.appendElement(5);
      ASSERT(1       == ARRAY.length());
      ASSERT(5       == ARRAY.constElement(0));
      ASSERT(false   == ARRAY.isElementNull(0));
      ASSERT(0       == ARRAY.numNullElements());
      ASSERT(true    == ARRAY.isAnyElementNonNull());
      ASSERT(false   == ARRAY.isAnyElementNull());

      array.appendNullElement();
      ASSERT(2           == ARRAY.length());
      ASSERT(5           == ARRAY.constElement(0));
      ASSERT(DEFAULT_INT == ARRAY.constElement(1));
      ASSERT(false       == ARRAY.isElementNull(0));
      ASSERT(true        == ARRAY.isElementNull(1));
      ASSERT(1           == ARRAY.numNullElements());
      ASSERT(true        == ARRAY.isAnyElementNonNull());
      ASSERT(true        == ARRAY.isAnyElementNull());

      array.makeNonNull(1);
      ASSERT(2           == ARRAY.length());
      ASSERT(5           == ARRAY.constElement(0));
      ASSERT(DEFAULT_INT == ARRAY.constElement(1));
      ASSERT(false       == ARRAY.isElementNull(0));
      ASSERT(false       == ARRAY.isElementNull(1));
      ASSERT(0           == ARRAY.numNullElements());
      ASSERT(true        == ARRAY.isAnyElementNonNull());
      ASSERT(false       == ARRAY.isAnyElementNull());

      array.makeNull(0);
      ASSERT(2           == ARRAY.length());
      ASSERT(DEFAULT_INT == ARRAY.constElement(0));
      ASSERT(DEFAULT_INT == ARRAY.constElement(1));
      ASSERT(true        == ARRAY.isElementNull(0));
      ASSERT(false       == ARRAY.isElementNull(1));
      ASSERT(1           == ARRAY.numNullElements());
      ASSERT(true        == ARRAY.isAnyElementNonNull());
      ASSERT(true       == ARRAY.isAnyElementNull());

      array.removeElement(0);
      ASSERT(1           == ARRAY.length());
      ASSERT(DEFAULT_INT == ARRAY.constElement(0));
      ASSERT(false       == ARRAY.isElementNull(0));
      ASSERT(0           == ARRAY.numNullElements());
      ASSERT(true        == ARRAY.isAnyElementNonNull());
      ASSERT(false       == ARRAY.isAnyElementNull());

      array.removeElement(0);
      ASSERT(0       == ARRAY.length());
      ASSERT(0       == ARRAY.numNullElements());
      ASSERT(false   == ARRAY.isAnyElementNonNull());
      ASSERT(false   == ARRAY.isAnyElementNull());
//..
      } break;
      case 30: {
        // --------------------------------------------------------------------
        // TESTING FINDBIT1ATSMALLESTINDEX METHODS
        //   The concerns for each of the methods are:
        //     - the correct result is obtained
        //     - memory is not allocated
        //
        // Plan:
        //   For each of a sequence of test vectors, verify that the methods
        //   give the expected value and do not allocate memory.  Test vectors
        //   are comprised of a specification and exhaustive enumeration of all
        //   possible starting indices for the specification.  The correct
        //   result is obtained through simple oracle functions.
        //
        // Testing:
        //   int find1AtSmallestIndex() const;
        //   int find1AtSmallestIndexGE(int index) const;
        //   int find1AtSmallestIndexGT(int index) const;
        //   int find1AtSmallestIndexLE(int index) const;
        //   int find1AtSmallestIndexLT(int index) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing find1AtSmallestIndex Methods" << endl
                          << "=======================================" << endl;

        if (verbose) cout << "\nTesting find1AtSmallestIndex Methods"
                          << endl;
        {
            const char *SPECS[] = {
                "0",      "01",     "011",    "0110",   "01100",
                "0110001",         "01100011",         "011000110",
                "011000110001100", "0110001100011000", "01100011000110001",
                "0000100011100001001100000100110",
                "00001000111000010011000001001101",
           "000010001110000100110000010011010000100011100001001100000100110",
           "0000100011100001001100000100110100001000111000010011000001001100",
           "00001000111000010011000001001101000010001110000100110000010011001",
                LONG_SPEC_3,
                LONG_SPEC_4,
                LONG_SPEC_5,
                LONG_SPEC_6,
                LONG_SPEC_7,
                LONG_SPEC_8,
                LONG_SPEC_9,
            0}; // Null string required as last element.

            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const DST = SPECS[ti];
                const int curLen = (int)strlen(DST);

                if (verbose) {
                    cout << "\tFor an object of length " << curLen << ":\t";
                    P(DST);
                }
                LOOP_ASSERT(DST, oldLen < curLen);  // strictly increasing
                oldLen = curLen;

                const Obj X = g(DST);
                LOOP_ASSERT(ti, curLen == X.length());  // same lengths

                const int BB = testAllocator.numBlocksTotal();
                const int B = testAllocator.numBlocksInUse();

                for (int index = 0; index < curLen; ++index) {
                    int expGE = -1;
                    int expGT = -1;
                    int expLE = -1;
                    int expLT = -1;

                    { // compute expGE result
                        for (int i = index; expGE == -1 && i < curLen; ++i) {
                            if (1 == X[i]) {
                                expGE = i;
                            }
                        }
                    }

                    { // compute expGT result
                        for (int i = index + 1; expGT == -1 &&
                                                             i < curLen; ++i) {
                            if (1 == X[i]) {
                                expGT = i;
                            }
                        }
                    }

                    { // compute expLE result
                        for (int i = 0; expLE == -1 && i <= index; ++i) {
                            if (1 == X[i]) {
                                expLE = i;
                            }
                        }
                    }

                    { // compute expLT result
                        for (int i = 0; expLT == -1 && i < index; ++i) {
                            if (1 == X[i]) {
                                expLT = i;
                            }
                        }
                    }

                    if (veryVerbose) {
                        P(X);
                    }

                    const int GE = X.find1AtSmallestIndexGE(index);
                    const int GT = X.find1AtSmallestIndexGT(index);
                    const int LE = X.find1AtSmallestIndexLE(index);
                    const int LT = X.find1AtSmallestIndexLT(index);

                    if (0 == index) {
                        LOOP_ASSERT(ti, expGE == X.find1AtSmallestIndex());
                    }
                    LOOP5_ASSERT(ti, index, expGE, GE, X, expGE == GE);
                    LOOP5_ASSERT(ti, index, expGT, GT, X, expGT == GT);
                    LOOP2_ASSERT(ti, index, expLE == LE);
                    LOOP2_ASSERT(ti, index, expLT == LT);
                }

                const int AA = testAllocator.numBlocksTotal();
                const int A = testAllocator.numBlocksInUse();
                ASSERT(BB == AA);
                ASSERT(B  == A);
            }
        }

      } break;
      case 29: {
        // --------------------------------------------------------------------
        // TESTING FINDBIT1ATLARGESTINDEX METHODS
        //   The concerns for each of the methods are:
        //     - the correct result is obtained
        //     - memory is not allocated
        //
        // Plan:
        //   For each of a sequence of test vectors, verify that the methods
        //   give the expected value and do not allocate memory.  Test vectors
        //   are comprised of a specification and exhaustive enumeration of all
        //   possible starting indices for the specification.  The correct
        //   result is obtained through simple oracle functions.
        //
        // Testing:
        //   int find1AtLargestIndex() const;
        //   int find1AtLargestIndexGE(int index) const;
        //   int find1AtLargestIndexGT(int index) const;
        //   int find1AtLargestIndexLE(int index) const;
        //   int find1AtLargestIndexLT(int index) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing find1AtLargestIndex Methods" << endl
                          << "======================================" << endl;

        if (verbose) cout << "\nTesting find1AtLargestIndex Methods"
                          << endl;
        {
            const char *SPECS[] = {
                "0",      "01",     "011",    "0110",   "01100",
                "0110001",         "01100011",         "011000110",
                "011000110001100", "0110001100011000", "01100011000110001",
                "0000100011100001001100000100110",
                "00001000111000010011000001001101",
          "000010001110000100110000010011010000100011100001001100000100110",
          "0000100011100001001100000100110100001000111000010011000001001100",
          "00001000111000010011000001001101000010001110000100110000010011001",
          "000010001110000100110000010011010000000000000000000000000000000000",
                LONG_SPEC_3,
                LONG_SPEC_4,
                LONG_SPEC_5,
                LONG_SPEC_6,
                LONG_SPEC_7,
                LONG_SPEC_8,
                LONG_SPEC_9,
            0}; // Null string required as last element.

            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const DST = SPECS[ti];
                const int curLen = (int)strlen(DST);

                if (verbose) {
                    cout << "\tFor an object of length " << curLen << ":\t";
                    P(DST);
                }
                LOOP_ASSERT(DST, oldLen < curLen);  // strictly increasing
                oldLen = curLen;

                const Obj X = g(DST);
                LOOP_ASSERT(ti, curLen == X.length());  // same lengths

                const int BB = testAllocator.numBlocksTotal();
                const int B = testAllocator.numBlocksInUse();

                for (int index = 0; index < curLen; ++index) {
                    int expGE = -1;
                    int expGT = -1;
                    int expLE = -1;
                    int expLT = -1;

                    { // compute expGE result
                        for (int i = curLen - 1; expGE == -1 && i >= index;
                                                                         --i) {
                            if (1 == X[i]) {
                                expGE = i;
                            }
                        }
                    }

                    { // compute expGT result
                        for (int i = curLen - 1; expGT == -1 && i > index;
                                                                         --i) {
                            if (1 == X[i]) {
                                expGT = i;
                            }
                        }
                    }

                    { // compute expLE result
                        for (int i = index; expLE == -1 && i >= 0; --i) {
                            if (1 == X[i]) {
                                expLE = i;
                            }
                        }
                    }

                    { // compute expLT result
                        for (int i = index - 1; expLT == -1 && i >= 0; --i) {
                            if (1 == X[i]) {
                                expLT = i;
                            }
                        }
                    }

                    const int GE = X.find1AtLargestIndexGE(index);
                    const int GT = X.find1AtLargestIndexGT(index);
                    const int LE = X.find1AtLargestIndexLE(index);
                    const int LT = X.find1AtLargestIndexLT(index);

                    if (0 == index) {
                        LOOP_ASSERT(ti, expGE == X.find1AtLargestIndex());
                    }
                    LOOP5_ASSERT(ti, index, X, expGE, GE, expGE == GE);
                    LOOP5_ASSERT(ti, index, X, expGT, GT, expGT == GT);
                    LOOP2_ASSERT(ti, index, expLE == LE);
                    LOOP2_ASSERT(ti, index, expLT == LT);
                }

                const int AA = testAllocator.numBlocksTotal();
                const int A = testAllocator.numBlocksInUse();
                ASSERT(BB == AA);
                ASSERT(B  == A);
            }
        }

      } break;
      case 28: {
        // --------------------------------------------------------------------
        // TESTING FINDBIT0ATSMALLESTINDEX METHODS
        //   The concerns for each of the methods are:
        //     - the correct result is obtained
        //     - memory is not allocated
        //
        // Plan:
        //   For each of a sequence of test vectors, verify that the methods
        //   give the expected value and do not allocate memory.  Test vectors
        //   are comprised of a specification and exhaustive enumeration of all
        //   possible starting indices for the specification.  The correct
        //   result is obtained through simple oracle functions.
        //
        // Testing:
        //   int find0AtSmallestIndex() const;
        //   int find0AtSmallestIndexGE(int index) const;
        //   int find0AtSmallestIndexGT(int index) const;
        //   int find0AtSmallestIndexLE(int index) const;
        //   int find0AtSmallestIndexLT(int index) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing find0AtSmallestIndex Methods" << endl
                          << "=======================================" << endl;

        if (verbose) cout << "\nTesting find0AtSmallestIndex Methods"
                          << endl;
        {
            const char *SPECS[] = {
                "0",      "01",     "011",    "0110",   "01100", "111111",
                "0110001",         "01100011",         "011000110",
                "011000110001100", "0110001100011000", "01100011000110001",
                "0000100011100001001100000100110",
                "00001000111000010011000001001101",
           "000010001110000100110000010011010000100011100001001100000100110",
           "0000100011100001001100000100110100001000111000010011000001001100",
           "00001000111000010011000001001101000010001110000100110000010011001",
                LONG_SPEC_3,
                LONG_SPEC_4,
                LONG_SPEC_5,
                LONG_SPEC_6,
                LONG_SPEC_7,
                LONG_SPEC_8,
                LONG_SPEC_9,
            0}; // Null string required as last element.

            {
                // Verify result for empty array.
                const Obj X;
                ASSERT(-1 == X.find0AtSmallestIndex());
            }

            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const DST = SPECS[ti];
                const int curLen = (int)strlen(DST);

                if (verbose) {
                    cout << "\tFor an object of length " << curLen << ":\t";
                    P(DST);
                }
                LOOP_ASSERT(DST, oldLen < curLen);  // strictly increasing
                oldLen = curLen;

                const Obj X = g(DST);
                LOOP_ASSERT(ti, curLen == X.length());  // same lengths

                const int BB = testAllocator.numBlocksTotal();
                const int B = testAllocator.numBlocksInUse();

                for (int index = 0; index < curLen; ++index) {
                    int expGE = -1;
                    int expGT = -1;
                    int expLE = -1;
                    int expLT = -1;

                    { // compute expGE result
                        for (int i = index; expGE == -1 && i < curLen; ++i) {
                            if (0 == X[i]) {
                                expGE = i;
                            }
                        }
                    }

                    { // compute expGT result
                        for (int i = index + 1; expGT == -1 &&
                                                             i < curLen; ++i) {
                            if (0 == X[i]) {
                                expGT = i;
                            }
                        }
                    }

                    { // compute expLE result
                        for (int i = 0; expLE == -1 && i <= index; ++i) {
                            if (0 == X[i]) {
                                expLE = i;
                            }
                        }
                    }

                    { // compute expLT result
                        for (int i = 0; expLT == -1 && i < index; ++i) {
                            if (0 == X[i]) {
                                expLT = i;
                            }
                        }
                    }

                    const int GE = X.find0AtSmallestIndexGE(index);
                    const int GT = X.find0AtSmallestIndexGT(index);
                    const int LE = X.find0AtSmallestIndexLE(index);
                    const int LT = X.find0AtSmallestIndexLT(index);

                    if (0 == index) {
                        LOOP_ASSERT(ti, expGE == X.find0AtSmallestIndex());
                    }
                    LOOP2_ASSERT(ti, index, expGE == GE);
                    LOOP2_ASSERT(ti, index, expGT == GT);
                    LOOP2_ASSERT(ti, index, expLE == LE);
                    LOOP2_ASSERT(ti, index, expLT == LT);
                }

                const int AA = testAllocator.numBlocksTotal();
                const int A = testAllocator.numBlocksInUse();
                ASSERT(BB == AA);
                ASSERT(B  == A);
            }
        }

      } break;
      case 27: {
        // --------------------------------------------------------------------
        // TESTING FINDBIT0ATLARGESTINDEX METHODS
        //   The concerns for each of the methods are:
        //     - the correct result is obtained
        //     - memory is not allocated
        //
        // Plan:
        //   For each of a sequence of test vectors, verify that the methods
        //   give the expected value and do not allocate memory.  Test vectors
        //   are comprised of a specification and exhaustive enumeration of all
        //   possible starting indices for the specification.  The correct
        //   result is obtained through simple oracle functions.
        //
        // Testing:
        //   int find0AtLargestIndex() const;
        //   int find0AtLargestIndexGE(int index) const;
        //   int find0AtLargestIndexGT(int index) const;
        //   int find0AtLargestIndexLE(int index) const;
        //   int find0AtLargestIndexLT(int index) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing find0AtLargestIndex Methods" << endl
                          << "======================================" << endl;

        if (verbose) cout << "\nTesting find0AtLargestIndex Methods"
                          << endl;
        {
            const char *SPECS[] = {
                "0",      "01",     "011",    "0110",   "01100", "111111",
                "0110001",         "01100011",         "011000110",
                "011000110001100", "0110001100011000", "01100011000110001",
                "0000100011100001001100000100110",
                "00001000111000010011000001001101",
           "000010001110000100110000010011010000100011100001001100000100110",
           "0000100011100001001100000100110100001000111000010011000001001100",
           "00001000111000010011000001001101000010001110000100110000010011001",
                LONG_SPEC_3,
                LONG_SPEC_4,
                LONG_SPEC_5,
                LONG_SPEC_6,
                LONG_SPEC_7,
                LONG_SPEC_8,
                LONG_SPEC_9,
            0}; // Null string required as last element.

            {
                // Verify result for empty array.
                const Obj X;
                ASSERT(-1 == X.find0AtLargestIndex());
            }

            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const DST = SPECS[ti];
                const int curLen = (int)strlen(DST);

                if (verbose) {
                    cout << "\tFor an object of length " << curLen << ":\t";
                    P(DST);
                }
                LOOP_ASSERT(DST, oldLen < curLen);  // strictly increasing
                oldLen = curLen;

                const Obj X = g(DST);
                LOOP_ASSERT(ti, curLen == X.length());  // same lengths

                const int BB = testAllocator.numBlocksTotal();
                const int B = testAllocator.numBlocksInUse();

                for (int index = 0; index < curLen; ++index) {
                    int expGE = -1;
                    int expGT = -1;
                    int expLE = -1;
                    int expLT = -1;

                    { // compute expGE result
                        for (int i = curLen - 1; expGE == -1 && i >= index;
                                                                         --i) {
                            if (0 == X[i]) {
                                expGE = i;
                            }
                        }
                    }

                    { // compute expGT result
                        for (int i = curLen - 1; expGT == -1 && i > index;
                                                                         --i) {
                            if (0 == X[i]) {
                                expGT = i;
                            }
                        }
                    }

                    { // compute expLE result
                        for (int i = index; expLE == -1 && i >= 0; --i) {
                            if (0 == X[i]) {
                                expLE = i;
                            }
                        }
                    }

                    { // compute expLT result
                        for (int i = index - 1; expLT == -1 && i >= 0; --i) {
                            if (0 == X[i]) {
                                expLT = i;
                            }
                        }
                    }

                    const int GE = X.find0AtLargestIndexGE(index);
                    const int GT = X.find0AtLargestIndexGT(index);
                    const int LE = X.find0AtLargestIndexLE(index);
                    const int LT = X.find0AtLargestIndexLT(index);

                    if (0 == index) {
                        LOOP4_ASSERT(ti, expGE, X, X.find0AtLargestIndex(),
                                     expGE == X.find0AtLargestIndex());
                    }
                    LOOP2_ASSERT(ti, index, expGE == GE);
                    LOOP2_ASSERT(ti, index, expGT == GT);
                    LOOP2_ASSERT(ti, index, expLE == LE);
                    LOOP2_ASSERT(ti, index, expLT == LT);
                }

                const int AA = testAllocator.numBlocksTotal();
                const int A = testAllocator.numBlocksInUse();
                ASSERT(BB == AA);
                ASSERT(B  == A);
            }
        }

      } break;
      case 26: {
        // --------------------------------------------------------------------
        // TESTING BIT-WISE OPERATORS
        //   The concerns for each of the methods are:
        //     - the correct result is obtained
        //     - the "Duff's Device"-like implementation is fully tested
        //   For the assignment operators, the additional concern(s) are:
        //     - no allocations are performed
        //     - alias-safe
        //   For the non-assignment operators, the additional concern(s) are:
        //     - exception-neutrality
        //
        // Plan:
        //   For each of a sequence of test vectors, verify that the methods
        //   give the expected value and either do not allocate memory or are
        //   exception-neutral.  The test vectors are selected to exercise each
        //   case of the "Duff's Device"-like implementation.  A second
        //   sequence of vectors is used to verify alias-safety.
        //
        // Testing:
        //   bdea_BitArray& operator&=(const bdea_BitArray& rhs);
        //   bdea_BitArray& operator|=(const bdea_BitArray& rhs);
        //   bdea_BitArray& operator^=(const bdea_BitArray& rhs);
        //   bdea_BitArray& operator-=(const bdea_BitArray& rhs);
        //   operator&(const bdea_BitArray&, const bdea_BitArray&);
        //   operator|(const bdea_BitArray&, const bdea_BitArray&);
        //   operator^(const bdea_BitArray&, const bdea_BitArray&);
        //   operator-(const bdea_BitArray&, const bdea_BitArray&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Bit-Wise Operators" << endl
                          << "==========================" << endl;

        if (verbose) cout <<
            "\nTesting 'operator&=', et al.; not aliased" << endl;
        {
            const char *SPECS[] = {
                "0",      "01",     "011",    "0110",   "01100",
                "0110001",         "01100011",         "011000110",
                "011000110001100", "0110001100011000", "01100011000110001",
                "0000100011100001001100000100110",
                "00001000111000010011000001001101",
           "000010001110000100110000010011010000100011100001001100000100110",
           "0000100011100001001100000100110100001000111000010011000001001100",
           "00001000111000010011000001001101000010001110000100110000010011001",
                LONG_SPEC_3,
                LONG_SPEC_4,
                LONG_SPEC_5,
                LONG_SPEC_6,
                LONG_SPEC_7,
                LONG_SPEC_8,
                LONG_SPEC_9,
            0}; // Null string required as last element.

            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const DST = SPECS[ti];
                const int curLen = (int)strlen(DST);

                if (verbose) {
                    cout << "\tFor an object of length " << curLen << ":\t";
                    P(DST);
                }
                LOOP_ASSERT(DST, oldLen < curLen);  // strictly increasing
                oldLen = curLen;

                const Obj X = g(DST);
                LOOP_ASSERT(ti, curLen == X.length());  // same lengths

                for (int tj = 0; SPECS[tj]; ++tj) {
                    const char *const SRC = SPECS[tj];
                    const Obj Y = g(SRC);                  // control

                    if (veryVerbose) {
                        P(X) P(Y)
                    }

                    { // and
                      Obj mE(X, &testAllocator);
                      const Obj& E = mE;
                      int i;
                      for (i = 0; i < X.length() && i < Y.length(); ++i) {
                          mE.andEqual(i, Y[i]);
                      }
                      for (; i < X.length(); ++i) {
                          mE.andEqual(i, 0);
                      }

                      Obj mR(X, &testAllocator);
                      const Obj& R = mR;

                      const int BB = testAllocator.numBlocksTotal();
                      const int B = testAllocator.numBlocksInUse();

                      if (veryVerbose) {
                          P(R) P(Y)
                      }

                      mR &= Y;

                      if (veryVerbose) {
                          P(R) P(Y)
                      }

                      const int AA = testAllocator.numBlocksTotal();
                      const int A = testAllocator.numBlocksInUse();
                      ASSERT(BB == AA);
                      ASSERT(B  == A);

                      LOOP4_ASSERT(ti, tj, E, R, E == R);

                      {
                        BEGIN_BSLMA_EXCEPTION_TEST {
                          const int AL = testAllocator.allocationLimit();
                          testAllocator.setAllocationLimit(-1);
                          Obj mZ(&testAllocator);  const Obj& Z = mZ;
                          testAllocator.setAllocationLimit(AL);

                          mZ = X & Y;

                          LOOP2_ASSERT(ti, tj, E == Z);

                        } END_BSLMA_EXCEPTION_TEST
                      }
                    }

                    { // or
                      Obj mE(X, &testAllocator);
                      const Obj& E = mE;
                      int i;
                      for (i = 0; i < X.length() && i < Y.length(); ++i) {
                          mE.orEqual(i, Y[i]);
                      }
                      for (; i < X.length(); ++i) {
                          mE.orEqual(i, 0);
                      }

                      Obj mR(X, &testAllocator);
                      const Obj& R = mR;

                      const int BB = testAllocator.numBlocksTotal();
                      const int B = testAllocator.numBlocksInUse();

                      if (veryVerbose) {
                          P(R) P(Y)
                      }

                      mR |= Y;

                      if (veryVerbose) {
                          P(R) P(Y)
                      }

                      const int AA = testAllocator.numBlocksTotal();
                      const int A = testAllocator.numBlocksInUse();
                      ASSERT(BB == AA);
                      ASSERT(B  == A);

                      LOOP4_ASSERT(ti, tj, E, R, E == R);

                      {
                        BEGIN_BSLMA_EXCEPTION_TEST {
                          const int AL = testAllocator.allocationLimit();
                          testAllocator.setAllocationLimit(-1);
                          Obj mZ(&testAllocator);  const Obj& Z = mZ;
                          testAllocator.setAllocationLimit(AL);

                          mZ = X | Y;

                          LOOP2_ASSERT(ti, tj, E == Z);

                        } END_BSLMA_EXCEPTION_TEST
                      }
                    }

                    { // xor
                      Obj mE(X, &testAllocator);
                      const Obj& E = mE;
                      int i;
                      for (i = 0; i < X.length() && i < Y.length(); ++i) {
                          mE.xorEqual(i, Y[i]);
                      }
                      for (; i < X.length(); ++i) {
                          mE.xorEqual(i, 0);
                      }

                      Obj mR(X, &testAllocator);
                      const Obj& R = mR;

                      const int BB = testAllocator.numBlocksTotal();
                      const int B = testAllocator.numBlocksInUse();

                      if (veryVerbose) {
                          P(R) P(Y)
                      }

                      mR ^= Y;

                      if (veryVerbose) {
                          P(R) P(Y)
                      }

                      const int AA = testAllocator.numBlocksTotal();
                      const int A = testAllocator.numBlocksInUse();
                      ASSERT(BB == AA);
                      ASSERT(B  == A);

                      LOOP4_ASSERT(ti, tj, E, R, E == R);

                      {
                        BEGIN_BSLMA_EXCEPTION_TEST {
                          const int AL = testAllocator.allocationLimit();
                          testAllocator.setAllocationLimit(-1);
                          Obj mZ(&testAllocator);  const Obj& Z = mZ;
                          testAllocator.setAllocationLimit(AL);

                          mZ = X ^ Y;

                          LOOP2_ASSERT(ti, tj, E == Z);

                        } END_BSLMA_EXCEPTION_TEST
                      }
                    }

                    { // minus
                      Obj mE(X, &testAllocator);
                      const Obj& E = mE;
                      int i;
                      for (i = 0; i < X.length() && i < Y.length(); ++i) {
                          mE.minusEqual(i, Y[i]);
                      }
                      for (; i < X.length(); ++i) {
                          mE.minusEqual(i, 0);
                      }

                      Obj mR(X, &testAllocator);
                      const Obj& R = mR;

                      const int BB = testAllocator.numBlocksTotal();
                      const int B = testAllocator.numBlocksInUse();

                      if (veryVerbose) {
                          P(R) P(Y)
                      }

                      mR -= Y;

                      if (veryVerbose) {
                          P(R) P(Y)
                      }

                      const int AA = testAllocator.numBlocksTotal();
                      const int A = testAllocator.numBlocksInUse();
                      ASSERT(BB == AA);
                      ASSERT(B  == A);

                      LOOP4_ASSERT(ti, tj, E, R, E == R);

                      {
                        BEGIN_BSLMA_EXCEPTION_TEST {
                          const int AL = testAllocator.allocationLimit();
                          testAllocator.setAllocationLimit(-1);
                          Obj mZ(&testAllocator);  const Obj& Z = mZ;
                          testAllocator.setAllocationLimit(AL);

                          mZ = X - Y;

                          LOOP2_ASSERT(ti, tj, E == Z);

                        } END_BSLMA_EXCEPTION_TEST
                      }
                    }

                }
            }
        }

        if (verbose) cout <<
            "\nTesting 'operator&=', et al.; aliased" << endl;
        {
            const char *SPECS[] = {
                "0",      "01",     "011",    "0110",   "01100",
                "0110001",         "01100011",         "011000110",
                "011000110001100", "0110001100011000", "01100011000110001",
                "0000100011100001001100000100110",
                "00001000111000010011000001001101",
           "000010001110000100110000010011010000100011100001001100000100110",
           "0000100011100001001100000100110100001000111000010011000001001100",
           "00001000111000010011000001001101000010001110000100110000010011001",
                LONG_SPEC_3,
                LONG_SPEC_4,
                LONG_SPEC_5,
                LONG_SPEC_6,
                LONG_SPEC_7,
                LONG_SPEC_8,
                LONG_SPEC_9,
            0}; // Null string required as last element.

            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const DST = SPECS[ti];
                const int curLen = (int)strlen(DST);

                if (verbose) {
                    cout << "\tFor an object of length " << curLen << ":\t";
                    P(DST);
                }
                LOOP_ASSERT(DST, oldLen < curLen);  // strictly increasing
                oldLen = curLen;

                const Obj X = g(DST);
                LOOP_ASSERT(ti, curLen == X.length());  // same lengths

                if (veryVerbose) {
                    P(X)
                }

                { // and
                    Obj mE(X, &testAllocator);
                    const Obj& E = mE;
                    int i;
                    for (i = 0; i < X.length(); ++i) {
                        mE.andEqual(i, X[i]);
                    }

                    Obj mR(X, &testAllocator);
                    const Obj& R = mR;

                    const int BB = testAllocator.numBlocksTotal();
                    const int B = testAllocator.numBlocksInUse();

                    mR &= R;

                    const int AA = testAllocator.numBlocksTotal();
                    const int A = testAllocator.numBlocksInUse();
                    ASSERT(BB == AA);
                    ASSERT(B  == A);

                    LOOP3_ASSERT(ti, E, R, E == R);
                }

                { // or
                    Obj mE(X, &testAllocator);
                    const Obj& E = mE;
                    int i;
                    for (i = 0; i < X.length(); ++i) {
                        mE.orEqual(i, X[i]);
                    }

                    Obj mR(X, &testAllocator);
                    const Obj& R = mR;

                    const int BB = testAllocator.numBlocksTotal();
                    const int B = testAllocator.numBlocksInUse();

                    mR |= R;

                    const int AA = testAllocator.numBlocksTotal();
                    const int A = testAllocator.numBlocksInUse();
                    ASSERT(BB == AA);
                    ASSERT(B  == A);

                    LOOP3_ASSERT(ti, E, R, E == R);
                }

                { // xor
                    Obj mE(X, &testAllocator);
                    const Obj& E = mE;
                    int i;
                    for (i = 0; i < X.length(); ++i) {
                        mE.xorEqual(i, X[i]);
                    }

                    Obj mR(X, &testAllocator);
                    const Obj& R = mR;

                    const int BB = testAllocator.numBlocksTotal();
                    const int B = testAllocator.numBlocksInUse();

                    mR ^= R;

                    const int AA = testAllocator.numBlocksTotal();
                    const int A = testAllocator.numBlocksInUse();
                    ASSERT(BB == AA);
                    ASSERT(B  == A);

                    LOOP3_ASSERT(ti, E, R, E == R);
                }

                { // minus
                    Obj mE(X, &testAllocator);
                    const Obj& E = mE;
                    int i;
                    for (i = 0; i < X.length(); ++i) {
                        mE.minusEqual(i, X[i]);
                    }

                    Obj mR(X, &testAllocator);
                    const Obj& R = mR;

                    const int BB = testAllocator.numBlocksTotal();
                    const int B = testAllocator.numBlocksInUse();

                    mR -= R;

                    const int AA = testAllocator.numBlocksTotal();
                    const int A = testAllocator.numBlocksInUse();
                    ASSERT(BB == AA);
                    ASSERT(B  == A);

                    LOOP3_ASSERT(ti, E, R, E == R);
                }

            }
        }
      } break;
      case 25: {
        // --------------------------------------------------------------------
        // TESTING BIT-WISE OPERATION METHODS
        //   The concerns for each of the methods are:
        //     - the correct result is obtained
        //     - no allocations are performed
        //     - alias-safe
        //
        // Plan:
        //   For each of a sequence of test vectors, verify that the methods
        //   give the expected value and do not allocate memory.  A second
        //   sequence of vectors is used to verify alias-safety.  A test vector
        //   is comprised of an element of the specifications array, and
        //   element of the number of bits array, and an exhaustive coverage of
        //   the valid source and destination indices.
        //
        // Testing:
        //   void andEqual(dstIndex, srcArray, srcIndex, numBits);
        //   void orEqual(dstIndex, srcArray, srcIndex, numBits);
        //   void xorEqual(dstIndex, srcArray, srcIndex, numBits);
        //   void minusEqual(dstIndex, srcArray, srcIndex, numBits);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Bit-Wise Operation Methods" << endl
                          << "==================================" << endl;

        if (verbose) cout <<
         "\nTesting 'andEqual(dstIndex, srcArray, srcIndex, numBits)', et al."
                          << endl << "\t\tno aliasing"
                          << endl;
        {
            static const char *SPECS[] = {
                "0",      "01",     "011",    "0110",   "01100",
                "0110001",         "01100011",         "011000110",
                "011000110001100", "0110001100011000", "01100011000110001",
                "0000100011100001001100000100110",
                "00001000111000010011000001001101",
             "000010001110000100110000010011010000100011100001001100000100110",
            "0000100011100001001100000100110100001000111000010011000001001100",
           "00001000111000010011000001001101000010001110000100110000010011001",
            0}; // Null string required as last element.

            static const int NUMBITS[] = {
                0, 1, 3, 5, 32, 64
            };
            const int NUM_NUMBITS = sizeof NUMBITS / sizeof *NUMBITS;

            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const DST = SPECS[ti];
                const int curLen = (int)strlen(DST);

                if (verbose) {
                    cout << "\tFor an object of length " << curLen << ":\t";
                    P(DST);
                }
                LOOP_ASSERT(DST, oldLen < curLen);  // strictly increasing
                oldLen = curLen;

                const Obj X = g(DST);                  // control
                LOOP_ASSERT(ti, curLen == X.length());  // same lengths

                for (int tj = 0; SPECS[tj]; ++tj) {
                    const char *const SRC = SPECS[tj];
                    const Obj Y = g(SRC);                  // control

                    for (int tk = 0; tk < NUM_NUMBITS; ++tk) {
                        const int NB = NUMBITS[tk];

                        for (int di = 0; di < X.length() - NB; ++di) {
                            for (int si = 0; si < Y.length() - NB; ++si) {

                                if (veryVerbose) {
                                    P_(X); P_(Y); P_(di); P_(si); P(NB);
                                }

                                { // and
                                    Obj mE(X, &testAllocator);
                                    const Obj& E = mE;
                                    for (int i = 0; i < NB; ++i) {
                                        mE.andEqual(di + i, Y[si + i]);
                                    }

                                    Obj mR(X, &testAllocator);
                                    const Obj& R = mR;

                                    const int BB =
                                                testAllocator.numBlocksTotal();
                                    const int B =
                                                testAllocator.numBlocksInUse();

                                    mR.andEqual(di, Y, si, NB);

                                    const int AA =
                                                testAllocator.numBlocksTotal();
                                    const int A =
                                                testAllocator.numBlocksInUse();
                                    ASSERT(BB == AA);
                                    ASSERT(B  == A);

                                    if (veryVerbose) {
                                        cout << "\t\tand:   ";
                                        P_(E);
                                        P(R);
                                    }

                                    LOOP5_ASSERT(ti, tj, tk, di, si, E == R);
                                }

                                { // or
                                    Obj mE(X, &testAllocator);
                                    const Obj& E = mE;
                                    for (int i = 0; i < NB; ++i) {
                                        mE.orEqual(di + i, Y[si + i]);
                                    }

                                    Obj mR(X, &testAllocator);
                                    const Obj& R = mR;

                                    const int BB =
                                                testAllocator.numBlocksTotal();
                                    const int B =
                                                testAllocator.numBlocksInUse();

                                    mR.orEqual(di, Y, si, NB);

                                    const int AA =
                                                testAllocator.numBlocksTotal();
                                    const int A =
                                                testAllocator.numBlocksInUse();
                                    ASSERT(BB == AA);
                                    ASSERT(B  == A);

                                    if (veryVerbose) {
                                        cout << "\t\tor:    ";
                                        P_(E);
                                        P(R);
                                    }

                                    LOOP5_ASSERT(ti, tj, tk, di, si, E == R);
                                }

                                { // xor
                                    Obj mE(X, &testAllocator);
                                    const Obj& E = mE;
                                    for (int i = 0; i < NB; ++i) {
                                        mE.xorEqual(di + i, Y[si + i]);
                                    }

                                    Obj mR(X, &testAllocator);
                                    const Obj& R = mR;

                                    const int BB =
                                                testAllocator.numBlocksTotal();
                                    const int B =
                                                testAllocator.numBlocksInUse();

                                    mR.xorEqual(di, Y, si, NB);

                                    const int AA =
                                                testAllocator.numBlocksTotal();
                                    const int A =
                                                testAllocator.numBlocksInUse();
                                    ASSERT(BB == AA);
                                    ASSERT(B  == A);

                                    if (veryVerbose) {
                                        cout << "\t\txor:   ";
                                        P_(E);
                                        P(R);
                                    }

                                    LOOP5_ASSERT(ti, tj, tk, di, si, E == R);
                                }

                                { // minus
                                    Obj mE(X, &testAllocator);
                                    const Obj& E = mE;
                                    for (int i = 0; i < NB; ++i) {
                                        mE.minusEqual(di + i, Y[si + i]);
                                    }

                                    Obj mR(X, &testAllocator);
                                    const Obj& R = mR;

                                    const int BB =
                                                testAllocator.numBlocksTotal();
                                    const int B =
                                                testAllocator.numBlocksInUse();

                                    mR.minusEqual(di, Y, si, NB);

                                    const int AA =
                                                testAllocator.numBlocksTotal();
                                    const int A =
                                                testAllocator.numBlocksInUse();
                                    ASSERT(BB == AA);
                                    ASSERT(B  == A);

                                    if (veryVerbose) {
                                        cout << "\t\tminus: ";
                                        P_(E);
                                        P(R);
                                    }

                                    LOOP5_ASSERT(ti, tj, tk, di, si, E == R);
                                }

                            }
                        }
                    }
                }
            }
        }

        if (verbose) cout <<
         "\nTesting 'andEqual(dstIndex, srcArray, srcIndex, numBits)', et al."
                          << endl << "\t\taliased"
                          << endl;
        {
            static const char *SPECS[] = {
                "0",      "01",     "011",    "0110",   "01100",
                "0110001",         "01100011",         "011000110",
                "011000110001100", "0110001100011000", "01100011000110001",
                "0000100011100001001100000100110",
                "00001000111000010011000001001101",
           "000010001110000100110000010011010000100011100001001100000100110",
           "0000100011100001001100000100110100001000111000010011000001001100",
           "00001000111000010011000001001101000010001110000100110000010011001",
            0}; // Null string required as last element.

            static const int NUMBITS[] = {
                0, 1, 3, 5, 32, 64
            };
            const int NUM_NUMBITS = sizeof NUMBITS / sizeof *NUMBITS;

            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const DST = SPECS[ti];
                const int curLen = (int)strlen(DST);

                if (verbose) {
                    cout << "\tFor an object of length " << curLen << ":\t";
                    P(DST);
                }
                LOOP_ASSERT(DST, oldLen < curLen);  // strictly increasing
                oldLen = curLen;

                const Obj X = g(DST);                  // control
                LOOP_ASSERT(ti, curLen == X.length());  // same lengths

                for (int tk = 0; tk < NUM_NUMBITS; ++tk) {
                    const int NB = NUMBITS[tk];

                    for (int di = 0; di < X.length() - NB; ++di) {
                        for (int si = 0; si < X.length() - NB; ++si) {

                            { // and
                                Obj mE (X, &testAllocator);
                                const Obj& E = mE;
                                for (int i = 0; i < NB; ++i) {
                                    mE.andEqual(di + i, X[si + i]);
                                }

                                Obj mR(X, &testAllocator);
                                const Obj& R = mR;

                                const int BB = testAllocator.numBlocksTotal();
                                const int B = testAllocator.numBlocksInUse();

                                mR.andEqual(di, R, si, NB);

                                const int AA = testAllocator.numBlocksTotal();
                                const int A = testAllocator.numBlocksInUse();
                                ASSERT(BB == AA);
                                ASSERT(B  == A);

                                LOOP4_ASSERT(ti, tk, di, si, E == R);
                            }

                            { // or
                                Obj mE (X, &testAllocator);
                                const Obj& E = mE;
                                for (int i = 0; i < NB; ++i) {
                                    mE.orEqual(di + i, X[si + i]);
                                }

                                Obj mR(X, &testAllocator);
                                const Obj& R = mR;

                                const int BB = testAllocator.numBlocksTotal();
                                const int B = testAllocator.numBlocksInUse();

                                mR.orEqual(di, R, si, NB);

                                const int AA = testAllocator.numBlocksTotal();
                                const int A = testAllocator.numBlocksInUse();
                                ASSERT(BB == AA);
                                ASSERT(B  == A);

                                LOOP4_ASSERT(ti, tk, di, si, E == R);
                            }

                            { // xor
                                Obj mE (X, &testAllocator);
                                const Obj& E = mE;
                                for (int i = 0; i < NB; ++i) {
                                    mE.xorEqual(di + i, X[si + i]);
                                }

                                Obj mR(X, &testAllocator);
                                const Obj& R = mR;

                                const int BB = testAllocator.numBlocksTotal();
                                const int B = testAllocator.numBlocksInUse();

                                mR.xorEqual(di, R, si, NB);

                                const int AA = testAllocator.numBlocksTotal();
                                const int A = testAllocator.numBlocksInUse();
                                ASSERT(BB == AA);
                                ASSERT(B  == A);

                                LOOP4_ASSERT(ti, tk, di, si, E == R);
                            }

                            { // minus
                                Obj mE (X, &testAllocator);
                                const Obj& E = mE;
                                for (int i = 0; i < NB; ++i) {
                                    mE.minusEqual(di + i, X[si + i]);
                                }

                                Obj mR(X, &testAllocator);
                                const Obj& R = mR;

                                const int BB = testAllocator.numBlocksTotal();
                                const int B = testAllocator.numBlocksInUse();

                                mR.minusEqual(di, R, si, NB);

                                const int AA = testAllocator.numBlocksTotal();
                                const int A = testAllocator.numBlocksInUse();
                                ASSERT(BB == AA);
                                ASSERT(B  == A);

                                LOOP4_ASSERT(ti, tk, di, si, E == R);
                            }

                        }
                    }
                }
            }
        }
      } break;
      case 24: {
        // --------------------------------------------------------------------
        // TESTING MINUSEQUAL(INDEX, VALUE) METHOD
        //   The concerns for the method are:
        //     - the correct result is obtained
        //     - no allocations are performed
        //     - does not depend upon internal representation
        //
        // Plan:
        //   For each of a sequence of test vectors, verify that the methods
        //   give the expected value and do not allocate memory.  For each
        //   explicit test vector, a number of vectors are created with the
        //   same value but different internal representation.  From these
        //   vectors, the index and value used in the method are exhaustively
        //   tested.
        //
        // Testing:
        //   void minusEqual(int index, bool value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "Testing 'minusEqual(index, value)' Method" << endl
            << "=========================================" << endl;

        if (verbose) cout << "\nTesting 'minusEqual(idx, bv)'" << endl;
        {
            static const char *SPECS[] = {
                "0",      "01",     "011",    "0110",   "01100",
                "0110001",         "01100011",         "011000110",
                "011000110001100", "0110001100011000", "01100011000110001",
                "0000100011100001001100000100110",
                "00001000111000010011000001001101",
           "000010001110000100110000010011010000100011100001001100000100110",
           "0000100011100001001100000100110100001000111000010011000001001100",
           "00001000111000010011000001001101000010001110000100110000010011001",
            0}; // Null string required as last element.

            static const int EXTEND[] = {
                0, 1, 2, 4, 8, 16, 32, 64, 100
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC = SPECS[ti];
                const int curLen = (int)strlen(SPEC);

                if (verbose) {
                    cout << "\tFor an object of length " << curLen << ":\t";
                    P(SPEC);
                }
                LOOP_ASSERT(SPEC, oldLen < curLen);  // strictly increasing
                oldLen = curLen;

                const Obj X = g(SPEC);                  // control
                LOOP_ASSERT(ti, curLen == X.length());  // same lengths

                for (int tj = 0; tj < NUM_EXTEND; ++tj) {
                    const int N = EXTEND[tj];

                    {
                        Obj mY(&testAllocator);
                        stretchRemoveAll(&mY, N);
                        const Obj& Y = mY;       gg(&mY, SPEC);
                        LOOP_ASSERT(SPEC, X == Y);

                        const int B = testAllocator.numBlocksInUse();

                        int i;

                        if (veryVerbose) { cout << "\t\t"; P_(N); P(Y); }
                        for (i = 0; i < X.length(); ++i) {
                            mY.minusEqual(i, 0);
                        }
                        LOOP_ASSERT(SPEC, X == Y);
                        if (veryVerbose) { cout << "\t\t\t"; P(Y); }

                        mY = X;
                        for (i = 0; i < X.length(); ++i) {
                            mY.minusEqual(i, -1);
                        }
                        LOOP_ASSERT(SPEC, X.length() == Y.length());
                        for (i = 0; i < X.length(); ++i) {
                            LOOP_ASSERT(SPEC, 0 == Y[i]);
                        }

                        mY = X;
                        for (i = 0; i < X.length(); ++i) {
                            mY.minusEqual(i, 1);
                        }
                        LOOP_ASSERT(SPEC, X.length() == Y.length());
                        for (i = 0; i < X.length(); ++i) {
                            LOOP_ASSERT(SPEC, 0 == Y[i]);
                        }

                        mY = X;
                        for (i = 0; i < X.length(); ++i) {
                            mY.minusEqual(i, 2);
                        }
                        LOOP_ASSERT(SPEC, X.length() == Y.length());
                        for (i = 0; i < X.length(); ++i) {
                            LOOP_ASSERT(SPEC, 0 == Y[i]);
                        }

                        const int A = testAllocator.numBlocksInUse();
                        ASSERT(B  == A);
                    }

                    {
                        Obj mNX(X);  const Obj& NX = mNX;
                        mNX.toggleAll();

                        Obj mY(&testAllocator);
                        stretchRemoveAll(&mY, N);
                        const Obj& Y = mY;
                        mY = NX;
                        LOOP_ASSERT(SPEC, NX == Y);

                        const int B = testAllocator.numBlocksInUse();

                        int i;

                        if (veryVerbose) { cout << "\t\t"; P_(N); P(Y); }

                        for (i = 0; i < NX.length(); ++i) {
                            mY.minusEqual(i, 0);
                        }
                        LOOP_ASSERT(SPEC, NX == Y);
                        if (veryVerbose) { cout << "\t\t\t"; P(Y); }

                        mY = NX;
                        for (i = 0; i < NX.length(); ++i) {
                            mY.minusEqual(i, -1);
                        }
                        LOOP_ASSERT(SPEC, NX.length() == Y.length());
                        for (i = 0; i < NX.length(); ++i) {
                            LOOP_ASSERT(SPEC, 0 == Y[i]);
                        }

                        mY = NX;
                        for (i = 0; i < NX.length(); ++i) {
                            mY.minusEqual(i, 1);
                        }
                        LOOP_ASSERT(SPEC, NX.length() == Y.length());
                        for (i = 0; i < NX.length(); ++i) {
                            LOOP_ASSERT(SPEC, 0 == Y[i]);
                        }

                        mY = NX;
                        for (i = 0; i < NX.length(); ++i) {
                            mY.minusEqual(i, 2);
                        }
                        LOOP_ASSERT(SPEC, NX.length() == Y.length());
                        for (i = 0; i < NX.length(); ++i) {
                            LOOP_ASSERT(SPEC, 0 == Y[i]);
                        }

                        const int A = testAllocator.numBlocksInUse();
                        ASSERT(B  == A);
                    }
                }
            }
        }
      } break;
      case 23: {
        // --------------------------------------------------------------------
        // TESTING XOREQUAL(INDEX, VALUE) METHOD
        //   The concerns for the method are:
        //     - the correct result is obtained
        //     - no allocations are performed
        //     - does not depend upon internal representation
        //
        // Plan:
        //   For each of a sequence of test vectors, verify that the methods
        //   give the expected value and do not allocate memory.  For each
        //   explicit test vector, a number of vectors are created with the
        //   same value but different internal representation.  From these
        //   vectors, the index and value used in the method are exhaustively
        //   tested.
        //
        // Testing:
        //   void xorEqual(int index, bool value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "Testing 'xorEqual(index, value)' Method" << endl
            << "=======================================" << endl;

        if (verbose) cout << "\nTesting 'xorEqual(idx, bv)'" << endl;
        {
            static const char *SPECS[] = {
                "0",      "01",     "011",    "0110",   "01100",
                "0110001",         "01100011",         "011000110",
                "011000110001100", "0110001100011000", "01100011000110001",
                "0000100011100001001100000100110",
                "00001000111000010011000001001101",
           "000010001110000100110000010011010000100011100001001100000100110",
           "0000100011100001001100000100110100001000111000010011000001001100",
           "00001000111000010011000001001101000010001110000100110000010011001",
            0}; // Null string required as last element.

            static const int EXTEND[] = {
                0, 1, 2, 4, 8, 16, 32, 64, 100
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC = SPECS[ti];
                const int curLen = (int)strlen(SPEC);

                if (verbose) {
                    cout << "\tFor an object of length " << curLen << ":\t";
                    P(SPEC);
                }
                LOOP_ASSERT(SPEC, oldLen < curLen);  // strictly increasing
                oldLen = curLen;

                const Obj X = g(SPEC);                  // control
                LOOP_ASSERT(ti, curLen == X.length());  // same lengths

                for (int tj = 0; tj < NUM_EXTEND; ++tj) {
                    const int N = EXTEND[tj];

                    {
                        Obj mNX(X);  const Obj& NX = mNX;
                        mNX.toggleAll();

                        Obj mY(&testAllocator);
                        stretchRemoveAll(&mY, N);
                        const Obj& Y = mY;       gg(&mY, SPEC);
                        LOOP_ASSERT(SPEC, X == Y);

                        const int B = testAllocator.numBlocksInUse();

                        int i;

                        if (veryVerbose) { cout << "\t\t"; P_(N); P(Y); }
                        for (i = 0; i < X.length(); ++i) {
                            mY.xorEqual(i, 0);
                        }
                        LOOP_ASSERT(SPEC, X == Y);
                        if (veryVerbose) { cout << "\t\t\t"; P(Y); }

                        mY = X;
                        for (i = 0; i < X.length(); ++i) {
                            mY.xorEqual(i, -1);
                        }
                        LOOP_ASSERT(SPEC, NX == Y);

                        mY = X;
                        for (i = 0; i < X.length(); ++i) {
                            mY.xorEqual(i, 1);
                        }
                        LOOP_ASSERT(SPEC, NX == Y);

                        mY = X;
                        for (i = 0; i < X.length(); ++i) {
                            mY.xorEqual(i, 2);
                        }
                        LOOP_ASSERT(SPEC, NX == Y);

                        const int A = testAllocator.numBlocksInUse();
                        ASSERT(B  == A);
                    }
                }
            }
        }
      } break;
      case 22: {
        // --------------------------------------------------------------------
        // TESTING OREQUAL(INDEX, VALUE) METHOD
        //   The concerns for the method are:
        //     - the correct result is obtained
        //     - no allocations are performed
        //     - does not depend upon internal representation
        //
        // Plan:
        //   For each of a sequence of test vectors, verify that the methods
        //   give the expected value and do not allocate memory.  For each
        //   explicit test vector, a number of vectors are created with the
        //   same value but different internal representation.  From these
        //   vectors, the index and value used in the method are exhaustively
        //   tested.
        //
        // Testing:
        //   void orEqual(int index, bool value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "Testing 'orEqual(index, value)' Method" << endl
            << "======================================" << endl;

        if (verbose) cout << "\nTesting 'orEqual(idx, bv)'" << endl;
        {
            static const char *SPECS[] = {
                "0",      "01",     "011",    "0110",   "01100",
                "0110001",         "01100011",         "011000110",
                "011000110001100", "0110001100011000", "01100011000110001",
                "0000100011100001001100000100110",
                "00001000111000010011000001001101",
           "000010001110000100110000010011010000100011100001001100000100110",
           "0000100011100001001100000100110100001000111000010011000001001100",
           "00001000111000010011000001001101000010001110000100110000010011001",
            0}; // Null string required as last element.

            static const int EXTEND[] = {
                0, 1, 2, 4, 8, 16, 32, 64, 100
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC = SPECS[ti];
                const int curLen = (int)strlen(SPEC);

                if (verbose) {
                    cout << "\tFor an object of length " << curLen << ":\t";
                    P(SPEC);
                }
                LOOP_ASSERT(SPEC, oldLen < curLen);  // strictly increasing
                oldLen = curLen;

                const Obj X = g(SPEC);                  // control
                LOOP_ASSERT(ti, curLen == X.length());  // same lengths

                for (int tj = 0; tj < NUM_EXTEND; ++tj) {
                    const int N = EXTEND[tj];

                    {
                        Obj mY(&testAllocator);
                        stretchRemoveAll(&mY, N);
                        const Obj& Y = mY;       gg(&mY, SPEC);
                        LOOP_ASSERT(SPEC, X == Y);

                        const int B = testAllocator.numBlocksInUse();

                        int i;

                        if (veryVerbose) { cout << "\t\t"; P_(N); P(Y); }
                        for (i = 0; i < X.length(); ++i) {
                            mY.orEqual(i, 0);
                        }
                        LOOP_ASSERT(SPEC, X == Y);
                        if (veryVerbose) { cout << "\t\t\t"; P(Y); }

                        mY = X;
                        for (i = 0; i < X.length(); ++i) {
                            mY.orEqual(i, -1);
                        }
                        LOOP_ASSERT(SPEC, X.length() == Y.length());
                        for (i = 0; i < X.length(); ++i) {
                            LOOP_ASSERT(SPEC, 1 == Y[i]);
                        }

                        mY = X;
                        for (i = 0; i < X.length(); ++i) {
                            mY.orEqual(i, 1);
                        }
                        LOOP_ASSERT(SPEC, X.length() == Y.length());
                        for (i = 0; i < X.length(); ++i) {
                            LOOP_ASSERT(SPEC, 1 == Y[i]);
                        }

                        mY = X;
                        for (i = 0; i < X.length(); ++i) {
                            mY.orEqual(i, 2);
                        }
                        LOOP_ASSERT(SPEC, X.length() == Y.length());
                        for (i = 0; i < X.length(); ++i) {
                            LOOP_ASSERT(SPEC, 1 == Y[i]);
                        }

                        const int A = testAllocator.numBlocksInUse();
                        ASSERT(B  == A);
                    }

                    {
                        Obj mNX(X);  const Obj& NX = mNX;
                        mNX.toggleAll();

                        Obj mY(&testAllocator);
                        stretchRemoveAll(&mY, N);
                        const Obj& Y = mY;
                        mY = NX;
                        LOOP_ASSERT(SPEC, NX == Y);

                        const int B = testAllocator.numBlocksInUse();

                        int i;

                        if (veryVerbose) { cout << "\t\t"; P_(N); P(Y); }

                        for (i = 0; i < NX.length(); ++i) {
                            mY.orEqual(i, 0);
                        }
                        LOOP_ASSERT(SPEC, NX == Y);
                        if (veryVerbose) { cout << "\t\t\t"; P(Y); }

                        mY = NX;
                        for (i = 0; i < NX.length(); ++i) {
                            mY.orEqual(i, -1);
                        }
                        LOOP_ASSERT(SPEC, NX.length() == Y.length());
                        for (i = 0; i < NX.length(); ++i) {
                            LOOP_ASSERT(SPEC, 1 == Y[i]);
                        }

                        mY = NX;
                        for (i = 0; i < NX.length(); ++i) {
                            mY.orEqual(i, 1);
                        }
                        LOOP_ASSERT(SPEC, NX.length() == Y.length());
                        for (i = 0; i < NX.length(); ++i) {
                            LOOP_ASSERT(SPEC, 1 == Y[i]);
                        }

                        mY = NX;
                        for (i = 0; i < NX.length(); ++i) {
                            mY.orEqual(i, 2);
                        }
                        LOOP_ASSERT(SPEC, NX.length() == Y.length());
                        for (i = 0; i < NX.length(); ++i) {
                            LOOP_ASSERT(SPEC, 1 == Y[i]);
                        }

                        const int A = testAllocator.numBlocksInUse();
                        ASSERT(B  == A);
                    }
                }
            }
        }
      } break;
      case 21: {
        // --------------------------------------------------------------------
        // TESTING ANDEQUAL(INDEX, VALUE) METHOD
        //   The concerns for the method are:
        //     - the correct result is obtained
        //     - no allocations are performed
        //     - does not depend upon internal representation
        //
        // Plan:
        //   For each of a sequence of test vectors, verify that the methods
        //   give the expected value and do not allocate memory.  For each
        //   explicit test vector, a number of vectors are created with the
        //   same value but different internal representation.  From these
        //   vectors, the index and value used in the method are exhaustively
        //   tested.
        //
        // Testing:
        //   void andEqual(int index, bool value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "Testing 'andEqual(index, value)' Method" << endl
            << "=======================================" << endl;

        if (verbose) cout << "\nTesting 'andEqual(idx, bv)'" << endl;
        {
            static const char *SPECS[] = {
                "0",      "01",     "011",    "0110",   "01100",
                "0110001",         "01100011",         "011000110",
                "011000110001100", "0110001100011000", "01100011000110001",
                "0000100011100001001100000100110",
                "00001000111000010011000001001101",
           "000010001110000100110000010011010000100011100001001100000100110",
           "0000100011100001001100000100110100001000111000010011000001001100",
           "00001000111000010011000001001101000010001110000100110000010011001",
            0}; // Null string required as last element.

            static const int EXTEND[] = {
                0, 1, 2, 4, 8, 16, 32, 64, 100
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC = SPECS[ti];
                const int curLen = (int)strlen(SPEC);

                if (verbose) {
                    cout << "\tFor an object of length " << curLen << ":\t";
                    P(SPEC);
                }
                LOOP_ASSERT(SPEC, oldLen < curLen);  // strictly increasing
                oldLen = curLen;

                const Obj X = g(SPEC);                  // control
                LOOP_ASSERT(ti, curLen == X.length());  // same lengths

                for (int tj = 0; tj < NUM_EXTEND; ++tj) {
                    const int N = EXTEND[tj];

                    {
                        Obj mY(&testAllocator);
                        stretchRemoveAll(&mY, N);
                        const Obj& Y = mY;       gg(&mY, SPEC);
                        LOOP_ASSERT(SPEC, X == Y);

                        const int BB = testAllocator.numBlocksTotal();
                        const int B = testAllocator.numBlocksInUse();

                        int i;

                        if (veryVerbose) { cout << "\t\t"; P_(N); P(Y); }
                        for (i = 0; i < X.length(); ++i) {
                            mY.andEqual(i, -1);
                        }
                        if (veryVerbose) { cout << "\t\t\t"; P(Y); }
                        LOOP_ASSERT(SPEC, X == Y);

                        for (i = 0; i < X.length(); ++i) {
                            mY.andEqual(i, 1);
                        }
                        LOOP_ASSERT(SPEC, X == Y);

                        for (i = 0; i < X.length(); ++i) {
                            mY.andEqual(i, 2);
                        }
                        LOOP_ASSERT(SPEC, X == Y);

                        for (i = 0; i < X.length(); ++i) {
                            mY.andEqual(i, 0);
                        }
                        LOOP_ASSERT(SPEC, X.length() == Y.length());
                        for (i = 0; i < X.length(); ++i) {
                            LOOP_ASSERT(SPEC, 0 == Y[i]);
                        }

                        const int AA = testAllocator.numBlocksTotal();
                        const int A = testAllocator.numBlocksInUse();
                        ASSERT(BB == AA);
                        ASSERT(B  == A);
                    }

                    {
                        Obj mNX(X);  const Obj& NX = mNX;
                        mNX.toggleAll();

                        Obj mY(&testAllocator);
                        stretchRemoveAll(&mY, N);
                        const Obj& Y = mY;
                        mY = NX;
                        LOOP_ASSERT(SPEC, NX == Y);

                        const int BB = testAllocator.numBlocksTotal();
                        const int B = testAllocator.numBlocksInUse();

                        int i;

                        if (veryVerbose) { cout << "\t\t"; P_(N); P(Y); }
                        for (i = 0; i < NX.length(); ++i) {
                            mY.andEqual(i, -1);
                        }
                        if (veryVerbose) { cout << "\t\t\t"; P(Y); }
                        LOOP_ASSERT(SPEC, NX == Y);

                        for (i = 0; i < NX.length(); ++i) {
                            mY.andEqual(i, 1);
                        }
                        LOOP_ASSERT(SPEC, NX == Y);

                        for (i = 0; i < NX.length(); ++i) {
                            mY.andEqual(i, 2);
                        }
                        LOOP_ASSERT(SPEC, NX == Y);

                        for (i = 0; i < NX.length(); ++i) {
                            mY.andEqual(i, 0);
                        }
                        LOOP_ASSERT(SPEC, NX.length() == Y.length());
                        for (i = 0; i < NX.length(); ++i) {
                            LOOP_ASSERT(SPEC, 0 == Y[i]);
                        }

                        const int AA = testAllocator.numBlocksTotal();
                        const int A = testAllocator.numBlocksInUse();
                        ASSERT(BB == AA);
                        ASSERT(B  == A);
                    }
                }
            }
        }
      } break;
      case 20: {
        // --------------------------------------------------------------------
        // TESTING SHIFT METHODS
        //   The concerns for each of the methods are:
        //     - the correct result is obtained
        //     - does not depend upon internal representation
        //   For the assignment operators, the additional concern(s) are:
        //     - no allocations are performed
        //   For the non-assignment operators, the additional concern(s) are:
        //     - exception-neutrality
        //
        // Plan:
        //   For each of a sequence of test vectors, verify that the methods
        //   give the expected value and either do not allocate memory or are
        //   exception-neutral.  For each explicit test vector, a number of
        //   vectors are created with the same value but different internal
        //   representation.  For each of these vectors, the number of bits to
        //   shift is selected from an array.
        //
        // Testing:
        //   bdea_BitArray& operator<<=(int numBits);
        //   bdea_BitArray& operator>>=(int numBits);
        //   operator<<(const bdea_BitArray& bitArray, int numBits);
        //   operator>>(const bdea_BitArray& bitArray, int numBits);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "Testing Rotate Methods" << endl
            << "======================" << endl;

        if (verbose) cout <<
       "\nTesting 'operator<<=', 'operator>>=', 'operator<<', and 'operator>>'"
                          << endl;
        {
            static const char *SPECS[] = {
                "0",      "01",     "011",    "0110",   "01100",
                "0110001",
                "01100011",         "011000110",
                "011000110001100", "0110001100011000", "01100011000110001",
                "0000100011100001001100000100110",
                "00001000111000010011000001001101",
           "000010001110000100110000010011010000100011100001001100000100110",
           "0000100011100001001100000100110100001000111000010011000001001100",
           "00001000111000010011000001001101000010001110000100110000010011001",
            0}; // Null string required as last element.

            static const int ROTATE[] = {
                0, 1, 2, 3, 5, 16, 31, 32, 64, 65
            };
            const int NUM_ROTATE = sizeof ROTATE / sizeof *ROTATE;

            static const int EXTEND[] = {
                0, 1, 2, 4, 8, 16, 32, 64, 100
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC = SPECS[ti];
                const int curLen = (int)strlen(SPEC);

                if (verbose) {
                    cout << "\tFor an object of length " << curLen << ":\t";
                    P(SPEC);
                }
                LOOP_ASSERT(SPEC, oldLen < curLen);  // strictly increasing
                oldLen = curLen;

                const Obj X = g(SPEC);                  // control
                LOOP_ASSERT(ti, curLen == X.length());  // same lengths

                for (int tj = 0; tj < NUM_ROTATE; ++tj) {
                    const int NUMBITS = ROTATE[tj] < X.length()
                                                     ? ROTATE[tj] : X.length();

                    for (int tk = 0; tk < NUM_EXTEND; ++tk) {
                        const int N = EXTEND[tk];

                        {
                            Obj mY(&testAllocator);
                            stretchRemoveAll(&mY, N);
                            const Obj& Y = mY;       gg(&mY, SPEC);
                            LOOP3_ASSERT(SPEC, NUMBITS, N, X == Y);

                            Obj mZ(&testAllocator);  const Obj& Z = mZ;
                            BEGIN_BSLMA_EXCEPTION_TEST {
                                mZ = Y >> NUMBITS;
                            } END_BSLMA_EXCEPTION_TEST

                            const int BB = testAllocator.numBlocksTotal();
                            const int B = testAllocator.numBlocksInUse();

                            if (veryVerbose) { cout << "\t\t"; P_(N); P(Y); }
                            mY >>= NUMBITS;
                            if (veryVerbose) { cout << "\t\t\t"; P(Y); }

                            LOOP_ASSERT(SPEC, X.length() == Y.length());
                            int i;
                            for (i = 0; i < X.length() - NUMBITS; ++i) {
                                LOOP5_ASSERT(SPEC, i, NUMBITS, X[i + NUMBITS],
                                             Y[i], X[i + NUMBITS] == Y[i]);
                            }
                            for (; i < X.length(); ++i) {
                                LOOP3_ASSERT(SPEC, NUMBITS, N, 0 == Y[i]);
                            }

                            const int AA = testAllocator.numBlocksTotal();
                            const int A = testAllocator.numBlocksInUse();
                            ASSERT(BB == AA);
                            ASSERT(B  == A);

                            LOOP3_ASSERT(SPEC, NUMBITS, N, Y == Z);
                        }
                        {
                            Obj mY(&testAllocator);
                            stretchRemoveAll(&mY, N);
                            const Obj& Y = mY;       gg(&mY, SPEC);
                            LOOP3_ASSERT(SPEC, NUMBITS, N, X == Y);

                            Obj mZ(&testAllocator);  const Obj& Z = mZ;
                            BEGIN_BSLMA_EXCEPTION_TEST {
                                mZ = Y << NUMBITS;
                            } END_BSLMA_EXCEPTION_TEST

                            const int BB = testAllocator.numBlocksTotal();
                            const int B = testAllocator.numBlocksInUse();

                            if (veryVerbose) { cout << "\t\t"; P_(N); P(Y); }
                            mY <<= NUMBITS;
                            if (veryVerbose) { cout << "\t\t\t"; P(Y); }

                            LOOP_ASSERT(SPEC, X.length() == Y.length());
                            int i;
                            for (i = 0; i < NUMBITS; ++i) {
                                LOOP3_ASSERT(SPEC, NUMBITS, N, 0 == Y[i]);
                            }
                            for (; i < X.length(); ++i) {
                                LOOP3_ASSERT(SPEC, NUMBITS, N,
                                                       X[i - NUMBITS] == Y[i]);
                            }

                            const int AA = testAllocator.numBlocksTotal();
                            const int A = testAllocator.numBlocksInUse();
                            ASSERT(BB == AA);
                            ASSERT(B  == A);

                            LOOP3_ASSERT(SPEC, NUMBITS, N, Y == Z);
                        }
                    }
                }
            }
        }
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // TESTING ROTATE METHODS
        //   The concerns for each of the methods are:
        //     - the correct result is obtained
        //     - does not depend upon internal representation
        //     - no allocations are performed
        //
        // Plan:
        //   For each of a sequence of test vectors, verify that the methods
        //   give the expected value and do not allocate memory.  For each
        //   explicit test vector, a number of vectors are created with the
        //   same value but different internal representation.  For each of
        //   these vectors, the number of bits to rotate is selected from an
        //   array.
        //
        // Testing:
        //   void rotateLeft(int numBits);
        //   void rotateRight(int numBits);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "Testing Rotate Methods" << endl
            << "======================" << endl;

        if (verbose) cout <<
            "\nTesting 'rotateLeft(numBits)' and 'rotateRight(numBits)'"
                          << endl;
        {
            static const char *SPECS[] = {
                "0",      "01",     "011",    "0110",   "01100",
                "0110001",         "01100011",         "011000110",
                "011000110001100", "0110001100011000", "01100011000110001",
                "0000100011100001001100000100110",
                "00001000111000010011000001001101",
           "000010001110000100110000010011010000100011100001001100000100110",
           "0000100011100001001100000100110100001000111000010011000001001100",
           "00001000111000010011000001001101000010001110000100110000010011001",
            0}; // Null string required as last element.

            static const int ROTATE[] = {
                0, 1, 2, 3, 5, 16, 31, 32, 64, 65
            };
            const int NUM_ROTATE = sizeof ROTATE / sizeof *ROTATE;

            static const int EXTEND[] = {
                0, 1, 2, 4, 8, 16, 32, 64, 100
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC = SPECS[ti];
                const int curLen = (int)strlen(SPEC);

                if (verbose) {
                    cout << "\tFor an object of length " << curLen << ":\t";
                    P(SPEC);
                }
                LOOP_ASSERT(SPEC, oldLen < curLen);  // strictly increasing
                oldLen = curLen;

                const Obj X = g(SPEC);                  // control
                LOOP_ASSERT(ti, curLen == X.length());  // same lengths

                for (int tj = 0; tj < NUM_ROTATE; ++tj) {
                    const int NUMBITS = ROTATE[tj];

                    if (X.length() < NUMBITS) { break; }

                    for (int tk = 0; tk < NUM_EXTEND; ++tk) {
                        const int N = EXTEND[tk];

                        BEGIN_BSLMA_EXCEPTION_TEST {
                            const int AL = testAllocator.allocationLimit();
                            testAllocator.setAllocationLimit(-1);
                            Obj mY(&testAllocator);
                            stretchRemoveAll(&mY, N);
                            const Obj& Y = mY;       gg(&mY, SPEC);
                            LOOP3_ASSERT(SPEC, NUMBITS, N, X == Y);
                            testAllocator.setAllocationLimit(AL);

                            if (veryVerbose) { cout << "\t\t"; P_(N);
                                                           P_(NUMBITS); P(Y); }
                            mY.rotateRight(NUMBITS);
                            if (veryVerbose) { cout << "\t\t\t"; P(Y); }

                            LOOP_ASSERT(SPEC, X.length() == Y.length());
                            for (int i = 0; i < X.length(); ++i) {
                                LOOP3_ASSERT(SPEC, NUMBITS, N,
                                        X[(i + NUMBITS) % X.length()] == Y[i]);
                            }

                            mY.rotateLeft(NUMBITS);
                            if (veryVerbose) { cout << "\t\t\t"; P(Y); }
                            LOOP4_ASSERT(SPEC, NUMBITS, X, Y, X == Y);
                        } END_BSLMA_EXCEPTION_TEST
                    }
                }
            }
        }
      } break;
      case 18: {
        // -------------------------------------------------------------------
        // TESTING TOGGLE METHODS AND NOT OPERATOR
        //   The concerns for each of the methods are:
        //     - the correct result is obtained
        //     - no allocations are performed
        //     - the "Duff's Device"-like implementation is fully tested
        //     - does not depend upon internal representation
        //
        // Plan:
        //   For each of a sequence of test vectors, verify that the methods
        //   give the expected value and do not allocate memory.  The test
        //   vectors are selected to exercise each case of the
        //   "Duff's Device"-like implementation.  For each explicit test
        //   vector, a number of vectors are created with the same value but
        //   different internal representation.  From these vectors, the index
        //   and value used in the method are exhaustively tested.
        //
        // Testing:
        //   void toggleAll();
        //   void toggle(int index);
        //   void toggle(int index, int numBits);
        //   operator~(const bdea_BitArray& bitArray);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "Testing Toggle Methods and Not Operator" << endl
            << "=======================================" << endl;

        if (verbose) cout <<
            "\nTesting 'toggleAll' and 'operator~'" << endl;
        {
            static const char *SPECS[] = {
                "",      "0",      "01",     "011",    "0110",   "01100",
                "0110001",         "01100011",         "011000110",
                "011000110001100", "0110001100011000", "01100011000110001",
                "0000100011100001001100000100110",
                "00001000111000010011000001001101",
           "000010001110000100110000010011010000100011100001001100000100110",
           "0000100011100001001100000100110100001000111000010011000001001100",
           "00001000111000010011000001001101000010001110000100110000010011001",
                LONG_SPEC_3,
                LONG_SPEC_4,
                LONG_SPEC_5,
                LONG_SPEC_6,
                LONG_SPEC_7,
                LONG_SPEC_8,
                LONG_SPEC_9,
            0}; // Null string required as last element.

            static const int EXTEND[] = {
                0, 1, 2, 3, 4, 5, 8, 16, 31, 32, 33, 63, 64, 65, 100
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC = SPECS[ti];
                const int curLen = (int)strlen(SPEC);

                if (verbose) {
                    cout << "\tFor an object of length " << curLen << ":\t";
                    P(SPEC);
                }
                LOOP_ASSERT(SPEC, oldLen < curLen);  // strictly increasing
                oldLen = curLen;

                const Obj X = g(SPEC);                  // control
                LOOP_ASSERT(ti, curLen == X.length());  // same lengths

                for (int tj = 0; tj < NUM_EXTEND; ++tj) {
                    {
                        const int N = EXTEND[tj];
                        Obj mY(&testAllocator);  stretchRemoveAll(&mY, N);
                        const Obj& Y = mY;       gg(&mY, SPEC);
                        LOOP2_ASSERT(SPEC, N, X == Y);
                        Obj mZ(&testAllocator);  stretchRemoveAll(&mZ, N);
                        const Obj& Z = mZ;       gg(&mZ, SPEC);
                        LOOP2_ASSERT(SPEC, N, X == Y);

                        const int B = testAllocator.numBlocksInUse();

                        if (veryVerbose) { cout << "\t\t"; P_(N); P(Y); }

                        mZ = ~X;

                        mY.toggleAll();
                        LOOP_ASSERT(ti, X.length() == Y.length());
                        for (int i = 0; i < X.length(); ++i) {
                            LOOP_ASSERT(ti, X[i] != Y[i]);
                        }
                        LOOP_ASSERT(ti, Y == Z);

                        mY.toggleAll();
                        LOOP_ASSERT(ti, X == Y);

                        const int A = testAllocator.numBlocksInUse();
                        ASSERT(B  == A);
                    }
                }
            }
        }

        if (verbose) cout <<
            "\nTesting 'toggle(index)', and 'toggle(index, numBits)'"
                          << endl;
        {
            static const char *SPECS[] = {
//                 "",      "0",      "01",     "011",    "0110",   "01100",
                "0110001",
//                 "01100011",         "011000110",
//                 "011000110001100", "0110001100011000", "01100011000110001",
//                 "0000100011100001001100000100110",
//                 "00001000111000010011000001001101",
//         "000010001110000100110000010011010000100011100001001100000100110",
//         "0000100011100001001100000100110100001000111000010011000001001100",
//         "00001000111000010011000001001101000010001110000100110000010011001",
            0}; // Null string required as last element.

            static const int EXTEND[] = {
                0, 1,
//                 1, 2, 3, 4, 5, 8, 16, 31, 32, 33, 63, 64, 65, 100
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC = SPECS[ti];
                const int curLen = (int)strlen(SPEC);

                if (verbose) {
                    cout << "\tFor an object of length " << curLen << ":\t";
                    P(SPEC);
                }
                LOOP_ASSERT(SPEC, oldLen < curLen);  // strictly increasing
                oldLen = curLen;

                const Obj X = g(SPEC);                  // control
                LOOP_ASSERT(ti, curLen == X.length());  // same lengths

                for (int tj = 0; tj < NUM_EXTEND; ++tj) {
                    {
                        const int N = EXTEND[tj];
                        Obj mY(&testAllocator);  stretchRemoveAll(&mY, N);
                        const Obj& Y = mY;       gg(&mY, SPEC);
                        LOOP2_ASSERT(SPEC, N, X == Y);
                        const int BB = testAllocator.numBlocksTotal();
                        const int B = testAllocator.numBlocksInUse();

                        if (veryVerbose) { cout << "\t\t"; P_(N); P(Y); }

                        for (int idx = 0; idx < X.length(); ++idx) {
                            mY.toggle(idx);
                            LOOP_ASSERT(ti, X.length() == Y.length());
                            LOOP_ASSERT(ti, X[idx] != Y[idx]);

                            mY.toggle(idx);
                            LOOP_ASSERT(ti, X == Y);
                        }

                        const int AA = testAllocator.numBlocksTotal();
                        const int A = testAllocator.numBlocksInUse();
                        ASSERT(BB == AA);
                        ASSERT(B  == A);
                    }
                    {
                        for (int idx = 0; idx < X.length(); ++idx) {
                            for (int nb = 0; nb < X.length() - idx; ++nb) {
                                const int N = EXTEND[tj];
                                Obj mY(&testAllocator);
                                stretchRemoveAll(&mY, N);
                                const Obj& Y = mY;       gg(&mY, SPEC);
                                LOOP2_ASSERT(SPEC, N, X == Y);
                                const int BB = testAllocator.numBlocksTotal();
                                const int B = testAllocator.numBlocksInUse();

                                if (veryVerbose) {
                                    cout << "\t\t"; P_(N); P(X) P(Y);
                                    P(idx) P(nb)
                                }

                                mY.toggle(idx, nb);

                                if (veryVerbose) {
                                    cout << "\t\t"; P(Y);
                                }

                                int i;
                                for (i = 0; i < idx; ++i) {
                                    ASSERT(X[i] == Y[i]);
                                }
                                for (; i < idx + nb; ++i) {
                                    ASSERT(X[i] != Y[i]);
                                }
                                for (; i < X.length(); ++i) {
                                    ASSERT(X[i] == Y[i]);
                                }

                                mY.toggle(idx, nb);
                                ASSERT(X == Y);

                                const int AA = testAllocator.numBlocksTotal();
                                const int A = testAllocator.numBlocksInUse();
                                ASSERT(BB == AA);
                                ASSERT(B  == A);
                            }
                        }
                    }
                }
            }
        }
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING SET METHODS
        //   The concerns for each of the methods are:
        //     - the correct result is obtained
        //     - no allocations are performed
        //     - the "Duff's Device"-like implementation is fully tested
        //     - does not depend upon internal representation
        //
        // Plan:
        //   For each of a sequence of test vectors, verify that the methods
        //   give the expected value and do not allocate memory.  The test
        //   vectors are selected to exercise each case of the
        //   "Duff's Device"-like implementation.  For each explicit test
        //   vector, a number of vectors are created with the same value but
        //   different internal representation.  From these vectors, the index
        //   and value used in the method are exhaustively tested.
        //
        // Testing:
        //   void setAll1();
        //   void set1(int index);
        //   void set1(int index, int numBits);
        //   void setAll0();
        //   void set0(int index);
        //   void set0(int index, int numBits);
        //   void set(int index, bool value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Set Methods" << endl
                          << "===================" << endl;

        if (verbose) cout << "\nTesting 'setAll0' and 'setAll1'" << endl;
        {
            static const char *SPECS[] = {
                "",      "0",      "01",     "011",    "0110",   "01100",
                "0110001",         "01100011",         "011000110",
                "011000110001100", "0110001100011000", "01100011000110001",
                "0000100011100001001100000100110",
                "00001000111000010011000001001101",
           "000010001110000100110000010011010000100011100001001100000100110",
           "0000100011100001001100000100110100001000111000010011000001001100",
           "00001000111000010011000001001101000010001110000100110000010011001",
                LONG_SPEC_3,
                LONG_SPEC_4,
                LONG_SPEC_5,
                LONG_SPEC_6,
                LONG_SPEC_7,
                LONG_SPEC_8,
                LONG_SPEC_9,
            0}; // Null string required as last element.

            static const int EXTEND[] = {
                0, 1, 2, 3, 4, 5, 8, 16, 31, 32, 33, 63, 64, 65, 100
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC = SPECS[ti];
                const int curLen = (int)strlen(SPEC);

                if (verbose) {
                    cout << "\tFor an object of length " << curLen << ":\t";
                    P(SPEC);
                }
                LOOP_ASSERT(SPEC, oldLen < curLen);  // strictly increasing
                oldLen = curLen;

                const Obj X = g(SPEC);                  // control
                LOOP_ASSERT(ti, curLen == X.length());  // same lengths

                for (int tj = 0; tj < NUM_EXTEND; ++tj) {
                    {
                        const int N = EXTEND[tj];
                        Obj mY(&testAllocator);  stretchRemoveAll(&mY, N);
                        const Obj& Y = mY;       gg(&mY, SPEC);
                        LOOP2_ASSERT(SPEC, N, X == Y);
                        const int BB = testAllocator.numBlocksTotal();
                        const int B = testAllocator.numBlocksInUse();

                        if (veryVerbose) { cout << "\t\t"; P_(N); P(Y); }

                        mY.setAll1();
                        LOOP_ASSERT(ti, X.length() == Y.length());
                        LOOP4_ASSERT(ti, X.length(), Y, Y.numSet1(),
                                     X.length() == Y.numSet1());

                        const int AA = testAllocator.numBlocksTotal();
                        const int A = testAllocator.numBlocksInUse();
                        ASSERT(BB == AA);
                        ASSERT(B  == A);
                    }
                    {
                        const int N = EXTEND[tj];
                        Obj mY(&testAllocator);  stretchRemoveAll(&mY, N);
                        const Obj& Y = mY;       gg(&mY, SPEC);
                        LOOP2_ASSERT(SPEC, N, X == Y);
                        const int BB = testAllocator.numBlocksTotal();
                        const int B = testAllocator.numBlocksInUse();

                        if (veryVerbose) { cout << "\t\t"; P_(N); P(Y); }

                        mY.setAll0();
                        LOOP_ASSERT(ti, X.length() == Y.length());
                        LOOP4_ASSERT(ti, X.length(), Y, Y.numSet0(),
                                     X.length() == Y.numSet0());

                        const int AA = testAllocator.numBlocksTotal();
                        const int A = testAllocator.numBlocksInUse();
                        ASSERT(BB == AA);
                        ASSERT(B  == A);
                    }
                }
            }
        }

        if (verbose) cout <<
            "\nTesting 'set?(index)', and 'set?(index, numBits)'"
                          << endl;
        {
            static const char *SPECS[] = {
                "",      "0",      "01",     "011",    "0110",   "01100",
                "0110001",         "01100011",         "011000110",
                "011000110001100", "0110001100011000", "01100011000110001",
                "0000100011100001001100000100110",
                "00001000111000010011000001001101",
           "000010001110000100110000010011010000100011100001001100000100110",
           "0000100011100001001100000100110100001000111000010011000001001100",
           "00001000111000010011000001001101000010001110000100110000010011001",
            0}; // Null string required as last element.

            static const int EXTEND[] = {
                0, 1, 2, 3, 4, 5, 8, 16, 31, 32, 33, 63, 64, 65, 100
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC = SPECS[ti];
                const int curLen = (int)strlen(SPEC);

                if (verbose) {
                    cout << "\tFor an object of length " << curLen << ":\t";
                    P(SPEC);
                }
                LOOP_ASSERT(SPEC, oldLen < curLen);  // strictly increasing
                oldLen = curLen;

                const Obj X = g(SPEC);                  // control
                LOOP_ASSERT(ti, curLen == X.length());  // same lengths

                for (int tj = 0; tj < NUM_EXTEND; ++tj) {
                    {
                        const int N = EXTEND[tj];
                        Obj mY(&testAllocator);  stretchRemoveAll(&mY, N);
                        const Obj& Y = mY;       gg(&mY, SPEC);
                        LOOP2_ASSERT(SPEC, N, X == Y);
                        const int BB = testAllocator.numBlocksTotal();
                        const int B = testAllocator.numBlocksInUse();

                        if (veryVerbose) { cout << "\t\t"; P_(N); P(Y); }

                        for (int idx = 0; idx < X.length(); ++idx) {
                            const int val = Y[idx];
                            const int num0ExcIdx = Y.numSet0() - (0 == val);
                            const int num1ExcIdx = Y.numSet1() - (1 == val);

                            mY.set1(idx);
                            LOOP_ASSERT(ti, X.length() == Y.length());
                            LOOP5_ASSERT(ti, idx, num0ExcIdx, Y, Y.numSet0(),
                                         num0ExcIdx == Y.numSet0());
                            LOOP4_ASSERT(ti, 1 + num1ExcIdx, Y, Y.numSet1(),
                                         1 + num1ExcIdx == Y.numSet1());

                            mY.set0(idx);
                            LOOP_ASSERT(ti, X.length() == Y.length());
                            LOOP_ASSERT(ti, 1 + num0ExcIdx == Y.numSet0());
                            LOOP_ASSERT(ti,     num1ExcIdx == Y.numSet1());

                            mY.set1(idx);
                            LOOP_ASSERT(ti, X.length() == Y.length());
                            LOOP_ASSERT(ti,     num0ExcIdx == Y.numSet0());
                            LOOP_ASSERT(ti, 1 + num1ExcIdx == Y.numSet1());

                            mY.set(idx, 0);
                            LOOP_ASSERT(ti, X.length() == Y.length());
                            LOOP_ASSERT(ti, 1 + num0ExcIdx == Y.numSet0());
                            LOOP_ASSERT(ti,     num1ExcIdx == Y.numSet1());

                            mY.set(idx, 1);
                            LOOP_ASSERT(ti, X.length() == Y.length());
                            LOOP_ASSERT(ti,     num0ExcIdx == Y.numSet0());
                            LOOP_ASSERT(ti, 1 + num1ExcIdx == Y.numSet1());

                            mY.set(idx, 0);
                            LOOP_ASSERT(ti, X.length() == Y.length());
                            LOOP_ASSERT(ti, 1 + num0ExcIdx == Y.numSet0());
                            LOOP_ASSERT(ti,     num1ExcIdx == Y.numSet1());

                            mY.set(idx, 2);
                            LOOP_ASSERT(ti, X.length() == Y.length());
                            LOOP_ASSERT(ti,     num0ExcIdx == Y.numSet0());
                            LOOP_ASSERT(ti, 1 + num1ExcIdx == Y.numSet1());

                            mY.set(idx, 0);
                            LOOP_ASSERT(ti, X.length() == Y.length());
                            LOOP_ASSERT(ti, 1 + num0ExcIdx == Y.numSet0());
                            LOOP_ASSERT(ti,     num1ExcIdx == Y.numSet1());

                            mY.set(idx, -1);
                            LOOP_ASSERT(ti, X.length() == Y.length());
                            LOOP_ASSERT(ti,     num0ExcIdx == Y.numSet0());
                            LOOP_ASSERT(ti, 1 + num1ExcIdx == Y.numSet1());

                            mY.set(idx, val);
                            LOOP_ASSERT(ti, X.length() == Y.length());
                            LOOP_ASSERT(ti, val == Y[idx]);
                        }

                        const int AA = testAllocator.numBlocksTotal();
                        const int A = testAllocator.numBlocksInUse();
                        ASSERT(BB == AA);
                        ASSERT(B  == A);
                    }
                    {
                        for (int idx = 0; idx < X.length(); ++idx) {
                            for (int nb = 0; nb < X.length() - idx; ++nb) {
                                const int N = EXTEND[tj];
                                Obj mY(&testAllocator);
                                stretchRemoveAll(&mY, N);
                                const Obj& Y = mY;       gg(&mY, SPEC);
                                LOOP2_ASSERT(SPEC, N, X == Y);
                                const int BB = testAllocator.numBlocksTotal();
                                const int B = testAllocator.numBlocksInUse();

                                if (veryVerbose) {
                                    cout << "\t\t"; P_(N); P(Y);
                                }

                                mY.set0(idx, nb);
                                int i;
                                for (i = 0; i < idx; ++i) {
                                    ASSERT(X[i] == Y[i]);
                                }
                                for (; i < idx + nb; ++i) {
                                    ASSERT(0 == Y[i]);
                                }
                                for (; i < X.length(); ++i) {
                                    ASSERT(X[i] == Y[i]);
                                }

                                const int AA = testAllocator.numBlocksTotal();
                                const int A = testAllocator.numBlocksInUse();
                                ASSERT(BB == AA);
                                ASSERT(B  == A);
                            }
                        }
                    }
                    {
                        for (int idx = 0; idx < X.length(); ++idx) {
                            for (int nb = 0; nb < X.length() - idx; ++nb) {
                                const int N = EXTEND[tj];
                                Obj mY(&testAllocator);
                                stretchRemoveAll(&mY, N);
                                const Obj& Y = mY;       gg(&mY, SPEC);
                                LOOP2_ASSERT(SPEC, N, X == Y);
                                const int BB = testAllocator.numBlocksTotal();
                                const int B = testAllocator.numBlocksInUse();

                                if (veryVerbose) {
                                    cout << "\t\t"; P_(N); P(Y);
                                }

                                mY.set1(idx, nb);
                                int i;
                                for (i = 0; i < idx; ++i) {
                                    ASSERT(X[i] == Y[i]);
                                }
                                for (; i < idx + nb; ++i) {
                                    ASSERT(1 == Y[i]);
                                }
                                for (; i < X.length(); ++i) {
                                    ASSERT(X[i] == Y[i]);
                                }

                                const int AA = testAllocator.numBlocksTotal();
                                const int A = testAllocator.numBlocksInUse();
                                ASSERT(BB == AA);
                                ASSERT(B  == A);
                            }
                        }
                    }
                }
            }
        }
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING CAPACITY-RESERVING CONSTRUCTOR AND METHOD
        //   The concerns are as follows:
        //    1) capacity-reserving constructor:
        //       a) The initial value is correct (empty).
        //       b) The initial capacity is correct.
        //       c) The constructor is exception neutral w.r.t. allocation.
        //       d) The internal memory management system is hooked up properly
        //          so that *all* internally allocated memory draws from a
        //          user-supplied allocator whenever one is specified.
        //    2) 'reserveCapacityRaw' method:
        //       a) The resulting value is correct (unchanged).
        //       b) The resulting capacity is correct (not less than initial).
        //       c) The method is exception neutral w.r.t. allocation.
        //    3) 'reserveCapacity' method:
        //       a) The resulting value is correct (unchanged).
        //       b) The resulting capacity is correct (not less than initial).
        //       c) The method is exception neutral w.r.t. allocation.
        //       d) The resulting value is unchanged in the event of
        //          exceptions.
        //       e) The amount of memory allocated is current * 2^N
        //
        // Plan:
        //   In a loop, use the capacity-reserving constructor to create empty
        //   objects with increasing initial capacity.  Verify that each object
        //   has the same value as a control default object.  Then, append as
        //   many values as the requested initial capacity, and use
        //   'bslma_TestAllocator' to verify that no additional allocations
        //   have occurred.  Perform each test in the standard 'bdema'
        //   exception-testing macro block.
        //
        //   Repeat the constructor test initially specifying no allocator and
        //   again, specifying a static buffer allocator.  These tests (without
        //   specifying a 'bslma_TestAllocator') cannot confirm correct
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
        //   specified number of elements, and use 'bslma_TestAllocator' to
        //   verify that no additional allocations have occurred.  Perform each
        //   test in the standard 'bdema' exception-testing macro block.
        //
        // Testing:
        //   bdea_BitArray(const InitialCapacity& ne, *ba = 0);
        //   bdea_BitArray(const InitialCapacity& ne, int iLen, *ba = 0);
        //   bdea_BitArray(const InitialCapacity& ne, iLen, value, *ba = 0);
        //   void reserveCapacityRaw(int ne);
        //   void reserveCapacity(int ne);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "Testing Capacity Reserving Constructor and Methods" << endl
            << "==================================================" << endl;

        if (verbose) cout <<
            "\nTesting 'bdea_BitArray(capacity, ba)' Constructor" << endl;
        if (verbose) cout << "\twith a 'bslma_TestAllocator':" << endl;
        {
            const Obj W(&testAllocator);  // control value
            const int MAX_NUM_ELEMS = 70;
            for (int ne = 0; ne <= MAX_NUM_ELEMS; ++ne) {
                if (veryVerbose) { cout << "\t\t"; P(ne) }
              BEGIN_BSLMA_EXCEPTION_TEST {
                const Obj::InitialCapacity NE(ne);
                Obj mX(NE, &testAllocator);  const Obj &X = mX;
                LOOP_ASSERT(ne, W == X);
                const int NUM_BLOCKS = testAllocator.numBlocksTotal();
                const int NUM_BYTES  = testAllocator.numBytesInUse();
                if (veryVerbose) P_(X);
                for (int i = 0; i < ne; ++i) {
                    mX.append(0);
                }
                if (veryVerbose) P(X);
                LOOP_ASSERT(ne, NUM_BLOCKS == testAllocator.numBlocksTotal());
                LOOP_ASSERT(ne, NUM_BYTES  == testAllocator.numBytesInUse());
              } END_BSLMA_EXCEPTION_TEST
            }
        }

        if (verbose) cout << "\twith no allocator (exercise only):" << endl;
        {
            const Obj W(&testAllocator);  // control value
            const int MAX_NUM_ELEMS = 70;
            for (int ne = 0; ne <= MAX_NUM_ELEMS; ++ne) {
                if (veryVerbose) { cout << "\t\t"; P(ne) }
                const Obj::InitialCapacity NE(ne);
                Obj mX(NE);  const Obj &X = mX;
                LOOP_ASSERT(ne, W == X);
                if (veryVerbose) P_(X);
                for (int i = 0; i < ne; ++i) {
                    mX.append(0);
                }
                if (veryVerbose) P(X);
            }
        }

        if (verbose)
            cout << "\twith a buffer allocator (exercise only):" << endl;
        {
            char memory[8192];
            bdema_BufferedSequentialAllocator a(memory, sizeof memory);
            const Obj W(&testAllocator);  // control value
            const int MAX_NUM_ELEMS = 70;
            for (int ne = 0; ne <= MAX_NUM_ELEMS; ++ne) {
                if (veryVerbose) { cout << "\t\t"; P(ne) }
                const Obj::InitialCapacity NE(ne);
                Obj *doNotDelete =
                    new(a.allocate(sizeof(Obj))) Obj(NE, &a);
                Obj &mX = *doNotDelete;  const Obj &X = mX;
                LOOP_ASSERT(ne, W == X);
                if (veryVerbose) P_(X);
                for (int i = 0; i < ne; ++i) {
                    mX.append(0);
                }
                if (veryVerbose) P(X);
            }
             // No destructor is called; will produce memory leak in purify
             // if internal allocators are not hooked up properly.
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout <<
          "\nTesting initial-capacity ctor with user-specified initial length."
                          << endl;
        if (verbose) cout << "\tWithout passing in an allocator." << endl;
        {
            const int MAX_NUM_ELEMS = 70;
            const int MAX_LENGTH = 100;
            for (int ne = 0; ne <= MAX_NUM_ELEMS; ++ne) {
                if (veryVerbose) { cout << "\t\t"; P(ne) }
                const Obj::InitialCapacity NE(ne);
                for (int length = 0; length <= MAX_LENGTH; ++length) {
                    if (verbose) P(length);
                    bslma_TestAllocator ga(veryVeryVerbose);
                    bslma_DefaultAllocatorGuard dag(&ga);
                    Obj mX(NE, length);  const Obj &X = mX;
                    const int NUM_BYTES = ga.numBytesInUse();
                    if (veryVerbose) P(X);
                    LOOP2_ASSERT(ne, length, length == X.length());
                    mX.setLength(ne);
                    LOOP_ASSERT(ne, NUM_BYTES == ga.numBytesInUse());
                    LOOP_ASSERT(ne, ne == X.length());
                }
            }
        }

        if (verbose) cout << "\tPassing in an allocator." << endl;

        if (verbose) cout << "\t\tWith no exceptions." << endl;
        {
            const int MAX_NUM_ELEMS = 70;
            const int MAX_LENGTH = 100;
            for (int ne = 0; ne <= MAX_NUM_ELEMS; ++ne) {
                if (veryVerbose) { cout << "\t\t"; P(ne) }
                const Obj::InitialCapacity NE(ne);
                for (int length = 0; length <= MAX_LENGTH; ++length) {
                    if (verbose) P(length);
                    bslma_TestAllocator ta(veryVeryVerbose);
                    Obj mX(NE, length, &ta);  const Obj &X = mX;
                    const int NUM_BYTES = ta.numBytesInUse();
                    if (veryVerbose) P(X);
                    LOOP2_ASSERT(ne, length, length == X.length());
                    mX.setLength(ne);
                    LOOP_ASSERT(ne, NUM_BYTES == ta.numBytesInUse());
                    LOOP_ASSERT(ne, ne == X.length());
                }
            }
        }

        if (verbose) cout << "\t\tWith exceptions." << endl;
        {
            const int MAX_NUM_ELEMS = 70;
            const int MAX_LENGTH = 100;
            for (int ne = 0; ne <= MAX_NUM_ELEMS; ++ne) {
                if (veryVerbose) { cout << "\t\t"; P(ne) }
                const Obj::InitialCapacity NE(ne);
                for (int length = 0; length <= MAX_LENGTH; ++length) {
                    if (verbose) P(length);
                    bslma_TestAllocator testAllocator(veryVeryVerbose);
                    bslma_TestAllocator& ta = testAllocator;

                  BEGIN_BSLMA_EXCEPTION_TEST {
                    Obj mX(NE, length, &ta);  const Obj &X = mX;
                    const int NUM_BYTES = ta.numBytesInUse();
                    if (veryVerbose) P(X);
                    LOOP2_ASSERT(ne, length, length == X.length());
                    mX.setLength(ne);
                    LOOP_ASSERT(ne, NUM_BYTES == ta.numBytesInUse());
                    LOOP_ASSERT(ne, ne == X.length());
                  } END_BSLMA_EXCEPTION_TEST
                }
            }
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\nTesting initial-capacity ctor with "
                          << "user-specified initial length and value" << endl;
        if (verbose) cout << "\tWithout passing in an allocator." << endl;
        {
            const int     MAX_NUM_ELEMS = 70;
            const int     MAX_LENGTH    = 100;
            const Element VALUE         = 1;
            for (int ne = 0; ne <= MAX_NUM_ELEMS; ++ne) {
                if (veryVerbose) { cout << "\t\t"; P(ne) }
                const Obj::InitialCapacity NE(ne);
                for (int length = 0; length <= MAX_LENGTH; ++length) {
                    if (verbose) P(length);
                    bslma_TestAllocator ga(veryVeryVerbose);
                    bslma_DefaultAllocatorGuard dag(&ga);
                    Obj mX(NE, length, VALUE);  const Obj &X = mX;
                    const int NUM_BYTES = ga.numBytesInUse();
                    if (veryVerbose) P(X);
                    LOOP2_ASSERT(ne, length, length == X.length());
                    for (int i = 0; i < length; ++i) {
                        LOOP2_ASSERT(length, i, VALUE == X[i]);
                    }
                    mX.setLength(ne, VALUE);
                    LOOP_ASSERT(ne, NUM_BYTES == ga.numBytesInUse());
                    LOOP_ASSERT(ne, ne == X.length());
                    for (int i = 0; i < ne; ++i) {
                        LOOP2_ASSERT(ne, i, VALUE == X[i]);
                    }
                }
            }
        }

        if (verbose) cout << "\tPassing in an allocator." << endl;

        if (verbose) cout << "\t\tWith no exceptions." << endl;
        {
            const int     MAX_NUM_ELEMS = 70;
            const int     MAX_LENGTH    = 100;
            const Element VALUE         = 1;
            for (int ne = 0; ne <= MAX_NUM_ELEMS; ++ne) {
                if (veryVerbose) { cout << "\t\t"; P(ne) }
                const Obj::InitialCapacity NE(ne);
                for (int length = 0; length <= MAX_LENGTH; ++length) {
                    if (verbose) P(length);
                    bslma_TestAllocator ta(veryVeryVerbose);
                    Obj mX(NE, length, VALUE, &ta);  const Obj &X = mX;
                    const int NUM_BYTES = ta.numBytesInUse();
                    if (veryVerbose) P(X);
                    LOOP2_ASSERT(ne, length, length == X.length());
                    for (int i = 0; i < length; ++i) {
                        LOOP2_ASSERT(length, i, VALUE == X[i]);
                    }
                    mX.setLength(ne, VALUE);
                    LOOP_ASSERT(ne, NUM_BYTES == ta.numBytesInUse());
                    LOOP_ASSERT(ne, ne == X.length());
                    for (int i = 0; i < ne; ++i) {
                        LOOP2_ASSERT(ne, i, VALUE == X[i]);
                    }
                }
            }
        }

        if (verbose) cout << "\t\tWith exceptions." << endl;
        {
            const int     MAX_NUM_ELEMS = 70;
            const int     MAX_LENGTH    = 100;
            const Element VALUE         = 1;
            for (int ne = 0; ne <= MAX_NUM_ELEMS; ++ne) {
                if (veryVerbose) { cout << "\t\t"; P(ne) }
                const Obj::InitialCapacity NE(ne);
                for (int length = 0; length <= MAX_LENGTH; ++length) {
                    if (verbose) P(length);
                    bslma_TestAllocator testAllocator(veryVeryVerbose);
                    bslma_TestAllocator& ta = testAllocator;

                  BEGIN_BSLMA_EXCEPTION_TEST {
                    Obj mX(NE, length, VALUE, &ta);  const Obj &X = mX;
                    const int NUM_BYTES = ta.numBytesInUse();
                    if (veryVerbose) P(X);
                    LOOP2_ASSERT(ne, length, length == X.length());
                    for (int i = 0; i < length; ++i) {
                        LOOP2_ASSERT(length, i, VALUE == X[i]);
                    }
                    mX.setLength(ne, VALUE);
                    LOOP_ASSERT(ne, NUM_BYTES == ta.numBytesInUse());
                    LOOP_ASSERT(ne, ne == X.length());
                    for (int i = 0; i < ne; ++i) {
                        LOOP2_ASSERT(ne, i, VALUE == X[i]);
                    }
                  } END_BSLMA_EXCEPTION_TEST
                }
            }
        }

        // Test 'reserveCapacityRaw' and 'reserveCapacity' methods.

        static const struct {
            int         d_lineNum;  // source line number
            const char *d_x;        // initial value
            int         d_ne;       // total number of elements to reserve
        } DATA[] = {
                //line  initialValue   numElements
                //----  ------------   -----------
                { L_,   "",               0        },
                { L_,   "",               1        },
                { L_,   "",               2        },
                { L_,   "",               3        },
                { L_,   "",               4        },
                { L_,   "",               5        },
                { L_,   "",              15        },
                { L_,   "",              16        },
                { L_,   "",              17        },

                { L_,   "0",              0        },
                { L_,   "0",              1        },
                { L_,   "0",              2        },
                { L_,   "0",              3        },
                { L_,   "0",              4        },
                { L_,   "0",              5        },
                { L_,   "0",             15        },
                { L_,   "0",             16        },
                { L_,   "0",             17        },
                { L_,   "0",             32        },
                { L_,   "0",             33        },
                { L_,   "0",             64        },
                { L_,   "0",             65        },
                { L_,   "0",             96        },
                { L_,   "0",             97        },
                { L_,   "0",            128        },
                { L_,   "0",            129        },

                { L_,   "01",             0        },
                { L_,   "01",             1        },
                { L_,   "01",             2        },
                { L_,   "01",             3        },
                { L_,   "01",             4        },
                { L_,   "01",             5        },
                { L_,   "01",            15        },
                { L_,   "01",            16        },
                { L_,   "01",            17        },
                { L_,   "01",            32        },
                { L_,   "01",            33        },
                { L_,   "01",            64        },
                { L_,   "01",            65        },
                { L_,   "01",            96        },
                { L_,   "01",            97        },
                { L_,   "01",           128        },
                { L_,   "01",           129        },

                { L_,   "01100",          0        },
                { L_,   "01100",          1        },
                { L_,   "01100",          2        },
                { L_,   "01100",          3        },
                { L_,   "01100",          4        },
                { L_,   "01100",          5        },
                { L_,   "01100",         15        },
                { L_,   "01100",         16        },
                { L_,   "01100",         17        },
                { L_,   "01100",         32        },
                { L_,   "01100",         33        },
                { L_,   "01100",         64        },
                { L_,   "01100",         65        },
                { L_,   "01100",         96        },
                { L_,   "01100",         97        },
                { L_,   "01100",        128        },
                { L_,   "01100",        129        },

        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose)
            cout << "\nTesting the 'reserveCapacityRaw' method" << endl;
        {
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
              BEGIN_BSLMA_EXCEPTION_TEST {
                const int   LINE = DATA[ti].d_lineNum;
                const char *SPEC = DATA[ti].d_x;
                const int   NE   = DATA[ti].d_ne;
                if (veryVerbose) { cout << "\t\t"; P_(SPEC); P(NE); }

                const Obj W(g(SPEC), &testAllocator);
                Obj mX(W, &testAllocator);  const Obj &X = mX;
                mX.reserveCapacityRaw(NE);
                LOOP_ASSERT(LINE, W == X);
                const int NUM_BLOCKS = testAllocator.numBlocksTotal();
                const int NUM_BYTES  = testAllocator.numBytesInUse();
                if (veryVerbose) P_(X);
                for (int i = X.length(); i < NE; ++i) {
                    mX.append(0);
                }
                if (veryVerbose) P(X);
                LOOP_ASSERT(LINE, NUM_BLOCKS== testAllocator.numBlocksTotal());
                LOOP_ASSERT(LINE, NUM_BYTES == testAllocator.numBytesInUse());
              } END_BSLMA_EXCEPTION_TEST
            }
        }

        if (verbose)
           cout << "\nTesting the 'reserveCapacity' method" << endl;
        {
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
              BEGIN_BSLMA_EXCEPTION_TEST {
                const int   LINE    = DATA[ti].d_lineNum;
                const char *SPEC    = DATA[ti].d_x;
                const int   NE      = DATA[ti].d_ne;
                if (veryVerbose) {
                        cout << "\t\t"; P_(SPEC); P(NE);
                }

                const Obj W(g(SPEC), &testAllocator);
                Obj mX(W, &testAllocator); const Obj &X = mX;
                {
                   // Verify that X does not change after exceptions in
                   // reserveCapacity by comparing it to the control W.

                   EqualityTester chX(&X, &W);

                   mX.reserveCapacity(NE);
                }
                LOOP_ASSERT(LINE, W == X);
                const int NUM_BLOCKS = testAllocator.numBlocksTotal();
                const int NUM_BYTES  = testAllocator.numBytesInUse();

                if (veryVerbose) P_(X);
                for (int i = X.length(); i < NE; ++i) {
                    mX.append(0);
                }
                if (veryVerbose) P(X);
                LOOP_ASSERT(LINE, NUM_BLOCKS== testAllocator.numBlocksTotal());
                LOOP_ASSERT(LINE, NUM_BYTES == testAllocator.numBytesInUse());
              } END_BSLMA_EXCEPTION_TEST
            }
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
        //
        // Plan:
        //   For each object X in a set of objects ordered by increasing length
        //   L containing uniform values, V0.
        //     For each valid index position P1 in [0 .. L-1]:
        //       For each valid index position P2 in [0 .. L-1]:
        //         1.  Create a test object T from X using the copy ctor.
        //         2.  Replace the element at P1 with V1 and at P2 with V2.
        //         3.  Swap these elements in T.
        //         4.  Verify that
        //               (i)     V2 == T[P1]     always
        //         5.  Verify that
        //               (ii)    V1 == T[P2]     if (P1 != P2)
        //                       V2 == T[P2]     if (P1 == P2)
        //         6.  For each index position, i, in [0 .. L-1] verify that:
        //               (iii)   V0 == T[i]      if (P1 != i && P2 != i)
        //
        // Testing:
        //   void swap(int index1, int index2);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'swap' Method" << endl
                          << "=====================" << endl;

        if (verbose) cout << "\nTesting swap(index1, index2)" << endl;

        const int NUM_TRIALS = 70;

        Obj mX(&testAllocator);  const Obj& X = mX;  // control

        for (int iLen = 0; iLen < NUM_TRIALS; ++iLen) { // iLen: initial length
            if (verbose) { cout << "\t"; P_(iLen); P(X); }
            for (int pos1 = 0; pos1 < iLen; ++pos1) { // for each position
                for (int pos2 = 0; pos2 < iLen; ++pos2) { // for each position
                    Obj mT(X, &testAllocator);
                    const Obj& T = mT;                  // object under test
                    LOOP3_ASSERT(iLen, pos1, pos2, X == T);

                    mT.set(pos1, 1);
                    mT.set(pos2, 0);
                    if (veryVerbose) { cout << "\t\t";  P_(pos1);
                                       P_(pos2);        P(T); }
                    mT.swap(pos1, pos2);
                    if (veryVerbose) { cout << "\t\t";  P_(pos1);
                                       P_(pos2);        P(T); }

                    LOOP3_ASSERT(iLen, pos1, pos2, 0 == T[pos1]);

                    const Element& VX = pos1 == pos2 ? 0 : 1;
                    LOOP3_ASSERT(iLen, pos1, pos2, VX == T[pos2]);

                    for (int i = 0; i < iLen; ++i) {
                        if (i == pos1 || i == pos2) continue;
                        LOOP4_ASSERT(iLen, pos1, pos2, i,
                                     (i % 3 > 0) == T[i]);
                    }
                }
            }
            if (veryVerbose) cout <<
                "\t--------------------------------------------------" << endl;
            mX.append(iLen % 3);  // Extend control with cyclic values.
        }

      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING REPLACE
        //   Concerns:
        //     For the 'replace' method, the following properties must hold:
        //       1) The source is left unaffected (apart from aliasing).
        //       2) The subsequent existence of the source has no effect on the
        //          result object (apart from aliasing).
        //       3) The function is alias-safe.
        //       4) The function preserves object invariants.
        //     Note that the 'replace' methods cannot allocate, but is tested
        //     for exceptions anyway.                                 // ADJUST
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
        //       BSLMA test assert macros and use gg as an optimization.
        // Testing:
        //   void replace(int di, bool value);
        //   void replace(int di, const bdea_BitArray& sa, int si, int ne);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                << "Testing 'replace'" << endl
                << "=================" << endl;

        if (verbose) cout <<
            "\nTesting replace(di, sa, si, ne) et al.(no aliasing)" << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_daSpec;   // initial (destination) array
                int         d_di;       // index at which to replace into da
                const char *d_saSpec;   // source array
                int         d_si;       // index at which to replace from sa
                int         d_ne;       // number of elements to replace
                const char *d_expSpec;  // expected array value
            } DATA[] = {
                //line  d-array di   s-array si  ne  expected
                //----  ------- --   ------- --  --  --------   Depth = 0
                { L_,   "",      0,  "",      0,  0, ""      },

                //line  d-array di   s-array si  ne  expected
                //----  ------- --   ------- --  --  --------   Depth = 1
                { L_,   "0",     0,  "",      0,  0, "0"     },
                { L_,   "0",     1,  "",      0,  0, "0"     },

                { L_,   "",      0,  "1",     0,  0, ""      },
                { L_,   "",      0,  "1",     1,  0, ""      },

                //line  d-array di   s-array si  ne  expected
                //----  ------- --   ------- --  --  --------   Depth = 2
                { L_,   "01",    0,  "",      0,  0, "01"    },
                { L_,   "01",    1,  "",      0,  0, "01"    },
                { L_,   "01",    2,  "",      0,  0, "01"    },

                { L_,   "0",     0,  "1",     0,  0, "0"     },
                { L_,   "0",     0,  "1",     0,  1, "1"     },
                { L_,   "0",     0,  "1",     1,  0, "0"     },
                { L_,   "0",     1,  "1",     0,  0, "0"     },
                { L_,   "0",     1,  "1",     1,  0, "0"     },

                { L_,   "",      0,  "01",    0,  0, ""      },
                { L_,   "",      0,  "01",    1,  0, ""      },
                { L_,   "",      0,  "01",    2,  0, ""      },

                //line  d-array di   s-array si  ne  expected
                //----  ------- --   ------- --  --  --------   Depth = 3
                { L_,   "011",   0,  "",      0,  0, "011"   },
                { L_,   "011",   1,  "",      0,  0, "011"   },
                { L_,   "011",   2,  "",      0,  0, "011"   },
                { L_,   "011",   3,  "",      0,  0, "011"   },

                { L_,   "01",    0,  "1",     0,  0, "01"    },
                { L_,   "01",    0,  "1",     0,  1, "11"    },
                { L_,   "01",    0,  "1",     1,  0, "01"    },
                { L_,   "01",    1,  "1",     0,  0, "01"    },
                { L_,   "01",    1,  "1",     0,  1, "01"    },
                { L_,   "01",    1,  "1",     1,  0, "01"    },
                { L_,   "01",    2,  "1",     0,  0, "01"    },
                { L_,   "01",    2,  "1",     1,  0, "01"    },

                { L_,   "0",     0,  "11",    0,  0, "0"     },
                { L_,   "0",     0,  "11",    0,  1, "1"     },
                { L_,   "0",     0,  "11",    1,  0, "0"     },
                { L_,   "0",     0,  "11",    1,  1, "1"     },
                { L_,   "0",     0,  "11",    2,  0, "0"     },
                { L_,   "0",     1,  "11",    0,  0, "0"     },
                { L_,   "0",     1,  "11",    1,  0, "0"     },
                { L_,   "0",     1,  "11",    2,  0, "0"     },

                { L_,   "",      0,  "011",   0,  0, ""      },
                { L_,   "",      0,  "011",   1,  0, ""      },
                { L_,   "",      0,  "011",   2,  0, ""      },
                { L_,   "",      0,  "011",   3,  0, ""      },

                //line  d-array di   s-array si  ne  expected
                //----  ------- --   ------- --  --  --------   Depth = 4
                { L_,   "0110",  0,  "",      0,  0, "0110"  },
                { L_,   "0110",  1,  "",      0,  0, "0110"  },
                { L_,   "0110",  2,  "",      0,  0, "0110"  },
                { L_,   "0110",  3,  "",      0,  0, "0110"  },
                { L_,   "0110",  4,  "",      0,  0, "0110"  },

                { L_,   "011",   0,  "0",     0,  0, "011"   },
                { L_,   "011",   0,  "0",     0,  1, "011"   },
                { L_,   "011",   0,  "0",     1,  0, "011"   },
                { L_,   "011",   1,  "0",     0,  0, "011"   },
                { L_,   "011",   1,  "0",     0,  1, "001"   },
                { L_,   "011",   1,  "0",     1,  0, "011"   },
                { L_,   "011",   2,  "0",     0,  0, "011"   },
                { L_,   "011",   2,  "0",     0,  1, "010"   },
                { L_,   "011",   2,  "0",     1,  0, "011"   },
                { L_,   "011",   3,  "0",     0,  0, "011"   },
                { L_,   "011",   3,  "0",     1,  0, "011"   },

                { L_,   "01",    0,  "10",    0,  0, "01"    },
                { L_,   "01",    0,  "10",    0,  1, "11"    },
                { L_,   "01",    0,  "10",    0,  2, "10"    },
                { L_,   "01",    0,  "10",    1,  0, "01"    },
                { L_,   "01",    0,  "10",    1,  1, "01"    },
                { L_,   "01",    0,  "10",    2,  0, "01"    },
                { L_,   "01",    1,  "10",    0,  0, "01"    },
                { L_,   "01",    1,  "10",    0,  1, "01"    },
                { L_,   "01",    1,  "10",    1,  0, "01"    },
                { L_,   "01",    1,  "10",    1,  1, "00"    },
                { L_,   "01",    1,  "10",    2,  0, "01"    },
                { L_,   "01",    2,  "10",    0,  0, "01"    },
                { L_,   "01",    2,  "10",    1,  0, "01"    },
                { L_,   "01",    2,  "10",    2,  0, "01"    },

                { L_,   "0",     0,  "110",   0,  0, "0"     },
                { L_,   "0",     0,  "110",   0,  1, "1"     },
                { L_,   "0",     0,  "110",   1,  0, "0"     },
                { L_,   "0",     0,  "110",   1,  1, "1"     },
                { L_,   "0",     0,  "110",   2,  0, "0"     },
                { L_,   "0",     0,  "110",   2,  1, "0"     },
                { L_,   "0",     0,  "110",   3,  0, "0"     },
                { L_,   "0",     1,  "110",   0,  0, "0"     },
                { L_,   "0",     1,  "110",   1,  0, "0"     },
                { L_,   "0",     1,  "110",   2,  0, "0"     },
                { L_,   "0",     1,  "110",   3,  0, "0"     },

                { L_,   "",      0,  "0110",  0,  0, ""      },
                { L_,   "",      0,  "0110",  1,  0, ""      },
                { L_,   "",      0,  "0110",  2,  0, ""      },
                { L_,   "",      0,  "0110",  3,  0, ""      },
                { L_,   "",      0,  "0110",  4,  0, ""      },

                //line  d-array di   s-array si  ne  expected
                //----  ------- --   ------- --  --  --------   Depth = 5
                { L_,   "01100", 0,  "",      0,  0, "01100" },
                { L_,   "01100", 1,  "",      0,  0, "01100" },
                { L_,   "01100", 2,  "",      0,  0, "01100" },
                { L_,   "01100", 3,  "",      0,  0, "01100" },
                { L_,   "01100", 4,  "",      0,  0, "01100" },
                { L_,   "01100", 5,  "",      0,  0, "01100" },

                { L_,   "0110",  0,  "0",     0,  0, "0110"  },
                { L_,   "0110",  0,  "0",     0,  1, "0110"  },
                { L_,   "0110",  0,  "0",     1,  0, "0110"  },
                { L_,   "0110",  1,  "0",     0,  0, "0110"  },
                { L_,   "0110",  1,  "0",     0,  1, "0010"  },
                { L_,   "0110",  1,  "0",     1,  0, "0110"  },
                { L_,   "0110",  2,  "0",     0,  0, "0110"  },
                { L_,   "0110",  2,  "0",     0,  1, "0100"  },
                { L_,   "0110",  2,  "0",     1,  0, "0110"  },
                { L_,   "0110",  3,  "0",     0,  0, "0110"  },
                { L_,   "0110",  3,  "0",     0,  1, "0110"  },
                { L_,   "0110",  3,  "0",     1,  0, "0110"  },
                { L_,   "0110",  4,  "0",     0,  0, "0110"  },
                { L_,   "0110",  4,  "0",     1,  0, "0110"  },

                { L_,   "011",   0,  "00",    0,  0, "011"   },
                { L_,   "011",   0,  "00",    0,  1, "011"   },
                { L_,   "011",   0,  "00",    0,  2, "001"   },
                { L_,   "011",   0,  "00",    1,  0, "011"   },
                { L_,   "011",   0,  "00",    1,  1, "011"   },
                { L_,   "011",   0,  "00",    2,  0, "011"   },
                { L_,   "011",   1,  "00",    0,  0, "011"   },
                { L_,   "011",   1,  "00",    0,  1, "001"   },
                { L_,   "011",   1,  "00",    0,  2, "000"   },
                { L_,   "011",   1,  "00",    1,  0, "011"   },
                { L_,   "011",   1,  "00",    1,  1, "001"   },
                { L_,   "011",   1,  "00",    2,  0, "011"   },
                { L_,   "011",   2,  "00",    0,  0, "011"   },
                { L_,   "011",   2,  "00",    0,  1, "010"   },
                { L_,   "011",   2,  "00",    1,  0, "011"   },
                { L_,   "011",   2,  "00",    1,  1, "010"   },
                { L_,   "011",   2,  "00",    2,  0, "011"   },
                { L_,   "011",   3,  "00",    0,  0, "011"   },
                { L_,   "011",   3,  "00",    1,  0, "011"   },
                { L_,   "011",   3,  "00",    2,  0, "011"   },

                { L_,   "01",    0,  "100",   0,  0, "01"    },
                { L_,   "01",    0,  "100",   0,  1, "11"    },
                { L_,   "01",    0,  "100",   0,  2, "10"    },
                { L_,   "01",    0,  "100",   1,  0, "01"    },
                { L_,   "01",    0,  "100",   1,  1, "01"    },
                { L_,   "01",    0,  "100",   2,  0, "01"    },
                { L_,   "01",    0,  "100",   2,  1, "01"    },
                { L_,   "01",    0,  "100",   3,  0, "01"    },
                { L_,   "01",    1,  "100",   0,  0, "01"    },
                { L_,   "01",    1,  "100",   0,  1, "01"    },
                { L_,   "01",    1,  "100",   1,  0, "01"    },
                { L_,   "01",    1,  "100",   1,  1, "00"    },
                { L_,   "01",    1,  "100",   2,  0, "01"    },
                { L_,   "01",    1,  "100",   2,  1, "00"    },
                { L_,   "01",    1,  "100",   3,  0, "01"    },
                { L_,   "01",    2,  "100",   0,  0, "01"    },
                { L_,   "01",    2,  "100",   1,  0, "01"    },
                { L_,   "01",    2,  "100",   2,  0, "01"    },
                { L_,   "01",    2,  "100",   3,  0, "01"    },

                { L_,   "0",     0,  "1100",  0,  0, "0"     },
                { L_,   "0",     0,  "1100",  0,  1, "1"     },
                { L_,   "0",     0,  "1100",  1,  0, "0"     },
                { L_,   "0",     0,  "1100",  1,  1, "1"     },
                { L_,   "0",     0,  "1100",  2,  0, "0"     },
                { L_,   "0",     0,  "1100",  2,  1, "0"     },
                { L_,   "0",     0,  "1100",  3,  0, "0"     },
                { L_,   "0",     0,  "1100",  3,  1, "0"     },
                { L_,   "0",     0,  "1100",  4,  0, "0"     },
                { L_,   "0",     1,  "1100",  0,  0, "0"     },
                { L_,   "0",     1,  "1100",  1,  0, "0"     },
                { L_,   "0",     1,  "1100",  2,  0, "0"     },
                { L_,   "0",     1,  "1100",  3,  0, "0"     },
                { L_,   "0",     1,  "1100",  4,  0, "0"     },

                { L_,   "",      0,  "01100", 0,  0, ""      },
                { L_,   "",      0,  "01100", 1,  0, ""      },
                { L_,   "",      0,  "01100", 2,  0, ""      },
                { L_,   "",      0,  "01100", 3,  0, ""      },
                { L_,   "",      0,  "01100", 4,  0, ""      },
                { L_,   "",      0,  "01100", 5,  0, ""      },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldDepth = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int   LINE   = DATA[ti].d_lineNum;
                const char *D_SPEC = DATA[ti].d_daSpec;
                const int   DI     = DATA[ti].d_di;
                const char *S_SPEC = DATA[ti].d_saSpec;
                const int   SI     = DATA[ti].d_si;
                const int   NE     = DATA[ti].d_ne;
                const char *E_SPEC = DATA[ti].d_expSpec;

                const int   DEPTH  = (int) strlen(D_SPEC) + strlen(S_SPEC);
                if (DEPTH > oldDepth) {
                    oldDepth = DEPTH;
                    if (verbose) { cout << '\t';  P(DEPTH); }
                }

                Obj DD(g(D_SPEC));              // control for destination
                Obj SS(g(S_SPEC));              // control for source
                Obj EE(g(E_SPEC));              // control for expected value

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
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    Obj x(DD, &testAllocator);  const Obj &X = x;
                    {
                        Obj s(SS, &testAllocator);  const Obj &S = s;
                        if (veryVerbose) { outerP("\t\t\tBEFORE: ", X); }
                        x.replace(DI, s, SI, NE);       // source non-'const'
                        if (veryVerbose) { outerP("\t\t\t AFTER: ", X); }
                        LOOP_ASSERT(LINE, EE == X);
                        LOOP_ASSERT(LINE, SS == S);     // source unchanged?
                    }
                    LOOP_ASSERT(LINE, EE == X);  // source is out of scope
                  } END_BSLMA_EXCEPTION_TEST
                }

                if (veryVerbose)
                               cout << "\t\treplace(di, value)" << endl;
                if (1 == NE) {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    Obj x(DD, &testAllocator);  const Obj &X = x;
                    {
                        Obj s(SS, &testAllocator);  const Obj &S = s;
                        if (veryVerbose) { outerP("\t\t\tBEFORE: ", X); }
                        x.replace(DI, s[SI]);           // source non-'const'
                        if (veryVerbose) { outerP("\t\t\t AFTER: ", X); }
                        LOOP_ASSERT(LINE, EE == X);
                        LOOP_ASSERT(LINE, SS == S);     // source unchanged?
                    }
                    LOOP_ASSERT(LINE, EE == X);  // source is out of scope
                  } END_BSLMA_EXCEPTION_TEST
                }

                if (veryVerbose) {
                    cout << "\t\t\t---------- WHITE BOX ----------" << endl;
                }

                Obj x(&testAllocator);  const Obj &X = x;

                const int STRETCH_SIZE = 50;

                stretchRemoveAll(&x, STRETCH_SIZE);

                const int NUM_BLOCKS = testAllocator.numBlocksTotal();
                const int NUM_BYTES  = testAllocator.numBytesInUse();

                if (veryVerbose) cout << "\t\treplace(di, sa, si, ne)" << endl;
                {
                    x.removeAll();  gg(&x, D_SPEC);
                    if (veryVerbose) { outerP("\t\t\tBEFORE: ", X); }
                    x.replace(DI, SS, SI, NE);
                    if (veryVerbose) { outerP("\t\t\t AFTER: ", X); }
                    LOOP_ASSERT(LINE, EE == X);
                }

                if (veryVerbose) cout << "\t\treplace(di, value)" << endl;
                if (1 == NE) {
                    x.removeAll();  gg(&x, D_SPEC);
                    if (veryVerbose) { outerP("\t\t\tBEFORE: ", X); }
                    x.replace(DI, SS[SI]);
                    if (veryVerbose) { outerP("\t\t\t AFTER: ", X); }
                    LOOP_ASSERT(LINE, EE == X);
                }

                LOOP_ASSERT(LINE, NUM_BLOCKS ==testAllocator.numBlocksTotal());
                LOOP_ASSERT(LINE, NUM_BYTES == testAllocator.numBytesInUse());
            }
        }

        //---------------------------------------------------------------------

        if (verbose) cout <<
            "\nTesting x.replace(di, sa, si, ne) et al.(aliasing)" << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_xSpec;    // initial array (= srcArray)
                int         d_di;       // index at which to insert into x
                int         d_si;       // index at which to insert from x
                int         d_ne;       // number of elements to insert
                const char *d_expSpec;  // expected array value
            } DATA[] = {
                //line  x-array di   si  ne  expected
                //----  ------- --   --  --  --------   Depth = 0
                { L_,   "",      0,  0,  0,  ""      },

                //line  x-array di   si  ne  expected
                //----  ------- --   --  --  --------   Depth = 1
                { L_,   "0",     0,  0,  0,  "0"     },
                { L_,   "0",     0,  0,  1,  "0"     },
                { L_,   "0",     0,  1,  0,  "0"     },

                { L_,   "0",     1,  0,  0,  "0"     },
                { L_,   "0",     1,  1,  0,  "0"     },

                //line  x-array di   si  ne  expected
                //----  ------- --   --  --  --------   Depth = 2
                { L_,   "01",    0,  0,  0,  "01"    },
                { L_,   "01",    0,  0,  1,  "01"    },
                { L_,   "01",    0,  0,  2,  "01"    },
                { L_,   "01",    0,  1,  0,  "01"    },
                { L_,   "01",    0,  1,  1,  "11"    },
                { L_,   "01",    0,  2,  0,  "01"    },

                { L_,   "01",    1,  0,  0,  "01"    },
                { L_,   "01",    1,  0,  1,  "00"    },
                { L_,   "01",    1,  1,  0,  "01"    },
                { L_,   "01",    1,  1,  1,  "01"    },
                { L_,   "01",    1,  2,  0,  "01"    },

                { L_,   "01",    2,  0,  0,  "01"    },
                { L_,   "01",    2,  1,  0,  "01"    },
                { L_,   "01",    2,  2,  0,  "01"    },

                //line  x-array di   si  ne  expected
                //----  ------- --   --  --  --------   Depth = 3
                { L_,   "011",   0,  0,  0,  "011"   },
                { L_,   "011",   0,  0,  1,  "011"   },
                { L_,   "011",   0,  0,  2,  "011"   },
                { L_,   "011",   0,  0,  3,  "011"   },
                { L_,   "011",   0,  1,  0,  "011"   },
                { L_,   "011",   0,  1,  1,  "111"   },
                { L_,   "011",   0,  1,  2,  "111"   },
                { L_,   "011",   0,  2,  0,  "011"   },
                { L_,   "011",   0,  2,  1,  "111"   },
                { L_,   "011",   0,  3,  0,  "011"   },

                { L_,   "011",   1,  0,  0,  "011"   },
                { L_,   "011",   1,  0,  1,  "001"   },
                { L_,   "011",   1,  0,  2,  "001"   },
                { L_,   "011",   1,  1,  0,  "011"   },
                { L_,   "011",   1,  1,  1,  "011"   },
                { L_,   "011",   1,  1,  2,  "011"   },
                { L_,   "011",   1,  2,  0,  "011"   },
                { L_,   "011",   1,  2,  1,  "011"   },
                { L_,   "011",   1,  3,  0,  "011"   },

                { L_,   "011",   2,  0,  0,  "011"   },
                { L_,   "011",   2,  0,  1,  "010"   },
                { L_,   "011",   2,  1,  0,  "011"   },
                { L_,   "011",   2,  1,  1,  "011"   },
                { L_,   "011",   2,  2,  0,  "011"   },
                { L_,   "011",   2,  2,  1,  "011"   },
                { L_,   "011",   2,  3,  0,  "011"   },

                { L_,   "011",   3,  0,  0,  "011"   },
                { L_,   "011",   3,  1,  0,  "011"   },
                { L_,   "011",   3,  2,  0,  "011"   },
                { L_,   "011",   3,  3,  0,  "011"   },

                //line  x-array di   si  ne  expected
                //----  ------- --   --  --  --------   Depth = 4
                { L_,   "0110",  0,  0,  0,  "0110"  },
                { L_,   "0110",  0,  0,  1,  "0110"  },
                { L_,   "0110",  0,  0,  2,  "0110"  },
                { L_,   "0110",  0,  0,  3,  "0110"  },
                { L_,   "0110",  0,  0,  4,  "0110"  },
                { L_,   "0110",  0,  1,  0,  "0110"  },
                { L_,   "0110",  0,  1,  1,  "1110"  },
                { L_,   "0110",  0,  1,  2,  "1110"  },
                { L_,   "0110",  0,  1,  3,  "1100"  },
                { L_,   "0110",  0,  2,  0,  "0110"  },
                { L_,   "0110",  0,  2,  1,  "1110"  },
                { L_,   "0110",  0,  2,  2,  "1010"  },
                { L_,   "0110",  0,  3,  0,  "0110"  },
                { L_,   "0110",  0,  3,  1,  "0110"  },
                { L_,   "0110",  0,  4,  0,  "0110"  },

                { L_,   "0110",  1,  0,  0,  "0110"  },
                { L_,   "0110",  1,  0,  1,  "0010"  },
                { L_,   "0110",  1,  0,  2,  "0010"  },
                { L_,   "0110",  1,  0,  3,  "0011"  },
                { L_,   "0110",  1,  1,  0,  "0110"  },
                { L_,   "0110",  1,  1,  1,  "0110"  },
                { L_,   "0110",  1,  1,  2,  "0110"  },
                { L_,   "0110",  1,  1,  3,  "0110"  },
                { L_,   "0110",  1,  2,  0,  "0110"  },
                { L_,   "0110",  1,  2,  1,  "0110"  },
                { L_,   "0110",  1,  2,  2,  "0100"  },
                { L_,   "0110",  1,  3,  0,  "0110"  },
                { L_,   "0110",  1,  3,  1,  "0010"  },
                { L_,   "0110",  1,  4,  0,  "0110"  },

                { L_,   "0110",  2,  0,  0,  "0110"  },
                { L_,   "0110",  2,  0,  1,  "0100"  },
                { L_,   "0110",  2,  0,  2,  "0101"  },
                { L_,   "0110",  2,  1,  0,  "0110"  },
                { L_,   "0110",  2,  1,  1,  "0110"  },
                { L_,   "0110",  2,  1,  2,  "0111"  },
                { L_,   "0110",  2,  2,  0,  "0110"  },
                { L_,   "0110",  2,  2,  1,  "0110"  },
                { L_,   "0110",  2,  2,  2,  "0110"  },
                { L_,   "0110",  2,  3,  0,  "0110"  },
                { L_,   "0110",  2,  3,  1,  "0100"  },
                { L_,   "0110",  2,  4,  0,  "0110"  },

                { L_,   "0110",  3,  0,  0,  "0110"  },
                { L_,   "0110",  3,  0,  1,  "0110"  },
                { L_,   "0110",  3,  1,  0,  "0110"  },
                { L_,   "0110",  3,  1,  1,  "0111"  },
                { L_,   "0110",  3,  2,  0,  "0110"  },
                { L_,   "0110",  3,  2,  1,  "0111"  },
                { L_,   "0110",  3,  3,  0,  "0110"  },
                { L_,   "0110",  3,  3,  1,  "0110"  },
                { L_,   "0110",  3,  4,  0,  "0110"  },

                { L_,   "0110",  4,  0,  0,  "0110"  },
                { L_,   "0110",  4,  1,  0,  "0110"  },
                { L_,   "0110",  4,  2,  0,  "0110"  },
                { L_,   "0110",  4,  3,  0,  "0110"  },
                { L_,   "0110",  4,  4,  0,  "0110"  },

                //line  x-array di   si  ne  expected
                //----  ------- --   --  --  --------   Depth = 5
                { L_,   "01100", 0,  0,  0,  "01100" },
                { L_,   "01100", 0,  0,  1,  "01100" },
                { L_,   "01100", 0,  0,  2,  "01100" },
                { L_,   "01100", 0,  0,  3,  "01100" },
                { L_,   "01100", 0,  0,  4,  "01100" },
                { L_,   "01100", 0,  0,  5,  "01100" },
                { L_,   "01100", 0,  1,  0,  "01100" },
                { L_,   "01100", 0,  1,  1,  "11100" },
                { L_,   "01100", 0,  1,  2,  "11100" },
                { L_,   "01100", 0,  1,  3,  "11000" },
                { L_,   "01100", 0,  1,  4,  "11000" },
                { L_,   "01100", 0,  2,  0,  "01100" },
                { L_,   "01100", 0,  2,  1,  "11100" },
                { L_,   "01100", 0,  2,  2,  "10100" },
                { L_,   "01100", 0,  2,  3,  "10000" },
                { L_,   "01100", 0,  3,  0,  "01100" },
                { L_,   "01100", 0,  3,  1,  "01100" },
                { L_,   "01100", 0,  3,  2,  "00100" },
                { L_,   "01100", 0,  4,  0,  "01100" },
                { L_,   "01100", 0,  4,  1,  "01100" },
                { L_,   "01100", 0,  5,  0,  "01100" },

                { L_,   "01100", 1,  0,  0,  "01100" },
                { L_,   "01100", 1,  0,  1,  "00100" },
                { L_,   "01100", 1,  0,  2,  "00100" },
                { L_,   "01100", 1,  0,  3,  "00110" },
                { L_,   "01100", 1,  0,  4,  "00110" },
                { L_,   "01100", 1,  1,  0,  "01100" },
                { L_,   "01100", 1,  1,  1,  "01100" },
                { L_,   "01100", 1,  1,  2,  "01100" },
                { L_,   "01100", 1,  1,  3,  "01100" },
                { L_,   "01100", 1,  1,  4,  "01100" },
                { L_,   "01100", 1,  2,  0,  "01100" },
                { L_,   "01100", 1,  2,  1,  "01100" },
                { L_,   "01100", 1,  2,  2,  "01000" },
                { L_,   "01100", 1,  2,  3,  "01000" },
                { L_,   "01100", 1,  3,  0,  "01100" },
                { L_,   "01100", 1,  3,  1,  "00100" },
                { L_,   "01100", 1,  3,  2,  "00000" },
                { L_,   "01100", 1,  4,  0,  "01100" },
                { L_,   "01100", 1,  4,  1,  "00100" },
                { L_,   "01100", 1,  5,  0,  "01100" },

                { L_,   "01100", 2,  0,  0,  "01100" },
                { L_,   "01100", 2,  0,  1,  "01000" },
                { L_,   "01100", 2,  0,  2,  "01010" },
                { L_,   "01100", 2,  0,  3,  "01011" },
                { L_,   "01100", 2,  1,  0,  "01100" },
                { L_,   "01100", 2,  1,  1,  "01100" },
                { L_,   "01100", 2,  1,  2,  "01110" },
                { L_,   "01100", 2,  1,  3,  "01110" },
                { L_,   "01100", 2,  2,  0,  "01100" },
                { L_,   "01100", 2,  2,  1,  "01100" },
                { L_,   "01100", 2,  2,  2,  "01100" },
                { L_,   "01100", 2,  2,  3,  "01100" },
                { L_,   "01100", 2,  3,  0,  "01100" },
                { L_,   "01100", 2,  3,  1,  "01000" },
                { L_,   "01100", 2,  3,  2,  "01000" },
                { L_,   "01100", 2,  4,  0,  "01100" },
                { L_,   "01100", 2,  4,  1,  "01000" },
                { L_,   "01100", 2,  5,  0,  "01100" },

                { L_,   "01100", 3,  0,  0,  "01100" },
                { L_,   "01100", 3,  0,  1,  "01100" },
                { L_,   "01100", 3,  0,  2,  "01101" },
                { L_,   "01100", 3,  1,  0,  "01100" },
                { L_,   "01100", 3,  1,  1,  "01110" },
                { L_,   "01100", 3,  1,  2,  "01111" },
                { L_,   "01100", 3,  2,  0,  "01100" },
                { L_,   "01100", 3,  2,  1,  "01110" },
                { L_,   "01100", 3,  2,  2,  "01110" },
                { L_,   "01100", 3,  3,  0,  "01100" },
                { L_,   "01100", 3,  3,  1,  "01100" },
                { L_,   "01100", 3,  3,  2,  "01100" },
                { L_,   "01100", 3,  4,  0,  "01100" },
                { L_,   "01100", 3,  4,  1,  "01100" },
                { L_,   "01100", 3,  5,  0,  "01100" },

                { L_,   "01100", 4,  0,  0,  "01100" },
                { L_,   "01100", 4,  0,  1,  "01100" },
                { L_,   "01100", 4,  1,  0,  "01100" },
                { L_,   "01100", 4,  1,  1,  "01101" },
                { L_,   "01100", 4,  2,  0,  "01100" },
                { L_,   "01100", 4,  2,  1,  "01101" },
                { L_,   "01100", 4,  3,  0,  "01100" },
                { L_,   "01100", 4,  3,  1,  "01100" },
                { L_,   "01100", 4,  4,  0,  "01100" },
                { L_,   "01100", 4,  4,  1,  "01100" },
                { L_,   "01100", 4,  5,  0,  "01100" },

                { L_,   "01100", 5,  0,  0,  "01100" },
                { L_,   "01100", 5,  1,  0,  "01100" },
                { L_,   "01100", 5,  2,  0,  "01100" },
                { L_,   "01100", 5,  3,  0,  "01100" },
                { L_,   "01100", 5,  4,  0,  "01100" },
                { L_,   "01100", 5,  5,  0,  "01100" },

            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldDepth = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int   LINE   = DATA[ti].d_lineNum;
                const char *X_SPEC = DATA[ti].d_xSpec;
                const int   DI     = DATA[ti].d_di;
                const int   SI     = DATA[ti].d_si;
                const int   NE     = DATA[ti].d_ne;
                const char *E_SPEC = DATA[ti].d_expSpec;

                const int   DEPTH  = strlen(X_SPEC);
                if (DEPTH > oldDepth) {
                    oldDepth = DEPTH;
                    if (verbose) { cout << '\t';  P(DEPTH); }
                }

                Obj DD(g(X_SPEC));              // control for destination
                Obj EE(g(E_SPEC));              // control for expected value

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
                    Obj x2(DD, &testAllocator);  const Obj &X2 = x2; // control
                    x2.replace(DI, X, SI, NE);
                    LOOP_ASSERT(LINE, EE == X2);
                }

                if (veryVerbose) cout << "\t\treplace(di, sa, si, ne)" << endl;
                {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    Obj x(DD, &testAllocator);  const Obj &X = x;
                    if (veryVerbose) { outerP("\t\t\tBEFORE: ", X); }
                    x.replace(DI, X, SI, NE);
                    if (veryVerbose) { outerP("\t\t\t AFTER: ", X); }
                    LOOP_ASSERT(LINE, EE == X);
                  } END_BSLMA_EXCEPTION_TEST
                }

                if (veryVerbose) cout << "\t\treplace(di, value)" << endl;
                if (1 == NE) {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    Obj x(DD, &testAllocator);  const Obj &X = x;
                    if (veryVerbose) { outerP("\t\t\tBEFORE: ", X); }
                    x.replace(DI, X[SI]);
                    if (veryVerbose) { outerP("\t\t\t AFTER: ", X); }
                    LOOP_ASSERT(LINE, EE == X);
                  } END_BSLMA_EXCEPTION_TEST
                }

                if (veryVerbose) {
                    cout << "\t\t\t---------- WHITE BOX ----------" << endl;
                }

                Obj x(&testAllocator);  const Obj &X = x;

                const int STRETCH_SIZE = 50;

                stretchRemoveAll(&x, STRETCH_SIZE);

                if (veryVerbose) cout << "\t\treplace(di, sa, si, ne)" << endl;
                {
                    x.removeAll();  gg(&x, X_SPEC);
                    if (veryVerbose) { outerP("\t\t\tBEFORE: ", X); }
                    x.replace(DI, X, SI, NE);
                    if (veryVerbose) { outerP("\t\t\t AFTER: ", X); }
                    LOOP_ASSERT(LINE, EE == X);
                }

                if (veryVerbose) cout << "\t\treplace(di, value)" << endl;
                if (1 == NE) {
                    x.removeAll();  gg(&x, X_SPEC);
                    if (veryVerbose) { outerP("\t\t\tBEFORE: ", X); }
                    x.replace(DI, X[SI]);
                    if (veryVerbose) { outerP("\t\t\t AFTER: ", X); }
                    LOOP_ASSERT(LINE, EE == X);
                }
            }
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING APPEND, INSERT, REMOVE
        //   Concerns:
        //     For the 'append' and 'insert' methods, the following properties
        //     must hold:
        //       1) The source is left unaffected (apart from aliasing).
        //       2) The subsequent existing of the source has no effect on the
        //          result object (apart from aliasing).
        //       3) The function is alias-safe.
        //       4) The function is exception neutral (w.r.t. allocation).
        //       5) The function preserves object invariants.
        //       6) The function is independent of internal representation.
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
        //   (strlen(D_SPEC) == DI).  All methods using the entire source
        //   object use test data where the source length equals the number of
        //   elements (strlen(S_SPEC) == NE), while the "scalar" methods use
        //   data where the number of elements equals 1 (1 == NE).  In
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
        //       BSLMA test assert macros and use gg as an optimization.
        //
        // Testing:
        //   void append(bool value);
        //   void append(const bdea_BitArray& sa);
        //   void append(const bdea_BitArray& sa, int si, int ne);
        //
        //   void insert(int di, bool value);
        //   void insert(int di, const bdea_BitArray& sa);
        //   void insert(int di, const bdea_BitArray& sa, int si, int ne);
        //
        //   void remove(int index);
        //   void remove(int index, int ne);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                << "Testing 'append', 'insert', and 'remove'" << endl
                << "========================================" << endl;

        if (verbose) cout <<
            "\nTesting x.insert(di, sa, si, ne) et al. (no aliasing)" << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_daSpec;   // initial (destination) array
                int         d_di;       // index at which to insert into da
                const char *d_saSpec;   // source array
                int         d_si;       // index at which to insert from sa
                int         d_ne;       // number of elements to insert
                const char *d_expSpec;  // expected array value
            } DATA[] = {
                //line  d-array di   s-array si  ne  expected
                //----  ------- --   ------- --  --  --------   Depth = 0
                { L_,   "",      0,  "",      0,  0, ""      },

                //line  d-array di   s-array si  ne  expected
                //----  ------- --   ------- --  --  --------   Depth = 1
                { L_,   "0",     0,  "",      0,  0, "0"     },
                { L_,   "0",     1,  "",      0,  0, "0"     },

                { L_,   "",      0,  "0",     0,  0, ""      },
                { L_,   "",      0,  "0",     0,  1, "0"     },
                { L_,   "",      0,  "0",     1,  0, ""      },

                //line  d-array di   s-array si  ne  expected
                //----  ------- --   ------- --  --  --------   Depth = 2
                { L_,   "01",    0,  "",      0,  0, "01"    },
                { L_,   "01",    1,  "",      0,  0, "01"    },
                { L_,   "01",    2,  "",      0,  0, "01"    },

                { L_,   "0",     0,  "1",     0,  0, "0"     },
                { L_,   "0",     0,  "1",     0,  1, "10"    },
                { L_,   "0",     0,  "1",     1,  0, "0"     },
                { L_,   "0",     1,  "1",     0,  0, "0"     },
                { L_,   "0",     1,  "1",     0,  1, "01"    },
                { L_,   "0",     1,  "1",     1,  0, "0"     },

                { L_,   "",      0,  "01",    0,  0, ""      },
                { L_,   "",      0,  "01",    0,  1, "0"     },
                { L_,   "",      0,  "01",    0,  2, "01"    },
                { L_,   "",      0,  "01",    1,  0, ""      },
                { L_,   "",      0,  "01",    1,  1, "1"     },
                { L_,   "",      0,  "01",    2,  0, ""      },

                //line  d-array di   s-array si  ne  expected
                //----  ------- --   ------- --  --  --------   Depth = 3
                { L_,   "011",   0,  "",      0,  0, "011"   },
                { L_,   "011",   1,  "",      0,  0, "011"   },
                { L_,   "011",   2,  "",      0,  0, "011"   },
                { L_,   "011",   3,  "",      0,  0, "011"   },

                { L_,   "01",    0,  "1",     0,  0, "01"    },
                { L_,   "01",    0,  "1",     0,  1, "101"   },
                { L_,   "01",    0,  "1",     1,  0, "01"    },
                { L_,   "01",    1,  "1",     0,  0, "01"    },
                { L_,   "01",    1,  "1",     0,  1, "011"   },
                { L_,   "01",    1,  "1",     1,  0, "01"    },
                { L_,   "01",    2,  "1",     0,  0, "01"    },
                { L_,   "01",    2,  "1",     0,  1, "011"   },
                { L_,   "01",    2,  "1",     1,  0, "01"    },

                { L_,   "0",     0,  "11",    0,  0, "0"     },
                { L_,   "0",     0,  "11",    0,  1, "10"    },
                { L_,   "0",     0,  "11",    0,  2, "110"   },
                { L_,   "0",     0,  "11",    1,  0, "0"     },
                { L_,   "0",     0,  "11",    1,  1, "10"    },
                { L_,   "0",     0,  "11",    2,  0, "0"     },
                { L_,   "0",     1,  "11",    0,  0, "0"     },
                { L_,   "0",     1,  "11",    0,  1, "01"    },
                { L_,   "0",     1,  "11",    0,  2, "011"   },
                { L_,   "0",     1,  "11",    1,  0, "0"     },
                { L_,   "0",     1,  "11",    1,  1, "01"    },
                { L_,   "0",     1,  "11",    2,  0, "0"     },

                { L_,   "",      0,  "011",   0,  0, ""      },
                { L_,   "",      0,  "011",   0,  1, "0"     },
                { L_,   "",      0,  "011",   0,  2, "01"    },
                { L_,   "",      0,  "011",   0,  3, "011"   },
                { L_,   "",      0,  "011",   1,  0, ""      },
                { L_,   "",      0,  "011",   1,  1, "1"     },
                { L_,   "",      0,  "011",   1,  2, "11"    },
                { L_,   "",      0,  "011",   2,  0, ""      },
                { L_,   "",      0,  "011",   2,  1, "1"     },
                { L_,   "",      0,  "011",   3,  0, ""      },

                //line  d-array di   s-array si  ne  expected
                //----  ------- --   ------- --  --  --------   Depth = 4
                { L_,   "0110",  0,  "",      0,  0, "0110"  },
                { L_,   "0110",  1,  "",      0,  0, "0110"  },
                { L_,   "0110",  2,  "",      0,  0, "0110"  },
                { L_,   "0110",  3,  "",      0,  0, "0110"  },
                { L_,   "0110",  4,  "",      0,  0, "0110"  },

                { L_,   "011",   0,  "0",     0,  0, "011"   },
                { L_,   "011",   0,  "0",     0,  1, "0011"  },
                { L_,   "011",   0,  "0",     1,  0, "011"   },
                { L_,   "011",   1,  "0",     0,  0, "011"   },
                { L_,   "011",   1,  "0",     0,  1, "0011"  },
                { L_,   "011",   1,  "0",     1,  0, "011"   },
                { L_,   "011",   2,  "0",     0,  0, "011"   },
                { L_,   "011",   2,  "0",     0,  1, "0101"  },
                { L_,   "011",   2,  "0",     1,  0, "011"   },
                { L_,   "011",   3,  "0",     0,  0, "011"   },
                { L_,   "011",   3,  "0",     0,  1, "0110"  },
                { L_,   "011",   3,  "0",     1,  0, "011"   },

                { L_,   "01",    0,  "10",    0,  0, "01"    },
                { L_,   "01",    0,  "10",    0,  1, "101"   },
                { L_,   "01",    0,  "10",    0,  2, "1001"  },
                { L_,   "01",    0,  "10",    1,  0, "01"    },
                { L_,   "01",    0,  "10",    1,  1, "001"   },
                { L_,   "01",    0,  "10",    2,  0, "01"    },
                { L_,   "01",    1,  "10",    0,  0, "01"    },
                { L_,   "01",    1,  "10",    0,  1, "011"   },
                { L_,   "01",    1,  "10",    0,  2, "0101"  },
                { L_,   "01",    1,  "10",    1,  0, "01"    },
                { L_,   "01",    1,  "10",    1,  1, "001"   },
                { L_,   "01",    1,  "10",    2,  0, "01"    },
                { L_,   "01",    2,  "10",    0,  0, "01"    },
                { L_,   "01",    2,  "10",    0,  1, "011"   },
                { L_,   "01",    2,  "10",    0,  2, "0110"  },
                { L_,   "01",    2,  "10",    1,  0, "01"    },
                { L_,   "01",    2,  "10",    1,  1, "010"   },
                { L_,   "01",    2,  "10",    2,  0, "01"    },

                { L_,   "0",     0,  "110",   0,  0, "0"     },
                { L_,   "0",     0,  "110",   0,  1, "10"    },
                { L_,   "0",     0,  "110",   0,  2, "110"   },
                { L_,   "0",     0,  "110",   0,  3, "1100"  },
                { L_,   "0",     0,  "110",   1,  0, "0"     },
                { L_,   "0",     0,  "110",   1,  1, "10"    },
                { L_,   "0",     0,  "110",   1,  2, "100"   },
                { L_,   "0",     0,  "110",   2,  0, "0"     },
                { L_,   "0",     0,  "110",   2,  1, "00"    },
                { L_,   "0",     0,  "110",   3,  0, "0"     },
                { L_,   "0",     1,  "110",   0,  0, "0"     },
                { L_,   "0",     1,  "110",   0,  1, "01"    },
                { L_,   "0",     1,  "110",   0,  2, "011"   },
                { L_,   "0",     1,  "110",   0,  3, "0110"  },
                { L_,   "0",     1,  "110",   1,  0, "0"     },
                { L_,   "0",     1,  "110",   1,  1, "01"    },
                { L_,   "0",     1,  "110",   1,  2, "010"   },
                { L_,   "0",     1,  "110",   2,  0, "0"     },
                { L_,   "0",     1,  "110",   2,  1, "00"    },
                { L_,   "0",     1,  "110",   3,  0, "0"     },

                { L_,   "",      0,  "0110",  0,  0, ""      },
                { L_,   "",      0,  "0110",  0,  1, "0"     },
                { L_,   "",      0,  "0110",  0,  2, "01"    },
                { L_,   "",      0,  "0110",  0,  3, "011"   },
                { L_,   "",      0,  "0110",  0,  4, "0110"  },
                { L_,   "",      0,  "0110",  1,  0, ""      },
                { L_,   "",      0,  "0110",  1,  1, "1"     },
                { L_,   "",      0,  "0110",  1,  2, "11"    },
                { L_,   "",      0,  "0110",  1,  3, "110"   },
                { L_,   "",      0,  "0110",  2,  0, ""      },
                { L_,   "",      0,  "0110",  2,  1, "1"     },
                { L_,   "",      0,  "0110",  2,  2, "10"    },
                { L_,   "",      0,  "0110",  3,  0, ""      },
                { L_,   "",      0,  "0110",  3,  1, "0"     },
                { L_,   "",      0,  "0110",  4,  0, ""      },

                //line  d-array di   s-array si  ne  expected
                //----  ------- --   ------- --  --  --------   Depth = 5
                { L_,   "01100", 0,  "",      0,  0, "01100" },
                { L_,   "01100", 1,  "",      0,  0, "01100" },
                { L_,   "01100", 2,  "",      0,  0, "01100" },
                { L_,   "01100", 3,  "",      0,  0, "01100" },
                { L_,   "01100", 4,  "",      0,  0, "01100" },
                { L_,   "01100", 5,  "",      0,  0, "01100" },

                { L_,   "0110",  0,  "0",     0,  0, "0110"  },
                { L_,   "0110",  0,  "0",     0,  1, "00110" },
                { L_,   "0110",  0,  "0",     1,  0, "0110"  },
                { L_,   "0110",  1,  "0",     0,  0, "0110"  },
                { L_,   "0110",  1,  "0",     0,  1, "00110" },
                { L_,   "0110",  1,  "0",     1,  0, "0110"  },
                { L_,   "0110",  2,  "0",     0,  0, "0110"  },
                { L_,   "0110",  2,  "0",     0,  1, "01010" },
                { L_,   "0110",  2,  "0",     1,  0, "0110"  },
                { L_,   "0110",  3,  "0",     0,  0, "0110"  },
                { L_,   "0110",  3,  "0",     0,  1, "01100" },
                { L_,   "0110",  3,  "0",     1,  0, "0110"  },
                { L_,   "0110",  4,  "0",     0,  0, "0110"  },
                { L_,   "0110",  4,  "0",     0,  1, "01100" },
                { L_,   "0110",  4,  "0",     1,  0, "0110"  },

                { L_,   "011",   0,  "00",    0,  0, "011"   },
                { L_,   "011",   0,  "00",    0,  1, "0011"  },
                { L_,   "011",   0,  "00",    0,  2, "00011" },
                { L_,   "011",   0,  "00",    1,  0, "011"   },
                { L_,   "011",   0,  "00",    1,  1, "0011"  },
                { L_,   "011",   0,  "00",    2,  0, "011"   },
                { L_,   "011",   1,  "00",    0,  0, "011"   },
                { L_,   "011",   1,  "00",    0,  1, "0011"  },
                { L_,   "011",   1,  "00",    0,  2, "00011" },
                { L_,   "011",   1,  "00",    1,  0, "011"   },
                { L_,   "011",   1,  "00",    1,  1, "0011"  },
                { L_,   "011",   1,  "00",    2,  0, "011"   },
                { L_,   "011",   2,  "00",    0,  0, "011"   },
                { L_,   "011",   2,  "00",    0,  1, "0101"  },
                { L_,   "011",   2,  "00",    0,  2, "01001" },
                { L_,   "011",   2,  "00",    1,  0, "011"   },
                { L_,   "011",   2,  "00",    1,  1, "0101"  },
                { L_,   "011",   2,  "00",    2,  0, "011"   },
                { L_,   "011",   3,  "00",    0,  0, "011"   },
                { L_,   "011",   3,  "00",    0,  1, "0110"  },
                { L_,   "011",   3,  "00",    0,  2, "01100" },
                { L_,   "011",   3,  "00",    1,  0, "011"   },
                { L_,   "011",   3,  "00",    1,  1, "0110"  },
                { L_,   "011",   3,  "00",    2,  0, "011"   },

                { L_,   "01",    0,  "100",   0,  0, "01"    },
                { L_,   "01",    0,  "100",   0,  1, "101"   },
                { L_,   "01",    0,  "100",   0,  2, "1001"  },
                { L_,   "01",    0,  "100",   0,  3, "10001" },
                { L_,   "01",    0,  "100",   1,  0, "01"    },
                { L_,   "01",    0,  "100",   1,  1, "001"   },
                { L_,   "01",    0,  "100",   1,  2, "0001"  },
                { L_,   "01",    0,  "100",   2,  0, "01"    },
                { L_,   "01",    0,  "100",   2,  1, "001"   },
                { L_,   "01",    0,  "100",   3,  0, "01"    },
                { L_,   "01",    1,  "100",   0,  0, "01"    },
                { L_,   "01",    1,  "100",   0,  1, "011"   },
                { L_,   "01",    1,  "100",   0,  2, "0101"  },
                { L_,   "01",    1,  "100",   0,  3, "01001" },
                { L_,   "01",    1,  "100",   1,  0, "01"    },
                { L_,   "01",    1,  "100",   1,  1, "001"   },
                { L_,   "01",    1,  "100",   1,  2, "0001"  },
                { L_,   "01",    1,  "100",   2,  0, "01"    },
                { L_,   "01",    1,  "100",   2,  1, "001"   },
                { L_,   "01",    1,  "100",   3,  0, "01"    },
                { L_,   "01",    2,  "100",   0,  0, "01"    },
                { L_,   "01",    2,  "100",   0,  1, "011"   },
                { L_,   "01",    2,  "100",   0,  2, "0110"  },
                { L_,   "01",    2,  "100",   0,  3, "01100" },
                { L_,   "01",    2,  "100",   1,  0, "01"    },
                { L_,   "01",    2,  "100",   1,  1, "010"   },
                { L_,   "01",    2,  "100",   1,  2, "0100"  },
                { L_,   "01",    2,  "100",   2,  0, "01"    },
                { L_,   "01",    2,  "100",   2,  1, "010"   },
                { L_,   "01",    2,  "100",   3,  0, "01"    },

                { L_,   "0",     0,  "1100",  0,  0, "0"     },
                { L_,   "0",     0,  "1100",  0,  1, "10"    },
                { L_,   "0",     0,  "1100",  0,  2, "110"   },
                { L_,   "0",     0,  "1100",  0,  3, "1100"  },
                { L_,   "0",     0,  "1100",  1,  0, "0"     },
                { L_,   "0",     0,  "1100",  1,  1, "10"    },
                { L_,   "0",     0,  "1100",  1,  2, "100"   },
                { L_,   "0",     0,  "1100",  2,  0, "0"     },
                { L_,   "0",     0,  "1100",  2,  1, "00"    },
                { L_,   "0",     0,  "1100",  3,  0, "0"     },
                { L_,   "0",     1,  "1100",  0,  0, "0"     },
                { L_,   "0",     1,  "1100",  0,  1, "01"    },
                { L_,   "0",     1,  "1100",  0,  2, "011"   },
                { L_,   "0",     1,  "1100",  0,  3, "0110"  },
                { L_,   "0",     1,  "1100",  1,  0, "0"     },
                { L_,   "0",     1,  "1100",  1,  1, "01"    },
                { L_,   "0",     1,  "1100",  1,  2, "010"   },
                { L_,   "0",     1,  "1100",  2,  0, "0"     },
                { L_,   "0",     1,  "1100",  2,  1, "00"    },
                { L_,   "0",     1,  "1100",  3,  0, "0"     },

                { L_,   "",      0,  "01100", 0,  0, ""      },
                { L_,   "",      0,  "01100", 0,  1, "0"     },
                { L_,   "",      0,  "01100", 0,  2, "01"    },
                { L_,   "",      0,  "01100", 0,  3, "011"   },
                { L_,   "",      0,  "01100", 0,  4, "0110"  },
                { L_,   "",      0,  "01100", 0,  5, "01100" },
                { L_,   "",      0,  "01100", 1,  0, ""      },
                { L_,   "",      0,  "01100", 1,  1, "1"     },
                { L_,   "",      0,  "01100", 1,  2, "11"    },
                { L_,   "",      0,  "01100", 1,  3, "110"   },
                { L_,   "",      0,  "01100", 1,  4, "1100"  },
                { L_,   "",      0,  "01100", 2,  0, ""      },
                { L_,   "",      0,  "01100", 2,  1, "1"     },
                { L_,   "",      0,  "01100", 2,  2, "10"    },
                { L_,   "",      0,  "01100", 2,  3, "100"   },
                { L_,   "",      0,  "01100", 3,  0, ""      },
                { L_,   "",      0,  "01100", 3,  1, "0"     },
                { L_,   "",      0,  "01100", 3,  2, "00"    },
                { L_,   "",      0,  "01100", 4,  0, ""      },
                { L_,   "",      0,  "01100", 4,  1, "0"     },
                { L_,   "",      0,  "01100", 5,  0, ""      },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldDepth = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int   LINE   = DATA[ti].d_lineNum;
                const char *D_SPEC = DATA[ti].d_daSpec;
                const int   DI     = DATA[ti].d_di;
                const char *S_SPEC = DATA[ti].d_saSpec;
                const int   SI     = DATA[ti].d_si;
                const int   NE     = DATA[ti].d_ne;
                const char *E_SPEC = DATA[ti].d_expSpec;

                const int   DEPTH  = (int) strlen(D_SPEC) + strlen(S_SPEC);
                if (DEPTH > oldDepth) {
                    oldDepth = DEPTH;
                    if (verbose) { cout << '\t';  P(DEPTH); }
                }

                Obj DD(g(D_SPEC));              // control for destination
                Obj SS(g(S_SPEC));              // control for source
                Obj EE(g(E_SPEC));              // control for expected value

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
                  BEGIN_BSLMA_EXCEPTION_TEST {
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
                  } END_BSLMA_EXCEPTION_TEST
                }

                if (veryVerbose) cout << "\t\tinsert(di, sa)" << endl;
                if ((int)strlen(S_SPEC) == NE) {
                  BEGIN_BSLMA_EXCEPTION_TEST {
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
                  } END_BSLMA_EXCEPTION_TEST
                }

                if (veryVerbose) cout << "\t\tinsert(di, value)" << endl;
                if (1 == NE) {
                  BEGIN_BSLMA_EXCEPTION_TEST {
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
                  } END_BSLMA_EXCEPTION_TEST
                }

                // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                if (veryVerbose) cout << "\t\tappend(sa, si, ne)" << endl;
                if ((int)strlen(D_SPEC) == DI) {
                  BEGIN_BSLMA_EXCEPTION_TEST {
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
                  } END_BSLMA_EXCEPTION_TEST
                }

                if (veryVerbose) cout << "\t\tappend(sa)" << endl;
                if ((int)strlen(D_SPEC) == DI && (int)strlen(S_SPEC) == NE) {
                  BEGIN_BSLMA_EXCEPTION_TEST {
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
                  } END_BSLMA_EXCEPTION_TEST
                }

                if (veryVerbose) cout << "\t\tappend(value)" << endl;
                if ((int)strlen(D_SPEC) == DI && 1 == NE) {
                  BEGIN_BSLMA_EXCEPTION_TEST {
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
                  } END_BSLMA_EXCEPTION_TEST
                }

                // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                if (veryVerbose) cout << "\t\tremove(index, ne)" << endl;
                {
                  if (veryVerbose) { P_(DI) P(NE); }
                  BEGIN_BSLMA_EXCEPTION_TEST {  // Note specs are switched.
                    Obj x(EE, &testAllocator);  const Obj &X = x;
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    x.remove(DI, NE);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP5_ASSERT(LINE, DD, X, DI, NE, DD == X);
                  } END_BSLMA_EXCEPTION_TEST
                }

                if (veryVerbose) cout << "\t\tremove(index)" << endl;
                if (1 == NE) {
                  BEGIN_BSLMA_EXCEPTION_TEST {  // Note specs are switched
                    Obj x(EE, &testAllocator);  const Obj &X = x;
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    x.remove(DI);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP4_ASSERT(LINE, DD, X, DI, DD == X);
                  } END_BSLMA_EXCEPTION_TEST
                }

                if (veryVerbose) {
                    cout << "\t\t\t---------- WHITE BOX ----------" << endl;
                }

                Obj x(&testAllocator);  const Obj &X = x;

                const int STRETCH_SIZE = 50;

                stretchRemoveAll(&x, STRETCH_SIZE);

                const int NUM_BLOCKS = testAllocator.numBlocksTotal();
                const int NUM_BYTES  = testAllocator.numBytesInUse();

                if (veryVerbose) cout << "\t\tinsert(di, sa, si, ne)" << endl;
                {
                    x.removeAll();  gg(&x, D_SPEC);
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    x.insert(DI, SS, SI, NE);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                }

                if (veryVerbose) cout << "\t\tinsert(di, sa)" << endl;
                if ((int)strlen(S_SPEC) == NE) {
                    x.removeAll();  gg(&x, D_SPEC);
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    x.insert(DI, SS);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                }

                if (veryVerbose) cout << "\t\tinsert(di, value)" << endl;
                if (1 == NE) {
                    x.removeAll();  gg(&x, D_SPEC);
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    x.insert(DI, SS[SI]);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                }

                // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                if (veryVerbose) cout << "\t\tappend(sa, si, ne)" << endl;
                if ((int)strlen(D_SPEC) == DI) {
                    x.removeAll();  gg(&x, D_SPEC);
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    x.append(SS, SI, NE);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                }

                if (veryVerbose) cout << "\t\tappend(sa)" << endl;
                if ((int)strlen(D_SPEC) == DI && (int)strlen(S_SPEC) == NE) {
                    x.removeAll();  gg(&x, D_SPEC);
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    x.append(SS);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                }

                if (veryVerbose) cout << "\t\tappend(value)" << endl;
                if ((int)strlen(D_SPEC) == DI && 1 == NE) {
                    x.removeAll();  gg(&x, D_SPEC);
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    x.append(SS[SI]);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                }

                // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                if (veryVerbose) cout << "\t\tremove(index, ne)" << endl;
                {
                    x.removeAll();  gg(&x, E_SPEC);  // Note: specs switched!
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    x.remove(DI, NE);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP5_ASSERT(LINE, DD, X, DI, NE, DD == X);
                }

                if (veryVerbose) cout << "\t\tremove(index)" << endl;
                if (1 == NE) {
                    x.removeAll();  gg(&x, E_SPEC);  // Note: specs switched!
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    x.remove(DI);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP4_ASSERT(LINE, DD, X, DI, DD == X);
                }

                LOOP_ASSERT(LINE, NUM_BLOCKS ==testAllocator.numBlocksTotal());
                LOOP_ASSERT(LINE, NUM_BYTES == testAllocator.numBytesInUse());
            }
        }

        //---------------------------------------------------------------------

        if (verbose) cout <<
            "\nTesting x.insert(di, sa, si, ne) et al. (aliasing)" << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_xSpec;    // initial array (= srcArray)
                int         d_di;       // index at which to insert into x
                int         d_si;       // index at which to insert from x
                int         d_ne;       // number of elements to insert
                const char *d_expSpec;  // expected array value
            } DATA[] = {
                //line  x-array di   si  ne  expected
                //----  ------- --   --  --  --------   Depth = 0
                { L_,   "",      0,   0,  0,  ""      },

                //line  x-array di   si  ne  expected
                //----  ------- --   --  --  --------   Depth = 1
                { L_,   "0",     0,   0,  0,  "0"     },
                { L_,   "0",     0,   0,  1,  "00"    },
                { L_,   "0",     0,   1,  0,  "0"     },

                { L_,   "0",     1,   0,  0,  "0"     },
                { L_,   "0",     1,   0,  1,  "00"    },
                { L_,   "0",     1,   1,  0,  "0"     },

                //line  x-array di   si  ne  expected
                //----  ------- --   --  --  --------   Depth = 2
                { L_,   "01",    0,   0,  0,  "01"    },
                { L_,   "01",    0,   0,  1,  "001"   },
                { L_,   "01",    0,   0,  2,  "0101"  },
                { L_,   "01",    0,   1,  0,  "01"    },
                { L_,   "01",    0,   1,  1,  "101"   },
                { L_,   "01",    0,   2,  0,  "01"    },

                { L_,   "01",    1,   0,  0,  "01"    },
                { L_,   "01",    1,   0,  1,  "001"   },
                { L_,   "01",    1,   0,  2,  "0011"  },
                { L_,   "01",    1,   1,  0,  "01"    },
                { L_,   "01",    1,   1,  1,  "011"   },
                { L_,   "01",    1,   2,  0,  "01"    },

                { L_,   "01",    2,   0,  0,  "01"    },
                { L_,   "01",    2,   0,  1,  "010"   },
                { L_,   "01",    2,   0,  2,  "0101"  },
                { L_,   "01",    2,   1,  0,  "01"    },
                { L_,   "01",    2,   1,  1,  "011"   },
                { L_,   "01",    2,   2,  0,  "01"    },

                //line  x-array di   si  ne  expected
                //----  ------- --   --  --  --------   Depth = 3
                { L_,   "011",   0,   0,  0,  "011"     },
                { L_,   "011",   0,   0,  1,  "0011"    },
                { L_,   "011",   0,   0,  2,  "01011"   },
                { L_,   "011",   0,   0,  3,  "011011"  },
                { L_,   "011",   0,   1,  0,  "011"     },
                { L_,   "011",   0,   1,  1,  "1011"    },
                { L_,   "011",   0,   1,  2,  "11011"   },
                { L_,   "011",   0,   2,  0,  "011"     },
                { L_,   "011",   0,   2,  1,  "1011"    },
                { L_,   "011",   0,   3,  0,  "011"     },

                { L_,   "011",   1,   0,  0,  "011"     },
                { L_,   "011",   1,   0,  1,  "0011"    },
                { L_,   "011",   1,   0,  2,  "00111"   },
                { L_,   "011",   1,   0,  3,  "001111"  },
                { L_,   "011",   1,   1,  0,  "011"     },
                { L_,   "011",   1,   1,  1,  "0111"    },
                { L_,   "011",   1,   1,  2,  "01111"   },
                { L_,   "011",   1,   2,  0,  "011"     },
                { L_,   "011",   1,   2,  1,  "0111"    },
                { L_,   "011",   1,   3,  0,  "011"     },

                { L_,   "011",   2,   0,  0,  "011"     },
                { L_,   "011",   2,   0,  1,  "0101"    },
                { L_,   "011",   2,   0,  2,  "01011"   },
                { L_,   "011",   2,   0,  3,  "010111"  },
                { L_,   "011",   2,   1,  0,  "011"     },
                { L_,   "011",   2,   1,  1,  "0111"    },
                { L_,   "011",   2,   1,  2,  "01111"   },
                { L_,   "011",   2,   2,  0,  "011"     },
                { L_,   "011",   2,   2,  1,  "0111"    },
                { L_,   "011",   2,   3,  0,  "011"     },

                { L_,   "011",   3,   0,  0,  "011"     },
                { L_,   "011",   3,   0,  1,  "0110"    },
                { L_,   "011",   3,   0,  2,  "01101"   },
                { L_,   "011",   3,   0,  3,  "011011"  },
                { L_,   "011",   3,   1,  0,  "011"     },
                { L_,   "011",   3,   1,  1,  "0111"    },
                { L_,   "011",   3,   1,  2,  "01111"   },
                { L_,   "011",   3,   2,  0,  "011"     },
                { L_,   "011",   3,   2,  1,  "0111"    },
                { L_,   "011",   3,   3,  0,  "011"     },

                //line  x-array di   si  ne  expected
                //----  ------- --   --  --  --------   Depth = 4
                { L_,   "0110",  0,   0,  0,  "0110"      },
                { L_,   "0110",  0,   0,  1,  "00110"     },
                { L_,   "0110",  0,   0,  2,  "010110"    },
                { L_,   "0110",  0,   0,  3,  "0110110"   },
                { L_,   "0110",  0,   0,  4,  "01100110"  },
                { L_,   "0110",  0,   1,  0,  "0110"      },
                { L_,   "0110",  0,   1,  1,  "10110"     },
                { L_,   "0110",  0,   1,  2,  "110110"    },
                { L_,   "0110",  0,   1,  3,  "1100110"   },
                { L_,   "0110",  0,   2,  0,  "0110"      },
                { L_,   "0110",  0,   2,  1,  "10110"     },
                { L_,   "0110",  0,   2,  2,  "100110"    },
                { L_,   "0110",  0,   3,  0,  "0110"      },
                { L_,   "0110",  0,   3,  1,  "00110"     },
                { L_,   "0110",  0,   4,  0,  "0110"      },

                { L_,   "0110",  1,   0,  0,  "0110"      },
                { L_,   "0110",  1,   0,  1,  "00110"     },
                { L_,   "0110",  1,   0,  2,  "001110"    },
                { L_,   "0110",  1,   0,  3,  "0011110"   },
                { L_,   "0110",  1,   0,  4,  "00110110"  },
                { L_,   "0110",  1,   1,  0,  "0110"      },
                { L_,   "0110",  1,   1,  1,  "01110"     },
                { L_,   "0110",  1,   1,  2,  "011110"    },
                { L_,   "0110",  1,   1,  3,  "0110110"   },
                { L_,   "0110",  1,   2,  0,  "0110"      },
                { L_,   "0110",  1,   2,  1,  "01110"     },
                { L_,   "0110",  1,   2,  2,  "010110"    },
                { L_,   "0110",  1,   3,  0,  "0110"      },
                { L_,   "0110",  1,   3,  1,  "00110"     },
                { L_,   "0110",  1,   4,  0,  "0110"      },

                { L_,   "0110",  2,   0,  0,  "0110"      },
                { L_,   "0110",  2,   0,  1,  "01010"     },
                { L_,   "0110",  2,   0,  2,  "010110"    },
                { L_,   "0110",  2,   0,  3,  "0101110"   },
                { L_,   "0110",  2,   0,  4,  "01011010"  },
                { L_,   "0110",  2,   1,  0,  "0110"      },
                { L_,   "0110",  2,   1,  1,  "01110"     },
                { L_,   "0110",  2,   1,  2,  "011110"    },
                { L_,   "0110",  2,   1,  3,  "0111010"   },
                { L_,   "0110",  2,   2,  0,  "0110"      },
                { L_,   "0110",  2,   2,  1,  "01110"     },
                { L_,   "0110",  2,   2,  2,  "011010"    },
                { L_,   "0110",  2,   3,  0,  "0110"      },
                { L_,   "0110",  2,   3,  1,  "01010"     },
                { L_,   "0110",  2,   4,  0,  "0110"      },

                { L_,   "0110",  3,   0,  0,  "0110"      },
                { L_,   "0110",  3,   0,  1,  "01100"     },
                { L_,   "0110",  3,   0,  2,  "011010"    },
                { L_,   "0110",  3,   0,  3,  "0110110"   },
                { L_,   "0110",  3,   0,  4,  "01101100"  },
                { L_,   "0110",  3,   1,  0,  "0110"      },
                { L_,   "0110",  3,   1,  1,  "01110"     },
                { L_,   "0110",  3,   1,  2,  "011110"    },
                { L_,   "0110",  3,   1,  3,  "0111100"   },
                { L_,   "0110",  3,   2,  0,  "0110"      },
                { L_,   "0110",  3,   2,  1,  "01110"     },
                { L_,   "0110",  3,   2,  2,  "011100"    },
                { L_,   "0110",  3,   3,  0,  "0110"      },
                { L_,   "0110",  3,   3,  1,  "01100"     },
                { L_,   "0110",  3,   4,  0,  "0110"      },

                { L_,   "0110",  4,   0,  0,  "0110"      },
                { L_,   "0110",  4,   0,  1,  "01100"     },
                { L_,   "0110",  4,   0,  2,  "011001"    },
                { L_,   "0110",  4,   0,  3,  "0110011"   },
                { L_,   "0110",  4,   0,  4,  "01100110"  },
                { L_,   "0110",  4,   1,  0,  "0110"      },
                { L_,   "0110",  4,   1,  1,  "01101"     },
                { L_,   "0110",  4,   1,  2,  "011011"    },
                { L_,   "0110",  4,   1,  3,  "0110110"   },
                { L_,   "0110",  4,   2,  0,  "0110"      },
                { L_,   "0110",  4,   2,  1,  "01101"     },
                { L_,   "0110",  4,   2,  2,  "011010"    },
                { L_,   "0110",  4,   3,  0,  "0110"      },
                { L_,   "0110",  4,   3,  1,  "01100"     },
                { L_,   "0110",  4,   4,  0,  "0110"      },

                //line  x-array di   si  ne  expected
                //----  ------- --   --  --  --------   Depth = 5
                { L_,   "01100", 0,   0,  0,  "01100"      },
                { L_,   "01100", 0,   0,  1,  "001100"     },
                { L_,   "01100", 0,   0,  2,  "0101100"    },
                { L_,   "01100", 0,   0,  3,  "01101100"   },
                { L_,   "01100", 0,   0,  4,  "011001100"  },
                { L_,   "01100", 0,   0,  5,  "0110001100" },
                { L_,   "01100", 0,   1,  0,  "01100"      },
                { L_,   "01100", 0,   1,  1,  "101100"     },
                { L_,   "01100", 0,   1,  2,  "1101100"    },
                { L_,   "01100", 0,   1,  3,  "11001100"   },
                { L_,   "01100", 0,   1,  4,  "110001100"  },
                { L_,   "01100", 0,   2,  0,  "01100"      },
                { L_,   "01100", 0,   2,  1,  "101100"     },
                { L_,   "01100", 0,   2,  2,  "1001100"    },
                { L_,   "01100", 0,   2,  3,  "10001100"   },
                { L_,   "01100", 0,   3,  0,  "01100"      },
                { L_,   "01100", 0,   3,  1,  "001100"     },
                { L_,   "01100", 0,   3,  2,  "0001100"    },
                { L_,   "01100", 0,   4,  0,  "01100"      },
                { L_,   "01100", 0,   4,  1,  "001100"     },
                { L_,   "01100", 0,   5,  0,  "01100"      },

                { L_,   "01100", 1,   0,  0,  "01100"      },
                { L_,   "01100", 1,   0,  1,  "001100"     },
                { L_,   "01100", 1,   0,  2,  "0011100"    },
                { L_,   "01100", 1,   0,  3,  "00111100"   },
                { L_,   "01100", 1,   0,  4,  "001101100"  },
                { L_,   "01100", 1,   0,  5,  "0011001100" },
                { L_,   "01100", 1,   1,  0,  "01100"      },
                { L_,   "01100", 1,   1,  1,  "011100"     },
                { L_,   "01100", 1,   1,  2,  "0111100"    },
                { L_,   "01100", 1,   1,  3,  "01101100"   },
                { L_,   "01100", 1,   1,  4,  "011001100"  },
                { L_,   "01100", 1,   2,  0,  "01100"      },
                { L_,   "01100", 1,   2,  1,  "011100"     },
                { L_,   "01100", 1,   2,  2,  "0101100"    },
                { L_,   "01100", 1,   2,  3,  "01001100"   },
                { L_,   "01100", 1,   3,  0,  "01100"      },
                { L_,   "01100", 1,   3,  1,  "001100"     },
                { L_,   "01100", 1,   3,  2,  "0001100"    },
                { L_,   "01100", 1,   4,  0,  "01100"      },
                { L_,   "01100", 1,   4,  1,  "001100"     },
                { L_,   "01100", 1,   5,  0,  "01100"      },

                { L_,   "01100", 2,   0,  0,  "01100"      },
                { L_,   "01100", 2,   0,  1,  "010100"     },
                { L_,   "01100", 2,   0,  2,  "0101100"    },
                { L_,   "01100", 2,   0,  3,  "01011100"   },
                { L_,   "01100", 2,   0,  4,  "010110100"  },
                { L_,   "01100", 2,   0,  5,  "0101100100" },
                { L_,   "01100", 2,   1,  0,  "01100"      },
                { L_,   "01100", 2,   1,  1,  "011100"     },
                { L_,   "01100", 2,   1,  2,  "0111100"    },
                { L_,   "01100", 2,   1,  3,  "01110100"   },
                { L_,   "01100", 2,   1,  4,  "011100100"  },
                { L_,   "01100", 2,   2,  0,  "01100"      },
                { L_,   "01100", 2,   2,  1,  "011100"     },
                { L_,   "01100", 2,   2,  2,  "0110100"    },
                { L_,   "01100", 2,   2,  3,  "01100100"   },
                { L_,   "01100", 2,   3,  0,  "01100"      },
                { L_,   "01100", 2,   3,  1,  "010100"     },
                { L_,   "01100", 2,   3,  2,  "0100100"    },
                { L_,   "01100", 2,   4,  0,  "01100"      },
                { L_,   "01100", 2,   4,  1,  "010100"     },
                { L_,   "01100", 2,   5,  0,  "01100"      },

                { L_,   "01100", 3,   0,  0,  "01100"      },
                { L_,   "01100", 3,   0,  1,  "011000"     },
                { L_,   "01100", 3,   0,  2,  "0110100"    },
                { L_,   "01100", 3,   0,  3,  "01101100"   },
                { L_,   "01100", 3,   0,  4,  "011011000"  },
                { L_,   "01100", 3,   0,  5,  "0110110000" },
                { L_,   "01100", 3,   1,  0,  "01100"      },
                { L_,   "01100", 3,   1,  1,  "011100"     },
                { L_,   "01100", 3,   1,  2,  "0111100"    },
                { L_,   "01100", 3,   1,  3,  "01111000"   },
                { L_,   "01100", 3,   1,  4,  "011110000"  },
                { L_,   "01100", 3,   2,  0,  "01100"      },
                { L_,   "01100", 3,   2,  1,  "011100"     },
                { L_,   "01100", 3,   2,  2,  "0111000"    },
                { L_,   "01100", 3,   2,  3,  "01110000"   },
                { L_,   "01100", 3,   3,  0,  "01100"      },
                { L_,   "01100", 3,   3,  1,  "011000"     },
                { L_,   "01100", 3,   3,  2,  "0110000"    },
                { L_,   "01100", 3,   4,  0,  "01100"      },
                { L_,   "01100", 3,   4,  1,  "011000"     },
                { L_,   "01100", 3,   5,  0,  "01100"      },

                { L_,   "01100", 4,   0,  0,  "01100"      },
                { L_,   "01100", 4,   0,  1,  "011000"     },
                { L_,   "01100", 4,   0,  2,  "0110010"    },
                { L_,   "01100", 4,   0,  3,  "01100110"   },
                { L_,   "01100", 4,   0,  4,  "011001100"  },
                { L_,   "01100", 4,   0,  5,  "0110011000" },
                { L_,   "01100", 4,   1,  0,  "01100"      },
                { L_,   "01100", 4,   1,  1,  "011010"     },
                { L_,   "01100", 4,   1,  2,  "0110110"    },
                { L_,   "01100", 4,   1,  3,  "01101100"   },
                { L_,   "01100", 4,   1,  4,  "011011000"  },
                { L_,   "01100", 4,   2,  0,  "01100"      },
                { L_,   "01100", 4,   2,  1,  "011010"     },
                { L_,   "01100", 4,   2,  2,  "0110100"    },
                { L_,   "01100", 4,   2,  3,  "01101000"   },
                { L_,   "01100", 4,   3,  0,  "01100"      },
                { L_,   "01100", 4,   3,  1,  "011000"     },
                { L_,   "01100", 4,   3,  2,  "0110000"    },
                { L_,   "01100", 4,   4,  0,  "01100"      },
                { L_,   "01100", 4,   4,  1,  "011000"     },
                { L_,   "01100", 4,   5,  0,  "01100"      },

                { L_,   "01100", 5,   0,  0,  "01100"      },
                { L_,   "01100", 5,   0,  1,  "011000"     },
                { L_,   "01100", 5,   0,  2,  "0110001"    },
                { L_,   "01100", 5,   0,  3,  "01100011"   },
                { L_,   "01100", 5,   0,  4,  "011000110"  },
                { L_,   "01100", 5,   0,  5,  "0110001100" },
                { L_,   "01100", 5,   1,  0,  "01100"      },
                { L_,   "01100", 5,   1,  1,  "011001"     },
                { L_,   "01100", 5,   1,  2,  "0110011"    },
                { L_,   "01100", 5,   1,  3,  "01100110"   },
                { L_,   "01100", 5,   1,  4,  "011001100"  },
                { L_,   "01100", 5,   2,  0,  "01100"      },
                { L_,   "01100", 5,   2,  1,  "011001"     },
                { L_,   "01100", 5,   2,  2,  "0110010"    },
                { L_,   "01100", 5,   2,  3,  "01100100"   },
                { L_,   "01100", 5,   3,  0,  "01100"      },
                { L_,   "01100", 5,   3,  1,  "011000"     },
                { L_,   "01100", 5,   3,  2,  "0110000"    },
                { L_,   "01100", 5,   4,  0,  "01100"      },
                { L_,   "01100", 5,   4,  1,  "011000"     },
                { L_,   "01100", 5,   5,  0,  "01100"      },

            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldDepth = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int   LINE   = DATA[ti].d_lineNum;
                const char *X_SPEC = DATA[ti].d_xSpec;
                const int   DI     = DATA[ti].d_di;
                const int   SI     = DATA[ti].d_si;
                const int   NE     = DATA[ti].d_ne;
                const char *E_SPEC = DATA[ti].d_expSpec;

                const int   DEPTH  = (int)strlen(X_SPEC);
                if (DEPTH > oldDepth) {
                    oldDepth = DEPTH;
                    if (verbose) { cout << '\t';  P(DEPTH); }
                }

                Obj DD(g(X_SPEC));              // control for destination
                Obj EE(g(E_SPEC));              // control for expected value

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
                    Obj x2(DD, &testAllocator);  const Obj &X2 = x2; // control
                    x2.insert(DI, X, SI, NE);
                    LOOP_ASSERT(LINE, EE == X2);
                }

                if (veryVerbose) cout << "\t\tinsert(di, sa, si, ne)" << endl;
                {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    Obj x(DD, &testAllocator);  const Obj &X = x;
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    x.insert(DI, X, SI, NE);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); P(EE); }
                    LOOP4_ASSERT(LINE, DI, SI, NE, EE == X);
                  } END_BSLMA_EXCEPTION_TEST
                }

                if (veryVerbose) cout << "\t\tinsert(di, sa)" << endl;
                if ((int)strlen(X_SPEC) == NE) {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    Obj x(DD, &testAllocator);  const Obj &X = x;
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    x.insert(DI, X);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                  } END_BSLMA_EXCEPTION_TEST
                }

                if (veryVerbose) cout << "\t\tinsert(di, value)" << endl;
                if (1 == NE) {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    Obj x(DD, &testAllocator);  const Obj &X = x;
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    x.insert(DI, X[SI]);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                  } END_BSLMA_EXCEPTION_TEST
                }

                // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                if (veryVerbose) cout << "\t\tappend(sa, si, ne)" << endl;
                if ((int)strlen(X_SPEC) == DI) {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    Obj x(DD, &testAllocator);  const Obj &X = x;
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    x.append(X, SI, NE);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                  } END_BSLMA_EXCEPTION_TEST
                }

                if (veryVerbose) cout << "\t\tappend(sa)" << endl;
                if ((int)strlen(X_SPEC) == DI && (int)strlen(X_SPEC) == NE) {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    Obj x(DD, &testAllocator);  const Obj &X = x;
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    x.append(X);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                  } END_BSLMA_EXCEPTION_TEST
                }

                if (veryVerbose) cout << "\t\tappend(value)" << endl;
                if ((int)strlen(X_SPEC) == DI && 1 == NE) {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    Obj x(DD, &testAllocator);  const Obj &X = x;
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    x.append(X[SI]);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                  } END_BSLMA_EXCEPTION_TEST
                }

                if (veryVerbose) {
                    cout << "\t\t\t---------- WHITE BOX ----------" << endl;
                }

                Obj x(&testAllocator);  const Obj &X = x;

                const int STRETCH_SIZE = 50;

                stretchRemoveAll(&x, STRETCH_SIZE);

                const int NUM_BLOCKS = testAllocator.numBlocksTotal();
                const int NUM_BYTES  = testAllocator.numBytesInUse();

                if (veryVerbose) cout << "\t\tinsert(di, sa, si, ne)" << endl;
                {
                    x.removeAll();  gg(&x, X_SPEC);
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    x.insert(DI, X, SI, NE);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                }

                if (veryVerbose) cout << "\t\tinsert(di, sa)" << endl;
                if ((int)strlen(X_SPEC) == NE) {
                    x.removeAll();  gg(&x, X_SPEC);
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    x.insert(DI, X);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                }

                if (veryVerbose) cout << "\t\tinsert(di, value)" << endl;
                if (1 == NE) {
                    x.removeAll();  gg(&x, X_SPEC);
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    x.insert(DI, X[SI]);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                }

                // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                if (veryVerbose) cout << "\t\tappend(sa, si, ne)" << endl;
                if ((int)strlen(X_SPEC) == DI) {
                    x.removeAll();  gg(&x, X_SPEC);
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    x.append(X, SI, NE);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                }

                if (veryVerbose) cout << "\t\tappend(sa)" << endl;
                if ((int)strlen(X_SPEC) == DI && (int)strlen(X_SPEC) == NE) {
                    x.removeAll();  gg(&x, X_SPEC);
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    x.append(X);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                }

                if (veryVerbose) cout << "\t\tappend(value)" << endl;
                if ((int)strlen(X_SPEC) == DI && 1 == NE) {
                    x.removeAll();  gg(&x, X_SPEC);
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    x.append(X[SI]);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                }

                LOOP_ASSERT(LINE, NUM_BLOCKS ==testAllocator.numBlocksTotal());
                LOOP_ASSERT(LINE, NUM_BYTES == testAllocator.numBytesInUse());
            }
        }

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING SET-LENGTH FUNCTIONS:
        //   We have the following concerns:
        //    - The resulting length is correct.
        //    - The resulting element values are correct when:
        //        new length <  initial length
        //        new length == initial length
        //        new length >  initial length (undefined for 'setLengthRaw')
        //   We are also concerned that the test data include sufficient
        //   differences in initial and final length that resizing is
        //   guaranteed to occur.  Beyond that, no explicit "white box" test is
        //   required.
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
        // Testing:
        //   void setLengthRaw(int newLength);
        //   void setLength(int newLength);
        //   void setLength(int newLength, bit iVal);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Set-Length Functions" << endl
                          << "============================" << endl;

        if (verbose) cout << "\nTesting 'setLengthRaw'" << endl;
        {
            const int lengths[] = { 0, 1, 2, 3, 31, 32, 33, 63, 64, 65, 100 };
            const int NUM_TESTS = sizeof lengths / sizeof lengths[0];
            const Element VALUE  = 1; // ADJUST
            for (int i = 0; i < NUM_TESTS; ++i) {
                const int a1 = lengths[i];
                if (verbose) { cout << "\t"; P(a1); }
                for (int j = 0; j < NUM_TESTS; ++j) {
                    const int a2 = lengths[j];
                    if (veryVerbose) { cout << "\t\t"; P(a2); }
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    Obj mX(a1, VALUE, &testAllocator);
                    const Obj &X = mX;
                    if (veryVerbose) P(X);
                    LOOP2_ASSERT(i, j, a1 == X.length());
                    mX.setLengthRaw(a2);
                    if (veryVerbose) P(X);
                    LOOP2_ASSERT(i, j, a2 == X.length());
                    for (int k = 0; k < (a2 < a1 ? a2 : a1); ++k) {
                        LOOP3_ASSERT(i, j, k, VALUE == X[k]);
                    }
                  } END_BSLMA_EXCEPTION_TEST
                }
            }
        }

        if (verbose) cout << "\nTesting 'setLength(int)'" << endl;
        {
            const int lengths[] = { 0, 1, 2, 3, 31, 32, 33, 63, 64, 65, 100 };
            const int NUM_TESTS = sizeof lengths / sizeof lengths[0];
            const Element I_VALUE       = 1;
            const Element DEFAULT_VALUE = 0;
            for (int i = 0; i < NUM_TESTS; ++i) {
                const int a1 = lengths[i];
                if (verbose) { cout << "\t"; P(a1); }
                for (int j = 0; j < NUM_TESTS; ++j) {
                    const int a2 = lengths[j];
                    if (veryVerbose) { cout << "\t\t"; P(a2); }
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    Obj mX(a1, I_VALUE, &testAllocator);
                    const Obj &X = mX;
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
                  } END_BSLMA_EXCEPTION_TEST
                }
            }
        }

        if (verbose) cout << "\nTesting 'setLength(int, 0)'" << endl;
        {
            const int lengths[] = { 0, 1, 2, 3, 31, 32, 33, 63, 64, 65, 100 };
            const int NUM_TESTS = sizeof lengths / sizeof lengths[0];
            const Element I_VALUE = 1;
            const Element F_VALUE = 0;
            for (int i = 0; i < NUM_TESTS; ++i) {
                const int a1 = lengths[i];
                if (verbose) { cout << "\t"; P(a1); }
                for (int j = 0; j < NUM_TESTS; ++j) {
                    const int a2 = lengths[j];
                    if (veryVerbose) { cout << "\t\t"; P(a2); }
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    Obj mX(a1, I_VALUE, &testAllocator);
                    const Obj &X = mX;
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
                            LOOP3_ASSERT(i, j, k, (F_VALUE != 0) == X[k]);
                        }
                    }
                  } END_BSLMA_EXCEPTION_TEST
                }
            }
        }

        if (verbose) cout << "\nTesting 'setLength(int, 1)'" << endl;
        {
            const int lengths[] = { 0, 1, 2, 3, 31, 32, 33, 63, 64, 65, 100 };
            const int NUM_TESTS = sizeof lengths / sizeof lengths[0];
            const Element I_VALUE = 0;
            const Element F_VALUE = 1;
            for (int i = 0; i < NUM_TESTS; ++i) {
                const int a1 = lengths[i];
                if (verbose) { cout << "\t"; P(a1); }
                for (int j = 0; j < NUM_TESTS; ++j) {
                    const int a2 = lengths[j];
                    if (veryVerbose) { cout << "\t\t"; P(a2); }
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    Obj mX(a1, I_VALUE, &testAllocator);
                    const Obj &X = mX;
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
                            LOOP3_ASSERT(i, j, k, (F_VALUE!=0) == (bool) X[k]);
                        }
                    }
                  } END_BSLMA_EXCEPTION_TEST
                }
            }
        }

        if (verbose) cout << "\nTesting 'setLength(int, 2)'" << endl;
        {
            const int lengths[] = { 0, 1, 2, 3, 31, 32, 33, 63, 64, 65, 100 };
            const int NUM_TESTS = sizeof lengths / sizeof lengths[0];
            const Element I_VALUE = 0;
            const Element F_VALUE = 2;
            for (int i = 0; i < NUM_TESTS; ++i) {
                const int a1 = lengths[i];
                if (verbose) { cout << "\t"; P(a1); }
                for (int j = 0; j < NUM_TESTS; ++j) {
                    const int a2 = lengths[j];
                    if (veryVerbose) { cout << "\t\t"; P(a2); }
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    Obj mX(a1, I_VALUE, &testAllocator);
                    const Obj &X = mX;
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
                            LOOP3_ASSERT(i, j, k, (F_VALUE!=0) == (bool) X[k]);
                        }
                    }
                  } END_BSLMA_EXCEPTION_TEST
                }
            }
        }

        if (verbose) cout << "\nTesting 'setLength(int, -1)'" << endl;
        {
            const int lengths[] = { 0, 1, 2, 3, 31, 32, 33, 63, 64, 65, 100 };
            const int NUM_TESTS = sizeof lengths / sizeof lengths[0];
            const Element I_VALUE = 0;
            const Element F_VALUE = -1;
            for (int i = 0; i < NUM_TESTS; ++i) {
                const int a1 = lengths[i];
                if (verbose) { cout << "\t"; P(a1); }
                for (int j = 0; j < NUM_TESTS; ++j) {
                    const int a2 = lengths[j];
                    if (veryVerbose) { cout << "\t\t"; P(a2); }
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    Obj mX(a1, I_VALUE, &testAllocator);
                    const Obj &X = mX;
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
                            LOOP3_ASSERT(i, j, k, (F_VALUE!=0) == (bool) X[k]);
                        }
                    }
                  } END_BSLMA_EXCEPTION_TEST
                }
            }
        }

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING INITIAL-LENGTH CONSTRUCTORS:
        //   We have the following concerns:
        //    1) The initial value is correct.
        //    2) The constructor is exception neutral w.r.t. memory allocation.
        //    3) The internal memory management system is hooked up properly
        //       so that *all* internally allocated memory draws from a
        //       user-supplied allocator whenever one is specified.
        //
        // Plan:
        //   For each constructor we will create objects
        //    - With and without passing in an allocator.
        //    - In the presence of exceptions during memory allocations using
        //        a 'bslma_TestAllocator' and varying its *allocation* *limit*.
        //    - Where the object is constructed entirely in static memory
        //      (using a 'bdema_BufferedSequentialAllocator') and never
        //      destroyed.
        //   and use basic accessors to verify
        //      - length
        //      - element value at each index position { 0 .. length - 1 }.
        //
        // Testing:
        //   bdea_BitArray(int iLen, *ba = 0);
        //   bdea_BitArray(int iLen, int iVal, *ba = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Initial-Length Constructor" << endl
                          << "==================================" << endl;

        if (verbose) cout << "\nTesting initial-length ctor with unspecified "
                             "(default) initial value." << endl;

        if (verbose) cout << "\tWithout passing in an allocator." << endl;
        {
            const Element DEFAULT_VALUE = 0;  // ADJUST
            const int MAX_LENGTH = 100;
            for (int length = 0; length <= MAX_LENGTH; ++length) {
                if (verbose) P(length);
                Obj mX(length);  const Obj &X = mX;
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
            const Element DEFAULT_VALUE = 0;  // ADJUST
            const int MAX_LENGTH = 100;
            for (int length = 0; length <= MAX_LENGTH; ++length) {
                if (verbose) P(length);
                Obj mX(length, &testAllocator);  const Obj &X = mX;
                if (veryVerbose) P(X);
                LOOP_ASSERT(length, length == X.length());
                for (int i = 0; i < length; ++i) {
                    LOOP2_ASSERT(length, i, DEFAULT_VALUE == X[i]);
                }
            }
        }

        if (verbose) cout << "\t\tWith exceptions." << endl;
        {
            const Element DEFAULT_VALUE = 0;  // ADJUST
            const int MAX_LENGTH = 100;
            for (int length = 0; length <= MAX_LENGTH; ++length) {
              BEGIN_BSLMA_EXCEPTION_TEST {
                if (verbose) P(length);
                Obj mX(length, &testAllocator);  const Obj &X = mX;
                if (veryVerbose) P(X);
                LOOP_ASSERT(length, length == X.length());
                for (int i = 0; i < length; ++i) {
                    LOOP2_ASSERT(length, i, DEFAULT_VALUE == X[i]);
                }
              } END_BSLMA_EXCEPTION_TEST
            }
        }

        if (verbose) cout << "\tIn place using a buffer allocator." << endl;
        {
            char memory[8192];
            bdema_BufferedSequentialAllocator a(memory, sizeof memory);
            const Element DEFAULT_VALUE = 0;  // ADJUST
            const int MAX_LENGTH = 100;
            for (int length = 0; length <= MAX_LENGTH; ++length) {
                if (verbose) P(length);
                int e = length;
                Obj *doNotDelete = new(a.allocate(sizeof(Obj))) Obj(e, &a);
                Obj &mX = *doNotDelete;  const Obj &X = mX;
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
            const Element VALUE = 1;  // ADJUST
            const int MAX_LENGTH = 100;
            for (int length = 0; length <= MAX_LENGTH; ++length) {
                if (verbose) P(length);
                Obj mX(length, VALUE);  const Obj &X = mX;
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
            const Element VALUE = 1;  // ADJUST
            const int MAX_LENGTH = 100;
            for (int length = 0; length <= MAX_LENGTH; ++length) {
                if (verbose) P(length);
                Obj mX(length, VALUE, &testAllocator);  const Obj &X = mX;
                if (veryVerbose) P(X);
                LOOP_ASSERT(length, length == X.length());
                for (int i = 0; i < length; ++i) {
                    LOOP2_ASSERT(length, i, VALUE == X[i]);
                }
            }
        }

        if (verbose) cout << "\t\tWith exceptions." << endl;
        {
            const Element VALUE = 1;  // ADJUST
            const int MAX_LENGTH = 100;
            for (int length = 0; length <= MAX_LENGTH; ++length) {
              BEGIN_BSLMA_EXCEPTION_TEST {
                if (verbose) P(length);
                Obj mX(length, VALUE, &testAllocator);  const Obj &X = mX;
                if (veryVerbose) P(X);
                LOOP_ASSERT(length, length == X.length());
                for (int i = 0; i < length; ++i) {
                    LOOP2_ASSERT(length, i, VALUE == X[i]);
                }
              } END_BSLMA_EXCEPTION_TEST
            }
        }

        if (verbose) cout << "\tIn place using a buffer allocator." << endl;
        {
            char memory[8192];
            bdema_BufferedSequentialAllocator a(memory, sizeof memory);
            const Element VALUE = 1;  // ADJUST
            const int MAX_LENGTH = 100;
            for (int length = 0; length <= MAX_LENGTH; ++length) {
                if (verbose) P(length);
                Obj *doNotDelete =
                    new(a.allocate(sizeof(Obj))) Obj(length, VALUE, &a);
                Obj &mX = *doNotDelete;  const Obj &X = mX;
                if (veryVerbose) P(X);
                LOOP_ASSERT(length, length == X.length());
                for (int i = 0; i < length; ++i) {
                    LOOP2_ASSERT(length, i, VALUE == X[i]);
                }
            }
            // No destructor is called; will produce memory leak in purify
            // if internal allocators are not hooked up properly.
        }

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING STREAMING FUNCTIONALITY:
        //   1) Streaming must be neutral to exceptions thrown as a result of
        //      either allocating memory or streaming in values.
        //   2) Ensure that streaming works under the following conditions:
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
        //      object of) v, and assert that u == v.
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
        //     original object to leave scope without further modification,
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
        //   Finally, tests of the explicit wire format will be performed.
        //
        // Testing:
        //   int maxSupportedBdexVersion() const;
        //   STREAM& bdexStreamIn(STREAM& stream, int version);
        //   STREAM& bdexStreamOut(STREAM& stream, version) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Streaming Functionality" << endl
                          << "===============================" << endl;

        const int VERSION = Obj::maxSupportedBdexVersion();

        if (verbose) cout << "\nDirect initial trial of 'streamOut' and"
                             " (valid) 'streamIn' functionality." << endl;
        {
            const Obj X(g("011"), &testAllocator);
            if (veryVerbose) { cout << "\t   Value being streamed: "; P(X); }

            bdex_TestOutStream out;
            bdex_OutStreamFunctions::streamOut(out, X, VERSION);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            bdex_TestInStream in(OD, LOD);  ASSERT(in);  ASSERT(!in.isEmpty());

            Obj t(g("00"), &testAllocator);

            if (veryVerbose) { cout << "\tValue being overwritten: "; P(t); }
            ASSERT(X != t);

            bdex_InStreamFunctions::streamIn(in, t, VERSION);
            ASSERT(in);  ASSERT(in.isEmpty());

            if (veryVerbose) { cout << "\t  Value after overwrite: "; P(t); }
            ASSERT(X == t);
        }

        if (verbose) cout <<
            "\nTesting stream operators ('<<' and '>>')." << endl;

        if (verbose) cout << "\tOn valid, non-empty stream data." << endl;
        {
            static const char *SPECS[] = {
                "",      "0",      "01",     "011",    "0110",   "01100",
                "0110001",         "01100011",         "011000110",
                "011000110001100", "0110001100011000", "01100011000110001",
                "0000100011100001001100000100110",
                "00001000111000010011000001001101",
           "000010001110000100110000010011010000100011100001001100000100110",
           "0000100011100001001100000100110100001000111000010011000001001100",
           "00001000111000010011000001001101000010001110000100110000010011001",
            0}; // Null string required as last element.

            static const int EXTEND[] = {
                0, 1, 3, 5, 32, 64, 100
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            int uOldLen = -1;
            for (int ui = 0; SPECS[ui]; ++ui) {
                const char *const U_SPEC = SPECS[ui];
                const int uLen = (int)strlen(U_SPEC);

                if (verbose) {
                    cout << "\t\tFor source objects of length "
                         << uLen << ":\t";
                    P(U_SPEC);
                }

                LOOP_ASSERT(U_SPEC, uOldLen < uLen);  // strictly increasing
                uOldLen = uLen;

                const Obj UU = g(U_SPEC);               // control
                LOOP_ASSERT(ui, uLen == UU.length());   // same lengths

                for (int uj = 0; uj < NUM_EXTEND; ++uj) {
                    const int U_N = EXTEND[uj];

                    Obj mU(&testAllocator);     stretchRemoveAll(&mU, U_N);
                    const Obj& U = mU;          gg(&mU, U_SPEC);

                    bdex_TestOutStream out;

                    out.putVersion(VERSION);
                    bdex_OutStreamFunctions::streamOut(out, U, VERSION);
                    // testing stream-out operator here

                    const char *const OD  = out.data();
                    const int         LOD = out.length();

                    // Must reset stream for each iteration of inner loop.
                    bdex_TestInStream testInStream(OD, LOD);
                    LOOP2_ASSERT(U_SPEC, U_N, testInStream);
                    LOOP2_ASSERT(U_SPEC, U_N, !testInStream.isEmpty());

                    for (int vi = 0; SPECS[vi]; ++vi) {
                        const char *const V_SPEC = SPECS[vi];
                        const int vLen = (int)strlen(V_SPEC);

                        const Obj VV = g(V_SPEC);               // control

                        if ((0 == uj && veryVerbose) || veryVeryVerbose) {
                            cout << "\t\t\tFor destination objects of length "
                                                        << vLen << ":\t";
                            P(V_SPEC);
                        }

                        const int Z = ui == vi; // flag indicating same values

                        for (int vj = 0; vj < NUM_EXTEND; ++vj) {
                          BEGIN_BSLMA_EXCEPTION_TEST {
                          BEGIN_BDEX_EXCEPTION_TEST {
                            testInStream.reset();
                            const int V_N = EXTEND[vj];
                            const int AL = testAllocator.allocationLimit();
                            testAllocator.setAllocationLimit(-1);
            //--------------^
            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, testInStream);
            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, !testInStream.isEmpty());
            //--------------v
                            Obj mV(&testAllocator); stretchRemoveAll(&mV, V_N);
                            const Obj& V = mV;      gg(&mV, V_SPEC);

                            static int firstFew = 2 * NUM_EXTEND * NUM_EXTEND;
                            if (veryVeryVerbose ||
                                               (veryVerbose && firstFew > 0)) {
                                cout << "\t |"; P_(U_N); P_(V_N); P_(U); P(V);
                                --firstFew;
                            }

                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, UU == U);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == V);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, Z==(U==V));

                            testAllocator.setAllocationLimit(AL);
                            int version;
                            testInStream.getVersion(version);
                            ASSERT(VERSION == version);
                            bdex_InStreamFunctions::streamIn(testInStream,
                                                             mV,
                                                             VERSION);
                            // test stream-in operator here

                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, UU == U);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, UU == V);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N,  U == V);

                          } END_BDEX_EXCEPTION_TEST
                          } END_BSLMA_EXCEPTION_TEST
                        }
                    }
                }
            }
        }

        if (verbose) cout << "\tOn empty and invalid streams." << endl;
        {
            static const char *SPECS[] = {
                "",      "0",      "01",     "011",    "0110",   "01100",
                "0110001",         "01100011",         "011000110",
                "011000110001100", "0110001100011000", "01100011000110001",
                "0000100011100001001100000100110",
                "00001000111000010011000001001101",
           "000010001110000100110000010011010000100011100001001100000100110",
           "0000100011100001001100000100110100001000111000010011000001001100",
           "00001000111000010011000001001101000010001110000100110000010011001",
            0}; // Null string required as last element.

            static const int EXTEND[] = {
                0, 1, 2, 3, 4, 5, 8, 16, 31, 32, 33, 63, 64, 65, 100
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            bdex_TestInStream testInStream("", 0);
            testInStream.setSuppressVersionCheck(1);

            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC = SPECS[ti];
                const int curLen = (int)strlen(SPEC);

                if (verbose) cout << "\t\tFor objects of length "
                                                    << curLen << '.' << endl;
                LOOP_ASSERT(SPEC, oldLen < curLen); // strictly increasing
                oldLen = curLen;

                if (veryVerbose) { cout << "\t\t\t"; P(SPEC); }

                // Create control object X.

                Obj mX(&testAllocator); gg(&mX, SPEC); const Obj& X = mX;
                LOOP_ASSERT(ti, curLen == X.length());  // same lengths

                for (int ei = 0; ei < NUM_EXTEND; ++ei) {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                  BEGIN_BDEX_EXCEPTION_TEST {
                    testInStream.reset();

                    const int N = EXTEND[ei];

                    if (veryVerbose) { cout << "\t\t\t\t"; P(N); }

                    Obj t(&testAllocator);      gg(&t, SPEC);
                    stretchRemoveAll(&t, N);    gg(&t, SPEC);

                  // Ensure that reading from an empty or invalid input stream
                  // leaves the stream invalid and the target object unchanged.

                                        LOOP2_ASSERT(ti, ei, testInStream);
                                        LOOP2_ASSERT(ti, ei, X == t);

                    bdex_InStreamFunctions::streamIn(testInStream,
                                                     t,
                                                     VERSION);
                    LOOP2_ASSERT(ti, ei, !testInStream);
                    LOOP2_ASSERT(ti, ei, X == t);

                    bdex_InStreamFunctions::streamIn(testInStream,
                                                     t,
                                                     VERSION);
                    LOOP2_ASSERT(ti, ei, !testInStream);
                    LOOP2_ASSERT(ti, ei, X == t);

                  } END_BDEX_EXCEPTION_TEST
                  } END_BSLMA_EXCEPTION_TEST
                }
            }
        }

        if (verbose) cout <<
            "\tOn incomplete (but otherwise valid) data." << endl;
        {
            const Obj X1 = g("1010"),   Y1 = g("11"),    Z1 = g("01100");
            const Obj X2 = g("000"),    Y2 = g("10100"), Z2 = g("1");
            const Obj X3 = g("00000"),  Y3 = g("1"),     Z3 = g("0100");

            bdex_TestOutStream out;
            bdex_OutStreamFunctions::streamOut(out, Y1, VERSION);
            const int LOD1 = out.length();
            bdex_OutStreamFunctions::streamOut(out, Y2, VERSION);
            const int LOD2 = out.length();
            bdex_OutStreamFunctions::streamOut(out, Y3, VERSION);
            const int LOD  = out.length();
            const char *const OD = out.data();

            for (int i = 0; i < LOD; ++i) {
                bdex_TestInStream testInStream(OD, i);
                bdex_TestInStream& in = testInStream;
                in.setSuppressVersionCheck(1);
                LOOP_ASSERT(i, in);  LOOP_ASSERT(i, !i == in.isEmpty());

                if (veryVerbose) { cout << "\t\t"; P(i); }

                Obj t1(X1, &testAllocator),
                    t2(X2, &testAllocator),
                    t3(X3, &testAllocator);

                if (i < LOD1) {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                  BEGIN_BDEX_EXCEPTION_TEST { in.reset();
                    const int AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);
                    LOOP_ASSERT(i, in);  LOOP_ASSERT(i, !i == in.isEmpty());
                    t1 = X1;
                    t2 = X2;
                    t3 = X3;

                    testAllocator.setAllocationLimit(AL);
                    bdex_InStreamFunctions::streamIn(in, t1, VERSION);
                    LOOP_ASSERT(i, !in);
                    if (0 == i) LOOP_ASSERT(i, X1 == t1);
                    bdex_InStreamFunctions::streamIn(in, t2, VERSION);
                    LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, X2 == t2);
                    bdex_InStreamFunctions::streamIn(in, t3, VERSION);
                    LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, X3 == t3);
                  } END_BDEX_EXCEPTION_TEST
                  } END_BSLMA_EXCEPTION_TEST
                }
                else if (i < LOD2) {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                  BEGIN_BDEX_EXCEPTION_TEST { in.reset();
                    const int AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);
                    LOOP_ASSERT(i, in);  LOOP_ASSERT(i, !i == in.isEmpty());
                    t1 = X1;
                    t2 = X2;
                    t3 = X3;

                    testAllocator.setAllocationLimit(AL);
                    bdex_InStreamFunctions::streamIn(in, t1, VERSION);
                    LOOP_ASSERT(i,  in);  LOOP3_ASSERT(i, Y1, t1, Y1 == t1);
                    bdex_InStreamFunctions::streamIn(in, t2, VERSION);
                    LOOP_ASSERT(i, !in);
                    if (LOD1 == i) LOOP_ASSERT(i, X2 == t2);
                    bdex_InStreamFunctions::streamIn(in, t3, VERSION);
                    LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, X3 == t3);
                  } END_BDEX_EXCEPTION_TEST
                  } END_BSLMA_EXCEPTION_TEST
                }
                else {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                  BEGIN_BDEX_EXCEPTION_TEST {
                    in.reset();
                    testAllocator.setAllocationLimit(-1);
                    LOOP_ASSERT(i, in);  LOOP_ASSERT(i, !i == in.isEmpty());
                    t1 = X1;
                    t2 = X2;
                    t3 = X3;

                    bdex_InStreamFunctions::streamIn(in, t1, VERSION);
                    LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, Y1 == t1);
                    bdex_InStreamFunctions::streamIn(in, t2, VERSION);
                    LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, Y2 == t2);
                    bdex_InStreamFunctions::streamIn(in, t3, VERSION);
                    LOOP_ASSERT(i, !in);
                    if (LOD2 == i) LOOP_ASSERT(i, X3 == t3);
                  } END_BDEX_EXCEPTION_TEST
                  } END_BSLMA_EXCEPTION_TEST
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

        const Obj W = g("");            // default value
        const Obj X = g("01100");       // original value
        const Obj Y = g("011");         // new value

        if (verbose) cout << "\t\tGood stream (for control)." << endl;
        {
            const int length   = 3;

            bdex_TestOutStream out;
            out.putVersion(VERSION);
            out.putLength(length);
            out.putInt32(0);
            out.putInt32(6);
            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);   ASSERT(W != t);    ASSERT(X == t);      ASSERT(Y != t);
            bdex_TestInStream in(OD, LOD); ASSERT(in);
            int version;
            in.getVersion(version);
            ASSERT(VERSION == version);
            bdex_InStreamFunctions::streamIn(in, t, version); ASSERT(in);
            ASSERT(W != t);    ASSERT(X != t);      ASSERT(Y == t);
        }

        if (verbose) cout << "\t\tBad version." << endl;
        {
            const char version = 0; // too small
            const int length   = 3;

            bdex_TestOutStream out;
            out.putLength(length);
            out.putInt32(0);
            out.putInt32(6);
            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);   ASSERT(W != t);    ASSERT(X == t);      ASSERT(Y != t);
            bdex_TestInStream in(OD, LOD);
            in.setSuppressVersionCheck(1);
            ASSERT(in);
            bdex_InStreamFunctions::streamIn(in, t, version);
            ASSERT(!in);
            ASSERT(W != t);    ASSERT(X == t);      ASSERT(Y != t);
        }
        {
            const char version = 2; // too large
            const int length   = 3;

            bdex_TestOutStream out;
            out.putLength(length);
            out.putInt32(0);
            out.putInt32(6);
            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);   ASSERT(W != t);    ASSERT(X == t);      ASSERT(Y != t);
            bdex_TestInStream in(OD, LOD); ASSERT(in);
            bdex_InStreamFunctions::streamIn(in, t, version);
            ASSERT(!in);
            ASSERT(W != t);    ASSERT(X == t);      ASSERT(Y != t);
        }

        if (verbose) cout << "\t\tTruncated data." << endl;
        {
            const char version = 1;
            const int length   = 3;

            bdex_TestOutStream out;
            out.putLength(length);
            out.putInt32(6);
            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);   ASSERT(W != t);    ASSERT(X == t);      ASSERT(Y != t);
            bdex_TestInStream in(OD, LOD); ASSERT(in);
            bdex_InStreamFunctions::streamIn(in, t, version);
            ASSERT(!in);
            ASSERT(W == t);    ASSERT(X != t);      ASSERT(Y != t);
        }

        if (verbose) cout << "\t\tData past length." << endl;
        {
            const char version = 1;
            const int length   = 3;

            bdex_TestOutStream out;
            out.putLength(length);
            out.putInt32(0);
            out.putInt32(8);
            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);   ASSERT(W != t);    ASSERT(X == t);      ASSERT(Y != t);
            bdex_TestInStream in(OD, LOD); ASSERT(in);
            bdex_InStreamFunctions::streamIn(in, t, version);
            ASSERT(!in);
            ASSERT(W != t);    ASSERT(X != t);      ASSERT(Y != t);
        }

        if (verbose) cout << "\nTesting 'maxSupportedBdexVersion'." << endl;
        {
            const Obj X;
            ASSERT(1 == X.maxSupportedBdexVersion());
        }

        if (verbose) cout << "\nWire format direct tests." << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_version;  // version to stream with
                int         d_length;   // expect output length
                const char *d_fmt_p;    // expected output format
            } DATA[] = {
                //line  spec           ver  len  format
                //----  -------------  ---  ---  --------------------------
                { L_,   "",              0,   0, ""                         },
                { L_,   "0",             0,   0, ""                         },

                { L_,   "",              1,   1, "\x00"                     },

                { L_,   "0",             1,   9,
                                     "\x01\x00\x00\x00\x00\x00\x00\x00\x00" },
                { L_,   "01",            1,   9,
                                     "\x02\x00\x00\x00\x00\x00\x00\x00\x02" },
                { L_,   "010",           1,   9,
                                     "\x03\x00\x00\x00\x00\x00\x00\x00\x02" },
                { L_,   "0101",          1,   9,
                                     "\x04\x00\x00\x00\x00\x00\x00\x00\x0a" },
                { L_,   "01010",         1,   9,
                                     "\x05\x00\x00\x00\x00\x00\x00\x00\x0a" },

                { L_,   "00001000111000010011000001001101",
                                         1,   9,
                                     "\x20\x00\x00\x00\x00\xb2\x0c\x87\x10" },
                { L_,   "000010001110000100110000010011011",
                                         1,   9,
                                     "\x21\x00\x00\x00\x01\xb2\x0c\x87\x10" },

                { L_,   "00001000111000010011000001001101"
                        "11000000001100001110000000000111",
                                         1,   9,
                                     "\x40\xe0\x07\x0c\x03\xb2\x0c\x87\x10" },
                { L_,   "00001000111000010011000001001101"
                        "110000000011000011100000000001111",
                                         1,  17,
                                     "\x41\xe0\x07\x0c\x03\xb2\x0c\x87\x10"
                                         "\x00\x00\x00\x00\x00\x00\x00\x01" },

                { L_,   "00001000111000010011000001001101"
                        "11000000001100001110000000000111"
                        "01001100011100001111000111001101",
                                         1,  17,
                                     "\x60\xe0\x07\x0c\x03\xb2\x0c\x87\x10"
                                         "\x00\x00\x00\x00\xb3\x8f\x0e\x32" },
                { L_,   "00001000111000010011000001001101"
                        "11000000001100001110000000000111"
                        "010011000111000011110001110011011",
                                         1,  17,
                                     "\x61\xe0\x07\x0c\x03\xb2\x0c\x87\x10"
                                         "\x00\x00\x00\x01\xb3\x8f\x0e\x32" },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int LINE         = DATA[i].d_lineNum;
                const char *const SPEC = DATA[i].d_spec_p;
                const int VERSION      = DATA[i].d_version;
                const int LEN          = DATA[i].d_length;
                const char *const FMT  = DATA[i].d_fmt_p;

                Obj mX(&testAllocator);
                mX = g(SPEC);
                const Obj& X = mX;
                bdex_ByteOutStream out;  X.bdexStreamOut(out, VERSION);

                LOOP_ASSERT(LINE, LEN == out.length());
                LOOP_ASSERT(LINE, 0 == memcmp(out.data(),
                                              FMT,
                                              LEN));

                if (verbose && LEN != out.length()) {
                    P_(LINE);
                    P(out.length());
                }
                if (verbose && memcmp(out.data(), FMT, LEN)) {
                    const char *hex = "0123456789abcdef";
                    P_(LINE);
                    for (int j = 0; j < out.length(); ++j) {
                        cout << "\\x"
                             << hex[(unsigned char)*
                                          (out.data() + j) >> 4]
                             << hex[(unsigned char)*
                                       (out.data() + j) & 0x0f];
                    }
                    cout << endl;
                }

                Obj mY(&testAllocator);  const Obj& Y = mY;
                if (LEN) { // version is supported
                    bdex_ByteInStream in(out.data(),
                                         out.length());
                    mY.bdexStreamIn(in, VERSION);
                }
                else { // version is not supported
                    mY = X;
                    bdex_ByteInStream in;
                    mY.bdexStreamIn(in, VERSION);
                    LOOP_ASSERT(LINE, !in);
                }
                LOOP_ASSERT(LINE, X == Y);
            }
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR AND SWAP METHOD:
        // We have the following concerns:
        //   1.  The value represented by any object can be assigned to any
        //         other object regardless of how either value is represented
        //         internally.
        //   2.  The 'rhs' value must not be affected by the operation.
        //   3.  'rhs' going out of scope has no effect on the value of 'lhs'
        //       after the assignment.
        //   4.  Aliasing (x = x): The assignment operator must always work --
        //       even when the lhs and rhs are identically the same object.
        //   5.  The assignment operator must be neutral with respect to memory
        //       allocation exceptions.
        //   6.  Swapping objects with different content should work: empty
        //       with empty, empty with non-empty and non-empty with non-empty.
        //   7.  Swapping two objects should not involve memory allocation
        //       (because it's no-throw).
        //   8.  Free function 'swap' and member function 'swap' must do the
        //       same thing.
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
        //   before the assignment that UU == u, VV == v, and v == u if and
        //   only if VV == UU.  After the assignment, assert that VV == u,
        //   VV == v, and, for grins, that v == u.  Let v go out of scope and
        //   confirm that VV == u.  All of these tests are performed within the
        //   'bdema' exception testing apparatus.
        //
        //   As a separate exercise, we address 4 and 5 by constructing tests
        //   y = y for all y in T.  Using a canonical control X, we will verify
        //   that X == y before and after the assignment, again within
        //   the bdema exception testing apparatus.
        //
        //   To address concerns 6-8, create objects with needed properties,
        //   apply 'swap' to them and check the correctness of the result.
        //
        // Testing:
        //   bdea_BitArray& operator=(const bdea_BitArray& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Assignment Operator" << endl
                          << "===========================" << endl;

        if (verbose) cout <<
            "\nAssign cross product of values with varied representations."
                          << endl;
        
        {
            static const char *SPECS[] = {
                "",      "0",      "01",     "011",    "0110",   "01100",
                "0110001",         "01100011",         "011000110",
                "011000110001100", "0110001100011000", "01100011000110001",
                "0000100011100001001100000100110",
                "00001000111000010011000001001101",
           "000010001110000100110000010011010000100011100001001100000100110",
           "0000100011100001001100000100110100001000111000010011000001001100",
           "00001000111000010011000001001101000010001110000100110000010011001",
            0}; // Null string required as last element.

            static const int EXTEND[] = {
                0, 1, 2, 3, 4, 5, 8, 16, 31, 32, 33, 63, 64, 65, 100
            };
           const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            int uOldLen = -1;
            for (int ui = 0; SPECS[ui]; ++ui) {
                const char *const U_SPEC = SPECS[ui];
                const int uLen = (int)strlen(U_SPEC);

                if (verbose) {
                    cout << "\tFor lhs objects of length " << uLen << ":\t";
                    P(U_SPEC);
                }

                LOOP_ASSERT(U_SPEC, uOldLen < uLen);  // strictly increasing
                uOldLen = uLen;

                const Obj UU = g(U_SPEC);               // control
                LOOP_ASSERT(ui, uLen == UU.length());   // same lengths

                // int vOldLen = -1;
                for (int vi = 0; SPECS[vi]; ++vi) {
                    const char *const V_SPEC = SPECS[vi];
                    const int vLen = (int)strlen(V_SPEC);

                    if (veryVerbose) {
                        cout << "\t\tFor rhs objects of length " << vLen
                                                                 << ":\t";
                        P(V_SPEC);
                    }

                    const Obj VV = g(V_SPEC);           // control

                    const int Z = ui == vi; // flag indicating same values

                    for (int uj = 0; uj < NUM_EXTEND; ++uj) {
                        const int U_N = EXTEND[uj];
                        for (int vj = 0; vj < NUM_EXTEND; ++vj) {
                            const int V_N = EXTEND[vj];

                          BEGIN_BSLMA_EXCEPTION_TEST {
                            const int AL = testAllocator.allocationLimit();
                            testAllocator.setAllocationLimit(-1);
                            Obj mU(&testAllocator); stretchRemoveAll(&mU, U_N);
                            const Obj& U = mU; gg(&mU, U_SPEC);
                            {
                            //--^
                            Obj mV(&testAllocator); stretchRemoveAll(&mV, V_N);
                            const Obj& V = mV; gg(&mV, V_SPEC);

                            static int firstFew = 2 * NUM_EXTEND * NUM_EXTEND;
                            if (veryVeryVerbose ||
                                               (veryVerbose && firstFew > 0)) {
                                cout << "\t| "; P_(U_N); P_(V_N); P_(U); P(V);
                                --firstFew;
                            }

                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, UU == U);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == V);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, Z==(V==U));

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
                    }
                }
            }
        }

        if (verbose) cout << "\nTesting self assignment (Aliasing)." << endl;

        {
            static const char *SPECS[] = {
                "",      "0",      "01",     "011",    "0110",   "01100",
                "0110001",         "01100011",         "011000110",
                "011000110001100", "0110001100011000", "01100011000110001",
                "0000100011100001001100000100110",
                "00001000111000010011000001001101",
           "000010001110000100110000010011010000100011100001001100000100110",
           "0000100011100001001100000100110100001000111000010011000001001100",
           "00001000111000010011000001001101000010001110000100110000010011001",
            0}; // Null string required as last element.

            static const int EXTEND[] = {
                0, 1, 2, 3, 4, 5, 8, 16, 31, 32, 33, 63, 64, 65, 100
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC = SPECS[ti];
                const int curLen = (int)strlen(SPEC);

                if (verbose) {
                    cout << "\tFor an object of length " << curLen << ":\t";
                    P(SPEC);
                }
                LOOP_ASSERT(SPEC, oldLen < curLen);  // strictly increasing
                oldLen = curLen;

                const Obj X = g(SPEC);                  // control
                LOOP_ASSERT(ti, curLen == X.length());  // same lengths

                for (int tj = 0; tj < NUM_EXTEND; ++tj) {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    const int AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);

                    const int N = EXTEND[tj];
                    Obj mY(&testAllocator);  stretchRemoveAll(&mY, N);
                    const Obj& Y = mY;       gg(&mY, SPEC);

                    if (veryVerbose) { cout << "\t\t"; P_(N); P(Y); }

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

        if (verbose) {
            cout << "Testing 'swap' method\n"
                 << "=====================\n";
        }

        {
            if (veryVerbose) {
                cout << "General 'swap'piness\n";
            }

            {
                const char *specs[] = { "", "1001" };
                const int specSize = sizeof(specs) / sizeof(*specs);

                for (int i = 0; i < specSize; ++i) {
                    for (int j = 0; j < specSize; ++j) {
                        Obj X1(g(specs[i])), X2(X1);
                        Obj Y1(g(specs[j])), Y2(Y1);

                        X1.swap(Y1);

                        LOOP3_ASSERT(specs[i], i, j, X1 == Y2);
                        LOOP3_ASSERT(specs[j], i, j, X2 == Y1);
                    }
                }
            }

            if (veryVerbose) {
                cout << "'swap' shouldn't allocate memory\n";
            }

            {
                Obj X1(g("11"), &testAllocator);
                Obj Y1(g("10"), &testAllocator);
                int blocksTotalBefore = testAllocator.numBlocksTotal();
                int blocksInUseBefore = testAllocator.numBlocksInUse();

                X1.swap(Y1);

                ASSERT(testAllocator.numBlocksTotal() == blocksTotalBefore);
                ASSERT(testAllocator.numBlocksInUse() == blocksInUseBefore);
            }

            if (veryVerbose) {
                cout << "'swap' method and free function are equivalent\n";
            }

            {
                using bsl::swap;

                Obj X1(g("01")), X2(X1);
                Obj Y1(g("10")), Y2(Y1);

                X1.swap(Y1);
                swap(X2, Y2);

                ASSERT(X1 == X2);
                ASSERT(Y1 == Y2);
            }
        }
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
        //   object returned (by value) from the generator function, 'g(SPEC)'
        //   with the value of a newly constructed OBJECT configured using
        //   'gg(&OBJECT, SPEC)'.  Compare the results of calling the
        //   allocator's 'numBlocksTotal' and 'numBytesInUse' methods before
        //   and after calling 'g' in order to demonstrate that 'g' has no
        //   effect on the test allocator.  Finally, use 'sizeof' to confirm
        //   that the (temporary) returned by 'g' differs in size from that
        //   returned by 'gg'.
        //
        // Testing:
        //   bdea_BitArray g(const char *spec);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Generator Function 'g'" << endl
                          << "==============================" << endl;

        static const char *SPECS[] = {
            "", "~", "0", "1", "0~1~1~0~0", "01100", "011~00",
        0}; // Null string required as last element.

        if (verbose) cout <<
            "\nCompare values produced by 'g' and 'gg' for various inputs."
                          << endl;
        for (int ti = 0; SPECS[ti]; ++ti) {
            const char *spec = SPECS[ti];
            if (veryVerbose) { P_(ti);  P(spec); }
            Obj mX(&testAllocator);  gg(&mX, spec);  const Obj& X = mX;
            if (veryVerbose) {
                cout << "\t g = " << g(spec) << endl;
                cout << "\tgg = " << X       << endl;
            }
            const int TOTAL_BLOCKS_BEFORE = testAllocator.numBlocksTotal();
            const int IN_USE_BYTES_BEFORE = testAllocator.numBytesInUse();
            LOOP_ASSERT(ti, X == g(spec));
            const int TOTAL_BLOCKS_AFTER = testAllocator.numBlocksTotal();
            const int IN_USE_BYTES_AFTER = testAllocator.numBytesInUse();
            LOOP_ASSERT(ti, TOTAL_BLOCKS_BEFORE == TOTAL_BLOCKS_AFTER);
            LOOP_ASSERT(ti, IN_USE_BYTES_BEFORE == IN_USE_BYTES_AFTER);
        }

        if (verbose) cout << "\nConfirm return-by-value." << endl;
        {
            const char *spec = "01100";

            ASSERT(sizeof(Obj) == sizeof g(spec));      // compile-time fact

            Obj x(&testAllocator);                      // runtime tests
            Obj& r1 = gg(&x, spec);
            Obj& r2 = gg(&x, spec);
            const Obj& r3 = g(spec);
            const Obj& r4 = g(spec);
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
        //   1) The new object's value is the same as that of the original
        //       object (relying on the previously tested equality operators).
        //   2) All internal representations of a given value can be used to
        //        create a new object of equivalent value.
        //   3) The value of the original object is left unaffected.
        //   4) Subsequent changes in or destruction of the source object have
        //      no effect on the copy-constructed object.
        //   5) The function is exception neutral w.r.t. memory allocation.
        //   6) The object has its internal memory management system hooked up
        //         properly so that *all* internally allocated memory draws
        //         from a user-supplied allocator whenever one is specified.
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
        // Testing:
        //   bdea_BitArray(const bdea_BitArray& original, *ba = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Copy Constructor" << endl
                          << "========================" << endl;

        if (verbose) cout <<
            "\nCopy construct values with varied representations." << endl;
        {
            static const char *SPECS[] = {
                "",      "0",      "01",     "011",    "0110",   "01100",
                "0110001",         "01100011",         "011000110",
                "011000110001100", "0110001100011000", "01100011000110001",
                "0000100011100001001100000100110",
                "00001000111000010011000001001101",
           "000010001110000100110000010011010000100011100001001100000100110",
           "0000100011100001001100000100110100001000111000010011000001001100",
           "00001000111000010011000001001101000010001110000100110000010011001",
            0}; // Null string required as last element.

            static const int EXTEND[] = {
                0, 1, 2, 3, 4, 5, 8, 16, 31, 32, 33, 63, 64, 65, 100
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC = SPECS[ti];
                const int curLen = (int)strlen(SPEC);

                if (verbose) {
                    cout << "\tFor an object of length " << curLen << ":\t";
                    P(SPEC);
                }

                LOOP_ASSERT(SPEC, oldLen < curLen); // strictly increasing
                oldLen = curLen;

                // Create control object w.
                Obj mW(&testAllocator); gg(&mW, SPEC); const Obj& W = mW;
                LOOP_ASSERT(ti, curLen == W.length()); // same lengths
                if (veryVerbose) { cout << "\t"; P(W); }

                // Stretch capacity of x object by different amounts.

                for (int ei = 0; ei < NUM_EXTEND; ++ei) {
                    const int N = EXTEND[ei];
                    if (veryVerbose) { cout << "\t\t"; P(N) }

                    Obj *pX = new Obj(&testAllocator);
                    Obj &mX = *pX;              stretchRemoveAll(&mX, N);
                    const Obj& X = mX;          gg(&mX, SPEC);
                    if (veryVerbose) { cout << "\t\t"; P(X); }

                    {                                   // No allocator.
                        const Obj Y0(X);
                        if (veryVerbose) { cout << "\t\t\t"; P(Y0); }
                        LOOP2_ASSERT(SPEC, N, W == Y0);
                        LOOP2_ASSERT(SPEC, N, W == X);
                    }

                    {                                   // Null allocator.
                        const Obj Y1(X, (bslma_Allocator *) 0);
                        if (veryVerbose) { cout << "\t\t\t"; P(Y1); }
                        LOOP2_ASSERT(SPEC, N, W == Y1);
                        LOOP2_ASSERT(SPEC, N, W == X);
                    }

                    BEGIN_BSLMA_EXCEPTION_TEST {        // Test allocator.
                        const Obj Y2(X, &testAllocator);
                        if (veryVerbose) { cout << "\t\t\t"; P(Y2); }
                        LOOP2_ASSERT(SPEC, N, W == Y2);
                        LOOP2_ASSERT(SPEC, N, W == X);
                    } END_BSLMA_EXCEPTION_TEST

                    {                                   // Buffer Allocator.
                        char memory[1024];
                        bdema_BufferedSequentialAllocator a(memory,
                                                            sizeof memory);
                        Obj *Y = new(a.allocate(sizeof(Obj))) Obj(X, &a);
                        if (veryVerbose) { cout << "\t\t\t"; P(*Y); }
                        LOOP2_ASSERT(SPEC, N, W == *Y);
                        LOOP2_ASSERT(SPEC, N, W == X);
                    }

                    {                             // with 'original' destroyed
                        const Obj Y2(X, &testAllocator);

                        // testAllocator will erase the footprint of pX
                        // preventing further reference to this object.

                        delete pX;
                        if (veryVerbose) { cout << "\t\t\t"; P(Y2); }
                        LOOP2_ASSERT(SPEC, N, W == Y2);
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
        //   or subtle differences, ordered by non-decreasing length.  Verify
        //   the correctness of 'operator==' and 'operator!=' (returning either
        //   true or false) using all elements (u, v) of the cross product
        //   S X S.
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
        // Testing:
        //   operator==(const bdea_BitArray&, const bdea_BitArray&);
        //   operator!=(const bdea_BitArray&, const bdea_BitArray&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Equality Operators" << endl
                          << "==========================" << endl;

        if (verbose) cout <<
            "\nCompare each pair of similar values (u, v) in S X S." << endl;
        {
            static const char *SPECS[] = {
                "",
                "0",      "1",
                "00",     "01",     "11",     "10",
                "000",    "100",    "010",    "001",
                "0000",   "1000",   "0100",   "0010",   "0001",
                "00000",  "10000",  "01000",  "00100",  "00010",  "00001",
                "000000", "100000", "001000", "000100", "000001",
                "0000000",          "1000000",          "0000010",
                "00000000",         "01000000",         "00000100",
                "000000000",        "001000000",        "000001000",
                "0000000000",       "0001000000",       "0000010000",
                "0000000000000000000000000000000",
                "0000000000000000000000000000001",
                "00000000000000000000000000000000",
                "00000000000000000000000000000001",
                "00000100000000000000000000000000",
                "00000010000000000000000000000000",
           "000000000000000000000000000000000000000000000000000000000000000",
           "000000000000000000000000000000000000000000000000000000000000001",
           "0000000000000000000000000000000000000000000000000000000000000000",
           "0000000000000000000000000000000000000000000000000000000000000001",
           "0000010000000000000000000000000000000000000000000000000000000000",
           "0000001000000000000000000000000000000000000000000000000000000000",
            0}; // Null string required as last element.

            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const U_SPEC = SPECS[ti];
                const int curLen = (int)strlen(U_SPEC);

                Obj mU(&testAllocator); gg(&mU, U_SPEC); const Obj& U = mU;
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
                    Obj mV(&testAllocator); gg(&mV, V_SPEC); const Obj& V = mV;

                    if (veryVerbose) { cout << "  "; P_(tj); P_(V_SPEC); P(V);}
                    const bool isSame = ti == tj;
                    LOOP2_ASSERT(ti, tj,  isSame == (U == V));
                    LOOP2_ASSERT(ti, tj, !isSame == (U != V));
                }
            }
        }

        if (verbose) cout << "\nCompare objects of equal value having "
                             "potentially different internal state." << endl;
        {
            static const char *SPECS[] = {
                "",      "0",      "01",     "011",    "0110",   "01100",
                "0110001",         "01100011",         "011000110",
                "011000110001100", "0110001100011000", "01100011000110001",
                "0000100011100001001100000100110",
                "00001000111000010011000001001101",
           "000010001110000100110000010011010000100011100001001100000100110",
           "0000100011100001001100000100110100001000111000010011000001001100",
           "00001000111000010011000001001101000010001110000100110000010011001",
            0}; // Null string required as last element.

            static const int EXTEND[] = {
                0, 1, 2, 3, 4, 5, 8, 16, 31, 32, 33, 63, 64, 65, 100
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC = SPECS[ti];
                const int curLen = (int)strlen(SPEC);

                if (curLen != oldLen) {
                    if (verbose) cout << "\tUsing objects having (logical) "
                                         "length " << curLen << '.' << endl;
                    LOOP_ASSERT(SPEC, oldLen < curLen); // strictly increasing
                    oldLen = curLen;
                }

                Obj mX(&testAllocator); gg(&mX, SPEC); const Obj& X = mX;
                LOOP_ASSERT(ti, curLen == X.length()); // same lengths
                if (veryVerbose) { cout << "\t\t"; P_(ti); P_(SPEC); P(X)}

                for (int u = 0; u < NUM_EXTEND; ++u) {
                    const int U_N = EXTEND[u];
                    Obj mU(&testAllocator);  stretchRemoveAll(&mU, U_N);
                    const Obj& U = mU;       gg(&mU, SPEC);

                    if (veryVerbose) { cout << "\t\t\t"; P_(U_N); P(U)}

                    // compare canonical representation with every variation

                    LOOP2_ASSERT(SPEC, U_N, 1 == (U == X));
                    LOOP2_ASSERT(SPEC, U_N, 1 == (X == U));
                    LOOP2_ASSERT(SPEC, U_N, 0 == (U != X));
                    LOOP2_ASSERT(SPEC, U_N, 0 == (X != U));

                    for (int v = 0; v < NUM_EXTEND; ++v) {
                        const int V_N = EXTEND[v];
                        Obj mV(&testAllocator);  stretchRemoveAll(&mV, V_N);
                        const Obj& V = mV;       gg(&mV, SPEC);

                        static int firstFew = 2 * NUM_EXTEND * NUM_EXTEND;
                        if (veryVeryVerbose || (veryVerbose && firstFew > 0)) {
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
        // --------------------------------------------------------------------
        // TESTING PRINT METHOD
        //   The print method formats the value of the object directly from
        //   the underlying state information according to supplied arguments.
        //   Ensure that the method formats properly for:
        //     - empty and non-empty values
        //     - negative, 0, and positive levels.
        //     - 0 and non-zero spaces per level.
        // Plan:
        //   For each of an enumerated set of object, 'level', and
        //   'spacesPerLevel' values, ordered by increasing object length, use
        //   'ostrstream' to 'print' that object's value, using the tabulated
        //   parameters, to two separate character buffers each with different
        //   initial values.  Compare the contents of these buffers with the
        //   literal expected output format and verify that the characters
        //   beyond the null characters are unaffected in both buffers.
        //
        // Testing:
        //   ostream& print(ostream& stream, int level, int spacesPerLevel);
        //   operator<<(ostream&, const bdea_BitArray&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'print' method" << endl
                          << "======================" << endl;

        if (verbose) cout << "\nTesting 'print' (ostream)." << endl;
#define NL "\n"
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_indent;   // indentation level
                int         d_spaces;   // spaces per indentation level
                const char *d_fmt_p;    // expected output format
            } DATA[] = {
                //line  spec    indent +/-  spaces/Tab  format        // ADJUST
                //----  ----    ----------  ----------  ------------------
                { L_,   "",        0,         0,        "["           NL
                                                        "]"           NL  },

                { L_,   "",        0,        -1,        "[ ]"             },

                { L_,   "",        0,         2,        "["           NL
                                                        "]"           NL  },

                { L_,   "",        1,         1,        " ["          NL
                                                        " ]"          NL  },

                { L_,   "",        1,         2,        "  ["         NL
                                                        "  ]"         NL  },

                { L_,   "",       -1,         2,        "["           NL
                                                        "  ]"         NL  },

                { L_,   "0",       0,         0,        "["           NL
                                                        "0"           NL
                                                        "]"           NL  },

                { L_,   "0",       0,        -1,        "[ 0 ]"           },

                { L_,   "0",      -2,         1,        "["           NL
                                                        "   0"        NL
                                                        "  ]"         NL  },

                { L_,   "10",      1,         2,        "  ["         NL
                                                        "    1"       NL
                                                        "    0"       NL
                                                        "  ]"         NL  },

                { L_,   "10",       0,        -1,        "[ 10 ]"          },

                { L_,   "11",      2,         1,        "  ["         NL
                                                        "   1"        NL
                                                        "   1"        NL
                                                        "  ]"         NL  },

                { L_,   "01010",   1,         3,        "   ["        NL
                                                        "      0"     NL
                                                        "      1"     NL
                                                        "      0"     NL
                                                        "      1"     NL
                                                        "      0"     NL
                                                        "   ]"        NL  },

                { L_,   "10101",    0,        -1,        "[ 10101 ]"      },
            };
#undef NL

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const int SIZE = 1000; // Must be big enough to hold output string.
            const char Z1 = (char) 0xFF;
                                   // Value 1 used to represent an unset char.
            const char Z2 = 0x00;  // Value 2 used to represent an unset char.

            char mCtrlBuf1[SIZE];  memset(mCtrlBuf1, Z1, SIZE);
            char mCtrlBuf2[SIZE];  memset(mCtrlBuf2, Z2, SIZE);
            char mCtrlBuf3[SIZE];  memset(mCtrlBuf3, Z2, SIZE);
            const char *CTRL_BUF1 = mCtrlBuf1;
            const char *CTRL_BUF2 = mCtrlBuf2;
            const char *CTRL_BUF3 = mCtrlBuf3;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA;  ++ti) {
                const int         LINE   = DATA[ti].d_lineNum;
                const char *const SPEC   = DATA[ti].d_spec_p;
                const int         IND    = DATA[ti].d_indent;
                const int         SPL    = DATA[ti].d_spaces;
                const char *const FMT    = DATA[ti].d_fmt_p;
                const int         curLen = (int)strlen(SPEC);

                char buf1[SIZE], buf2[SIZE], buf3[SIZE];
                memcpy(buf1, CTRL_BUF1, SIZE); // Preset buf1 to Z1 values.
                memcpy(buf2, CTRL_BUF2, SIZE); // Preset buf2 to Z2 values.
                memcpy(buf3, CTRL_BUF3, SIZE); // Preset buf3 to Z2 values.

                Obj mX(&testAllocator);  const Obj& X = gg(&mX, SPEC);
                LOOP_ASSERT(ti, curLen == X.length()); // same lengths

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton objects of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen < curLen); // strictly increasing
                    oldLen = curLen;
                }

                if (verbose) { cout << "\t\tSpec = \"" << SPEC << "\", ";
                               P_(IND); P(SPL); }
                if (veryVerbose) cout << "EXPECTED FORMAT:" << endl<<FMT<<endl;
                ostrstream out1(buf1, SIZE);  X.print(out1, IND, SPL) << ends;
                ostrstream out2(buf2, SIZE);  X.print(out2, IND, SPL) << ends;
                if (veryVerbose) cout << "ACTUAL FORMAT:" << endl<<buf1<<endl;

                const int SZ = (int)strlen(FMT) + 1;
                const int REST = SIZE - SZ;
                LOOP_ASSERT(ti, SZ < SIZE);  // Check buffer is large enough.
                LOOP_ASSERT(ti, Z1 == buf1[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(ti, Z2 == buf2[SIZE - 1]);  // Check for overrun.
                LOOP3_ASSERT(ti, buf1, FMT, 0 == memcmp(buf1, FMT, SZ));
                LOOP_ASSERT(ti, 0 == memcmp(buf2, FMT, SZ));
                LOOP_ASSERT(ti, 0 == memcmp(buf1 + SZ, CTRL_BUF1 + SZ, REST));
                LOOP_ASSERT(ti, 0 == memcmp(buf2 + SZ, CTRL_BUF2 + SZ, REST));
                if (0 == IND && -1 == SPL) {
                    ostrstream out3(buf3, SIZE);  out3 << X << ends;
                    LOOP_ASSERT(ti, Z2 == buf3[SIZE - 1]);  // Check for
                                                            // overrun.
                    LOOP_ASSERT(ti, 0 == memcmp(buf3, FMT, SZ));
                    LOOP_ASSERT(ti, 0 == memcmp(buf3 + SZ, CTRL_BUF3 + SZ,
                                                REST));
                }
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING BASIC ACCESSORS:
        //   Having implemented an effective generation mechanism, we now
        //   would like to test thoroughly the basic accessor functions
        //     - length() const
        //     - numSet1() const
        //     - numSet0() const
        //     - isAnySet1() const
        //     - isAnySet0() const
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
        // Testing:
        //   int length() const;
        //   int numSet1() const;
        //   int numSet0() const;
        //   bool isAnySet1() const;
        //   bool isAnySet0() const;
        //   int operator[](int index) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Basic Accessors" << endl
                          << "=======================" << endl;

        if (verbose) cout << "\nTesting 'length' & 'operator[]'" << endl;
        {
            const int SZ = 128;
            static const struct {
                int         d_lineNum;          // source line number
                const char *d_spec_p;           // specification string
                int         d_length;           // expected length
                Element     d_elements[SZ];     // expected element values
            } DATA[] = {
                //line  spec            length  elements
                //----  --------------  ------  ------------------------
                { L_,   "",             0,      { 0 }                   },
                { L_,   "0",            1,      { 0 }                   },
                { L_,   "1",            1,      { 1 }                   },
                { L_,   "01",           2,      { 0, 1 }                },
                { L_,   "11",           2,      { 1, 1 }                },
                { L_,   "110",          3,      { 1, 1, 0 }             },
                { L_,   "101",          3,      { 1, 0, 1 }             },
                { L_,   "1001",         4,      { 1, 0, 0, 1 }          },
                { L_,   "0011",         4,      { 0, 0, 1, 1 }          },
                { L_,   "01100",        5,      { 0, 1, 1, 0, 0 }       },
                { L_,   "00110",        5,      { 0, 0, 1, 1, 0 }       },
                { L_,   "0110001",      7,      { 0, 1, 1, 0, 0, 0, 1 } },
                { L_,   "10100011",     8,      { 1, 0, 1, 0, 0, 0, 1,
                                                  1 }                   },
                { L_,   "110000110",    9,      { 1, 1, 0, 0, 0, 0, 1,
                                                  1, 0 }                },
                { L_,   "1100001100111001000101000110111",
                                       31,      { 1, 1, 0, 0, 0, 0, 1,
                                                  1, 0, 0, 1, 1, 1, 0,
                                                  0, 1, 0, 0, 0, 1, 0,
                                                  1, 0, 0, 0, 1, 1, 0,
                                                  1, 1, 1 }             },
                { L_,   "11000011001110010001010001101110",
                                       32,      { 1, 1, 0, 0, 0, 0, 1,
                                                  1, 0, 0, 1, 1, 1, 0,
                                                  0, 1, 0, 0, 0, 1, 0,
                                                  1, 0, 0, 0, 1, 1, 0,
                                                  1, 1, 1, 0 }          },
                { L_,   "110000110011100100010100011011100",
                                       33,      { 1, 1, 0, 0, 0, 0, 1,
                                                  1, 0, 0, 1, 1, 1, 0,
                                                  0, 1, 0, 0, 0, 1, 0,
                                                  1, 0, 0, 0, 1, 1, 0,
                                                  1, 1, 1, 0, 0 }       },
                { L_,
           "110000110011100100010100011011101100001100111001000101000110111",
                                       63,      { 1, 1, 0, 0, 0, 0, 1,
                                                  1, 0, 0, 1, 1, 1, 0,
                                                  0, 1, 0, 0, 0, 1, 0,
                                                  1, 0, 0, 0, 1, 1, 0,
                                                  1, 1, 1, 0, 1, 1, 0,
                                                  0, 0, 0, 1, 1, 0, 0,
                                                  1, 1, 1, 0, 0, 1, 0,
                                                  0, 0, 1, 0, 1, 0, 0,
                                                  0, 1, 1, 0, 1, 1, 1 } },
                { L_,
           "1100001100111001000101000110111011000011001110010001010001101110",
                                       64,      { 1, 1, 0, 0, 0, 0, 1,
                                                  1, 0, 0, 1, 1, 1, 0,
                                                  0, 1, 0, 0, 0, 1, 0,
                                                  1, 0, 0, 0, 1, 1, 0,
                                                  1, 1, 1, 0, 1, 1, 0,
                                                  0, 0, 0, 1, 1, 0, 0,
                                                  1, 1, 1, 0, 0, 1, 0,
                                                  0, 0, 1, 0, 1, 0, 0,
                                                  0, 1, 1, 0, 1, 1, 1,
                                                  0 }                   },
                { L_,
           "11000011001110010001010001101110110000110011100100010100011011101",
                                       65,      { 1, 1, 0, 0, 0, 0, 1,
                                                  1, 0, 0, 1, 1, 1, 0,
                                                  0, 1, 0, 0, 0, 1, 0,
                                                  1, 0, 0, 0, 1, 1, 0,
                                                  1, 1, 1, 0, 1, 1, 0,
                                                  0, 0, 0, 1, 1, 0, 0,
                                                  1, 1, 1, 0, 0, 1, 0,
                                                  0, 0, 1, 0, 1, 0, 0,
                                                  0, 1, 1, 0, 1, 1, 1,
                                                  0, 1 }                },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            Obj mY(&testAllocator);  // object with extended internal capacity
            const int EXTEND = 50; stretch(&mY, EXTEND); ASSERT(mY.length());
            if (veryVerbose) cout << "\tEXTEND = " << EXTEND << endl;

            int oldLen= -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int LENGTH       = DATA[ti].d_length;
                const Element *const e = DATA[ti].d_elements;
                const int curLen = LENGTH;

                Obj mX(&testAllocator);

                const Obj& X = gg(&mX, SPEC);   // canonical organization
                mY.removeAll();
                const Obj& Y = gg(&mY, SPEC);   // has extended capacity

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
                    LOOP2_ASSERT(LINE, i, e[i] == X[i]);
                    LOOP2_ASSERT(LINE, i, e[i] == Y[i]);
                }
                for (; i < SZ; ++i) {
                    LOOP2_ASSERT(LINE, i, 0 == e[i]);
                }
            }
        }

        if (verbose) cout << "\nTesting 'numSet?' and 'isAnySet?'" << endl;
        {
            static const struct {
                int         d_lineNum;          // source line number
                const char *d_spec_p;           // specification string
                int         d_numSet0;          // expected number of 0 bits
                int         d_numSet1;          // expected number of 1 bits
            } DATA[] = {
                //line  spec            num0  num1
                //----  --------------  ----  ----
                { L_,   "",               0,    0  },

                { L_,   "0",              1,    0  },
                { L_,   "1",              0,    1  },

                { L_,   "00",             2,    0  },
                { L_,   "01",             1,    1  },
                { L_,   "11",             0,    2  },

                { L_,   "000",            3,    0  },
                { L_,   "110",            1,    2  },
                { L_,   "101",            1,    2  },
                { L_,   "111",            0,    3  },

                { L_,   "0000",           4,    0  },
                { L_,   "0011",           2,    2  },
                { L_,   "1001",           2,    2  },
                { L_,   "1111",           0,    4  },

                { L_,   "00000",          5,    0  },
                { L_,   "01100",          3,    2  },
                { L_,   "00110",          3,    2  },
                { L_,   "11111",          0,    5  },

                { L_,   "0000000",        7,    0  },
                { L_,   "0110001",        4,    3  },
                { L_,   "1111111",        0,    7  },

                { L_,   "00000000",       8,    0  },
                { L_,   "10100011",       4,    4  },
                { L_,   "11111111",       0,    8  },

                { L_,   "000000000",      9,    0  },
                { L_,   "110000110",      5,    4  },
                { L_,   "111111111",      0,    9  },

                { L_,
           "0000000000000000000000000000000000000000000000000000000000000000",
                                         64,    0  },
                { L_,
           "1100001100111001000101000110111011000011001110010001010001101110",
                                         34,   30  },
                { L_,
           "1111111111111111111111111111111111111111111111111111111111111111",
                                          0,   64  },

                { L_,
           "00000000000000000000000000000000000000000000000000000000000000000",
                                         65,    0  },
                { L_,
           "11000011001110010001010001101110110000110011100100010100011011101",
                                         34,   31  },
                { L_,
           "11111111111111111111111111111111111111111111111111111111111111111",
                                          0,   65  },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            Obj mY(&testAllocator);  // object with extended internal capacity
            const int EXTEND = 50; stretch(&mY, EXTEND); ASSERT(mY.length());
            if (veryVerbose) cout << "\tEXTEND = " << EXTEND << endl;

            int oldLen= -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int NUMSET0      = DATA[ti].d_numSet0;
                const int NUMSET1      = DATA[ti].d_numSet1;
                const int curLen       = (int)strlen(SPEC);

                Obj mX(&testAllocator);

                const Obj& X = gg(&mX, SPEC);   // canonical organization
                mY.removeAll();
                const Obj& Y = gg(&mY, SPEC);   // has extended capacity

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

                LOOP_ASSERT(LINE, NUMSET0 == X.numSet0());
                LOOP_ASSERT(LINE, NUMSET0 == Y.numSet0());
                LOOP_ASSERT(LINE, NUMSET1 == X.numSet1());
                LOOP_ASSERT(LINE, NUMSET1 == Y.numSet1());
                LOOP_ASSERT(LINE, (NUMSET0 > 0) == X.isAnySet0());
                LOOP_ASSERT(LINE, (NUMSET0 > 0) == Y.isAnySet0());
                LOOP_ASSERT(LINE, (NUMSET1 > 0) == X.isAnySet1());
                LOOP_ASSERT(LINE, (NUMSET1 > 0) == Y.isAnySet1());
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
        // Testing:
        //   bdea_BitArray& gg(bdea_BitArray* object, const char *spec);
        //   int ggg(bdea_BitArray *object, const char *spec, int vF = 1);
        //   void stretch(Obj *object, int size);
        //   void stretchRemoveAll(Obj *object, int size);
        //   CONCERN: Is the internal memory organization behaving as intended?
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "Testing Primitive Generator Function 'gg'" << endl
                 << "=========================================" << endl;

        if (verbose) cout << "\nTesting generator on valid specs." << endl;
        {
            const int SZ = 10;
            static const struct {
                int         d_lineNum;          // source line number
                const char *d_spec_p;           // specification string
                int         d_length;           // expected length
                Element     d_elements[SZ];     // expected element values
            } DATA[] = {
                //line  spec            length  elements
                //----  --------------  ------  ------------------------
                { L_,   "",             0,      { 0 }                   },

                { L_,   "0",            1,      { 0 }                   },
                { L_,   "1",            1,      { 1 }                   },
                { L_,   "~",            0,      { 0 }                   },

                { L_,   "10",           2,      { 1, 0 }                },
                { L_,   "0~",           0,      { 0 }                   },
                { L_,   "~0",           1,      { 0 }                   },
                { L_,   "~~",           0,      { 0 }                   },

                { L_,   "011",          3,      { 0, 1, 1 }             },
                { L_,   "~11",          2,      { 1, 1 }                },
                { L_,   "0~1",          1,      { 1 }                   },
                { L_,   "01~",          0,      { 0 }                   },
                { L_,   "~~1",          1,      { 1 }                   },
                { L_,   "~1~",          0,      { 0 }                   },
                { L_,   "0~~",          0,      { 0 }                   },
                { L_,   "~~~",          0,      { 0 }                   },

                { L_,   "0110",         4,      { 0, 1, 1, 0 }          },
                { L_,   "~110",         3,      { 1, 1, 0 }             },
                { L_,   "0~10",         2,      { 1, 0 }                },
                { L_,   "01~0",         1,      { 0 }                   },
                { L_,   "011~",         0,      { 0 }                   },

                { L_,   "01100",        5,      { 0, 1, 1, 0, 0 }       },
                { L_,   "~1100",        4,      { 1, 1, 0, 0 }          },
                { L_,   "01~00",        2,      { 0, 0 }                },
                { L_,   "0110~",        0,      { 0 }                   },
                { L_,   "0~1~0",        1,      { 0 }                   },
                { L_,   "~1~0~",        0,      { 0 }                   },

                { L_,   "~110~~01100",  5,      { 0, 1, 1, 0, 0 }  },

                { L_,   "01100~1001~0", 1,      { 0 }                  },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int LENGTH       = DATA[ti].d_length;
                const Element *const e = DATA[ti].d_elements;
                const int curLen = (int)strlen(SPEC);

                Obj mX(&testAllocator);
                const Obj& X = gg(&mX, SPEC);   // original spec

                static const char *const MORE_SPEC = "~011000110001100~";
                char buf[100]; strcpy(buf, MORE_SPEC); strcat(buf, SPEC);

                Obj mY(&testAllocator);
                const Obj& Y = gg(&mY, buf);    // extended spec

                if (curLen != oldLen) {
                    if (verbose) cout << "\tof length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                if (veryVerbose) {
                    cout << "\t\t   Spec = \"" << SPEC << '"' << endl;
                    cout << "\t\tBigSpec = \"" << buf  << '"' << endl;
                    cout << "\t\t\t"; P(X);
                    cout << "\t\t\t"; P(Y);
                }

                LOOP_ASSERT(LINE, LENGTH == X.length());
                LOOP_ASSERT(LINE, LENGTH == Y.length());
                for (int i = 0; i < LENGTH; ++i) {
                    LOOP2_ASSERT(LINE, i, e[i] == X[i]);
                    LOOP2_ASSERT(LINE, i, e[i] == Y[i]);
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
                { L_,   "2",             0,     },

                { L_,   "00",           -1,     }, // control
                { L_,   "30",            0,     },
                { L_,   "03",            1,     },
                { L_,   ".~",            0,     },
                { L_,   "~!",            1,     },
                { L_,   "  ",            0,     },

                { L_,   "011",          -1,     }, // control
                { L_,   " 11",           0,     },
                { L_,   "0 1",           1,     },
                { L_,   "01 ",           2,     },
                { L_,   "?#:",           0,     },
                { L_,   "   ",           0,     },

                { L_,   "01100",        -1,     }, // control
                { L_,   "41100",         0,     },
                { L_,   "01500",         2,     },
                { L_,   "01106",         4,     },
                { L_,   "04170",         1,     },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int INDEX        = DATA[ti].d_index;
                const int curLen       = (int)strlen(SPEC);

                Obj mX(&testAllocator);

                if (curLen != oldLen) {
                    if (verbose) cout << "\tof length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                if (veryVerbose) cout <<
                    "\t\tSpec = \"" << SPEC << '"' << endl;

                int result = ggg(&mX, SPEC, veryVerbose);

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
                { L_,   "0",            0,      0,              0       },

                { L_,   "",             2,      0,              0       },
                { L_,   "0",            1,      0,              0       },
                { L_,   "01",           0,      0,              0       },

                { L_,   "",             3,      0,              0       },
                { L_,   "0",            2,      0,              0       },
                { L_,   "01",           1,      0,              0       },
                { L_,   "011",          0,      0,              0       },

                { L_,   "",             4,      0,              0       },
                { L_,   "0",            3,      0,              0       },
                { L_,   "01",           2,      0,              0       },
                { L_,   "011",          1,      0,              0       },
                { L_,   "0110",         0,      0,              0       },

                { L_,   "",             5,      0,              0       },
                { L_,   "0",            4,      0,              0       },
                { L_,   "01",           3,      0,              0       },
                { L_,   "011",          2,      0,              0       },
                { L_,   "0110",         1,      0,              0       },
                { L_,   "01100",        0,      0,              0       },

                { L_,   "010101010101010101",
                                        0,      0,              0       },
                { L_,   "010101010101010101",
                                       10,      0,              0       },
                { L_,   "010101010101010101",
                                       20,      0,              1       },

                { L_,   "0101010101010101010101010101010101010101",
                                        0,      1,              0       },
                { L_,   "0101010101010101010101010101010101010101",
                                       10,      1,              0       },
                { L_,   "0101010101010101010101010101010101010101",
                                       20,      1,              0       },
                { L_,   "0101010101010101010101010101010101010101",
                                       40,      1,              1       },
                { L_,   "0101010101010101010101010101010101010101",
                                       80,      1,              1       },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldDepth = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int size         = DATA[ti].d_size;
                const int firstResize  = DATA[ti].d_firstResize;
                const int secondResize = DATA[ti].d_secondResize;
                const int curLen       = (int)strlen(SPEC);
                const int curDepth     = curLen + size;

                Obj mX(&testAllocator);  const Obj& X = mX;
                Obj mY(&testAllocator);  const Obj& Y = mY;

                if (curDepth != oldDepth) {
                    if (verbose) cout << "\ton test vectors of depth "
                                      << curDepth << '.' << endl;
                    LOOP_ASSERT(LINE, oldDepth <= curDepth); // non-decreasing
                    oldDepth = curDepth;
                }

                if (veryVerbose) {
                    cout << "\t\t";  P_(SPEC); P(size);
                    P_(firstResize); P_(secondResize);
                    P_(curLen);      P(curDepth);
                }

                // Create identical objects using the gg function.
                {
                    int blocks1A = testAllocator.numBlocksTotal();
                    int bytes1A = testAllocator.numBytesInUse();

                    gg(&mX, SPEC);

                    int blocks2A = testAllocator.numBlocksTotal();
                    int bytes2A = testAllocator.numBytesInUse();

                    gg(&mY, SPEC);

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

                    stretch(&mX, size);

                    int blocks2B = testAllocator.numBlocksTotal();
                    int bytes2B = testAllocator.numBytesInUse();

                    stretchRemoveAll(&mY, size);

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
        //    1) The default Constructor
        //        1a) creates the correct initial value.
        //        1b) is exception neutral with respect to memory allocation.
        //        1c) has the internal memory management system hooked up
        //              properly so that *all* internally allocated memory
        //              draws from the same user-supplied allocator whenever
        //              one is specified.
        //    2) The destructor properly deallocates all allocated memory to
        //         its corresponding allocator from any attainable state.
        //    3) 'append'
        //        3a) produces the expected value.
        //        3b) increases capacity as needed.
        //        3c) maintains valid internal state.
        //        3d) treats argument as boolean
        //        3e) is exception neutral with respect to memory allocation.
        //    4) 'removeAll'
        //        4a) produces the expected value (empty).
        //        4b) properly destroys each contained element value.
        //        4c) maintains valid internal state.
        //        4d) does not allocate memory.
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
        //   To address concerns 3a - 3d, construct a series of independent
        //   objects, ordered by increasing length using the input values 0, 1,
        //   and 2.  In each test, allow the object to leave scope without
        //   further modification, so that the destructor asserts internal
        //   object invariants appropriately.  After the final append operation
        //   in each test, use the (untested) basic accessors to cross-check
        //   the value of the object and the 'bslma_TestAllocator' to confirm
        //   whether a resize has occurred.
        //
        //   To address concerns 4a-4c, construct a similar test, replacing
        //   'append' with 'removeAll'; this time, however, use the test
        //   allocator to record *numBlocksInUse* rather than *numBlocksTotal*.
        //
        //   To address concerns 2, 3e, 4d, create a small "area" test that
        //   exercises the construction and destruction of objects of various
        //   lengths and capacities in the presence of memory allocation
        //   exceptions.  Two separate tests will be performed.
        //
        //   Let S be the sequence of integers { 0 .. N - 1 } mod 3.
        //      (1) for each i in S, use the default constructor and 'append'
        //          to create an object of length i, confirm its value (using
        //           basic accessors), and let it leave scope.
        //      (2) for each (i, j) in S X S, use 'append' to create an
        //          object of length i, use 'removeAll' to erase its value
        //          and confirm (with 'length'), use append to set the object
        //          to a value of length j, verify the value, and allow the
        //          object to leave scope.
        //
        //   The first test acts as a "control" in that 'removeAll' is not
        //   called; if only the second test produces an error, we know that
        //   'removeAll' is to blame.  We will rely on 'bslma_TestAllocator'
        //   and purify to address concern 2, and on the object invariant
        //   assertions in the destructor to address concerns 3e and 4d.
        //
        // Testing:
        //   bdea_BitArray(bslma_Allocator *ba);
        //   ~bdea_BitArray();
        //   BOOTSTRAP: void append(bool value); // no aliasing
        //   void removeAll();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Primary Manipulators" << endl
                          << "============================" << endl;

        if (verbose) cout << "\nTesting default ctor (thoroughly)." << endl;

        if (verbose) cout << "\tWithout passing in an allocator." << endl;
        {
            const Obj X((bslma_Allocator *)0);
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
          BEGIN_BSLMA_EXCEPTION_TEST {
            if (veryVerbose) cout <<
                "\tTesting Exceptions In Default Ctor" << endl;
            const Obj X(&testAllocator);
            if (veryVerbose) { cout << "\t\t"; P(X); }
            ASSERT(0 == X.length());
          } END_BSLMA_EXCEPTION_TEST
        }

        if (verbose) cout << "\tIn place using a buffer allocator." << endl;
        {
            char memory[1024];
            bdema_BufferedSequentialAllocator a(memory, sizeof memory);
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
            mX.append(0);
            const int AA = testAllocator.numBlocksTotal();
            const int A  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\t AFTER: "; P_(A); P(X); }
            ASSERT(BB + 0 == AA); // ADJUST
            ASSERT(B  - 0 == A);  // ADJUST
            ASSERT(1 == X.length());
            ASSERT(0 == X[0]);
        }
        {
            if (verbose) cout << "\tOn an object of initial length 1." << endl;
            Obj mX(&testAllocator);  const Obj& X = mX;
            mX.append(0);

            const int BB = testAllocator.numBlocksTotal();
            const int B  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\tBEFORE: "; P_(B); P(X); }
            mX.append(1);
            const int AA = testAllocator.numBlocksTotal();
            const int A  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\t AFTER: "; P_(A); P(X); }
            ASSERT(2 == X.length());
            ASSERT(BB + 0 == AA); // ADJUST
            ASSERT(B  - 0 == A);  // ADJUST
            ASSERT(0 == X[0]);
            ASSERT(1 == X[1]);
        }
        {
            if (verbose) cout << "\tOn an object of initial length 2." << endl;
            Obj mX(&testAllocator);  const Obj& X = mX;
            mX.append(0); mX.append(1);

            const int BB = testAllocator.numBlocksTotal();
            const int B  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\tBEFORE: "; P_(B); P(X); }
            mX.append(2);
            const int AA = testAllocator.numBlocksTotal();
            const int A  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\t AFTER: "; P_(A); P(X); }
            ASSERT(BB + 0 == AA); // ADJUST
            ASSERT(B  - 0 == A);  // ADJUST
            ASSERT(3 == X.length());
            ASSERT(0 == X[0]);
            ASSERT(1 == X[1]);
            ASSERT(1 == X[2]);
        }
        {
            if (verbose) cout << "\tOn an object of initial length 3." << endl;
            Obj mX(&testAllocator);  const Obj& X = mX;
            mX.append(0); mX.append(1); mX.append(2);

            const int BB = testAllocator.numBlocksTotal();
            const int B  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\tBEFORE: "; P_(B); P(X); }
            mX.append(0);
            const int AA = testAllocator.numBlocksTotal();
            const int A  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\t AFTER: "; P_(A); P(X); }
            ASSERT(BB + 0 == AA); // ADJUST
            ASSERT(B  - 0 == A);  // ADJUST
            ASSERT(4 == X.length());
            ASSERT(0 == X[0]);
            ASSERT(1 == X[1]);
            ASSERT(1 == X[2]);
            ASSERT(0 == X[3]);
        }
        {
            if (verbose) cout << "\tOn an object of initial length 4." << endl;
            Obj mX(&testAllocator);  const Obj& X = mX;
            mX.append(0); mX.append(1); mX.append(2); mX.append(0);

            const int BB = testAllocator.numBlocksTotal();
            const int B  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\tBEFORE: "; P_(B); P(X); }
            mX.append(1);
            const int AA = testAllocator.numBlocksTotal();
            const int A  = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\t AFTER: "; P_(A); P(X); }
            ASSERT(BB + 0 == AA); // ADJUST
            ASSERT(B  - 0 == A);  // ADJUST
            ASSERT(5 == X.length());
            ASSERT(0 == X[0]);
            ASSERT(1 == X[1]);
            ASSERT(1 == X[2]);
            ASSERT(0 == X[3]);
            ASSERT(1 == X[4]);
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
            mX.append(0);
            ASSERT(1 == X.length());

            const int BB = testAllocator.numBlocksTotal();
            const int B = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\tBEFORE: "; P_(B); P(X); }
            mX.removeAll();
            const int AA = testAllocator.numBlocksTotal();
            const int A = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\t AFTER: "; P_(A); P(X); }
            ASSERT(BB == AA);    // always
            ASSERT(B - 0 == A);  // ADJUST
            ASSERT(0 == X.length());
        }
        {
            if (verbose) cout << "\tOn an object of initial length 2." << endl;
            Obj mX(&testAllocator);  const Obj& X = mX;
            mX.append(0); mX.append(1);
            ASSERT(2 == X.length());

            const int BB = testAllocator.numBlocksTotal();
            const int B = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\tBEFORE: "; P_(B); P(X); }
            mX.removeAll();
            const int AA = testAllocator.numBlocksTotal();
            const int A = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\t AFTER: "; P_(A); P(X); }
            ASSERT(BB == AA);    // always
            ASSERT(B - 0 == A);  // ADJUST
            ASSERT(0 == X.length());
        }
        {
            if (verbose) cout << "\tOn an object of initial length 3." << endl;
            Obj mX(&testAllocator);  const Obj& X = mX;
            mX.append(0); mX.append(1); mX.append(2);
            ASSERT(3 == X.length());

            const int BB = testAllocator.numBlocksTotal();
            const int B = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\tBEFORE: "; P_(B); P(X); }
            mX.removeAll();
            const int AA = testAllocator.numBlocksTotal();
            const int A = testAllocator.numBlocksInUse();
            if (veryVerbose) { cout << "\t\t AFTER: "; P_(A); P(X); }
            ASSERT(BB == AA);    // always
            ASSERT(B - 0 == A);  // ADJUST
            ASSERT(0 == X.length());
        }

        // --------------------------------------------------------------------

        if (verbose) cout <<
          "\nTesting the destructor and exception neutrality." << endl;

        if (verbose) cout << "\tWith 'append' only" << endl;
        {
            // For each lengths i up to some modest limit:
            //    1) create an object
            //    2) append { 0, 1, 2, 0, 1, 2, 0, ... }  up to length i
            //    3) verify initial length and contents
            //    4) allow the object to leave scope

            const int NUM_TRIALS = 100;
            for (int i = 0; i < NUM_TRIALS; ++i) { // i is the length
                if (verbose) cout <<
                    "\t\tOn an object of length " << i << '.' << endl;

              BEGIN_BSLMA_EXCEPTION_TEST {
                int k; // loop index

                Obj mX(&testAllocator);  const Obj& X = mX;             // 1.
                for (k = 0; k < i; ++k) {                               // 2.
                    mX.append(k % 3);
                }

                LOOP_ASSERT(i, i == X.length());                        // 3.
                for (k = 0; k < i; ++k) {
                    LOOP2_ASSERT(i, k, (k % 3 > 0) == X[k]);
                }

              } END_BSLMA_EXCEPTION_TEST                                // 4.
            }
        }

        if (verbose) cout << "\tWith 'append' and 'removeAll'" << endl;
        {
            // For each pair of lengths (i, j) up to some modest limit:
            //    1) create an object
            //    2) append 0 values up to a length of i
            //    3) verify initial length and contents
            //    4) removeAll contents from object
            //    5) verify length is 0
            //    6) append { 0, 1, 2, 0, 1, 2, 0, ... }  up to length j
            //    7) verify new length and contents
            //    8) allow the object to leave scope

            const int NUM_TRIALS = 100;
            for (int i = 0; i < NUM_TRIALS; ++i) { // i is first length
                if (verbose) cout <<
                    "\t\tOn an object of initial length " << i << '.' << endl;

                for (int j = 0; j < NUM_TRIALS; ++j) { // j is second length
                    if (veryVerbose) cout <<
                        "\t\t\tAnd with final length " << j << '.' << endl;

                  BEGIN_BSLMA_EXCEPTION_TEST {
                    int k; // loop index

                    Obj mX(&testAllocator);  const Obj& X = mX;         // 1.
                    for (k = 0; k < i; ++k) {                           // 2.
                        mX.append(0);
                    }

                    LOOP2_ASSERT(i, j, i == X.length());                // 3.
                    for (k = 0; k < i; ++k) {
                        LOOP3_ASSERT(i, j, k, 0 == X[k]);
                    }

                    mX.removeAll();                                     // 4.
                    LOOP2_ASSERT(i, j, 0 == X.length());                // 5.

                    for (k = 0; k < j; ++k) {                           // 6.
                        mX.append(k % 3);
                    }

                    LOOP2_ASSERT(i, j, j == X.length());                // 7.
                    for (k = 0; k < j; ++k) {
                        LOOP3_ASSERT(i, j, k, (k % 3 > 0) == X[k]);
                    }
                  } END_BSLMA_EXCEPTION_TEST                            // 8.
                }
            }
        }

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
        //   Exercise these objects using primary manipulators, basic
        //   accessors, equality operators, and the assignment operator.
        //   Invoke the primary manipulator [1&5], copy constructor [2&8], and
        //   assignment operator [9&10] in situations where the internal data
        //   (i) does *not* and (ii) *does* have to resize.  Try aliasing with
        //   assignment for a non-empty object [11] and allow the result to
        //   leave scope, enabling the destructor to assert internal object
        //   invariants.  Display object values frequently in verbose mode:
        //
        // 1) Create an object x1 (default ctor).       { x1: }
        // 2) Create a second object x2 (copy from x1). { x1: x2: }
        // 3) Append an element value 0 to x1).         { x1:0 x2: }
        // 4) Append the same element value 0 to x2).   { x1:0 x2:0 }
        // 5) Append another element value 1 to x2).    { x1:0 x2:01 }
        // 6) Remove all elements from x1.              { x1: x2:01 }
        // 7) Create a third object x3 (default ctor).  { x1: x2:01 x3: }
        // 8) Create a fourth object x4 (copy of x2).   { x1: x2:01 x3: x4:01 }
        // 9) Assign x2 = x1 (non-empty becomes empty). { x1: x2: x3: x4:01 }
        // 10) Assign x3 = x4 (empty becomes non-empty).{ x1: x2: x3:01 x4:01 }
        // 11) Assign x4 = x4 (aliasing).               { x1: x2: x3:01 x4:01 }
        //
        // Testing:
        //   This "test" *exercises* basic functionality, but *tests* nothing.

        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 1) Create an object x1 (default ctor)."
                             "\t\t\t{ x1: }" << endl;
        Obj mX1(&testAllocator);  const Obj& X1 = mX1;
        if (verbose) { cout << '\t';  P(X1); }

        if (verbose) cout <<
            "\ta) Check initial state of x1." << endl;
        ASSERT(0 == X1.length());

        if (verbose) cout <<
            "\tb) Try equality operators: x1 <op> x1." << endl;
        ASSERT((X1 == X1) == 1);          ASSERT((X1 != X1) == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 2) Create a second object x2 (copy from x1)."
                             "\t\t{ x1: x2: }" << endl;
        Obj mX2(X1, &testAllocator);  const Obj& X2 = mX2;
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout <<
            "\ta) Check the initial state of x2." << endl;
        ASSERT(0 == X2.length());

        if (verbose) cout <<
            "\tb) Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT((X2 == X1) == 1);          ASSERT((X2 != X1) == 0);
        ASSERT((X2 == X2) == 1);          ASSERT((X2 != X2) == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 3) Append an element value 0 to x1)."
                             "\t\t\t{ x1:0 x2: }" << endl;
        mX1.append(0);
        if (verbose) { cout << '\t';  P(X1); }

        if (verbose) cout <<
            "\ta) Check new state of x1." << endl;
        ASSERT(1 == X1.length());
        ASSERT(0 == X1[0]);

        if (verbose) cout <<
            "\tb) Try equality operators: x1 <op> x1, x2." << endl;
        ASSERT((X1 == X1) == 1);          ASSERT((X1 != X1) == 0);
        ASSERT((X1 == X2) == 0);          ASSERT((X1 != X2) == 1);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 4) Append the same element value 0 to x2)."
                             "\t\t{ x1:0 x2:0 }" << endl;
        mX2.append(0);
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout <<
            "\ta) Check new state of x2." << endl;
        ASSERT(1 == X2.length());
        ASSERT(0 == X2[0]);

        if (verbose) cout <<
            "\tb) Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT((X2 == X1) == 1);          ASSERT((X2 != X1) == 0);
        ASSERT((X2 == X2) == 1);          ASSERT((X2 != X2) == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 5) Append another element value 1 to x2)."
                             "\t\t{ x1:0 x2:01 }" << endl;
        mX2.append(1);
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout <<
            "\ta) Check new state of x2." << endl;
        ASSERT(2 == X2.length());
        ASSERT(0 == X2[0]);
        ASSERT(1 == X2[1]);

        if (verbose) cout <<
            "\tb) Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT((X2 == X1) == 0);          ASSERT((X2 != X1) == 1);
        ASSERT((X2 == X2) == 1);          ASSERT((X2 != X2) == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 6) Remove all elements from x1."
                             "\t\t\t{ x1: x2:01 }" << endl;
        mX1.removeAll();
        if (verbose) { cout << '\t';  P(X1); }

        if (verbose) cout <<
            "\ta) Check new state of x1." << endl;
        ASSERT(0 == X1.length());

        if (verbose) cout <<
            "\tb) Try equality operators: x1 <op> x1, x2." << endl;
        ASSERT((X1 == X1) == 1);          ASSERT((X1 != X1) == 0);
        ASSERT((X1 == X2) == 0);          ASSERT((X1 != X2) == 1);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 7) Create a third object x3 (default ctor)."
                             "\t\t{ x1: x2:01 x3: }" << endl;

        Obj mX3(&testAllocator);  const Obj& X3 = mX3;
        if (verbose) { cout << '\t';  P(X3); }

        if (verbose) cout <<
            "\ta) Check new state of x3." << endl;
        ASSERT(0 == X3.length());

        if (verbose) cout <<
            "\tb) Try equality operators: x3 <op> x1, x2, x3." << endl;
        ASSERT((X3 == X1) == 1);          ASSERT((X3 != X1) == 0);
        ASSERT((X3 == X2) == 0);          ASSERT((X3 != X2) == 1);
        ASSERT((X3 == X3) == 1);          ASSERT((X3 != X3) == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 8) Create a fourth object x4 (copy of x2)."
                             "\t\t{ x1: x2:01 x3: x4:01 }" << endl;

        Obj mX4(X2, &testAllocator);  const Obj& X4 = mX4;
        if (verbose) { cout << '\t';  P(X4); }

        if (verbose) cout <<
            "\ta) Check new state of x4." << endl;

        ASSERT(2 == X4.length());
        ASSERT(0 == X4[0]);
        ASSERT(1 == X4[1]);

        if (verbose) cout <<
            "\tb) Try equality operators: x4 <op> x1, x2, x3, x4." << endl;
        ASSERT((X4 == X1) == 0);          ASSERT((X4 != X1) == 1);
        ASSERT((X4 == X2) == 1);          ASSERT((X4 != X2) == 0);
        ASSERT((X4 == X3) == 0);          ASSERT((X4 != X3) == 1);
        ASSERT((X4 == X4) == 1);          ASSERT((X4 != X4) == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 9) Assign x2 = x1 (non-empty becomes empty)."
                             "\t\t{ x1: x2: x3: x4:01 }" << endl;

        mX2 = X1;
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout <<
            "\ta) Check new state of x2." << endl;
        ASSERT(0 == X2.length());

        if (verbose) cout <<
            "\tb) Try equality operators: x2 <op> x1, x2, x3, x4." << endl;
        ASSERT((X2 == X1) == 1);          ASSERT((X2 != X1) == 0);
        ASSERT((X2 == X2) == 1);          ASSERT((X2 != X2) == 0);
        ASSERT((X2 == X3) == 1);          ASSERT((X2 != X3) == 0);
        ASSERT((X2 == X4) == 0);          ASSERT((X2 != X4) == 1);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n10) Assign x3 = x4 (empty becomes non-empty)."
                             "\t\t{ x1: x2: x3:01 x4:01 }" << endl;

        mX3 = X4;
        if (verbose) { cout << '\t';  P(X3); }

        if (verbose) cout <<
            "\ta) Check new state of x3." << endl;
        ASSERT(2 == X3.length());
        ASSERT(0 == X3[0]);
        ASSERT(1 == X3[1]);

        if (verbose) cout <<
            "\tb) Try equality operators: x3 <op> x1, x2, x3, x4." << endl;
        ASSERT((X3 == X1) == 0);          ASSERT((X3 != X1) == 1);
        ASSERT((X3 == X2) == 0);          ASSERT((X3 != X2) == 1);
        ASSERT((X3 == X3) == 1);          ASSERT((X3 != X3) == 0);
        ASSERT((X3 == X4) == 1);          ASSERT((X3 != X4) == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n11) Assign x4 = x4 (aliasing)."
                             "\t\t\t\t{ x1: x2: x3:01 x4:01 }" << endl;

        mX4 = X4;
        if (verbose) { cout << '\t';  P(X4); }

        if (verbose) cout <<
            "\ta) Check new state of x4." << endl;
        ASSERT(2 == X4.length());
        ASSERT(0 == X4[0]);
        ASSERT(1 == X4[1]);

        if (verbose) cout <<
            "\tb) Try equality operators: x4 <op> x1, x2, x3, x4." << endl;
        ASSERT((X4 == X1) == 0);          ASSERT((X4 != X1) == 1);
        ASSERT((X4 == X2) == 0);          ASSERT((X4 != X2) == 1);
        ASSERT((X4 == X3) == 1);          ASSERT((X4 != X3) == 0);
        ASSERT((X4 == X4) == 1);          ASSERT((X4 != X4) == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout <<
                "\n 1) Create an object X5 (default ctor, default allocator)."
                                                       "\t\t\t{ X5: }" << endl;
        {
            bslma_TestAllocator ga(veryVeryVerbose);
            bslma_DefaultAllocatorGuard dag(&ga);
            Obj mX5;  const Obj& X5 = mX5;
            if (verbose) { cout << '\t';  P(X5); }

            if (verbose) cout << "\ta) Check initial state of X5." << endl;
            ASSERT(0 == X5.length());

            if (verbose) cout <<
                            "\tb) Try equality operators: X5 <op> X5." << endl;
            ASSERT((X5 == X5) == 1);          ASSERT((X5 != X5) == 0);

            mX5.append(0);
            if (verbose) { cout << '\t';  P(X5); }

            ASSERT(ga.numBytesInUse() > 0);
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout <<
                "\n 1) Create an object X6 with ctor(int, bool, ...\n"
                                                       "\t\t\t{ X6: }" << endl;
        {
            Obj mX6(8, false, &testAllocator);  const Obj& X6 = mX6;
            if (verbose) { cout << '\t';  P(X6); }

            if (verbose) cout << "\ta) Check initial state of X6." << endl;
            ASSERT(8 == X6.length());

            if (verbose) cout <<
                            "\tb) Try equality operators: X6 <op> X6." << endl;
            ASSERT((X6 == X6) == 1);        ASSERT((X6 != X6) == 0);

            ASSERT(X6.length() == 8);
            ASSERT(false == X6[0]);         ASSERT(false == X6[4]);
            ASSERT(false == X6[5]);         ASSERT(false == X6[7]);
        }
        {
            bslma_TestAllocator ga(veryVeryVerbose);
            bslma_DefaultAllocatorGuard dag(&ga);
            // Obj mX6(8, 0);  const Obj& X6 = mX6; <-- note: causes error
            Obj mX6(8, false);  const Obj& X6 = mX6;
            if (verbose) { cout << '\t';  P(X6); }

            ASSERT(ga.numBytesInUse() > 0);

            if (verbose) cout << "\ta) Check initial state of X6." << endl;
            ASSERT(8 == X6.length());

            if (verbose) cout <<
                            "\tb) Try equality operators: X6 <op> X6." << endl;
            ASSERT((X6 == X6) == 1);        ASSERT((X6 != X6) == 0);

            ASSERT(X6.length() == 8);
            ASSERT(false == X6[0]);         ASSERT(false == X6[4]);
            ASSERT(false == X6[5]);         ASSERT(false == X6[7]);
        }
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
