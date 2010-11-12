// bdea_cstrarray.h              -*-C++-*-
#ifndef INCLUDED_BDEA_CSTRARRAY
#define INCLUDED_BDEA_CSTRARRAY

//@PURPOSE: Provide an in-place array of null-terminated C-style string values.
//
//@CLASSES:
//   bdea_CstrArray: memory manager for in-place array of string values (DEP)
//
//@AUTHOR: Tom Marshall (tmarshal)
//
//@SEE_ALSO: bdea_FastCstrArray
//
//@DESCRIPTION: This component implements an efficient, in-place array of
// null-terminated character string values.  Both the null string (0)
// and the empty string ("") are considered valid string values.  Hinting 
// at construction that elements of the array are either rarely or often 
// modified may improve performance.  
//
// Note that this general form of the class permits only a limited form
// of the 'reserveCapacity' method in that memory for the array mechanism 
// itself (i.e., number of strings) but not the string data comprising each
// string can be pre-allocated.  See 'bdea_fastcstrarray.h for a similar, 
// more highly specialized container type in which complete pre-allocation
// is supported.

#ifndef INCLUDED_MULTIPOOLALLOCATOR
#include <bdema_multipoolallocator.h>
#endif

#ifndef INCLUDED_SEQUENTIALALLOCATOR
#include <bdema_sequentialallocator.h>
#endif

#ifndef INCLUDED_ALLOCATOR
#include <bdema_allocator.h>
#endif

#ifndef INCLUDED_IOSFWD
#include <iosfwd>
#define INCLUDED_IOSFWD
#endif

namespace BloombergLP {

                        // ====================
                        // class bdea_CstrArray
                        // ====================

class bdea_CstrArray {
    // This class implements an efficient, in-place array-of-string type.  
    // The string entries themselves may be null (0); otherwise, they contain 
    // the address of a null-terminated sequence of 'const' characters.  
    // Hinting at construction that elements of the array are rarely or often 
    // modified may improve performance.  Note that this class is capable of 
    // reserving capacity only for the array mechanism (i.e., maximum number
    // of strings) and not the memory used to hold the string data itself (see
    // 'bdea_FastCstrArray').
    // 
    // More generally, this class supports a complete set of *value*
    // *semantic* operations, including copy construction, assignment,
    // equality comparison, 'ostream' printing, and 'bdex' serialization.  (A
    // precise operational definition of when two instances have the same
    // value can be found in the description of 'operator==' for the class.)
    // This container is *exception* *neutral* with no guarantee of rollback:
    // if an exception is thrown during the invocation of a method on a
    // pre-existing instance, the class is left in a valid state, but its
    // value is undefined.  In no event is memory leaked.  Finally, *aliasing*
    // (e.g., using all or part of an object as both source and destination)
    // is supported in all cases.

    char            **d_array_p;             // managed C-string array
    int               d_size;                // max.  length without resizing
    int               d_length;              // current number of elements
    bdema_Allocator  *d_originalAllocator_p; // constructor argument
    bdema_Allocator  *d_stringAllocator_p;   // used for string allocations
    bdema_Allocator  *d_arrayAllocator_p;    // used to allocate array (ptrs.)

    friend bool operator==(const bdea_CstrArray& lhs,
                           const bdea_CstrArray& rhs);

  private:
    // PRIVATE CLASS METHODS
    static int calculateCapacity(int numElements, int currentCapacity);
        // Return a valid capacity sufficient to accommodate the specified
        // 'numElements' given the specified 'currentCapacity'.  The behavior
        // is undefined unless 'currentCapacity' is a valid capacity for this
        // implementation and 0 <= numElements.  Note that if
        // numElements <= currentCapacity then 'currentCapacity' is returned.

  public:
    // TYPES
    enum Hint {
        // This enumeration provides a set of hints that can be supply to a
        // constructor of the c-str-array to indicate the frequency with which
        // the client expects the string data contained within the array
        // instance to be modified.  If the modifications are known to be
        // infrequent ('WRITE ONCE'), there is no need waste time and space
        // tracking the memory for each string individually.  If, on the other
        // hand, it is anticipated that there will be considerable, on-going
        // modifications, memory can be organized so as to trade off some
        // amount of space (i.e., a factor of two) to significantly improve
        // runtime performance.  Note that -- in any event -- all memory is
        // reclaimed implied allocator the array instance itself is destroyed.

