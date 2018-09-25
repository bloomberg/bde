// bslmf_istransparentpredicate.t.cpp                                 -*-C++-*-
#include <bslmf_istransparentpredicate.h>

#include <bslmf_enableif.h>

#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>
#include <bsls_libraryfeatures.h>
#include <bsls_nameof.h>

#include <stdio.h>   // 'printf'
#include <stdlib.h>  // 'atoi'

using namespace BloombergLP;
using namespace bslmf;
using bsls::NameOf;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// The metafunction defined in this component checks for the presence of a
// member type.  To test, we ensure that the component gives the correct answer
// for various class types with the typedef and without, as well as non-class
// types.
//-----------------------------------------------------------------------------
// [1] FULL TEST
// [2] USAGE EXAMPLE
//-----------------------------------------------------------------------------

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
//                             USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Specifying Behavior Of Comparator
/// - - - - - - - - - - - - - - - - - - - - - -
// In this example, we demonstrate the use of 'IsTransparentPredicate' to
// determine different comparator's behavior.  Our goal is to create an
// overload of an associative container's method that participates in overload
// resolution only if the comparator is transparent, otherwise we fall back on
// a default behavior.
//
// First, we define simple container, 'Vector', that is used as a foundation
// for our associative container:
//..
    template <class TYPE>
    class Vector {
        // DATA
        const TYPE * d_begin_p;   // pointer to the beginning
        const TYPE * d_end_p;     // pointer to the end

      public:
        // CREATORS
        Vector(const TYPE *first, const TYPE *last)
            // Construct an object that references the specified 'first' and
            // 'last' items in the sequence.
        : d_begin_p(first)
        , d_end_p(last)
        {
        }

        // ACCESSORS
        const TYPE *begin() const
            // Return a reference providing non-modifiable access to the first
            // object in the underlying sequence.
        {
            return d_begin_p;
        }

        const TYPE *end() const
            // Return a reference providing non-modifiable access to the last
            // object in the underlying sequence.
        {
            return d_end_p;
        }
    };
//..
// Then we define simple type, 'String', that is used as a key.  Note that we
// have comparison operators that allow to compare both two 'String' objects
// and 'String' object with character sequence.
//..
    class String {
        // CLASS DATA
        static int  s_numObjectsCreated;  // total number of created instances

        // DATA
        const char *d_data_p;             // reference to a string

      public:
        // CLASS METHODS
        static int numObjectsCreated()
            // Return the total number of created instances of class 'String'.
        {
            return s_numObjectsCreated;
        }

        // CREATORS
        String()
            // Construct an empty string
        : d_data_p("")
        {
            ++s_numObjectsCreated;
        }

        String(const char *data)  // IMPLICIT
            // Construct a string that references the specified 'data'.  The
            // behavior is undefined unless 'data' points to a null-terminated
            // string.
        : d_data_p(data)
        {
            ++s_numObjectsCreated;
        }

        // FREE FUNCTIONS
        friend bool operator<(const String& lhs, const String& rhs)
            // Return 'true' if the value of the specified 'lhs' string is
            // lexicographically less than that of the specified 'rhs' string,
            // and 'false' otherwise.
        {
            const char *lhsData = lhs.d_data_p;
            const char *rhsData = rhs.d_data_p;
            while (true) {
                if (*lhsData < *rhsData) {
                    return true;                                      // RETURN
                }
                else if (*lhsData > *rhsData || *lhsData == '\0') {
                    return false;                                     // RETURN
                }
                ++lhsData;
                ++rhsData;
            }
        }

        friend bool operator<(const String& lhs, const char *rhs)
            // Return 'true' if the value of the specified 'lhs' string is
            // lexicographically less than the specified 'rhs' character
            // sequence, and 'false' otherwise.
        {
            const char *lhsData = lhs.d_data_p;
            while (true) {
                if (*lhsData < *rhs) {
                    return true;                                      // RETURN
                }
                else if (*lhsData > *rhs || *lhsData == '\0') {
                    return false;                                     // RETURN
                }
                ++lhsData;
                ++rhs;
            }
        }

        friend bool operator<(const char *lhs, const String& rhs)
            // Return 'true' if the specified 'lhs' character sequence is
            // lexicographically less than the value of the specified 'rhs'
            // string, and 'false' otherwise.
        {
            const char *rhsData = rhs.d_data_p;
            while (true) {
                if (*lhs < *rhsData) {
                    return true;                                      // RETURN
                }
                else if (*lhs > *rhsData || *lhs == '\0') {
                    return false;                                     // RETURN
                }
                ++lhs;
                ++rhsData;
            }
        }
    };

    int String::s_numObjectsCreated = 0;
