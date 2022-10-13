// bslstl_vector.3.t.cpp                                              -*-C++-*-
#define BSLSTL_VECTOR_0T_AS_INCLUDE
#include <bslstl_vector.0.t.cpp>

#include <bslh_hash.h>

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
// See the test plan in 'bslstl_vector.0.t.cpp'.

// ============================================================================
//        COMPILER CONFIGURATION AND WORKAROUNDS FOR TEST DRIVER PART 3
// ----------------------------------------------------------------------------

#if defined(BSLS_COMPILERFEATURES_SIMULATE_FORWARD_WORKAROUND)
# define BSL_DO_NOT_TEST_MOVE_FORWARDING 1
// Some compilers produce ambiguities when trying to construct our test types
// for 'emplace'-type functionality with the C++03 move-emulation.  This is a
// compiler bug triggering in lower level components, so we simply disable
// those aspects of testing, and rely on the extensive test coverage on other
// platforms.
#endif

#if defined(BSLS_PLATFORM_CMP_SUN) && BSLS_PLATFORM_CMP_VERSION >= 0x5130
// Some compilers struggle with the number of template instantiations in this
// test driver.  We define this macro to simplify the test driver for them,
// until such time as we can provide a more specific review of the type based
// concerns, and narrow the range of tests needed for confirmed coverage.
//
// Currently we are enabling the minimal set of test types on:
// Sun Studio 12.4            (CMP_SUN)
// (note: despite over-eager version check, we have not tested later compilers)
# define BSLSTL_VECTOR_TEST_LOW_MEMORY  1
#endif

#if defined(BSLSTL_VECTOR_TEST_LOW_MEMORY)
// For platforms that cannot sustain the full set of test concerns, reduce the
// number of elements in the most commonly use macro defining sets of test
// tyoes.
# undef  BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR
# define BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR  \
        signed char,                                    \
        bsltf::TemplateTestFacility::MethodPtr,         \
        bsltf::AllocBitwiseMoveableTestType,            \
        bsltf::MovableAllocTestType
#endif

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_BOOL_CONSTANT)
# define DECLARE_BOOL_CONSTANT(NAME, EXPRESSION)                              \
    const BSLS_KEYWORD_CONSTEXPR bsl::bool_constant<EXPRESSION> NAME{}
    // This leading branch is the preferred version for C++17, but the feature
    // test macro is (currently) for documentation purposes only, and never
    // defined.  This is the ideal (simplest) form for such declarations:
#elif defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR)
# define DECLARE_BOOL_CONSTANT(NAME, EXPRESSION)                              \
    constexpr bsl::integral_constant<bool, EXPRESSION> NAME{}
    // This is the preferred C++11 form for the definition of integral constant
    // variables.  It assumes the presence of 'constexpr' in the compiler as an
    // indication that brace-initialization and traits are available, as it has
    // historically been one of the last C++11 features to ship.
#else
# define DECLARE_BOOL_CONSTANT(NAME, EXPRESSION)                              \
    static const bsl::integral_constant<bool, EXPRESSION> NAME =              \
                 bsl::integral_constant<bool, EXPRESSION>()
    // 'bsl::integral_constant' is not an aggregate prior to C++17 extending
    // the rules, so a C++03 compiler must explicitly initialize integral
    // constant variables in a way that is unambiguously not a vexing parse
    // that declares a function instead.
#endif

//=============================================================================
//                  GLOBAL CONSTANTS FOR TEST DRIVER PART 3
//-----------------------------------------------------------------------------

// Define values used to initialize positional arguments for
// 'bsltf::EmplacableTestType' and 'bsltf::AllocEmplacableTestType'
// constructors.  Note, that you cannot change those values as they are used by
// 'TemplateTestFacility::getIdentifier' to map the constructed emplacable
// objects to their integer identifiers.
static const int V01 = 1;
static const int V02 = 20;
static const int V03 = 23;
static const int V04 = 44;
static const int V05 = 66;
static const int V06 = 176;
static const int V07 = 878;
static const int V08 = 8;
static const int V09 = 912;
static const int V10 = 102;

//=============================================================================
//                   HELPER CLASSES FOR TEST DRIVER PART 3
//-----------------------------------------------------------------------------

                        // ========================
                        // class FixedArrayIterator
                        // ========================

// FORWARD DECLARATIONS
template <class TYPE>
class FixedArray;

template <class TYPE>
class FixedArrayIterator {
    // This testing class provides simple, fixed size array with a non-pointer
    // iterator for use in test case 31.

    // DATA
    FixedArray<TYPE> *d_array_p;
    int               d_index;

  public:
    typedef std::forward_iterator_tag  iterator_category;
    typedef int                        difference_type;
    typedef int                        size_type;
    typedef TYPE                       value_type;
    typedef TYPE                      *pointer;
    typedef TYPE&                      reference;

    // CREATORS
    FixedArrayIterator(const FixedArray<TYPE> *array, int index);

    FixedArrayIterator(const FixedArrayIterator<TYPE>& original);

    // MANIPULATORS
    FixedArrayIterator<TYPE>& operator++();

    // ACCESSORS
    TYPE& operator*() const;

    bool isEqual(const FixedArrayIterator<TYPE>& obj) const;
};

// FREE OPERATORS
template <class TYPE>
bool operator==(const FixedArrayIterator<TYPE>& lhs,
                const FixedArrayIterator<TYPE>& rhs);

#ifndef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON
template <class TYPE>
bool operator!=(const FixedArrayIterator<TYPE>& lhs,
                const FixedArrayIterator<TYPE>& rhs);
#endif

// CREATORS
template <class TYPE>
FixedArrayIterator<TYPE>::FixedArrayIterator(const FixedArray<TYPE> *array,
                                             int                     index)
: d_array_p(const_cast<FixedArray<TYPE> *>(array))
, d_index(index)
{
}

template <class TYPE>
FixedArrayIterator<TYPE>::FixedArrayIterator(
                                      const FixedArrayIterator<TYPE>& original)
: d_array_p(original.d_array_p)
, d_index(  original.d_index)
{
}

// MANIPULATORS
template <class TYPE>
FixedArrayIterator<TYPE>& FixedArrayIterator<TYPE>::operator++()
{
    ++d_index;
    return *this;
}

// ACCESSORS
template <class TYPE>
TYPE& FixedArrayIterator<TYPE>::operator*() const
{
    return d_array_p->operator[](d_index);
}

template <class TYPE>
bool FixedArrayIterator<TYPE>::isEqual(
                                     const FixedArrayIterator<TYPE>& obj) const
{
    return (d_array_p == obj.d_array_p && d_index == obj.d_index);
}

// FREE OPERATORS
template <class TYPE>
bool operator==(const FixedArrayIterator<TYPE>& lhs,
                const FixedArrayIterator<TYPE>& rhs)
{
    return lhs.isEqual(rhs);
}

#ifndef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON
template <class TYPE>
bool operator!=(const FixedArrayIterator<TYPE>& lhs,
                const FixedArrayIterator<TYPE>& rhs)
{
    return !lhs.isEqual(rhs);
}
#endif

                              // ================
                              // class FixedArray
                              // ================

static const size_t k_FIXED_ARRAY_SIZE = 100;

template <class TYPE>
class FixedArray {
    // This testing class provides simple, fixed size array with a non-pointer
    // iterator for use in test case 31.

  public:
    // PUBLIC TYPES
    typedef FixedArrayIterator<TYPE> iterator;

  private:
    // DATA
    TYPE d_data[k_FIXED_ARRAY_SIZE];
    int  d_length;

  public:
    // CREATORS
    FixedArray();

    // MANIPULATORS
    TYPE& operator[](int index);

    void push_back(const TYPE& value);

    // ACCESSORS
    iterator begin() const;

    iterator end() const;
};

                              // ----------------
                              // class FixedArray
                              // ----------------

// CREATORS
template <class TYPE>
FixedArray<TYPE>::FixedArray()
: d_length(0)
{
}

// MANIPULATORS
template <class TYPE>
TYPE& FixedArray<TYPE>::operator[](int index)
{
    return d_data[index];
}

template <class TYPE>
void FixedArray<TYPE>::push_back(const TYPE& value)
{
    d_data[d_length++] = value;
}

// ACCESSORS
template <class TYPE>
typename FixedArray<TYPE>::iterator FixedArray<TYPE>::begin() const
{
    return typename FixedArray<TYPE>::iterator(this, 0);
}

template <class TYPE>
typename FixedArray<TYPE>::iterator FixedArray<TYPE>::end() const
{
    return typename FixedArray<TYPE>::iterator(this, d_length);
}

                        // =======================
                        // class TestAllocatorUtil
                        // =======================

class TestAllocatorUtil
{
  public:
    template <class TYPE>
    static void test(const TYPE&, const bslma::Allocator&)
    {
    }

    static void test(const bsltf::AllocEmplacableTestType& value,
                     const bslma::Allocator&               oa)
    {
        ASSERTV(&oa == value.arg01().allocator());
        ASSERTV(&oa == value.arg02().allocator());
        ASSERTV(&oa == value.arg03().allocator());
        ASSERTV(&oa == value.arg04().allocator());
        ASSERTV(&oa == value.arg05().allocator());
        ASSERTV(&oa == value.arg06().allocator());
        ASSERTV(&oa == value.arg07().allocator());
        ASSERTV(&oa == value.arg08().allocator());
        ASSERTV(&oa == value.arg09().allocator());
        ASSERTV(&oa == value.arg10().allocator());
    }
};

                             // ============
                             // class EqPred
                             // ============

template <class TYPE>
struct EqPred
    // A predicate for testing 'erase_if'; it takes a value at construction
    // and uses it for comparisons later.
{
    TYPE d_ch;
    EqPred(TYPE ch) : d_ch(ch) {}

    bool operator() (TYPE ch) const
        // return 'true' if the specified 'ch' is equal to the stored value,
        // and 'false' otherwise.
    {
        return d_ch == ch;
    }
};

                     // ==================================
                     // class IncompleteTypeSupportChecker
                     // ==================================

class IncompleteTypeSupportChecker {
    // This class tests that 'bsl::vector' can be instantiated using an
    // incomplete type and that methods within that incomplete type can
    // reference such a 'vector'.  The interface completely copies the existing
    // 'bsl::vector' interface and the only purpose of the functions is to call
    // the corresponding methods of the vector parameterized by the incomplete
    // type.  Each method increases its own invocation counter so we can make
    // sure that each method is compiled and called.

    // PRIVATE TYPES
    typedef BloombergLP::bslmf::MovableRefUtil                      MoveUtil;
    typedef BloombergLP::bslmf::MovableRef<IncompleteTypeSupportChecker>
                                                                    MovableRef;

  public:
    // TYPES
    typedef bsl::vector<IncompleteTypeSupportChecker> VectorType;
    typedef VectorType::value_type                    value_type;
    typedef VectorType::size_type                     size_type;
    typedef VectorType::allocator_type                allocator_type;
    typedef VectorType::iterator                      iterator;
    typedef VectorType::const_iterator                const_iterator;
    typedef VectorType::reverse_iterator              reverse_iterator;
    typedef VectorType::const_reverse_iterator        const_reverse_iterator;
    typedef VectorType::reference                     reference;
    typedef VectorType::const_reference               const_reference;

  private:
    // CLASS DATA
    static int const s_numFunctions;            // number of public methods
    static int       s_functionCallCounters[];  // number of times each
                                                // public method is called
    // DATA
    VectorType d_vector;  // underlying vector parameterized with incomplete
                          // type

  public:
    // CLASS METHODS
    static void checkInvokedFunctions();
        // Check that all public functions have been called at least once.

    static void increaseFunctionCallCounter(std::size_t index);
        // Increase the call count of function with the specified 'index' by
        // one.

    static void resetFunctionCallCounters();
        // Reset the call count of each function to zero.

    // CREATORS
    IncompleteTypeSupportChecker()
        // Call 'bsl::vector' default constructor implicitly.
        //
        // NOTE: This function has been implemented inline to preserve
        // 'IncompleteTypeSupportChecker' incompleteness at compile time.
    {
        s_functionCallCounters[0]++;
    }

    explicit IncompleteTypeSupportChecker(const allocator_type& basicAllocator)
        // Call 'bsl::vector' constructor passing the specified
        // 'basicAllocator' as a parameter.
    : d_vector(basicAllocator)
    {
        s_functionCallCounters[1]++;
    }

    explicit IncompleteTypeSupportChecker(size_type             initialSize,
                                          const allocator_type& basicAllocator
                                                            = allocator_type())
        // Call 'bsl::vector' constructor passing the specified 'initialSize'
        // and 'basicAllocator' as parameters.
    : d_vector(initialSize, basicAllocator)
    {
        s_functionCallCounters[2]++;
    }

    IncompleteTypeSupportChecker(
                            size_type                           initialSize,
                            const IncompleteTypeSupportChecker& value,
                            const allocator_type&               basicAllocator
                                                            = allocator_type())
        // Call 'bsl::vector' constructor passing the specified 'initialSize',
        // 'value', and 'basicAllocator' as parameters.
    : d_vector(initialSize, value, basicAllocator)
    {
        s_functionCallCounters[3]++;
    }

    template <class INPUT_ITER>
    IncompleteTypeSupportChecker(INPUT_ITER            first,
                                 INPUT_ITER            last,
                                 const allocator_type& basicAllocator
                                                            = allocator_type())
        // Call 'bsl::vector' constructor passing the specified 'first',
        // 'last', and 'basicAllocator' as parameters.
    : d_vector(first, last, basicAllocator)
    {
        s_functionCallCounters[4]++;
    }

    IncompleteTypeSupportChecker(const IncompleteTypeSupportChecker& original)
        // Call 'bsl::vector' constructor passing the underlying vector of the
        // specified 'original' as a parameter.
    : d_vector(original.d_vector)
    {
        s_functionCallCounters[5]++;
    }

    IncompleteTypeSupportChecker(
                            const IncompleteTypeSupportChecker& original,
                            const allocator_type&               basicAllocator)
        // Call 'bsl::vector' constructor passing the underlying vector of the
        // specified 'original' and the specified 'basicAllocator' as
        // parameters.
    : d_vector(original.d_vector, basicAllocator)
    {
        s_functionCallCounters[6]++;
    }

    IncompleteTypeSupportChecker(MovableRef original)
        // Call 'bsl::vector' constructor passing the underlying vector of the
        // specified movable 'original' as a parameter.
    : d_vector(MoveUtil::move(MoveUtil::access(original).d_vector))
    {
        s_functionCallCounters[7]++;
    }

    IncompleteTypeSupportChecker(MovableRef            original,
                                 const allocator_type& basicAllocator)
        // Call 'bsl::vector' constructor passing the specified
        // 'basicAllocator' and passing the underlying vector of the specified
        // movable 'original' as parameters.
    : d_vector(MoveUtil::move(MoveUtil::access(original).d_vector),
               basicAllocator)
    {
        s_functionCallCounters[8]++;
    }

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    IncompleteTypeSupportChecker(
            std::initializer_list<IncompleteTypeSupportChecker> values,
            const allocator_type&                               basicAllocator
                                                            = allocator_type())
        // Call 'bsl::vector' constructor passing the specified 'values' and
        // 'basicAllocator' as parameters.
    : d_vector(values, basicAllocator)
    {
        s_functionCallCounters[9]++;
    }
#endif

    ~IncompleteTypeSupportChecker()
        // Call 'bsl::vector' destructor.
    {
        s_functionCallCounters[10]++;
    }

    // MANIPULATORS
    IncompleteTypeSupportChecker& operator=(
                                       const IncompleteTypeSupportChecker& rhs)
        // Call the assignment operator of 'bsl::vector' passing the underlying
        // vector of the specified 'rhs' as a parameter, and return a reference
        // providing modifiable access to this object.
    {
        s_functionCallCounters[11]++;
        d_vector = rhs.d_vector;
        return *this;
    }

    IncompleteTypeSupportChecker& operator=(MovableRef rhs)
        // Call the assignment operator of 'bsl::vector' passing the underlying
        // vector of the specified movable 'rhs' as a parameter, and return a
        // reference providing modifiable access to this object.
    {
        s_functionCallCounters[12]++;
        d_vector = MoveUtil::move(MoveUtil::access(rhs).d_vector);
        return *this;
    }

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    IncompleteTypeSupportChecker& operator=(
                    std::initializer_list<IncompleteTypeSupportChecker> values)
        // Call the assignment operator of 'bsl::vector' passing the specified
        // 'values' as a parameter, and return a reference providing modifiable
        // access to this object.
    {
        s_functionCallCounters[13]++;
        d_vector = values;
        return *this;
    }

    void assign(std::initializer_list<IncompleteTypeSupportChecker> values)
        // Call the 'assign' method of 'bsl::vector' passing the specified
        // 'values' as a parameter.
    {
        s_functionCallCounters[14]++;
        d_vector.assign(values);
    }
#endif

    template <class INPUT_ITER>
    void assign(INPUT_ITER first, INPUT_ITER last)
        // Call the 'assign' method of 'bsl::vector' passing the specified
        // 'first' and 'last' as parameters.
    {
        s_functionCallCounters[15]++;
        d_vector.assign(first, last);
    }

    void assign(size_type                           numElements,
                const IncompleteTypeSupportChecker& value)
        // Call the 'assign' method of 'bsl::vector' passing the specified
        // 'numElements' and 'value' as parameters.
    {
        s_functionCallCounters[16]++;
        d_vector.assign(numElements, value);
    }

    iterator begin()
        // Return the result of calling the 'begin' manipulator of
        // 'bsl::vector'.
    {
        s_functionCallCounters[17]++;
        return d_vector.begin();
    }

    iterator end()
        // Return the result of calling the 'end' manipulator of 'bsl::vector'.
    {
        s_functionCallCounters[18]++;
        return d_vector.end();
    }

    reverse_iterator rbegin()
        // Return the result of calling the 'rbegin' manipulator of
        // 'bsl::vector'.
    {
        s_functionCallCounters[19]++;
        return d_vector.rbegin();
    }

    reverse_iterator rend()
        // Return the result of calling the 'rend' manipulator of
        // 'bsl::vector'.
    {
        s_functionCallCounters[20]++;
        return d_vector.rend();
    }

                          // *** element access ***

    reference operator[](size_type position)
        // Call the subscript operator of 'bsl::vector' passing the specified
        // 'position' as a parameter, and return the result.
    {
        s_functionCallCounters[21]++;
        return d_vector[position];
    }

    reference at(size_type position)
        // Call the 'at' manipulator of 'bsl::vector' passing the specified
        // 'position' as a parameter and return the result.
    {
        s_functionCallCounters[22]++;
        return d_vector.at(position);
    }

    reference front()
        // Return the result of calling the 'front' manipulator of
        // 'bsl::vector'.
    {
        s_functionCallCounters[23]++;
        return d_vector.front();
    }

    reference back()
        // Return the result of calling the 'back' manipulator of
        // 'bsl::vector'.
    {
        s_functionCallCounters[24]++;
        return d_vector.back();
    }

    IncompleteTypeSupportChecker *data()
        // Return the result of calling the 'data' manipulator of
        // 'bsl::vector'.
    {
        s_functionCallCounters[25]++;
        return d_vector.data();
    }

    void resize(size_type newSize)
        // Call the 'resize' method of 'bsl::vector' passing the specified
        // 'newSize' as a parameter.
    {
        s_functionCallCounters[26]++;
        d_vector.resize(newSize);
    }

    void resize(size_type newSize, const IncompleteTypeSupportChecker& value)
        // Call the 'resize' method of 'bsl::vector' passing the specified
        // 'newSize' and 'value' as parameters.
    {
        s_functionCallCounters[27]++;
        d_vector.resize(newSize, value);
    }

    void reserve(size_type newCapacity)
        // Call the 'reserve' method of 'bsl::vector' passing the specified
        // 'newCapacity' as a parameter.
    {
        s_functionCallCounters[28]++;
        d_vector.reserve(newCapacity);
    }

    void shrink_to_fit()
        // Call the 'shrink_to_fit' method of 'bsl::vector'.
    {
        s_functionCallCounters[29]++;
        d_vector.shrink_to_fit();
    }

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    template <class... Args>
    reference emplace_back(Args&&... arguments)
        // Call the 'emplace_back' method of 'bsl::vector' passing the
        // specified 'arguments' as a parameter, and return the result.
    {
        s_functionCallCounters[30]++;
        return d_vector.emplace_back(
                            BSLS_COMPILERFEATURES_FORWARD(Args, arguments)...);
    }
#else
    template <class Args_01,
              class Args_02>
    reference emplace_back(
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02)
        // Call the 'emplace_back' method of 'bsl::vector' passing the
        // specified 'arguments_01' and 'arguments_02' as parameters, and
        // return the result.
    {
        s_functionCallCounters[30]++;
        return d_vector.emplace_back(
                         BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                         BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02));
    }
#endif

    void push_back(const IncompleteTypeSupportChecker& value)
        // Call the 'push_back' method of 'bsl::vector' passing the specified
        // 'value' as a parameter.
    {
        s_functionCallCounters[31]++;
        d_vector.push_back(value);
    }

    void push_back(MovableRef value)
        // Call the 'push_back' method of 'bsl::vector' passing the specified
        // movable 'value' as a parameter.
    {
        s_functionCallCounters[32]++;
        IncompleteTypeSupportChecker& lvalue = value;
        d_vector.push_back(MoveUtil::move(lvalue));
    }

    void pop_back()
        // Call the 'pop_back' method of 'bsl::vector'.
    {
        s_functionCallCounters[33]++;
        d_vector.pop_back();
    }

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    template <class... Args>
    iterator emplace(const_iterator position, Args&&... arguments)
        // Call the 'emplace' method of 'bsl::vector' passing the specified
        // 'position' and 'arguments' as parameters, and return the result.
    {
        s_functionCallCounters[34]++;
        return d_vector.emplace(
                            position,
                            BSLS_COMPILERFEATURES_FORWARD(Args, arguments)...);
    }
#else
    template <class Args_01,
              class Args_02>
    iterator emplace(const_iterator                              position,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02)
        // Call the 'emplace' method of 'bsl::vector' passing the specified
        // 'position', 'arguments_01' and 'arguments_02' as parameters, and
        // return the result.
    {
        s_functionCallCounters[34]++;
        return d_vector.emplace(
                         position,
                         BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                         BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02));
    }
#endif

    iterator insert(const_iterator                      position,
                    const IncompleteTypeSupportChecker& value)
        // Call the 'insert' method of 'bsl::vector' passing the specified
        // 'position' and 'value' as parameters, and return the result.
    {
        s_functionCallCounters[35]++;
        return d_vector.insert(position, value);
    }

    iterator insert(const_iterator position, MovableRef value)
        // Call the 'insert' method of 'bsl::vector' passing the specified
        // 'position' and the specified movable 'value' as parameters, and
        // return the result.
    {
        s_functionCallCounters[36]++;
        IncompleteTypeSupportChecker& lvalue = value;
        return d_vector.insert(position, MoveUtil::move(lvalue));
    }

    iterator insert(const_iterator                      position,
                    size_type                           numElements,
                    const IncompleteTypeSupportChecker& value)
        // Call the 'insert' method of 'bsl::vector' passing the specified
        // 'position', 'numElements' and 'value' as parameters, and return the
        // result.
    {
        s_functionCallCounters[37]++;
        return d_vector.insert(position, numElements, value);
    }

    template <class INPUT_ITER>
    iterator insert(const_iterator position, INPUT_ITER first, INPUT_ITER last)
        // Call the 'insert' method of 'bsl::vector' passing the specified
        // 'position', 'first' and 'last' as parameters, and return the result.
    {
        s_functionCallCounters[38]++;
        return d_vector.insert(position, first, last);
    }

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    iterator insert(
                  const_iterator                                      position,
                  std::initializer_list<IncompleteTypeSupportChecker> values)
        // Call the 'insert' method of 'bsl::vector' passing the specified
        // 'position' and 'values' as parameters, and return the result.
    {
        s_functionCallCounters[39]++;
        return d_vector.insert(position, values);
    }
#endif

    iterator erase(const_iterator position)
        // Call the 'erase' method of 'bsl::vector' passing the specified
        // 'position' as a parameter, and return the result.
    {
        s_functionCallCounters[40]++;
        return d_vector.erase(position);
    }

    iterator erase(const_iterator first, const_iterator last)
        // Call the 'erase' method of 'bsl::vector' passing the specified
        // 'first' and 'last' as parameters, and return the result.
    {
        s_functionCallCounters[41]++;
        return d_vector.erase(first, last);
    }

    void swap(IncompleteTypeSupportChecker& other)
        // Call the 'swap' method of 'bsl::vector' passing the underlying
        // vector of the specified 'other' as a parameter.
    {
        s_functionCallCounters[42]++;
        d_vector.swap(other.d_vector);
    }

    void clear()
        // Call the 'clear' method of 'bsl::vector'.
    {
        s_functionCallCounters[43]++;
        d_vector.clear();
    }

    // ACCESSORS

                             // *** iterators ***

    const_iterator  begin() const
        // Return the result of calling the 'begin' accessor of 'bsl::vector'.
    {
        s_functionCallCounters[44]++;
        return d_vector.begin();
    }

    const_iterator cbegin() const
        // Return the result of calling the 'cbegin' accessor of 'bsl::vector'.
    {
        s_functionCallCounters[45]++;
        return d_vector.cbegin();
    }

    const_iterator end() const
        // Return the result of calling the 'end' accessor of 'bsl::vector'.
    {
        s_functionCallCounters[46]++;
        return d_vector.end();
    }

    const_iterator cend() const
        // Return the result of calling the 'cend' accessor of 'bsl::vector'.
    {
        s_functionCallCounters[47]++;
        return d_vector.cend();
    }

    const_reverse_iterator  rbegin() const
        // Return the result of calling the 'rbegin' accessor of 'bsl::vector'.
    {
        s_functionCallCounters[48]++;
        return d_vector.rbegin();
    }

    const_reverse_iterator crbegin() const
        // Return the result of calling the 'crbegin' accessor of
        // 'bsl::vector'.
    {
        s_functionCallCounters[49]++;
        return d_vector.crbegin();
    }

    const_reverse_iterator  rend() const
        // Return the result of calling the 'rend' accessor of 'bsl::vector'.
    {
        s_functionCallCounters[50]++;
        return d_vector.rend();
    }

    const_reverse_iterator crend() const
        // Return the result of calling the 'crend' accessor of 'bsl::vector'.
    {
        s_functionCallCounters[51]++;
        return d_vector.crend();
    }

                            // *** capacity ***

    size_type size() const
        // Return the result of calling the 'size' method of 'bsl::vector'.
    {
        s_functionCallCounters[52]++;
        return d_vector.size();
    }

    size_type capacity() const
        // Return the result of calling the 'capacity' method of 'bsl::vector'.
    {
        s_functionCallCounters[53]++;
        return d_vector.capacity();
    }

    bool empty() const
        // Return the result of calling the 'empty' method of 'bsl::vector'.
    {
        s_functionCallCounters[54]++;
        return d_vector.empty();
    }

    allocator_type get_allocator() const
        // Return the result of calling the 'get_allocator' method of
        // 'bsl::vector'.
    {
        s_functionCallCounters[55]++;
        return d_vector.get_allocator();
    }

    size_type max_size() const
        // Return the result of calling the 'max_size' method of 'bsl::vector'.
    {
        s_functionCallCounters[56]++;
        return d_vector.max_size();
    }

                          // *** element access ***

    const_reference operator[](size_type position) const
        // Call the constantsubscript operator of 'bsl::vector' passing the
        // specified 'position' as a parameter, and return the result.
    {
        s_functionCallCounters[57]++;
        return d_vector[position];
    }

    const_reference at(size_type position) const
        // Call the 'at' method of 'bsl::vector' passing the specified
        // 'position' as a parameter.
    {
        s_functionCallCounters[58]++;
        return d_vector.at(position);
    }

    const_reference front() const
        // Return the result of calling the 'front' accessor of 'bsl::vector'.
    {
        s_functionCallCounters[59]++;
        return d_vector.front();
    }

    const_reference back() const
        // Return the result of calling the 'back' accessor of 'bsl::vector'.
    {
        s_functionCallCounters[60]++;
        return d_vector.back();
    }

    const IncompleteTypeSupportChecker *data() const
        // Return the result of calling  the 'data' accessor of 'bsl::vector'.
    {
        s_functionCallCounters[61]++;
        return d_vector.data();
    }

    const VectorType& content() const;
        // Return a reference providing non-modifiable access to the underlying
        // vector.
};