        PASS_THROUGH,  // Direct immediate use of the original allocator 
                       // is the default.

        WRITE_ONCE,    // Optimizes both space and speed when few 
                       // modifications are expected.

        WRITE_MANY     // Optimizes speed at the expense of some space when 
                       // numerous modifications are expected.
    };

    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    // CREATORS
    bdea_CstrArray(bdema_Allocator *basicAllocator = 0);
        // Create an empty array of null-terminated c-style strings.  
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    bdea_CstrArray(bdea_CstrArray::Hint  usagePattern,
                   bdema_Allocator      *basicAllocator = 0);
        // Create an empty array of null-terminated c-style strings optimized
        // for the specified anticipated 'usagePattern'.  Optionally specify a 
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0, 
        // the currently installed default allocator is used.

    bdea_CstrArray(int initialLength, bdema_Allocator *basicAllocator = 0);
        // Create an array of null-terminated c-style strings having the
        // specified non-negative 'initialLength' with all elements
        // initialized to 0.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    bdea_CstrArray(int                   initialLength, 
                   bdea_CstrArray::Hint  usagePattern, 
                   bdema_Allocator      *basicAllocator = 0);
        // Create an array of null-terminated c-style strings having the
        // specified non-negative 'initialLength' with all elements initialized
        // to 0, optimized for the specified anticipated 'usagePattern'.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    bdea_CstrArray(const bdea_CstrArray&  original,
                   bdema_Allocator       *basicAllocator = 0);
        // Create an independent copy of the specified 'original' c-string
        // array.  Optionally specify a 'basicAllocator' used to supply memory.
        // If 'basicAllocator' is 0, the currently installed default allocator
        // is used.  Note that a general usage pattern is assumed for this
        // array independent of any optimization used for the 'original' array.

    bdea_CstrArray(const bdea_CstrArray&  original, 
                   bdea_CstrArray::Hint   usagePattern,
                   bdema_Allocator       *basicAllocator = 0);
        // Create an independent copy of the specified 'original' c-string
        // array optimized for the specified anticipated usage pattern.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    ~bdea_CstrArray();
        // Destroy this c-string array.

    // MANIPULATORS
    bdea_CstrArray& operator=(const bdea_CstrArray& rhs);
        // Assign to this array the value of the specified 'rhs' array.

    void append(const char *cstring);
        // Append (a copy of) the specified 'cstring' to the end of this array.
        // Note that this function is defined to be equivalent to the 
        // following: 'insert(length(), string)'.

    void append(const bdea_CstrArray& srcArray);
        // Append (copies of) all strings in the specified 'srcArray' to the
        // end of this array.  Note that this function is defined to be
        // equivalent to the following: 'insert(length(), srcArray)'.

    void append(const bdea_CstrArray& srcArray, int srcIndex, int numElements);
        // Append (copies of) the specified 'numElements' strings 
        // from the specified 'srcArray' beginning at the specified 
        // 'srcIndex' to the end of this array.  The behavior is undefined 
        // unless 0 <= 'srcIndex, 0 <= 'numElements', and 
        // 'srcIndex' + 'numElements' <= 'srcArray.length()'.
        // Note that this function is defined to be equivalent to the 
        // following: 'insert(length(), array, arrayIndex, numElements)'.

    void insert(int index, const char *string);
        // Insert (a copy of) the specified 'string' at the specified 'index'
        // position of this array.  All strings in this array with indices at 
        // or above 'index' are shifted up by one index position.  The behavior
        // is undefined unless 0 <= 'index' <= 'length'.  Note that null (0)
        // and empty ("") values for 'string' are supported.

    void insert(int dstIndex, const bdea_CstrArray& srcArray);
        // Insert (copies of) all strings in the specified 'srcArray' at the 
        // specified 'dstIndex' position of this array.  All strings in this
        // array with indices at or above 'dstIndex' are shifted up by
        // 'srcArray.length()' index positions.  The behavior is undefined
        // unless 0 <= 'dstIndex' <= 'length'

    void insert(int                   dstIndex, 
                const bdea_CstrArray& srcArray, 
                int                   srcIndex, 
                int                   numElements);
        // Insert (copies of) the specified 'numElements' strings beginning 
        // at the specified 'srcIndex' of the specified 'srcArray', at the 
        // specified 'dstIndex' position of this array.  All strings in this 
        // array with indices at or above 'dstIndex' are shifted up by 
        // 'numElements' index positions.  The behavior is undefined unless 
        // 0 <= 'dstIndex' <= 'length()', 0 <= 'srcIndex, 0 <= 'numElements', 
        // and 'srcIndex' + 'numElements' <= 'srcArray.length()'.  

    void remove(int index);
        // Remove the string at the specified 'index' position.  All strings 
        // in this array with indices above 'index' are shifted down by one 
        // index position.  The behavior is undefined unless 
        // 0 <= 'index' < 'length()'.

    void remove(int index, int numElements);
        // Remove the specified 'numElements' strings beginning at the 
        // specified 'index' position.  All strings in this array with indices 
        // above 'index' are shifted down by 'numElements' index positions.  
        // The behavior is undefined unless 0 <= 'index', 0 <= 'numElements'
        // and 'index' + 'numElements' < 'length()'.

    void removeAll();
        // Remove all elements from this array.  Note that 'length()' is now 0.

    void replace(int index, const char *cstring);
        // Replace the string at the specified 'index' position of this array
        // with (a copy of) the specified 'cstring'.  The behavior is
        // undefined unless 0 <= 'index' < 'length()'.  Note that null (0) and
        // empty ("") values for 'cstring' are supported.

    void replace(int                   dstIndex, 
                 const bdea_CstrArray& srcArray, 
                 int                   srcIndex, 
                 int                   numElements);
        // Replace the specified 'numElements' strings beginning at the
        // specified 'dstIndex' postion of this array with the sequence of 
        // (copies of) strings beginning at the specified 'srcIndex' of the 
        // specified 'srcArray'.  The behavior is undefined unless 
        // 0 <= 'dstIindex', 0 <= 'srcIndex, 0 <= 'numElements', 
        // 'dstIndex' + 'numElements' <= 'length()', and  
        // 'srcIndex' + 'numElements' <= 'srcArray.length()'.  

    void reserveCapacity(int numElements);
        // Reserve sufficient internal capacity to accommodate up to the
        // specified 'numElements' values without subsequent reallocation
        // of the array mechanism only.  Memory for string data may trigger
        // additional allocation of memory.  That is, the address returned 
        // by the 'data()' method is guaranteed not to change unless 'length()'
        // subsequently exceeds 'numElements'.  The behavior is undefined 
        // unless 0 <= 'numElements'.  Note that if 'numElements' <= length(), 
        // this operation has no effect.

