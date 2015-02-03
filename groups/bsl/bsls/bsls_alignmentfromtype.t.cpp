// bsls_alignmentfromtype.t.cpp                                       -*-C++-*-

#include <bsls_alignmentfromtype.h>

#include <bsls_platform.h>

#include <algorithm>
#include <cstddef>     // offsetof() macro
#include <cstdlib>     // atoi()
#include <cstring>
#include <iostream>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                             Overview
//                             --------
// Most of what this component implements are compile-time computations that
// differ among platforms.  The tests do assume that alignment of 'char' is 1,
// 'short' is 2, 'int' is 4, and 'double' is at least 4.  In addition, the
// invariant that the alignment of a 'struct' equals the alignment of its
// most-strictly aligned member.
//
//-----------------------------------------------------------------------------
// [ 1] bsls::AlignmentFromType<T>::VALUE
//
//-----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE -- Ensure the usage example compiles and works.
//=============================================================================

//-----------------------------------------------------------------------------
//                  STANDARD BDE ASSERT TEST MACRO
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

//=============================================================================
//                    STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

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

//=============================================================================
//                      SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define A(X) cout << #X " = " << ((void *) X) << endl;  // Print address
#define A_(X) cout << #X " = " << ((void *) X) << ", " << flush;
#define L_ __LINE__                           // current Line number
#define TAB cout << '\t';

//=============================================================================
//                             USAGE EXAMPLE
//-----------------------------------------------------------------------------

namespace {

///Usage
///-----
///Usage Example 1
///- - - - - - - -
// The following shows how 'bsls::AlignmentFromType<T>::VALUE' can be used to
// create a static "database" of types storing their size and required
// alignment.
//
// This information can be populated into an array of 'my_ElemAttr' elements
// below:
//..
    enum my_ElemType { MY_CHAR, MY_INT, MY_DOUBLE, MY_POINTER };

    struct my_ElemAttr {
        my_ElemType d_type;       // type indicator
        int         d_size;       // 'sizeof' the type
        int         d_alignment;  // alignment requirement for the type
    };

    static const my_ElemAttr MY_ATTRIBUTES[] = {
       { MY_CHAR,     sizeof(char),   bsls::AlignmentFromType<char>::VALUE   },
       { MY_INT,      sizeof(int),    bsls::AlignmentFromType<int>::VALUE    },
       { MY_DOUBLE,   sizeof(double), bsls::AlignmentFromType<double>::VALUE },
       { MY_POINTER,  sizeof(void *), bsls::AlignmentFromType<void *>::VALUE }
    };
//..
///Usage Example 2
///- - - - - - - -
// Consider a parameterized type, 'my_AlignedBuffer', that provides aligned
// memory to store a user-defined type.  A 'my_AlignedBuffer' object is useful
// in situations where efficient (e.g., stack-based) storage is required.
//
// The 'my_AlignedBuffer' 'union' (defined below) takes a template parameter
// 'TYPE', and provides an appropriately sized and aligned block of memory via
// the 'buffer' functions.  Note that 'my_AlignedBuffer' ensures that the
// returned memory is aligned correctly for the specified size by using
// 'bsls::AlignmentFromType<TYPE>::Type', which provides a primitive type
// having the same alignment requirement as 'TYPE'.  The class definition of
// 'my_AlignedBuffer' is as follows:
//..
    template <class TYPE>
    union my_AlignedBuffer {
      private:
        // DATA
        char                                         d_buffer[sizeof(TYPE)];
        typename bsls::AlignmentFromType<TYPE>::Type d_align; //force alignment

      public:
        // MANIPULATORS
        char *buffer();
            // Return the address of the modifiable first byte of memory
            // contained by this object as a 'char *' pointer.

        TYPE& object();
            // Return a reference to the modifiable 'TYPE' object stored in
            // this buffer.  The referenced object has an undefined state
            // unless a valid 'TYPE' object has been constructed in this
            // buffer.

