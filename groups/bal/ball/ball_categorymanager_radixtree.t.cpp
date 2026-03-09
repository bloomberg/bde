// ball_categorymanager_radixtree.t.cpp                               -*-C++-*-
#include <ball_categorymanager_radixtree.h>

#include <bsl_functional.h>                           // for testing only

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>

#include <bslma_testallocator.h>

#include <bdlb_numericparseutil.h>

#include <bslim_testutil.h>

#include <bsl_iostream.h>
#include <bsl_cstddef.h>
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_vector.h>

using namespace BloombergLP;

using bsl::cout;
using bsl::cerr;
using bsl::endl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// The component under test implements a radix tree data structure with string
// keys and generic values.  The tests use the standard BDE test driver pattern
// with numbered test cases, breathing test, and usage example.
//
// Primary Manipulators:
// - `pair<bool, t_VALUE&> emplace(const bsl::string_view&, Args&&...)`
// - `bool erase(const bsl::string_view&)` - `void clear()`
//
// Basic Accessors:
// - `optional<reference_wrapper<const t_VALUE>> find(...) const`
// - `bool contains(const bsl::string_view&) const` - `size_type size() const`
// - `bool empty() const`
//
// This test driver tests each of these operations in the standard case
// numbering scheme, as well as testing memory allocation and the container's
// value-semantic behavior.
// ----------------------------------------------------------------------------
// CREATORS
// [ 1] BREATHING TEST
// [ 2] CategoryManager_RadixTree();
// [ 2] CategoryManager_RadixTree(const allocator_type&);
// [ 7] CategoryManager_RadixTree(other, allocator);
// [16] CategoryManager_RadixTree(MovableRef original);
// [16] CategoryManager_RadixTree(MovableRef original, const allocator_type&);
// [ 2] ~CategoryManager_RadixTree();
//
// MANIPULATORS
// [ 2] pair<bool, t_VALUE&> emplace(const bsl::string_view&, Args&&...);
// [ 2] bool erase(const bsl::string_view&);
// [11] size_type erasePrefix(const bsl::string_view&);
// [13] eraseChildrenOfPrefix(const bsl::string_view&);
// [ 2] void clear();
// [ 8] void swap(CategoryManager_RadixTree&);
// [ 9] CategoryManager_RadixTree& operator=(rhs);
// [16] CategoryManager_RadixTree& operator=(MovableRef rhs);
// [ 3] optional<reference_wrapper<t_VALUE>> find(...);
// [10] forEach(const t_FUNCTOR&);
// [14] forEachPrefix(const bsl::string_view&, const t_FUNCTOR&);
//
// ACCESSORS
// [ 3] optional<reference_wrapper<const t_VALUE>> find(...) const;
// [ 3] bool contains(const bsl::string_view&) const;
// [10] void forEach(const t_FUNCTOR&) const;
// [ 3] size_type size() const;
// [ 3] bool empty() const;
// [12] string findLongestCommonPrefix(OptValueRef*, const string_view&)
// [12] string findLongestCommonPrefix(OptValueCRef*, const string_view&) const
// [ 3] allocator_type get_allocator() const;
// [14] forEachPrefix(const bsl::string_view&, const t_FUNCTOR&) const;
// [ 5] bsl::ostream& printNodes(bsl::ostream&, int, int) const;
//
// FREE OPERATORS
// [ 6] bool operator==(lhs, rhs);
// [ 6] bool operator!=(lhs, rhs);
//
// FREE FUNCTIONS
// [ 8] void swap(CategoryManager_RadixTree&, CategoryManager_RadixTree&);
//
// SPECIAL TESTS
// [ 4] PREFIX COMPRESSION
// [15] ALLOCATOR PROPAGATION TO VALUE
// [17] USAGE EXAMPLE

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT BSLIM_TESTUTIL_ASSERT
#define ASSERTV BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q BSLIM_TESTUTIL_Q    // Quote identifier literally.
#define P BSLIM_TESTUTIL_P    // Print identifier and value.
#define P_ BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_ BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_ BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef ball::CategoryManager_RadixTree<int>         IntTree;
typedef ball::CategoryManager_RadixTree<bsl::string> StringTree;

// ============================================================================
//                  GLOBAL VARIABLES FOR TESTING
// ----------------------------------------------------------------------------

bool verbose;
bool veryVerbose;
bool veryVeryVerbose;
bool veryVeryVeryVerbose;

// ============================================================================
//                  GENERATOR FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

/// Generate a `CategoryManager_RadixTree<int>` from a spec string.
///
/// The spec string format:
///   - Each entry is: "key:value"
///   - Entries are separated by whitespace or comma
///   - Example: "a:1 ab:2 abc:3" or "foo:10,bar:20,baz:30"
///
/// Return 0 on success, or a positive value indicating the position of the
/// first parsing error.
///
/// Note: Only `int` and `bsl::string` value types are supported.
int gg(ball::CategoryManager_RadixTree<int> *object,
       const char           *spec,
       bool                  verboseFlag = true)
{
    bsl::string_view specStr(spec);

    // Parse entries separated by whitespace or comma
    bsl::size_t pos = 0;

    while (pos < specStr.size()) {
        // Skip whitespace and commas
        while (pos < specStr.size() &&
               (specStr[pos] == ' '  || specStr[pos] == '\t' ||
                specStr[pos] == '\n' || specStr[pos] == ',')) {
            ++pos;
        }

        if (pos >= specStr.size()) {
            break;                                                     // BREAK
        }

        // Find colon separator
        const bsl::size_t colonPos = specStr.find(':', pos);
        if (colonPos == bsl::string::npos) {
            if (verboseFlag) {
                cout << "Error: Missing ':' in entry at position "
                     << pos << endl;
            }
            return static_cast<int>(pos + 1);
        }

        // Extract key
        const bsl::string_view key = specStr.substr(pos, colonPos - pos);

        // Find end of value (next whitespace, comma, or end)
        pos = colonPos + 1;
        bsl::size_t valueEnd = pos;
        while (valueEnd < specStr.size() &&
               specStr[valueEnd] != ' '  &&
               specStr[valueEnd] != '\t' &&
               specStr[valueEnd] != '\n' &&
               specStr[valueEnd] != ',') {
            ++valueEnd;
        }

        const bsl::string_view valueStr = specStr.substr(pos, valueEnd - pos);

        // Convert value string to int
        int value;
        if (bdlb::NumericParseUtil::parseInt(&value, valueStr, 10) != 0) {
            if (verboseFlag) {
                cout << "Error: Cannot convert value '" << valueStr
                     << "' to `int` at position " << pos << endl;
            }
            return static_cast<int>(pos + 1);
        }

        // Insert into tree
        object->emplace(key, value);

        pos = valueEnd;
    }

    return 0;
}

/// Overload for `CategoryManager_RadixTree<bsl::string>` values.
///
/// For string values, the spec format supports:
///   - Unquoted strings (no whitespace): "key:value"
///   - Quoted strings (with whitespace): "key:\"value with spaces\""
///   - Escaped quotes: "key:\"value with \\\"quotes\\\"\""
int gg(ball::CategoryManager_RadixTree<bsl::string> *object,
                              const char                   *spec,
                              bool                          verboseFlag = true)
{
    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
    bsl::string_view     specStr(spec);

    // Parse entries separated by whitespace or comma
    bsl::size_t pos = 0;

    while (pos < specStr.size()) {
        // Skip whitespace and commas
        while (pos < specStr.size() &&
               (specStr[pos] == ' '  || specStr[pos] == '\t' ||
                specStr[pos] == '\n' || specStr[pos] == ',')) {
            ++pos;
        }

        if (pos >= specStr.size()) {
            break;
        }

        // Find colon separator
        const bsl::size_t colonPos = specStr.find(':', pos);
        if (colonPos == bsl::string::npos) {
            if (verboseFlag) {
                cout << "Error: Missing ':' in entry at position "
                     << pos << endl;
            }
            return static_cast<int>(pos + 1);
        }

        // Extract key
        const bsl::string_view key = specStr.substr(pos, colonPos - pos);

        // Parse value (handle quoted strings)
        pos = colonPos + 1;
        bsl::string value(&scratch);

        if (pos < specStr.size() && specStr[pos] == '"') {
            // Quoted string value
            ++pos;  // Skip opening quote
            bsl::size_t valueEnd = pos;
            while (valueEnd < specStr.size()) {
                if (specStr[valueEnd] == '\\' &&
                    valueEnd + 1 < specStr.size()) {
                    // Escaped character
                    ++valueEnd;  // Skip backslash
                    ++valueEnd;  // Skip escaped char
                }
                else if (specStr[valueEnd] == '"') {
                    // Found closing quote
                    break;
                }
                else {
                    ++valueEnd;
                }
            }

            if (valueEnd >= specStr.size()) {
                if (verboseFlag) {
                    cout << "Error: Unterminated quoted string at position "
                         << colonPos + 1 << endl;
                }
                return static_cast<int>(colonPos + 2);
            }

            // Extract value and process escape sequences
            const bsl::string_view rawValue = specStr.substr(pos,
                                                             valueEnd - pos);
            for (bsl::size_t i = 0; i < rawValue.size(); ++i) {
                if (rawValue[i] == '\\' && i + 1 < rawValue.size()) {
                    ++i;  // Skip backslash
                }
                value.push_back(rawValue[i]);
            }

            pos = valueEnd + 1;  // Skip closing quote
        }
        else {
            // Unquoted string value
            bsl::size_t valueEnd = pos;
            while (valueEnd < specStr.size() &&
                   specStr[valueEnd] != ' '  &&
                   specStr[valueEnd] != '\t' &&
                   specStr[valueEnd] != '\n' &&
                   specStr[valueEnd] != ',') {
                ++valueEnd;
            }

            value = specStr.substr(pos, valueEnd - pos);
            pos   = valueEnd;
        }

        // Insert into tree
        object->emplace(key, value);
    }

    return 0;
}

// ============================================================================
//                  GLOBAL HELPER FUNCTORS FOR TESTING
// ----------------------------------------------------------------------------

                    // =====================
                    // struct EntryCollector
                    // =====================

/// Functor to collect entries into a vector
struct EntryCollector {
    typedef bsl::vector<bsl::pair<bsl::string, int> > Entries;

    Entries *d_entries_p;

    EntryCollector(Entries *entries) : d_entries_p(entries) {}

    void operator()(const bsl::string_view& key, int value) const
    {
        d_entries_p->push_back(bsl::make_pair(key, value));
    }

    const Entries& entries() const { return *d_entries_p; }
};

                    // =====================
                    // struct CounterFunctor
                    // =====================

/// Functor to count applications
struct CounterFunctor {
    int *d_count_p;

    CounterFunctor(int *count) : d_count_p(count) { }

    void operator()(const bsl::string_view&, int) const { ++(*d_count_p); }
};

                       // ======================
                       // struct MutatingFunctor
                       // ======================

/// Functor to mutate values in the tree for `forEach[Prefix]` mutation test
struct MutatingFunctor {
    void operator()(const bsl::string_view&, int& value) const
    {
        value += 1000;
    }
};

                        // ===================
                        // struct AllocTracker
                        // ===================

/// AllocTracker: minimal allocator-aware type for propagation test
struct AllocTracker {
    BSLMF_NESTED_TRAIT_DECLARATION(AllocTracker, bslma::UsesBslmaAllocator);

    const bslma::Allocator *d_allocator_p;

    explicit AllocTracker(bslma::Allocator *basicAllocator = 0)
    : d_allocator_p(basicAllocator) { }

    AllocTracker(const AllocTracker& other)
    : d_allocator_p(other.d_allocator_p) { }

    AllocTracker(const AllocTracker&, bslma::Allocator *ba)
    : d_allocator_p(bslma::Default::allocator(ba)) { }

    AllocTracker& operator=(const AllocTracker& rhs)
    {
        d_allocator_p = rhs.d_allocator_p;
        return *this;
    }

    const bslma::Allocator *allocator() const { return d_allocator_p; }
};

                      // ====================
                      // class AllocAwareType
                      // ====================

/// Test type that is allocator-aware with no default constructor.
/// Uses `bsl::string` members to test allocator propagation.
class AllocAwareType {
    bsl::string d_first;
    bsl::string d_second;

  public:
    BSLMF_NESTED_TRAIT_DECLARATION(AllocAwareType, bslma::UsesBslmaAllocator);

    // CREATORS

    /// Create an `AllocAwareType` with the specified `first` and `second`
    /// values, using the optionally specified `basicAllocator` to supply
    /// memory. If `basicAllocator` is 0, the currently installed default
    /// allocator is used.
    AllocAwareType(const bsl::string_view&  first,
                   const bsl::string_view&  second,
                   bslma::Allocator        *basicAllocator = 0)
    : d_first(first, basicAllocator)
    , d_second(second, basicAllocator)
    {
    }

    /// Create an `AllocAwareType` having the value of the specified
    /// `original` object, using the optionally specified `basicAllocator`
    /// to supply memory. If `basicAllocator` is 0, the currently installed
    /// default allocator is used.
    AllocAwareType(const AllocAwareType&  original,
                   bslma::Allocator      *basicAllocator = 0)
    : d_first(original.d_first, basicAllocator)
    , d_second(original.d_second, basicAllocator)
    {
    }

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object and
    /// return a reference providing modifiable access to this object.
    AllocAwareType& operator=(const AllocAwareType& rhs)
    {
        d_first  = rhs.d_first;
        d_second = rhs.d_second;
        return *this;
    }

    // ACCESSORS

    /// Return the first value.
    const bsl::string& first() const { return d_first; }

    /// Return the second value.
    const bsl::string& second() const { return d_second; }

    /// Return the allocator used by this object to supply memory.
    bslma::Allocator *allocator() const
    {
        return d_first.get_allocator().mechanism();
    }
};

                       // ==================
                       // class NonAllocType
                       // ==================

/// Test type that is not allocator-aware with no default constructor.
/// Uses integer members.
class NonAllocType {
    int d_first;
    int d_second;
    int d_third;

  public:
    // CREATORS

    /// Create a `NonAllocType` with the specified `first`, `second`, and
    /// `third` values.
    NonAllocType(int first, int second, int third)
    : d_first(first)
    , d_second(second)
    , d_third(third)
    {
    }

