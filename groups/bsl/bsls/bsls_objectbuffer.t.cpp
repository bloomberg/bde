// bsls_objectbuffer.t.cpp                                            -*-C++-*-

#include <bsls_objectbuffer.h>

#include <bsls_platform.h>          // for testing only

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// bsls::ObjectBuffer is a simple template class that provides a few
// compile-time invariants and four trivial run-time functions.  Our tests
// involve instantiating it with various representative template parameters
// and verifying the invariants.
//-----------------------------------------------------------------------------
// [2] T& object();
// [2] char *buffer();
// [2] const T& object() const;
// [2] const char *buffer() const;
//
// [1] CLASS INVARIANTS
// [3] USAGE EXAMPLE
//-----------------------------------------------------------------------------

//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//-----------------------------------------------------------------------------

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
// #define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) printf("<| " #X " |>\n");  // Quote identifier literally.
//#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ printf("\t");             // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

// Placement new defined here since we can't include <new>.
// Can't use obvious definition because some compilers have placement new
// built-in.
struct Placement { void *d_ptr; Placement(void *p) : d_ptr(p) { } };
inline void *operator new(size_t, Placement p) throw() { return p.d_ptr; }
#if !defined(BSLS_PLATFORM_CMP_MSVC) && \
   (!defined(BSLS_PLATFORM_CMP_GNU) || BSLS_PLATFORM_CMP_VER_MAJOR >= 30000)
inline void operator delete(void *, Placement) throw() { }
#elif defined(BSLS_PLATFORM_CMP_MSVC)
// Visual C++ produces an internal compiler error if we provide the delete
// operator above, but insists on giving us warnings if we don't.  Explicitly
// disable that warning until the compiler is fixed.  Last tested with VC2008.
#pragma warning(disable : 4291)
#endif

//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

class my_String
{
    size_t  d_len;
    char   *d_data;

    void set(const char* s, size_t len);

  public:
    my_String(const char* s);                                       // IMPLICIT
    my_String(const my_String& rhs);                                // IMPLICIT
    my_String& operator=(const my_String& rhs);
    ~my_String();

    const char* c_str() const;
    size_t length() const;
};

bool operator==(const my_String& s1, const my_String& s2);
bool operator!=(const my_String& s1, const my_String& s2);

void my_String::set(const char* s, size_t len)
{
    d_len = len;
    d_data = new char[len + 1];
    memcpy(d_data, s, len);
    d_data[len] = '\0';
}

my_String::my_String(const char* s)
: d_len(0), d_data(0)
{
    set(s, strlen(s));
}

my_String::my_String(const my_String& rhs)
: d_len(0), d_data(0)
{
    set(rhs.d_data, rhs.d_len);
}

my_String& my_String::operator=(const my_String& rhs)
{
    if (this != &rhs) {
        delete[] d_data;
        set(rhs.d_data, rhs.d_len);
    }
    return *this;
}

my_String::~my_String()
{
    delete[] d_data;
}

const char* my_String::c_str() const
{
    return d_data;
}

size_t my_String::length() const
{
    return d_len;
}

bool operator==(const my_String& s1, const my_String& s2)
{
    return s1.length() == s2.length() && 0 == strcmp(s1.c_str(), s2.c_str());
}

bool operator!=(const my_String& s1, const my_String& s2)
{
    return ! (s1 == s2);
}

struct my_Type
{
    int   d_i;
    void *d_p;
    char  d_c;
};

//=============================================================================
//                              USAGE EXAMPLES
//-----------------------------------------------------------------------------

