// baecs_mappingmanager.cpp                                           -*-C++-*-
#include <baecs_mappingmanager.h>
#include <bdesu_memoryutil.h>
#include <bael_log.h>

#include <bsls_assert.h>
#include <bsls_platform.h>

namespace BloombergLP {

namespace {

const char LOG_CATEGORY[] = "BAECS.MAPPINGMANAGER";

}

// TBD -- review BAEL log usage.

struct baecs_MappingManager_Page {
    struct Link {
        baecs_MappingManager_Page      *d_next;
        baecs_MappingManager_Page      *d_prev;
    };
    bdesu_FileUtil::FileDescriptor  d_fd;
    void*                           d_address;
    bdesu_FileUtil::Offset          d_offset;
    int                             d_size;
    bces_AtomicInt                  d_useCount;
    bool                            d_isReadWrite;
    int                             d_priority;
    baecs_MappingManager_PageList  *d_dirtyList;
    Link                            d_links[2];
};

baecs_MappingManager_PageList::baecs_MappingManager_PageList(
                                                           PageListIndex index)
: d_first_p(NULL)
, d_last_p(NULL)
, d_index(index)
{
}

baecs_MappingManager::baecs_MappingManager(size_t mappingLimit,
                                           int    numPriorityLevels,
                                           bslma_Allocator *allocator)
: d_totalMapped(0)
, d_mappingLimit(mappingLimit)
, d_usedPages(PageList::BAECS_PAGE_LIST_CLEAN)
, d_unusedPages(numPriorityLevels, PageList(PageList::BAECS_PAGE_LIST_CLEAN),
                allocator)
, d_mapCount(0)
, d_pagesPool(sizeof(Page), allocator)
, d_pageListPool(sizeof(PageList), allocator)
{
    BSLS_ASSERT(0 < numPriorityLevels);
    BSLS_ASSERT((int) d_unusedPages.size() == numPriorityLevels);
}

baecs_MappingManager::~baecs_MappingManager()
{
    // Assertion failures in the destructor mean that some pages
    // were not removed from the mapping manager before it was destroyed.
    BSLS_ASSERT(d_usedPages.isEmpty());
    for (bsl::vector<PageList>::size_type i = 0;
                                               i < d_unusedPages.size(); ++i) {
        BSLS_ASSERT(d_unusedPages[i].isEmpty());
    }
}

int baecs_MappingManager::getPageUseCount(Handle pageHandle) const
{
    const Page* page = reinterpret_cast<const Page*>(pageHandle);
    return page->d_useCount;
}

int baecs_MappingManager::mapCountReset() {
    int t = d_mapCount;
    d_mapCount = 0;
    return t;
}

baecs_MappingManager_Page* baecs_MappingManager_PageList::popPage() {
    Page* page = d_first_p;
    BSLS_ASSERT(!d_first_p == !d_last_p);
    BSLS_ASSERT(page);
    d_first_p = page->d_links[d_index].d_next;
    if (d_first_p == NULL) {
        d_last_p = NULL;
    } else {
        d_first_p->d_links[d_index].d_prev = NULL;
    }
    page->d_links[d_index].d_next = 0;
    page->d_links[d_index].d_prev = 0;
    BSLS_ASSERT(!d_first_p == !d_last_p);
    return page;
}

void baecs_MappingManager_PageList::pushPage(Page* page) {
    BSLS_ASSERT(page);
    BSLS_ASSERT(!d_first_p == !d_last_p);
    BSLS_ASSERT(page->d_links[d_index].d_next == 0);
    BSLS_ASSERT(page->d_links[d_index].d_prev == 0);
    Page *prev = d_last_p;
    page->d_links[d_index].d_prev = prev;
    page->d_links[d_index].d_next = NULL;
    if (prev != NULL) {
        prev->d_links[d_index].d_next = page;
    } else {
        d_first_p = page;
    }
    d_last_p = page;
    BSLS_ASSERT(!d_first_p == !d_last_p);
}

void baecs_MappingManager_PageList::removePage(Page* page) {
    BSLS_ASSERT(page);
    BSLS_ASSERT(!d_first_p == !d_last_p);
    Page *next = page->d_links[d_index].d_next;
    Page *prev = page->d_links[d_index].d_prev;
    page->d_links[d_index].d_next = 0;
    page->d_links[d_index].d_prev = 0;
    if (d_first_p == page) d_first_p = next;
    if (d_last_p == page) d_last_p = prev;
    if (prev != NULL) prev->d_links[d_index].d_next = next;
    if (next != NULL) next->d_links[d_index].d_prev = prev;
    BSLS_ASSERT(!d_first_p == !d_last_p);
}

bool baecs_MappingManager_PageList::isEmpty() const {
    BSLS_ASSERT(!d_first_p == !d_last_p);
    return d_first_p == NULL;
}

void baecs_MappingManager_PageList::printList() const
{
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);
    Page *prev = NULL;

