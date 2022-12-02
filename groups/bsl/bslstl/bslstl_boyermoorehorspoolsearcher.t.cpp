// bslstl_boyermoorehorspoolsearcher.t.cpp                            -*-C++-*-
#include <bslstl_boyermoorehorspoolsearcher.h>

#include <bslstl_equalto.h>
#include <bslstl_forwarditerator.h>
#include <bslstl_iterator.h>  // for 'bsl::distance', et al.
#include <bslstl_list.h>
#include <bslstl_pair.h>
#include <bslstl_string.h>
#include <bslstl_vector.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bslmf_issame.h>
#include <bslmf_movableref.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_buildtarget.h>
#include <bsls_libraryfeatures.h>
#include <bsls_nameof.h>
#include <bsls_platform.h>
#include <bsls_timeutil.h>  // TC -1
#include <bsls_types.h>     // for 'bsls::Types::Int64'

#include <cctype>     // for 'std::tolower'
#include <cstddef>    // for 'std::size_t'
#include <cstdlib>    // for 'std::atoi'
#include <cstring>    // for 'std::strlen'
#include <functional> // for 'std::equal_to'
#include <utility>    // for 'operator!=' and 'make_pair'

#include <float.h>    // for 'FLT_MAX' and 'FLT_MIN'
#include <limits.h>   // for 'UCHAR_MAX'
#include <stdio.h>    // for 'stdio' and 'printf'
#include <stddef.h>   // for 'NULL'

using namespace BloombergLP;

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_FUNCTORS
    #include <functional>
    namespace XYZ = std;
#else
    namespace XYZ = bsl;
#endif // BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_FUNCTORS

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// This component defines a template for a mechanism class,
// 'bslstl::BoyerMooreHorspoolSearcher' in which two iterators define the range
// of values being sought for (the "needle") and another two iterators define
// the range of values being sought through (the "haystack").  Note that the
// two pairs of iterators must refer to the same value type and must be random
// access iterators, but otherwise need not be the same type.  Thus, a
// sequences in one type of container (e.g., a vector) can be sought in another
// type of container (e.g., a string).  Our test concerns will focus on the
// attributes of the iterators -- e.g., can it be a constant iterator rather
// than on the value types (which we assume are tested elsewhere).
//
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] BoyerMooreHorspoolSearcher(RAI f, RAI l, HASH l, EQUAL e, *bA);
// [ 4] BoyerMooreHorspoolSearcher(const b_m_h_s& original);
// [ 4] BoyerMooreHorspoolSearcher(const b_m_h_s& original, *bA);
// [ 6] BoyerMooreHorspoolSearcher(MovableRef<b_m_h_s> original);
// [ 6] BoyerMooreHorspoolSearcher(MovableRef<b_m_h_s> original, *bA);
//
// MANIPULATORS
// [ 5] b_m_h& operator=(const b_m_h& rhs);
// [ 7] b_m_h& operator=(MovableRef<b_m_h> rhs);
//
// ACCESSORS
// [ 3] bsl::pair<RAI, RAI> operator()(RAI hsFirst, RAI hsLast) const;
// [ 2] RAI needleFirst() const;
// [ 2] RAI needleLast() const;
// [ 2] HASH hash() const;
// [ 2] EQUAL equal() const;
// [ 2] BloombergLP::bslma::Allocator *allocator() const;
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [11] USAGE EXAMPLE
// [10] CLASS TEMPLATE DEDUCTION GUIDES
// [ 8] boyer_moore_horspool_searcher: facade forwards correctly
// [ 9] TRAITS
// [ 9] PUBLIC TYPES

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
//                       STANDARD BDE TEST DRIVER MACROS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//              ADDITIONAL TEST MACROS FOR THIS TEST DRIVER
// ----------------------------------------------------------------------------

#define RUN_EACH_TYPE BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                      GLOBAL TYPEDEFS
// ----------------------------------------------------------------------------

typedef bsls::Types::Int64 Int64;

// ============================================================================
//                      GLOBAL HELPER CLASSES FOR TESTING
// ----------------------------------------------------------------------------

static bool             verbose;
static bool         veryVerbose;
static bool     veryVeryVerbose;
static bool veryVeryVeryVerbose;

                              // ===============
                              // class CharArray
                              // ===============

template <class TYPE>
class CharArray {
    // This array class is a simple wrapper on a deque offering an input
    // iterator access via the 'begin' and 'end' accessors.  The iterator is
    // specifically a *random-access* iterator and its value type is the
    // (template parameter) 'TYPE'.

    // DATA
    bsl::vector<TYPE>  d_value;

  public:
    // TYPES
    typedef const TYPE *const_iterator;
        // Random-access iterator.

    // CREATORS
    CharArray() {}
    explicit CharArray(const bsl::vector<TYPE>& value);

    // ACCESSORS
    const TYPE& operator[](size_t index) const;
    const_iterator begin() const;
    const_iterator end() const;
};

// CREATORS
template <class TYPE>
CharArray<TYPE>::CharArray(const bsl::vector<TYPE>& value)
: d_value(value)
{
}

// ACCESSORS
template <class TYPE>
const TYPE& CharArray<TYPE>::operator[](size_t index) const
{
    return d_value[index];
}

template <class TYPE>
typename CharArray<TYPE>::const_iterator CharArray<TYPE>::begin() const
{
    return const_iterator(d_value.begin());
}

template <class TYPE>
typename CharArray<TYPE>::const_iterator CharArray<TYPE>::end() const
{
    return const_iterator(d_value.end());
}

                              // ==============================
                              // class CharEqualCaseInsensitive
                              // ==============================

class CharEqualCaseInsensitive
{
    // DATA
    int         d_id;
    mutable int d_useCount;

  public:
    // CREATORS
    CharEqualCaseInsensitive();
    explicit CharEqualCaseInsensitive(int id);

    // ACCESSORS
    bool operator()(const char& a, const char& b) const;
    int id() const;
    int useCount() const;
};

// FREE OPERATORS
bool operator==(const CharEqualCaseInsensitive& lhs,
                const CharEqualCaseInsensitive& rhs);

                              // ------------------------------
                              // class CharEqualCaseInsensitive
                              // ------------------------------

// CREATORS
inline
CharEqualCaseInsensitive::CharEqualCaseInsensitive()
: d_id(0)
, d_useCount(0)
{
}

inline
CharEqualCaseInsensitive::CharEqualCaseInsensitive(int id)
: d_id(id)
, d_useCount(0)
{
}

// ACCESSORS
inline
bool CharEqualCaseInsensitive::operator()(const char& a, const char& b) const
{
    ++d_useCount;
    return std::tolower(a) == std::tolower(b);
}

inline
int CharEqualCaseInsensitive::id() const
{
    return d_id;
}

inline
int CharEqualCaseInsensitive::useCount() const
{
    return d_useCount;
}

// FREE OPERATORS
inline
bool operator==(const CharEqualCaseInsensitive& lhs,
                const CharEqualCaseInsensitive& rhs)
{
    return lhs.id() == rhs.id();
}

                              // =============================
                              // class CharHashCaseInsensitive
                              // =============================

class CharHashCaseInsensitive
{
    // DATA
    int         d_id;
    mutable int d_useCount;

  public:
    // CREATORS
    CharHashCaseInsensitive();
    explicit CharHashCaseInsensitive(int id);

    // ACCESSORS
    bool operator()(const char& value) const;
    int id() const;
    int useCount() const;
};

// FREE OPERATORS
bool operator==(const CharHashCaseInsensitive& lhs,
                const CharHashCaseInsensitive& rhs);

                              // -----------------------------
                              // class CharHashCaseInsensitive
                              // -----------------------------

// CREATORS
inline
CharHashCaseInsensitive::CharHashCaseInsensitive()
: d_id(0)
, d_useCount(0)
{
}

inline
CharHashCaseInsensitive::CharHashCaseInsensitive(int id)
: d_id(id)
, d_useCount(0)
{
}

// ACCESSORS
inline
bool CharHashCaseInsensitive::operator()(const char& value) const
{
    ++d_useCount;
    static bsl::hash<char> s_hash;
    return s_hash(static_cast<char>(std::tolower(value)));
}

inline
int CharHashCaseInsensitive::id() const
{
    return d_id;
}

inline
int CharHashCaseInsensitive::useCount() const
{
    return d_useCount;
}

// FREE OPERATORS
inline
bool operator==(const CharHashCaseInsensitive& lhs,
                const CharHashCaseInsensitive& rhs)
{
    return lhs.id() == rhs.id();
}

// ============================================================================
//                               GLOBAL DATA
// ----------------------------------------------------------------------------

static const char u_haystackText[] =
    " IN CONGRESS, July 4, 1776.\n"
    "\n"
    " The unanimous Declaration of the thirteen united States of America,\n"
    "\n"
    " When in the Course of human events, it becomes necessary for one\n"
    " people to dissolve the political bands which have connected them with\n"
    " another, and to assume among the powers of the earth, the separate\n"
    " and equal station to which the Laws of Nature and of Nature's God\n"
    " entitle them, a decent respect to the opinions of mankind requires\n"
    " that they should declare the causes which impel them to the\n"
    " separation.  We hold these truths to be self-evident, that all men\n"
    " are created equal, that they are endowed by their Creator with\n"
    " certain unalienable Rights, that among these are Life, Liberty and\n"
    " the pursuit of Happiness.--That to secure these rights, Governments\n"
    " are instituted among Men, deriving their just powers from the consent\n"
    " of the governed, --That whenever any Form of Government becomes\n"
    " destructive of these ends, it is the Right of the People to alter or\n"
    " to abolish it, and to institute new Government, laying its foundation\n"
    " on such principles and organizing its powers in such form, as to them\n"
    " shall seem most likely to effect their Safety and Happiness.\n"
    " Prudence, indeed, will dictate that Governments long established\n"
    " should not be changed for light and transient causes; and accordingly\n"
    " all experience hath shewn, that mankind are more disposed to suffer,\n"
    " while evils are sufferable, than to right themselves by abolishing\n"
    " the forms to which they are accustomed.  But when a long train of\n"
    " abuses and usurpations, pursuing invariably the same Object evinces a\n"
    " design to reduce them under absolute Despotism, it is their right, it\n"
    " is their duty, to throw off such Government, and to provide new\n"
    " Guards for their future security.--Such has been the patient\n"
    " sufferance of these Colonies; and such is now the necessity which\n"
    " constrains them to alter their former Systems of Government.  The\n"
    " history of the present King of Great Britain is a history of repeated\n"
    " injuries and usurpations, all having in direct object the\n"
    " establishment of an absolute Tyranny over these States.  To prove\n"
    " this, let Facts be submitted to a candid world.\n"
    "\n"
    ": o He has refused his Assent to Laws, the most wholesome and\n"
    ":   necessary for the public good.\n"
    ":\n"
    ": o He has forbidden his Governors to pass Laws of immediate and\n"
    ":   pressing importance, unless suspended in their operation till his\n"
    ":   Assent should be obtained; and when so suspended, he has utterly\n"
    ":   neglected to attend to them.\n"
    ":\n"
    ": o He has refused to pass other Laws for the accommodation of large\n"
    ":   districts of people, unless those people would relinquish the\n"
    ":   right of Representation in the Legislature, a right inestimable to\n"
    ":   them and formidable to tyrants only.\n"
    ":\n"
    ": o He has called together legislative bodies at places unusual,\n"
    ":   uncomfortable, and distant from the depository of their public\n"
    ":   Records, for the sole purpose of fatiguing them into compliance\n"
    ":   with his measures.\n"
    ":\n"
    ": o He has dissolved Representative Houses repeatedly, for opposing\n"
    ":   with manly firmness his invasions on the rights of the people.\n"
    ":\n"
    ": o He has refused for a long time, after such dissolutions, to cause\n"
    ":   others to be elected; whereby the Legislative powers, incapable of\n"
    ":   Annihilation, have returned to the People at large for their\n"
    ":   exercise; the State remaining in the mean time exposed to all the\n"
    ":   dangers of invasion from without, and convulsions within.\n"
    ":\n"
    ": o He has endeavoured to prevent the population of these States; for\n"
    ":   that purpose obstructing the Laws for Naturalization of\n"
    ":   Foreigners; refusing to pass others to encourage their migrations\n"
    ":   hither, and raising the conditions of new Appropriations of Lands.\n"
    ":\n"
    ": o He has obstructed the Administration of Justice, by refusing his\n"
    ":   Assent to Laws for establishing Judiciary powers.\n"
    ":\n"
    ": o He has made Judges dependent on his Will alone, for the tenure of\n"
    ":   their offices, and the amount and payment of their salaries.\n"
    ":\n"
    ": o He has erected a multitude of New Offices, and sent hither swarms\n"
    ":   of Officers to harrass our people, and eat out their substance.\n"
    ":\n"
    ": o He has kept among us, in times of peace, Standing Armies without\n"
    ":   the Consent of our legislatures.\n"
    ":\n"
    ": o He has affected to render the Military independent of and superior\n"
    ":   to the Civil power.\n"
    ":\n"
    ": o He has combined with others to subject us to a jurisdiction\n"
    ":   foreign to our constitution, and unacknowledged by our laws;\n"
    ":   giving his Assent to their Acts of pretended Legislation:\n"
    ":\n"
    ": o For Quartering large bodies of armed troops among us: For\n"
    ":   protecting them, by a mock Trial, from punishment for any Murders\n"
    ":   which they should commit on the Inhabitants of these States:\n"
    ":\n"
    ": o For cutting off our Trade with all parts of the world:\n"
    ":\n"
    ": o For imposing Taxes on us without our Consent: For depriving us in\n"
    ":   many cases, of the benefits of Trial by Jury:\n"
    ":\n"
    ": o For transporting us beyond Seas to be tried for pretended offences\n"
    ":\n"
    ": o For abolishing the free System of English Laws in a neighbouring\n"
    ":   Province, establishing therein an Arbitrary government, and\n"
    ":   enlarging its Boundaries so as to render it at once an example and\n"
    ":   fit instrument for introducing the same absolute rule into these\n"
    ":   Colonies:\n"
    ":\n"
    ": o For taking away our Charters, abolishing our most valuable Laws,\n"
    ":   and altering fundamentally the Forms of our Governments:\n"
    ":\n"
    ": o For suspending our own Legislatures, and declaring themselves\n"
    ":   invested with power to legislate for us in all cases whatsoever.\n"
    ":\n"
    ": o He has abdicated Government here, by declaring us out of his\n"
    ":   Protection and waging War against us.\n"
    ":\n"
    ": o He has plundered our seas, ravaged our Coasts, burnt our towns,\n"
    ":   and destroyed the lives of our people.  He is at this time\n"
    ":   transporting large Armies of foreign Mercenaries to compleat the\n"
    ":   works of death, desolation and tyranny, already begun with\n"
    ":   circumstances of Cruelty & perfidy scarcely paralleled in the most\n"
    ":   barbarous ages, and totally unworthy the Head of a civilized\n"
    ":   nation.\n"
    ":\n"
    ": o He has constrained our fellow Citizens taken Captive on the high\n"
    ":   Seas to bear Arms against their Country, to become the\n"
    ":   executioners of their friends and Brethren, or to fall themselves\n"
    ":   by their Hands.\n"
    ":\n"
    ": o He has excited domestic insurrections amongst us, and has\n"
    ":   endeavoured to bring on the inhabitants of our frontiers, the\n"
    ":   merciless Indian Savages, whose known rule of warfare, is an\n"
    ":   undistinguished destruction of all ages, sexes and conditions.\n"
    "\n"
    " In every stage of these Oppressions We have Petitioned for Redress in\n"
    " the most humble terms: Our repeated Petitions have been answered only\n"
    " by repeated injury.  A Prince whose character is thus marked by every\n"
    " act which may define a Tyrant, is unfit to be the ruler of a free\n"
    " people.\n"
    "\n"
    " Nor have We been wanting in attentions to our Brittish brethren.  We\n"
    " have warned them from time to time of attempts by their legislature\n"
    " to extend an unwarrantable jurisdiction over us.  We have reminded\n"
    " them of the circumstances of our emigration and settlement here.  We\n"
    " have appealed to their native justice and magnanimity, and we have\n"
    " conjured them by the ties of our common kindred to disavow these\n"
    " usurpations, which, would inevitably interrupt our connections and\n"
    " correspondence.  They too have been deaf to the voice of justice and\n"
    " of consanguinity.  We must, therefore, acquiesce in the necessity,\n"
    " which denounces our Separation, and hold them, as we hold the rest of\n"
    " mankind, Enemies in War, in Peace Friends.\n"
    "\n"
    " We, therefore, the Representatives of the united States of America,\n"
    " in General Congress, Assembled, appealing to the Supreme Judge of the\n"
    " world for the rectitude of our intentions, do, in the Name, and by\n"
    " Authority of the good People of these Colonies, solemnly publish and\n"
    " declare, That these United Colonies are, and of Right ought to be\n"
    " Free and Independent States; that they are Absolved from all\n"
    " Allegiance to the British Crown, and that all political connection\n"
    " between them and the State of Great Britain, is and ought to be\n"
    " totally dissolved; and that as Free and Independent States, they have\n"
    " full Power to levy War, conclude Peace, contract Alliances, establish\n"
    " Commerce, and to do all other Acts and Things which Independent\n"
    " States may of right do.  And for the support of this Declaration,\n"
    " with a firm reliance on the protection of divine Providence, we\n"
    " mutually pledge to each other our Lives, our Fortunes and our sacred\n"
    " Honor.\n";

const std::size_t  HAYSTACK_TEXT_LENGTH = sizeof u_haystackText - 1;
const char * const HAYSTACK_TEXT_FIRST  = u_haystackText;
const char * const HAYSTACK_TEXT_LAST   = u_haystackText
                                        + HAYSTACK_TEXT_LENGTH;

// Represent the same content as 'u_haystackTest' using a smaller alphabet.  In
// the string below the ASCII value of each character above is given as three
// characters in octal format.

static const char u_haystackOctal[] =
    "040111116040103117116107122105123123054040112165"
    "154171040064054040061067067066056012012124150145"
    "040165156141156151155157165163040104145143154141"
    "162141164151157156040157146040164150145040164150"
    "151162164145145156040165156151164145144040123164"
    "141164145163040157146040101155145162151143141054"
    "012012127150145156040151156040164150145040103157"
    "165162163145040157146040150165155141156040145166"
    "145156164163054040151164040142145143157155145163"
    "040156145143145163163141162171040146157162040157"
    "156145012160145157160154145040164157040144151163"
    "163157154166145040164150145040160157154151164151"
    "143141154040142141156144163040167150151143150040"
    "150141166145040143157156156145143164145144040164"
    "150145155040167151164150012141156157164150145162"
    "054040141156144040164157040141163163165155145040"
    "141155157156147040164150145040160157167145162163"
    "040157146040164150145040145141162164150054040164"
    "150145040163145160141162141164145012141156144040"
    "145161165141154040163164141164151157156040164157"
    "040167150151143150040164150145040114141167163040"
    "157146040116141164165162145040141156144040157146"
    "040116141164165162145047163040107157144012145156"
    "164151164154145040164150145155054040141040144145"
    "143145156164040162145163160145143164040164157040"
    "164150145040157160151156151157156163040157146040"
    "155141156153151156144040162145161165151162145163"
    "012164150141164040164150145171040163150157165154"
    "144040144145143154141162145040164150145040143141"
    "165163145163040167150151143150040151155160145154"
    "040164150145155040164157040164150145012163145160"
    "141162141164151157156056040040127145040150157154"
    "144040164150145163145040164162165164150163040164"
    "157040142145040163145154146055145166151144145156"
    "164054040164150141164040141154154040155145156012"
    "141162145040143162145141164145144040145161165141"
    "154054040164150141164040164150145171040141162145"
    "040145156144157167145144040142171040164150145151"
    "162040103162145141164157162040167151164150012143"
    "145162164141151156040165156141154151145156141142"
    "154145040122151147150164163054040164150141164040"
    "141155157156147040164150145163145040141162145040"
    "114151146145054040114151142145162164171040141156"
    "144012164150145040160165162163165151164040157146"
    "040110141160160151156145163163056055055124150141"
    "164040164157040163145143165162145040164150145163"
    "145040162151147150164163054040107157166145162156"
    "155145156164163012141162145040151156163164151164"
    "165164145144040141155157156147040115145156054040"
    "144145162151166151156147040164150145151162040152"
    "165163164040160157167145162163040146162157155040"
    "164150145040143157156163145156164012157146040164"
    "150145040147157166145162156145144054040055055124"
    "150141164040167150145156145166145162040141156171"
    "040106157162155040157146040107157166145162156155"
    "145156164040142145143157155145163012144145163164"
    "162165143164151166145040157146040164150145163145"
    "040145156144163054040151164040151163040164150145"
    "040122151147150164040157146040164150145040120145"
    "157160154145040164157040141154164145162040157162"
    "012164157040141142157154151163150040151164054040"
    "141156144040164157040151156163164151164165164145"
    "040156145167040107157166145162156155145156164054"
    "040154141171151156147040151164163040146157165156"
    "144141164151157156012157156040163165143150040160"
    "162151156143151160154145163040141156144040157162"
    "147141156151172151156147040151164163040160157167"
    "145162163040151156040163165143150040146157162155"
    "054040141163040164157040164150145155012163150141"
    "154154040163145145155040155157163164040154151153"
    "145154171040164157040145146146145143164040164150"
    "145151162040123141146145164171040141156144040110"
    "141160160151156145163163056012120162165144145156"
    "143145054040151156144145145144054040167151154154"
    "040144151143164141164145040164150141164040107157"
    "166145162156155145156164163040154157156147040145"
    "163164141142154151163150145144012163150157165154"
    "144040156157164040142145040143150141156147145144"
    "040146157162040154151147150164040141156144040164"
    "162141156163151145156164040143141165163145163073"
    "040141156144040141143143157162144151156147154171"
    "012141154154040145170160145162151145156143145040"
    "150141164150040163150145167156054040164150141164"
    "040155141156153151156144040141162145040155157162"
    "145040144151163160157163145144040164157040163165"
    "146146145162054012167150151154145040145166151154"
    "163040141162145040163165146146145162141142154145"
    "054040164150141156040164157040162151147150164040"
    "164150145155163145154166145163040142171040141142"
    "157154151163150151156147012164150145040146157162"
    "155163040164157040167150151143150040164150145171"
    "040141162145040141143143165163164157155145144056"
    "040040102165164040167150145156040141040154157156"
    "147040164162141151156040157146012141142165163145"
    "163040141156144040165163165162160141164151157156"
    "163054040160165162163165151156147040151156166141"
    "162151141142154171040164150145040163141155145040"
    "117142152145143164040145166151156143145163040141"
    "012144145163151147156040164157040162145144165143"
    "145040164150145155040165156144145162040141142163"
    "157154165164145040104145163160157164151163155054"
    "040151164040151163040164150145151162040162151147"
    "150164054040151164012151163040164150145151162040"
    "144165164171054040164157040164150162157167040157"
    "146146040163165143150040107157166145162156155145"
    "156164054040141156144040164157040160162157166151"
    "144145040156145167012107165141162144163040146157"
    "162040164150145151162040146165164165162145040163"
    "145143165162151164171056055055123165143150040150"
    "141163040142145145156040164150145040160141164151"
    "145156164012163165146146145162141156143145040157"
    "146040164150145163145040103157154157156151145163"
    "073040141156144040163165143150040151163040156157"
    "167040164150145040156145143145163163151164171040"
    "167150151143150012143157156163164162141151156163"
    "040164150145155040164157040141154164145162040164"
    "150145151162040146157162155145162040123171163164"
    "145155163040157146040107157166145162156155145156"
    "164056040040124150145012150151163164157162171040"
    "157146040164150145040160162145163145156164040113"
    "151156147040157146040107162145141164040102162151"
    "164141151156040151163040141040150151163164157162"
    "171040157146040162145160145141164145144012151156"
    "152165162151145163040141156144040165163165162160"
    "141164151157156163054040141154154040150141166151"
    "156147040151156040144151162145143164040157142152"
    "145143164040164150145012145163164141142154151163"
    "150155145156164040157146040141156040141142163157"
    "154165164145040124171162141156156171040157166145"
    "162040164150145163145040123164141164145163056040"
    "040124157040160162157166145012164150151163054040"
    "154145164040106141143164163040142145040163165142"
    "155151164164145144040164157040141040143141156144"
    "151144040167157162154144056012012072040157040110"
    "145040150141163040162145146165163145144040150151"
    "163040101163163145156164040164157040114141167163"
    "054040164150145040155157163164040167150157154145"
    "163157155145040141156144012072040040040156145143"
    "145163163141162171040146157162040164150145040160"
    "165142154151143040147157157144056012072012072040"
    "157040110145040150141163040146157162142151144144"
    "145156040150151163040107157166145162156157162163"
    "040164157040160141163163040114141167163040157146"
    "040151155155145144151141164145040141156144012072"
    "040040040160162145163163151156147040151155160157"
    "162164141156143145054040165156154145163163040163"
    "165163160145156144145144040151156040164150145151"
    "162040157160145162141164151157156040164151154154"
    "040150151163012072040040040101163163145156164040"
    "163150157165154144040142145040157142164141151156"
    "145144073040141156144040167150145156040163157040"
    "163165163160145156144145144054040150145040150141"
    "163040165164164145162154171012072040040040156145"
    "147154145143164145144040164157040141164164145156"
    "144040164157040164150145155056012072012072040157"
    "040110145040150141163040162145146165163145144040"
    "164157040160141163163040157164150145162040114141"
    "167163040146157162040164150145040141143143157155"
    "155157144141164151157156040157146040154141162147"
    "145012072040040040144151163164162151143164163040"
    "157146040160145157160154145054040165156154145163"
    "163040164150157163145040160145157160154145040167"
    "157165154144040162145154151156161165151163150040"
    "164150145012072040040040162151147150164040157146"
    "040122145160162145163145156164141164151157156040"
    "151156040164150145040114145147151163154141164165"
    "162145054040141040162151147150164040151156145163"
    "164151155141142154145040164157012072040040040164"
    "150145155040141156144040146157162155151144141142"
    "154145040164157040164171162141156164163040157156"
    "154171056012072012072040157040110145040150141163"
    "040143141154154145144040164157147145164150145162"
    "040154145147151163154141164151166145040142157144"
    "151145163040141164040160154141143145163040165156"
    "165163165141154054012072040040040165156143157155"
    "146157162164141142154145054040141156144040144151"
    "163164141156164040146162157155040164150145040144"
    "145160157163151164157162171040157146040164150145"
    "151162040160165142154151143012072040040040122145"
    "143157162144163054040146157162040164150145040163"
    "157154145040160165162160157163145040157146040146"
    "141164151147165151156147040164150145155040151156"
    "164157040143157155160154151141156143145012072040"
    "040040167151164150040150151163040155145141163165"
    "162145163056012072012072040157040110145040150141"
    "163040144151163163157154166145144040122145160162"
    "145163145156164141164151166145040110157165163145"
    "163040162145160145141164145144154171054040146157"
    "162040157160160157163151156147012072040040040167"
    "151164150040155141156154171040146151162155156145"
    "163163040150151163040151156166141163151157156163"
    "040157156040164150145040162151147150164163040157"
    "146040164150145040160145157160154145056012072012"
    "072040157040110145040150141163040162145146165163"
    "145144040146157162040141040154157156147040164151"
    "155145054040141146164145162040163165143150040144"
    "151163163157154165164151157156163054040164157040"
    "143141165163145012072040040040157164150145162163"
    "040164157040142145040145154145143164145144073040"
    "167150145162145142171040164150145040114145147151"
    "163154141164151166145040160157167145162163054040"
    "151156143141160141142154145040157146012072040040"
    "040101156156151150151154141164151157156054040150"
    "141166145040162145164165162156145144040164157040"
    "164150145040120145157160154145040141164040154141"
    "162147145040146157162040164150145151162012072040"
    "040040145170145162143151163145073040164150145040"
    "123164141164145040162145155141151156151156147040"
    "151156040164150145040155145141156040164151155145"
    "040145170160157163145144040164157040141154154040"
    "164150145012072040040040144141156147145162163040"
    "157146040151156166141163151157156040146162157155"
    "040167151164150157165164054040141156144040143157"
    "156166165154163151157156163040167151164150151156"
    "056012072012072040157040110145040150141163040145"
    "156144145141166157165162145144040164157040160162"
    "145166145156164040164150145040160157160165154141"
    "164151157156040157146040164150145163145040123164"
    "141164145163073040146157162012072040040040164150"
    "141164040160165162160157163145040157142163164162"
    "165143164151156147040164150145040114141167163040"
    "146157162040116141164165162141154151172141164151"
    "157156040157146012072040040040106157162145151147"
    "156145162163073040162145146165163151156147040164"
    "157040160141163163040157164150145162163040164157"
    "040145156143157165162141147145040164150145151162"
    "040155151147162141164151157156163012072040040040"
    "150151164150145162054040141156144040162141151163"
    "151156147040164150145040143157156144151164151157"
    "156163040157146040156145167040101160160162157160"
    "162151141164151157156163040157146040114141156144"
    "163056012072012072040157040110145040150141163040"
    "157142163164162165143164145144040164150145040101"
    "144155151156151163164162141164151157156040157146"
    "040112165163164151143145054040142171040162145146"
    "165163151156147040150151163012072040040040101163"
    "163145156164040164157040114141167163040146157162"
    "040145163164141142154151163150151156147040112165"
    "144151143151141162171040160157167145162163056012"
    "072012072040157040110145040150141163040155141144"
    "145040112165144147145163040144145160145156144145"
    "156164040157156040150151163040127151154154040141"
    "154157156145054040146157162040164150145040164145"
    "156165162145040157146012072040040040164150145151"
    "162040157146146151143145163054040141156144040164"
    "150145040141155157165156164040141156144040160141"
    "171155145156164040157146040164150145151162040163"
    "141154141162151145163056012072012072040157040110"
    "145040150141163040145162145143164145144040141040"
    "155165154164151164165144145040157146040116145167"
    "040117146146151143145163054040141156144040163145"
    "156164040150151164150145162040163167141162155163"
    "012072040040040157146040117146146151143145162163"
    "040164157040150141162162141163163040157165162040"
    "160145157160154145054040141156144040145141164040"
    "157165164040164150145151162040163165142163164141"
    "156143145056012072012072040157040110145040150141"
    "163040153145160164040141155157156147040165163054"
    "040151156040164151155145163040157146040160145141"
    "143145054040123164141156144151156147040101162155"
    "151145163040167151164150157165164012072040040040"
    "164150145040103157156163145156164040157146040157"
    "165162040154145147151163154141164165162145163056"
    "012072012072040157040110145040150141163040141146"
    "146145143164145144040164157040162145156144145162"
    "040164150145040115151154151164141162171040151156"
    "144145160145156144145156164040157146040141156144"
    "040163165160145162151157162012072040040040164157"
    "040164150145040103151166151154040160157167145162"
    "056012072012072040157040110145040150141163040143"
    "157155142151156145144040167151164150040157164150"
    "145162163040164157040163165142152145143164040165"
    "163040164157040141040152165162151163144151143164"
    "151157156012072040040040146157162145151147156040"
    "164157040157165162040143157156163164151164165164"
    "151157156054040141156144040165156141143153156157"
    "167154145144147145144040142171040157165162040154"
    "141167163073012072040040040147151166151156147040"
    "150151163040101163163145156164040164157040164150"
    "145151162040101143164163040157146040160162145164"
    "145156144145144040114145147151163154141164151157"
    "156072012072012072040157040106157162040121165141"
    "162164145162151156147040154141162147145040142157"
    "144151145163040157146040141162155145144040164162"
    "157157160163040141155157156147040165163072040106"
    "157162012072040040040160162157164145143164151156"
    "147040164150145155054040142171040141040155157143"
    "153040124162151141154054040146162157155040160165"
    "156151163150155145156164040146157162040141156171"
    "040115165162144145162163012072040040040167150151"
    "143150040164150145171040163150157165154144040143"
    "157155155151164040157156040164150145040111156150"
    "141142151164141156164163040157146040164150145163"
    "145040123164141164145163072012072012072040157040"
    "106157162040143165164164151156147040157146146040"
    "157165162040124162141144145040167151164150040141"
    "154154040160141162164163040157146040164150145040"
    "167157162154144072012072012072040157040106157162"
    "040151155160157163151156147040124141170145163040"
    "157156040165163040167151164150157165164040157165"
    "162040103157156163145156164072040106157162040144"
    "145160162151166151156147040165163040151156012072"
    "040040040155141156171040143141163145163054040157"
    "146040164150145040142145156145146151164163040157"
    "146040124162151141154040142171040112165162171072"
    "012072012072040157040106157162040164162141156163"
    "160157162164151156147040165163040142145171157156"
    "144040123145141163040164157040142145040164162151"
    "145144040146157162040160162145164145156144145144"
    "040157146146145156143145163012072012072040157040"
    "106157162040141142157154151163150151156147040164"
    "150145040146162145145040123171163164145155040157"
    "146040105156147154151163150040114141167163040151"
    "156040141040156145151147150142157165162151156147"
    "012072040040040120162157166151156143145054040145"
    "163164141142154151163150151156147040164150145162"
    "145151156040141156040101162142151164162141162171"
    "040147157166145162156155145156164054040141156144"
    "012072040040040145156154141162147151156147040151"
    "164163040102157165156144141162151145163040163157"
    "040141163040164157040162145156144145162040151164"
    "040141164040157156143145040141156040145170141155"
    "160154145040141156144012072040040040146151164040"
    "151156163164162165155145156164040146157162040151"
    "156164162157144165143151156147040164150145040163"
    "141155145040141142163157154165164145040162165154"
    "145040151156164157040164150145163145012072040040"
    "040103157154157156151145163072012072012072040157"
    "040106157162040164141153151156147040141167141171"
    "040157165162040103150141162164145162163054040141"
    "142157154151163150151156147040157165162040155157"
    "163164040166141154165141142154145040114141167163"
    "054012072040040040141156144040141154164145162151"
    "156147040146165156144141155145156164141154154171"
    "040164150145040106157162155163040157146040157165"
    "162040107157166145162156155145156164163072012072"
    "012072040157040106157162040163165163160145156144"
    "151156147040157165162040157167156040114145147151"
    "163154141164165162145163054040141156144040144145"
    "143154141162151156147040164150145155163145154166"
    "145163012072040040040151156166145163164145144040"
    "167151164150040160157167145162040164157040154145"
    "147151163154141164145040146157162040165163040151"
    "156040141154154040143141163145163040167150141164"
    "163157145166145162056012072012072040157040110145"
    "040150141163040141142144151143141164145144040107"
    "157166145162156155145156164040150145162145054040"
    "142171040144145143154141162151156147040165163040"
    "157165164040157146040150151163012072040040040120"
    "162157164145143164151157156040141156144040167141"
    "147151156147040127141162040141147141151156163164"
    "040165163056012072012072040157040110145040150141"
    "163040160154165156144145162145144040157165162040"
    "163145141163054040162141166141147145144040157165"
    "162040103157141163164163054040142165162156164040"
    "157165162040164157167156163054012072040040040141"
    "156144040144145163164162157171145144040164150145"
    "040154151166145163040157146040157165162040160145"
    "157160154145056040040110145040151163040141164040"
    "164150151163040164151155145012072040040040164162"
    "141156163160157162164151156147040154141162147145"
    "040101162155151145163040157146040146157162145151"
    "147156040115145162143145156141162151145163040164"
    "157040143157155160154145141164040164150145012072"
    "040040040167157162153163040157146040144145141164"
    "150054040144145163157154141164151157156040141156"
    "144040164171162141156156171054040141154162145141"
    "144171040142145147165156040167151164150012072040"
    "040040143151162143165155163164141156143145163040"
    "157146040103162165145154164171040046040160145162"
    "146151144171040163143141162143145154171040160141"
    "162141154154145154145144040151156040164150145040"
    "155157163164012072040040040142141162142141162157"
    "165163040141147145163054040141156144040164157164"
    "141154154171040165156167157162164150171040164150"
    "145040110145141144040157146040141040143151166151"
    "154151172145144012072040040040156141164151157156"
    "056012072012072040157040110145040150141163040143"
    "157156163164162141151156145144040157165162040146"
    "145154154157167040103151164151172145156163040164"
    "141153145156040103141160164151166145040157156040"
    "164150145040150151147150012072040040040123145141"
    "163040164157040142145141162040101162155163040141"
    "147141151156163164040164150145151162040103157165"
    "156164162171054040164157040142145143157155145040"
    "164150145012072040040040145170145143165164151157"
    "156145162163040157146040164150145151162040146162"
    "151145156144163040141156144040102162145164150162"
    "145156054040157162040164157040146141154154040164"
    "150145155163145154166145163012072040040040142171"
    "040164150145151162040110141156144163056012072012"
    "072040157040110145040150141163040145170143151164"
    "145144040144157155145163164151143040151156163165"
    "162162145143164151157156163040141155157156147163"
    "164040165163054040141156144040150141163012072040"
    "040040145156144145141166157165162145144040164157"
    "040142162151156147040157156040164150145040151156"
    "150141142151164141156164163040157146040157165162"
    "040146162157156164151145162163054040164150145012"
    "072040040040155145162143151154145163163040111156"
    "144151141156040123141166141147145163054040167150"
    "157163145040153156157167156040162165154145040157"
    "146040167141162146141162145054040151163040141156"
    "012072040040040165156144151163164151156147165151"
    "163150145144040144145163164162165143164151157156"
    "040157146040141154154040141147145163054040163145"
    "170145163040141156144040143157156144151164151157"
    "156163056012012111156040145166145162171040163164"
    "141147145040157146040164150145163145040117160160"
    "162145163163151157156163040127145040150141166145"
    "040120145164151164151157156145144040146157162040"
    "122145144162145163163040151156012164150145040155"
    "157163164040150165155142154145040164145162155163"
    "072040117165162040162145160145141164145144040120"
    "145164151164151157156163040150141166145040142145"
    "145156040141156163167145162145144040157156154171"
    "012142171040162145160145141164145144040151156152"
    "165162171056040040101040120162151156143145040167"
    "150157163145040143150141162141143164145162040151"
    "163040164150165163040155141162153145144040142171"
    "040145166145162171012141143164040167150151143150"
    "040155141171040144145146151156145040141040124171"
    "162141156164054040151163040165156146151164040164"
    "157040142145040164150145040162165154145162040157"
    "146040141040146162145145012160145157160154145056"
    "012012116157162040150141166145040127145040142145"
    "145156040167141156164151156147040151156040141164"
    "164145156164151157156163040164157040157165162040"
    "102162151164164151163150040142162145164150162145"
    "156056040040127145012150141166145040167141162156"
    "145144040164150145155040146162157155040164151155"
    "145040164157040164151155145040157146040141164164"
    "145155160164163040142171040164150145151162040154"
    "145147151163154141164165162145012164157040145170"
    "164145156144040141156040165156167141162162141156"
    "164141142154145040152165162151163144151143164151"
    "157156040157166145162040165163056040040127145040"
    "150141166145040162145155151156144145144012164150"
    "145155040157146040164150145040143151162143165155"
    "163164141156143145163040157146040157165162040145"
    "155151147162141164151157156040141156144040163145"
    "164164154145155145156164040150145162145056040040"
    "127145012150141166145040141160160145141154145144"
    "040164157040164150145151162040156141164151166145"
    "040152165163164151143145040141156144040155141147"
    "156141156151155151164171054040141156144040167145"
    "040150141166145012143157156152165162145144040164"
    "150145155040142171040164150145040164151145163040"
    "157146040157165162040143157155155157156040153151"
    "156144162145144040164157040144151163141166157167"
    "040164150145163145012165163165162160141164151157"
    "156163054040167150151143150054040167157165154144"
    "040151156145166151164141142154171040151156164145"
    "162162165160164040157165162040143157156156145143"
    "164151157156163040141156144012143157162162145163"
    "160157156144145156143145056040040124150145171040"
    "164157157040150141166145040142145145156040144145"
    "141146040164157040164150145040166157151143145040"
    "157146040152165163164151143145040141156144012157"
    "146040143157156163141156147165151156151164171056"
    "040040127145040155165163164054040164150145162145"
    "146157162145054040141143161165151145163143145040"
    "151156040164150145040156145143145163163151164171"
    "054012167150151143150040144145156157165156143145"
    "163040157165162040123145160141162141164151157156"
    "054040141156144040150157154144040164150145155054"
    "040141163040167145040150157154144040164150145040"
    "162145163164040157146012155141156153151156144054"
    "040105156145155151145163040151156040127141162054"
    "040151156040120145141143145040106162151145156144"
    "163056012012127145054040164150145162145146157162"
    "145054040164150145040122145160162145163145156164"
    "141164151166145163040157146040164150145040165156"
    "151164145144040123164141164145163040157146040101"
    "155145162151143141054012151156040107145156145162"
    "141154040103157156147162145163163054040101163163"
    "145155142154145144054040141160160145141154151156"
    "147040164157040164150145040123165160162145155145"
    "040112165144147145040157146040164150145012167157"
    "162154144040146157162040164150145040162145143164"
    "151164165144145040157146040157165162040151156164"
    "145156164151157156163054040144157054040151156040"
    "164150145040116141155145054040141156144040142171"
    "012101165164150157162151164171040157146040164150"
    "145040147157157144040120145157160154145040157146"
    "040164150145163145040103157154157156151145163054"
    "040163157154145155156154171040160165142154151163"
    "150040141156144012144145143154141162145054040124"
    "150141164040164150145163145040125156151164145144"
    "040103157154157156151145163040141162145054040141"
    "156144040157146040122151147150164040157165147150"
    "164040164157040142145012106162145145040141156144"
    "040111156144145160145156144145156164040123164141"
    "164145163073040164150141164040164150145171040141"
    "162145040101142163157154166145144040146162157155"
    "040141154154012101154154145147151141156143145040"
    "164157040164150145040102162151164151163150040103"
    "162157167156054040141156144040164150141164040141"
    "154154040160157154151164151143141154040143157156"
    "156145143164151157156012142145164167145145156040"
    "164150145155040141156144040164150145040123164141"
    "164145040157146040107162145141164040102162151164"
    "141151156054040151163040141156144040157165147150"
    "164040164157040142145012164157164141154154171040"
    "144151163163157154166145144073040141156144040164"
    "150141164040141163040106162145145040141156144040"
    "111156144145160145156144145156164040123164141164"
    "145163054040164150145171040150141166145012146165"
    "154154040120157167145162040164157040154145166171"
    "040127141162054040143157156143154165144145040120"
    "145141143145054040143157156164162141143164040101"
    "154154151141156143145163054040145163164141142154"
    "151163150012103157155155145162143145054040141156"
    "144040164157040144157040141154154040157164150145"
    "162040101143164163040141156144040124150151156147"
    "163040167150151143150040111156144145160145156144"
    "145156164012123164141164145163040155141171040157"
    "146040162151147150164040144157056040040101156144"
    "040146157162040164150145040163165160160157162164"
    "040157146040164150151163040104145143154141162141"
    "164151157156054012167151164150040141040146151162"
    "155040162145154151141156143145040157156040164150"
    "145040160162157164145143164151157156040157146040"
    "144151166151156145040120162157166151144145156143"
    "145054040167145012155165164165141154154171040160"
    "154145144147145040164157040145141143150040157164"
    "150145162040157165162040114151166145163054040157"
    "165162040106157162164165156145163040141156144040"
    "157165162040163141143162145144012110157156157162"
    "056012";

