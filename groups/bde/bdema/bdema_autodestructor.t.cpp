// bdema_autodestructor.t.cpp  -*-C++-*-

#include <bdema_autodestructor.h>

#include <bsl_new.h>
#include <bsl_iostream.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // memcpy(), memmove()

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// We are testing a proctor object to ensure that it destroys the proper
// sequence of contiguous objects (in unspecified order).  We achieve this goal
// by creating objects of a user-defined type that are each initialized with
// the address of a unique counter.  As each object is destroyed, its
// destructor increments the counter held by the object, indicating the number
// of times the object's destructor is called.  We create proctors to manage
// varying sequences (differing in origin and length) of such user-defined-type
// objects in an array.  After each proctor is destroyed, we verify that the
// corresponding counters of the objects managed by the proctor are modified.
//-----------------------------------------------------------------------------
// [3] bdema_AutoDestructor<TYPE>(origin, length);
// [3] ~bdema_AutoDestructor<TYPE>();
// [3] void release();
// [3] bdema_AutoDestructor<TYPE>& operator++();
// [3] bdema_AutoDestructor<TYPE>& operator--();
//-----------------------------------------------------------------------------
// [1] my_Class(counter);
// [1] ~my_Class();
// [1] void setCounter(counter);
// [2] int areEqual(a1, a2, len);
// [4] USAGE EXAMPLE
//=============================================================================
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define PA(X, L) cout << #X " = "; printArray(X, L); cout << endl;
                                              // Print array 'X' of length 'L'
#define PA_(X, L) cout << #X " = "; printArray(X, L); cout << ", " << flush;
                                              // PA(X, L) without '\n'
#define L_ __LINE__                           // current Line number

//=============================================================================
//                        STATIC FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

static int areEqual(const int *array1, const int *array2, int numElements)
    // Compare the specified 'numElements' of the specified integer arrays
    // 'array1' and 'array2'.  Return 1 if the contents are equal, and 0
    // otherwise.  The behavior is undefined unless 0 <= numElements.
{
    ASSERT(array1);
    ASSERT(array2);
    ASSERT(0 <= numElements);
    for (int i = 0; i < numElements; ++i) {
        if (array1[i] != array2[i]) {
            return 0;
        }
    }
    return 1;
}

static void printArray(const int *array, int numElements)
    // Output the specified 'numElements' of the specified 'array' in a single-
    // line format.  The behavior is undefined unless 0 <= numElements.
{
    ASSERT(array);
    ASSERT(0 <= numElements);
    cout << "[ ";
    for (int i = 0; i < numElements; ++i) {
        cout << array[i] << ' ';
    }
    cout << ']';
}

//=============================================================================
//                          HELPER CLASS FOR TESTING
//-----------------------------------------------------------------------------

class my_Class {
    // This object indicates that its destructor is called by incrementing the
    // counter it *holds* (provided at construction) in the destructor.
  private:
    int *d_counter_p; // Counter to be incremented at destruction

  public:
    // CREATORS
    my_Class(int *counter = 0) : d_counter_p(counter) {}
        // Create this object and optionally specify the address of the
        // 'counter' to be held.

    ~my_Class() { if (d_counter_p) ++*d_counter_p; }
        // Destroy this object.  Also increment this object's counter if it is
        // not 'null'.

    // MANIPULATORS
    void setCounter(int *counter) { d_counter_p = counter; }
        // Sets this object's counter to refer to the specified 'counter'.
};

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

// my_string.h

class my_String {
    char *d_string_p;
    int   d_length;
    int   d_size;

  public:
    my_String(const char *string);
    my_String(const my_String& original);
    ~my_String();
    // ...

    inline int length() const { return d_length; }
    inline operator const char *() const { return d_string_p; }
    // ...
};

// FREE OPERATORS
inline bool operator==(const my_String& lhs, const char *rhs)
{ return strcmp(lhs, rhs) ==  0; }

// ...

// my_string.cpp

