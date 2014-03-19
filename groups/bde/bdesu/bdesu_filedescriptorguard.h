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
//  bdesu_FileDescriptorGuard: RAII proctor class used to close files
//
//@SEE_ALSO: bdesu_filesystemutil
//
//@AUTHOR: Bill Chapman (bchapman2)
//
//@DESCRIPTION: This component defines a class, 'bdesu_FileDescriptorGuard', an
// object of which manages an open file descriptor, and closes it when the
// guard goes out of scope and is destroyed.  A 'release' method is provided,
// which will release the descriptor from management by the proctor.  When a
// released proctor is destroyed, nothing happens.  A 'closeAndRelease' method
// is also provided, which closes the managed file handle and puts the proctor
// into a released state.
//
///Usage
///-----
//
///Example 1: Close a File Descriptor
/// - - - - - - - - - - - - - - - - -
// Suppose we want to open a file and do some I/O to it, and be sure the
// file handle will be closed when we're done with it.  We  use an object
// of type 'bdesu_FileDescriptorGuard' to facilitate this.
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
//  assert(bdesu_FileUtil::k_INVALID_FD != fd);
//..
// Next, we enter a scope and create a proctor object to manage 'fd':
//..
//  {
//      bdesu_FileDescriptorGuard proctor(fd);
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

#ifndef INCLUDED_BDESU_FILESYSTEMUTIL
#include <bdesu_filesystemutil.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

namespace BloombergLP {

                     // ================================
                     // struct bdesu_FileDescriptorGuard
                     // ================================

struct bdesu_FileDescriptorGuard {
    // This class implements a guard that conditionally closes an open file
    // descriptor upon its destruction.

    // PRIVATE TYPE
    typedef bdesu_FilesystemUtil FsUtil;

    // DATA
    FsUtil::FileDescriptor d_descriptor;  // Handle for the file being managed.

  private:
    // NOT IMPLEMENTED
    bdesu_FileDescriptorGuard(const bdesu_FileDescriptorGuard&);
    bdesu_FileDescriptorGuard& operator=(const bdesu_FileDescriptorGuard&);

  public:
    // CREATORS
    explicit
    bdesu_FileDescriptorGuard(FsUtil::FileDescriptor descriptor);
        // Create a proctor object that will manage the specified 'descriptor',
        // closing it upon destruction (unless either 'realease' or
        // 'closeAndRelease' has been called).  It is permissible for
        // 'descriptor == FsUtil::k_INVALID_FD', in which case the guard
        // created will not manage anything.

    ~bdesu_FileDescriptorGuard();
        // If this proctor object manages a file, close the file.

    // MANIPULATORS
    void closeAndRelease();
        // Close the file managed by this guard and release that file from
        // management by this object.  The behavior is undefined unless this
        // object is managing a file.

    void release();
        // Release the file from management by this object (without closing
        // it).  The behavior is undefined unless this object is managing a
        // file.

    // ACCESSORS
    FsUtil::FileDescriptor descriptor() const;
        // If this guard is managing a file, return the file descriptor
        // referring to that file, and return
        // 'bdesu_FileSystemUtil::INVALID_FD' otherwise.
};

// ============================================================================
//                          INLINE FUNCTION DEFINITIONS
// ============================================================================

// CREATORS
inline
bdesu_FileDescriptorGuard::bdesu_FileDescriptorGuard(
                               bdesu_FilesystemUtil::FileDescriptor descriptor)
: d_descriptor(descriptor)
{
}

inline
bdesu_FileDescriptorGuard::~bdesu_FileDescriptorGuard()
{
    if (FsUtil::k_INVALID_FD != d_descriptor) {
        closeAndRelease();
    }
}

// MANIPULATORS
inline
void bdesu_FileDescriptorGuard::release()
{
    BSLS_ASSERT(FsUtil::k_INVALID_FD != d_descriptor);

    d_descriptor = FsUtil::k_INVALID_FD;
}

// ACCESSORS
inline
bdesu_FilesystemUtil::FileDescriptor bdesu_FileDescriptorGuard::descriptor()
                                                                          const
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
