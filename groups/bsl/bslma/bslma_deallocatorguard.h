// bslma_deallocatorguard.h                                           -*-C++-*-
#ifndef INCLUDED_BSLMA_DEALLOCATORGUARD
#define INCLUDED_BSLMA_DEALLOCATORGUARD

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a guard to unconditionally manage a block of memory.
//
//@CLASSES:
//  bslma::DeallocatorGuard: guard to unconditionally manage a block of memory
//
//@SEE_ALSO: bslma_deallocatorproctor, bslma_autodeallocator
//
//@DESCRIPTION: This component provides a guard class template to
// unconditionally manage a block of (otherwise-unmanaged) memory.  The managed
// memory is deallocated automatically when the guard object goes out of scope
// using the 'deallocate' method of the parameterized 'ALLOCATOR' (allocator or
// pool) supplied at construction.
//
///Requirement
///-----------
// The parameterized 'ALLOCATOR' type of the 'bslma::DeallocatorGuard' class
// template must provide a (possibly 'virtual') method:
//..
//  void deallocate(void *address);
//..
// to deallocate memory at the specified 'address' (originally supplied by the
// 'ALLOCATOR' object).
//
///Usage
///-----
// A 'bslma::DeallocatorGuard' can be used to ensure that a dynamically
// allocated raw memory resource is safely deallocated in the presence of
// multiple return statements or exceptions in an exception-neutral way (i.e.,
// without the need for 'try'/'catch' blocks).  In this simple example,
// consider the function 'evaluatePassword' which attempts to determine how
// secure a given password might be:
//..
//  double evaluatePassword(const char *password, bslma::Allocator *allocator);
//      // Evaluate the strength of the specified 'password', using the
//      // specified 'allocator' to supply memory for evaluation.  Return a
//      // real value in the range '[ 0.0 .. 1.0 ]' where 0.0 indicates the
//      // weakest password, and 1.0 the strongest.
//..
// This function will be implemented in terms of three *exception* *neutral*
// subroutines, each of which operates on a writable copy of the
// null-terminated password, (perturbing its contents slightly) and requiring
// unbounded amounts of scratch memory (to be allocated and deallocated from a
// supplied allocator):
//..
//  int subroutine1(char *inOut, bslma::Allocator *allocator);
//  int subroutine2(char *inOut, bslma::Allocator *allocator);
//  int subroutine3(char *inOut, bslma::Allocator *allocator);
//..
// A final subroutine is then used to determine and return the score:
//..
//  double finalSubroutine(const char *result);
//..
// The top-level routine is implemented as follows:
//..
//  double evaluatePassword(const char *password, bslma::Allocator *allocator)
//  {
//
//      // Set up local writable copy of password in buffer.
//
//      size_t size = strlen(password) + 1;
//      char *buffer = (char *)allocator->allocate(size);
//      memcpy(buffer, password, size);
//
//      //**************************************************************
//      //* Note the use of the deallocator guard on 'buffer' (below). *
//      //**************************************************************
//
//      bslma::DeallocatorGuard<bslma::Allocator> guard(buffer, allocator);
//
//      // Process and evaluate the supplied password.
//
//      if (0 != subroutine1(buffer, allocator)) {
//          return 0.0;                                               // RETURN
//      }
//      if (0 != subroutine2(buffer, allocator)) {
//          return 0.2;                                               // RETURN
//      }
//      if (0 != subroutine3(buffer, allocator)) {
//          return 0.4;                                               // RETURN
//      }
//
//      return finalSubroutine(buffer);
//
//  }  // note that 'buffer' is deallocated at end of block regardless
//..
// Notice that if any of the initial (numbered) subroutines returns a non-zero
// status value, the top-level 'evaluatePassword' routine returns immediately
// with a predetermined score.  Moreover, each of these routines may encounter
// a 'bad_alloc' exception should the supplied allocator fail to return the
// requested memory.  Even if all of these subroutines evaluates successfully,
// the score calculated using 'finalEval' is returned directly by
// 'evaluatePassword', yet we still need to deallocate 'buffer'.  By guarding
// buffer with a 'bslma::DeallocatorGuard' as shown above, all of these issues
// are fully addressed, and the top-level routine is also *exception* *neutral*
// as desired.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

namespace BloombergLP {

namespace bslma {

                        // ======================
                        // class DeallocatorGuard
                        // ======================

template <class ALLOCATOR>
class DeallocatorGuard {
    // This class implements a guard that unconditionally deallocates a block
    // of managed memory upon destruction by invoking the 'deallocate' method
    // of an allocator (or pool) of parameterized 'ALLOCATOR' type supplied to
    // it at construction.  The managed memory must have been supplied by the
    // allocator (or pool), which must remain valid throughout the lifetime of
    // the guard object.

    // DATA
    void      *d_memory_p;     // address of managed memory
    ALLOCATOR *d_allocator_p;  // allocator or pool (held, not owned)

    // NOT IMPLEMENTED
    DeallocatorGuard(const DeallocatorGuard&);
    DeallocatorGuard& operator=(const DeallocatorGuard&);

  public:
    // CREATORS
    DeallocatorGuard(void *memory, ALLOCATOR *allocator);
        // Create a deallocator guard that unconditionally manages the
        // specified 'memory' block, and that uses the specified 'allocator' to
        // deallocate 'memory' upon destruction of this guard.  The behavior is
        // undefined unless 'memory' and 'allocator' are non-zero, and
        // 'allocator' supplied 'memory'.  Note that 'allocator' must remain
        // valid throughout the lifetime of this guard.

    ~DeallocatorGuard();
        // Destroy this deallocator guard and deallocate the block of memory it
        // manages by invoking the 'deallocate' method of the allocator (or
        // pool) that was supplied with the address of the (managed) memory at
        // construction.
};

// ============================================================================
//                      INLINE AND TEMPLATE FUNCTION DEFINITIONS
// ============================================================================

                        // ----------------------
                        // class DeallocatorGuard
                        // ----------------------

// CREATORS
template <class ALLOCATOR>
inline
DeallocatorGuard<ALLOCATOR>::DeallocatorGuard(void      *memory,
                                              ALLOCATOR *allocator)
: d_memory_p(memory)
, d_allocator_p(allocator)
{
    BSLS_ASSERT_SAFE(memory);
    BSLS_ASSERT_SAFE(allocator);
}

template <class ALLOCATOR>
inline
DeallocatorGuard<ALLOCATOR>::~DeallocatorGuard()
{
    BSLS_ASSERT_SAFE(d_memory_p);
    BSLS_ASSERT_SAFE(d_allocator_p);

    d_allocator_p->deallocate(d_memory_p);
}

}  // close package namespace


}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
