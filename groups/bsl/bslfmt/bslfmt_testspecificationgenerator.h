// bslfmt_testspecificationgenerator.h                                -*-C++-*-

#ifndef INCLUDED_BSLFMT_TESTSPECIFICATIONGENERATOR
#define INCLUDED_BSLFMT_TESTSPECIFICATIONGENERATOR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a generator for test format specifications
//
//@CLASSES:
//  bslfmt::TestSpecificationGenerator: format specification generator
//
//@DESCRIPTION: This component provides a class that generates test
// specifications containing all possible combinations of standard options to
// format according to the given instructions.  The generator is a state
// machine, each state of which corresponds to a separate specification.
//
// The set of options can be configured using a special language:
//
///Language Specification
///----------------------
// 'V' - flag indicating if argument id must be present in format
//       specification.  The generator substitutes only the preset zero index
//       (i.e. `0:`).
// 'F' - command indicating if fill characters must be added to the produced
//       specifications.  By default two preset characters are used: '*' and
//       '=', but the user might add additional characters using
//       `addFillCharacter` method.
// '^' - command indicating if alignment option must be added to the generated
//       specifications. Three standard options are iterated through in
//       sequence: '<', '^' and '>'.
// '+' - command indicating if sign option must be added to the generated
//       specifications. Three standard options are iterated through in
//       sequence: '+', '-' and ' '.
// '#' - flag indicating if alternate form option must be present in the
//       generated specifications.
// '0' - flag indicating if zero option must be present in the generated
//       specifications.
// 'W' - command indicating if width option must be added to the produced
//       specifications.  By default the following values are iterated through:
//       1, 5, 6 and 10,  but the user might add additional values using
//       `addWidth` method.  This command does not add nested width values.
// 'P' - command indicating if precision option must be added to the produced
//       specifications.  By default the following values are iterated through:
//       0, 5, 6 and 10,  but the user might add additional values using
//       `addPrecision` method.  This command does not add nested precision
//       values.
// '{' - command indicating if nested width option must be added to the
//       produced specifications.  By default the following indexes are
//       iterated through: empty, 1, and 2 (i.e. `{}`, `{1}` and `{2}`.  Note
//       that due to implementation specifics this command automatically adds
//       non-nested width values (i.e. the default full set looks like ``(no
//       option), `{}`, `{1}`, `{2}`, `1`, `5`, `6` and `10`.
// '}' - command indicating if nested precision option must be added to the
//       produced specifications.  By default the following indexes are
//       iterated through: empty, 1, and 2 (i.e. `.{}`, `.{1}` and `.{2}`.
//       Note that due to implementation specifics this command automatically
//       adds non-nested precision values (i.e. the default full set looks like
//       ``(no option), `.{}`, `.{1}`, `.{2}`, `.0`, `.5`, `.6` and `.10`.
// 'L' - flag indicating if locale option must be present in the generated
//       specifications.
// 's' - command indicating if string presentation type (i.e. `s`) must be
//       present in the generated specifications.
// 'i' - command indicating if integer presentation types must be present in
//       the generated specifications. The  following standard options are
//       iterated through in sequence: `b`, `B`, `c`, `d`, `o`, `x` and `X`.
// 'c' - command indicating if character presentation types must be present in
//       the generated specifications. The  following standard options are
//       iterated through in sequence: `b`, `B`, `c`, `d`, `o`, `x` and `X`.
// 'b' - command indicating if boolean presentation types must be present in
//       the generated specifications. The  following standard options are
//       iterated through in sequence: `s`, `b`, `B`, `c`, `d`, `o`, `x` and
//       `X`.
// 'f' - command indicating if floating-point presentation types must be
//       present in the generated specifications. The  following standard
//       options are iterated through in sequence: `a`, `A`, `e`, `E`, `f`,
//       `F`, `g` and `G`.
// 'p' - command indicating if pointer presentation types must be present in
//       the generated specifications. The  following standard options are
//       iterated through in sequence: `p` and `P`.
// 'a' - command indicating if all presentation types must be present in the
//       generated specifications. The  following standard options are iterated
//       through in sequence: `s`, `b`, `B`, `c`, `d`, `o`, `x`, `X`, `a`, `A`,
//       `e`, `E`, `f`, `F`, `g`, `G`, `p` and `P`.
//
// The default generator instruction is "F^+#0WP{}L".  Note that adding a
// particular command to an instruction does not mean that the corresponding
// option will be present in every generated specification; a specification
// with the missing option will also be generated.
//
// The generator produces both valid and invalid strings from the point of view
// of the standard, which also allows thorough testing of the processing of
// incorrect input data. But at the same time, it remains possible to iterate
// only through valid specifications using 'nextValidForParse` or
// 'nextValidForFormat` methods.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example: Basic Usage of Generator
///- - - - - - - - - - - - - - - - -
// The generator is a state machine, each state of which generates a unique
// format specification.  Depending on the set of instructions, the number of
// possible states also changes.  For simplicity, we will take one parameter,
// the sign, that takes three possible values: `+`, `-` and ` `. Accordingly,
// we expect that the generator will go through four states, because the
// default state always comes first, generating an empty specification.
// First, let's create the generator and configure it:
// ```
//  typedef bslfmt::TestSpecificationGenerator<char> Generator;
//
//  const Generator::Sign POSITIVE = Generator::e_SIGN_POSITIVE;
//  const Generator::Sign NEGATIVE = Generator::e_SIGN_NEGATIVE;
//  const Generator::Sign SPACE    = Generator::e_SIGN_SPACE;
//
//  bslfmt::TestSpecificationGenerator<char> generator;
// ```
// Note that the instructions are given to the generator in a special language,
// a description of which can be found in the component documentation:
// ```
//  generator.setup("+");
// ```
// You can get information about the current state of the generator using
// multiple accessors.  Since we requested only one option, most of these
// accessors return false when asked about the presence of a particular option.
// Note that a request to get the value of an option, if this option is not
// present in the current state, is undefined behavior:
// ```
//  assert(true    == generator.isStateValidForParse());
//  assert(true    == generator.isStateValidForFormat());
//
//  assert(false   == generator.isValueIdOptionPresent());
//  assert(false   == generator.isFillCharacterPresent());
//  assert(false   == generator.isAlignOptionPresent());
//  assert(false   == generator.isSignOptionPresent());
//  assert(false   == generator.isHashOptionPresent());
//  assert(false   == generator.isZeroOptionPresent());
//  assert(false   == generator.isWidthOptionPresent());
//  assert(false   == generator.isNestedWidthPresent());
//  assert(false   == generator.isPrecisionOptionPresent());
//  assert(false   == generator.isNestedPrecisionPresent());
//  assert(false   == generator.isLocaleOptionPresent());
//  assert(false   == generator.isTypeOptionPresent());
// ```
// As we have already said, the initial state of the generator generates an
// empty string.  Therefore, even though we added the sign to the instruction,
// this option is not present in this state.  Keep in mind that the absence of
// an option is always another variant when iterating through the states of the
// generator.
// For each state, the generator creates two strings: one that the
// `bsl::format` passes to the formatter's `parse` function, and one that can
// be passed to the `bsl::format` function itself:
// ```
//  assert(""      == generator.spec());
//  assert("{:}"   == generator.formatSpec());
// ```
// Then, let's move on to the next state of the generator. When the generator
// has iterated over all possible states, the function `next` will return
// `false`, but for now we expect it to return `true`:
// ```
//  assert(true == generator.next());
// ```
// For ease of reading, we will not check all the options each time, but only
// those that are significant for our example:
//  assert(true     == generator.isStateValidForParse());
//  assert(true     == generator.isStateValidForFormat());
//  assert(true     == generator.isSignOptionPresent());
//  assert(POSITIVE == generator.sign());
//  assert("+"      == generator.spec());
//  assert("{:+}"   == generator.formatSpec());
// ```
// The function `next` iterates over all possible states of the generator,
// including those that generate strings, the processing of which by the
// corresponding functions will lead to an exception being thrown. Such states
// can be determined using the functions `isStateValidForParse` and
// `isStateValidForFormat`.  Another option can be to skip invalid states using
// the functions `nextValidForParse` and `nextValidForFormat`, respectively:
// ```
//  assert(true     == generator.nextValidForParse());
//  assert(true     == generator.isStateValidForParse());
//  assert(true     == generator.isStateValidForFormat());
//  assert(true     == generator.isSignOptionPresent());
//  assert(NEGATIVE == generator.sign());
//  assert("-"      == generator.spec());
//  assert("{:-}"   == generator.formatSpec());
//
//  assert(true     == generator.nextValidForFormat());
//  assert(true     == generator.isStateValidForParse());
//  assert(true     == generator.isStateValidForFormat());
//  assert(true     == generator.isSignOptionPresent());
//  assert(SPACE    == generator.sign());
//  assert(" "      == generator.spec());
//  assert("{: }"   == generator.formatSpec());
// ```
// Now, the generator is in its last state, so the `next` function should
// return` false`:
// ```
//  assert(false == generator.next());
// ```
// Finally, let's make sure that the generator is back in its initial state and
// ready to go for the second round:
// ```
//  assert(""    == generator.spec());
//  assert("{:}" == generator.formatSpec());
//  assert(true  == generator.next());
// ```

