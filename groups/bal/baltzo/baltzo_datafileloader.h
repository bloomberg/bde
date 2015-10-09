// baltzo_datafileloader.h                                            -*-C++-*-
#ifndef INCLUDED_BALTZO_DATAFILELOADER
#define INCLUDED_BALTZO_DATAFILELOADER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: provide a concrete 'baltzo::Loader' for Zoneinfo binary files.
//
//@CLASSES:
//  baltzo::DataFileLoader: concrete 'baltzo::Loader' for Zoneinfo binary data
//
//@SEE_ALSO: 'baltzo_zoneinfobinaryreader', 'baltzo_zoneinfoutil'
//
//@DESCRIPTION: This component provides a mechanism, 'baltzo::DataFileLoader',
// that is a concrete implementation of the 'baltzo::Loader' protocol for
// loading, into a 'baltzo::Zoneinfo' object, the properties of a time zone
// described in a Zoneinfo binary database file.  The following inheritance
// hierarchy diagram shows the classes involved and their methods:
//..
//   ,----------------------.
//  ( baltzo::DataFileLoader )
//   `----------------------'
//              |      ctor
//              |      configureRootPath
//              |      configureRootPathIfPlausible
//              |      loadTimeZoneFilePath
//              |      rootPath
//              |      isRootPathPlausible
//              V
//       ,--------------.
//      ( baltzo::Loader )
//       `--------------'
//                 dtor
//                 loadTimeZone
//..
// A 'baltzo::DataFileLoader' is supplied a file-system location using the
// 'configureRootPath' method.  This location should correspond to the root
// directory of a hierarchy containing Zoneinfo binary data files, where each
// Zoneinfo time-zone identifier indicates a relative path from the root
// directory to the binary data file containing the information for that time
// zone.  Accordingly, 'baltzo::DataFileLoader' provides a method that, given a
// time-zone identifier, will open the corresponding data file (relative to the
// root directory tree supplied at construction), and load, into a
// 'baltzo::Zoneinfo' object, the data from that file.
//
///Zoneinfo (TZ Database) Files
///----------------------------
// The Zoneinfo database, also referred to as either the TZ database or the
// Olson database (after its creator, Arthur Olson), is a standard
// public-domain time-zone information distribution used by many software
// systems (including a number of Unix variants and the Java Runtime
// Environment).  Information about the Zoneinfo database can be found online
// at 'http://www.twinsun.com/tz/tz-link.htm', including the time-zone rules
// for the supported time zones, and source code for the 'zic' compiler (for
// compiling those rules into the binary representation used by this
// component).  See 'baltzo_zoneinfobinaryreader' for more information about
// the binary file format.
//
///Directory Hierarchy
///- - - - - - - - - -
// Zoneinfo database files are typically held in a standard file-system
// directory hierarchy.  Zoneinfo time-zone identifiers (e.g.,
// "America/New_York") serve not only as an identifier, but as a relative path
// (using the UNIX file separator, '/') to the file containing data for the
// time zone.  So, given a hypothetical root directory "/etc/time_zones", the
// time-zone data file for "America/New_York" will be located in
// "/etc/time_zones/America/New_York".
//
///Thread Safety
///-------------
// 'baltzo::DataFileLoader' is *const* *thread-safe*, meaning that accessors
// may be invoked concurrently from different threads, but it is not safe to
// access or modify a 'baltzo::DataFileLoader' in one thread while another
// thread modifies the same object.
//
///Usage
///-----
// The following examples illustrate how to use a 'baltzo::DataFileLoader' to
// load the Zoneinfo time-zone data for a time zone.
//
///Example 1: Prologue: Creating a Example Data File
///- - - - - - - - - - - - - - - - - - - - - - - - -
// First we need to create one time-zone data file on which to operate.  In
// practice, clients should *not* generate data files in this manner.  Data
// files are typically created using the 'zic' compiler -- a publicly available
// tool provided as part of the standard Zoneinfo distribution (see
// 'http://www.twinsun.com/tz/tz-link.htm') -- and deployed in a standard
// directory location (see 'baltzo_defaultzoneinfocache').
//
// We start by defining static binary data for "Asia/Bangkok", (chosen because
// it is relatively small):
//..
//  const char ASIA_BANGKOK_DATA[] = {
//    0x54, 0x5a, 0x69, 0x66, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02,
//    0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
//    0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x08, 0xa2, 0x6a, 0x67, 0xc4,
//    0x01, 0x00, 0x00, 0x5e, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x62, 0x70, 0x00,
//    0x04, 0x42, 0x4d, 0x54, 0x00, 0x49, 0x43, 0x54, 0x00, 0x00, 0x00, 0x00,
//    0x00, 0x54, 0x5a, 0x69, 0x66, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//    0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//    0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x0c, 0xff, 0xff, 0xff,
//    0xff, 0x56, 0xb6, 0x85, 0xc4, 0xff, 0xff, 0xff, 0xff, 0xa2, 0x6a, 0x67,
//    0xc4, 0x01, 0x02, 0x00, 0x00, 0x5e, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x5e,
//    0x3c, 0x00, 0x04, 0x00, 0x00, 0x62, 0x70, 0x00, 0x08, 0x4c, 0x4d, 0x54,
//    0x00, 0x42, 0x4d, 0x54, 0x00, 0x49, 0x43, 0x54, 0x00, 0x00, 0x00, 0x00,
//    0x00, 0x00, 0x00, 0x0a, 0x49, 0x43, 0x54, 0x2d, 0x37, 0x0a
//  };
//..
// Then we create a testing sub-directory "test/Asia" that will hold the data
// file for Bangkok.  Note that "Asia/Bangkok" is the time-zone identifier for
// Bangkok and "Asia/Bangkok" also serves as a relative path (from our "./test"
// sub-directory) to that data file.
//..
//  #ifdef BSLS_PLATFORM_OS_WINDOWS
//  const char *TEST_DIRECTORY = "test\\Asia";
//  const char *TEST_FILE      = "test\\Asia\\Bangkok";
//  #else
//  const char *TEST_DIRECTORY = "test/Asia";
//  const char *TEST_FILE      = "test/Asia/Bangkok";
//  #endif
//  int rc = bdls::FileUtil::createDirectories(TEST_DIRECTORY, true);
//  assert(0 == rc);
//..
// Now we create a file for Bangkok and write the binary time-zone data to that
// file.
//..
//  bsl::ofstream outputFile(TEST_FILE, bsl::ofstream::binary);
//  assert(outputFile.is_open());
//  outputFile.write(ASIA_BANGKOK_DATA, sizeof(ASIA_BANGKOK_DATA));
//  assert(outputFile);
//  outputFile.close();
//..
// The file 'Bangkok' should now appear in the 'Asia' sub-directory, under out
// 'test' directory.
//
///Example 2: Using a 'baltzo::DataFileLoader' to Load a Zoneinfo File
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example we demonstrate how to use a 'baltzo::DataFileLoader' to load
// a time-zone data file into a 'baltzo::Zoneinfo' object.  We start by
// creating a 'baltzo::DataFileLoader' object, 'loader', and configure it with
// the relative path "test" which we created in Example 1 (Prologue).
//..
//  baltzo::DataFileLoader loader;
//  loader.configureRootPath("test");
//..
// Then we use the 'loadTimeZoneFilePath' method to verify that 'loader' will
// correctly locate the test data file we've created:
//..
//  const char *BANGKOK_ID = "Asia/Bangkok";
//  bsl::string bangkokDataPath;
//  rc = loader.loadTimeZoneFilePath(&bangkokDataPath, BANGKOK_ID);
//  assert(0         == rc);
//  assert(TEST_FILE == bangkokDataPath);  // Note 'TEST_FILE' from Example 1.
//..
// Now we create a 'baltzo::Zoneinfo' object, 'timeZone', and load it using
// 'loader':
//..
//  baltzo::Zoneinfo timeZone;
//  rc = loader.loadTimeZone(&timeZone, BANGKOK_ID);
//  assert(0 == rc);
//..
// Finally we confirm that certain properties of the 'timezone' object are in
// agreement with the properties defined in the binary data (see
// 'baltzo_zoneinfobinaryreader'): (1) That the object's identifier is
// "Asia/Bangkok", and (2) the object contains three local time descriptors,
// "LMT" (Local Mean Time), "BMT" (Bangkok Mean Time) and "ICT" (Indochina
// Time), in that order:
//..
//  assert(BANGKOK_ID == timeZone.identifier());
//  baltzo::Zoneinfo::LocalTimeDescriptorConstIterator iterator =
//                                                  timeZone.descriptorBegin();
//  assert("LMT" == iterator->description());
//  ++iterator;
//  assert("BMT" == iterator->description());
//  ++iterator;
//  assert("ICT" == iterator->description());
//..
// The 'timeZone' object can now be use for time-zone calculations.  See
// 'baltzo_zoneinfoutil'.
//
///Epilogue: Removing the Created Files
///  -  -  -  -  -  -  -  -  -  -  -  -
// The file hierarchy we created Example 1 solely for Example 2, is no longer
// needed, and is removed by:
//..
//  int rc = bdls::FileUtil::remove(TEST_DIRECTORY, true);
//  assert(0 == rc);
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALTZO_LOADER
#include <baltzo_loader.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP {
namespace baltzo {

class Zoneinfo;

