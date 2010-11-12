// bcemt_threadattributesimpl_win32.cpp                               -*-C++-*-
#include <bcemt_threadattributesimpl_win32.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcemt_threadattributesimpl_win32_cpp,"$Id$ $CSID$")

#ifdef BCES_PLATFORM__WIN32_THREADS
#include <windows.h>
#endif

namespace BloombergLP {

        // -------------------------------------------------------------
        // class bcemt_ThreadAttributesImpl<bces_Platform::Win32Threads>
        // -------------------------------------------------------------

// CREATORS
bcemt_ThreadAttributesImpl<bces_Platform::Win32Threads>::
                                                   bcemt_ThreadAttributesImpl()
: d_stackSize(0x100000)
, d_priority(0)
, d_guardSize(0)
, d_schedulingPolicy(BCEMT_SCHED_OTHER)
, d_detachedState(BCEMT_CREATE_JOINABLE)
, d_inheritSchedule(1)
{
#ifdef BCES_PLATFORM__WIN32_THREADS
    // obtain default stack reserve size from the PE header
    char *imageBase = (char *)GetModuleHandle(NULL);
    if (imageBase) {
        IMAGE_OPTIONAL_HEADER *header =
            (IMAGE_OPTIONAL_HEADER *) (imageBase
                + ((IMAGE_DOS_HEADER *) imageBase)->e_lfanew
                + sizeof (IMAGE_NT_SIGNATURE) + sizeof (IMAGE_FILE_HEADER));
        d_stackSize = header->SizeOfStackReserve;
    }
#endif
}

bcemt_ThreadAttributesImpl<bces_Platform::Win32Threads>::
                                                    bcemt_ThreadAttributesImpl(
      const bcemt_ThreadAttributesImpl<bces_Platform::Win32Threads>& attribute)
: d_stackSize(attribute.d_stackSize)
, d_priority(attribute.d_priority)
, d_guardSize(attribute.d_guardSize)
, d_schedulingPolicy(attribute.d_schedulingPolicy)
, d_detachedState(attribute.d_detachedState)
, d_inheritSchedule(attribute.d_inheritSchedule)
{
}

// MANIPULATORS
bcemt_ThreadAttributesImpl<bces_Platform::Win32Threads>&
bcemt_ThreadAttributesImpl<bces_Platform::Win32Threads>::operator=(
            const bcemt_ThreadAttributesImpl<bces_Platform::Win32Threads>& rhs)
{
    if (this != &rhs) {
        d_stackSize        = rhs.d_stackSize;
        d_priority         = rhs.d_priority;
        d_guardSize        = rhs.d_guardSize;
        d_schedulingPolicy = rhs.d_schedulingPolicy;
        d_detachedState    = rhs.d_detachedState;
        d_inheritSchedule  = rhs.d_inheritSchedule;
    }
    return *this;
}

// FREE OPERATORS
bool
operator==(const bcemt_ThreadAttributesImpl<bces_Platform::Win32Threads>& lhs,
           const bcemt_ThreadAttributesImpl<bces_Platform::Win32Threads>& rhs)
{
    return lhs.stackSize()          == rhs.stackSize()
        && lhs.guardSize()          == rhs.guardSize()
        && lhs.schedulingPolicy()   == rhs.schedulingPolicy()
        && lhs.schedulingPriority() == rhs.schedulingPriority()
        && lhs.inheritSchedule()    == rhs.inheritSchedule()
        && lhs.detachedState()      == rhs.detachedState();
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