//..
// Next we define our associative container.  Note that we use
// 'IsTransparentPredicate' for a method with 'enable_if' in the return type.
// This adds our function template into the function overload resolution set if
// and only if the comparison function object is considered transparent, using
// a technique known as "SFINAE".
//..
    template <class COMPARATOR>
    class FlatStringSet {

        // DATA
        COMPARATOR     d_comparator;  // comparison object
        Vector<String> d_data;        // stores the data

      public:
        // TYPES
        typedef const String * const_iterator;

        // CREATORS
        template <class INPUT_ITERATOR>
        FlatStringSet(INPUT_ITERATOR    first,
                      INPUT_ITERATOR    last,
                      const COMPARATOR& comparator = COMPARATOR());
            // Create a set, and insert each 'String' object in the sequence
            // starting at the specified 'first' element, and ending
            // immediately before the specified 'last' element, ignoring those
            // keys having a value equivalent to that which appears earlier in
            // the sequence.  Optionally specify a 'comparator' used to order
            // keys contained in this object.  If 'comparator' is not supplied,
            // a default-constructed object of the (template parameter) type
            // 'COMPARATOR' is used.  This operation has 'O[N]' complexity,
            // where 'N' is the number of elements between 'first' and 'last'.
            // The (template parameter) type 'INPUT_ITERATOR' shall meet the
            // requirements of an input iterator defined in the C++11 standard
            // [24.2.3] providing access to values of a type convertible to
            // 'String', and 'String' must be 'emplace-constructible' from '*i'
            // into this set, where 'i' is a dereferenceable iterator in the
            // range '[first .. last)'.  The behavior is undefined unless
            // 'first' and 'last' refer to a sequence of valid values where
            // 'first' is at a position at or before 'last', the range is
            // ordered according to 'COMPARATOR', and there are no duplicates
            // in the range.

        // ACCESSORS
        const_iterator begin() const;
            // Return an iterator providing non-modifiable access to the first
            // 'String' object in the ordered sequence of 'String' objects
            // maintained by this set, or the 'end' iterator if this set is
            // empty.

        const_iterator end() const;
            // Return an iterator providing non-modifiable access to the
            // past-the-end element in the ordered sequence of 'String' objects
            // maintained by this set.

        const_iterator find(const String& key) const
            // Return an iterator to the element that compares equal to the
            // specified 'key' if such an element exists, and an end iterator
            // otherwise.
            //
            // Note: implemented inline due to Sun CC compilation error.
        {
            for (const_iterator first = begin(); first != end(); ++first) {
                if (d_comparator(key, *first)) {
                    return end();                                     // RETURN
                }
                if (!d_comparator(*first, key)) {
                    return first;                                     // RETURN
                }
            }
            return end();
        }

        template <class KEY>
        typename bsl::enable_if<IsTransparentPredicate<COMPARATOR, KEY>::value,
                                const_iterator>::type
        find(const KEY& key) const
            // Return an iterator to the element that compares equal to the
            // specified 'key' if such an element exists, and an end iterator
            // otherwise.
            //
            // Note: implemented inline due to Sun CC compilation error.
        {
            for (const_iterator first = begin(); first != end(); ++first) {
                if (d_comparator(key, *first)) {
                    return end();                                     // RETURN
                }
                if (!d_comparator(*first, key)) {
                    return first;                                     // RETURN
                }
            }
            return end();
        }

        int size() const;
            // Return the number of elements in this container.
    };

    // CREATORS
    template <class COMPARATOR>
    template <class INPUT_ITERATOR>
    FlatStringSet<COMPARATOR>::FlatStringSet(INPUT_ITERATOR    first,
                                             INPUT_ITERATOR    last,
                                             const COMPARATOR& comparator)
    : d_comparator(comparator),
      d_data(first, last)
    {
    }

    // ACCESSORS
    template <class COMPARATOR>
    typename FlatStringSet<COMPARATOR>::const_iterator
    FlatStringSet<COMPARATOR>::begin() const
    {
        return d_data.begin();
    }

    template <class COMPARATOR>
    typename FlatStringSet<COMPARATOR>::const_iterator
    FlatStringSet<COMPARATOR>::end() const
    {
        return d_data.end();
    }

    template <class COMPARATOR>
    int FlatStringSet<COMPARATOR>::size() const
    {
        return static_cast<int>(end() - begin());
    }
//..
// Then we define two comparators.  These classes are completely identical
// except that one defines an 'is_transparent' type, and the other does not.
//..
    struct TransparentComp
        // This class can be used as a comparator for containers.  It has a
        // nested type 'is_transparent', so it is classified as transparent by
        // the 'bslmf::IsTransparentPredicate' metafunction and can be used for
        // heterogeneous comparison.
    {
        // TYPES
        typedef void is_transparent;

        // ACCESSORS
        template <class LHS, class RHS>
        bool operator()(const LHS& lhs, const RHS& rhs) const
            // Return 'true' if the specified 'lhs' is less than the specified
            // 'rhs' and 'false' otherwise.
        {
            return lhs < rhs;
        }
    };

    struct NonTransparentComp
        // This class can be used as a comparator for containers.  It has no
        // nested type 'is_transparent', so it is classified as
        // non-transparent by the 'bslmf::IsTransparentPredicate' metafunction
        // and can not be used for heterogeneous comparison.
    {
        template <class LHS, class RHS>
        bool operator()(const LHS& lhs, const RHS& rhs) const
            // Return 'true' if the specified 'lhs' is less than the specified
            // 'rhs' and 'false' otherwise.
        {
            return lhs < rhs;
        }
    };
