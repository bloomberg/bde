// baetzo_datafileloader.cpp                                          -*-C++-*-
#include <baetzo_datafileloader.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baetzo_datafileloader_cpp,"$Id$ $CSID$")

#include <baetzo_errorcode.h>
#include <baetzo_zoneinfo.h>
#include <baetzo_zoneinfobinaryreader.h>

#include <bael_log.h>

#include <bdesu_fileutil.h>
#include <bdesu_pathutil.h>
#include <bdeut_strtokenrefiter.h>

#include <bslmf_assert.h>
#include <bsls_assert.h>
#include <bsls_platform.h>

#include <bsl_cstring.h>
#include <bsl_fstream.h>

namespace BloombergLP {

static const char LOG_CATEGORY[] = "BAETZO.DATAFILELOADER";

static const char *INVALID_PATH = "! INVALID_FILE_PATH !";

enum { UNSPECIFIED_ERROR = -1 };
BSLMF_ASSERT(baetzo_ErrorCode::BAETZO_UNSUPPORTED_ID != UNSPECIFIED_ERROR);

// STATIC HELPER FUNCTIONS
static
void concatenatePath(bsl::string *result,
                     const char  *rootPath,
                     const char  *timeZoneId)
    // Load, into the specified 'result', the system independent path of the
    // specified 'timeZoneId' appended to the specified 'rootPath'.
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(rootPath);
    BSLS_ASSERT(timeZoneId);

    *result = rootPath;
    for (bdeut_StrTokenRefIter it(timeZoneId, "/"); it; ++it) {
        bdesu_PathUtil::appendIfValid(result, it());
    }
}

static
int validateTimeZoneId(const char *timeZoneId)
    // Returns 0 if the specified 'timeZoneId' contains only valid characters
    // and does not start with '/', and a non-zero value otherwise.
{
    BSLS_ASSERT(timeZoneId);

    if ('/' == timeZoneId[0]) {
        return -1;                                                    // RETURN
    }

    const char *VALID_CHAR = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                             "abcdefghijklmnopqrstuvwxyz"
                             "1234567890/_+-";

    const int length = bsl::strlen(timeZoneId);
    for (int i = 0; i < length; ++i) {
        const char *c = VALID_CHAR;
        while (*c != '\0') {
            if (*c == timeZoneId[i]) {
                break;
            }
            ++c;
        }
        if (*c == '\0') {
            return -2;                                                // RETURN
        }
    }
    return 0;
}

                       // ---------------------------
                       // class baetzo_DataFileLoader
                       // ---------------------------

// CLASS METHODS
bool baetzo_DataFileLoader::isPlausibleZoneinfoRootPath(const char *path)
{
    BSLS_ASSERT(path);

    if (!bdesu_FileUtil::isDirectory(path, true)) {
        return false;                                                 // RETURN
    }

    bsl::string gmtPath;
    concatenatePath(&gmtPath, path, "GMT");

    // If 'path' is a valid directory, appending "GMT" to it must result in a
    // valid path.

    return bdesu_FileUtil::isRegularFile(gmtPath.c_str(), true);
}

// CREATORS
baetzo_DataFileLoader::baetzo_DataFileLoader(bslma_Allocator *basicAllocator)
: d_rootPath(INVALID_PATH, basicAllocator)
{
}

baetzo_DataFileLoader::~baetzo_DataFileLoader()
{
}

// MANIPULATORS
void baetzo_DataFileLoader::configureRootPath(const char *path)
{
    BSLS_ASSERT(path);

    if (!isPlausibleZoneinfoRootPath(path)) {
        BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);
        BAEL_LOG_DEBUG << "Invalid directory provided to initialize "
                       << "Zoneinfo database time-zone information loader: "
                       << path << BAEL_LOG_END;
    }
    d_rootPath = path;
}

int baetzo_DataFileLoader::configureRootPathIfPlausible(const char *path)
{
    BSLS_ASSERT(path);

    if (!isPlausibleZoneinfoRootPath(path)) {
        return -1;                                                    // RETURN
    }

    d_rootPath = path;
    return 0;
}

int baetzo_DataFileLoader::loadTimeZone(baetzo_Zoneinfo *result,
                                        const char      *timeZoneId)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(timeZoneId);

    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);

    bsl::string path;
    const int rc = loadTimeZoneFilePath(&path, timeZoneId);
    if (0 != rc) {
        BAEL_LOG_INFO << "Poorly formed time-zone identifier '"
                      << timeZoneId << "'" << BAEL_LOG_END;
        return baetzo_ErrorCode::BAETZO_UNSUPPORTED_ID;               // RETURN
    }

    // Create a file stream for the olson data file.

    bsl::ifstream infile(path.c_str(),
                         bsl::ifstream::binary | bsl::ifstream::in);

    if (!infile.is_open()) {
        // Failed to open the data file for 'timeZoneId'.  If the data-file
        // loader is correctly configured, 'timeZoneId' is not a supported
        // time-zone identifier, so return 'BAETZO_UNSUPPORTED_ID'.  Otherwise,
        // if the data-file loader is not correctly configured, return
        // 'UNSPECIFIED_ERROR' (different from 'BAETZO_UNSUPPORTED_ID').

        BAEL_LOG_INFO << "Failed to open time-zone information file'"
                      << path << "'" << BAEL_LOG_END;

        return isRootPathPlausible()
               ? baetzo_ErrorCode::BAETZO_UNSUPPORTED_ID
               : UNSPECIFIED_ERROR;                                   // RETURN
    }

    result->setIdentifier(timeZoneId);
    return baetzo_ZoneinfoBinaryReader::read(result, infile);
}

// ACCESSORS
int baetzo_DataFileLoader::loadTimeZoneFilePath(bsl::string *result,
                                                const char  *timeZoneId) const
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(timeZoneId);
    BSLS_ASSERT(INVALID_PATH != d_rootPath);

    const int rc = validateTimeZoneId(timeZoneId);
    if (0 != rc) {
        return rc;
    }

    concatenatePath(result, d_rootPath.c_str(), timeZoneId);
    return 0;
}

const bsl::string& baetzo_DataFileLoader::rootPath() const
{
    BSLS_ASSERT(INVALID_PATH != d_rootPath);

    return d_rootPath;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
