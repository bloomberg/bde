// bcema_bufferset.h   -*-C++-*-
#ifndef INCLUDED_BCEMA_BUFFERSET
#define INCLUDED_BCEMA_BUFFERSET

//@PURPOSE: Provide a buffer manager classes
//
//@CLASSES:
//  bcema_Buffer: in-core buffer representation
//  bcema_BufferSet: place holder for bcema_Buffer
//  bcema_BufferSetManager: protocol
//
//@SEE_ALSO:
//  bcema_blob
//
//@AUTHOR: Guillaume Morin (gmorin1)
//
//@DESCRIPTION:
//
///Usage
///-----
//

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_IOFWD
#include <iosfwd>
#define INCLUDED_IOSFWD
#endif

#ifndef INCLUDED_VECTOR
#include <vector>
#define INCLUDED_VECTOR
#endif

namespace BloombergLP {

class bdema_Allocator;


                           // ==================
                           // class bcema_Buffer
                           // ==================
class bcema_Buffer {
    // 'bcema_Buffer' is a simple in-core representation of a buffer.  This
    // class is exception-neutral with no guarantee of rollback: if an
    // exception is thrown during the invocation of a method on a pre-existing
    // instance, the container is left in a valid state, but its value is
    // undefined.  In no event is memory leaked.

    char       *d_buffer; // buffer (held)
    std::size_t d_length; // length of the buffer

  private:
    // FRIENDS
    friend bool operator==(const bcema_Buffer& lhs, const bcema_Buffer& rhs);
    friend bool operator!=(const bcema_Buffer& lhs, const bcema_Buffer& rhs);

  public:
    // CREATORS
    bcema_Buffer();
        // Create a 'bcema_Buffer' objet representing a 0-length buffer.

    bcema_Buffer(char *buffer, std::size_t length);
        // Create a 'bcema_Buffer' object representing the specified 'buffer'
        // of the specified 'length'.  Note that the specified 'buffer' is
        // only *held* by this container.

    bcema_Buffer(const bcema_Buffer& original);
        // Create an object of type 'bcema_Buffer' having the value of the
        // specified 'original' object.

    ~bcema_Buffer();
        // Destroy this object.

    // MANIPULATORS
    bcema_Buffer& operator=(const bcema_Buffer& rhs);
        // Assign to this object the value of the specified 'rhs' object.

    char *buffer();
        // Return a pointer to the buffer represented by this object.

    void setBuffer(char *buffer, std::size_t length);
        // Set the buffer represented by this object to the specified 'buffer'
        // of the specified 'length'.  Note that the specified 'buffer' is
        // only *held* by this container.


    // ACCESSORS
    std::ostream& print(std::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object as an hexadecimal dump on the specified 'stream'.
        // Note that the 'level' and 'spacesPerLevel' arguments are specified
        // for interface compatibility only and are effectively ignored.

    const char *buffer() const;
        // Return a const-pointer to the buffer.

    std::size_t length() const;
        // Return the length of the buffer.
};

// FREE OPERATORS
inline
bool operator==(const bcema_Buffer& lhs, const bcema_Buffer& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const bcema_Buffer& lhs, const bcema_Buffer& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
std::ostream& operator<<(std::ostream& stream, const bcema_Buffer& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.


                           // =====================
                           // class bcema_BufferSet
                           // =====================

class bcema_BufferSet {
    // 'bcema_BufferSet' is a indexed placeholder for 'bcema_Buffer' object.
    // This class only holds the buffers but do not own them.  Users are
    // responsible for de-allocating the memory. This
    // class is exception-neutral with no guarantee of rollback: if an
    // exception is thrown during the invocation of a method on a pre-existing
    // instance, the container is left in a valid state, but its value is
    // undefined.  In no event is memory leaked.  This class is *not*
    // threadsafe.

    std::vector<bcema_Buffer> d_buffers; // buffers
    std::size_t               d_length;  // cached length

    private:
    // not implemented
    bcema_BufferSet(const bcema_BufferSet&);
    bcema_BufferSet& operator=(const bcema_BufferSet&);

    public:
    // CREATORS
    explicit bcema_BufferSet(bdema_Allocator *basicAllocator = 0);
        // Create an empty 'bcema_BufferSet' object.  Optionally specify
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    ~bcema_BufferSet();
        // Destroy this object.

    // MANIPULATORS
    bcema_Buffer& operator[](std::size_t index);
        // Return the 'bcema_Buffer' contained in this object and indexed
        // by the specified 'index'.  The behavior is undefined unless
        // 'index < numBuffers()'.

    void append(const bcema_Buffer& buffer);
        // Add the specified 'buffer' to the set.  Note that this container
        // only holds buffers but does not own them.

    void removeAll();
        // Remove all buffers in this containers.

    void reserveCapacity(std::size_t numBuffers);
        // Reserve the specified 'numBuffers' in this container.


    // ACCESSORS
    const bcema_Buffer& operator[](std::size_t index) const;
        // Return the 'bcema_Buffer' contained in this object and indexed
        // by the specified 'index'.  The behavior is undefined unless
        // 'index < numBuffers()'.

    std::size_t length() const;
        // Return the number of total number of bytes of all buffers in this
        // object.

    std::size_t numBuffers() const;
        // Return the number of buffers in this object.
};


                        // ============================
                        // class bcema_BufferSetManager
                        // ============================

class bcema_BufferSetManager {
    //

    public:
    // CREATORS
    virtual ~bcema_BufferSetManager();
        // Destroy this object.

    // MANIPULATORS
    virtual void deallocateBuffers(bcema_BufferSet *set) = 0;
        // Deallocate all buffers contained in the specified 'set'.

    virtual void growBufferSet(bcema_BufferSet *set, std::size_t length) = 0;
        // Grow the specified 'set' so it can contain at least the specified
        // 'length' number of bytes.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================


                           // ==================
                           // class bcema_Buffer
                           // ==================

// CREATORS
inline
bcema_Buffer::bcema_Buffer()
: d_buffer(0)
, d_length(0)
{
}

inline
bcema_Buffer::bcema_Buffer(char *buffer, std::size_t length)
: d_buffer(buffer)
, d_length(length)
{
}

inline
bcema_Buffer::bcema_Buffer(const bcema_Buffer& original)
: d_buffer(original.d_buffer)
, d_length(original.d_length)
{
}

inline
bcema_Buffer::~bcema_Buffer()
{
}

// MANIPULATORS
inline
bcema_Buffer& bcema_Buffer::operator=(const bcema_Buffer& rhs)
{
    d_buffer = rhs.d_buffer;
    d_length = rhs.d_length;
    return *this;
}

inline
char *bcema_Buffer::buffer()
{
    return d_buffer;
}

inline
void bcema_Buffer::setBuffer(char *buffer, std::size_t length)
{
    d_buffer = buffer;
    d_length = length;
}

// ACCESSORS
inline
const char *bcema_Buffer::buffer() const
{
    return d_buffer;
}

inline
std::size_t bcema_Buffer::length() const
{
    return d_length;
}

// FREE OPERATORS
inline
bool operator==(const bcema_Buffer& lhs, const bcema_Buffer& rhs)
{
    return  lhs.d_buffer == rhs.d_buffer
         && lhs.d_length == rhs.d_length;
}

inline
bool operator!=(const bcema_Buffer& lhs, const bcema_Buffer& rhs)
{
    return  lhs.d_buffer != rhs.d_buffer
         || lhs.d_length != rhs.d_length;
}

inline
std::ostream& operator<<(std::ostream& stream, const bcema_Buffer& rhs)
{
    return rhs.print(stream);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
