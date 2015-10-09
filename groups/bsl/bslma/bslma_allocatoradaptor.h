// bslma_allocatoradaptor.h                  -*-C++-*-
#ifndef INCLUDED_BSLMA_ALLOCATORADAPTOR
#define INCLUDED_BSLMA_ALLOCATORADAPTOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a polymorphic adaptor for STL-style allocators
//
//@CLASSES: AllocatorAdaptor<ALLOC>
//
//@SEE_ALSO:
//
//@DESCRIPTION: Within the BDE libraries, the prefered way to handle memory
// allocation is through a pointer to the polymorphic base class,
// 'bslma::Allocator'.  The use of a run-time polymorphism for the allocator
// has numerous advantages over the compile-time polymorphism used by the STL
// components.  However, there are times when client code may have an
// STL-style allocator available and needs to use it with a BDE component.
//
// This component provides a class template, 'AllocatorAdaptor' that wraps the
// STL-style allocator in an object of class derived from 'bslma::Allocator'.
// A pointer to the object can thus be used with any component that uses
// BDE-style memory allocation.
//
///Usage
///-----
// Let's start with a simple class, 'my::FilePath', which allocates storage
// using a 'bslma::Allocator':
//..
//  #include <bslma_allocator.h>
//  #include <bslma_default.h>
//  #include <bsls_nullptr.h>
//
//  #include <cstring>
//  #include <cstdlib>
//
//  namespace my {
//
//  class FilePath {
//      // Store the path of a file or directory
//      bslma::Allocator *d_allocator;
//      char             *d_data;
//
//  public:
//      FilePath(bslma::Allocator* basicAllocator = 0 /* nullptr */)
//          : d_allocator(bslma::Default::allocator(basicAllocator))
//          , d_data(0 /* nullptr */) { }
//
//      FilePath(const char* s, bslma::Allocator* basicAllocator = 0)
//          : d_allocator(bslma::Default::allocator(basicAllocator))
//      {
//          d_data =
//               static_cast<char*>(d_allocator->allocate(std::strlen(s) + 1));
//          std::strcpy(d_data, s);
//      }
//
//      bslma::Allocator *getAllocator() const { return d_allocator; }
//
//      //...
//  };
//
//  } // close namespace my
//..
// Next, assume that an STL-allocator exists that uses memory exactly the way
// you need:
//..
//  template <class TYPE>
//  class MagicAllocator {
//      bool d_useMalloc;
//  public:
//      typedef TYPE        value_type;
//      typedef TYPE       *pointer;
//      typedef const TYPE *const_pointer;
//      typedef unsigned    size_type;
//      typedef int         difference_type;
//
//      template <class U>
//      struct rebind {
//          typedef MagicAllocator<U> other;
//      };
//
//      explicit MagicAllocator(bool useMalloc = false)
//          : d_useMalloc(useMalloc) { }
//
//      template <class U>
//      MagicAllocator(const MagicAllocator<U>& other)
//          : d_useMalloc(other.getUseMalloc()) { }
//
//      value_type *allocate(std::size_t n, void* = 0 /* nullptr */) {
//          if (d_useMalloc)
//              return (value_type*) std::malloc(n * sizeof(value_type));
//          else
//              return (value_type*) ::operator new(n * sizeof(value_type));
//      }
//
//      void deallocate(value_type *p, std::size_t) {
//          if (d_useMalloc)
//              std::free(p);
//          else
//              ::operator delete(p);
//      }
//
//      static size_type max_size() { return UINT_MAX / sizeof(TYPE); }
//
//      void construct(pointer p, const TYPE& value)
//          { new((void *)p) TYPE(value); }
//
//      void destroy(pointer p) { p->~TYPE(); }
//
//      int getUseMalloc() const { return d_useMalloc; }
//  };
//
//  template <class T, class U>
//  inline
//  bool operator==(const MagicAllocator<T>& a, const MagicAllocator<U>& b)
//  {
//      return a.getUseMalloc() == b.getUseMalloc();
//  }
//
//  template <class T, class U>
//  inline
//  bool operator!=(const MagicAllocator<T>& a, const MagicAllocator<U>& b)
//  {
//      return a.getUseMalloc() != b.getUseMalloc();
//  }
//..
// Now, if we want to create a 'FilePath' using a 'MagicAllocator', we
// need to adapt the 'MagicAllocator' to the 'bslma::Allocator' protocol.
// This is where 'bslma::AllocatorAdaptor' comes in:
//..
//  int main()
//  {
//      MagicAllocator<char> ma(true);
//      bslma::AllocatorAdaptor<MagicAllocator<char> >::Type maa(ma);
//
//      my::FilePath usrbin("/usr/local/bin", &maa);
//
//      assert(&maa == usrbin.getAllocator());
//      assert(ma == maa.adaptedAllocator());
//
//      return 0;
//  }
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMF_ASSERT
#include <bslmf_assert.h>
#endif

