// bdlsu_pathutil.t.cpp                                               -*-C++-*-
#include <bdlsu_pathutil.h>

#include <bsls_platform.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

static int verbose, veryVerbose, veryVeryVerbose;

// ============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------
static int testStatus = 0;
static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

// ============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
// ----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
                    << J << "\t" \
                    << #K << ": " << K <<  "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

// ============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define T_  cout << "\t" << flush;          // Print a tab (w/o newline)
#define L_ __LINE__                           // current Line number

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
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef bdlsu::PathUtil Obj;

struct Parameters {
    int d_line;
    const char* d_path;
    const char* d_root;
    const char* d_leaf;
    const char* d_dirName;
    int d_numLeaves;
    bool d_isRelative;
    bool d_isValidRaw;
} parameters[] = {
#ifdef BSLS_PLATFORM_OS_WINDOWS
   {L_, "", "", 0, 0,
         0, 1, 0},
   {L_, "\\", "\\", 0, 0,
         0, 0, 0},
   {L_, "c:", "c:", 0, 0,
         0, 0, 1},
   {L_, "c:\\", "c:\\", 0, 0,
         0, 0, 1},
   {L_, "c:\\\\", "c:\\\\", 0, 0,
         0, 0, 1},
   {L_, "c:\\one", "c:\\", "one", "c:\\",
         1, 0, 1},
   {L_, "c:\\one\\", "c:\\", "one", "c:\\",
         1, 0, 0},
   {L_, "c:\\one\\two", "c:\\", "two", "c:\\one",
         2, 0, 1},
   {L_, "c:\\one\\two\\", "c:\\", "two", "c:\\one",
         2, 0, 0},
   {L_, "c:\\\\one", "c:\\\\", "one", "c:\\\\",
         1, 0, 1},
   {L_, "c:\\\\one\\", "c:\\\\", "one", "c:\\\\",
         1, 0, 0},
   {L_, "c:\\\\one\\two", "c:\\\\", "two", "c:\\\\one",
         2, 0, 1},
   {L_, "c:\\\\one\\two\\", "c:\\\\", "two", "c:\\\\one",
         2, 0, 0},
   {L_, "c:one", "c:", "one", "c:",
         1, 0, 1},
   {L_, "one", "", "one", "",
         1, 1, 1},
   {L_, "one\\", "", "one", "",
         1, 1, 0},
   {L_, "one\\two", "", "two", "one",
         2, 1, 1},
   {L_, "one\\two\\", "", "two", "one",
         2, 1, 0},
   {L_, "one\\two\\\\", "", "two", "one",
         2, 1, 1},
   {L_, "c:one\\two\\\\", "c:", "two", "c:one",
         2, 0, 0},
   {L_, "\\\\server\\dir", "\\\\server\\dir", 0, 0,
         0, 0, 1},
   {L_, "\\\\server\\dir\\", "\\\\server\\dir\\", 0, 0,
         0, 0, 1},
   {L_, "\\\\server\\dir\\\\", "\\\\server\\dir\\\\", 0, 0,
         0, 0, 1},
   {L_, "\\\\server\\dir\\one",
        "\\\\server\\dir\\", "one", "\\\\server\\dir\\",
        1, 0, 1},
   {L_, "\\\\server\\dir\\one\\",
        "\\\\server\\dir\\", "one", "\\\\server\\dir\\",
         1, 0, 0},
   {L_, "\\\\server\\dir\\one\\two",
         "\\\\server\\dir\\", "two", "\\\\server\\dir\\one",
         2, 0, 1},
   {L_, "\\\\.\\dir\\one\\two\\", "\\\\.\\dir\\", "two", "\\\\.\\dir\\one",
         2, 0, 0},
   {L_, "\\\\server\\dir\\\\one",
        "\\\\server\\dir\\\\", "one", "\\\\server\\dir\\\\",
         1, 0, 1},
   {L_, "\\\\server\\dir\\\\one\\",
        "\\\\server\\dir\\\\", "one", "\\\\server\\dir\\\\",
         1, 0, 0},
   {L_, "\\\\server\\dir\\\\one\\two",
        "\\\\server\\dir\\\\", "two", "\\\\server\\dir\\\\one",
         2, 0, 1},
   {L_, "\\\\server\\dir\\\\one\\two\\",
        "\\\\server\\dir\\\\", "two", "\\\\server\\dir\\\\one",
         2, 0, 0},
   {L_, "\\\\server\\dir\\one\\two\\\\",
        "\\\\server\\dir\\", "two", "\\\\server\\dir\\one",
         2, 0, 0}
#else
   {L_, "", "", 0, 0,
         0, 1, 0},
   {L_, "/", "/", 0, 0,
         0, 0, 1},
   {L_, "//", "//", 0, 0,
         0, 0, 1},
   {L_, "/one", "/", "one", "/",
         1, 0, 1},
   {L_, "/one/", "/", "one", "/",
         1, 0, 0},
   {L_, "/one/two", "/", "two", "/one",
         2, 0, 1},
   {L_, "/one/two/", "/", "two", "/one",
         2, 0, 0},
   {L_, "//one", "//", "one", "//",
         1, 0, 1},
   {L_, "//one/", "//", "one", "//",
         1, 0, 0},
   {L_, "//one/two", "//", "two", "//one",
         2, 0, 1},
   {L_, "//one/two/", "//", "two", "//one",
         2, 0, 0},
   {L_, "one", "", "one", "",
         1, 1, 1},
   {L_, "one/", "", "one", "",
         1, 1, 0},
   {L_, "one/two", "", "two", "one",
         2, 1, 1},
   {L_, "one/two/", "", "two", "one",
         2, 1, 0},
   {L_, "one/two//", "", "two", "one",
         2, 1, 0}
#endif
};

