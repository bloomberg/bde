// bdefr_vfunc6.h               -*-C++-*-
#ifndef INCLUDED_BDEFR_VFUNC6
#define INCLUDED_BDEFR_VFUNC6

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")


//@PURPOSE: Provide a common reference-counted base class representation.
//
//@CLASSES:
//   bdefr_Vfunc6: reference-counted base class with an allocator
//
//@AUTHOR: John Lakos (jlakos)
//
//@DEPRECATED: This component should not be used in new code, and will be
// deleted in the near future.  Please see 'bdef_function', 'bdef_bind', etc.
// for alternatives that should be used for all future development.
//
//@DESCRIPTION: This component defines the common (partially implemented) base
// class for all internal representations of the 'bdef_Vfunc6' family of
// function objects (functors).  This abstract base class declares the pure
// virtual 'execute' method, whose signature characterizes this family of
// functor representations, while exploiting structural inheritance to
// implement efficient (inline) count manipulation.  The count is intended to
// reflect the number of 'bdef_Vfunc6' objects (envelopes) or other partial
// owners that are currently using this functor representation (letter) and is
// manipulated by each owner accordingly.  The class also provides a 'static'
// 'deleteObject' method to allow clients to destroy the object (when the count
// reaches '0') without any information about the details of the 'bdefr_Vfunc6'
// object memory management scheme.  Note that the object must be allocated
// dynamically using the same allocator supplied at construction and that the
// allocator must remain valid through the life of the object.  The existence
// of the 'static' 'deleteObject' method allows concrete classes derived from
// 'bdefr_Vfunc6' to declare the destructor 'private', thereby limiting object
// instantiation to the heap.
//
///USAGE
///-----
// This example demonstrates the essential functionality of the common
// base-class representation.  We will need two global counters for this
// demonstration:
//..
//   static int executeUsageCounter = 0;
//   static int dtorUsageCounter = 0;
//..
// Implement a (templatized) 'ConcreteDerivedClass' derived from
// 'bdefr_Vfunc6':
//..
//   template <class A1, class A2, class A3, class A4, class A5, class A6>
//   class ConcreteDerivedClass : public bdefr_Vfunc6 <A1, A2, A3, A4, A5, A6>
//   {
//     public:
//       ConcreteDerivedClass(bslma_Allocator *basicAllocator)
//       : bdefr_Vfunc6(basicAllocator) { }
//
//       virtual void execute(const A1& argument1,
//                            const A2& argument2,
//                            const A3& argument3,
//                            const A4& argument4,
//                            const A5& argument5,
//                            const A6& argument6) const
//           // Increment the global counter 'executeUsageCounter'.
//       {
//           ++executeUsageCounter;
//       }
//
//     private:
//       virtual ~ConcreteDerivedClass()
//           // Destroy the class instance.  Increment the global
//           // 'dtorUsageCounter'.
//       {
//           ++dtorUsageCounter;
//       }
//   };
//..
// Create an envelope class 'EnvelopeClass' that shares joint ownership of a
// common (instantiated) 'bdefr_Vfunc6<A1, A2, A3, A4, A5, A6>' template
// object:
//..
//   template <class A1, class A2, class A3, class A4, class A5, class A6>
//   class EnvelopeClass {
//       // Provide an object that encapsulates a 'bdefr_Vfunc6' object.
//
//       bdefr_Vfunc6<A1, A2, A3, A4, A5, A6> *d_rep_p;
//           // polymorphic functor representation
//
//     public:
//       // CREATORS
//       EnvelopeClass(bdefr_Vfunc6<A1, A2, A3, A4, A5, A6> *rep) :
//           d_rep_p(rep)
//           // Create a functor that assumes shared ownership of the
//           // specified, dynamically allocated, reference-counted
//           // representation.
//       {
//           if (d_rep_p) {
//               d_rep_p->increment();
//           }
//       }
//
//       ~EnvelopeClass()
//           // Decrement the reference count of that internal representation
//           // object, and, if the count is now 0, destroy and deallocate the
//           // representation using 'deleteObject' method of 'bdefr_Vfunc6'
//           // class.
//       {
//           if (0 == d_rep_p->decrement()) {
//               bdefr_Vfunc6<A1, A2, A3, A4, A5, A6>::
//                   deleteObject(d_rep_p);
//           }
//       }
//
//       // ACCESSORS
//       void operator()(const A1& argument1,
//                       const A2& argument2,
//                       const A3& argument3,
//                       const A4& argument4,
//                       const A5& argument5,
//                       const A6& argument6) const
//           // Execute this functor.
//       {
//           d_rep_p->execute(argument1, argument2, argument3, argument4,
//                            argument5, argument6);
//       }
//   };
//..
// Then the body of the main program is as follows:
//..
//   executeUsageCounter = 0;
//   dtorUsageCounter = 0;
//
//   typedef ConcreteDerivedClass<int, int, int,
//                                int, int, int> DerivedObj;
//   typedef bdefr_Vfunc6<int, int, int, int, int, int> Obj;
//   bslma_Allocator *myAllocator = bslma_Default::defaultAllocator();
//
//   Obj *x = new(myAllocator) DerivedObj(myAllocator);
//   {
//                                                           //  REF  USE  DTOR
//                                                           //  ---  ---  ----
//       // The reference counter is 0.                      //   NA   0     0
//       EnvelopeClass<int, int, int, int, int, int> env0(x);
//                                                           //    1   0     0
//       // The reference counter is 1.
//       env0(1, 2, 3, 4, 5, 6);  assert(1 == executeUsageCounter);
//                                                           //    1   1     0
//       {
//           EnvelopeClass<int, int, int, int, int, int> env1(x);
//                                                           //    2   1     0
//           // The reference counter is 2.
//           env1(1, 2, 3, 4, 5, 6);  assert(2 == executeUsageCounter);
//                                                           //    2   2     0
//       }
//       // The reference counter is 1.
//       assert(0 == dtorUsageCounter);                      //    1   2     1
//   }
//   assert(1 == dtorUsageCounter);                          //   NA   2     2
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

