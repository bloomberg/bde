// baecs_mappingmanager.h                                             -*-C++-*-
#ifndef INCLUDED_BAECS_MAPPINGMANAGER
#define INCLUDED_BAECS_MAPPINGMANAGER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide an in-process manager of mmap pages
//
//@AUTHOR: Oleg Semenov (osemenov), Andrei Basov (abasov)
//
//@CLASSES:
//      baecs_MappingManager: manager of mmap pages.
//
//@DESCRIPTION: This component provides a manager of memory-mapped pages.
// The pages are mapped as needed by the process and unmapped to satisfy
// the limit on the memory used by the mappings on a per-process basis.
// The mapping manager can manage the mappings from different files and
// guarantees that the total number of bytes in use by all files will not
// exceed the specified mapping limit, which can be changed dynamically.
//
///PRIORITY LEVELS
///---------------
// The mapping manager associates a priority with every page and manages
// fixed (passed at creation) number of priority lists for unused pages.
// A page is added to the mapping manager with a priority, which can never
// change for this page until the page is removed.  The priority of a page
// affects when this page will be unmapped -- pages of lower priorities are
// unmapped first.  (In a typical usage, pages of higher priorities can be
// used for a metadata.  This way, metadata pages are rarely unmapped resulting
// in a better performance.)  For the pages of a given priority, the pages
// are unmapped in a Least-Recently Used fashion.
//
///DIRTY PAGE LISTS
///----------------
// The mapping manager supports operations on page lists via operations such
// as 'createDirtyList', 'deleteDirtyList', 'clearDirtyList' and
// 'flushDirtyList'.  A page list is an opaque list of pages -- the user cannot
// directly manipulate such a list.  'addPage' method can associate a page with
// the list.  At that point the page is NOT added to the list -- it can be
// added later via 'usePage' method.  As the name implies, the intended usage
// of this mechanism is to implement lists of dirty pages -- the pages that are
// modified in memory and need to be synchronized with the disk.
//
///THREAD SAFETY
///-------------
// 'baecs_MappingManager' is thread-safe, meaning that multiple threads may
// use distinct instances.  It is also thread-enabled, meaning that multiple
// threads may share a single instance, for all functions except 'removePage'.
// It is not safe for a thread to remove a page if any other threads hold a
// handle to that page, whether or not the page is in use.
//
///USAGE EXAMPLE: BASIC USAGE
///--------------------------
// First of all, let's create a mapping manager.  For this example we'll assume
// that at most 100MB can be mapped into the process space by persistent
// journals:
//..
//  enum {
//      MAPPING_LIMIT = (1 << 20) * 100,             // 100 MB
//      NUM_PRIORITIES = 1
//  };
//  baecs_MappingManager mappingManager(MAPPING_LIMIT, NUM_PRIORITIES);
//..
// Next, assuming that the pages are stored in a file with descriptor 'fd',
// add a few pages to this manager:
//..
//  enum { NUM_PAGES = 100; }
//  baecs_MappingManager::Handle pageHandles[NUM_PAGES];
//  for (int i = 0; i < NUM_PAGES; ++i)
//  {
//     pageHandles[i] = mappingManager.addPage(fd,
//                                             8192*i,  // offset
//                                             8192,    // size
//                                             true,    // read/write
//                                             NUM_PRIORITIES - 1 // priority
//                                            );
//  }
//..
// Verify that the map count is still 0 since to pages are used:
//..
//  assert(0 == mappingManager.mapCount());
//..
// Now use every page and observe the number of mmap calls:
//..
//  for (int i = 0; i < NUM_PAGES; ++i)
//  {
//     mappingManager.usePage(pageHandles[i]);
//  }
//
//  assert(NUM_PAGES == mappingManager.mapCount());
//..
// We must release and remove all pages before the manager goes out of scope:
//..
//  for (int i = 0; i < NUM_PAGES; ++i)
//  {
//     mappingManager.releasePage(pageHandles[i]);
//     mappingManager.removePage(pageHandles[i]);
//  }
//..
//
///USAGE EXAMPLE: PAGE LISTS
///-------------------------
// Using an example above, let's create a mapping manager with 100 MB limit and
// one priority.  Then add a few pages, write data to every page and flush all
// touched pages to disk:
//..
//  enum {
//      MAPPING_LIMIT = (1 << 20) * 100,             // 100 MB
//      NUM_PRIORITIES = 1
//  };
//  baecs_MappingManager mappingManager(MAPPING_LIMIT, NUM_PRIORITIES);
//..
// Next, create a page list:
//..
//  baecs_MappingManager::PageListHandle pageList
//                                           = mappingManager.createPageList();
//  assert(pageList);
//..
// Next, assuming that the pages are stored in a file with descriptor 'fd',
// add a few pages to this manager and associate each page with 'pageList'
// (we can use bdesu_MemoryUtil to figure out platform-dependent page size):
//..
//  enum { NUM_PAGES = 100 };
//  const int PAGE_SIZE = bdesu_MemoryUtil::pageSize();
//  baecs_MappingManager::Handle pageHandles[NUM_PAGES];
//  for (int i = 0; i < NUM_PAGES; ++i)
//  {
//     pageHandles[i] = mappingManager.addPage(fd,
//                                             i*PAGE_SIZE,  // offset
//                                             PAGE_SIZE,    // size
//                                             true,         // read/write
//                                             NUM_PRIORITIES - 1, // priority
//                                             pageList);
//  }
//..
// Verify that the map count is still 0 since no pages are used:
//..
//  assert(0 == mappingManager.mapCount());
//..
// Use every page and add it to the page list:
//..
//  for (int i = 0; i < NUM_PAGES; ++i)
//  {
//     char *data = mappingManager.usePage(pageHandles[i], true);
//     bsl::memset(data, 0xAB, PAGE_SIZE);
//  }
//..
//  Now every page is accumulated in the list.  Flush the list to disk:
//..
//  int numFlushed = mappingManager.flushPageList(pageList);
//  assert(NUM_PAGES == numFlushed);
//..
//
// [TBD] Describe page alignment properties

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BCEMA_POOL
#include <bcema_pool.h>
#endif