void convertToWindowsSeparator(bsl::string *path)
   // Replace each occurance of '/' with '\\' in the specified 'path'.
{
    bsl::string::size_type position = path->find('/'); 
    for ( ; position != bsl::string::npos; position = path->find('/')) {
        (*path)[position] = '\\';
    }
}

void convertToUnixSeparator(bsl::string *path)
   // Replace each occurance of '\\' with '\' in the specified 'path'.
{
    bsl::string::size_type position = path->find('\\'); 
    for ( ; 
         position != bsl::string::npos; 
         position = path->find('\\', position)) {
        (*path)[position] = '/';
    }
}


int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    switch(test) { case 0:
      case 5: {
        // --------------------------------------------------------------------
        // TESTING: 'appendIfValid'
        //
        // Concerns:
        //: 1 Return an error code is the appended path is an absolute path.
        //:
        //: 2 If 'path' does not contain trialing seperators, 'filename' is
        //:   appended with the appropriate separator.
        //:
        //: 3 If 'path' does contain trialing seperators, 'filename' is
        //:   appended with only a single separator.
        //:
        //: 4 If 'filename' contains trialing separators, they are not in
        //:   appeneded to 'path'.
        //:
        //: 5 If 'path' contains only separators it is simplified to a single
        //:   separator. 
        //:
        //: 6 If 'filename' is an alias for any element in 'path', it is still
        //:   correctly appended.
        //:
        //: 7 Windows file names properly handle drive names in the absolute
        //:   path.
        //
        // Plan:
        //: 1 Create a table of test input values and expected results, and
        //:   iterate over this table verifying that 'appendIfValue' produces
        //:   the expected results.  (C-1, C-2, C-3, C-4, C-5, C-6)
        //:  
        //: 1 Iterate over a series of simple test paths, and for each
        //:   path, iterate over a series of sub-string within that path.
        //:   For each sub-string, create a bslstl::StringRef aliasing that
        //:   sub-string, create an expected result value, and verify
        //:   that 'appendIfValid' also generates that expected value (C-7).
        //
        // Testing:
        //  int appendIfValid(bsl::string *, const bslstl::StringRef& );
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "TESTING: appendIfValid" << endl
                 << "======================" << endl;
        }
        
        if (verbose) {
            cout << "\tUse table of distinct object values." << endl;
        }

        {
            struct TestData {
                    int         d_line;
                    const char *d_path;
                    const char *d_filename;
                    bool        d_expectSuccess;
                    const char *d_expectedResult;
            } VALUES [] = {
                { L_,  "" , "" , true, ""    },
                { L_,  "a", "" , true, "a"   },
                { L_,  "" , "a", true, "a"   },
                { L_,  "a", "b", true, "a/b" },

                { L_,  "a/" , "b"   , true,  "a/b" },
                { L_,  "a//", "b"   , true,  "a/b" },
                { L_,  "a"  , "b/"  , true,  "a/b" },
                { L_,  "a"  , "b//" , true,  "a/b" },
                { L_,  "a//", "b//" , true,  "a/b" },
                { L_,  "a//", "/b//", false, "" },

                { L_,  "/a/" , "b"   , true , "/a/b" },
                { L_,  "/a//", "b"   , true , "/a/b" },
                { L_,  "/a"  , "b/"  , true , "/a/b" },
                { L_,  "/a"  , "b//" , true , "/a/b" },
                { L_,  "/a//", "b//" , true , "/a/b" },
                { L_,  "/a//", "/b//", false, "" },


                { L_,  "/"     , "b" , true, "/b" },
                { L_,  "//////", "b" , true, "/b" },
                { L_,  "//////", "b/", true, "/b" },

                { L_,  "/a/b/c/" , "e/f/g"   , true,  "/a/b/c/e/f/g" },
                { L_,  "/a/b/c//", "e/f/g"   , true,  "/a/b/c/e/f/g" },
                { L_,  "/a/b/c"  , "e/f/g/"  , true,  "/a/b/c/e/f/g" },
                { L_,  "/a/b/c"  , "e/f/g//" , true,  "/a/b/c/e/f/g" },
                { L_,  "/a/b/c//", "e/f/g//" , true,  "/a/b/c/e/f/g" },
                { L_,  "/a/b/c//", "/e/f/g//", false, "" },


#ifdef BSLS_PLATFORM__OS_WINDOWS
                // Test a path starting with a drive letter.
                { L_,  "z:"    , "b" , true ,  "z:/b" },
                { L_,  "z:/"   , "b" , true ,  "z:/b" },
                { L_,  "z://"  , "b" , true ,  "z:/b" },
                { L_,  "z://"  , "b/", true ,  "z:/b" },
                { L_,  "z:/a"  , "b" , true ,  "z:/a/b" },
                { L_,  "z:/a/" , "b" , true ,  "z:/a/b" },
                { L_,  "z:/a//", "b" , true ,  "z:/a/b" },
                { L_,  "z:/"   , "/b", false,  "z:/b" },

                // Test UNC paths
                { L_,  "//UNC"    , "b" , true ,  "//UNC/b" },
                { L_,  "//UNC/"   , "b" , true ,  "//UNC/b" },
                { L_,  "//UNC//"  , "b" , true ,  "//UNC/b" },
                { L_,  "//UNC//"  , "b/", true ,  "//UNC/b" },
                { L_,  "//UNC/a"  , "b" , true ,  "//UNC/a/b" },
                { L_,  "//UNC/a/" , "b" , true ,  "//UNC/a/b" },
                { L_,  "//UNC/a//", "b" , true ,  "//UNC/a/b" },
                { L_,  "//UNC/"   , "/b", false,  "//UNC/b" },

                // Test device paths
                { L_,  "//?/"   , "b" , true ,  "//?/b" },
                { L_,  "//?//"  , "b" , true ,  "//?/b" },
                { L_,  "//?//"  , "b/", true ,  "//?/b" },
                { L_,  "//?/a"  , "b" , true ,  "//?/a/b" },
                { L_,  "//?/a/" , "b" , true ,  "//?/a/b" },
                { L_,  "//?/a//", "b" , true ,  "//?/a/b" },
                { L_,  "//?/"   , "/b", false,  "//?/b" },                
#endif
            
            };
            const int NUM_VALUES = sizeof(VALUES) / sizeof(*VALUES);

            for (int i = 0; i < NUM_VALUES; ++i) {
                bsl::string path(VALUES[i].d_path);
                bsl::string filename(VALUES[i].d_filename);
                bsl::string expectedResult(VALUES[i].d_expectedResult);

                const int  LINE    = VALUES[i].d_line;
                const bool success = VALUES[i].d_expectSuccess;


#ifdef BSLS_PLATFORM__OS_WINDOWS
                convertToWindowsSeparator(&path);
                convertToWindowsSeparator(&filename);
                convertToWindowsSeparator(&expectedResult);
#endif

                bsl::string originalPath(path);

                int rc = Obj::appendIfValid(&path, filename);

                if (veryVeryVerbose) {
                    P_(LINE); P_(originalPath); P_(filename);
                    P_(path); P(expectedResult);
                }
            
                if (!success) {
                    LOOP_ASSERT(LINE, 0 != rc);
                    continue;
                }

                LOOP_ASSERT(LINE, 0 == rc);
                LOOP4_ASSERT(LINE, originalPath, filename, path, 
                             expectedResult == path);
            }
        }
        if (verbose) {
            cout << "\tTest for aliasing." << endl;
        }

        {
            const char *VALUES[] = {
                "",
                "a",
                "abc",
                "thisisalongpathnamexxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
            };
            const int NUM_VALUES = sizeof(VALUES) / sizeof(*VALUES);

            for (int i = 0; i < NUM_VALUES; ++i) {
                int pathLen = bsl::strlen(VALUES[i]);

                for (int subStrLen = 0; subStrLen < pathLen; ++subStrLen) {
                    for (int offset=0; offset<pathLen-subStrLen+1; ++offset){
                        
                        bsl::string path(VALUES[i]);
                        bslstl::StringRef filename(path.c_str() + offset,
                                                 subStrLen);

                        if (filename.length() > 0 && '/' == filename[0]) {
                            continue;
                        }

                        bsl::string expectedResult(path);
                        if (path.size() > 0 && filename.length() > 0) {
                            expectedResult += "/";                            
                        }
                        expectedResult+=
                            bsl::string(filename.data(), filename.length());

#ifdef BSLS_PLATFORM__OS_WINDOWS
                        convertToWindowsSeparator(&path);
                        convertToWindowsSeparator(&expectedResult);
#endif
                        bsl::string originalPath(path);
                        bsl::string originalFilename(filename.data(),
                                                     filename.length());

                        int rc = Obj::appendIfValid(&path, filename);

                        if (veryVeryVerbose) {
                            P_(originalPath); P(originalFilename);
                            P_(path); P(expectedResult);
                        }

                        ASSERT(0 == rc);
                        LOOP4_ASSERT(originalPath, 
                                     originalFilename, 
                                     path, 
                                     expectedResult,
                                     expectedResult == path);
                    }
                }
            }

        }
        
      } break;
      case 4: {
        /////////////////////////////////////////////////////////////
        // Special path parsing test
        //
        // Concern: "root-end" determination for certain paths (WIN)
        // Plan: Using the getRootEnd method, check parsing correctness
        //       for a few specific paths which caused issues during
        //       development
        /////////////////////////////////////////////////////////////

        if (verbose) {
            cout << "Special path parsing test" << endl;
        }
#ifdef BSLS_PLATFORM_OS_WINDOWS
        int rootEnd = bdlsu::PathUtil::getRootEnd("\\\\machine\\dir\\");
        LOOP_ASSERT(rootEnd, 14 == rootEnd);
#endif
      } break;
      case 3: {
        /////////////////////////////////////////////////////////////
        // Leafless append test
        //
        // Concern: Appending to the end of a path will not add a
        // separator iff there are no leaves.
        /////////////////////////////////////////////////////////////

        if (verbose) {
           cout << "Leafless Append Test" << endl;
        }
        enum {
           LEAF_ORIGINAL = 0,
           LEAF_EXPECTED = 1,
           LEAFLESS_ORIGINAL = 2,
           LEAFLESS_EXPECTED = 3
        };
        const char *absolute[] = {
#ifdef BSLS_PLATFORM_OS_WINDOWS
            "\\\\machine\\dir\\logs",
            "\\\\machine\\dir\\logs\\hello",
            "\\\\machine\\dir\\",
            "\\\\machine\\dir\\hello"
#else
            "/logs",
            "/logs/hello",
            "/",
            "/hello"
#endif
        };
        const char *relative[] = {
#ifdef BSLS_PLATFORM_OS_WINDOWS
            "logs",
            "logs\\hello",
            "",
            "hello",
#else
            "logs",
            "logs/hello",
            "",
            "hello"
#endif
        };
        const char** arrays[] = {absolute, relative};
        for (int i = 0; i < 2; ++i) {
           string leaf(arrays[i][LEAF_ORIGINAL]);
           bdlsu::PathUtil::appendRaw(&leaf, "hello");
           ASSERT(leaf == string(arrays[i][LEAF_EXPECTED]));

           string leafless(arrays[i][LEAFLESS_ORIGINAL]);
           bdlsu::PathUtil::appendRaw(&leafless, "hello");
           LOOP3_ASSERT(
                  arrays[i][LEAFLESS_ORIGINAL],
                  arrays[i][LEAFLESS_EXPECTED],
                  leafless, leafless == string(arrays[i][LEAFLESS_EXPECTED]));
        }

        string leafless2;
#ifdef BSLS_PLATFORM_OS_WINDOWS
        leafless2 = "\\\\.\\pipe";
#else
        leafless2 = "/var/tmp/"; // not really leafless but let's do
                                 // SOMEthing on unix for this part
#endif
        ASSERT( 0 == bdlsu::PathUtil::appendIfValid(&leafless2, "hello") );
#ifdef BSLS_PLATFORM_OS_WINDOWS
        LOOP_ASSERT(leafless2, "\\\\.\\pipe\\hello" == leafless2);
#else
        LOOP_ASSERT(leafless2, "/var/tmp/hello" == leafless2);
#endif
      } break;
      case 2: {
        ////////////////////////////////////////////////////////////
        // Native Parsing Test
        //
        // Concerns: Functionality of native path parsing logic
        //
        // Plan: Construct objects from various native paths and verify
        // properties.  Also verify the parser by invoking it through
        // the appendIfValid code path in addition to the assignment path.
        // Also verify correct parsing of the root.
        ////////////////////////////////////////////////////////////

        if (verbose) {
            cout << "Native Parsing Test" << endl;
        }
        const int NUM_PARAMETERS = sizeof(parameters) / sizeof(Parameters);
        for (int i = 0; i < NUM_PARAMETERS; ++i) {
            const Parameters& pi = parameters[i];

            string iTest(pi.d_path);

            // Verify root

            bsl::string root;
            if (pi.d_isRelative) {
                ASSERT(0 != bdlsu::PathUtil::getRoot(&root, iTest));
            }
            else {
                ASSERT(0 == bdlsu::PathUtil::getRoot(&root, iTest));
                ASSERT(root == pi.d_root);
            }

            // Verify getLeaf, getDirName

            bsl::string basename, dirname;
            if (pi.d_numLeaves) {
                ASSERT(0 == bdlsu::PathUtil::getLeaf(&basename, iTest));
                ASSERT(0 == bdlsu::PathUtil::getDirname(&dirname, iTest));

                //test invariant:

                ASSERT(pi.d_leaf && pi.d_dirName);

                LOOP2_ASSERT(pi.d_line, basename, basename == pi.d_leaf);
                LOOP2_ASSERT(pi.d_line, dirname, dirname == pi.d_dirName);
            }
            else {
                ASSERT(0 != bdlsu::PathUtil::getLeaf(&basename, iTest));
                ASSERT(0 != bdlsu::PathUtil::getDirname(&dirname, iTest));
            }

            // Count leaves by removing them iteratively...When we're done,
            // verify the root of the path (we should not be able to change it
            // by removing leaves)
 
            int count;
            for (count = 0;
                 bdlsu::PathUtil::hasLeaf(iTest);
                 ++count) {
                bdlsu::PathUtil::popLeaf(&iTest);
            }
            LOOP2_ASSERT(pi.d_line, count, count == pi.d_numLeaves);
            ASSERT(bdlsu::PathUtil::isRelative(iTest) == pi.d_isRelative);
            if (!pi.d_isRelative) {
                ASSERT(0 == bdlsu::PathUtil::getRoot(&root, iTest));
                LOOP_ASSERT(root, root == pi.d_root);
            }

            for (int j = 0; j < NUM_PARAMETERS; ++j) {
                const Parameters& pj = parameters[j];

                int referenceCount;
                if (pj.d_isRelative) {
                    ASSERT(0 ==
                             bdlsu::PathUtil::appendIfValid(&iTest, pj.d_path));
                    referenceCount = pj.d_numLeaves;
                }
                else {
                    ASSERT(0 !=
                             bdlsu::PathUtil::appendIfValid(&iTest, pj.d_path));
                    referenceCount = 0;
                }
                for (count = 0;
                     bdlsu::PathUtil::hasLeaf(iTest);
                     ++count) {
                    bdlsu::PathUtil::popLeaf(&iTest);
                }
                LOOP2_ASSERT(pi.d_path, pj.d_path, count == referenceCount);
                LOOP2_ASSERT(pi.d_path, iTest,
                           bdlsu::PathUtil::isRelative(iTest)==pi.d_isRelative);
            }
        }
      } break;
      case 1: {
        ////////////////////////////////////////////////////////////
        // Usage Example Test
        //
        // Concern: The component's Usage Example compiles and runs
        ////////////////////////////////////////////////////////////
        
        if (verbose) {
           cout << "Usage Example Test" << endl;
        }
#ifdef BSLS_PLATFORM_OS_WINDOWS
        bsl::string tempPath  = "c:\\windows\\temp";
        bsl::string otherPath = "22jan08\\log.txt";
#else
        bsl::string tempPath  = "/var/tmp";
        bsl::string otherPath = "22jan08/log.txt";
#endif

        ASSERT(false == bdlsu::PathUtil::isRelative(tempPath));
        ASSERT(true  == bdlsu::PathUtil::isAbsolute(tempPath));
        ASSERT(true  == bdlsu::PathUtil::hasLeaf(tempPath));
        
        bdlsu::PathUtil::appendRaw(&tempPath, "myApp");
        bdlsu::PathUtil::appendRaw(&tempPath, "logs");
        ASSERT(true == bdlsu::PathUtil::isRelative(otherPath));
        ASSERT(0 == bdlsu::PathUtil::appendIfValid(&tempPath, otherPath));
        ASSERT(true == bdlsu::PathUtil::hasLeaf(tempPath));
        bdlsu::PathUtil::popLeaf(&tempPath);
        bdlsu::PathUtil::appendRaw(&tempPath, "log2.txt");

#ifdef BSLS_PLATFORM_OS_WINDOWS
        ASSERT("c:\\windows\\temp\\myApp\\logs\\22jan08\\log2.txt" ==
                tempPath);
#else
        ASSERT("/var/tmp/myApp/logs/22jan08/log2.txt" ==
                tempPath);
#endif

        ASSERT(0 == bdlsu::PathUtil::appendIfValid(&otherPath, otherPath));

        // OK
        // bdlsu::PathUtil::append(&otherPath, tempPath);
        // UNDEFINED BEHAVIOR!

#ifdef BSLS_PLATFORM_OS_WINDOWS
        LOOP_ASSERT(otherPath,
                     "22jan08\\log.txt\\22jan08\\log.txt" == otherPath);
#else
        LOOP_ASSERT(otherPath,
                     "22jan08/log.txt/22jan08/log.txt" == otherPath);
#endif

        ASSERT(0 == bdlsu::PathUtil::appendIfValid(&otherPath,
                                                   otherPath.c_str()));
#ifdef BSLS_PLATFORM_OS_WINDOWS
        LOOP_ASSERT(otherPath,
                   "22jan08\\log.txt\\"
                   "22jan08\\log.txt\\"
                   "22jan08\\log.txt\\"
                   "22jan08\\log.txt" == otherPath);
#else
        LOOP_ASSERT(otherPath,
                   "22jan08/log.txt/"
                   "22jan08/log.txt/"
                   "22jan08/log.txt/"
                   "22jan08/log.txt" == otherPath);
#endif

        ASSERT(0 == bdlsu::PathUtil::appendIfValid(&otherPath,
                                                   otherPath.c_str() + 56));
#ifdef BSLS_PLATFORM_OS_WINDOWS
        LOOP_ASSERT(otherPath,
                    "22jan08\\log.txt\\"
                    "22jan08\\log.txt\\"
                    "22jan08\\log.txt\\"
                    "22jan08\\log.txt\\log.txt" == otherPath);
#else
        LOOP_ASSERT(otherPath,
                    "22jan08/log.txt/"
                    "22jan08/log.txt/"
                    "22jan08/log.txt/"
                    "22jan08/log.txt/log.txt" == otherPath);
#endif


      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "."
             << endl;
    }
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