#if 0
    //      This method cannot exist because we can reserve capacity for 
    //      strings only under the WRITE_ONCE allocation mode.  If that
    //      is the case, we can make a separate class and use the strpool
    //      directly WIN/WIN.  This class then becomes the vocabulary type
    //      and bdema_FastCstrArray becomes the optimized imp type with 
    //      a complete ability to reserve capacity (but not to give 
    //      back individual memory.  Making this design point is good for
    //      the class.

    void reserveCapacity(int numElements, int numCharacters);
        // Reserve sufficient internal capacity to accommodate up to the
        // specified 'numElements' strings of combinded length totaling 
        // 'numCharactrs' without any additional memory allocation.  The 
        // address returned by the 'data()' method is guaranteed not to 
        // change unless 'length()' subsequently exceeds 'numElements'.  
        // The behavior is undefined unless 0 <= 'numElements' and 
        // 0 <= 'numStrings'.  Note that if numElements <= length() the 
        // capacity of the array itself is not modified.
#endif

    void setLength(int newLength);
        // Set the length of this array to the specified 'newLength'.  If 
        // 'newLength' is greater than the current length, string values 
        // above the current length are initialized to 0 (null); otherwise,
        // all strings in this array with indices at or above 'newLength' 
        // are removed.  The behavior is undefined if unless 0 <= 'newLength'.

    void swap(int index1, int index2);
        // Efficiently swap the strings at the specified 'index1' and 'index2' 
        // index positions.  The behavior is undefined unless 
        // 0 <= 'index1' <= 'length()' and 0 <= 'index2' <= 'length()'.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, this object is valid, but its value is undefined.  If
        // 'version' is not supported, 'stream' is marked invalid and this
        // object is unaltered.  Note that no version is read from 'stream'.
        // See the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.

    // ACCESSORS
    const char * const& operator[](int index) const;
        // Return a reference to the string at the specified index.
        // The behavior is undefined if index is outside the range 
        // 0 <= index < length().

    const char * const * data() const;
        // Return the address of the internal array of character strings.
        // The value returned will remain valid until the internal capacity
        // is exceeded or this array is destroyed.

    int length() const;
        // Return the number of string elements in this array.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write this value to the specified output 'stream' using the
        // specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.
};