    BAEL_LOG_DEBUG << "[" << d_first_p
        << ".." << d_last_p << "]: ";
    for(Page *p = d_first_p; p != NULL; p = p->d_links[d_index].d_next) {
        BAEL_STREAM << p << " ";
        prev = p;
    }
    BAEL_STREAM << BAEL_LOG_END;

    BSLS_ASSERT(prev == d_last_p);
}

                         // --------------------------
                         // class baecs_MappingManager
                         // --------------------------

const baecs_MappingManager::Handle baecs_MappingManager::INVALID_HANDLE(NULL);

char *
baecs_MappingManager::usePage(Handle pageHandle, bool isDirty)
{
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);
    // We need to take a lock to change
    // the dirty flag of the page, or to change the useCount from nonzero
    // to zero or vice-versa.  Incrementing or decrementing the useCount
    // if it is positive (and stays positive) can be done without locks.
    bool makeDirty = isDirty;
    Page* page = reinterpret_cast<Page*>(pageHandle);
    int useCount;
    BSLS_ASSERT(page);
    while(1)
    {
        useCount = page->d_useCount;
        if (useCount < 2 || (bool)(useCount&1) != makeDirty) break;
        if (useCount == page->d_useCount.testAndSwap(useCount, useCount+2))
        {
            BSLS_ASSERT(page->d_address);
            return (char*)page->d_address;
        }
    }

    d_mapLock.lock();
    useCount = page->d_useCount;
    if (useCount&1) {
        makeDirty = false;
    }
    if (useCount >= 2) {
        int newUseCount;
        do {
            useCount = page->d_useCount;
            newUseCount = useCount+2;
            if (makeDirty) newUseCount |= 1;
            BSLS_ASSERT(useCount > 0);
        } while (useCount != page->d_useCount.testAndSwap(
                            useCount, newUseCount));
    } else {
        if (!page->d_address) { // page is not mapped
            int priority = 0;
            while (d_totalMapped + page->d_size >= d_mappingLimit
                && priority < (int) d_unusedPages.size())
            {
                if (!d_unusedPages[priority].isEmpty()) {
                    Page* pageToUnmap = d_unusedPages[priority].popPage();
                    BSLS_ASSERT(pageToUnmap->d_useCount < 2);
                    bdesu_FileUtil::unmap(pageToUnmap->d_address,
                                          pageToUnmap->d_size);
                    if (pageToUnmap->d_useCount & 1) {
                        pageToUnmap->d_dirtyList->removePage(pageToUnmap);
                        pageToUnmap->d_useCount = 0;
                    }
                    // TBD: rc check
                    pageToUnmap->d_address = 0;
                    d_totalMapped -= pageToUnmap->d_size;
                } else {
                    ++priority;
                }
            }
            ++d_mapCount;
            BAEL_LOG_TRACE << "Mapping page for fd = " << page->d_fd
                << " at " << page->d_offset << "(" << page->d_size << ")"
                << BAEL_LOG_END;
            int rc = bdesu_FileUtil::map(
                page->d_fd,
                &page->d_address,
                page->d_offset,
                page->d_size,
                bdesu_MemoryUtil::BDESU_ACCESS_READ |
                    (page->d_isReadWrite ?
                        bdesu_MemoryUtil::BDESU_ACCESS_WRITE : 0) );

            if (0 != rc)
            {
                BAEL_LOG_FATAL << "Mapping page for fd = " << page->d_fd
                    << " at " << page->d_offset << "(" << page->d_size
                    << ") FAILED: rc = " << rc << BAEL_LOG_END;
            }
            BSLS_ASSERT(0 == rc);
            d_totalMapped += page->d_size;
        } else { // page is mapped and so is in the unused list
            d_unusedPages[page->d_priority].removePage(page);
        }
        d_usedPages.pushPage(page);
        page->d_useCount = 2+(makeDirty ? 1 : (useCount & 1));
    }
    if (makeDirty) {
        // was not dirty, we've just set the dirty flag
        BSLS_ASSERT(0 ==
                        page->d_links[PageList::BAECS_PAGE_LIST_DIRTY].d_next);
        BSLS_ASSERT(0 ==
                        page->d_links[PageList::BAECS_PAGE_LIST_DIRTY].d_prev);
        page->d_dirtyList->pushPage(page);
    }
    d_mapLock.unlock();
    return (char*)page->d_address;
}

