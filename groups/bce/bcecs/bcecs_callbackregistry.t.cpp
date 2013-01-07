// bcecs_callbackregistry.t.cpp        -*-C++-*-

#include <bcecs_callbackregistry.h>

#include <bsls_platform.h>

#include <bdet_date.h>
#include <bdet_datetime.h>
#include <bdet_timeinterval.h>
#include <bdetu_datetime.h>
#include <bcema_testallocator.h>
#include <bcemt_barrier.h>
#include <bcemt_thread.h>
#include <bces_atomictypes.h>

#include <bdef_function.h>
#include <bdef_placeholder.h>
#include <bdef_bind.h>

#include <bsl_algorithm.h>
#include <bsl_cctype.h>
#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_ctime.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::ends;
using bsl::flush;

using namespace BloombergLP;
using namespace bdef_PlaceHolders;

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
//-----------------------------------------------------------------------------
// 'bcecs_callbackregistry' public interface
// CREATORS
// [ 2] bcecs_CallbackRegistry(bslma_Allocator *basicAllocator = 0);
// [ 2] ~bcecs_CallbackRegistry();
//
// MANIPULATORS
// [ 2] int registerCallback(const CATEGORY&    category,
//                           const CALLBACK_ID& id,
//                           const Callback&    callback);
// [ 6] int deregisterCallback(const CATEGORY&    category,
//                             const CALLBACK_ID& id);
// [ 7] int deleteCategory(const CATEGORY& category);
// [ 2] int notify(const CATEGORY& category, const EVENT& event);
//
// ACCESSORS
// [ 4] int numCategories(const CALLBACK_ID& id) const;
// [ 4] int numCallbacks(const CATEGORY& category) const;
// [ 4] int totalNumCategories() const;
// [ 4] int totalNumCallbacks() const;
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] PRIMARY TEST APPARATUS
// [ 5] OUTPUT (<<) OPERATOR
// [ 8] CONCERN: Instantiate with non-default comparators
// [ 9] USAGE EXAMPLE 1
//-----------------------------------------------------------------------------