        // ACCESSORS
        const char *buffer() const;
            // Return the address of the non-modifiable first byte of memory
            // contained by this object as a 'const char *' pointer.

        const TYPE& object() const;
            // Return a reference to the non-modifiable 'TYPE' object stored in
            // this buffer.  The referenced object has an undefined state
            // unless a valid 'TYPE' object has been constructed in this
            // buffer.
    };
//..
// The function definitions of 'my_AlignedBuffer' are as follows:
//..
    // MANIPULATORS
    template <class TYPE>
    inline
    char *my_AlignedBuffer<TYPE>::buffer()
    {
        return d_buffer;
    }

    template <class TYPE>
    inline
    TYPE& my_AlignedBuffer<TYPE>::object()
    {
        return *reinterpret_cast<TYPE *>(this);
    }

    // ACCESSORS
    template <class TYPE>
    inline
    const char *my_AlignedBuffer<TYPE>::buffer() const
    {
        return d_buffer;
    }

    template <class TYPE>
    inline
    const TYPE& my_AlignedBuffer<TYPE>::object() const
    {
        return *reinterpret_cast<const TYPE *>(this);
    }
//..
// 'my_AlignedBuffer' can be used to construct buffers for different types and
// with varied alignment requirements.  Consider that we want to construct an
// object that stores the response of a floating-point operation.  If the
// operation is successful, then the response object stores a 'double' result;
// otherwise, it stores an error string of type 'string', which is based on the
// standard type 'string' (see 'bslstl_string').  For the sake of brevity, the
// implementation of 'string' is not explored here.  Here is the definition for
// the 'Response' class:
//..
class string {

    // DATA
    char            *d_value_p;      // 0 terminated character array
    std::size_t      d_size;         // length of d_value_p

     // PRIVATE CLASS CONSTANTS
    static const char *EMPTY_STRING;

  public:
    // CREATORS
    explicit string()
    : d_value_p(const_cast<char *>(EMPTY_STRING))
    , d_size(0)
    {
    }

    string(const char *value)
    : d_value_p(const_cast<char *>(EMPTY_STRING))
    , d_size(std::strlen(value))
    {
        if (d_size > 0) {
            d_value_p = new char[d_size + 1];
            std::memcpy(d_value_p, value, d_size + 1);
        }
    }

    string(const string& original)
    : d_value_p(const_cast<char *>(EMPTY_STRING))
    , d_size(original.d_size)
    {
        if (d_size > 0) {
            d_value_p = new char[d_size + 1];
            std::memcpy(d_value_p, original.d_value_p, d_size + 1);
        }
    }

    ~string()
    {
        if (d_size > 0) {
            delete[] d_value_p;
        }
    }

    // MANIPULATORS
    string& operator=(const string& rhs) {
        string temp(rhs);
        temp.swap(*this);
        return *this;
    }

    char &operator[](int index)
    {
        return d_value_p[index];
    }

    void swap(string& other)
    {
        std::swap(d_value_p, other.d_value_p);
        std::swap(d_size, other.d_size);
    }

    // ACCESSORS
    std::size_t size() const
    {
        return d_size;
    }

    bool empty() const
    {
        return 0 == d_size;
    }

    const char *c_str() const
    {
        return d_value_p;
    }
};

inline
bool operator==(const string& lhs, const string& rhs)
{
    return 0 == std::strcmp(lhs.c_str(), rhs.c_str());
}

inline
bool operator!=(const string& lhs, const string& rhs)
{
    return !(lhs == rhs);
}

inline
bool operator<(const string& lhs, const string& rhs)
{
    return std::strcmp(lhs.c_str(), rhs.c_str()) < 0;
}

inline
bool operator>(const string& lhs, const string& rhs)
{
    return rhs < lhs;
}

const char *string::EMPTY_STRING = "";

