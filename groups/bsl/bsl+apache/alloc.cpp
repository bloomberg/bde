/************************************************************************
 *
 * alloc.cpp - definitions of rw_alloc and rw_free
 *
 * $Id: alloc.cpp 515619 2007-03-07 15:55:37Z faridz $
 *
 ************************************************************************
 *
 * Licensed to the Apache Software  Foundation (ASF) under one or more
 * contributor  license agreements.  See  the NOTICE  file distributed
 * with  this  work  for  additional information  regarding  copyright
 * ownership.   The ASF  licenses this  file to  you under  the Apache
 * License, Version  2.0 (the  "License"); you may  not use  this file
 * except in  compliance with the License.   You may obtain  a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the  License is distributed on an  "AS IS" BASIS,
 * WITHOUT  WARRANTIES OR CONDITIONS  OF ANY  KIND, either  express or
 * implied.   See  the License  for  the  specific language  governing
 * permissions and limitations under the License.
 * 
 **************************************************************************/

// expand _TEST_EXPORT macros
#define _RWSTD_TEST_SRC

#include <stdlib.h>   // for atoi(), getenv(), malloc(), free()
#include <string.h>   // for memset()

#include <rw_alloc.h>
#include <driver.h>   // for rw_error(), rw_fatal()


#ifdef __CYGWIN__
// use the Windows API on Cygwin
#  define _WIN32
#endif

#if !defined (_WIN32) && !defined (_WIN64)
#  ifdef __SUNPRO_CC
// working around SunOS bug #568
#    include <time.h>
#  endif
#  include <unistd.h>     // for getpagesize(), sysconf()
#  include <sys/mman.h>   // for mmap()
#  include <sys/types.h>

#  ifndef _SC_PAGE_SIZE
// fall back on the alternative macro if it exists,
// or use getpagesize() otherwise
#    ifndef _SC_PAGESIZE
#      define GETPAGESIZE()   getpagesize ()
#    else
#      define GETPAGESIZE()   sysconf (_SC_PAGESIZE)
#    endif
#  else
#      define GETPAGESIZE()   sysconf (_SC_PAGE_SIZE)
#  endif   // _SC_PAGE_SIZE

// POSIX mprotect() and munmap() take void* but some legacy systems
// still declare the functions to take char* (aliased as caddr_t)
typedef _RWSTD_MUNMAP_ARG1_T CaddrT;

#else   // defined (_WIN32) || defined (_WIN64)

#  include <windows.h>    // for everything (ugh)
#  include <sys/types.h>  // for off_t
#  include <errno.h>      // for errno

#  define GETPAGESIZE()   getpagesize ()

typedef void* CaddrT;

static long
getpagesize ()
{
    static long pagesize_ = 0;

    if (0 == pagesize_) {
        SYSTEM_INFO info;
        GetSystemInfo (&info);
        pagesize_ = long (info.dwPageSize);
    }

    return pagesize_;
}

enum {
    PROT_NONE  = 0,
    PROT_READ  = 1 << 0,
    PROT_WRITE = 1 << 1,
    PROT_RDWR  = PROT_READ | PROT_WRITE,
    PROT_EXEC  = 1 << 2
};

#define MAP_PRIVATE   0
#define MAP_ANONYMOUS 0

#define MAP_FAILED ((CaddrT)-1)

static const DWORD
_rw_prots [] = {
    PAGE_NOACCESS,
    PAGE_READONLY,
    PAGE_READWRITE,
    PAGE_READWRITE,
    PAGE_EXECUTE,
    PAGE_EXECUTE_READ,
    PAGE_EXECUTE_READWRITE,
    PAGE_EXECUTE_READWRITE
};


static inline DWORD
_rw_translate_prot (int prot)
{
    if (0 <= prot && prot < sizeof _rw_prots / sizeof *_rw_prots)
        return _rw_prots[prot];

    return PAGE_NOACCESS;
}


static inline CaddrT
mmap (CaddrT addr, size_t len, int prot, int, int, off_t)
{
    addr = VirtualAlloc (addr, len, MEM_RESERVE | MEM_COMMIT,
        _rw_translate_prot (prot));

    if (addr)
        return addr;

    errno = EINVAL;
    return MAP_FAILED;
}


static inline int
munmap (CaddrT addr, size_t)
{
    if (VirtualFree (addr, 0, MEM_RELEASE))
        return 0;
    
    errno = EINVAL;
    return -1;
}