    /// Create a `NonAllocType` having the value of the specified
    /// `original` object.
    NonAllocType(const NonAllocType& original)
    : d_first(original.d_first)
    , d_second(original.d_second)
    , d_third(original.d_third)
    {
    }

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object and
    /// return a reference providing modifiable access to this object.
    NonAllocType& operator=(const NonAllocType& rhs)
    {
        d_first  = rhs.d_first;
        d_second = rhs.d_second;
        d_third  = rhs.d_third;
        return *this;
    }

    // ACCESSORS

    /// Return the first value.
    int first() const { return d_first; }

    /// Return the second value.
    int second() const { return d_second; }

    /// Return the third value.
    int third() const { return d_third; }
};

// ============================================================================
//                  ADDITIONAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef ball::CategoryManager_RadixTree<AllocAwareType> AllocAwareTree;
typedef ball::CategoryManager_RadixTree<NonAllocType>   NonAllocTree;

void noop()
{
}

void noop2(const bsl::string_view&, int)
{
}

// ============================================================================
//                              USAGE EXAMPLE
// ----------------------------------------------------------------------------

struct Printer {
    void operator()(const bsl::string_view& key,
                    const bsl::string&      value) const {
        bsl::cout << key << ": " << value << bsl::endl;
    }
};

struct PrefixCollector {
    bsl::vector<bsl::string> *d_vec_p;
    PrefixCollector(bsl::vector<bsl::string> *vec)
    : d_vec_p(vec)
    {
    }

    void operator()(const bsl::string_view& key, const bsl::string&) const {
        d_vec_p->push_back(bsl::string(key));
    }
};