    class Response {
//..
// Note that we use 'my_AlignedBuffer' to allocate sufficient, aligned memory
// to store the result of the operation or an error message:
//..
      private:
        union {
            my_AlignedBuffer<double>      d_result;
            my_AlignedBuffer<string> d_errorMessage;
        };
//..
// The 'isError' flag indicates whether the response object stores valid data
// or an error message:
//..
        bool d_isError;
//..
// Below we provide a simple public interface suitable for illustration only:
//..
      public:
        // CREATORS
        Response(double result);
            // Create a response object that stores the specified 'result'.

        Response(const string& errorMessage);
            // Create a response object that stores the specified
            // 'errorMessage'.

        ~Response();
            // Destroy this response object.
//..
// The manipulator functions allow clients to update the response object to
// store either a 'double' result or an error message:
//..
        // MANIPULATORS
        void setResult(double result);
            // Update this object to store the specified 'result'.  After this
            // operation 'isError' returns 'false'.

        void setErrorMessage(const string& errorMessage);
            // Update this object to store the specified 'errorMessage'.  After
            // this operation 'isError' returns 'true'.
//..
// The 'isError' function informs clients whether a response object stores a
// result value or an error message:
//..
        // ACCESSORS
        bool isError() const;
            // Return 'true' if this object stores an error message, and
            // 'false' otherwise.

        double result() const;
            // Return the result value stored by this object.  The behavior is
            // undefined unless 'false == isError()'.

        const string& errorMessage() const;
            // Return a reference to the non-modifiable error message stored by
            // this object.  The behavior is undefined unless
            // 'true == isError()'.
    };
//..
// Below we provide the function definitions.  Note that we use the
// 'my_AlignedBuffer::buffer' function to access correctly aligned memory.
// Also note that 'my_AlignedBuffer' just provides the memory for an object;
// therefore, the 'Response' class is responsible for the construction and
// destruction of the specified objects.  Since our 'Response' class is for
// illustration purposes only, we ignore exception-safety concerns; nor do we
// supply an allocator to the string constructor, allowing the default
// allocator to be used instead:
//..
    // CREATORS
    Response::Response(double result)
    {
        new (d_result.buffer()) double(result);
        d_isError = false;
    }

    Response::Response(const string& errorMessage)
    {
        new (d_errorMessage.buffer()) string(errorMessage);
        d_isError = true;
    }

    Response::~Response()
    {
        if (d_isError) {
            typedef string Type;
            d_errorMessage.object().~Type();
        }
    }

    // MANIPULATORS
    void Response::setResult(double result)
    {
        if (!d_isError) {
            d_result.object() = result;
        }
        else {
            typedef string Type;
            d_errorMessage.object().~Type();
            new (d_result.buffer()) double(result);
            d_isError = false;
        }
    }

    void Response::setErrorMessage(const string& errorMessage)
    {
        if (d_isError) {
            d_errorMessage.object() = errorMessage;
        }
        else {
            new (d_errorMessage.buffer()) string(errorMessage);
            d_isError = true;
        }
    }

    // ACCESSORS
    bool Response::isError() const
    {
        return d_isError;
    }

    double Response::result() const
    {
        ASSERT(!d_isError);

        return d_result.object();
    }

    const string& Response::errorMessage() const
    {
        ASSERT(d_isError);

        return d_errorMessage.object();
    }
//..

}  // close unnamed namespace

//=============================================================================
//                  CLASSES AND FUNCTIONS USED IN TESTS
//-----------------------------------------------------------------------------

struct S1 { char d_buff[8]; S1(char); };
struct S2 { char d_buff[8]; int d_int; S2(); private: S2(const S2&); };
struct S3 { S1 d_s1; double d_double; short d_short; };
struct S4 { short d_shorts[5]; char d_c;  S4(int); private: S4(const S4&); };
#if (defined(BSLS_PLATFORM_OS_LINUX) || defined(BSLS_PLATFORM_OS_DARWIN)) \
 && defined(BSLS_PLATFORM_CPU_X86)
