// bdesu_filecloseproctor.h                                           -*-C++-*-
#ifndef INCLUDED_BDESU_FILECLOSEPROCTOR
#define INCLUDED_BDESU_FILECLOSEPROCTOR

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a RAII proctor class used to close files.
//
//@CLASSES:
//  bdesu_FileCloseProctor: RAII proctor class used to close files
//
//@SEE_ALSO: bdesu_fileutil
//
//@AUTHOR: Bill Chapman (bchapman2)
//
//@DESCRIPTION:

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDESU_FILEUTIL
#include <bdesu_fileutil.h>
#endif

#ifndef INCLUDED_BSL_CSTDIO
#include <bsl_cstdio.h>
#endif

namespace BloombergLP {

                     // =============================
                     // struct bdesu_FileCloseProctor
                     // =============================

struct bdesu_FileCloseProctor {
    // Proctor to manage an open file.  The file can be represented by a
    // 'FILE *' pointer or a 'bdesu_FileUtil::FileDescriptor' passed upon
    // construction.  The file will be closed upon destruction, unless this
    // proctor object has been released before that.  Note that an object of
    // this proctor type can manage only one file at a time, through one of
    // the two types of file handles that can be supplied at construction.

    bsl::FILE                      *d_file_p;
    bdesu_FileUtil::FileDescriptor  d_descriptor;

  private:
    // NOT IMPLEMENTED
    bdesu_FileCloseProctor(const bdesu_FileCloseProctor&);
    bdesu_FileCloseProctor& operator=(const bdesu_FileCloseProctor&);

  public:
    // CREATORS
    explicit
    bdesu_FileCloseProctor(bsl::FILE *file_p);
        // Create a proctor object that will manage the specified 'file_p',
        // closing it upon destruction.

    explicit
    bdesu_FileCloseProctor(bdesu_FileUtil::FileDescriptor descriptor);
        // Create a proctor object that will manage the specified 'descriptor',
        // closing it upon destruction.

    ~bdesu_FileCloseProctor();
        // If this proctor object manages a file, close the file.

    // MANIPULATORS
    void closeAndRelease();
        // If this proctor object manages a file, close the file, and release
        // the file from management by this object.

    void release();
        // If this proctor object manages a file, release the file from
        // management by this object.

    // ACCESSORS
    bsl::FILE *file_p() const;
        // If this proctor manages a 'FILE *' pointer, return that pointer and
        // return 0 otherwise.

    bdesu_FileUtil::FileDescriptor descriptor() const;
        // If this proctor manages a 'bdesu_FileUtil::FileDescriptor', return
        // that file descriptor, and return 'bdesu_FileUtil::INVALID_FD'
        // otherwise.
};

// ============================================================================
//                          INLINE FUNCTION DEFINITIONS
// ============================================================================

                              // ---------------------
                              // struct bdesu_FileUtil
                              // ---------------------

// CREATORS
inline
bdesu_FileCloseProctor::bdesu_FileCloseProctor(bsl::FILE *file_p)
: d_file_p(file_p)
, d_descriptor(bdesu_FileUtil::INVALID_FD)
{}

inline
bdesu_FileCloseProctor::bdesu_FileCloseProctor(
                                     bdesu_FileUtil::FileDescriptor descriptor)
: d_file_p(0)
, d_descriptor(descriptor)
{}

// MANIPULATORS
inline
void bdesu_FileCloseProctor::release()
{
    d_file_p = 0;
    d_descriptor = bdesu_FileUtil::INVALID_FD;
}

// ACCESSORS
inline
bsl::FILE *bdesu_FileCloseProctor::file_p() const
{
    return d_file_p;
}

inline
bdesu_FileUtil::FileDescriptor bdesu_FileCloseProctor::descriptor() const
{
    return d_descriptor;
}

}  // close namespace BloombergLP

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2013
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