//=============================================================================
//                       STANDARD BDE ASSERT TEST MACROS
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100)  ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\n";\
               aSsErT(1, #X, __LINE__); }}

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
                    << #K << ": " << K << "\n";                           \
               aSsErT(1, #X, __LINE__); }}

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
                    << #K << ": " << K << "\t" << #L << ": " << L << "\n";\
               aSsErT(1, #X, __LINE__); }}

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
                    << #K << ": " << K << "\t" << #L << ": " << L << "\t" \
                    << #M << ": " << M << "\n";                           \
               aSsErT(1, #X, __LINE__); }}

//=============================================================================
//                      SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number.
#define T_()  cout << '\t' << flush;          // Print tab w/o newline.

// The following macros facilitate thread-safe streaming to standard output.

#ifdef BSLS_PLATFORM_OS_WINDOWS
#define MTCOUT  coutMutex.lock(); { bsl::cout
#else
#define MTCOUT  coutMutex.lock(); { bsl::cout << bcemt_ThreadUtil::self() \
                                              << ": "
#endif

#define MTENDL  bsl::endl;  } coutMutex.unlock()
#define MTFLUSH bsl::flush; } coutMutex.unlock()

//=============================================================================
//              GLOBAL TYPES, CONSTANTS, AND VARIABLES FOR TESTING
//-----------------------------------------------------------------------------
static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;

static bcemt_Mutex coutMutex;

                           // ======================
                           // struct my_NotifyParams
                           // ======================

template <typename EVENT, typename CATEGORY, typename CALLBACK_ID>
struct my_NotifyParams {
    // This struct defines the parameters necessary to spawn a thread
    // which notifies a 'bcecs_CallbackRegistry' with an event for a
    // specific category.

    bcecs_CallbackRegistry<EVENT, CATEGORY, CALLBACK_ID>
                  *d_registry_p;    // a callback registry
    CATEGORY      *d_category_p;    // notify category
    EVENT         *d_event_p;       // notify event
    const bool    *d_stop_p;        // "stop thread" indicator
    bcemt_Barrier *d_barrier_p;     // synchronize notify threads
};

                             // =================
                             // struct my_Results
                             // =================

struct my_Results {
    // This struct defines a thread-safe container for string-valued
    // test results.

    bsl::string   d_string;    // result data
    bces_SpinLock d_lock;      // synchronize access to result data
};

//=============================================================================
//                      GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------
static
void nopCallback(int event)
{
    // A no-op callback for 'int'-type events.
}

//=============================================================================
//       CASE-SPECIFIC TYPES, HELPER FUNCTIONS, AND CLASSES FOR TESTING
//-----------------------------------------------------------------------------
                                // ============
                                // struct Case8
                                // ============

struct Case8 {
    // This struct encapsulates type definitions for use with test case 1.

    // TYPES
    enum {
        // Arbitrary events, category values, and callback identifiers.

        EVENT_UPPERCASE = 2,
        EVENT_LOWERCASE = 4,
        EVENT_MIXEDCASE = 8
    };
};

                    // ====================================
                    // struct my_CaseSensitiveStringCompare
                    // ====================================

struct my_CaseSensitiveStringCompare :
                    public bsl::binary_function<const char*, const char*, bool>
{
    // This struct defines a binary predicate for a partial ordering on
    // 'const char*' objects representing C-strings using a case-sensitive
    // comparator.

    bool operator()(const char *lhs, const char *rhs) const
    {
        // Return 'true' if 'lhs' is lexicographically less than 'rhs',
        // taking case into consideration.  Otherwise, return 'false'.

        return (0 > bsl::strcmp(lhs, rhs)) ? true : false;
    }
};

                   // ======================================
                   // struct my_CaseInsensitiveStringCompare
                   // ======================================

struct my_CaseInsensitiveStringCompare :
                    public bsl::binary_function<const char*, const char*, bool>
{
    // This struct defines a binary predicate for a partial ordering on
    // 'char*' objects representing C-strings using a case-insensitive
    // comparator.

  private:
    struct NoCaseCompare :
                      public bsl::binary_function<const char, const char, bool>
    {
        // Provide case-insensitive, locale-unaware comparison
        // of 'chars''s.

        bool operator()(const char lhs, const char rhs) const
        {
            return (bsl::tolower((unsigned char)lhs) <
                    bsl::tolower((unsigned char)rhs));
        }
    };

  public:
    bool operator()(const char *lhs, const char *rhs) const
    {
        // Return 'true' if 'lhs' is lexicographically less than 'rhs',
        // irrespective of case.  Otherwise, return 'false'.

        return bsl::lexicographical_compare(lhs, lhs + bsl::strlen(lhs),
                                            rhs, rhs + bsl::strlen(rhs),
                                            NoCaseCompare());
    }
};

//-----------------------------------------------------------------------------
typedef my_NotifyParams<int, bsl::string, bsl::string> Case7NotifyParams;

//extern "C"
void *case7NotifyThread(void *arg)
{
    // Notify the 'bcecs_CallbackRegistry' object specified by
    // the 'd_registry_p' parameter in the parameter list specified by 'arg'
    // of type 'my_NotifyParams' with the event specified by the 'd_event_p'
    // parameter for the category specified by the 'd_category_p' parameter.

    ASSERT(arg);

    Case7NotifyParams *params = static_cast<Case7NotifyParams*>(arg);
    ASSERT(params->d_registry_p);
    ASSERT(params->d_category_p);
    ASSERT(params->d_event_p);

    int rc = params->d_registry_p->notify(*params->d_category_p,
                                          *params->d_event_p);
    if (veryVerbose) {
        MTCOUT << '\t' << "Notify: "
               << "CATEGORY = " << *params->d_category_p << ", "
               << "EVENT = "    << *params->d_event_p    << ", "
               << (0 == rc ? "SUCCEEDED" : "FAILED")
               << MTENDL;
    }
    return 0;
}

static
void case7Cb(const int&      numSeconds,
             bcemt_Barrier  *startBarrier,
             bces_AtomicInt *counter)
{
    ASSERT(0 < numSeconds);
    ASSERT(counter);

    startBarrier->wait();
    if (veryVerbose) {
        MTCOUT << '\t'
               << "Callback: sleeping for " << numSeconds << " seconds."
               << MTENDL;
    }
    bcemt_ThreadUtil::microSleep(0, numSeconds);
    ++*counter;
}

//-----------------------------------------------------------------------------
typedef my_NotifyParams<int, bsl::string, bsl::string> Case6NotifyParams;

//extern "C"
void *case6NotifyThread(void *arg)
{
    // Notify the 'bcecs_CallbackRegistry' object specified by
    // the 'd_registry_p' parameter in the parameter list specified by 'arg'
    // of type 'my_NotifyParams' with the event specified by the 'd_event_p'
    // parameter for the category specified by the 'd_category_p' parameter.

    ASSERT(arg);

    Case6NotifyParams *params = static_cast<Case6NotifyParams*>(arg);
    ASSERT(params->d_registry_p);
    ASSERT(params->d_category_p);
    ASSERT(params->d_event_p);

    int rc = params->d_registry_p->notify(*params->d_category_p,
                                          *params->d_event_p);
    if (veryVerbose) {
        MTCOUT << '\t' << "Notify: "
               << "CATEGORY = " << *params->d_category_p << ", "
               << "EVENT = "    << *params->d_event_p    << ", "
               << (0 == rc ? "SUCCEEDED" : "FAILED")
               << MTENDL;
    }
    return 0;
}

static
void case6Cb(const int&      numSeconds,
             bcemt_Barrier  *startBarrier,
             bces_AtomicInt *counter)
{
    ASSERT(0 < numSeconds);
    ASSERT(counter);

    startBarrier->wait();
    if (veryVerbose) {
        MTCOUT << '\t'
               << "Callback: sleeping for " << numSeconds << " seconds."
               << MTENDL;
    }
    bcemt_ThreadUtil::microSleep(0, numSeconds);
    ++*counter;
}

//-----------------------------------------------------------------------------
                                // ============
                                // struct Case4
                                // ============
struct Case4 {
    // This struct encapsulates type definitions for use with test case 1.

    // TYPES
    enum {
        // Arbitrary events, category values, and callback identifiers.

        EVENT_UPPERCASE = 2,
        EVENT_LOWERCASE = 4,
        EVENT_MIXEDCASE = 8,

        CATEGORY_ONE   = 1001,
        CATEGORY_TWO   = 1010,
        CATEGORY_THREE = 1100,

        CALLBACK_RED    = 1101,
        CALLBACK_YELLOW = 1110,
        CALLBACK_BLUE   = 1111
    };
};

static
void case4RedCb(int event, bsl::string *results)
{
    ASSERT(results);

    switch (event) {
      case Case4::EVENT_UPPERCASE: {
        results->append("RED ");
      }  break;
      case Case4::EVENT_LOWERCASE: {
        results->append("red ");
      }  break;
      case Case4::EVENT_MIXEDCASE: {
        results->append("Red ");
      }  break;
    }
}

static
void case4YellowCb(int event, bsl::string *results)
{
    ASSERT(results);

    switch (event) {
      case Case4::EVENT_UPPERCASE: {
        results->append("YELLOW ");
      }  break;
      case Case4::EVENT_LOWERCASE: {
        results->append("yellow ");
      }  break;
      case Case4::EVENT_MIXEDCASE: {
        results->append("Yellow ");
      }  break;
    }
}

static
void case4BlueCb(int event, bsl::string *results)
{
    ASSERT(results);

    switch (event) {
      case Case4::EVENT_UPPERCASE: {
        results->append("BLUE ");
      }  break;
      case Case4::EVENT_LOWERCASE: {
        results->append("blue ");
      }  break;
      case Case4::EVENT_MIXEDCASE: {
        results->append("Blue ");
      }  break;
    }
}

//-----------------------------------------------------------------------------
typedef my_NotifyParams<int, bsl::string, int> Case2NotifyParams;

//extern "C"
void *case2NotifyThread(void *arg)
{
    // Continuously notify the 'bcecs_CallbackRegistry' object specified by
    // the 'd_registry_p' parameter in the parameter list specified by 'arg'
    // of type 'my_NotifyParams' with the event specified by the 'd_event_p'
    // parameter for the category specified by the 'd_category_p' parameter.
    // Terminate when the boolean value pointed to by the 'd_stop_p' parameter
    // becomes 'true'.

    ASSERT(arg);

    Case2NotifyParams *params = static_cast<Case2NotifyParams*>(arg);
    ASSERT(params->d_registry_p);
    ASSERT(params->d_category_p);
    ASSERT(params->d_event_p);
    ASSERT(params->d_stop_p);

    while (!*params->d_stop_p) {
        int rc = params->d_registry_p->notify(*params->d_category_p,
                                              *params->d_event_p);
        if (veryVerbose && 0 == rc) {
            MTCOUT << '\t' << "Notify: "
                   << "CATEGORY = " << *params->d_category_p << ", "
                   << "EVENT = "    << *params->d_event_p
                   << MTENDL;
        }
    }
    return 0;
}

static
void case2Cb(
        const int&         event,
        const bsl::string& category,
        my_Results        *results)
{
    ASSERT(results);

    bsl::ostringstream oss;
    oss << category << ' ' << event << endl;

    results->d_lock.lock();
    results->d_string.append(oss.str());
    results->d_lock.unlock();
}

//-----------------------------------------------------------------------------
static
void case1IdentityCb(
        const bdet_TimeInterval& event,
        bsl::string             *results)
{
    ASSERT(results);

    bsl::ostringstream oss;
    oss << event << ' ';
    results->append(oss.str());
}

static
void case1OffsetCb(
    const bdet_TimeInterval& event,
    const bdet_TimeInterval& offset,
    bsl::string             *results)
{
    ASSERT(results);

    bsl::ostringstream oss;
    oss << (event + offset) << ' ';
    results->append(oss.str());
}

//=============================================================================
//           CLASSES AND HELPER FUNCTIONS FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------
static
int parseDateTime(const char *dateString, bdet_Datetime *result)
{
    // Parse the string 'dateString' into day, month, year, hour, and
    // minute; and store the results into the 'bdet_Datetime' object
    // specified by 'result'.  Return 0 on success, and a non-zero value
    // otherwise.

    int  year, month, day, hour, minute;
    char mon[4] = {0};

    int rc = bsl::sscanf(dateString, "%2d%3s%4d_%2d:%2d",
                         &day, mon, &year, &hour, &minute);
    if (5 != rc) {
        return rc ? rc : -1;
    }

    const char *months = "JAN FEB MAR APR JUN JUL SEP OCT NOV DEC";
    const char *p      = bsl::strstr(months, mon);
    month = p ? (p - months) / 4 : -1;

    rc = result->setDatetimeIfValid(year, month, day, hour, minute);
    return rc ? -2 : 0;
}

static
void deleteCallback(const bsl::string& message, const char *sender)
{
    // Delete 'message' if 'sender' matches the sender specified in
    // 'message'.

    ASSERT(sender);

    bsl::string::size_type pos = message.find_first_of(' ');
    if (0 == message.compare(pos + 1, bsl::strlen(sender), sender)) {
        if (verbose) {
            bsl::cout << "\tDelete message." << bsl::endl;
        }
    }
}

static
void moveCallback(const bsl::string& message,
                  const char        *subject,
                  const char        *folder)
{
    // Move 'message' to the specified 'folder' if the 'message' subject
    // contains the specified 'subject'.

    ASSERT(subject);
    ASSERT(folder);

    bsl::string::size_type pos = message.find_first_of(' ');
    pos = message.find_first_of(' ', pos + 1);  // space after sender
    pos = message.find_first_of(' ', pos + 1);  // space after date

    if (bsl::string::npos != message.find(subject, pos)) {
        if (verbose) {
            bsl::cout << "\tMove message to folder '" << folder << "'."
                      << bsl::endl;
        }
    }
}

static
void flagCallback(const bsl::string& message, const bdet_Datetime& date)
{
    // Flag 'message' if its date is later than the specified 'date'.

    bsl::string::size_type pos = message.find_first_of(' ');
    pos = message.find_first_of(' ', pos + 1);  // space after sender

    bdet_Datetime sendDate;
    if (0 == parseDateTime(message.c_str() + pos + 1, &sendDate)) {
        if (sendDate < date) {
            if (verbose) {
                bsl::cout << "\tFlag message." << bsl::endl;
            }
        }
    }
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

    int test = (argc > 1) ? bsl::atoi(argv[1]) : 1;

    verbose = (argc > 2);
    veryVerbose = (argc > 3);
    veryVeryVerbose = (argc > 4);

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 9: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE 1
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and execute as shown.
        //
        // Plan:
        //   Incorporate the usage example from the header file into the test
        //   driver.  Make use of existing test apparatus by instantiating
        //   objects with a 'bslma_TestAllocator' object where applicable.
        //   Additionally, replace all calls to 'assert' in the usage example
        //   with calls to 'ASSERT'.  This now becomes the source, which is
        //   then "copied" back to the header file by reversing the above
        //   process.
        //
        // Testing:
        //   USAGE EXAMPLE 1
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "Testing Usage Example 1" << endl
                 << "=======================" << endl;
        }

        bcema_TestAllocator ta(veryVeryVerbose);
        {
            typedef bcecs_CallbackRegistry<bsl::string,
                                           bsl::string,
                                           bsl::string> RegistryType;
                // This typedef is provided for notational convenience when
                // declaring callbacks usable by the registry.

            RegistryType registry(&ta);

            // Set up filters for user "jimbo@bloomberg.net".

            RegistryType::Callback refson =
                bdef_BindUtil::bindA( &ta
                                    , &deleteCallback
                                    , _1
                                    , (const char *)"erefson@bloomberg.net");

            RegistryType::Callback nlrt =
                bdef_BindUtil::bindA( &ta
                                    , moveCallback
                                    , _1
                                    , (const char *)"TODAY'S NEWS"
                                    , (const char *)"NLRT");

            registry.registerCallback("jimbo@bloomberg.net", refson, "refson");
            registry.registerCallback("jimbo@bloomberg.net", nlrt, "nlrt");

            // Set up filters for user "ronaldo7@bloomberg.net".

            bdet_Datetime          flagDate;
            bdetu_Datetime::convertFromTimeT(&flagDate, bsl::time(0));
            flagDate.addDays(-5);

            RegistryType::Callback fiveDays =
                bdef_BindUtil::bindA(&ta, &flagCallback, _1, flagDate);
            registry.registerCallback("ronaldo7@bloomberg.net",
                                      fiveDays, "Five Day Filter");

            // Iterate over a set of email message headers, and notify the
            // corresponding recipient in the registry.

            const bsl::string MESSAGES[] = {
                "jimbo@bloomberg.net erefson@bloomberg.net "
                    "16SEP2004_02:13 Interested in the business of football?",
                "ronaldo7@bloomberg.net erefson@bloomberg.net "
                    "16SEP2004_02:13 Interested in the business of football?",
                "ronaldo9@bloomberg.net erefson@bloomberg.net "
                    "16SEP2004_02:13 Interested in the business of football?",
                "ronaldo7@bloomberg.net ronaldo9@bloomberg.net "
                    "28SEP2004_23:11 Real Madrid 4 - 2 Roma !",
                "ronaldo9@bloomberg.net ronaldo7@bloomberg.net "
                    "28SEP2004_23:13 Man. United 6 - 2 Fenerbahce :-)",
                "jimbo@bloomberg.net nlrt@bloomberg.net "
                    "08OCT2004_16:02 TODAY'S NEWS: 5 News Articles",
                "ronaldo7@bloomberg.net nlrt@bloomberg.net "
                    "08OCT2004_16:02 TODAY'S NEWS: 5 News Articles",
                "ronaldo9@bloomberg.net nlrt@bloomberg.net "
                    "08OCT2004_16:02 TODAY'S NEWS: 5 News Articles",
                "jimbo@bloomberg.net erefson@bloomberg.net "
                    "15OCT2004_07:34 "
                    "Another benefit of BLAW for your financial clients:",
                "ronaldo7@bloomberg.net erefson@bloomberg.net "
                    "15OCT2004_07:34 "
                    "Another benefit of BLAW for your financial clients:",
                "ronaldo9@bloomberg.net erefson@bloomberg.net "
                    "15OCT2004_07:34 "
                    "Another benefit of BLAW for your financial clients:",
                "jimbo@bloomberg.net admin@test.com 15OCT2004_12:45 test",
                "ronaldo7@bloomberg.net admin@test.com 15OCT2004_12:45 test",
                "ronaldo9@bloomberg.net admin@test.com 15OCT2004_12:45 test",
            };
            enum { NUM_MESSAGES = sizeof MESSAGES / sizeof *MESSAGES };

            for (int i = 0; i < NUM_MESSAGES; ++i) {
                const bsl::string&     MESSAGE = MESSAGES[i];
                bsl::string::size_type pos     = MESSAGE.find_first_of(' ');
                bsl::string            recipient(MESSAGE.substr(0, pos));

                if (verbose) {
                    bsl::cout << MESSAGE << bsl::endl;
                }
                registry.notify(recipient, MESSAGE);
            }

        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      }  break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING CONCERN: INSTANTIATE WITH NON-DEFAULT COMPARATORS
        //
        // Concerns:
        //   * That it is possible to instantiate a 'bcecs_CallbackRegistry'
        //     using comparators other than 'bsl::less<>', and that the
        //     registry exhibits correct behavior.
        //
        // Plan:
        //   Instantiate a modifiable 'bcecs_CallbackRegistry', 'mX', and a
        //   non-modifiable reference to 'mX' named 'X'.  Create three
        //   categories, ONE, TWO, and THREE.  Define three callbacks, 'red',
        //   'yellow', and 'blue' which accept three different events,
        //   'EVENT_UPPERCASE', 'EVENT_LOWERCASE', and 'EVENT_MIXEDCASE'.  Each
        //   callback is bound to a string, and appends the text "RED", "red",
        //   or "Red", etc, depending on the particular callback and event.
        //
        //   Register callback 'red' under categories ONE, TWO, and THREE.
        //   Register callback 'yellow' under categories TWO and THREE.
        //   Register callback 'blue' under category THREE.  Using 'X', verify
        //   the total number of callbacks and categories, as well as the
        //   number of callbacks or categories as each category is created and
        //   each callback is registered.
        //
        //   Notify category ONE with 'EVENT_UPPERCASE', category TWO with
        //   'EVENT_LOWERCASE', and category THREE with 'EVENT_MIXEDCASE'.
        //   Verify the results via the strings bound to each callback.
        //
        // Testing:
        //   Concern: Instantiate with non-default comparators
        // --------------------------------------------------------------------
        if (verbose) {
            cout << "Concern: Instantiate With Non-Default Comparators" << endl
                 << "================================================="
                 << endl;
        }

        typedef int                             EVENT;
        typedef const char*                     CATEGORY;
        typedef const char*                     CALLBACK_ID;
        typedef my_CaseInsensitiveStringCompare CATEGORY_COMPARATOR;
        typedef my_CaseSensitiveStringCompare   CALLBACK_COMPARATOR;
        typedef bcecs_CallbackRegistry<EVENT,
                                       CATEGORY,
                                       CALLBACK_ID,
                                       CATEGORY_COMPARATOR,
                                       CALLBACK_COMPARATOR> Obj;

        bcema_TestAllocator ta(veryVeryVerbose);
        {
            Obj mX(&ta);  const Obj& X = mX;

            bsl::string   resultRed, resultYellow, resultBlue;

            Obj::Callback red =
                bdef_BindUtil::bindA(&ta, &case4RedCb, _1, &resultRed);

            Obj::Callback yellow =
                bdef_BindUtil::bindA(&ta, &case4YellowCb, _1, &resultYellow);

            Obj::Callback blue =
                bdef_BindUtil::bindA(&ta, &case4BlueCb, _1, &resultBlue);

            ASSERT(0  == X.totalNumCallbacks());
            ASSERT(0  == X.totalNumCategories());
            ASSERT(-1 == X.numCategories("CALLBACK"));
            ASSERT(-1 == X.numCategories("Callback"));
            ASSERT(-1 == X.numCategories("callback"));
            ASSERT(-1 == X.numCallbacks("ONE"));
            ASSERT(-1 == X.numCallbacks("TWO"));
            ASSERT(-1 == X.numCallbacks("THREE"));

            // Add callback 'red' to category ONE.
            ASSERT(0 == mX.registerCallback("One", red, "CALLBACK"));
            ASSERT(1 == X.totalNumCategories());
            ASSERT(1 == X.totalNumCallbacks());
            ASSERT(1 == X.numCategories("CALLBACK"));
            ASSERT(1 == X.numCallbacks("oNe"));

            // Add callback 'yellow' to category TWO.
            ASSERT(0 == mX.registerCallback("Two", yellow, "Callback"));
            ASSERT(2 == X.totalNumCategories());
            ASSERT(2 == X.totalNumCallbacks());
            ASSERT(1 == X.numCategories("Callback"));
            ASSERT(1 == X.numCallbacks("tWo"));

            // Add callback 'blue' to category THREE.
            ASSERT(0 == mX.registerCallback("Three", blue, "callback"));
            ASSERT(3 == X.totalNumCategories());
            ASSERT(3 == X.totalNumCallbacks());
            ASSERT(1 == X.numCategories("callback"));
            ASSERT(1 == X.numCallbacks("ThReE"));

            // Add callback 'red' to categories TWO and THREE
            ASSERT(0 == mX.registerCallback("TwO", red, "CALLBACK"));
            ASSERT(0 == mX.registerCallback("THrEE", red, "CALLBACK"));
            ASSERT(3 == X.totalNumCategories());
            ASSERT(5 == X.totalNumCallbacks());
            ASSERT(3 == X.numCategories("CALLBACK"));
            ASSERT(2 == X.numCallbacks("TwO"));
            ASSERT(2 == X.numCallbacks("thRee"));

            // Add callback 'yellow' to category THREE
            ASSERT(0 == mX.registerCallback("tHrEe", yellow, "Callback"));
            ASSERT(3 == X.totalNumCategories());
            ASSERT(6 == X.totalNumCallbacks());
            ASSERT(2 == X.numCategories("Callback"));
            ASSERT(2 == X.numCallbacks("two"));
            ASSERT(3 == X.numCallbacks("three"));

            // Notify categories, and verify results.

            ASSERT(0 == mX.notify("oNe", Case8::EVENT_UPPERCASE));
            ASSERT(0 == mX.notify("twO", Case8::EVENT_LOWERCASE));
            ASSERT(0 == mX.notify("tHREe", Case8::EVENT_MIXEDCASE));
            ASSERT("RED red Red " == resultRed);
            ASSERT("yellow Yellow " == resultYellow);
            ASSERT("Blue " == resultBlue);
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      }  break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'deleteCategory' FUNCTION
        //
        // Concerns:
        //   * That 'deleteCategory' returns -1 if the specified category
        //     does not exist in the registry.
        //
        //   * That 'deleteCategory' removes all registered callbacks from
        //     the specified category, removes the category from the registry,
        //     and returns 0.
        //
        //   * That 'deleteCategory' blocks until all threads have
        //     completed execution of any callbacks registered under that
        //     category.
        //
        // Plan:
        //   Instantiate a modifiable 'bcecs_CallbackRegistry', 'mX', and a
        //   non-modifiable reference to 'mX' named 'X'.  Verify that calling
        //   'deleteCategory' for a non-existent category returns -1.
        //   Register several callbacks under category "Three" and some
        //   callbacks under category "Two" with some shared callbacks.
        //   Register one callback under category "One".  Verify the number
        //   of categories and callbacks using 'X'.  Deregister the callback
        //   under category "One".  Verify that calling 'deleteCategory'
        //   for category "One" returns 0, and removes category "One" from
        //   the registry.  Verify that calling 'deleteCategory' on 'mX' for
        //   for category "Two" deletes all callbacks from category "Two", as
        //   well as removes category "Three" from the registry, but does not
        //   affect shared callbacks registered under category "Three".
        //
        //   Instantiate a modifiable 'bcecs_CallbackRegistry', 'mX', and a
        //   non-modifiable reference to 'mX' named 'X'.  Register a callback
        //   'cb' under category "Local" which is parameterized by a integer
        //   number of seconds, and which is bound to a counter.  When 'cb' is
        //   executed, it sleeps for the specified number of seconds, and then
        //   increments the counter.  Spawn several threads which notify 'mX'
        //   with a timeout event for the "Local" category.  In the main
        //   thread, delete the "Local" category, and assert that the counter
        //   has been incremented by each thread.
        //
        // Testing:
        //   int deleteCategory(const CATEGORY& category);
        // --------------------------------------------------------------------
        if (verbose) {
            cout << "Testing 'deleteCategory' Function" << endl
                 << "=================================" << endl;
        }

        typedef int         EVENT;
        typedef bsl::string CATEGORY;
        typedef bsl::string CALLBACK_ID;
        typedef bcecs_CallbackRegistry<EVENT, CATEGORY, CALLBACK_ID> Obj;

        bcema_TestAllocator ta(veryVeryVerbose);
        {
            //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Single-threaded test of 'deleteCategory' result codes.
            if (verbose) {
                cout << "\nVerify 'deleteCategory' result codes."
                     << "\n-------------------------------------"
                     << endl;
            }
            {
                Obj mX(&ta);  const Obj& X = mX;

                Obj::Callback cb(&nopCallback, &ta);

                ASSERT(0 == X.totalNumCategories());
                ASSERT(0 == X.totalNumCallbacks());

                // Add categories and callbacks.
                ASSERT(0 == mX.registerCallback("THREE", cb, "white"));
                ASSERT(0 == mX.registerCallback("THREE", cb, "black"));
                ASSERT(0 == mX.registerCallback("THREE", cb, "green"));
                ASSERT(0 == mX.registerCallback("TWO",   cb, "black"));
                ASSERT(0 == mX.registerCallback("TWO",   cb, "green"));
                ASSERT(0 == mX.registerCallback("ONE",   cb, "pink"));

                ASSERT(3 == X.totalNumCategories());
                ASSERT(6 == X.totalNumCallbacks());
                ASSERT(3 == X.numCallbacks("THREE"));
                ASSERT(2 == X.numCallbacks("TWO"));
                ASSERT(1 == X.numCallbacks("ONE"));
                ASSERT(1 == X.numCategories("white"));
                ASSERT(2 == X.numCategories("black"));
                ASSERT(2 == X.numCategories("green"));
                ASSERT(1 == X.numCategories("pink"));

                // Delete a non-existent category.
                ASSERT(-1 == mX.deleteCategory("FOUR"));

                // Deregister the only callback in category "ONE".
                ASSERT(0  == mX.deregisterCallback("ONE", "pink"));
                ASSERT(3  == X.totalNumCategories());
                ASSERT(5  == X.totalNumCallbacks());
                ASSERT(3  == X.numCallbacks("THREE"));
                ASSERT(2  == X.numCallbacks("TWO"));
                ASSERT(0  == X.numCallbacks("ONE"));
                ASSERT(1  == X.numCategories("white"));
                ASSERT(2  == X.numCategories("black"));
                ASSERT(2  == X.numCategories("green"));
                ASSERT(-1 == X.numCategories("pink"));

                // Delete category "ONE".
                ASSERT(0  == mX.deleteCategory("ONE"));
                ASSERT(2  == X.totalNumCategories());
                ASSERT(5  == X.totalNumCallbacks());
                ASSERT(3  == X.numCallbacks("THREE"));
                ASSERT(2  == X.numCallbacks("TWO"));
                ASSERT(-1 == X.numCallbacks("ONE"));
                ASSERT(1  == X.numCategories("white"));
                ASSERT(2  == X.numCategories("black"));
                ASSERT(2  == X.numCategories("green"));
                ASSERT(-1 == X.numCategories("pink"));

                // Delete category "TWO".
                ASSERT(0  == mX.deleteCategory("TWO"));
                ASSERT(1  == X.totalNumCategories());
                ASSERT(3  == X.totalNumCallbacks());
                ASSERT(3  == X.numCallbacks("THREE"));
                ASSERT(-1 == X.numCallbacks("TWO"));
                ASSERT(-1 == X.numCallbacks("ONE"));
                ASSERT(1  == X.numCategories("white"));
                ASSERT(1  == X.numCategories("black"));
                ASSERT(1  == X.numCategories("green"));
                ASSERT(-1 == X.numCategories("pink"));
            }

            //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Multi-threaded test of 'deleteCategory' blocking semantics.
            if (verbose) {
                cout << "\nVerify 'deleteCategory' blocking semantics."
                     << "\n-------------------------------------------"
                     << endl;
            }
            {
                Obj mX(&ta);  const Obj& X = mX;

                const int NUM_NOTIFY_THREADS = 16;
                int       TIMEOUT            = 1;    // seconds

                bcemt_Barrier  startBarrier(1 + NUM_NOTIFY_THREADS);
                bces_AtomicInt numNotifications(0);

                Obj::Callback cb =
                    bdef_BindUtil::bindA( &ta
                                        , &case7Cb
                                        , _1
                                        , &startBarrier
                                        , &numNotifications);

                bsl::string LOCAL("Local");

                ASSERT(0 == mX.registerCallback(LOCAL, cb, "registered"));
                ASSERT(1 == X.totalNumCategories());
                ASSERT(1 == X.totalNumCallbacks());
                ASSERT(1 == X.numCallbacks(LOCAL));
                ASSERT(1 == X.numCategories("registered"));
                bcemt_ThreadUtil::Handle handles[NUM_NOTIFY_THREADS];
                Case7NotifyParams        params[NUM_NOTIFY_THREADS];

                for (int i = 0; i < NUM_NOTIFY_THREADS; ++i) {
                    params[i].d_registry_p = &mX;
                    params[i].d_category_p = &LOCAL;
                    params[i].d_event_p    = &TIMEOUT;

                    if (veryVerbose) {
                        MTCOUT << "Spawn thread " << i + 1 << MTENDL;
                    }
                    LOOP_ASSERT(i, 0 == bcemt_ThreadUtil::create(
                                  &handles[i], case7NotifyThread, &params[i]));
                }
                ASSERT(0 == numNotifications);
                startBarrier.wait();

                if (veryVerbose) {
                    MTCOUT << "Delete category \"Local\"." << MTENDL;
                }
                ASSERT(0 == mX.deleteCategory(LOCAL));

                if (veryVerbose) {
                    MTCOUT << "Verify results." << MTENDL;
                }
                ASSERT(NUM_NOTIFY_THREADS == numNotifications);

                ASSERT(0  == X.totalNumCategories());
                ASSERT(0  == X.totalNumCallbacks());
                ASSERT(-1 == X.numCallbacks(LOCAL));
                ASSERT(-1 == X.numCategories("registered"));

                for (int i = 0; i < NUM_NOTIFY_THREADS; ++i) {
                    LOOP_ASSERT(i, 0 == bcemt_ThreadUtil::join(handles[i]));
                }
            }
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      }  break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'deregisterCallback' FUNCTION
        //
        // Concerns:
        //   * That 'deregisterCallback' returns -1 if the specified callback
        //     is not registered under the specified category.
        //
        //   * That 'deregisterCallback' returns -2 if the specified category
        //     does not exist in the registry.
        //
        //   * That 'deregisterCallback' removes the specified callback from
        //     the specified category, and returns 0.
        //
        //   * That 'deregisterCallback' does not delete the category from the
        //     registry if the specified callback is the only callback
        //     registered under the category.
        //
        //   * That 'deregisterCallback' blocks until all threads have
        //     completed execution of the specified callback.
        //
        // Plan:
        //   Instantiate a modifiable 'bcecs_CallbackRegistry', 'mX', and a
        //   non-modifiable reference to 'mX' named 'X'.  Verify that calling
        //   'deregisterCallback' for a non-existent category returns -2.
        //   Register a callback, 'cb', for category "Local".  Verify the
        //   number of categories and callbacks using 'X'.  Verify that
        //   calling 'deregisterCallback' for category "Local", but a
        //   non-existent callback returns -1.  Verify that calling
        //   'deregisterCallback' on 'mX' for 'cb' under category "Local"
        //   returns 0, and removes the callback 'cb', but not the category
        //   "Local".
        //
        //   Instantiate a modifiable 'bcecs_CallbackRegistry', 'mX', and a
        //   non-modifiable reference to 'mX' named 'X'.  Register a callback
        //   'cb' under category "Local" which is parameterized by a integer
        //   number of seconds, and which is bound to a counter.  When 'cb' is
        //   executed, it sleeps for the specified number of seconds, and then
        //   increments the counter.  Spawn several threads which notify 'mX'
        //   with a timeout event for the "Local" category.  In the main
        //   thread, deregister 'cb' under the "Local" category, and assert
        //   that the counter has been incremented by each thread.
        //
        // Testing:
        //   int deregisterCallback(const CATEGORY&    category,
        //                          const CALLBACK_ID& id);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "Testing 'deregisterCallback' Function" << endl
                 << "=====================================" << endl;
        }

        typedef int         EVENT;
        typedef bsl::string CATEGORY;
        typedef bsl::string CALLBACK_ID;
        typedef bcecs_CallbackRegistry<EVENT, CATEGORY, CALLBACK_ID> Obj;

        bcema_TestAllocator ta(veryVeryVerbose);
        {
            //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Single-threaded test of 'deregisterCallback' result codes.
            if (verbose) {
                cout << "\nVerify 'deregisterCallback' result codes."
                     << "\n-----------------------------------------"
                     << endl;
            }
            {
                Obj mX(&ta);  const Obj& X = mX;

                Obj::Callback cb(&nopCallback, &ta);

                ASSERT(0 == X.totalNumCategories());
                ASSERT(0 == X.totalNumCallbacks());

                ASSERT(-2 == mX.deregisterCallback("Local", "not registered"));

                ASSERT(0  == mX.registerCallback("Local", cb, "registered"));
                ASSERT(1  == X.totalNumCategories());
                ASSERT(1  == X.totalNumCallbacks());
                ASSERT(1  == X.numCallbacks("Local"));
                ASSERT(-1 == X.numCallbacks("Remote"));
                ASSERT(1  == X.numCategories("registered"));

                ASSERT(-1 == mX.deregisterCallback("Local", "not registered"));
                ASSERT(-2 == mX.deregisterCallback("Remote", "registered"));

                ASSERT(0  == mX.deregisterCallback("Local", "registered"));
                ASSERT(1  == X.totalNumCategories());
                ASSERT(0  == X.totalNumCallbacks());
                ASSERT(0  == X.numCallbacks("Local"));
                ASSERT(-1 == X.numCallbacks("Remote"));
                ASSERT(-1 == X.numCategories("registered"));
            }

            //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Multi-threaded test of 'deregisterCallback' blocking semantics.
            if (verbose) {
                cout << "\nVerify 'deregisterCallback' blocking semantics."
                     << "\n-----------------------------------------------"
                     << endl;
            }
            {
                Obj mX(&ta);  const Obj& X = mX;

                const int NUM_NOTIFY_THREADS = 16;
                int       TIMEOUT            = 1;    // seconds

                bcemt_Barrier  startBarrier(1 + NUM_NOTIFY_THREADS);
                bces_AtomicInt numNotifications(0);

                Obj::Callback  cb =
                    bdef_BindUtil::bindA( &ta
                                        , &case6Cb
                                        , _1
                                        , &startBarrier
                                        , &numNotifications);

                bsl::string LOCAL("Local");

                ASSERT(0 == mX.registerCallback(LOCAL, cb, "registered"));
                ASSERT(1 == X.totalNumCategories());
                ASSERT(1 == X.totalNumCallbacks());
                ASSERT(1 == X.numCallbacks(LOCAL));
                ASSERT(1 == X.numCategories("registered"));
                bcemt_ThreadUtil::Handle handles[NUM_NOTIFY_THREADS];
                Case6NotifyParams        params[NUM_NOTIFY_THREADS];

                for (int i = 0; i < NUM_NOTIFY_THREADS; ++i) {
                    params[i].d_registry_p = &mX;
                    params[i].d_category_p = &LOCAL;
                    params[i].d_event_p    = &TIMEOUT;

                    if (veryVerbose) {
                        MTCOUT << "Spawn thread " << i + 1 << MTENDL;
                    }
                    LOOP_ASSERT(i, 0 == bcemt_ThreadUtil::create(
                                  &handles[i], case6NotifyThread, &params[i]));
                }
                ASSERT(0 == numNotifications);
                startBarrier.wait();

                if (veryVerbose) {
                    MTCOUT << "Deregister callback." << MTENDL;
                }
                ASSERT(0 == mX.deregisterCallback(LOCAL, "registered"));

                if (veryVerbose) {
                    MTCOUT << "Verify results." << MTENDL;
                }
                ASSERT(NUM_NOTIFY_THREADS == numNotifications);

                ASSERT(1  == X.totalNumCategories());
                ASSERT(0  == X.totalNumCallbacks());
                ASSERT(0  == X.numCallbacks(LOCAL));
                ASSERT(-1 == X.numCategories("registered"));

                for (int i = 0; i < NUM_NOTIFY_THREADS; ++i) {
                    LOOP_ASSERT(i, 0 == bcemt_ThreadUtil::join(handles[i]));
                }
            }
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      }  break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR
        //
        // Concerns:
        //   Verify the correct behavior of the output operator.
        //
        //   Currently, there is no 'operator<<' defined for
        //   'btemt_ChannelOutStreamBuf', so this test case remains empty.
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "Testing Output (<<) Operator" << endl
                 << "============================" << endl;
        }
      }  break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING BASIC ACCESSORS
        //
        // Concerns:
        //   * That 'numCategories' returns the total number of categories to
        //     which the callback specified by 'id' is registered, or -1 if
        //     the specified callback is not in the registry.
        //
        //   * That 'numCallbacks' returns the total number of callbacks
        //     registered to the specified 'category' group, or -1 if the
        //     specified category is not in the registry.
        //
        //   * That 'totalNumCategories' returns the total number of categories
        //     in the registry.
        //
        //   * That 'totalNumCallbacks' returns the total number of callbacks
        //     registered to all categories, and that callbacks registered
        //     to multiple categories are counted more than once.
        //
        // Plan:
        //   Instantiate a modifiable 'bcecs_CallbackRegistry', 'mX', and a
        //   non-modifiable reference to 'mX' named 'X'.  Create three
        //   categories, ONE, TWO, and THREE.  Define three callbacks, 'red',
        //   'yellow', and 'blue' which accept three different events,
        //   'EVENT_UPPERCASE', 'EVENT_LOWERCASE', and 'EVENT_MIXEDCASE'.  Each
        //   callback is bound to a string, and appends the text "RED", "red",
        //   or "Red", etc, depending on the particular callback and event.
        //
        //   Register callback 'red' under categories ONE, TWO, and THREE.
        //   Register callback 'yellow' under categories TWO and THREE.
        //   Register callback 'blue' under category THREE.  Using 'X', verify
        //   the total number of callbacks and categories, as well as the
        //   number of callbacks or categories as each category is created and
        //   each callback is registered.
        //
        //   Notify category ONE with 'EVENT_UPPERCASE', category TWO with
        //   'EVENT_LOWERCASE', and category THREE with 'EVENT_MIXEDCASE'.
        //
        //   Deregister callback 'red' from categories ONE and TWO.  Delete
        //   category THREE.  Repeat the same notifications as before, and
        //   verify the results via the strings bound to each callback.
        //
        // Testing:
        //   int numCategories(const CALLBACK_ID& id) const;
        //   int numCallbacks(const CATEGORY& category) const;
        //   int totalNumCategories() const;
        //   int totalNumCallbacks() const;
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "Testing Basic Accessors" << endl
                 << "=======================" << endl;
        }

        typedef int EVENT;
        typedef int CATEGORY;
        typedef int CALLBACK_ID;
        typedef bcecs_CallbackRegistry<EVENT, CATEGORY, CALLBACK_ID> Obj;

        bcema_TestAllocator ta(veryVeryVerbose);
        {
            Obj mX(&ta);  const Obj& X = mX;

            bsl::string   resultRed, resultYellow, resultBlue;

            Obj::Callback red =
                bdef_BindUtil::bindA(&ta, &case4RedCb, _1, &resultRed);

            Obj::Callback yellow =
                bdef_BindUtil::bindA(&ta, &case4YellowCb, _1, &resultYellow);

            Obj::Callback blue =
                bdef_BindUtil::bindA(&ta, &case4BlueCb, _1, &resultBlue);

            ASSERT(0  == X.totalNumCallbacks());
            ASSERT(0  == X.totalNumCategories());
            ASSERT(-1 == X.numCategories(Case4::CALLBACK_RED));
            ASSERT(-1 == X.numCategories(Case4::CALLBACK_YELLOW));
            ASSERT(-1 == X.numCategories(Case4::CALLBACK_BLUE));
            ASSERT(-1 == X.numCallbacks(Case4::CATEGORY_ONE));
            ASSERT(-1 == X.numCallbacks(Case4::CATEGORY_TWO));
            ASSERT(-1 == X.numCallbacks(Case4::CATEGORY_THREE));

            // Add callback RED to category ONE.
            ASSERT(0 == mX.registerCallback(Case4::CATEGORY_ONE, red,
                                            Case4::CALLBACK_RED));
            ASSERT(1 == X.totalNumCategories());
            ASSERT(1 == X.totalNumCallbacks());
            ASSERT(1 == X.numCategories(Case4::CALLBACK_RED));
            ASSERT(1 == X.numCallbacks(Case4::CATEGORY_ONE));

            // Add callback YELLOW to category TWO.
            ASSERT(0 == mX.registerCallback(Case4::CATEGORY_TWO, yellow,
                                            Case4::CALLBACK_YELLOW));
            ASSERT(2 == X.totalNumCategories());
            ASSERT(2 == X.totalNumCallbacks());
            ASSERT(1 == X.numCategories(Case4::CALLBACK_YELLOW));
            ASSERT(1 == X.numCallbacks(Case4::CATEGORY_TWO));

            // Add callback BLUE to category THREE.
            ASSERT(0 == mX.registerCallback(Case4::CATEGORY_THREE, blue,
                                            Case4::CALLBACK_BLUE));
            ASSERT(3 == X.totalNumCategories());
            ASSERT(3 == X.totalNumCallbacks());
            ASSERT(1 == X.numCategories(Case4::CALLBACK_BLUE));
            ASSERT(1 == X.numCallbacks(Case4::CATEGORY_THREE));

            // Add callback RED to categories TWO and THREE
            ASSERT(0 == mX.registerCallback(Case4::CATEGORY_TWO, red,
                                            Case4::CALLBACK_RED));
            ASSERT(0 == mX.registerCallback(Case4::CATEGORY_THREE, red,
                                            Case4::CALLBACK_RED));
            ASSERT(3 == X.totalNumCategories());
            ASSERT(5 == X.totalNumCallbacks());
            ASSERT(3 == X.numCategories(Case4::CALLBACK_RED));
            ASSERT(2 == X.numCallbacks(Case4::CATEGORY_TWO));
            ASSERT(2 == X.numCallbacks(Case4::CATEGORY_THREE));

            // Add callback YELLOW to category THREE
            ASSERT(0 == mX.registerCallback(Case4::CATEGORY_THREE, yellow,
                                            Case4::CALLBACK_YELLOW));
            ASSERT(3 == X.totalNumCategories());
            ASSERT(6 == X.totalNumCallbacks());
            ASSERT(2 == X.numCategories(Case4::CALLBACK_YELLOW));
            ASSERT(2 == X.numCallbacks(Case4::CATEGORY_TWO));
            ASSERT(3 == X.numCallbacks(Case4::CATEGORY_THREE));

            // Notify categories, and verify results.

            ASSERT(0 == mX.notify(Case4::CATEGORY_ONE,
                                  Case4::EVENT_UPPERCASE));
            ASSERT("RED " == resultRed);

            ASSERT(0 == mX.notify(Case4::CATEGORY_TWO,
                                  Case4::EVENT_LOWERCASE));
            ASSERT("RED red " == resultRed);
            ASSERT("yellow " == resultYellow);

            ASSERT(0 == mX.notify(Case4::CATEGORY_THREE,
                                  Case4::EVENT_MIXEDCASE));
            ASSERT("RED red Red " == resultRed);
            ASSERT("yellow Yellow " == resultYellow);
            ASSERT("Blue " == resultBlue);

            // Remove some categories and callbacks.

            ASSERT(0 == mX.deregisterCallback(Case4::CATEGORY_ONE,
                                              Case4::CALLBACK_RED));
            ASSERT(0 == mX.deregisterCallback(Case4::CATEGORY_TWO,
                                              Case4::CALLBACK_RED));
            ASSERT(0 == mX.deleteCategory(Case4::CATEGORY_THREE));

            ASSERT(2 == X.totalNumCategories());
            ASSERT(1 == X.totalNumCallbacks());
            ASSERT(1 == X.numCategories(Case4::CALLBACK_YELLOW));
            ASSERT(0 == X.numCallbacks(Case4::CATEGORY_ONE));
            ASSERT(1 == X.numCallbacks(Case4::CATEGORY_TWO));

            ASSERT(-1 == X.numCategories(Case4::CALLBACK_RED));
            ASSERT(-1 == X.numCategories(Case4::CALLBACK_BLUE));
            ASSERT(-1 == X.numCallbacks(Case4::CATEGORY_THREE));

            // Notify, and verify again.

            ASSERT(0  == mX.notify(Case4::CATEGORY_ONE,
                                   Case4::EVENT_UPPERCASE));
            ASSERT(0  == mX.notify(Case4::CATEGORY_TWO,
                                   Case4::EVENT_UPPERCASE));
            ASSERT(-1 == mX.notify(Case4::CATEGORY_THREE,
                                   Case4::EVENT_UPPERCASE));
            ASSERT("RED red Red " == resultRed);
            ASSERT("yellow Yellow YELLOW " == resultYellow);
            ASSERT("Blue " == resultBlue);
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      }  break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY TEST APPARATUS
        //
        // Concerns:
        //   * That 'my_CaseSensitiveStringCompare' returns 'true' when its
        //     left-hand-side argument lexicographically less than its right-
        //     hand-side argument, and false otherwise, taking case into
        //     account.
        //
        //   * That 'my_CaseInsensitiveStringCompare' returns 'true' when its
        //     left-hand-side argument lexicographically less than its right-
        //     hand-side argument, and false otherwise, ignoring case.
        //
        // Plan:
        //   Iterate over a set of test vectors varying in left-hand-side and
        //   right-hand-side arguments, and the expected comparison result.
        //   For each test vector, compare the left- and right-hand-side
        //   arguments with an instance of 'my_CaseSensitiveStringCompare',
        //   and verify the result against the expected result.
        //
        //   Iterate over a set of test vectors varying in left-hand-side and
        //   right-hand-side arguments, and the expected comparison result.
        //   For each test vector, compare the left- and right-hand-side
        //   arguments with an instance of 'my_CaseInsensitiveStringCompare',
        //   and verify the result against the expected result.
        //
        // Tests:
        //   bool my_CaseSensitiveStringCompare::operator()(const char *lhs,
        //                                                  const char *rhs);
        //   bool my_CaseInsensitiveStringCompare::operator()(const char *lhs,
        //                                                    const char *rhs);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "Testing Primary Test Apparatus" << endl
                 << "==============================" << endl;
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) {
            cout << "\nTesting 'my_CaseSensitiveStringCompare'."
                 << "\n----------------------------------------" << endl;
        }
        {
            const struct {
                int         d_line;      // source line number
                const char *d_lhs_p;     // left hand operand
                const char *d_rhs_p;     // right hand operand
                bool        d_result;    // expected result
            } DATA[] = {
                //Line  LHS    RHS    Result
                //----  ---    ---    ------
                { L_,   "A",   "B",   true,  },
                { L_,   "B",   "A",   false, },
                { L_,   "A",   "A",   false, },
                { L_,   "A",   "a",   true,  },
                { L_,   "a",   "A",   false, },
                { L_,   "a",   "B",   false, },
                { L_,   "B",   "a",   true,  },
            };
            enum { DATA_SIZE = sizeof DATA / sizeof *DATA };

            for (int i = 0; i < DATA_SIZE; ++i) {
                const int   LINE   = DATA[i].d_line;
                const char *LHS    = DATA[i].d_lhs_p;
                const char *RHS    = DATA[i].d_rhs_p;
                const bool  RESULT = DATA[i].d_result;

                if (verbose) {
                    P_(i); P_(LINE); P_(LHS); P_(RHS); P(RESULT);
                }

                my_CaseSensitiveStringCompare compare;

                bool result = compare(LHS, RHS);
                LOOP2_ASSERT(i, LINE, RESULT == result);
            }
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) {
            cout << "\nTesting 'my_CaseInsensitiveStringCompare'."
                 << "\n------------------------------------------" << endl;
        }
        {
            const struct {
                int         d_line;      // source line number
                const char *d_lhs_p;     // left hand operand
                const char *d_rhs_p;     // right hand operand
                bool        d_result;    // expected result
            } DATA[] = {
                //Line  LHS    RHS    Result
                //----  ---    ---    ------
                { L_,   "A",   "B",   true,  },
                { L_,   "B",   "A",   false, },
                { L_,   "A",   "A",   false, },
                { L_,   "A",   "a",   false, },
                { L_,   "a",   "A",   false, },
                { L_,   "a",   "B",   true,  },
                { L_,   "B",   "a",   false, },
            };
            enum { DATA_SIZE = sizeof DATA / sizeof *DATA };

            for (int i = 0; i < DATA_SIZE; ++i) {
                const int   LINE   = DATA[i].d_line;
                const char *LHS    = DATA[i].d_lhs_p;
                const char *RHS    = DATA[i].d_rhs_p;
                const bool  RESULT = DATA[i].d_result;

                if (verbose) {
                    P_(i); P_(LINE); P_(LHS); P_(RHS); P(RESULT);
                }

                my_CaseInsensitiveStringCompare compare;

                bool result = compare(LHS, RHS);
                LOOP2_ASSERT(i, LINE, RESULT == result);
            }
        }
      }  break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS (BOOTSTRAP)
        //
        // Concerns:
        //   * That it is possible to instantiate a 'bcecs_CallbackRegistry'
        //     object with a variety of template parameters.
        //
        //   * That registering the same callback ID more than once under one
        //     category fails, returning 1.
        //
        //   * That registering the same callback ID under multiple categories
        //     succeeds, returning 0.
        //
        //   * That notifying an non-existent category fails, returning 1.
        //
        //   * That notifying an existing category succeeds, returning 0.
        //
        // Plan:
        //   Instantiate a modifiable 'bcecs_CallbackRegistry', 'mX'.
        //   Verify that registering the same callback ID more than once under
        //   one category fails.  Verify that notifying a non-existent category
        //   fails.
        //
        //   Instantiate a modifiable 'bcecs_CallbackRegistry', 'mX', and a
        //   non-modifiable reference to 'mX' named 'X'.  Iterate over a set
        //   of vectors varying in category name and event.  For each
        //   vector, spawn a thread which constantly notifies the
        //   registry with the specified category and event.  Iterate over a
        //   second set of vectors varying in category, callback, and
        //   callback ID.  For each vector, register the callback under
        //   the specified category with the specified callback ID.  Each
        //   callback functor is bound to the specified category, and a
        //   'my_Results' object.  Each callback function appends the category
        //   and the event to the result string contained within the
        //   'my_Results' object.  After the last callback has been registered,
        //   sleep for some time.  Then, clean up the threads, and verify that
        //   every result object string indicates that the callback was called
        //   from every notify thread at least once.
        //
        // Testing:
        //   bcecs_CallbackRegistry(bslma_Allocator *basicAllocator = 0);
        //   ~bcecs_CallbackRegistry();
        //   int registerCallback(const CATEGORY&    category,
        //                        const CALLBACK_ID& id,
        //   int notify(const CATEGORY& category, const EVENT& event);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "TESTING PRIMARY MANIPULATORS (BOOTSTRAP)" << endl
                 << "========================================" << endl;
        }

        typedef int         EVENT;
        typedef bsl::string CATEGORY;
        typedef int         CALLBACK_ID;
        typedef bcecs_CallbackRegistry<EVENT, CATEGORY, CALLBACK_ID> Obj;

        bcema_TestAllocator ta(veryVeryVerbose);
        {
            enum {
                EVENT,
                CALLBACK_1,
                CALLBACK_2
            };
            CATEGORY CATEGORY_A("CATEGORY_A");
            CATEGORY CATEGORY_B("CATEGORY_B");

            Obj mX(&ta);  const Obj& X = mX;

            ASSERT(0 == X.totalNumCallbacks());
            ASSERT(0 == X.totalNumCategories());

            Obj::Callback cb(&nopCallback, &ta);

            ASSERT(0 == mX.registerCallback(CATEGORY_A, cb, CALLBACK_1));
            ASSERT(1 == mX.registerCallback(CATEGORY_A, cb, CALLBACK_1));
            ASSERT(0 == mX.registerCallback(CATEGORY_B, cb, CALLBACK_1));
            ASSERT(0 == mX.registerCallback(CATEGORY_B, cb, CALLBACK_2));

            ASSERT(2 == X.totalNumCategories());
            ASSERT(3 == X.totalNumCallbacks());
            ASSERT(2 == X.numCategories(CALLBACK_1));
            ASSERT(1 == X.numCategories(CALLBACK_2));
            ASSERT(1 == X.numCallbacks(CATEGORY_A));
            ASSERT(2 == X.numCallbacks(CATEGORY_B));
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        {
            Obj mX(&ta);  const Obj& X = mX;

            CATEGORY CATEGORIES[] = {
                "BLUE",
                "RED",
                "YELLOW",
            };
            enum { NUM_CATEGORIES = sizeof CATEGORIES / sizeof *CATEGORIES };

            EVENT EVENTS[] = {
                10, 20, 30,
            };
            enum { NUM_EVENTS = sizeof EVENTS / sizeof *EVENTS };

            ASSERT((int)NUM_EVENTS == (int)NUM_CATEGORIES);

            //-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
            // Start notify threads.

            struct {
                int                      d_line;      // source line number
                CATEGORY                 d_category;  // notify category
                EVENT                    d_event;     // notify event
                bcemt_ThreadUtil::Handle d_handle;    // thread handle
            } NOTIFY_THREADS[] = {
                //Line  Category        Event
                //----  --------        -----
                { L_,   CATEGORIES[0],  EVENTS[0], },
                { L_,   CATEGORIES[1],  EVENTS[0], },
                { L_,   CATEGORIES[2],  EVENTS[0], },
                { L_,   CATEGORIES[0],  EVENTS[1], },
                { L_,   CATEGORIES[1],  EVENTS[1], },
                { L_,   CATEGORIES[2],  EVENTS[1], },
                { L_,   CATEGORIES[0],  EVENTS[2], },
                { L_,   CATEGORIES[1],  EVENTS[2], },
                { L_,   CATEGORIES[2],  EVENTS[2], },
            };
            enum { NUM_NOTIFY_THREADS = sizeof NOTIFY_THREADS
                                      / sizeof *NOTIFY_THREADS };

            bool              stop = false;    // "stop threads" indicator
            Case2NotifyParams params[NUM_NOTIFY_THREADS];

            for (int i = 0; i < NUM_NOTIFY_THREADS; ++i) {
                const int LINE     = NOTIFY_THREADS[i].d_line;
                CATEGORY  category = NOTIFY_THREADS[i].d_category;
                EVENT     event    = NOTIFY_THREADS[i].d_event;

                params[i].d_registry_p = &mX;
                params[i].d_category_p = &NOTIFY_THREADS[i].d_category;
                params[i].d_event_p    = &NOTIFY_THREADS[i].d_event;
                params[i].d_stop_p     = &stop;

                bcemt_ThreadUtil::Handle *handle = &NOTIFY_THREADS[i].d_handle;
                LOOP2_ASSERT(i, LINE, 0 == bcemt_ThreadUtil::create(
                                       handle, case2NotifyThread, &params[i]));

                if (verbose) {
                    MTCOUT << "i = "        << i        << ", "
                           << "LINE = "     << LINE     << ", "
                           << "Spawn notify thread: "
// TBD - Windows           << "HANDLE = "   << *handle  << ", "
                           << "CATEGORY = " << category << ", "
                           << "EVENT = "    << event
                           << MTENDL;
                }
            }

            //-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
            // Register callbacks.

            struct {
                int           d_line;        // source line number
                CATEGORY      d_category;    // callback category
                CALLBACK_ID   d_callbackId;  // callback ID
                Obj::Callback d_callback;    // callback functor
            } CALLBACKS[] = {
                //Line  Category        ID
                //----  --------        --
                { L_,   CATEGORIES[0],  0, },
                { L_,   CATEGORIES[0],  1, },
                { L_,   CATEGORIES[0],  2, },
                { L_,   CATEGORIES[1],  0, },
                { L_,   CATEGORIES[1],  1, },
                { L_,   CATEGORIES[1],  2, },
                { L_,   CATEGORIES[2],  0, },
                { L_,   CATEGORIES[2],  1, },
                { L_,   CATEGORIES[2],  2, },
            };
            enum { NUM_CALLBACKS = sizeof CALLBACKS / sizeof *CALLBACKS };

            my_Results results[NUM_CATEGORIES];

            for (int i = 0; i < NUM_CALLBACKS; ++i) {
                const int   LINE     = CALLBACKS[i].d_line;
                CATEGORY    category = CALLBACKS[i].d_category;
                CALLBACK_ID id       = CALLBACKS[i].d_callbackId;

                Obj::Callback& cb = CALLBACKS[i].d_callback;
                cb = bdef_BindUtil::bindA( &ta
                                         , &case2Cb
                                         , _1
                                         , CALLBACKS[i].d_category
                                         , &results[id]);

                LOOP2_ASSERT(i, LINE, 0 == mX.registerCallback(category,
                                                               cb, id));
                if (verbose) {
                    MTCOUT << "i = "           << i        << ", "
                           << "LINE = "        << LINE     << ", "
                           << "Register Callback: "
                           << "CATEGORY = "    << category << ", "
                           << "CALLBACK_ID = " << id
                           << MTENDL;
                }
                bcemt_ThreadUtil::microSleep(500000, 0);  // 0.5 seconds
            }
            ASSERT(NUM_CATEGORIES == X.totalNumCategories());

            //-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
            // Stop threads.

            stop = true;
            for (int i = 0; i < NUM_NOTIFY_THREADS; ++i) {
                const int LINE = NOTIFY_THREADS[i].d_line;

                bcemt_ThreadUtil::Handle& handle = NOTIFY_THREADS[i].d_handle;

                LOOP2_ASSERT(i, LINE, 0 == bcemt_ThreadUtil::join(handle));
            }

            //-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
            // Verify results.

            bsl::ostringstream oss;
            for (int i = 0; i < NUM_CATEGORIES; ++i) {
                for (int j = 0; j < NUM_CATEGORIES; ++j) {
                    oss << CATEGORIES[i] << ' ' << EVENTS[j] << endl;
                }
            }

            for (int i = 0; i < NUM_CATEGORIES; ++i) {
                bsl::sort(results[i].d_string.begin(),
                          results[i].d_string.end());

                if (verbose) {
                    MTCOUT << "Verify: "
                           << "CALLBACK_ID = " << i
                           << MTENDL;
                }

                LOOP_ASSERT(i, bsl::string::npos !=
                                 results[i].d_string.find_first_of(oss.str()));
            }
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      }  break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   Exercise the basic functionality of the 'bcecs_CallbackRegistry'
        //   class.  We want to ensure that registry objects can be
        //   instantiated and destroyed.  We also want to exercise the primary
        //   manipulators and accessors.
        //
        // Plan:
        //   Iterate over ..., and for each, create a
        //   modifiable 'bcecs_CallbackRegistry' object 'mX', and a
        //   non-modifiable reference to 'mX' named 'X'.
        //
        // Testing:
        //   Exercise basic functionality.
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "BREATHING TEST" << endl
                 << "==============" << endl;
        }

        typedef bdet_TimeInterval EVENT;
        typedef bsl::string       CATEGORY;
        typedef int               CALLBACK_ID;
        typedef bcecs_CallbackRegistry<EVENT, CATEGORY, CALLBACK_ID> Obj;

        bcema_TestAllocator ta(veryVeryVerbose);
        {
            Obj mX(&ta);  const Obj& X = mX;

            const bsl::string CATEGORY_ONE("First Category");
            const bsl::string CATEGORY_TWO("Second Category");
            const int         CALLBACK_ONE = 0xc01d50da;
            const int         CALLBACK_TWO = 0xd099f00d;

            bsl::string       result;
            bdet_TimeInterval offset(1234, 4321);

            Obj::Callback     identityCb =
                bdef_BindUtil::bindA(&ta, &case1IdentityCb, _1, &result);

            Obj::Callback     offsetCb =
                bdef_BindUtil::bindA(&ta, &case1OffsetCb, _1, offset, &result);

            ASSERT(0  == X.totalNumCallbacks());
            ASSERT(0  == X.totalNumCategories());
            ASSERT(-1 == X.numCategories(CALLBACK_ONE));
            ASSERT(-1 == X.numCategories(CALLBACK_TWO));
            ASSERT(-1 == X.numCallbacks(CATEGORY_ONE));
            ASSERT(-1 == X.numCallbacks(CATEGORY_TWO));

            bdet_TimeInterval t1(1066, 123456789);
            bdet_TimeInterval t2(4321, 1234);

            ASSERT(0 == mX.registerCallback(CATEGORY_ONE,
                                            identityCb, CALLBACK_ONE));
            ASSERT(1  == X.totalNumCallbacks());
            ASSERT(1  == X.totalNumCategories());
            ASSERT(1  == X.numCategories(CALLBACK_ONE));
            ASSERT(-1 == X.numCategories(CALLBACK_TWO));
            ASSERT(1  == X.numCallbacks(CATEGORY_ONE));
            ASSERT(-1 == X.numCallbacks(CATEGORY_TWO));

            if (verbose) {
                cout << "Notify '" << CATEGORY_ONE << "' " << t1 << endl;
            }
            mX.notify(CATEGORY_ONE, t1);
            if (veryVerbose) {
                T_(); P(result);
            }
            ASSERT("(1066, 123456789) " == result);

            if (verbose) {
                cout << "Notify '" << CATEGORY_ONE << "' " << t2 << endl;
            }
            mX.notify(CATEGORY_ONE, t2);
            if (veryVerbose) {
                T_(); P(result);
            }
            ASSERT("(1066, 123456789) (4321, 1234) " == result);

            ASSERT(0 == mX.registerCallback(CATEGORY_TWO,
                                            offsetCb, CALLBACK_TWO));
            ASSERT(2 == X.totalNumCallbacks());
            ASSERT(2 == X.totalNumCategories());
            ASSERT(1 == X.numCategories(CALLBACK_ONE));
            ASSERT(1 == X.numCategories(CALLBACK_TWO));
            ASSERT(1 == X.numCallbacks(CATEGORY_ONE));
            ASSERT(1 == X.numCallbacks(CATEGORY_TWO));

            if (verbose) {
                cout << "Notify '" << CATEGORY_TWO << "' " << t2 << endl;
            }
            mX.notify(CATEGORY_TWO, t2);
            if (veryVerbose) {
                T_(); P(result);
            }
            ASSERT("(1066, 123456789) (4321, 1234) (5555, 5555) " == result);
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      }  break;
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