namespace BloombergLP {

class bslma_Allocator;

                        // ==================
                        // class bdefr_Vfunc6
                        // ==================

template <class A1, class A2, class A3, class A4, class A5, class A6>
class bdefr_Vfunc6 {
    // Common (partially-implemented) abstract base class declaring the
    // characteristic pure virtual 'execute' method and exploiting structural
    // inheritance to achieve efficient (inline) count manipulation.  This
    // class also implements a 'static' 'deleteObject' method.  This method
    // facilitates the use of 'bdefr_Vfunc6' as a reference-counted letter
    // class in the envelope-letter pattern.  'deleteObject' is used to destroy
    // and deallocate the concrete objects derived from 'bdefr_Vfunc6', which
    // allows the derived classes to declare the destructor 'private', and
    // limit an object instantiation to the heap.

    int d_count;                    // dumb data (number of active references)
    bslma_Allocator *d_allocator_p; // holds (but doesn't own) memory allocator

  private:
    bdefr_Vfunc6(const bdefr_Vfunc6&);                  // not implemented
    bdefr_Vfunc6& operator=(const bdefr_Vfunc6&);       // not implemented

  protected:
    virtual ~bdefr_Vfunc6();
        // Destroy this object.  This method is declared 'protected' to allow
        // derivation from this class, yet disallow direct destruction of the
        // (derived) concrete object.  Clients must use the 'static'
        // 'deleteObject' method to destroy and deallocate the derived-class
        // object from its base-class pointer.

  public:
    // CLASS METHODS
    static void deleteObject(bdefr_Vfunc6 *object);
        // Destroy the specified 'object' and use the memory allocator held by
        // 'object' to deallocate it.  The behavior is undefined unless
        // 'object' holds a valid memory allocator.

    // CREATORS
    bdefr_Vfunc6(bslma_Allocator *basicAllocator);
        // Create the base portion of a functor object that holds the specified
        // 'basicAllocator' and that has its reference count set to 0.

    // MANIPULATORS
    void increment();
        // Increase the reference count of this base representation by 1.

    int decrement();
        // Decrease the reference count of this base representation by 1 and
        // return its current value.

    // ACCESSORS
    virtual void execute(const A1& argument1, const A2& argument2,
                         const A3& argument3, const A4& argument4,
                         const A5& argument5, const A6& argument6) const = 0;
        // Invoke the client-supplied callback function with the specified
        // 'argument1', 'argument2', 'argument3', 'argument4', 'argument5',
        // and 'argument6'.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

// PROTECTED CREATORS
template <class A1, class A2, class A3, class A4, class A5, class A6>
inline bdefr_Vfunc6<A1, A2, A3, A4, A5, A6>::~bdefr_Vfunc6()
{
}

// CLASS METHODS
template <class A1, class A2, class A3, class A4, class A5, class A6>
inline
void bdefr_Vfunc6<A1, A2, A3, A4, A5, A6>::deleteObject(bdefr_Vfunc6 *object)
{
    object->~bdefr_Vfunc6();
    object->d_allocator_p->deallocate(object);
}

// CREATORS
template <class A1, class A2, class A3, class A4, class A5, class A6>
inline bdefr_Vfunc6<A1, A2, A3, A4, A5, A6>::bdefr_Vfunc6(
                                             bslma_Allocator *basicAllocator)
: d_count(0)
, d_allocator_p(basicAllocator)
{
}

// MANIPULATORS
template <class A1, class A2, class A3, class A4, class A5, class A6>
inline void bdefr_Vfunc6<A1, A2, A3, A4, A5, A6>::increment()
{
    ++d_count;  // Warning: not guaranteed to be an atomic operation.
}

template <class A1, class A2, class A3, class A4, class A5, class A6>
inline int bdefr_Vfunc6<A1, A2, A3, A4, A5, A6>::decrement()
{
    return --d_count;  // Warning: not guaranteed to be an atomic operation.
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