struct S5 { long long d_longLong __attribute__((__aligned__(8))); };
#endif
union  U1 { char d_c; int *d_pointer; };

//=============================================================================
//                  CLASSES AND FUNCTIONS USED IN TESTS
//-----------------------------------------------------------------------------

template <class T>
inline
bool samePtrType(T *, void *)
{
    return false;
}

template <class T>
inline
bool samePtrType(T *, T *)
{
    return true;
}

template <class T1, class T2>
inline
bool sameType(T1 t1, T2 t2)
{
    return samePtrType(&t1, &t2);
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    (void) veryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 2: {
        // --------------------------------------------------------------------
        // USAGE TEST
        //   Make sure main usage examples compile and work as advertized.
        //
        // Test plan:
        //   Copy usage example verbatim into test driver then change 'assert'
        //   to 'ASSERT'.  Since usage example code declares template classes
        //   and functions, it must be placed outside of main().  Within this
        //   test case, therefore, we call the externally-declared functions
        //   defined above.
        //
        // Testing:
        //   USAGE EXAMPLE -- Ensure the usage example compiles and works.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nUSAGE" << endl
                          << "\n=====" << endl;

// Clients of the 'Response' class can use it as follows:
//..
    double value1 = 111.2, value2 = 92.5;

    if (0 == value2) {
        Response response("Division by 0");

        // Return erroneous response
    }
    else {
        Response response(value1 / value2);

        // Process response object
    }
//..

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING META-FUNCTION bsls::AlignmentFromType<T>::VALUE
        //   Ensure that alignment values are correctly computed.
        //   Ensure that alignment of structs is computed as alignment of
        //   most restrictive member.
        //   Ensure that bsls::AlignmentFromType can be instantiated on a type
        //   that has no default constructor and has a private copy
        //   constructor.
        //
        // PLAN
        //   To test 'bsls::AlignmentFromType<T>::VALUE', we must instantiate
        //   'bsls::AlignmentFromType' for several types and test that the
        //   VALUE member has the expected value.  Since the alignment
        //   requirements for char, short and int are 1, 2 and 4 on all
        //   currently-supported platforms, we use these as our test cases.  We
        //   also verify that double has an alignment at least as restrictive
        //   as int and that 'unsigned int' has exactly the same alignment as
        //   'int'.
        //
        //   To test the alignment of structs, we choose three structures with
        //   different compositions as well as one union and test that
        //   bsls::AlignmentFromType computes an alignment value for each of
        //   them equal to the alignment of the most restrictive element.  At
        //   least one of these structured type has no default constructor
        //   and/or has a private copy constructor.
        //
        //   In veryVerbose mode, print the alignment of each type being
        //   tested.
        //
        // TACTICS
        //   Ad-hoc test data.
        //
        // TESTING
        //   bsls::AlignmentFromType<T>::VALUE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTest bsls::AlignmentFromType<T>::VALUE"
                          << "\n======================================"
                          << endl;

        typedef void (*FuncPtr)();
        enum {
            CHAR_ALIGNMENT        = bsls::AlignmentFromType<char>::VALUE,
            SHORT_ALIGNMENT       = bsls::AlignmentFromType<short>::VALUE,
            INT_ALIGNMENT         = bsls::AlignmentFromType<int>::VALUE,
            LONG_ALIGNMENT        = bsls::AlignmentFromType<long>::VALUE,
            INT64_ALIGNMENT       = bsls::AlignmentFromType<long long>::VALUE,
            BOOL_ALIGNMENT        = bsls::AlignmentFromType<bool>::VALUE,
            WCHAR_T_ALIGNMENT     = bsls::AlignmentFromType<wchar_t>::VALUE,
            PTR_ALIGNMENT         = bsls::AlignmentFromType<void*>::VALUE,
            FUNC_PTR_ALIGNMENT    = bsls::AlignmentFromType<FuncPtr>::VALUE,
            FLOAT_ALIGNMENT       = bsls::AlignmentFromType<float>::VALUE,
            DOUBLE_ALIGNMENT      = bsls::AlignmentFromType<double>::VALUE,
            LONG_DOUBLE_ALIGNMENT = bsls::AlignmentFromType<long double>
                                                                       ::VALUE,

            S1_ALIGNMENT          = bsls::AlignmentFromType<S1>::VALUE,
            S2_ALIGNMENT          = bsls::AlignmentFromType<S2>::VALUE,
            S3_ALIGNMENT          = bsls::AlignmentFromType<S3>::VALUE,
            S4_ALIGNMENT          = bsls::AlignmentFromType<S4>::VALUE,
#if (defined(BSLS_PLATFORM_OS_LINUX) || defined(BSLS_PLATFORM_OS_DARWIN)) \
 && defined(BSLS_PLATFORM_CPU_X86)
            S5_ALIGNMENT          = bsls::AlignmentFromType<S5>::VALUE,
#endif
            U1_ALIGNMENT          = bsls::AlignmentFromType<U1>::VALUE
        };

        {
            int EXP_CHAR_ALIGNMENT        = 1;
            int EXP_BOOL_ALIGNMENT        = 1;
            int EXP_SHORT_ALIGNMENT       = 2;
            int EXP_WCHAR_T_ALIGNMENT     = 4;
            int EXP_INT_ALIGNMENT         = 4;
            int EXP_LONG_ALIGNMENT        = 4;
            int EXP_INT64_ALIGNMENT       = 8;
            int EXP_PTR_ALIGNMENT         = 4;
            int EXP_FUNC_PTR_ALIGNMENT    = 4;
            int EXP_FLOAT_ALIGNMENT       = 4;
            int EXP_DOUBLE_ALIGNMENT      = 8;
            int EXP_LONG_DOUBLE_ALIGNMENT = 8;

            int EXP_S1_ALIGNMENT          = 1;
            int EXP_S2_ALIGNMENT          = 4;
            int EXP_S3_ALIGNMENT          = 8;
            int EXP_S4_ALIGNMENT          = 2;
            int EXP_S5_ALIGNMENT          = 8;
            (void) EXP_S5_ALIGNMENT;
            int EXP_U1_ALIGNMENT          = 4;

// Specializations for different architectures
#if (defined(BSLS_PLATFORM_OS_LINUX) || defined(BSLS_PLATFORM_OS_DARWIN)) \
 && defined(BSLS_PLATFORM_CPU_X86)
            EXP_INT64_ALIGNMENT           = 4;
            EXP_DOUBLE_ALIGNMENT          = 4;
#ifdef BSLS_PLATFORM_OS_LINUX
            EXP_LONG_DOUBLE_ALIGNMENT     = 4;
#else
            EXP_LONG_DOUBLE_ALIGNMENT     = 16;
#endif
            EXP_S3_ALIGNMENT              = 4;
            LOOP2_ASSERT(S5_ALIGNMENT, EXP_S5_ALIGNMENT,
                         EXP_S5_ALIGNMENT == S5_ALIGNMENT);
#endif

#if defined(BSLS_PLATFORM_CPU_64_BIT)
            EXP_LONG_ALIGNMENT            = 8;
            EXP_PTR_ALIGNMENT             = 8;
            EXP_FUNC_PTR_ALIGNMENT        = 8;
            EXP_U1_ALIGNMENT              = 8;
#if defined(BSLS_PLATFORM_CPU_POWERPC) && defined(BSLS_PLATFORM_OS_LINUX)
            EXP_LONG_DOUBLE_ALIGNMENT     = 8;
#else
            EXP_LONG_DOUBLE_ALIGNMENT     = 16;
#endif
#endif

#if defined(BSLS_PLATFORM_OS_AIX)
    #if !defined(BSLS_PLATFORM_CPU_64_BIT)
            EXP_WCHAR_T_ALIGNMENT         = 2;
    #endif
            EXP_DOUBLE_ALIGNMENT          = 4;
            EXP_LONG_DOUBLE_ALIGNMENT     = 4;
            EXP_S3_ALIGNMENT              = 4;
#endif

#if defined(BSLS_PLATFORM_OS_CYGWIN)
            EXP_WCHAR_T_ALIGNMENT         = 2;
            EXP_LONG_DOUBLE_ALIGNMENT     = 4;
#endif

#if defined(BSLS_PLATFORM_OS_WINDOWS)
            EXP_WCHAR_T_ALIGNMENT         = 2;
    #if defined(BSLS_PLATFORM_CPU_64_BIT)
            EXP_LONG_ALIGNMENT            = 4;
            EXP_LONG_DOUBLE_ALIGNMENT     = 8;
    #endif
#endif

            LOOP2_ASSERT(CHAR_ALIGNMENT, EXP_CHAR_ALIGNMENT,
                         EXP_CHAR_ALIGNMENT == CHAR_ALIGNMENT);
            LOOP2_ASSERT(BOOL_ALIGNMENT, EXP_BOOL_ALIGNMENT,
                         EXP_BOOL_ALIGNMENT == BOOL_ALIGNMENT);
            LOOP2_ASSERT(SHORT_ALIGNMENT, EXP_SHORT_ALIGNMENT,
                         EXP_SHORT_ALIGNMENT == SHORT_ALIGNMENT);
            LOOP2_ASSERT(WCHAR_T_ALIGNMENT, EXP_WCHAR_T_ALIGNMENT,
                         EXP_WCHAR_T_ALIGNMENT == WCHAR_T_ALIGNMENT);
            LOOP2_ASSERT(INT_ALIGNMENT, EXP_INT_ALIGNMENT,
                         EXP_INT_ALIGNMENT == INT_ALIGNMENT);
            LOOP2_ASSERT(LONG_ALIGNMENT, EXP_LONG_ALIGNMENT,
                         EXP_LONG_ALIGNMENT == LONG_ALIGNMENT);
            LOOP2_ASSERT(INT64_ALIGNMENT, EXP_INT64_ALIGNMENT,
                         EXP_INT64_ALIGNMENT == INT64_ALIGNMENT);
            LOOP2_ASSERT(PTR_ALIGNMENT, EXP_PTR_ALIGNMENT,
                         EXP_PTR_ALIGNMENT == PTR_ALIGNMENT);
            LOOP2_ASSERT(FUNC_PTR_ALIGNMENT, EXP_FUNC_PTR_ALIGNMENT,
                         EXP_FUNC_PTR_ALIGNMENT == FUNC_PTR_ALIGNMENT);
            LOOP2_ASSERT(FLOAT_ALIGNMENT, EXP_FLOAT_ALIGNMENT,
                         EXP_FLOAT_ALIGNMENT == FLOAT_ALIGNMENT);
            LOOP2_ASSERT(DOUBLE_ALIGNMENT, EXP_DOUBLE_ALIGNMENT,
                         EXP_DOUBLE_ALIGNMENT == DOUBLE_ALIGNMENT);
            LOOP2_ASSERT(LONG_DOUBLE_ALIGNMENT, EXP_LONG_DOUBLE_ALIGNMENT,
                         EXP_LONG_DOUBLE_ALIGNMENT == LONG_DOUBLE_ALIGNMENT);

            LOOP2_ASSERT(S1_ALIGNMENT, EXP_S1_ALIGNMENT,
                         EXP_S1_ALIGNMENT == S1_ALIGNMENT);
            LOOP2_ASSERT(S2_ALIGNMENT, EXP_S2_ALIGNMENT,
                         EXP_S2_ALIGNMENT == S2_ALIGNMENT);
            LOOP2_ASSERT(S3_ALIGNMENT, EXP_S3_ALIGNMENT,
                         EXP_S3_ALIGNMENT == S3_ALIGNMENT);
            LOOP2_ASSERT(S4_ALIGNMENT, EXP_S4_ALIGNMENT,
                         EXP_S4_ALIGNMENT == S4_ALIGNMENT);
            LOOP2_ASSERT(U1_ALIGNMENT, EXP_U1_ALIGNMENT,
                         EXP_U1_ALIGNMENT == U1_ALIGNMENT);
        }

        if (verbose) cout << "\nTest bsls::AlignmentFromType<T>::Type"
                          << "\n====================================="
                          << endl;

        {
            long double  LD = 0.0;
            void        *V  = 0;
            long long    LL = 0;

            (void) LD;
            (void) V;
            (void) LL;

#if defined(BSLS_PLATFORM_OS_CYGWIN)
            bsls::AlignmentImp8ByteAlignedType _8BAT;
#endif

            ASSERT(sameType(bsls::AlignmentFromType<char>::Type(), char()));
            ASSERT(sameType(bsls::AlignmentFromType<short>::Type(), short()));
            ASSERT(sameType(bsls::AlignmentFromType<int>::Type(), int()));
            ASSERT(sameType(bsls::AlignmentFromType<bool>::Type(), char()));
            ASSERT(sameType(bsls::AlignmentFromType<float>::Type(), int()));

#if (defined(BSLS_PLATFORM_OS_AIX) && !defined(BSLS_PLATFORM_CPU_64_BIT))   \
 || defined(BSLS_PLATFORM_OS_WINDOWS) || defined(BSLS_PLATFORM_OS_CYGWIN)
            ASSERT(sameType(bsls::AlignmentFromType<wchar_t>::Type(), short()));
#else
            ASSERT(sameType(bsls::AlignmentFromType<wchar_t>::Type(), int()));
#endif

#if defined(BSLS_PLATFORM_OS_LINUX) || defined(BSLS_PLATFORM_OS_DARWIN) \
                                    || defined(BSLS_PLATFORM_OS_CYGWIN)
    #if defined(BSLS_PLATFORM_CPU_64_BIT)
            ASSERT(sameType(bsls::AlignmentFromType<long long>::Type(),
                            long()));
            ASSERT(sameType(bsls::AlignmentFromType<double>::Type(), long()));
            #if (defined(BSLS_PLATFORM_CPU_POWERPC) \
              && defined(BSLS_PLATFORM_OS_LINUX))
            ASSERT(sameType(bsls::AlignmentFromType<long double>::Type(),
                            long()));
            #else
            ASSERT(sameType(bsls::AlignmentFromType<long double>::Type(), LD));
            #endif
    #else
        #if defined(BSLS_PLATFORM_CPU_ARM)
            ASSERT(sameType(bsls::AlignmentFromType<long long>::Type(),
                            LL));
            ASSERT(sameType(bsls::AlignmentFromType<double>::Type(),
                            LL));
        #elif defined(BSLS_PLATFORM_OS_CYGWIN)
            ASSERT(sameType(bsls::AlignmentFromType<long long>::Type(),
                            _8BAT));
            ASSERT(sameType(bsls::AlignmentFromType<double>::Type(),
                            _8BAT));
        #elif (defined(BSLS_PLATFORM_CPU_POWERPC) \
            && defined(BSLS_PLATFORM_OS_LINUX))
            ASSERT(sameType(bsls::AlignmentFromType<long long>::Type(),
                            LL));
            ASSERT(sameType(bsls::AlignmentFromType<double>::Type(),
                            LL));
        #else
            ASSERT(sameType(bsls::AlignmentFromType<long long>::Type(),
                            int()));
            ASSERT(sameType(bsls::AlignmentFromType<double>::Type(),
                            int()));
        #endif
        #if defined(BSLS_PLATFORM_OS_LINUX) || defined(BSLS_PLATFORM_OS_CYGWIN)
            #if defined(BSLS_PLATFORM_CPU_ARM)
            ASSERT(
                  sameType(bsls::AlignmentFromType<long double>::Type(),
                           LL));
            #elif (defined(BSLS_PLATFORM_CPU_POWERPC) \
                && defined(BSLS_PLATFORM_OS_LINUX))
            ASSERT(
                  sameType(bsls::AlignmentFromType<long double>::Type(),
                           LL));
            #else
            ASSERT(
                  sameType(bsls::AlignmentFromType<long double>::Type(),
                           int()));
            #endif
        #else
            ASSERT(
                  sameType(bsls::AlignmentFromType<long double>::Type(),
                           LD));
        #endif
    #endif
