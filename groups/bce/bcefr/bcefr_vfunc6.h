// bcefr_vfunc6.h               -*-C++-*-
#ifndef INCLUDED_BCEFR_VFUNC6
#define INCLUDED_BCEFR_VFUNC6

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")


//@PURPOSE: Provide a common reference-counted base class representation.
//
//@CLASSES:
//   bcefr_Vfunc6: thread-safe reference-counted base class with an allocator
//
//@AUTHOR: John Lakos (jlakos)
//
//@DEPRECATED: This component should not be used in new code, and will be
// deleted in the near future.  Please see 'bdef_function', 'bdef_bind', etc.
// for alternatives that should be used for all future development.
//
//@DESCRIPTION: This component defines the common (partially implemented) base
// class for all internal representations of the 'bcef_Vfunc6' family of
// function objects (functors).  This abstract base class declares the pure
// virtual 'execute' method, whose signature characterizes this family of
// functor representations, while exploiting structural inheritance to
// implement efficient (inline) count manipulators.  The count is intended to
// reflect the number of 'bcef_Vfunc6' objects (envelopes) or other partial
// owners that are currently using this functor representation (letter) and is
// manipulated by each owner accordingly.  The counter used by this component
// is atomic therefore providing thread-safe increment and decrement
// operations.  The class also provides a static 'deleteObject' method to allow
// clients to destroy the object (when the count reaches '0') without any
// information about the details of the 'bcefr_Vfunc6' object memory management
// scheme.  Note that the object must be allocated dynamically using the same
// allocator supplied at construction and that the allocator must remain valid
// through the life of the object.  'deleteObject' method allows concrete
// classes derived from 'bcefr_Vfunc6' to declare destructor 'private' and
// limit an object instantiation to the heap.
//
///USAGE
///-----
// This example demonstrates the essential functionality of the common
// base-class representation.  We will need two global counters for this
// demonstration.
//..
//   static int executeUsageCounter = 0;
//   static int dtorUsageCounter = 0;
//..
// Instantiate a 'ConcreteDerivedClass' class derived from 'bcefr_Vfunc6'.
//..
//   template <class A1, class A2, class A3, class A4, class A5, class A6>
//   class ConcreteDerivedClass : public bcefr_Vfunc6 <A1, A2, A3, A4, A5, A6>
//   {
//     public:
//       ConcreteDerivedClass(bslma_Allocator *basicAllocator)
//       : bcefr_Vfunc6(basicAllocator) { }
//
//       virtual void execute(const A1& argument1,
//                            const A2& argument2,
//                            const A3& argument3,
//                            const A4& argument4,
//                            const A5& argument5,
//                            const A6& argument6) const
//           // Increment global counter 'testCounter'.
//       {
//           ++executeUsageCounter;
//       }
//
//     private:
//       virtual ~ConcreteDerivedClass()
//           // Destroy the class instance.  Increment a global
//           // 'dtorUsageCounter'.
//       {
//           ++dtorUsageCounter;
//       }
//   };
//..
// Create an envelope class 'EnvelopeClass' that is using
// bcefr_Vfunc6<A1, A2, A3, A4, A5, A6>
//..
//   template <class A1, class A2, class A3, class A4, class A5, class A6>
//   class EnvelopeClass {
//       // Provide an object that encapsulates a 'bcefr_Vfunc6' object.
//
//       bcefr_Vfunc6<A1, A2, A3, A4, A5, A6> *d_rep_p;
//           // polymorphic functor representation
//
//     public:
//       // CREATORS
//       EnvelopeClass(bcefr_Vfunc6<A1, A2, A3, A4, A5, A6> *rep) :
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
//           // representation using 'deleteObject' method of 'bcefr_Vfunc6'
//           // class.
//       {
//           if (0 == d_rep_p->decrement()) {
//               bcefr_Vfunc6<A1, A2, A3, A4, A5, A6>::
//                   deleteObject(d_rep_p);
//           }
//       }
//
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
// Then in the body of the program:
//..
//   executeUsageCounter = 0;
//   dtorUsageCounter = 0;
//
//   typedef ConcreteDerivedClass<int, int, int,
//                                int, int, int> DerivedObj;
//   typedef bcefr_Vfunc6<int, int, int, int, int, int> Obj;
//   bslma_Allocator *myAllocator = bslma_Default::defaultAllocator();
//
//   Obj *x = new(*myAllocator) DerivedObj(myAllocator);
//   {
//       // The reference counter is 0
//       EnvelopeClass<int, int, int, int, int, int> env1(x);
//       // The reference counter is 1
//       env1(1, 2, 3, 4, 5, 6);    ASSERT(1 == executeUsageCounter);
//       {
//           EnvelopeClass<int, int, int, int, int, int> env1(x);
//           // The reference counter is 2
//           env1(1, 2, 3, 4, 5, 6); ASSERT(2 == executeUsageCounter);
//       }
//       // The reference counter is 1
//       ASSERT(0 == dtorUsageCounter);
//   }
//   ASSERT(1 == dtorUsageCounter);
//..

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCES_ATOMICUTIL
#include <bces_atomicutil.h>
#endif

