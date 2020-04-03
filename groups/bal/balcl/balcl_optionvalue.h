// balcl_optionvalue.h                                                -*-C++-*-
#ifndef INCLUDED_BALCL_OPTIONVALUE
#define INCLUDED_BALCL_OPTIONVALUE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a variant type for command-line-option values.
//
//@CLASSES:
//  balcl::OptionValue: the value of a user supplied command-line option
//
//@SEE_ALSO: balcl_optiontype, balcl_commandline
//
//@DESCRIPTION: This component provides a value-semantic class,
// 'balcl::OptionValue', that can have a value of any of the types specified by
// 'balcl::OptionType' -- i.e., any of the values that can be associated with a
// command-line option by 'balcl::CommandLine'.  The 'balcl::OptionValue' class
// has two related states:
//
//: 1 A default-constructed 'balcl::OptionValue' object is in the "unset state"
//:   -- meaning that no type has been defined for a value (its type is
//:   'void').  In this state, 'balcl::OptionType::e_VOID == type()' and
//:   'false == hasNonVoidType()'.  To have a value, a type must be specified
//:   for the value by using the 'setType' method or using one of the
//:   constructors that define an initial value.  {Example 1} shows how this
//:   state can be set and reset.
//
//: 2 If a 'balcl::OptionValue' object has a (non-'void') type it can have a
//:   value of that type or be in a "null state".  Objects in the null state
//:   can be used to represent the value of command-line options that were not
//:   entered on the command line (assuming no default value was specified for
//:   the option).  {Example 2} shows how this feature can be used.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Use of 'balcl::OptionValue'
/// - - - - - - - - - - - - - - - - - - - - - -
// The following snippets of code illustrate how to create and use a
// 'balcl::OptionValue' object.  Note that 'balcl::OptionValue' objects are
// typically used in a description of a sequence of command-line options (see
// {'balcl_optiontype'}).
//
// First, we create a default 'balcl::OptionValue', 'valueA', and observe that
// it is in the unset state:
//..
//  balcl::OptionValue valueA;
//
//  assert(false                     == valueA.hasNonVoidType());
//  assert(balcl::OptionType::e_VOID == valueA.type());
//..
// Next, we create a second 'balcl::OptionValue' having the value 5, and then
// confirm its value and observe that it does not compare equal to the
// 'valueA':
//..
//  balcl::OptionValue valueB(5);
//
//  assert(true                     == valueB.hasNonVoidType());
//  assert(balcl::OptionType::e_INT == valueB.type());
//  assert(5                        == valueB.the<int>());
//
//  assert(valueA != valueB);
//..
// Then, we call the 'reset' method of 'valueB' resetting it to the unset
// state, and observe that 'valueA' now compares equal to 'valueB':
//..
//  valueB.reset();
//
//  assert(valueA == valueB);
//..
// Now, we change the type of 'valueA' so that it can be hold a 'double' value:
//..
//  valueA.setType(balcl::OptionType::e_DOUBLE);
//  assert(true                        == valueA.hasNonVoidType());
//  assert(balcl::OptionType::e_DOUBLE == valueA.type());
//  assert(double()                    == valueA.the<double>());
//
//  valueA.set(6.0);
//  assert(6.0                         == valueA.the<double>());
//..
// Finally, we set the object to the null state.  Notice that the type of that
// value is not changed:
//..
//  valueA.setNull();
//  assert(true                        == valueA.isNull());
//  assert(balcl::OptionType::e_DOUBLE == valueA.type());
//..
//
///Example 2: Interpreting Option Parser Results
///- - - - - - - - - - - - - - - - - - - - - - -
// Command-line options have values of many different types (e.g., 'int',
// 'double', string, date) or their values may not be specified -- after all,
// some command-line options may be *optional*.  The 'balcl::OptionValue' class
// can be used to represent such values.
//
// First, we define 'MyCommandLineParser', a simple command-line argument
// parser.  This class accepts a description (e.g., option name, value type) of
// allowable options on construction and provides a 'parse' method that accepts
// 'argc' and 'argv', the values made available (by the operating system) to
// 'main':
//..
//                      // =========================
//                      // class MyCommandLineParser
//                      // =========================
//
//  class MyCommandLineParser {
//      // ...
//
//    public:
//      // CREATORS
//      MyCommandLineParser(const MyOptionDescription *descriptions,
//                          bsl::size_t                count);
//          // Create an object that can parse command-line arguments that
//          // satisfy the specified 'descriptions', an array containing the
//          // specified 'count' elements.
//
//      // ...
//
//      // MANIPULATORS
//      int parse(int argc, const char **argv);
//          // Parse the command-line options in the specified 'argv', an array
//          // having the specified 'argc' elements.  Return 0 on success --
//          // i.e., the options were compatible with the option descriptions
//          // specified on construction -- and a non-zero value otherwise.
//
//      // ...
//..
// After a successful call to the 'parse' method, the results are available by
// several accessors.  Note that the 'index' of a result corresponds to the
// index of that option in the description provided on construction:
//..
//      // ACCESSORS
//      bool isParsed() const;
//          // Return 'true' if the most recent call to 'parsed' was successful
//          // and 'false' otherwise.
//
//      const char *name (bsl::size_t index) const;
//          // Return of the name of the parsed option at the specified 'index'
//          // position.  The behavior is undefined unless
//          // '0 <= index < numOptions()' and 'true == isParsed()'
//
//      const balcl::OptionValue& value(bsl::size_t index) const;
//          // Return a 'const' reference to the value (possibly in a null
//          // state) of the parsed option at the specified 'index' position.
//          // The behavior is undefined unless '0 <= index < numOptions()' and
//          // 'true == isParsed()'.
//
//      bsl::size_t numOptions() const;
//          // Return the number of parsed options.  The behavior is undefined
//          // unless 'true == isParsed()'.
//
//      // ...
//  };
//..
// Note that neither our option description nor our parser support the concept
// of default values for options that are not entered on the command line.
//
// Then, we create a description having three allowable options (elided), a
// parser object, and invoke 'parse' on the arguments available from 'main':
//..
//  int main(int argc, const char **argv)
//  {
//      MyOptionDescription optionDescriptions[NUM_OPTIONS] = {
//          // ...
//      };
//
//      MyCommandLineParser parser(optionDescriptions, NUM_OPTIONS);
//
//      int rc = parser.parse(argc, argv);
//      assert(0    == rc);
//      assert(true == parser.isParsed());
//..
// Now, we examine the value of each defined option:
//..
//      for (bsl::size_t i = 0; i < parser.numOptions(); ++i) {
//          const char                *name  = parser.name(i);
//          const balcl::OptionValue&  value = parser.value(i);
//..
// Since our (toy) parser has no feature for handling default values for
// options that are not specified on the command line, we must handle those
// explicitly.
//
// If the option named "outputDir" was set, we use that value; otherwise, we
// set a default value, the current directory:
//..
//          if (0 == bsl::strcmp("outputDir", name)) {
//              setOutputDir(value.isNull()
//                           ? "."
//                           : value.the<bsl::string>().c_str());
//          }
//..
// If the option named "verbosityLevel" was set we use that value; otherwise,
// we set a default value, '1':
//..
//          if (0 == bsl::strcmp("verbosityLevel", name)) {
//              setVerbosityLevel(value.isNull()
//                                ? 1
//                                : value.the<int>());
//          }
//..
// The option named "caseInsensitive" has no associated value.  If that option
// appeared on the command line, the value of the program flag is set to
// 'true', otherwise ('false == isNull()') that flag is set to 'false':
//..
//          if (0 == bsl::strcmp("caseInsensitive", name)) {
//              setCaseInsensitivityFlag(value.isNull()
//                                       ? false
//                                       : true);
//          }
//      }
//..
// Finally, we continue with the execution of our program using the values
// obtained from the command-line options:
//..
//      // ...
//
//      return 0;
//  }
//..