// The examples below use a value-semantic string class, 'my_String' which can
// be constructed from a null-terminated string and contains a member, 'c_str'
// which returns a null-terminated string.  'my_String' does not have a default
// constructor and thus cannot be used in C-style arrays or unions.
//
///Usage Example 1:
///- - - - - - - -
// Here we use 'bsls::ObjectBuffer' to create a variable-length array of
// 'my_String' objects.  For efficiency, the array is created on the stack as
// a fixed-sized array of 'bsls::ObjectBuffer<my_String>' objects and the
// length is kept in a separate variable.  Only 'len' calls are made to the
// 'my_String' constructor, with the unused array elements left as raw
// memory.  An array directly containing 'my_String' objects would not have
// been possible because 'my_String' does not have a default constructor.
//
// WARNING: the 'manipulateStrings' function below is not exception-safe.
// If an exception is thrown anywhere within the function (e.g., from a
// constructor call), the destructor will not be called on the constructed
// string objects.  This logic would typically be augmented with guard objects
// that call destructors in case of exception.
//..
    void manipulateStrings(const my_String* stringArray, int len)
    {
        ASSERT(len <= 10);

        bsls::ObjectBuffer<my_String> tempArray[10];
        for (int i = 0; i < len; ++i) {
            new (tempArray[i].buffer()) my_String(stringArray[i]);
            ASSERT(stringArray[i] == tempArray[i].object())
        }

        for (int i = 0; i < len; ++i)
        {
            my_String& s = tempArray[i].object();
            ASSERT(s.c_str());  // use 's'
            // ... String manipulations go here.  's' might be analyzed,
            // appended-to, passed to other functions, etc.
        }

        while (len) {
            // Destroy strings.  Although not critical to this example, we
            // follow the general rule of destroying the objects in reverse
            // order of their construction, thus mimicking the
            // compiler-generated destruction order for normal array objects.
            tempArray[--len].object().~my_String();
        }
    }

    int usageExample1()
    {
        const my_String INARRAY[3] = {
            my_String("hello"),
            my_String("goodbye"),
            my_String("Bloomberg")
        };

        manipulateStrings(INARRAY, 3);

        return 0;
    }
//..
///Usage Example 2:
///- - - - - - - -
// Here we use 'bsls::ObjectBuffer' to compose a variable-type object capable
// of holding a string or an integer:
//..
    class my_Union
    {
      public:
        enum TypeTag { INT, STRING };

      private:
        TypeTag                           d_type;
        union {
            int                           d_int;
            bsls::ObjectBuffer<my_String> d_string;
        };

      public:
        my_Union(int i = 0) : d_type(INT) { d_int = i; }            // IMLPICIT
        my_Union(const my_String& s) : d_type(STRING) {             // IMLPICIT
            new (d_string.buffer()) my_String(s); }
        my_Union(const char *s) : d_type(STRING) {                  // IMLPICIT
            new (d_string.buffer()) my_String(s); }
        my_Union(const my_Union& rhs) : d_type(rhs.d_type) {
            if (INT == d_type) {
                d_int = rhs.d_int;
            }
            else {
                new (d_string.buffer()) my_String(rhs.d_string.object());
            }
        }
        ~my_Union() {
            if (STRING == d_type) d_string.object().~my_String(); }

        my_Union& operator=(const my_Union& rhs) {
            if (INT == d_type) {
                if (INT == rhs.d_type) {
                    d_int = rhs.d_int;
                }
                else { // if STRING == rhs.d_type
                    new (d_string.buffer()) my_String(rhs.d_string.object());
                }
            }
            else { // if (STRING == d_type)
                if (INT == rhs.d_type) {
                    d_string.object().~my_String();
                    d_int = rhs.d_int;
                }
                else { // if STRING == rhs.d_type
                    d_string.object() = rhs.d_string.object();
                }
            }
            d_type = rhs.d_type;
            return *this;
        }

        TypeTag typeTag() const { return d_type; }

        int asInt() const {
            return INT == d_type ?
                            d_int : static_cast<int>(
                                      strtol(d_string.object().c_str(), 0, 0));
        }

        my_String asString() const {
            if (INT == d_type) {
                char temp[15];
                sprintf(temp, "%d", d_int);
                return my_String(temp);                               // RETURN
            }
            else {
                return d_string.object();                             // RETURN
            }
        }
    };

    int usageExample2()
    {
        ASSERT(sizeof(bsls::ObjectBuffer<my_String>) == sizeof(my_String));

        // Create a 'my_Union' object containing a string.
        const my_Union U1("hello");
        ASSERT(my_Union::STRING == U1.typeTag());
        ASSERT(0 == U1.asInt());
        ASSERT("hello" == U1.asString());

        // Create a 'my_Union' object containing an integer.
        const my_Union U2(123);
        ASSERT(my_Union::INT == U2.typeTag());
        ASSERT(123 == U2.asInt());
        ASSERT("123" == U2.asString());

        // Create a 'my_Union' object containing a string that can be
        // interpreted as an integer.
        const my_Union U3("0x456");
        ASSERT(my_Union::STRING == U3.typeTag());
        ASSERT(0x456 == U3.asInt());
        ASSERT("0x456" == U3.asString());

        // Copy-construct a 'my_Union' object containing a string.
        my_Union u4(U3);
        ASSERT(my_Union::STRING == u4.typeTag());
        ASSERT(0x456 == u4.asInt());
        ASSERT("0x456" == u4.asString());

        // Use assignment to change 'u4' from string to integer.
        u4 = U2;
        ASSERT(my_Union::INT == u4.typeTag());
        ASSERT(123 == u4.asInt());
        ASSERT("123" == u4.asString());

        return 0;
    }