static inline int
mprotect (CaddrT addr, size_t len, int prot)
{
    DWORD flOldProt;
    if (VirtualProtect (addr, len, _rw_translate_prot (prot), &flOldProt))
        return 0;
    
    errno = EINVAL;
    return -1;
}

#endif   // _WIN{32,64}

#ifndef MAP_PRIVATE
#  define MAP_PRIVATE     0
#endif   // MAP_PRIVATE

#ifndef MAP_ANONYMOUS
   // no MAP_ANONYMOUS on IRIX64
#  define MAP_ANONYMOUS   0
#endif   // MAP_ANONYMOUS

/************************************************************************/

struct BlockInfo
{
    void*  addr_;   // address of the allocated block
                    // if addr_ == 0 block is unused
    size_t size_;   // size of the allocated block
    void*  data_;   // address of the user data
    size_t udsz_;   // size of the user data
    int    flags_;  // memory protection flags
};

struct Stats
{
    size_t blocks_;     // number of the current allocated blocks
    size_t maxblocks_;  // max number of the allocated blocks
};


static Stats
_rw_stats;


struct Blocks
{
    Blocks*   next_;
    size_t    nblocks_;
    BlockInfo blocks_[1];
};


static Blocks* _rw_head = 0;   // pointer to the first Blocks in list
static Blocks* _rw_tail = 0;   // pointer to the last Blocks in list

struct Pair
{
    void*      addr_;       // pointer to the user data in block
    BlockInfo* info_;       // pointer to the corresponding BlockInfo
};

static Pair*  _rw_table           = 0; // pointer to the table
static size_t _rw_table_size      = 0; // size of table in bytes
static size_t _rw_table_max_size  = 0; // max number of items in table

/************************************************************************/

static Pair*
_rw_lower_bound (Pair* first, Pair* last, void* addr)
{
    for (size_t dist = last - first; dist > 0; ) {

        // half = dist / 2
        const size_t half = dist >> 1;
        Pair* const middle = first + half;

        if (middle->addr_ < addr) {
            first = middle + 1;
            dist -= half + 1;
        }
        else
            dist = half;
    }

    return first;
}


static inline Pair*
_rw_binary_search (Pair* first, Pair* last, void* addr)
{
    Pair* it = _rw_lower_bound (first, last, addr);
    return (it != last && it->addr_ == addr) ? it : 0;
}

/************************************************************************/

// constructor sets r/w access to the specified memory pages
// destructor sets r/o access to the specified memory pages
class MemRWGuard
{
    CaddrT caddr_;
    size_t size_;

public:
    MemRWGuard (void* addr, size_t size) {
        static const size_t pagemask = GETPAGESIZE () - 1;

        // check that pagesize is power of 2
        RW_ASSERT (0 == ((pagemask + 1) & pagemask));

        // caddr_ should be aligned to memory page boundary
        const size_t off = size_t (addr) & pagemask;

        addr = _RWSTD_STATIC_CAST (char*, addr) - off;

        caddr_ = _RWSTD_STATIC_CAST (CaddrT, addr);
        size_  = size + off;

        const int res = mprotect (caddr_, size, PROT_READ | PROT_WRITE);
        rw_error (0 == res, __FILE__, __LINE__,
                  "mprotect failed: errno = %{#m} (%{m})");
    }

    ~MemRWGuard () {
        const int res = mprotect (caddr_, size_, PROT_READ);
        rw_error (0 == res, __FILE__, __LINE__,
                  "mprotect failed: errno = %{#m} (%{m})");
    }

private:
    // not defined
    MemRWGuard (const MemRWGuard&);
    MemRWGuard& operator= (const MemRWGuard&);
};

/************************************************************************/

static void
_rw_table_free ()
{
    if (!_rw_table)
        return;

    const CaddrT caddr = _RWSTD_REINTERPRET_CAST (CaddrT, _rw_table);

    const int res = munmap (caddr, _rw_table_size);
    rw_error (0 == res, __FILE__, __LINE__,
              "munmap(%#p, %zu) failed: errno = %{#m} (%{m})",
              _rw_table, _rw_table_size);

    _rw_table          = 0;
    _rw_table_size     = 0;
    _rw_table_max_size = 0;
}