#elif defined(BSLS_PLATFORM_OS_AIX)
    #if defined(BSLS_PLATFORM_CPU_64_BIT)
            ASSERT(sameType(bsls::AlignmentFromType<long long>::Type(),
                            long()));
            ASSERT(sameType(bsls::AlignmentFromType<double>::Type(),
                            int()));
            ASSERT(
                  sameType(bsls::AlignmentFromType<long double>::Type(),
                           int()));
    #else
            ASSERT(sameType(bsls::AlignmentFromType<long long>::Type(),
                            LL));
            ASSERT(sameType(bsls::AlignmentFromType<double>::Type(),
                            int()));
            ASSERT(
                  sameType(bsls::AlignmentFromType<long double>::Type(),
                           int()));
    #endif
#else // !defined(BSLS_PLATFORM_OS_AIX) && !defined(BSLS_PLATFORM_OS_LINUX)

    #if defined(BSLS_PLATFORM_CPU_64_BIT)
        #if defined(BSLS_PLATFORM_OS_WINDOWS)
            ASSERT(sameType(bsls::AlignmentFromType<long long>::Type(),
                            LL));
            ASSERT(sameType(bsls::AlignmentFromType<double>::Type(),
                            LL));
            ASSERT(sameType(bsls::AlignmentFromType<long double>::Type(),
                            LL));
        #else
            ASSERT(sameType(bsls::AlignmentFromType<long long>::Type(),
                            long()));
            ASSERT(sameType(bsls::AlignmentFromType<double>::Type(),
                            long()));
            ASSERT(sameType(bsls::AlignmentFromType<long double>::Type(),
                            LD));
        #endif
    #else
            ASSERT(sameType(bsls::AlignmentFromType<long long>::Type(),
                            LL));
            ASSERT(sameType(bsls::AlignmentFromType<double>::Type(),
                            LL));
            ASSERT(
                  sameType(bsls::AlignmentFromType<long double>::Type(),
                           LL));
    #endif
