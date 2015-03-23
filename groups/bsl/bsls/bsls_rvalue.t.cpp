// bsls_rvalue.t.cpp                                                  -*-C++-*-
#include <bsls_rvalue.h>
#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>

#include <new>
#include <stdio.h>
#include <stdlib.h>

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                                   Overview
//                                   --------
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] RVALUE<TYPE> AND RVALUEUTIL FUNCTIONALITY
// [ 3] USAGE EXAMPLE

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

namespace {

bool testFunctionCall(int *pointer, bsls::Rvalue<int> rvalue)
    // This function returns 'true' if the specified 'pointer' and the
    // specified 'rvalue' refer to the same object.
{
    return pointer == &bsls::RvalueUtil::access(rvalue);
}

template <class TYPE>
class vector
{
    TYPE *d_begin;
    TYPE *d_end;
    TYPE *d_capacity;

    static void copy(const TYPE *it, const TYPE *end, vector<TYPE> *to);
        // This auxiliary function copies the range specified by 'it' and
        // 'end' to vector specified by 'to'. The capacity of 'to' has to
        // be at least 'end - it'.
    static void swap(TYPE*& a, TYPE*& b);
        // This auxiliary function swaps the specified pointers 'a' and
        // 'b'.
  public:
    vector();
        // Create an empty vector.
    explicit vector(bsls::Rvalue<vector<TYPE> > other);
        // Create a vector by transfering the content of the specified
        // 'other'.
    vector(const vector<TYPE>& other);
        // Create a vector by copying the content of the specified 'other'.
    vector& operator= (vector<TYPE> other);
        // Assign a vector by copying the content of the specified 'other'.
        // The function returns a reference to the object.
    ~vector();
        // Destroy the vector's elements and release any allocated memory.

    TYPE&       operator[](int index)      { return this->d_begin[index]; }
        // Return a reference to the object at the specified 'index'.
    const TYPE& operator[](int index) const{ return this->d_begin[index]; }
        // Return a reference to the object at the specified 'index'.
    TYPE       *begin()       { return this->d_begin; }
        // Return a pointer to the first element.
    const TYPE *begin() const { return this->d_begin; }
        // Return a pointer to the first element.
    int capacity() const { return int(this->d_capacity - this->d_begin); }
        // Return the capacity of the vector.
    TYPE       *end()       { return this->d_end; }
        // Return a pointer to the end of the range.
    const TYPE *end() const { return this->d_end; }
        // Return a pointer to the end of the range.