#include <bslscm_version.h>

#include <bslalg_numericformatterutil.h>

#include <bslma_managedptr.h>

#include <bsls_assert.h>

#include <bslstl_string.h>
#include <bslstl_vector.h>

namespace BloombergLP {
namespace bslfmt {


                 // =======================================
                 // class TestSpecificationGenerator_Option
                 // =======================================

/// This class is a base class for `TestSpecificationGenerator` option nodes,
/// which, when compiled into a list, represent the state of the generator.
/// The payload and the process of changing this state are programmed into
/// specific implementations of this interface.  This class contains the basic
/// functions of adding a new link to the options chain and finding the last
/// link in the chain.
class TestSpecificationGenerator_Option {
    // DATA
    bslma::ManagedPtr<TestSpecificationGenerator_Option> d_nextOption;
                                                             // pointer to the
                                                             // next option in
                                                             // the list

  public:
    // CREATORS

    /// Create an object, having no successor in the list of generator options.
    TestSpecificationGenerator_Option()
    : d_nextOption(0)
    {}

    /// Destroy this object.
    virtual ~TestSpecificationGenerator_Option()
    {}

    // MANIPULATORS

    /// Switch the state of this object to the next one.
    virtual bool nextState() = 0;

    /// Set the specified `nextOption` as a successor in the list of generator
    /// options.  Use the specified `allocator` to supply memory.
    void setNextOption(TestSpecificationGenerator_Option *nextOption,
                       bslma::Allocator                  *allocator)
    {
        if (d_nextOption) {
            d_nextOption->setNextOption(nextOption, allocator);
        }
        else {
            d_nextOption.load(nextOption, allocator);
        }
    }

    /// Return the pointer to the last item in the list of generator options
    /// (this object can also be one).
    TestSpecificationGenerator_Option *lastOption()
    {
        if (d_nextOption) {
            return d_nextOption->lastOption();                        // RETURN
        }
        else {
            return this;
        }
    }

    // ACCESSORS

    /// Return the pointer to the next item in the list of generator options
    /// if such an object exists and null otherwise..
    TestSpecificationGenerator_Option *nextOption() const
    {
        return d_nextOption.get();
    }
};

             // ===========================================
             // class TestSpecificationGenerator_BoolOption
             // ===========================================

/// This class is an implementation of the `TestSpecificationGenerator_Option`
/// for options that have only two states: they are either present or not.
class TestSpecificationGenerator_BoolOption
: public TestSpecificationGenerator_Option {
    // DATA
    bool *d_isPresent_p;  // current state (held, not owned)

  public:
    // CREATORS

    /// Create an object that displays its state through the specified `value`.
    TestSpecificationGenerator_BoolOption(bool *value)
    : d_isPresent_p(value)
    {}

    /// Destroy this object.
    ~TestSpecificationGenerator_BoolOption()
    {}

    // MANIPULATORS

    /// Switch the state of this object to the next one.
    bool nextState()
    {
        if (!nextOption() || !nextOption()->nextState()) {
            *d_isPresent_p = !(*d_isPresent_p);
            return *d_isPresent_p;
        }
        return true;
    }
};

