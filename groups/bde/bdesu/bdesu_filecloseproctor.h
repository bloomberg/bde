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
//@DESCRIPTION: The component defines a proctor class, an object of which will
// manage an open file and close it when it goes out of scope.  These proctor
// objects are supplied with one of two types of file handles at construction,
// and they use that handle to close the file when the proctor object is
// destroyed.
//
// A 'release' method is provided, which will release the handle from
// management by the proctor.  When a released proctor is destroyed, nothing
// happens.  A 'closeAndRelease' method is also provided, which closes the
// managed file handle and puts the proctor into a released state.
//
///Usage
///-----
//
///Example 1: Close a File Descriptor
/// - - - - - - - - - - - - - - - - -
// Suppose we want to open a file and do some I/O to it, and be sure the
// file handle will be closed when we're done with it.  We  use an object
// of type 'bdesu_FileCloseProctor' to facilitate this.
//
// First, we create a name for our temporary file and a few local varriables.
//..
//  const bsl::string txtPath = "essay.txt";
//  int rc;
//  char buffer[1000];
//..
// Then, we open the file:
//..
//  bdesu_FileUtil::FileDescriptor fd = bdesu_FileUtil::open(
//                                              txtPath,
//                                              true,       // writable
//                                              false);     // non-existsent
//  assert(bdesu_FileUtil::INVALID_FD != fd);
//..
// Next, we enter a scope and create a proctor object to manage 'fd':
//..
//  {
//      bdesu_FileCloseProctor proctor(fd);
//..
// Then, we declare an essay we would like to write to the file:
//..
//      const char essay[] = {
//                         "If you can't annoy somebody, there is little\n"
//                         "point in writing.\n"
//                         "                Kingsley Amis\n"
//                         "\n"
//                         "It takes a lifetime to build a reputation, and\n"
//                         "five minutes to lose it.\n"
//                         "                Warren Buffet\n"
//                         "\n"
//                         "Originality is stubborn but not indestructible.\n"
//                         "You can't tell it what to do, and if you try too\n"
//                         "hard to steer it, you either chase it away or\n"
//                         "murder it.\n"
//                         "                Salman Khan\n" };
//..
// Next, we write our essay to the file:
//..
//      rc = bdesu_FileUtil::write(fd, essay, sizeof(essay));
//      assert(sizeof(essay) == rc);
//..
// Then, we seek back to the beginning of the file.
//..
//      Util::Offset off = Util::seek(fd, 0, Util::BDESU_SEEK_FROM_BEGINNING);
//      assert(0 == off);
//..
// Next, we read the file back and see if it's the same thing we wrote before:
//..
//      bsl::memset(buffer, 0, sizeof(buffer));
//      rc = bdesu_FileUtil::read(fd, buffer, sizeof(buffer));
//      assert(sizeof(essay) == rc);
//      assert(! bsl::strcmp(essay, buffer));
//..
// Now, 'proctor' goes out of scope, and its destructor closes the file
// descriptor.
//..
//  }
//..
// Finally, we observe that further attempts to write to 'fd' fail because the
// descriptor has been closed:
//..
//  const char finalWord[] = { "No matter where you go, there you are.\n"
//                             "                Buckaroo Banzai\n" };
//
//  rc = bdesu_FileUtil::write(fd, finalWord, sizeof(finalWord));
//  assert(rc < 0);
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDESU_FILEUTIL
#include <bdesu_fileutil.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
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
        // closing it upon destruction.  The behavior is undefined unless
        // 'file_p' refers to a file opened with 'bsl::fopen'.

    explicit
    bdesu_FileCloseProctor(bdesu_FileUtil::FileDescriptor descriptor);
        // Create a proctor object that will manage the specified 'descriptor',
        // closing it upon destruction.  The behavior is undefined unless
        // 'descriptor' refers to an opened file.

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
{
    BSLS_ASSERT_OPT(file_p);
}

inline
bdesu_FileCloseProctor::bdesu_FileCloseProctor(
                                     bdesu_FileUtil::FileDescriptor descriptor)
: d_file_p(0)
, d_descriptor(descriptor)
{
    BSLS_ASSERT_OPT(bdesu_FileUtil::INVALID_FD != descriptor);
}

inline
bdesu_FileCloseProctor::~bdesu_FileCloseProctor()
{
    this->closeAndRelease();
}

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