//..

namespace {
//=============================================================================
//                  GLOBAL TYPEDEFS/CLASSES FOR TESTING
//-----------------------------------------------------------------------------

namespace e {
// Following two enums are placed to separate namespace to avoid ambiguity with
// 'is_transparent' struct, declared below.

enum EnumWithoutValue
    // This enumeration type should be classified as a non-transparent by the
    // 'bslmf::IsTransparentPredicate' metafunction.
{};

enum EnumWithValue
    // This enumeration type has 'is_transparent' value but still should be
    // classified as a non-transparent by the 'bslmf::IsTransparentPredicate'
    // metafunction.
{
    is_transparent
};

}  // close namespace e

struct DummyMemberType
    // This class is used to define pointer-to-member type.
{};

typedef int DummyMemberType::*PointerToMember;

typedef void (ConstAbdominableFunctionType   )() const;
typedef void (VolatileAbdominableFunctionType)() volatile;

union TransparentUnion
    // This union has a nested type 'is_transparent' and should be classified
    // as a transparent by the 'bslmf::IsTransparentPredicate' metafunction.
{
    // TYPES
    typedef void is_transparent;
};

union NonTransparentUnion
    // This union has not a nested type 'is_transparent' and should be
    // classified as a non-transparent by the 'bslmf::IsTransparentPredicate'
    // metafunction.
 {};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_FINAL
struct TransparentFinalType final
    // This final class has a nested type 'is_transparent' and should be
    // classified as a transparent by the 'bslmf::IsTransparentPredicate'
    // metafunction.
{
    // TYPES
    typedef void is_transparent;
};

struct NonTransparentFinalType final
    // This final class has not a nested type 'is_transparent' and should be
    // classified as a non-transparent by the 'bslmf::IsTransparentPredicate'
    // metafunction.
{};
#endif

struct IsTransparentMemberType
    // This class has a public member 'is_transparent' and should be classified
    // as a non-transparent by the 'bslmf::IsTransparentPredicate'
    // metafunction.
{
    // DATA
    // BDE_VERIFY pragma: push
    // BDE_VERIFY pragma: -MN02
    int is_transparent;  // A 'd_' prefix would defeat the purpose!
    // BDE_VERIFY pragma: pop
};

struct IsTransparentMethodType
    // This class has a public method 'is_transparent' and should be classified
    // as a non-transparent by the 'bslmf::IsTransparentPredicate'
    // metafunction.
{
    // MANIPULATORS
    void is_transparent()
        // The presence of this function should not indicate trtansparency.
    {
    }
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
template <class T>
struct IsTransparentAliasTemplateType
    // This class has an alias template 'is_transparent' and should be
    // classified as a transparent by the 'bslmf::IsTransparentPredicate'
    // metafunction.
{
    // TYPES
    using is_transparent = T;
};
#endif

struct IsTransparentNestedClassType
    // This class has a nested class 'is_transparent' and should be classified
    // as a transparent by the 'bslmf::IsTransparentPredicate' metafunction.
{
    // TYPES
    class is_transparent {};
};

struct IsTransparentNestedEnumType
    // This class has a nested enum 'is_transparent' and should be classified
    // as a transparent by the 'bslmf::IsTransparentPredicate' metafunction.
{
    // TYPES
    enum is_transparent {};
};

struct IsTransparentPrivateType
    // This class has a private nested type 'is_transparent' and should be
    // classified as a non-transparent by the 'bslmf::IsTransparentPredicate'
    // metafunction.
{
  private:
    // PRIVATE TYPES
    typedef void is_transparent;
};

struct IsTransparentProtectedType {
    // This class has a protected nested type 'is_transparent' and should be
    // classified as a non-transparent by the 'bslmf::IsTransparentPredicate'
    // metafunction.
  protected:
    // TYPES
    typedef void is_transparent;
};

struct is_transparent
    // This class has a name 'is_transparent' and should be classified as a
    // transparent by the 'bslmf::IsTransparentPredicate' metafunction.
{};

struct PublicChildOf_is_transparent : public is_transparent
    // This class is publicly derived from a class having a name
    // 'is_transparent' and should be classified as a transparent by the
    // 'bslmf::IsTransparentPredicate' metafunction.
{};

struct PrivateChildOf_is_transparent : private is_transparent
    // This class is privately derived from a class having a name
    // 'is_transparent' and should be classified as a non-transparent by the
    // 'bslmf::IsTransparentPredicate' metafunction.
{};

struct PublicChildOfTransparentType : public TransparentComp
    // This class is publicly derived from a transparent class and should be
    // classified as a transparent by the 'bslmf::IsTransparentPredicate'
    // metafunction.
{};

struct PrivateChildOfTransparentType : private TransparentComp
    // This class is privately derived from a transparent class and should be
    // classified as a non-transparent by the 'bslmf::IsTransparentPredicate'
    // metafunction.
{};

template <class TYPE>
struct PublicChildOfTransparentTemplateType : public TYPE
    // This class is publicly derived from (template parameter) type 'TYPE' and
    // should be classified as a transparent by the
    // 'bslmf::IsTransparentPredicate' metafunction if the type 'TYPE' is
    // transparent and non-transparent otherwise.
{};

// Following two types are defined to shorten records and preserve table format
// in the tests.
//
typedef PublicChildOfTransparentTemplateType<TransparentComp>    PCOTTTTC;
typedef PublicChildOfTransparentTemplateType<NonTransparentComp> PCOTTTNTC;

struct AnotherTransparentComp
    // This final class has a nested type 'is_transparent' and should be
    // classified as a transparent by the 'bslmf::IsTransparentPredicate'
    // metafunction.
{
    // TYPES
    typedef void is_transparent;
};

struct AmbiguousChildOfTransparentType : public TransparentComp
                                       , public AnotherTransparentComp
    // This class is ambiguously publicly derived from a two transparent
    // classes and should be classified as a non-transparent by the
    // 'bslmf::IsTransparentPredicate' metafunction.
{};

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

template <class TYPE>
void verifyTransparency()
    // Pass an assertion if the (template parameter) 'TYPE' is transparent and
    // fail otherwise.
{
    ASSERTV(NameOf<TYPE>(), (IsTransparentPredicate<TYPE, int>::value));
}

template <class TYPE>
void verifyNonTransparency()
    // Pass an assertion if the (template parameter) 'TYPE' is non-transparent
    // and fail otherwise.
{
    ASSERTV(NameOf<TYPE>(), (!IsTransparentPredicate<TYPE, int>::value));
}

// Some compilers get some transparency tests wrong.  For those, we invert the
// tests so that the test driver passes for now, but will fail in the future
// when the compilers are corrected.  We define a macro that sepcializes the
// 'verify' templates and asserts the opposite results.
#define InvertTransparencyTestImpl(TYPE)                                      \
    template <>                                                               \
    void verifyTransparency<TYPE>()                                           \
    {                                                                         \
        ASSERTV(NameOf<TYPE>(), (!IsTransparentPredicate<TYPE, int>::value)); \
    }
#define InvertNonTransparencyTestImpl(TYPE)                                   \
    template <>                                                               \
    void verifyNonTransparency<TYPE>()                                        \
    {                                                                         \
        ASSERTV(NameOf<TYPE>(), (IsTransparentPredicate<TYPE, int>::value));  \
    }

#define InvertTransparencyTest(TYPE)                                          \
    InvertTransparencyTestImpl(               TYPE)                           \
    InvertTransparencyTestImpl(const          TYPE)                           \
    InvertTransparencyTestImpl(volatile       TYPE)                           \
    InvertTransparencyTestImpl(const volatile TYPE)
#define InvertNonTransparencyTest(TYPE)                                       \
    InvertNonTransparencyTestImpl(               TYPE)                        \
    InvertNonTransparencyTestImpl(const          TYPE)                        \
    InvertNonTransparencyTestImpl(volatile       TYPE)                        \
    InvertNonTransparencyTestImpl(const volatile TYPE)

#if defined(BSLS_PLATFORM_CMP_SUN)
// Sun CC has an error whereby an identical typedef in two base classes is not
// an ambiguous name in the derived class.  This breaks the assumption upon
// which 'AmbiguousChildOfTransparentType' works.
InvertNonTransparencyTest(AmbiguousChildOfTransparentType)
#endif

#if defined(BSLS_PLATFORM_CMP_CLANG)
// Clang does not consider that a class named 'is_transparent' is transparent,
// It's not clear whether the injected name 'is_transparent::is_transparent'
// should pass the transparency test or not.
InvertTransparencyTest(is_transparent)
#endif

#if defined(BSLS_PLATFORM_CMP_MSVC)
// Microsoft regards an 'enum' is transparent when it has an 'is_transparent'
// enumerator.
InvertNonTransparencyTest(e::EnumWithValue)

// Versions before 2017 also find non-public 'is_transparent'.
#if BSLS_PLATFORM_CMP_VERSION < 1910
InvertNonTransparencyTest(IsTransparentPrivateType)
InvertNonTransparencyTest(IsTransparentProtectedType)
InvertNonTransparencyTest(PrivateChildOf_is_transparent)
InvertNonTransparencyTest(PrivateChildOfTransparentType)
#endif
#endif

}  // close unnamed namespace

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
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
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");
//..
// Next we create an array of 'String' objects and two 'FlatStringSet' objects,
// basing on this array and having different comparators, transparent and
// non-transparent.  Note that creation of 'FlatStringSet' object does not
// increase number of created 'String' objects, because it just holds pointers
// to these objects, but does not copy them.
//..
    int OBJECTS_NUMBER = String::numObjectsCreated();
    ASSERT(0 == OBJECTS_NUMBER);