const std::size_t  HAYSTACK_OCTAL_LENGTH = sizeof u_haystackOctal - 1;
const char * const HAYSTACK_OCTAL_FIRST  = u_haystackOctal;
const char * const HAYSTACK_OCTAL_LAST   = u_haystackOctal
                                         + HAYSTACK_OCTAL_LENGTH;

// Represent the same content as 'u_haystackTest' using an even smaller
// alphabet.  In the string below the ASCII value of each character above is
// given as eight characters in binary ('0' or '1') format.

static const char u_haystackBinary[] =
    "0010000001001001010011100010000001000011010011110100111001000111"
    "0101001001000101010100110101001100101100001000000100101001110101"
    "0110110001111001001000000011010000101100001000000011000100110111"
    "0011011100110110001011100000101000001010010101000110100001100101"
    "0010000001110101011011100110000101101110011010010110110101101111"
    "0111010101110011001000000100010001100101011000110110110001100001"
    "0111001001100001011101000110100101101111011011100010000001101111"
    "0110011000100000011101000110100001100101001000000111010001101000"
    "0110100101110010011101000110010101100101011011100010000001110101"
    "0110111001101001011101000110010101100100001000000101001101110100"
    "0110000101110100011001010111001100100000011011110110011000100000"
    "0100000101101101011001010111001001101001011000110110000100101100"
    "0000101000001010010101110110100001100101011011100010000001101001"
    "0110111000100000011101000110100001100101001000000100001101101111"
    "0111010101110010011100110110010100100000011011110110011000100000"
    "0110100001110101011011010110000101101110001000000110010101110110"
    "0110010101101110011101000111001100101100001000000110100101110100"
    "0010000001100010011001010110001101101111011011010110010101110011"
    "0010000001101110011001010110001101100101011100110111001101100001"
    "0111001001111001001000000110011001101111011100100010000001101111"
    "0110111001100101000010100111000001100101011011110111000001101100"
    "0110010100100000011101000110111100100000011001000110100101110011"
    "0111001101101111011011000111011001100101001000000111010001101000"
    "0110010100100000011100000110111101101100011010010111010001101001"
    "0110001101100001011011000010000001100010011000010110111001100100"
    "0111001100100000011101110110100001101001011000110110100000100000"
    "0110100001100001011101100110010100100000011000110110111101101110"
    "0110111001100101011000110111010001100101011001000010000001110100"
    "0110100001100101011011010010000001110111011010010111010001101000"
    "0000101001100001011011100110111101110100011010000110010101110010"
    "0010110000100000011000010110111001100100001000000111010001101111"
    "0010000001100001011100110111001101110101011011010110010100100000"
    "0110000101101101011011110110111001100111001000000111010001101000"
    "0110010100100000011100000110111101110111011001010111001001110011"
    "0010000001101111011001100010000001110100011010000110010100100000"
    "0110010101100001011100100111010001101000001011000010000001110100"
    "0110100001100101001000000111001101100101011100000110000101110010"
    "0110000101110100011001010000101001100001011011100110010000100000"
    "0110010101110001011101010110000101101100001000000111001101110100"
    "0110000101110100011010010110111101101110001000000111010001101111"
    "0010000001110111011010000110100101100011011010000010000001110100"
    "0110100001100101001000000100110001100001011101110111001100100000"
    "0110111101100110001000000100111001100001011101000111010101110010"
    "0110010100100000011000010110111001100100001000000110111101100110"
    "0010000001001110011000010111010001110101011100100110010100100111"
    "0111001100100000010001110110111101100100000010100110010101101110"
    "0111010001101001011101000110110001100101001000000111010001101000"
    "0110010101101101001011000010000001100001001000000110010001100101"
    "0110001101100101011011100111010000100000011100100110010101110011"
    "0111000001100101011000110111010000100000011101000110111100100000"
    "0111010001101000011001010010000001101111011100000110100101101110"
    "0110100101101111011011100111001100100000011011110110011000100000"
    "0110110101100001011011100110101101101001011011100110010000100000"
    "0111001001100101011100010111010101101001011100100110010101110011"
    "0000101001110100011010000110000101110100001000000111010001101000"
    "0110010101111001001000000111001101101000011011110111010101101100"
    "0110010000100000011001000110010101100011011011000110000101110010"
    "0110010100100000011101000110100001100101001000000110001101100001"
    "0111010101110011011001010111001100100000011101110110100001101001"
    "0110001101101000001000000110100101101101011100000110010101101100"
    "0010000001110100011010000110010101101101001000000111010001101111"
    "0010000001110100011010000110010100001010011100110110010101110000"
    "0110000101110010011000010111010001101001011011110110111000101110"
    "0010000000100000010101110110010100100000011010000110111101101100"
    "0110010000100000011101000110100001100101011100110110010100100000"
    "0111010001110010011101010111010001101000011100110010000001110100"
    "0110111100100000011000100110010100100000011100110110010101101100"
    "0110011000101101011001010111011001101001011001000110010101101110"
    "0111010000101100001000000111010001101000011000010111010000100000"
    "0110000101101100011011000010000001101101011001010110111000001010"
    "0110000101110010011001010010000001100011011100100110010101100001"
    "0111010001100101011001000010000001100101011100010111010101100001"
    "0110110000101100001000000111010001101000011000010111010000100000"
    "0111010001101000011001010111100100100000011000010111001001100101"
    "0010000001100101011011100110010001101111011101110110010101100100"
    "0010000001100010011110010010000001110100011010000110010101101001"
    "0111001000100000010000110111001001100101011000010111010001101111"
    "0111001000100000011101110110100101110100011010000000101001100011"
    "0110010101110010011101000110000101101001011011100010000001110101"
    "0110111001100001011011000110100101100101011011100110000101100010"
    "0110110001100101001000000101001001101001011001110110100001110100"
    "0111001100101100001000000111010001101000011000010111010000100000"
    "0110000101101101011011110110111001100111001000000111010001101000"
    "0110010101110011011001010010000001100001011100100110010100100000"
    "0100110001101001011001100110010100101100001000000100110001101001"
    "0110001001100101011100100111010001111001001000000110000101101110"
    "0110010000001010011101000110100001100101001000000111000001110101"
    "0111001001110011011101010110100101110100001000000110111101100110"
    "0010000001001000011000010111000001110000011010010110111001100101"
    "0111001101110011001011100010110100101101010101000110100001100001"
    "0111010000100000011101000110111100100000011100110110010101100011"
    "0111010101110010011001010010000001110100011010000110010101110011"
    "0110010100100000011100100110100101100111011010000111010001110011"
    "0010110000100000010001110110111101110110011001010111001001101110"
    "0110110101100101011011100111010001110011000010100110000101110010"
    "0110010100100000011010010110111001110011011101000110100101110100"
    "0111010101110100011001010110010000100000011000010110110101101111"
    "0110111001100111001000000100110101100101011011100010110000100000"
    "0110010001100101011100100110100101110110011010010110111001100111"
    "0010000001110100011010000110010101101001011100100010000001101010"
    "0111010101110011011101000010000001110000011011110111011101100101"
    "0111001001110011001000000110011001110010011011110110110100100000"
    "0111010001101000011001010010000001100011011011110110111001110011"
    "0110010101101110011101000000101001101111011001100010000001110100"
    "0110100001100101001000000110011101101111011101100110010101110010"
    "0110111001100101011001000010110000100000001011010010110101010100"
    "0110100001100001011101000010000001110111011010000110010101101110"
    "0110010101110110011001010111001000100000011000010110111001111001"
    "0010000001000110011011110111001001101101001000000110111101100110"
    "0010000001000111011011110111011001100101011100100110111001101101"
    "0110010101101110011101000010000001100010011001010110001101101111"
    "0110110101100101011100110000101001100100011001010111001101110100"
    "0111001001110101011000110111010001101001011101100110010100100000"
    "0110111101100110001000000111010001101000011001010111001101100101"
    "0010000001100101011011100110010001110011001011000010000001101001"
    "0111010000100000011010010111001100100000011101000110100001100101"
    "0010000001010010011010010110011101101000011101000010000001101111"
    "0110011000100000011101000110100001100101001000000101000001100101"
    "0110111101110000011011000110010100100000011101000110111100100000"
    "0110000101101100011101000110010101110010001000000110111101110010"
    "0000101001110100011011110010000001100001011000100110111101101100"
    "0110100101110011011010000010000001101001011101000010110000100000"
    "0110000101101110011001000010000001110100011011110010000001101001"
    "0110111001110011011101000110100101110100011101010111010001100101"
    "0010000001101110011001010111011100100000010001110110111101110110"
    "0110010101110010011011100110110101100101011011100111010000101100"
    "0010000001101100011000010111100101101001011011100110011100100000"
    "0110100101110100011100110010000001100110011011110111010101101110"
    "0110010001100001011101000110100101101111011011100000101001101111"
    "0110111000100000011100110111010101100011011010000010000001110000"
    "0111001001101001011011100110001101101001011100000110110001100101"
    "0111001100100000011000010110111001100100001000000110111101110010"
    "0110011101100001011011100110100101111010011010010110111001100111"
    "0010000001101001011101000111001100100000011100000110111101110111"
    "0110010101110010011100110010000001101001011011100010000001110011"
    "0111010101100011011010000010000001100110011011110111001001101101"
    "0010110000100000011000010111001100100000011101000110111100100000"
    "0111010001101000011001010110110100001010011100110110100001100001"
    "0110110001101100001000000111001101100101011001010110110100100000"
    "0110110101101111011100110111010000100000011011000110100101101011"
    "0110010101101100011110010010000001110100011011110010000001100101"
    "0110011001100110011001010110001101110100001000000111010001101000"
    "0110010101101001011100100010000001010011011000010110011001100101"
    "0111010001111001001000000110000101101110011001000010000001001000"
    "0110000101110000011100000110100101101110011001010111001101110011"
    "0010111000001010010100000111001001110101011001000110010101101110"
    "0110001101100101001011000010000001101001011011100110010001100101"
    "0110010101100100001011000010000001110111011010010110110001101100"
    "0010000001100100011010010110001101110100011000010111010001100101"
    "0010000001110100011010000110000101110100001000000100011101101111"
    "0111011001100101011100100110111001101101011001010110111001110100"
    "0111001100100000011011000110111101101110011001110010000001100101"
    "0111001101110100011000010110001001101100011010010111001101101000"
    "0110010101100100000010100111001101101000011011110111010101101100"
    "0110010000100000011011100110111101110100001000000110001001100101"
    "0010000001100011011010000110000101101110011001110110010101100100"
    "0010000001100110011011110111001000100000011011000110100101100111"
    "0110100001110100001000000110000101101110011001000010000001110100"
    "0111001001100001011011100111001101101001011001010110111001110100"
    "0010000001100011011000010111010101110011011001010111001100111011"
    "0010000001100001011011100110010000100000011000010110001101100011"
    "0110111101110010011001000110100101101110011001110110110001111001"
    "0000101001100001011011000110110000100000011001010111100001110000"
    "0110010101110010011010010110010101101110011000110110010100100000"
    "0110100001100001011101000110100000100000011100110110100001100101"
    "0111011101101110001011000010000001110100011010000110000101110100"
    "0010000001101101011000010110111001101011011010010110111001100100"
    "0010000001100001011100100110010100100000011011010110111101110010"
    "0110010100100000011001000110100101110011011100000110111101110011"
    "0110010101100100001000000111010001101111001000000111001101110101"
    "0110011001100110011001010111001000101100000010100111011101101000"
    "0110100101101100011001010010000001100101011101100110100101101100"
    "0111001100100000011000010111001001100101001000000111001101110101"
    "0110011001100110011001010111001001100001011000100110110001100101"
    "0010110000100000011101000110100001100001011011100010000001110100"
    "0110111100100000011100100110100101100111011010000111010000100000"
    "0111010001101000011001010110110101110011011001010110110001110110"
    "0110010101110011001000000110001001111001001000000110000101100010"
    "0110111101101100011010010111001101101000011010010110111001100111"
    "0000101001110100011010000110010100100000011001100110111101110010"
    "0110110101110011001000000111010001101111001000000111011101101000"
    "0110100101100011011010000010000001110100011010000110010101111001"
    "0010000001100001011100100110010100100000011000010110001101100011"
    "0111010101110011011101000110111101101101011001010110010000101110"
    "0010000000100000010000100111010101110100001000000111011101101000"
    "0110010101101110001000000110000100100000011011000110111101101110"
    "0110011100100000011101000111001001100001011010010110111000100000"
    "0110111101100110000010100110000101100010011101010111001101100101"
    "0111001100100000011000010110111001100100001000000111010101110011"
    "0111010101110010011100000110000101110100011010010110111101101110"
    "0111001100101100001000000111000001110101011100100111001101110101"
    "0110100101101110011001110010000001101001011011100111011001100001"
    "0111001001101001011000010110001001101100011110010010000001110100"
    "0110100001100101001000000111001101100001011011010110010100100000"
    "0100111101100010011010100110010101100011011101000010000001100101"
    "0111011001101001011011100110001101100101011100110010000001100001"
    "0000101001100100011001010111001101101001011001110110111000100000"
    "0111010001101111001000000111001001100101011001000111010101100011"
    "0110010100100000011101000110100001100101011011010010000001110101"
    "0110111001100100011001010111001000100000011000010110001001110011"
    "0110111101101100011101010111010001100101001000000100010001100101"
    "0111001101110000011011110111010001101001011100110110110100101100"
    "0010000001101001011101000010000001101001011100110010000001110100"
    "0110100001100101011010010111001000100000011100100110100101100111"
    "0110100001110100001011000010000001101001011101000000101001101001"
    "0111001100100000011101000110100001100101011010010111001000100000"
    "0110010001110101011101000111100100101100001000000111010001101111"
    "0010000001110100011010000111001001101111011101110010000001101111"
    "0110011001100110001000000111001101110101011000110110100000100000"
    "0100011101101111011101100110010101110010011011100110110101100101"
    "0110111001110100001011000010000001100001011011100110010000100000"
    "0111010001101111001000000111000001110010011011110111011001101001"
    "0110010001100101001000000110111001100101011101110000101001000111"
    "0111010101100001011100100110010001110011001000000110011001101111"
    "0111001000100000011101000110100001100101011010010111001000100000"
    "0110011001110101011101000111010101110010011001010010000001110011"
    "0110010101100011011101010111001001101001011101000111100100101110"
    "0010110100101101010100110111010101100011011010000010000001101000"
    "0110000101110011001000000110001001100101011001010110111000100000"
    "0111010001101000011001010010000001110000011000010111010001101001"
    "0110010101101110011101000000101001110011011101010110011001100110"
    "0110010101110010011000010110111001100011011001010010000001101111"
    "0110011000100000011101000110100001100101011100110110010100100000"
    "0100001101101111011011000110111101101110011010010110010101110011"
    "0011101100100000011000010110111001100100001000000111001101110101"
    "0110001101101000001000000110100101110011001000000110111001101111"
    "0111011100100000011101000110100001100101001000000110111001100101"
    "0110001101100101011100110111001101101001011101000111100100100000"
    "0111011101101000011010010110001101101000000010100110001101101111"
    "0110111001110011011101000111001001100001011010010110111001110011"
    "0010000001110100011010000110010101101101001000000111010001101111"
    "0010000001100001011011000111010001100101011100100010000001110100"
    "0110100001100101011010010111001000100000011001100110111101110010"
    "0110110101100101011100100010000001010011011110010111001101110100"
    "0110010101101101011100110010000001101111011001100010000001000111"
    "0110111101110110011001010111001001101110011011010110010101101110"
    "0111010000101110001000000010000001010100011010000110010100001010"
    "0110100001101001011100110111010001101111011100100111100100100000"
    "0110111101100110001000000111010001101000011001010010000001110000"
    "0111001001100101011100110110010101101110011101000010000001001011"
    "0110100101101110011001110010000001101111011001100010000001000111"
    "0111001001100101011000010111010000100000010000100111001001101001"
    "0111010001100001011010010110111000100000011010010111001100100000"
    "0110000100100000011010000110100101110011011101000110111101110010"
    "0111100100100000011011110110011000100000011100100110010101110000"
    "0110010101100001011101000110010101100100000010100110100101101110"
    "0110101001110101011100100110100101100101011100110010000001100001"
    "0110111001100100001000000111010101110011011101010111001001110000"
    "0110000101110100011010010110111101101110011100110010110000100000"
    "0110000101101100011011000010000001101000011000010111011001101001"
    "0110111001100111001000000110100101101110001000000110010001101001"
    "0111001001100101011000110111010000100000011011110110001001101010"
    "0110010101100011011101000010000001110100011010000110010100001010"
    "0110010101110011011101000110000101100010011011000110100101110011"
    "0110100001101101011001010110111001110100001000000110111101100110"
    "0010000001100001011011100010000001100001011000100111001101101111"
    "0110110001110101011101000110010100100000010101000111100101110010"
    "0110000101101110011011100111100100100000011011110111011001100101"
    "0111001000100000011101000110100001100101011100110110010100100000"
    "0101001101110100011000010111010001100101011100110010111000100000"
    "0010000001010100011011110010000001110000011100100110111101110110"
    "0110010100001010011101000110100001101001011100110010110000100000"
    "0110110001100101011101000010000001000110011000010110001101110100"
    "0111001100100000011000100110010100100000011100110111010101100010"
    "0110110101101001011101000111010001100101011001000010000001110100"
    "0110111100100000011000010010000001100011011000010110111001100100"
    "0110100101100100001000000111011101101111011100100110110001100100"
    "0010111000001010000010100011101000100000011011110010000001001000"
    "0110010100100000011010000110000101110011001000000111001001100101"
    "0110011001110101011100110110010101100100001000000110100001101001"
    "0111001100100000010000010111001101110011011001010110111001110100"
    "0010000001110100011011110010000001001100011000010111011101110011"
    "0010110000100000011101000110100001100101001000000110110101101111"
    "0111001101110100001000000111011101101000011011110110110001100101"
    "0111001101101111011011010110010100100000011000010110111001100100"
    "0000101000111010001000000010000000100000011011100110010101100011"
    "0110010101110011011100110110000101110010011110010010000001100110"
    "0110111101110010001000000111010001101000011001010010000001110000"
    "0111010101100010011011000110100101100011001000000110011101101111"
    "0110111101100100001011100000101000111010000010100011101000100000"
    "0110111100100000010010000110010100100000011010000110000101110011"
    "0010000001100110011011110111001001100010011010010110010001100100"
    "0110010101101110001000000110100001101001011100110010000001000111"
    "0110111101110110011001010111001001101110011011110111001001110011"
    "0010000001110100011011110010000001110000011000010111001101110011"
    "0010000001001100011000010111011101110011001000000110111101100110"
    "0010000001101001011011010110110101100101011001000110100101100001"
    "0111010001100101001000000110000101101110011001000000101000111010"
    "0010000000100000001000000111000001110010011001010111001101110011"
    "0110100101101110011001110010000001101001011011010111000001101111"
    "0111001001110100011000010110111001100011011001010010110000100000"
    "0111010101101110011011000110010101110011011100110010000001110011"
    "0111010101110011011100000110010101101110011001000110010101100100"
    "0010000001101001011011100010000001110100011010000110010101101001"
    "0111001000100000011011110111000001100101011100100110000101110100"
    "0110100101101111011011100010000001110100011010010110110001101100"
    "0010000001101000011010010111001100001010001110100010000000100000"
    "0010000001000001011100110111001101100101011011100111010000100000"
    "0111001101101000011011110111010101101100011001000010000001100010"
    "0110010100100000011011110110001001110100011000010110100101101110"
    "0110010101100100001110110010000001100001011011100110010000100000"
    "0111011101101000011001010110111000100000011100110110111100100000"
    "0111001101110101011100110111000001100101011011100110010001100101"
    "0110010000101100001000000110100001100101001000000110100001100001"
    "0111001100100000011101010111010001110100011001010111001001101100"
    "0111100100001010001110100010000000100000001000000110111001100101"
    "0110011101101100011001010110001101110100011001010110010000100000"
    "0111010001101111001000000110000101110100011101000110010101101110"
    "0110010000100000011101000110111100100000011101000110100001100101"
    "0110110100101110000010100011101000001010001110100010000001101111"
    "0010000001001000011001010010000001101000011000010111001100100000"
    "0111001001100101011001100111010101110011011001010110010000100000"
    "0111010001101111001000000111000001100001011100110111001100100000"
    "0110111101110100011010000110010101110010001000000100110001100001"
    "0111011101110011001000000110011001101111011100100010000001110100"
    "0110100001100101001000000110000101100011011000110110111101101101"
    "0110110101101111011001000110000101110100011010010110111101101110"
    "0010000001101111011001100010000001101100011000010111001001100111"
    "0110010100001010001110100010000000100000001000000110010001101001"
    "0111001101110100011100100110100101100011011101000111001100100000"
    "0110111101100110001000000111000001100101011011110111000001101100"
    "0110010100101100001000000111010101101110011011000110010101110011"
    "0111001100100000011101000110100001101111011100110110010100100000"
    "0111000001100101011011110111000001101100011001010010000001110111"
    "0110111101110101011011000110010000100000011100100110010101101100"
    "0110100101101110011100010111010101101001011100110110100000100000"
    "0111010001101000011001010000101000111010001000000010000000100000"
    "0111001001101001011001110110100001110100001000000110111101100110"
    "0010000001010010011001010111000001110010011001010111001101100101"
    "0110111001110100011000010111010001101001011011110110111000100000"
    "0110100101101110001000000111010001101000011001010010000001001100"
    "0110010101100111011010010111001101101100011000010111010001110101"
    "0111001001100101001011000010000001100001001000000111001001101001"
    "0110011101101000011101000010000001101001011011100110010101110011"
    "0111010001101001011011010110000101100010011011000110010100100000"
    "0111010001101111000010100011101000100000001000000010000001110100"
    "0110100001100101011011010010000001100001011011100110010000100000"
    "0110011001101111011100100110110101101001011001000110000101100010"
    "0110110001100101001000000111010001101111001000000111010001111001"
    "0111001001100001011011100111010001110011001000000110111101101110"
    "0110110001111001001011100000101000111010000010100011101000100000"
    "0110111100100000010010000110010100100000011010000110000101110011"
    "0010000001100011011000010110110001101100011001010110010000100000"
    "0111010001101111011001110110010101110100011010000110010101110010"
    "0010000001101100011001010110011101101001011100110110110001100001"
    "0111010001101001011101100110010100100000011000100110111101100100"
    "0110100101100101011100110010000001100001011101000010000001110000"
    "0110110001100001011000110110010101110011001000000111010101101110"
    "0111010101110011011101010110000101101100001011000000101000111010"
    "0010000000100000001000000111010101101110011000110110111101101101"
    "0110011001101111011100100111010001100001011000100110110001100101"
    "0010110000100000011000010110111001100100001000000110010001101001"
    "0111001101110100011000010110111001110100001000000110011001110010"
    "0110111101101101001000000111010001101000011001010010000001100100"
    "0110010101110000011011110111001101101001011101000110111101110010"
    "0111100100100000011011110110011000100000011101000110100001100101"
    "0110100101110010001000000111000001110101011000100110110001101001"
    "0110001100001010001110100010000000100000001000000101001001100101"
    "0110001101101111011100100110010001110011001011000010000001100110"
    "0110111101110010001000000111010001101000011001010010000001110011"
    "0110111101101100011001010010000001110000011101010111001001110000"
    "0110111101110011011001010010000001101111011001100010000001100110"
    "0110000101110100011010010110011101110101011010010110111001100111"
    "0010000001110100011010000110010101101101001000000110100101101110"
    "0111010001101111001000000110001101101111011011010111000001101100"
    "0110100101100001011011100110001101100101000010100011101000100000"
    "0010000000100000011101110110100101110100011010000010000001101000"
    "0110100101110011001000000110110101100101011000010111001101110101"
    "0111001001100101011100110010111000001010001110100000101000111010"
    "0010000001101111001000000100100001100101001000000110100001100001"
    "0111001100100000011001000110100101110011011100110110111101101100"
    "0111011001100101011001000010000001010010011001010111000001110010"
    "0110010101110011011001010110111001110100011000010111010001101001"
    "0111011001100101001000000100100001101111011101010111001101100101"
    "0111001100100000011100100110010101110000011001010110000101110100"
    "0110010101100100011011000111100100101100001000000110011001101111"
    "0111001000100000011011110111000001110000011011110111001101101001"
    "0110111001100111000010100011101000100000001000000010000001110111"
    "0110100101110100011010000010000001101101011000010110111001101100"
    "0111100100100000011001100110100101110010011011010110111001100101"
    "0111001101110011001000000110100001101001011100110010000001101001"
    "0110111001110110011000010111001101101001011011110110111001110011"
    "0010000001101111011011100010000001110100011010000110010100100000"
    "0111001001101001011001110110100001110100011100110010000001101111"
    "0110011000100000011101000110100001100101001000000111000001100101"
    "0110111101110000011011000110010100101110000010100011101000001010"
    "0011101000100000011011110010000001001000011001010010000001101000"
    "0110000101110011001000000111001001100101011001100111010101110011"
    "0110010101100100001000000110011001101111011100100010000001100001"
    "0010000001101100011011110110111001100111001000000111010001101001"
    "0110110101100101001011000010000001100001011001100111010001100101"
    "0111001000100000011100110111010101100011011010000010000001100100"
    "0110100101110011011100110110111101101100011101010111010001101001"
    "0110111101101110011100110010110000100000011101000110111100100000"
    "0110001101100001011101010111001101100101000010100011101000100000"
    "0010000000100000011011110111010001101000011001010111001001110011"
    "0010000001110100011011110010000001100010011001010010000001100101"
    "0110110001100101011000110111010001100101011001000011101100100000"
    "0111011101101000011001010111001001100101011000100111100100100000"
    "0111010001101000011001010010000001001100011001010110011101101001"
    "0111001101101100011000010111010001101001011101100110010100100000"
    "0111000001101111011101110110010101110010011100110010110000100000"
    "0110100101101110011000110110000101110000011000010110001001101100"
    "0110010100100000011011110110011000001010001110100010000000100000"
    "0010000001000001011011100110111001101001011010000110100101101100"
    "0110000101110100011010010110111101101110001011000010000001101000"
    "0110000101110110011001010010000001110010011001010111010001110101"
    "0111001001101110011001010110010000100000011101000110111100100000"
    "0111010001101000011001010010000001010000011001010110111101110000"
    "0110110001100101001000000110000101110100001000000110110001100001"
    "0111001001100111011001010010000001100110011011110111001000100000"
    "0111010001101000011001010110100101110010000010100011101000100000"
    "0010000000100000011001010111100001100101011100100110001101101001"
    "0111001101100101001110110010000001110100011010000110010100100000"
    "0101001101110100011000010111010001100101001000000111001001100101"
    "0110110101100001011010010110111001101001011011100110011100100000"
    "0110100101101110001000000111010001101000011001010010000001101101"
    "0110010101100001011011100010000001110100011010010110110101100101"
    "0010000001100101011110000111000001101111011100110110010101100100"
    "0010000001110100011011110010000001100001011011000110110000100000"
    "0111010001101000011001010000101000111010001000000010000000100000"
    "0110010001100001011011100110011101100101011100100111001100100000"
    "0110111101100110001000000110100101101110011101100110000101110011"
    "0110100101101111011011100010000001100110011100100110111101101101"
    "0010000001110111011010010111010001101000011011110111010101110100"
    "0010110000100000011000010110111001100100001000000110001101101111"
    "0110111001110110011101010110110001110011011010010110111101101110"
    "0111001100100000011101110110100101110100011010000110100101101110"
    "0010111000001010001110100000101000111010001000000110111100100000"
    "0100100001100101001000000110100001100001011100110010000001100101"
    "0110111001100100011001010110000101110110011011110111010101110010"
    "0110010101100100001000000111010001101111001000000111000001110010"
    "0110010101110110011001010110111001110100001000000111010001101000"
    "0110010100100000011100000110111101110000011101010110110001100001"
    "0111010001101001011011110110111000100000011011110110011000100000"
    "0111010001101000011001010111001101100101001000000101001101110100"
    "0110000101110100011001010111001100111011001000000110011001101111"
    "0111001000001010001110100010000000100000001000000111010001101000"
    "0110000101110100001000000111000001110101011100100111000001101111"
    "0111001101100101001000000110111101100010011100110111010001110010"
    "0111010101100011011101000110100101101110011001110010000001110100"
    "0110100001100101001000000100110001100001011101110111001100100000"
    "0110011001101111011100100010000001001110011000010111010001110101"
    "0111001001100001011011000110100101111010011000010111010001101001"
    "0110111101101110001000000110111101100110000010100011101000100000"
    "0010000000100000010001100110111101110010011001010110100101100111"
    "0110111001100101011100100111001100111011001000000111001001100101"
    "0110011001110101011100110110100101101110011001110010000001110100"
    "0110111100100000011100000110000101110011011100110010000001101111"
    "0111010001101000011001010111001001110011001000000111010001101111"
    "0010000001100101011011100110001101101111011101010111001001100001"
    "0110011101100101001000000111010001101000011001010110100101110010"
    "0010000001101101011010010110011101110010011000010111010001101001"
    "0110111101101110011100110000101000111010001000000010000000100000"
    "0110100001101001011101000110100001100101011100100010110000100000"
    "0110000101101110011001000010000001110010011000010110100101110011"
    "0110100101101110011001110010000001110100011010000110010100100000"
    "0110001101101111011011100110010001101001011101000110100101101111"
    "0110111001110011001000000110111101100110001000000110111001100101"
    "0111011100100000010000010111000001110000011100100110111101110000"
    "0111001001101001011000010111010001101001011011110110111001110011"
    "0010000001101111011001100010000001001100011000010110111001100100"
    "0111001100101110000010100011101000001010001110100010000001101111"
    "0010000001001000011001010010000001101000011000010111001100100000"
    "0110111101100010011100110111010001110010011101010110001101110100"
    "0110010101100100001000000111010001101000011001010010000001000001"
    "0110010001101101011010010110111001101001011100110111010001110010"
    "0110000101110100011010010110111101101110001000000110111101100110"
    "0010000001001010011101010111001101110100011010010110001101100101"
    "0010110000100000011000100111100100100000011100100110010101100110"
    "0111010101110011011010010110111001100111001000000110100001101001"
    "0111001100001010001110100010000000100000001000000100000101110011"
    "0111001101100101011011100111010000100000011101000110111100100000"
    "0100110001100001011101110111001100100000011001100110111101110010"
    "0010000001100101011100110111010001100001011000100110110001101001"
    "0111001101101000011010010110111001100111001000000100101001110101"
    "0110010001101001011000110110100101100001011100100111100100100000"
    "0111000001101111011101110110010101110010011100110010111000001010"
    "0011101000001010001110100010000001101111001000000100100001100101"
    "0010000001101000011000010111001100100000011011010110000101100100"
    "0110010100100000010010100111010101100100011001110110010101110011"
    "0010000001100100011001010111000001100101011011100110010001100101"
    "0110111001110100001000000110111101101110001000000110100001101001"
    "0111001100100000010101110110100101101100011011000010000001100001"
    "0110110001101111011011100110010100101100001000000110011001101111"
    "0111001000100000011101000110100001100101001000000111010001100101"
    "0110111001110101011100100110010100100000011011110110011000001010"
    "0011101000100000001000000010000001110100011010000110010101101001"
    "0111001000100000011011110110011001100110011010010110001101100101"
    "0111001100101100001000000110000101101110011001000010000001110100"
    "0110100001100101001000000110000101101101011011110111010101101110"
    "0111010000100000011000010110111001100100001000000111000001100001"
    "0111100101101101011001010110111001110100001000000110111101100110"
    "0010000001110100011010000110010101101001011100100010000001110011"
    "0110000101101100011000010111001001101001011001010111001100101110"
    "0000101000111010000010100011101000100000011011110010000001001000"
    "0110010100100000011010000110000101110011001000000110010101110010"
    "0110010101100011011101000110010101100100001000000110000100100000"
    "0110110101110101011011000111010001101001011101000111010101100100"
    "0110010100100000011011110110011000100000010011100110010101110111"
    "0010000001001111011001100110011001101001011000110110010101110011"
    "0010110000100000011000010110111001100100001000000111001101100101"
    "0110111001110100001000000110100001101001011101000110100001100101"
    "0111001000100000011100110111011101100001011100100110110101110011"
    "0000101000111010001000000010000000100000011011110110011000100000"
    "0100111101100110011001100110100101100011011001010111001001110011"
    "0010000001110100011011110010000001101000011000010111001001110010"
    "0110000101110011011100110010000001101111011101010111001000100000"
    "0111000001100101011011110111000001101100011001010010110000100000"
    "0110000101101110011001000010000001100101011000010111010000100000"
    "0110111101110101011101000010000001110100011010000110010101101001"
    "0111001000100000011100110111010101100010011100110111010001100001"
    "0110111001100011011001010010111000001010001110100000101000111010"
    "0010000001101111001000000100100001100101001000000110100001100001"
    "0111001100100000011010110110010101110000011101000010000001100001"
    "0110110101101111011011100110011100100000011101010111001100101100"
    "0010000001101001011011100010000001110100011010010110110101100101"
    "0111001100100000011011110110011000100000011100000110010101100001"
    "0110001101100101001011000010000001010011011101000110000101101110"
    "0110010001101001011011100110011100100000010000010111001001101101"
    "0110100101100101011100110010000001110111011010010111010001101000"
    "0110111101110101011101000000101000111010001000000010000000100000"
    "0111010001101000011001010010000001000011011011110110111001110011"
    "0110010101101110011101000010000001101111011001100010000001101111"
    "0111010101110010001000000110110001100101011001110110100101110011"
    "0110110001100001011101000111010101110010011001010111001100101110"
    "0000101000111010000010100011101000100000011011110010000001001000"
    "0110010100100000011010000110000101110011001000000110000101100110"
    "0110011001100101011000110111010001100101011001000010000001110100"
    "0110111100100000011100100110010101101110011001000110010101110010"
    "0010000001110100011010000110010100100000010011010110100101101100"
    "0110100101110100011000010111001001111001001000000110100101101110"
    "0110010001100101011100000110010101101110011001000110010101101110"
    "0111010000100000011011110110011000100000011000010110111001100100"
    "0010000001110011011101010111000001100101011100100110100101101111"
    "0111001000001010001110100010000000100000001000000111010001101111"
    "0010000001110100011010000110010100100000010000110110100101110110"
    "0110100101101100001000000111000001101111011101110110010101110010"
    "0010111000001010001110100000101000111010001000000110111100100000"
    "0100100001100101001000000110100001100001011100110010000001100011"
    "0110111101101101011000100110100101101110011001010110010000100000"
    "0111011101101001011101000110100000100000011011110111010001101000"
    "0110010101110010011100110010000001110100011011110010000001110011"
    "0111010101100010011010100110010101100011011101000010000001110101"
    "0111001100100000011101000110111100100000011000010010000001101010"
    "0111010101110010011010010111001101100100011010010110001101110100"
    "0110100101101111011011100000101000111010001000000010000000100000"
    "0110011001101111011100100110010101101001011001110110111000100000"
    "0111010001101111001000000110111101110101011100100010000001100011"
    "0110111101101110011100110111010001101001011101000111010101110100"
    "0110100101101111011011100010110000100000011000010110111001100100"
    "0010000001110101011011100110000101100011011010110110111001101111"
    "0111011101101100011001010110010001100111011001010110010000100000"
    "0110001001111001001000000110111101110101011100100010000001101100"
    "0110000101110111011100110011101100001010001110100010000000100000"
    "0010000001100111011010010111011001101001011011100110011100100000"
    "0110100001101001011100110010000001000001011100110111001101100101"
    "0110111001110100001000000111010001101111001000000111010001101000"
    "0110010101101001011100100010000001000001011000110111010001110011"
    "0010000001101111011001100010000001110000011100100110010101110100"
    "0110010101101110011001000110010101100100001000000100110001100101"
    "0110011101101001011100110110110001100001011101000110100101101111"
    "0110111000111010000010100011101000001010001110100010000001101111"
    "0010000001000110011011110111001000100000010100010111010101100001"
    "0111001001110100011001010111001001101001011011100110011100100000"
    "0110110001100001011100100110011101100101001000000110001001101111"
    "0110010001101001011001010111001100100000011011110110011000100000"
    "0110000101110010011011010110010101100100001000000111010001110010"
    "0110111101101111011100000111001100100000011000010110110101101111"
    "0110111001100111001000000111010101110011001110100010000001000110"
    "0110111101110010000010100011101000100000001000000010000001110000"
    "0111001001101111011101000110010101100011011101000110100101101110"
    "0110011100100000011101000110100001100101011011010010110000100000"
    "0110001001111001001000000110000100100000011011010110111101100011"
    "0110101100100000010101000111001001101001011000010110110000101100"
    "0010000001100110011100100110111101101101001000000111000001110101"
    "0110111001101001011100110110100001101101011001010110111001110100"
    "0010000001100110011011110111001000100000011000010110111001111001"
    "0010000001001101011101010111001001100100011001010111001001110011"
    "0000101000111010001000000010000000100000011101110110100001101001"
    "0110001101101000001000000111010001101000011001010111100100100000"
    "0111001101101000011011110111010101101100011001000010000001100011"
    "0110111101101101011011010110100101110100001000000110111101101110"
    "0010000001110100011010000110010100100000010010010110111001101000"
    "0110000101100010011010010111010001100001011011100111010001110011"
    "0010000001101111011001100010000001110100011010000110010101110011"
    "0110010100100000010100110111010001100001011101000110010101110011"
    "0011101000001010001110100000101000111010001000000110111100100000"
    "0100011001101111011100100010000001100011011101010111010001110100"
    "0110100101101110011001110010000001101111011001100110011000100000"
    "0110111101110101011100100010000001010100011100100110000101100100"
    "0110010100100000011101110110100101110100011010000010000001100001"
    "0110110001101100001000000111000001100001011100100111010001110011"
    "0010000001101111011001100010000001110100011010000110010100100000"
    "0111011101101111011100100110110001100100001110100000101000111010"
    "0000101000111010001000000110111100100000010001100110111101110010"
    "0010000001101001011011010111000001101111011100110110100101101110"
    "0110011100100000010101000110000101111000011001010111001100100000"
    "0110111101101110001000000111010101110011001000000111011101101001"
    "0111010001101000011011110111010101110100001000000110111101110101"
    "0111001000100000010000110110111101101110011100110110010101101110"
    "0111010000111010001000000100011001101111011100100010000001100100"
    "0110010101110000011100100110100101110110011010010110111001100111"
    "0010000001110101011100110010000001101001011011100000101000111010"
    "0010000000100000001000000110110101100001011011100111100100100000"
    "0110001101100001011100110110010101110011001011000010000001101111"
    "0110011000100000011101000110100001100101001000000110001001100101"
    "0110111001100101011001100110100101110100011100110010000001101111"
    "0110011000100000010101000111001001101001011000010110110000100000"
    "0110001001111001001000000100101001110101011100100111100100111010"
    "0000101000111010000010100011101000100000011011110010000001000110"
    "0110111101110010001000000111010001110010011000010110111001110011"
    "0111000001101111011100100111010001101001011011100110011100100000"
    "0111010101110011001000000110001001100101011110010110111101101110"
    "0110010000100000010100110110010101100001011100110010000001110100"
    "0110111100100000011000100110010100100000011101000111001001101001"
    "0110010101100100001000000110011001101111011100100010000001110000"
    "0111001001100101011101000110010101101110011001000110010101100100"
    "0010000001101111011001100110011001100101011011100110001101100101"
    "0111001100001010001110100000101000111010001000000110111100100000"
    "0100011001101111011100100010000001100001011000100110111101101100"
    "0110100101110011011010000110100101101110011001110010000001110100"
    "0110100001100101001000000110011001110010011001010110010100100000"
    "0101001101111001011100110111010001100101011011010010000001101111"
    "0110011000100000010001010110111001100111011011000110100101110011"
    "0110100000100000010011000110000101110111011100110010000001101001"
    "0110111000100000011000010010000001101110011001010110100101100111"
    "0110100001100010011011110111010101110010011010010110111001100111"
    "0000101000111010001000000010000000100000010100000111001001101111"
    "0111011001101001011011100110001101100101001011000010000001100101"
    "0111001101110100011000010110001001101100011010010111001101101000"
    "0110100101101110011001110010000001110100011010000110010101110010"
    "0110010101101001011011100010000001100001011011100010000001000001"
    "0111001001100010011010010111010001110010011000010111001001111001"
    "0010000001100111011011110111011001100101011100100110111001101101"
    "0110010101101110011101000010110000100000011000010110111001100100"
    "0000101000111010001000000010000000100000011001010110111001101100"
    "0110000101110010011001110110100101101110011001110010000001101001"
    "0111010001110011001000000100001001101111011101010110111001100100"
    "0110000101110010011010010110010101110011001000000111001101101111"
    "0010000001100001011100110010000001110100011011110010000001110010"
    "0110010101101110011001000110010101110010001000000110100101110100"
    "0010000001100001011101000010000001101111011011100110001101100101"
    "0010000001100001011011100010000001100101011110000110000101101101"
    "0111000001101100011001010010000001100001011011100110010000001010"
    "0011101000100000001000000010000001100110011010010111010000100000"
    "0110100101101110011100110111010001110010011101010110110101100101"
    "0110111001110100001000000110011001101111011100100010000001101001"
    "0110111001110100011100100110111101100100011101010110001101101001"
    "0110111001100111001000000111010001101000011001010010000001110011"
    "0110000101101101011001010010000001100001011000100111001101101111"
    "0110110001110101011101000110010100100000011100100111010101101100"
    "0110010100100000011010010110111001110100011011110010000001110100"
    "0110100001100101011100110110010100001010001110100010000000100000"
    "0010000001000011011011110110110001101111011011100110100101100101"
    "0111001100111010000010100011101000001010001110100010000001101111"
    "0010000001000110011011110111001000100000011101000110000101101011"
    "0110100101101110011001110010000001100001011101110110000101111001"
    "0010000001101111011101010111001000100000010000110110100001100001"
    "0111001001110100011001010111001001110011001011000010000001100001"
    "0110001001101111011011000110100101110011011010000110100101101110"
    "0110011100100000011011110111010101110010001000000110110101101111"
    "0111001101110100001000000111011001100001011011000111010101100001"
    "0110001001101100011001010010000001001100011000010111011101110011"
    "0010110000001010001110100010000000100000001000000110000101101110"
    "0110010000100000011000010110110001110100011001010111001001101001"
    "0110111001100111001000000110011001110101011011100110010001100001"
    "0110110101100101011011100111010001100001011011000110110001111001"
    "0010000001110100011010000110010100100000010001100110111101110010"
    "0110110101110011001000000110111101100110001000000110111101110101"
    "0111001000100000010001110110111101110110011001010111001001101110"
    "0110110101100101011011100111010001110011001110100000101000111010"
    "0000101000111010001000000110111100100000010001100110111101110010"
    "0010000001110011011101010111001101110000011001010110111001100100"
    "0110100101101110011001110010000001101111011101010111001000100000"
    "0110111101110111011011100010000001001100011001010110011101101001"
    "0111001101101100011000010111010001110101011100100110010101110011"
    "0010110000100000011000010110111001100100001000000110010001100101"
    "0110001101101100011000010111001001101001011011100110011100100000"
    "0111010001101000011001010110110101110011011001010110110001110110"
    "0110010101110011000010100011101000100000001000000010000001101001"
    "0110111001110110011001010111001101110100011001010110010000100000"
    "0111011101101001011101000110100000100000011100000110111101110111"
    "0110010101110010001000000111010001101111001000000110110001100101"
    "0110011101101001011100110110110001100001011101000110010100100000"
    "0110011001101111011100100010000001110101011100110010000001101001"
    "0110111000100000011000010110110001101100001000000110001101100001"
    "0111001101100101011100110010000001110111011010000110000101110100"
    "0111001101101111011001010111011001100101011100100010111000001010"
    "0011101000001010001110100010000001101111001000000100100001100101"
    "0010000001101000011000010111001100100000011000010110001001100100"
    "0110100101100011011000010111010001100101011001000010000001000111"
    "0110111101110110011001010111001001101110011011010110010101101110"
    "0111010000100000011010000110010101110010011001010010110000100000"
    "0110001001111001001000000110010001100101011000110110110001100001"
    "0111001001101001011011100110011100100000011101010111001100100000"
    "0110111101110101011101000010000001101111011001100010000001101000"
    "0110100101110011000010100011101000100000001000000010000001010000"
    "0111001001101111011101000110010101100011011101000110100101101111"
    "0110111000100000011000010110111001100100001000000111011101100001"
    "0110011101101001011011100110011100100000010101110110000101110010"
    "0010000001100001011001110110000101101001011011100111001101110100"
    "0010000001110101011100110010111000001010001110100000101000111010"
    "0010000001101111001000000100100001100101001000000110100001100001"
    "0111001100100000011100000110110001110101011011100110010001100101"
    "0111001001100101011001000010000001101111011101010111001000100000"
    "0111001101100101011000010111001100101100001000000111001001100001"
    "0111011001100001011001110110010101100100001000000110111101110101"
    "0111001000100000010000110110111101100001011100110111010001110011"
    "0010110000100000011000100111010101110010011011100111010000100000"
    "0110111101110101011100100010000001110100011011110111011101101110"
    "0111001100101100000010100011101000100000001000000010000001100001"
    "0110111001100100001000000110010001100101011100110111010001110010"
    "0110111101111001011001010110010000100000011101000110100001100101"
    "0010000001101100011010010111011001100101011100110010000001101111"
    "0110011000100000011011110111010101110010001000000111000001100101"
    "0110111101110000011011000110010100101110001000000010000001001000"
    "0110010100100000011010010111001100100000011000010111010000100000"
    "0111010001101000011010010111001100100000011101000110100101101101"
    "0110010100001010001110100010000000100000001000000111010001110010"
    "0110000101101110011100110111000001101111011100100111010001101001"
    "0110111001100111001000000110110001100001011100100110011101100101"
    "0010000001000001011100100110110101101001011001010111001100100000"
    "0110111101100110001000000110011001101111011100100110010101101001"
    "0110011101101110001000000100110101100101011100100110001101100101"
    "0110111001100001011100100110100101100101011100110010000001110100"
    "0110111100100000011000110110111101101101011100000110110001100101"
    "0110000101110100001000000111010001101000011001010000101000111010"
    "0010000000100000001000000111011101101111011100100110101101110011"
    "0010000001101111011001100010000001100100011001010110000101110100"
    "0110100000101100001000000110010001100101011100110110111101101100"
    "0110000101110100011010010110111101101110001000000110000101101110"
    "0110010000100000011101000111100101110010011000010110111001101110"
    "0111100100101100001000000110000101101100011100100110010101100001"
    "0110010001111001001000000110001001100101011001110111010101101110"
    "0010000001110111011010010111010001101000000010100011101000100000"
    "0010000000100000011000110110100101110010011000110111010101101101"
    "0111001101110100011000010110111001100011011001010111001100100000"
    "0110111101100110001000000100001101110010011101010110010101101100"
    "0111010001111001001000000010011000100000011100000110010101110010"
    "0110011001101001011001000111100100100000011100110110001101100001"
    "0111001001100011011001010110110001111001001000000111000001100001"
    "0111001001100001011011000110110001100101011011000110010101100100"
    "0010000001101001011011100010000001110100011010000110010100100000"
    "0110110101101111011100110111010000001010001110100010000000100000"
    "0010000001100010011000010111001001100010011000010111001001101111"
    "0111010101110011001000000110000101100111011001010111001100101100"
    "0010000001100001011011100110010000100000011101000110111101110100"
    "0110000101101100011011000111100100100000011101010110111001110111"
    "0110111101110010011101000110100001111001001000000111010001101000"
    "0110010100100000010010000110010101100001011001000010000001101111"
    "0110011000100000011000010010000001100011011010010111011001101001"
    "0110110001101001011110100110010101100100000010100011101000100000"
    "0010000000100000011011100110000101110100011010010110111101101110"
    "0010111000001010001110100000101000111010001000000110111100100000"
    "0100100001100101001000000110100001100001011100110010000001100011"
    "0110111101101110011100110111010001110010011000010110100101101110"
    "0110010101100100001000000110111101110101011100100010000001100110"
    "0110010101101100011011000110111101110111001000000100001101101001"
    "0111010001101001011110100110010101101110011100110010000001110100"
    "0110000101101011011001010110111000100000010000110110000101110000"
    "0111010001101001011101100110010100100000011011110110111000100000"
    "0111010001101000011001010010000001101000011010010110011101101000"
    "0000101000111010001000000010000000100000010100110110010101100001"
    "0111001100100000011101000110111100100000011000100110010101100001"
    "0111001000100000010000010111001001101101011100110010000001100001"
    "0110011101100001011010010110111001110011011101000010000001110100"
    "0110100001100101011010010111001000100000010000110110111101110101"
    "0110111001110100011100100111100100101100001000000111010001101111"
    "0010000001100010011001010110001101101111011011010110010100100000"
    "0111010001101000011001010000101000111010001000000010000000100000"
    "0110010101111000011001010110001101110101011101000110100101101111"
    "0110111001100101011100100111001100100000011011110110011000100000"
    "0111010001101000011001010110100101110010001000000110011001110010"
    "0110100101100101011011100110010001110011001000000110000101101110"
    "0110010000100000010000100111001001100101011101000110100001110010"
    "0110010101101110001011000010000001101111011100100010000001110100"
    "0110111100100000011001100110000101101100011011000010000001110100"
    "0110100001100101011011010111001101100101011011000111011001100101"
    "0111001100001010001110100010000000100000001000000110001001111001"
    "0010000001110100011010000110010101101001011100100010000001001000"
    "0110000101101110011001000111001100101110000010100011101000001010"
    "0011101000100000011011110010000001001000011001010010000001101000"
    "0110000101110011001000000110010101111000011000110110100101110100"
    "0110010101100100001000000110010001101111011011010110010101110011"
    "0111010001101001011000110010000001101001011011100111001101110101"
    "0111001001110010011001010110001101110100011010010110111101101110"
    "0111001100100000011000010110110101101111011011100110011101110011"
    "0111010000100000011101010111001100101100001000000110000101101110"
    "0110010000100000011010000110000101110011000010100011101000100000"
    "0010000000100000011001010110111001100100011001010110000101110110"
    "0110111101110101011100100110010101100100001000000111010001101111"
    "0010000001100010011100100110100101101110011001110010000001101111"
    "0110111000100000011101000110100001100101001000000110100101101110"
    "0110100001100001011000100110100101110100011000010110111001110100"
    "0111001100100000011011110110011000100000011011110111010101110010"
    "0010000001100110011100100110111101101110011101000110100101100101"
    "0111001001110011001011000010000001110100011010000110010100001010"
    "0011101000100000001000000010000001101101011001010111001001100011"
    "0110100101101100011001010111001101110011001000000100100101101110"
    "0110010001101001011000010110111000100000010100110110000101110110"
    "0110000101100111011001010111001100101100001000000111011101101000"
    "0110111101110011011001010010000001101011011011100110111101110111"
    "0110111000100000011100100111010101101100011001010010000001101111"
    "0110011000100000011101110110000101110010011001100110000101110010"
    "0110010100101100001000000110100101110011001000000110000101101110"
    "0000101000111010001000000010000000100000011101010110111001100100"
    "0110100101110011011101000110100101101110011001110111010101101001"
    "0111001101101000011001010110010000100000011001000110010101110011"
    "0111010001110010011101010110001101110100011010010110111101101110"
    "0010000001101111011001100010000001100001011011000110110000100000"
    "0110000101100111011001010111001100101100001000000111001101100101"
    "0111100001100101011100110010000001100001011011100110010000100000"
    "0110001101101111011011100110010001101001011101000110100101101111"
    "0110111001110011001011100000101000001010010010010110111000100000"
    "0110010101110110011001010111001001111001001000000111001101110100"
    "0110000101100111011001010010000001101111011001100010000001110100"
    "0110100001100101011100110110010100100000010011110111000001110000"
    "0111001001100101011100110111001101101001011011110110111001110011"
    "0010000001010111011001010010000001101000011000010111011001100101"
    "0010000001010000011001010111010001101001011101000110100101101111"
    "0110111001100101011001000010000001100110011011110111001000100000"
    "0101001001100101011001000111001001100101011100110111001100100000"
    "0110100101101110000010100111010001101000011001010010000001101101"
    "0110111101110011011101000010000001101000011101010110110101100010"
    "0110110001100101001000000111010001100101011100100110110101110011"
    "0011101000100000010011110111010101110010001000000111001001100101"
    "0111000001100101011000010111010001100101011001000010000001010000"
    "0110010101110100011010010111010001101001011011110110111001110011"
    "0010000001101000011000010111011001100101001000000110001001100101"
    "0110010101101110001000000110000101101110011100110111011101100101"
    "0111001001100101011001000010000001101111011011100110110001111001"
    "0000101001100010011110010010000001110010011001010111000001100101"
    "0110000101110100011001010110010000100000011010010110111001101010"
    "0111010101110010011110010010111000100000001000000100000100100000"
    "0101000001110010011010010110111001100011011001010010000001110111"
    "0110100001101111011100110110010100100000011000110110100001100001"
    "0111001001100001011000110111010001100101011100100010000001101001"
    "0111001100100000011101000110100001110101011100110010000001101101"
    "0110000101110010011010110110010101100100001000000110001001111001"
    "0010000001100101011101100110010101110010011110010000101001100001"
    "0110001101110100001000000111011101101000011010010110001101101000"
    "0010000001101101011000010111100100100000011001000110010101100110"
    "0110100101101110011001010010000001100001001000000101010001111001"
    "0111001001100001011011100111010000101100001000000110100101110011"
    "0010000001110101011011100110011001101001011101000010000001110100"
    "0110111100100000011000100110010100100000011101000110100001100101"
    "0010000001110010011101010110110001100101011100100010000001101111"
    "0110011000100000011000010010000001100110011100100110010101100101"
    "0000101001110000011001010110111101110000011011000110010100101110"
    "0000101000001010010011100110111101110010001000000110100001100001"
    "0111011001100101001000000101011101100101001000000110001001100101"
    "0110010101101110001000000111011101100001011011100111010001101001"
    "0110111001100111001000000110100101101110001000000110000101110100"
    "0111010001100101011011100111010001101001011011110110111001110011"
    "0010000001110100011011110010000001101111011101010111001000100000"
    "0100001001110010011010010111010001110100011010010111001101101000"
    "0010000001100010011100100110010101110100011010000111001001100101"
    "0110111000101110001000000010000001010111011001010000101001101000"
    "0110000101110110011001010010000001110111011000010111001001101110"
    "0110010101100100001000000111010001101000011001010110110100100000"
    "0110011001110010011011110110110100100000011101000110100101101101"
    "0110010100100000011101000110111100100000011101000110100101101101"
    "0110010100100000011011110110011000100000011000010111010001110100"
    "0110010101101101011100000111010001110011001000000110001001111001"
    "0010000001110100011010000110010101101001011100100010000001101100"
    "0110010101100111011010010111001101101100011000010111010001110101"
    "0111001001100101000010100111010001101111001000000110010101111000"
    "0111010001100101011011100110010000100000011000010110111000100000"
    "0111010101101110011101110110000101110010011100100110000101101110"
    "0111010001100001011000100110110001100101001000000110101001110101"
    "0111001001101001011100110110010001101001011000110111010001101001"
    "0110111101101110001000000110111101110110011001010111001000100000"
    "0111010101110011001011100010000000100000010101110110010100100000"
    "0110100001100001011101100110010100100000011100100110010101101101"
    "0110100101101110011001000110010101100100000010100111010001101000"
    "0110010101101101001000000110111101100110001000000111010001101000"
    "0110010100100000011000110110100101110010011000110111010101101101"
    "0111001101110100011000010110111001100011011001010111001100100000"
    "0110111101100110001000000110111101110101011100100010000001100101"
    "0110110101101001011001110111001001100001011101000110100101101111"
    "0110111000100000011000010110111001100100001000000111001101100101"
    "0111010001110100011011000110010101101101011001010110111001110100"
    "0010000001101000011001010111001001100101001011100010000000100000"
    "0101011101100101000010100110100001100001011101100110010100100000"
    "0110000101110000011100000110010101100001011011000110010101100100"
    "0010000001110100011011110010000001110100011010000110010101101001"
    "0111001000100000011011100110000101110100011010010111011001100101"
    "0010000001101010011101010111001101110100011010010110001101100101"
    "0010000001100001011011100110010000100000011011010110000101100111"
    "0110111001100001011011100110100101101101011010010111010001111001"
    "0010110000100000011000010110111001100100001000000111011101100101"
    "0010000001101000011000010111011001100101000010100110001101101111"
    "0110111001101010011101010111001001100101011001000010000001110100"
    "0110100001100101011011010010000001100010011110010010000001110100"
    "0110100001100101001000000111010001101001011001010111001100100000"
    "0110111101100110001000000110111101110101011100100010000001100011"
    "0110111101101101011011010110111101101110001000000110101101101001"
    "0110111001100100011100100110010101100100001000000111010001101111"
    "0010000001100100011010010111001101100001011101100110111101110111"
    "0010000001110100011010000110010101110011011001010000101001110101"
    "0111001101110101011100100111000001100001011101000110100101101111"
    "0110111001110011001011000010000001110111011010000110100101100011"
    "0110100000101100001000000111011101101111011101010110110001100100"
    "0010000001101001011011100110010101110110011010010111010001100001"
    "0110001001101100011110010010000001101001011011100111010001100101"
    "0111001001110010011101010111000001110100001000000110111101110101"
    "0111001000100000011000110110111101101110011011100110010101100011"
    "0111010001101001011011110110111001110011001000000110000101101110"
    "0110010000001010011000110110111101110010011100100110010101110011"
    "0111000001101111011011100110010001100101011011100110001101100101"
    "0010111000100000001000000101010001101000011001010111100100100000"
    "0111010001101111011011110010000001101000011000010111011001100101"
    "0010000001100010011001010110010101101110001000000110010001100101"
    "0110000101100110001000000111010001101111001000000111010001101000"
    "0110010100100000011101100110111101101001011000110110010100100000"
    "0110111101100110001000000110101001110101011100110111010001101001"
    "0110001101100101001000000110000101101110011001000000101001101111"
    "0110011000100000011000110110111101101110011100110110000101101110"
    "0110011101110101011010010110111001101001011101000111100100101110"
    "0010000000100000010101110110010100100000011011010111010101110011"
    "0111010000101100001000000111010001101000011001010111001001100101"
    "0110011001101111011100100110010100101100001000000110000101100011"
    "0111000101110101011010010110010101110011011000110110010100100000"
    "0110100101101110001000000111010001101000011001010010000001101110"
    "0110010101100011011001010111001101110011011010010111010001111001"
    "0010110000001010011101110110100001101001011000110110100000100000"
    "0110010001100101011011100110111101110101011011100110001101100101"
    "0111001100100000011011110111010101110010001000000101001101100101"
    "0111000001100001011100100110000101110100011010010110111101101110"
    "0010110000100000011000010110111001100100001000000110100001101111"
    "0110110001100100001000000111010001101000011001010110110100101100"
    "0010000001100001011100110010000001110111011001010010000001101000"
    "0110111101101100011001000010000001110100011010000110010100100000"
    "0111001001100101011100110111010000100000011011110110011000001010"
    "0110110101100001011011100110101101101001011011100110010000101100"
    "0010000001000101011011100110010101101101011010010110010101110011"
    "0010000001101001011011100010000001010111011000010111001000101100"
    "0010000001101001011011100010000001010000011001010110000101100011"
    "0110010100100000010001100111001001101001011001010110111001100100"
    "0111001100101110000010100000101001010111011001010010110000100000"
    "0111010001101000011001010111001001100101011001100110111101110010"
    "0110010100101100001000000111010001101000011001010010000001010010"
    "0110010101110000011100100110010101110011011001010110111001110100"
    "0110000101110100011010010111011001100101011100110010000001101111"
    "0110011000100000011101000110100001100101001000000111010101101110"
    "0110100101110100011001010110010000100000010100110111010001100001"
    "0111010001100101011100110010000001101111011001100010000001000001"
    "0110110101100101011100100110100101100011011000010010110000001010"
    "0110100101101110001000000100011101100101011011100110010101110010"
    "0110000101101100001000000100001101101111011011100110011101110010"
    "0110010101110011011100110010110000100000010000010111001101110011"
    "0110010101101101011000100110110001100101011001000010110000100000"
    "0110000101110000011100000110010101100001011011000110100101101110"
    "0110011100100000011101000110111100100000011101000110100001100101"
    "0010000001010011011101010111000001110010011001010110110101100101"
    "0010000001001010011101010110010001100111011001010010000001101111"
    "0110011000100000011101000110100001100101000010100111011101101111"
    "0111001001101100011001000010000001100110011011110111001000100000"
    "0111010001101000011001010010000001110010011001010110001101110100"
    "0110100101110100011101010110010001100101001000000110111101100110"
    "0010000001101111011101010111001000100000011010010110111001110100"
    "0110010101101110011101000110100101101111011011100111001100101100"
    "0010000001100100011011110010110000100000011010010110111000100000"
    "0111010001101000011001010010000001001110011000010110110101100101"
    "0010110000100000011000010110111001100100001000000110001001111001"
    "0000101001000001011101010111010001101000011011110111001001101001"
    "0111010001111001001000000110111101100110001000000111010001101000"
    "0110010100100000011001110110111101101111011001000010000001010000"
    "0110010101101111011100000110110001100101001000000110111101100110"
    "0010000001110100011010000110010101110011011001010010000001000011"
    "0110111101101100011011110110111001101001011001010111001100101100"
    "0010000001110011011011110110110001100101011011010110111001101100"
    "0111100100100000011100000111010101100010011011000110100101110011"
    "0110100000100000011000010110111001100100000010100110010001100101"
    "0110001101101100011000010111001001100101001011000010000001010100"
    "0110100001100001011101000010000001110100011010000110010101110011"
    "0110010100100000010101010110111001101001011101000110010101100100"
    "0010000001000011011011110110110001101111011011100110100101100101"
    "0111001100100000011000010111001001100101001011000010000001100001"
    "0110111001100100001000000110111101100110001000000101001001101001"
    "0110011101101000011101000010000001101111011101010110011101101000"
    "0111010000100000011101000110111100100000011000100110010100001010"
    "0100011001110010011001010110010100100000011000010110111001100100"
    "0010000001001001011011100110010001100101011100000110010101101110"
    "0110010001100101011011100111010000100000010100110111010001100001"
    "0111010001100101011100110011101100100000011101000110100001100001"
    "0111010000100000011101000110100001100101011110010010000001100001"
    "0111001001100101001000000100000101100010011100110110111101101100"
    "0111011001100101011001000010000001100110011100100110111101101101"
    "0010000001100001011011000110110000001010010000010110110001101100"
    "0110010101100111011010010110000101101110011000110110010100100000"
    "0111010001101111001000000111010001101000011001010010000001000010"
    "0111001001101001011101000110100101110011011010000010000001000011"
    "0111001001101111011101110110111000101100001000000110000101101110"
    "0110010000100000011101000110100001100001011101000010000001100001"
    "0110110001101100001000000111000001101111011011000110100101110100"
    "0110100101100011011000010110110000100000011000110110111101101110"
    "0110111001100101011000110111010001101001011011110110111000001010"
    "0110001001100101011101000111011101100101011001010110111000100000"
    "0111010001101000011001010110110100100000011000010110111001100100"
    "0010000001110100011010000110010100100000010100110111010001100001"
    "0111010001100101001000000110111101100110001000000100011101110010"
    "0110010101100001011101000010000001000010011100100110100101110100"
    "0110000101101001011011100010110000100000011010010111001100100000"
    "0110000101101110011001000010000001101111011101010110011101101000"
    "0111010000100000011101000110111100100000011000100110010100001010"
    "0111010001101111011101000110000101101100011011000111100100100000"
    "0110010001101001011100110111001101101111011011000111011001100101"
    "0110010000111011001000000110000101101110011001000010000001110100"
    "0110100001100001011101000010000001100001011100110010000001000110"
    "0111001001100101011001010010000001100001011011100110010000100000"
    "0100100101101110011001000110010101110000011001010110111001100100"
    "0110010101101110011101000010000001010011011101000110000101110100"
    "0110010101110011001011000010000001110100011010000110010101111001"
    "0010000001101000011000010111011001100101000010100110011001110101"
    "0110110001101100001000000101000001101111011101110110010101110010"
    "0010000001110100011011110010000001101100011001010111011001111001"
    "0010000001010111011000010111001000101100001000000110001101101111"
    "0110111001100011011011000111010101100100011001010010000001010000"
    "0110010101100001011000110110010100101100001000000110001101101111"
    "0110111001110100011100100110000101100011011101000010000001000001"
    "0110110001101100011010010110000101101110011000110110010101110011"