my_String::my_String(const char *string)
: d_length(strlen(string))
{
    ASSERT(string);
    d_size = d_length + 1;
    d_string_p = (char *) operator new(d_size);
    memcpy(d_string_p, string, d_size);
}

my_String::my_String(const my_String& original)
: d_length(original.d_length)
, d_size(original.d_length + 1)
{
    d_string_p = (char *) operator new(d_size);
    memcpy(d_string_p, original.d_string_p, d_size);
}

my_String::~my_String()
{
    ASSERT(d_string_p);
    delete d_string_p;
}

// ...

// my_strarray.h

class my_String;

class my_StrArray {
    my_String   *d_array_p;   // dynamically allocated array
    int          d_size;      // physical capacity of this array
    int          d_length;    // logical length of this array

  private:
    void increaseSize();

  public:
    my_StrArray();
    ~my_StrArray();
    // ...

    void append(const char *item);
    void insert(int dstIndex, const char *item);
    inline const my_String& operator[](int ind) const { return d_array_p[ind];}
    inline int length() const { return d_length; }
    // ...
};

// FREE OPERATORS
ostream& operator<<(ostream& stream, const my_StrArray& array);

inline
void my_StrArray::append(const char *item)
{
    if (d_length >= d_size) {
        this->increaseSize();
    }
    new(&d_array_p[d_length++]) my_String(item);
}

// ...

// my_strarray.cpp

enum {
    INITIAL_SIZE = 1, // initial physical capacity (number of elements)
    GROW_FACTOR = 2   // multiplicative factor by which to grow 'd_size'
};

inline static
int nextSize(int size)
    // Return the specified 'size' multiplied by 'GROW_FACTOR'.
{
    return size * GROW_FACTOR;
}

inline static
void reallocate(my_String **array, int *size,
                int newSize, int length)
    // Reallocate memory in the specified 'array' and update the
    // specified size to the specified 'newSize'.  The specified 'length'
    // number of leading elements are preserved.  If 'new' should throw
    // an exception, this function has no effect.  The behavior is
    // undefined unless 1 <= newSize, 0 <= length, and newSize <= length.
{
    ASSERT(array);
    ASSERT(*array);             // this is not 'allocate'
    ASSERT(size);
    ASSERT(1 <= newSize);
    ASSERT(0 <= length);
    ASSERT(length <= *size);    // sanity check
    ASSERT(length <= newSize);  // ensure class invarient

    my_String *tmp = *array;

    *array = (my_String *) operator new(newSize * sizeof **array);
    // COMMIT
    memcpy(*array, tmp, length * sizeof **array);
    *size = newSize;
    operator delete((void *) tmp);
}

void my_StrArray::increaseSize()
{
    reallocate(&d_array_p, &d_size, nextSize(d_size), d_length);
}

my_StrArray::my_StrArray()
: d_size(INITIAL_SIZE)
, d_length(0)
{
    d_array_p = (my_String *) operator new(d_size * sizeof *d_array_p);
}

my_StrArray::~my_StrArray()
{
    ASSERT(1 <= d_size);
    ASSERT(0 <= d_length);
    ASSERT(d_length <= d_size);

    for (int i = 0; i < d_length; ++i) {
        d_array_p[i].~my_String();
    }
    operator delete((void *) d_array_p);
}

// Assume no aliasing.
void my_StrArray::insert(int dstIndex, const char *item)
{
    ASSERT(0 <= dstIndex);  ASSERT(dstIndex <= d_length);

    if (d_length >= d_size) {
        increaseSize();
    }
    ASSERT(d_size > d_length);

    my_String *here = d_array_p + dstIndex;
    memmove(here + 1, here, (d_length - dstIndex) * sizeof *d_array_p);

    int origLen = d_length;
    d_length = dstIndex;
    bdema_AutoDestructor<my_String> autoDtor(&d_array_p[dstIndex + 1],
                                             origLen - dstIndex);
    new(&d_array_p[dstIndex]) my_String(item);
    autoDtor.release();

    d_length = origLen + 1;
}