    String data[] = { "1", "2", "3", "5" };
    enum { dataSize = sizeof data / sizeof *data };

    ASSERT(OBJECTS_NUMBER + 4 == String::numObjectsCreated());
    OBJECTS_NUMBER = String::numObjectsCreated();

    FlatStringSet<NonTransparentComp> nts(data, data + dataSize);
    FlatStringSet<   TransparentComp>  ts(data, data + dataSize);

    ASSERT(4                  == nts.size()                 );
    ASSERT(4                  ==  ts.size()                 );
    ASSERT(OBJECTS_NUMBER     == String::numObjectsCreated());
//..
// Then we call 'find' method of set, having non-transparent comparator.
// Explicit creation of temporary 'String' object predictably increases the
// number of created 'String' objects.  But using character sequence as a
// parameter for 'find' method increases it too.  Because comparison operator
// that accepts two 'String' objects is used instead of operator that accepts
// 'String' and char sequence, so 'String' constructor is called implicitly.
//..
    ASSERT(nts.begin() + 0    == nts.find(String("1"))      );
    ASSERT(nts.begin() + 1    == nts.find(String("2"))      );
    ASSERT(nts.begin() + 2    == nts.find(String("3"))      );
    ASSERT(nts.begin() + 3    == nts.find(String("5"))      );
    ASSERT(nts.end()          == nts.find(String("6"))      );

