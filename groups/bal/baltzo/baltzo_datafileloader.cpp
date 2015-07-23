// baltzo_datafileloader.cpp                                          -*-C++-*-
#include <baltzo_datafileloader.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(baltzo_datafileloader_cpp,"$Id$ $CSID$")

#include <baltzo_errorcode.h>
#include <baltzo_zoneinfo.h>
#include <baltzo_zoneinfobinaryreader.h>

#include <ball_log.h>

#include <bdlsu_filesystemutil.h>
#include <bdlsu_pathutil.h>
#include <bdlb_strtokenrefiter.h>

#include <bslmf_assert.h>
#include <bsls_assert.h>
#include <bsls_platform.h>

#include <bsl_cstring.h>
#include <bsl_fstream.h>

namespace BloombergLP {

static const char LOG_CATEGORY[] = "BAETZO.DATAFILELOADER";

static const char *INVALID_PATH = "! INVALID_FILE_PATH !";

static const int UNSPECIFIED_ERROR = -1;
static const int UNSUPPORTED_ID    = baltzo::ErrorCode::BAETZO_UNSUPPORTED_ID;

BSLMF_ASSERT(UNSUPPORTED_ID != UNSPECIFIED_ERROR);

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
    for (bdlb::StrTokenRefIter it(timeZoneId, "/"); it; ++it) {
        bdlsu::PathUtil::appendIfValid(result, it());
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

namespace baltzo {
                       // ---------------------------
                       // class DataFileLoader
                       // ---------------------------

// CLASS METHODS
bool DataFileLoader::isPlausibleZoneinfoRootPath(const char *path)
{
    BSLS_ASSERT(path);

    if (!bdlsu::FileSystemUtil::isDirectory(path, true)) {
        return false;                                                 // RETURN
    }

    bsl::string gmtPath;
    concatenatePath(&gmtPath, path, "GMT");

    // If 'path' is a valid directory, appending "GMT" to it must result in a
    // valid path.

    return bdlsu::FileSystemUtil::isRegularFile(gmtPath.c_str(), true);
}

// CREATORS
DataFileLoader::DataFileLoader(bslma::Allocator *basicAllocator)
: d_rootPath(INVALID_PATH, basicAllocator)
{
}

DataFileLoader::~DataFileLoader()
{
}

// MANIPULATORS
void DataFileLoader::configureRootPath(const char *path)
{
    BSLS_ASSERT(path);

    if (!isPlausibleZoneinfoRootPath(path)) {
        BALL_LOG_SET_CATEGORY(LOG_CATEGORY);
        BALL_LOG_DEBUG << "Invalid directory provided to initialize "
                       << "Zoneinfo database time-zone information loader: "
                       << path << BALL_LOG_END;
    }
    d_rootPath = path;
}

int DataFileLoader::configureRootPathIfPlausible(const char *path)
{
    BSLS_ASSERT(path);

    if (!isPlausibleZoneinfoRootPath(path)) {
        return -1;                                                    // RETURN
    }

    d_rootPath = path;
    return 0;
}

int DataFileLoader::loadTimeZone(Zoneinfo *result,
                                        const char      *timeZoneId)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(timeZoneId);

    BALL_LOG_SET_CATEGORY(LOG_CATEGORY);

    bsl::string path;
    const int rc = loadTimeZoneFilePath(&path, timeZoneId);
    if (0 != rc) {
        BALL_LOG_ERROR << "Poorly formed time-zone identifier '"
                       << timeZoneId << "'" << BALL_LOG_END;
        return UNSUPPORTED_ID;                                        // RETURN
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

        BALL_LOG_ERROR << "Failed to open time-zone information file '"
                       << path << "'" << BALL_LOG_END;

        return isRootPathPlausible()
               ? UNSUPPORTED_ID
               : UNSPECIFIED_ERROR;                                   // RETURN
    }

    result->setIdentifier(timeZoneId);
    return ZoneinfoBinaryReader::read(result, infile);
}

// ACCESSORS
int DataFileLoader::loadTimeZoneFilePath(bsl::string *result,
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

const bsl::string& DataFileLoader::rootPath() const
{
    BSLS_ASSERT(INVALID_PATH != d_rootPath);

    return d_rootPath;
}
}  // close package namespace

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
