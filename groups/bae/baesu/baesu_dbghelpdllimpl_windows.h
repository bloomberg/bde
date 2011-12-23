// baesu_dbghelpdllimpl_windows.h                                     -*-C++-*-
#ifndef INCLUDED_BAESU_DBGHELPDLLIMPL_WINDOWS
#define INCLUDED_BAESU_DBGHELPDLLIMPL_WINDOWS

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide access to the 'dbghelp.dll' shared library on Windows.
//
//@CLASSES:
//   baesu_DbghelpDllImpl_Windows: interface to 'dbghelp.dll' shared library
//
//@AUTHOR: Bill Chapman (bchapman2), Steven Breitstein (sbreitstein)
//
// This component provides a single class, 'baesu_DbghelpDllImpl_Windows',
// which provides Windows platform-specific facilities in support of the
// 'baesu' stack trace facility.  This component is *not* intended for public
// use.
//
// The 'baesu_DbghelpDllImpl_Windows' class:
//
//: 1 Provides a suite of static methods which wrap calls to several functions
//:   of 'dbghelp.dll', a Windows shared library.
//:
//: 2 Loads (once) the '.dll' on the first call of any of its methods that call
//:   functions in that '.dll'.
//:
//: 3 On process termination, cleans up data structures, and unloads the
//:   '.dll'.
//
// The mapping of class' static methods to '.dll' functions is:
//..
//  baesu_DbghelpDllImpl_Windows  dbghelp.dll
//  ----------------------------  -----------
//  symSetOptions                 SymSetOptions
//  symFromAddr                   SymFromAddr
//  symGetSymFromAddr64           SymGetSymFromAddr64
//  symGetLineFromAddr64          SymGetLineFromAddr64
//  stackWalk64                   StackWalk64
//..
// The signatures of these static methods typically have fewer parameters than
// those of the '.dll' functions.  Those arguments (e.g., a Windows handle to
// the current process) are provided in the implementation.  Otherwise, the
// parameters of the static methods match in order and type those of the '.dll'
// functions.
//
///Thread Safety
///-------------
// Since the functions in 'dbghelp.dll' are *not* thread-safe, so this class
// provides a static mutex (of type 'bcemt_QLock') which must be acquired
// before any of the '.dll'-invoking methods of this function are called.
//
// !TBD: Needed in a single threaded process?!
// !TBD: Is this library shared system wide?
//       If so, don't we need a IPC lock?!
// !TBD: Once acquired, how long is it held?
//       Released after each static method call?
//       Released after the results have been copied?
//       After a series of method calls?!
//
///Usage
///-----
// TBD

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifdef BSLS_PLATFORM__OS_WINDOWS

#ifndef INCLUDED_BCEMT_QLOCK
#include <bcemt_qlock.h>
#endif

#ifndef INCLUDED_WINDOWS
#include <windows.h>
#define INCLUDED_WINDOWS
#endif

#ifndef INCLUDED_INTRIN
#include <intrin.h>
#define INCLUDED_INTRIN
#endif

#ifndef INCLUDED_DBGHELP
#include <dbghelp.h>
#define INCLUDED_DBGHELP
#endif

namespace BloombergLP {

                        // ==================================
                        // class baesu_DbghelpDllImpl_Windows
                        // ==================================

class baesu_DbghelpDllImpl_Windows {
    // This class provides a namespace for static methods (defined only on
    // Windows platforms) which call call functions from the 'dbghelp.dll'
    // shared library.  The dynamic library is lazily loaded and initialized
    // when needed, the implementation arranges for automatic unloading of that
    // library at process termination.  (TBD: Say something about exactly
    // when.)  Typically, the methods of this class that call '.dll' functions
    // implicitly provide some of the needed arguments (e.g., a handle to the
    // current process); otherwise, the specified parameters of these methods
    // match in order and type those of the '.dll' functions.  See
    // '@DESCRIPTION' for further details.
    //
    // The methods of this class are *not* thread-safe.  In a multi-threaded
    // environment acquire the (static) mutex (see the 'qLock' method) before
    // invoking any other methods of this class.

    // DATA
    static bcemt_QLock s_qLock;  // mutex to synchronize access to '.dll'
                                 // functions, which are not *thread* *safe*

  public:
    // CLASS METHODS
    static bool isLoaded();
        // Return 'true' if 'dbghelp.dll' has already been successfully loaded
        // in this process, and 'false' otherwise.  For testing only.

    static bcemt_QLock& qLock();
        // Return a reference providing modifiable access to static mutex
        // provided for synchronizing access to the 'dbghelp.dll' shared
        // library.

        // *** Methods corresponding to Windows 'dbghelp.dll' functions ***