    ASSERT(OBJECTS_NUMBER + 5 == String::numObjectsCreated());
    OBJECTS_NUMBER = String::numObjectsCreated();

    ASSERT(nts.begin() + 0    == nts.find(       "1" )      );
    ASSERT(nts.begin() + 1    == nts.find(       "2" )      );
    ASSERT(nts.begin() + 2    == nts.find(       "3" )      );
    ASSERT(nts.begin() + 3    == nts.find(       "5" )      );
    ASSERT(nts.end()          == nts.find(       "6" )      );

    ASSERT(OBJECTS_NUMBER + 5 == String::numObjectsCreated());
    OBJECTS_NUMBER = String::numObjectsCreated();
//..
// Finally we call 'find' method of set, having transparent comparator.
// Explicit creation of temporary 'String' object still increases the number of
// created 'String' objects.  But using character sequence as a parameter for
// 'find' method does not.  Because comparison operator that accepts 'String'
// and char sequence is available and more appropriate then operator accepting
// two 'String' objects.  So there is no need for implicit constructor
// invocation.
//..
    ASSERT( ts.begin() + 0    ==  ts.find(String("1"))      );
    ASSERT( ts.begin() + 1    ==  ts.find(String("2"))      );
    ASSERT( ts.begin() + 2    ==  ts.find(String("3"))      );
    ASSERT( ts.begin() + 3    ==  ts.find(String("5"))      );
    ASSERT( ts.end()          ==  ts.find(String("6"))      );

    ASSERT(OBJECTS_NUMBER + 5 == String::numObjectsCreated());
    OBJECTS_NUMBER = String::numObjectsCreated();

    ASSERT( ts.begin() + 0    ==  ts.find(        "1" )     );
    ASSERT( ts.begin() + 1    ==  ts.find(        "2" )     );
    ASSERT( ts.begin() + 2    ==  ts.find(        "3" )     );
    ASSERT( ts.begin() + 3    ==  ts.find(        "5" )     );
    ASSERT( ts.end()          ==  ts.find(        "6" )     );