               // ============================================
               // class TestSpecificationGenerator_TypedOption
               // ============================================

/// This class is an implementation of the `TestSpecificationGenerator_Option`
/// for options that iterate through the states specified by the set of
/// `t_TYPE` (template parameter) values.  The state is determined by the
/// interaction of two class fields: a flag indicating the presence of the
/// option and a specific value for the option.  If the flag indicates that the
/// option is not present, the value of the second field is ignored.
template <class t_TYPE>
class TestSpecificationGenerator_TypedOption
: public TestSpecificationGenerator_Option {
    // DATA
    bool                *d_isPresent_p;  // flag indicating if this option is
                                         // present in the current state of the
                                         // generator (held, not owned)

    t_TYPE              *d_value_p;      // current state (held, not owned)

    size_t               d_index;        // index of the current state

    bsl::vector<t_TYPE>  d_values;       // set of available states

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(TestSpecificationGenerator_TypedOption,
                                   bslma::UsesBslmaAllocator);

    // CREATORS

    /// Create an object that iterates through the specified set of possible
    /// `values` and displays its state through the specified
    /// `isPresentVariable` and `valueVariable` accordingly.  Optionally
    /// specified `basicAllocator` is used to supply memory.
    TestSpecificationGenerator_TypedOption(
                                bool                       *isPresentVariable,
                                t_TYPE                     *valueVariable,
                                const bsl::vector<t_TYPE>&  values,
                                bslma::Allocator           *basicAllocator = 0)
    : d_isPresent_p(isPresentVariable)
    , d_value_p(valueVariable)
    , d_index(0)
    , d_values(values, bslma::Default::allocator(basicAllocator))
    {
        BSLS_ASSERT(!d_values.empty());
        *d_value_p = d_values[d_index];
    }

    /// Destroy this object.
    ~TestSpecificationGenerator_TypedOption()
    {}

    // MANIPULATORS

    /// Switch the state of this object to the next one.
    bool nextState()
    {
        if (!nextOption() || !nextOption()->nextState()) {
            // There is no subsequent node, or all subsequent nodes finished
            // their cycles.  This node needs to change state.

            if (*d_isPresent_p && d_values.size() - 1 == d_index) {
                // Current state is the latest in the cycle.  Resetting to
                // unrepresented state.
                *d_isPresent_p = false;
                d_index = 0;
                *d_value_p = d_values[d_index];
                return false;                                         // RETURN
            }
            else if (!*d_isPresent_p) {
                // Node is in unrepresented state.  Just switch to the
                // represented state.
                *d_isPresent_p = true;
            }
            else {
                // Node is present and there are values to iterate through in
                // this cycle.
                ++d_index;
                *d_value_p = d_values[d_index];
            }
        }

        // Some of the subsequent nodes changed its state, so this one doesn't
        // need to yet.  Just notify generator.
        return true;
    }
};

                   // ================================
                   // class TestSpecificationGenerator
                   // ================================

/// This class is a generator that provides a set of formatting specifications
/// obtained by exhaustive search of all combinations of standard options.  Its
/// use allows easy and thorough testing of formatters for standard types.  The
/// generator is a state machine, each state of which corresponds to a separate
/// specification.
template <class t_CHAR>
class TestSpecificationGenerator {
  public:
    // PUBLIC TYPES

    enum Alignment {
        e_ALIGN_DEFAULT,
        e_ALIGN_LEFT,
        e_ALIGN_MIDDLE,
        e_ALIGN_RIGHT
    };

    enum Sign {
        e_SIGN_DEFAULT,
        e_SIGN_POSITIVE,
        e_SIGN_NEGATIVE,
        e_SIGN_SPACE
    };

    enum NestedVariant {
        e_NESTED_DEFAULT,
        e_NESTED_ARG_1,
        e_NESTED_ARG_2
    };

    enum FormatType {
        e_TYPE_STRING,         // 's'
        e_TYPE_ESCAPED,        // '?'
        e_TYPE_BINARY,         // `b`
        e_TYPE_BINARY_UC,      // `B`
        e_TYPE_CHARACTER,      // `c`
        e_TYPE_DECIMAL,        // `d`
        e_TYPE_OCTAL,          // `o`
        e_TYPE_INT_HEX,        // `x`
        e_TYPE_INT_HEX_UC,     // `X`
        e_TYPE_FLOAT_HEX,      // `a`
        e_TYPE_FLOAT_HEX_UC,   // `A`
        e_TYPE_SCIENTIFIC,     // `e`
        e_TYPE_SCIENTIFIC_UC,  // `E`
        e_TYPE_FIXED,          // `f`
        e_TYPE_FIXED_UC,       // `F`
        e_TYPE_GENERAL,        // `g`
        e_TYPE_GENERAL_UC,     // `G`
        e_TYPE_POINTER,        // 'p'
        e_TYPE_POINTER_UC      // 'P'
    };

  private:
    // TYPES
    typedef TestSpecificationGenerator_Option Option;

    // DATA
    bslma::ManagedPtr<Option>  d_firstOption;
                                   // first node in the option list (if any)

    bool                       d_valueIdOptionIsPresent;
                                   // flag indicating that value argument id is
                                   // present in the current specification

    bool                       d_fillCharacterIsPresent;
                                   // flag indicating that fill character is
                                   // present in the current specification

    char                       d_fillCharacter;
                                   // fill character value

    bool                       d_alignOptionIsPresent;
                                   // flag indicating that align option is
                                   // present in the current specification

    Alignment                  d_alignment;
                                   // alignment option value

    bool                       d_signOptionIsPresent;
                                   // flag indicating that sign option is
                                   // present in the current specification

    Sign                       d_sign;
                                   // sign option value

    bool                       d_hashOptionIsPresent;
                                   // flag indicating that alternative form
                                   // option is present in the current
                                   // specification

    bool                       d_zeroOptionIsPresent;
                                   // flag indicating that zero option is
                                   // present in the current specification

    bool                       d_widthOptionIsPresent;
                                   // flag indicating that width option is
                                   // present in the current specification

    int                        d_width;
                                   // width value

    bool                       d_nestedWidthIsPresent;
                                   // flag indicating that nested width option
                                   // is present in the current specification

    NestedVariant              d_nestedWidthVariant;
                                   // nested width option value

    bool                       d_precisionOptionIsPresent;
                                   // flag indicating that precision option is
                                   // present in the current specification

    int                        d_precision;
                                   // precision value

    bool                       d_nestedPrecisionIsPresent;
                                   // flag indicating that nested precision
                                   // option is present in the current
                                   // specification

    NestedVariant              d_nestedPrecisionVariant;
                                   // nested precision option value

    bool                       d_localeOptionIsPresent;
                                   // flag indicating that locale option is
                                   // present in the current specification

    bool                       d_typeOptionIsPresent;
                                   // flag indicating that presentation type
                                   // option is present in the current
                                   // specification

    FormatType                 d_type;
                                   // presentation type option value

    bsl::basic_string<t_CHAR>  d_spec;
                                   // generated specification suitable for
                                   // formatters `parse` method

    bsl::basic_string<t_CHAR>  d_formatSpec;
                                   // generated specification suitable for
                                   // formatters `format` method

    bsl::vector<char>          d_fillCharacters;
                                   // fill characters set

    bsl::vector<int>           d_widths;
                                   // width values set

    bsl::vector<int>           d_precisions;
                                   // precision values set

    bslma::Allocator          *d_allocator_p;
                                  // allocator used to supply memory (held, not
                                  // own)
  public:

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(TestSpecificationGenerator,
                                   bslma::UsesBslmaAllocator);

  private:
    // PRIVATE MANIPULATORS

    /// Add the specified `newOption` to the option list.
    void addOption(Option *newOption);