ostream& operator<<(ostream& stream, const my_StrArray& array)
{
    stream << "[ ";
    for (int i = 0; i < array.length(); ++i) {
        stream << '"' << array[i] << "\" ";
    }
    return stream << ']' << flush;
}

// ...

// my_autodeallocator.h

class my_AutoDeallocator {
  private:
    void *d_memory;

  private:
    my_AutoDeallocator(const my_AutoDeallocator&);           // not impl.
    my_AutoDeallocator& operator=(const my_AutoDeallocator&);// not impl.

  public:
    // CREATORS
    my_AutoDeallocator(void *memory) : d_memory(memory) {}
    ~my_AutoDeallocator() { if (d_memory) operator delete(d_memory); }
    void release() { d_memory = 0; };
};

// my_array.h

template <class TYPE>
class my_Array {
    TYPE *d_array_p; // dynamically allocated array
    int   d_size;    // physical capacity of this array
    int   d_length;  // logical length of this array

  private:

    enum {
        INITIAL_SIZE = 1, // initial physical capacity
        GROW_FACTOR = 2   // multiplicative factor by which to grow
                          // 'd_size'
    };

    static int nextSize(int sz) { return sz * GROW_FACTOR; }
    static void reallocate(TYPE **array, int *size, int newSize, int length);
    void increaseSize();

  public:
    // CREATORS
    my_Array();
    ~my_Array();
    // ...

    void append(const TYPE& item);
    void insert(int dstIndex, const TYPE& item);
    void remove(int index);
    inline int length() const { return d_length; }
    inline const TYPE& operator[](int index) const { return d_array_p[index]; }
};

// FREE OPERATORS
template <class TYPE> inline
ostream& operator<<(ostream& stream, const my_Array<TYPE>& array);

// ...

template <class TYPE> inline
void my_Array<TYPE>::reallocate(TYPE **array, int *size,
                                int newSize, int length)
    // Reallocate memory in the specified 'array' and update the
    // specified size to the specified 'newSize'.  The specified 'length'
    // number of leading elements are preserved.  If 'new' should throw
    // an exception, this function has no effect.  The behavior is
    // undefined unless 1 <= newSize, 0 <= length, and newSize <= length.
    // Note that an "auto deallocator" is needed here to ensure that
    // memory allocated for the new array is deallocated when an exception
    // occurs.
{
    ASSERT(0 < size);
    ASSERT(1 <= newSize);
    ASSERT(0 <= length);
    ASSERT(length <= *size);    // sanity check
    ASSERT(length <= newSize);  // ensure class invarient

    TYPE *newArray = (TYPE *) operator new(newSize * sizeof **array);

    // 'autoDealloc' and 'autoDtor' are destroyed in reverse order
    my_AutoDeallocator autoDealloc(newArray);
    bdema_AutoDestructor<TYPE> autoDtor(newArray, 0);
    int i;
    for (i = 0; i < length; ++i, ++autoDtor) {
        new(&newArray[i]) TYPE((*array)[i]);
    }
    autoDtor.release();
    autoDealloc.release();

    for (i = 0; i < length; ++i) {
        (*array)[i].~TYPE();
    }
    operator delete((void *) *array);
    *array = newArray;
    *size = newSize;
}

template <class TYPE> inline
void my_Array<TYPE>::increaseSize()
{
    my_Array<TYPE>::reallocate(&d_array_p, &d_size,
                               nextSize(d_size), d_length);
}

// CREATORS
template <class TYPE> inline
my_Array<TYPE>::my_Array()
: d_size(INITIAL_SIZE)
, d_length(0)
{
    d_array_p = (TYPE *) operator new(d_size * sizeof *d_array_p);
}

template <class TYPE> inline
my_Array<TYPE>::~my_Array()
{
    for (int i = 0; i < d_length; ++i) {
        d_array_p[i].~TYPE();
    }
    operator delete((void *) d_array_p); // delete all allocated memory
}