    ASSERT(OBJECTS_NUMBER     == String::numObjectsCreated());
//..
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // FULL TEST
        //
        // Concerns:
        //: 1 'bslmf::IsTransparentPredicate<TYPE>::value' is 'false' for
        //:   fundamental and void types.
        //:
        //: 2 'bslmf::IsTransparentPredicate<TYPE>::value' is 'false' for
        //:   fundamental, void, and enum types.
        //:
        //: 3 'bslmf::IsTransparentPredicate<TYPE>::value' is 'false' for
        //:   function types.
        //:
        //: 4 'bslmf::IsTransparentPredicate<TYPE>::value' is 'false' for
        //:   abominable function types.
        //:
        //: 5 'bslmf::IsTransparentPredicate<TYPE>::value' is 'false' for
        //:   references.
        //:
        //: 6 'bslmf::IsTransparentPredicate<TYPE>::value' is 'false' for
        //:   pointers.
        //:
        //: 7 'bslmf::IsTransparentPredicate<TYPE>::value' is 'false' for
        //:   pointer-to-members.
        //:
        //: 8 'bslmf::IsTransparentPredicate<TYPE>::value' is 'false' for
        //:   unions without the defined 'is_transparent' type.
        //:
        //: 9 'bslmf::IsTransparentPredicate<TYPE>::value' is 'true' for unions
        //:   without the defined 'is_transparent' type.
        //:
        //:10 'bslmf::IsTransparentPredicate<TYPE>::value' is 'false' for final
        //:   classes without the defined 'is_transparent' type. (C++11)
        //:
        //:11 'bslmf::IsTransparentPredicate<TYPE>::value' is 'true' for final
        //:   classes with the defined 'is_transparent' type. (C++11)
        //:
        //:12 'bslmf::IsTransparentPredicate<TYPE>::value' is equal for
        //:   cv-qualified and non-cv-qualified versions of all of the types
        //:   from C-1..12 (apart from references and functions).
        //:
        //:13 'bslmf::IsTransparentPredicate<TYPE>::value' is 'true' for a
        //:   class with a public type named 'is_transparent'.
        //:
        //:14 'bslmf::IsTransparentPredicate<TYPE>::value' is 'false' for a
        //:   class with a protected type named 'is_transparent'.
        //:
        //:15 'bslmf::IsTransparentPredicate<TYPE>::value' is 'false' for a
        //:   class with a private type named 'is_transparent'.
        //:
        //:16 'bslmf::IsTransparentPredicate<TYPE>::value' is 'false' for a
        //:   class with a data member named 'is_transparent'.
        //:
        //:17 'bslmf::IsTransparentPredicate<TYPE>::value' is 'false' for a
        //:   class with a member function named 'is_transparent'.
        //:
        //:18 'bslmf::IsTransparentPredicate<TYPE>::value' is 'true' for a
        //:   class with an alias-template named is_transparent (C++11).
        //:
        //:19 'bslmf::IsTransparentPredicate<TYPE>::value' is 'true' for a
        //:   class with a nested class named 'is_transparent'.
        //:
        //:20 'bslmf::IsTransparentPredicate<TYPE>::value' is 'true' for a
        //:   class with a nested enum type named 'is_transparent'.
        //:
        //:21 'bslmf::IsTransparentPredicate<TYPE>::value' is 'true' for a
        //:   class named 'is_transparent'.
        //:
        //:22 'bslmf::IsTransparentPredicate<TYPE>::value' is 'true' for a
        //:   class publicly derived from a class named 'is_transparent'.
        //:
        //:23 'bslmf::IsTransparentPredicate<TYPE>::value' is 'false' for a
        //:   class privately derived from a class named 'is_transparent'.
        //:
        //:24 'bslmf::IsTransparentPredicate<TYPE>::value' is 'true' for a
        //:   class publicly derived from a transparent class (i.e. that
        //:   produces 'true_type').
        //:
        //:25 'bslmf::IsTransparentPredicate<TYPE>::value' is 'false' for a
        //:   class privately derived from a transparent class (i.e. that
        //:   produces 'true_type').
        //:
        //:26 'bslmf::IsTransparentPredicate<TYPE>::value' is 'true' for a
        //:   class template publicly derived from the template parameter,
        //:   instantiated for a transparent class.
        //:
        //:27 'bslmf::IsTransparentPredicate<TYPE>::value' is 'false' for a
        //:   class template publicly derived from the template parameter,
        //:   instantiated for a non-transparent class.
        //:
        //:28 'bslmf::IsTransparentPredicate<TYPE>::value' is 'false' for a
        //:   class ambiguously derived from transparent classes.
        //:
        // Plan:
        //: 1 Instantiate 'bslmf::IsTransparentPredicate<TYPE>::value' for a
        //:   types from C-1..28 and verify the result.  (C-1..28)
        //
        // Testing:
        //   FULL TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nFULL TEST"
                            "\n=========\n");

        // Testing non-transparent types.

        verifyNonTransparency<          void                             >();
        verifyNonTransparency<          bool                             >();
        verifyNonTransparency<          char                             >();
        verifyNonTransparency<          int                              >();
        verifyNonTransparency<          long                             >();
        verifyNonTransparency<          NonTransparentComp               >();
        verifyNonTransparency<          e::EnumWithoutValue              >();
        verifyNonTransparency<          e::EnumWithValue                 >();
        verifyNonTransparency<          ConstAbdominableFunctionType     >();
        verifyNonTransparency<          VolatileAbdominableFunctionType  >();
        verifyNonTransparency<          PointerToMember                  >();
        verifyNonTransparency<          NonTransparentUnion              >();
#ifdef BSLS_COMPILERFEATURES_SUPPORT_FINAL
        verifyNonTransparency<          NonTransparentFinalType          >();