#endif // end defined(BSLS_PLATFORM_OS_AIX)
       //  || defined(BSLS_PLATFORM_OS_LINUX)

#if defined(BSLS_PLATFORM_CPU_64_BIT)

    #if defined(BSLS_PLATFORM_OS_WINDOWS)
            ASSERT(sameType(bsls::AlignmentFromType<long>::Type(), int()));
            ASSERT(sameType(bsls::AlignmentFromType<void *>::Type(), LL));
            ASSERT(sameType(bsls::AlignmentFromType<FuncPtr>::Type(), LL));
    #else
            ASSERT(sameType(bsls::AlignmentFromType<long>::Type(), long()));
            ASSERT(sameType(bsls::AlignmentFromType<void *>::Type(), long()));
            ASSERT(sameType(bsls::AlignmentFromType<FuncPtr>::Type(), long()));
    #endif
#else // !defined(BSLS_PLATFORM_CPU_64_BIT)

            ASSERT(sameType(bsls::AlignmentFromType<long>::Type(),
                            int()));
            ASSERT(sameType(bsls::AlignmentFromType<void *>::Type(),
                            int()));
            ASSERT(sameType(bsls::AlignmentFromType<FuncPtr>::Type(),
                            int()));
#endif // end defined(BSLS_PLATFORM_CPU_64_BIT)
        }
      } break;

      default: {
        cerr << "WARNING: CASE `"<< test << "' NOT FOUND." <<endl;
        testStatus = -1;
      } break;
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
