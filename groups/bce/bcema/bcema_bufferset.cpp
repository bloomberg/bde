// bcema_bufferset.cpp -*-C++-*-
#include <bcema_bufferset.h>

#include <bdes_assert.h>
#include <bdeu_print.h>

#include <ostream>

namespace BloombergLP {
                           // ==================
                           // class bcema_Buffer
                           // ==================

// ACCESSORS
std::ostream& bcema_Buffer::print(std::ostream& stream,
                            int           level,
                            int           spacesPerLevel) const
{
    bdeu_Print::hexDump(stream, d_buffer, d_length);
    return stream << std::flush;
}


                           // =====================
                           // class bcema_BufferSet
                           // =====================

// CREATORS
bcema_BufferSet::bcema_BufferSet(bdema_Allocator *basicAllocator)
: d_buffers(basicAllocator)
, d_length(0)
{
}

bcema_BufferSet::~bcema_BufferSet()
{
}

// MANIPULATORS
bcema_Buffer& bcema_BufferSet::operator[](std::size_t index)
{
    BDE_ASSERT_CPP(index < d_buffers.size());
    return d_buffers[index];
}

void bcema_BufferSet::append(const bcema_Buffer& buffer) {
    d_buffers.push_back(buffer);
    d_length += buffer.length();
}

void bcema_BufferSet::removeAll()
{
    d_buffers.clear();
    d_length = 0;
}

void bcema_BufferSet::reserveCapacity(std::size_t numBuffers)
{
    d_buffers.reserve(numBuffers);
}

// ACCESSORS
const bcema_Buffer& bcema_BufferSet::operator[](std::size_t index) const
{
    BDE_ASSERT_CPP(index < d_buffers.size());
    return d_buffers[index];
}

std::size_t bcema_BufferSet::length() const
{
    return d_length;
}

std::size_t bcema_BufferSet::numBuffers() const
{
    return d_buffers.size();
}


                        // ============================
                        // class bcema_BufferSetManager
                        // ============================

bcema_BufferSetManager::~bcema_BufferSetManager()
{
}

} // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