#ifndef BSLS_PLATFORM_OS_WINDOWS
    // Windows disallows "strings" longer than 65,535 bytes.
    "0010110000100000011001010111001101110100011000010110001001101100"
    "0110100101110011011010000000101001000011011011110110110101101101"
    "0110010101110010011000110110010100101100001000000110000101101110"
    "0110010000100000011101000110111100100000011001000110111100100000"
    "0110000101101100011011000010000001101111011101000110100001100101"
    "0111001000100000010000010110001101110100011100110010000001100001"
    "0110111001100100001000000101010001101000011010010110111001100111"
    "0111001100100000011101110110100001101001011000110110100000100000"
    "0100100101101110011001000110010101110000011001010110111001100100"
    "0110010101101110011101000000101001010011011101000110000101110100"
    "0110010101110011001000000110110101100001011110010010000001101111"
    "0110011000100000011100100110100101100111011010000111010000100000"
    "0110010001101111001011100010000000100000010000010110111001100100"
    "0010000001100110011011110111001000100000011101000110100001100101"
    "0010000001110011011101010111000001110000011011110111001001110100"
    "0010000001101111011001100010000001110100011010000110100101110011"
    "0010000001000100011001010110001101101100011000010111001001100001"
    "0111010001101001011011110110111000101100000010100111011101101001"
    "0111010001101000001000000110000100100000011001100110100101110010"
    "0110110100100000011100100110010101101100011010010110000101101110"
    "0110001101100101001000000110111101101110001000000111010001101000"
    "0110010100100000011100000111001001101111011101000110010101100011"
    "0111010001101001011011110110111000100000011011110110011000100000"
    "0110010001101001011101100110100101101110011001010010000001010000"
    "0111001001101111011101100110100101100100011001010110111001100011"
    "0110010100101100001000000111011101100101000010100110110101110101"
    "0111010001110101011000010110110001101100011110010010000001110000"
    "0110110001100101011001000110011101100101001000000111010001101111"
    "0010000001100101011000010110001101101000001000000110111101110100"
    "0110100001100101011100100010000001101111011101010111001000100000"
    "0100110001101001011101100110010101110011001011000010000001101111"
    "0111010101110010001000000100011001101111011100100111010001110101"
    "0110111001100101011100110010000001100001011011100110010000100000"
    "0110111101110101011100100010000001110011011000010110001101110010"
    "0110010101100100000010100100100001101111011011100110111101110010"
    "0010111000001010"