// FREE OPERATORS
bool operator==(const IncompleteTypeSupportChecker& lhs,
                const IncompleteTypeSupportChecker& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'IncompleteTypeSupportChecker'
    // objects 'lhs' and 'rhs' have the same value if their underlying vectors
    // have the same value.

                  // ----------------------------------
                  // class IncompleteTypeSupportChecker
                  // ----------------------------------
// CLASS DATA
const int IncompleteTypeSupportChecker::s_numFunctions = 62;
int       IncompleteTypeSupportChecker::s_functionCallCounters[s_numFunctions]
                                                                         = { };

// CLASS METHODS
void IncompleteTypeSupportChecker::checkInvokedFunctions()
{
    for (std::size_t i = 0; i < s_numFunctions; ++i) {
        const size_t INDEX = i;
        ASSERTV(INDEX, 0 < s_functionCallCounters[INDEX]);
    }
}

void IncompleteTypeSupportChecker::increaseFunctionCallCounter(
                                                             std::size_t index)
{
    s_functionCallCounters[index]++;
}

void IncompleteTypeSupportChecker::resetFunctionCallCounters()
{
    for (std::size_t i = 0; i < s_numFunctions; ++i) {
        s_functionCallCounters[i] = 0;
    }
}

// ACCESSORS
const IncompleteTypeSupportChecker::VectorType&
IncompleteTypeSupportChecker::content() const
{
    return d_vector;
}

// FREE OPERATORS
bool operator==(const IncompleteTypeSupportChecker& lhs,
                const IncompleteTypeSupportChecker& rhs)
{
    return lhs.content() == rhs.content();
}

                             // ===============
                             // class UniqueInt
                             // ===============

class UniqueInt {
    // Unique int value set on construction.

    // CLASS DATA
    static int s_counter;

    // DATA
    int d_value;

  public:
    // CREATORS
    UniqueInt() : d_value(s_counter++)
        // Create a 'UniqueInt' object.
    {
    }

    // FREE OPERATORS
    friend bool operator==(const UniqueInt &v1, const UniqueInt &v2)
        // Equality comparison.
    {
        return v1.d_value == v2.d_value;
    }

#ifndef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON
    friend bool operator!=(const UniqueInt &v1, const UniqueInt &v2)
        // Inequality comparison.
    {
        return !(v1 == v2);
    }
#endif
};
int UniqueInt::s_counter = 0;

                     // TEST DRIVER PART 3 TINY HELPERS

template <int N>
int myFunc()
{
    return N;
}

template <char N>
char TestFunc()
{
    return N;
}

typedef char (*charFnPtr) ();

                           //  HYMAN'S TEST TYPES

                                // ========
                                // struct A
                                // ========

struct A     { int x; A() : x('a') { } };

                                // ========
                                // struct B
                                // ========

struct B : A { int y; B() : y('b') { } };

                          // ==================
                          // template struct HI
                          // ==================


template <class TYPE, size_t BITS>
struct HI
{
    typedef std::random_access_iterator_tag  iterator_category;
    typedef TYPE                             value_type;
    typedef ptrdiff_t                        difference_type;
    typedef TYPE                            *pointer;
    typedef TYPE&                            reference;

    static const size_t SIDE = size_t(1) << BITS;
    static const size_t SIZE = SIDE * SIDE;

    TYPE   *p;
    size_t  d;

    explicit HI(TYPE *p = 0, size_t d = SIZE) : p(p), d(d) { }
    HI(const HI& original) : p(original.p), d(original.d) { }

    size_t htoi() const
    {
        size_t x = 0, y = 0, t = d;
        for (size_t s = 1; s < SIDE; s *= 2) {
            size_t rx = 1 & (t / 2);
            size_t ry = 1 & (t ^ rx);
            if (ry == 0) {
                if (rx == 1) {
                    x = s - 1 - x;
                    y = s - 1 - y;
                }
                size_t z = x;
                x = y;
                y = z;
            }
            x += s * rx;
            y += s * ry;
            t /= 4;
        }
        return y * SIDE + x;
    }

    TYPE &operator*()  const { return p[htoi()];  }
    TYPE *operator->() const { return p + htoi(); }

    HI& operator++() { ++d; return *this; }
    HI& operator--() { --d; return *this; }

    HI  operator++(int) { HI t(p, d); ++d; return t; }
    HI  operator--(int) { HI t(p, d); --d; return t; }

    HI& operator+=(ptrdiff_t rhs) { d += rhs; return *this; }
    HI& operator-=(ptrdiff_t rhs) { d -= rhs; return *this; }

    HI  operator+ (ptrdiff_t rhs) const { return HI(p, d + rhs); }
    HI  operator- (ptrdiff_t rhs) const { return HI(p, d - rhs); }

    ptrdiff_t operator-(const HI& rhs) const { return d - rhs.d; }

    TYPE &operator[](ptrdiff_t rhs) const { return *(*this + rhs); }

    operator TYPE*()   const { return p + htoi(); }
        // Conversion operator to confuse badly written traits code.
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON

template <class TYPE, size_t BITS>
inline
auto operator<=>(const HI<TYPE, BITS>& lhs, const HI<TYPE, BITS>& rhs)
{
    auto result = lhs.p <=> rhs.p;
    return result == 0 ? lhs.d <=> rhs.d : result;
}

template <class TYPE, size_t BITS>
inline
bool operator==(const HI<TYPE, BITS>& lhs, const HI<TYPE, BITS>& rhs)
{
    return lhs <=> rhs == 0;
}

#else

template <class TYPE, size_t BITS>
inline
bool operator< (const HI<TYPE, BITS>& lhs, const HI<TYPE, BITS>& rhs)
{
    return (lhs.p < rhs.p) || (lhs.p == rhs.p && lhs.d < rhs.d);
}

template <class TYPE, size_t BITS>
inline
bool operator>=(const HI<TYPE, BITS>& lhs, const HI<TYPE, BITS>& rhs)
{
    return !(lhs <  rhs);
}

template <class TYPE, size_t BITS>
inline
bool operator> (const HI<TYPE, BITS>& lhs, const HI<TYPE, BITS>& rhs)
{
    return !(lhs <= rhs);
}

template <class TYPE, size_t BITS>
inline
bool operator<=(const HI<TYPE, BITS>& lhs, const HI<TYPE, BITS>& rhs)
{
    return !(lhs >  rhs);
}

template <class TYPE, size_t BITS>
inline
bool operator==(const HI<TYPE, BITS>& lhs, const HI<TYPE, BITS>& rhs)
{
    return !(lhs < rhs) && !(rhs < lhs);
}

template <class TYPE, size_t BITS>
inline
bool operator!=(const HI<TYPE, BITS>& lhs, const HI<TYPE, BITS>& rhs)
{
    return !(lhs == rhs);
}

#endif

                    // TEST DRIVER PART 3 TRAITS HELPERS

template <class TYPE>
struct IsDefaultConstructible : bsl::true_type {};

template <>
struct IsDefaultConstructible<NotAssignable> : bsl::false_type {};

template <>
struct IsDefaultConstructible<BitwiseNotAssignable> : bsl::false_type {};

template <>
struct IsDefaultConstructible<bsltf::NonDefaultConstructibleTestType>
    : bsl::false_type {
};

//=============================================================================
//                            TEST DRIVER TEMPLATE
//=============================================================================

template <class TYPE, class ALLOC = bsl::allocator<TYPE> >
struct TestDriver3 : TestSupport<TYPE, ALLOC> {

                     // NAMES FROM DEPENDENT BASE

    BSLSTL_VECTOR_0T_PULL_TESTSUPPORT_NAMES;
    // Unfortunately the names have to be made available "by hand" due to two
    // phase name lookup not reaching into dependent bases.

    // CONSTANTS
    static
    const bool s_typeIsMoveEnabled =
            bsl::is_same<TYPE, bsltf::MovableTestType>::value ||
            bsl::is_same<TYPE, bsltf::MovableAllocTestType>::value ||
            bsl::is_same<TYPE, bsltf::MoveOnlyAllocTestType>::value ||
            bsl::is_same<TYPE, bsltf::WellBehavedMoveOnlyAllocTestType>::value;

                               // TEST CASES
    // CLASS METHODS

    static void testCase40();
        // Test free functions 'erase' and 'erase_if'

    static void testCase38();
        // Test ability to move movable types when growing vector.

    static void testCase37();
        // Test member pointer compilation.

    static void testCase35();
        // Test 'noexcept' specifications

    static void testCase34();
        // Test hashAppend.

    static void testCase33();
        // Test Hyman's test case 2.

    static void testCase32();
        // Test Hyman's test case 1.

    static void testCase31();
        // Test vector of function pointers.

    static void testCase30();
        // Test bugfix of range insertion of function pointers.

    static void testCase29();
        // Test functions that take an initializer list.

    template <int N_ARGS,
              int N01,
              int N02,
              int N03,
              int N04,
              int N05,
              int N06,
              int N07,
              int N08,
              int N09,
              int N10>
    static void testCase28a_RunTest(Obj *target, const_iterator position);
        // Call 'emplace' on the specified 'target' container at the specified
        // 'position'.  Forward (template parameter) 'N_ARGS' arguments to the
        // 'emplace' method and ensure 1) that values are properly passed to
        // the constructor of 'value_type', 2) that the allocator is correctly
        // configured for each argument in the newly inserted element in
        // 'target', and 3) that the arguments are forwarded using copy or move
        // semantics based on integer template parameters 'N01' ... 'N10'.

    static void testCase28a();
        // Test forwarding of arguments in 'emplace' method.

    static void testCase28();
        // Test 'emplace' other than forwarding of arguments (see '28a').

    template <int N_ARGS,
              int N01,
              int N02,
              int N03,
              int N04,
              int N05,
              int N06,
              int N07,
              int N08,
              int N09,
              int N10>
    static void testCase26a_RunTest(Obj *target);
        // Call 'emplace_back' on the specified 'target' container.  Forward
        // (template parameter) 'N_ARGS' arguments to the 'emplace' method and
        // ensure 1) that values are properly passed to the constructor of
        // 'value_type', 2) that the allocator is correctly configured for each
        // argument in the newly inserted element in 'target', and 3) that the
        // arguments are forwarded using copy or move semantics based on
        // integer template parameters 'N01' ... 'N10'.

    static void testCase26();
        // Test 'emplace_back' other than forwarding of arguments (see '27a').

    static void testCase26a();
        // Test forwarding of arguments in 'emplace_back' method.

    static void testCase26_EmplaceDefault(Obj*, bsl::false_type);
    static void testCase26_EmplaceDefault(Obj* objPtr, bsl::true_type);
        // Test that 'emplace_back()' appends a single value-initialized
        // element to the specified 'objPtr' vector.  The bool-constant type
        // indicated whether 'TYPE' is default constructible, and so whether
        // the test would compile.  In the 'false_type' case, the function
        // does nothing.  Both functions guarantee to leave the passed vector
        // in its original state, so the caller can verify that it is is
        // unchanged.

    static void testCase27();
        // Test 'insert' method that takes a movable ref.

    template <class CONTAINER>
    static void testCase27Range(const CONTAINER&);
        // Test 'insert' method that takes move-only and other awkward types.

    static void testCase25();
        // Test 'push_back' method that takes a movable ref.

    template <bool PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT_FLAG,
              bool OTHER_FLAGS>
    static void testCase24_propagate_on_container_move_assignment_dispatch();
        // Test 'propagate_on_container_move_assignment'.

    static void testCase24_propagate_on_container_move_assignment();
        // Test 'propagate_on_container_move_assignment'.

    static void testCase24_move_assignment_noexcept();
        // Test noexcept specification of the move assignment operator.

    static void testCase24_dispatch();
        // Test move assignment operator.
};

                  // ==================================
                  // template class StdBslmaTestDriver3
                  // ==================================

template <class TYPE>
class StdBslmaTestDriver3 : public StdBslmaTestDriverHelper<TestDriver3, TYPE>
{
};

                    // ==============================
                    // template class MetaTestDriver3
                    // ==============================

template <class TYPE>
struct MetaTestDriver3 {
    // This 'struct' is to be call by the 'RUN_EACH_TYPE' macro, and the
    // functions within it dispatch to functions in 'TestDriver3' instantiated
    // with different types of allocator.

    static void testCase24();
        // Test move-assign.
};

                                 // ----------
                                 // TEST CASES
                                 // ----------

template <class TYPE, class ALLOC>
void TestDriver3<TYPE, ALLOC>::testCase40()
    // test 'bsl::erase' and 'bsl::erase_if' with 'bsl::vector'.
{
    static const struct {
        int         d_line;       // source line number
        const char *d_initial_p;  // initial values
        char        d_element;    // value to remove
        const char *d_results_p;  // expected result value
    } DATA[] = {
        //line  initial              element  results
        //----  -------------------  -------  -------------------
        { L_,   "",                  'A',     ""                  },
        { L_,   "A",                 'A',     ""                  },
        { L_,   "A",                 'B',     "A"                 },
        { L_,   "B",                 'A',     "B"                 },
        { L_,   "AB",                'A',     "B"                 },
        { L_,   "BA",                'A',     "B"                 },
        { L_,   "BC",                'D',     "BC"                },
        { L_,   "ABC",               'C',     "AB"                },
        { L_,   "CBADEABCDAB",       'B',     "CADEACDA"          },
        { L_,   "CBADEABCDABCDEA",   'E',     "CBADABCDABCDA"     },
        { L_,   "ZZZZZZZZZZZZZZZZ",  'Z',     ""                  }
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    for (size_t i = 0; i < NUM_DATA; ++i)
    {
        int         LINE = DATA[i].d_line;
        const char *initial = DATA[i].d_initial_p;
        size_t      initialLen = strlen(initial);
        const char *results = DATA[i].d_results_p;
        size_t      resultsLen = strlen(results);

        Obj    v1(initial, initial + initialLen);
        Obj    v2(initial, initial + initialLen);
        Obj    vres(results, results + resultsLen);
        size_t ret1 = bsl::erase   (v1, DATA[i].d_element);
        size_t ret2 = bsl::erase_if(v2, EqPred<TYPE>(DATA[i].d_element));

        // Are the modified containers correct?
        ASSERTV(LINE, v1 == vres);
        ASSERTV(LINE, v2 == vres);

        // Are the return values correct?
        ASSERTV(LINE, ret1 == initialLen - resultsLen);
        ASSERTV(LINE, ret2 == initialLen - resultsLen);
    }
}


template <class TYPE, class ALLOC>
void TestDriver3<TYPE, ALLOC>::testCase38()
{
    // ------------------------------------------------------------------------
    // TEST ABILITY TO MOVE MOVABLE TYPES WHEN GROWING VECTOR.
    //
    // Concerns:
    //: 1 Growing a 'vector' moved 'value_type's that are (nothrow) movable
    //:   instead of copying them.
    //
    // Plan:
    //: 1 Fill a vector (using moving 'push_back') with unique movable values
    //:   then verify that all elements are results of moves and not copies, as
    //:   well as hold the expected values.
    //
    // Testing:
    //   CONCERN: Movable types are moved when growing a vector
    // ------------------------------------------------------------------------

    enum { k_NUM_ELEMENTS = 513 };

    const char *typeName = bsls::NameOf<ValueType>().name();
    if (verbose) P(typeName);

    bslma::TestAllocator ta(veryVeryVeryVerbose);

    Obj mX(&ta);    const Obj& X = mX;

    for (int ii = 0; ii < k_NUM_ELEMENTS; ++ii) {
        bsls::ObjectBuffer<ValueType>  buffer;
        ValueType                     *valptr = buffer.address();
        TstFacility::emplace(valptr, ii / 8, &ta);

        mX.push_back(bslmf::MovableRefUtil::move(*valptr));

        valptr->~ValueType();
    }

    for (int ii = k_NUM_ELEMENTS; 0 < ii--; ) {
        ASSERTV(ii / 8, X[ii].data(), ii / 8 == X[ii].data());
        ASSERTV(typeName, ii, X[ii].movedInto(),
                                      X[ii].movedInto() == MoveState::e_MOVED);
    }
}

template <class TYPE, class ALLOC>
void TestDriver3<TYPE, ALLOC>::testCase37()
{
    // ------------------------------------------------------------------------
    // MEMBER POINTER COMPILATION
    //
    // Concerns:
    //: 1 The specialization of vector or pointer formerly used 'using' to
    //:   make a few privately inherited methods public.  This causes
    //:   compilation errors when attempting to use pointers to these methods.
    //
    // Plan:
    //: 1 Now that they've been rewritten, check that calls through member
    //:   pointers compile.  No runtime test is needed.
    //
    // Testing:
    //   CONCERN: Access through member pointers compiles
    // ------------------------------------------------------------------------

    if (verbose) {
        T_ P(bsls::NameOf<Obj>())
        T_ P(bsls::NameOf<TYPE>())
        T_ P(bsls::NameOf<ALLOC>())
    }

    (void)sizeof((Obj().*&Obj::reserve)(0),      0);
    (void)sizeof((Obj().*&Obj::shrink_to_fit)(), 0);
    (void)sizeof((Obj().*&Obj::pop_back)(),      0);
    (void)sizeof((Obj().*&Obj::clear)(),         0);
    (void)sizeof((Obj().*&Obj::max_size)(),      0);
    (void)sizeof((Obj().*&Obj::size)(),          0);
    (void)sizeof((Obj().*&Obj::capacity)(),      0);
    (void)sizeof((Obj().*&Obj::empty)(),         0);
}

template <class TYPE, class ALLOC>
void TestDriver3<TYPE, ALLOC>::testCase35()
{
    // ------------------------------------------------------------------------
    // 'noexcept' SPECIFICATION
    //
    // Concerns:
    //: 1 The 'noexcept' specification has been applied to all class interfaces
    //:   required by the standard.
    //
    // Plan:
    //: 1 Apply the unary 'noexcept' operator to expressions that mimic those
    //:   appearing in the standard and confirm that calculated boolean value
    //:   matches the expected value.
    //:
    //: 2 Since the 'noexcept' specification does not vary with the 'TYPE'
    //:   of the container, we need test for just one general type and any
    //:   'TYPE' specializations.
    //
    // Testing:
    //   CONCERN: Methods qualified 'noexcept' in standard are so implemented.
    // ------------------------------------------------------------------------

    if (verbose) {
        T_ P(bsls::NameOf<Obj>())
        T_ P(bsls::NameOf<TYPE>())
        T_ P(bsls::NameOf<ALLOC>())
    }

    // N4594: 23.3.11 Class template vector

    // page 853
    //..
    //  // 23.3.11.2, construct/copy/destroy:
    //  vector() noexcept(noexcept(Allocator())) : vector(Allocator()) { }
    //  explicit vector(const Allocator&) noexcept;
    //..

    {
        // not implemented
        // ASSERT(false
        //     == BSLS_KEYWORD_NOEXCEPT_OPERATOR(Obj()));

        ALLOC a;
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(Obj(a)));
    }

    // page 854
    //..
    //  vector(vector&&) noexcept;
    //  vector& operator=(vector&& x) noexcept(
    //         allocator_traits<Allocator>::
    //                    propagate_on_container_move_assignment::value ||
    //         allocator_traits<Allocator>::is_always_equal::value);
    //
    //  allocator_type get_allocator() const noexcept;
    //..

    {
        Obj mX;
        Obj mY;

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(
                                        Obj(bslmf::MovableRefUtil::move(mY))));

        ASSERT(false
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(
                                        mX = bslmf::MovableRefUtil::move(mY)));

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(mX.get_allocator()));
    }

    // page 854
    //..
    //  // iterators:
    //  iterator begin() noexcept;
    //  const_iterator begin() const noexcept;
    //  iterator end() noexcept;
    //  const_iterator end() const noexcept;
    //  reverse_iterator rbegin() noexcept;
    //  const_reverse_iterator rbegin() const noexcept;
    //  reverse_iterator rend() noexcept;
    //  const_reverse_iterator rend() const noexcept;
    //  const_iterator cbegin() const noexcept;
    //  const_iterator cend() const noexcept;
    //  const_reverse_iterator crbegin() const noexcept;
    //  const_reverse_iterator crend() const noexcept;
    //..

    {
        Obj mX; const Obj& X = mX;    (void) X;

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(mX.begin()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR( X.begin()));

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(mX.end()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR( X.end()));

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(mX.rbegin()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR( X.rbegin()));

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(mX.rend()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR( X.rend()));

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR( X.cbegin()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR( X.cend()));

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR( X.crbegin()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR( X.crend()));
    }

    // page 854
    //..
    //  // 23.3.11.3, capacity:
    //  bool empty() const noexcept;
    //  size_type size() const noexcept;
    //  size_type max_size() const noexcept;
    //  size_type capacity() const noexcept;
    //..
    {
        Obj mX; const Obj& X = mX;    (void) X;

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR( X.empty()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR( X.size()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR( X.max_size()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR( X.capacity()));
    }

    // page 854-855
    //..
    //  // 23.3.11.4, data access
    //  T* data() noexcept;
    //  const T* data() const noexcept;
    //..

    {
        Obj mX; const Obj& X = mX;    (void) X;

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(mX.data()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR( X.data()));
    }

    // page 855
    //..
    //  // 23.3.11.5, modifiers:
    //  void swap(vector&) noexcept(
    //       allocator_traits<Allocator>::propagate_on_container_swap::value ||
    //       allocator_traits<Allocator>::is_always_equal::value);
    //  void clear() noexcept;
    //..

    {
        Obj mX;
        Obj mY;

        ASSERT(false
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(mX.swap(mY)));

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(mX.clear()));
     }

    // page 855
    //..
    //  // 23.3.11.6, specialized algorithms:
    //  template <class T, class Allocator>
    //  void swap(vector<T, Allocator>& x, vector<T, Allocator>& y)
    //                                           noexcept(noexcept(x.swap(y)));
    //..

    {
        Obj mX;
        Obj mY;

        ASSERT(false == BSLS_KEYWORD_NOEXCEPT_OPERATOR(swap(mX, mY)));
     }
}

template <class TYPE, class ALLOC>
void TestDriver3<TYPE, ALLOC>::testCase34()
{
    // ------------------------------------------------------------------------
    // TESTING 'hashAppend'
    //
    // Concerns:
    //   1) Objects constructed with the same values hash as equal.
    //   2) Objects constructed such that they have same (logical) value but
    //      different internal representation (due to the lack or presence
    //      of an allocator, and/or different capacities) always hash as equal.
    //   3) Unequal objects hash as unequal (not required, but we can hope).
    //
    // Plan:
    //   For concerns 1 and 3, Specify a set A of unique allocators including
    //   no allocator.  Specify a set S of unique object values having various
    //   minor or subtle differences, ordered by non-decreasing length.
    //   Verify the correctness of hash values matching using all elements (u,
    //   ua, v, va) of the cross product S X A X S X A.
    //
    //   For concern 2 create two objects using all elements in S one at a
    //   time.  For the second object change its internal representation by
    //   extending it by different amounts in the set E, followed by erasing
    //   its contents using 'clear'.  Then recreate the original value and
    //   verify that the second object still hashes equal to the first.
    //
    // Testing:
    //   void hashAppend(HASHALG& hashAlg, const vector<T,A>&);
    // ------------------------------------------------------------------------

    if (verbose) printf("\nTesting '%s'.\n", NameOf<TYPE>().name());

    typedef ::BloombergLP::bslh::Hash<> Hasher;
    typedef Hasher::result_type         HashType;
    Hasher                              hasher;

    bslma::TestAllocator testAllocator1(veryVeryVeryVerbose);
    bslma::TestAllocator testAllocator2(veryVeryVeryVerbose);

    bslma::Allocator *ALLOCATOR[] = {
        &testAllocator1,
        &testAllocator2
    };

    enum { NUM_ALLOCATOR = sizeof ALLOCATOR / sizeof *ALLOCATOR };

    const TestValues VALUES;
    const int        NUM_VALUES = 5;         // TBD: fix this

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
        "AAAAAAAAAAA",      "AAAABAAAAAA",      "AAAAABAAAAA",
        "AAAAAAAAAAAA",     "AAAABAAAAAAA",     "AAAAABAAAAAA",
        "AAAAAAAAAAAAA",    "AAAABAAAAAAAA",    "AAAAABAAAAAAA",
        "AAAAAAAAAAAAAA",   "AAAABAAAAAAAAA",   "AAAAABAAAAAAAA",
        "AAAAAAAAAAAAAAA",  "AAAABAAAAAAAAAA",  "AAAAABAAAAAAAAA",
    };
    enum { NUM_SPECS = sizeof SPECS / sizeof *SPECS };

    if (verbose)
        printf("\tCompare hash values of each pair without perturbation.\n");
    {
        int oldLen = -1;

        // Create first object
        for (int si = 0; si < NUM_SPECS; ++si) {
            for (int ai = 0; ai < NUM_ALLOCATOR; ++ai) {

                const char *const U_SPEC = SPECS[si];
                const int         LENGTH = static_cast<int>(strlen(U_SPEC));

                Obj mU(ALLOCATOR[ai]); const Obj& U = gg(&mU, U_SPEC);
                // same lengths
                ASSERTV(si, ai, LENGTH == static_cast<int>(U.size()));

                if (LENGTH != oldLen) {
                    if (verbose)
                        printf( "\tUsing lhs objects of length %d.\n",
                                                                  LENGTH);
                    ASSERTV(U_SPEC, oldLen <= LENGTH);//non-decreasing
                    oldLen = LENGTH;
                }

                if (veryVerbose) { T_; T_;
                    P_(si); P_(U_SPEC); P(U); }

                // Create second object
                for (int sj = 0; sj < NUM_SPECS; ++sj) {
                    for (int aj = 0; aj < NUM_ALLOCATOR; ++aj) {

                        const char *const V_SPEC = SPECS[sj];
                        Obj               mV(ALLOCATOR[aj]);
                        const Obj&        V = gg(&mV, V_SPEC);

                        if (veryVerbose) {
                            T_; T_; P_(sj); P_(V_SPEC); P(V);
                        }

                        HashType hU = hasher(U);
                        HashType hV = hasher(V);
                        ASSERTV(si, sj, (si == sj) == (hU == hV));
                    }
                }
            }
        }
    }

    if (verbose)
        printf("\tCompare hash values of each pair after perturbing.\n");
    {
        static const std::size_t EXTEND[] = {
            0, 1, 2, 3, 4, 5, 7, 8, 9, 15
        };

        enum { NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND };

        int oldLen = -1;

        // Create first object
        for (int si = 0; si < NUM_SPECS; ++si) {
            for (int ai = 0; ai < NUM_ALLOCATOR; ++ai) {

                const char *const U_SPEC = SPECS[si];
                const int         LENGTH = static_cast<int>(strlen(U_SPEC));

                Obj mU(ALLOCATOR[ai]); const Obj& U = gg(&mU, U_SPEC);

                // same lengths
                ASSERTV(si, LENGTH == static_cast<int>(U.size()));

                if (LENGTH != oldLen) {
                    if (verbose)
                        printf( "\tUsing lhs objects of length %d.\n",
                                                                  LENGTH);
                    ASSERTV(U_SPEC, oldLen <= LENGTH);
                    oldLen = LENGTH;
                }

                if (veryVerbose) { P_(si); P_(U_SPEC); P(U); }

                // Create second object
                for (int sj = 0; sj < NUM_SPECS; ++sj) {
                    for (int aj = 0; aj < NUM_ALLOCATOR; ++aj) {
                        //Perform perturbation
                        for (int e = 0; e < NUM_EXTEND; ++e) {

                            const char *const V_SPEC = SPECS[sj];
                            Obj               mV(ALLOCATOR[aj]);
                            const Obj&        V = gg(&mV, V_SPEC);

                            stretchRemoveAll(&mV,
                                             EXTEND[e],
                                             TstFacility::getIdentifier(
                                                      VALUES[e % NUM_VALUES]));
                            gg(&mV, V_SPEC);

                            if (veryVerbose) {
                                T_; T_; P_(sj); P_(V_SPEC); P(V);
                            }

                            HashType hU = hasher(U);
                            HashType hV = hasher(V);
                            ASSERTV(si, sj, (si == sj) == (hU == hV));
                        }
                    }
                }
            }
        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver3<TYPE, ALLOC>::testCase33()
{
    // ------------------------------------------------------------------------
    // TESTING HYMAN'S TEST CASE 2
    //
    // Concerns
    //: 1 Can construct a vector from an iterator range where the iterator
    //:    type has an unfortunate implicit conversion to 'ELEMENT_TYPE *'.
    //: 2: Can insert into a vector from an iterator range where the
    //:    iterator type has an unfortunate implicit conversion to
    //:    'ELEMENT_TYPE *'.
    //
    // Plan:
    //
    // Testing:
    //   CONCERN: Range ops work correctly for types convertible to 'iterator'
    // ------------------------------------------------------------------------
    int d[4][4] = {
        {  0,  1,  2,  3 },
        {  4,  5,  6,  7 },
        {  8,  9, 10, 11 },
        { 12, 13, 14, 15 }
    };

    const HI<int, 2> b(&d[0][0], 0);
    const HI<int, 2> e(&d[0][0]);
    {
        bsl::vector<int> bh(b, e);

        HI<int, 2> iter = b;
        for (size_t i = 0; i < bh.size(); ++i, ++iter) {
            if (veryVerbose) printf(ZU " %d %d\n", i, bh[i], *iter);
            ASSERTV(i, bh[i] == *iter);
        }

        bh.assign(b, e);
        for (size_t i = 0; i < bh.size(); ++i, ++iter) {
            if (veryVerbose) printf(ZU " %d %d\n", i, bh[i], *iter);
            ASSERTV(i, bh[i] == *iter);
        }
    }

    {
        bsl::vector<int> bh;
        bh.insert(bh.begin(), b, e);
        HI<int, 2> iter = b;
        for (size_t i = 0; i < bh.size(); ++i, ++iter) {
            if (veryVerbose) printf(ZU " %d %d\n", i, bh[i], *iter);
            ASSERTV(i, bh[i] == *iter);
        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver3<TYPE, ALLOC>::testCase32()
{
    // ------------------------------------------------------------------------
    // TESTING HYMAN'S TEST CASE 1
    //
    // Concerns
    // 1: A range of derived objects is correctly sliced when inserted into
    //    a vector of base objects.
    //
    // Plan:
    //
    // Testing:
    //   CONCERN: Range operations slice from ranges of derived types
    // ------------------------------------------------------------------------

    {
        bsl::vector<B> bB(10);
        bsl::vector<A> bA(bB.begin(), bB.end());
        for (unsigned i = 0; i < bA.size(); ++i) {
            ASSERTV(i, bA[i].x, bA[i].x == 'a');
        }

        bA.assign(bB.begin(), bB.end());
        for (unsigned i = 0; i < bA.size(); ++i) {
            ASSERTV(i, bA[i].x, bA[i].x == 'a');
        }
    }

    {
        bsl::vector<B> bB(10);
        bsl::vector<A> bA;
        bA.insert(bA.begin(), bB.begin(), bB.end());
        for (unsigned i = 0; i < bA.size(); ++i) {
            ASSERTV(i, bA[i].x, bA[i].x == 'a');
        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver3<TYPE, ALLOC>::testCase31()
{
    // ------------------------------------------------------------------------
    // TESTING VECTORS OF FUNCTION POINTERS
    //   In DRQS 34693876, it was observed that function pointers cannot be
    //   cast into 'void *' pointers.  A 'reinterpret_cast' is required in this
    //   case. This is handled in 'bslalg_arrayprimitives'.
    //
    // Diagnosis:
    //   Vector is specialized for pointer types, and the specialization
    //   assumes that any pointer type can be cast or copy constructed into a
    //   'void *', but for function pointers on g++, this is not the case.  Had
    //   to fix 'bslalg_arrayprimitives' to deal with this, this test verifies
    //   that the fix worked.  DRQS 34693876.
    //
    // Concerns:
    //: 1 A vector of function pointers can be constructed from a sequence of
    //:   function pointers described by iterators that may be pointers or
    //:   simpler input iterators.
    //: 2 A vector of function pointers can insert a sequence of function
    //:   pointers described by iterators that may be pointers or simpler
    //:   input iterators.
    //
    // Testing:
    //   DRQS 34693876
    // ------------------------------------------------------------------------

    const char VA = 'A';
    const char VB = 'B';
    const char VC = 'C';
    const char VD = 'D';

    const charFnPtr VALUES[] = {
        TestFunc<VA>,
        TestFunc<VB>,
        TestFunc<VC>,
        TestFunc<VD>
    };
    enum { NUM_VALUES = sizeof VALUES / sizeof VALUES[0] };

    typedef FixedArray<charFnPtr>::iterator ArrayIterator;

    FixedArray<charFnPtr> l;
    l.push_back(TestFunc<VA>);
    l.push_back(TestFunc<VB>);
    l.push_back(TestFunc<VC>);
    l.push_back(TestFunc<VD>);

    {
        // construct vector from a pair of iterators that are not pointers
        vector<charFnPtr> w(l.begin(), l.end());
        ASSERT(4 == w.size());

        // Check the elements of w.
        vector<charFnPtr>::iterator wit = w.begin();

        for (ArrayIterator it = l.begin(); it != l.end(); ++it) {
            ASSERT(wit != w.end());
            ASSERT(*it == *wit);
            ++wit;
        }
        ASSERT(wit == w.end());

        // insert a range from a pair of pointers indicating an array
        w.insert(w.end(), &VALUES[0], &VALUES[0] + NUM_VALUES);

        // Check the elements of w.
        wit = w.begin();

        for (ArrayIterator it = l.begin(); it != l.end(); ++it) {
            ASSERT(wit != w.end());
            ASSERT(*it == *wit);
            ++wit;
        }

        for (int i = 0; i != NUM_VALUES; ++i) {
            ASSERT(wit != w.end());
            ASSERT(VALUES[i] == *wit);
            ++wit;
        }
        ASSERT(wit == w.end());
    }

    {
        // construct vector from a pair of pointers indicating an array
        vector<charFnPtr> w(&VALUES[0], &VALUES[0] + NUM_VALUES);
        ASSERT(4 == w.size());

        // Check the elements of w.
        vector<charFnPtr>::iterator wit = w.begin();

        for (ArrayIterator it = l.begin(); it != l.end(); ++it) {
            ASSERT(wit != w.end());
            ASSERT(*it == *wit);
            ++wit;
        }
        ASSERT(wit == w.end());

        // insert a range with iterators that are not pointers
        w.insert(w.end(), l.begin(), l.end());

        // Check the elements of w.
        wit = w.begin();

        for (int i = 0; i != NUM_VALUES; ++i) {
            ASSERT(wit != w.end());
            ASSERT(VALUES[i] == *wit);
            ++wit;
        }

        for (ArrayIterator it = l.begin(); it != l.end(); ++it) {
            ASSERT(wit != w.end());
            ASSERT(*it == *wit);
            ++wit;
        }
        ASSERT(wit == w.end());
    }
}

template <class TYPE, class ALLOC>
void TestDriver3<TYPE, ALLOC>::testCase30()
{
    // ------------------------------------------------------------------------
    // TESTING FUNCTION POINTER RANGE-INSERT BUGFIX
    //   In DRQS 31711031, it was observed that a range insert constructor from
    //   an array of function pointers broke 'g++'.  Reproduce the bug.
    //
    // Diagnosis:
    //   Vector is specialized for pointer types, and the specialization
    //   assumes that any pointer type can be cast or copy constructed into a
    //   'void *', but for function pointers on g++, this is not the case.  Had
    //   to fix 'bslalg_arrayprimitives' to deal with this, this test verifies
    //   that the fix worked.  DRQS 31711031.
    //
    // Concerns:
    //: 1 A vector of function pointers can insert a sequence of function
    //:   pointers described by iterators that may be pointers or simpler
    //:   input iterators.
    //
    // Testing:
    //   DRQS 31711031
    // ------------------------------------------------------------------------

    typedef int (*FuncPtr)();
    static FuncPtr funcPtrs[] = { &myFunc<0>, &myFunc<1>, &myFunc<2>,
                                  &myFunc<3>, &myFunc<4>, &myFunc<5>,
                                  &myFunc<6>, &myFunc<7>, &myFunc<8>,
                                  &myFunc<9> };
    enum { ARRAY_SIZE = sizeof(funcPtrs) /  sizeof(*funcPtrs) };

    vector<FuncPtr> v(funcPtrs + 0, funcPtrs + ARRAY_SIZE);
    for (int i = 0; i < 10; ++i) {
        ASSERTV(i, (*v[i])(), i == (*v[i])());
    }

    v.clear();

    const FuncPtr * const cFuncPtrs = funcPtrs;

    v.insert(v.begin(), cFuncPtrs, cFuncPtrs + 10);
    for (int i = 0; i < 10; ++i) {
        ASSERTV(i, (*v[i])(), i == (*v[i])());
    }

    v.clear();

    v.insert(v.begin(),     funcPtrs + 5, funcPtrs + 10);
    v.insert(v.begin(),     funcPtrs + 0, funcPtrs + 2 );
    v.insert(v.begin() + 2, funcPtrs + 2, funcPtrs + 5 );
    for (int i = 0; i < 10; ++i) {
        ASSERTV(i, (*v[i])(), i == (*v[i])());
    }

    const vector<FuncPtr>& cv = v;
    vector<FuncPtr>        w(cv);
    for (int i = 0; i < 10; ++i) {
        ASSERTV(i, (w[i])(), i == (*w[i])());
    }
    w.insert(w.begin() + 5, cv.begin(), cv.begin() + 10);
    for (int i = 0; i < 20; ++i) {
        const int match = i - (i < 5 ? 0 : i < 15 ? 5 : 10);
        ASSERTV(i, (w[i])(), match == (*w[i])());
    }
}

template <class TYPE, class ALLOC>
void TestDriver3<TYPE, ALLOC>::testCase29()
{
    // ------------------------------------------------------------------------
    // TESTING FUNCTIONS TAKING INITIALIZER LISTS
    //
    // Concerns:
    //:  The functions that take an initializer lists (constructor, assignment
    //:  operator, 'assign', and 'insert') simply forward to another already
    //:  tested function.  We are interested here only in ensuring that the
    //:  forwarding is working -- not retesting already functionality.
    //
    // Plan:
    //:
    //:
    // Testing:
    //   vector(initializer_list<T>, const A& allocator);
    //   void assign(initializer_list<T>);
    //   vector& operator=(initializer_list<T>);
    //   iterator insert(const_iterator pos, initializer_list<T>);
    // ------------------------------------------------------------------------
#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    const TestValues V;

    if (verbose) printf("\nTesting '%s'.\n", NameOf<TYPE>().name());

    if (verbose)
        printf("\tTesting constructor with initializer lists.\n");

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    {
        const struct {
            int                          d_line;    // source line number
            std::initializer_list<TYPE>  d_list;    // source list
            const char                  *d_result;  // expected result
        } DATA[] = {
            //line  list                   result
            //----  --------------------   ------
            { L_,   {                  },  ""     },
            { L_,   { V[0]             },  "A"    },
            { L_,   { V[0], V[0]       },  "AA"   },
            { L_,   { V[1], V[0]       },  "BA"   },
            { L_,   { V[0], V[1], V[2] },  "ABC"  },
            { L_,   { V[0], V[1], V[0] },  "ABA"  },
        };

        enum { NUM_SPECS = sizeof DATA / sizeof *DATA };

        bslma::TestAllocator        oa("object", veryVeryVeryVerbose);
        ALLOC                       xoa(&oa);
        bslma::TestAllocatorMonitor dam(&da);
        for (int ti = 0; ti < NUM_SPECS; ++ti) {
            ASSERT(0 == oa.numBytesInUse());

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            ALLOC                xscratch(&scratch);
            Obj                  mY(xscratch);
            const Obj&           Y = gg(&mY, DATA[ti].d_result);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                Obj mX(DATA[ti].d_list, xoa); const Obj& X = mX;

                ASSERTV(Y, X, Y == X);

                ASSERT(xoa == X.get_allocator());

            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERT(xscratch == Y.get_allocator());
        }
        ASSERT(dam.isTotalSame());
    }

    {
        const struct {
            int                          d_line;    // source line number
            std::initializer_list<TYPE>  d_list;    // source list
            const char                  *d_result;  // expected result
        } DATA[] = {
            //line  list                   result
            //----  --------------------   ------
            { L_,   {                  },  ""     },
            { L_,   { V[0]             },  "A"    },
            { L_,   { V[0], V[0]       },  "AA"   },
            { L_,   { V[1], V[0]       },  "BA"   },
            { L_,   { V[0], V[1], V[2] },  "ABC"  },
            { L_,   { V[0], V[1], V[0] },  "ABA"  },
        };

        enum { NUM_SPECS = sizeof DATA / sizeof *DATA };

        bslma::TestAllocatorMonitor dam(&da);
        for (int ti = 0; ti < NUM_SPECS; ++ti) {

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            ALLOC                xscratch(&scratch);
            Obj                  mY(xscratch);
            const Obj&           Y = gg(&mY, DATA[ti].d_result);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(da) {
                if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                Obj        mX(DATA[ti].d_list);
                const Obj& X = mX;

                ASSERTV(Y, X, Y == X);

                ASSERT(&da == X.get_allocator());

            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERT(xscratch == Y.get_allocator());
        }
        ASSERT(dam.isTotalUp());
    }

    {
        const struct {
            int                          d_line;    // source line number
            std::initializer_list<TYPE>  d_list;    // source list
            const char                  *d_result;  // expected result
        } DATA[] = {
            //line  list                   result
            //----  --------------------   ------
            { L_,   {                  },  ""     },
            { L_,   { V[0]             },  "A"    },
            { L_,   { V[0], V[0]       },  "AA"   },
            { L_,   { V[1], V[0]       },  "BA"   },
            { L_,   { V[0], V[1], V[2] },  "ABC"  },
            { L_,   { V[0], V[1], V[0] },  "ABA"  },
        };

        enum { NUM_SPECS = sizeof DATA / sizeof *DATA };

        bslma::TestAllocatorMonitor dam(&da);
        for (int ti = 0; ti < NUM_SPECS; ++ti) {

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            ALLOC                xscratch(&scratch);
            Obj                  mY(xscratch);
            const Obj&           Y = gg(&mY, DATA[ti].d_result);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(da) {
                if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                Obj        mX = DATA[ti].d_list;
                const Obj& X  = mX;

                ASSERTV(Y, X, Y == X);

                ASSERT(&da == X.get_allocator());

            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERT(xscratch == Y.get_allocator());
        }
        ASSERT(dam.isTotalUp());
    }

    if (verbose)
        printf("\nTesting 'operator=' with initializer lists.\n");

    {
        const struct {
            int                          d_line;    // source line number
            const char                  *d_spec;    // target string
            std::initializer_list<TYPE>  d_list;    // source list
            const char                  *d_result;  // expected result
        } DATA[] = {
            //line  lhs    list                   result
            //----  ----   ---------------------  ------
            { L_,   "",    {                  },  ""     },
            { L_,   "",    { V[0]             },  "A"    },
            { L_,   "A",   {                  },  ""     },
            { L_,   "A",   { V[1]             },  "B"    },
            { L_,   "A",   { V[0], V[1]       },  "AB"   },
            { L_,   "A",   { V[1], V[2]       },  "BC"   },
            { L_,   "AB",  {                  },  ""     },
            { L_,   "AB",  { V[0], V[1], V[2] },  "ABC"  },
            { L_,   "AB",  { V[2], V[3], V[4] },  "CDE"  },
        };

        enum { NUM_SPECS = sizeof DATA / sizeof *DATA };

        bslma::TestAllocator        oa("object", veryVeryVeryVerbose);
        ALLOC                       xoa(&oa);
        bslma::TestAllocatorMonitor dam(&da);

        for (int ti = 0; ti < NUM_SPECS; ++ti) {

            ASSERT(0 == oa.numBytesInUse());

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            ALLOC                xscratch(&scratch);
            Obj                  mY(xscratch);
            const Obj&           Y = gg(&mY, DATA[ti].d_result);

            Obj        mX(xoa);
            const Obj& X = gg(&mX, DATA[ti].d_spec);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                Obj *mR = &(mX = DATA[ti].d_list);
                ASSERTV(mR, &mX, mR == &mX);
                ASSERTV(Y,  X,   Y  == X  );

            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        }
        ASSERT(dam.isTotalSame());
    }

    if (verbose)
        printf("\tTesting 'assign' with initializer lists\n");

    {
        const struct {
            int                          d_line;   // source line number
            const char                  *d_spec;   // target string
            std::initializer_list<TYPE>  d_list;   // source list
            const char                  *d_result; // expected result
        } DATA[] = {
            //line  lhs    list                   result
            //----  ----   ---------------------  ------
            { L_,   "",    {                  },  ""     },
            { L_,   "",    { V[0]             },  "A"    },
            { L_,   "A",   {                  },  ""     },
            { L_,   "A",   { V[1]             },  "B"    },
            { L_,   "A",   { V[0], V[1]       },  "AB"   },
            { L_,   "A",   { V[1], V[2]       },  "BC"   },
            { L_,   "AB",  {                  },  ""     },
            { L_,   "AB",  { V[0], V[1], V[2] },  "ABC"  },
            { L_,   "AB",  { V[2], V[3], V[4] },  "CDE"  },
        };

        enum { NUM_SPECS = sizeof DATA / sizeof *DATA };

        bslma::TestAllocator        oa("object", veryVeryVeryVerbose);
        ALLOC                       xoa(&oa);
        bslma::TestAllocatorMonitor dam(&da);

        for (int ti = 0; ti < NUM_SPECS; ++ti) {

            ASSERT(0 == oa.numBytesInUse());

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            ALLOC                xscratch(&scratch);
            Obj                  mY(xscratch);
            const Obj&           Y = gg(&mY, DATA[ti].d_result);

            Obj        mX(xoa);
            const Obj& X = gg(&mX, DATA[ti].d_spec);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                mX.assign(DATA[ti].d_list);
                ASSERTV(Y, X, Y == X);

            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        }
        ASSERT(dam.isTotalSame());
    }

    if (verbose)
        printf("\tTesting 'insert' with initializer lists\n");

    {
        const struct {
            int                          d_line;    // source line number
            const char                  *d_spec;    // target string
            int                          d_pos;     // position to insert
            std::initializer_list<TYPE>  d_list;    // source list
            const char                  *d_result;  // expected result
        } DATA[] = {
            //line  source  pos  list                   result
            //----  ------  ---  --------------------   -------
            { L_,   "",     -1,  {                  },  ""      },
            { L_,   "",     99,  { V[0]             },  "A"     },
            { L_,   "A",     0,  {                  },  "A"     },
            { L_,   "A",     0,  { V[1]             },  "BA"    },
            { L_,   "A",     1,  { V[1]             },  "AB"    },
            { L_,   "AB",    0,  {                  },  "AB"    },
            { L_,   "AB",    0,  { V[0], V[1]       },  "ABAB"  },
            { L_,   "AB",    1,  { V[1], V[2]       },  "ABCB"  },
            { L_,   "AB",    2,  { V[0], V[1], V[2] },  "ABABC" },
            { L_,   "ABC",   0,  { V[3]             },  "DABC"  },
        };

        enum { NUM_SPECS = sizeof DATA / sizeof *DATA };

        bslma::TestAllocator        oa("object", veryVeryVeryVerbose);
        ALLOC                       xoa(&oa);
        bslma::TestAllocatorMonitor dam(&da);

        for (int ti = 0; ti < NUM_SPECS; ++ti) {
            const int LINE = DATA[ti].d_line;
            const int POS  = DATA[ti].d_pos;

            ASSERT(0 == oa.numBytesInUse());

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            ALLOC                xscratch(&scratch);
            Obj                  mY(xscratch);
            const Obj&           Y = gg(&mY, DATA[ti].d_result);

            Obj        mX(xoa);
            const Obj& X = gg(&mX, DATA[ti].d_spec);

            size_t index = POS == -1 ? 0
                                     : POS == 99 ? X.size()
                                                 : POS;
            iterator result = mX.insert(X.begin() + index, DATA[ti].d_list);
            ASSERTV(LINE, result == X.begin() + index);
            ASSERTV(LINE, X, Y, X == Y);
        }
        ASSERT(dam.isTotalSame());
    }
#endif
}

template <class TYPE, class ALLOC>
template <int N_ARGS,
          int N01,
          int N02,
          int N03,
          int N04,
          int N05,
          int N06,
          int N07,
          int N08,
          int N09,
          int N10>
void
TestDriver3<TYPE, ALLOC>::testCase28a_RunTest(Obj            *target,
                                              const_iterator  pos)
{
    DECLARE_BOOL_CONSTANT(MOVE_01, N01 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_02, N02 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_03, N03 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_04, N04 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_05, N05 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_06, N06 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_07, N07 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_08, N08 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_09, N09 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_10, N10 == 1);

    bslma::TestAllocator *testAlloc = dynamic_cast<bslma::TestAllocator *>(
                                          target->get_allocator().mechanism());
    if (!testAlloc) {
        ASSERT(!"Allocator in test case 30 is not a test allocator!");
        return;                                                       // RETURN
    }
    bslma::TestAllocator& oa = *testAlloc;
    Obj&                  mX = *target;
    const Obj&            X  = mX;

    bslma::TestAllocator aa("args", veryVeryVeryVerbose);
    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

    size_t                  len   = X.size();
    typename Obj::size_type index = pos - X.begin();

    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {

        // Construct all arguments inside the exception test loop as the
        // exception thrown after moving only a portion of arguments leave the
        // moved arguments in a valid, but unspecified state.
        bsls::ObjectBuffer<typename TYPE::ArgType01> BUF01;
        ConstrUtil::construct(BUF01.address(), &aa, V01);
        typename TYPE::ArgType01& A01 = BUF01.object();
        bslma::DestructorGuard<typename TYPE::ArgType01> G01(&A01);

        bsls::ObjectBuffer<typename TYPE::ArgType02> BUF02;
        ConstrUtil::construct(BUF02.address(), &aa, V02);
        typename TYPE::ArgType02& A02 = BUF02.object();
        bslma::DestructorGuard<typename TYPE::ArgType02> G02(&A02);

        bsls::ObjectBuffer<typename TYPE::ArgType03> BUF03;
        ConstrUtil::construct(BUF03.address(), &aa, V03);
        typename TYPE::ArgType03& A03 = BUF03.object();
        bslma::DestructorGuard<typename TYPE::ArgType03> G03(&A03);

        bsls::ObjectBuffer<typename TYPE::ArgType04> BUF04;
        ConstrUtil::construct(BUF04.address(), &aa, V04);
        typename TYPE::ArgType04& A04 = BUF04.object();
        bslma::DestructorGuard<typename TYPE::ArgType04> G04(&A04);

        bsls::ObjectBuffer<typename TYPE::ArgType05> BUF05;
        ConstrUtil::construct(BUF05.address(), &aa, V05);
        typename TYPE::ArgType05& A05 = BUF05.object();
        bslma::DestructorGuard<typename TYPE::ArgType05> G05(&A05);

        bsls::ObjectBuffer<typename TYPE::ArgType06> BUF06;
        ConstrUtil::construct(BUF06.address(), &aa, V06);
        typename TYPE::ArgType06& A06 = BUF06.object();
        bslma::DestructorGuard<typename TYPE::ArgType06> G06(&A06);

        bsls::ObjectBuffer<typename TYPE::ArgType07> BUF07;
        ConstrUtil::construct(BUF07.address(), &aa, V07);
        typename TYPE::ArgType07& A07 = BUF07.object();
        bslma::DestructorGuard<typename TYPE::ArgType07> G07(&A07);

        bsls::ObjectBuffer<typename TYPE::ArgType08> BUF08;
        ConstrUtil::construct(BUF08.address(), &aa,  V08);
        typename TYPE::ArgType08& A08 = BUF08.object();
        bslma::DestructorGuard<typename TYPE::ArgType08> G08(&A08);

        bsls::ObjectBuffer<typename TYPE::ArgType09> BUF09;
        ConstrUtil::construct(BUF09.address(), &aa, V09);
        typename TYPE::ArgType09& A09 = BUF09.object();
        bslma::DestructorGuard<typename TYPE::ArgType09> G09(&A09);

        bsls::ObjectBuffer<typename TYPE::ArgType10> BUF10;
        ConstrUtil::construct(BUF10.address(), &aa, V10);
        typename TYPE::ArgType10& A10 = BUF10.object();
        bslma::DestructorGuard<typename TYPE::ArgType10> G10(&A10);

        ExceptionProctor<Obj, ALLOC> proctor(&X, L_, &scratch);

        switch (N_ARGS) {
          case 0: {
            mX.emplace(pos);
          } break;
          case 1: {
            mX.emplace(pos, testArg(A01, MOVE_01));
          } break;
          case 2: {
            mX.emplace(pos, testArg(A01, MOVE_01), testArg(A02, MOVE_02));
          } break;
          case 3: {
            mX.emplace(pos,
                       testArg(A01, MOVE_01),
                       testArg(A02, MOVE_02),
                       testArg(A03, MOVE_03));
          } break;
          case 4: {
            mX.emplace(pos,
                       testArg(A01, MOVE_01),
                       testArg(A02, MOVE_02),
                       testArg(A03, MOVE_03),
                       testArg(A04, MOVE_04));
          } break;
          case 5: {
            mX.emplace(pos,
                       testArg(A01, MOVE_01),
                       testArg(A02, MOVE_02),
                       testArg(A03, MOVE_03),
                       testArg(A04, MOVE_04),
                       testArg(A05, MOVE_05));
          } break;
          case 6: {
            mX.emplace(pos,
                       testArg(A01, MOVE_01),
                       testArg(A02, MOVE_02),
                       testArg(A03, MOVE_03),
                       testArg(A04, MOVE_04),
                       testArg(A05, MOVE_05),
                       testArg(A06, MOVE_06));
          } break;
          case 7: {
            mX.emplace(pos,
                       testArg(A01, MOVE_01),
                       testArg(A02, MOVE_02),
                       testArg(A03, MOVE_03),
                       testArg(A04, MOVE_04),
                       testArg(A05, MOVE_05),
                       testArg(A06, MOVE_06),
                       testArg(A07, MOVE_07));
          } break;
          case 8: {
            mX.emplace(pos,
                       testArg(A01, MOVE_01),
                       testArg(A02, MOVE_02),
                       testArg(A03, MOVE_03),
                       testArg(A04, MOVE_04),
                       testArg(A05, MOVE_05),
                       testArg(A06, MOVE_06),
                       testArg(A07, MOVE_07),
                       testArg(A08, MOVE_08));
          } break;
          case 9: {
            mX.emplace(pos,
                       testArg(A01, MOVE_01),
                       testArg(A02, MOVE_02),
                       testArg(A03, MOVE_03),
                       testArg(A04, MOVE_04),
                       testArg(A05, MOVE_05),
                       testArg(A06, MOVE_06),
                       testArg(A07, MOVE_07),
                       testArg(A08, MOVE_08),
                       testArg(A09, MOVE_09));
          } break;
          case 10: {
            mX.emplace(pos,
                       testArg(A01, MOVE_01),
                       testArg(A02, MOVE_02),
                       testArg(A03, MOVE_03),
                       testArg(A04, MOVE_04),
                       testArg(A05, MOVE_05),
                       testArg(A06, MOVE_06),
                       testArg(A07, MOVE_07),
                       testArg(A08, MOVE_08),
                       testArg(A09, MOVE_09),
                       testArg(A10, MOVE_10));
          } break;
          default: {
            ASSERTV(!"Invalid # of args!");
          } break;
        }
        proctor.release();

        ASSERTV(len + 1, X.size(), len + 1 == X.size());

        ASSERTV(MOVE_01, A01.movedFrom(),
               MOVE_01 == (MoveState::e_MOVED == A01.movedFrom()) || 2 == N01);
        ASSERTV(MOVE_02, A02.movedFrom(),
               MOVE_02 == (MoveState::e_MOVED == A02.movedFrom()) || 2 == N02);
        ASSERTV(MOVE_03, A03.movedFrom(),
               MOVE_03 == (MoveState::e_MOVED == A03.movedFrom()) || 2 == N03);
        ASSERTV(MOVE_04, A04.movedFrom(),
               MOVE_04 == (MoveState::e_MOVED == A04.movedFrom()) || 2 == N04);
        ASSERTV(MOVE_05, A05.movedFrom(),
               MOVE_05 == (MoveState::e_MOVED == A05.movedFrom()) || 2 == N05);
        ASSERTV(MOVE_06, A06.movedFrom(),
               MOVE_06 == (MoveState::e_MOVED == A06.movedFrom()) || 2 == N06);
        ASSERTV(MOVE_07, A07.movedFrom(),
               MOVE_07 == (MoveState::e_MOVED == A07.movedFrom()) || 2 == N07);
        ASSERTV(MOVE_08, A08.movedFrom(),
               MOVE_08 == (MoveState::e_MOVED == A08.movedFrom()) || 2 == N08);
        ASSERTV(MOVE_09, A09.movedFrom(),
               MOVE_09 == (MoveState::e_MOVED == A09.movedFrom()) || 2 == N09);
        ASSERTV(MOVE_10, A10.movedFrom(),
               MOVE_10 == (MoveState::e_MOVED == A10.movedFrom()) || 2 == N10);

        const TYPE& V = X[index];

        ASSERTV(V01, V.arg01(), V01 == V.arg01() || 2 == N01);
        ASSERTV(V02, V.arg02(), V02 == V.arg02() || 2 == N02);
        ASSERTV(V03, V.arg03(), V03 == V.arg03() || 2 == N03);
        ASSERTV(V04, V.arg04(), V04 == V.arg04() || 2 == N04);
        ASSERTV(V05, V.arg05(), V05 == V.arg05() || 2 == N05);
        ASSERTV(V06, V.arg06(), V06 == V.arg06() || 2 == N06);
        ASSERTV(V07, V.arg07(), V07 == V.arg07() || 2 == N07);
        ASSERTV(V08, V.arg08(), V08 == V.arg08() || 2 == N08);
        ASSERTV(V09, V.arg09(), V09 == V.arg09() || 2 == N09);
        ASSERTV(V10, V.arg10(), V10 == V.arg10() || 2 == N10);

        TestAllocatorUtil::test(V, oa);

    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
}

template <class TYPE, class ALLOC>
void TestDriver3<TYPE, ALLOC>::testCase28()
{
    // ------------------------------------------------------------------------
    // TESTING 'emplace(const_iterator position, Args&&...)'
    //
    // Concerns:
    //: 1 A newly created element is inserted at the correct position in the
    //:   container and the order of elements in the container, before and
    //:   after the insertion point, remain correct.
    //:
    //: 2 The capacity is increased as expected.
    //:
    //: 3 The internal memory management system is hooked up properly so that
    //:   *all* internally allocated memory draws from a user-supplied
    //:   allocator whenever one is specified.
    //:
    //: 4 Insertion is exception neutral w.r.t. memory allocation.
    //
    // Plan:
    //: 1 We will use 'value' as the single argument to the 'emplace' function
    //:   and will test proper forwarding of constructor arguments in test
    //:   'testCase28a'.
    //:
    //: 2 For 'emplace' we will create objects of varying sizes and
    //:   capacities containing default values, and emplace a 'value' at
    //:   various positions.
    //:
    //:   1 Verify that the element was added at the expected position in the
    //:     container.(C-1)
    //:
    //:   2 Ensure that the order is preserved for elements before and after
    //:     the insertion point.
    //:
    //:   3 Compute the number of allocations and verify it is as expected.
    //:                                                                   (C-2)
    //:
    //:   4 Verify all allocations are from the object's allocator.       (C-3)
    //:
    //: 3 Repeat P-1 under the presence of exceptions.                    (C-4)
    //
    // Testing:
    //   iterator emplace(Args&&...);
    // ------------------------------------------------------------------------

    const TestValues VALUES;

    const int TYPE_COPY = !k_IS_MOVABLE
                       &&  bsl::is_copy_constructible<TYPE>::value;

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<TYPE>::value ||
                           bsl::uses_allocator<TYPE, ALLOC>::value;

    if (verbose) printf(
         "\nTesting '%s' (TYPE_ALLOC = %d, TYPE_MOVE = %d, TYPE_COPY = %d).\n",
         NameOf<TYPE>().name(),
         TYPE_ALLOC,
         k_IS_MOVABLE,
         TYPE_COPY);

    static const struct {
        int         d_line;     // source line number
        const char *d_spec;     // specification string
        int         d_pos;      // position to insert
        char        d_element;  // specification string
        const char *d_results;  // expected element values
    } DATA[] = {
        //line  spec         pos  element  results
        //----  -----------  ---  -------  -----------
        { L_,   "",          -1,  'Z',     "Z"         },
        { L_,   "",          99,  'Z',     "Z"         },
        { L_,   "A",         -1,  'Z',     "ZA"        },
        { L_,   "A",          0,  'Z',     "ZA"        },
        { L_,   "A",          1,  'Z',     "AZ"        },
        { L_,   "A",         99,  'Z',     "AZ"        },
        { L_,   "AB",        -1,  'B',     "BAB"       },
        { L_,   "AB",         0,  'B',     "BAB"       },
        { L_,   "AB",         1,  'Z',     "AZB"       },
        { L_,   "AB",         2,  'A',     "ABA"       },
        { L_,   "AB",        99,  'Z',     "ABZ"       },
        { L_,   "CAB",       -1,  'Z',     "ZCAB"      },
        { L_,   "CAB",        0,  'A',     "ACAB"      },
        { L_,   "CAB",        1,  'B',     "CBAB"      },
        { L_,   "CAB",        2,  'C',     "CACB"      },
        { L_,   "CAB",        3,  'Z',     "CABZ"      },
        { L_,   "CAB",       99,  'Z',     "CABZ"      },
        { L_,   "CABD",      -1,  'A',     "ACABD"     },
        { L_,   "CABD",       0,  'Z',     "ZCABD"     },
        { L_,   "CABD",       1,  'Z',     "CZABD"     },
        { L_,   "CABD",       2,  'B',     "CABBD"     },
        { L_,   "CABD",       3,  'Z',     "CABZD"     },
        { L_,   "CABD",       4,  'B',     "CABDB"     },
        { L_,   "CABD",      99,  'A',     "CABDA"     },
        { L_,   "HGFEDCBA",   0,  'Z',     "ZHGFEDCBA" },
        { L_,   "HGFEDCBA",   7,  'Z',     "HGFEDCBZA" },
        { L_,   "HGFEDCBA",   8,  'Z',     "HGFEDCBAZ" }
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };
    if (verbose) printf("\tTesting 'emplace' without exceptions.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE     = DATA[ti].d_line;
            const char *const SPEC     = DATA[ti].d_spec;
            const int         POS      = DATA[ti].d_pos;
            const char        ELEMENT  = DATA[ti].d_element;
            const char *const EXPECTED = DATA[ti].d_results;
            const size_t      SIZE     = strlen(SPEC);

            for (char cfg = 'a'; cfg <= 'a'; ++cfg) {
                const char CONFIG = cfg;

                bslma::TestAllocator         da("default",
                                                veryVeryVeryVerbose);
                bslma::DefaultAllocatorGuard dag(&da);

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                ALLOC                xoa(&oa);
                Obj                  mX(xoa);
                const Obj&           X = gg(&mX, SPEC);

                if (veryVerbose) { T_ P_(LINE) P_(CONFIG) P(X) }

                // Verify any attribute allocators are installed properly.

                ASSERTV(LINE, CONFIG, xoa == X.get_allocator());

                ASSERTV(LINE, CONFIG, SIZE, X.size(), SIZE == X.size());

                const bsls::Types::Int64 BB = oa.numBlocksTotal();
                const bsls::Types::Int64 B  = oa.numBlocksInUse();

                size_t index = POS == -1 ? 0
                                         : POS == 99 ? X.size()
                                                     : POS;
                iterator result = mX.emplace(
                                     POS == -1 ? X.begin()
                                               : POS == 99 ? X.end()
                                                           : (X.begin() + POS),
                                     VALUES[ELEMENT - 'A']);

                if (veryVerbose) { T_ P_(LINE) P_(CONFIG) P_(ELEMENT) P(X) }

                const bsls::Types::Int64 AA = oa.numBlocksTotal();
                const bsls::Types::Int64 A  = oa.numBlocksInUse();

                ASSERTV(LINE, CONFIG, SIZE, X.size(), SIZE + 1 == X.size());
                ASSERTV(LINE, CONFIG, result == X.begin() + index);
                ASSERTV(LINE, CONFIG, *result == VALUES[ELEMENT - 'A']);

                TestValues exp(EXPECTED);
                ASSERTV(LINE, CONFIG, 0 == verifyContainer(X, exp, SIZE + 1));

                // Vector increases capacity only if the current length is zero
                // or a power of 2.  In addition, when the type allocates, an
                // extra allocation is used for the new element, and when the
                // type is not bitwise moveable, size() allocations are used
                // during the move, but an equal amount is destroyed thus the
                // number of blocks in use is unchanged.

                if (expectToAllocate(SIZE))  {
                    const bsls::Types::Int64 EXP = BB
                          +  1                                  // realloc
                          +  TYPE_ALLOC                         // new element
                          +  TYPE_ALLOC * (SIZE * TYPE_COPY);   // SIZE MOVES
                    ASSERTV(LINE, CONFIG, BB, AA, EXP, AA == EXP);
                }
                else {
                    const bsls::Types::Int64 EXP = BB
                          +  TYPE_ALLOC                          // new element
                          +  TYPE_ALLOC * TYPE_COPY * (index != SIZE) // temp
                          +  TYPE_ALLOC * ((SIZE - index) * TYPE_COPY);
                                                          // SIZE - index MOVES
                    ASSERTV(LINE, CONFIG, BB, AA, EXP, AA == EXP);
                }
                ASSERTV(LINE, CONFIG, SIZE, B, A,
                        B + (SIZE == 0) + TYPE_ALLOC == A);
            }
        }
    }

    // TBD: There is no strong exception guarantee when the copy constructor
    // throws during 'emplace' of a single element
    if (verbose) printf("\tTesting 'emplace' with injected exceptions.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE     = DATA[ti].d_line;
            const char *const SPEC     = DATA[ti].d_spec;
            const int         POS      = DATA[ti].d_pos;
            const char        ELEMENT  = DATA[ti].d_element;
            const char *const EXPECTED = DATA[ti].d_results;
            const size_t      SIZE     = strlen(SPEC);
            for (char cfg = 'a'; cfg <= 'a'; ++cfg) {
                bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                ALLOC                xoa(&oa);
                // TBD: since there is no strong exception guarantee, the
                //      following three lines moved to within the exception
                //      test block:
                //..
                //  Obj        mX(xoa);
                //  const Obj& X = gg(&mX, SPEC);
                //  ASSERTV(LINE, SIZE, X.size(), SIZE == X.size());
                //..

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
                ALLOC                xscratch(&scratch);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {

                    const bsls::Types::Int64 AL = oa.allocationLimit();
                    oa.setAllocationLimit(-1);
                    Obj        mX(xoa);
                    const Obj& X = gg(&mX, SPEC);

                    ASSERTV(LINE, SIZE, X.size(), SIZE == X.size());

                    oa.setAllocationLimit(AL);

                    size_t index = POS == -1 ? 0
                                 : POS == 99 ? X.size()
                                 :             POS;

                    // The strong exception guarantee is in effect only if
                    // inserting at the end.
                    ExceptionProctor<Obj, ALLOC> proctor(
                                                        index == SIZE ? &X : 0,
                                                        LINE,
                                                        xscratch);

                    iterator result = mX.emplace(
                                     POS == -1 ? X.begin()
                                               : POS == 99 ? X.end()
                                                           : (X.begin() + POS),
                                     VALUES[ELEMENT - 'A']);

                    ASSERTV(LINE, SIZE, X.size(), SIZE + 1 == X.size());
                    ASSERTV(LINE, SIZE,  result == X.begin() + index);
                    ASSERTV(LINE, SIZE, *result == VALUES[ELEMENT - 'A']);

                    TestValues exp(EXPECTED);
                    ASSERTV(LINE, 0 == verifyContainer(X, exp, SIZE + 1));

                    proctor.release();
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            }
        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver3<TYPE, ALLOC>::testCase28a()
{
    // ------------------------------------------------------------------------
    // TESTING FORWARDING OF ARGUMENTS WITH 'emplace'
    //
    // Concerns:
    //: 1 'emplace' correctly forwards arguments to the constructor of the
    //:   value type, up to 10 arguments, the max number of arguments provided
    //:   for C++03 compatibility.  Note that only the forwarding of arguments
    //:   is tested in this function; all other functionality is tested in
    //:  'testCase30'.
    //:
    //: 2 'emplace' is exception neutral w.r.t. memory allocation.
    //
    // Plan:
    //: 1 This test makes material use of template method 'testCase30a_RunTest'
    //:   with first integer template parameter indicating the number of
    //:   arguments to use, the next 10 integer template parameters indicating
    //:   '0' for copy, '1' for move, and '2' for not-applicable (i.e., beyond
    //:   the number of arguments), and taking as the only argument a pointer
    //:   to a modifiable container.
    //:   1 Create 10 argument values with their own argument-specific
    //:     allocator.
    //:
    //:   2 Based on (first) template parameter indicating the number of args
    //:     to pass in, call 'emplace' with the corresponding argument values,
    //:     performing an explicit move of the argument if so indicated by
    //:     the template parameter corresponding to the argument, all in the
    //:     presence of injected exceptions.
    //:
    //:   3 Verify that the argument values were passed correctly.
    //:
    //:   4 Verify that the allocator was forwarded correctly.
    //:
    //:   5 Verify that the move-state for each argument is as expected.
    //:
    //:   6 If the object did not contain the emplaced value, verify it now
    //:     exists.  Otherwise, verify the return value is as expected.
    //:
    //: 2 Create a container with it's own object-specific allocator.
    //:
    //: 3 Call 'testCase31a_RunTest' in various configurations:
    //:   1 For 1..10 arguments, call with the move flag set to '1' and then
    //:     with the move flag set to '0'.
    //:
    //:   2 For 1, 2, 3, and 10 arguments, call with move flags set to '0',
    //:     '1', and each move flag set independently.
    //:
    //
    // Testing:
    //   iterator emplace(Args&&... args);
    // ------------------------------------------------------------------------

#ifndef BSL_DO_NOT_TEST_MOVE_FORWARDING
    if (verbose) printf("\tTesting emplace 1..10 args, move=1.\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);
        const Obj&           X = mX;

        testCase28a_RunTest< 0,2,2,2,2,2,2,2,2,2,2>(&mX, X.end());
        testCase28a_RunTest< 1,1,2,2,2,2,2,2,2,2,2>(&mX, X.end());
        testCase28a_RunTest< 2,1,1,2,2,2,2,2,2,2,2>(&mX, X.end());
        testCase28a_RunTest< 3,1,1,1,2,2,2,2,2,2,2>(&mX, X.end());
        testCase28a_RunTest< 4,1,1,1,1,2,2,2,2,2,2>(&mX, X.end());
        testCase28a_RunTest< 5,1,1,1,1,1,2,2,2,2,2>(&mX, X.end());
        testCase28a_RunTest< 6,1,1,1,1,1,1,2,2,2,2>(&mX, X.end());
        testCase28a_RunTest< 7,1,1,1,1,1,1,1,2,2,2>(&mX, X.end());
        testCase28a_RunTest< 8,1,1,1,1,1,1,1,1,2,2>(&mX, X.end());
        testCase28a_RunTest< 9,1,1,1,1,1,1,1,1,1,2>(&mX, X.end());
        testCase28a_RunTest<10,1,1,1,1,1,1,1,1,1,1>(&mX, X.end());
    }
    if (verbose) printf("\tTesting emplace 1..10 args, move=0.\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);
        const Obj&           X = mX;

        testCase28a_RunTest< 0,2,2,2,2,2,2,2,2,2,2>(&mX, X.end());
        testCase28a_RunTest< 1,0,2,2,2,2,2,2,2,2,2>(&mX, X.end());
        testCase28a_RunTest< 2,0,0,2,2,2,2,2,2,2,2>(&mX, X.end());
        testCase28a_RunTest< 3,0,0,0,2,2,2,2,2,2,2>(&mX, X.end());
        testCase28a_RunTest< 4,0,0,0,0,2,2,2,2,2,2>(&mX, X.end());
        testCase28a_RunTest< 5,0,0,0,0,0,2,2,2,2,2>(&mX, X.end());
        testCase28a_RunTest< 6,0,0,0,0,0,0,2,2,2,2>(&mX, X.end());
        testCase28a_RunTest< 7,0,0,0,0,0,0,0,2,2,2>(&mX, X.end());
        testCase28a_RunTest< 8,0,0,0,0,0,0,0,0,2,2>(&mX, X.end());
        testCase28a_RunTest< 9,0,0,0,0,0,0,0,0,0,2>(&mX, X.end());
        testCase28a_RunTest<10,0,0,0,0,0,0,0,0,0,0>(&mX, X.end());
    }
    if (verbose) printf("\tTesting emplace with 0 args.\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);
        const Obj&           X = mX;

        testCase28a_RunTest<0,2,2,2,2,2,2,2,2,2,2>(&mX, X.end());
    }
    if (verbose) printf("\tTesting emplace with 1 args.\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);
        const Obj&           X = mX;

        testCase28a_RunTest<1,0,2,2,2,2,2,2,2,2,2>(&mX, X.end());
        testCase28a_RunTest<1,1,2,2,2,2,2,2,2,2,2>(&mX, X.end());
    }
    if (verbose) printf("\tTesting emplace with 2 args.\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);
        const Obj&           X = mX;

        testCase28a_RunTest<2,0,0,2,2,2,2,2,2,2,2>(&mX, X.end());
        testCase28a_RunTest<2,1,0,2,2,2,2,2,2,2,2>(&mX, X.end());
        testCase28a_RunTest<2,0,1,2,2,2,2,2,2,2,2>(&mX, X.end());
        testCase28a_RunTest<2,1,1,2,2,2,2,2,2,2,2>(&mX, X.end());
    }
    if (verbose) printf("\tTesting emplace with 3 args.\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);
        const Obj&           X = mX;

        testCase28a_RunTest<3,0,0,0,2,2,2,2,2,2,2>(&mX, X.end());
        testCase28a_RunTest<3,1,0,0,2,2,2,2,2,2,2>(&mX, X.end());
        testCase28a_RunTest<3,0,1,0,2,2,2,2,2,2,2>(&mX, X.end());
        testCase28a_RunTest<3,0,0,1,2,2,2,2,2,2,2>(&mX, X.end());
        testCase28a_RunTest<3,1,1,1,2,2,2,2,2,2,2>(&mX, X.end());
    }

    if (verbose) printf("\tTesting emplace with 10 args.\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);
        const Obj&           X = mX;

        testCase28a_RunTest<10,0,0,0,0,0,0,0,0,0,0>(&mX, X.end());
        testCase28a_RunTest<10,1,0,0,0,0,0,0,0,0,0>(&mX, X.end());
        testCase28a_RunTest<10,0,1,0,0,0,0,0,0,0,0>(&mX, X.end());
        testCase28a_RunTest<10,0,0,1,0,0,0,0,0,0,0>(&mX, X.end());
        testCase28a_RunTest<10,0,0,0,1,0,0,0,0,0,0>(&mX, X.end());
        testCase28a_RunTest<10,0,0,0,0,1,0,0,0,0,0>(&mX, X.end());
        testCase28a_RunTest<10,0,0,0,0,0,1,0,0,0,0>(&mX, X.end());
        testCase28a_RunTest<10,0,0,0,0,0,0,1,0,0,0>(&mX, X.end());
        testCase28a_RunTest<10,0,0,0,0,0,0,0,1,0,0>(&mX, X.end());
        testCase28a_RunTest<10,0,0,0,0,0,0,0,0,1,0>(&mX, X.end());
        testCase28a_RunTest<10,0,0,0,0,0,0,0,0,0,1>(&mX, X.end());
        testCase28a_RunTest<10,1,1,1,1,1,1,1,1,1,1>(&mX, X.end());
    }
#else
    if (verbose) printf("\tTesting emplace 1..10 args, move=0.\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);
        const Obj&           X = mX;

        testCase28a_RunTest< 0,2,2,2,2,2,2,2,2,2,2>(&mX, X.end());
        testCase28a_RunTest< 1,0,2,2,2,2,2,2,2,2,2>(&mX, X.end());
        testCase28a_RunTest< 2,0,0,2,2,2,2,2,2,2,2>(&mX, X.end());
        testCase28a_RunTest< 3,0,0,0,2,2,2,2,2,2,2>(&mX, X.end());
        testCase28a_RunTest< 4,0,0,0,0,2,2,2,2,2,2>(&mX, X.end());
        testCase28a_RunTest< 5,0,0,0,0,0,2,2,2,2,2>(&mX, X.end());
        testCase28a_RunTest< 6,0,0,0,0,0,0,2,2,2,2>(&mX, X.end());
        testCase28a_RunTest< 7,0,0,0,0,0,0,0,2,2,2>(&mX, X.end());
        testCase28a_RunTest< 8,0,0,0,0,0,0,0,0,2,2>(&mX, X.end());
        testCase28a_RunTest< 9,0,0,0,0,0,0,0,0,0,2>(&mX, X.end());
        testCase28a_RunTest<10,0,0,0,0,0,0,0,0,0,0>(&mX, X.end());
    }
#endif
}

template <class TYPE, class ALLOC>
template <int N_ARGS,
          int N01,
          int N02,
          int N03,
          int N04,
          int N05,
          int N06,
          int N07,
          int N08,
          int N09,
          int N10>
void
TestDriver3<TYPE, ALLOC>::testCase26a_RunTest(Obj *target)
{
    DECLARE_BOOL_CONSTANT(MOVE_01, N01 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_02, N02 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_03, N03 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_04, N04 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_05, N05 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_06, N06 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_07, N07 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_08, N08 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_09, N09 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_10, N10 == 1);

    bslma::TestAllocator *testAlloc = dynamic_cast<bslma::TestAllocator *>(
                                          target->get_allocator().mechanism());
    if (!testAlloc) {
        ASSERT(!"Allocator in test case 30 is not a test allocator!");
        return;                                                       // RETURN
    }
    bslma::TestAllocator& oa = *testAlloc;
    Obj&                  mX = *target;
    const Obj&            X  = mX;

    bslma::TestAllocator aa("args", veryVeryVeryVerbose);
    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

    size_t len = X.size();

    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {

        // Construct all arguments inside the exception test loop as the
        // exception thrown after moving only a portion of arguments leave the
        // moved arguments in a valid, but unspecified state.
        bsls::ObjectBuffer<typename TYPE::ArgType01> BUF01;
        ConstrUtil::construct(BUF01.address(), &aa, V01);
        typename TYPE::ArgType01& A01 = BUF01.object();
        bslma::DestructorGuard<typename TYPE::ArgType01> G01(&A01);

        bsls::ObjectBuffer<typename TYPE::ArgType02> BUF02;
        ConstrUtil::construct(BUF02.address(), &aa, V02);
        typename TYPE::ArgType02& A02 = BUF02.object();
        bslma::DestructorGuard<typename TYPE::ArgType02> G02(&A02);

        bsls::ObjectBuffer<typename TYPE::ArgType03> BUF03;
        ConstrUtil::construct(BUF03.address(), &aa, V03);
        typename TYPE::ArgType03& A03 = BUF03.object();
        bslma::DestructorGuard<typename TYPE::ArgType03> G03(&A03);

        bsls::ObjectBuffer<typename TYPE::ArgType04> BUF04;
        ConstrUtil::construct(BUF04.address(), &aa, V04);
        typename TYPE::ArgType04& A04 = BUF04.object();
        bslma::DestructorGuard<typename TYPE::ArgType04> G04(&A04);

        bsls::ObjectBuffer<typename TYPE::ArgType05> BUF05;
        ConstrUtil::construct(BUF05.address(), &aa, V05);
        typename TYPE::ArgType05& A05 = BUF05.object();
        bslma::DestructorGuard<typename TYPE::ArgType05> G05(&A05);

        bsls::ObjectBuffer<typename TYPE::ArgType06> BUF06;
        ConstrUtil::construct(BUF06.address(), &aa, V06);
        typename TYPE::ArgType06& A06 = BUF06.object();
        bslma::DestructorGuard<typename TYPE::ArgType06> G06(&A06);

        bsls::ObjectBuffer<typename TYPE::ArgType07> BUF07;
        ConstrUtil::construct(BUF07.address(), &aa, V07);
        typename TYPE::ArgType07& A07 = BUF07.object();
        bslma::DestructorGuard<typename TYPE::ArgType07> G07(&A07);

        bsls::ObjectBuffer<typename TYPE::ArgType08> BUF08;
        ConstrUtil::construct(BUF08.address(), &aa,  V08);
        typename TYPE::ArgType08& A08 = BUF08.object();
        bslma::DestructorGuard<typename TYPE::ArgType08> G08(&A08);

        bsls::ObjectBuffer<typename TYPE::ArgType09> BUF09;
        ConstrUtil::construct(BUF09.address(), &aa, V09);
        typename TYPE::ArgType09& A09 = BUF09.object();
        bslma::DestructorGuard<typename TYPE::ArgType09> G09(&A09);

        bsls::ObjectBuffer<typename TYPE::ArgType10> BUF10;
        ConstrUtil::construct(BUF10.address(), &aa, V10);
        typename TYPE::ArgType10& A10 = BUF10.object();
        bslma::DestructorGuard<typename TYPE::ArgType10> G10(&A10);

        ExceptionProctor<Obj, ALLOC> proctor(&X, L_, &scratch);
        switch (N_ARGS) {
          case 0: {
            mX.emplace_back();
          } break;
          case 1: {
            mX.emplace_back(testArg(A01, MOVE_01));
          } break;
          case 2: {
            mX.emplace_back(testArg(A01, MOVE_01), testArg(A02, MOVE_02));
          } break;
          case 3: {
            mX.emplace_back(testArg(A01, MOVE_01),
                            testArg(A02, MOVE_02),
                            testArg(A03, MOVE_03));
          } break;
          case 4: {
            mX.emplace_back(testArg(A01, MOVE_01),
                            testArg(A02, MOVE_02),
                            testArg(A03, MOVE_03),
                            testArg(A04, MOVE_04));
          } break;
          case 5: {
            mX.emplace_back(testArg(A01, MOVE_01),
                            testArg(A02, MOVE_02),
                            testArg(A03, MOVE_03),
                            testArg(A04, MOVE_04),
                            testArg(A05, MOVE_05));
          } break;
          case 6: {
            mX.emplace_back(testArg(A01, MOVE_01),
                            testArg(A02, MOVE_02),
                            testArg(A03, MOVE_03),
                            testArg(A04, MOVE_04),
                            testArg(A05, MOVE_05),
                            testArg(A06, MOVE_06));
          } break;
          case 7: {
            mX.emplace_back(testArg(A01, MOVE_01),
                            testArg(A02, MOVE_02),
                            testArg(A03, MOVE_03),
                            testArg(A04, MOVE_04),
                            testArg(A05, MOVE_05),
                            testArg(A06, MOVE_06),
                            testArg(A07, MOVE_07));
          } break;
          case 8: {
            mX.emplace_back(testArg(A01, MOVE_01),
                            testArg(A02, MOVE_02),
                            testArg(A03, MOVE_03),
                            testArg(A04, MOVE_04),
                            testArg(A05, MOVE_05),
                            testArg(A06, MOVE_06),
                            testArg(A07, MOVE_07),
                            testArg(A08, MOVE_08));
          } break;
          case 9: {
            mX.emplace_back(testArg(A01, MOVE_01),
                            testArg(A02, MOVE_02),
                            testArg(A03, MOVE_03),
                            testArg(A04, MOVE_04),
                            testArg(A05, MOVE_05),
                            testArg(A06, MOVE_06),
                            testArg(A07, MOVE_07),
                            testArg(A08, MOVE_08),
                            testArg(A09, MOVE_09));
          } break;
          case 10: {
            mX.emplace_back(testArg(A01, MOVE_01),
                            testArg(A02, MOVE_02),
                            testArg(A03, MOVE_03),
                            testArg(A04, MOVE_04),
                            testArg(A05, MOVE_05),
                            testArg(A06, MOVE_06),
                            testArg(A07, MOVE_07),
                            testArg(A08, MOVE_08),
                            testArg(A09, MOVE_09),
                            testArg(A10, MOVE_10));
          } break;
          default: {
            ASSERTV(!"Invalid # of args!");
          } break;
        }
        proctor.release();

        ASSERTV(len + 1, X.size(), len + 1 == X.size());

        ASSERTV(MOVE_01, A01.movedFrom(),
               MOVE_01 == (MoveState::e_MOVED == A01.movedFrom()) || 2 == N01);
        ASSERTV(MOVE_02, A02.movedFrom(),
               MOVE_02 == (MoveState::e_MOVED == A02.movedFrom()) || 2 == N02);
        ASSERTV(MOVE_03, A03.movedFrom(),
               MOVE_03 == (MoveState::e_MOVED == A03.movedFrom()) || 2 == N03);
        ASSERTV(MOVE_04, A04.movedFrom(),
               MOVE_04 == (MoveState::e_MOVED == A04.movedFrom()) || 2 == N04);
        ASSERTV(MOVE_05, A05.movedFrom(),
               MOVE_05 == (MoveState::e_MOVED == A05.movedFrom()) || 2 == N05);
        ASSERTV(MOVE_06, A06.movedFrom(),
               MOVE_06 == (MoveState::e_MOVED == A06.movedFrom()) || 2 == N06);
        ASSERTV(MOVE_07, A07.movedFrom(),
               MOVE_07 == (MoveState::e_MOVED == A07.movedFrom()) || 2 == N07);
        ASSERTV(MOVE_08, A08.movedFrom(),
               MOVE_08 == (MoveState::e_MOVED == A08.movedFrom()) || 2 == N08);
        ASSERTV(MOVE_09, A09.movedFrom(),
               MOVE_09 == (MoveState::e_MOVED == A09.movedFrom()) || 2 == N09);
        ASSERTV(MOVE_10, A10.movedFrom(),
               MOVE_10 == (MoveState::e_MOVED == A10.movedFrom()) || 2 == N10);

        const TYPE& V = X[len];

        ASSERTV(V01, V.arg01(), V01 == V.arg01() || 2 == N01);
        ASSERTV(V02, V.arg02(), V02 == V.arg02() || 2 == N02);
        ASSERTV(V03, V.arg03(), V03 == V.arg03() || 2 == N03);
        ASSERTV(V04, V.arg04(), V04 == V.arg04() || 2 == N04);
        ASSERTV(V05, V.arg05(), V05 == V.arg05() || 2 == N05);
        ASSERTV(V06, V.arg06(), V06 == V.arg06() || 2 == N06);
        ASSERTV(V07, V.arg07(), V07 == V.arg07() || 2 == N07);
        ASSERTV(V08, V.arg08(), V08 == V.arg08() || 2 == N08);
        ASSERTV(V09, V.arg09(), V09 == V.arg09() || 2 == N09);
        ASSERTV(V10, V.arg10(), V10 == V.arg10() || 2 == N10);

        TestAllocatorUtil::test(V, oa);

    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
}

template <class TYPE, class ALLOC>
void
TestDriver3<TYPE, ALLOC>::testCase26_EmplaceDefault(Obj*, bsl::false_type)
{
    // Do nothing
}

template <class TYPE, class ALLOC>
void TestDriver3<TYPE, ALLOC>::testCase26_EmplaceDefault(Obj* objPtr,
                                                         bsl::true_type)
    // This method verifies that 'emplace_back()' will append a single
    // value-initialized element to the specified vector 'obj', and then 'pop'
    // the vector to leave with its original value, which can be verified by
    // the caller.  Deferring such validation to the caller resolves any issues
    // with non-copyable 'TYPE's.
{
    const Obj& OBJ = *objPtr;

    const typename Obj::size_type ORIGINAL_SIZE = OBJ.size();
    const TYPE&                   RESULT        = objPtr->emplace_back();

    const TYPE *ADDRESS_OF_RESULT       = bsls::Util::addressOf(RESULT);
    const TYPE *ADDRESS_OF_LAST_ELEMENT = bsls::Util::addressOf(OBJ.back());

    ASSERTV(ORIGINAL_SIZE + 1       == OBJ.size());
    ASSERTV(ADDRESS_OF_LAST_ELEMENT == ADDRESS_OF_RESULT);
    ASSERTV(OBJ.back()              == TYPE());

    objPtr->pop_back();
    ASSERTV(ORIGINAL_SIZE == OBJ.size());
}

template <class TYPE, class ALLOC>
void TestDriver3<TYPE, ALLOC>::testCase26()
{
    // ------------------------------------------------------------------------
    // TESTING 'emplace_back(Args&&...)'
    //   This test case tests the 'emplace_back' method with only a single
    //   argument, like the 'push_back' test.  Test case 27a will verify the
    //   perfect forwarding of multiple arguments to the constructor.
    //
    // Concerns:
    //: 1 A newly created element is added to the end of the container and the
    //:   order of the container remains correct.
    //:
    //: 2 The capacity is increased as expected.
    //:
    //: 3 The returned reference provides access to the inserted element.
    //:
    //: 4 The internal memory management system is hooked up properly so that
    //:   *all* internally allocated memory draws from a user-supplied
    //:   allocator whenever one is specified.
    //:
    //: 5 Insertion is exception neutral w.r.t. memory allocation.
    //
    // Plan:
    //: 1 We will use 'value' as the single argument to the 'emplace_back'
    //:   function and will test proper forwarding of constructor arguments
    //:   in test 'testCase26a'.
    //:
    //: 2 For 'emplace_back' we will create objects of varying sizes and
    //:   capacities containing default values, and insert a 'value' at the
    //:   end.
    //:
    //:   1 Verify that the element was added to the end of the container.
    //:
    //:   2 Ensure that the order is preserved for elements before and after
    //:     the insertion point.                                       (C-1..2)
    //:
    //:   3 Verify that the returned reference provides access to the
    //:     inserted value.  (C-3)
    //:
    //:   4 Compute the number of allocations and verify it is as expected.
    //:
    //:   5 Verify all allocations are from the object's allocator.       (C-4)
    //:
    //: 3 Repeat P-1 under the presence of exceptions.                    (C-5)
    //
    // Testing:
    //   reference emplace_back(Args&&... args);
    // ------------------------------------------------------------------------

    const TestValues VALUES;

    const int TYPE_COPY = !k_IS_MOVABLE
                       &&  bsl::is_copy_constructible<TYPE>::value;

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<TYPE>::value ||
                           bsl::uses_allocator<TYPE, ALLOC>::value;

    if (verbose) printf(
         "\nTesting '%s' (TYPE_ALLOC = %d, TYPE_MOVE = %d, TYPE_COPY = %d).\n",
         NameOf<TYPE>().name(),
         TYPE_ALLOC,
         k_IS_MOVABLE,
         TYPE_COPY);

    static const struct {
        int         d_line;     // source line number
        const char *d_spec;     // specification string
        char        d_element;  // specification string
        const char *d_results;  // expected element values
    } DATA[] = {
        //line  spec                 element   results
        //----  -------------------  --------  -------------------
        { L_,   "",                  'A',      "A"                 },
        { L_,   "A",                 'A',      "AA"                },
        { L_,   "A",                 'B',      "AB"                },
        { L_,   "B",                 'A',      "BA"                },
        { L_,   "AB",                'A',      "ABA"               },
        { L_,   "BC",                'D',      "BCD"               },
        { L_,   "BCA",               'Z',      "BCAZ"              },
        { L_,   "CAB",               'C',      "CABC"              },
        { L_,   "CDAB",              'D',      "CDABD"             },
        { L_,   "DABC",              'Z',      "DABCZ"             },
        { L_,   "ABCDE",             'Z',      "ABCDEZ"            },
        { L_,   "EDCBA",             'E',      "EDCBAE"            },
        { L_,   "ABCDEA",            'E',      "ABCDEAE"           },
        { L_,   "ABCDEAB",           'Z',      "ABCDEABZ"          },
        { L_,   "BACDEABC",          'D',      "BACDEABCD"         },
        { L_,   "CBADEABCD",         'Z',      "CBADEABCDZ"        },
        { L_,   "CBADEABCDAB",       'B',      "CBADEABCDABB"      },
        { L_,   "CBADEABCDABC",      'Z',      "CBADEABCDABCZ"     },
        { L_,   "CBADEABCDABCDE",    'B',      "CBADEABCDABCDEB"   },
        { L_,   "CBADEABCDABCDEA",   'E',      "CBADEABCDABCDEAE"  },
        { L_,   "CBADEABCDABCDEAB",  'Z',      "CBADEABCDABCDEABZ" }
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    if (verbose) printf("\tTesting 'emplace_back' without exceptions.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE     = DATA[ti].d_line;
            const char *const SPEC     = DATA[ti].d_spec;
            const char        ELEMENT  = DATA[ti].d_element;
            const char *const EXPECTED = DATA[ti].d_results;
            const size_t      SIZE     = strlen(SPEC);

            bslma::TestAllocator         da("default", veryVeryVeryVerbose);
            bslma::DefaultAllocatorGuard dag(&da);

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            ALLOC                xoa(&oa);
            Obj                  mX(xoa);
            const Obj&           X = gg(&mX, SPEC);

            if (veryVerbose) { T_ P_(LINE) P(X) }

            // Verify any attribute allocators are installed properly.

            ASSERTV(LINE, xoa == X.get_allocator());

            ASSERTV(LINE, SIZE, X.size(), SIZE == X.size());

            TestValues sourceValues(&oa); // Values may be moved from, so need
                                          // the correct allocator for testing.

            const bsls::Types::Int64 BB = oa.numBlocksTotal();
            const bsls::Types::Int64 B  = oa.numBlocksInUse();

            const TYPE& RESULT =
                mX.emplace_back(
                     bslmf::MovableRefUtil::move(sourceValues[ELEMENT - 'A']));

            if (veryVerbose) { T_ P_(LINE) P_(ELEMENT) P(X) }

            const bsls::Types::Int64 AA = oa.numBlocksTotal();
            const bsls::Types::Int64 A  = oa.numBlocksInUse();

            const TYPE *ADDRESS_OF_RESULT = bsls::Util::addressOf(RESULT);
            const TYPE *ADDRESS_OF_LAST_ELEMENT =
                                               bsls::Util::addressOf(X.back());

            ASSERTV(LINE, SIZE, X.size(), SIZE + 1      == X.size());
            ASSERTV(LINE, SIZE, ADDRESS_OF_LAST_ELEMENT == ADDRESS_OF_RESULT);

            TestValues exp(EXPECTED);
            ASSERTV(LINE, 0 == verifyContainer(X, exp, SIZE + 1));

            // Vector increases capacity only if the current length is zero or
            // a power of 2.  In addition, when the type allocates, an extra
            // allocation is used for the new element, and when the type is not
            // bitwise moveable, size() allocations are used during the move,
            // but an equal amount is destroyed thus the number of blocks in
            // use is unchanged.

            if (expectToAllocate(SIZE))  {
                const bsls::Types::Int64 EXP = BB
                              + 1                                // realloc
                              + TYPE_ALLOC                       // new element
                              + TYPE_ALLOC * (SIZE * TYPE_COPY); // SIZE MOVES
                ASSERTV(LINE, BB, AA, EXP, AA == EXP);
            }
            else {
                const bsls::Types::Int64 EXP = BB
                                             + TYPE_ALLOC;       // new element
                ASSERTV(LINE, BB, AA, EXP, AA == EXP);
            }
            ASSERTV(LINE, SIZE, B, A,
                    B + (SIZE == 0) + TYPE_ALLOC == A);

            // Additional test for emplacing a default value, which is not
            // known to 'bsltf' and so would fail scripted tests above that
            // depend on the 'bsltf' identifier for the value.

            // Really want to check early values remain correct, but cannot
            // safely make a copy of container if 'TYPE' is move-only, so 'pop'
            // the newly inserted default item, and repeat the previous
            // validation.

            testCase26_EmplaceDefault(&mX, IsDefaultConstructible<TYPE>());
            ASSERTV(LINE, 0 == verifyContainer(X, exp, SIZE + 1));
        }
    }

    if (verbose)
        printf("\tTesting 'emplace_back' with injected exceptions.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE     = DATA[ti].d_line;
            const char *const SPEC     = DATA[ti].d_spec;
            const char        ELEMENT  = DATA[ti].d_element;
            const char *const EXPECTED = DATA[ti].d_results;
            const size_t      SIZE     = strlen(SPEC);
            for (char cfg = 'a'; cfg <= 'a'; ++cfg) {
                bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                ALLOC                xoa(&oa);
                Obj                  mX(xoa);
                const Obj&           X = gg(&mX, SPEC);

                ASSERTV(LINE, SIZE, X.size(), SIZE == X.size());

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
                ALLOC                xscratch(&scratch);
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    // This method provides the strong exception guarantee.
                    ExceptionProctor<Obj, ALLOC> proctor(&X, L_, xscratch);

                    const TYPE& RESULT =
                                        mX.emplace_back(VALUES[ELEMENT - 'A']);

                    const TYPE *ADDRESS_OF_RESULT =
                                                 bsls::Util::addressOf(RESULT);
                    const TYPE *ADDRESS_OF_LAST_ELEMENT =
                                               bsls::Util::addressOf(X.back());

                    ASSERTV(LINE, SIZE, X.size(), SIZE + 1 == X.size());
                    ASSERTV(LINE, SIZE,
                            ADDRESS_OF_LAST_ELEMENT == ADDRESS_OF_RESULT);

                    TestValues exp(EXPECTED);
                    ASSERTV(LINE, 0 == verifyContainer(X, exp, SIZE + 1));

                    proctor.release();
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            }
        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver3<TYPE, ALLOC>::testCase26a()
{
    // ------------------------------------------------------------------------
    // TESTING FORWARDING OF ARGUMENTS WITH 'emplace_back'
    //
    // Concerns:
    //: 1 'emplace_back' correctly forwards arguments to the constructor of the
    //:   value type, up to 10 arguments, the max number of arguments provided
    //:   for C++03 compatibility.  Note that only the forwarding of arguments
    //:   is tested in this function; all other functionality is tested in
    //:  'testCase30'.
    //:
    //: 2 'emplace' is exception neutral w.r.t. memory allocation.
    //
    // Plan:
    //: 1 This test makes material use of template method 'testCase30a_RunTest'
    //:   with first integer template parameter indicating the number of
    //:   arguments to use, the next 10 integer template parameters indicating
    //:   '0' for copy, '1' for move, and '2' for not-applicable (i.e., beyond
    //:   the number of arguments), and taking as the only argument a pointer
    //:   to a modifiable container.
    //:   1 Create 10 argument values with their own argument-specific
    //:     allocator.
    //:
    //:   2 Based on (first) template parameter indicating the number of args
    //:     to pass in, call 'emplace' with the corresponding argument values,
    //:     performing an explicit move of the argument if so indicated by
    //:     the template parameter corresponding to the argument, all in the
    //:     presence of injected exceptions.
    //:
    //:   3 Verify that the argument values were passed correctly.
    //:
    //:   4 Verify that the allocator was forwarded correctly.
    //:
    //:   5 Verify that the move-state for each argument is as expected.
    //:
    //:   6 If the object did not contain the emplaced value, verify it now
    //:     exists.  Otherwise, verify the return value is as expected.
    //:
    //: 2 Create a container with it's own object-specific allocator.
    //:
    //: 3 Call 'testCase31a_RunTest' in various configurations:
    //:   1 For 1..10 arguments, call with the move flag set to '1' and then
    //:     with the move flag set to '0'.
    //:
    //:   2 For 1, 2, 3, and 10 arguments, call with move flags set to '0',
    //:     '1', and each move flag set independently.
    //:
    //
    // Testing:
    //   reference emplace_back(Args&&... args);
    // ------------------------------------------------------------------------

#ifndef BSL_DO_NOT_TEST_MOVE_FORWARDING
    if (verbose) printf("\tTesting emplace_back 1..10 args, move=1.\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);

        testCase26a_RunTest< 0,2,2,2,2,2,2,2,2,2,2>(&mX);
        testCase26a_RunTest< 1,1,2,2,2,2,2,2,2,2,2>(&mX);
        testCase26a_RunTest< 2,1,1,2,2,2,2,2,2,2,2>(&mX);
        testCase26a_RunTest< 3,1,1,1,2,2,2,2,2,2,2>(&mX);
        testCase26a_RunTest< 4,1,1,1,1,2,2,2,2,2,2>(&mX);
        testCase26a_RunTest< 5,1,1,1,1,1,2,2,2,2,2>(&mX);
        testCase26a_RunTest< 6,1,1,1,1,1,1,2,2,2,2>(&mX);
        testCase26a_RunTest< 7,1,1,1,1,1,1,1,2,2,2>(&mX);
        testCase26a_RunTest< 8,1,1,1,1,1,1,1,1,2,2>(&mX);
        testCase26a_RunTest< 9,1,1,1,1,1,1,1,1,1,2>(&mX);
        testCase26a_RunTest<10,1,1,1,1,1,1,1,1,1,1>(&mX);
    }
    if (verbose) printf("\tTesting emplace_back 1..10 args, move=0.\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);

        testCase26a_RunTest< 0,2,2,2,2,2,2,2,2,2,2>(&mX);
        testCase26a_RunTest< 1,0,2,2,2,2,2,2,2,2,2>(&mX);
        testCase26a_RunTest< 2,0,0,2,2,2,2,2,2,2,2>(&mX);
        testCase26a_RunTest< 3,0,0,0,2,2,2,2,2,2,2>(&mX);
        testCase26a_RunTest< 4,0,0,0,0,2,2,2,2,2,2>(&mX);
        testCase26a_RunTest< 5,0,0,0,0,0,2,2,2,2,2>(&mX);
        testCase26a_RunTest< 6,0,0,0,0,0,0,2,2,2,2>(&mX);
        testCase26a_RunTest< 7,0,0,0,0,0,0,0,2,2,2>(&mX);
        testCase26a_RunTest< 8,0,0,0,0,0,0,0,0,2,2>(&mX);
        testCase26a_RunTest< 9,0,0,0,0,0,0,0,0,0,2>(&mX);
        testCase26a_RunTest<10,0,0,0,0,0,0,0,0,0,0>(&mX);
    }
    if (verbose) printf("\tTesting emplace_back with 0 args.\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);

        testCase26a_RunTest<0,2,2,2,2,2,2,2,2,2,2>(&mX);
    }
    if (verbose) printf("\tTesting emplace_back with 1 args.\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);

        testCase26a_RunTest<1,0,2,2,2,2,2,2,2,2,2>(&mX);
        testCase26a_RunTest<1,1,2,2,2,2,2,2,2,2,2>(&mX);
    }
    if (verbose) printf("\tTesting emplace_back with 2 args.\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);

        testCase26a_RunTest<2,0,0,2,2,2,2,2,2,2,2>(&mX);
        testCase26a_RunTest<2,1,0,2,2,2,2,2,2,2,2>(&mX);
        testCase26a_RunTest<2,0,1,2,2,2,2,2,2,2,2>(&mX);
        testCase26a_RunTest<2,1,1,2,2,2,2,2,2,2,2>(&mX);
    }
    if (verbose) printf("\tTesting emplace_back with 3 args.\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);

        testCase26a_RunTest<3,0,0,0,2,2,2,2,2,2,2>(&mX);
        testCase26a_RunTest<3,1,0,0,2,2,2,2,2,2,2>(&mX);
        testCase26a_RunTest<3,0,1,0,2,2,2,2,2,2,2>(&mX);
        testCase26a_RunTest<3,0,0,1,2,2,2,2,2,2,2>(&mX);
        testCase26a_RunTest<3,1,1,1,2,2,2,2,2,2,2>(&mX);
    }

    if (verbose) printf("\tTesting emplace_back with 10 args.\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);

        testCase26a_RunTest<10,0,0,0,0,0,0,0,0,0,0>(&mX);
        testCase26a_RunTest<10,1,0,0,0,0,0,0,0,0,0>(&mX);
        testCase26a_RunTest<10,0,1,0,0,0,0,0,0,0,0>(&mX);
        testCase26a_RunTest<10,0,0,1,0,0,0,0,0,0,0>(&mX);
        testCase26a_RunTest<10,0,0,0,1,0,0,0,0,0,0>(&mX);
        testCase26a_RunTest<10,0,0,0,0,1,0,0,0,0,0>(&mX);
        testCase26a_RunTest<10,0,0,0,0,0,1,0,0,0,0>(&mX);
        testCase26a_RunTest<10,0,0,0,0,0,0,1,0,0,0>(&mX);
        testCase26a_RunTest<10,0,0,0,0,0,0,0,1,0,0>(&mX);
        testCase26a_RunTest<10,0,0,0,0,0,0,0,0,1,0>(&mX);
        testCase26a_RunTest<10,0,0,0,0,0,0,0,0,0,1>(&mX);
        testCase26a_RunTest<10,1,1,1,1,1,1,1,1,1,1>(&mX);
    }
#else
    if (verbose) printf("\tTesting emplace_back 1..10 args, move=0.\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);

        testCase26a_RunTest< 0,2,2,2,2,2,2,2,2,2,2>(&mX);
        testCase26a_RunTest< 1,0,2,2,2,2,2,2,2,2,2>(&mX);
        testCase26a_RunTest< 2,0,0,2,2,2,2,2,2,2,2>(&mX);
        testCase26a_RunTest< 3,0,0,0,2,2,2,2,2,2,2>(&mX);
        testCase26a_RunTest< 4,0,0,0,0,2,2,2,2,2,2>(&mX);
        testCase26a_RunTest< 5,0,0,0,0,0,2,2,2,2,2>(&mX);
        testCase26a_RunTest< 6,0,0,0,0,0,0,2,2,2,2>(&mX);
        testCase26a_RunTest< 7,0,0,0,0,0,0,0,2,2,2>(&mX);
        testCase26a_RunTest< 8,0,0,0,0,0,0,0,0,2,2>(&mX);
        testCase26a_RunTest< 9,0,0,0,0,0,0,0,0,0,2>(&mX);
        testCase26a_RunTest<10,0,0,0,0,0,0,0,0,0,0>(&mX);
    }
#endif
}

template <class TYPE, class ALLOC>
void TestDriver3<TYPE, ALLOC>::testCase27()
{
    // ------------------------------------------------------------------------
    // TESTING 'iterator insert(const_iterator position, T&&)'
    //
    // Concerns:
    //: 1 A new element is inserted at the correct position in the container
    //:   and the order of elements in the container, before and after the
    //:   insertion point, remain correct.
    //:
    //: 2 The newly inserted item is move-inserted.
    //
    //: 3 The capacity is increased as expected.
    //:
    //: 4 The internal memory management system is hooked up properly so that
    //:   *all* internally allocated memory draws from a user-supplied
    //:   allocator whenever one is specified.
    //:
    //: 5 Insertion is exception neutral w.r.t. memory allocation.
    //
    // Plan:
    //: 1 For 'insert' we will create objects of varying sizes and capacities
    //:   containing default values, and insert a 'value' at various positions.
    //:
    //:   1 Verify that the element was added at the correct position in the
    //:     container.(C-1)
    //:
    //:   2 Verify that the moved-into state for the new element is MOVED.(C-2)
    //:
    //:   3 Compute the number of allocations and verify it is as expected.
    //:                                                                   (C-3)
    //:
    //:   4 Verify all allocations are from the object's allocator.       (C-4)
    //:
    //: 2 Repeat P-1 under the presence of exceptions.                    (C-5)
    //
    // Testing:
    //   iterator insert(const_iterator position, value_type&&)
    // ------------------------------------------------------------------------

    const TestValues VALUES;

    const int TYPE_COPY = !k_IS_MOVABLE
                       &&  bsl::is_copy_constructible<TYPE>::value;

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<TYPE>::value ||
                           bsl::uses_allocator<TYPE, ALLOC>::value;

    if (verbose) printf(
         "\nTesting '%s' (TYPE_ALLOC = %d, TYPE_MOVE = %d, TYPE_COPY = %d).\n",
         NameOf<TYPE>().name(),
         TYPE_ALLOC,
         k_IS_MOVABLE,
         TYPE_COPY);

    static const struct {
        int         d_line;     // source line number
        const char *d_spec;     // specification string
        int         d_pos;      // position to insert
        char        d_element;  // specification string
        const char *d_results;  // expected element values
    } DATA[] = {
        //line  spec         pos  element   results
        //----  -----------  ---  --------  -----------
        { L_,   "",          -1,  'Z',      "Z"         },
        { L_,   "",          99,  'Z',      "Z"         },
        { L_,   "A",         -1,  'Z',      "ZA"        },
        { L_,   "A",          0,  'Z',      "ZA"        },
        { L_,   "A",          1,  'Z',      "AZ"        },
        { L_,   "A",         99,  'Z',      "AZ"        },
        { L_,   "AB",        -1,  'B',      "BAB"       },
        { L_,   "AB",         0,  'B',      "BAB"       },
        { L_,   "AB",         1,  'Z',      "AZB"       },
        { L_,   "AB",         2,  'A',      "ABA"       },
        { L_,   "AB",        99,  'Z',      "ABZ"       },
        { L_,   "CAB",       -1,  'Z',      "ZCAB"      },
        { L_,   "CAB",        0,  'A',      "ACAB"      },
        { L_,   "CAB",        1,  'B',      "CBAB"      },
        { L_,   "CAB",        2,  'C',      "CACB"      },
        { L_,   "CAB",        3,  'Z',      "CABZ"      },
        { L_,   "CAB",       99,  'Z',      "CABZ"      },
        { L_,   "CABD",      -1,  'A',      "ACABD"     },
        { L_,   "CABD",       0,  'Z',      "ZCABD"     },
        { L_,   "CABD",       1,  'Z',      "CZABD"     },
        { L_,   "CABD",       2,  'B',      "CABBD"     },
        { L_,   "CABD",       3,  'Z',      "CABZD"     },
        { L_,   "CABD",       4,  'B',      "CABDB"     },
        { L_,   "CABD",      99,  'A',      "CABDA"     },
        { L_,   "HGFEDCBA",   0,  'Z',      "ZHGFEDCBA" },
        { L_,   "HGFEDCBA",   7,  'Z',      "HGFEDCBZA" },
        { L_,   "HGFEDCBA",   8,  'Z',      "HGFEDCBAZ" }
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    if (verbose) printf("\tTesting 'insert' without exceptions.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE     = DATA[ti].d_line;
            const char *const SPEC     = DATA[ti].d_spec;
            const int         POS      = DATA[ti].d_pos;
            const char        ELEMENT  = DATA[ti].d_element;
            const char *const EXPECTED = DATA[ti].d_results;
            const size_t      SIZE     = strlen(SPEC);

            for (char cfg = 'a'; cfg <= 'b'; ++cfg) {
                const char CONFIG = cfg;

                bslma::TestAllocator         da("default",
                                                veryVeryVeryVerbose);
                bslma::DefaultAllocatorGuard dag(&da);

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                ALLOC                xoa(&oa);
                Obj                  mX(xoa);
                const Obj&           X = gg(&mX, SPEC);

                bslma::TestAllocator  za("different", veryVeryVeryVerbose);
                bslma::TestAllocator *valAllocator = 0;
                switch (cfg) {
                  case 'a': {
                    // inserted value has same allocator
                    valAllocator = &oa;
                  } break;
                  case 'b': {
                    // inserted value has different allocator
                    valAllocator = &za;
                  } break;
                  default: {
                    ASSERTV(CONFIG, !"Bad allocator config.");
                  } return;                                           // RETURN
                }
                bslma::TestAllocator& sa = *valAllocator;
                ALLOC                 xsa(&sa);

                if (veryVerbose) { T_ P_(LINE) P_(CONFIG) P(X) }

                // Verify any attribute allocators are installed properly.

                ASSERTV(LINE, CONFIG, &oa == X.get_allocator());

                ASSERTV(LINE, CONFIG, SIZE, X.size(), SIZE == X.size());

                const bsls::Types::Int64 BB = oa.numBlocksTotal();
                const bsls::Types::Int64 B  = oa.numBlocksInUse();

                bsls::ObjectBuffer<ValueType>  buffer;
                ValueType                     *valptr = buffer.address();
                TstFacility::emplace(valptr, ELEMENT, xsa);

                ASSERTV(SIZE, numNotMovedInto(X), SIZE == numNotMovedInto(X));

                size_t index = POS == -1 ? 0
                             : POS == 99 ? X.size()
                             :             POS;
                iterator result =
                    mX.insert(POS == -1 ? X.begin()
                            : POS == 99 ? X.end()
                            :             (X.begin() + POS),
                              bslmf::MovableRefUtil::move(*valptr));

                MoveState::Enum mState = TstFacility::getMovedFromState(
                                                                      *valptr);
                bslma::DestructionUtil::destroy(valptr);

                if (veryVerbose) { T_ P_(LINE) P_(CONFIG) P_(ELEMENT) P(X) }

                {
                    const MoveState::Enum exp = !s_typeIsMoveEnabled
                                              ? MoveState::e_UNKNOWN
                                              : k_IS_WELL_BEHAVED && &oa != &sa
                                              ? MoveState::e_NOT_MOVED
                                              : MoveState::e_MOVED;

                    ASSERTV(exp, mState, exp == mState);
                }

                const bsls::Types::Int64 AA = oa.numBlocksTotal();
                const bsls::Types::Int64 A  = oa.numBlocksInUse();

                ASSERTV(LINE, CONFIG, SIZE, X.size(), SIZE + 1 == X.size());
                ASSERTV(LINE, CONFIG, result == X.begin() + index);
                ASSERTV(LINE, CONFIG, *result == VALUES[ELEMENT - 'A']);

                TestValues exp(EXPECTED);
                ASSERTV(LINE, CONFIG, 0 == verifyContainer(X, exp, SIZE + 1));

                // Vector increases capacity only if the current length is zero
                // or a power of 2.  In addition, when the type allocates, an
                // extra allocation is used for the new element.  When the type
                // is not bitwise moveable and a move is not performed, size()
                // allocations are used during the move, but an equal amount is
                // destroyed thus the number of blocks in use is unchanged.

                if (expectToAllocate(SIZE)) {
                    ASSERTV(SIZE, numMovedInto(X),
                          !k_IS_MOVABLE || (k_IS_WELL_BEHAVED && &oa != &sa) ||
                                                (SIZE + 1 == numMovedInto(X)));
                    const bsls::Types::Int64 EXP = BB +  1      // realloc
                          +  TYPE_ALLOC                         // new element
                          +  TYPE_ALLOC * (SIZE * TYPE_COPY)    // SIZE copies
                          +  TYPE_ALLOC * (&sa == &oa   // temporary not moved
                                              && MoveState::e_MOVED != mState);
                    ASSERTV(LINE, CONFIG, BB, AA, EXP, AA == EXP);
                }
                else {
                    ASSERTV(SIZE,
                            numMovedInto(X, index + 1),
                            !k_IS_MOVABLE
                         || (SIZE - index) == numMovedInto(X, index + 1));
                    const bsls::Types::Int64 EXP = BB
                          +  TYPE_ALLOC                          // new element
                          +  TYPE_ALLOC * ((SIZE - index) * TYPE_COPY)
                                                         // SIZE - index copies
                          +  TYPE_ALLOC * (&sa == &oa    // temporary not moved
                                              && MoveState::e_MOVED != mState);
                    ASSERTV(LINE, CONFIG, BB, AA, EXP, AA == EXP);
                }
                ASSERTV(LINE, CONFIG, SIZE, B, A,
                        B + (SIZE == 0) + TYPE_ALLOC == A);
            }
        }
    }

    // TBD: The strong exception guarantee applies only when inserting to the
    // end of the array -- so we install the guard conditionally.
    if (verbose) printf("\tTesting 'insert' with injected exceptions.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE     = DATA[ti].d_line;
            const char *const SPEC     = DATA[ti].d_spec;
            const int         POS      = DATA[ti].d_pos;
            const char        ELEMENT  = DATA[ti].d_element;
            const char *const EXPECTED = DATA[ti].d_results;
            const size_t      SIZE     = strlen(SPEC);
            for (char cfg = 'a'; cfg <= 'b'; ++cfg) {
                const char CONFIG = cfg;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                ALLOC                xoa(&oa);

                bslma::TestAllocator         da("default",
                                                veryVeryVeryVerbose);
                bslma::DefaultAllocatorGuard dag(&da);

                bslma::TestAllocator  za("different", veryVeryVeryVerbose);
                bslma::TestAllocator *valAllocator = 0;
                switch (cfg) {
                  case 'a': {
                    // inserted value has same allocator
                    valAllocator = &oa;
                  } break;
                  case 'b': {
                    // inserted value has different allocator
                    valAllocator = &za;
                  } break;
                  default: {
                    ASSERTV(CONFIG, !"Bad allocator config.");
                  } break;
                }
                bslma::TestAllocator& sa = *valAllocator;
                ALLOC                 xsa(&sa);

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
                ALLOC                xscratch(&scratch);

                size_t index = POS == -1 ? 0
                             : POS == 99 ? SIZE
                             :             POS;

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    const bsls::Types::Int64 AL = oa.allocationLimit();
                    oa.setAllocationLimit(-1);
                    Obj mX(xoa);  const Obj& X = gg(&mX, SPEC);

                    ASSERTV(LINE, SIZE, X.size(), SIZE == X.size());

                    bsls::ObjectBuffer<ValueType>  buffer;
                    ValueType                     *valptr = buffer.address();
                    TstFacility::emplace(valptr, ELEMENT, xsa);
                    bslma::DestructorGuard<ValueType> guard(valptr);

                    oa.setAllocationLimit(AL);

                    Obj mZ(xscratch);
                    gg(&mZ, SPEC);

                    // The strong exception guarantee is in effect only if
                    // inserting at the end.
                    ExceptionProctor<Obj, ALLOC> proctor(
                                                        index == SIZE ? &X : 0,
                                                        MoveUtil::move(mZ),
                                                        LINE);

                    iterator result =
                        mX.insert(POS == -1 ? X.begin()
                                : POS == 99 ? X.end()
                                :            (X.begin() + POS),
                                  bslmf::MovableRefUtil::move(*valptr));

                    proctor.release();

                    ASSERTV(LINE, SIZE, X.size(), SIZE + 1 == X.size());
                    ASSERTV(LINE, result == X.begin() + index);
                    ASSERTV(LINE, *result == VALUES[ELEMENT - 'A']);

                    TestValues exp(EXPECTED);
                    ASSERTV(LINE, 0 == verifyContainer(X, exp, SIZE + 1));
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            }
        }
    }
}
template <class TYPE, class ALLOC>
template <class CONTAINER>
void TestDriver3<TYPE, ALLOC>::testCase27Range(const CONTAINER&)
{
    // ------------------------------------------------------------------------
    // TESTING INSERTION
    //
    // We have the following concerns:
    //   1) That the resulting vector value is correct.
    //   2) That the initial range is correctly imported and then moved if the
    //      initial 'FWD_ITER' is an input iterator.
    //   3) That the resulting capacity is correctly set up if the initial
    //      'FWD_ITER' is a random-access iterator.
    //   4) That existing elements are moved without copy-construction if the
    //      bitwise-moveable trait is present.
    //   5) That insertion is exception neutral w.r.t. memory allocation.
    //   6) The internal memory management system is hooked up properly
    //      so that *all* internally allocated memory draws from a
    //      user-supplied allocator whenever one is specified.
    //
    // Plan:
    //   For insertion we will create objects of varying sizes with different
    //   'value' as argument.  Perform the above tests:
    //      - From the parameterized 'CONTAINER::const_iterator'.
    //      - Without exceptions, and compute the number of allocations.
    //      - In the presence of exceptions during memory allocations using
    //        a 'bslma::TestAllocator' and varying its *allocation* *limit*,
    //        but do not compute the number of allocations.
    //   and use basic accessors to verify
    //      - size
    //      - capacity
    //      - element value at each index position { 0 .. length - 1 }.
    //   In addition, the number of allocations should reflect proper internal
    //   memory management: the number of allocations should equal the sum of
    //      - NUM_ELEMENTS + (INIT_LENGTH - POS) if the type uses an allocator
    //        and is not bitwise-moveable,  0 otherwise
    //      - 1 if there is a change in capacity, 0 otherwise
    //      - 1 if the type uses an allocator and the value is an alias.
    //      -
    //   For concern 4, we test with a bitwise-moveable type that the only
    //   reallocations are for the new elements plus one if the vector
    //   undergoes a reallocation (capacity changes).
    //
    //   iterator insert(const_iterator pos, T&& val);
    // ------------------------------------------------------------------------

    if (verbose) printf("\nTesting '%s'.\n", NameOf<TYPE>().name());

    bslma::TestAllocator ta(veryVeryVeryVerbose);

    const TestValues VALUES;
    const int        NUM_VALUES = 5;  // TBD: fix this
    const TYPE&      DEFAULT_VALUE = VALUES['Z' - 'A'];

    const int TYPE_ALLOC    = bslma::UsesBslmaAllocator<TYPE>::value;

    const int INPUT_ITERATOR_TAG =
        bsl::is_same<std::input_iterator_tag,
                      typename bsl::iterator_traits<
                         typename CONTAINER::const_iterator>::iterator_category
                      >::value;

    static const struct {
        int d_lineNum;  // source line number
        int d_length;   // expected length
    } DATA[] = {
        //line  length
        //----  ------
        { L_,        0 },
        { L_,        1 },
        { L_,        2 },
        { L_,        3 },
        { L_,        4 },
        { L_,        5 },
        { L_,        6 },
        { L_,        7 },
        { L_,        8 },
        { L_,        9 },
        { L_,       11 },
        { L_,       12 },
        { L_,       14 },
        { L_,       15 },
        { L_,       16 },
        { L_,       17 }
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    static const struct {
        int         d_lineNum;  // source line number
        const char *d_spec;     // container spec
    } U_DATA[] = {
        //line  spec                   length
        //----  -------------------    ------
        { L_,   ""                  }, // 0
        { L_,   "A"                 }, // 1
        { L_,   "AB"                }, // 2
        { L_,   "ABC"               }, // 3
        { L_,   "ABCD"              }, // 4
        { L_,   "ABCDE"             }, // 5
        { L_,   "ABCDEAB"           }, // 7
        { L_,   "ABCDEABC"          }, // 8
        { L_,   "ABCDEABCD"         }, // 9
        { L_,   "ABCDEABCDEABCDE"   }, // 15
        { L_,   "ABCDEABCDEABCDEA"  }, // 16
        { L_,   "ABCDEABCDEABCDEAB" }  // 17
    };
    enum { NUM_U_DATA = sizeof U_DATA / sizeof *U_DATA };

    if (verbose) printf("\tUsing '%s::const_iterator'.\n",
                        NameOf<CONTAINER>().name());
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_CAP = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_CAP);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_CAP);
                    printf("using default value.\n");
                }

                for (int ti = 0; ti < NUM_U_DATA; ++ti) {
                    const int    LINE         = U_DATA[ti].d_lineNum;
                    const char  *SPEC         = U_DATA[ti].d_spec;
                    const int    NUM_ELEMENTS = static_cast<int>(strlen(SPEC));
                    const size_t LENGTH       = INIT_LENGTH + NUM_ELEMENTS;

                    for (size_t j = 0; j <= INIT_LENGTH; ++j) {
                        // Declare inside loop, as we are going to move out
                        // elements on each iteration.
                        Obj       mUA;    const Obj&       UA = gg(&mUA, SPEC);
                        CONTAINER mU(UA); const CONTAINER& U = mU;

                        const size_t POS = j;

                        Obj mX(&ta);    const Obj& X = mX;
                        mX.reserve(INIT_CAP);

                        size_t k;
                        for (k = 0; k != INIT_LENGTH; ++k) {
                            mX.emplace_back(VALUES[k % NUM_VALUES]);
                        }

                        const size_t CAP = 0 == INIT_CAP && 0 == LENGTH
                                         ? 0
                                         : LENGTH <= INIT_CAP
                                         ? -1
                                         : ImpUtil::computeNewCapacity(
                                                                 LENGTH,
                                                                 INIT_CAP,
                                                                 X.max_size());

                        if (veryVerbose) {
                            printf("\t\t\tInsert "); P_(NUM_ELEMENTS);
                            printf("at "); P_(POS);
                            printf("using "); P(SPEC);
                        }

                        const bsls::Types::Int64 BB = ta.numBlocksTotal();
                        const bsls::Types::Int64 B  = ta.numBlocksInUse();

                        if (veryVerbose) {
                            printf("\t\t\t\tBefore:"); P_(BB); P(B);
                        }

#if 1  // need to use move-iterators in the future, to test move-only types

                        mX.insert(X.begin() + POS, U.begin(), U.end());
#else
                        mX.insert(X.begin() + POS,
                                  std::make_move_iterator(mU.begin()),
                                  std::make_move_iterator(mU.end()));
#endif

                        const bsls::Types::Int64 AA = ta.numBlocksTotal();
                        const bsls::Types::Int64 A  = ta.numBlocksInUse();

                        if (veryVerbose) {
                            printf("\t\t\t\tAfter :"); P_(AA); P(A);
                            T_; T_; T_; T_; P_(X); P(X.capacity());
                        }

                        ASSERTV(INIT_LINE, LINE, i, j, LENGTH == X.size());
                        if ((size_t) -1 != CAP) {
                            ASSERTV(INIT_LINE, LINE, i, j,
                                    CAP == X.capacity());
                        }

                        for (k = 0; k < POS; ++k) {
                            ASSERTV(INIT_LINE, LINE, j, k,
                                    VALUES[k % NUM_VALUES] == X[k]);
                        }
                        for (size_t m = 0; k < POS + NUM_ELEMENTS; ++k, ++m) {
                            ASSERTV(INIT_LINE, LINE, j, k, m, U[m] == X[k]);
                        }
                        for (size_t m = POS; k < LENGTH; ++k, ++m) {
                            ASSERTV(INIT_LINE, LINE, j, k, m,
                                    VALUES[m % NUM_VALUES] == X[k]);
                        }

                        if (k_IS_MOVABLE && INPUT_ITERATOR_TAG) {
                            ASSERTV(NUM_ELEMENTS,  NUM_NUM_ALLOCS,
                                    NUM_ELEMENTS + ZERO < NUM_NUM_ALLOCS);
                            ASSERTV(X.capacity(),  NUM_NUM_ALLOCS,
                                    X.capacity() < NUM_NUM_ALLOCS);

                            const int REALLOC = X.capacity() > INIT_CAP
                                              ? NUM_ALLOCS[NUM_ELEMENTS] -
                                                NUM_ALLOCS[X.capacity()]
                                              : 0;
                            const bsls::Types::Int64 TYPE_ALLOCS =
                                              NUM_ELEMENTS &&
                                                   TYPE_ALLOC && !k_IS_MOVABLE
                                              ? (REALLOC ? INIT_LENGTH
                                                         : INIT_LENGTH - POS)
                                              : 0;
                            const bsls::Types::Int64 EXP_ALLOCS  =
                                                     REALLOC + TYPE_ALLOCS +
                                                     NUM_ELEMENTS * TYPE_ALLOC;

                            ASSERTV(INIT_LINE, INIT_LENGTH, INIT_CAP, j,
                                    BB + EXP_ALLOCS,   AA,
                                    BB + EXP_ALLOCS <= AA);
                            ASSERTV(INIT_LINE, INIT_LENGTH, INIT_CAP, j,
                                    B  + (REALLOC && 0 == INIT_CAP) +
                                              NUM_ELEMENTS * TYPE_ALLOC <=  A);
                        } else {
                            const int                REALLOC =
                                                       X.capacity() > INIT_CAP;
                            const bsls::Types::Int64 TYPE_ALLOCS =
                                              NUM_ELEMENTS &&
                                                   TYPE_ALLOC && !k_IS_MOVABLE
                                              ? (REALLOC ? INIT_LENGTH
                                                         : INIT_LENGTH - POS)
                                              : 0;
                            const bsls::Types::Int64 EXP_ALLOCS  =
                                                     REALLOC + TYPE_ALLOCS +
                                                     NUM_ELEMENTS * TYPE_ALLOC;

                            ASSERTV(INIT_LINE, INIT_LENGTH, INIT_CAP, j,
                                    BB + EXP_ALLOCS,   AA,
                                    BB + EXP_ALLOCS <= AA);
                            ASSERTV(INIT_LINE, INIT_LENGTH, INIT_CAP, j,
                                    B  + (REALLOC && 0 == INIT_CAP) +
                                              NUM_ELEMENTS * TYPE_ALLOC <=  A);
                        }
                    }
                }
                ASSERT(0 == ta.numMismatches());
                ASSERT(0 == ta.numBlocksInUse());
            }
        }
    }
    ASSERT(0 == ta.numMismatches());
    ASSERT(0 == ta.numBlocksInUse());

    if (verbose) printf("\tWith exceptions.\n");
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_CAP = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_CAP);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_CAP);
                    printf("using default value.\n");
                }

                for (int ti = 0; ti < NUM_U_DATA; ++ti) {
                    const int     LINE         = U_DATA[ti].d_lineNum;
                    const char   *SPEC         = U_DATA[ti].d_spec;
                    const size_t  NUM_ELEMENTS = strlen(SPEC);
                    const size_t  LENGTH       = INIT_LENGTH + NUM_ELEMENTS;

                    for (size_t j = 0; j <= INIT_LENGTH; ++j) {
                        // Declare inside loop, as we are going to move out
                        // elements on each iteration.
                        Obj       mUA;      const Obj& UA = gg(&mUA, SPEC);
                        CONTAINER mU(UA);
                        const CONTAINER& U  = mU;

                        const size_t POS = j;

                        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(ta) {
                            const bsls::Types::Int64 AL = ta.allocationLimit();
                            ta.setAllocationLimit(-1);

                            Obj        mX(INIT_LENGTH, DEFAULT_VALUE, &ta);
                            const Obj& X = mX;
                            mX.reserve(INIT_CAP);

                            const size_t CAP = 0 == INIT_CAP && 0 == LENGTH
                                             ? 0
                                             : LENGTH <= INIT_CAP
                                             ? -1
                                             : ImpUtil::computeNewCapacity(
                                                                 LENGTH,
                                                                 INIT_CAP,
                                                                 X.max_size());

                            ta.setAllocationLimit(AL);

#if 1  // need to use move-iterators in the future, to test move-only types
                            mX.insert(X.begin() + POS, U.begin(), U.end());
                                                         // test insertion here
#else
                            mX.insert(X.begin() + POS,
                                      std::make_move_iterator(mU.begin()),
                                      std::make_move_iterator(mU.end()));
                                                         // test insertion here
#endif

                            if (veryVerbose) {
                                T_; T_; T_; P_(X); P(X.capacity());
                            }

                            ASSERTV(INIT_LINE, LINE, i, j, LENGTH == X.size());
                            if ((size_t) -1 != CAP) {
                                ASSERTV(INIT_LINE, LINE, i, j,
                                        CAP == X.capacity());
                            }

                            size_t k;
                            for (k = 0; k < POS; ++k) {
                                ASSERTV(INIT_LINE, LINE, i, j, k,
                                        DEFAULT_VALUE == X[k]);
                            }
                            for (; k < POS + NUM_ELEMENTS; ++k) {
                                ASSERTV(INIT_LINE, LINE, i, j, k,
                                        U[k - POS] == X[k]);
                            }
                            for (; k < LENGTH; ++k) {
                                ASSERTV(INIT_LINE, LINE, i, j, k,
                                        DEFAULT_VALUE == X[k]);
                            }
                        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                    }
                }
                ASSERT(0 == ta.numMismatches());
                ASSERT(0 == ta.numBlocksInUse());
            }
        }
    }
    ASSERT(0 == ta.numMismatches());
    ASSERT(0 == ta.numBlocksInUse());
}


template <class TYPE, class ALLOC>
void TestDriver3<TYPE, ALLOC>::testCase25()
{
    // ------------------------------------------------------------------------
    // TESTING 'void push_back(T&&)'
    //
    // Concerns:
    //: 1 A new element is added to the end of the container and the order of
    //:   the container remains correct.
    //:
    //: 2 The newly inserted item is move-inserted.
    //:
    //: 3 The capacity is increased as expected.
    //:
    //: 4 The internal memory management system is hooked up properly so that
    //:   *all* internally allocated memory draws from a user-supplied
    //:   allocator whenever one is specified.
    //:
    //: 5 Insertion is exception neutral w.r.t. memory allocation.
    //
    // Plan:
    //: 1 For 'push_back' we will create objects of varying sizes and
    //:   capacities containing default values, and insert a 'value'.
    //:
    //:   1 Verify that the element was added to the end of the container.(C-1)
    //:
    //:   2 Verify that the moved-into state for the new element is MOVED.(C-2)
    //:
    //:   3 Compute the number of allocations and verify it is as expected.
    //:                                                                   (C-3)
    //:
    //:   4 Verify all allocations are from the object's allocator.       (C-4)
    //:
    //: 2 Repeat P-1 under the presence of exceptions.                    (C-5)
    //
    // Testing:
    //   void push_back(T&&);
    // ------------------------------------------------------------------------

    const TestValues VALUES;

    const int TYPE_COPY = !k_IS_MOVABLE &&
                           bsl::is_copy_constructible<TYPE>::value;

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<TYPE>::value ||
                           bsl::uses_allocator<TYPE, ALLOC>::value;

    if (verbose) printf(
         "\nTesting '%s' (TYPE_ALLOC = %d, TYPE_MOVE = %d, TYPE_COPY = %d).\n",
         NameOf<TYPE>().name(),
         TYPE_ALLOC,
         k_IS_MOVABLE,
         TYPE_COPY);

    static const struct {
        int         d_line;     // source line number
        const char *d_spec;     // specification string
        char        d_element;  // specification string
        const char *d_results;  // expected element values
    } DATA[] = {
        //line  spec                 element  results
        //----  -------------------  -------  -------------------
        { L_,   "",                  'A',     "A"                 },
        { L_,   "A",                 'A',     "AA"                },
        { L_,   "A",                 'B',     "AB"                },
        { L_,   "B",                 'A',     "BA"                },
        { L_,   "AB",                'A',     "ABA"               },
        { L_,   "BC",                'D',     "BCD"               },
        { L_,   "BCA",               'Z',     "BCAZ"              },
        { L_,   "CAB",               'C',     "CABC"              },
        { L_,   "CDAB",              'D',     "CDABD"             },
        { L_,   "DABC",              'Z',     "DABCZ"             },
        { L_,   "ABCDE",             'Z',     "ABCDEZ"            },
        { L_,   "EDCBA",             'E',     "EDCBAE"            },
        { L_,   "ABCDEA",            'E',     "ABCDEAE"           },
        { L_,   "ABCDEAB",           'Z',     "ABCDEABZ"          },
        { L_,   "BACDEABC",          'D',     "BACDEABCD"         },
        { L_,   "CBADEABCD",         'Z',     "CBADEABCDZ"        },
        { L_,   "CBADEABCDAB",       'B',     "CBADEABCDABB"      },
        { L_,   "CBADEABCDABC",      'Z',     "CBADEABCDABCZ"     },
        { L_,   "CBADEABCDABCDE",    'B',     "CBADEABCDABCDEB"   },
        { L_,   "CBADEABCDABCDEA",   'E',     "CBADEABCDABCDEAE"  },
        { L_,   "CBADEABCDABCDEAB",  'Z',     "CBADEABCDABCDEABZ" }
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    if (verbose) printf("\tTesting 'push_back' without exceptions.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE     = DATA[ti].d_line;
            const char *const SPEC     = DATA[ti].d_spec;
            const char        ELEMENT  = DATA[ti].d_element;
            const char *const EXPECTED = DATA[ti].d_results;
            const size_t      SIZE     = strlen(SPEC);
            for (char cfg = 'a'; cfg <= 'b'; ++cfg) {
                const char CONFIG = cfg;

                bslma::TestAllocator         da("default",
                                                veryVeryVeryVerbose);
                bslma::DefaultAllocatorGuard dag(&da);

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                ALLOC                xoa(&oa);
                Obj                  mX(xoa);
                const Obj&           X = gg(&mX, SPEC);

                bslma::TestAllocator  za("different", veryVeryVeryVerbose);
                bslma::TestAllocator *valAllocator = 0;
                switch (cfg) {
                  case 'a': {
                    // inserted value has same allocator
                    valAllocator = &oa;
                  } break;
                  case 'b': {
                    // inserted value has different allocator
                    valAllocator = &za;
                  } break;
                  default: {
                    ASSERTV(CONFIG, !"Bad allocator config.");
                  } break;
                }
                bslma::TestAllocator& sa = *valAllocator;
                ALLOC                 xsa(&sa);

                if (veryVerbose) { T_ P_(LINE) P_(CONFIG) P(X) }

                // Verify any attribute allocators are installed properly.

                ASSERTV(LINE, CONFIG, &oa == X.get_allocator());

                ASSERTV(LINE, CONFIG, SIZE, X.size(), SIZE == X.size());

                const bsls::Types::Int64 BB = oa.numBlocksTotal();
                const bsls::Types::Int64 B  = oa.numBlocksInUse();

                bsls::ObjectBuffer<ValueType>  buffer;
                ValueType                     *valptr = buffer.address();
                TstFacility::emplace(valptr, ELEMENT, xsa);

                ASSERTV(SIZE, numNotMovedInto(X), SIZE == numNotMovedInto(X));

                mX.push_back(MoveUtil::move(*valptr));

                MoveState::Enum mState = TstFacility::getMovedFromState(
                                                                      *valptr);
                bslma::DestructionUtil::destroy(valptr);

                if (veryVerbose) { T_ P_(LINE) P_(CONFIG) P_(ELEMENT) P(X) }

                {
                    const MoveState::Enum exp = !s_typeIsMoveEnabled
                                              ? MoveState::e_UNKNOWN
                                              : k_IS_WELL_BEHAVED && &oa != &sa
                                              ? MoveState::e_NOT_MOVED
                                              : MoveState::e_MOVED;

                    ASSERTV(exp, mState, exp == mState);
                }

                const bsls::Types::Int64 AA = oa.numBlocksTotal();
                const bsls::Types::Int64 A  = oa.numBlocksInUse();

                ASSERTV(LINE, CONFIG, SIZE, X.size(), SIZE + 1 == X.size());

                TestValues exp(EXPECTED);
                ASSERTV(LINE, CONFIG, 0 == verifyContainer(X, exp, SIZE + 1));

                // Vector increases capacity only if the current length is zero
                // or a power of 2.  In addition, when the type allocates, an
                // extra allocation is used for the new element.  When the type
                // is neither bitwise nor nothrow movable and a move is not
                // performed, 'size()' allocations are used during the move,
                // but an equal number of elements are destroyed/deallocated,
                // thus the number of blocks in use is unchanged.

                if (expectToAllocate(SIZE))  {
                    ASSERTV(SIZE, numMovedInto(X),
                          !k_IS_MOVABLE || (k_IS_WELL_BEHAVED && &oa != &sa) ||
                                                (SIZE + 1 == numMovedInto(X)));
                    const bsls::Types::Int64 EXP = BB
                          +  1                                  // realloc
                          +  TYPE_ALLOC                         // new element
                          +  TYPE_ALLOC * (SIZE * TYPE_COPY)    // SIZE MOVES
                          +  TYPE_ALLOC * (&sa == &oa    // temporary not moved
                                              && MoveState::e_MOVED != mState);
                    ASSERTV(LINE, CONFIG, BB, AA, EXP, AA == EXP);
                }
                else {
                    ASSERTV(numNotMovedInto(X, 0, SIZE),
                            SIZE == numNotMovedInto(X, 0, SIZE));
                    const bsls::Types::Int64 EXP = BB
                          +  TYPE_ALLOC                         // new element
                          +  TYPE_ALLOC * (&sa == &oa   // temporary not moved
                                             && MoveState::e_MOVED != mState);
                    ASSERTV(LINE, CONFIG, BB, AA, EXP, AA == EXP);
                }
                ASSERTV(LINE, CONFIG, SIZE, B, A,
                        B + (SIZE == 0) + TYPE_ALLOC == A);
            }
        }
    }

    if (verbose) printf("\tTesting 'push_back' with injected exceptions.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE     = DATA[ti].d_line;
            const char *const SPEC     = DATA[ti].d_spec;
            const char        ELEMENT  = DATA[ti].d_element;
            const char *const EXPECTED = DATA[ti].d_results;
            const size_t      SIZE     = strlen(SPEC);
            for (char cfg = 'a'; cfg <= 'b'; ++cfg) {
                const char CONFIG = cfg;

                bslma::TestAllocator         da("default",
                                                veryVeryVeryVerbose);
                bslma::DefaultAllocatorGuard dag(&da);

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                ALLOC                xoa(&oa);
                Obj                  mX(xoa);
                const Obj&           X = gg(&mX, SPEC);

                bslma::TestAllocator za("different", veryVeryVeryVerbose);
                bslma::TestAllocator *valAllocator = 0;
                switch (cfg) {
                  case 'a': {
                    // inserted value has same allocator
                    valAllocator = &oa;
                  } break;
                  case 'b': {
                    // inserted value has different allocator
                    valAllocator = &za;
                  } break;
                  default: {
                    ASSERTV(CONFIG, !"Bad allocator config.");
                  } break;
                }
                bslma::TestAllocator& sa = *valAllocator;
                ALLOC                 xsa(&sa);

                ASSERTV(LINE, SIZE, X.size(), SIZE == X.size());

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
                ALLOC                xscratch(&scratch);
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    Obj mZ(xscratch);   const Obj& Z = gg(&mZ, SPEC);
                    ASSERTV(Z, X, Z == X);
                    // This method provides the strong exception guarantee.
                    ExceptionProctor<Obj, ALLOC> proctor(&X,
                                                         MoveUtil::move(mZ),
                                                         L_);

                    bsls::ObjectBuffer<ValueType>  buffer;
                    ValueType                     *valptr = buffer.address();
                    TstFacility::emplace(valptr, ELEMENT, xsa);
                    bslma::DestructorGuard<ValueType> guard(valptr);

                    mX.push_back(MoveUtil::move(*valptr));

                    ASSERTV(LINE, SIZE, X.size(), SIZE + 1 == X.size());

                    TestValues exp(EXPECTED);
                    ASSERTV(LINE, 0 == verifyContainer(X, exp, SIZE + 1));

                    proctor.release();
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            }
        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver3<TYPE, ALLOC>::testCase24_move_assignment_noexcept()
{
    // ------------------------------------------------------------------------
    // TESTING MOVE-ASSIGNMENT OPERATOR: NOEXCEPT SPECIFICATION
    //
    // Concerns:
    //: 1 If either 'allocator_traits<Allocator>::
    //:   propagate_on_container_move_assignment::value' or
    //:   'allocator_traits<Allocator>::is_always_equal::value' is
    //:   true, the move assignment operator is 'noexcept(true)'.
    //
    // Plan:
    //: 1 Get ORed value of the both traits.
    //:
    //: 2 Compare the value with noexcept specification of the move assignment
    //:   operator.
    //
    // Testing:
    //   vector& operator=(bslmf::MovableRef<vector> rhs);
    // ------------------------------------------------------------------------

#if BSLS_KEYWORD_NOEXCEPT_AVAILABLE
    bsl::vector<TYPE, ALLOC> a, b;

    const bool isNoexcept =
        AllocatorTraits::propagate_on_container_move_assignment::value ||
        AllocatorTraits::is_always_equal::value;
    ASSERT(isNoexcept ==
           BSLS_KEYWORD_NOEXCEPT_OPERATOR(a = bslmf::MovableRefUtil::move(b)));
#endif
}

template <class TYPE, class ALLOC>
void TestDriver3<TYPE, ALLOC>::testCase24_dispatch()
{
    // ------------------------------------------------------------------------
    // TESTING MOVE-ASSIGNMENT OPERATOR
    //
    // Concerns:
    //  TBD: the test does not yet cover the case where allocator propagation
    //       is enabled for move assignment (hard-coded to 'false') -- i.e.,
    //       parts of C-5..6 are currently not addressed.
    //
    //: 1 The signature and return type are standard.
    //:
    //: 2 The reference returned is to the target object (i.e., '*this').
    //:
    //: 3 The move assignment operator can change the value of a modifiable
    //:   target object to that of any source object.
    //:
    //: 4 The object has its internal memory management system hooked up
    //:   properly so that *all* internally allocated memory draws from a
    //:   user-supplied allocator whenever one is specified.
    //:
    //: 5 If allocator propagation is not enabled for move-assignment, the
    //:   allocator address held by the target object is unchanged; otherwise,
    //:   the allocator address held by the target object is changed to that of
    //:   the source.
    //:
    //: 6 If allocator propagation is enabled for move-assignment, any memory
    //:   allocation from the original target allocator is released after the
    //:   operation has completed.
    //:
    //: 7 All elements in the target object are either move-assigned to or
    //:   destroyed.
    //:
    //: 8 The source object is left in a valid but unspecified state; the
    //:   allocator address held by the original object is unchanged.
    //:
    //: 9 Subsequent changes to or destruction of the original object have no
    //:   effect on the move-constructed object and vice-versa.
    //:
    //:10 Assigning a source object having the default-constructed value
    //:   allocates no memory; assigning a value to a target object in the
    //:   default state does not allocate or deallocate any memory if the
    //:   allocators of the source and target object are the same.
    //:
    //:11 Every object releases any allocated memory at destruction.
    //:
    //:12 Any memory allocation is exception neutral.
    //:
    //:13 Assigning an object to itself behaves as expected (alias-safety).
    //:
    //:14 If either 'allocator_traits<Allocator>::is_always_equal::value' or
    //:   'allocator_traits<Allocator>::propagate_on_container_move_assignment
    //:   ::value' is true, the move assignment operator is 'noexcept(true)'.
    //
    // Plan:
    //
    //: 1 Use the address of 'operator=' to initialize a member-function
    //:   pointer having the appropriate signature and return type for the
    //:   copy-assignment operator defined in this component.             (C-1)
    //:
    //: 2 Iterate over a set of object values with substantial and varied
    //:   differences, ordered by increasing length, and create for each a
    //:   control object representing the source of the assignment, with its
    //:   own scratch allocator.
    //:
    //: 3 Iterate again over the same set of object values and create a
    //:   object representing the target of the assignment, with its own unique
    //:   object allocator.
    //:
    //: 4 In a loop consisting of two iterations, create a source object (a
    //:   copy of the control object in P-1) with 1) a different allocator than
    //:   that of target and 2) the same allocator as that of the target,
    //:
    //: 5 Call the move-assignment operator in the presence of exceptions
    //:   during memory allocations (using a 'bslma::Allocator' and varying
    //:   its allocation limit) and verify the following:                (C-12)
    //:
    //:   1 The address of the return value is the same as that of the target
    //:     object.                                                       (C-2)
    //:
    //:   2 The object being assigned to has the same value as that of the
    //:     source object before assignment (i.e., the control object).   (C-3)
    //:
    //:   3 CONTAINER SPECIFIC NOTE: none
    //:
    //:   4 If the source and target objects use the same allocator, ensure
    //:     that there is no net increase in memory use from the common
    //:     allocator.  Also consider the following cases:
    //:
    //:     1 If the source object is empty, confirm that there are no bytes
    //:       currently in use from the common allocator.                (C-10)
    //:
    //:     2 If the target object is empty, confirm that there was no memory
    //:       change in memory usage.                                    (C-10)
    //:
    //:   5 If the source and target objects use different allocators, ensure
    //:     that each element in the source object is move-inserted into the
    //:     target object.                                                (C-7)
    //:
    //:   6 Ensure that the source, target, and control object continue to have
    //:     the correct allocator and that all memory allocations come from the
    //:     appropriate allocator.                                        (C-4)
    //:
    //:   7 Manipulate the source object (after assignment) to ensure that it
    //:     is in a valid state, destroy it, and then manipulate the target
    //:     object to ensure that it is in a valid state.                 (C-8)
    //:
    //:   8 Verify all memory is released when the source and target objects
    //:     are destroyed.                                               (C-11)
    //:
    //: 6 Use a test allocator installed as the default allocator to verify
    //:   that no memory is ever allocated from the default allocator.
    //:
    //: 7 To address concern 14, pass allocators with all combinations of
    //:   'is_always_equal' and 'propagate_on_container_move_assignment'
    //:   values.
    //
    // Testing:
    //   vector& operator=(bslmf::MovableRef<vector> rhs);
    // ------------------------------------------------------------------------

    // Since this function is called with a variety of template arguments, it
    // is necessary to infer some things about our template arguments in order
    // to print a meaningful banner.

    const bool isPropagate =
                AllocatorTraits::propagate_on_container_move_assignment::value;
    const bool otherTraitsSet =
                AllocatorTraits::propagate_on_container_copy_assignment::value;

    // We can print the banner now:

    if (verbose) printf(
                   "%sTESTING MOVE ASSIGN '%s' OTHER:%c PROP:%c"" ALLOC: %s\n",
                   veryVerbose ? "\n" : "",
                   NameOf<TYPE>().name(), otherTraitsSet ? 'T' : 'F',
                   isPropagate ? 'T' : 'F',
                   allocCategoryAsStr());

    // Assign the address of the function to a variable.
    {
        typedef Obj& (Obj::*OperatorMAg)(bslmf::MovableRef<Obj>);

        using namespace bsl;

        OperatorMAg op = &Obj::operator=;
        (void) op;  // quash potential compiler warning
    }

    // Create a test allocator and install it as the default.

    bslma::TestAllocator         da("default",   veryVeryVeryVerbose);
    bslma::TestAllocator         oa("object",    veryVeryVeryVerbose);
    bslma::TestAllocator         za("other",     veryVeryVeryVerbose);
    bslma::TestAllocator         sa("scratch",   veryVeryVeryVerbose);
    bslma::TestAllocator         fa("footprint", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    ALLOC xda(&da);
    ALLOC xoa(&oa);
    ALLOC xza(&za);
    ALLOC xsa(&sa);

    // Check remaining properties of allocator to make sure they all match
    // 'otherTraitsSet'.

    BSLMF_ASSERT(otherTraitsSet ==
                          AllocatorTraits::propagate_on_container_swap::value);
    ASSERT((otherTraitsSet ? xsa : xda) ==
                  AllocatorTraits::select_on_container_copy_construction(xsa));

    // Use a table of distinct object values and expected memory usage.

    enum { NUM_DATA                         = DEFAULT_NUM_DATA };
    const  DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    // Testing function signature
    {
        using namespace bsl;

        typedef Obj& (Obj::*OperatorMoveAssign)(bslmf::MovableRef<Obj>);
        OperatorMoveAssign oma = &Obj::operator=;    (void) oma;
    }

    Obj        szc(xsa);
    const Obj& SZC = szc;
    primaryManipulator(&szc, 'Z');
    const TYPE& zValue = SZC.front();

    // Create first object.
    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const char *const SPEC1   = DATA[ti].d_spec;
        const size_t      LENGTH1 = strlen(SPEC1);

        Obj  mZZ(xsa);  const Obj&  ZZ = gg(&mZZ,  SPEC1);

        if (veryVerbose) { T_ P(ZZ) }

        // Ensure the first row of the table contains the default-constructed
        // value.
        if (0 == ti) {
            ASSERTV(SPEC1, Obj(xsa), ZZ, Obj(xsa) == ZZ);
        }

        // Create second object.
        for (int tj = 0; tj < NUM_DATA; ++tj) {
            const char *const SPEC2   = DATA[tj].d_spec;

            Obj mXX(xsa);    const Obj& XX = gg(&mXX, SPEC2);

            for (char cfg = 'a'; cfg <= 'b'; ++cfg) {
                const char CONFIG = cfg;  // how we specify the allocator

                Obj        *objPtr = new (fa) Obj(xoa);
                Obj&        mX     = *objPtr;
                const Obj&  X      = gg(&mX, SPEC2);

                Obj                   *srcPtr = 0;
                ALLOC&                 xra    = 'a' == CONFIG ? xza : xoa;
                bslma::TestAllocator&  ra     = 'a' == CONFIG ? za  : oa;

                const bool empty = 0 == ZZ.size();

                typename Obj::const_pointer pointers[2] = {};

                Int64 oaBase;
                Int64 zaBase;

                int numPasses = 0;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    ++numPasses;

                    Int64 al = oa.allocationLimit();
                    oa.setAllocationLimit(-1);

                    srcPtr = new (fa) Obj(xra);
                    bslma::RawDeleterProctor<Obj, bslma::Allocator> proctor(
                                                                        srcPtr,
                                                                        &fa);
                    gg(srcPtr, SPEC1);

                    Obj& mZ = *srcPtr;  const Obj& Z = mZ;
                    ASSERT(ZZ == Z);

                    // TBD: add exception guard for mX here

                    ASSERTV(SPEC1, SPEC2, Z, X, (Z == X) == (ti == tj));

                    storeFirstNElemAddr(pointers,
                                        Z,
                                        sizeof pointers / sizeof *pointers);

                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    ASSERT(XX == X);

                    oaBase = oa.numAllocations();
                    zaBase = za.numAllocations();

                    oa.setAllocationLimit(al);

                    Obj *mR = &(mX = MoveUtil::move(mZ));
                    ASSERTV(SPEC1, SPEC2, mR, &mX, mR == &mX);

                    // Verify the value of the object.
                    ASSERTV(SPEC1, SPEC2,  X,  ZZ,  X ==  ZZ);

                    proctor.release();
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                Obj& mZ = *srcPtr;  const Obj& Z = mZ;

                ASSERTV(SPEC1, SPEC2, &xra == &xoa, X, LENGTH1 == X.size());

                // CONTAINER-SPECIFIC NOTE: For 'deque', if the allocators
                // differ, the source object is left with the same number of
                // elements but with each element in the "moved-from" state
                // (whatever that is); otherwise, the source object has the
                // same number of elements as the destination object had (and
                // vice versa).

                ASSERTV(SPEC1, SPEC2, za.numAllocations(), zaBase,
                        za.numAllocations() == zaBase);

                if (&xra == &xoa || isPropagate) {

                    // same allocator

                    // 1. no allocations from the (common) object allocator
                    ASSERTV(SPEC1, SPEC2, oa.numAllocations() == oaBase);

                    // 2. unchanged address(es) of contained element(s)
                    ASSERT(0 == checkFirstNElemAddr(
                                          pointers,
                                          X,
                                          sizeof pointers / sizeof *pointers));

                    // 3. source is empty
                    ASSERTV(SPEC1, SPEC2, &xra == &xoa, Z, Z.empty());
                }
                else {
                    // 1. each element in original move-inserted
                    if (!k_IS_WELL_BEHAVED) {
                        ASSERTV(SPEC1, SPEC2, X.end() ==
                                  TstMoveUtil::findFirstNotMovedInto(X.begin(),
                                                                     X.end()));
                    }

                    // 2. additional memory checks
                    ASSERTV(SPEC1, SPEC2, &xra == &xoa,
                            empty || oaBase < oa.numAllocations());

                    // 3. Size of Z is unchanged.
                    ASSERTV(SPEC1, SPEC2, &xra == &xoa, Z,
                            LENGTH1 == Z.size());
                }

                // Verify that 'X', 'Z', and 'ZZ' have correct allocator.
                ASSERTV(SPEC1, SPEC2, xsa == ZZ.get_allocator());
                ASSERTV(SPEC1, SPEC2,
                        (isPropagate ? xra : xoa) == X.get_allocator());
                ASSERTV(SPEC1, SPEC2, xra ==  Z.get_allocator());

                // Manipulate source object 'Z' to ensure it is in a valid
                // state and is independent of 'X'.

                const size_t zAfterSize = Z.size();

                primaryManipulator(&mZ, 'Z');
                ASSERTV(SPEC1, SPEC2, Z, zAfterSize + 1 == Z.size());
                ASSERTV(SPEC1, SPEC2, zValue == Z.back());

                ASSERTV(SPEC1, SPEC2, Z, ZZ, Z != ZZ);
                ASSERTV(SPEC1, SPEC2, X, ZZ, X == ZZ);

                fa.deleteObject(srcPtr);

                ASSERTV(SPEC1, SPEC2, ra.numBlocksInUse(),
                        empty || ((&xra == &xoa || isPropagate) ==
                                                   (0 < ra.numBlocksInUse())));

                // Verify subsequent manipulation of target object 'X'.

                primaryManipulator(&mX, 'Z');

                ASSERTV(SPEC1, SPEC2, LENGTH1 + 1, X.size(),
                        LENGTH1 + 1 == X.size());
                ASSERTV(SPEC1, SPEC2, zValue == X.back());
                ASSERTV(SPEC1, SPEC2, X, ZZ, X != ZZ);

                fa.deleteObject(objPtr);

                ASSERTV(SPEC1, SPEC2, oa.numBlocksInUse(),
                        0 == oa.numBlocksInUse());
                ASSERTV(SPEC1, SPEC2, za.numBlocksInUse(),
                        0 == za.numBlocksInUse());
            }
        }

        // self-assignment

        {
            Obj mZ(xoa);   const Obj& Z  = gg(&mZ,  SPEC1);

            ASSERTV(SPEC1, ZZ, Z, ZZ == Z);

            bslma::TestAllocatorMonitor oam(&oa);
            bslma::TestAllocatorMonitor sam(&sa);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                Obj *mR = &(mZ = MoveUtil::move(mZ));
                ASSERTV(SPEC1, ZZ,  Z, ZZ ==  Z);
                ASSERTV(SPEC1, mR, &Z, mR == &Z);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERTV(SPEC1, xoa == Z.get_allocator());

            ASSERTV(SPEC1, sam.isTotalSame());
            ASSERTV(SPEC1, oam.isTotalSame());
        }

        // Verify all object memory is released on destruction.

        ASSERTV(SPEC1, oa.numBlocksInUse(), 0 == oa.numBlocksInUse());
    }

    ASSERTV(e_STATEFUL == s_allocCategory || 0 == da.numBlocksTotal());

    // Verify move assignment noexcept specifications for the basic template
    // and the partial specialization of 'vector' for pointer types.

    TestDriver3<TYPE , ALLOC>::testCase24_move_assignment_noexcept();
    TestDriver3<TYPE*, ALLOC>::testCase24_move_assignment_noexcept();
}

template <class TYPE>
void MetaTestDriver3<TYPE>::testCase24()
{
    typedef bsl::allocator<TYPE>            BAP;
    typedef bsltf::StdAllocatorAdaptor<BAP> SAA;

    // The low-order bit of the identifier specifies whether the fourth boolean
    // argument of the stateful allocator, which indicates propagate on move
    // assign, is set.

    typedef bsltf::StdStatefulAllocator<TYPE, false, false, false, false> A00;
    typedef bsltf::StdStatefulAllocator<TYPE, false, false, false, true>  A01;
    typedef bsltf::StdStatefulAllocator<TYPE, true,  true,  true,  false> A10;
    typedef bsltf::StdStatefulAllocator<TYPE, true,  true,  true,  true>  A11;

    if (verbose) printf("\n");

    TestDriver3<TYPE, BAP>::testCase24_dispatch();

    TestDriver3<TYPE, SAA>::testCase24_dispatch();

    TestDriver3<TYPE, A00>::testCase24_dispatch();
    TestDriver3<TYPE, A01>::testCase24_dispatch();
    TestDriver3<TYPE, A10>::testCase24_dispatch();
    TestDriver3<TYPE, A11>::testCase24_dispatch();

    // is_always_equal == true &&
    // propagate_on_container_move_assignment == true
    TestDriver3<TYPE , StatelessAllocator<TYPE , false, true> >::
        testCase24_move_assignment_noexcept();
    TestDriver3<TYPE*, StatelessAllocator<TYPE*, false, true> >::
        testCase24_move_assignment_noexcept();

    // is_always_equal == true &&
    // propagate_on_container_move_assignment == false
    TestDriver3<TYPE , StatelessAllocator<TYPE > >::
        testCase24_move_assignment_noexcept();
    TestDriver3<TYPE*, StatelessAllocator<TYPE*> >::
        testCase24_move_assignment_noexcept();
}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
struct TestDeductionGuides {
    // This struct provides a namespace for functions testing deduction guides.
    // The tests are compile-time only; it is not necessary that these routines
    // be called at run-time.  Note that the following constructors do not have
    // associated deduction guides because the template parameters for
    // 'bsl::vector' cannot be deduced from the constructor parameters.
    //..
    // vector()
    // vector(ALLOC)
    // vector(size_t, ALLOC)
    //..

#define ASSERT_SAME_TYPE(...) \
 static_assert((bsl::is_same<__VA_ARGS__>::value), "Types differ unexpectedly")

    static void SimpleConstructors ()
        // Test that constructing a 'bsl::vector' from various combinations of
        // arguments deduces the correct type.
        //..
        // vector(const vector&  v)        -> decltype(v)
        // vector(const vector&  v, ALLOC) -> decltype(v)
        // vector(      vector&& v)        -> decltype(v)
        // vector(      vector&& v, ALLOC) -> decltype(v)
        // vector(size_type, VALUE_TYPE)        -> vector<VALUE_TYPE>
        // vector(size_type, VALUE_TYPE, ALLOC) -> vector<VALUE_TYPE, ALLOC>
        // vector(iter, iter)        -> vector<iter::VALUE_TYPE>
        // vector(iter, iter, ALLOC) -> vector<iter::VALUE_TYPE, ALLOC>
        // vector(initializer_list<T>)        -> vector<T>
        // vector(initializer_list<T>, ALLOC) -> vector<T>
        //..
    {
        bslma::Allocator     *a1 = nullptr;
        bslma::TestAllocator *a2 = nullptr;

        bsl::vector<int> v1;
        bsl::vector      v1a(v1);
        ASSERT_SAME_TYPE(decltype(v1a), bsl::vector<int>);

        typedef float T2;
        bsl::vector<T2> v2;
        bsl::vector     v2a(v2, bsl::allocator<T2>());
        bsl::vector     v2b(v2, a1);
        bsl::vector     v2c(v2, a2);
        bsl::vector     v2d(v2, bsl::allocator<int>());
        ASSERT_SAME_TYPE(decltype(v2a), bsl::vector<T2, bsl::allocator<T2>>);
        ASSERT_SAME_TYPE(decltype(v2b), bsl::vector<T2, bsl::allocator<T2>>);
        ASSERT_SAME_TYPE(decltype(v2c), bsl::vector<T2, bsl::allocator<T2>>);
        ASSERT_SAME_TYPE(decltype(v2d), bsl::vector<T2, bsl::allocator<T2>>);

        bsl::vector<short> v3;
        bsl::vector        v3a(std::move(v3));
        ASSERT_SAME_TYPE(decltype(v3a), bsl::vector<short>);

        typedef long double T4;
        bsl::vector<T4> v4;
        bsl::vector     v4a(std::move(v4), bsl::allocator<T4>());
        bsl::vector     v4b(std::move(v4), a1);
        bsl::vector     v4c(std::move(v4), a2);
        bsl::vector     v4d(std::move(v4), bsl::allocator<int>());
        ASSERT_SAME_TYPE(decltype(v4a), bsl::vector<T4, bsl::allocator<T4>>);
        ASSERT_SAME_TYPE(decltype(v4b), bsl::vector<T4, bsl::allocator<T4>>);
        ASSERT_SAME_TYPE(decltype(v4c), bsl::vector<T4, bsl::allocator<T4>>);
        ASSERT_SAME_TYPE(decltype(v4d), bsl::vector<T4, bsl::allocator<T4>>);

    // Turn off complaints about passing allocators in non-allocator positions
    // BDE_VERIFY pragma: push
    // BDE_VERIFY pragma: -AM01
        bsl::vector v5a(42, 3L);
        bsl::vector v5b(42, bsl::allocator<long>{});
        bsl::vector v5c(42, a1); // Deduce a vector of 'bslma::Allocator *'
        bsl::vector v5d(42, std::allocator<long>{});
        ASSERT_SAME_TYPE(decltype(v5a), bsl::vector<long>);
        ASSERT_SAME_TYPE(decltype(v5b), bsl::vector<bsl::allocator<long>>);
        ASSERT_SAME_TYPE(decltype(v5c), bsl::vector<bslma::Allocator *>);
        ASSERT_SAME_TYPE(decltype(v5d), bsl::vector<std::allocator<long>>);
    // BDE_VERIFY pragma: pop

        typedef double T6;
        bsl::vector v6a(42, 3.0, bsl::allocator<T6>());
        bsl::vector v6b(42, 3.0, a1);
        bsl::vector v6c(42, 3.0, a2);
        bsl::vector v6d(42, 3.0, std::allocator<T6>());
        ASSERT_SAME_TYPE(decltype(v6a), bsl::vector<T6, bsl::allocator<T6>>);
        ASSERT_SAME_TYPE(decltype(v6b), bsl::vector<T6, bsl::allocator<T6>>);
        ASSERT_SAME_TYPE(decltype(v6c), bsl::vector<T6, bsl::allocator<T6>>);
        ASSERT_SAME_TYPE(decltype(v6d), bsl::vector<T6, std::allocator<T6>>);

        typedef char T7;
        T7                        *p7b = nullptr;
        T7                        *p7e = nullptr;
        bsl::vector<T7>::iterator  i7b = nullptr;
        bsl::vector<T7>::iterator  i7e = nullptr;
        bsl::vector                v7a(p7b, p7e);
        bsl::vector                v7b(i7b, i7e);
        ASSERT_SAME_TYPE(decltype(v7a), bsl::vector<T7>);
        ASSERT_SAME_TYPE(decltype(v7b), bsl::vector<T7>);

        typedef unsigned short T8;
        T8                        *p8b = nullptr;
        T8                        *p8e = nullptr;
        bsl::vector<T8>::iterator  i8b = nullptr;
        bsl::vector<T8>::iterator  i8e = nullptr;

        bsl::vector v8a(p8b, p8e, bsl::allocator<T8>());
        bsl::vector v8b(p8b, p8e, a1);
        bsl::vector v8c(p8b, p8e, a2);
        bsl::vector v8d(p8b, p8e, std::allocator<T8>());
        bsl::vector v8e(i8b, i8e, bsl::allocator<T8>());
        bsl::vector v8f(i8b, i8e, a1);
        bsl::vector v8g(i8b, i8e, a2);
        bsl::vector v8h(i8b, i8e, std::allocator<T8>());
        ASSERT_SAME_TYPE(decltype(v8a), bsl::vector<T8, bsl::allocator<T8>>);
        ASSERT_SAME_TYPE(decltype(v8b), bsl::vector<T8, bsl::allocator<T8>>);
        ASSERT_SAME_TYPE(decltype(v8c), bsl::vector<T8, bsl::allocator<T8>>);
        ASSERT_SAME_TYPE(decltype(v8d), bsl::vector<T8, std::allocator<T8>>);
        ASSERT_SAME_TYPE(decltype(v8e), bsl::vector<T8, bsl::allocator<T8>>);
        ASSERT_SAME_TYPE(decltype(v8f), bsl::vector<T8, bsl::allocator<T8>>);
        ASSERT_SAME_TYPE(decltype(v8g), bsl::vector<T8, bsl::allocator<T8>>);
        ASSERT_SAME_TYPE(decltype(v8h), bsl::vector<T8, std::allocator<T8>>);

        bsl::vector v9({1LL, 2LL, 3LL, 4LL, 5LL});
        ASSERT_SAME_TYPE(decltype(v9), bsl::vector<long long>);

        typedef long long T10;
        std::initializer_list<T10> il = {1LL, 2LL, 3LL, 4LL, 5LL};
        bsl::vector                v10a(il, bsl::allocator<T10>{});
        bsl::vector                v10b(il, a1);
        bsl::vector                v10c(il, a2);
        bsl::vector                v10d(il, std::allocator<T10>{});
        ASSERT_SAME_TYPE(decltype(v10a), bsl::vector<T10,
                                                         bsl::allocator<T10>>);
        ASSERT_SAME_TYPE(decltype(v10b), bsl::vector<T10,
                                                         bsl::allocator<T10>>);
        ASSERT_SAME_TYPE(decltype(v10c), bsl::vector<T10,
                                                         bsl::allocator<T10>>);
        ASSERT_SAME_TYPE(decltype(v10d), bsl::vector<T10,
                                                         std::allocator<T10>>);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // Compile-fail tests
// #define BSLSTL_VECTOR_COMPILE_FAIL_POINTER_IS_NOT_A_SIZE
#if defined BSLSTL_VECTOR_COMPILE_FAIL_POINTER_IS_NOT_A_SIZE
        bsl::Vector_Util *lnp = nullptr; // pointer to random class type
        bsl::vector v99(lnp, 3.0, a1);
        // This should fail to compile (pointer is not a size)
#endif
    }

#undef ASSERT_SAME_TYPE
};
#endif  // BSLS_COMPILERFEATURES_SUPPORT_CTAD

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int            test = argc > 1 ? atoi(argv[1]) : 0;
                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    (void) veryVeryVeryVerbose; // Suppressing the "unused variable" warning

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&defaultAllocator);

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    // Test integrity of DATA

    {
        const size_t NUM_DATA                  = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        bool foundMax = false;
        for (unsigned ii = 0; ii < NUM_DATA; ++ii) {
            size_t len = strlen(DATA[ii].d_spec);
            ASSERT(len <= DEFAULT_MAX_LENGTH);
            foundMax |= DEFAULT_MAX_LENGTH == len;

            for (unsigned jj = 0; jj < NUM_DATA; ++jj) {
                ASSERT(ii == jj || strcmp(DATA[ii].d_spec, DATA[jj].d_spec));
            }
        }
        ASSERT(foundMax);
    }

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 43: {
        if (verbose) printf(
                    "\nUSAGE EXAMPLE TEST CASE IS IN 'bslstl_vector.t.cpp'"
                    "\n===================================================\n");
      } break;
      case 42: {
        // --------------------------------------------------------------------
        // CONCERN: Default constructor is called for default-inserted elems
        //
        // Concerns:
        //: 1 Default constructor must be called for each new default-inserted
        //:   element.
        //
        // Plan:
        //: 1 Every element gets a unique int value in the default constructor.
        //:   All default-constructed elements must have different values.  If
        //:   two elements have the same value, one is a copy of another.
        //
        // Testing:
        //   CONCERN: Default constructor is called for default-inserted elems
        // --------------------------------------------------------------------

        if (verbose) printf(
        "\nCONCERN: Default constructor is called for default-inserted elems"
        "\n================================================================="
        "\n");

        bsl::vector<UniqueInt> mX(4);
        ASSERT(mX.size() == 4);
        // No duplicates
        ASSERT(mX[0] != mX[1] && mX[0] != mX[2] && mX[0] != mX[3]);
        ASSERT(                  mX[1] != mX[2] && mX[1] != mX[3]);
        ASSERT(                                    mX[2] != mX[3]);

        mX.resize(8);
        ASSERT(mX.size() == 8);
        ASSERT(mX[4] != mX[5] && mX[4] != mX[6] && mX[4] != mX[7]);
        ASSERT(                  mX[5] != mX[6] && mX[5] != mX[7]);
        ASSERT(                                    mX[6] != mX[7]);
      } break;
      case 41: {
        // --------------------------------------------------------------------
        // TESTING INCOMPLETE TYPE SUPPORT
        //
        // Concerns:
        //: 1 All public methods of 'bsl::vector' parameterized with incomplete
        //:   type are successfully compiled.
        //
        // Plan:
        //: 1 Invoke each public method of the special test type
        //:   'IncompleteTypeSupportChecker'.
        //
        // Testing:
        //   INCOMPLETE TYPE SUPPORT
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING INCOMPLETE TYPE SUPPORT"
                            "\n===============================\n");

        bslma::TestAllocator                         da("default",
                                                        veryVeryVeryVerbose);
        bslma::TestAllocator                         sa("supplied",
                                                        veryVeryVeryVerbose);
        bsl::allocator<IncompleteTypeSupportChecker> defaultAllocator(&da);
        bsl::allocator<IncompleteTypeSupportChecker> suppliedAllocator(&sa);
        bslma::DefaultAllocatorGuard                 dag(&da);

        const IncompleteTypeSupportChecker::size_type initialSize = 5;

        IncompleteTypeSupportChecker::resetFunctionCallCounters();

        {
            IncompleteTypeSupportChecker        mIT;                       // 0
            const IncompleteTypeSupportChecker& IT = mIT;

            ASSERT(0 == IT.size());
        }

        {
            IncompleteTypeSupportChecker        mIT(suppliedAllocator);    // 1
            const IncompleteTypeSupportChecker& IT = mIT;

            ASSERT(suppliedAllocator == IT.get_allocator());
        }

        {
            IncompleteTypeSupportChecker        mIT(initialSize);          // 2
            const IncompleteTypeSupportChecker& IT = mIT;

            ASSERT(initialSize ==  IT.size());
        }

        {
            IncompleteTypeSupportChecker        mIT(
                                               initialSize,
                                               IncompleteTypeSupportChecker());
                                                                           // 3
            const IncompleteTypeSupportChecker& IT = mIT;

            ASSERT(initialSize == IT.size());
        }

        {
            IncompleteTypeSupportChecker        source[initialSize];
            IncompleteTypeSupportChecker        mIT(source,
                                                    source + initialSize);
            const IncompleteTypeSupportChecker& IT = mIT;                  // 4

            ASSERT(initialSize == IT.size());
        }

        {
            const IncompleteTypeSupportChecker  source(initialSize);
            IncompleteTypeSupportChecker        mIT(source);               // 5
            const IncompleteTypeSupportChecker& IT = mIT;

            ASSERT(initialSize == IT.size());
        }

        {
            const IncompleteTypeSupportChecker  source(initialSize);
            IncompleteTypeSupportChecker        mIT(source,
                                                    suppliedAllocator);    // 6
            const IncompleteTypeSupportChecker& IT = mIT;

            ASSERT(initialSize       == IT.size()         );
            ASSERT(suppliedAllocator == IT.get_allocator());
        }

        {
            IncompleteTypeSupportChecker        source(initialSize);
            IncompleteTypeSupportChecker        mIT(
                                    bslmf::MovableRefUtil::move(source));  // 7
            const IncompleteTypeSupportChecker& IT = mIT;

            ASSERT(initialSize == IT.size());
        }

        {
            IncompleteTypeSupportChecker        source(initialSize);
            IncompleteTypeSupportChecker        mIT(
                                           bslmf::MovableRefUtil::move(source),
                                           suppliedAllocator);             // 8
            const IncompleteTypeSupportChecker& IT = mIT;

            ASSERT(initialSize       == IT.size()         );
            ASSERT(suppliedAllocator == IT.get_allocator());
        }                                                                 // 10

        {
#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
            std::initializer_list<IncompleteTypeSupportChecker> values;
            IncompleteTypeSupportChecker                        mIT(values);
                                                                           // 9
            const IncompleteTypeSupportChecker&                 IT = mIT;

            ASSERT(0 == IT.size());
#else
            IncompleteTypeSupportChecker::increaseFunctionCallCounter(9);
#endif
        }

        {
            const IncompleteTypeSupportChecker         source1(initialSize);
            IncompleteTypeSupportChecker               source2;
            IncompleteTypeSupportChecker               source3;
            IncompleteTypeSupportChecker               source4;
            IncompleteTypeSupportChecker               source5(initialSize);
            IncompleteTypeSupportChecker               source6[initialSize];
            IncompleteTypeSupportChecker              *nullPtr = 0;

            IncompleteTypeSupportChecker        mIT;
            const IncompleteTypeSupportChecker& IT = mIT;

            IncompleteTypeSupportChecker *mR = &(mIT = source1);          // 11

            ASSERT(initialSize == IT.size());
            ASSERT(mR          == &mIT     );

            mR = &(mIT = bslmf::MovableRefUtil::move(source2));           // 12

            ASSERT(0  == IT.size());
            ASSERT(mR == &mIT     );

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
            std::initializer_list<IncompleteTypeSupportChecker> values;

            mR = &(mIT = values);                                         // 13

            ASSERT(0  == IT.size());
            ASSERT(mR == &mIT     );

            mIT.assign(values);                                           // 14

            ASSERT(0  == IT.size());

#else
            IncompleteTypeSupportChecker::increaseFunctionCallCounter(13);
            IncompleteTypeSupportChecker::increaseFunctionCallCounter(14);
#endif

            mIT.assign(source6, source6 + initialSize);                   // 15

            ASSERT(initialSize == IT.size());

            mIT.assign(2 * initialSize, source1);                         // 16

            ASSERT(2 * initialSize == IT.size());

            ASSERT(mIT.end()    != mIT.begin() );                         // 17
            ASSERT(mIT.begin()  != mIT.end()   );                         // 18
            ASSERT(mIT.rend()   != mIT.rbegin());                         // 19
            ASSERT(mIT.rbegin() != mIT.rend()  );                         // 20

            ASSERT(source1 == mIT[0]     );                               // 21
            ASSERT(source1 == mIT.at(0)  );                               // 22
            ASSERT(source1 == mIT.front());                               // 23
            ASSERT(source1 == mIT.back() );                               // 24
            ASSERT(nullPtr != mIT.data());                                // 25

            mIT.resize(0);                                                // 26

            ASSERT(0 == IT.size());

            mIT.resize(initialSize, source1);                             // 27

            ASSERT(initialSize == IT.size());

            mIT.reserve(2 * initialSize);                                 // 28

            ASSERT(2 * initialSize <= IT.capacity());

            mIT.shrink_to_fit();                                          // 29

            ASSERT(initialSize == IT.capacity());

            mR = &(mIT.emplace_back(initialSize,
                                    IncompleteTypeSupportChecker()));     // 30

            ASSERT(initialSize + 1 == IT.size());
            ASSERT(source1         == IT.back());
            ASSERT(source1         == *mR      );

            mIT.push_back(source1);                                       // 31

            ASSERT(initialSize + 2 == IT.size());
            ASSERT(source1         == IT.back());

            mIT.push_back(BloombergLP::bslmf::MovableRefUtil::move(source3));
                                                                          // 32

            ASSERT(initialSize + 3  == IT.size());
            ASSERT(IncompleteTypeSupportChecker() == IT.back());

            mIT.pop_back();                                               // 33

            ASSERT(initialSize + 2 == IT.size());
            ASSERT(source1         == IT.back());

            IncompleteTypeSupportChecker::iterator mIter = mIT.emplace(
                                               mIT.cbegin(),
                                               initialSize,
                                               IncompleteTypeSupportChecker());
                                                                          // 34

            ASSERT(initialSize + 3 == IT.size());
            ASSERT(IT.begin()      == mIter    );

            mIter = mIT.insert(mIT.cbegin(), source1);                    // 35

            ASSERT(initialSize + 4 == IT.size());
            ASSERT(IT.begin()      == mIter    );

            mIter = mIT.insert(
                            mIT.cbegin(),
                            BloombergLP::bslmf::MovableRefUtil::move(source4));
                                                                          // 36

            ASSERT(initialSize + 5 == IT.size());
            ASSERT(IT.begin()      == mIter    );

            mIter = mIT.insert(mIT.cbegin(), 5, source1);                 // 37

            ASSERT(initialSize + 10 == IT.size());
            ASSERT(IT.begin()       == mIter    );

            mIter = mIT.insert(mIT.cbegin(), source6, source6 + initialSize);
                                                                          // 38
            ASSERT(initialSize + 15 == IT.size());
            ASSERT(IT.begin()       == mIter    );

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
            mIter = mIT.insert(mIT.cbegin(), values);                     // 39

            ASSERT(initialSize + 15 == IT.size());
            ASSERT(IT.begin()       == mIter    );
#else
            IncompleteTypeSupportChecker::increaseFunctionCallCounter(39);
#endif

            mIT.erase(mIT.cbegin());                                      // 40

            ASSERT(initialSize + 14 == IT.size());

            mIT.erase(mIT.cbegin(), mIT.cbegin() + 5);                    // 41

            ASSERT(initialSize + 9 == IT.size());

            mIT.swap(source5);                                            // 42

            ASSERT(initialSize     == IT.size()     );
            ASSERT(initialSize + 9 == source5.size());

            mIT.clear();

            ASSERT(0 == IT.size());                                       // 43

            ASSERT(IT.end()     == IT.begin()  );                         // 44
            ASSERT(IT.cend()    == IT.cbegin() );                         // 45
            ASSERT(IT.begin()   == IT.end()    );                         // 46
            ASSERT(IT.cbegin()  == IT.cend()   );                         // 47
            ASSERT(IT.rend()    == IT.rbegin() );                         // 48
            ASSERT(IT.crend()   == IT.crbegin());                         // 49
            ASSERT(IT.rbegin()  == IT.rend()   );                         // 50
            ASSERT(IT.crbegin() == IT.crend()  );                         // 51

            ASSERT(0                == IT.size()         );               // 52
            ASSERT(0                != IT.capacity()     );               // 53
            ASSERT(true             == IT.empty()        );               // 54
            ASSERT(defaultAllocator == IT.get_allocator());               // 55
            ASSERT(0                != IT.max_size()     );               // 56

            mIT.push_back(source1);

            ASSERT(source1 ==  IT[0]     );                               // 57
            ASSERT(source1 ==  IT.at(0)  );                               // 58
            ASSERT(source1 ==  IT.front());                               // 59
            ASSERT(source1 ==  IT.back() );                               // 60
            ASSERT(nullPtr !=  IT.data() );                               // 61
        }

        // Check if each function has been called.

        IncompleteTypeSupportChecker::checkInvokedFunctions();
      } break;
      case 40: {
        // --------------------------------------------------------------------
        // TESTING FREE FUNCTIONS 'BSL::ERASE' AND 'BSL::ERASE_IF'
        //
        // Concerns:
        //: 1 The free functions exist, and are callable with a vector.
        //
        // Plan:
        //: 1 Fill a vector with known values, then attempt to erase some of
        //:   the values using 'bsl::erase' and 'bsl::erase_if'.  Verify that
        //:   the resultant vector is the right size, contains the correct
        //:   values, and that the value returned from the functions is
        //:   correct.
        //
        // Testing:
        //   size_t erase(vector<T,A>&, const T&);
        //   size_t erase_if(vector<T,A>&, PREDICATE);
        // --------------------------------------------------------------------

        if (verbose)
            printf(
                "\nTESTING FREE FUNCTIONS 'BSL::ERASE' AND 'BSL::ERASE_IF'"
                "\n=======================================================\n");

        TestDriver3<char>::testCase40();
        TestDriver3<int>::testCase40();
        TestDriver3<long>::testCase40();
      } break;
      case 39: {
        //---------------------------------------------------------------------
        // TESTING CLASS TEMPLATE DEDUCTION GUIDES (AT COMPILE TIME)
        //   Ensure that the deduction guides are properly specified to deduce
        //   the template arguments from the arguments supplied to the
        //   constructors.
        //
        // Concerns:
        //: 1 Construction from iterators deduces the value type from the value
        //:   type of the iterator.
        //
        //: 2 Construction with a 'bslma::Allocator *' deduces the correct
        //:   specialization of 'bsl::allocator' for the type of the allocator.
        //
        // Plan:
        //: 1 Create a vector by invoking the constructor without supplying the
        //:   template arguments explicitly.
        //:
        //: 2 Verify that the deduced type is correct.
        //
        // Testing:
        //   CLASS TEMPLATE DEDUCTION GUIDES
        //---------------------------------------------------------------------
        if (verbose)
            printf(
              "\nTESTING CLASS TEMPLATE DEDUCTION GUIDES (AT COMPILE TIME)"
              "\n=========================================================\n");

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
        // This is a compile-time only test case.
        TestDeductionGuides test;
        (void) test; // This variable only exists for ease of IDE navigation.
#endif
      } break;
      case 38: {
        // --------------------------------------------------------------------
        // TESTING ABILITY TO MOVE BSLTF MOVABLE TYPES
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver3,
                      testCase38,
                      bsltf::MovableTestType,
                      bsltf::MovableAllocTestType);
      } break;
      case 37: {
        // --------------------------------------------------------------------
        // TESTING ACCESS THROUGH MEMBER POINTERS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING ACCESS THROUGH MEMBER POINTERS"
                            "\n======================================\n");

        TestDriver3<int *>::testCase37();
        TestDriver3<const char *>::testCase37();
      } break;
      case 36: {
        // --------------------------------------------------------------------
        // TESTING 'vector<bool>'
        //
        // Concerns:
        //: 1 The 'vector<bool>' type is awkward to test with the 'bsltf'
        //:   facility as 'bool' has just 2 valid states, but 'bsltf' requires
        //:   a 'value_type' with at least 128 valid states.
        //: 2 The type 'vector<bool>' should have an extra specialization per
        //:   the standard.  Since 'bsl' does not (yet) implement the standard
        //:   interface, nor a compressed representation, we do not verify this
        //:   concern.
        //
        // Plan:
        //: 1 Verify each constructor, accessor and manipulator.
        //
        // Testing:
        //   CONCERN: 'vector<bool>' is also verified
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'vector<bool>'"
                            "\n======================\n");

        typedef bsl::vector<bool> Obj;

        bslma::TestAllocator ta("'bool' tests", veryVeryVeryVerbose);

        if (veryVerbose) printf("\tDefault-constructed state\n");

        Obj mX(&ta);  const Obj&X = mX;
        ASSERTV(X.empty(), X.empty());
        ASSERTV(X.size(), 0 == X.size());

        ASSERTV( (X == X) );
        ASSERTV(!(X != X) );

        if (veryVerbose) printf("\t'push_back'\n");

        mX.push_back(true);
        ASSERTV(X.empty(), !X.empty());
        ASSERTV(X.size(), 1u == X.size());
        ASSERTV(X.back(), X.back());
        ASSERTV(X.front(), X.front());

        ASSERTV( (X == X) );
        ASSERTV(!(X != X) );

        mX.push_back(false);
        ASSERTV(X.empty(), !X.empty());
        ASSERTV(X.size(), 2u == X.size());
        ASSERTV(X.back(), !X.back());
        ASSERTV(X.front(), X.front());

        ASSERTV( (X == X) );
        ASSERTV(!(X != X) );

        if (veryVerbose) printf("\tself-assignment\n");

        mX = X;
        ASSERTV(X.empty(), !X.empty());
        ASSERTV(X.size(), 2u == X.size());
        ASSERTV(X.back(), !X.back());
        ASSERTV(X.front(), X.front());

        ASSERTV( (X == X) );
        ASSERTV(!(X != X) );

        if (veryVerbose) printf("\t'pop_back'\n");

        mX.pop_back();
        ASSERTV(X.empty(), !X.empty());
        ASSERTV(X.size(), 1u == X.size());
        ASSERTV(X.back(), X.back());
        ASSERTV(X.front(), X.front());

        ASSERTV( (X == X) );
        ASSERTV(!(X != X) );

        if (veryVerbose) printf("\t'clear'\n");

        mX.clear();
        ASSERTV(X.empty(), X.empty());
        ASSERTV(X.size(), 0 == X.size());

        ASSERTV( (X == X) );
        ASSERTV(!(X != X) );

        if (veryVerbose) printf("\t'emplace_back'\n");

        mX.emplace_back(99);
        ASSERTV(X.size(), 1u == X.size());
        ASSERTV(X.back(), X.back());

        mX.emplace_back();
        ASSERTV(X.size(), 2u == X.size());
        ASSERTV(X.front(), X.front());
        ASSERTV(X.back(), !X.back());

        mX.emplace_back(0);
        ASSERTV(X.size(), 3u == X.size());
        ASSERTV(X.front(), X.front());
        ASSERTV(X.at(1), !X.at(1));
        ASSERTV(X.back(), !X.back());

        if (veryVerbose) printf("\t(alias-safe) 'insert' single element\n");

        // insert rvalue, reach capacity
        Obj::iterator it = mX.insert(X.cbegin()+1, false);
        ASSERTV(X.size(), 4u == X.size());
        ASSERTV(X.capacity(), 4u == X.capacity());
        ASSERTV(it - X.cbegin(), X.cbegin()+1 == it);
        for (Obj::size_type i = 0; i != 3u; ++i) {
            ASSERTV(i, X[i], !i == X[i]);
        }

        // insert past capacity
        it = mX.insert(X.cbegin()+1, mX[0]);
        ASSERTV(X.size(), 5u == X.size());
        ASSERTV(X.capacity(), 8u == X.capacity());
        ASSERTV(it - X.cbegin(), X.cbegin()+1 == it);
        for (Obj::size_type i = 0; i != 5u; ++i) {
            ASSERTV(i, X[i], (i < 2) == X[i]);
        }

        // insert nothing
        it = mX.insert(X.begin()+3, 0, 0);
        ASSERTV(X.size(), 5u == X.size());
        ASSERTV(X.capacity(), 8u == X.capacity());
        ASSERTV(it - X.cbegin(), X.cbegin()+3 == it);
        for (Obj::size_type i = 0; i != 5u; ++i) {
            ASSERTV(i, X[i], (i < 2) == X[i]);
        }

        // insert 3 'true' values, back to capacity
        it = mX.insert(X.begin()+2, 3, 3);
        ASSERTV(X.size(), 8u == X.size());
        ASSERTV(X.capacity(), 8u == X.capacity());
        ASSERTV(it - X.cbegin(), X.cbegin()+2 == it);
        for (Obj::size_type i = 0; i != 8u; ++i) {
            ASSERTV(i, X[i], (i < 5) == X[i]);
        }

        // alias-safe insert with growth
        it = mX.insert(X.begin()+1, 2, X[1]);
        ASSERTV(X.size(), 10u == X.size());
        ASSERTV(X.capacity(), 16u == X.capacity());
        ASSERTV(it - X.cbegin(), X.cbegin()+1 == it);
        for (Obj::size_type i = 0; i != 10u; ++i) {
            ASSERTV(i, X[i], (i < 7) == X[i]);
        }

        if (veryVerbose) printf("\t'erase'\n");

        it = mX.erase(X.begin()+2);
        ASSERTV(it - X.cbegin(), X.cbegin()+2 == it);
        ASSERTV(X.size(), 9u == X.size());

        it = mX.erase(it, mX.cbegin()+7);
        ASSERTV(it - X.cbegin(), X.cbegin()+2 == it);
        ASSERTV(X.size(), 4u == X.size());
        for (Obj::size_type i = 0; i != 4u; ++i) {
            ASSERTV(i, X[i], (i < 2u) == X[i]);
        }

        if (veryVerbose) printf("\t'resize'\n");

        mX.resize(16);
        ASSERTV(X.size(), 16u == X.size());
        ASSERTV(X.capacity(), 16u == X.capacity());
        for (Obj::size_type i = 0; i != 16u; ++i) {
            ASSERTV(i, X[i], (i < 2u) == X[i]);
        }

        mX.resize(10);
        ASSERTV(X.size(), 10u == X.size());
        ASSERTV(X.capacity(), 16u == X.capacity());
        for (Obj::size_type i = 0; i != 10u; ++i) {
            ASSERTV(i, X[i], (i < 2u) == X[i]);
        }

        if (veryVerbose) printf("\t'reserve'\n");

        mX.reserve(17);
        ASSERTV(X.size(), 10u == X.size());
        ASSERTV(X.capacity(), 17u == X.capacity());
        for (Obj::size_type i = 0; i != 10u; ++i) {
            ASSERTV(i, X[i], (i < 2u) == X[i]);
        }

        mX.resize(17);
        ASSERTV(X.size(), 17u == X.size());
        ASSERTV(X.capacity(), 17u == X.capacity());
        for (Obj::size_type i = 0; i != 17u; ++i) {
            ASSERTV(i, X[i], (i < 2u) == X[i]);
        }

        if (veryVerbose) printf("\tsecond vector to test more operations\n");

        // Range constructor

        Obj mY(X.begin(), X.end(), &ta);  const Obj&Y = mY;
        ASSERTV(Y.size(), 17u == Y.size());
        ASSERTV(Y.capacity(), 32u == Y.capacity());

        ASSERTV( (X == Y) );
        ASSERTV(!(X != Y) );

        if (veryVerbose) printf("\t'swap'\n");

        ASSERTV(X.capacity(), 17u == X.capacity());
        ASSERTV(Y.capacity(), 32u == Y.capacity());

        mX.swap(mY);

        ASSERTV(X.capacity(), 32u == X.capacity());
        ASSERTV(Y.capacity(), 17u == Y.capacity());

        ASSERTV( (X == Y) );
        ASSERTV(!(X != Y) );

        if (veryVerbose) printf("\t'clear'\n");

        mY.clear();
        ASSERTV(Y.empty(), Y.empty());

        if (veryVerbose) printf("\t'operator=(const &)'\n");

        mY = X;

        ASSERTV( (X == Y) );
        ASSERTV(!(X != Y) );

        if (veryVerbose) printf("\t'erase' all\n");

        mY.erase(mY.cbegin(), mY.cend());
        ASSERTV(Y.empty(), Y.empty());
        ASSERTV(Y.size(), 0u == Y.size());
        ASSERTV(Y.capacity(), 17u == Y.capacity());

        if (veryVerbose) printf("\t'operator=(&&)'\n");

        mY = bslmf::MovableRefUtil::move(mX);
        ASSERTV(X.empty(), X.empty());
        ASSERTV(X.size(), 0u == X.size());
        ASSERTV(X.capacity(), 0u == X.capacity());

        ASSERTV(Y.size(), 17u == Y.size());
        ASSERTV(Y.capacity(), 32u == Y.capacity());
        for (Obj::size_type i = 0; i != 17u; ++i) {
            ASSERTV(i, Y[i], (i < 2u) == Y[i]);
        }

        if (veryVerbose) printf("\t'assign' range with allocation\n");

        mX.assign(Y.begin() + 1, Y.begin() + 7);
        ASSERTV(X.size(), 6u == X.size());
        ASSERTV(X.capacity(), 8u == X.capacity());
        for (Obj::size_type i = 0; i != 6u; ++i) {
            ASSERTV(i, X[i], !i == X[i]);
        }

        if (veryVerbose) printf("\t'assign' copies without allocation\n");

        mX.assign(3, false);
        ASSERTV(X.size(), 3u == X.size());
        ASSERTV(X.capacity(), 8u == X.capacity());
        for (Obj::size_type i = 0; i != 3u; ++i) {
            ASSERTV(i, X[i], !X[i]);
        }

        if (veryVerbose) printf("\t'assign' copies with allocation\n");

        mX.assign(11, true);
        ASSERTV(X.size(), 11u == X.size());
        ASSERTV(X.capacity(), 16u == X.capacity());
        for (Obj::size_type i = 0; i != 11u; ++i) {
            ASSERTV(i, X[i], X[i]);
        }

        if (veryVerbose) printf("\t'assign' range without allocation\n");

        mX.assign(Y.begin() + 3, Y.begin() + 8);
        ASSERTV(X.size(), 5u == X.size());
        ASSERTV(X.capacity(), 16u == X.capacity());
        for (Obj::size_type i = 0; i != 5u; ++i) {
            ASSERTV(i, X[i], !X[i]);
        }

        if (veryVerbose) printf("\t'insert' range without allocation\n");

        it = mX.insert(X.begin() + 2, Y.begin(), Y.begin() + 2);
        ASSERTV(X.begin() + 2 == it);
        ASSERTV(X.size(), 7u == X.size());
        ASSERTV(X.capacity(), 16u == X.capacity());
        for (Obj::size_type i = 0; i != 5u; ++i) {
            ASSERTV(i, X[i], (i == 2 || i == 3) == X[i]);
        }

        if (veryVerbose) printf("\t'insert' range with allocation\n");

        it = mX.insert(it + 2, Y.begin(), Y.end() - 2);
        ASSERTV(X.begin() + 4 == it);
        ASSERTV(X.size(), 22u == X.size());
        ASSERTV(X.capacity(), 32u == X.capacity());
        for (Obj::size_type i = 0; i != 5u; ++i) {
            ASSERTV(i, X[i], (i > 1 && i < 6) == X[i]);
        }
      } break;
      case 35: {
        // --------------------------------------------------------------------
        // TESTING 'noexcept' SPECIFICATIONS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'noexcept' SPECIFICATIONS"
                            "\n=================================\n");

        TestDriver3<int  >::testCase35();
        TestDriver3<int *>::testCase35();
        TestDriver3<const char *>::testCase35();

      } break;
      case 34: {
        // --------------------------------------------------------------------
        // TESTING 'hashAppend'
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'hashAppend'"
                            "\n====================\n");

        RUN_EACH_TYPE(TestDriver3,
                      testCase34,
                      signed char,
                      size_t,
                      bsltf::TemplateTestFacility::ObjectPtr,
                      bsltf::TemplateTestFacility::FunctionPtr,
                      const char *);
      } break;
      case 33: {
        // --------------------------------------------------------------------
        // TESTING HYMAN'S TEST CASE 2
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING HYMAN'S TEST CASE 2"
                            "\n===========================\n");

        TestDriver3<int>::testCase33();

      } break;
      case 32: {
        // --------------------------------------------------------------------
        // TESTING HYMAN'S TEST CASE 1
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING HYMAN'S TEST CASE 1"
                            "\n===========================\n");

        TestDriver3<int>::testCase32();

      } break;
      case 31: {
        // --------------------------------------------------------------------
        // TESTING VECTORS OF FUNCTION POINTERS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING VECTORS OF FUNCTION POINTERS"
                            "\n====================================\n");

        TestDriver3<int>::testCase31();

      } break;
      case 30: {
        // --------------------------------------------------------------------
        // TESTING FUNCTION POINTER RANGE-INSERT BUGFIX
        // --------------------------------------------------------------------

        if (verbose) printf(
                           "\nTESTING FUNCTION POINTER RANGE-INSERT BUGFIX"
                           "\n============================================\n");

        TestDriver3<int>::testCase30();

      } break;
      case 29: {
        // --------------------------------------------------------------------
        // TESTING INITIALIZER LIST FUNCTIONS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING INITIALIZER LIST FUNCTIONS"
                            "\n==================================\n");

        RUN_EACH_TYPE(TestDriver3,
                      testCase29,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(StdBslmaTestDriver3,
                      testCase29,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
      } break;
      case 28: {
        // --------------------------------------------------------------------
        // TESTING 'emplace'
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'emplace'"
                            "\n=================\n");

        if (verbose)
            printf("This test has not yet been fully implemented.\n");

        if (verbose) printf("\nTesting Value Emplacement"
                            "\n=======================\n");

        RUN_EACH_TYPE(TestDriver3,
                      testCase28,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonDefaultConstructibleTestType
//                    , bsltf::MoveOnlyAllocTestType
                     );

        RUN_EACH_TYPE(TestDriver3,
                      testCase28a,
                      bsltf::EmplacableTestType,
                      bsltf::AllocEmplacableTestType);

        RUN_EACH_TYPE(StdBslmaTestDriver3,
                      testCase28,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE
//                    , bsltf::MoveOnlyAllocTestType
                     );
      } break;
      case 27: {
        // --------------------------------------------------------------------
        // TESTING 'insert' ON MOVABLE VALUES
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'insert' ON MOVABLE VALUES"
                            "\n==================================\n");

        if (verbose)
            printf("This test has not yet been fully implemented.\n");

        RUN_EACH_TYPE(TestDriver3,
                      testCase27,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);

        RUN_EACH_TYPE(StdBslmaTestDriver3,
                      testCase27,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);