#endif

        // Testing const non-transparent types.

        verifyNonTransparency< const    bool                             >();
        verifyNonTransparency< const    char                             >();
        verifyNonTransparency< const    int                              >();
        verifyNonTransparency< const    long                             >();
        verifyNonTransparency< const    NonTransparentComp               >();
        verifyNonTransparency< const    e::EnumWithoutValue              >();
        verifyNonTransparency< const    e::EnumWithValue                 >();
        verifyNonTransparency< const    PointerToMember                  >();
        verifyNonTransparency< const    NonTransparentUnion              >();
#ifdef BSLS_COMPILERFEATURES_SUPPORT_FINAL
        verifyNonTransparency< const    NonTransparentFinalType          >();
#endif

        // Testing volatile non-transparent types.

        verifyNonTransparency< volatile bool                             >();
        verifyNonTransparency< volatile char                             >();
        verifyNonTransparency< volatile int                              >();
        verifyNonTransparency< volatile long                             >();
        verifyNonTransparency< volatile NonTransparentComp               >();
        verifyNonTransparency< volatile e::EnumWithoutValue              >();
        verifyNonTransparency< volatile e::EnumWithValue                 >();
        verifyNonTransparency< volatile PointerToMember                  >();
        verifyNonTransparency< volatile NonTransparentUnion              >();
#ifdef BSLS_COMPILERFEATURES_SUPPORT_FINAL
        verifyNonTransparency< volatile NonTransparentFinalType          >();
#endif

        // Testing pointers to const non-transparent types.

        verifyNonTransparency<          void                           * >();
        verifyNonTransparency<          bool                           * >();
        verifyNonTransparency<          char                           * >();
        verifyNonTransparency<          int                            * >();
        verifyNonTransparency<          long                           * >();
        verifyNonTransparency<          NonTransparentComp             * >();
        verifyNonTransparency<          e::EnumWithoutValue            * >();
        verifyNonTransparency<          e::EnumWithValue               * >();
        verifyNonTransparency<          PointerToMember                * >();
        verifyNonTransparency<          NonTransparentUnion            * >();
#ifdef BSLS_COMPILERFEATURES_SUPPORT_FINAL
        verifyNonTransparency<          NonTransparentFinalType        * >();
#endif

        // Testing pointers to const non-transparent types.

        verifyNonTransparency< const    void                           * >();
        verifyNonTransparency< const    bool                           * >();
        verifyNonTransparency< const    char                           * >();
        verifyNonTransparency< const    int                            * >();
        verifyNonTransparency< const    long                           * >();
        verifyNonTransparency< const    NonTransparentComp             * >();
        verifyNonTransparency< const    PointerToMember                * >();
        verifyNonTransparency< const    NonTransparentUnion            * >();
#ifdef BSLS_COMPILERFEATURES_SUPPORT_FINAL
        verifyNonTransparency< const    NonTransparentFinalType        * >();
#endif

        // Testing pointers to volatile non-transparent types.

        verifyNonTransparency< volatile void                           * >();
        verifyNonTransparency< volatile bool                           * >();
        verifyNonTransparency< volatile char                           * >();
        verifyNonTransparency< volatile int                            * >();
        verifyNonTransparency< volatile long                           * >();
        verifyNonTransparency< volatile NonTransparentComp             * >();
        verifyNonTransparency< volatile PointerToMember                * >();
        verifyNonTransparency< volatile NonTransparentUnion            * >();
#ifdef BSLS_COMPILERFEATURES_SUPPORT_FINAL
        verifyNonTransparency< volatile NonTransparentFinalType        * >();
#endif

        // Testing references to non-transparent types.

        verifyNonTransparency<          bool                           & >();
        verifyNonTransparency<          char                           & >();
        verifyNonTransparency<          int                            & >();
        verifyNonTransparency<          long                           & >();
        verifyNonTransparency<          NonTransparentComp             & >();
        verifyNonTransparency<          e::EnumWithoutValue            & >();
        verifyNonTransparency<          e::EnumWithValue               & >();
        verifyNonTransparency<          PointerToMember                & >();
        verifyNonTransparency<          NonTransparentUnion            & >();
#ifdef BSLS_COMPILERFEATURES_SUPPORT_FINAL
        verifyNonTransparency<          NonTransparentFinalType        & >();
#endif

        // Testing references to const non-transparent types.

        verifyNonTransparency< const    bool                           & >();
        verifyNonTransparency< const    char                           & >();
        verifyNonTransparency< const    int                            & >();
        verifyNonTransparency< const    long                           & >();
        verifyNonTransparency< const    NonTransparentComp             & >();
        verifyNonTransparency< const    PointerToMember                & >();
        verifyNonTransparency< const    NonTransparentUnion            & >();
#ifdef BSLS_COMPILERFEATURES_SUPPORT_FINAL
        verifyNonTransparency< const    NonTransparentFinalType        & >();