    void push_back(const TYPE& value);
        // Append a copy of the specified 'value' to the vector.
    void push_back(bsls::Rvalue<TYPE> value);
        // Append an object moving the specified 'value' to the new
        // location.
    void reserve(int newCapacity);
        // Reserve enough capacity to fit at least as many elements as
        // specified by 'newCapacity'.
    int size() const { return int(this->d_end - this->d_begin); }
        // Return the size of the object.
    void swap(vector<TYPE>& other);
        // Swap the content of the vector with the specified 'other'.
};

template <class TYPE>
vector<TYPE>::vector()
    : d_begin()
    , d_end()
    , d_capacity() {
}

template <class TYPE>
void vector<TYPE>::copy(const TYPE *it, const TYPE *end, vector<TYPE> *to)
{
    for (; it != end; ++it) {
        new (to->d_end++) TYPE(*it);
    }
}

template <class TYPE>
vector<TYPE>::~vector() {
    if (this->d_begin) {
        while (this->d_begin != this->d_end--) {
            this->d_end->~TYPE();
        }
        operator delete[](this->d_begin);
    }
}

template <class TYPE>
vector<TYPE>::vector(const vector<TYPE>& other)
    : d_begin()
    , d_end()
    , d_capacity() {
    this->reserve(4 < other.size()? other.size(): 4);
    this->copy(other.begin(), other.end(), this);
}

template <class TYPE>
vector<TYPE>& vector<TYPE>::operator= (vector<TYPE> other) {
    this->swap(other);
    return *this;
}

template <class TYPE>
vector<TYPE>::vector(bsls::Rvalue<vector<TYPE> > other)
    : d_begin(bsls::RvalueUtil::access(other).d_begin)
    , d_end(bsls::RvalueUtil::access(other).d_end)
    , d_capacity(bsls::RvalueUtil::access(other).d_capacity) {
    vector<TYPE>& reference(other);
    reference.d_begin = 0;
    reference.d_end = 0;
    reference.d_capacity = 0;
}

template <class TYPE>
void vector<TYPE>::push_back(const TYPE& value) {
    if (this->d_end == this->d_capacity) {
        this->reserve(this->size()? int(1.5 * this->size()): 4);
    }
    ASSERT(this->d_end != this->d_capacity);
    new(this->d_end++) TYPE(value);
}

template <class TYPE>
void vector<TYPE>::push_back(bsls::Rvalue<TYPE> value) {
    if (this->d_end == this->d_capacity) {
        this->reserve(this->size()? int(1.5 * this->size()): 4);
    }
    new(this->d_end++) TYPE(bsls::RvalueUtil::move(value));
}

template <class TYPE>
void vector<TYPE>::reserve(int newCapacity) {
    if (this->capacity() < newCapacity) {
        if (this->d_begin) {
            vector<TYPE> tmp;
            tmp.reserve(newCapacity);
            this->copy(this->begin(), this->end(), &tmp);
            this->swap(tmp);
        }
        else {
            int size = int(sizeof(TYPE) * newCapacity);
            this->d_begin = static_cast<TYPE*>(operator new[](size));
            this->d_end = this->d_begin;
            this->d_capacity = this->d_begin + newCapacity;
        }
    }
}

template <class TYPE>
void vector<TYPE>::swap(TYPE*& a, TYPE*& b) {
    TYPE *tmp = a;
    a = b;
    b = tmp;
}

template <class TYPE>
void vector<TYPE>::swap(vector<TYPE>& other) {
    this->swap(this->d_begin, other.d_begin);
    this->swap(this->d_end, other.d_end);
    this->swap(this->d_capacity, other.d_capacity);
}

}  // close unnamed namespace

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;

    testStatus = 0;
    switch (test) {
      case 0:
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        vector<int> vector0;
        for (int i = 0; i != 5; ++i) {
            vector0.push_back(i);
        }
        for (int i = 0; i != vector0.size(); ++i) {
            ASSERT(vector0[i] == i);
        }

        vector<int> vector1(vector0);
        ASSERT(vector1.size() == vector0.size());
        for (int i = 0; i != vector1.size(); ++i) {
            ASSERT(vector1[i] == vector0[i]);
        }

        const int   *first = vector0.begin();
        vector<int>  vector2(bsls::RvalueUtil::move(vector0));
        ASSERT(first == vector2.begin());

        vector<vector<int> > vvector;
        vvector.push_back(vector2);                          // copy
        ASSERT(vvector.size() == 1);
        ASSERT(vvector[0].size() == vector2.size());
        ASSERT(vvector[0].begin() != first);
        for (int i = 0; i != 5; ++i) {
            ASSERT(vvector[0][i] == i);
        }

        vvector.push_back(bsls::RvalueUtil::move(vector2)); // move
        ASSERT(vvector.size() == 2);
        ASSERT(vvector[1].begin() == first);
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // RVALUE<TYPE> AND RVALUEUTIL FUNCTIONALITY
        //
        // Concerns:
        //: 1 Verify that an 'Rvalue<int>' can be created from an 'int' using
        //:   'RvalueUtil::move()' and that references obtained using the
        //:   implicit conversion to 'int&' or using 'RvalueUtil::access()'
        //:   refer to the original object.
        //: 2 Verify that an 'Rvalue<int>' can be moved using
        //:   'RvalueUtil::move()' and that the newly created 'Rvalue<int>'
        //:   references the original object.
        //: 3 Verify that a function can be called with an 'Rvalue<int>' and
        //:   that the argument stores a reference to the original object.
        //
        // Plan:
        //: 1 Define an 'int' object 'value' and obtain an 'Rvalue<int>' named
        //:   'rvalue0' using 'RvalueUtil::move(value)'. Then use an implicit
        //:   conversion from 'rvalue0' to 'int&' to initialize 'reference'
        //:   verify that '&value' and '&reference' are identical. Also verify
        //:   that '&value' and '&RvalueUtil::access(rvalue0)' are identical.
        //: 2 Create a new 'Rvalue<int>' named 'rvalue1' using
        //:   'RvalueUtil::move(rvalue0)' and verify that it references the
        //:   original object by comparing the address of 'value' and the
        //:   address of the result of 'RvalueUtil::access(rvalue1).
        //: 3 Call a function with the address of 'value' and
        //:   'RvalueUtil::move(value)'. From this function return the result
        //:   comparing the address of 'value' and the address of the result of
        //:   'RvalueUtil::access()' called on the second argument.
        //
        // Testing:
        //     RVALUE<TYPE> AND RVALUEUTIL FUNCTIONALITY
        // --------------------------------------------------------------------

        if (verbose) printf("\nRVALUE<TYPE> AND RVALUEUTIL FUNCTIONALITY"
                            "\n========================================\n");

        int                value(0);
        bsls::Rvalue<int>  rvalue0(bsls::RvalueUtil::move(value));
        int&               reference(rvalue0);
        ASSERT(&value == &reference);
        ASSERT(&value == &bsls::RvalueUtil::access(rvalue0));

        bsls::Rvalue<int> rvalue1(bsls::RvalueUtil::move(rvalue0));
        ASSERT(&value == &bsls::RvalueUtil::access(rvalue1));

        ASSERT(testFunctionCall(&value, bsls::RvalueUtil::move(value)));
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //: 1 Verify that all operations of 'Rvalue<TYPE>' and 'RvalueUtil' can
        //:   be used.
        //
        // Plan:
        //: 1 Use 'RvalueUtil::move()' to create an 'Rvalue<int>' and use the
        //:   the implicate conversion to 'int&' and 'RvalueUtil::access()' to
        //:   to obtain a reference to the original value.
        //
        // Testing:
        //     BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        int               value(0);
        bsls::Rvalue<int> rvalue(bsls::RvalueUtil::move(value));
        int&              reference(rvalue);
        int&              lvalue(bsls::RvalueUtil::access(rvalue));
        ASSERT(&reference == &lvalue);
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      } break;
    }

    return testStatus;
}

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