                            // ====================
                            // class DataFileLoader
                            // ====================

class DataFileLoader : public Loader {
    // This component provides a concrete implementation of the 'DataLoader'
    // protocol for loading, into a 'Zoneinfo', the properties of a time zone
    // defined by an Zoneinfo (TZ Database) binary file located on the file
    // system.

    // DATA
    bsl::string d_rootPath;  // root path for time-zone data file

  private:
    // NOT IMPLEMENTED
    DataFileLoader(const DataFileLoader&);
    DataFileLoader& operator=(const DataFileLoader&);

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(DataFileLoader,
                                 bslalg::TypeTraitUsesBslmaAllocator);

    // CLASS METHODS
    static bool isPlausibleZoneinfoRootPath(const char *path);
        // Return 'true' if there currently exists a directory at the specified
        // file-system 'path' that appears to contain Zoneinfo time-zone
        // information files.  This method verifies (at a minimum) that 'path'
        // is a valid directory, and contains files for a subset of common
        // time-zone identifiers, but does not *guarantee* 'path' is currently
        // (or will remain) a correctly configured Zoneinfo database containing
        // a complete set of time-zone data.

    // CREATORS
    explicit DataFileLoader(bslma::Allocator *basicAllocator = 0);
        // Create an unconfigured data-file loader.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    virtual ~DataFileLoader();
        // Destroy this data-file loader.