#if 0
        ITER_CONTAINER_RUN_EACH_TYPE(TestDriver3,
                                     testCase27Range,
                                     bsltf::NonDefaultConstructibleTestType,
                                     bsltf::MoveOnlyAllocTestType,
                                     bsltf::WellBehavedMoveOnlyAllocTestType,
                                     NotAssignable,
                                     BitwiseNotAssignable);
#else
        ITER_CONTAINER_RUN_EACH_TYPE(TestDriver3,
                                     testCase27Range,
                                     bsltf::NonDefaultConstructibleTestType,
                                     int,   // dummy, need 4 types
                                     int,   // dummy, need 4 types
                                     int);  // dummy, need 4 types
#endif

#if BSLS_COMPILERFEATURES_CPLUSPLUS >= 201103L
        // Testing move-only types.  Although we can simulate a move-only type
        // in C++03, we cannot simulate a noexcept move constructor as vector
        // requires.
        {
            bsl::vector<MoveOnlyType> mX;
            MoveOnlyType value;
            mX.insert(mX.end(), MoveUtil::move(value));
        }
#endif
      } break;
      case 26: {
        // --------------------------------------------------------------------
        // TESTING 'emplace_back'
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'emplace_back'"
                            "\n======================\n");

        if (verbose)
            printf("This test has not yet been fully implemented.\n");

        if (verbose) printf("\nTesting Value Emplacement"
                            "\n=========================\n");

        // TBD: should be 'BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR'
        RUN_EACH_TYPE(TestDriver3,
                      testCase26,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonDefaultConstructibleTestType,
//                      bsltf::MoveOnlyAllocTestType,
                     NotAssignable,
                     BitwiseNotAssignable);

        RUN_EACH_TYPE(TestDriver3,
                      testCase26a,
                      bsltf::EmplacableTestType,
                      bsltf::AllocEmplacableTestType);

        RUN_EACH_TYPE(StdBslmaTestDriver3,
                      testCase26,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE
//                   , bsltf::MoveOnlyAllocTestType
                     );
      } break;
      case 25: {
        // --------------------------------------------------------------------
        // TESTING 'push_back' ON MOVABLE VALUES
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'push_back' ON MOVABLE VALUES"
                            "\n=====================================\n");

        if (verbose)
            printf("This test has not yet been fully implemented.\n");

        RUN_EACH_TYPE(TestDriver3,
                      testCase25,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::WellBehavedMoveOnlyAllocTestType);

        RUN_EACH_TYPE(StdBslmaTestDriver3,
                      testCase25,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);
      } break;
      case 24: {
        // --------------------------------------------------------------------
        // TESTING MOVE ASSIGNMENT
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING MOVE ASSIGNMENT"
                            "\n=======================\n");

        RUN_EACH_TYPE(MetaTestDriver3,
                      testCase24,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);

        RUN_EACH_TYPE(TestDriver3,
                      testCase24_dispatch,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_USER_DEFINED);

      } break;
      case 23: BSLA_FALLTHROUGH;
      case 22: BSLA_FALLTHROUGH;
      case 21: BSLA_FALLTHROUGH;
      case 20: BSLA_FALLTHROUGH;
      case 19: BSLA_FALLTHROUGH;
      case 18: BSLA_FALLTHROUGH;
      case 17: BSLA_FALLTHROUGH;
      case 16: BSLA_FALLTHROUGH;
      case 15: BSLA_FALLTHROUGH;
      case 14: BSLA_FALLTHROUGH;
      case 13: BSLA_FALLTHROUGH;
      case 12: BSLA_FALLTHROUGH;
      case 11: BSLA_FALLTHROUGH;
      case 10: BSLA_FALLTHROUGH;
      case  9: BSLA_FALLTHROUGH;
      case  8: BSLA_FALLTHROUGH;
      case  7: BSLA_FALLTHROUGH;
      case  6: BSLA_FALLTHROUGH;
      case  5: BSLA_FALLTHROUGH;
      case  4: BSLA_FALLTHROUGH;
      case  3: BSLA_FALLTHROUGH;
      case  2: BSLA_FALLTHROUGH;
      case  1: {
        if (verbose)
            printf("Test case %d is in another test driver part.\n"
                   "See 'bslstl_vector.0.t.cpp' for the test plan.\n",
                   test);
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    ASSERTV(defaultAllocator.numBlocksInUse(),
            0 == defaultAllocator.numBlocksInUse());

    // CONCERN: In no case does memory come from the global allocator.
    ASSERTV(globalAllocator.numBlocksTotal(),
            0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