#ifndef INCLUDED_BCEMT_MUTEX
#include <bcemt_mutex.h>
#endif

#ifndef INCLUDED_BDESU_FILEUTIL
#include <bdesu_fileutil.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BCEMT_THREAD
#include <bcemt_thread.h>
#endif

#ifndef INCLUDED_BCES_ATOMICTYPES
#include <bces_atomictypes.h>
#endif

namespace BloombergLP {

struct baecs_MappingManager_Page;
class baecs_MappingManager;

class baecs_MappingManager_PageList {
    friend class baecs_MappingManager;
    typedef baecs_MappingManager_Page Page;

    Page            *d_first_p;
    Page            *d_last_p;
    int              d_index;

    enum PageListIndex {
        BAECS_PAGE_LIST_CLEAN = 0,
        BAECS_PAGE_LIST_DIRTY = 1
    };

  private:
    // CREATORS
    explicit baecs_MappingManager_PageList(PageListIndex index);

  private:
    // NOT DEFINED for the sake of bsl::vector (and other evangelists)
    // baecs_MappingManager_PageList(const baecs_MappingManager_PageList&);

    // baecs_MappingManager_PageList&
    //     operator=(const baecs_MappingManager_PageList&);

  private:
    // MANIPULATORS
    Page* popPage();
        // Remove the first page from this page list and return its
        // handle.  The behavior is undefined if this page list is empty.

    void pushPage(Page* pageHandle);
        // Add the page with the specified 'pageHandle' to the end of this
        // page list.

    void removePage(Page* pageHandle);
        // Remove the page with the specified 'pageHandle' from this
        // pageList.  The behavior is undefined if the page with this handle
        // does not belong to this list

    // ACCESSORS
    bool isEmpty() const;
        // Return 'true' if this page list is empty, and 'false' otherwise.

    void  printList() const;
        // Print out the contents of this page list to bsl::cout.  This method
        // is used for debugging only.
};

                     // ============================
                     //  class baecs_MappingManager
                     // ============================
class baecs_MappingManager {
  public:
    typedef baecs_MappingManager_Page          *Handle;
    typedef baecs_MappingManager_PageList      *PageListHandle;

    static const Handle INVALID_HANDLE;

    BSLALG_DECLARE_NESTED_TRAITS(baecs_MappingManager,
                                 bslalg_TypeTraitUsesBslmaAllocator);
  private:
    // PRIVATE TYPES
    typedef baecs_MappingManager_PageList PageList;
    typedef baecs_MappingManager_Page     Page;

    // INSTANCE DATA
    bcemt_Mutex              d_mapLock;
    bsl::size_t              d_totalMapped;
    bsl::size_t              d_mappingLimit;
    PageList                 d_usedPages;
    bsl::vector<PageList>    d_unusedPages;
    int                      d_mapCount;
    bcema_Pool               d_pagesPool;
    bcema_Pool               d_pageListPool;

    // not implemented
    baecs_MappingManager(const baecs_MappingManager&);
    baecs_MappingManager& operator=(const baecs_MappingManager&);
  public:
    // CREATORS
    explicit baecs_MappingManager(bsl::size_t      mappingLimit,
                                  int              numPriorityLevels,
                                  bslma_Allocator *basicAllocator = 0);
        // Create an empty 'baecs_MappingManager' with the specified
        // 'mappingLimit' and the specified 'numPriorityLevels' number of
        // priority levels.  Optionally specify a 'basicAllocator' used to
        // supply that uses the specified 'basicAllocator' to supply memory.
        // If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The behavior is undefined unless
        // 0 < numPriorityLevels (e.g., there is at least one priority level.