#endif
    ;

const std::size_t  HAYSTACK_BINARY_LENGTH = sizeof u_haystackBinary - 1;
const char * const HAYSTACK_BINARY_FIRST  = u_haystackBinary;
const char * const HAYSTACK_BINARY_LAST   = u_haystackBinary
                                          + HAYSTACK_BINARY_LENGTH;

// Test a short haystack where the time to construct metadata of a
// "sophisticated" method may exceed the time to search by the "naive" method.

static const char u_haystackShort[] =
    " When in the Course of human events,";

const std::size_t  HAYSTACK_SHORT_LENGTH = sizeof u_haystackShort - 1;
const char * const HAYSTACK_SHORT_FIRST  = u_haystackShort;
const char * const HAYSTACK_SHORT_LAST   = u_haystackShort
                                         + HAYSTACK_SHORT_LENGTH;

typedef struct DATA {
    int         d_line;
    bool        d_expected;
    const char *d_needle_p;
} DATA_t;

const static DATA_t U_DATA_TEXT[] = {
    //LINE  EXP  PATTERN
    //----  ---  --------------------------------------------------------------
  // Phrases in Document
    { L_,  true, "Allegiance to the British Crown"
    }
  , { L_,  true, "Cruelty & perfidy"
    }
  , { L_,  true, "Governors"
    }
  , { L_,  true, "Honor"
    }
  , { L_,  true, "Independent"
    }
  , { L_,  true, "Judiciary"
    }
  , { L_,  true, "Offices"
    }
  , { L_,  true, "Taxes"
    }
  , { L_,  true, "Trade"
    }
  , { L_,  true, "Tyrant"
    }
  , { L_,  true, "endeavoured"
    }
  , { L_,  true, "evident"
    }
  , { L_,  true, "foreign Mercenaries"
    }
  , { L_,  true, "formidable"
    }
  , { L_,  true, "neighbouring"
    }
  , { L_,  true, "our Brittish brethren."
    }
  , { L_,  true, "unalienable"
    }

  // Punctuation in Document
  , { L_,  true, ": o "
    }
  , { L_,  true, "\n"
    }
  , { L_,  true, "\n\n"
    }
  , { L_,  true, " "
    }
  , { L_,  true, "--"
    }
  , { L_,  true, "self-evident"
    }
  , { L_,  true, ""
    }

  // Phrases/Punctuation Not in Document
  , { L_, false, "%"
    }
  , { L_, false, "BAZINGA!"
    }
  , { L_, false, "One small step for man ..."
    }
  , { L_, false, "Barack Obama"
    }
};

const std::size_t NUM_U_DATA_TEXT = sizeof  U_DATA_TEXT
                                  / sizeof *U_DATA_TEXT;

const static DATA_t U_DATA_TEXT_PLUS[] = {
    //LINE  EXP  PATTERN
    //----  ---  -------------------
    { L_,  true, HAYSTACK_TEXT_FIRST
    }
  , { L_,  true, HAYSTACK_TEXT_FIRST
    }
  , { L_,  true, HAYSTACK_TEXT_FIRST
    }
  , { L_,  true, HAYSTACK_TEXT_FIRST
    }
  , { L_,  true, HAYSTACK_TEXT_FIRST
    }
  , { L_,  true, HAYSTACK_TEXT_FIRST
    }
  , { L_,  true, HAYSTACK_TEXT_FIRST
    }
  , { L_,  true, HAYSTACK_TEXT_FIRST
    }
  , { L_,  true, HAYSTACK_TEXT_FIRST
    }
  , { L_,  true, HAYSTACK_TEXT_FIRST
    }
  , { L_,  true, HAYSTACK_TEXT_FIRST
    }
  , { L_,  true, HAYSTACK_TEXT_FIRST
    }
  , { L_,  true, HAYSTACK_TEXT_FIRST
    }
  , { L_,  true, HAYSTACK_TEXT_FIRST
    }
  , { L_,  true, HAYSTACK_TEXT_FIRST
    }
  , { L_,  true, HAYSTACK_TEXT_FIRST
    }
  , { L_,  true, HAYSTACK_TEXT_FIRST
    }
  , { L_,  true, HAYSTACK_TEXT_FIRST
    }
  , { L_,  true, HAYSTACK_TEXT_FIRST
    }
  , { L_,  true, HAYSTACK_TEXT_FIRST
    }
  , { L_,  true, HAYSTACK_TEXT_FIRST
    }
  , { L_,  true, HAYSTACK_TEXT_FIRST
    }
  , { L_,  true, HAYSTACK_TEXT_FIRST
    }
  , { L_,  true, HAYSTACK_TEXT_FIRST
    }
  , { L_,  true, HAYSTACK_TEXT_FIRST
    }
  , { L_,  true, HAYSTACK_TEXT_FIRST
    }
  , { L_,  true, HAYSTACK_TEXT_FIRST
    }
  , { L_,  true, HAYSTACK_TEXT_FIRST
    }
};

const std::size_t NUM_U_DATA_TEXT_PLUS = sizeof  U_DATA_TEXT_PLUS
                                       / sizeof *U_DATA_TEXT_PLUS;

const static DATA_t U_DATA_OCTAL[] = {
    { L_,  true, // "Allegiance to the British Crown"
                    "101154154145147151141156143145040164157040164150145040102"
                    "162151164151163150040103162157167156"
    }
  , { L_,  true, // "Cruelty & perfidy"
                    "103162165145154164171040046040160145162146151144171"
    }
  , { L_,  true, // "Governors"
                    "107157166145162156157162163"
    }
  , { L_,  true, // "Honor"
                    "110157156157162"
    }
  , { L_,  true, // "Independent"
                    "111156144145160145156144145156164"
    }
  , { L_,  true, // "Judiciary"
                     "112165144151143151141162171"
    }
  , { L_,  true, // "Offices"
                    "117146146151143145163"
    }
  , { L_,  true, // "Taxes"
                    "124141170145163"
    }
  , { L_,  true, // "Trade"
                    "124162141144145"
    }
  , { L_,  true, // "Tyrant"
                    "124171162141156164"
    }
  , { L_,  true, // "endeavoured"
                    "145156144145141166157165162145144"
    }
  , { L_,  true, // "evident"
                    "145166151144145156164"
    }
  , { L_,  true, // "foreign Mercenaries"
                    "146157162145151147156040115145162143145156141162151145163"
    }
  , { L_,  true, // "formidable"
                    "146157162155151144141142154145"
    }
  , { L_,  true, // "neighbouring"
                    "156145151147150142157165162151156147"
    }
  , { L_,  true, // "our Brittish brethren."
                    "157165162040102162151164164151163150040142162145164150162"
                    "145156056"
    }
  , { L_,  true, // "unalienable"
                    "165156141154151145156141142154145"
    }
  , { L_,  true, // ": o "
                    "072040157040"
    }
  , { L_,  true, // "\n"
                    "012"
    }
  , { L_,  true, // "\n\n"
                    "012012"
    }
  , { L_,  true, // " "
                    "040"
    }
  , { L_,  true, // "--"
                    "055055"
    }
  , { L_,  true, // "self-evident"
                    "163145154146055145166151144145156164"
    }
  , { L_,  true, // ""
                    ""
    }
  , { L_, false, // "%"
                    "045"
    }
  , { L_, false, // "BAZINGA!"
                    "102101132111116107101041"
    }
  , { L_, false, // "One small step for man ..."
                    "117156145040163155141154154040163164145160040146157162040"
                    "155141156040056056056"
    }
  , { L_, false, // "Barack Obama"
                    "102141162141143153040117142141155141"
    }
};

const std::size_t NUM_U_DATA_OCTAL = sizeof  U_DATA_OCTAL
                                   / sizeof *U_DATA_OCTAL;

const static DATA_t U_DATA_OCTAL_PLUS[] = {
    { L_,  true, HAYSTACK_OCTAL_FIRST
    }
  , { L_,  true, HAYSTACK_OCTAL_FIRST
    }
  , { L_,  true, HAYSTACK_OCTAL_FIRST
    }
  , { L_,  true, HAYSTACK_OCTAL_FIRST
    }
  , { L_,  true, HAYSTACK_OCTAL_FIRST
    }
  , { L_,  true, HAYSTACK_OCTAL_FIRST
    }
  , { L_,  true, HAYSTACK_OCTAL_FIRST
    }
  , { L_,  true, HAYSTACK_OCTAL_FIRST
    }
  , { L_,  true, HAYSTACK_OCTAL_FIRST
    }
  , { L_,  true, HAYSTACK_OCTAL_FIRST
    }
  , { L_,  true, HAYSTACK_OCTAL_FIRST
    }
  , { L_,  true, HAYSTACK_OCTAL_FIRST
    }
  , { L_,  true, HAYSTACK_OCTAL_FIRST
    }
  , { L_,  true, HAYSTACK_OCTAL_FIRST
    }
  , { L_,  true, HAYSTACK_OCTAL_FIRST
    }
  , { L_,  true, HAYSTACK_OCTAL_FIRST
    }
  , { L_,  true, HAYSTACK_OCTAL_FIRST
    }
  , { L_,  true, HAYSTACK_OCTAL_FIRST
    }
  , { L_,  true, HAYSTACK_OCTAL_FIRST
    }
  , { L_,  true, HAYSTACK_OCTAL_FIRST
    }
  , { L_,  true, HAYSTACK_OCTAL_FIRST
    }
  , { L_,  true, HAYSTACK_OCTAL_FIRST
    }
  , { L_,  true, HAYSTACK_OCTAL_FIRST
    }
  , { L_,  true, HAYSTACK_OCTAL_FIRST
    }
  , { L_,  true, HAYSTACK_OCTAL_FIRST
    }
  , { L_,  true, HAYSTACK_OCTAL_FIRST
    }
  , { L_,  true, HAYSTACK_OCTAL_FIRST
    }
  , { L_,  true, HAYSTACK_OCTAL_FIRST
    }
};

const std::size_t NUM_U_DATA_OCTAL_PLUS = sizeof  U_DATA_OCTAL_PLUS
                                        / sizeof *U_DATA_OCTAL_PLUS;

const static DATA_t U_DATA_BINARY[] = {
    { L_,  true, // "Allegiance to the British Crown"
                    "010000010110110001101100011001010110011101101001"
                    "011000010110111001100011011001010010000001110100"
                    "011011110010000001110100011010000110010100100000"
                    "010000100111001001101001011101000110100101110011"
                    "011010000010000001000011011100100110111101110111"
                    "01101110"
    }
  , { L_,  true, // "Cruelty & perfidy"
                    "010000110111001001110101011001010110110001110100"
                    "011110010010000000100110001000000111000001100101"
                    "0111001001100110011010010110010001111001"
    }
  , { L_,  true, // "Governors"
                    "010001110110111101110110011001010111001001101110"
                    "011011110111001001110011"
    }

#ifndef BSLS_PLATFORM_OS_WINDOWS
    // Windows disallows "strings" longer than 65,535 bytes.  The word "Honor",
    // the final word of the document, is not found in the shorted text used
    // on the Windows platform

  , { L_,  true, // "Honor"
                    "0100100001101111011011100110111101110010"
    }
#endif

  , { L_,  true, // "Independent"
                    "010010010110111001100100011001010111000001100101"
                    "0110111001100100011001010110111001110100"
    }
  , { L_,  true, // "Judiciary"
                    "010010100111010101100100011010010110001101101001"
                    "011000010111001001111001"
    }
  , { L_,  true, // "Offices"
                    "010011110110011001100110011010010110001101100101"
                    "01110011"
    }
  , { L_,  true, // "Taxes"
                    "0101010001100001011110000110010101110011"
    }
  , { L_,  true, // "Trade"
                    "0101010001110010011000010110010001100101"
    }
  , { L_,  true, // "Tyrant"
                    "010101000111100101110010011000010110111001110100"
    }
  , { L_,  true, // "endeavoured"
                    "011001010110111001100100011001010110000101110110"
                    "0110111101110101011100100110010101100100"
    }
  , { L_,  true, // "evident"
                    "011001010111011001101001011001000110010101101110"
                    "01110100"
    }
  , { L_,  true, // "foreign Mercenaries"
                    "011001100110111101110010011001010110100101100111"
                    "011011100010000001001101011001010111001001100011"
                    "011001010110111001100001011100100110100101100101"
                    "01110011"
    }
  , { L_,  true, // "formidable"
                    "011001100110111101110010011011010110100101100100"
                    "01100001011000100110110001100101"
    }
  , { L_,  true, // "neighbouring"
                    "011011100110010101101001011001110110100001100010"
                    "011011110111010101110010011010010110111001100111"
    }
  , { L_,  true, // "our Brittish brethren."
                    "011011110111010101110010001000000100001001110010"
                    "011010010111010001110100011010010111001101101000"
                    "001000000110001001110010011001010111010001101000"
                    "01110010011001010110111000101110"
    }
  , { L_,  true, // "unalienable"
                    "011101010110111001100001011011000110100101100101"
                    "0110111001100001011000100110110001100101"
    }
  , { L_,  true, // ": o "
                    "00111010001000000110111100100000"
    }
  , { L_,  true, // "\n"
                    "00001010"
    }
  , { L_,  true, // "\n\n"
                    "0000101000001010"
    }
  , { L_,  true, // " "
                    "00100000"
    }
  , { L_,  true, // "--"
                    "0010110100101101"
    }
  , { L_,  true, // "self-evident"
                    "011100110110010101101100011001100010110101100101"
                    "011101100110100101100100011001010110111001110100"
    }
  , { L_,  true, // ""
                    ""
    }
  , { L_, true, // "%"    // Found since search crosses byte boundaries.
                    "00100101"
    }
  , { L_, false, // "BAZINGA!"
                    "010000100100000101011010010010010100111001000111"
                    "0100000100100001"
    }
  , { L_, false, // "One small step for man ..."
                    "010011110110111001100101001000000111001101101101"
                    "011000010110110001101100001000000111001101110100"
                    "011001010111000000100000011001100110111101110010"
                    "001000000110110101100001011011100010000000101110"
                    "0010111000101110"
    }
  , { L_, false, // "Barack Obama"
                    "010000100110000101110010011000010110001101101011"
                    "001000000100111101100010011000010110110101100001"
    }
};

const std::size_t   NUM_U_DATA_BINARY = sizeof  U_DATA_BINARY
                                      / sizeof *U_DATA_BINARY;

const static DATA_t U_DATA_BINARY_PLUS[] = {
    { L_,  true, HAYSTACK_BINARY_FIRST
    }
  , { L_,  true, HAYSTACK_BINARY_FIRST
    }
  , { L_,  true, HAYSTACK_BINARY_FIRST
    }
  , { L_,  true, HAYSTACK_BINARY_FIRST
    }
  , { L_,  true, HAYSTACK_BINARY_FIRST
    }
  , { L_,  true, HAYSTACK_BINARY_FIRST
    }
  , { L_,  true, HAYSTACK_BINARY_FIRST
    }
  , { L_,  true, HAYSTACK_BINARY_FIRST
    }
  , { L_,  true, HAYSTACK_BINARY_FIRST
    }
  , { L_,  true, HAYSTACK_BINARY_FIRST
    }
  , { L_,  true, HAYSTACK_BINARY_FIRST
    }
  , { L_,  true, HAYSTACK_BINARY_FIRST
    }
  , { L_,  true, HAYSTACK_BINARY_FIRST
    }
  , { L_,  true, HAYSTACK_BINARY_FIRST
    }
  , { L_,  true, HAYSTACK_BINARY_FIRST
    }
  , { L_,  true, HAYSTACK_BINARY_FIRST
    }
  , { L_,  true, HAYSTACK_BINARY_FIRST
    }
  , { L_,  true, HAYSTACK_BINARY_FIRST
    }
  , { L_,  true, HAYSTACK_BINARY_FIRST
    }
  , { L_,  true, HAYSTACK_BINARY_FIRST
    }
  , { L_,  true, HAYSTACK_BINARY_FIRST
    }
  , { L_,  true, HAYSTACK_BINARY_FIRST
    }
  , { L_,  true, HAYSTACK_BINARY_FIRST
    }
  , { L_,  true, HAYSTACK_BINARY_FIRST
    }
  , { L_,  true, HAYSTACK_BINARY_FIRST
    }
  , { L_,  true, HAYSTACK_BINARY_FIRST
    }
  , { L_,  true, HAYSTACK_BINARY_FIRST
    }
  , { L_,  true, HAYSTACK_BINARY_FIRST
    }
};

const std::size_t   NUM_U_DATA_BINARY_PLUS = sizeof  U_DATA_BINARY_PLUS
                                           / sizeof *U_DATA_BINARY_PLUS;

//    " When in the Course of human events,"

const static DATA_t U_DATA_SHORT[] = {
    //LINE  EXP   PATTERN
    //----  ---   --------

  // Words in Document
    { L_,  true, "When"   }
  , { L_,  true, "in"     }
  , { L_,  true, "the"    }
  , { L_,  true, "Course" }
  , { L_,  true, "of"     }
  , { L_,  true, "human"  }
  , { L_,  true, "events" }

  // Sub-Words in Document
  , { L_,  true, "hen"    }
  , { L_,  true, "Cour"   }
  , { L_,  true, "our"    }
  , { L_,  true, "ur"     }
  , { L_,  true, "man"    }
  , { L_,  true, "eve"    }
  , { L_,  true, "even"   }
  , { L_,  true, "event"  }
  , { L_,  true, "vents"  }
  , { L_,  true, "he"     }
  , { L_,  true, "hum"    }

  // Punctuation in Document
  , { L_,  true, ""       }
  , { L_,  true, " "      }
  , { L_,  true, ","      }

  // Phrases/Punctuation Not in Document
  , { L_, false, "%"      }
  , { L_, false, "!!"     }
  , { L_, false, "USA"    }
  , { L_, false, "1776"   }
};

const std::size_t NUM_U_DATA_SHORT = sizeof  U_DATA_SHORT
                                   / sizeof *U_DATA_SHORT;

const static DATA_t U_DATA_SHORT_PLUS[] = {
    //LINE  EXP   PATTERN
    //----  ---   --------

  // Words in Document
    { L_,  true, HAYSTACK_SHORT_FIRST }
  , { L_,  true, HAYSTACK_SHORT_FIRST }
  , { L_,  true, HAYSTACK_SHORT_FIRST }
  , { L_,  true, HAYSTACK_SHORT_FIRST }
  , { L_,  true, HAYSTACK_SHORT_FIRST }
  , { L_,  true, HAYSTACK_SHORT_FIRST }
  , { L_,  true, HAYSTACK_SHORT_FIRST }

  // Sub-Words in Document
  , { L_,  true, HAYSTACK_SHORT_FIRST }
  , { L_,  true, HAYSTACK_SHORT_FIRST }
  , { L_,  true, HAYSTACK_SHORT_FIRST }
  , { L_,  true, HAYSTACK_SHORT_FIRST }
  , { L_,  true, HAYSTACK_SHORT_FIRST }
  , { L_,  true, HAYSTACK_SHORT_FIRST }
  , { L_,  true, HAYSTACK_SHORT_FIRST }
  , { L_,  true, HAYSTACK_SHORT_FIRST }
  , { L_,  true, HAYSTACK_SHORT_FIRST }
  , { L_,  true, HAYSTACK_SHORT_FIRST }
  , { L_,  true, HAYSTACK_SHORT_FIRST }

  // Punctuation in Document
  , { L_,  true, HAYSTACK_SHORT_FIRST }
  , { L_,  true, HAYSTACK_SHORT_FIRST }
  , { L_,  true, HAYSTACK_SHORT_FIRST }

  // Phrases/Punctuation Not in Document
  , { L_,  true, HAYSTACK_SHORT_FIRST }
  , { L_,  true, HAYSTACK_SHORT_FIRST }
  , { L_,  true, HAYSTACK_SHORT_FIRST }
  , { L_,  true, HAYSTACK_SHORT_FIRST }
};

const std::size_t NUM_U_DATA_SHORT_PLUS = sizeof  U_DATA_SHORT_PLUS
                                        / sizeof *U_DATA_SHORT_PLUS;

// Correctness Tests

const struct {
    int          d_line;
    const char  *d_haystack_p;
    const char  *d_needle_p;

    // result: Case Sensitive
    int          d_offsetCs;
    int          d_lengthCs;

    // result: Case Insensitive
    int          d_offsetCi;
    int          d_lengthCi;

} DATA[]  = {
    //LINE HAYSTACK     NEEDLE  EXP CS  EXP CI
    //---- --------     ------  ------  ------

    // Degenerate combinations
    { L_,  ""         , ""     , 0, 0,  0, 0  }
  , { L_,  ""         , "a"    , 0, 0,  0, 0  }
  , { L_,  ""         , "ab"   , 0, 0,  0, 0  }
  , { L_,  ""         , "abc"  , 0, 0,  0, 0  }

    // No match at all.
  , { L_,  "A"        , ""     , 0, 0,  0, 0  }
  , { L_,  "A"        , "a"    , 1, 0,  0, 1  }
  , { L_,  "A"        , "ab"   , 1, 0,  1, 0  }
  , { L_,  "A"        , "abc"  , 1, 0,  1, 0  }
  , { L_,  "AB"       , ""     , 0, 0,  0, 0  }
  , { L_,  "AB"       , "a"    , 2, 0,  0, 1  }
  , { L_,  "AB"       , "ab"   , 2, 0,  0, 2  }
  , { L_,  "AB"       , "abc"  , 2, 0,  2, 0  }
  , { L_,  "ABC"      , ""     , 0, 0,  0, 0  }
  , { L_,  "ABC"      , "a"    , 3, 0,  0, 1  }
  , { L_,  "ABC"      , "ab"   , 3, 0,  0, 2  }
  , { L_,  "ABC"      , "abc"  , 3, 0,  0, 3  }

    // Mismatch on second character (of two or more).
  , { L_,  "AB"       , "Ab"   , 2, 0,  0, 2  }
  , { L_,  "AB"       , "Abc"  , 2, 0,  2, 0  }
  , { L_,  "ABC"      , "Ab"   , 3, 0,  0, 2  }
  , { L_,  "ABC"      , "AbC"  , 3, 0,  0, 3  }

    // Mismatch on third character (of three).
  , { L_,  "ABC"      , "ABc"  , 3, 0,  0, 3  }

    // Match at beginning of haystack
  , { L_,  "A"        , ""     , 0, 0,  0, 0  }
  , { L_,  "A"        , "A"    , 0, 1,  0, 1  }
  , { L_,  "AB"       , ""     , 0, 0,  0, 0  }
  , { L_,  "AB"       , "A"    , 0, 1,  0, 1  }
  , { L_,  "AB"       , "AB"   , 0, 2,  0, 2  }
  , { L_,  "ABC"      , ""     , 0, 0,  0, 0  }
  , { L_,  "ABC"      , "A"    , 0, 1,  0, 1  }
  , { L_,  "ABC"      , "AB"   , 0, 2,  0, 2  }
  , { L_,  "ABC"      , "ABC"  , 0, 3,  0, 3  }

    // Match in middle of haystack
  , { L_,  "xAz"      , "A"    , 1, 1,  1, 1  }
  , { L_,  "xABz"     , "A"    , 1, 1,  1, 1  }
  , { L_,  "xABz"     , "AB"   , 1, 2,  1, 2  }
  , { L_,  "xABCz"    , "A"    , 1, 1,  1, 1  }
  , { L_,  "xABCz"    , "AB"   , 1, 2,  1, 2  }
  , { L_,  "xABCz"    , "ABC"  , 1, 3,  1, 3  }

    // Match first needle of two adjacent
  , { L_,  "xAAz"      , "A"   , 1, 1,  1, 1  }
  , { L_,  "xABABz"    , "AB"  , 1, 2,  1, 2  }
  , { L_,  "xABCABCz"  , "ABC" , 1, 3,  1, 3  }

    // Match first needle of two separated
  , { L_,  "xAyAz"     , "A"   , 1, 1,  1, 1  }
  , { L_,  "xAByABz"   , "AB"  , 1, 2,  1, 2  }
  , { L_,  "xABCyABCz" , "ABC" , 1, 3,  1, 3  }

    // Match at end of haystack
  , { L_,  "xA"        , "A"   , 1, 1,  1, 1  }
  , { L_,  "xAB"       , "AB"  , 1, 2,  1, 2  }
  , { L_,  "xABC"      , "ABC" , 1, 3,  1, 3  }

    // False starts
  , { L_,  "xAAB"      , "AB"  , 2, 2,  2, 2  }
  , { L_,  "xABABC"    , "ABC" , 3, 3,  3, 3  }
};

const std::size_t numDATA = sizeof DATA / sizeof *DATA;

// ============================================================================
//                               USAGE EXAMPLES
// ----------------------------------------------------------------------------

// Support for Example 3
void doTestRun(bsl::vector<float> *data)
{
    ASSERT(data);

    // Copied from Example 3;
    const float       markerSequence[]     = { FLT_MAX , FLT_MIN , FLT_MAX };
    const std::size_t markerSequenceLength = sizeof  markerSequence
                                           / sizeof *markerSequence;
    // Start with some noise-like data.
    data->push_back(-1.0f);
    data->push_back( 1.1f);
    data->push_back( 7.6f);

    // Add the marker sequence.
    for (std::size_t i = 0; i < markerSequenceLength; ++i) {
        data->push_back(markerSequence[i]);
    }

    // Add some "test run" data.
    data->push_back(3.f);
    data->push_back(0.1f);
    data->push_back(0.04f);
    data->push_back(0.001f);
    data->push_back(0.0005f);
    data->push_back(0.00009f);
    data->push_back(0.000003f);

    // Add the marker sequence.
    for (std::size_t i = 0; i < markerSequenceLength; ++i) {
        data->push_back(markerSequence[i]);
    }

    // End with some noise-like data.
    data->push_back(-3.61f);
    data->push_back( 1.10f);
    data->push_back(-0.56f);
    data->push_back( 0.81f);
}

void processTestRun(bsl::vector<float>::const_iterator first,
                    bsl::vector<float>::const_iterator last)
{
    // Stub.
    (void)first;
    (void)last;
}

// INSERT for Example 1.
//
// First, define (at file scope if using a pre-C++11 compiler) an equality
// comparison class that provides the required functor interface:
//..
    struct MyCaseInsensitiveCharComparator {
        bool operator()(const char& a, const char& b) const {
            return std::tolower(a) == std::tolower(b);
        }
    };
//..
// Then, define (again at file scope, if pre-C++11), a hash functor so that two
// values, irrespective of their case, hash to the same value.
//..

    struct MyCaseInsensitiveCharHasher {
        bool operator()(const char& value) const {
            static bsl::hash<char> s_hash;
            return s_hash(static_cast<char>(std::tolower(value)));
        }
    };
//..

typedef bslma::TestAllocator MyAllocator; // support for Example 4

// INSERT in Example 4
//
///Design Choices
///--------------
// To implement our cache will will use a 'bsl::unordered_map' container.
// Allocating types must meet certain requirements to work properly with
// allocator enabled containers such as 'bsl::unordered_map'.
// 'bsl::boyer_moore_horpool_searcher' does not, so we will use
// 'bslstl::BoyerMooreHorspoolSearcher' that does.
//
// To clarity exposition, our cache will have the simple policy of retaining
// searcher objects indefinitely and ignore the real-world concern that our
// cache may grow so large that the search time exceeds construction time.
// Also, we will forgo techniques that might minimize the number of times data
// is copied.
//
///Steps
///-----
// First, we define our cache class:
//..
                        // ====================================
                        // class MyCaseInsensitiveSearcherCache
                        // ====================================

    class MyCaseInsensitiveSearcherCache {

        // TYPES
      public:
        typedef bslstl::BoyerMooreHorspoolSearcher<
                                               bsl::string::const_iterator,
                                               MyCaseInsensitiveCharHasher,
                                               MyCaseInsensitiveCharComparator>
                                                                      Searcher;
        // PRIVATE TYPES
      private:
        typedef bsl::unordered_map<bsl::string, Searcher> Map;

        // DATA
        Map d_map;

        // PRIVATE MANIPULATORS
        const Searcher& insertSearcher(const bsl::string& key);
            // Insert into this cache a key-value pair where the key is the
            // specified 'key' and the value is a 'Searcher' object created to
            // seek the needle specified by the key part.  Note that this
            // arrangement guarantees that the iterators used by this cached
            // searcher object remain valid for the life of the searcher
            // object.

      public:
        // CREATORS
        explicit MyCaseInsensitiveSearcherCache(bslma::Allocator
                                                          *basicAllocator = 0);
            // Create an empty 'MyCaseInsensitiveSearcherCache' object.
            // Optionally specify 'basicAllocator' to supply memory.  If
            // 'basicAllocator' is 0, the currently installed default allocator
            // is used.

        // MANIPULATORS
        const Searcher& getSearcher(const char *needle);
            // Return a 'const'-reference to the cached server that can do a
            // case-insensitive search for the specified 'needle'.  If such a
            // server does not exist in the cache on entry, such a searcher is
            // constructed, added to the cache, and returned (by
            // 'const'-reference).

        // ACCESSORS
        std::size_t numSearchers() const;
            // Return the number of searcher objects in this cache.
    };
//..
// Notice (see the 'typedef') that we reuse the hash functor,
// 'MyCaseInsensitiveCharHasher', and equality comparison functor,
// 'MyCaseInsensitiveCharComparator', that were defined in {Example 2}.
//
// Note that 'MyCaseInsensitiveSearcherCache' itself is an allocating type.  If
// we needed to make it compatible with BDE containers (e.g., to allow a cache
// of caches) a few additional features are needed.  As we have no such need,
// those features are deferred.
//
// Then, we implement the constructor:
//..
                        // ------------------------------------
                        // class MyCaseInsensitiveSearcherCache
                        // ------------------------------------

    // CREATORS
    MyCaseInsensitiveSearcherCache::
    MyCaseInsensitiveSearcherCache(bslma::Allocator *basicAllocator)
    : d_map(basicAllocator)
    {
    }

