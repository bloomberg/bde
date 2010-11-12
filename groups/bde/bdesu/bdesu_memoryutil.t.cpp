// bdesu_memoryutil.t.cpp -*-C++-*-

#include <bdesu_memoryutil.h>

// TBD: this needs to test setting memory to executable

#include <bsls_platform.h>
#include <bsls_platformutil.h>

#include <bsl_iostream.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

#ifdef BSLS_PLATFORM__OS_WINDOWS
#include <windows.h>
#endif
#ifdef BSLS_PLATFORM__OS_UNIX
#include <sys/resource.h>
#endif

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

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
                    << J << "\t" << #K << ": " << K << "\t" << #L << ": " \
                    << L << "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                       SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_()  cout << "\t" << flush;          // Print tab w/o newline

//=============================================================================
//                               MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
     int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
     verbose = argc > 2;
     veryVerbose = argc > 3;
     veryVeryVerbose = argc > 4;

#ifdef BSLS_PLATFORM__OS_WINDOWS
          // disable popup on crash
          SetErrorMode(SEM_NOGPFAULTERRORBOX);
#endif
#ifdef BSLS_PLATFORM__OS_UNIX
    // disable core dumps
    {
        struct rlimit rl;
        rl.rlim_cur = rl.rlim_max = 0;
        setrlimit(RLIMIT_CORE, &rl);
    }
