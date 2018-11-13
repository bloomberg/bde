// balb_filecleanerconfiguration.h                                    -*-C++-*-
#ifndef INCLUDED_BALB_FILECLEANERCONFIGURATION
#define INCLUDED_BALB_FILECLEANERCONFIGURATION

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an attribute class for file cleaner configuration.
//
//@CLASSES:
// balb::FileCleanerConfiguration: configuration spec for a file cleaner
//
//@SEE_ALSO: balb_filecleanerutil
//
//@AUTHOR: Oleg Subbotin (osubbotin)
//
//@DESCRIPTION: This component provides a single, simply constrained
// (value-semantic) attribute class, 'balb::FileCleanerConfiguration', that
// contains a set of attributes (objects and parameters) of a file cleaner.
//
///Attributes
///----------
//..
//  Name            Type                Default         Simple Constraints
//  --------------  ------------------  --------------  ------------------
//  filePattern     bsl::string         ""              none
//  maxFileAge      bsls::TimeInterval  TimeInterval()  none
//  minNumFiles     int                 0               [0 .. INT_MAX]
//..
//: o 'filePattern': filesystem pattern used for file matching.
//:
//: o 'maxFileAge' : maximum file age (since last modification).
//:
//: o 'minNumFiles': minumum number of (newest) files, matching the pattern,
//:   that must be kept by the file cleaner.
//
///Thread Safety
///-------------
// 'balb::FileCleanerConfiguration' is *const* *thread-safe*, meaning that
// accessors may be invoked concurrently from different threads, but it is not
// safe to access or modify a 'balb::FileCleanerConfiguration' in one thread
// while another thread modifies the same object.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Usage
/// - - - - - - - - - - -
// The following code illustrates how to create a configuration that can be
// later supplied to a file cleanup utility (see {'balb_filecleanerutil'} for
// an example of how to use the created configuration to perform file
// cleaning).
//
// First, we create a 'balb::FileCleanerConfiguration' object having the
// default value:
//..
//  balb::FileCleanerConfiguration config;
//..
// Next, we populate the attributes of our configuration object:
//..
//  config.setFilePattern("/var/log/myApp/log*");
//  config.setMaxFileAge(bsls::TimeInterval(60*60*24));
//  config.setMinFilesNumber(4);
//..
// Now, we verify the options are configured correctly:
//..
//  assert("/var/log/myApp/log*" == config.filePattern());
//  assert(bsls::TimeInterval(60*60*24) == config.maxFileAge());
//  assert(4 == config.minNumFiles());
//..
// Finally, we print the configuration value to 'cout' and return:
//..
//  bsl::cout << config << bsl::endl;
//..
// This produces the following (multi-line) output:
//..
//  [
//      FilePattern = /var/log/myApp/log*
//      MaxFileAge = (86400, 0)
//      MinNumFiles = 4
//  ]
//..

#include <balscm_version.h>

#include <bslma_allocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_assert.h>
#include <bsls_review.h>
#include <bsls_timeinterval.h>

#include <bsl_iosfwd.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace balb {

                    // ==============================
                    // class FileCleanerConfiguration
                    // ==============================

class FileCleanerConfiguration {
    // This simply constrained (value-semantic) attribute class characterizes
    // configuration parameters for a file cleaner.  See the {Attributes}
    // section under @DESCRIPTION in the component-level documentation for
    // information on the class attributes.  Note that the class invariants are
    // identically the constraints on the individual attributes.
    //
    // This class:
    //: o supports a complete set of *value-semantic* operations
    //:   o except for 'bdex' serialization
    //: o is *exception-neutral* (agnostic)
    //: o is *alias-safe*
    //: o is 'const' *thread-safe*
    // For terminology see 'bsldoc_glossary'.

  private:
    // DATA
    bsl::string         d_filePattern;  // file pattern

    bsls::TimeInterval  d_maxFileAge;   // maximum age of the files to keep
                                        // (since last modification)

    int                 d_minNumFiles;  // minimum number of files to keep

    // FRIENDS
    friend bool operator==(const FileCleanerConfiguration&,
                           const FileCleanerConfiguration&);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(FileCleanerConfiguration,
                                   bslma::UsesBslmaAllocator);