static bool
_rw_table_grow ()
{
    // _rw_table_max_size cannot be less than allocated blocks
    RW_ASSERT (_rw_table_max_size >= _rw_stats.blocks_);

    // check for free space in current table
    if (_rw_table_max_size == _rw_stats.blocks_) {
        // realloc more memory
        static const size_t pagesize = GETPAGESIZE ();

        const size_t new_size = _rw_table_size + pagesize;

        const CaddrT caddr = mmap (0, new_size,
                                   PROT_READ | PROT_WRITE,
                                   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

        if (MAP_FAILED == caddr)
            // no memory available
            return false;

        // copy old table
        memcpy (caddr, _rw_table, _rw_stats.blocks_ * sizeof (Pair));

        // protect the new table
        const int res = mprotect (caddr, new_size, PROT_READ);
        rw_error (0 == res, __FILE__, __LINE__,
                  "mprotect(%#p, %zu, PROT_READ) failed: errno = %{#m} (%{m})",
                  caddr, new_size);

        // free old table
        _rw_table_free ();

        _rw_table          = _RWSTD_REINTERPRET_CAST (Pair*, caddr);
        _rw_table_size     = new_size;
        _rw_table_max_size = new_size / sizeof (Pair);
    }

    return true;
}


// inserts info about newly created BlockInfo
// increments the number of the allocated blocks
static void
_rw_table_insert (BlockInfo& info)
{
    Pair* const begin = _rw_table;
    Pair* const end   = begin + _rw_stats.blocks_;
    Pair* const it    = _rw_lower_bound (begin, end, info.data_);

    {
        MemRWGuard guard (_rw_table, _rw_table_size);

        // move items [it, end) to the end of table
        memmove (it + 1, it, (end - it) * sizeof (Pair));

        it->addr_ = info.data_;
        it->info_ = &info;
    }

    ++_rw_stats.blocks_;
    if (_rw_stats.blocks_ > _rw_stats.maxblocks_)
        _rw_stats.maxblocks_ = _rw_stats.blocks_;
}


// removes the specified item from table
// decrements the number of the allocated blocks
static void
_rw_table_remove (Pair* it)
{
    const size_t index = size_t (it - _rw_table);

    rw_fatal (index < _rw_stats.blocks_, __FILE__, __LINE__,
              "invalid index in _rw_table_remove: %zu", index);

    MemRWGuard guard (_rw_table, _rw_table_size);
    memmove (it, it + 1, (--_rw_stats.blocks_ - index) * sizeof (Pair));
}

/************************************************************************/

// allocate more blocks (allocates one memory page)
static bool
_rw_allocate_blocks ()
{
    // count of the blocks per memory page
    static size_t blocks_per_page = 0;

    static const size_t pagesize = GETPAGESIZE ();

    if (0 == blocks_per_page)
        blocks_per_page = (pagesize - sizeof (Blocks)) / sizeof (BlockInfo) + 1;

    const CaddrT caddr = mmap (0, pagesize,
                               PROT_READ | PROT_WRITE,
                               MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (MAP_FAILED != caddr) {

        memset (caddr, 0, pagesize);

        Blocks* const blocks = _RWSTD_REINTERPRET_CAST (Blocks*, caddr);

        blocks->nblocks_ = blocks_per_page;

        // set r/o access to the new page
        const int res = mprotect (caddr, pagesize, PROT_READ);
        rw_error (0 == res, __FILE__, __LINE__,
                  "mprotect(%#p, %zu, PROT_READ) failed: errno = %{#m} (%{m})",
                  caddr, pagesize);

        if (0 == _rw_head)
            _rw_head = blocks;
        else {
            MemRWGuard guard (&_rw_tail->next_, sizeof (_rw_tail->next_));
            _rw_tail->next_ = blocks;
        }

        _rw_tail = blocks;

        return true;
    }

    return false;
}


// free allocated blocks
// should be called when all user memory allocated
// by rw_alloc were freed by rw_free
static void
_rw_free_blocks ()
{
    rw_fatal (0 == _rw_stats.blocks_, __FILE__, __LINE__,
              "_rw_free_blocks called when %zu blocks are not freed",
              _rw_stats.blocks_);

    static const size_t pagesize = GETPAGESIZE ();

    while (_rw_head) {
        const CaddrT caddr = _RWSTD_REINTERPRET_CAST (CaddrT, _rw_head);

        _rw_head = _rw_head->next_;

        const int res = munmap (caddr, pagesize);

        rw_error (0 == res, __FILE__, __LINE__,
                  "munmap(%#p, %zu) failed: errno = %{#m} (%{m})",
                  caddr, pagesize);
    }

    _rw_tail = 0;
}


static BlockInfo*
_rw_find_unused_from (Blocks* it)
{
    while (it) {
        for (size_t i = 0; i < it->nblocks_; ++i) {
            BlockInfo & info = it->blocks_ [i];
            if (0 == info.addr_)
                return &info;
        }

        it = it->next_;
    }

    return 0;
}


// returns pointer to the first unused BlockInfo
// if none unused items tries allocate more blocks
// returns 0 if no memory
static BlockInfo*
_rw_find_unused ()
{
    BlockInfo * res = _rw_find_unused_from (_rw_head);

    if (!res && _rw_allocate_blocks ()) {
        // find the unused block from newly allocated blocks
        // res = _rw_find_unused_from (_rw_tail);
        res = _rw_tail->blocks_;
        // res should be != 0
        rw_fatal (0 != res, __FILE__, __LINE__,
                  "logic error in _rw_find_unused: res == 0 after "
                  "_rw_allocate_blocks() succeeded ");
    }

    return res;
}


// returns pointer to the Pair which corresponds to addr
// returns 0 if addr is not valid pointer, returned by rw_alloc
static Pair*
_rw_find_by_addr (void* addr)
{
    Pair* const end = _rw_table + _rw_stats.blocks_;
    return _rw_binary_search (_rw_table, end, addr);
}


static inline int
_rw_get_prot (int flags)
{
    return (flags & RW_PROT_READ  ? PROT_READ  : 0)
         | (flags & RW_PROT_WRITE ? PROT_WRITE : 0);
}


static inline int
_rw_getenvflags ()
{
    const char* const envvar = getenv ("RWSTD_ALLOC_FLAGS");
    if (envvar)
        return atoi (envvar);

    return 0;
}

/************************************************************************/

_TEST_EXPORT void*
rw_alloc (size_t nbytes, int flags /* = -1 */)
{
    static const int RWSTD_ALLOC_FLAGS = _rw_getenvflags ();

    // redefine flags if environment variable was set
    if (-1 == flags && 0 != RWSTD_ALLOC_FLAGS)
        flags = RWSTD_ALLOC_FLAGS;

    // make sure that table has free space
    if (!_rw_table_grow ())
        return 0;

    if (BlockInfo *info = _rw_find_unused ()) {

        BlockInfo newinfo = BlockInfo ();

        newinfo.udsz_ = nbytes;
        newinfo.flags_ = flags;

        if (-1 == flags) {

            newinfo.addr_ = malloc (nbytes);

            if (newinfo.addr_) {
                newinfo.size_ = nbytes;
                newinfo.data_ = newinfo.addr_;
            }
        }
        else {
            static const size_t pagesize = GETPAGESIZE ();

            size_t size = nbytes + pagesize;

            // check that pagesize is power of 2
            RW_ASSERT (0 == (pagesize & (pagesize - 1)));
            size_t offset = size & (pagesize - 1);

            if (offset) {
                offset = pagesize - offset;
                size += offset;
            }

            newinfo.addr_ = mmap (0, size, _rw_get_prot(flags),
                                  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

            if (MAP_FAILED != newinfo.addr_) {

                char* data = _RWSTD_STATIC_CAST (char*, newinfo.addr_);
                char* guard = data;
                
                if (RW_PROT_BELOW & flags)
                    offset = pagesize;
                else
                    guard += size - pagesize;

                // deny access to the guard page
                int res = mprotect (guard, pagesize, PROT_NONE);
                rw_error (0 == res, __FILE__, __LINE__,
                          "mprotect failed: errno = %{#m} (%{m})");

                newinfo.size_ = size;
                newinfo.data_ = data + offset;
            }
        }

        if (newinfo.data_) {

            {
                MemRWGuard guard (info, sizeof (*info));
                *info = newinfo;
            }

            _rw_table_insert (*info);

            return info->data_;
        }
    }

    return 0;
}

/************************************************************************/

_TEST_EXPORT void
rw_free (void* addr)
{
    if (Pair *it = _rw_find_by_addr (addr)) {

        BlockInfo &info = *it->info_;

        if (-1 == info.flags_)
            free (addr);
        else {
            const CaddrT caddr = _RWSTD_REINTERPRET_CAST (CaddrT, info.addr_);

            const int res = munmap (caddr, info.size_);
            rw_error (0 == res, __FILE__, __LINE__,
                      "munmap(%#p, %zu) failed: errno = %{#m} (%{m})",
                      caddr, info.size_);
        }

        {
            MemRWGuard guard (&info, sizeof (info));
            info = BlockInfo ();
        }

        _rw_table_remove (it);

        if (0 == _rw_stats.blocks_) {
            _rw_free_blocks ();
            _rw_table_free ();
        }
    }
    else
        rw_error (0 == addr, __FILE__, __LINE__,
                  "rw_free(%#p): the address is not a valid address, "
                  "returned by rw_alloc()",
                  addr);
}