    /// Generate a specification based on the current state of the object.
    void generateSpecification();

    // PRIVATE ACCESSORS

    /// Return `true` if the current state of the object assumes an automatic
    /// mode of indexing parameters, and `false` otherwise.
    bool isAutoIndexingMode() const;

    /// Return `true` if the current state of the object assumes a manual mode
    /// of indexing parameters, and `false` otherwise.
    bool isManualIndexingMode() const;

  public:
    // CREATORS

    /// Create a generator object.  Optionally specify the `basicAllocator` to
    /// supply memory.
    TestSpecificationGenerator(bslma::Allocator *basicAllocator = 0);

    // MANIPULATORS

    /// Add the specified `character` to the set of fill characters.  Note that
    /// generated instructions will contain the `character` only if this
    /// function is called before the `setup` method invocation.
    void addFillCharacter(char character);

    /// Add the specified `width` to the set of width values. Note that
    /// generated instructions will contain the `width` only if this function
    /// is called before the `setup` method invocation.
    void addWidth(int width);

    /// Add the specified `precision` to the set of precision values. Note that
    /// generated instructions will contain the `precision` only if this
    /// function is called before the `setup` method invocation.
    void addPrecision(int precision);

    /// Switch this object to the next state and generate specification based
    /// on the new state.
    bool next();

    /// Switch this object to the next state appropriate for generating a
    /// specification valid for formatters `parse` method and generate
    /// specification based on the new state.
    bool nextValidForParse();

    /// Switch this object to the next state appropriate for generating a
    /// specification valid for formatters `format` method and generate
    /// specification based on the new state.
    bool nextValidForFormat();

    /// Setup the sequence of this object's states based on the optionally
    /// specified `instruction`.
    void setup(const char *instruction = 0);

    // ACCESSORS

    /// Return the specification for the formatters `parse` method based on the
    /// current state of this object.
    const bsl::basic_string<t_CHAR>& spec() const;

    /// Return the specification for the formatters `format` method based on
    /// the current state of this object.
    const bsl::basic_string<t_CHAR>& formatSpec() const;

    /// Return `true` if the generated specification based on the current state
    /// of this object is valid for the formatters `parse` method, and `false`
    /// otherwise.
    bool isStateValidForParse() const;

    /// Return `true` if the generated specification based on the current state
    /// of this object is valid for the formatters `format` method, and `false`
    /// otherwise.
    bool isStateValidForFormat() const;

    /// Return `true` if the value id option is present in the current state of
    /// this object, and `false` otherwise.
    bool isValueIdOptionPresent() const;

    /// Return `true` if the value id option is present in the current state of
    /// this object, and `false` otherwise.
    bool isFillCharacterPresent() const;

    /// Return the current fill character.
    char fillCharacter() const;

    /// Return `true` if the align option is present in the current state of
    /// this object, and `false` otherwise.
    bool isAlignOptionPresent() const;

    /// Return the current alignment type.
    Alignment alignment() const;

    /// Return `true` if the sign option is present in the current state of
    /// this object, and `false` otherwise.
    bool isSignOptionPresent() const;

    /// Return the current sign option value.
    Sign sign() const;

    /// Return `true` if the alternative form option is present in the current
    /// state of this object, and `false` otherwise.
    bool isHashOptionPresent() const;

    /// Return `true` if the zero option is present in the current state of
    /// this object, and `false` otherwise.
    bool isZeroOptionPresent() const;

    /// Return `true` if the width option is present in the current state of
    /// this object, and `false` otherwise.
    bool isWidthOptionPresent() const;

    /// Return the current width value.
    int width() const;

    /// Return `true` if the nested width option is present in the current
    /// state of this object, and `false` otherwise.
    bool isNestedWidthPresent() const;

    /// Return the current nested width variant.
    NestedVariant nestedWidthVariant() const;

    /// Return `true` if the precision option is present in the current state
    /// of this object, and `false` otherwise.
    bool isPrecisionOptionPresent() const;

    /// Return the current precision value.
    int precision() const;

    /// Return `true` if the nested precision option is present in the current
    /// state of this object, and `false` otherwise.
    bool isNestedPrecisionPresent() const;

    /// Return the current nested precision variant.
    NestedVariant nestedPrecisionVariant() const;

    /// Return `true` if the locale option is present in the current state of
    /// this object, and `false` otherwise.
    bool isLocaleOptionPresent() const;

    /// Return `true` if the presentation type option is present in the current
    /// state of this object, and `false` otherwise.
    bool isTypeOptionPresent() const;