#endif

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        //----------------------------------------------------------------
        // Concern: functionality of protect()
        //
        // Test plan: enumerate all possible access mode combinations and
        // try reading, writing and executing the memory with these
        // protection modes.  Verify the actual protection modes match the
        // specified ones.
        //----------------------------------------------------------------

        // this test expects BDESU_ACCESS_READ==1, BDESU_ACCESS_WRITE==2,
        // BDESU_ACCESS_EXECUTE==4
        ASSERT(bdesu_MemoryUtil::BDESU_ACCESS_READ == 1);
        ASSERT(bdesu_MemoryUtil::BDESU_ACCESS_WRITE == 2);
        ASSERT(bdesu_MemoryUtil::BDESU_ACCESS_EXECUTE == 4);

        static const char*const operations[] = { "read", "write" };
        static const char*const modes[] = {
            "BDESU_ACCESS_NONE", "BDESU_ACCESS_READ",
            "BDESU_ACCESS_WRITE", "BDESU_ACCESS_READ_WRITE",
            "BDESU_ACCESS_EXECUTE",
            "BDESU_ACCESS_READ_EXECUTE", "BDESU_ACCESS_WRITE_EXECUTE",
            "BDESU_ACCESS_READ_WRITE_EXECUTE"
        };

        // test all 8 modes
        for (int mode=0; mode<sizeof(modes)/sizeof(*modes); ++mode) {
            // do not try to set executable bit when on HP-UX
#ifdef BSLS_PLATFORM__OS_HPUX
            if (mode & bdesu_MemoryUtil::BDESU_ACCESS_EXECUTE) {
                continue;
            }
#endif
            // test read & write
            for (int op=0;
                op<sizeof(operations)/sizeof(*operations);
                ++op)
            {
                if (op == 0 && mode != bdesu_MemoryUtil::BDESU_ACCESS_NONE
                    && !(mode & bdesu_MemoryUtil::BDESU_ACCESS_READ))
                {
                    // do not test disabled read with write/execute
                    // allowed: most platforms do not have fine-grained
                    // read access control
                    if (verbose) cout << "Skipping op:" << operations[op]
                                      << ", mode:" << modes[mode] << bsl::endl;
                    continue;
                }
                int expected_rc = !(mode & (1<<op));
                if (verbose) {
                    cout << "Testing op:" << operations[op]
                         << ", mode:" << modes[mode]
                         << ", expected to "
                         << ( expected_rc ? "fail" : "succeed" )
                         << bsl::endl;
                }
                enum {
                    ARBITRARY_BUT_SUFFICIENT_BUFFER_SIZE = 1000
                };
                char buffer[ARBITRARY_BUT_SUFFICIENT_BUFFER_SIZE];
#ifdef BSLS_PLATFORM__OS_WINDOWS
		const char* redirectToNull = " >NUL 2>&1";
#else
		const char* redirectToNull = " >/dev/null 2>&1";
#endif
                sprintf(buffer, "%s -1 %s %d %d%s",
                                             argv[0], argv[0], -10-op, mode,
                                             verbose ? "" : redirectToNull);
                int rc = system(buffer);
                LOOP4_ASSERT(modes[mode], operations[op], rc, expected_rc,
                             !rc == !expected_rc);
            }
        }
      } break;
      case 1: {
        ///USAGE EXAMPLE
        ///-------------
        //
        // First, allocate one page of memory.

        int pageSize = bdesu_MemoryUtil::pageSize();
        char* data = (char*)bdesu_MemoryUtil::allocate(pageSize);

        // Write into the allocated buffer.
        data[0] = 1;

        // Make the memory write protected

        bdesu_MemoryUtil::protect(data, pageSize,
                                  bdesu_MemoryUtil::BDESU_ACCESS_READ);

        // Once again, try writing into the buffer.  This should crash our
        // process.

        // data[0] = 2;

        // Restore read/write access and free the allocated memory.
        // Actually, this will never be executed, as the process has already
        // crashed.

        bdesu_MemoryUtil::protect(data, pageSize,
                                  bdesu_MemoryUtil::BDESU_ACCESS_READ_WRITE);
        bdesu_MemoryUtil::deallocate(data);
      } break;
      case -1: {
        //--------------------------------------------------------------
        // Helper test case for case 2
        //
        // This case implements the system command, it allows case 2
        // to call system within system, and thereby make abort
        // messages redirectable.
        //--------------------------------------------------------------

        char buffer[1000];
        buffer[0] = 0;

        for (int i = 2; i < argc; ++i) {
            strcat(buffer, " ");
            strcat(buffer, argv[i]);
        }

        cout << "Within system: " << buffer << endl;

        return !!system(buffer);
      } break;
      case -10: {
        //--------------------------------------------------------------
        // Helper test case for case 2
        //
        // Plan: allocate some memory with protection mode specified in
        // argv[2] and verify it is readable.  Note that it is normal for
        // this test case to fail for some values of argv[2].
        //--------------------------------------------------------------
        int size = bdesu_MemoryUtil::pageSize();
        char* ptr = (char*) bdesu_MemoryUtil::allocate(size);
        memset(ptr, 0x55, size);
        int rc = bdesu_MemoryUtil::protect(ptr, size, atoi(argv[2]));
        ASSERT(0 == rc);
        for(int i=0; i<size; ++i) {
            ASSERT(((volatile char*)ptr)[i] == 0x55);
        }
        rc = bdesu_MemoryUtil::protect(ptr, size,
                                    bdesu_MemoryUtil::BDESU_ACCESS_READ_WRITE);
        ASSERT(0 == rc);
        rc = bdesu_MemoryUtil::deallocate(ptr);
        ASSERT(0 == rc);
      } break;
      case -11: {
        //--------------------------------------------------------------
        // Helper test case for case 2
        //
        // Plan: allocate some memory with protection mode specified in
        // argv[2] and verify it is writable.  Note that it is normal for
        // this test case to fail for some values of argv[2].
        //--------------------------------------------------------------
        int size = bdesu_MemoryUtil::pageSize();
        char* ptr = (char*) bdesu_MemoryUtil::allocate(size);
        int rc = bdesu_MemoryUtil::protect(ptr, size, atoi(argv[2]));
        ASSERT(0 == rc);
        for(int i=0; i<size; ++i) {
            ((volatile char*)ptr)[i] = 0x55;
        }
        rc = bdesu_MemoryUtil::protect(ptr, size,
                                    bdesu_MemoryUtil::BDESU_ACCESS_READ_WRITE);
        ASSERT(0 == rc);
        rc = bdesu_MemoryUtil::deallocate(ptr);
        ASSERT(0 == rc);
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