template <class TYPE> inline
void my_Array<TYPE>::append(const TYPE& item)
{
    if (d_length >= d_size) {
        this->increaseSize();
    }
    new(&d_array_p[d_length++]) TYPE(item);
}

// Assume no aliasing.
template <class TYPE> inline
void my_Array<TYPE>::insert(int dstIndex, const TYPE& item)
{
    if (d_length >= d_size) {
        this->increaseSize();
    }

    bdema_AutoDestructor<TYPE> autoDtor(&d_array_p[d_length + 1], 0);
    int origLen = d_length;
    for (int i = d_length - 1; i >= dstIndex; --i,--autoDtor,--d_length) {
        new(&d_array_p[i + 1]) TYPE(d_array_p[i]); // copy to new index
        d_array_p[i].~TYPE();                      // destroy original
    }

    new(&d_array_p[dstIndex]) TYPE(item);
    autoDtor.release();
    d_length = origLen + 1;
}

template <class TYPE> inline
void my_Array<TYPE>::remove(int dstIndex)
{
    d_array_p[dstIndex].~TYPE();
    bdema_AutoDestructor<TYPE> autoDtor(&d_array_p[d_length],
                                     dstIndex + 1 - d_length);
    int origLen = d_length;
    d_length = dstIndex;
    for (int i = dstIndex + 1; i < origLen; ++i, ++autoDtor, ++d_length) {
        new(&d_array_p[i - 1]) TYPE(d_array_p[i]); // copy to new index
        d_array_p[i].~TYPE();                      // destroy original
    }

    autoDtor.release();
    d_length = origLen - 1;
}