    ~baecs_MappingManager();
        // Destroy this 'baecs_MappingManager'.  Note that all outstanding
        // pages (i.e., pages allocated using 'addPage' and not yet returned)
        // are automatically removed and the mappings managed by this object
        // are unmapped.

    // MANIPULATORS
    Handle addPage(bdesu_FileUtil::FileDescriptor   fileDescriptor,
                   bdesu_FileUtil::Offset           offset,
                   bsl::size_t                      size,
                   bool                             isReadWrite,
                   int                              priorityLevel,
                   PageListHandle                   pageList = NULL);
        // Add a page with the specified 'fileDescriptor', 'offset', 'size' to
        // this 'baecs_MappingManager'.  The page will be mapped read/write if
        // 'isReadWrite' is true, and read only otherwise.   Optionally,
        // specify a 'pageList' to associate this page with.  If 'pageList' is
        // not specified, the newly created page is not associated with any
        // lists.  Return a handle for the newly added page.  Note that the
        // 'fileDescriptor' must not be closed before this object goes out of
        // scope or all pages associated with the 'fileDescriptor' have been
        // removed.

    char *getPageData(Handle pageHandle);
        // Return the pointer to the mapped data of the page with the specified
        // 'pageHandle'.  The behavior is undefined unless the page is in use.
        // Note that the pointer may become invalid when the page is released.

    void releasePage(Handle pageHandle);
        // Decrement the reference count of the page with the specified
        // 'pageHandle'.  Note that when the reference count becomes zero, the
        // page is considered unused and may be unmapped later.

    void removePage(Handle pageHandle);
        // Removes the page with the specified 'pageHandle' from this
        // 'baecs_MappingManager'.  The behavior is undefined unless
        // 'pageHandle' is valid.  Note that the page will be removed
        // irrespective of the reference count, so that any existing pointers
        // returned by usePage() and not yet released will become invalid.

    void  setMappingLimit(size_t numBytes);
        // Set the mapping limit of this object to the specified 'numBytes'.
        // Note that this operation has effect only the next time remapping
        // occurs.

    char *usePage(Handle pageHandle, bool isDirty = false);
        // Increment the reference count of the page with the specified
        // 'pageHandle', and map it to the address space of the process if it
        // is not mapped yet.  If 'addPageToList' is true, add this page to the
        // associated page list.  The page will stay on the list until it is
        // unmapped, removed, or until the list is flushed with
        // 'flushPageList' method.  The behavior is undefined unless
        // 'pageHandle' is valid.  If 'addPagesToList' is true, the behavior
        // is undefined unless the page is associated with the list.
        // The 'baecs_MappingManager' will try to prevent the total size of
        // mapped pages from reaching the mapping limit by unmapping the least
        // recently used pages of lowest priority with zero reference count.
        // However, it never refuses to map a page, so the limit is not
        // strictly enforced.

    /// OPERATIONS WITH THE LIST OF DIRTY PAGES
    int clearDirtyList(PageListHandle handle);
        // Remove all the pages from the list with the specified 'handle'
        // without explicitly synchronizing them with the backing storage.
        // Return the number of pages removed.

    PageListHandle createDirtyList();
        // Create an empty list of pages.

    void deleteDirtyList(PageListHandle handle);
        // Delete the list of pages having the specified 'handle'.  The
        // behavior is undefined unless 'handle' was previously returned by
        // 'createPageList' method.  If the list is NOT empty, all pages in
        // the list are flushed (TBD - discuss)

    int flushDirtyList(PageListHandle handle, bool isSynchronous);
        // Flush the pages in the list having the specified 'handle' to disk.
        // If 'isSynchronous' flag is 'true' each page is explicitly
        // synchronized and this call blocks until all pages are written.
        // If 'isSynchronous' is false, the operating system will manage
        // the timing of actual flushes.  Return the number of pages flushed.

    int mapCountReset();
        // Atomically reset the number of system memory-map calls this object
        // made since the last call of 'mapCountReset' and return that number.

    // ACCESSORS
    bsl::size_t mappingLimit() const;
        // Return the mapping limit of this object.

    int getPageUseCount(Handle pageHandle) const;
        // Return the current use count of the page with the specified
        // 'pageHandle.'  The behavior is undefined unless 'pageHandle' is
        // the handle of a currently valid page.  Note that a page with
        // a use count of 0 may be unmapped at any time.

    int mapCount() const;
        // Return the number of (system) memory-map calls this object has made
        // since the last call of 'mapCountReset'.  This method is used for
        // diagnostics only.

    int numPriorityLevels() const;
        // Return the number of priority levels recognized by this mapping
        // manager.
};

inline
void baecs_MappingManager::setMappingLimit(size_t numBytes)
{
    d_mappingLimit = numBytes;
}

inline
bsl::size_t baecs_MappingManager::mappingLimit() const
{
    return d_mappingLimit;
}

inline
int baecs_MappingManager::mapCount() const
{
    return d_mapCount;
}

inline
int baecs_MappingManager::numPriorityLevels() const
{
    return d_unusedPages.size();
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