    // CREATORS
    explicit FileCleanerConfiguration(bslma::Allocator *basicAllocator = 0);
        // Create a file cleaner configuration object having default values for
        // all attributes.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    FileCleanerConfiguration(const bslstl::StringRef&   filePattern,
                             const bsls::TimeInterval&  maxAge,
                             int                        minNumber,
                             bslma::Allocator          *basicAllocator = 0);
        // Create a file cleaner configuration object having the specified
        // 'filePattern', 'maxAge', and 'minNumber' attribute values.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    FileCleanerConfiguration(
                          const FileCleanerConfiguration&  original,
                          bslma::Allocator                *basicAllocator = 0);
        // Create a file cleaner configuration object having the in-core value
        // of the specified 'original' object.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    //! ~FileCleanerConfiguration() = default;
        // Destroy this object.

    // MANIPULATORS
    FileCleanerConfiguration& operator=(const FileCleanerConfiguration& rhs);
        // Assign to this file cleaner configuration object the in-core value
        // of the specified 'rhs' object, and return a reference providing
        // modifiable access to this object.

    void setFilePattern(const bslstl::StringRef& filePattern);
        // Set the file pattern attribute of this object to the specified
        // 'filePattern'.

    void setMaxFileAge(const bsls::TimeInterval& maxAge);
        // Set the maximum file age attribute of this object to the specified
        // 'maxAge'.

    void setMinNumFiles(int minNumber);
        // Set the minimum number of files to keep attribute of this object to
        // the specified 'minNumber'.

    // ACCESSORS
    const bsl::string& filePattern() const;
        // Return a 'const' reference to the file pattern attribute of this
        // object.

    bsls::TimeInterval maxFileAge() const;
        // Return the maximum file age attribute of this object.

    int minNumFiles() const;
        // Return the minimum number of files to keep attribute of this object.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;
        // Format a reasonable representation of this object to the specified
        // output 'stream' at the (absolute value of) the optionally specified
        // indentation 'level' and return a reference to 'stream'.  If 'level'
        // is specified, optionally specify 'spacesPerLevel', the number of
        // spaces per indentation level for this and all of its nested objects.
        // If 'level' is negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, suppress all indentation AND format
        // the entire output on one line.  If 'stream' is not valid on entry,
        // this operation has no effect.
};

// FREE OPERATORS
bool operator==(const FileCleanerConfiguration& lhs,
                const FileCleanerConfiguration& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same (in-core) value, and 'false' otherwise.  Two attribute objects
    // have the same in-core value if each respective attribute has the same
    // in-core value.

bool operator!=(const FileCleanerConfiguration& lhs,
                const FileCleanerConfiguration& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same (in-core) value, and 'false' otherwise.  Two attribute
    // objects do not have the same in-core value if one or more respective
    // attributes differ in in-core values.

bsl::ostream& operator<<(bsl::ostream&                   stream,
                         const FileCleanerConfiguration& configuration);
    // Write a reasonable representation of the specified 'configuration'
    // object to the specified output 'stream', and return a reference to
    // 'stream'.

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                         // ------------------------------
                         // class FileCleanerConfiguration
                         // ------------------------------

// CREATORS
inline
FileCleanerConfiguration::FileCleanerConfiguration(
                                              bslma::Allocator *basicAllocator)
: d_filePattern(basicAllocator)
, d_maxFileAge(0, 0)
, d_minNumFiles(0)
{
}

inline
FileCleanerConfiguration::FileCleanerConfiguration(
                                     const bslstl::StringRef&   filePattern,
                                     const bsls::TimeInterval&  maxAge,
                                     int                        minNumber,
                                     bslma::Allocator          *basicAllocator)
: d_filePattern(filePattern, basicAllocator)
, d_maxFileAge(maxAge)
, d_minNumFiles(minNumber)
{
}

inline
FileCleanerConfiguration::FileCleanerConfiguration(
                               const FileCleanerConfiguration&  original,
                               bslma::Allocator                *basicAllocator)
: d_filePattern(original.d_filePattern, basicAllocator)
, d_maxFileAge(original.d_maxFileAge)
, d_minNumFiles(original.d_minNumFiles)
{
}

// MANIPULATORS
inline
FileCleanerConfiguration&
FileCleanerConfiguration::operator=(const FileCleanerConfiguration& rhs)
{
    d_filePattern = rhs.d_filePattern;
    d_maxFileAge  = rhs.d_maxFileAge;
    d_minNumFiles = rhs.d_minNumFiles;

    return *this;
}

inline
void FileCleanerConfiguration::setFilePattern(
                                          const bslstl::StringRef& filePattern)
{
    d_filePattern = filePattern;
}

inline
void FileCleanerConfiguration::setMaxFileAge(const bsls::TimeInterval& maxAge)
{
    d_maxFileAge = maxAge;
}

inline
void FileCleanerConfiguration::setMinNumFiles(int minNumber)
{
    BSLS_REVIEW(0 <= minNumber);
    d_minNumFiles = minNumber;
}

// ACCESSORS
inline
const bsl::string& FileCleanerConfiguration::filePattern() const
{
    return d_filePattern;
}

inline
bsls::TimeInterval FileCleanerConfiguration::maxFileAge() const
{
    return d_maxFileAge;
}

inline
int FileCleanerConfiguration::minNumFiles() const
{
    return d_minNumFiles;
}

}  // close package namespace

// FREE OPERATORS
inline
bool balb::operator==(const balb::FileCleanerConfiguration& lhs,
                      const balb::FileCleanerConfiguration& rhs)
{
    return lhs.d_filePattern == rhs.d_filePattern
        && lhs.d_maxFileAge  == rhs.d_maxFileAge
        && lhs.d_minNumFiles == rhs.d_minNumFiles;
}

inline
bool balb::operator!=(const balb::FileCleanerConfiguration& lhs,
                      const balb::FileCleanerConfiguration& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& balb::operator<<(
                           bsl::ostream&                         stream,
                           const balb::FileCleanerConfiguration& configuration)
{
    return configuration.print(stream);
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2017 Bloomberg Finance L.P.
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