    static DWORD symSetOptions(DWORD symOptions);
        // Invoke the 'SymSetOptions' function of 'dbghelp.dll' with the
        // specified 'symOptions', and return the result.  The behavior is
        // undefined if the mutex provided by the 'qLock' method is not held.
        //
        // Note that 'symOptions' (a bitwise-OR of 'SYMOPT_*' values) specifies
        // flags affecting subsequent calls to the '.dll'.  Also note that the
        // current options are returned.  Finally, note that further details
        // are available at 'http://msdn.com'.

#ifdef BSLS_PLATFORM__CPU_32_BIT
    // For some reason, 'symFromAddr' doesn't work right on 64-bit, so we
    // '#ifdef' everything and use 'SymGetSymFromAddr64'.

    static BOOL symFromAddr(DWORD64      address,
                            PDWORD64     displacement,
                            PSYMBOL_INFO symbol);
        // Invoke the 'SymFromAddr' function of 'dbghelp.dll' with the
        // specified 'address', 'displacement', and 'symbol', and with an
        // (internally generated) handle for the current Windows process, and
        // return the result.  The behavior is undefined if the mutex provided
        // by the 'qLock' method is not held or if 'symbol->MaxNameLen' has not
        // been set to the maximum length.  (TBD: How is that known?)
        //
        // Note that 'symbol' is loaded with a pointer to the symbol
        // information for the symbol at 'address' and 'displacement' is loaded
        // with the difference between 'address' and the address of the symbol
        // described at 'symbol'.  Also note that 'true' is returned if a
        // symbol is found for 'address', and 'false' is returned otherwise.
        // Finally, note that further details of 'SymFromAddr' are available at
        // 'http://msdn.com'.

#else
    static BOOL symGetSymFromAddr64(DWORD64            address,
                                    PDWORD64           displacement,
                                    PIMAGEHLP_SYMBOL64 symbol);
        // Invoke the 'SymFromAddr64' function of 'dbghelp.dll' with the
        // specified 'address', 'displacement', and 'symbol', and with an
        // (internally generated) handle for the current Windows process, and
        // return the result.  The behavior is undefined if the mutex provided
        // by the 'qLock' method is not held or if 'symbol->MaxNameLen' has not
        // been set to the maximum length.  (TBD: How is that known?)
        //
        // Note that 'symbol' is loaded with a pointer to the symbol
        // information for the symbol at 'address' and 'displacement' is loaded
        // with the difference between 'address' and the address of the symbol
        // described at 'symbol'.  Also note that 'true' is returned if a
        // symbol is found for 'address', and 'false' is returned otherwise.
        // Finally, note that further details of 'SymFromAddr64' are available
        // at 'http://msdn.com'.

#endif

    static BOOL symGetLineFromAddr64(DWORD64          dwAddr,
                                     PDWORD           pdwDisplacement,
                                     PIMAGEHLP_LINE64 line);
        // Invoke the 'SymGetLineFromAddr64' function of 'dbghelp.dll' with the
        // specified 'dwAddr', 'pdwDisplacement', and 'line', and with an
        // (internally generated) handle for the current Windows process, and
        // return the result.  The behavior is undefined if the mutex provided
        // by the 'qLock' method is not held.
        //
        // Note that 'line' is loaded with a pointer to line information (e.g.,
        // source file name and line number) for the code at 'dwAddr' and
        // 'pdwDisplacement' is loaded with the displacement of that code from
        // the beginning of the line.  Also note that 'true' is returned if
        // information is found for 'dwAddr', and 'false' is returned
        // otherwise.  Finally, note that further details of
        // 'SymGetLineFromAddr64' are available at 'http://msdn.com'.

    static BOOL stackWalk64(DWORD          machineType,
                            HANDLE         hThread,
                            LPSTACKFRAME64 stackFrame,
                            PVOID          contextRecord);
        // Invoke the 'StackWalk64' function of 'dbghelp.dll' with the
        // specified 'machineType', 'hThread', 'stackFrame', and
        // 'contextRecord', and with internally set handlers for remaining
        // arguments, and return the result.  The behavior is undefined if the
        // mutex provided by the 'qLock' method is not held.
        //
        // Note that 'machineType' distinguishes between 32 and 64 bit
        // executables, 'hThread' is a Windows handle for the thread of
        // interest, 'stackFrame' has the address of a properly initialized
        // structure (including a field to receive the program counter), and
        // 'contextRecord' has the address of an initialized Windows 'CONTEXT'
        // structure.  The call to this method provides information on
        // consecutive stack frame.  Also note that 'true' is returned
        // if there are remaining stack frames on the stack, and 'false' is
        // returned otherwise.  Finally, note that further details of
        // 'StackWalk64' are available at 'http://msdn.com'.
};

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // ----------------------------------
                        // class baesu_DbghelpDllImpl_Windows
                        // ----------------------------------

// CLASS METHODS
inline
bcemt_QLock& baesu_DbghelpDllImpl_Windows::qLock()
{
    return s_qLock;
}

}  // close namespace BloombergLP

#endif  // BSLS_PLATFORM__OS_WINDOWS

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