// Notice that 'basicAllocator' is simply forwarded to 'd_map'.
//
// Next, we implement the public methods:
//..
    // MANIPULATORS
    const
    MyCaseInsensitiveSearcherCache::Searcher&
    MyCaseInsensitiveSearcherCache::getSearcher(const char *needle)
    {
        bsl::string   key(needle);
        Map::iterator findResult = d_map.find(key);

        if (d_map.end() == findResult) {
            return insertSearcher(key);                               // RETURN
        } else {
            return findResult->second;                                // RETURN
        }
    }

    // ACCESSORS
    std::size_t MyCaseInsensitiveSearcherCache::numSearchers() const
    {
        return d_map.size();
    }
//..
// Then, to complete our class, we implement the cache class private method:
//..
    // PRIVATE MANIPULATORS
    const
    MyCaseInsensitiveSearcherCache::Searcher&
    MyCaseInsensitiveSearcherCache::insertSearcher(const bsl::string& key)
    {
        Searcher        dummy(key.begin(), key.begin()); // to be overwritten
        Map::value_type value(key, dummy);

        bsl::pair<Map::iterator, bool> insertResult = d_map.insert(value);
        ASSERT(true == insertResult.second);

        Map::iterator iterator  = insertResult.first;

        iterator->second = Searcher(iterator->first.begin(),
                                    iterator->first.end());
        return iterator->second;
    }
//..
// Notice creating our element is a two step process.  First, we insert the key
// with an arbitrary "dummy" searcher.  Once the key (a string) exists in the
// map (at an address that is stable for the life of the map) we create a
// searcher object that refers to that key string for its search sequence, and
// overwrite the "dummy" part of previously inserted element.

static void usage()
    // Test the Usage examples.
{
///Usage
///-----
// In this section we show the intended usage of this component.
//
///Example 1: Basic Usage
/// - - - - - - - - - - -
// The problem of searching a sequence of characters for a particular
// sub-sequence arises in many applications.  For example, one might need to
// know if some document contains a particular word of interest.  For example,
// suppose we would like to know the first occurrence of the word "United" in
// the Declaration of Independence (of the United States):
//
// First, we obtain the text of document and word of interest as sequences of
// 'char' values.
//..
    const char document[] =
    " IN CONGRESS, July 4, 1776.\n"                // 28
    "\n"                                           //  1
    " The unanimous Declaration of the thirteen united States of America,\n"
   //----^----|----^----|----^----|----^----|----  // 44
   //                                              // --
   //                                              // 73rd character
   //
    "\n"
    // ...
    " declare, That these United Colonies are, and of Right ought to be\n"
    // ...
    "Honor.";

    const char *word = "United";
//..
// Then, we create a 'default_searcher' object (a functor) using the given
// 'word':
//..
    XYZ::boyer_moore_horspool_searcher<const char *> searchForUnited(
                                                            word,
                                                            word
                                                          + std::strlen(word));
//..
// Notice that no equality comparison functor was specified so
// 'searchForUnited' will use 'bsl::equal_to<char>' by default.
//
// Now, we invoke our functor, specifying the range of the document to be
// searched:
//..
    bsl::pair<const char *, const char *> result = searchForUnited(
                                                              document,
                                                              document
                                                            + sizeof document);

    std::size_t offset = result.first - document;

    ASSERT(120 == offset);
    ASSERT(static_cast<std::size_t>(result.second - result.first)
               == std::strlen(word));
//..
// Finally, we notice that search correctly ignored the appearance of the word
// "united" (all lower case) in the second sentence.
//
// {'bslstl_default'|Example 1} shows how the same problem is addressed using
// 'bsl::default_searcher'.
//
///Example 2: Defining a Comparator and Hash
///- - - - - - - - - - - - - - - - - - - - -
// As seen in {Example 1} above, the default equality comparison functor is
// case sensitive.  If one needs case *in*-sensitive searches, a non-default
// equality comparison class *and* a non-default hash functor must be
// specified.
//
// First, define (at file scope if using a pre-C++11 compiler) an equality
// comparison class that provides the required functor interface:
//..
//  [INSERT FROM ABOVE]
//..
// Then, define (again at file scope, if pre-C++11), a hash functor so that two
// values, irrespective of their case, hash to the same value.
//..
//  [INSERT FROM ABOVE]
//..
// Now, specify 'bsl::boyer_moore_horspool_searcher' type for and create a
// searcher object to search for 'word':
//..
    XYZ::boyer_moore_horspool_searcher<const char *,
                                       MyCaseInsensitiveCharHasher,
                                       MyCaseInsensitiveCharComparator>
                                                    searchForUnitedInsensitive(
                                                            word,
                                                            word
                                                          + std::strlen(word));
//..
// Note that the new searcher object will use defaulted constructed
// 'MyCaseInsensitiveCharHasher' and 'MyCaseInsensitiveCharComparator' classes.
// If stateful functors are required such objects can be passed in the optional
// constructor arguments.
//
// Now, we invoke our searcher functor, specifying that the same document
// searched in {Example 1}:
//..
    bsl::pair<const char *, const char *> resultInsensitive =
                                                    searchForUnitedInsensitive(
                                                              document,
                                                              document
                                                            + sizeof document);

    std::size_t offsetInsensitive = resultInsensitive.first - document;

    ASSERT( 72 == offsetInsensitive);
    ASSERT(static_cast<std::size_t>(resultInsensitive.second
                                  - resultInsensitive.first)
               == std::strlen(word));
//..
// Finally, we find the next occurrence of 'word' by *reusing* the same
// searcher object, this time instructing it to begin its search just after the
// previous occurrence of 'word' was found:
//..
    resultInsensitive = searchForUnitedInsensitive(resultInsensitive.second,
                                                   document + sizeof document);

    offsetInsensitive = resultInsensitive.first - document;

    ASSERT(120 == offsetInsensitive);
    ASSERT(static_cast<std::size_t>(resultInsensitive.second
                                  - resultInsensitive.first)
               == std::strlen(word));
//..
//
// {'bslstl_default'|Example 2} shows how the same problem is addressed using
// 'bsl::default_searcher'.
//
///Example 3: Non-'char' Searches
/// - - - - - - - - - - - - - - -
// The BMH searcher class template is not constrained to searching for 'char'
// values.  Searches can be done on other types (see {Iterator Requirements}).
// Moreover the container of the sequence being sought (the "needle") need not
// the same as the sequence being searched (the "haystack").
//
// Suppose one has data from an instrument that reports 'float' values and that
// inserts the sequence '{ FLT_MAX, FLT_MIN, FLT_MAX }' as a marker for the
// start and end of a test run.  We can assume the probably of the instrument
// reporting this sequence as readings is negligible and that data reported
// outside of the test runs is random noise.  Here is how we can search for the
// first test run data in the data sequence.
//
// First, we create a representation of the sequence that denotes the limit of
// a test run.
//..
    const float       markerSequence[]     = { FLT_MAX , FLT_MIN , FLT_MAX };
    const std::size_t markerSequenceLength = sizeof  markerSequence
                                           / sizeof *markerSequence;
//..
// Next, we obtain the data to be searched.  (In this example, we will use
// simulated data.)
//..
    bsl::vector<float> data;  // Container provides random access iterators.
    doTestRun(&data);
//..
// Then, we define and create our searcher object:
//..
    XYZ::boyer_moore_horspool_searcher<const float *>
                                         searchForMarker(markerSequence,
                                                         markerSequence
                                                       + markerSequenceLength);
//..
// Notice that no equality comparison functor was specified so
// 'searchForMarker' will use 'bsl::equal_to<float>' by default.
//
// Now, we invoke our searcher on the instrument data.
//..
    typedef bsl::vector<float>::const_iterator DataConstItr;

    const bsl::pair<DataConstItr, DataConstItr> notFound(data.cend(),
                                                         data.cend());

    bsl::pair<DataConstItr, DataConstItr> markerPosition = searchForMarker(
                                                                 data.cbegin(),
                                                                 data.cend());

    ASSERT(notFound != markerPosition);

    DataConstItr startOfTestRun = markerPosition.second;
//..
// Finally, we locate the marker of the end of the first test run and pass the
// location of the first test run data to some other function for processing.
//..
    markerPosition = searchForMarker(markerPosition.second, data.cend());

    ASSERT(notFound != markerPosition);

    DataConstItr endOfTestRun = markerPosition.first;

    processTestRun(startOfTestRun, endOfTestRun);
//..
// {'bslstl_defaultsearcher'|Example 3} shows how the same problem is addressed
// using 'bsl::default_searcher'.  Notice that other example uses 'data' from a
// container that provides bidirectional iterators (and forward iterators would
// have sufficed), whereas here random access iterators are required.
//
    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);
    bslma::TestAllocatorMonitor  dam(&da);

    {

///Example 4: Caching Searcher Objects
///- - - - - - - - - - - - - - - - - -
// The construction of 'bsl::boyer_moore_horspool_searcher' objects is small
// (the needle must be scanned, meta-data calculated, and results saved) but
// can be non-neglibible when one needs a great number of them.  When there is
// a reasonable chance that one will have to repeat a given search, it can be
// worthwhile to cache the searcher objects for reuse.
//
// Suppose we have a long list of names, each consisting of a given name (first
// name) and a surname (last name), and that we wish to identify instances of
// reduplication of the given name in the surname.  That is, we want to
// identify the cases where the given name is a *case-insensitive* substring of
// the surname.  Examples include "Durand Durand", "James Jameson", "John St.
// John", and "Jean Valjean".  In this example we will not accept nicknames and
// other approximate name forms as matches (e.g., "Joe Joseph", "Jack
// Johnson").
//
// Since we want to perform our task as efficiently as possible, and since we
// expect many entries to have common given names (e.g., "John"), we decide to
// create a cache of searcher objects for those first names so they need not be
// reconstructed for each search of a surname.
//
// -- INSERT FROM ABOVE --
//
// Now, we show how the searcher object cache can be used.  In this example, a
// fixed array represents our source of name entries, in random order.
//..
    struct {
        const char *d_givenName_p;
        const char *d_surname_p;
    } DATA[] = {
        // GIVEN     SURNAME
        // --------  ------------
        { "Donald" , "McDonald"    }
      , { "John"   , "Johnson"     }
      , { "John"   , "Saint Johns" }
      , { "Jon"    , "Literjon"    }
      , { "Jean"   , "Valjean"     }
      , { "James"  , "Jameson"     }
      , { "Will"   , "Freewill"    }
      , { "John"   , "Johns"       }
      , { "John"   , "John"        }
      , { "John"   , "Jones"       }
      , { "J'onn"  , "J'onzz"      }
      , { "Donald" , "Donalds"     }
      , { "Donald" , "Mac Donald"  }
      , { "William", "Williams"    }
      , { "Durand" , "Durand"      }
      , { "John"   , "Johnstown"   }
      , { "Major"  , "Major"       }
      , { "Donald" , "MacDonald"   }
      , { "Patrick", "O'Patrick"   }
      , { "Chris",   "Christie"    }
      , { "Don",     "London"      }
        // ...
      , { "Ivan"   , "Ivanovich"   }
    };

    std::size_t NUM_NAMES = sizeof DATA / sizeof *DATA;

    typedef bsl::pair<const char *, const char *> Result;

    MyAllocator                    myAllocator;
    MyCaseInsensitiveSearcherCache searcherCache(&myAllocator);
    bsl::string                    output;

    for (std::size_t ti = 0; ti < NUM_NAMES; ++ti) {
        const char * const givenName = DATA[ti].d_givenName_p;
        const char * const surname   = DATA[ti].d_surname_p;

        const MyCaseInsensitiveSearcherCache::Searcher& searcher =
                                          searcherCache.getSearcher(givenName);

        ASSERT(&myAllocator == searcher.allocator());
//..
// Notice that each searcher object in the cache (correctly) uses the same
// allocator as we specified for the cache itself.
//
// The rest of the application:
//..
        const Result result   = searcher(surname,
                                         surname + std::strlen(surname));

        const Result notFound = std::make_pair(surname + std::strlen(surname),
                                               surname + std::strlen(surname));

        char buffer[32];

        if (notFound == result) {
            snprintf(buffer,
                     sizeof(buffer),
                     "ng: %-10s %-11s\n",
                     givenName,
                     surname);
        } else {
            snprintf(buffer,
                     sizeof(buffer),
                     "OK: %-10s %-11s\n",
                     givenName,
                     surname);
        }

        output.append(buffer);
    }
//..
// Finally, we examine the collected 'output' and confirm that our code is
// properly identifying the names of interest.
//..
    ASSERT(0 == std::strcmp(output.c_str(),
                            "OK: Donald     McDonald   \n"
                            "OK: John       Johnson    \n"
                            "OK: John       Saint Johns\n"
                            "OK: Jon        Literjon   \n"
                            "OK: Jean       Valjean    \n"
                            "OK: James      Jameson    \n"
                            "OK: Will       Freewill   \n"
                            "OK: John       Johns      \n"
                            "OK: John       John       \n"
                            "ng: John       Jones      \n"
                            "ng: J'onn      J'onzz     \n"
                            "OK: Donald     Donalds    \n"
                            "OK: Donald     Mac Donald \n"
                            "OK: William    Williams   \n"
                            "OK: Durand     Durand     \n"
                            "OK: John       Johnstown  \n"
                            "OK: Major      Major      \n"
                            "OK: Donald     MacDonald  \n"
                            "OK: Patrick    O'Patrick  \n"
                            "OK: Chris      Christie   \n"
                            "OK: Don        London     \n"
                            "OK: Ivan       Ivanovich  \n"));

    ASSERT(searcherCache.numSearchers() < NUM_NAMES);
//..
    }

    ASSERT(dam.isInUseSame());
}

// ============================================================================
//                               HELPER FUNCTIONS
// ----------------------------------------------------------------------------

static const char *performanceSyntaxMessage(int test)
    // Return the (valid) command line syntax message corresponding to the
    // specified 'test' number, or '*UNKNOWN*' if no message is defined for
    // 'test'.
{
    switch (test) {
      case -1: return
          "syntax: "
          "<numRepetitions> "
          "<text|octal|binary|short>[+]";
      default: return
          "syntax: *UNKNOWN*";
    }
}

static int getHaystack(const char **haystackFirstPtr,
                       const char **haystackLastPtr,
                       const char  *haystackOption)
    // Load to the specified 'haystackFirstPtr' and 'haystackLastPtr' the
    // beginning and end address of the statically defined test haystack data
    // corresponding to the specified 'haystackOption'.  Return 0 on success
    // and a non-zero value if 'haystackOption' is not recognized.
{
    ASSERT(haystackFirstPtr);
    ASSERT(haystackLastPtr);
    ASSERT(haystackOption);

    *haystackFirstPtr =
          0 == std::strcmp("text",    haystackOption) ? HAYSTACK_TEXT_FIRST  :
          0 == std::strcmp("text+",   haystackOption) ? HAYSTACK_TEXT_FIRST  :
          0 == std::strcmp("octal",   haystackOption) ? HAYSTACK_OCTAL_FIRST :
          0 == std::strcmp("octal+",  haystackOption) ? HAYSTACK_OCTAL_FIRST :
          0 == std::strcmp("binary",  haystackOption) ? HAYSTACK_BINARY_FIRST:
          0 == std::strcmp("binary+", haystackOption) ? HAYSTACK_BINARY_FIRST:
          0 == std::strcmp("short",   haystackOption) ? HAYSTACK_SHORT_FIRST :
          0 == std::strcmp("short+",  haystackOption) ? HAYSTACK_SHORT_FIRST :
          /* unknown */                                 0                    ;

    if (0 == *haystackFirstPtr) {
        return -1;                                                    // RETURN
    }

    *haystackLastPtr =
           0 == std::strcmp("text",    haystackOption) ? HAYSTACK_TEXT_LAST  :
           0 == std::strcmp("text+",   haystackOption) ? HAYSTACK_TEXT_LAST  :
           0 == std::strcmp("octal",   haystackOption) ? HAYSTACK_OCTAL_LAST :
           0 == std::strcmp("octal+",  haystackOption) ? HAYSTACK_OCTAL_LAST :
           0 == std::strcmp("binary",  haystackOption) ? HAYSTACK_BINARY_LAST:
           0 == std::strcmp("binary+", haystackOption) ? HAYSTACK_BINARY_LAST:
           0 == std::strcmp("short",   haystackOption) ? HAYSTACK_SHORT_LAST :
           0 == std::strcmp("short+",  haystackOption) ? HAYSTACK_SHORT_LAST :
           /* unknown */                                 0                   ;

    if (0 == *haystackLastPtr) {
        return -1;                                                    // RETURN
    }

    return 0;
}

static int getDataForHaystack(const DATA_t **DATA,
                              std::size_t   *NUM_DATA,
                              const char    *haystackOption)
    // Load to the specified 'DATA' and 'NUM_DATA' the address of and number of
    // entries, respectively, of the statically defined sets of "needle" data
    // corresponding to the specified 'haystackOption'.  Return 0 on success
    // and a non-zero value if 'haystackOption' is not recognized.
{
    ASSERT(DATA);
    ASSERT(NUM_DATA);
    ASSERT(haystackOption);

    *DATA = 0 == std::strcmp("text",    haystackOption) ? U_DATA_TEXT         :
            0 == std::strcmp("text+",   haystackOption) ? U_DATA_TEXT_PLUS    :
            0 == std::strcmp("octal",   haystackOption) ? U_DATA_OCTAL        :
            0 == std::strcmp("octal+",  haystackOption) ? U_DATA_OCTAL_PLUS   :
            0 == std::strcmp("binary",  haystackOption) ? U_DATA_BINARY       :
            0 == std::strcmp("binary+", haystackOption) ? U_DATA_BINARY_PLUS  :
            0 == std::strcmp("short",   haystackOption) ? U_DATA_SHORT        :
            0 == std::strcmp("short+",  haystackOption) ? U_DATA_SHORT_PLUS   :
            /* unknown */                                 0                   ;

    if (0 == *DATA) {
        return -1;                                                    // RETURN
    }

    *NUM_DATA =
        0 == std::strcmp("text",    haystackOption)  ? NUM_U_DATA_TEXT        :
        0 == std::strcmp("text+",   haystackOption)  ? NUM_U_DATA_TEXT_PLUS   :
        0 == std::strcmp("octal",   haystackOption)  ? NUM_U_DATA_OCTAL       :
        0 == std::strcmp("octal+",  haystackOption)  ? NUM_U_DATA_OCTAL_PLUS  :
        0 == std::strcmp("binary",  haystackOption)  ? NUM_U_DATA_BINARY      :
        0 == std::strcmp("binary+", haystackOption)  ? NUM_U_DATA_BINARY_PLUS :
        0 == std::strcmp("short",   haystackOption)  ? NUM_U_DATA_SHORT       :
        0 == std::strcmp("short+",  haystackOption)  ? NUM_U_DATA_SHORT_PLUS  :
        /* unknown */                             static_cast<std::size_t>(-1);

    if (static_cast<std::size_t>(-1) == *NUM_DATA) {
        return -1;                                                    // RETURN
    }

    return 0;
}

static void convertToNonAscii(std::string *out, const std::string& input)
    // Load to the specified 'out' string a sequence of values in which every
    // zero character (0x30) in the specified 'input' is mapped to '0' (0x00)
    // and every decimal 1 character is mapped to 'UCHAR_MAX'.
{
    ASSERT(out);

    for (std::size_t i = 0; i < input.length(); ++i) {
        int chOld = input.data()[i];
        int chNew = '0' == chOld ?         0 :
                    '1' == chOld ? UCHAR_MAX :
                     /* else */           -1 ;

        ASSERT(-1 != chNew);

        out->push_back(static_cast<char>(chNew));
    }
}

static void loadVectorOfChars(bsl::vector<char> *dst, const char *src)
    // Load to the specified 'dst' vector a sequence of characters matching the
    // sequence found in the specified 'src'.
{
    ASSERT(dst);
    ASSERT(src);

    for (const char *ptr = src; *ptr; ++ptr) {
        dst->push_back(*ptr);
    }
}

static bool expectAllocationCase7(Int64             dstNumBytes,
                                  bslma::Allocator *dstAllocator,
                                  Int64             srcNumBytes,
                                  bslma::Allocator *srcAllocator)
    // Return 'true' if the move assignment test in case 7 for the special
    // implementation (for 'char *') from the source object initially holding
    // the specified 'dstNumBytes' from the specified 'dstAllocator' to the
    // destination object initially holding the specified 'srcNumBytes' from
    // the 'srcAllocator' will allocate memory from 'dstAllocator', and 'false'
    // otherwise.
{
    ASSERT(dstAllocator);
    ASSERT(srcAllocator);

    if (dstNumBytes == srcNumBytes) {
        return false;                                                 // RETURN
    }

    if (0           == srcNumBytes) {
        return false;                                                 // RETURN
    }

    if (dstAllocator == srcAllocator) {
        return false;                                                 // RETURN
    }

    return true;
}

// ============================================================================
//                               TEST CASES
// ----------------------------------------------------------------------------

template <class FUNCTOR, bool ARE_COMPARABLE>
class CompareFunctors {
  public:
    bool areEqual(const FUNCTOR& lhs, const FUNCTOR& rhs);
};

template <class FUNCTOR>
class CompareFunctors<FUNCTOR, true> {
  public:
    bool areEqual(const FUNCTOR& lhs, const FUNCTOR& rhs) const {
        return lhs == rhs;
    }
};

template <class FUNCTOR>
class CompareFunctors<FUNCTOR, false> {
  public:
    bool areEqual(const FUNCTOR& , const FUNCTOR& ) const {
        return true;
    }
};

template <class RNDACC_ITR,
          class HASH,
          class EQUAL,
          bool  ARE_FUNCTORS_COMPARABLE>
static void testMoveConstructors()
{
    if (veryVerbose) {
        P(bsls::NameOf<RNDACC_ITR>())
        P(bsls::NameOf<HASH>())
        P(bsls::NameOf<EQUAL>())
        bool areFunctorsComparable = ARE_FUNCTORS_COMPARABLE;
        P(areFunctorsComparable)
    }

    typedef bslstl::BoyerMooreHorspoolSearcher<RNDACC_ITR,
                                               HASH,
                                               EQUAL> Mech;
    typedef bsl::pair<RNDACC_ITR, RNDACC_ITR>         Result;

    typedef BloombergLP::bslmf::MovableRefUtil MoveUtil;

    bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
    bslma::TestAllocator da("default",   veryVeryVeryVerbose);
    bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);
    bslma::TestAllocator za("different", veryVeryVeryVerbose);

    bslma::DefaultAllocatorGuard dag(&da);

    for (std::size_t ti = 0; ti < numDATA; ++ti) {
        const int          LINE      = DATA[ti].d_line; (void) LINE;
        const char * const HAYSTACK  = DATA[ti].d_haystack_p;
        const char * const NEEDLE    = DATA[ti].d_needle_p;

        if (veryVerbose) {
            P_(LINE) P_(HAYSTACK) P(NEEDLE)
        }

        for (char dstCfg = 'a'; dstCfg <= 'd'; ++dstCfg) {
            const char CONFIG = dstCfg;

            if (veryVerbose) {
                P(CONFIG)
            }

            Mech mZ(NEEDLE,
                    NEEDLE + std::strlen(NEEDLE),
                    HASH(),
                    EQUAL(),
                    &sa);  const Mech& Z = mZ;

            ASSERT(NEEDLE                       == Z.needleFirst());
            ASSERT(NEEDLE + std::strlen(NEEDLE) == Z.needleLast());

            Result resultZ = Z(HAYSTACK,
                               HAYSTACK + std::strlen(HAYSTACK));

            bslma::TestAllocatorMonitor fam(&fa);
            bslma::TestAllocatorMonitor dam(&da);
            bslma::TestAllocatorMonitor sam(&sa);
            bslma::TestAllocatorMonitor zam(&za);

            Mech *mechDstPtr;

            switch (CONFIG) {
              case 'a': {
                mechDstPtr = new (fa) Mech(MoveUtil::move(mZ));      // ACTIONa
              } break;
              case 'b': {
                mechDstPtr = new (fa) Mech(MoveUtil::move(mZ), 0);   // ACTIONb
              } break;
              case 'c': {
                mechDstPtr = new (fa) Mech(MoveUtil::move(mZ), &sa); // ACTIONc
              } break;
              case 'd': {
                mechDstPtr = new (fa) Mech(MoveUtil::move(mZ), &za); // ACTIONd
              } break;
              default: {
                ASSERTV(CONFIG, !"Bad allocator config.");
              } return;                                               // RETURN
            }

            Mech& mX = *mechDstPtr;  const Mech& X = mX;

            bslma::TestAllocator& oa = 'a' == CONFIG ? sa :
                                       'b' == CONFIG ? da :
                                       'c' == CONFIG ? sa :
                                       /* else */      za ;

            ASSERT((CompareFunctors<HASH,
                                    ARE_FUNCTORS_COMPARABLE>().areEqual(
                                                               HASH(),
                                                               X.hash())));
            ASSERT((CompareFunctors<EQUAL,
                                    ARE_FUNCTORS_COMPARABLE>().areEqual(
                                                              EQUAL(),
                                                              X.equal())));
            ASSERT(&oa == X.allocator());

            Result resultX = X(HAYSTACK,
                               HAYSTACK + std::strlen(HAYSTACK));

            ASSERT(resultZ == resultX);

            // Is 'mZ' in a valid state?

            ASSERT(NEEDLE == Z.needleFirst());
            ASSERT(NEEDLE == Z.needleLast());

            const Result resultZafter = Z(HAYSTACK,
                                          HAYSTACK + std::strlen(HAYSTACK));

            const Result expected(HAYSTACK, HAYSTACK);

            ASSERT(expected == resultZafter);

            mZ = X;

            const Result resultZrestored = Z(HAYSTACK,
                                             HAYSTACK + std::strlen(HAYSTACK));

            ASSERT(resultX == resultZrestored);

            fa.deleteObject(mechDstPtr);

            ASSERT(0 == fam.numBlocksInUseChange());
            ASSERT(0 == dam.numBlocksInUseChange());
            ASSERT(0 == zam.numBlocksInUseChange());
        }

        ASSERT(0 == sa.numBlocksInUse()); // 'mZ' destroyed at end-of-block
    }
}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
struct TestDeductionGuides {
    // This struct provides a namespace for functions testing deduction guides.
    // The tests are compile-time only; it is not necessary that these routines
    // be called at run-time.

#define ASSERT_SAME_TYPE(...) \
 static_assert((bsl::is_same<__VA_ARGS__>::value), "Types differ unexpectedly")

    template <class KEY_TYPE>
    struct StupidEqual {
        bool operator()(const KEY_TYPE&, const KEY_TYPE&) const
            // Always return true
        {
            return true;
        }
    };

    template <class KEY_TYPE>
    static size_t StupidEqualFn(const KEY_TYPE&, const KEY_TYPE&)
        // Always return true
    {
        return true;
    }

    template <class KEY_TYPE>
    struct StupidHash {
        size_t operator()(const KEY_TYPE&) const
            // Always hash to bucket #0
        {
            return 0U;
        }
    };

    template <class KEY_TYPE>
    static size_t StupidHashFn(const KEY_TYPE&)
        // Always hash to bucket #0
    {
        return 0U;
    }

