// bdesu_pathutil.t.cpp -*-C++-*-
#include <bdesu_pathutil.h>

#include <bsls_platform.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

static int testStatus = 0;
static int verbose, veryVerbose, veryVeryVerbose;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
                    << J << "\t" \
                    << #K << ": " << K <<  "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                       SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_()  cout << "\t" << flush;          // Print tab w/o newline

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
#ifdef BSLS_PLATFORM__OS_WINDOWS
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

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    switch(test) { case 0:
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
#ifdef BSLS_PLATFORM__OS_WINDOWS
          int rootEnd = bdesu_PathUtil::getRootEnd("\\\\machine\\dir\\");
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
#ifdef BSLS_PLATFORM__OS_WINDOWS
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
#ifdef BSLS_PLATFORM__OS_WINDOWS
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
            bdesu_PathUtil::appendRaw(&leaf, "hello");
            ASSERT(leaf == string(arrays[i][LEAF_EXPECTED]));

            string leafless(arrays[i][LEAFLESS_ORIGINAL]);
            bdesu_PathUtil::appendRaw(&leafless, "hello");
            LOOP3_ASSERT(
                   arrays[i][LEAFLESS_ORIGINAL],
                   arrays[i][LEAFLESS_EXPECTED],
                   leafless, leafless == string(arrays[i][LEAFLESS_EXPECTED]));
         }

         string leafless2;
#ifdef BSLS_PLATFORM__OS_WINDOWS
         leafless2 = "\\\\.\\pipe";
#else
         leafless2 = "/bb/data/tmp/"; // not really leafless but let's do
                                     // SOMEthing on unix for this part
#endif
         ASSERT( 0 == bdesu_PathUtil::appendIfValid(&leafless2, "hello") );
#ifdef BSLS_PLATFORM__OS_WINDOWS
         LOOP_ASSERT(leafless2, "\\\\.\\pipe\\hello" == leafless2);
#else
         LOOP_ASSERT(leafless2, "/bb/data/tmp/hello" == leafless2);
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
         for (int i = 0; i < sizeof(parameters) / sizeof(Parameters); ++i) {
            const Parameters& pi = parameters[i];

            string iTest(pi.d_path);

            // Verify root
            bsl::string root;
            if (pi.d_isRelative) {
               ASSERT(0 != bdesu_PathUtil::getRoot(&root, iTest));
            }
            else {
               ASSERT(0 == bdesu_PathUtil::getRoot(&root, iTest));
               ASSERT(root == pi.d_root);
            }

            // Verify getLeaf, getDirName
            bsl::string basename, dirname;
            if (pi.d_numLeaves) {
               ASSERT(0 == bdesu_PathUtil::getLeaf(&basename, iTest));
               ASSERT(0 == bdesu_PathUtil::getDirname(&dirname, iTest));

               //test invariant:
               ASSERT(pi.d_leaf && pi.d_dirName);

               LOOP2_ASSERT(pi.d_line, basename, basename == pi.d_leaf);
               LOOP2_ASSERT(pi.d_line, dirname, dirname == pi.d_dirName);
            }
            else {
               ASSERT(0 != bdesu_PathUtil::getLeaf(&basename, iTest));
               ASSERT(0 != bdesu_PathUtil::getDirname(&dirname, iTest));
            }

            // Count leaves by removing them iteratively...When we're done,
            // verify the root of the path (we should not be able to change it
            // by removing leaves)
            int count;
            for (count = 0;
                 bdesu_PathUtil::hasLeaf(iTest);
                 ++count) {
               bdesu_PathUtil::popLeaf(&iTest);
            }
            LOOP2_ASSERT(pi.d_line, count, count == pi.d_numLeaves);
            ASSERT(bdesu_PathUtil::isRelative(iTest) == pi.d_isRelative);
            if (!pi.d_isRelative) {
               ASSERT(0 == bdesu_PathUtil::getRoot(&root, iTest));
               LOOP_ASSERT(root, root == pi.d_root);
            }

            for (int j = 0; j < sizeof(parameters) / sizeof(Parameters); ++j) {
              const Parameters& pj = parameters[j];

              int referenceCount;
              if (pj.d_isRelative) {
                 ASSERT(0 == bdesu_PathUtil::appendIfValid(&iTest, pj.d_path));
                 referenceCount = pj.d_numLeaves;
              }
              else {
                 ASSERT(0 != bdesu_PathUtil::appendIfValid(&iTest, pj.d_path));
                 referenceCount = 0;
              }
              for (count = 0;
                   bdesu_PathUtil::hasLeaf(iTest);
                   ++count) {
                  bdesu_PathUtil::popLeaf(&iTest);
              }
              LOOP2_ASSERT(pi.d_path, pj.d_path, count == referenceCount);
              LOOP2_ASSERT(pi.d_path, iTest,
                           bdesu_PathUtil::isRelative(iTest)==pi.d_isRelative);
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
#ifdef BSLS_PLATFORM__OS_WINDOWS
         bsl::string tempPath  = "c:\\windows\\temp";
         bsl::string otherPath = "22jan08\\log.txt";
#else
         bsl::string tempPath  = "/var/tmp";
         bsl::string otherPath = "22jan08/log.txt";
#endif

         ASSERT(false == bdesu_PathUtil::isRelative(tempPath));
         ASSERT(true  == bdesu_PathUtil::isAbsolute(tempPath));
         ASSERT(true  == bdesu_PathUtil::hasLeaf(tempPath));

         bdesu_PathUtil::appendRaw(&tempPath, "myApp");
         bdesu_PathUtil::appendRaw(&tempPath, "logs");
         ASSERT(true == bdesu_PathUtil::isRelative(otherPath));
         ASSERT(0 == bdesu_PathUtil::appendIfValid(&tempPath, otherPath));
         ASSERT(true == bdesu_PathUtil::hasLeaf(tempPath));
         bdesu_PathUtil::popLeaf(&tempPath);
         bdesu_PathUtil::appendRaw(&tempPath, "log2.txt");

#ifdef BSLS_PLATFORM__OS_WINDOWS
         ASSERT("c:\\windows\\temp\\myApp\\logs\\22jan08\\log2.txt" ==
                tempPath);
#else
         ASSERT("/var/tmp/myApp/logs/22jan08/log2.txt" ==
                tempPath);
#endif

         ASSERT(0 == bdesu_PathUtil::appendIfValid(&otherPath, otherPath));
             // OK
         /* bdesu_PathUtil::append(&otherPath, tempPath); */
             // UNDEFINED BEHAVIOR!

#ifdef BSLS_PLATFORM__OS_WINDOWS
         LOOP_ASSERT(otherPath,
                     "22jan08\\log.txt\\22jan08\\log.txt" == otherPath);
#else
         LOOP_ASSERT(otherPath,
                     "22jan08/log.txt/22jan08/log.txt" == otherPath);
#endif

         ASSERT(0 == bdesu_PathUtil::appendIfValid(&otherPath,
                                                   otherPath.c_str()));
#ifdef BSLS_PLATFORM__OS_WINDOWS
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

         ASSERT(0 == bdesu_PathUtil::appendIfValid(&otherPath,
                                                   otherPath.c_str() + 56));
#ifdef BSLS_PLATFORM__OS_WINDOWS
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