void baecs_MappingManager::releasePage(Handle pageHandle)
{
    Page* page = reinterpret_cast<Page*>(pageHandle);
    BSLS_ASSERT(page);
    int useCount;
    while((useCount = page->d_useCount) >= 4) {
        if (useCount == page->d_useCount.testAndSwap(useCount, useCount-2))
            return;
    }

    d_mapLock.lock();
    do {
        useCount = page->d_useCount;
        BSLS_ASSERT(useCount >= 2);
    } while (useCount != page->d_useCount.testAndSwap(useCount, useCount-2));
    if (useCount < 4) {
        d_usedPages.removePage(page);
        d_unusedPages[page->d_priority].pushPage(page);
    }
    d_mapLock.unlock();
}

baecs_MappingManager::PageListHandle baecs_MappingManager::createDirtyList() {
    return new (d_pageListPool)
        baecs_MappingManager_PageList(PageList::BAECS_PAGE_LIST_DIRTY);
}

int baecs_MappingManager::clearDirtyList(PageListHandle  pageListHandle)
{
    PageList *list = reinterpret_cast<PageList*>(pageListHandle);
    int numPages = 0;
    d_mapLock.lock();
    while(!list->isEmpty()) {
        Page* page = list->popPage();
        ++numPages;
        int useCount;
        do {
            useCount = page->d_useCount;
            BSLS_ASSERT(useCount & 1);
        } while(useCount != page->d_useCount.testAndSwap(useCount,
                                                         useCount & ~1));
    }
    d_mapLock.unlock();
    return numPages;
}

int baecs_MappingManager::flushDirtyList(PageListHandle  pageListHandle,
                                         bool            isSync)
{
    PageList *list = reinterpret_cast<PageList*>(pageListHandle);
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);
    int numPages = 0;
    d_mapLock.lock();
    while(!list->isEmpty()) {
        Page* page = list->popPage();
        ++numPages;
        int useCount;
        do {
            useCount = page->d_useCount;
            BSLS_ASSERT(useCount & 1);
        } while(useCount != page->d_useCount.testAndSwap(useCount,
                                                         useCount & ~1));
        BAEL_LOG_TRACE << "Flushing page " << (void*)page
                       << " at " << (void*)page->d_address
                       << BAEL_LOG_END;

        bdesu_FileUtil::sync((char*)page->d_address, page->d_size, isSync);
    }
    d_mapLock.unlock();
    return numPages;
}

void baecs_MappingManager::deleteDirtyList(PageListHandle pageListHandle) {
    PageList *list = reinterpret_cast<PageList*>(pageListHandle);
    d_pageListPool.deleteObjectRaw(list);
}

char* baecs_MappingManager::getPageData(Handle pageHandle)
{
    Page* page = reinterpret_cast<Page*>(pageHandle);
    BSLS_ASSERT(page->d_useCount >= 2);
    BSLS_ASSERT(page->d_address);
    return (char*)page->d_address;
}

baecs_MappingManager::Handle
baecs_MappingManager::addPage(
    bdesu_FileUtil::FileDescriptor           fd,
    bdesu_FileUtil::Offset                   offset,
    bsl::size_t                              size,
    bool                                     isReadWrite,
    int                                      priority,
    PageListHandle                           pageListHandle)
{
    Page *page = new (d_pagesPool) Page;
    page->d_fd = fd;
    page->d_offset = offset;
    page->d_size = size;
    page->d_useCount = 0;
    page->d_isReadWrite = isReadWrite;
    page->d_address = 0;
    page->d_priority = priority;
    page->d_dirtyList = reinterpret_cast<PageList*>(pageListHandle);
    page->d_links[PageList::BAECS_PAGE_LIST_CLEAN].d_next = 0;
    page->d_links[PageList::BAECS_PAGE_LIST_CLEAN].d_prev = 0;
    page->d_links[PageList::BAECS_PAGE_LIST_DIRTY].d_next = 0;
    page->d_links[PageList::BAECS_PAGE_LIST_DIRTY].d_prev = 0;
    return page;
}

void baecs_MappingManager::removePage(Handle pageHandle)
{
    Page* page = reinterpret_cast<Page*>(pageHandle);
    d_mapLock.lock();
    BSLS_ASSERT(page);
    if (page->d_address) { // page is mapped
        if (page->d_useCount >= 2) {
            d_usedPages.removePage(page);
        } else {
            d_unusedPages[page->d_priority].removePage(page);
        }
        int rc = bdesu_FileUtil::unmap(page->d_address, page->d_size);
        BSLS_ASSERT(rc == 0);
        if (page->d_useCount & 1) {
            page->d_dirtyList->removePage(page);
        }
        d_totalMapped -= page->d_size;
    } else {
        // DO NOTHING
    }

    d_pagesPool.deleteObjectRaw(page);
    d_mapLock.unlock();
}

} // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