    void TestBSLSearcher ()
        // Test that constructing a 'bslstl::BoyerMooreHorspoolSearcher' from
        // various combinations of arguments deduces the correct type.
        //..
        //  BoyerMooreHorspoolSearcher(const BoyerMooreHorspoolSearcher&  s)
        //                                                       -> decltype(s)
        //  BoyerMooreHorspoolSearcher(      BoyerMooreHorspoolSearcher&& s)
        //                                                       -> decltype(s)
        //
        //  BoyerMooreHorspoolSearcher(Iter, Iter, HASH=HASH(), EQUAL=EQUAL())
        //
        //..
    {
        typedef int T1;

        T1                                       *p1b = nullptr;
        T1                                       *p1e = nullptr;
        bslstl::BoyerMooreHorspoolSearcher<T1 *>  ds1(p1b, p1e);
        bslstl::BoyerMooreHorspoolSearcher        ds1a(ds1);
        ASSERT_SAME_TYPE(decltype(ds1a),
                         bslstl::BoyerMooreHorspoolSearcher<T1 *>);

        typedef long T2;

        T2                                       *p2b = nullptr;
        T2                                       *p2e = nullptr;
        bslstl::BoyerMooreHorspoolSearcher<T2 *>  ds2(p2b, p2e);
        bslstl::BoyerMooreHorspoolSearcher        ds2a(std::move(ds2));
        ASSERT_SAME_TYPE(decltype(ds2a),
                         bslstl::BoyerMooreHorspoolSearcher<T2 *>);

        typedef float                       T3;
        typedef StupidHash<T3>              HashT3;
        typedef StupidEqual<T3>             EqualT3;
        typedef decltype(StupidHashFn<T3>)  HashFnT3;
        typedef decltype(StupidEqualFn<T3>) EqualFnT3;

        T3                                 *p3b = nullptr;
        T3                                 *p3e = nullptr;
        bslstl::BoyerMooreHorspoolSearcher  ds3a(p3b, p3e);
        bslstl::BoyerMooreHorspoolSearcher  ds3b(p3b, p3e, StupidHash<T3>{});
        bslstl::BoyerMooreHorspoolSearcher  ds3c(p3b, p3e, StupidHashFn<T3>);
        bslstl::BoyerMooreHorspoolSearcher  ds3d(p3b,
                                                 p3e,
                                                 StupidHash<T3>{},
                                                 StupidEqual<T3>{});
        bslstl::BoyerMooreHorspoolSearcher  ds3e(p3b,
                                                 p3e,
                                                 StupidHash<T3>{},
                                                 StupidEqualFn<T3>);
        bslstl::BoyerMooreHorspoolSearcher  ds3f(p3b,
                                                 p3e,
                                                 StupidHashFn<T3>,
                                                 StupidEqual<T3>{});
        bslstl::BoyerMooreHorspoolSearcher  ds3g(p3b,
                                                 p3e,
                                                 StupidHashFn<T3>,
                                                 StupidEqualFn<T3>);

        ASSERT_SAME_TYPE(decltype(ds3a),
                         bslstl::BoyerMooreHorspoolSearcher<T3 *>);
        ASSERT_SAME_TYPE(decltype(ds3b),
                         bslstl::BoyerMooreHorspoolSearcher<T3 *, HashT3>);
        ASSERT_SAME_TYPE(decltype(ds3c),
                         bslstl::BoyerMooreHorspoolSearcher<T3 *, HashFnT3 *>);
        ASSERT_SAME_TYPE(decltype(ds3d),
                         bslstl::BoyerMooreHorspoolSearcher<T3 *,
                                                            HashT3,
                                                            EqualT3>);
        ASSERT_SAME_TYPE(decltype(ds3e),
                         bslstl::BoyerMooreHorspoolSearcher<T3 *,
                                                            HashT3,
                                                            EqualFnT3 *>);
        ASSERT_SAME_TYPE(decltype(ds3f),
                         bslstl::BoyerMooreHorspoolSearcher<T3 *,
                                                            HashFnT3 *,
                                                            EqualT3>);
        ASSERT_SAME_TYPE(decltype(ds3g),
                         bslstl::BoyerMooreHorspoolSearcher<T3 *,
                                                            HashFnT3 *,
                                                            EqualFnT3 *>);

    }

#ifndef BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_FUNCTORS
    void TestSTLSearcher ()
        // Test that constructing a 'bsl::boyer_moore_horspool_searcher' from
        // various combinations of arguments deduces the correct type.
        //..
        //  boyer_moore_horspool_searcher(
        //              const boyer_moore_horspool_searcher&  s) -> decltype(s)
        //  boyer_moore_horspool_searcher(
        //                    boyer_moore_horspool_searcher&& s) -> decltype(s)
        //
        //  boyer_moore_horspool_searcher(Iter, Iter,
        //                               HASH = HASH(), EQUAL = EQUAL())
        //
        //..
    {
        typedef int T1;

        T1                                       *p1b = nullptr;
        T1                                       *p1e = nullptr;
        bsl::boyer_moore_horspool_searcher<T1 *>  ds1(p1b, p1e);
        bsl::boyer_moore_horspool_searcher        ds1a(ds1);
        ASSERT_SAME_TYPE(decltype(ds1a),
                         bsl::boyer_moore_horspool_searcher<T1 *>);

        typedef long T2;

        T2                                       *p2b = nullptr;
        T2                                       *p2e = nullptr;
        bsl::boyer_moore_horspool_searcher<T2 *>  ds2(p2b, p2e);
        bsl::boyer_moore_horspool_searcher        ds2a(std::move(ds2));
        ASSERT_SAME_TYPE(decltype(ds2a),
                         bsl::boyer_moore_horspool_searcher<T2 *>);

        typedef float                       T3;
        typedef StupidHash<T3>              HashT3;
        typedef StupidEqual<T3>             EqualT3;
        typedef decltype(StupidHashFn<T3>)  HashFnT3;
        typedef decltype(StupidEqualFn<T3>) EqualFnT3;

        T3                                 *p3b = nullptr;
        T3                                 *p3e = nullptr;
        bsl::boyer_moore_horspool_searcher  ds3a(p3b, p3e);
        bsl::boyer_moore_horspool_searcher  ds3b(p3b, p3e, StupidHash<T3>{});
        bsl::boyer_moore_horspool_searcher  ds3c(p3b, p3e, StupidHashFn<T3>);
        bsl::boyer_moore_horspool_searcher  ds3d(p3b,
                                                 p3e,
                                                 StupidHash<T3>{},
                                                 StupidEqual<T3>{});
        bsl::boyer_moore_horspool_searcher  ds3e(p3b,
                                                 p3e,
                                                 StupidHash<T3>{},
                                                 StupidEqualFn<T3>);
        bsl::boyer_moore_horspool_searcher  ds3f(p3b,
                                                 p3e,
                                                 StupidHashFn<T3>,
                                                 StupidEqual<T3>{});
        bsl::boyer_moore_horspool_searcher  ds3g(p3b,
                                                 p3e,
                                                 StupidHashFn<T3>,
                                                 StupidEqualFn<T3>);

        ASSERT_SAME_TYPE(decltype(ds3a),
                         bsl::boyer_moore_horspool_searcher<T3 *>);
        ASSERT_SAME_TYPE(decltype(ds3b),
                         bsl::boyer_moore_horspool_searcher<T3 *, HashT3>);
        ASSERT_SAME_TYPE(decltype(ds3c),
                         bsl::boyer_moore_horspool_searcher<T3 *, HashFnT3 *>);
        ASSERT_SAME_TYPE(decltype(ds3d),
                         bsl::boyer_moore_horspool_searcher<T3 *,
                                                            HashT3,
                                                            EqualT3>);
        ASSERT_SAME_TYPE(decltype(ds3e),
                         bsl::boyer_moore_horspool_searcher<T3 *,
                                                            HashT3,
                                                            EqualFnT3 *>);
        ASSERT_SAME_TYPE(decltype(ds3f),
                         bsl::boyer_moore_horspool_searcher<T3 *,
                                                            HashFnT3 *,
                                                            EqualT3>);
        ASSERT_SAME_TYPE(decltype(ds3g),
                         bsl::boyer_moore_horspool_searcher<T3 *,
                                                            HashFnT3 *,
                                                            EqualFnT3 *>);
    }
#endif

#undef ASSERT_SAME_TYPE
};
#endif  // BSLS_COMPILERFEATURES_SUPPORT_CTAD

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? std::atoi(argv[1]) : 0;
                     verbose = argc > 2; (void)             verbose;
                 veryVerbose = argc > 3; (void)         veryVerbose;
             veryVeryVerbose = argc > 4; (void)     veryVeryVerbose;
         veryVeryVeryVerbose = argc > 5; (void) veryVeryVeryVerbose;

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 11: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, replace 'assert' with 'ASSERT', and
        //:   replace 'bsl::' with 'std::'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\n" "USAGE EXAMPLE"
                            "\n" "=============" "\n");
        usage();

      } break;
      case 10: {
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
        // Plan:
        //: 1 Create a 'BoyerMooreHorspoolSearcher' by invoking the constructor
        //:   without supplying the template arguments explicitly.
        //:
        //: 2 If we are not using the system's 'boyer_moore_horspool_searcher',
        //:   create a 'bsl::boyer_moore_horspool_searcher' by invoking the
        //:   constructor without supplying the template arguments explicitly.
        //:
        //: 3 Verify that the deduced types are correct.
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
        (void) test;  // This variable only exists for ease of IDE navigation.
#endif
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TRAITS AND PUBLIC TYPES
        //
        // Concerns:
        //: 1 The 'bslma::UsesBslmaAllocator' trait is set for each
        //:   allocating class defined in this component.
        //:
        //: 2 Each public, user facing type is correct.
        //
        // Plan:
        //: 1 Check the required trait for each allocating type.
        //:
        //: 2 Check each public type against an independent definition.
        //
        // Testing:
        //   TRAITS
        //   PUBLIC TYPES
        // --------------------------------------------------------------------

        if (verbose) printf("\n" "TRAITS AND PUBLIC TYPES"
                            "\n" "=======================" "\n");

        typedef CharArray<char>::const_iterator RndAccConstItr;
        typedef bsl::hash<    char>             DefaultHash;
        typedef bsl::equal_to<char>             DefaultEqual;

        typedef bslstl::BoyerMooreHorspoolSearcher<RndAccConstItr,
                                                   DefaultHash,
                                                   DefaultEqual> MechChar;

        typedef bslstl::BoyerMooreHorspoolSearcher_CharImp<
                                                   RndAccConstItr,
                                                   DefaultHash,
                                                   DefaultEqual> MechCharImp;

        typedef bslstl::BoyerMooreHorspoolSearcher<
                                    RndAccConstItr,
                                    MyCaseInsensitiveCharHasher,
                                    MyCaseInsensitiveCharComparator> MechGnrl;

        typedef bslstl::BoyerMooreHorspoolSearcher_GeneralImp<
                                    RndAccConstItr,
                                    MyCaseInsensitiveCharHasher,
                                    MyCaseInsensitiveCharComparator>
                                                                   MechGnrlImp;

        ASSERT((bslma::UsesBslmaAllocator<MechChar   >::VALUE));
        ASSERT((bslma::UsesBslmaAllocator<MechCharImp>::VALUE));
        ASSERT((bslma::UsesBslmaAllocator<MechGnrl   >::VALUE));
        ASSERT((bslma::UsesBslmaAllocator<MechGnrlImp>::VALUE));

        ASSERT((bsl::is_same<char,         MechChar::value_type  >()));
        ASSERT((bsl::is_same<DefaultHash,  MechChar::DefaultHash >()));
        ASSERT((bsl::is_same<DefaultEqual, MechChar::DefaultEqual>()));
        ASSERT((bsl::is_same<char,         MechGnrl::value_type  >()));
        ASSERT((bsl::is_same<DefaultHash,  MechGnrl::DefaultHash >()));
        ASSERT((bsl::is_same<DefaultEqual, MechGnrl::DefaultEqual>()));

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TEST 'boyer_moore_horspool_seacher' FACADE
        //   The 'boyer_moore_horspool_searcher' class is a facade that
        //   forwards to selected interfaces of
        //   'BoyerMooreHorsepoolSearcher', a class that has been thoroughly
        //   tested below.  Our concerns are largely those of correct
        //   forwarding of arguments.
        //
        // Concerns:
        //: 1 The (non-'default') constructor has the expected default
        //:   parameters for 'hf' and 'pred'.
        //:
        //: 2 Constructor arguments are forwarded in correct order to the
        //:   'BoyerMooreHorspoolSearcher' implementation.
        //:
        //: 3 The search 'operator()' forwards its arguments in the correct
        //:   order to the 'BoyerMooreHorspoolSearcher' implementation.
        //:
        //: 4 QoI: Any allocated memory comes the default allocator.
        //:
        //: 5 QoI: Precondition violations are detected in appropriate build
        //:   modes.
        //
        // Plan:
        //: 1 Use the 'bsl::is_same' meta-function to confirm that a searcher
        //:   type defined without optional parameters matches one declared
        //:   with the known default types.  (C-1)
        //:
        //: 2 Install a test allocator as the default allocator, then create
        //:   searcher objects, and confirm that the test allocator is used
        //:   when expected, and not otherwise.  (C-4)
        //:
        //: 3 Confirm proper forwarding of range arguments (both for the CTOR
        //:   and 'operator()') by creating a series of sets that would produce
        //:   noticeably different results if the range beginning or end was
        //:   off by even one position.  (C-2, C-3)
        //:
        //: 4 Confirm proper forwarding the functors by creating cases that
        //:   would produce noticeably different results if the intended
        //:   functors were not used.  Note that switching the two functors is
        //:   not a concern as that would produce a compile error.  (C-2)
        //:
        //: 5 Use the conventional Negative testing idiom to confirm that
        //:   pre-conditions are tested in the appropriate build modes.  (C-5)
        //
        // Testing:
        //   boyer_moore_horspool_searcher: facade forwards correctly
        // --------------------------------------------------------------------

        if (verbose) printf(
                       "\n" "TEST 'boyer_moore_horspool_seacher' FACADE"
                       "\n" "==========================================" "\n");

#ifndef BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_FUNCTORS
        typedef CharArray<char>::const_iterator    RndAccConstItr;
        typedef bsl::boyer_moore_horspool_searcher<RndAccConstItr>    MechChar;
        typedef bsl::boyer_moore_horspool_searcher<
                                     RndAccConstItr,
                                     MyCaseInsensitiveCharHasher,
                                     MyCaseInsensitiveCharComparator> MechGnrl;

        typedef bsl::pair<RndAccConstItr, RndAccConstItr> Result;
        typedef bsl::hash<    char>                       DefaultHash;
        typedef bsl::equal_to<char>                       DefaultEqual;

        if (verbose) printf("\n" "Default Functors" "\n");
        {
            ASSERT((bsl::is_same<MechChar,
                                 bsl::boyer_moore_horspool_searcher<
                                                     RndAccConstItr,
                                                     bsl::hash<char>,
                                                     bsl::equal_to<char> >
                                >() ));
        }

        if (verbose) printf("\n" "Allocator Tests" "\n");
        {
            bslma::TestAllocator         da("default", veryVeryVeryVerbose);
            bslma::DefaultAllocatorGuard dag(&da);

            bsl::vector<char>  vectorOfChars;
            loadVectorOfChars(&vectorOfChars, "zzzzzzzzzzzzzz");

            CharArray<char> needleSource(vectorOfChars);

            bslma::TestAllocatorMonitor dam(&da);

            MechChar mXC(needleSource.begin(), needleSource.end());

            ASSERT(dam.isTotalUp());   // Special case allocates a fixed-sized
            dam.reset();               // table.

            MechGnrl mXG(needleSource.begin(), needleSource.end());

            ASSERT(dam.isTotalUp());   // General case uses 'unordered_map'
                                       // and allocates for non-empty needle.
        }

        if (verbose) printf("\n" "Range Tests" "\n");
        {
            const struct {
                int          d_line;

                const char  *d_needleSource_p;
                std::size_t  d_idxNeedleFirst;
                std::size_t  d_idxNeedleLast;

                const char  *d_hstckSource_p;
                std::size_t  d_idxHstckFirst;
                std::size_t  d_idxHstckLast;

                std::size_t  d_idxExpectFirst;
                std::size_t  d_idxExpectLast;

            } DATA[] = {
                  //LINE NS    NF  NL  HS   HF  HL  EF  EL
                  //---- ---   --  --  --   --  --  --  --
                  { L_,  "ab",  1,  2, "bb", 0,  2,  0,  1 }  // needle   first
                , { L_,  "ba",  0,  1, "bb", 0,  2,  0,  1 }  // needle   last
                , { L_ , "A",   0,  1, "Ab", 1,  2,  2,  2 }  // haystack first
                , { L_ , "A",   0,  1, "bA", 0,  1,  1,  1 }  // haystack last
            };

            std::size_t NUM_DATA = sizeof DATA / sizeof *DATA;

            for (std::size_t ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE             = DATA[ti].d_line;
                const char *const NEEDLE_SOURCE    = DATA[ti].d_needleSource_p;
                const std::size_t IDX_NEEDLE_FIRST = DATA[ti].d_idxNeedleFirst;
                const std::size_t IDX_NEEDLE_LAST  = DATA[ti].d_idxNeedleLast;
                const char *const HSTCK_SOURCE     = DATA[ti].d_hstckSource_p;
                const std::size_t IDX_HSTCK_FIRST  = DATA[ti].d_idxHstckFirst;
                const std::size_t IDX_HSTCK_LAST   = DATA[ti].d_idxHstckLast;
                const std::size_t IDX_EXPECT_FIRST = DATA[ti].d_idxExpectFirst;
                const std::size_t IDX_EXPECT_LAST  = DATA[ti].d_idxExpectLast;

                if (veryVerbose) {
                    P(LINE)
                    P_(NEEDLE_SOURCE)
                    P_(IDX_NEEDLE_FIRST)
                     P(IDX_NEEDLE_LAST)
                    P_(HSTCK_SOURCE)
                    P_(IDX_HSTCK_FIRST)
                     P(IDX_HSTCK_LAST)
                    P_(IDX_EXPECT_FIRST)
                     P(IDX_EXPECT_LAST)
                }

                MechChar mXC(NEEDLE_SOURCE + IDX_NEEDLE_FIRST,
                             NEEDLE_SOURCE + IDX_NEEDLE_LAST);
                MechGnrl mXG(NEEDLE_SOURCE + IDX_NEEDLE_FIRST,
                             NEEDLE_SOURCE + IDX_NEEDLE_LAST);

                const MechChar& XC = mXC;
                const MechGnrl& XG = mXG;

                RndAccConstItr haystackFirst = HSTCK_SOURCE
                                             + IDX_HSTCK_FIRST;
                RndAccConstItr haystackLast  = HSTCK_SOURCE
                                             + IDX_HSTCK_LAST;

                const Result expected = std::make_pair(
                                               HSTCK_SOURCE + IDX_EXPECT_FIRST,
                                               HSTCK_SOURCE + IDX_EXPECT_LAST);

                const Result resultXC = XC(haystackFirst, haystackLast);
                const Result resultXG = XG(haystackFirst, haystackLast);

                ASSERTV(LINE, expected == resultXC);
                ASSERTV(LINE, expected == resultXG);
            }
        }

        if (verbose) printf("\n" "Functor Forwarding" "\n");
        {
            typedef bsl::boyer_moore_horspool_searcher<const char *,
                                                       DefaultHash,
                                                       DefaultEqual> Sensitive;

            typedef bsl::boyer_moore_horspool_searcher<
                                  const char *,
                                  MyCaseInsensitiveCharHasher,
                                  MyCaseInsensitiveCharComparator> InSensitive;

            typedef bsl::pair<const char *, const char*> Result;

            const char *haystack =
              "When 'polish' starts a sentence I confuse it with 'Polish'.";
            // ----^----|----^----|----^----|----^----|----^----|----^----|
            // 1     | 10        20        30        40        50 |      60

            const char *needle = "Polish"; // length 6

            const Result expectedSensitive(haystack + 51,
                                           haystack + 51 + 6);

            const Result expectedInSensitive(haystack + 6,
                                             haystack + 6 + 6);

            Sensitive caseSensitiveSearcher(needle,
                                            needle + std::strlen(needle));

            InSensitive caseInSensitiveSearcher(needle,
                                                needle + std::strlen(needle));

            const Result resultSensitive = caseSensitiveSearcher(
                                             haystack,
                                             haystack + std::strlen(haystack));

            const Result resultInSensitive = caseInSensitiveSearcher(
                                             haystack,
                                             haystack + std::strlen(haystack));
            if (veryVerbose) {
                std::size_t offsetSensitive = resultSensitive.first
                                            - haystack;
                std::size_t lengthSensitive = resultSensitive.second
                                            - resultSensitive.first;

                std::size_t offsetInSensitive = resultInSensitive.first
                                              - haystack;
                std::size_t lengthInSensitive = resultInSensitive.second
                                              - resultInSensitive.first;

                P_(  offsetSensitive) P(  lengthSensitive)
                P_(offsetInSensitive) P(lengthInSensitive)
            }

            ASSERT(  expectedSensitive ==   resultSensitive);
            ASSERT(expectedInSensitive == resultInSensitive);
        }

        if (verbose) printf("\n" "Negative Tests" "\n");
        {
            CharArray<char> containerHavingRndAccIterators(
                                                    bsl::vector<char>('b', 5));

            typedef CharArray<char>::const_iterator    RndAccConstItr;
            typedef bsl::boyer_moore_horspool_searcher<RndAccConstItr> Mech;

            RndAccConstItr middleNeedle =
                                    containerHavingRndAccIterators.begin() + 2;

            bsls::AssertTestHandlerGuard hG;

            ASSERT_FAIL(Mech(middleNeedle, middleNeedle - 1));
            ASSERT_PASS(Mech(middleNeedle, middleNeedle + 0));
            ASSERT_PASS(Mech(middleNeedle, middleNeedle + 1));

            // Create arbitrary "needle".
            Mech mX(containerHavingRndAccIterators.begin(),
                    containerHavingRndAccIterators.end()); const Mech& X = mX;

            // Create arbitrary "haystack".

            const CharArray<char> haystack(bsl::vector<char>('a', 5));

            RndAccConstItr middleHaystack = haystack.begin() + 2;

            ASSERT_FAIL(X(middleHaystack, middleHaystack - 1));
            ASSERT_PASS(X(middleHaystack, middleHaystack + 0));
            ASSERT_PASS(X(middleHaystack, middleHaystack + 1));
        }
#else
        if (verbose) printf("Test Skipped: native searcher class used\n");
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_FUNCTORS
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // MOVE ASSIGNMENT
        //
        // Concerns:
        //: 1 The copied object has the same state as the original with respect
        //:   to the 'needleFirst', 'needleLast', 'hash', and 'equal'
        //:   attributes.
        //:
        //: 2 The assigned to object continues to use the allocator held before
        //:   assignment.
        //:
        //: 3 The 'rhs' object can be 'const'-qualified.
        //:
        //: 4 The 'rhs' object is left in the (implementation dependent)
        //:   expected (valid) state.
        //:
        //: 5 The return value of copy assignment has the expected value.
        //:
        //: 6 The behaviors hold true for both general and optimized
        //:   implementations.
        //:
        //: 7 All memory allocation by the special implementation (for
        //:   'char *') is exception safe.
        //:
        //: 8 An object is not changed by move assignment to itself.
        //
        // Plan:
        //: 1 Ad hoc test: Searcher objects are created and copied.
        //:
        //: 2 The accessors are used at each step to confirm the expected
        //:   state.
        //:
        //: 3 Exception safety of memory allocations is tested using the
        //:   'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros.
        //
        // Testing:
        //   b_m_h& operator=(MovableRef<b_m_h> rhs);
        // --------------------------------------------------------------------

        if (verbose) printf("\n" "MOVE ASSIGNMENT"
                            "\n" "==============" "\n");

        if (verbose) printf("General Implementation\n");
        {
            typedef  CharHashCaseInsensitive HASH;
            typedef CharEqualCaseInsensitive EQUAL;

            HASH   hashX(42);
            EQUAL equalX(42);

            HASH   hashY(43);
            EQUAL equalY(43);

            CharArray<char> containerHavingRandomIterators(
                                                    bsl::vector<char>('b', 5));

            typedef CharArray<char>::const_iterator           RandConstItr;
            typedef bslstl::BoyerMooreHorspoolSearcher<RandConstItr,
                                                       HASH,
                                                       EQUAL> Mech;

            RandConstItr needleFirst = containerHavingRandomIterators.begin();
            RandConstItr needleLast  = containerHavingRandomIterators.end();

            if (verbose) printf("default allocator\n");
            {
                bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);
                bslma::TestAllocator da("default",  veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                Mech mZ(needleFirst, needleLast, hashX, equalX, &sa);
                Mech mX(needleFirst, needleLast, hashX, equalX, &sa);
                Mech mY(needleLast,  needleLast, hashY, equalY);  // unalike

                const Mech& Z = mZ;
                const Mech& X = mX;
                const Mech& Y = mY;

                Mech *mR = &(mY = bslmf::MovableRefUtil::move(mX));  // ACTION
                ASSERT(mR == &mY);

                // The assigned to object state matches original ('Z') except
                // for the allocator.

                ASSERT(Z.needleFirst() == Y.needleFirst());
                ASSERT(Z.needleLast()  == Y.needleLast());
                ASSERT(Z.hash()        == Y.hash());
                ASSERT(Z.equal()       == Y.equal());
                ASSERT(Z.allocator()   != Y.allocator());
                ASSERT(&da             == Y.allocator());

                // The original object ('X') in expected (implementation
                // specific) state.

                ASSERT(Z.needleFirst() == X.needleFirst());
                ASSERT(Z.needleFirst() == X.needleLast());
                ASSERT(Z.hash()        == X.hash());
                ASSERT(Z.equal()       == X.equal());
                ASSERT(Z.allocator()   == X.allocator());
            }

            if (verbose) printf("supplied allocator\n");
            {
                bslma::TestAllocator saX("suppliedX", veryVeryVeryVerbose);
                bslma::TestAllocator saY("suppliedY", veryVeryVeryVerbose);

                Mech mZ(needleFirst, needleLast, hashX, equalX, &saX);
                Mech mX(needleFirst, needleLast, hashX, equalX, &saX);
                Mech mY(needleLast,  needleLast, hashY, equalX, &saY);
                                                                     // unalike

                const Mech& Z = mZ;
                const Mech& X = mX;
                const Mech& Y = mY;

                Mech *mR = &(mY = bslmf::MovableRefUtil::move(mX));  // ACTION
                ASSERT(mR == &mY);

                // The assigned to object state matches the 'rhs' except for
                // the allocator.

                ASSERT(Z.needleFirst() == Y.needleFirst());
                ASSERT(Z.needleLast()  == Y.needleLast());
                ASSERT(Z.hash()        == Y.hash());
                ASSERT(Z.equal()       == Y.equal());
                ASSERT(Z.allocator()   != Y.allocator());
                ASSERT(&saY            == Y.allocator());

                // The 'original' object state has the (implementation
                // dependent) expected (valid) state.

                ASSERT(Z.needleFirst() == X.needleFirst());
                ASSERT(Z.needleFirst() == X.needleLast());
                ASSERT(Z.hash()        == X.hash());
                ASSERT(Z.equal()       == X.equal());
                ASSERT(Z.allocator()   == X.allocator());
            }
        }

        if (verbose) printf("Specialized Implementation\n");
        {
            typedef bsl::hash<char>     HASH;   // These types lack equality
            typedef bsl::equal_to<char> EQUAL;  // comparison operators.

            HASH   hash;
            EQUAL equal;

            CharArray<char> containerHavingRandomIterators(
                                                    bsl::vector<char>('b', 5));

            typedef CharArray<char>::const_iterator           RandConstItr;
            typedef bslstl::BoyerMooreHorspoolSearcher<RandConstItr,
                                                       HASH,
                                                       EQUAL> Mech;

            RandConstItr needleFirst = containerHavingRandomIterators.begin();
            RandConstItr needleLast  = containerHavingRandomIterators.end();

            if (veryVerbose) printf("default allocator\n");
            {
                bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);
                bslma::TestAllocator da("default",  veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                Mech mZ(needleFirst, needleLast, hash, equal, &sa);
                Mech mX(needleFirst, needleLast, hash, equal, &sa);
                Mech mY(needleLast,  needleLast, hash, equal); // unalike

                const Mech& Z = mZ;
                const Mech& X = mX;
                const Mech& Y = mY;

                ASSERT(&da             == Y.allocator());

                Mech *mR = &(mY = bslmf::MovableRefUtil::move(mX));  // ACTION
                ASSERT(mR == &mY);

                // The assigned to object state matches 'rhs' except for the
                // allocator.

                ASSERT(Z.needleFirst() == Y.needleFirst());
                ASSERT(Z.needleLast()  == Y.needleLast());
             // ASSERT(Z.hash()        == Y.hash());
             // ASSERT(Z.equal()       == Y.equal());
                ASSERT(Z.allocator()   != Y.allocator());
                ASSERT(&da             == Y.allocator());

                // The 'rhs' object state has the (implementation dependent)
                // expected (valid) state.

                ASSERT(Z.needleFirst() == X.needleFirst());
                ASSERT(Z.needleFirst() == X.needleLast());
             // ASSERT(Z.hash()        == X.hash());
             // ASSERT(Z.equal()       == X.equal());
                ASSERT(Z.allocator()   == X.allocator());
            }

            if (veryVerbose) printf("supplied allocator\n");
            {
                bslma::TestAllocator saX("suppliedX", veryVeryVeryVerbose);
                bslma::TestAllocator saY("suppliedY", veryVeryVeryVerbose);

                Mech mZ(needleFirst, needleLast, hash, equal, &saX);
                Mech mX(needleFirst, needleLast, hash, equal, &saX);
                Mech mY(needleLast,  needleLast, hash, equal, &saY); // unalike

                const Mech& Z = mZ;
                const Mech& X = mX;
                const Mech& Y = mY;

                Mech *mR = &(mY = bslmf::MovableRefUtil::move(mX));  // ACTION
                ASSERT(mR == &mY);

                // The assigned to object state matches 'rhs' except for the
                // allocator.

                ASSERT(Z.needleFirst() == Y.needleFirst());
                ASSERT(Z.needleLast()  == Y.needleLast());
             // ASSERT(Z.hash()        == Y.hash());
             // ASSERT(Z.equal()       == Y.equal());
                ASSERT(Z.allocator()   != Y.allocator());
                ASSERT(&saY            == Y.allocator());

                // The 'rhs' object has the (implementation dependent)
                // expected (valid) state.

                ASSERT(Z.needleFirst() == X.needleFirst());
                ASSERT(Z.needleFirst() == X.needleLast());
             // ASSERT(Z.hash()        == X.hash());
             // ASSERT(Z.equal()       == X.equal());
                ASSERT(Z.allocator()   == X.allocator());
            }
        }

        if (verbose) printf("Compare results\n");
        {
            for (std::size_t ti = 0; ti < numDATA; ++ti) {
                const int         LINE      = DATA[ti].d_line;
                const char *const HAYSTACK  = DATA[ti].d_haystack_p;
                const char *const NEEDLE    = DATA[ti].d_needle_p;
                const int         OFFSET_CS = DATA[ti].d_offsetCs;
                const int         LENGTH_CS = DATA[ti].d_lengthCs;
                const int         OFFSET_CI = DATA[ti].d_offsetCi;
                const int         LENGTH_CI = DATA[ti].d_lengthCi;

                if (veryVerbose) {
                    P_(LINE      )
                    P_(HAYSTACK  )
                    P_(NEEDLE    )
                    P_(OFFSET_CS )
                    P_(LENGTH_CS )
                    P_(OFFSET_CI )
                    P (LENGTH_CI )
                }

                if (veryVerbose) printf("General Implementation\n");
                {
                    bsl::vector<char>  vectorOfChars;
                    loadVectorOfChars(&vectorOfChars, NEEDLE);

                    CharArray<char> needleContent(vectorOfChars);

                    typedef CharArray<char>::const_iterator RandConstItr;
                    typedef  CharHashCaseInsensitive        HASH;
                    typedef CharEqualCaseInsensitive        EQUAL;

                    typedef bslstl::BoyerMooreHorspoolSearcher<RandConstItr,
                                                               HASH,
                                                               EQUAL> Mech;

                    RandConstItr needleFirst = needleContent.begin();
                    RandConstItr needleLast  = needleContent.end();

                    Mech mZ(needleFirst, needleLast);
                    Mech mX(needleFirst, needleLast);
                    Mech mY(needleFirst, needleFirst);  // unalike

                    const Mech& Z = mZ;
                    const Mech& X = mX;
                    const Mech& Y = mY;

                    Mech *mR = &(mY = bslmf::MovableRefUtil::move(mX));  // ACT
                    ASSERT(mR == &mY);

                    typedef bsl::pair<const char *, const char *> Result;

                    const std::size_t HAYSTACK_LENGTH = std::strlen(HAYSTACK);

                    Result resultZ = Z(HAYSTACK, HAYSTACK + HAYSTACK_LENGTH);
                    Result resultX = X(HAYSTACK, HAYSTACK + HAYSTACK_LENGTH);
                    Result resultY = Y(HAYSTACK, HAYSTACK + HAYSTACK_LENGTH);

                    Result emptyNeedleResult(HAYSTACK, HAYSTACK);

                    ASSERT(resultZ           == resultY);
                    ASSERT(emptyNeedleResult == resultX);

                    mX = Y;

                    Result resultXrestored = X(HAYSTACK,
                                               HAYSTACK + HAYSTACK_LENGTH);
                    ASSERT(resultY == resultXrestored);
                }

                if (veryVerbose) printf("Specialized Implementation\n");
                {
                    typedef bslstl::BoyerMooreHorspoolSearcher<const char *>
                                                                         Mech;

                    Mech mZ(NEEDLE, NEEDLE + std::strlen(NEEDLE));
                    Mech mX(NEEDLE, NEEDLE + std::strlen(NEEDLE));
                    Mech mY(NEEDLE, NEEDLE);

                    const Mech& Z = mZ;
                    const Mech& X = mX;
                    const Mech& Y = mY;

                    Mech *mR = &(mY = bslmf::MovableRefUtil::move(mX));  // ACT
                    ASSERT(mR == &mY);

                    typedef bsl::pair<const char *, const char *> Result;

                    const std::size_t HAYSTACK_LENGTH = std::strlen(HAYSTACK);

                    Result resultZ = Z(HAYSTACK, HAYSTACK + HAYSTACK_LENGTH);
                    Result resultX = X(HAYSTACK, HAYSTACK + HAYSTACK_LENGTH);
                    Result resultY = Y(HAYSTACK, HAYSTACK + HAYSTACK_LENGTH);

                    Result emptyNeedleResult(HAYSTACK, HAYSTACK);

                    ASSERT(resultZ           == resultY);
                    ASSERT(emptyNeedleResult == resultX);

                    mX = Y;

                    Result resultXrestored = X(HAYSTACK,
                                               HAYSTACK + HAYSTACK_LENGTH);
                    ASSERT(resultY == resultXrestored);
                }
            }
        }

        if (verbose) printf("Special Implementation Exception Test\n");
        {
            CharArray<char> containerHavingRandomIterators(
                                                    bsl::vector<char>('b', 5));

            typedef CharArray<char>::const_iterator    RandConstItr;
            typedef bslstl::BoyerMooreHorspoolSearcher<RandConstItr> Mech;

            const std::size_t SHRT = (UCHAR_MAX + 1) * sizeof (unsigned char);
            const std::size_t TALL = (UCHAR_MAX + 1) * sizeof (
                          bsl::iterator_traits<RandConstItr>::difference_type);

            RandConstItr begin = containerHavingRandomIterators.begin();
            RandConstItr end   = containerHavingRandomIterators.end();

            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator sa("source",    veryVeryVeryVerbose);
            bslma::TestAllocator ta("target",    veryVeryVeryVerbose);

            for (char srcCfg = 'a'; srcCfg <= 'f'; ++srcCfg) {
            for (char dstCfg = 'a'; dstCfg <= 'f'; ++dstCfg) {

                if (veryVerbose) { P_(srcCfg) P(dstCfg) }

                Mech                 *srcMechPtr          = 0;
                bslma::TestAllocator *srcMechAllocatorPtr = 0;
                Int64                 srcNumBytes         = 0;

                Int64 numBytesAfore;
                Int64 numBytesAfter;

                switch (srcCfg) {
                  case 'a': {
                    numBytesAfore = sa.numBytesInUse();

                    srcMechPtr = new (fa) Mech(begin,
                                               begin,  // empty
                                               Mech::DefaultHash(),
                                               Mech::DefaultEqual(),
                                               &sa);

                    srcMechAllocatorPtr = &sa;

                    numBytesAfter = sa.numBytesInUse();
                    srcNumBytes   = numBytesAfter - numBytesAfore;
                  } break;
                  case 'b': {
                    numBytesAfore = sa.numBytesInUse();

                    srcMechPtr  = new (fa) Mech(begin,
                                                end, // short
                                                Mech::DefaultHash(),
                                                Mech::DefaultEqual(),
                                                &sa);
                    srcMechAllocatorPtr = &sa;

                    numBytesAfter = sa.numBytesInUse();
                    srcNumBytes   = numBytesAfter - numBytesAfore;
                  } break;
                  case 'c': {
                    numBytesAfore = sa.numBytesInUse();

                    srcMechPtr  = new (fa) Mech(HAYSTACK_TEXT_FIRST,
                                                HAYSTACK_TEXT_LAST, // long
                                                Mech::DefaultHash(),
                                                Mech::DefaultEqual(),
                                                &sa);
                    srcMechAllocatorPtr = &sa;

                    numBytesAfter = sa.numBytesInUse();
                    srcNumBytes   = numBytesAfter - numBytesAfore;
                  } break;
                  case 'd': {
                    numBytesAfore = ta.numBytesInUse();

                    srcMechPtr = new (fa) Mech(begin,
                                               begin,  // empty
                                               Mech::DefaultHash(),
                                               Mech::DefaultEqual(),
                                               &ta);  // target allocator
                    srcMechAllocatorPtr = &ta;

                    numBytesAfter = ta.numBytesInUse();
                    srcNumBytes   = numBytesAfter - numBytesAfore;
                  } break;
                  case 'e': {
                    numBytesAfore = ta.numBytesInUse();

                    srcMechPtr  = new (fa) Mech(begin,
                                                end, // short
                                                Mech::DefaultHash(),
                                                Mech::DefaultEqual(),
                                                &ta);  // target allocator
                    srcMechAllocatorPtr = &ta;

                    numBytesAfter = ta.numBytesInUse();
                    srcNumBytes   = numBytesAfter - numBytesAfore;
                  } break;
                  case 'f': {
                    numBytesAfore = ta.numBytesInUse();

                    srcMechPtr  = new (fa) Mech(HAYSTACK_TEXT_FIRST,
                                                HAYSTACK_TEXT_LAST, // long
                                                Mech::DefaultHash(),
                                                Mech::DefaultEqual(),
                                                &ta); // target allocator
                    srcMechAllocatorPtr = &ta;

                    numBytesAfter = ta.numBytesInUse();
                    srcNumBytes   = numBytesAfter - numBytesAfore;
                  } break;
                  default: {
                    ASSERTV(srcCfg, !"Bad allocator config.");
                  } break;
                }

                Mech& mS = *srcMechPtr;
                (void)srcMechAllocatorPtr;

                switch (srcCfg) {
                    case 'a':
                    case 'd': ASSERT(   0 == srcNumBytes); break;
                    case 'b':
                    case 'e': ASSERT(SHRT == srcNumBytes); break;
                    case 'c':
                    case 'f': ASSERT(TALL == srcNumBytes); break;
                    default : ASSERTV(dstCfg, !"Bad allocator config."); break;
                }

                Mech                 *dstMechPtr          = 0;
                bslma::TestAllocator *dstMechAllocatorPtr = 0;
                Int64                 dstNumBytes         = 0;

                switch (dstCfg) {
                  case 'a': {
                    numBytesAfore = sa.numBytesInUse();

                    dstMechPtr = new (fa) Mech(begin,
                                               begin,  // empty
                                               Mech::DefaultHash(),
                                               Mech::DefaultEqual(),
                                               &sa);
                    dstMechAllocatorPtr = &sa;

                    numBytesAfter = sa.numBytesInUse();
                    dstNumBytes   = numBytesAfter - numBytesAfore;
                  } break;
                  case 'b': {
                    numBytesAfore = sa.numBytesInUse();

                    dstMechPtr = new (fa) Mech(begin,
                                               end, // short
                                               Mech::DefaultHash(),
                                               Mech::DefaultEqual(),
                                               &sa);
                    dstMechAllocatorPtr = &sa;

                    numBytesAfter = sa.numBytesInUse();
                    dstNumBytes   = numBytesAfter - numBytesAfore;
                  } break;
                  case 'c': {
                    numBytesAfore = sa.numBytesInUse();

                    dstMechPtr = new (fa) Mech(HAYSTACK_TEXT_FIRST,
                                               HAYSTACK_TEXT_LAST, // long
                                               Mech::DefaultHash(),
                                               Mech::DefaultEqual(),
                                               &sa);
                    dstMechAllocatorPtr = &sa;

                    numBytesAfter = sa.numBytesInUse();
                    dstNumBytes   = numBytesAfter - numBytesAfore;
                  } break;
                  case 'd': {
                    numBytesAfore = ta.numBytesInUse();

                    dstMechPtr = new (fa) Mech(begin,
                                               begin,  // empty
                                               Mech::DefaultHash(),
                                               Mech::DefaultEqual(),
                                               &ta);  // target allocator
                    dstMechAllocatorPtr = &ta;

                    numBytesAfter = ta.numBytesInUse();
                    dstNumBytes   = numBytesAfter - numBytesAfore;
                  } break;
                  case 'e': {
                    numBytesAfore = ta.numBytesInUse();

                    dstMechPtr = new (fa) Mech(begin,
                                               end, // short
                                               Mech::DefaultHash(),
                                               Mech::DefaultEqual(),
                                               &ta);  // target allocator
                    dstMechAllocatorPtr = &ta;

                    numBytesAfter = ta.numBytesInUse();
                    dstNumBytes   = numBytesAfter - numBytesAfore;
                  } break;
                  case 'f': {
                    numBytesAfore = ta.numBytesInUse();

                    dstMechPtr = new (fa) Mech(HAYSTACK_TEXT_FIRST,
                                               HAYSTACK_TEXT_LAST, // long
                                               Mech::DefaultHash(),
                                               Mech::DefaultEqual(),
                                               &ta);  // target allocator
                    dstMechAllocatorPtr = &ta;

                    numBytesAfter = ta.numBytesInUse();
                    dstNumBytes   = numBytesAfter - numBytesAfore;
                  } break;
                  default: {
                    ASSERTV(dstCfg, !"Bad allocator config.");
                  } break;
                }

                switch (dstCfg) {
                    case 'a':
                    case 'd': ASSERT(   0 == dstNumBytes); break;
                    case 'b':
                    case 'e': ASSERT(SHRT == dstNumBytes); break;
                    case 'c':
                    case 'f': ASSERT(TALL == dstNumBytes); break;
                    default : ASSERTV(dstCfg, !"Bad allocator config."); break;
                }

                Mech&  mD  = *dstMechPtr;

#ifdef BDE_BUILD_TARGET_EXC
                bslma::TestAllocator& dstMechAllocator = *dstMechAllocatorPtr;
#endif

                bool expectAllocation = expectAllocationCase7(
                                                          dstNumBytes,
                                                          dstMechAllocatorPtr,
                                                          srcNumBytes,
                                                          srcMechAllocatorPtr);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(dstMechAllocator) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    bslma::TestAllocatorMonitor tam(dstMechAllocatorPtr);

                                                                     // ACTION
                    Mech *mR = &(mD = bslmf::MovableRefUtil::move(mS));
                    ASSERT(mR == &mD);

                    if (expectAllocation) {
                        ASSERT(tam.isTotalUp());
                    } else {
                        ASSERT(tam.isTotalSame());
                    }

                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                fa.deleteObject(srcMechPtr);
                fa.deleteObject(dstMechPtr);

                ASSERT(0 == fa.numBlocksInUse());
                ASSERT(0 == fa.numBytesInUse());
            }
            }

            ASSERT(0 == ta.numBlocksInUse());
            ASSERT(0 == ta.numBytesInUse());
            ASSERT(0 == sa.numBlocksInUse());
            ASSERT(0 == sa.numBytesInUse());
        }

        if (verbose) printf("Alias Safety\n");
        {
            typedef  CharHashCaseInsensitive HASH;
            typedef CharEqualCaseInsensitive EQUAL;

            HASH   hash(42);
            EQUAL equal(42);

            CharArray<char> containerHavingRandomIterators(
                                                    bsl::vector<char>('b', 5));

            typedef CharArray<char>::const_iterator    RandConstItr;
            typedef bslstl::BoyerMooreHorspoolSearcher<RandConstItr,
                                                       HASH,
                                                       EQUAL> Mech;

            RandConstItr begin = containerHavingRandomIterators.begin();
            RandConstItr end   = containerHavingRandomIterators.end();

            Mech mZ(begin, end, hash, equal); const Mech& Z = mZ;
            Mech mX(begin, end, hash, equal); const Mech& X = mX;

            Mech *mR = &(mX = bslmf::MovableRefUtil::move(mX));  // ACTION

            ASSERT(mR == &mX);

            ASSERT(Z.needleFirst() == X.needleFirst());
            ASSERT(Z.needleLast()  == X.needleLast());
            ASSERT(Z.hash()        == X.hash());
            ASSERT(Z.equal()       == X.equal());
            ASSERT(Z.allocator()   == X.allocator());
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // MOVE CONSTRUCTORS
        //
        // Concerns:
        //: 1 The copied object has the same state as the original with respect
        //:   to the 'needleFirst', 'needleLast', 'hash', and 'equal'
        //:   attributes.
        //:
        //: 2 The allocator of the new object is the supplied allocator, if
        //:   specified, and the default allocator otherwise.  The allocator of
        //:   the original object is irrelevant.
        //:
        //: 3 A 'const'-qualified searcher object can be copied.
        //:
        //: 4 The 'original' object is not changed when copied.
        //:
        //: 5 The behaviors hold true for both general and optimized
        //:   implementations.
        //
        //: 6 All memory allocation by the special implementation (for
        //:   'char *') is exception safe.
        //
        // Plan:
        //: 1 Ad hoc test: Searcher objects are created and copied.
        //:
        //: 2 The accessors are used at each step to confirm the expected
        //:   state.
        //:
        //: 3 Exception safety of memory allocations is tested using the
        //:   'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros.
        //
        // Testing:
        //   BoyerMooreHorspoolSearcher(MovableRef<b_m_h_s> original);
        //   BoyerMooreHorspoolSearcher(MovableRef<b_m_h_s> original, *bA);
        // --------------------------------------------------------------------

        if (verbose) printf("\n" "MOVE CONSTRUCTORS"
                            "\n" "=================" "\n");

        typedef  CharHashCaseInsensitive  HASH_GNRL;
        typedef CharEqualCaseInsensitive EQUAL_GNRL;

        typedef bsl::hash<char>      HASH_CHAR;  // These types lack equality
        typedef bsl::equal_to<char> EQUAL_CHAR;  // comparison operators.

        typedef CharArray<char>::const_iterator RandConstItr;

        testMoveConstructors<RandConstItr, HASH_CHAR, EQUAL_CHAR, false>();
        testMoveConstructors<RandConstItr, HASH_GNRL, EQUAL_GNRL, true >();

        if (verbose) printf("Special Implementation Exception Test\n");
        {
            CharArray<char> containerHavingRandomIterators(
                                                    bsl::vector<char>('b', 5));

            typedef CharArray<char>::const_iterator    RandConstItr;
            typedef bslstl::BoyerMooreHorspoolSearcher<RandConstItr> Mech;
            typedef BloombergLP::bslmf::MovableRefUtil MoveUtil;

            RandConstItr begin = containerHavingRandomIterators.begin();
            RandConstItr end   = containerHavingRandomIterators.end();

            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator sa("source",    veryVeryVeryVerbose);
            bslma::TestAllocator ta("target",    veryVeryVeryVerbose);

            for (char cfg = 'a'; cfg <= 'c'; ++cfg) {

                if (veryVerbose) { P_(cfg) }

                Mech *mechPtr = 0;

                switch (cfg) {
                  case 'a': {
                    mechPtr = new (fa) Mech(begin,
                                            begin,  // empty
                                            Mech::DefaultHash(),
                                            Mech::DefaultEqual(),
                                            &sa);
                  } break;
                  case 'b': {
                    mechPtr = new (fa) Mech(begin,
                                            end, // short
                                            Mech::DefaultHash(),
                                            Mech::DefaultEqual(),
                                            &sa);
                  } break;
                  case 'c': {
                    mechPtr  = new (fa) Mech(HAYSTACK_TEXT_FIRST,
                                             HAYSTACK_TEXT_LAST, // long
                                             Mech::DefaultHash(),
                                             Mech::DefaultEqual(),
                                             &sa);
                  } break;
                  default: {
                    ASSERTV(cfg, !"Bad allocator config.");
                  } break;
                }

                Mech& mS = *mechPtr; const Mech& S = mS;

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody: 1 arg) }

                    bslma::Allocator *srcAllocator = S.allocator();

                    Int64 numBytesInUseAfore = sa.numBytesInUse();

                    Mech mechCopy(MoveUtil::move(mS)); // ACTION

                    Int64 numBytesInUseAfter = sa.numBytesInUse();

                    Int64 numBytesInUseChange = numBytesInUseAfter
                                              - numBytesInUseAfore;

                    if (veryVeryVerbose) {
                        P_(numBytesInUseAfore)
                        P_(numBytesInUseAfter)
                         P(numBytesInUseChange);
                    }

                    ASSERT(0            == numBytesInUseChange);
                    ASSERT(srcAllocator == mechCopy.allocator());
                    ASSERT(srcAllocator == S.allocator());

                    fa.deleteObject(mechPtr);

                    ASSERT(0 == fa.numBlocksInUse());
                    ASSERT(0 == fa.numBytesInUse());

                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERT(0 == sa.numBlocksInUse());
                ASSERT(0 == sa.numBytesInUse());
            }

            for (char cfg = 'a'; cfg <= 'f'; ++cfg) {

                if (veryVerbose) { P_(cfg) }

                Mech *mechPtr = 0;

                switch (cfg) {
                  case 'a': {
                    mechPtr = new (fa) Mech(begin,
                                            begin,  // empty
                                            Mech::DefaultHash(),
                                            Mech::DefaultEqual(),
                                            &sa);
                  } break;
                  case 'b': {
                    mechPtr = new (fa) Mech(begin,
                                            end, // short
                                            Mech::DefaultHash(),
                                            Mech::DefaultEqual(),
                                            &sa);
                  } break;
                  case 'c': {
                    mechPtr  = new (fa) Mech(HAYSTACK_TEXT_FIRST,
                                             HAYSTACK_TEXT_LAST,  // long
                                             Mech::DefaultHash(),
                                             Mech::DefaultEqual(),
                                             &sa);
                  } break;
                  case 'd': {
                    mechPtr = new (fa) Mech(begin,
                                            begin,  // empty
                                            Mech::DefaultHash(),
                                            Mech::DefaultEqual(),
                                            &ta);  // same allocator as target
                  } break;
                  case 'e': {
                    mechPtr = new (fa) Mech(begin,
                                            end,  // short
                                            Mech::DefaultHash(),
                                            Mech::DefaultEqual(),
                                            &ta);  // same allocator as target
                  } break;
                  case 'f': {
                    mechPtr  = new (fa) Mech(HAYSTACK_TEXT_FIRST,
                                             HAYSTACK_TEXT_LAST, // long
                                             Mech::DefaultHash(),
                                             Mech::DefaultEqual(),
                                             &ta);  // same allocator as target
                  } break;
                  default: {
                    ASSERTV(cfg, !"Bad allocator config.");
                  } break;
                }

                Mech& mS = *mechPtr; const Mech& S = mS;

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(ta) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody: 2 arg) }

                    bslma::Allocator *srcAllocator = S.allocator();

                    Int64 numBytesInUseAfore = ta.numBytesInUse();

                    Mech mechCopy(MoveUtil::move(mS), &ta); // ACTION

                    Int64 numBytesInUseAfter = ta.numBytesInUse();

                    Int64 numBytesInUseChange = numBytesInUseAfter
                                              - numBytesInUseAfore;

                    if (veryVeryVerbose) {
                        P_(numBytesInUseAfore)
                        P_(numBytesInUseAfter)
                         P(numBytesInUseChange);
                    }

                    bool expectNumBytesInUseSame =
                                        'a' == cfg            // empty needle
                                     || 'd' == cfg            // empty needle
                                     || &ta == srcAllocator;  // same allocator

                    if (expectNumBytesInUseSame) {
                        ASSERT(0 == numBytesInUseChange);
                    } else {
                        ASSERT(0 <  numBytesInUseChange);
                    }
                    ASSERT(&ta          == mechCopy.allocator());
                    ASSERT(srcAllocator == S.allocator());

                    fa.deleteObject(mechPtr);

                    ASSERT(0 == fa.numBlocksInUse());
                    ASSERT(0 == fa.numBytesInUse());

                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERT(0 == sa.numBlocksInUse());
                ASSERT(0 == sa.numBytesInUse());
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // COPY ASSIGNMENT
        //
        // Concerns:
        //: 1 The copied object has the same state as the original with respect
        //:   to the 'needleFirst', 'needleLast', 'hash', and 'equal'
        //:   attributes.
        //:
        //: 2 The assigned to object continues to use the allocator held before
        //:   assignment.
        //:
        //: 3 The 'rhs' object can be 'const'-qualified.
        //:
        //: 4 The 'rhs' object is not changed when assigned from.
        //:
        //: 5 The return value of copy assignment has the expected value.
        //:
        //: 6 The behaviors hold true for both general and optimized
        //:   implementations.
        //:
        //: 7 All memory allocation by the special implementation (for
        //:   'char *') is exception safe.
        //
        // Plan:
        //: 1 Ad hoc test: Searcher objects are created and copied.
        //:
        //: 2 The accessors are used at each step to confirm the expected
        //:   state.
        //:
        //: 3 Exception safety of memory allocations is tested using the
        //:   'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros.
        //
        // Testing:
        //   b_m_h& operator=(const b_m_h& rhs);
        // --------------------------------------------------------------------

        if (verbose) printf("\n" "COPY ASSIGNMENT"
                            "\n" "==============" "\n");

        if (veryVerbose) printf("General Implementation\n");
        {
            typedef  CharHashCaseInsensitive HASH;
            typedef CharEqualCaseInsensitive EQUAL;

            HASH   hashX(42);
            EQUAL equalX(42);

            HASH   hashY(43);
            EQUAL equalY(43);

            CharArray<char> containerHavingRandomIterators(
                                                    bsl::vector<char>('b', 5));

            typedef CharArray<char>::const_iterator           RandConstItr;
            typedef bslstl::BoyerMooreHorspoolSearcher<RandConstItr,
                                                       HASH,
                                                       EQUAL> Mech;

            RandConstItr needleFirst = containerHavingRandomIterators.begin();
            RandConstItr needleLast  = containerHavingRandomIterators.end();

            if (veryVerbose) printf("default allocator\n");
            {
                bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);
                bslma::TestAllocator da("default",  veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                Mech mZ(needleFirst, needleLast, hashX, equalX, &sa);
                Mech mX(needleFirst, needleLast, hashX, equalX, &sa);
                Mech mY(needleLast,  needleLast, hashY, equalY);  // unalike

                const Mech& Z = mZ;
                const Mech& X = mX;
                const Mech& Y = mY;

                Mech *mR = &(mY = X);  // ACTION
                ASSERT(mR == &mY);

                // The copied object state matches 'original' except for the
                // allocator.

                ASSERT(X.needleFirst() == Y.needleFirst());
                ASSERT(X.needleLast()  == Y.needleLast());
                ASSERT(X.hash()        == Y.hash());
                ASSERT(X.equal()       == Y.equal());
                ASSERT(X.allocator()   != Y.allocator());
                ASSERT(&da             == Y.allocator());

                // The 'original' object state unchanged since construction.

                ASSERT(Z.needleFirst() == X.needleFirst());
                ASSERT(Z.needleLast()  == X.needleLast());
                ASSERT(Z.hash()        == X.hash());
                ASSERT(Z.equal()       == X.equal());
                ASSERT(Z.allocator()   == X.allocator());
            }

            if (veryVerbose) printf("supplied allocator\n");
            {
                bslma::TestAllocator saX("suppliedX", veryVeryVeryVerbose);
                bslma::TestAllocator saY("suppliedY", veryVeryVeryVerbose);

                Mech mZ(needleFirst, needleLast, hashX, equalX, &saX);
                Mech mX(needleFirst, needleLast, hashX, equalX, &saX);
                Mech mY(needleLast,  needleLast, hashY, equalX, &saY);
                                                                     // unalike

                const Mech& Z = mZ;
                const Mech& X = mX;
                const Mech& Y = mY;

                Mech *mR = &(mY = X);  // ACTION
                ASSERT(mR == &mY);

                // The copied object state matches 'original' except for the
                // allocator.

                ASSERT(X.needleFirst() == Y.needleFirst());
                ASSERT(X.needleLast()  == Y.needleLast());
                ASSERT(X.hash()        == Y.hash());
                ASSERT(X.equal()       == Y.equal());
                ASSERT(X.allocator()   != Y.allocator());
                ASSERT(&saY            == Y.allocator());

                // The 'original' object state unchanged since construction.

                ASSERT(Z.needleFirst() == X.needleFirst());
                ASSERT(Z.needleLast()  == X.needleLast());
                ASSERT(Z.hash()        == X.hash());
                ASSERT(Z.equal()       == X.equal());
                ASSERT(Z.allocator()   == X.allocator());
            }
        }

        if (veryVerbose) printf("Specialized Implementation\n");
        {
            typedef bsl::hash<char>     HASH;   // These types lack equality
            typedef bsl::equal_to<char> EQUAL;  // comparison operators.

            HASH   hash;
            EQUAL equal;

            CharArray<char> containerHavingRandomIterators(
                                                    bsl::vector<char>('b', 5));

            typedef CharArray<char>::const_iterator           RandConstItr;
            typedef bslstl::BoyerMooreHorspoolSearcher<RandConstItr,
                                                       HASH,
                                                       EQUAL> Mech;

            RandConstItr needleFirst = containerHavingRandomIterators.begin();
            RandConstItr needleLast  = containerHavingRandomIterators.end();

            if (veryVerbose) printf("default allocator\n");
            {
                bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);
                bslma::TestAllocator da("default",  veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                Mech mZ(needleFirst, needleLast, hash, equal, &sa);
                Mech mX(needleFirst, needleLast, hash, equal, &sa);
                Mech mY(needleLast,  needleLast, hash, equal); // unalike

                const Mech& Z = mZ;
                const Mech& X = mX;
                const Mech& Y = mY;

                ASSERT(&da             == Y.allocator());

                Mech *mR = &(mY = X);  // ACTION
                ASSERT(mR == &mY);

                // The copied object state matches 'original' except for the
                // allocator.

                ASSERT(X.needleFirst() == Y.needleFirst());
                ASSERT(X.needleLast()  == Y.needleLast());
             // ASSERT(X.hash()        == Y.hash());
             // ASSERT(X.equal()       == Y.equal());
                ASSERT(X.allocator()   != Y.allocator());
                ASSERT(&da             == Y.allocator());

                // The 'original' object state unchanged since construction.

                ASSERT(Z.needleFirst() == X.needleFirst());
                ASSERT(Z.needleLast()  == X.needleLast());
             // ASSERT(Z.hash()        == X.hash());
             // ASSERT(Z.equal()       == X.equal());
                ASSERT(Z.allocator()   == X.allocator());
            }

            if (veryVerbose) printf("supplied allocator\n");
            {
                bslma::TestAllocator saX("suppliedX", veryVeryVeryVerbose);
                bslma::TestAllocator saY("suppliedY", veryVeryVeryVerbose);

                Mech mZ(needleFirst, needleLast, hash, equal, &saX);
                Mech mX(needleFirst, needleLast, hash, equal, &saX);
                Mech mY(needleLast,  needleLast, hash, equal, &saY); // unalike

                const Mech& Z = mZ;
                const Mech& X = mX;
                const Mech& Y = mY;

                Mech *mR = &(mY = X);  // ACTION
                ASSERT(mR == &mY);

                // The copied object state matches 'original' except for the
                // allocator.

                ASSERT(X.needleFirst() == Y.needleFirst());
                ASSERT(X.needleLast()  == Y.needleLast());
             // ASSERT(X.hash()        == Y.hash());
             // ASSERT(X.equal()       == Y.equal());
                ASSERT(X.allocator()   != Y.allocator());
                ASSERT(&saY            == Y.allocator());

                // The 'original' object state unchanged since construction.

                ASSERT(Z.needleFirst() == X.needleFirst());
                ASSERT(Z.needleLast()  == X.needleLast());
             // ASSERT(Z.hash()        == X.hash());
             // ASSERT(Z.equal()       == X.equal());
                ASSERT(Z.allocator()   == X.allocator());
            }
        }

        if (veryVerbose) printf("Compare results\n");
        {
            for (std::size_t ti = 0; ti < numDATA; ++ti) {
                const int         LINE      = DATA[ti].d_line;
                const char *const HAYSTACK  = DATA[ti].d_haystack_p;
                const char *const NEEDLE    = DATA[ti].d_needle_p;
                const int         OFFSET_CS = DATA[ti].d_offsetCs;
                const int         LENGTH_CS = DATA[ti].d_lengthCs;
                const int         OFFSET_CI = DATA[ti].d_offsetCi;
                const int         LENGTH_CI = DATA[ti].d_lengthCi;

                if (veryVerbose) {
                    P_(LINE      )
                    P_(HAYSTACK  )
                    P_(NEEDLE    )
                    P_(OFFSET_CS )
                    P_(LENGTH_CS )
                    P_(OFFSET_CI )
                    P (LENGTH_CI )
                }

                if (veryVerbose) printf("General Implementation\n");
                {
                    bsl::vector<char> vectorOfChars;
                    loadVectorOfChars(&vectorOfChars, NEEDLE);

                    CharArray<char> needleContent(vectorOfChars);

                    typedef CharArray<char>::const_iterator RandConstItr;
                    typedef  CharHashCaseInsensitive        HASH;
                    typedef CharEqualCaseInsensitive        EQUAL;

                    typedef bslstl::BoyerMooreHorspoolSearcher<RandConstItr,
                                                               HASH,
                                                               EQUAL> Mech;

                    RandConstItr needleFirst = needleContent.begin();
                    RandConstItr needleLast  = needleContent.end();

                    Mech mZ(needleFirst, needleLast);
                    Mech mX(needleFirst, needleLast);
                    Mech mY(needleFirst, needleFirst);  // unalike

                    const Mech& Z = mZ;
                    const Mech& X = mX;
                    const Mech& Y = mY;

                    Mech *mR = &(mY = X);  // ACTION
                    ASSERT(mR == &mY);

                    typedef bsl::pair<const char *, const char *> Result;

                    const std::size_t HAYSTACK_LENGTH = std::strlen(HAYSTACK);

                    Result resultZ = Z(HAYSTACK, HAYSTACK + HAYSTACK_LENGTH);
                    Result resultX = X(HAYSTACK, HAYSTACK + HAYSTACK_LENGTH);
                    Result resultY = Y(HAYSTACK, HAYSTACK + HAYSTACK_LENGTH);

                    ASSERT(resultZ == resultX);
                    ASSERT(resultX == resultY);
                }

                if (veryVerbose) printf("Specialized Implementation\n");
                {
                    typedef bslstl::BoyerMooreHorspoolSearcher<const char *>
                                                                         Mech;

                    Mech mZ(NEEDLE, NEEDLE + std::strlen(NEEDLE));
                    Mech mX(NEEDLE, NEEDLE + std::strlen(NEEDLE));
                    Mech mY(NEEDLE, NEEDLE);

                    const Mech& Z = mZ;
                    const Mech& X = mX;
                    const Mech& Y = mY;

                    Mech *mR = &(mY = X);  // ACTION
                    ASSERT(mR == &mY);

                    typedef bsl::pair<const char *, const char *> Result;

                    const std::size_t HAYSTACK_LENGTH = std::strlen(HAYSTACK);

                    Result resultZ = Z(HAYSTACK, HAYSTACK + HAYSTACK_LENGTH);
                    Result resultX = X(HAYSTACK, HAYSTACK + HAYSTACK_LENGTH);
                    Result resultY = Y(HAYSTACK, HAYSTACK + HAYSTACK_LENGTH);

                    ASSERT(resultZ == resultX);
                    ASSERT(resultX == resultY);
                }
            }
        }

        if (verbose) printf("Special Implementation Exception Test\n");
        {
            CharArray<char> containerHavingRandomIterators(
                                                    bsl::vector<char>('b', 5));

            typedef CharArray<char>::const_iterator    RandConstItr;
            typedef bslstl::BoyerMooreHorspoolSearcher<RandConstItr> Mech;

            RandConstItr begin = containerHavingRandomIterators.begin();
            RandConstItr end   = containerHavingRandomIterators.end();

            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

            for (char srcCfg = 'a'; srcCfg <= 'c'; ++srcCfg) {
            for (char dstCfg = 'a'; dstCfg <= 'c'; ++dstCfg) {

                if (veryVerbose) { P_(srcCfg) P(dstCfg) }

                Mech  *srcMechPtr  = 0;
                Int64  srcNumBytes = 0;

                Int64 numBytesAfore;
                Int64 numBytesAfter;

                switch (srcCfg) {
                  case 'a': {
                    numBytesAfore = sa.numBytesInUse();

                    srcMechPtr = new (fa) Mech(begin,
                                               begin,  // empty
                                               Mech::DefaultHash(),
                                               Mech::DefaultEqual(),
                                               &sa);

                    numBytesAfter = sa.numBytesInUse();
                    srcNumBytes   = numBytesAfter - numBytesAfore;
                  } break;
                  case 'b': {
                    numBytesAfore = sa.numBytesInUse();

                    srcMechPtr  = new (fa) Mech(begin,
                                                end, // short
                                                Mech::DefaultHash(),
                                                Mech::DefaultEqual(),
                                                &sa);

                    numBytesAfter = sa.numBytesInUse();
                    srcNumBytes   = numBytesAfter - numBytesAfore;
                  } break;
                  case 'c': {
                    numBytesAfore = sa.numBytesInUse();

                    srcMechPtr  = new (fa) Mech(HAYSTACK_TEXT_FIRST,
                                                HAYSTACK_TEXT_LAST, // long
                                                Mech::DefaultHash(),
                                                Mech::DefaultEqual(),
                                                &sa);

                    numBytesAfter = sa.numBytesInUse();
                    srcNumBytes   = numBytesAfter - numBytesAfore;
                  } break;
                  default: {
                    ASSERTV(srcCfg, !"Bad allocator config.");
                  } break;
                }

                Mech& mS = *srcMechPtr; const Mech& S = mS;

                Mech  *dstMechPtr  = 0;
                Int64  dstNumBytes = 0;

                switch (dstCfg) {
                  case 'a': {
                    numBytesAfore = sa.numBytesInUse();

                    dstMechPtr = new (fa) Mech(begin,
                                               begin,  // empty
                                               Mech::DefaultHash(),
                                               Mech::DefaultEqual(),
                                               &sa);

                    numBytesAfter = sa.numBytesInUse();
                    dstNumBytes   = numBytesAfter - numBytesAfore;
                  } break;
                  case 'b': {
                    numBytesAfore = sa.numBytesInUse();

                    dstMechPtr = new (fa) Mech(begin,
                                               end, // short
                                               Mech::DefaultHash(),
                                               Mech::DefaultEqual(),
                                               &sa);

                    numBytesAfter = sa.numBytesInUse();
                    dstNumBytes   = numBytesAfter - numBytesAfore;
                  } break;
                  case 'c': {
                    numBytesAfore = sa.numBytesInUse();

                    dstMechPtr = new (fa) Mech(HAYSTACK_TEXT_FIRST,
                                               HAYSTACK_TEXT_LAST, // long
                                               Mech::DefaultHash(),
                                               Mech::DefaultEqual(),
                                               &sa);

                    numBytesAfter = sa.numBytesInUse();
                    dstNumBytes   = numBytesAfter - numBytesAfore;
                  } break;
                  default: {
                    ASSERTV(dstCfg, !"Bad allocator config.");
                  } break;
                }

                Mech& mD = *dstMechPtr;

                char expectedMemoryChange =  srcNumBytes > dstNumBytes ? '+' :
                                             srcNumBytes < dstNumBytes ? '-' :
                                             /* equal */                 '=' ;

                if (veryVerbose) { P(expectedMemoryChange) }

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    Int64 numBytesInUseAfore = sa.numBytesInUse();

                    Mech *mR = &(mD = S);  // ACTION
                    ASSERT(mR == &mD);

                    Int64 numBytesInUseAfter = sa.numBytesInUse();

                    Int64 numBytesInUseChange = numBytesInUseAfter
                                              - numBytesInUseAfore;

                    if (veryVeryVerbose) {
                        P_(srcNumBytes)
                         P(dstNumBytes)
                        P_(numBytesInUseAfore)
                        P_(numBytesInUseAfter)
                         P(numBytesInUseChange);
                    }

                    switch (expectedMemoryChange) {
                      case '-': ASSERT(0 >  numBytesInUseChange); break;
                      case '=': ASSERT(0 == numBytesInUseChange); break;
                      case '+': ASSERT(0 <  numBytesInUseChange); break;
                      default: {
                        ASSERTV(dstCfg, !"Expected memory change.");
                      } break;
                    };

                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                fa.deleteObject(srcMechPtr);
                fa.deleteObject(dstMechPtr);

                ASSERT(0 == sa.numBlocksInUse());
                ASSERT(0 == sa.numBytesInUse());

                ASSERT(0 == fa.numBlocksInUse());
                ASSERT(0 == fa.numBytesInUse());
            }
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // COPY CONSTRUCTORS
        //
        // Concerns:
        //: 1 The copied object has the same state as the original with respect
        //:   to the 'needleFirst', 'needleLast', 'hash', and 'equal'
        //:   attributes.
        //:
        //: 2 The allocator of the new object is the supplied allocator, if
        //:   specified, and the default allocator otherwise.  The allocator of
        //:   the original object is irrelevant.
        //:
        //: 3 A 'const'-qualified searcher object can be copied.
        //:
        //: 4 The 'original' object is not changed when copied.
        //:
        //: 5 The behaviors hold true for both general and optimized
        //:   implementations.
        //:
        //: 6 All memory allocation by the special implementation (for
        //:   'char *') is exception safe.
        //
        // Plan:
        //: 1 Ad hoc test: Searcher objects are created and copied.
        //:
        //: 2 The accessors are used at each step to confirm the expected
        //:   state.
        //:
        //: 3 Exception safety of memory allocations is tested using the
        //:   'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros.
        //
        // Testing:
        //   BoyerMooreHorspoolSearcher(const b_m_h_s& original);
        //   BoyerMooreHorspoolSearcher(const b_m_h_s& original, *bA);
        // --------------------------------------------------------------------

        if (verbose) printf("\n" "COPY CONSTRUCTORS"
                            "\n" "=================" "\n");

        if (veryVerbose) printf("General Implementation\n");
        {
            typedef  CharHashCaseInsensitive HASH;
            typedef CharEqualCaseInsensitive EQUAL;

            HASH   hash;
            EQUAL equal;

            CharArray<char> containerHavingRandomIterators(
                                                    bsl::vector<char>('b', 5));

            typedef CharArray<char>::const_iterator           RandConstItr;
            typedef bslstl::BoyerMooreHorspoolSearcher<RandConstItr,
                                                       HASH,
                                                       EQUAL> Mech;

            RandConstItr needleFirst = containerHavingRandomIterators.begin();
            RandConstItr needleLast  = containerHavingRandomIterators.end();

            if (veryVerbose) printf("default allocator\n");
            {
                bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);
                bslma::TestAllocator da("default",  veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                Mech mZ(needleFirst, needleLast, hash, equal, &sa);
                Mech mX(needleFirst, needleLast, hash, equal, &sa);

                const Mech& Z = mZ;
                const Mech& X = mX;

                Mech mY(X); const Mech& Y = mY;  // ACTION

                // The copied object state matches 'original' except for the
                // allocator.

                ASSERT(X.needleFirst() == Y.needleFirst());
                ASSERT(X.needleLast()  == Y.needleLast());
                ASSERT(X.hash()        == Y.hash());
                ASSERT(X.equal()       == Y.equal());
                ASSERT(X.allocator()   != Y.allocator());
                ASSERT(&da             == Y.allocator());

                // The 'original' object state unchanged since construction.

                ASSERT(Z.needleFirst() == X.needleFirst());
                ASSERT(Z.needleLast()  == X.needleLast());
                ASSERT(Z.hash()        == X.hash());
                ASSERT(Z.equal()       == X.equal());
                ASSERT(Z.allocator()   == X.allocator());
            }

            if (veryVerbose) printf("supplied allocator\n");
            {
                bslma::TestAllocator saX("suppliedX", veryVeryVeryVerbose);
                bslma::TestAllocator saY("suppliedY", veryVeryVeryVerbose);

                Mech mZ(needleFirst, needleLast, hash, equal, &saX);
                Mech mX(needleFirst, needleLast, hash, equal, &saX);

                const Mech& X = mX;
                const Mech& Z = mZ;

                Mech mY(X, &saY); const Mech& Y = mY;  // ACTION

                // The copied object state matches 'original' except for the
                // allocator.

                ASSERT(X.needleFirst() == Y.needleFirst());
                ASSERT(X.needleLast()  == Y.needleLast());
                ASSERT(X.hash()        == Y.hash());
                ASSERT(X.equal()       == Y.equal());
                ASSERT(X.allocator()   != Y.allocator());
                ASSERT(&saY            == Y.allocator());

                // The 'original' object state unchanged since construction.

                ASSERT(Z.needleFirst() == X.needleFirst());
                ASSERT(Z.needleLast()  == X.needleLast());
                ASSERT(Z.hash()        == X.hash());
                ASSERT(Z.equal()       == X.equal());
                ASSERT(Z.allocator()   == X.allocator());
            }
        }

        if (veryVerbose) printf("Specialized Implementation\n");
        {
            typedef bsl::hash<char>     HASH;   // These types lack equality
            typedef bsl::equal_to<char> EQUAL;  // comparison operators.

            HASH   hash;
            EQUAL equal;

            CharArray<char> containerHavingRandomIterators(
                                                    bsl::vector<char>('b', 5));

            typedef CharArray<char>::const_iterator           RandConstItr;
            typedef bslstl::BoyerMooreHorspoolSearcher<RandConstItr,
                                                       HASH,
                                                       EQUAL> Mech;

            RandConstItr needleFirst = containerHavingRandomIterators.begin();
            RandConstItr needleLast  = containerHavingRandomIterators.end();

            if (veryVerbose) printf("default allocator\n");
            {
                bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);
                bslma::TestAllocator da("default",  veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                Mech mZ(needleFirst, needleLast, hash, equal, &sa);
                Mech mX(needleFirst, needleLast, hash, equal, &sa);

                const Mech& X = mX;
                const Mech& Z = mZ;

                Mech mY(X); const Mech& Y = mY;  // ACTION

                // The copied object state matches 'original' except for the
                // allocator.

                ASSERT(X.needleFirst() == Y.needleFirst());
                ASSERT(X.needleLast()  == Y.needleLast());
             // ASSERT(X.hash()        == Y.hash());
             // ASSERT(X.equal()       == Y.equal());
                ASSERT(X.allocator()   != Y.allocator());
                ASSERT(&da             == Y.allocator());

                // The 'original' object state unchanged since construction.

                ASSERT(Z.needleFirst() == X.needleFirst());
                ASSERT(Z.needleLast()  == X.needleLast());
             // ASSERT(Z.hash()        == X.hash());
             // ASSERT(Z.equal()       == X.equal());
                ASSERT(Z.allocator()   == X.allocator());
            }

            if (veryVerbose) printf("supplied allocator\n");
            {
                bslma::TestAllocator saX("suppliedX", veryVeryVeryVerbose);
                bslma::TestAllocator saY("suppliedY", veryVeryVeryVerbose);

                Mech mZ(needleFirst, needleLast, hash, equal, &saX);
                Mech mX(needleFirst, needleLast, hash, equal, &saX);

                const Mech& X = mX;
                const Mech& Z = mZ;

                Mech mY(X, &saY); const Mech& Y = mY;  // ACTION

                // The copied object state matches 'original' except for the
                // allocator.

                ASSERT(X.needleFirst() == Y.needleFirst());
                ASSERT(X.needleLast()  == Y.needleLast());
             // ASSERT(X.hash()        == Y.hash());
             // ASSERT(X.equal()       == Y.equal());
                ASSERT(X.allocator()   != Y.allocator());
                ASSERT(&saY            == Y.allocator());

                // The 'original' object state unchanged since construction.

                ASSERT(Z.needleFirst() == X.needleFirst());
                ASSERT(Z.needleLast()  == X.needleLast());
             // ASSERT(Z.hash()        == X.hash());
             // ASSERT(Z.equal()       == X.equal());
                ASSERT(Z.allocator()   == X.allocator());
            }
        }

        if (veryVerbose) printf("Compare results\n");
        {
            for (std::size_t ti = 0; ti < numDATA; ++ti) {
                const int         LINE      = DATA[ti].d_line;
                const char *const HAYSTACK  = DATA[ti].d_haystack_p;
                const char *const NEEDLE    = DATA[ti].d_needle_p;
                const int         OFFSET_CS = DATA[ti].d_offsetCs;
                const int         LENGTH_CS = DATA[ti].d_lengthCs;
                const int         OFFSET_CI = DATA[ti].d_offsetCi;
                const int         LENGTH_CI = DATA[ti].d_lengthCi;

                if (veryVerbose) {
                    P_(LINE      )
                    P_(HAYSTACK  )
                    P_(NEEDLE    )
                    P_(OFFSET_CS )
                    P_(LENGTH_CS )
                    P_(OFFSET_CI )
                    P (LENGTH_CI )
                }

                if (veryVerbose) printf("General Implementation\n");
                {
                    bsl::vector<char> vectorOfChars;
                    loadVectorOfChars(&vectorOfChars, NEEDLE);

                    CharArray<char> needleContent(vectorOfChars);

                    typedef CharArray<char>::const_iterator RandConstItr;
                    typedef  CharHashCaseInsensitive        HASH;
                    typedef CharEqualCaseInsensitive        EQUAL;

                    typedef bslstl::BoyerMooreHorspoolSearcher<RandConstItr,
                                                               HASH,
                                                               EQUAL> Mech;

                    RandConstItr needleFirst = needleContent.begin();
                    RandConstItr needleLast  = needleContent.end();

                    Mech mZ(needleFirst, needleLast);
                    Mech mX(needleFirst, needleLast);

                    const Mech& Z = mZ;
                    const Mech& X = mX;

                    Mech mY(X); const Mech& Y = mY;  // ACTION

                    typedef bsl::pair<const char *, const char *> Result;

                    const std::size_t HAYSTACK_LENGTH = std::strlen(HAYSTACK);

                    Result resultZ = Z(HAYSTACK, HAYSTACK + HAYSTACK_LENGTH);
                    Result resultX = X(HAYSTACK, HAYSTACK + HAYSTACK_LENGTH);
                    Result resultY = Y(HAYSTACK, HAYSTACK + HAYSTACK_LENGTH);

                    ASSERT(resultZ == resultX);
                    ASSERT(resultX == resultY);
                }

                if (veryVerbose) printf("Specialized Implementation\n");
                {
                    typedef bslstl::BoyerMooreHorspoolSearcher<const char *>
                                                                         Mech;

                    Mech mZ(NEEDLE, NEEDLE + std::strlen(NEEDLE));
                    Mech mX(NEEDLE, NEEDLE + std::strlen(NEEDLE));

                    const Mech& Z = mZ;
                    const Mech& X = mX;

                    Mech mY(X); const Mech& Y = mY;  // ACTION

                    typedef bsl::pair<const char *, const char *> Result;

                    const std::size_t HAYSTACK_LENGTH = std::strlen(HAYSTACK);

                    Result resultZ = Z(HAYSTACK, HAYSTACK + HAYSTACK_LENGTH);
                    Result resultX = X(HAYSTACK, HAYSTACK + HAYSTACK_LENGTH);
                    Result resultY = Y(HAYSTACK, HAYSTACK + HAYSTACK_LENGTH);

                    ASSERT(resultZ == resultX);
                    ASSERT(resultX == resultY);
                }
            }
        }

        if (verbose) printf("\n" "Special Implementation Exception Test" "\n");
        {
            CharArray<char> containerHavingRandomIterators(
                                                    bsl::vector<char>('b', 5));

            typedef CharArray<char>::const_iterator    RandConstItr;
            typedef bslstl::BoyerMooreHorspoolSearcher<RandConstItr> Mech;

            const std::size_t SHRT = (UCHAR_MAX + 1) * sizeof (unsigned char);
            const std::size_t TALL = (UCHAR_MAX + 1) * sizeof (
                          bsl::iterator_traits<RandConstItr>::difference_type);

            bslma::TestAllocator         sa("supplied", veryVeryVeryVerbose);
            bslma::TestAllocator         da("default",  veryVeryVeryVerbose);
            bslma::DefaultAllocatorGuard dag(&da);
            bslma::TestAllocatorMonitor  dam(&da);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(da) {
                if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody: default) }

                Mech mechEmpty(containerHavingRandomIterators.begin(),
                               containerHavingRandomIterators.begin(), // empty
                               Mech::DefaultHash(),
                               Mech::DefaultEqual(),
                               &sa);

                Mech mechEmptyCopy(mechEmpty);   // ACTION
                ASSERT(0 == da.numBlocksInUse());
                ASSERT(0 == da.numBytesInUse());

                Mech mechShort(containerHavingRandomIterators.begin(),
                               containerHavingRandomIterators.end(),
                               Mech::DefaultHash(),
                               Mech::DefaultEqual(),
                               &sa);

                Int64 numBlocksInUseAfore = da.numBlocksInUse();
                Int64 numBytesInUseAfore  = da.numBytesInUse();

                Mech mechShortCopy(mechShort);   // ACTION

                Int64 numBlocksInUseAfter = da.numBlocksInUse();
                Int64 numBytesInUseAfter  = da.numBytesInUse();

                Int64 numBlocksInUseChange = numBlocksInUseAfter
                                           - numBlocksInUseAfore;
                Int64 numBytesInUseChange  = numBytesInUseAfter
                                           - numBytesInUseAfore;

                ASSERT(   1 == numBlocksInUseChange);
                ASSERT(SHRT == numBytesInUseChange);

                Mech mechLong(HAYSTACK_TEXT_FIRST,
                              HAYSTACK_TEXT_LAST,
                              Mech::DefaultHash(),
                              Mech::DefaultEqual(),
                              &sa);

                numBlocksInUseAfore = da.numBlocksInUse();
                numBytesInUseAfore  = da.numBytesInUse();

                Mech mechLongCopy(mechLong);   // ACTION

                numBlocksInUseAfter = da.numBlocksInUse();
                numBytesInUseAfter  = da.numBytesInUse();

                numBlocksInUseChange = numBlocksInUseAfter
                                     - numBlocksInUseAfore;
                numBytesInUseChange  = numBytesInUseAfter
                                     - numBytesInUseAfore;

                ASSERT(   1 == numBlocksInUseChange);
                ASSERT(TALL == numBytesInUseChange);

            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody: supplied) }

                Mech mechEmpty(containerHavingRandomIterators.begin(),
                               containerHavingRandomIterators.begin(), // empty
                               Mech::DefaultHash(),
                               Mech::DefaultEqual(),
                               &sa);

                Mech mechEmptyCopy(mechEmpty, &sa);   // ACTION
                ASSERT(0 == sa.numBlocksInUse());
                ASSERT(0 == sa.numBytesInUse());

                Mech mechShort(containerHavingRandomIterators.begin(),
                               containerHavingRandomIterators.end(),
                               Mech::DefaultHash(),
                               Mech::DefaultEqual(),
                               &sa);

                Int64 numBlocksInUseAfore = sa.numBlocksInUse();
                Int64 numBytesInUseAfore  = sa.numBytesInUse();

                Mech mechShortCopy(mechShort, &sa);   // ACTION

                Int64 numBlocksInUseAfter = sa.numBlocksInUse();
                Int64 numBytesInUseAfter  = sa.numBytesInUse();

                Int64 numBlocksInUseChange = numBlocksInUseAfter
                                           - numBlocksInUseAfore;
                Int64 numBytesInUseChange  = numBytesInUseAfter
                                           - numBytesInUseAfore;

                ASSERT(   1 == numBlocksInUseChange);
                ASSERT(SHRT == numBytesInUseChange);

                Mech mechLong(HAYSTACK_TEXT_FIRST,
                              HAYSTACK_TEXT_LAST,
                              Mech::DefaultHash(),
                              Mech::DefaultEqual(),
                              &sa);

                numBlocksInUseAfore = sa.numBlocksInUse();
                numBytesInUseAfore  = sa.numBytesInUse();

                Mech mechLongCopy(mechLong, &sa);   // ACTION

                numBlocksInUseAfter = sa.numBlocksInUse();
                numBytesInUseAfter  = sa.numBytesInUse();

                numBlocksInUseChange = numBlocksInUseAfter
                                     - numBlocksInUseAfore;
                numBytesInUseChange  = numBytesInUseAfter
                                     - numBytesInUseAfore;

                ASSERT(   1 == numBlocksInUseChange);
                ASSERT(TALL == numBytesInUseChange);

            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERT(0 == sa.numBlocksInUse());
            ASSERT(0 == sa.numBytesInUse());
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TEST 'operator()'
        //
        // Concerns:
        //: 1 The method has the expected signature.  In particular,
        //:   1 the method is 'const' qualified.
        //:   2 the method returns the expected type.
        //:
        //: 2 The haystack iterator type may be just a constant iterator.
        //:
        //: 3 The haystack iterator type can be just a forward iterator.
        //
        //: 4 The method uses the equality comparison functor supplied on
        //:   construction.
        //:
        //: 5 The method searches the specified range (and no further).
        //:
        //: 6 The method returns the position of the first occurrence of needle
        //:   in the specified haystack.
        //:
        //: 7 If the needle is not found in the haystack, a zero-length
        //:   range positioned at the end of the haystack is returned.
        //:
        //: 8 The returns the expected results for each combination of
        //:   needle and haystack in these special cases:
        //:   o length 0, no elements
        //:   o length 1, one element that is both first and last
        //:   o length 2, separate first and last elements
        //:   o length 3, separate first, last elements, and one
        //:     undistinguished (middle) element.
        //:
        //: 9 The method does not allocate from the object memory allocator
        //:    or the default allocator.
        //:
        //:10 QoI: Precondition violations are detected in appropriate build
        //:   modes.
        //
        // Plan:
        //: 1 Define a table of test cases that cover a spectrum of concerns:
        //:   o Needles and haystacks of varying combinations of lengths from
        //:     empty to at least three, including edge cases on
        //:     needle/haystack lengths.
        //:   o Haystacks that do not contain the needle.
        //:   o Haystacks that contain the needle at the beginning, middle, and
        //:     end.
        //:   o Haystacks that contain multiple instances of the needle.
        //:   o Haystacks that have partial needles (false starts) before the
        //:     actual needle.
        //:
        //: 2 Each entry in the table has pre-calculated expected results, both
        //:   for case sensitive searches (the default) and case insensitive
        //:   searches.  Both searches are done to show that the equality
        //:   comparison functor that is shown is actually used.
        //:
        //: 3 A pair of searcher objects are created:
        //:   o Comparator type: case-sensitive (default) and case-insensitive.
        //:
        //: 4 Each of the two searcher objects are invoked.
        //:
        //: 5 All defined iterators are constant.
        //:
        //: 6 All searches are done using 'const'-qualified searcher objects.
        //
        // Testing:
        //   bsl::pair<RAI, RAI> operator()(RAI hsFirst, RAI hsLast) const;
        // --------------------------------------------------------------------

        if (verbose) printf("\n" "TEST 'operator()'"
                            "\n" "=================\n");

        for (std::size_t ti = 0; ti < numDATA; ++ti) {
            const int         LINE      = DATA[ti].d_line;
            const char *const HAYSTACK  = DATA[ti].d_haystack_p;
            const char *const NEEDLE    = DATA[ti].d_needle_p;
            const int         OFFSET_CS = DATA[ti].d_offsetCs;
            const int         LENGTH_CS = DATA[ti].d_lengthCs;
            const int         OFFSET_CI = DATA[ti].d_offsetCi;
            const int         LENGTH_CI = DATA[ti].d_lengthCi;

            if (veryVerbose) {
                P_(LINE      )
                P_(HAYSTACK  )
                P_(NEEDLE    )
                P_(OFFSET_CS )
                P_(LENGTH_CS )
                P_(OFFSET_CI )
                P (LENGTH_CI )
            }

            // Searcher type-space:  CaseSensitive and Case Insensitive

            typedef CharArray<char>::const_iterator     RndConstItr;
            typedef bsl::pair<RndConstItr, RndConstItr> RndResult;
            typedef std::ptrdiff_t                      RndDiff;

            typedef bslstl::BoyerMooreHorspoolSearcher<RndConstItr>  RndMechCs;
            typedef bslstl::BoyerMooreHorspoolSearcher<
                                                      RndConstItr,
                                                      CharHashCaseInsensitive,
                                                      CharEqualCaseInsensitive>
                                                                     RndMechCi;
            // Construct needle having Random Access Iterators.

            bsl::vector<char> needleChars;
            for (const char *itr = NEEDLE; 0 != *itr; ++itr) {
                needleChars.push_back(*itr);
            }
            const CharArray<char> needleRnd(needleChars);

            ASSERT(static_cast<RndDiff>(std::strlen(NEEDLE))
                == bsl::distance(needleRnd.begin(), needleRnd.end()));

            // Construct haystacks having Random Access Iterators.

            bsl::vector<char> haystackChars;
            for (const char *itr = HAYSTACK; 0 != *itr; ++itr) {
                haystackChars.push_back(*itr);
            }
            const CharArray<char> haystackRnd(haystackChars);

            ASSERT(static_cast<RndDiff>(std::strlen(HAYSTACK))
                == bsl::distance(haystackRnd.begin(), haystackRnd.end()));

            bslma::TestAllocator         da("default", veryVeryVeryVerbose);
            bslma::DefaultAllocatorGuard dag(&da);
            bslma::TestAllocatorMonitor  dam(&da);

            // Create the set of objects to be tested.
            const RndMechCs rndMechCs(needleRnd.begin(), needleRnd.end());

            if (veryVeryVerbose) {
                P_(da.numAllocations())
                P_(da.numBlocksInUse())
                P_(da.numBytesInUse())
            }

            if (0 == std::strlen(NEEDLE)) {
                ASSERT(dam.isTotalSame());
            } else {
                ASSERT(dam.isTotalUp());
            }
            dam.reset();

            const RndMechCi rndMechCi(needleRnd.begin(), needleRnd.end());

            if (veryVeryVerbose) {
                P_(da.numAllocations())
                P_(da.numBlocksInUse())
                P_(da.numBytesInUse())
            }

            if (1 < std::strlen(NEEDLE)) {
                ASSERT(dam.isTotalUp());
            } else {
                ASSERT(dam.isTotalSame());
            }
            dam.reset();

            // Invoke each using both kinds of haystack
  //--------^
  RndResult rndResultRndCs = rndMechCs(haystackRnd.begin(), haystackRnd.end());
  RndResult rndResultRndCi = rndMechCi(haystackRnd.begin(), haystackRnd.end());
  //--------v

            ASSERT(dam.isTotalSame());

            // Check each result against expected offset value.
 //---------^
 ASSERT(OFFSET_CS == bsl::distance(haystackRnd.begin(), rndResultRndCs.first));
 ASSERT(OFFSET_CI == bsl::distance(haystackRnd.begin(), rndResultRndCi.first));
 //---------v

            // Check each result against expected length value.
            ASSERT(LENGTH_CS == bsl::distance(rndResultRndCs.first,
                                              rndResultRndCs.second));
            ASSERT(LENGTH_CI == bsl::distance(rndResultRndCi.first,
                                              rndResultRndCi.second));
        }

        if (verbose) printf("\n" "Test Extremes of 'char' Values" "\n");
        {

            bslma::TestAllocator         da("default", veryVeryVeryVerbose);
            bslma::DefaultAllocatorGuard dag(&da);
            bslma::TestAllocatorMonitor  dam(&da);

            // Test helper function, 'convertToNonAscii'
            std::string testString("0101");
            std::string testStringNonAscii;
            convertToNonAscii(&testStringNonAscii, testString);

            ASSERT(testString.size() == testStringNonAscii.size());
            ASSERT(0         == testStringNonAscii.data()[0]);
            ASSERT(UCHAR_MAX == static_cast<unsigned char>(
                                testStringNonAscii.data()[1]));

            ASSERT(0         == testStringNonAscii.data()[2]);
            ASSERT(UCHAR_MAX == static_cast<unsigned char>(
                                testStringNonAscii.data()[3]));

            std::string haystackAsString(HAYSTACK_BINARY_FIRST,
                                         HAYSTACK_BINARY_LAST);
                // Actually, sequence 'char' values that are either '0' or '1'.

            std::string haystackNonAscii;
            convertToNonAscii(&haystackNonAscii, haystackAsString);

            for (std::size_t i = 0; i < NUM_U_DATA_BINARY; ++i) {
                const int          LINE     = U_DATA_BINARY[i].d_line;
                const bool         EXPECTED = U_DATA_BINARY[i].d_expected;
                const char * const NEEDLE   = U_DATA_BINARY[i].d_needle_p;

                if (veryVerbose) {
                    P_(LINE)
                    P_(EXPECTED)
                    P(NEEDLE)
                }

                std::string needleNonAscii;
                convertToNonAscii(&needleNonAscii, std::string(NEEDLE));

                const bslstl::BoyerMooreHorspoolSearcher<const char *>
                                               searcher(needleNonAscii.data(),
                                                        needleNonAscii.data()
                                                      + needleNonAscii.size());

                const bsl::pair<const char *, const char *> result =
                                             searcher(haystackNonAscii.data(),
                                                      haystackNonAscii.data()
                                                    + haystackNonAscii.size());

                const bsl::pair<const char *, const char *> NOT_FOUND(
                                                      haystackNonAscii.data()
                                                    + haystackNonAscii.size(),
                                                      haystackNonAscii.data()
                                                    + haystackNonAscii.size());

                bool wasFound = NOT_FOUND != result;

                ASSERTV(LINE, EXPECTED == wasFound);
            }
        }

        if (verbose) printf("\n" "Negative Tests" "\n");
        {
            typedef CharArray<char>::const_iterator RndConstItr;

            // Create arbitrary "needle".
            const CharArray< char> needle  (bsl::vector<char>('a', 3));

            typedef bslstl::BoyerMooreHorspoolSearcher<RndConstItr> Mech;

            Mech mX(needle.begin(), needle.end()); const Mech& X = mX;

            // Create arbitrary "haystack".

            const CharArray<char> haystack(bsl::vector<char>('b', 5));

            RndConstItr middle = haystack.begin() + 2;

            bsls::AssertTestHandlerGuard hG;

            ASSERT_FAIL(X(middle, middle - 1));
            ASSERT_PASS(X(middle, middle + 0));
            ASSERT_PASS(X(middle, middle + 1));
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // CONSTRUCTOR AND BASIC ACCESSORS
        //
        // Concerns:
        //: 1 The constructor creates an object.
        //:
        //: 2 The basic accessors show object attributes in agreement with the
        //:   object's constructor arguments.
        //:
        //: 3 The optional parameters, 'HASH' and 'EQUAL', default to the
        //:   expected values, 'bsl::hash<TYPE>' and 'bsl::equal_to<TYPE>',
        //:   respectively
        //:
        //: 4 The needle iterator type may be just a constant iterator.
        //:
        //: 5 The needle iterator type can be just a random access iterator.
        //:
        //: 6 The basic accessors are 'const'-qualified.
        //:
        //: 7 The object holds copies of constructor arguments.
        //:
        //: 8 The accessor to the equality comparison functor returns a copy of
        //:   the object's functor.
        //:
        //: 9 The object's public type, 'value_type', is the expected type.
        //:
        //:10 Memory allocation, if any, uses the intended allocator.
        //:
        //:11 There is no temporary memory allocation.
        //:
        //:12 All memory allocation by the special implementation (for
        //:   'char *') is exception safe.
        //:
        //:13 QoI: Precondition violations are detected in appropriate build
        //:   modes.
        //
        // Plan:
        //: 1 Accessor 'const'-qualification is demonstrated by invoking each
        //:   accessor via a 'const'-reference to an object.
        //:
        //: 2 Acceptance of iterator types is demonstrated by creating objects
        //:   using a constant iterator type derived from
        //:   'bslstl::RandomAccessIterator'.
        //:
        //: 3 Show that the object type is the same, irrespective of whether or
        //:   not the optional arguments are supplied.
        //:
        //: 4 We demonstrate that the object holds copies of constructor
        //:   arguments by changing those objects and confirming that they are
        //:   no longer equal to the values returned by the accessors.
        //:
        //: 5 Use the "footprint" idiom to demonstrate that the intended
        //:   allocator is installed and is returned by the 'allocator' method.
        //:
        //: 6 Exception safety of memory allocations is tested using the
        //:   'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros.
        //
        // Testing:
        //   BoyerMooreHorspoolSearcher(RAI f, RAI l, HASH l, EQUAL e, *bA);
        //   RAI needleFirst() const;
        //   RAI needleLast() const;
        //   HASH hash() const;
        //   EQUAL equal() const;
        //   BloombergLP::bslma::Allocator *allocator() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\n" "CONSTRUCTOR AND BASIC ACCESSORS"
                            "\n" "===============================\n");

        if (verbose) printf("\n" "Default Functors" "\n");
        {
            CharArray<char> containerHavingRandomIterators(
                                                    bsl::vector<char>('b', 5));

            typedef CharArray<char>::const_iterator    RandConstItr;
            typedef bslstl::BoyerMooreHorspoolSearcher<RandConstItr> Mech;

            RandConstItr needleFirst = containerHavingRandomIterators.begin();
            RandConstItr needleLast  = containerHavingRandomIterators.end();

            // Create a object without providing any optional parameters.

            Mech mX(needleFirst, needleLast); const Mech& X = mX;

            ASSERT(needleFirst == X.needleFirst());
            ASSERT(needleLast  == X.needleLast());

            const bsl::hash<char>&     hash  = X.hash();   (void)hash;
            const bsl::equal_to<char>& equal = X.equal();  (void)equal;

            ASSERT((bsl::is_same<Mech,
                                 bslstl::BoyerMooreHorspoolSearcher<
                                                     RandConstItr,
                                                     bsl::hash<char>,
                                                     bsl::equal_to<char> >
                                >() ));

            ASSERT((bsl::is_same<char, Mech::value_type>()));
        }

        if (verbose) printf("\n" "Non-Default Hash/Equality Functors" "\n");
        {

            CharArray<char> containerHavingRandomIterators(
                                                    bsl::vector<char>('b', 5));

            bslma::TestAllocator         sa("supplied", veryVeryVeryVerbose);
            bslma::TestAllocator         da("default" , veryVeryVeryVerbose);
            bslma::DefaultAllocatorGuard dag(&da);
            bslma::TestAllocatorMonitor  sam(&sa);
            bslma::TestAllocatorMonitor  dam(&da);

            CharEqualCaseInsensitive equalFunctor(42);
            CharHashCaseInsensitive   hashFunctor(43);

            typedef CharArray<char>::const_iterator RandConstItr;
            typedef bslstl::BoyerMooreHorspoolSearcher<
                                                RandConstItr,
                                                CharHashCaseInsensitive,
                                                CharEqualCaseInsensitive> Mech;

            RandConstItr needleFirst = containerHavingRandomIterators.begin();
            RandConstItr needleLast  = containerHavingRandomIterators.end();

            Mech mX(needleFirst, needleLast, hashFunctor, equalFunctor, &sa);
            const Mech& X = mX;

            ASSERT(sam.isTotalUp());
            ASSERT(dam.isTotalSame());

            // Show that the object holds copies.

            ASSERT(needleFirst             == X.needleFirst());
            ASSERT(needleLast              == X.needleLast());

            ASSERT( hashFunctor.id()       == X.hash().id());
            ASSERT(equalFunctor.id()       == X.equal().id());
            ASSERT(0                       ==  hashFunctor.useCount());
            ASSERT(0                       == equalFunctor.useCount());
            ASSERT(0                       <     X. hash().useCount()); // CTOR
            ASSERT(0                       <     X.equal().useCount()); // CTOR

            ++needleFirst;

            ASSERT(needleFirst             != X.needleFirst());
            ASSERT(needleLast              == X.needleLast());

            ASSERT(0 < bsl::distance(needleFirst, needleLast));
            --needleLast;

            ASSERT(needleFirst             != X.needleFirst());
            ASSERT(needleLast              != X.needleLast());
        }

        if (verbose) printf("\n" "Allocator Installation and Accessor" "\n");
        {
            CharArray<char> containerHavingRandomIterators(
                                                    bsl::vector<char>('b', 5));

            CharEqualCaseInsensitive equalFunctor(42);
            CharHashCaseInsensitive   hashFunctor(43);

            typedef CharArray<char>::const_iterator RandConstItr;
            typedef bslstl::BoyerMooreHorspoolSearcher<
                                                RandConstItr,
                                                CharHashCaseInsensitive,
                                                CharEqualCaseInsensitive> Mech;

            RandConstItr needleFirst = containerHavingRandomIterators.begin();
            RandConstItr needleLast  = containerHavingRandomIterators.end();

            for (char cfg = 'a'; cfg <= 'c'; ++cfg) {
                const char CONFIG = cfg;  // how we specify the allocator

                if (veryVerbose) {
                    P(CONFIG)
                }

                bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);
                bslma::TestAllocator da("default",   veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                Mech                 *mechPtr          = 0;
                bslma::TestAllocator *mechAllocatorPtr = 0;

                switch (CONFIG) {
                  case 'a': {
                    mechPtr          = new (fa) Mech(needleFirst,
                                                     needleLast,
                                                     hashFunctor,
                                                     equalFunctor);
                    mechAllocatorPtr = &da;
                  } break;
                  case 'b': {
                    mechPtr          = new (fa) Mech(needleFirst,
                                                     needleLast,
                                                     hashFunctor,
                                                     equalFunctor,
                                                     0);
                    mechAllocatorPtr = &da;
                  } break;
                  case 'c': {
                    mechPtr          = new (fa) Mech(needleFirst,
                                                     needleLast,
                                                     hashFunctor,
                                                     equalFunctor,
                                                     &sa);
                    mechAllocatorPtr = &sa;
                  } break;
                  default: {
                    ASSERTV(CONFIG, !"Bad allocator config.");
                  } break;
                }

                Mech&                  mX = *mechPtr; const Mech& X = mX;
                bslma::TestAllocator&  oa = *mechAllocatorPtr;
                bslma::TestAllocator& noa = 'c' != CONFIG ? sa : da;

                bslma::Allocator             *result   = X.allocator();
                const bslma::TestAllocator *expected = &oa;

                ASSERT(expected == result);
                ASSERT(0   <   oa.numBlocksTotal());
                ASSERT(0   == noa.numBlocksTotal());

                fa.deleteObject(mechPtr);
            }
        }

        if (verbose) printf("\n" "Special Implementation Exception Test" "\n");
        {
            CharArray<char> containerHavingRandomIterators(
                                                    bsl::vector<char>('b', 5));

            typedef CharArray<char>::const_iterator    RandConstItr;
            typedef bslstl::BoyerMooreHorspoolSearcher<RandConstItr> Mech;

            const int SHRT = (UCHAR_MAX + 1) * sizeof (unsigned char);
            const int TALL = (UCHAR_MAX + 1) * sizeof (
                          bsl::iterator_traits<RandConstItr>::difference_type);

            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            int loopCount = 0;
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }
                ++loopCount;

                Mech mechEmpty(containerHavingRandomIterators.begin(),
                               containerHavingRandomIterators.begin(), // empty
                               Mech::DefaultHash(),
                               Mech::DefaultEqual(),
                               &sa);
                ASSERT(0 == bsl::distance(mechEmpty.needleFirst(),
                                          mechEmpty.needleLast()));
                ASSERT(0 == sa.numBlocksInUse());
                ASSERT(0 == sa.numBytesInUse());

                Mech mechShort(containerHavingRandomIterators.begin(),
                               containerHavingRandomIterators.end(),
                               Mech::DefaultHash(),
                               Mech::DefaultEqual(),
                               &sa);

                ASSERT(   0 <  bsl::distance(mechShort.needleFirst(),
                                             mechShort.needleLast()));
                ASSERT(SHRT >  bsl::distance(mechShort.needleFirst(),
                                             mechShort.needleLast()));
                ASSERT(   1 == sa.numBlocksInUse());
                ASSERT(SHRT == sa.numBytesInUse());