#include <balscm_version.h>

#include <balcl_optiontype.h>

#include <bdlt_date.h>
#include <bdlt_datetime.h>
#include <bdlt_time.h>

#include <bdlb_nullablevalue.h>
#include <bdlb_printmethods.h> // 'bdlb::HasPrintMethod'
#include <bdlb_variant.h>

#include <bslma_usesbslmaallocator.h>

#include <bslmf_nestedtraitdeclaration.h>
#include <bslmf_nil.h>

#include <bsls_assert.h>
#include <bsls_types.h>  // 'bsls::Types::Int64'

#include <bsl_iosfwd.h>  // 'bsl::ostream'
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {

namespace bslma { class Allocator; }

namespace balcl {

class OptionValue_IsNullVisitor;
class OptionValue_SetNullVisitor;

                        // =================
                        // class OptionValue
                        // =================

class OptionValue {
    // This class implements a special-use value-semantic variant type used to
    // represent values parsed from process command lines.  Accordingly, this
    // class can represent values of any of the types defined in
    // {'balcl_optiontype'}.  Furthermore, that value can also be in a null
    // state (defined type but no defined value) to represent allowed options
    // that do not appear among the command-line arguments (and for which no
    // default value has been configured).

  private:
    // PRIVATE TYPES
    typedef OptionType Ot;

    typedef bdlb::NullableValue<Ot::Bool>          Bool;
    typedef bdlb::NullableValue<Ot::Char>          Char;
    typedef bdlb::NullableValue<Ot::Int>           Int;
    typedef bdlb::NullableValue<Ot::Int64>         Int64;
    typedef bdlb::NullableValue<Ot::Double>        Double;
    typedef bdlb::NullableValue<Ot::String>        String;
    typedef bdlb::NullableValue<Ot::Datetime>      Datetime;
    typedef bdlb::NullableValue<Ot::Date>          Date;
    typedef bdlb::NullableValue<Ot::Time>          Time;
    typedef bdlb::NullableValue<Ot::CharArray>     CharArray;
    typedef bdlb::NullableValue<Ot::IntArray>      IntArray;
    typedef bdlb::NullableValue<Ot::Int64Array>    Int64Array;
    typedef bdlb::NullableValue<Ot::DoubleArray>   DoubleArray;
    typedef bdlb::NullableValue<Ot::StringArray>   StringArray;
    typedef bdlb::NullableValue<Ot::DatetimeArray> DatetimeArray;
    typedef bdlb::NullableValue<Ot::DateArray>     DateArray;
    typedef bdlb::NullableValue<Ot::TimeArray>     TimeArray;

    typedef bdlb::Variant<Bool,
                          Char,
                          Int,
                          Int64,
                          Double,
                          String,
                          Datetime,
                          Date,
                          Time,
                          CharArray,
                          IntArray,
                          Int64Array,
                          DoubleArray,
                          StringArray,
                          DatetimeArray,
                          DateArray,
                          TimeArray> ValueVariant;

    // DATA
    ValueVariant d_value;  // the object's value

    // FRIENDS
    friend bool operator==(const OptionValue&, const OptionValue&);
    friend void swap(OptionValue&, OptionValue&);

    friend class OptionValue_IsNullVisitor;
    friend class OptionValue_SetNullVisitor;

    // PRIVATE MANIPULATORS
    void init(OptionType::Enum type);
        // Set the type of the contained variant object to have the specified
        // 'type' and have the default value for 'type'.  The behavior is
        // undefined unless 'OptionType::e_VOID != type()'.

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(OptionValue, bslma::UsesBslmaAllocator);
    BSLMF_NESTED_TRAIT_DECLARATION(OptionValue, bdlb::HasPrintMethod);

    // CREATORS
    OptionValue();
    explicit
    OptionValue(bslma::Allocator *basicAllocator);
        // Create a command-line-option value object in the unset state (i.e.,
        // 'OptionType::e_VOID == type()').  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is not
        // specified, the currently installed default allocator is used.  Note
        // that (atypically) this constructor disallows 0 for 'basicAllocator';
        // invoking the constructor with 0 dispatches to the constructor
        // overload that explicitly accepts an 'int' value for its first
        // argument (see below).

    OptionValue(OptionType::Enum  type,
                bslma::Allocator *basicAllocator = 0); // IMPLICIT
        // Create a command-line-option value object having the type
        // corresponding to the specified 'type' and, if
        // 'OptionType::e_VOID != type', having the default value of that type.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    explicit
    OptionValue(bool                value,
                bslma::Allocator   *basicAllocator = 0);
    explicit
    OptionValue(char                value,
                bslma::Allocator   *basicAllocator = 0);
    explicit
    OptionValue(int                 value,
                bslma::Allocator   *basicAllocator = 0);
    explicit
    OptionValue(bsls::Types::Int64  value,
                bslma::Allocator   *basicAllocator = 0);
    explicit
    OptionValue(double              value,
                bslma::Allocator   *basicAllocator = 0);
    explicit
    OptionValue(const bsl::string&  value,
                bslma::Allocator   *basicAllocator = 0);
    explicit
    OptionValue(bdlt::Datetime      value,
                bslma::Allocator   *basicAllocator = 0);
    explicit
    OptionValue(bdlt::Date          value,
                bslma::Allocator   *basicAllocator = 0);
    explicit
    OptionValue(bdlt::Time          value,
                bslma::Allocator   *basicAllocator = 0);
    explicit
    OptionValue(const bsl::vector<char>&                value,
                bslma::Allocator                       *basicAllocator = 0);
    explicit
    OptionValue(const bsl::vector<int>&                 value,
                bslma::Allocator                       *basicAllocator = 0);
    explicit
    OptionValue(const bsl::vector<bsls::Types::Int64>&  value,
                bslma::Allocator                       *basicAllocator = 0);
    explicit
    OptionValue(const bsl::vector<double>&              value,
                bslma::Allocator                       *basicAllocator = 0);
    explicit
    OptionValue(const bsl::vector<bsl::string>&         value,
                bslma::Allocator                       *basicAllocator = 0);
    explicit
    OptionValue(const bsl::vector<bdlt::Datetime>&      value,
                bslma::Allocator                       *basicAllocator = 0);
    explicit
    OptionValue(const bsl::vector<bdlt::Date>&          value,
                bslma::Allocator                       *basicAllocator = 0);
    explicit
    OptionValue(const bsl::vector<bdlt::Time>&          value,
                bslma::Allocator                       *basicAllocator = 0);
        // Create a command-line-option value object having the type and value
        // of the specified 'value'.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    OptionValue(const OptionValue&  original,
                bslma::Allocator   *basicAllocator = 0);
        // Create a 'OptionValue' object having the same value as the specified
        // 'original' object.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    //! ~OptionValue() = default;
        // Destroy this object.

    // MANIPULATORS
    OptionValue& operator=(const OptionValue& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    void reset();
        // Reset this object to its default constructed (unset) state.  The
        // existing value, if any, is destroyed.  Note that on return
        // 'OptionType::e_VOID == type()'.

    template <class TYPE>
    void set(const TYPE& value);
        // Set the value of this object to the specified 'value'.  The behavior
        // is undefined unless 'OptionType::TypeToEnum<TYPE>::value == type()'
        // for the (template parameter) 'TYPE' and
        // 'OptionType::e_VOID != type()'.

    void setNull();
        // Set the value of this object, irrespective of that value's type, to
        // its null state.  The behavior is undefined unless
        // 'true == hasNonVoidType()'.  Note that 'type()' is not changed.

    void setType(OptionType::Enum type);
        // Set the type of this object to the specified 'type' and the value to
        // the default value of that type.

    template <class TYPE>
    TYPE& the();
        // Return a reference providing modifiable access to the underlying
        // variant object of this command-line-option value object.  The
        // behavior is undefined unless 'OptionType::e_VOID != type()',
        // 'OptionType::TypeToEnum<TYPE>::value == type()', and
        // 'false == isNull()'.

                                  // Aspects

    void swap(OptionValue& other);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.  This method provides the no-throw
        // exception-safety guarantee if either 'type()' is the same as
        // 'other.type()', or neither 'type()' nor 'other.type()' is a type
        // that requires allocation; otherwise, it provides the basic
        // guarantee.  The behavior is undefined unless this object was created
        // with the same allocator as 'other'.

    // ACCESSORS
    bool hasNonVoidType() const;
        // Return 'true' if this object is in the unset state, and 'false'
        // otherwise.  Note that if 'false == hasNonVoidType()' then
        // 'OptionType::e_VOID == type()'.

    bool isNull() const;
        // Return 'true' if the value of this object (irrespective of
        // non-'void' type) is null.  The behavior is undefined unless
        // 'true == hasNonVoidType()'.

    template <class TYPE>
    const TYPE& the() const;
        // Return a 'const' reference to the value of this command line option.
        // The behavior is undefined unless 'OptionType::e_VOID != type()',
        // 'OptionType::typeToEnum<TYPE>::value == type()', and
        // 'false == isNull()'.

    OptionType::Enum type() const;
        // Return the type of this command-line-option value.  The type
        // 'OptionType::e_VOID' represents the unset state.

                                  // Aspects

    bslma::Allocator *allocator() const;
        // Return the allocator used by this object to supply memory.  Note
        // that if no allocator was supplied at construction the currently
        // installed default allocator is used.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Write the value of this object to the specified output 'stream' in a
        // human-readable format, and return a reference to 'stream'.
        // Optionally specify an initial indentation 'level', whose absolute
        // value is incremented recursively for nested objects.  If 'level' is
        // specified, optionally specify 'spacesPerLevel', whose absolute value
        // indicates the number of spaces per indentation level for this and
        // all of its nested objects.  If 'level' is negative, suppress
        // indentation of the first line.  If 'spacesPerLevel' is negative,
        // format the entire output on one line, suppressing all but the
        // initial indentation (as governed by 'level').  If 'stream' is not
        // valid on entry, this operation has no effect.  Note that the format
        // is not fully specified, and can change without notice.
};

// FREE OPERATORS
bool operator==(const OptionValue& lhs, const OptionValue& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'OptionValue' objects have the same
    // value if they have the same type, and (if the type is not 'e_VOID') the
    // value of that type (as accessed through 'the*' methods) is the same.

bool operator!=(const OptionValue& lhs, const OptionValue& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'OptionValue' objects do not
    // have the same value if their type is not the same, or (if their type is
    // not 'e_VOID') the value of that type (as accessed through 'the*'
    // methods) is not the same.

bsl::ostream& operator<<(bsl::ostream& stream, const OptionValue& object);
    // Write the value of the specified 'object' to the specified output
    // 'stream' in a single-line format, and return a reference to 'stream'.
    // If 'stream' is not valid on entry, this operation has no effect.  Note
    // that this human-readable format is not fully specified, can change
    // without notice, and is logically equivalent to:
    //..
    //  print(stream, 0, -1);
    //..

// FREE FUNCTIONS
void swap(OptionValue& a, OptionValue& b);
    // Swap the value of the specified 'a' object with the value of the
    // specified 'b' object.  This method provides the no-throw
    // exception-safety guarantee if either 'a.type()' is the same as
    // 'b.type()' and 'a' and 'b' were created with the same allocator, or
    // neither 'a.type()' nor 'b.type()' is a type that requires allocation;
    // otherwise, it provides the basic guarantee.

                        // ===============================
                        // class OptionValue_IsNullVisitor
                        // ===============================

class OptionValue_IsNullVisitor {
    // This class defines a functor that is compatible with the 'applyRaw'
    // method of the 'bdlb::Variant' class.

  public:
    // TYPES
    typedef bool ResultType;

    // ACCESSORS
    template <class TYPE>
    ResultType operator()(const TYPE& value) const;
        // Return 'true' if the specified 'value' is in the null state (no
        // specified value), and 'false' otherwise.
};

                        // ================================
                        // class OptionValue_SetNullVisitor
                        // ================================

class OptionValue_SetNullVisitor {
    // This class defines a functor that is compatible with the 'applyRaw'
    // method of the 'bdlb::Variant' class.

  public:
    // ACCESSORS
    template <class TYPE>
    void operator()(TYPE& value) const;
        // Set the specified 'value' to its null state.

    void operator()(bslmf::Nil value) const;
        // The behavior is undefined if the specified 'value' is of a
        // non-existing type (represented by 'bslmf::Nil') )
};

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                        // -----------------
                        // class OptionValue
                        // -----------------

// CREATORS
inline
OptionValue::OptionValue()
: d_value()
{
}

inline
OptionValue::OptionValue(bslma::Allocator *basicAllocator)
: d_value(basicAllocator)
{
}

inline
OptionValue::OptionValue(OptionType::Enum  type,
                         bslma::Allocator *basicAllocator)
: d_value(basicAllocator)
{
    init(type);
}

inline
OptionValue::OptionValue(bool              value,
                         bslma::Allocator *basicAllocator)
: d_value(bdlb::NullableValue<bool>(value), basicAllocator)
{
}

inline
OptionValue::OptionValue(char              value,
                         bslma::Allocator *basicAllocator)
: d_value(bdlb::NullableValue<char>(value), basicAllocator)
{
}

inline
OptionValue::OptionValue(int               value,
                         bslma::Allocator *basicAllocator)
: d_value(bdlb::NullableValue<int>(value), basicAllocator)
{
}

inline
OptionValue::OptionValue(bsls::Types::Int64  value,
                         bslma::Allocator   *basicAllocator)
: d_value(bdlb::NullableValue<bsls::Types::Int64>(value), basicAllocator)
{
}

inline
OptionValue::OptionValue(double            value,
                         bslma::Allocator *basicAllocator)
: d_value(bdlb::NullableValue<double>(value), basicAllocator)
{
}

inline
OptionValue::OptionValue(const bsl::string&  value,
                         bslma::Allocator   *basicAllocator)
: d_value(bdlb::NullableValue<bsl::string>(value), basicAllocator)
{
}

inline
OptionValue::OptionValue(bdlt::Datetime    value,
                         bslma::Allocator *basicAllocator)
: d_value(bdlb::NullableValue<bdlt::Datetime>(value), basicAllocator)
{
}

inline
OptionValue::OptionValue(bdlt::Date        value,
                         bslma::Allocator *basicAllocator)
: d_value(bdlb::NullableValue<bdlt::Date>(value), basicAllocator)
{
}

inline
OptionValue::OptionValue(bdlt::Time        value,
                         bslma::Allocator *basicAllocator)
: d_value(bdlb::NullableValue<bdlt::Time>(value), basicAllocator)
{
}

inline
OptionValue::OptionValue(const bsl::vector<char>&  value,
                         bslma::Allocator         *basicAllocator)
: d_value(bdlb::NullableValue<bsl::vector<char> >(value), basicAllocator)
{
}

inline
OptionValue::OptionValue(const bsl::vector<int>&  value,
                         bslma::Allocator        *basicAllocator)
: d_value(bdlb::NullableValue<bsl::vector<int> >(value), basicAllocator)
{
}

inline
OptionValue::OptionValue(
                        const bsl::vector<bsls::Types::Int64>&  value,
                        bslma::Allocator                       *basicAllocator)
: d_value(bdlb::NullableValue<bsl::vector<bsls::Types::Int64> >(value),
          basicAllocator)
{
}

inline
OptionValue::OptionValue(const bsl::vector<double>&  value,
                         bslma::Allocator           *basicAllocator)
: d_value(bdlb::NullableValue<bsl::vector<double> >(value), basicAllocator)
{
}

inline
OptionValue::OptionValue(const bsl::vector<bsl::string>&  value,
                         bslma::Allocator                *basicAllocator)
: d_value(bdlb::NullableValue<bsl::vector<bsl::string> >(value),
          basicAllocator)
{
}

inline
OptionValue::OptionValue(const bsl::vector<bdlt::Datetime>&  value,
                         bslma::Allocator                   *basicAllocator)
: d_value(bdlb::NullableValue<bsl::vector<bdlt::Datetime> >(value),
          basicAllocator)
{
}

inline
OptionValue::OptionValue(const bsl::vector<bdlt::Date>&  value,
                         bslma::Allocator               *basicAllocator)
: d_value(bdlb::NullableValue<bsl::vector<bdlt::Date> >(value), basicAllocator)
{
}

inline
OptionValue::OptionValue(const bsl::vector<bdlt::Time>&  value,
                         bslma::Allocator               *basicAllocator)
: d_value(bdlb::NullableValue<bsl::vector<bdlt::Time> >(value), basicAllocator)
{
}

inline
OptionValue::OptionValue(const OptionValue&  original,
                         bslma::Allocator   *basicAllocator)
: d_value(original.d_value, basicAllocator)
{
}

// MANIPULATORS
inline
OptionValue& OptionValue::operator=(const OptionValue& rhs)
{
    d_value = rhs.d_value;
    return *this;
}

inline
void OptionValue::reset()
{
    d_value.reset();
}

template <class TYPE>
inline
void OptionValue::set(const TYPE& value)
{
    BSLS_ASSERT(d_value.is<bdlb::NullableValue<TYPE> >());

    d_value.the<bdlb::NullableValue<TYPE> >().makeValue(value);
}

inline
void OptionValue::setNull()
{
    BSLS_ASSERT(!d_value.isUnset());

    OptionValue_SetNullVisitor setNullVisitor;

    return d_value.applyRaw(setNullVisitor);

}

inline
void OptionValue::setType(OptionType::Enum type)
{
    d_value.reset();
    init(type);
}

template <class TYPE>
inline
TYPE& OptionValue::the()
{
    BSLS_ASSERT( d_value. is<bdlb::NullableValue<TYPE> >());
    BSLS_ASSERT(!d_value.the<bdlb::NullableValue<TYPE> >().isNull());

    return d_value.the<bdlb::NullableValue<TYPE> >().value();
}

                                  // Aspects

inline
void OptionValue::swap(OptionValue& other)
{
    BSLS_ASSERT(allocator() == other.allocator());

    d_value.swap(other.d_value);
}

// ACCESSORS
inline
bool OptionValue::hasNonVoidType() const
{
    return !d_value.isUnset();
}

inline
bool OptionValue::isNull() const
{
    BSLS_ASSERT(!d_value.isUnset());

    OptionValue_IsNullVisitor isNullVisitor;

    return d_value.applyRaw(isNullVisitor);
}

template <class TYPE>
inline
const TYPE& OptionValue::the() const
{
    BSLS_ASSERT( d_value. is<bdlb::NullableValue<TYPE> >());
    BSLS_ASSERT(!d_value.the<bdlb::NullableValue<TYPE> >().isNull());

    return d_value.the<bdlb::NullableValue<TYPE> >().value();
}

                                  // Aspects

inline
bslma::Allocator *OptionValue::allocator() const
{
    return d_value.getAllocator();
}

}  // close package namespace

// FREE OPERATORS
inline
bool balcl::operator==(const OptionValue& lhs, const OptionValue& rhs)
{
    return lhs.d_value == rhs.d_value;
}

inline
bool balcl::operator!=(const OptionValue& lhs, const OptionValue& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& balcl::operator<<(bsl::ostream&      stream,
                                const OptionValue& object)
{
    return object.print(stream, 0, -1);
}

// FREE FUNCTIONS
inline
void balcl::swap(OptionValue& a, OptionValue& b)
{
    // 'bdlb::Variant' member 'swap' supports differing allocators.

    a.d_value.swap(b.d_value);
}

namespace balcl {

                        // -------------------------------
                        // class OptionValue_IsNullVisitor
                        // -------------------------------

// ACCESSORS
template <class TYPE>
inline
OptionValue_IsNullVisitor::ResultType
OptionValue_IsNullVisitor::operator()(const TYPE& value) const
{
    return value.isNull();
}

                        // --------------------------------
                        // class OptionValue_SetNullVisitor
                        // --------------------------------

// ACCESSORS
template <class TYPE>
inline
void OptionValue_SetNullVisitor::operator()(TYPE& value) const
{
    value.reset();   // Set nullable type to null state.
}

inline
void OptionValue_SetNullVisitor::operator()(bslmf::Nil) const
{
    BSLS_ASSERT_OPT(!"Reached");
}

}  // close package namespace
}  // close enterprise namespace

#endif

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