struct SuffixAppender {
    void operator()(const bsl::string_view&, bsl::string& value) const {
        value += "!";
    }
};

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int  test                = argc > 1 ? atoi(argv[1]) : 0;
    verbose                  = argc > 2;
    veryVerbose              = argc > 3;
    veryVeryVerbose          = argc > 4;
    veryVeryVeryVerbose      = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // Set up a default allocator for all test cases
    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard guard(&da);

    switch (test) {  case 0:
      case 17: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        // 1. The usage example provided in the component header file compiles,
        //    links, and runs as shown.
        //
        // Plan:
        // 1. Incorporate usage example from header into test driver, remove
        //    leading comment characters, and replace `assert` with `ASSERT`.
        //    (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "USAGE EXAMPLE\n"
                             "=============\n";
        // clang-format off

///Example 1: Basic Usage
/// - - - - - - - - - - -
// Suppose we want to store a mapping of words to their definitions.  Using
// `ball::CategoryManager_RadixTree` allows us to efficiently store many words
// that share common prefixes.
//
// First, we create a radix tree:
        ball::CategoryManager_RadixTree<bsl::string> dictionary;
//
// Then, we insert some words and their definitions:
        dictionary.emplace("car",  "a road vehicle with four wheels");
        dictionary.emplace("card", "a piece of stiff paper");
        dictionary.emplace("care", "the provision of what is needed");
        dictionary.emplace("cat",  "a small domesticated carnivorous mammal");
//
// Next, we can look up definitions:
        StringTree::OptValueRef definition =
                                                        dictionary.find("car");
        ASSERT(definition.has_value());
        ASSERT(definition->get() == "a road vehicle with four wheels");
//
// We can also check if a key exists:
        ASSERT(dictionary.contains("card"));
        ASSERT(!dictionary.contains("dog"));
//
// We can find the longest prefix of a key that exists in the tree:
        ball::CategoryManager_RadixTree<bsl::string>::OptValueRef optValue;
        bsl::string_view prefix = dictionary.findLongestCommonPrefix(&optValue,
                                                                     "cards");
        ASSERT(prefix == "card");
        ASSERT(optValue.has_value()
            && optValue->get() == "a piece of stiff paper");
        prefix = dictionary.findLongestCommonPrefix(&optValue, "carpet");
        ASSERT(prefix == "car");
        ASSERT(optValue.has_value()
            && optValue->get() == "a road vehicle with four wheels");
        prefix = dictionary.findLongestCommonPrefix(&optValue, "dog");
        ASSERT(prefix == "");
        ASSERT(!optValue.has_value());
//
// We can run a functor for all entries:
        if (veryVerbose) {
            dictionary.forEach(Printer());
        }
//
// We can run a functor for all entries with a given prefix:
// See `PrefixCollector` definition at namespace level for C++03 compatibility
        bsl::vector<bsl::string> found;
        PrefixCollector          collector(&found);
        dictionary.forEachPrefix("car", collector);
        ASSERT(found.size() == 3);
        bool foundCar  = false;
        bool foundCard = false;
        bool foundCare = false;
        for (bsl::size_t i = 0; i < found.size(); ++i) {
            if (found[i] == "car") {
                foundCar = true;
            } else if (found[i] == "card") {
                foundCard = true;
            } else if (found[i] == "care") {
                foundCare = true;
            }
        }
        ASSERT(foundCar);
        ASSERT(foundCard);
        ASSERT(foundCare);
//
// We can also mutate values for all entries with a given prefix:
// See `SuffixAppender` at namespace level for C++03 compatibility
    dictionary.forEachPrefix("car", SuffixAppender());
        ASSERT(dictionary.find("car")->get() ==
                                           "a road vehicle with four wheels!");
        ASSERT(dictionary.find("card")->get() == "a piece of stiff paper!");
        ASSERT(dictionary.find("care")->get() ==
                                           "the provision of what is needed!");
//
// Finally, we can remove entries:
        dictionary.erase("card");
        ASSERT(!dictionary.contains("card"));
        // clang-format on
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // MOVE CONSTRUCTOR AND MOVE ASSIGNMENT OPERATOR
        //
        // Concerns:
        // 1. The move constructor transfers the contents and leaves the source
        //    in a valid, empty state.
        // 2. The move assignment operator transfers the contents and leaves
        //    the source in a valid, empty state.
        // 3. Allocator propagation follows BDE semantics: if allocators are
        //    the same, resources are moved; if different, a deep copy is
        //     performed.
        //
        // Plan:
        // 1. Create a tree, move-construct another from it, and verify the new
        //    tree has the values and the old is empty.
        // 2. Move-assign between trees and verify the result and source state.
        // 3. Test allocator propagation by moving between trees with different
        //    allocators.
        //
        // Testing:
        //   CategoryManager_RadixTree(MovableRef, const allocator_type&);
        //   CategoryManager_RadixTree& operator=(MovableRef);
        // --------------------------------------------------------------------

        if (verbose) cout << "MOVE CONSTRUCTOR AND MOVE ASSIGNMENT OPERATOR\n"
                             "=============================================\n";

        // Move constructor, same allocator
        {
            bslma::TestAllocator ta("test", veryVeryVeryVerbose);
            IntTree mX(&ta);
            mX.emplace("foo", 1);
            mX.emplace("bar", 2);

            IntTree mY(bslmf::MovableRefUtil::move(mX));
            ASSERT(mY.size() == 2);
            ASSERT(mY.contains("foo"));
            ASSERT(mY.contains("bar"));
            ASSERT(mX.empty());
        }

        // Move constructor, same allocator
        {
            bslma::TestAllocator ta("test", veryVeryVeryVerbose);
            IntTree mX(&ta);
            mX.emplace("foo", 1);
            mX.emplace("bar", 2);

            IntTree mY(bslmf::MovableRefUtil::move(mX), &ta);
            ASSERT(mY.size() == 2);
            ASSERT(mY.contains("foo"));
            ASSERT(mY.contains("bar"));
            ASSERT(mX.empty());
        }

        // Move assignment, same allocator
        {
            bslma::TestAllocator ta("test", veryVeryVeryVerbose);
            IntTree mX(&ta);
            mX.emplace("a", 10);
            mX.emplace("b", 20);

            IntTree mY(&ta);
            mY.emplace("z", 99);

            mY = bslmf::MovableRefUtil::move(mX);
            ASSERT(mY.size() == 2);
            ASSERT(mY.contains("a"));
            ASSERT(mY.contains("b"));
            ASSERT(!mY.contains("z"));
            ASSERT(mX.empty());
        }

        // Move constructor, different allocator (should deep copy)
        {
            bslma::TestAllocator ta1("ta1", veryVeryVeryVerbose);
            bslma::TestAllocator ta2("ta2", veryVeryVeryVerbose);
            IntTree mX(&ta1);
            mX.emplace("foo", 1);
            mX.emplace("bar", 2);

            IntTree mY(bslmf::MovableRefUtil::move(mX), &ta2);
            ASSERT(mY.size() == 2);
            ASSERT(mY.contains("foo"));
            ASSERT(mY.contains("bar"));
            // mX is not required to be empty if allocators differ,
            // but should be valid
            ASSERT(mX.get_allocator() == bsl::allocator<int>(&ta1));
            ASSERT(mY.get_allocator() == bsl::allocator<int>(&ta2));
            ASSERT(ta2.numBlocksInUse() > 0);
        }

        // Move assignment, different allocator (should deep copy)
        {
            bslma::TestAllocator ta1("ta1", veryVeryVeryVerbose);
            bslma::TestAllocator ta2("ta2", veryVeryVeryVerbose);
            IntTree mX(&ta1);
            mX.emplace("foo", 1);
            mX.emplace("bar", 2);

            IntTree mY(&ta2);
            mY.emplace("z", 99);

            mY = bslmf::MovableRefUtil::move(mX);
            ASSERT(mY.size() == 2);
            ASSERT(mY.contains("foo"));
            ASSERT(mY.contains("bar"));
            ASSERT(!mY.contains("z"));
            ASSERT(mY.get_allocator() == bsl::allocator<int>(&ta2));
            ASSERT(ta2.numBlocksInUse() > 0);
        }

#ifdef BDE_BUILD_TARGET_EXC
        {  // Exception testing of move assignment operator with different
           // allocators (requires deep copy)

            bslma::TestAllocator sa("source", veryVeryVeryVerbose);
            bslma::TestAllocator xa("exc",    veryVeryVeryVerbose);

            StringTree source(&sa);
            source.emplace("apple",       "apple_value");
            source.emplace("applesauce",  "applesauce_value");
            source.emplace("application", "application_value");
            source.emplace("apply",       "apply_value");
            source.emplace("banana",      "banana_value");
            source.emplace("band",        "band_value");

            ASSERTV(source.size(),       source.size()       == 6);
            ASSERTV(source.countNodes(), source.countNodes() == 9);

            StringTree target(&xa);
            target.emplace("old_key", "old_value");

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(xa) {
                // Create a temporary source for each iteration since move
                // assignment may leave it in an unspecified state
                StringTree tempSource(source, &sa);

                target = bslmf::MovableRefUtil::move(tempSource);

                ASSERTV(target.size(),       target.size()       == 6);
                ASSERTV(target.countNodes(), target.countNodes() == 9);

                ASSERT(target.get_allocator().mechanism() == &xa);

                ASSERT(target.contains("apple"));
                ASSERT(target.contains("applesauce"));
                ASSERT(target.contains("application"));
                ASSERT(target.contains("apply"));
                ASSERT(target.contains("banana"));
                ASSERT(target.contains("band"));
                ASSERT(!target.contains("old_key"));

                StringTree::OptValueRef result = target.find("apple");
                ASSERT(result && result->get() == "apple_value");
                result = target.find("applesauce");
                ASSERT(result && result->get() == "applesauce_value");
                result = target.find("application");
                ASSERT(result && result->get() == "application_value");
                result = target.find("apply");
                ASSERT(result && result->get() == "apply_value");
                result = target.find("banana");
                ASSERT(result && result->get() == "banana_value");
                result = target.find("band");
                ASSERT(result && result->get() == "band_value");
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        }
#endif  // BDE_BUILD_TARGET_EXC
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // ALLOCATOR PROPAGATION TO VALUE
        //
        // Concerns:
        // 1. Values stored in the `CategoryManager_RadixTree` must be
        //    constructed using the supplied object allocator.
        // 2. This must work for allocator-aware types (those with the
        //    `bslma::UsesBslmaAllocator` trait).
        // 3. Types that use `bsl::allocator_arg` style allocation must also
        //    pass the allocator argument properly.
        //
        // Plan:
        // 1. Define a minimal allocator-aware value type with an accessor for
        //    the allocator pointer.
        // 2. Create a `CategoryManager_RadixTree` using a
        //    `bslma::TestAllocator` as the object allocator.
        // 3. Insert values using `emplace` and verify, via the accessor of the
        //    allocator-aware value, that each value was constructed with the
        //    supplied object allocator.
        // 4. Confirm that the object allocator has allocations.
        // 5. Confirm that the default allocator has no active allocations.
        // 6. Use `bsl::function` as value to test `bsl::allocator_arg` style
        //    allocator passing.
        //
        // Testing:
        //   ALLOCATOR PROPAGATION TO VALUE
        // --------------------------------------------------------------------

        if (verbose) cout << "ALLOCATOR PROPAGATION TO VALUE\n"
                             "==============================\n";
        {
            typedef ball::CategoryManager_RadixTree<AllocTracker> TrackerTree;

            bslma::TestAllocator ta("object", veryVeryVeryVerbose);

            TrackerTree tree(&ta);
            const TrackerTree::EmplaceResult r1 = tree.emplace("foo");
            const TrackerTree::EmplaceResult r2 = tree.emplace("bar");
            const TrackerTree::EmplaceResult r3 = tree.emplace("baz");

            ASSERT(r1.second.get().allocator() == &ta);
            ASSERT(r2.second.get().allocator() == &ta);
            ASSERT(r3.second.get().allocator() == &ta);
            ASSERT(ta.numBlocksInUse() > 0);
            ASSERTV(da.numBlocksInUse(), da.numBlocksInUse() == 0);
        }

        {
            typedef ball::CategoryManager_RadixTree<bsl::function<void()> >
                                                                      FuncTree;

            bslma::TestAllocator ta("object", veryVeryVeryVerbose);

            FuncTree tree(&ta);
            const FuncTree::EmplaceResult r1 = tree.emplace("foo");
            const FuncTree::EmplaceResult r2 = tree.emplace("bar", &noop);
            ASSERT(r1.second.get().get_allocator().mechanism() == &ta);
            ASSERT(r2.second.get().get_allocator().mechanism() == &ta);
            ASSERT(ta.numBlocksInUse() > 0);
            ASSERTV(da.numBlocksInUse(), da.numBlocksInUse() == 0);
        }
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // FOREACH PREFIX (CONST AND NON-CONST)
        //
        // Concerns:
        // 1. The const version of `forEachPrefix` applies the functor to all
        //    entries with the given prefix, but does not allow mutation.
        // 2. The non-const version of `forEachPrefix` applies the functor to
        //    all entries with the given prefix and allows mutation of values.
        // 3. The functor receives the correct keys and values.
        // 4. Specifying a prefix argument that does not exist results in no
        //    calls to the functor.
        // 5. Specifying an empty prefix calls the functor on all entries.
        //
        // Plan:
        // 1. Create a tree with known entries and shared prefixes.
        // 2. Use the `const` `forEachPrefix` to collect all entries with a
        //    prefix.
        // 3. Use the non-const `forEachPrefix` to mutate values with a prefix
        //    and verify mutation.
        // 4. Test with a prefix that does not exist.
        // 5. Test with an empty prefix
        //
        // Testing:
        //   forEachPrefix(const bsl::string_view&, const t_FUNCTOR&) const;
        //   forEachPrefix(const bsl::string_view&, const t_FUNCTOR&);
        // --------------------------------------------------------------------

        if (verbose) cout << "FOREACH PREFIX\n"
                             "==============\n";

        typedef ball::CategoryManager_RadixTree<int> TreeType;

        bslma::TestAllocator ta("test",    veryVeryVeryVerbose);
        bslma::TestAllocator sa("scratch", veryVeryVeryVerbose);

        TreeType mX(&ta);

        // Insert a deep and wide set of keys
        const char* fooKeys[] = {
            "foo",
            "foobar",
            "foobaz",
            "food",
            "foom",
            "foomoo",
            "foomoozle",
            "foomoozlebar",
            "foomoozlebat",
            "foomoozlebatman",
            "foomoozlebatmobile",
            "foomoozlebatcave",
            "foomoozlebatwing",
            "foomoozlebatgirl",
            "foomoozlebatboy",
            "foomoozlebatdog",
            "foomoozlebatcat",
            "foomoozlebatfish",
            "foomoozlebatfox",
            "foomoozlebatfrog",
            "foomoozlebatfalcon",
            "foomoozlebatfinch",
            "foomoozlebatferret",
            "foomoozlebatfawn",
            "foomoozlebatfire",
            "foomoozlebatfairy",
            "foomoozlebatfruit",
            "foomoozlebatfungi"
        };
        const bsl::size_t numFooKeys = sizeof fooKeys / sizeof *fooKeys;

        for (bsl::size_t i = 0; i < numFooKeys; ++i) {
            const int value = static_cast<int>(i + 1); // values: 1..numFooKeys
            mX.emplace(fooKeys[i], value);
        }
        // Add some non-foo keys
        const char* otherKeys[] = {
            "bar",
            "baz",
            "bark",
            "barn",
            "barkley",
            "barker",
            "bazaar",
            "bazooka"
        };
        const bsl::size_t numOtherKeys = sizeof otherKeys / sizeof *otherKeys;

        for (bsl::size_t i = 0; i < numOtherKeys; ++i) {
            const int value = static_cast<int>(i + 100);
            mX.emplace(otherKeys[i], value);
        }

        // 1. Test const functor collects correct entries for a deep prefix
        {
            const TreeType& X = mX;
            EntryCollector::Entries entries(&sa);
            EntryCollector          functor(&entries);
            // should match all "foomoozlebatf*"
            bsl::size_t count = X.forEachPrefix("foomoozlebatf", functor);
            // Should apply functor to what's listed below
            const char* expected[] = {
                "foomoozlebatfish",
                "foomoozlebatfox",
                "foomoozlebatfrog",
                "foomoozlebatfalcon",
                "foomoozlebatfinch",
                "foomoozlebatferret",
                "foomoozlebatfawn",
                "foomoozlebatfire",
                "foomoozlebatfairy",
                "foomoozlebatfruit",
                "foomoozlebatfungi"
            };
            const bsl::size_t numExpected = sizeof expected / sizeof *expected;

            ASSERT(entries.size() == numExpected);
            ASSERT(count == numExpected);
            for (bsl::size_t i = 0; i < numExpected; ++i) {
                bool found = false;
                for (size_t j = 0; j < entries.size(); ++j) {
                    if (entries[j].first == expected[i]) {
                        found = true;
                        break;                                         // BREAK
                    }
                }
                ASSERTV(expected[i], found);
            }
        }

        // 2. Test non-const functor mutates values for a wide prefix
        {
            // should match all "foomoozlebat*"
            bsl::size_t count = mX.forEachPrefix("foomoozlebat",
                                                 MutatingFunctor());
            // All keys starting with "foomoozlebat" should have +1000
            bsl::size_t expectedCount = 0;
            for (bsl::size_t i = 0; i < numFooKeys; ++i) {
                bsl::string key(fooKeys[i]);
                if (key.find("foomoozlebat") == 0) {
                    ++expectedCount;
                    ASSERT(mX.find(key).has_value());
                    const int val = mX.find(key).value().get();
                    ASSERT(val >= 1001 &&
                           val <= 1000 + static_cast<int>(numFooKeys));
                }
            }
            ASSERT(count == expectedCount);
            // Others unchanged
            for (bsl::size_t i = 0; i < numFooKeys; ++i) {
                bsl::string key(fooKeys[i]);
                if (key.find("foomoozlebat") != 0) {
                    ASSERT(mX.find(key).has_value());
                    const int val = mX.find(key).value().get();
                    ASSERT(val == static_cast<int>(i + 1));
                }
            }
            for (bsl::size_t i = 0; i < numOtherKeys; ++i) {
                bsl::string key(otherKeys[i]);
                ASSERT(mX.find(key).has_value());
                const int val = mX.find(key).value().get();
                ASSERT(val == static_cast<int>(100 + i));
            }
        }

        // 3. Test prefix that does not exist
        {
            const TreeType& X = mX;

            EntryCollector::Entries entries(&sa);
            EntryCollector          functor(&entries);

            const bsl::size_t count = X.forEachPrefix("notfound", functor);

            ASSERT(entries.empty());
            ASSERT(count == 0);
        }

        // 4. Test empty prefix (should apply functor to all entries)
        {
            const TreeType& X = mX;

            EntryCollector::Entries entries(&sa);
            EntryCollector          functor(&entries);

            const bsl::size_t count = X.forEachPrefix("", functor);

            // Should apply to all fooKeys and otherKeys
            ASSERT(entries.size() == numFooKeys + numOtherKeys);
            ASSERT(count == numFooKeys + numOtherKeys);

            // Check all keys are present
            for (bsl::size_t i = 0; i < numFooKeys; ++i) {
                bool found = false;
                for (size_t j = 0; j < entries.size(); ++j) {
                    if (entries[j].first == fooKeys[i]) {
                        found = true;
                        break;                                         // BREAK
                    }
                }
                ASSERTV(fooKeys[i], found);
            }
            for (bsl::size_t i = 0; i < numOtherKeys; ++i) {
                bool found = false;
                for (size_t j = 0; j < entries.size(); ++j) {
                    if (entries[j].first == otherKeys[i]) {
                        found = true;
                        break;                                         // BREAK
                    }
                }
                ASSERTV(otherKeys[i], found);
            }
        }

        // REGRESSION: partial keys of node not found by `forEach`
        {
            TreeType tree(&ta);
            tree.emplace("E.M.P",   0);
            tree.emplace("E.M.Q",   1);
            tree.emplace("E.G.M.A", 2);
            tree.emplace("E.G.M.B", 3);

            const bsl::size_t numRun = tree.forEachPrefix("E.G.", noop2);
            ASSERTV(numRun, 2 == numRun);
        }

        // Test forEachPrefix empty prefix, tree with only empty key
        {
            if (veryVerbose) {
                cout << "\tEmpty prefix, tree with only empty key\n";
            }

            TreeType tree(&ta);
            tree.emplace("", 99);

            int            count   = 0;
            CounterFunctor counter(&count);

            const bsl::size_t numRun = tree.forEachPrefix("", counter);
            ASSERT(numRun == 1);
            ASSERT(count == 1);
        }

        // Test forEachPrefix non-empty prefix, tree with empty key
        {
            if (veryVerbose) {
                cout << "\tNon-empty prefix, tree with only empty key\n";
            }

            TreeType tree(&ta);
            tree.emplace("", 99);

            int            count   = 0;
            CounterFunctor counter(&count);

            const bsl::size_t numRun = tree.forEachPrefix("anything", counter);
            ASSERT(numRun == 0);
            ASSERT(count == 0);
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // ERASE CHILDREN OF PREFIX
        //
        // Concerns:
        // 1. `eraseChildrenOfPrefix` erases all children of the prefix node
        //    but not the node itself.
        // 2. The correct number of entries is returned and the tree size is
        //    updated.
        // 3. Entries not under the prefix are preserved.
        // 4. If the prefix does not exist, nothing is erased.
        // 5. If the prefix node has no children, nothing is erased.
        //
        // Plan:
        // 1. Create a tree with a variety of keys and shared prefixes.
        // 2. Call `eraseChildrenOfPrefix` with various prefixes and verify:
        //    a. The correct entries are removed.
        //    b. The correct count is returned.
        //    c. The tree size is correctly updated.
        //    d. Remaining entries are accessible.
        // 3. Test edge cases: non-existent prefix, prefix node with no
        //    children, prefix node with value.
        //
        // Testing:
        //   size_type eraseChildrenOfPrefix(const bsl::string_view&);
        // --------------------------------------------------------------------

        if (verbose) cout << "ERASE CHILDREN OF PREFIX\n"
                             "========================\n";

        {
            if (verbose)
                 cout << "\tTest erasing children of a prefix with children\n";

            bslma::TestAllocator ta("test", veryVeryVeryVerbose);
            IntTree mX(&ta);

            mX.emplace("foo",    1);
            mX.emplace("foobar", 2);
            mX.emplace("foobaz", 3);
            mX.emplace("food",   4);
            mX.emplace("bar",    5);

            ASSERT(mX.size() == 5);

            // Erase children of "foo" (should erase foobar, foobaz, food)
            const bsl::size_t removed = mX.eraseChildrenOfPrefix("foo");
            ASSERT(removed == 3);
            ASSERT(mX.size() == 2);
            ASSERT(mX.contains("foo"));
            ASSERT(!mX.contains("foobar"));
            ASSERT(!mX.contains("foobaz"));
            ASSERT(!mX.contains("food"));
            ASSERT(mX.contains("bar"));
        }

        {
            if (verbose)
              cout << "\tTest erasing children of a prefix with no children\n";

            bslma::TestAllocator ta("test", veryVerbose);
            IntTree mX(&ta);

            mX.emplace("foo", 1);
            mX.emplace("bar", 2);

            ASSERT(mX.size() == 2);

            // "foo" has no children
            const bsl::size_t removed = mX.eraseChildrenOfPrefix("foo");
            ASSERT(removed == 0);
            ASSERT(mX.size() == 2);
            ASSERT(mX.contains("foo"));
            ASSERT(mX.contains("bar"));
        }

        {
            if (verbose)
                  cout << "\tTest erasing children of a non-existent prefix\n";

            bslma::TestAllocator ta("test", veryVerbose);
            IntTree mX(&ta);

            mX.emplace("foo", 1);
            mX.emplace("bar", 2);

            ASSERT(mX.size() == 2);

            const bsl::size_t removed = mX.eraseChildrenOfPrefix("baz");
            ASSERT(removed == 0);
            ASSERT(mX.size() == 2);
            ASSERT(mX.contains("foo"));
            ASSERT(mX.contains("bar"));
        }

        {
            if (verbose)
                cout << "\tTest erasing children of the root (empty prefix)\n";

            bslma::TestAllocator ta("test", veryVerbose);
            IntTree mX(&ta);

            mX.emplace("foo", 1);
            mX.emplace("bar", 2);
            mX.emplace("baz", 3);

            ASSERT(mX.size() == 3);

            const bsl::size_t removed = mX.eraseChildrenOfPrefix("");
            ASSERT(removed == 3);
            ASSERT(mX.size() == 0);
            ASSERT(!mX.contains("foo"));
            ASSERT(!mX.contains("bar"));
            ASSERT(!mX.contains("baz"));
        }
        {
            if (verbose) cout << "\tTest erasing children of a prefix with "
                                             "children (deep and wide tree)\n";

            bslma::TestAllocator ta("test", veryVerbose);
            IntTree mX(&ta);

            // Add more nodes for depth and width
            mX.emplace("foo",                 1);
            mX.emplace("foobar",              2);
            mX.emplace("foobaz",              3);
            mX.emplace("food",                4);
            mX.emplace("foom",                5);
            mX.emplace("foomoo",              6);
            mX.emplace("foomoozle",           7);
            mX.emplace("foomoozlebar",        8);
            mX.emplace("foomoozlebat",        9);
            mX.emplace("foomoozlebatman",    10);
            mX.emplace("foomoozlebatmobile", 11);
            mX.emplace("foomoozlebatcave",   12);
            mX.emplace("foomoozlebatwing",   13);
            mX.emplace("foomoozlebatgirl",   14);
            mX.emplace("foomoozlebatboy",    15);
            mX.emplace("foomoozlebatdog",    16);
            mX.emplace("foomoozlebatcat",    17);
            mX.emplace("foomoozlebatfish",   18);
            mX.emplace("foomoozlebatfox",    19);
            mX.emplace("foomoozlebatfrog",   20);
            mX.emplace("foomoozlebatfalcon", 21);
            mX.emplace("foomoozlebatfinch",  22);
            mX.emplace("foomoozlebatferret", 23);
            mX.emplace("foomoozlebatfawn",   24);
            mX.emplace("foomoozlebatfire",   25);
            mX.emplace("foomoozlebatfairy",  26);
            mX.emplace("foomoozlebatfruit",  27);
            mX.emplace("foomoozlebatfungi",  28);
            mX.emplace("bar",                29);
            mX.emplace("baz",                30);
            mX.emplace("bark",               31);
            mX.emplace("barn",               32);
            mX.emplace("barkley",            33);
            mX.emplace("barker",             34);
            mX.emplace("bazaar",             35);
            mX.emplace("bazooka",            36);

            ASSERT(mX.size() == 36);

            // Erase children of "foo" (should erase all under "foo" except
            // "foo" itself)
            const bsl::size_t removed = mX.eraseChildrenOfPrefix("foo");
            ASSERT(removed == 27);
            ASSERT(mX.size() == 9);
            ASSERT(mX.contains("foo"));
            ASSERT(mX.contains("bar"));
            ASSERT(mX.contains("baz"));
            ASSERT(mX.contains("bark"));
            ASSERT(mX.contains("barn"));
            ASSERT(mX.contains("barkley"));
            ASSERT(mX.contains("barker"));
            ASSERT(mX.contains("bazaar"));
            ASSERT(mX.contains("bazooka"));
            // Check that all foo* children are gone
            ASSERT(!mX.contains("foobar"));
            ASSERT(!mX.contains("foobaz"));
            ASSERT(!mX.contains("food"));
            ASSERT(!mX.contains("foom"));
            ASSERT(!mX.contains("foomoo"));
            ASSERT(!mX.contains("foomoozle"));
            ASSERT(!mX.contains("foomoozlebar"));
            ASSERT(!mX.contains("foomoozlebat"));
            ASSERT(!mX.contains("foomoozlebatman"));
            ASSERT(!mX.contains("foomoozlebatmobile"));
            ASSERT(!mX.contains("foomoozlebatcave"));
            ASSERT(!mX.contains("foomoozlebatwing"));
            ASSERT(!mX.contains("foomoozlebatgirl"));
            ASSERT(!mX.contains("foomoozlebatboy"));
            ASSERT(!mX.contains("foomoozlebatdog"));
            ASSERT(!mX.contains("foomoozlebatcat"));
            ASSERT(!mX.contains("foomoozlebatfish"));
            ASSERT(!mX.contains("foomoozlebatfox"));
            ASSERT(!mX.contains("foomoozlebatfrog"));
            ASSERT(!mX.contains("foomoozlebatfalcon"));
            ASSERT(!mX.contains("foomoozlebatfinch"));
            ASSERT(!mX.contains("foomoozlebatferret"));
            ASSERT(!mX.contains("foomoozlebatfawn"));
            ASSERT(!mX.contains("foomoozlebatfire"));
            ASSERT(!mX.contains("foomoozlebatfairy"));
            ASSERT(!mX.contains("foomoozlebatfruit"));
            ASSERT(!mX.contains("foomoozlebatfungi"));
        }

#ifdef BDE_BUILD_TARGET_EXC
        {  // Exception testing of `eraseChildrenOfPrefix`.

            bslma::TestAllocator xa("exc", veryVeryVeryVerbose);

            StringTree tree(&xa);
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(xa) {
                // Rebuild tree in each iteration
                tree.emplace("foo",    "foo_value");
                tree.emplace("foob",   "foob_value");
                tree.emplace("foobar", "foobar_value");
                tree.emplace("foobaz", "foobaz_value");
                tree.emplace("food",   "food_value");
                tree.emplace("bar",    "bar_value");

                ASSERTV(tree.size(), tree.size() == 6);

                // Erase children of "foob" (should erase foobar, foobaz)
                bsl::size_t removed = tree.eraseChildrenOfPrefix("foob");
                ASSERTV(removed,           removed           == 2);
                ASSERTV(tree.size(),       tree.size()       == 4);
                ASSERTV(tree.countNodes(), tree.countNodes() == 5);
                // Root + "foo", "food", "bar" nodes

                ASSERT( tree.contains("foo"));
                ASSERT( tree.contains("foob"));
                ASSERT(!tree.contains("foobar"));
                ASSERT(!tree.contains("foobaz"));
                ASSERT( tree.contains("food"));
                ASSERT( tree.contains("bar"));

                // Erase children of "foo" (should erase foob, food)
                removed = tree.eraseChildrenOfPrefix("foo");
                ASSERTV(removed,           removed           == 2);
                ASSERTV(tree.size(),       tree.size()       == 2);
                ASSERTV(tree.countNodes(), tree.countNodes() == 3);
                // Root + "foo", "bar" nodes

                ASSERT( tree.contains("foo"));
                ASSERT(!tree.contains("foob"));
                ASSERT(!tree.contains("foobar"));
                ASSERT(!tree.contains("foobaz"));
                ASSERT(!tree.contains("food"));
                ASSERT( tree.contains("bar"));
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        }

        // Test erasing children of root (empty prefix)
        {
            if (veryVerbose) {
                cout << "\tErase children of root (empty prefix)\n";
            }

            bslma::TestAllocator ta("test", veryVeryVeryVerbose);
            StringTree           mX(&ta);

            mX.emplace("", "root");
            mX.emplace("apple", "apple_value");
            mX.emplace("banana", "banana_value");

            ASSERT(mX.size() == 3);

            // Erase all children of root (keep root value)
            const bsl::size_t removed = mX.eraseChildrenOfPrefix("");
            ASSERT(removed == 2);
            ASSERT(mX.size() == 1);
            ASSERT(mX.contains(""));
            ASSERT(!mX.contains("apple"));
            ASSERT(!mX.contains("banana"));

            const StringTree::OptValueRef result = mX.find("");
            ASSERT(result && result->get() == "root");
        }

        // Test erasing from tree with only root value (empty key)
        {
            if (veryVerbose) {
                cout << "\tTree with only root value\n";
            }

            bslma::TestAllocator ta("test", veryVeryVeryVerbose);
            IntTree              mX(&ta);

            mX.emplace("", 123);

            ASSERT(mX.size() == 1);

            // Try to erase children of empty key (no children exist)
            bsl::size_t removed = mX.eraseChildrenOfPrefix("");
            ASSERT(removed == 0);
            ASSERT(mX.size() == 1);
            ASSERT(mX.contains(""));
        }
#endif  // BDE_BUILD_TARGET_EXC
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // LONGEST COMMON PREFIX
        //
        // Concerns:
        // 1. The function returns the longest prefix that is an actual key
        //    (has a value).
        // 2. Partial prefixes without a value are not returned.
        // 3. The function returns an empty string if no prefix matches.
        // 4. The function works for empty trees and for keys not present.
        //
        // Plan:
        // 1. Insert multiple keys with shared prefixes and test various
        //    queries.
        // 2. Test edge cases: empty tree, no match, exact match, partial
        //    match.
        //
        // Testing:
        //   string findLongestCommonPrefix(OptValueRef&, const string_view&)
        //   string findLongestCommonPrefix(OptValueCRef&,
        //                                  const string_view&) const
        // --------------------------------------------------------------------

        if (verbose) cout << "LONGEST COMMON PREFIX\n"
                             "=====================\n";

        bslma::TestAllocator ta("test", veryVeryVeryVerbose);
        IntTree mX(&ta);

        // Empty tree
        IntTree::OptValueRef optVal;
        ASSERT(mX.findLongestCommonPrefix(&optVal, "anything").empty());
        ASSERT(!optVal.has_value());
        ASSERT(mX.findLongestCommonPrefix(&optVal, "").empty());
        ASSERT(!optVal.has_value());

        // Insert keys
        mX.emplace("car",          1);
        mX.emplace("card",         2);
        mX.emplace("care",         3);
        mX.emplace("cat",          4);
        mX.emplace("catastrophes", 5);
        mX.emplace("dog",          6);
        mX.emplace("do",           7);

        // Test `const` version
        {
            const IntTree& X = mX;
            IntTree::OptValueCRef ocVal;
            ASSERT(X.findLongestCommonPrefix(&ocVal, "car")         == "car" );
            ASSERT(ocVal.has_value() && ocVal->get() == 1);
            ASSERT(X.findLongestCommonPrefix(&ocVal, "card")        == "card");
            ASSERT(ocVal.has_value() && ocVal->get() == 2);
            ASSERT(X.findLongestCommonPrefix(&ocVal, "careful")     == "care");
            ASSERT(ocVal.has_value() && ocVal->get() == 3);
            ASSERT(X.findLongestCommonPrefix(&ocVal, "catastrophe") == "cat" );
            ASSERT(ocVal.has_value() && ocVal->get() == 4);
            ASSERT(X.findLongestCommonPrefix(&ocVal, "dogma")       == "dog" );
            ASSERT(ocVal.has_value() && ocVal->get() == 6);
            ASSERT(X.findLongestCommonPrefix(&ocVal, "doable")      == "do"  );
            ASSERT(ocVal.has_value() && ocVal->get() == 7);
            ASSERT(X.findLongestCommonPrefix(&ocVal, "zebra").empty());
            ASSERT(!ocVal.has_value());
            ASSERT(X.findLongestCommonPrefix(&ocVal, "c").empty());
            ASSERT(!ocVal.has_value());
            ASSERT(X.findLongestCommonPrefix(&ocVal, "caz").empty());
            ASSERT(!ocVal.has_value());
            ASSERT(X.findLongestCommonPrefix(&ocVal, "d") == "");
            ASSERT(!ocVal.has_value());
            ASSERT(X.findLongestCommonPrefix(&ocVal, "caterpillar") == "cat");
            ASSERT(ocVal.has_value() && ocVal->get() == 4);
            ASSERT(X.findLongestCommonPrefix(&ocVal, "").empty());
            ASSERT(!ocVal.has_value());
        }

        // Test non-const version (including modification)
        {
            IntTree::OptValueRef oMutVal;
            ASSERT(mX.findLongestCommonPrefix(&oMutVal, "car") == "car");
            ASSERT(oMutVal.has_value() && oMutVal->get() == 1);
            oMutVal->get() = 42;
            ASSERT(mX.findLongestCommonPrefix(&oMutVal, "car") == "car");
            ASSERT(oMutVal && *oMutVal == 42);
            // Restore
            oMutVal->get() = 1;
        }

        // REGRESSION: Ensure no infinite loop on empty tree or empty prefix
        if (verbose) cout << "Regression: longestCommonPrefix does not hang on"
                                               " empty tree or empty prefix\n";

        // 1. Empty tree
        {
            IntTree                emptyTree;
            IntTree::OptValueRef   optVal;
            const bsl::string_view result =
                             emptyTree.findLongestCommonPrefix(&optVal, "foo");
            ASSERT(result.empty());
        }

        // 2. Tree with a single empty-string key (edge case)
        {
            IntTree treeWithEmptyKey;
            treeWithEmptyKey.emplace("", 123);
            IntTree::OptValueRef optVal;
            bsl::string_view     result =
                      treeWithEmptyKey.findLongestCommonPrefix(&optVal, "foo");
            // Should not hang, should return empty
            ASSERT(result.empty());
            ASSERTV(optVal.has_value(), optVal && optVal->get() == 123);

            result = treeWithEmptyKey.findLongestCommonPrefix(&optVal, "");
            ASSERT(result == "");
            ASSERTV(optVal.has_value(), optVal && optVal->get() == 123);
        }

        // 3. Query longer than all keys in tree
        {
            if (veryVerbose) cout << "\tQuery longer than all keys\n";

            IntTree mX;
            mX.emplace("a", 1);
            mX.emplace("ab", 2);
            mX.emplace("abc", 3);

            IntTree::OptValueRef   optVal;
            const bsl::string_view result =
                               mX.findLongestCommonPrefix(&optVal, "abcdefgh");
            ASSERT(result == "abc");
            ASSERT(optVal && optVal->get() == 3);
        }

        // 4. Tree with only empty key, various queries
        {
            if (veryVerbose) cout << "\tTree with only empty key\n";

            IntTree              mX;
            IntTree::OptValueRef optVal;

            mX.emplace("", 99);

            // Query empty string
            bsl::string_view result = mX.findLongestCommonPrefix(&optVal, "");
            ASSERT(result == "");
            ASSERT(optVal && optVal->get() == 99);

            // Query non-empty string with only empty key in tree
            result = mX.findLongestCommonPrefix(&optVal, "anything");
            ASSERT(result.empty());
            ASSERT(optVal && optVal->get() == 99);
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // ERASE PREFIX
        //
        // Concerns:
        // 1. `erasePrefix` removes all entries with keys that start with the
        //    specified prefix.
        // 2. `erasePrefix` returns the correct count of removed entries.
        // 3. `erasePrefix` correctly handles the case when the prefix matches
        //    a complete key (node with value).
        // 4. `erasePrefix` correctly handles the case when the prefix matches
        //    a partial node (node without value).
        // 5. `erasePrefix` correctly handles the case when the prefix doesn't
        //    exist in the tree.
        // 6. `erasePrefix` with empty prefix removes all entries.
        // 7. The size of the tree is correctly updated.
        // 8. Entries not matching the prefix are preserved.
        //
        // Plan:
        // 1. Use table-driven testing with the generator function to create
        //    trees in various states.
        // 2. For each test case, call `erasePrefix` and verify:
        //    a. The correct entries are removed.
        //    b. The correct count is returned.
        //    c. The tree size is correctly updated.
        //    d. Remaining entries are accessible.
        // 3. Test edge cases via the data table.
        //
        // Testing:
        //   size_type erasePrefix(const bsl::string_view&);
        // --------------------------------------------------------------------

        if (verbose) cout << "ERASE PREFIX\n"
                             "============\n";

        static const struct {
            int         d_line;
            const char *d_spec;
            const char *d_prefix;
            int         d_expectedRemoved;
            int         d_expectedSizeAfter;
            const char *d_shouldContain;      // comma-separated
            const char *d_shouldNotContain;   // comma-separated
        } DATA[] = {
            // LINE SPEC                               PREFIX    REM SIZE
            // ---- ---------------------------------- --------- --- ----
            //       CONTAIN
            //       ---------------------------------
            //       NOT_CONTAIN
            //       ---------------------------------

            {  L_,  "",                                "any",    0,  0,
                    "",
                    ""                                                       },
            {  L_,  "apple:1 app:2 application:3 "
                    "apply:4 banana:5 band:6 cat:7",   "app",    4,  3,
                    "banana,band,cat",
                    "app,apple,application,apply"                            },
            {  L_,  "car:1 card:2 care:3 cat:4",       "car",    3,  1,
                    "cat",
                    "car,card,care"                                          },
            {  L_,  "testing:1 tester:2 test:3",       "te",     3,  0,
                    "",
                    "testing,tester,test"                                    },
            {  L_,  "apple:1 banana:2",                "cherry", 0,  2,
                    "apple,banana",
                    ""                                                       },
            {  L_,  "apple:1 banana:2 cat:3",          "",       3,  0,
                    "",
                    "apple,banana,cat"                                       },
            {  L_,  "ab:1 cd:2",                       "abc",    0,  2,
                    "ab,cd",
                    ""                                                       },
            {  L_,  "a:1 ab:2 abc:3 b:4 bc:5 bcd:6",   "ab",     2,  4,
                    "a,b,bc,bcd",
                    "ab,abc"                                                 },
            {  L_,  "a:1 ab:2 abc:3 b:4 bc:5 bcd:6",   "b",      3,  3,
                    "a,ab,abc",
                    "b,bc,bcd"                                               },
        };
        const bsl::size_t NUM_DATA = sizeof DATA / sizeof *DATA;

        for (bsl::size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE       = DATA[ti].d_line;
            const char *const SPEC       = DATA[ti].d_spec;
            const char *const PREFIX     = DATA[ti].d_prefix;
            const int         EXP_REM    = DATA[ti].d_expectedRemoved;
            const int         EXP_SIZE   = DATA[ti].d_expectedSizeAfter;
            const char *const CONTAIN    = DATA[ti].d_shouldContain;
            const char *const NOT_CONTAIN= DATA[ti].d_shouldNotContain;

            if (veryVerbose) {
                T_ P_(LINE) P_(SPEC) P_(PREFIX) P_(EXP_REM) P(EXP_SIZE)
            }

            bslma::TestAllocator ta("test", veryVeryVeryVerbose);
            IntTree              mX(&ta);

            ASSERTV(LINE, 0 == gg(&mX, SPEC, veryVerbose));

            const bsl::size_t removed = mX.erasePrefix(PREFIX);

            ASSERTV(LINE, PREFIX, EXP_REM, removed,
                    EXP_REM == static_cast<int>(removed));
            ASSERTV(LINE, PREFIX, EXP_SIZE, mX.size(),
                    EXP_SIZE == static_cast<int>(mX.size()));

            // Verify keys that should be present
            if (CONTAIN[0] != '\0') {
                bsl::string containStr(CONTAIN);
                bsl::size_t pos = 0;
                while (pos < containStr.size()) {
                    bsl::size_t commaPos = containStr.find(',', pos);
                    if (commaPos == bsl::string::npos) {
                        commaPos = containStr.size();
                    }
                    bsl::string key = containStr.substr(pos, commaPos - pos);
                    ASSERTV(LINE, PREFIX, key, mX.contains(key));
                    pos = commaPos + 1;
                }
            }

            // Verify keys that should not be present
            if (NOT_CONTAIN[0] != '\0') {
                bsl::string notContainStr(NOT_CONTAIN);
                bsl::size_t pos = 0;
                while (pos < notContainStr.size()) {
                    bsl::size_t commaPos = notContainStr.find(',', pos);
                    if (commaPos == bsl::string::npos) {
                        commaPos = notContainStr.size();
                    }
                    const bsl::string key =
                                     notContainStr.substr(pos, commaPos - pos);
                    ASSERTV(LINE, PREFIX, key,
                            !mX.contains(key));
                    pos = commaPos + 1;
                }
            }
        }

#ifdef BDE_BUILD_TARGET_EXC
        {  // Exception testing of `erasePrefix`.

            bslma::TestAllocator xa("exc", veryVeryVeryVerbose);

            StringTree tree(&xa);
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(xa) {
                // Rebuild tree in each iteration
                tree.emplace("apple",       "apple_value");
                tree.emplace("applesauce",  "applesauce_value");
                tree.emplace("application", "application_value");
                tree.emplace("apply",       "apply_value");
                tree.emplace("banana",      "banana_value");
                tree.emplace("band",        "band_value");
                tree.emplace("cat",         "cat_value");

                ASSERTV(tree.size(), tree.size() == 7);

                // Erase all entries with prefix "apple"
                bsl::size_t removed = tree.erasePrefix("apple");
                ASSERTV(removed,           removed           == 2);
                ASSERTV(tree.size(),       tree.size()       == 5);
                ASSERTV(tree.countNodes(), tree.countNodes() == 8);
                // Root, "appl", "ication", "y", "ban", "ana", "d", "cat" nodes

                ASSERT(!tree.contains("apple"));
                ASSERT(!tree.contains("applesauce"));
                ASSERT( tree.contains("application"));
                ASSERT( tree.contains("apply"));
                ASSERT( tree.contains("banana"));
                ASSERT( tree.contains("band"));
                ASSERT( tree.contains("cat"));

                // Erase all entries with prefix "app"
                removed = tree.erasePrefix("app");
                ASSERTV(removed,           removed           == 2);
                ASSERTV(tree.size(),       tree.size()       == 3);
                ASSERTV(tree.countNodes(), tree.countNodes() == 5);
                // Root, "ban", "ana", "d", "cat" nodes

                ASSERT(!tree.contains("apple"));
                ASSERT(!tree.contains("applesauce"));
                ASSERT(!tree.contains("application"));
                ASSERT(!tree.contains("apply"));
                ASSERT( tree.contains("banana"));
                ASSERT( tree.contains("band"));
                ASSERT( tree.contains("cat"));

                // Erase all entries with prefix "c"
                removed = tree.erasePrefix("c");
                ASSERTV(removed,           removed           == 1);
                ASSERTV(tree.size(),       tree.size()       == 2);
                ASSERTV(tree.countNodes(), tree.countNodes() == 4);
                // Root, "ban", "ana", "d" nodes

                ASSERT(!tree.contains("apple"));
                ASSERT(!tree.contains("applesauce"));
                ASSERT(!tree.contains("application"));
                ASSERT(!tree.contains("apply"));
                ASSERT( tree.contains("banana"));
                ASSERT( tree.contains("band"));
                ASSERT(!tree.contains("cat"));

                // Erase all entries with prefix "band"
                removed = tree.erasePrefix("band");
                ASSERTV(removed,           removed           == 1);
                ASSERTV(tree.size(),       tree.size()       == 1);
                ASSERTV(tree.countNodes(), tree.countNodes() == 2);
                // Root, "banana" nodes

                ASSERT(!tree.contains("apple"));
                ASSERT(!tree.contains("applesauce"));
                ASSERT(!tree.contains("application"));
                ASSERT(!tree.contains("apply"));
                ASSERT( tree.contains("banana"));
                ASSERT(!tree.contains("band"));
                ASSERT(!tree.contains("cat"));

                // Erase all entries with prefix "ba"
                removed = tree.erasePrefix("ba");
                ASSERTV(removed,           removed           == 1);
                ASSERTV(tree.size(),       tree.size()       == 0);
                ASSERTV(tree.countNodes(), tree.countNodes() == 1);
                // Root node

                ASSERT(!tree.contains("apple"));
                ASSERT(!tree.contains("applesauce"));
                ASSERT(!tree.contains("application"));
                ASSERT(!tree.contains("apply"));
                ASSERT(!tree.contains("banana"));
                ASSERT(!tree.contains("band"));
                ASSERT(!tree.contains("cat"));
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        }

        // Test erasing with empty prefix (should erase all)
        {
            if (veryVerbose) cout << "\tErase with empty prefix\n";

            bslma::TestAllocator ta("test", veryVeryVeryVerbose);
            IntTree              mX(&ta);

            mX.emplace("apple",  1);
            mX.emplace("banana", 2);
            mX.emplace("cherry", 3);
            mX.emplace("", 4);  // Empty key

            ASSERT(mX.size() == 4);

            // Erase all with empty prefix - should remove everything
            const IntTree::size_type removed = mX.erasePrefix("");
            ASSERT(removed == 4);
            ASSERT(mX.empty());
        }

        // Test erasing prefix where prefix node has value but no children
        {
            if (veryVerbose) cout << "\tPrefix node with value, no children\n";

            bslma::TestAllocator ta("test", veryVeryVeryVerbose);
            IntTree              mX(&ta);

            mX.emplace("test",  1);
            mX.emplace("other", 2);

            ASSERT(mX.size() == 2);

            // "test" has no children
            const IntTree::size_type removed = mX.erasePrefix("test");
            ASSERT(removed   == 1);
            ASSERT(mX.size() == 1);
            ASSERT(!mX.contains("test"));
            ASSERT( mX.contains("other"));
        }
#endif  // BDE_BUILD_TARGET_EXC
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // FOREACH
        //
        // Concerns:
        // 1. The `forEach` method calls the functor for every key-value pair.
        // 2. The functor receives the correct keys and values.
        // 3. The functor works with different callable types (functions,
        //    lambdas, functors).
        // 4. `forEach` an empty tree doesn't call the functor.
        //
        // Plan:
        // 1. Create a tree with known entries.
        // 2. Use a functor to collect all entries.
        // 3. Verify the collected entries match what was inserted.
        // 4. Test with empty tree.
        //
        // Testing:
        //   void forEach(const t_FUNCTOR&) const;
        // --------------------------------------------------------------------

        if (verbose) cout << "FOREACH\n"
                             "=======\n";

        {
            bslma::TestAllocator ta("test",    veryVeryVeryVerbose);
            bslma::TestAllocator sa("scratch", veryVeryVeryVerbose);

            IntTree mX(&ta);

            mX.emplace("apple",       1);
            mX.emplace("app",         2);
            mX.emplace("application", 3);
            mX.emplace("banana",      4);

            // Collect all entries using a functor
            EntryCollector::Entries entries(&sa);
            EntryCollector collector(&entries);
            mX.forEach(collector);

            ASSERT(entries.size() == 4);

            // Verify entries (exact order is unspecified, but all must be
            // present)
            bool foundApple       = false;
            bool foundApp         = false;
            bool foundApplication = false;
            bool foundBanana      = false;

            for (EntryCollector::Entries::const_iterator it = entries.begin();
                                                         it != entries.end();
                                                       ++it) {
                const EntryCollector::Entries::value_type& entry = *it;
                if (entry.first == "apple" && entry.second == 1) {
                    foundApple = true;
                }
                else if (entry.first == "app" && entry.second == 2) {
                    foundApp = true;
                }
                else if (entry.first == "application" && entry.second == 3) {
                    foundApplication = true;
                }
                else if (entry.first == "banana" && entry.second == 4) {
                    foundBanana = true;
                }
            }

            ASSERT(foundApple);
            ASSERT(foundApp);
            ASSERT(foundApplication);
            ASSERT(foundBanana);
        }

        // Test `forEach` with empty tree
        {
            bslma::TestAllocator ta("test", veryVeryVeryVerbose);

            IntTree mX(&ta);

            int            count   = 0;
            CounterFunctor counter(&count);
            mX.forEach(counter);

            ASSERT(0 == count);
        }

        // Test `forEach` with single-element tree
        {
            if (veryVerbose) cout << "\tforEach with single element\n";

            bslma::TestAllocator ta("test", veryVeryVeryVerbose);
            IntTree              mX(&ta);

            mX.emplace("only", 42);

            int            count   = 0;
            CounterFunctor counter(&count);
            mX.forEach(counter);

            ASSERT(1 == count);
        }

        // Test `forEach` with only empty key
        {
            if (veryVerbose) cout << "\tforEach with only empty key\n";

            bslma::TestAllocator ta("test", veryVeryVeryVerbose);
            IntTree              mX(&ta);

            mX.emplace("", 99);

            int            count   = 0;
            CounterFunctor counter(&count);
            mX.forEach(counter);

            ASSERT(1 == count);

            // Verify we can find the value
            bslma::TestAllocator    sa("scratch", veryVeryVeryVerbose);
            EntryCollector::Entries entries(&sa);
            EntryCollector          collector(&entries);
            mX.forEach(collector);

            ASSERT(entries.size() == 1);
            ASSERT(entries[0].first == "");
            ASSERT(entries[0].second == 99);
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // COPY-ASSIGNMENT OPERATOR
        //
        // Concerns:
        // 1. The assignment operator properly copies the contents.
        // 2. Self-assignment is handled correctly.
        // 3. Memory is allocated from the correct allocator.
        //
        // Plan:
        // 1. Create two trees with different content.
        // 2. Assign one to the other and verify the result.
        // 3. Test self-assignment.
        //
        // Testing:
        //   CategoryManager_RadixTree& operator=(rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << "COPY-ASSIGNMENT OPERATOR\n"
                             "========================\n";

        {
            bslma::TestAllocator ta("test", veryVeryVeryVerbose);

            IntTree mX(&ta);
            mX.emplace("foo",  42);
            mX.emplace("bar", 100);

            IntTree mY(&ta);
            mY.emplace("baz", 200);

            mY = mX;

            ASSERT(mY.size() == 2);

            IntTree::OptValueRef result = mY.find("foo");
            ASSERT(result && result->get() == 42);
            result = mY.find("bar");
            ASSERT(result && result->get() == 100);
            result = mY.find("baz");
            ASSERT(!result);
        }

        // Self-assignment
        {
            bslma::TestAllocator ta("test", veryVeryVeryVerbose);
            IntTree              mX(&ta);

            mX.emplace("test", 123);

#ifdef BSLS_PLATFORM_PRAGMA_GCC_DIAGNOSTIC_CLANG
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-assign-overloaded"
#endif
            mX = mX;
#ifdef BSLS_PLATFORM_PRAGMA_GCC_DIAGNOSTIC_CLANG
#pragma clang diagnostic pop
#endif

            ASSERT(mX.size() == 1);
            IntTree::OptValueRef result = mX.find("test");
            ASSERT(result && result->get() == 123);
        }

#ifdef BDE_BUILD_TARGET_EXC
        {  // Exception testing of copy assignment operator

            bslma::TestAllocator sa("source", veryVeryVeryVerbose);
            bslma::TestAllocator xa("exc",    veryVeryVeryVerbose);

            StringTree source(&sa);
            source.emplace("apple",       "apple_value");
            source.emplace("applesauce",  "applesauce_value");
            source.emplace("application", "application_value");
            source.emplace("apply",       "apply_value");
            source.emplace("banana",      "banana_value");
            source.emplace("band",        "band_value");

            const StringTree& SOURCE = source;

            ASSERTV(SOURCE.size(),       SOURCE.size()       == 6);
            ASSERTV(SOURCE.countNodes(), SOURCE.countNodes() == 9);

            StringTree target(&xa);
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(xa) {
                target = SOURCE;

                ASSERTV(target.size(),         SOURCE.size(),
                        target.size()       == SOURCE.size());
                ASSERTV(target.countNodes(),   SOURCE.countNodes(),
                        target.countNodes() == SOURCE.countNodes());

                ASSERT(target.get_allocator().mechanism() == &xa);

                ASSERT(target.contains("apple"));
                ASSERT(target.contains("applesauce"));
                ASSERT(target.contains("application"));
                ASSERT(target.contains("apply"));
                ASSERT(target.contains("banana"));
                ASSERT(target.contains("band"));

                StringTree::OptValueRef result = target.find("apple");
                ASSERT(result && result->get() == "apple_value");
                result = target.find("applesauce");
                ASSERT(result && result->get() == "applesauce_value");
                result = target.find("application");
                ASSERT(result && result->get() == "application_value");
                result = target.find("apply");
                ASSERT(result && result->get() == "apply_value");
                result = target.find("banana");
                ASSERT(result && result->get() == "banana_value");
                result = target.find("band");
                ASSERT(result && result->get() == "band_value");
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        }
#endif  // BDE_BUILD_TARGET_EXC
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // SWAP MEMBER AND FREE FUNCTIONS
        //
        // Concerns:
        // 1. Both methods exchange the values of two objects correctly.
        // 2. The free function is discoverable via ADL.
        // 3. The methods work with empty containers.
        // 4. The free function works with different allocators.
        //
        // Plan:
        // 1. Create two trees with different contents.
        // 2. Swap them and verify the contents were exchanged.
        // 3. Test with one or both containers empty.
        // 4. Test free function with objects having different allocators and
        //    verify allocators are preserved.
        //
        // Testing:
        //   void swap(CategoryManager_RadixTree&);
        //   void swap(CategoryManager_RadixTree&, CategoryManager_RadixTree&);
        // --------------------------------------------------------------------

        if (verbose) cout << "SWAP MEMBER AND FREE FUNCTIONS\n"
                             "==============================\n";

        {
            bslma::TestAllocator ta("test", veryVeryVeryVerbose);

            IntTree mX(&ta);
            mX.emplace("foo", 1);
            mX.emplace("bar", 2);

            IntTree mY(&ta);
            mY.emplace("baz", 3);

            mX.swap(mY);

            ASSERT(mX.size() == 1);
            ASSERT(mY.size() == 2);

            IntTree::OptValueRef result = mX.find("baz");
            ASSERT(result && result->get() == 3);
            result = mY.find("foo");
            ASSERT(result && result->get() == 1);
            result = mY.find("bar");
            ASSERT(result && result->get() == 2);
        }

        // Test free function with same allocator
        {
            bslma::TestAllocator ta("test", veryVeryVeryVerbose);

            IntTree mX(&ta);
            mX.emplace("alpha", 10);

            IntTree mY(&ta);
            mY.emplace("beta", 20);

            using ball::swap;
            swap(mX, mY);

            IntTree::OptValueRef result = mX.find("beta");
            ASSERT(result && result->get() == 20);
            result = mY.find("alpha");
            ASSERT(result && result->get() == 10);
        }

        // Test free function with different allocators
        {
            bslma::TestAllocator taX("testX", veryVeryVeryVerbose);
            bslma::TestAllocator taY("testY", veryVeryVeryVerbose);

            IntTree mX(&taX);
            mX.emplace("alpha", 10);

            IntTree mY(&taY);
            mY.emplace("beta", 20);

            using ball::swap;
            swap(mX, mY);

            // Verify values were exchanged
            IntTree::OptValueRef result = mX.find("beta");
            ASSERT(result && result->get() == 20);
            result = mY.find("alpha");
            ASSERT(result && result->get() == 10);

            // Verify allocators remain unchanged (each object keeps its own)
            ASSERT(mX.get_allocator().mechanism() == &taX);
            ASSERT(mY.get_allocator().mechanism() == &taY);
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // COPY CONSTRUCTOR
        //
        // Concerns:
        // 1. The new object has the same value as the original.
        // 2. The value of the original object is left unaffected.
        // 3. Memory is allocated from the correct allocator.
        //
        // Plan:
        // 1. Create an object with the copy constructor and verify it has
        //    the expected value using accessors.
        // 2. Verify the original object is unchanged.
        //
        // Testing:
        //   CategoryManager_RadixTree(lhs, rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << "COPY CONSTRUCTOR\n"
                             "================\n";

        {
            bslma::TestAllocator ta("test", veryVeryVeryVerbose);

            StringTree mX(&ta);
            mX.emplace("hello", "valueA");
            mX.emplace("world", "valueB");

            StringTree mY(mX, &ta);

            ASSERT(mY.size() == 2);

            StringTree::OptValueRef result = mY.find("hello");
            ASSERT(result && result->get() == "valueA");
            ASSERT(result && result->get().get_allocator().mechanism() == &ta);
            result = mY.find("world");
            ASSERT(result && result->get() == "valueB");
            ASSERT(result && result->get().get_allocator().mechanism() == &ta);

            // Verify original is unchanged
            ASSERT(mX.size() == 2);
            result = mX.find("hello");
            ASSERT(result && result->get() == "valueA");
        }

        // Copy constructor with different allocator, using StringTree
        {
            bslma::TestAllocator taSrc("src", veryVeryVeryVerbose);
            bslma::TestAllocator taDst("dst", veryVeryVeryVerbose);
            StringTree src(&taSrc);
            src.emplace("key1", "value1");
            src.emplace("key2", "value2");

            // Copy to target with different allocator
            StringTree dst(src, &taDst);

            // Verify allocator propagation
            ASSERT(dst.get_allocator() == bsl::allocator<bsl::string>(&taDst));
            ASSERT(src.get_allocator() == bsl::allocator<bsl::string>(&taSrc));

            // Verify contents are copied
            StringTree::OptValueRef r1 = dst.find("key1");
            StringTree::OptValueRef r2 = dst.find("key2");
            ASSERT(r1 && r1->get() == "value1");
            ASSERT(r2 && r2->get() == "value2");

            // Verify source is unchanged
            StringTree::OptValueRef s1 = src.find("key1");
            StringTree::OptValueRef s2 = src.find("key2");
            ASSERT(s1 && s1->get() == "value1");
            ASSERT(s2 && s2->get() == "value2");
        }

        {
            // Test copy constructor with AllocAwareType (no default ctor)
            if (verbose) cout << "\tTesting copy with AllocAwareType\n";

            bslma::TestAllocator ta("alloc-aware-copy", veryVeryVeryVerbose);
            AllocAwareTree       mX(&ta);

            mX.emplace("key1", "first1", "second1");
            mX.emplace("key2", "first2", "second2");

            AllocAwareTree mY(mX, &ta);

            ASSERT(mY.size() == 2);
            AllocAwareTree::OptValueRef result = mY.find("key1");
            ASSERT(result && result->get().first() == "first1");
            ASSERT(result && result->get().second() == "second1");
            ASSERT(result && result->get().allocator() == &ta);

            result = mY.find("key2");
            ASSERT(result && result->get().first() == "first2");
            ASSERT(result && result->get().second() == "second2");
            ASSERT(result && result->get().allocator() == &ta);

            // Verify original is unchanged
            ASSERT(mX.size() == 2);
        }

        {
            // Test copy constructor with NonAllocType (no default ctor)
            if (verbose)
                cout << "\tTesting copy with NonAllocType\n";

            bslma::TestAllocator ta("non-alloc-copy", veryVeryVeryVerbose);
            NonAllocTree         mX(&ta);

            mX.emplace("key1", 10, 20, 30);
            mX.emplace("key2", 100, 200, 300);

            NonAllocTree mY(mX, &ta);

            ASSERT(mY.size() == 2);
            NonAllocTree::OptValueRef result = mY.find("key1");
            ASSERT(result && result->get().first() == 10);
            ASSERT(result && result->get().second() == 20);
            ASSERT(result && result->get().third() == 30);

            result = mY.find("key2");
            ASSERT(result && result->get().first() == 100);
            ASSERT(result && result->get().second() == 200);
            ASSERT(result && result->get().third() == 300);

            // Verify original is unchanged
            ASSERT(mX.size() == 2);
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // EQUALITY-COMPARISON OPERATORS
        //
        // Concerns:
        // 1. Two objects, `X` and `Y`, compare equal if and only if they
        //    contain the same number of entries and each entry in `X` has the
        //    same value as the corresponding entry in `Y`.
        // 2. Comparison is symmetric with respect to user-defined conversion
        //    (i.e., both comparison operators are free functions).
        // 3. Non-modifiable objects can be compared (i.e., objects or
        //    references providing only non-modifiable access).
        //
        // Plan:
        // 1. Create pairs of objects with different contents and verify the
        //    comparison results are as expected.
        //
        // Testing:
        //   bool operator==(lhs, rhs);
        //   bool operator!=(lhs, rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << "EQUALITY-COMPARISON OPERATORS\n"
                             "=============================\n";

        bslma::TestAllocator ta("test", veryVeryVeryVerbose);

        IntTree mX(&ta);
        IntTree mY(&ta);

        ASSERT(mX == mY);
        ASSERT(!(mX != mY));

        mX.emplace("foo", 42);
        ASSERT(mX != mY);
        ASSERT(!(mX == mY));

        mY.emplace("foo", 42);
        ASSERT(mX == mY);
        ASSERT(!(mX != mY));

        mY.emplace("bar", 100);
        ASSERT(mX != mY);

        mX.emplace("bar", 100);
        ASSERT(mX == mY);
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // PRINTNODES METHOD
        //   Ensure that the value of the object can be formatted appropriately
        //   on an `ostream` in some standard, human-readable form.
        //
        // Concerns:
        // 1. The `printNodes` method writes the value to the specified
        //    `ostream`.
        //
        // 2. The `printNodes` method writes the value in the intended format.
        //
        // 3. The `printNodes` method signature and return type are standard.
        //
        // 4. The `printNodes` method returns the supplied `ostream`.
        //
        // 5. The optional `level` and `spacesPerLevel` parameters have the
        //    correct default values.
        //
        // Plan:
        // 1. Use the address of the `printNodes` member function defined in
        //    this component to initialize a member-function pointer having the
        //    appropriate signature and return type.  (C-3)
        //
        // 2. Using the table-driven technique:  (C-1..2, 4..5)
        //
        //   1. Define several combinations of object values and formatting
        //      parameters, along with the expected output:
        //
        //      ( `value` x  `level`   x `spacesPerLevel` ):
        //     1. { A   } x {  0     } x {  0, 1, -1, -8 } --> 4 expected o/ps
        //     2. { A   } x {  1, -1 } x {  0, 2, -2, -8 } --> 8 expected o/ps
        //     3. { B   } x {  0     } x {  4            } --> 1 expected o/p
        //     4. { C   } x { -8     } x { -8            } --> 1 expected o/p
        //
        //   2. For each row in the table defined in P-2.1:  (C-1..2, 4..5)
        //
        //     1. Using a `const` `Obj`, supply each object value and pair of
        //        formatting parameters to `printNodes`, omitting the `level`
        //        or `spacesPerLevel` parameter if the value of that argument
        //        is `-8`.
        //
        //     2. Use a standard `ostringstream` to capture the actual output.
        //
        //     3. Verify the address of what is returned is that of the
        //        supplied stream.  (C-4)
        //
        //     4. Compare the contents captured in P-2.2.2 with what is
        //        expected.  (C-1..2, 5)
        //
        // Testing:
        //   bsl::ostream& printNodes(bsl::ostream&, int level, int sPL) const;
        // --------------------------------------------------------------------

        if (verbose) cout << "PRINTNODES METHOD\n"
                             "=================\n";

        if (verbose) cout <<
               "\tAssign the address of the `printNodes` function to a "
               "variable.\n";
        {
            typedef bsl::ostream& (StringTree::*funcPtr)(bsl::ostream&,
                                                         int,
                                                         int) const;

            // Verify that the signature and return type are standard.

            funcPtr printMember = &StringTree::printNodes;

            (void)printMember;  // quash potential compiler warning
        }

        if (verbose) cout <<
                   "\tCreate a table of distinct value/format combinations.\n";

        static const struct {
            int         d_line;           // source line number
            int         d_level;
            int         d_spacesPerLevel;
            const char *d_spec;
            const char *d_expected_p;
        } DATA[] = {

#define NL "\n"

        // ------------------------------------------------------------------
        // P-2.1.1: { A } x { 0 }     x { 0, 1, -1, -8 } -->  4 expected o/ps
        // ------------------------------------------------------------------

        //LINE L SPL  SPEC              EXP
        //---- - ---  ----              ---

        { L_,  0,  0,  "a:foo ab:bar",  "\"\": **NO-VALUE**"         NL
                                        "\"a\": foo"                 NL
                                        "\"ab\": bar"                NL },

        { L_,  0,  1,  "a:foo ab:bar",  "\"\": **NO-VALUE**"         NL
                                        " \"a\": foo"                NL
                                        "  \"ab\": bar"              NL },

        { L_,  0, -1,  "a:foo ab:bar",  "{0} \"\": **NO-VALUE** "
                                        "{1} \"a\": foo "
                                        "{2} \"ab\": bar "              },

        { L_,  0, -3,  "a:foo ab:bar",  "{0} \"\": **NO-VALUE** "
                                        "{1} \"a\": foo "
                                        "{2} \"ab\": bar "              },

        // ------------------------------------------------------------------
        // P-2.1.2: { A } x { 1, -1 } x { 0, 2, -2, -3 } -->  8 expected o/ps
        // ------------------------------------------------------------------

        //LINE L SPL  SPEC              EXP
        //---- - ---  ----              ---

        { L_,  1,  0,  "a:foo ab:bar",  "\"\": **NO-VALUE**"         NL
                                        "\"a\": foo"                 NL
                                        "\"ab\": bar"                NL },

        { L_,  1,  2,  "a:foo ab:bar",  "  \"\": **NO-VALUE**"       NL
                                        "    \"a\": foo"             NL
                                        "      \"ab\": bar"          NL },

        { L_,  1, -2,  "a:foo ab:bar",  "  {0} \"\": **NO-VALUE** "
                                        "{1} \"a\": foo "
                                        "{2} \"ab\": bar "              },

        { L_,  1, -3,  "a:foo ab:bar",  "   {0} \"\": **NO-VALUE** "
                                        "{1} \"a\": foo "
                                        "{2} \"ab\": bar "              },

        { L_, -1,  0,  "a:foo ab:bar",  "\"\": **NO-VALUE**"         NL
                                        "\"a\": foo"                 NL
                                        "\"ab\": bar"                NL },

        { L_, -1,  2,  "a:foo ab:bar",  "\"\": **NO-VALUE**"         NL
                                        "    \"a\": foo"             NL
                                        "      \"ab\": bar"          NL },

        { L_, -1, -2,  "a:foo ab:bar",  "{0} \"\": **NO-VALUE** "
                                        "{1} \"a\": foo "
                                        "{2} \"ab\": bar "              },

        { L_, -1, -3,  "a:foo ab:bar",  "{0} \"\": **NO-VALUE** "
                                        "{1} \"a\": foo "
                                        "{2} \"ab\": bar "              },

        // -----------------------------------------------------------------
        // P-2.1.3: { B } x { 0 }   x { 4 }             -->  1 expected o/p
        // -----------------------------------------------------------------

        //LINE L SPL  SPEC              EXP
        //---- - ---  ----              ---

        { L_,  0,  4,  "x:alpha xy:beta xyz:gamma",
                                        "\"\": **NO-VALUE**"         NL
                                        "    \"x\": alpha"           NL
                                        "        \"xy\": beta"       NL
                                        "            \"xyz\": gamma" NL },

        // -----------------------------------------------------------------
        // P-2.1.4: { C } x { -8 }   x { -8 }          -->  1 expected o/p
        // -----------------------------------------------------------------

        //LINE L SPL  SPEC              EXP
        //---- - ---  ----              ---

        { L_, -8, -8,  "",              "\"\": **NO-VALUE**"         NL },

#undef NL

        };
        const bsl::size_t NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\tTesting with various print specifications.\n";
        {
            for (bsl::size_t ti = 0; ti < NUM_DATA; ++ti) {
                const int              LINE = DATA[ti].d_line;
                const int              L    = DATA[ti].d_level;
                const int              SPL  = DATA[ti].d_spacesPerLevel;
                const char *const      SPEC = DATA[ti].d_spec;
                const bsl::string_view EXP  = DATA[ti].d_expected_p;

                if (veryVerbose) { T_ P_(LINE) P_(L) P_(SPL) P_(SPEC) P(EXP) }

                bslma::TestAllocator ta("test", veryVeryVeryVerbose);

                StringTree mX(&ta);
                const StringTree& X = mX;

                ASSERTV(LINE, 0 == gg(&mX, SPEC));

                bsl::ostringstream os;

                // Verify supplied stream is returned by reference.

                if (-8 != SPL) {
                    ASSERTV(LINE, &os == &X.printNodes(os, L, SPL));
                }
                else if (-8 != L) {
                    ASSERTV(LINE, &os == &X.printNodes(os, L));
                }
                else {
                    ASSERTV(LINE, &os == &X.printNodes(os));
                }

                if (veryVeryVerbose) { T_ T_ Q(printNodes) }

                // Verify output is formatted as expected.

                if (veryVeryVerbose) { P(os.str()) }

                ASSERTV(LINE, EXP, os.str(), EXP == os.str());
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // PREFIX COMPRESSION TEST
        //
        // Concerns:
        // 1. The radix tree efficiently compresses common prefixes.
        // 2. Insertion and lookup work correctly with shared prefixes.
        // 3. Node splitting occurs correctly when necessary.
        //
        // Plan:
        // 1. Insert strings with common prefixes.
        // 2. Verify all strings can be looked up correctly.
        // 3. Verify the size is as expected.
        //
        // Testing:
        //   PREFIX COMPRESSION
        // --------------------------------------------------------------------

        if (verbose) cout << "PREFIX COMPRESSION TEST\n"
                             "=======================\n";

        {
            bslma::TestAllocator ta("test", veryVeryVeryVerbose);
            IntTree              mX(&ta);

            // Insert strings with common prefix "test"
            mX.emplace("test",    1);
            mX.emplace("tester",  2);
            mX.emplace("testing", 3);
            mX.emplace("testy",   4);

            ASSERT(mX.size() == 4);

            IntTree::OptValueRef result = mX.find("test");
            ASSERT(result && result->get() == 1);
            result = mX.find("tester");
            ASSERT(result && result->get() == 2);
            result = mX.find("testing");
            ASSERT(result && result->get() == 3);
            result = mX.find("testy");
            ASSERT(result && result->get() == 4);

            // Insert a string that splits an existing node
            mX.emplace("tea", 5);
            ASSERT(mX.size() == 5);
            result = mX.find("tea");
            ASSERT(result && result->get() == 5);

            // Verify all previous strings still work
            result = mX.find("test");
            ASSERT(result && result->get() == 1);
            result = mX.find("tester");
            ASSERT(result && result->get() == 2);
            result = mX.find("testing");
            ASSERT(result && result->get() == 3);
            result = mX.find("testy");
            ASSERT(result && result->get() == 4);
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        //
        // Concerns:
        // 1. `find` returns correct values for existing keys.
        // 2. `find` returns false for non-existent keys.
        // 3. `contains` returns true for existing keys and false otherwise.
        // 4. `size` reflects the number of entries.
        // 5. `empty` returns true for empty containers and false otherwise.
        // 6. `get_allocator` returns the allocator used at construction.
        //
        // Plan:
        // 1. Use table-driven testing with the generator function `gg` to
        //    create trees in various states.
        // 2. For each test case, verify accessors return expected values.
        //
        // Testing:
        //   optional<reference_wrapper<t_VALUE>> find(...);
        //   optional<reference_wrapper<const t_VALUE>> find(...) const;
        //   bool contains(const bsl::string_view&) const;
        //   size_type size() const;
        //   bool empty() const;
        //   allocator_type get_allocator() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "BASIC ACCESSORS\n"
                             "===============\n";

        static const struct {
            int          d_line;
            const char  *d_spec;
            bsl::size_t  d_expectedSize;
        } DATA[] = {
            // LINE  SPEC                              SIZE
            // ----  --------------------------------  ----
            {  L_,   "",                               0,   },
            {  L_,   "a:1",                            1,   },
            {  L_,   "a:1 b:2",                        2,   },
            {  L_,   "foo:42 bar:100",                 2,   },
            {  L_,   "a:1 ab:2 abc:3",                 3,   },
            {  L_,   "x:10 y:20 z:30 w:40",            4,   },
            {  L_,   "app:1 apple:2 application:3",    3,   },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE           = DATA[ti].d_line;
            const char *const SPEC           = DATA[ti].d_spec;
            const bsl::size_t EXPECTED_SIZE  = DATA[ti].d_expectedSize;
            const bool        EXPECTED_EMPTY = (EXPECTED_SIZE == 0);

            if (veryVerbose) {
                T_ P_(LINE) P_(SPEC) P_(EXPECTED_SIZE) P(EXPECTED_EMPTY)
            }

            bslma::TestAllocator ta("test", veryVeryVeryVerbose);
            IntTree              mX(&ta);
            const IntTree&       X = mX;

            ASSERTV(LINE, 0 == gg(&mX, SPEC, veryVerbose));

            // Test size() and empty()
            ASSERTV(LINE, EXPECTED_SIZE, X.size(),
                    EXPECTED_SIZE == X.size());
            ASSERTV(LINE, EXPECTED_EMPTY, X.empty(),
                    EXPECTED_EMPTY == X.empty());

            // Test get_allocator()
            ASSERTV(LINE, X.get_allocator() == bsl::allocator<>(&ta));
        }

        if (verbose) cout << "\nTest find() and contains() accessors\n";

        static const struct {
            int         d_line;
            const char *d_spec;
            const char *d_findKey;
            bool        d_shouldExist;
            int         d_expectedValue;  // Only meaningful if d_shouldExist
        } FIND_DATA[] = {
            // LINE  SPEC                  KEY      EXISTS  VALUE
            // ----  --------------------  -------  ------  -----
            {  L_,   "",                   "any",   false,  0     },
            {  L_,   "a:1",                "a",     true,   1     },
            {  L_,   "a:1",                "b",     false,  0     },
            {  L_,   "foo:42",             "foo",   true,   42    },
            {  L_,   "foo:42",             "bar",   false,  0     },
            {  L_,   "foo:42",             "fo",    false,  0     },
            {  L_,   "foo:42",             "fooo",  false,  0     },
            {  L_,   "a:1 ab:2 abc:3",     "a",     true,   1     },
            {  L_,   "a:1 ab:2 abc:3",     "ab",    true,   2     },
            {  L_,   "a:1 ab:2 abc:3",     "abc",   true,   3     },
            {  L_,   "a:1 ab:2 abc:3",     "abcd",  false,  0     },
            {  L_,   "a:1 ab:2 abc:3",     "x",     false,  0     },
        };
        const bsl::size_t NUM_FIND_DATA = sizeof FIND_DATA / sizeof *FIND_DATA;

        for (bsl::size_t ti = 0; ti < NUM_FIND_DATA; ++ti) {
            const int         LINE  = FIND_DATA[ti].d_line;
            const char *const SPEC  = FIND_DATA[ti].d_spec;
            const char *const KEY   = FIND_DATA[ti].d_findKey;
            const bool        EXISTS= FIND_DATA[ti].d_shouldExist;
            const int         VALUE = FIND_DATA[ti].d_expectedValue;

            if (veryVerbose) {
                T_ P_(LINE) P_(SPEC) P_(KEY) P_(EXISTS) P(VALUE)
            }

            bslma::TestAllocator ta("test", veryVeryVeryVerbose);
            IntTree              mX(&ta);
            const IntTree&       X = mX;

            ASSERTV(LINE, 0 == gg(&mX, SPEC, veryVerbose));

            // Test contains()
            ASSERTV(LINE, KEY, EXISTS, X.contains(KEY),
                    EXISTS == X.contains(KEY));

            // Test find() const
            IntTree::OptValueCRef result = X.find(KEY);
            ASSERTV(LINE, KEY, EXISTS, result.has_value(),
                    EXISTS == result.has_value());
            if (EXISTS) {
                ASSERTV(LINE, KEY, VALUE, result->get(),
                        VALUE == result->get());
            }

            // Test find() non-const
            IntTree::OptValueRef mutableResult = mX.find(KEY);
            ASSERTV(LINE, KEY, EXISTS, mutableResult.has_value(),
                    EXISTS == mutableResult.has_value());
            if (EXISTS) {
                ASSERTV(LINE, KEY, VALUE, mutableResult->get(),
                        VALUE == mutableResult->get());
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS TEST
        //
        // Concerns:
        // 1. Objects can be created with the default and allocator
        //    constructors.
        // 2. `emplace` correctly adds entries and returns true for new keys.
        // 3. `emplace` returns false when trying to insert a duplicate key.
        // 4. `erase` removes entries and returns true when key exists.
        // 5. `erase` returns false when key doesn't exist.
        // 6. `clear` removes all entries.
        // 7. The destructor properly cleans up resources.
        //
        // Plan:
        // 1. Create objects using both constructors.
        // 2. Test `emplace`, `erase`, and `clear` in various scenarios.
        // 3. Verify the container state after each operation.
        //
        // Testing:
        //   CategoryManager_RadixTree();
        //   CategoryManager_RadixTree(const allocator_type&);
        //   ~CategoryManager_RadixTree();
        //   pair<bool, t_VALUE&> emplace(const bsl::string_view&, Args&&...);
        //   bool erase(const bsl::string_view&);
        //   void clear();
        // --------------------------------------------------------------------

        if (verbose) cout << "PRIMARY MANIPULATORS TEST\n"
                             "=========================\n";

        {
            bslma::TestAllocator ta("test", veryVeryVeryVerbose);
            IntTree              mX(&ta);

            ASSERT(mX.empty());

            IntTree::EmplaceResult emplaceResult = mX.emplace("hello", 42);
            ASSERT(da.numBlocksInUse() == 0);
            ASSERT(emplaceResult.first);
            ASSERT(emplaceResult.second == 42);
            ASSERT(mX.size() == 1);
            ASSERT(da.numBlocksInUse() == 0);

            // Try to insert duplicate
            emplaceResult = mX.emplace("hello", 100);
            ASSERT(da.numBlocksInUse() == 0);
            ASSERT(!emplaceResult.first);
            ASSERT(emplaceResult.second == 42);  // Returns existing value
            ASSERT(mX.size() == 1);

            IntTree::OptValueRef result = mX.find("hello");
            ASSERT(da.numBlocksInUse() == 0);
            ASSERT(result && result->get() == 42);
            ASSERT(da.numBlocksInUse() == 0);

            // Erase existing key
            bool erased = mX.erase("hello");
            ASSERT(da.numBlocksInUse() == 0);
            ASSERT(erased);
            ASSERT(mX.empty());
            ASSERT(da.numBlocksInUse() == 0);

            // Erase non-existent key
            erased = mX.erase("hello");
            ASSERT(da.numBlocksInUse() == 0);
            ASSERT(!erased);

            // Test clear
            mX.emplace("foo", 1);
            ASSERT(da.numBlocksInUse() == 0);
            mX.emplace("bar", 2);
            ASSERT(da.numBlocksInUse() == 0);
            mX.emplace("baz", 3);
            ASSERT(da.numBlocksInUse() == 0);
            ASSERT(mX.size() == 3);
            ASSERT(da.numBlocksInUse() == 0);

            mX.clear();
            ASSERT(da.numBlocksInUse() == 0);
            ASSERT(mX.empty());
            ASSERT(da.numBlocksInUse() == 0);
            ASSERT(mX.size() == 0);
            ASSERT(da.numBlocksInUse() == 0);
        }

#ifdef BDE_BUILD_TARGET_EXC
        {  // Exception testing of `emplace`.

            bslma::TestAllocator xa("exc", veryVeryVeryVerbose);

            StringTree tree(&xa);
            // Try to insert multiple keys with exception testing
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(xa) {
                tree.emplace("addition", "value");
                tree.emplace("adage",    "second"); // Common root w. addition
                tree.emplace("babbage",  "third");
                tree.emplace("fourth",   "fourth");
                tree.emplace("",         "empty");  // This should go into root
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            // After test, tree should contain all keys and values
            StringTree::OptValueRef result = tree.find("addition");
            ASSERT(result && result->get() == "value");
            result = tree.find("adage");
            ASSERT(result && result->get() == "second");
            result = tree.find("babbage");
            ASSERT(result && result->get() == "third");
            result = tree.find("fourth");
            ASSERT(result && result->get() == "fourth");
            result = tree.find("");
            ASSERT(result && result->get() == "empty");

            // Verify size/structure
            ASSERTV(tree.size(),       tree.size()       == 5);
            ASSERTV(tree.countNodes(), tree.countNodes() == 6);
            // Plus one node for root and for "addition"/"adage" common prefix
        }

        {
            // Test exception safety of `erase` with merging.
            // We need to re-insert the elements of the tree in each exception
            // test iteration because `erase` is destructive.

            bslma::TestAllocator xa("exc", veryVeryVeryVerbose);
            StringTree           eraseTree(&xa);
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(xa) {
                // Rebuild tree with structure that requires merging on erase
                eraseTree.emplace("addition",  "addition_value");
                eraseTree.emplace("adage",     "adage_value");
                eraseTree.emplace("babbage",   "babbage_value");
                eraseTree.emplace("fourth",    "fourth_value");
                eraseTree.emplace("test",      "test_value");
                eraseTree.emplace("testing",   "testing_value");
                eraseTree.emplace("temporary", "temporary_value");
                eraseTree.emplace("",          "empty_value");

                ASSERTV(eraseTree.size(),       eraseTree.size()       == 8);
                ASSERTV(eraseTree.countNodes(), eraseTree.countNodes() == 10);

                // Erase "temporary" - should trigger merge of "te" node with
                // "st" because "te" has no value and both children start with
                // "st".  The "emporary" node is removed, the "te" node is
                // merged with "st" into "test", which means that it "becomes"
                // the "test" node with the "test_value", so that node is also
                // deleted.
                bool erased = eraseTree.erase("temporary");
                ASSERTV(erased, erased == true);

                ASSERTV(eraseTree.size(),       eraseTree.size()       == 7);
                ASSERTV(eraseTree.countNodes(), eraseTree.countNodes() == 8);

                ASSERT( eraseTree.contains("addition"));
                ASSERT( eraseTree.contains("adage"));
                ASSERT( eraseTree.contains("babbage"));
                ASSERT( eraseTree.contains("fourth"));
                ASSERT( eraseTree.contains("test"));
                ASSERT( eraseTree.contains("testing"));
                ASSERT(!eraseTree.contains("temporary"));
                ASSERT( eraseTree.contains(""));

                // Erase "test" - should trigger "move" of the "testing" node
                // up in the tree, so just one node and value is deleted.
                erased = eraseTree.erase("test");
                ASSERTV(erased, erased == true);

                ASSERTV(eraseTree.size(),       eraseTree.size()       == 6);
                ASSERTV(eraseTree.countNodes(), eraseTree.countNodes() == 7);
                // The "test" node is with the "ing" node, the "ing" node
                // removed.

                ASSERT( eraseTree.contains("addition"));
                ASSERT( eraseTree.contains("adage"));
                ASSERT( eraseTree.contains("babbage"));
                ASSERT( eraseTree.contains("fourth"));
                ASSERT(!eraseTree.contains("test"));
                ASSERT( eraseTree.contains("testing"));
                ASSERT(!eraseTree.contains("temporary"));
                ASSERT( eraseTree.contains(""));

                // Erase "addition" - should trigger "move" of the "adage" node
                // up in the tree, so just one node and value is deleted.
                erased = eraseTree.erase("addition");
                ASSERTV(erased, erased == true);

                ASSERTV(eraseTree.size(),       eraseTree.size()       == 5);
                ASSERTV(eraseTree.countNodes(), eraseTree.countNodes() == 5);
                // The "ad" node merged with the "age" node, the "age" is node
                // removed, so is the "dittion" node.

                ASSERT(!eraseTree.contains("addition"));
                ASSERT( eraseTree.contains("adage"));
                ASSERT( eraseTree.contains("babbage"));
                ASSERT( eraseTree.contains("fourth"));
                ASSERT(!eraseTree.contains("test"));
                ASSERT( eraseTree.contains("testing"));
                ASSERT(!eraseTree.contains("temporary"));
                ASSERT( eraseTree.contains(""));

                // Erase "fourth" - should delete one node and value.
                erased = eraseTree.erase("fourth");
                ASSERTV(erased, erased == true);

                ASSERTV(eraseTree.size(),       eraseTree.size()       == 4);
                ASSERTV(eraseTree.countNodes(), eraseTree.countNodes() == 4);

                ASSERT(!eraseTree.contains("addition"));
                ASSERT( eraseTree.contains("adage"));
                ASSERT( eraseTree.contains("babbage"));
                ASSERT(!eraseTree.contains("fourth"));
                ASSERT(!eraseTree.contains("test"));
                ASSERT( eraseTree.contains("testing"));
                ASSERT(!eraseTree.contains("temporary"));
                ASSERT( eraseTree.contains(""));

                // Erase "testing" - should delete one node and value.
                erased = eraseTree.erase("testing");
                ASSERTV(erased, erased == true);

                ASSERTV(eraseTree.size(),       eraseTree.size()       == 3);
                ASSERTV(eraseTree.countNodes(), eraseTree.countNodes() == 3);

                ASSERT(!eraseTree.contains("addition"));
                ASSERT( eraseTree.contains("adage"));
                ASSERT( eraseTree.contains("babbage"));
                ASSERT(!eraseTree.contains("fourth"));
                ASSERT(!eraseTree.contains("test"));
                ASSERT(!eraseTree.contains("testing"));
                ASSERT(!eraseTree.contains("temporary"));
                ASSERT( eraseTree.contains(""));

                // Erase "" - should delete no node and one value.
                erased = eraseTree.erase("");
                ASSERTV(erased, erased == true);

                ASSERTV(eraseTree.size(),       eraseTree.size()       == 2);
                ASSERTV(eraseTree.countNodes(), eraseTree.countNodes() == 3);

                ASSERT(!eraseTree.contains("addition"));
                ASSERT( eraseTree.contains("adage"));
                ASSERT( eraseTree.contains("babbage"));
                ASSERT(!eraseTree.contains("fourth"));
                ASSERT(!eraseTree.contains("test"));
                ASSERT(!eraseTree.contains("testing"));
                ASSERT(!eraseTree.contains("temporary"));
                ASSERT(!eraseTree.contains(""));

                // Erase "babbage" - should delete one node and value.
                erased = eraseTree.erase("babbage");
                ASSERTV(erased, erased == true);

                ASSERTV(eraseTree.size(),       eraseTree.size()       == 1);
                ASSERTV(eraseTree.countNodes(), eraseTree.countNodes() == 2);

                ASSERT(!eraseTree.contains("addition"));
                ASSERT( eraseTree.contains("adage"));
                ASSERT(!eraseTree.contains("babbage"));
                ASSERT(!eraseTree.contains("fourth"));
                ASSERT(!eraseTree.contains("test"));
                ASSERT(!eraseTree.contains("testing"));
                ASSERT(!eraseTree.contains("temporary"));
                ASSERT(!eraseTree.contains(""));

                // Erase "adage" - should delete one node and value.
                erased = eraseTree.erase("adage");
                ASSERTV(erased, erased == true);

                ASSERTV(eraseTree.size(),       eraseTree.size()       == 0);
                ASSERTV(eraseTree.countNodes(), eraseTree.countNodes() == 1);
                // Only the root node remains, without a value
                ASSERT(eraseTree.empty());

                ASSERT(!eraseTree.contains("addition"));
                ASSERT(!eraseTree.contains("adage"));
                ASSERT(!eraseTree.contains("babbage"));
                ASSERT(!eraseTree.contains("fourth"));
                ASSERT(!eraseTree.contains("test"));
                ASSERT(!eraseTree.contains("testing"));
                ASSERT(!eraseTree.contains("temporary"));
                ASSERT(!eraseTree.contains(""));
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        }

        {
            // Test allocator-aware type with no default constructor
            if (verbose)
                cout << "\tTesting AllocAwareType (allocator-aware, "
                        "no default ctor)\n";

            bslma::TestAllocator ta("alloc-aware", veryVeryVeryVerbose);
            AllocAwareTree       mX(&ta);

            ASSERT(mX.empty());

            // Emplace with constructor arguments (no allocator passed)
            AllocAwareTree::EmplaceResult result =
                mX.emplace("key1", "first", "second");
            ASSERT(result.first);
            ASSERT(result.second.get().first() == "first");
            ASSERT(result.second.get().second() == "second");
            ASSERT(result.second.get().allocator() == &ta);
            ASSERT(mX.size() == 1);

            // Emplace another entry
            result = mX.emplace("key2", "alpha", "beta");
            ASSERT(result.first);
            ASSERT(result.second.get().first() == "alpha");
            ASSERT(result.second.get().second() == "beta");
            ASSERT(result.second.get().allocator() == &ta);
            ASSERT(mX.size() == 2);

            // Verify find returns correct values
            AllocAwareTree::OptValueRef found = mX.find("key1");
            ASSERT(found && found->get().first() == "first");
            ASSERT(found && found->get().second() == "second");
            ASSERT(found && found->get().allocator() == &ta);

            found = mX.find("key2");
            ASSERT(found && found->get().first() == "alpha");
            ASSERT(found && found->get().second() == "beta");
            ASSERT(found && found->get().allocator() == &ta);

            // Erase and verify
            bool erased = mX.erase("key1");
            ASSERT(erased);
            ASSERT(mX.size() == 1);
            ASSERT(!mX.find("key1"));
            ASSERT(mX.find("key2"));

            mX.clear();
            ASSERT(mX.empty());
        }

        {
            // Test non-allocator-aware type with no default constructor
            if (verbose)
                cout << "\tTesting NonAllocType (non-allocator-aware, "
                        "no default ctor)\n";

            bslma::TestAllocator ta("non-alloc", veryVeryVeryVerbose);
            NonAllocTree         mX(&ta);

            ASSERT(mX.empty());

            // Emplace with constructor arguments
            NonAllocTree::EmplaceResult result =
                mX.emplace("key1", 10, 20, 30);
            ASSERT(result.first);
            ASSERT(result.second.get().first() == 10);
            ASSERT(result.second.get().second() == 20);
            ASSERT(result.second.get().third() == 30);
            ASSERT(mX.size() == 1);

            // Emplace another entry
            result = mX.emplace("key2", 100, 200, 300);
            ASSERT(result.first);
            ASSERT(result.second.get().first() == 100);
            ASSERT(result.second.get().second() == 200);
            ASSERT(result.second.get().third() == 300);
            ASSERT(mX.size() == 2);

            // Verify find returns correct values
            NonAllocTree::OptValueRef found = mX.find("key1");
            ASSERT(found && found->get().first() == 10);
            ASSERT(found && found->get().second() == 20);
            ASSERT(found && found->get().third() == 30);

            found = mX.find("key2");
            ASSERT(found && found->get().first() == 100);
            ASSERT(found && found->get().second() == 200);
            ASSERT(found && found->get().third() == 300);

            // Erase and verify
            bool erased = mX.erase("key1");
            ASSERT(erased);
            ASSERT(mX.size() == 1);
            ASSERT(!mX.find("key1"));
            ASSERT(mX.find("key2"));

            mX.clear();
            ASSERT(mX.empty());
        }

        // Test single-character keys
        {
            if (veryVerbose) cout << "\tSingle-character keys\n";

            bslma::TestAllocator ta("test", veryVeryVeryVerbose);
            IntTree              mX(&ta);

            mX.emplace("a",  1);
            mX.emplace("b",  2);
            mX.emplace("c",  3);
            mX.emplace("ab", 4);
            mX.emplace("ac", 5);

            ASSERT(mX.size() == 5);

            IntTree::OptValueRef result = mX.find("a");
            ASSERT(result && result->get() == 1);
            result = mX.find("b");
            ASSERT(result && result->get() == 2);
            result = mX.find("c");
            ASSERT(result && result->get() == 3);
            result = mX.find("ab");
            ASSERT(result && result->get() == 4);
            result = mX.find("ac");
            ASSERT(result && result->get() == 5);

            // Erase single-char key and verify longer keys still work
            bool erased = mX.erase("a");
            ASSERT(erased);
            ASSERT(mX.size() == 4);
            ASSERT(!mX.find("a"));
            result = mX.find("ab");
            ASSERT(result && result->get() == 4);
            result = mX.find("ac");
            ASSERT(result && result->get() == 5);
        }

        // Test deep nesting: keys that are prefixes of each other
        {
            if (veryVerbose) cout << "\tDeep nesting scenario\n";

            bslma::TestAllocator ta("test", veryVeryVeryVerbose);
            IntTree              mX(&ta);

            mX.emplace("a",     1);
            mX.emplace("ab",    2);
            mX.emplace("abc",   3);
            mX.emplace("abcd",  4);
            mX.emplace("abcde", 5);

            ASSERT(mX.size() == 5);

            IntTree::OptValueRef result = mX.find("a");
            ASSERT(result && result->get() == 1);
            result = mX.find("ab");
            ASSERT(result && result->get() == 2);
            result = mX.find("abc");
            ASSERT(result && result->get() == 3);
            result = mX.find("abcd");
            ASSERT(result && result->get() == 4);
            result = mX.find("abcde");
            ASSERT(result && result->get() == 5);

            // Test that partial matches don't return results
            result = mX.find("abcdef");
            ASSERT(!result);
            result = mX.find("abcdefg");
            ASSERT(!result);

            // Erase middle element and verify others still work
            bool erased = mX.erase("abc");
            ASSERT(erased);
            ASSERT(mX.size() == 4);
            ASSERT(!mX.find("abc"));
            result = mX.find("ab");
            ASSERT(result && result->get() == 2);
            result = mX.find("abcd");
            ASSERT(result && result->get() == 4);
        }

        // Test very long keys
        {
            if (veryVerbose) cout << "\tVery long keys\n";

            bslma::TestAllocator ta("test", veryVeryVeryVerbose);
            StringTree           mX(&ta);

            bsl::string longKey1(1000, 'a');
            bsl::string longKey2(1000, 'a');
            longKey2[999] = 'b';  // Different at end
            bsl::string longKey3(1000, 'b');

            mX.emplace(longKey1, "value1");
            mX.emplace(longKey2, "value2");
            mX.emplace(longKey3, "value3");

            ASSERT(mX.size() == 3);

            StringTree::OptValueRef result = mX.find(longKey1);
            ASSERT(result && result->get() == "value1");
            result = mX.find(longKey2);
            ASSERT(result && result->get() == "value2");
            result = mX.find(longKey3);
            ASSERT(result && result->get() == "value3");

            // Erase and verify
            bool erased = mX.erase(longKey1);
            ASSERT(erased);
            ASSERT(mX.size() == 2);
            ASSERT(!mX.find(longKey1));
            result = mX.find(longKey2);
            ASSERT(result && result->get() == "value2");
        }
#endif  // BDE_BUILD_TARGET_EXC
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        // 1. The class is sufficiently functional to enable comprehensive
        //    testing in subsequent test cases.
        //
        // Plan:
        // 1. Create a modifiable object (default constructor).
        // 2. Insert an entry and verify it can be found.
        // 3. Erase the entry and verify it's gone.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "BREATHING TEST\n"
                             "==============\n";
        {
            IntTree tree;

            tree.emplace("test", 42);

            IntTree::OptValueRef result = tree.find("test");
            ASSERT(result && result->get() == 42);

            tree.erase("test");
            result = tree.find("test");
            ASSERT(!result);
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

// ----------------------------------------------------------------------------
// Copyright 2025 Bloomberg Finance L.P.
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
//