#ifndef INCLUDED_BSLMF_ISSAME
#include <bslmf_issame.h>
#endif

#ifndef INCLUDED_BSLS_ALIGNMENTUTIL
#include <bsls_alignmentutil.h>
#endif

namespace BloombergLP {

namespace bslma {

                        // ===================================
                        // class template AllocatorAdaptor_Imp
                        // ===================================

template <class STL_ALLOC>
class AllocatorAdaptor_Imp : public Allocator {
    // Component-private class. Do not use.  This class provides the actual
    // interface and implementaiton for 'AllocatorAdaptor', which inherits
    // from it.  The indirection is necessary so that
    // 'AllocatorAdaptor<Alloc<T>>' and 'AllocatorAdaptor<Alloc<U>>' produce
    // only one instantiation of this template:
    // 'AllocatorAdaptor_imp<Alloc<char>>'.

    BSLMF_ASSERT((bsl::is_same<typename STL_ALLOC::value_type, char>::value));

    // PRIVATE TYPES
    typedef bsls::AlignmentUtil::MaxAlignedType MaxAlignedType;

    // PRIVATE DATA
    typename STL_ALLOC::template rebind<MaxAlignedType>::other d_stlAllocator;

    // NOT ASSIGNABLE
    AllocatorAdaptor_Imp& operator=(const AllocatorAdaptor_Imp&); // = delete

  public:
    // TYPES
    typedef AllocatorAdaptor_Imp Type;
    typedef STL_ALLOC            StlAllocatorType;

    // CREATORS
    AllocatorAdaptor_Imp(); // = default
        // Construct a polymorphic wrapper around a default-constructed
        // STL-style allocator.

    AllocatorAdaptor_Imp(const StlAllocatorType& stla);
        // Construct a polymorphic wrapper around a copy of the specified
        // 'stla' STL-style allocator.

    //! AllocatorAdaptor_Imp(const AllocatorAdaptor_Imp&);

    virtual ~AllocatorAdaptor_Imp();
        // Destroy this object and the STL-style allocator that it wraps.

    // MANIPULATORS
    virtual void *allocate(size_type size);
        // Return a maximally-aligned block of memory no smaller than 'size'
        // bytes allocated from the STL-style allocator that was supplied to
        // this object's constructor.  Any exceptions thrown by the underlying
        // STL-style allocator are propagated out from this member.

    virtual void deallocate(void *address);
        // Return the memory block at the specified 'address' back to the
        // STL-allocator.  If 'address' is null, this funciton has no effect.
        // The behavior is undefined unless 'address' was allocated using this
        // allocator object and has not already been deallocated.

    // ACCESSORS
    STL_ALLOC adaptedAllocator() const;
        // Return a copy of the STL allocator stored within this object.
};