// FREE OPERATORS
bool operator==(const bdea_CstrArray& lhs, const bdea_CstrArray& rhs);
    // Return 1 if the specified 'lhs' and 'rhs' arrays have the same value,
    // and 0 otherwise.  Two arrays have the same value of they have the same
    // length, and the respective strings at each index position are either
    // both null or else have the same value (as defined by 'strcmp');
    
inline
bool operator!=(const bdea_CstrArray& lhs, const bdea_CstrArray& rhs);
    // Return 1 if the specified 'lhs' and 'rhs' arrays do not have the same 
    // value, and 0 otherwise.  Two arrays do not have the same value if 
    // they have different lengths, or if at at least one index position either
    // only one of the respective strings is null or else their non-null values
    // (as defined by 'strcmp') differ.

std::ostream& operator<<(std::ostream& stream, const bdea_CstrArray& rhs);
    // Write the specified 'rhs' array to the specified output 'stream' in some
    // reasonable (multi-line) format.

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

// CLASS METHODS
inline
int bdea_CstrArray::maxSupportedBdexVersion()
{
    return 1;
}

// MANIPULATORS
template <class STREAM>
inline
STREAM& bdea_CstrArray::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {    // switch on the schema version (starting with 1)
          case 1: {
            int newLength;
            stream.getLength(newLength);
            if (!stream) {
                return stream;
            }                
            if (d_stringAllocator_p != d_originalAllocator_p) {
                // 'd_stringAllocator_p' is a managed alloc.; can 'release'
                static_cast<bdema_ManagedAllocator *>(d_stringAllocator_p)
                                                                   ->release();
            }
            else { // must 'deallocate' one at a time
                for (int i = 0; i < d_length; ++i) {
                    if (char *element = d_array_p[i]) {
                        d_stringAllocator_p->deallocate(element);
                    }
                }
            }
            d_length = 0;

            if (newLength > d_size) { // Capacity is not sufficient
                // Allocate new 'temp' array -- ok if this throws.
                int newSize = calculateCapacity(newLength, d_size);
                char **temp = (char **)
                    d_arrayAllocator_p->allocate(sizeof(*d_array_p) * newSize);

                d_arrayAllocator_p->deallocate(d_array_p);
                d_array_p = temp;
                d_size    = newSize;
            }

            // Read in non-0 rhs strings, or assign 0 if the string is 0.
            for (int i = 0; i < newLength; ++i) {
                int stringSize;  // written as strlen + 1
                stream.getLength(stringSize);
                if (!stream) {
                    return stream;
                }
                if (0 == stringSize) {
                    d_array_p[i] = 0;
                }
                else {
                    char **str_p = d_array_p + i;
                    *str_p = (char *)d_stringAllocator_p->allocate(stringSize);
                    stream.getArrayInt8(*str_p, stringSize);
                    if (!stream) {
                        d_stringAllocator_p->deallocate(*str_p);
                        return stream;
                    }
                }
                ++d_length;  // COMMIT to ith element
            }
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

// ACCESSORS
inline
const char * const& bdea_CstrArray::operator[](int index) const
{
    return (const char * const) d_array_p[index];
}

inline
const char * const * bdea_CstrArray::data() const
{
    return (const char * const *) d_array_p;
}

inline
int bdea_CstrArray::length() const
{
    return d_length;
}

template <class STREAM>
inline
STREAM& bdea_CstrArray::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
          // Imp Note: To distinguish between the null string (0) and the empty
          // string (""), the written "length" is strlen + 1.  This will also
          // be convenient for 'bdexStreamIn'.

          stream.putLength(d_length); // array length
          for (int i = 0; i < d_length; ++i) {
              char *str = d_array_p[i];
              if (0 == str) {
                  stream.putLength(0);
              }
              else {
                  int length = std::strlen(str) + 1;
                  stream.putLength(length);
                  stream.putArrayInt8(str, length);
              }
          }
      } break;
    }
    return stream;
}

// FREE OPERATORS
inline
bool operator!=(const bdea_CstrArray& lhs, const bdea_CstrArray& rhs)
{
    return !(lhs == rhs);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