#ifdef BDE_BUILD_TARGET_EXC
                ASSERT(   2 <= loopCount);
#endif

                Mech mechLong(HAYSTACK_TEXT_FIRST,
                              HAYSTACK_TEXT_LAST,
                              Mech::DefaultHash(),
                              Mech::DefaultEqual(),
                              &sa);
                ASSERT(SHRT <= bsl::distance(mechLong.needleFirst(),
                                             mechLong.needleLast()));
                ASSERT(   1 +    1 == sa.numBlocksInUse());
                ASSERT(TALL + SHRT == sa.numBytesInUse());
#ifdef BDE_BUILD_TARGET_EXC
                ASSERT(          3 == loopCount);
#endif
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERT(0 == sa.numBlocksInUse());
            ASSERT(0 == sa.numBytesInUse());
        }

        if (verbose) printf("\n" "Negative Tests" "\n");
        {

            // Arbitrary needle.
            CharArray<char> containerHavingRandomIterators(
                                                    bsl::vector<char>('b', 5));

            typedef CharArray<char>::const_iterator    RandConstItr;
            typedef bslstl::BoyerMooreHorspoolSearcher<RandConstItr> Mech;

            RandConstItr middle = containerHavingRandomIterators.begin() + 2;

            bsls::AssertTestHandlerGuard hG;

            ASSERT_FAIL(Mech(middle, middle - 1));
            ASSERT_PASS(Mech(middle, middle + 0));
            ASSERT_PASS(Mech(middle, middle + 1));
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Create a searcher object to search for the ", " sequence of the
        //:   literal string, "Hello, world!".  Confirm that the result, a
        //:   pair, matches the pointers used to construct the searcher.
        //
        //: 2 Create a small array of non-zero 'int' values, a needle, and
        //:   copy that sequence amidst a longer vector of 'int' zero values,
        //:   the haystack.  Create a searcher object and confirm that the
        //:   result matches in position and length the place where the needle
        //:   was injected into the haystack.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\n" "BREATHING TEST"
                            "\n" "==============\n");

        if (veryVerbose) printf("Hello, world!\n");
        {
            const char *haystack = "Hello, world!";

            bslstl::BoyerMooreHorspoolSearcher<const char *> mySearcher(
                                                                 haystack + 5,
                                                                 haystack + 7);

            bsl::pair<const char *, const char *> result =
                                             mySearcher(haystack,
                                                        haystack
                                                      + std::strlen(haystack));

            if (verbose) {
                P(result.first);
                P(result.second);
            }

            ASSERT(haystack + 5 == result.first);
            ASSERT(haystack + 7 == result.second);
        }

        if (veryVerbose) printf ("Vectors of 'int' values\n");
        {
            const int            intNeedle[] = { 2, 4, 6, 8 };
            const std::size_t numIntNeedle   = sizeof  intNeedle
                                             / sizeof *intNeedle;

            bsl::vector<int> intHaystack;
            intHaystack.push_back(0);
            intHaystack.push_back(0);
            intHaystack.push_back(0);
            for (std::size_t i = 0; i < numIntNeedle; ++i) {
                intHaystack.push_back(intNeedle[i]);
            }
            intHaystack.push_back(0);
            intHaystack.push_back(0);

            bslstl::BoyerMooreHorspoolSearcher<const int *>
                                                   myIntSearcher(intNeedle,
                                                                 intNeedle
                                                               + numIntNeedle);
            bsl::pair<bsl::vector<int>::const_iterator,
                      bsl::vector<int>::const_iterator> resultIntSearcher =
                                            myIntSearcher(intHaystack.cbegin(),
                                                          intHaystack.cend());
            if (verbose) {
                    P(bsl::distance(resultIntSearcher.first,
                                    intHaystack.cbegin()));
                    P(bsl::distance(resultIntSearcher.second,
                                    resultIntSearcher.first));
            }

            ASSERT(intHaystack.data()      + 3    == resultIntSearcher.first);
            ASSERT(resultIntSearcher.first + numIntNeedle
                                                  == resultIntSearcher.second);
        }

      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      } break;
      case -1: {
        // --------------------------------------------------------------------
        // PERFORMANCE: 'operator()'
        // --------------------------------------------------------------------

        if (verbose) printf("\n" "PERFORMANCE: 'operator()'" "\n"
                                 "=========================" "\n");

        if (argc < 4) {
            fprintf(stderr, "1 %s\n", performanceSyntaxMessage(test));
            return -1;                                                // RETURN
        }

        int                numRepetitions = std::atoi(argv[2]);
        const char * const haystackOption =           argv[3];

        bool isPlusHaystackOption = std::strchr(haystackOption, '+');

        const char *haystackFirst;
        const char *haystackLast;

        int rc  = getHaystack(&haystackFirst, &haystackLast, haystackOption);
        if (0 != rc) {
            P(haystackOption)
            fprintf(stderr, "2 %s\n", performanceSyntaxMessage(test));
            return -1;                                                // RETURN
        }

        const DATA_t *DATA;
        std::size_t   NUM_DATA;

        rc = getDataForHaystack(&DATA, &NUM_DATA, haystackOption);
        if (0 != rc) {
            P(haystackOption)
            fprintf(stderr, "3 %s\n", performanceSyntaxMessage(test));
            return -1;                                                // RETURN
        }

        const std::string haystackAsString(haystackFirst, haystackLast);

        for (std::size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE     = DATA[ti].d_line;
            const bool        EXPECTED = DATA[ti].d_expected;
            const char *const NEEDLE   = DATA[ti].d_needle_p;

            if (veryVerbose) {
                if (isPlusHaystackOption) {
                    P_(ti) P_(LINE) P_(EXPECTED)
                } else {
                    P_(ti) P_(LINE) P_(EXPECTED) P(NEEDLE)
                }
            }

            const bslstl::BoyerMooreHorspoolSearcher<const char *>
                                        searcher(NEEDLE,
                                                 NEEDLE + std::strlen(NEEDLE));

            if (veryVerbose) {
                const std::size_t searcherFootprint = sizeof searcher;
                P(searcherFootprint)
            }

            const char * const HAYSTACK_FIRST = haystackFirst;
            const char * const HAYSTACK_LAST  = haystackLast;

            const bsl::pair<const char *, const char *> NOT_FOUND(
                                                                HAYSTACK_LAST,
                                                                HAYSTACK_LAST);

            Int64 startUser = bsls::TimeUtil::getProcessUserTimer();
            Int64 startWall = bsls::TimeUtil::getTimer();

            bsl::pair<const char *, const char *> result;

            for (int i = 0; i < numRepetitions; ++i) {

                result = searcher(HAYSTACK_FIRST, HAYSTACK_LAST);  // WORK

            }

            Int64 stopUser = bsls::TimeUtil::getProcessUserTimer();
            Int64 stopWall = bsls::TimeUtil::getTimer();

            if (0 < numRepetitions) {
                const bool wasFound = NOT_FOUND != result;

                ASSERT(wasFound == EXPECTED);

                std::size_t offset = haystackAsString.find(NEEDLE);
                if (std::string::npos == offset) {
                    ASSERT(NOT_FOUND == result);
                } else {
                    ASSERT(static_cast<std::size_t>(result.first
                                                  - HAYSTACK_FIRST) == offset);
                }
            }

            if (veryVerbose) {
                Int64 userTime = stopUser - startUser;
                Int64 wallTime = stopWall - startWall;

                std::size_t needleLength = std::strlen(NEEDLE);
                std::size_t searchLength
                          = NOT_FOUND == result
                          ? HAYSTACK_LAST - HAYSTACK_FIRST
                          : result.first  - HAYSTACK_FIRST + needleLength;
                P_(ti)
                P_(numRepetitions)
                P_(needleLength)
                P_(userTime)
                P_(wallTime)
                P(searchLength)
            }
        }
      } break;

    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2019 Bloomberg Finance L.P.
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