    // MANIPULATORS
    void configureRootPath(const char *path);
        // Unconditionally set, to the specified 'path', the root of the
        // file-system directory hierarchy from which this loader will read
        // Zoneinfo binary time-zone information files.

    int configureRootPathIfPlausible(const char *path);
        // Set, to the specified 'path', the root of the file-system directory
        // hierarchy from which this loader will read Zoneinfo binary time-zone
        // information files.  Return 0 on success, and a non-zero value
        // (without no effect) if 'path' is not a directory that appears to
        // contain valid Zoneinfo data, as determined by calling
        // 'isPlausibleZoneinfoRootPath' on 'path'.

    virtual int loadTimeZone(Zoneinfo *result, const char *timeZoneId);
        // Load into the specified 'result' the time-zone information for the
        // time zone identified by the specified 'timeZoneId'.  Return 0 on
        // success, and a non-zero value otherwise.  A return status of
        // 'ErrorCode::k_UNSUPPORTED_ID' indicates that 'timeZoneId' is not
        // recognized.  If an error occurs during this operation, 'result' will
        // be left in a valid, but unspecified state.

    // ACCESSORS
    int loadTimeZoneFilePath(bsl::string *result,
                             const char  *timeZoneId) const;
        // Load into the specified 'result' the file-system path to the
        // Zoneinfo binary data file corresponding to the specified
        // 'timeZoneId' relative to the configured 'rootPath'.  Return 0 on
        // success, and a non-zero value otherwise.  On error, 'result' is left
        // in a valid, but unspecified state.  The behavior is undefined unless
        // either 'configureRootPath' or 'configureRootPathIfValid' has been
        // called successfully.  Note that this operation does not verify
        // 'result' refers to a valid file on the file system, or whether the
        // file (if it exists) contains valid Zoneinfo data.

    const bsl::string& rootPath() const;
        // Return the root of the directory hierarchy from which this loader
        // will attempt to load Zoneinfo binary data files.  The behavior is
        // undefined unless either 'configureRootPath' has been called or
        // 'configureRootPathIfValid' has been called successfully.

    bool isRootPathPlausible() const;
        // Return 'true' if the directory returned by the 'rootPath' method
        // exists and appears to contain valid Zoneinfo time-zone information
        // files, as determined by calling 'isPlausibleZoneinfoRootPath' on the
        // value returned by the 'rootPath' method.
};

}  // close package namespace

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

// ACCESSORS
inline
bool baltzo::DataFileLoader::isRootPathPlausible() const
{
    return isPlausibleZoneinfoRootPath(rootPath().c_str());
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