namespace BloombergLP {

class bslma_Allocator;

                        // ==================
                        // class bcefr_Vfunc6
                        // ==================

template <class A1, class A2, class A3, class A4, class A5, class A6>
class bcefr_Vfunc6 {
    // Common (partially-implemented) abstract base class declaring the
    // characteristic pure virtual 'execute' method and exploiting structural
    // inheritance to achieve efficient (inline) count manipulation.
    // This class also implements a 'deleteObject' class method.  This method
    // facilitates the use of 'bcefr_Vfunc6' as a reference-counted letter
    // class in the envelope-letter pattern.  'deleteObject' is used to to
    // destroy and deallocate the concrete objects derived from 'bcefr_Vfunc6',
    // which allows the derived classes to declare the destructor 'private',
    // and limit an object instantiation to the heap.

    bces_AtomicUtil::Int d_count;    // dumb data (number of active references)
    bslma_Allocator *d_allocator_p; // holds (but doesn't own) memory allocator

  private:
    bcefr_Vfunc6(const bcefr_Vfunc6&);                  // not implemented
    bcefr_Vfunc6& operator=(const bcefr_Vfunc6&);       // not implemented

  protected:
    virtual ~bcefr_Vfunc6();
        // The destructor is declared 'protected' to allow derivation from this
        // class and to disallow direct deletion of the derived concrete
        // object.  Clients must use the static ("class") 'deleteObject'
        // method to destroy and deallocate the object from its base class
        // pointer.

  public:
    // CLASS METHODS
    static void deleteObject(bcefr_Vfunc6 *object);
        // Destroy the specified 'object' and use the memory allocator held by
        // 'object' to deallocate it.  The behaviour is undefined unless the
        // specified 'object' holds a valid memory allocator.

    // CREATORS
    bcefr_Vfunc6(bslma_Allocator *basicAllocator);
        // Create the base portion of a functor object, with the initial
        // reference count set to 0.  Return the specified 'basicAllocator' to
        // deallocate memory when 'destroyObject' is invoked.

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
inline bcefr_Vfunc6<A1, A2, A3, A4, A5, A6>::~bcefr_Vfunc6()
{
}

// CLASS METHODS
template <class A1, class A2, class A3, class A4, class A5, class A6>
void bcefr_Vfunc6<A1, A2, A3, A4, A5, A6>::deleteObject(
                                                        bcefr_Vfunc6 *object)
{
    object->~bcefr_Vfunc6();
    object->d_allocator_p->deallocate(object);
}

// CREATORS
template <class A1, class A2, class A3, class A4, class A5, class A6>
inline bcefr_Vfunc6<A1, A2, A3, A4, A5, A6>::bcefr_Vfunc6(
                                             bslma_Allocator *basicAllocator)
: d_allocator_p(basicAllocator)
{
    bces_AtomicUtil::initInt(&d_count,0);
}

// MANIPULATORS
template <class A1, class A2, class A3, class A4, class A5, class A6>
inline void bcefr_Vfunc6<A1, A2, A3, A4, A5, A6>::increment()
{
    bces_AtomicUtil::incrementInt(&d_count);
}

template <class A1, class A2, class A3, class A4, class A5, class A6>
inline int bcefr_Vfunc6<A1, A2, A3, A4, A5, A6>::decrement()
{
    return bces_AtomicUtil::decrementIntNv(&d_count);
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