//..

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;
    // int veryVeryVerbose = argc > 4;

    setbuf(stdout, 0);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Simple example showing how one might use bsls::ObjectBuffer
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");
        usageExample1();
        usageExample2();

      } break;

      case 2: {
        // --------------------------------------------------------------------
        // object() and buffer() TEST
        //
        // Concerns:
        //   - object() returns a reference with the same address as buffer
        //   - buffer() returns the address of the first byte of the buffer
        //
        // Plan:
        //   - Create a 'bsls::ObjectBuffer' objects with different sizes and
        //     alignments.
        //   - For each object, verify that buffer() returns the address of
        //     the object.
        //
        // Testing:
        //   TYPE& object();
        //   char *buffer();
        //   const TYPE& object() const;
        //   const char *buffer() const;
        // --------------------------------------------------------------------

#       define TEST_METHODS(TYPE)                                       \
        do {                                                            \
            bsls::ObjectBuffer<TYPE> buff;                              \
            const bsls::ObjectBuffer<TYPE>& BUFF = buff;                \
            ASSERT(&BUFF == static_cast<void*>(&buff.object()));        \
            ASSERT((const char*) &BUFF == buff.buffer());               \
            ASSERT(&BUFF == static_cast<const void*>(&BUFF.object()));  \
            ASSERT((const char*) &BUFF == BUFF.buffer());               \
        } while (false)

        TEST_METHODS(char);
        TEST_METHODS(unsigned char);
        TEST_METHODS(int);
        TEST_METHODS(char*);
        TEST_METHODS(void (*)(int));
        TEST_METHODS(my_Type);
      } break;

      case 1: {
        // --------------------------------------------------------------------
        // INVARIANT TEST
        //
        // Concerns:
        //   - object buffer has same alignment as specified type
        //   - object buffer has same size as specified type
        //
        // Plan:
        //   For a representative set of TYPE template parameters
        //   instantiate bsls::ObjectBuffer<TYPE> (a.k.a. Buff) and
        //   verify that:
        //   - bsls::AlignmentFromType<Buff>::VALUE ==
        //                                 bsls::AlignmentFromType<TYPE>::VALUE
        //   - sizeof(Buff) == sizeof(TYPE)
        //
        // Testing:
        //   Class Invariants
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING CLASS INVARIANTS"
                            "\n========================\n");

#       define TEST_INVARIANTS(TYPE)                                 \
        do {                                                         \
            typedef bsls::ObjectBuffer<TYPE> Buff;                   \
            ASSERT((int) bsls::AlignmentFromType<Buff>::VALUE ==     \
                   (int) bsls::AlignmentFromType<TYPE>::VALUE);      \
            ASSERT(sizeof(Buff) == sizeof(TYPE));                    \
        } while (false)

        TEST_INVARIANTS(char);
        TEST_INVARIANTS(unsigned char);
        TEST_INVARIANTS(int);
        TEST_INVARIANTS(char*);
        TEST_INVARIANTS(void (*)(int));
        TEST_INVARIANTS(my_Type);
      } break;

      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
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