#endif

        // Testing references to volatile non-transparent types.

        verifyNonTransparency< volatile bool                           & >();
        verifyNonTransparency< volatile char                           & >();
        verifyNonTransparency< volatile int                            & >();
        verifyNonTransparency< volatile long                           & >();
        verifyNonTransparency< volatile NonTransparentComp             & >();
        verifyNonTransparency< volatile PointerToMember                & >();
        verifyNonTransparency< volatile NonTransparentUnion            & >();
#ifdef BSLS_COMPILERFEATURES_SUPPORT_FINAL
        verifyNonTransparency< volatile NonTransparentFinalType        & >();
#endif

        // Testing special non-transparent types.

        verifyNonTransparency<          IsTransparentMemberType          >();
        verifyNonTransparency<          IsTransparentMethodType          >();
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        verifyNonTransparency<          IsTransparentPrivateType         >();
        verifyNonTransparency<          IsTransparentProtectedType       >();
        verifyNonTransparency<          PrivateChildOf_is_transparent    >();
        verifyNonTransparency<          PrivateChildOfTransparentType    >();
#endif
        verifyNonTransparency<          AmbiguousChildOfTransparentType  >();
        verifyNonTransparency<          PCOTTTNTC                        >();

        // Testing const special non-transparent types.

        verifyNonTransparency< const    IsTransparentMemberType          >();
        verifyNonTransparency< const    IsTransparentMethodType          >();
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        verifyNonTransparency< const    IsTransparentPrivateType         >();
        verifyNonTransparency< const    IsTransparentProtectedType       >();
        verifyNonTransparency< const    PrivateChildOf_is_transparent    >();
        verifyNonTransparency< const    PrivateChildOfTransparentType    >();
#endif
        verifyNonTransparency< const    AmbiguousChildOfTransparentType  >();
        verifyNonTransparency< const    PCOTTTNTC                        >();

        // Testing volatile special non-transparent types.

        verifyNonTransparency< volatile IsTransparentMemberType          >();
        verifyNonTransparency< volatile IsTransparentMethodType          >();
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        verifyNonTransparency< volatile IsTransparentPrivateType         >();
        verifyNonTransparency< volatile IsTransparentProtectedType       >();
        verifyNonTransparency< volatile PrivateChildOf_is_transparent    >();
        verifyNonTransparency< volatile PrivateChildOfTransparentType    >();
#endif
        verifyNonTransparency< volatile AmbiguousChildOfTransparentType  >();
        verifyNonTransparency< volatile PCOTTTNTC                        >();

        // Testing transparent types.

        verifyTransparency<          TransparentComp                     >();
        verifyTransparency<          TransparentUnion                    >();
#ifdef BSLS_COMPILERFEATURES_SUPPORT_FINAL
        verifyTransparency<          TransparentFinalType                >();
#endif
#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
        verifyTransparency<          IsTransparentAliasTemplateType<int> >();
#endif
        verifyTransparency<          IsTransparentNestedClassType        >();
        verifyTransparency<          IsTransparentNestedEnumType         >();
        verifyTransparency<          is_transparent                      >();
        verifyTransparency<          PublicChildOf_is_transparent        >();
        verifyTransparency<          PublicChildOfTransparentType        >();
        verifyTransparency<          PCOTTTTC                            >();

        // Testing const transparent types.

        verifyTransparency< const    TransparentComp                     >();
        verifyTransparency< const    TransparentUnion                    >();
#ifdef BSLS_COMPILERFEATURES_SUPPORT_FINAL
        verifyTransparency< const    TransparentFinalType                >();
#endif
#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
        verifyTransparency< const    IsTransparentAliasTemplateType<int> >();
#endif
        verifyTransparency< const    IsTransparentNestedClassType        >();
        verifyTransparency< const    IsTransparentNestedEnumType         >();
        verifyTransparency< const    is_transparent                      >();
        verifyTransparency< const    PublicChildOf_is_transparent        >();
        verifyTransparency< const    PublicChildOfTransparentType        >();
        verifyTransparency< const    PCOTTTTC                            >();

        // Testing volatile transparent types.

        verifyTransparency< volatile TransparentComp                     >();
        verifyTransparency< volatile TransparentUnion                    >();
#ifdef BSLS_COMPILERFEATURES_SUPPORT_FINAL
        verifyTransparency< volatile TransparentFinalType                >();
#endif
#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
        verifyTransparency< volatile IsTransparentAliasTemplateType<int> >();
#endif
        verifyTransparency< volatile IsTransparentNestedClassType        >();
        verifyTransparency< volatile IsTransparentNestedEnumType         >();
        verifyTransparency< volatile is_transparent                      >();
        verifyTransparency< volatile PublicChildOf_is_transparent        >();
        verifyTransparency< volatile PublicChildOfTransparentType        >();
        verifyTransparency< volatile PCOTTTTC                            >();
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
// Copyright 2018 Bloomberg Finance L.P.
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