                        // ===============================
                        // class template AllocatorAdaptor
                        // ===============================

#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
template <class STL_ALLOC>
using AllocatorAdaptor =
    AllocatorAdaptor_Imp<typename STL_ALLOC::template rebind<char>::other>;
    // Polymorphic wrapper around an STL-style allocator.  Note that
    // 'AllocatorAdaptor<A>::Type' is the same type regardless of whether or
    // not the compiler supports alias templates.  It should be used,
    // therefore, whenever the exact type of the adaptor is important.
#else
template <class STL_ALLOC>
class AllocatorAdaptor : public
  AllocatorAdaptor_Imp<typename STL_ALLOC::template rebind<char>::other>
{
    // Polymorphic wrapper around an object of the specified 'STL_ALLOC'
    // STL-style allocator template parameter.  A pointer to an object of this
    // class can thus be used with any component that uses BDE-style memory
    // allocation.  Note that 'AllocatorAdaptor<A>::Type' is the same type
    // regardless of whether or not the compiler supports alias templates.  It
    // should be used, therefore, whenever the exact type of the adaptor is
    // important.

    typedef typename STL_ALLOC::template rebind<char>::other ReboundSTLAlloc;

    // Not assignable
    AllocatorAdaptor& operator=(const AllocatorAdaptor&); // = delete

public:
    // CREATORS
    AllocatorAdaptor(); // = default
        // Constructs a polymorphic wrapper around a default-constructed
        // STL-style allocator.

    AllocatorAdaptor(const STL_ALLOC& stla);
        // Constructs a polymorphic wrapper around a copy of the specified
        // 'stla' STL-style allocator.

    //! AllocatorAdaptor(const AllocatorAdaptor&);
    //! ~AllocatorAdaptor();
};
#endif //  BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES

}  // close package namespace

// ===========================================================================
//                      TEMPLATE IMPLEMENTATION
// ===========================================================================

                        // -----------------------------------
                        // class template AllocatorAdaptor_Imp
                        // -----------------------------------

// CREATORS
template <class STL_ALLOC>
inline
bslma::AllocatorAdaptor_Imp<STL_ALLOC>::AllocatorAdaptor_Imp()
    : d_stlAllocator()
{
}

template <class STL_ALLOC>
inline
bslma::AllocatorAdaptor_Imp<STL_ALLOC>::AllocatorAdaptor_Imp(
                                                  const StlAllocatorType& stla)
    : d_stlAllocator(stla)
{
}

template <class STL_ALLOC>
inline
bslma::AllocatorAdaptor_Imp<STL_ALLOC>::~AllocatorAdaptor_Imp()
{
}

// MANIPULATORS
template <class STL_ALLOC>
void *bslma::AllocatorAdaptor_Imp<STL_ALLOC>::allocate(size_type size)
{
    BSLMF_ASSERT(sizeof(size_type) <= sizeof(MaxAlignedType));

    // Compute number of 'MaxAlignedType' objects needed to make up 'size'
    // bytes plus an extra one to hold the size.
    size_type n = 1 + (size+sizeof(MaxAlignedType)-1) / sizeof(MaxAlignedType);
    MaxAlignedType* p = d_stlAllocator.allocate(n);
    *reinterpret_cast<size_type*>(p) = n;
    return ++p;
}

template <class STL_ALLOC>
void bslma::AllocatorAdaptor_Imp<STL_ALLOC>::deallocate(void *address)
{
    MaxAlignedType *p = static_cast<MaxAlignedType*>(address);

    // Extract size from slot before 'p'
    size_type n = *reinterpret_cast<size_type*>(--p);
    d_stlAllocator.deallocate(p, n);
}

// ACCESSORS
template <class STL_ALLOC>
STL_ALLOC bslma::AllocatorAdaptor_Imp<STL_ALLOC>::adaptedAllocator() const
{
    return d_stlAllocator;
}

#ifndef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES

                        // -------------------------------
                        // class template AllocatorAdaptor
                        // -------------------------------

// CREATORS
template <class STL_ALLOC>
inline
bslma::AllocatorAdaptor<STL_ALLOC>::AllocatorAdaptor() // = default
{
}

template <class STL_ALLOC>
inline
bslma::AllocatorAdaptor<STL_ALLOC>::AllocatorAdaptor(const STL_ALLOC& stla)
  : bslma::AllocatorAdaptor_Imp<ReboundSTLAlloc>(stla)
{
}
#endif // ! BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES

}  // close enterprise namespace


#endif // ! defined(INCLUDED_BSLMA_ALLOCATORADAPTOR)

// ----------------------------------------------------------------------------
// Copyright (C) 2013 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
