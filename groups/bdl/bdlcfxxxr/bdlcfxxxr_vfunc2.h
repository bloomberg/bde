// bdlcfxxxr_vfunc2.h               -*-C++-*-
#ifndef INCLUDED_BDLCFXXXR_VFUNC2
#define INCLUDED_BDLCFXXXR_VFUNC2

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")


//@PURPOSE: Provide a common reference-counted base class representation.
//
//@CLASSES:
//   bdlcfxxxr::Vfunc2: thread-safe reference-counted base class with an allocator
//
//@AUTHOR: John Lakos (jlakos)
//
//@DEPRECATED: This component should not be used in new code, and will be
// deleted in the near future.  Please see 'bdlf_function', 'bdlf_bind', etc.
// for alternatives that should be used for all future development.
//
//@DESCRIPTION: This component defines the common (partially implemented) base
// class for all internal representations of the 'bdlcfxxx::Vfunc2' family of
// function objects (functors).  This abstract base class declares the pure
// virtual 'execute' method, whose signature characterizes this family of
// functor representations, while exploiting structural inheritance to
// implement efficient (inline) count manipulators.  The count is intended to
// reflect the number of 'bdlcfxxx::Vfunc2' objects (envelopes) or other partial
// owners that are currently using this functor representation (letter) and is
// manipulated by each owner accordingly.  The counter used by this component
// is atomic therefore providing thread-safe increment and decrement
// operations.  The class also provides a static 'deleteObject' method to allow
// clients to destroy the object (when the count reaches '0') without any
// information about the details of the 'bdlcfxxxr::Vfunc2' object memory management
// scheme.  Note that the object must be allocated dynamically using the same
// allocator supplied at construction and that the allocator must remain valid
// through the life of the object.  'deleteObject' method allows concrete
// classes derived from 'bdlcfxxxr::Vfunc2' to declare destructor 'private' and
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
// Instantiate a 'ConcreteDerivedClass' class derived from 'bdlcfxxxr::Vfunc2'.
//..
//   template <class A1, class A2>
//   class ConcreteDerivedClass : public bdlcfxxxr::Vfunc2 <A1, A2> {
//     public:
//       ConcreteDerivedClass(bslma::Allocator *basicAllocator)
//       : bdlcfxxxr::Vfunc2(basicAllocator) { }
//
//       virtual void execute(const A1& argument1, const A2& argument2) const
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
// Create an envelope class 'EnvelopeClass' that is using bdlcfxxxr::Vfunc2<A1, A2>
//..
//   template <class A1, class A2>
//   class EnvelopeClass {
//       // Provide an object that encapsulates a 'bdlcfxxxr::Vfunc2' object.
//
//       bdlcfxxxr::Vfunc2<A1, A2> *d_rep_p;  // polymorphic functor representation
//
//     public:
//       // CREATORS
//       EnvelopeClass(bdlcfxxxr::Vfunc2<A1, A2> *rep) : d_rep_p(rep)
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
//           // representation using 'deleteObject' method of 'bdlcfxxxr::Vfunc2'
//           // class.
//       {
//           if (0 == d_rep_p->decrement()) {
//               bdlcfxxxr::Vfunc2<A1, A2>::deleteObject(d_rep_p);
//           }
//       }
//
//
//       // ACCESSORS
//       void operator()(const A1& argument1, const A2& argument2) const
//           // Execute this functor.
//       {
//           d_rep_p->execute(argument1, argument2);
//       }
//   };
//..
// Then in the body of the program:
//..
//   executeUsageCounter = 0;
//   dtorUsageCounter = 0;
//
//   typedef ConcreteDerivedClass<int, int> DerivedObj;
//   typedef bdlcfxxxr::Vfunc2<int, int> Obj;
//   bslma::Allocator *myAllocator = bslma::Default::defaultAllocator();
//
//   Obj *x = new(*myAllocator) DerivedObj(myAllocator);
//   {
//       // The reference counter is 0
//       EnvelopeClass<int, int> env1(x);
//       // The reference counter is 1
//       env1(3, 4);        ASSERT(1 == executeUsageCounter);
//       {
//           EnvelopeClass<int, int> env1(x);
//           // The reference counter is 2
//           env1(5, 7);    ASSERT(2 == executeUsageCounter);
//       }
//       // The reference counter is 1
//       ASSERT(0 == dtorUsageCounter);
//   }
//   ASSERT(1 == dtorUsageCounter);
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLMTT_XXXATOMICUTIL
#include <bdlmtt_xxxatomicutil.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

namespace BloombergLP {

namespace bdlcfxxxr {
                        // ==================
                        // class Vfunc2
                        // ==================

template <class A1, class A2>
class Vfunc2 {
    // Common (partially-implemented) abstract base class declaring the
    // characteristic pure virtual 'execute' method and exploiting structural
    // inheritance to achieve efficient (inline) count manipulation.
    // This class also implements a 'deleteObject' class method.  This method
    // facilitates the use of 'Vfunc2' as a reference-counted letter
    // class in the envelope-letter pattern.  'deleteObject' is used to to
    // destroy and deallocate the concrete objects derived from 'Vfunc2',
    // which allows the derived classes to declare the destructor 'private',
    // and limit an object instantiation to the heap.

    bdlmtt::AtomicUtil::Int d_count;    // dumb data (number of active references)
    bslma::Allocator *d_allocator_p; // holds (but doesn't own) memory
                                     // allocator

  private:
    Vfunc2(const Vfunc2&);                  // not implemented
    Vfunc2& operator=(const Vfunc2&);       // not implemented

  protected:
    virtual ~Vfunc2();
        // The destructor is declared 'protected' to allow derivation from this
        // class and to disallow direct deletion of the derived concrete
        // object.  Clients must use the static ("class") 'deleteObject'
        // method to destroy and deallocate the object from its base class
        // pointer.

  public:
    // CLASS METHODS
    static void deleteObject(Vfunc2 *object);
        // Destroy the specified 'object' and use the memory allocator held by
        // 'object' to deallocate it.  The behaviour is undefined unless the
        // specified 'object' holds a valid memory allocator.

    // CREATORS
    Vfunc2(bslma::Allocator *basicAllocator);
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
    virtual void execute(const A1& argument1, const A2& argument2) const = 0;
        // Invoke the client-supplied callback function with the specified
        // 'argument1' and 'argument2'.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

// PROTECTED CREATORS
template <class A1, class A2>
inline Vfunc2<A1, A2>::~Vfunc2()
{
}

// CLASS METHODS
template <class A1, class A2>
void Vfunc2<A1, A2>::deleteObject(Vfunc2 *object)
{
    object->~Vfunc2();
    object->d_allocator_p->deallocate(object);
}

// CREATORS
template <class A1, class A2>
inline Vfunc2<A1, A2>::Vfunc2(bslma::Allocator *basicAllocator)
: d_allocator_p(basicAllocator)
{
    bdlmtt::AtomicUtil::initInt(&d_count,0);
}

// MANIPULATORS
template <class A1, class A2>
inline void Vfunc2<A1, A2>::increment()
{
    bdlmtt::AtomicUtil::incrementInt(&d_count);
}

template <class A1, class A2>
inline int Vfunc2<A1, A2>::decrement()
{
    return bdlmtt::AtomicUtil::decrementIntNv(&d_count);
}
}  // close package namespace

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