    /// Return the current presentation type value.
    FormatType type() const;
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                   // --------------------------------
                   // class TestSpecificationGenerator
                   // --------------------------------

// PRIVATE MANIPULATORS
template <class t_CHAR>
void TestSpecificationGenerator<t_CHAR>::addOption(Option *newOption)
{
    if (d_firstOption) {
        d_firstOption->setNextOption(newOption, d_allocator_p);
    }
    else {
        d_firstOption.load(newOption, d_allocator_p);
    }
}

template <class t_CHAR>
void TestSpecificationGenerator<t_CHAR>::generateSpecification()
{
    typedef bslalg::NumericFormatterUtil NFUtil;
    const int maxIntValueLength = NFUtil::ToCharsMaxLength<int, 10>::k_VALUE;

    d_spec.clear();
    d_formatSpec.clear();

    if (d_fillCharacterIsPresent) {
        d_spec.push_back(d_fillCharacter);
    }

    if (d_alignOptionIsPresent) {
        switch (d_alignment) {
          case e_ALIGN_LEFT: {
            d_spec.push_back('<');
          } break;
          case e_ALIGN_MIDDLE: {
            d_spec.push_back('^');
          } break;
          case e_ALIGN_RIGHT: {
            d_spec.push_back('>');
          } break;
          default: {
            BSLS_ASSERT_INVOKE("Unexpected alignment option");
          }
        }
    }

    if (d_signOptionIsPresent) {
        switch (d_sign) {
          case e_SIGN_POSITIVE: {
            d_spec.push_back('+');
          } break;
          case e_SIGN_NEGATIVE: {
            d_spec.push_back('-');
          } break;
          case e_SIGN_SPACE: {
            d_spec.push_back(' ');
          } break;
          default: {
            BSLS_ASSERT_INVOKE("Unexpected sign option");
          }
        }
    }

    if (d_hashOptionIsPresent) {
        d_spec.push_back('#');
    }

    if (d_zeroOptionIsPresent) {
        d_spec.push_back('0');
    }

    if (d_widthOptionIsPresent && !d_nestedWidthIsPresent) {
        char  valueBuf[maxIntValueLength];
        char *valueEnd = NFUtil::toChars(valueBuf,
                                         valueBuf + maxIntValueLength,
                                         d_width);
        d_spec.insert(d_spec.end(), valueBuf, valueEnd);
    }
    else if (d_nestedWidthIsPresent) {
        switch (d_nestedWidthVariant) {
          case e_NESTED_DEFAULT: {
            d_spec.push_back('{');
            d_spec.push_back('}');
          } break;
          case e_NESTED_ARG_1: {
            d_spec.push_back('{');
            d_spec.push_back('1');
            d_spec.push_back('}');
          } break;
          case e_NESTED_ARG_2: {
            d_spec.push_back('{');
            d_spec.push_back('2');
            d_spec.push_back('}');
          } break;
        }
    }

    if (d_precisionOptionIsPresent && !d_nestedPrecisionIsPresent) {
        d_spec.push_back('.');
        char  valueBuf[maxIntValueLength];
        char *valueEnd = NFUtil::toChars(valueBuf,
                                         valueBuf + maxIntValueLength,
                                         d_precision);
        d_spec.insert(d_spec.end(), valueBuf, valueEnd);
    }
    else if (d_nestedPrecisionIsPresent) {
        d_spec.push_back('.');
        switch (d_nestedPrecisionVariant) {
          case e_NESTED_DEFAULT: {
            d_spec.push_back('{');
            d_spec.push_back('}');
          } break;
          case e_NESTED_ARG_1: {
            d_spec.push_back('{');
            d_spec.push_back('1');
            d_spec.push_back('}');
          } break;
          case e_NESTED_ARG_2: {
            d_spec.push_back('{');
            d_spec.push_back('2');
            d_spec.push_back('}');
          } break;
        }
    }

    if (d_localeOptionIsPresent) {
        d_spec.push_back('L');
    }
    if (d_typeOptionIsPresent) {
        switch (d_type) {
          case e_TYPE_STRING: {
            d_spec.push_back('s');
          } break;
          case e_TYPE_ESCAPED: {
            d_spec.push_back('?');
          } break;
          case e_TYPE_BINARY: {
            d_spec.push_back('b');
          } break;
          case e_TYPE_BINARY_UC: {
            d_spec.push_back('B');
          } break;
          case e_TYPE_CHARACTER: {
            d_spec.push_back('c');
          } break;
          case e_TYPE_DECIMAL: {
            d_spec.push_back('d');
          } break;
          case e_TYPE_OCTAL: {
            d_spec.push_back('o');
          } break;
          case e_TYPE_INT_HEX: {
            d_spec.push_back('x');
          } break;
          case e_TYPE_INT_HEX_UC: {
            d_spec.push_back('X');
          } break;
          case e_TYPE_FLOAT_HEX: {
            d_spec.push_back('a');
          } break;
          case e_TYPE_FLOAT_HEX_UC: {
            d_spec.push_back('A');
          } break;
          case e_TYPE_SCIENTIFIC: {
            d_spec.push_back('e');
          } break;
          case e_TYPE_SCIENTIFIC_UC: {
            d_spec.push_back('E');
          } break;
          case e_TYPE_FIXED: {
            d_spec.push_back('f');
          } break;
          case e_TYPE_FIXED_UC: {
            d_spec.push_back('F');
          } break;
          case e_TYPE_GENERAL: {
            d_spec.push_back('g');
          } break;
          case e_TYPE_GENERAL_UC: {
            d_spec.push_back('G');
          } break;
          case e_TYPE_POINTER: {
            d_spec.push_back('p');
          } break;
          case e_TYPE_POINTER_UC: {
            d_spec.push_back('P');
          } break;
          default: {
            BSLS_ASSERT_INVOKE("Unexpected type option");
          }
        }
    }

    // Assembling format specification
    if (d_valueIdOptionIsPresent) {
        d_formatSpec.push_back('{');
        d_formatSpec.push_back('0');
        d_formatSpec.push_back(':');
    }
    else {
        d_formatSpec.push_back('{');
        d_formatSpec.push_back(':');
    }
    d_formatSpec.append(d_spec);
    d_formatSpec.push_back('}');
}

// PRIVATE ACCESSORS
template <class t_CHAR>
bool TestSpecificationGenerator<t_CHAR>::isAutoIndexingMode() const
{
    bool result = true;
    if (d_nestedWidthIsPresent && e_NESTED_DEFAULT != d_nestedWidthVariant) {
        result = false;
    }
    else if (d_nestedPrecisionIsPresent && e_NESTED_DEFAULT !=
                                               d_nestedPrecisionVariant) {
        result = false;
    }

    return result;
}

template <class t_CHAR>
bool TestSpecificationGenerator<t_CHAR>::isManualIndexingMode() const
{
    bool result = false;
    if (d_nestedWidthIsPresent) {
        if (e_NESTED_DEFAULT != d_nestedWidthVariant) {
            if (d_nestedPrecisionIsPresent) {
                if (e_NESTED_DEFAULT != d_nestedPrecisionVariant) {
                    result = true;
                }
            }
            else {
                result = true;
            }
        }
    }
    else if (d_nestedPrecisionIsPresent && e_NESTED_DEFAULT !=
                                               d_nestedPrecisionVariant) {
        result = true;
    }
    return result;
}

// CREATORS
template <class t_CHAR>
TestSpecificationGenerator<t_CHAR>::TestSpecificationGenerator(
                                              bslma::Allocator *basicAllocator)
: d_valueIdOptionIsPresent(false)
, d_fillCharacterIsPresent(false)
, d_alignOptionIsPresent(false)
, d_signOptionIsPresent(false)
, d_hashOptionIsPresent(false)
, d_zeroOptionIsPresent(false)
, d_widthOptionIsPresent(false)
, d_nestedWidthIsPresent(false)
, d_precisionOptionIsPresent(false)
, d_nestedPrecisionIsPresent(false)
, d_localeOptionIsPresent(false)
, d_typeOptionIsPresent(false)
, d_spec(basicAllocator)
, d_formatSpec(basicAllocator)
, d_fillCharacters(basicAllocator)
, d_widths(basicAllocator)
, d_precisions(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    // Setup default sets of values.
    const char   defaultFillCharacters[] = { '*', '=' };
    const size_t numFillCharacters = sizeof(defaultFillCharacters);
    const int    defaultWidths[]         = { 1, 5, 6, 10 };
    const size_t numWidths               =
                          sizeof(defaultWidths) / sizeof(defaultWidths[0]);
    const int    defaultPrecisions[]     = { 0, 5, 6, 10 };
    const size_t numPrecisions           =
                  sizeof(defaultPrecisions) / sizeof(defaultPrecisions[0]);
    d_fillCharacters.insert(d_fillCharacters.begin(),
                            defaultFillCharacters,
                            defaultFillCharacters + numFillCharacters);
    d_widths.insert(d_widths.begin(),
                    defaultWidths,
                    defaultWidths + numWidths);
    d_precisions.insert(d_precisions.begin(),
                        defaultPrecisions,
                        defaultPrecisions + numPrecisions);
}

// MANIPULATORS
template <class t_CHAR>
void TestSpecificationGenerator<t_CHAR>::addFillCharacter(char character)
{
    d_fillCharacters.push_back(character);
}

template <class t_CHAR>
void TestSpecificationGenerator<t_CHAR>::addWidth(int width)
{
    BSLS_ASSERT(0 < width);
    d_widths.push_back(width);
}

template <class t_CHAR>
void TestSpecificationGenerator<t_CHAR>::addPrecision(int precision)
{
    BSLS_ASSERT(0 <= precision);
    d_precisions.push_back(precision);
}

template <class t_CHAR>
bool TestSpecificationGenerator<t_CHAR>::next()
{
    if (d_firstOption)
    {
        if (d_firstOption->nextState()) {
           generateSpecification();
           return true;                                               // RETURN
        }
    }
    generateSpecification();
    return false;
}

template <class t_CHAR>
bool TestSpecificationGenerator<t_CHAR>::nextValidForParse()
{
    if (d_firstOption)
    {
        bool rv = d_firstOption->nextState();
        while (  rv && !isStateValidForParse()) {
            // Skip invalid specifications
            rv = d_firstOption->nextState();
        }
        generateSpecification();
        return rv;                                                    // RETURN
    }

    generateSpecification();
    return false;
}

template <class t_CHAR>
bool TestSpecificationGenerator<t_CHAR>::nextValidForFormat()
{
    if (d_firstOption)
    {
        bool rv = d_firstOption->nextState();
        while (  rv && !isStateValidForFormat()) {
            // Skip invalid specifications
            rv = d_firstOption->nextState();
        }
        generateSpecification();
        return rv;                                                    // RETURN
    }

    generateSpecification();
    return false;
}

template <class t_CHAR>
void TestSpecificationGenerator<t_CHAR>::setup(const char *instruction)
{
    const char  *defaultInstruction = "F^+#0WP{}L";
    bsl::string  instructionStr     = instruction
                                    ? instruction
                                    : defaultInstruction;
    bool         typeIsSpecified    = false;
    bool         widthIsAdded       = false;
    bool         precisionIsAdded   = false;

    (void) typeIsSpecified;  // suppress compiler warning

    for (bsl::string::const_iterator it = instructionStr.begin();
         it != instructionStr.end();
         ++it) {
        switch (*it) {
          case 'V': {
            addOption(
                    new (*d_allocator_p) TestSpecificationGenerator_BoolOption(
                        &d_valueIdOptionIsPresent));
          } break;
          case 'F': {
            addOption(new (*d_allocator_p)
                          TestSpecificationGenerator_TypedOption<char>(
                                                     &d_fillCharacterIsPresent,
                                                     &d_fillCharacter,
                                                     d_fillCharacters));

          } break;
          case '^': {
            bsl::vector<Alignment> alignVector;
            alignVector.push_back(e_ALIGN_LEFT);
            alignVector.push_back(e_ALIGN_MIDDLE);
            alignVector.push_back(e_ALIGN_RIGHT);

            addOption(new (*d_allocator_p)
                          TestSpecificationGenerator_TypedOption<Alignment>(
                                                       &d_alignOptionIsPresent,
                                                       &d_alignment,
                                                       alignVector));
          } break;
          case '+': {
            bsl::vector<Sign> signVector;
            signVector.push_back(e_SIGN_POSITIVE);
            signVector.push_back(e_SIGN_NEGATIVE);
            signVector.push_back(e_SIGN_SPACE);

            addOption(new (*d_allocator_p)
                          TestSpecificationGenerator_TypedOption<Sign>(
                                                        &d_signOptionIsPresent,
                                                        &d_sign,
                                                        signVector));
          } break;
          case '#': {
            addOption(
                    new (*d_allocator_p) TestSpecificationGenerator_BoolOption(
                        &d_hashOptionIsPresent));
          } break;
          case '0': {
            addOption(
                    new (*d_allocator_p) TestSpecificationGenerator_BoolOption(
                        &d_zeroOptionIsPresent));
          } break;
          case 'W': {
            if (!widthIsAdded) {
                addOption(new (*d_allocator_p)
                              TestSpecificationGenerator_TypedOption<int>(
                                                       &d_widthOptionIsPresent,
                                                       &d_width,
                                                       d_widths));
                widthIsAdded = true;
            }
          } break;
          case 'P': {
            if (!precisionIsAdded) {
                addOption(new (*d_allocator_p)
                              TestSpecificationGenerator_TypedOption<int>(
                                                   &d_precisionOptionIsPresent,
                                                   &d_precision,
                                                   d_precisions));
                precisionIsAdded = true;
            }
          } break;
          case '{': {
            if (!widthIsAdded) {
                addOption(new (*d_allocator_p)
                              TestSpecificationGenerator_TypedOption<int>(
                                                       &d_widthOptionIsPresent,
                                                       &d_width,
                                                       d_widths));
                widthIsAdded = true;
            }

            bsl::vector<NestedVariant> nestedVector;
            nestedVector.push_back(e_NESTED_DEFAULT);
            nestedVector.push_back(e_NESTED_ARG_1);
            nestedVector.push_back(e_NESTED_ARG_2);

            addOption(
                     new (*d_allocator_p)
                         TestSpecificationGenerator_TypedOption<NestedVariant>(
                             &d_nestedWidthIsPresent,
                             &d_nestedWidthVariant,
                             nestedVector));
          } break;
          case '}': {
            if (!precisionIsAdded) {
                addOption(new (*d_allocator_p)
                              TestSpecificationGenerator_TypedOption<int>(
                                                   &d_precisionOptionIsPresent,
                                                   &d_precision,
                                                   d_precisions));
                precisionIsAdded = true;
            }

            bsl::vector<NestedVariant> nestedVector;
            nestedVector.push_back(e_NESTED_DEFAULT);
            nestedVector.push_back(e_NESTED_ARG_1);
            nestedVector.push_back(e_NESTED_ARG_2);

            addOption(
                     new (*d_allocator_p)
                         TestSpecificationGenerator_TypedOption<NestedVariant>(
                             &d_nestedPrecisionIsPresent,
                             &d_nestedPrecisionVariant,
                             nestedVector));
          } break;
          case 'L': {
            addOption(
                    new (*d_allocator_p) TestSpecificationGenerator_BoolOption(
                        &d_localeOptionIsPresent));
          } break;
          case 's': {
            // string presentation types
            BSLS_ASSERT(!typeIsSpecified);
            typeIsSpecified = true;

            bsl::vector<FormatType> typeVector;
            typeVector.push_back(e_TYPE_STRING);
            typeVector.push_back(e_TYPE_ESCAPED);

            addOption(new (*d_allocator_p)
                          TestSpecificationGenerator_TypedOption<FormatType>(
                                                        &d_typeOptionIsPresent,
                                                        &d_type,
                                                        typeVector));
          } break;
          case 'i':{
            // integral and character presentation types
            BSLS_ASSERT(!typeIsSpecified);
            typeIsSpecified = true;

            bsl::vector<FormatType> typeVector;
            typeVector.push_back(e_TYPE_BINARY);      // b
            typeVector.push_back(e_TYPE_BINARY_UC);   // B
            typeVector.push_back(e_TYPE_CHARACTER);   // c
            typeVector.push_back(e_TYPE_DECIMAL);     // d
            typeVector.push_back(e_TYPE_OCTAL);       // o
            typeVector.push_back(e_TYPE_INT_HEX);     // x
            typeVector.push_back(e_TYPE_INT_HEX_UC);  // X

            addOption(new (*d_allocator_p)
                          TestSpecificationGenerator_TypedOption<FormatType>(
                                                        &d_typeOptionIsPresent,
                                                        &d_type,
                                                        typeVector));
          } break;
          case 'c': {
            // integral and character presentation types
            BSLS_ASSERT(!typeIsSpecified);
            typeIsSpecified = true;

            bsl::vector<FormatType> typeVector;
            typeVector.push_back(e_TYPE_BINARY);      // b
            typeVector.push_back(e_TYPE_BINARY_UC);   // B
            typeVector.push_back(e_TYPE_CHARACTER);   // c
            typeVector.push_back(e_TYPE_ESCAPED);     // ?
            typeVector.push_back(e_TYPE_DECIMAL);     // d
            typeVector.push_back(e_TYPE_OCTAL);       // o
            typeVector.push_back(e_TYPE_INT_HEX);     // x
            typeVector.push_back(e_TYPE_INT_HEX_UC);  // X

            addOption(new (*d_allocator_p)
                          TestSpecificationGenerator_TypedOption<FormatType>(
                                                        &d_typeOptionIsPresent,
                                                        &d_type,
                                                        typeVector));
          } break;
          case 'b': {
            // bool presentation types
            BSLS_ASSERT(!typeIsSpecified);
            typeIsSpecified = true;

            bsl::vector<FormatType> typeVector;
            typeVector.push_back(e_TYPE_STRING);      // s
            typeVector.push_back(e_TYPE_BINARY);      // b
            typeVector.push_back(e_TYPE_BINARY_UC);   // B
            typeVector.push_back(e_TYPE_DECIMAL);     // d
            typeVector.push_back(e_TYPE_OCTAL);       // o
            typeVector.push_back(e_TYPE_INT_HEX);     // x
            typeVector.push_back(e_TYPE_INT_HEX_UC);  // X

            addOption(new (*d_allocator_p)
                          TestSpecificationGenerator_TypedOption<FormatType>(
                                                        &d_typeOptionIsPresent,
                                                        &d_type,
                                                        typeVector));
          } break;
          case 'f': {
            // floating-point presentation types
            BSLS_ASSERT(!typeIsSpecified);
            typeIsSpecified = true;

            bsl::vector<FormatType> typeVector;
           typeVector.push_back(e_TYPE_FLOAT_HEX);      // a
           typeVector.push_back(e_TYPE_FLOAT_HEX_UC);   // A
           typeVector.push_back(e_TYPE_SCIENTIFIC);     // e
           typeVector.push_back(e_TYPE_SCIENTIFIC_UC);  // E
           typeVector.push_back(e_TYPE_FIXED);          // f
           typeVector.push_back(e_TYPE_FIXED_UC);       // F
           typeVector.push_back(e_TYPE_GENERAL);        // g
           typeVector.push_back(e_TYPE_GENERAL_UC);     // G

           addOption(new (*d_allocator_p)
                         TestSpecificationGenerator_TypedOption<FormatType>(
                                                        &d_typeOptionIsPresent,
                                                        &d_type,
                                                        typeVector));
          } break;
          case 'p': {
            // pointer presentation types
            BSLS_ASSERT(!typeIsSpecified);
            typeIsSpecified = true;

            bsl::vector<FormatType> typeVector;
            typeVector.push_back(e_TYPE_POINTER);     // 'P'
            typeVector.push_back(e_TYPE_POINTER_UC);  // 'P'

            addOption(new (*d_allocator_p)
                          TestSpecificationGenerator_TypedOption<FormatType>(
                                                        &d_typeOptionIsPresent,
                                                        &d_type,
                                                        typeVector));
          } break;
          case 'a': {
            // all presentation types
            BSLS_ASSERT(!typeIsSpecified);
            typeIsSpecified = true;

            bsl::vector<FormatType> typeVector;
            typeVector.push_back(e_TYPE_STRING);         // 's'
            typeVector.push_back(e_TYPE_BINARY);         // `b`
            typeVector.push_back(e_TYPE_BINARY_UC);      // `B`
            typeVector.push_back(e_TYPE_CHARACTER);      // `c`
            typeVector.push_back(e_TYPE_DECIMAL);        // `d`
            typeVector.push_back(e_TYPE_OCTAL);          // `o`
            typeVector.push_back(e_TYPE_INT_HEX);        // `x`
            typeVector.push_back(e_TYPE_INT_HEX_UC);     // `X`
            typeVector.push_back(e_TYPE_FLOAT_HEX);      // `a`
            typeVector.push_back(e_TYPE_FLOAT_HEX_UC);   // `A`
            typeVector.push_back(e_TYPE_SCIENTIFIC);     // `e`
            typeVector.push_back(e_TYPE_SCIENTIFIC_UC);  // `E`
            typeVector.push_back(e_TYPE_FIXED);          // `f`
            typeVector.push_back(e_TYPE_FIXED_UC);       // `F`
            typeVector.push_back(e_TYPE_GENERAL);        // `g`
            typeVector.push_back(e_TYPE_GENERAL_UC);     // `G`
            typeVector.push_back(e_TYPE_POINTER);        // 'p'
            typeVector.push_back(e_TYPE_POINTER_UC);     // 'p'

            addOption(new (*d_allocator_p)
                          TestSpecificationGenerator_TypedOption<FormatType>(
                                                        &d_typeOptionIsPresent,
                                                        &d_type,
                                                        typeVector));
          } break;
          default: {
            BSLS_ASSERT_INVOKE("Unexpected instruction command");
          }
        }
    }

    generateSpecification();
}

// ACCESSORS
template <class t_CHAR>
const bsl::basic_string<t_CHAR>&
TestSpecificationGenerator<t_CHAR>::spec() const
{
    return d_spec;
}

template <class t_CHAR>
const bsl::basic_string<t_CHAR>&
TestSpecificationGenerator<t_CHAR>::formatSpec() const
{
    return d_formatSpec;
}

template <class t_CHAR>
bool TestSpecificationGenerator<t_CHAR>::isStateValidForParse() const
{
    // Rule: The sign, and # options are only valid when an integer or
    // floating-point presentation type is used.
    bool invalidSignOrHash = d_typeOptionIsPresent &&
                            (d_signOptionIsPresent || d_hashOptionIsPresent) &&
                            (e_TYPE_STRING     == d_type ||
                             e_TYPE_ESCAPED    == d_type ||
                             e_TYPE_CHARACTER  == d_type ||
                             e_TYPE_POINTER    == d_type ||
                             e_TYPE_POINTER_UC == d_type);

    // Rule: The  0 option is only valid when an integer, floating-point or
    // pointer presentation type is used.
    bool invalidZero = d_typeOptionIsPresent && d_zeroOptionIsPresent &&
                       (e_TYPE_STRING    == d_type ||
                        e_TYPE_ESCAPED   == d_type ||
                        e_TYPE_CHARACTER == d_type);

    // Rule: Precision can be used with floating-point and string types
    // only.
    bool invalidTypeForPrecision =
              (d_precisionOptionIsPresent || d_nestedPrecisionIsPresent) &&
              (e_TYPE_BINARY     == d_type ||
               e_TYPE_BINARY_UC  == d_type ||
               e_TYPE_CHARACTER  == d_type ||
               e_TYPE_DECIMAL    == d_type ||
               e_TYPE_OCTAL      == d_type ||
               e_TYPE_INT_HEX    == d_type ||
               e_TYPE_INT_HEX_UC == d_type ||
               e_TYPE_POINTER    == d_type ||
               e_TYPE_POINTER_UC == d_type);

    // Rule: An align option must follow a fill character.
    bool missedAlignOption = d_fillCharacterIsPresent &&
                             !d_alignOptionIsPresent;

    // Rule: All parameter indexes in specification must be defined either
    // automatically or manually.
    bool conflictingIndexingStyle = !isAutoIndexingMode() &&
                                    !isManualIndexingMode();

    return !(invalidSignOrHash ||
             invalidZero ||
             invalidTypeForPrecision ||
             missedAlignOption ||
             conflictingIndexingStyle);
}

template <class t_CHAR>
bool TestSpecificationGenerator<t_CHAR>::isStateValidForFormat() const
{
    // Value's index is defined differently than width's or precision's index.
    bool conflictingIndexingStyle =
                     (d_nestedWidthIsPresent || d_nestedPrecisionIsPresent) &&
                      ((d_valueIdOptionIsPresent && isAutoIndexingMode()) ||
                       (!d_valueIdOptionIsPresent && isManualIndexingMode()));

    return isStateValidForParse() && !conflictingIndexingStyle;
}

template <class t_CHAR>
bool TestSpecificationGenerator<t_CHAR>::isValueIdOptionPresent() const
{
    return d_valueIdOptionIsPresent;
}

template <class t_CHAR>
bool TestSpecificationGenerator<t_CHAR>::isFillCharacterPresent() const
{
    return d_fillCharacterIsPresent;
}

template <class t_CHAR>
char TestSpecificationGenerator<t_CHAR>::fillCharacter() const
{
    BSLS_ASSERT(isFillCharacterPresent());
    return d_fillCharacter;
}

template <class t_CHAR>
bool TestSpecificationGenerator<t_CHAR>::isAlignOptionPresent() const
{
    return d_alignOptionIsPresent;
}

template <class t_CHAR>
typename TestSpecificationGenerator<t_CHAR>::Alignment
TestSpecificationGenerator<t_CHAR>::alignment() const
{
    BSLS_ASSERT(isAlignOptionPresent());
    return d_alignment;
}

template <class t_CHAR>
bool TestSpecificationGenerator<t_CHAR>::isSignOptionPresent() const
{
    return d_signOptionIsPresent;
}

template <class t_CHAR>
typename TestSpecificationGenerator<t_CHAR>::Sign
TestSpecificationGenerator<t_CHAR>::sign() const
{
    BSLS_ASSERT(d_signOptionIsPresent);
    return d_sign;
}

template <class t_CHAR>
bool TestSpecificationGenerator<t_CHAR>::isHashOptionPresent() const
{
    return d_hashOptionIsPresent;
}

template <class t_CHAR>
bool TestSpecificationGenerator<t_CHAR>::isZeroOptionPresent() const
{
    return d_zeroOptionIsPresent;
}

template <class t_CHAR>
bool TestSpecificationGenerator<t_CHAR>::isWidthOptionPresent() const
{
    return d_widthOptionIsPresent;
}

template <class t_CHAR>
int TestSpecificationGenerator<t_CHAR>::width() const
{
    return d_width;
}

template <class t_CHAR>
bool TestSpecificationGenerator<t_CHAR>::isNestedWidthPresent() const
{
    return d_nestedWidthIsPresent;
}

template <class t_CHAR>
typename TestSpecificationGenerator<t_CHAR>::NestedVariant
TestSpecificationGenerator<t_CHAR>::nestedWidthVariant() const
{
    return d_nestedWidthVariant;
}

template <class t_CHAR>
bool TestSpecificationGenerator<t_CHAR>::isPrecisionOptionPresent() const
{
    return d_precisionOptionIsPresent;
}

template <class t_CHAR>
int TestSpecificationGenerator<t_CHAR>::precision() const
{
    return d_precision;
}

template <class t_CHAR>
bool TestSpecificationGenerator<t_CHAR>::isNestedPrecisionPresent() const
{
    return d_nestedPrecisionIsPresent;
}

template <class t_CHAR>
typename TestSpecificationGenerator<t_CHAR>::NestedVariant
TestSpecificationGenerator<t_CHAR>::nestedPrecisionVariant() const
{
    return d_nestedPrecisionVariant;
}

template <class t_CHAR>
bool TestSpecificationGenerator<t_CHAR>::isLocaleOptionPresent() const
{
    return d_localeOptionIsPresent;
}

template <class t_CHAR>
bool TestSpecificationGenerator<t_CHAR>::isTypeOptionPresent() const
{
    return d_typeOptionIsPresent;
}

template <class t_CHAR>
typename TestSpecificationGenerator<t_CHAR>::FormatType
TestSpecificationGenerator<t_CHAR>::type() const
{
    return d_type;
}

}  // close package namespace
}  // close enterprise namespace


#endif  // INCLUDED_BSLFMT_TESTSPECIFICATIONGENERATOR

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