template <class TYPE> inline
ostream& operator<<(ostream& stream, const my_Array<TYPE>& array)
{
    stream << "[ ";
    for (int i = 0; i < array.length(); ++i) {
        stream << array[i] << " ";
    }
    return stream << ']' << flush;
}

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 4: {
        // --------------------------------------------------------------------
        // USAGE TEST
        //
        // Testing:
        //   USAGE TEST - Make sure main usage example compiles and works.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE TEST" << endl
                                  << "==========" << endl;

        if (verbose) cout << "Testing 'my_StrArray::insert'." << endl;
        {
            const char *DATA[] = { "A", "B", "C", "D", "E" };
            const int NUM_ELEM = sizeof DATA / sizeof *DATA;

            my_StrArray mX;    const my_StrArray& X = mX;
            int i;
            for (i = 0; i < NUM_ELEM; ++i) {
                my_String s(DATA[i]);
                mX.append(s);
            }
            if (veryVerbose) { cout << '\t'; P(X); }

            if (verbose)
                cout << "\tInsert string \"F\" at index position 2." << endl;

            mX.insert(2, "F");
            if (veryVerbose) { cout << '\t'; P(X); }

            const char *EXP[] = { "A", "B", "F", "C", "D", "E" };
            const int EXP_ELEM = sizeof EXP / sizeof *EXP;
            ASSERT(EXP_ELEM == X.length());
            for (i = 0; i < EXP_ELEM; ++i) {
                LOOP_ASSERT(i, X[i] == EXP[i]);
            }
        }

        if (verbose)
            cout << "Testing 'my_Array::insert' and 'my_Array::remove'."
                 << endl;
        {
            const char *DATA[] = { "A", "B", "C", "D", "E" };
            const int NUM_ELEM = sizeof DATA / sizeof *DATA;

            my_Array<my_String> mX;    const my_Array<my_String>& X = mX;
            int i;
            for (i = 0; i < NUM_ELEM; ++i) {
                my_String s(DATA[i]);
                mX.append(s);
            }
            if (veryVerbose) { cout << '\t'; P(X); }

            if (verbose)
                cout << "\tRemove element at index position 2." << endl;

            mX.remove(2);
            if (veryVerbose) { cout << '\t'; P(X); }

            const char *EXP[] = { "A", "B", "D", "E" };
            const int EXP_ELEM = sizeof EXP / sizeof *EXP;
            ASSERT(EXP_ELEM == X.length());
            for (i = 0; i < X.length(); ++i) {
                LOOP_ASSERT(i, X[i] == EXP[i]);
            }

            if (verbose)
                cout << "\tInsert string \"C\" at index position 2." << endl;

            my_String item("C");
            mX.insert(2, item);
            if (veryVerbose) { cout << '\t'; P(X) }

            ASSERT(NUM_ELEM == X.length());
            for (i = 0; i < X.length(); ++i) {
                LOOP_ASSERT(i, X[i] == DATA[i]);
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // CTOR/OPERATORS TEST
        //   Iterate over a set of table-generated test vectors and perform
        //   independent tests.  For each test, create an array of 'my_Class'
        //   objects and a corresponding array of counters.  Initialize each
        //   element in the array of 'my_Class' objects with the element in the
        //   array of counters at the respective index position.  Create a
        //   'bdema_AutoDestructor' proctor initialized with 'd_origin' and
        //   'd_proctorLength' as specified in the test vector to manage a
        //   sequence of 'my_Class' objects, and increment/decrement the
        //   proctor according to 'd_incDec' in the test vector.  Destroy the
        //   the proctor and verify that the array of counters contains the
        //   expected contents.
        //
        // Testing:
        //   bdema_AutoDestructor<TYPE>(origin, length);
        //   ~bdema_AutoDestructor<TYPE>();
        //   void release();
        //   bdema_AutoDestructor<TYPE>& operator++();
        //   bdema_AutoDestructor<TYPE>& operator--();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "CTOR/OPERATORS TEST" << endl
                                  << "===================" << endl;

        const int NUM_ELEM = 5;

        struct {
            int d_line;                  // line number
            int d_origin;                // origin of proctor
            int d_proctorLength;         // initial length of proctor
            int d_incDec;                // increment/decrement amount
            int d_expCounters[NUM_ELEM]; // expected array of counters
        } DATA[] = {
    // line     origin    proctor      inc/      expected
    // no.      index     length       dec       counters
    // ----     ------    -------      ----      -----------------
     { L_,        0,         0,         0,       { 0, 0, 0, 0, 0 } },
     { L_,        0,         0,         1,       { 1, 0, 0, 0, 0 } },
     { L_,        0,         0,         3,       { 1, 1, 1, 0, 0 } },
     { L_,        0,         0,         5,       { 1, 1, 1, 1, 1 } },

     { L_,        0,         1,         0,       { 1, 0, 0, 0, 0 } },
     { L_,        0,         1,         1,       { 1, 1, 0, 0, 0 } },
     { L_,        0,         1,         3,       { 1, 1, 1, 1, 0 } },
     { L_,        0,         1,         4,       { 1, 1, 1, 1, 1 } },
     { L_,        0,         1,        -1,       { 0, 0, 0, 0, 0 } },

     { L_,        0,         5,         0,       { 1, 1, 1, 1, 1 } },
     { L_,        0,         5,        -1,       { 1, 1, 1, 1, 0 } },
     { L_,        0,         5,        -3,       { 1, 1, 0, 0, 0 } },
     { L_,        0,         5,        -5,       { 0, 0, 0, 0, 0 } },

     { L_,        1,         0,         0,       { 0, 0, 0, 0, 0 } },
     { L_,        1,         0,         1,       { 0, 1, 0, 0, 0 } },
     { L_,        1,         0,         3,       { 0, 1, 1, 1, 0 } },
     { L_,        1,         0,         4,       { 0, 1, 1, 1, 1 } },
     { L_,        1,         0,        -1,       { 1, 0, 0, 0, 0 } },

     { L_,        1,         1,         0,       { 0, 1, 0, 0, 0 } },
     { L_,        1,         1,         1,       { 0, 1, 1, 0, 0 } },
     { L_,        1,         1,         3,       { 0, 1, 1, 1, 1 } },
     { L_,        1,         1,        -1,       { 0, 0, 0, 0, 0 } },
     { L_,        1,         1,        -2,       { 1, 0, 0, 0, 0 } },

     { L_,        1,         4,         0,       { 0, 1, 1, 1, 1 } },
     { L_,        1,         4,        -1,       { 0, 1, 1, 1, 0 } },
     { L_,        1,         4,        -4,       { 0, 0, 0, 0, 0 } },
     { L_,        1,         4,        -5,       { 1, 0, 0, 0, 0 } },

     { L_,        5,         0,         0,       { 0, 0, 0, 0, 0 } },
     { L_,        5,         0,        -1,       { 0, 0, 0, 0, 1 } },
     { L_,        5,         0,        -3,       { 0, 0, 1, 1, 1 } },
     { L_,        5,         0,        -5,       { 1, 1, 1, 1, 1 } },

     { L_,        5,        -1,         0,       { 0, 0, 0, 0, 1 } },
     { L_,        5,        -1,        -1,       { 0, 0, 0, 1, 1 } },
     { L_,        5,        -1,        -4,       { 1, 1, 1, 1, 1 } },
     { L_,        5,        -1,         1,       { 0, 0, 0, 0, 0 } },

     { L_,        5,        -5,         0,       { 1, 1, 1, 1, 1 } },
     { L_,        5,        -5,         1,       { 0, 1, 1, 1, 1 } },
     { L_,        5,        -5,         3,       { 0, 0, 0, 1, 1 } },
     { L_,        5,        -5,         5,       { 0, 0, 0, 0, 0 } },
        };

        const int SIZE = sizeof DATA / sizeof *DATA;

        if (verbose)
            cout << "Testing ctor, 'operator++' and 'operator--'." << endl;

        int i;
        for (i = 0; i < SIZE; ++i) {
            const int  LINE   = DATA[i].d_line;
            const int  ORIGIN = DATA[i].d_origin;
            const int  PLEN   = DATA[i].d_proctorLength;
            const int  INCDEC = DATA[i].d_incDec;
            const int *EXP    = DATA[i].d_expCounters;

            int counters[NUM_ELEM] = { 0 }; // initialized to 0's
            my_Class myClassArray[NUM_ELEM];
            for (int j = 0; j < NUM_ELEM; ++j) {
                myClassArray[j].setCounter(&counters[j]);
            }

            {
                typedef bdema_AutoDestructor<my_Class> T;
                T mX(myClassArray + ORIGIN, PLEN);
                if (0 < INCDEC) {       // increment
                    for (int ii = 0; ii < INCDEC; ++ii, ++mX);
                }
                else if (0 > INCDEC) {  // decrement
                    for (int di = 0; di > INCDEC; --di, --mX);
                }
            }

            if (veryVerbose) { cout << '\t'; P_(i); PA(counters, NUM_ELEM); }
            LOOP2_ASSERT(LINE, i, areEqual(EXP, counters, NUM_ELEM));
        }

        if (verbose) cout << "Testing 'release'." << endl;

        for (i = 0; i < SIZE; ++i) {
            const int  LINE   = DATA[i].d_line;
            const int  ORIGIN = DATA[i].d_origin;
            const int  PLEN   = DATA[i].d_proctorLength;
            const int  INCDEC = DATA[i].d_incDec;
            const static int EXP[NUM_ELEM] = { 0 };

            int counters[NUM_ELEM] = { 0 }; // initialized to 0's
            my_Class myClassArray[NUM_ELEM];
            for (int j = 0; j < NUM_ELEM; ++j) {
                myClassArray[j].setCounter(&counters[j]);
            }

            {
                typedef bdema_AutoDestructor<my_Class> T;
                T mX(myClassArray + ORIGIN, PLEN);
                if (0 < INCDEC) {       // increment
                    for (int ii = 0; ii < INCDEC; ++ii, ++mX);
                }
                else if (0 > INCDEC) {  // decrement
                    for (int di = 0; di > INCDEC; --di, --mX);
                }
                mX.release();
            }

            if (veryVerbose) { cout << '\t'; P_(i); PA(counters, NUM_ELEM); }
            LOOP2_ASSERT(LINE, i, areEqual(EXP, counters, NUM_ELEM));
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // STATIC FUNCTIONS TEST
        //   Iterate over a set of table-generated test vectors and perform
        //   independent tests.  For each test, verify that the 'areEqual'
        //   function produces the expected result with the input arrays and
        //   array length specified in the test vector.
        //
        // Testing:
        //   int areEqual(ar1, ar2, len);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "STATIC FUNCTIONS TEST" << endl
                                  << "=====================" << endl;

        if (verbose) cout << "Testing 'areEqual'." << endl;
        {
            const int SZ = 5;
            struct {
                int d_line;       // line number
                int d_array1[SZ]; // first array argument
                int d_array2[SZ]; // second array argument
                int d_length;     // number of elements to compare
                int d_exp;        // expected result
            } DATA[] = {
    // line   array1                   array2               length    result
    // ----   ----------------         ----------------     ------    ------
     { L_,    { 0, 0, 0, 0, 0 },       { 0, 0, 0, 0, 0 },     5,        1 },
     { L_,    { 0, 0, 0, 0, 0 },       { 1, 0, 0, 0, 0 },     5,        0 },
     { L_,    { 0, 0, 0, 0, 0 },       { 1, 2, 3, 4, 5 },     5,        0 },
     { L_,    { 1, 2, 3, 4, 5 },       { 1, 2, 3, 4, 5 },     5,        1 },
     { L_,    { 1, 2, 3, 4, 5 },       { 1, 2, 3, 4, 0 },     5,        0 },
     { L_,    {-1, 2,-3, 4,-5 },       {-1, 2,-3, 4,-5 },     5,        1 },
     { L_,    { 1, 2, 3, 4, 5 },       { 6, 7, 8, 9, 0 },     0,        1 },
     { L_,    { 1, 2, 3, 4, 5 },       { 1, 2, 3, 9, 0 },     3,        1 }
            };

            const int NUM_TEST = sizeof DATA / sizeof *DATA;
            for (int i = 0; i < NUM_TEST; ++i) {
                const int  LINE = DATA[i].d_line;
                const int *A1   = DATA[i].d_array1;
                const int *A2   = DATA[i].d_array2;
                const int  LEN  = DATA[i].d_length;
                const int  EXP  = DATA[i].d_exp;
                if (veryVerbose) {
                    cout << '\t'; P_(i); P_(LEN); PA_(A1, SZ); PA(A2, SZ);
                }
                LOOP2_ASSERT(LINE, i, EXP == areEqual(A1, A2, LEN));
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // HELPER CLASS TEST
        //   Create 'my_Class' objects and assign (either at construction or
        //   through the 'setCounter' method) to each object a *counter*
        //   variable.  Verify that the counter is incremented after each
        //   object is destroyed.
        //
        // Testing:
        //   my_Class(counter);
        //   ~my_Class();
        //   void setCounter(counter);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "HELPER CLASS TEST" << endl
                                  << "=================" << endl;

        if (verbose) cout << "Testing 'my_Class'." << endl;

        if (verbose) cout << "\tTesting default ctor and dtor." << endl;
        {
            int counter = 0;
            const int NUM_TEST = 5;
            for (int i = 0; i < NUM_TEST; ++i) {
                LOOP_ASSERT(i, counter == i);
                my_Class X(&counter);
            }
            ASSERT(NUM_TEST == counter);
        }

        if (verbose) cout << "\tTesting 'setCounter'." << endl;
        {
            int dummy = -1;
            int counter = 0;
            const int NUM_TEST = 5;
            for (int i = 0; i < NUM_TEST; ++i) {
                LOOP_ASSERT(i, i == counter);
                LOOP_ASSERT(i, -1 == dummy);
                my_Class x(&dummy);
                x.setCounter(&counter);
            }
            ASSERT(NUM_TEST == counter);
            ASSERT(-1 == dummy);
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
