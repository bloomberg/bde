// bcema_factorydeleter.h          -*-C++-*-

#ifndef INCLUDED_BCEMA_FACTORYDELETER
#define INCLUDED_BCEMA_FACTORYDELETER

//@PURPOSE: Provide a templatized factory based deleter
//
//@CLASSES:
//   bcema_DefaultDeleter: a templateized factory based deleter.
//
//@SEE_ALSO: bcema_deleter bcema_defaultdeleter
//
//@AUTHOR: Ilougino Rocha (irocha)
//
//@DESCRIPTION:  This component provides a templatized deleter for object
//  factories that expose a 'deleteObject' interface(e.g., "bdema_Allocator",
//  "bcema_Pool").
//
///PROTOCOL HIERARCHY
//..
//                      ( bcema_FactoryDeleter )
//                                 |            ctor/dtor
//                                 v
//                        ( bcema_Deleter )
//                                         deleteObject
//..
///USAGE EXAMPLE
///-------------
// [TBD]
//

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCEMA_DELETER
#include <bcema_deleter.h>
#endif

namespace BloombergLP {

                        // ==========================
                        // class bcema_FactoryDeleter
                        // ==========================
template <class TYPE, class FACTORY>
class bcema_FactoryDeleter : public bcema_Deleter<TYPE> {
    FACTORY *d_factory_p; // held but not owned

  private: // not implemented
    bcema_FactoryDeleter(const bcema_FactoryDeleter<TYPE, FACTORY>&);
    bcema_FactoryDeleter<TYPE, FACTORY>&
        operator=(const bcema_FactoryDeleter<TYPE, FACTORY>&);
  public:
    // CREATORS
    bcema_FactoryDeleter(FACTORY *factory);
        // Create a deleter that uses the specified 'factory' to destroy the
        // memory footprint of an object when 'deleteObject' in invoked.

    virtual ~bcema_FactoryDeleter();
        // Destroy this deleter.

    // MANIPULATORS
    virtual void deleteObject(TYPE *instance);
        // Destroy the specified 'instance' based on its static type and
        // deallocate its memory footprint.  This method doesn't destroy
        // the deleter itself.
};

// ---------------------------------------------------------------------------

template <class TYPE, class FACTORY> inline
bcema_FactoryDeleter<TYPE,FACTORY>::bcema_FactoryDeleter(FACTORY *factory)
: d_factory_p(factory)
{

}

template <class TYPE, class FACTORY>
    bcema_FactoryDeleter<TYPE,FACTORY>::~bcema_FactoryDeleter()
{

}

template <class TYPE,class FACTORY>
void bcema_FactoryDeleter<TYPE,FACTORY>::deleteObject(TYPE *instance)
{
    d_factory_p->deleteObject(instance);
}

} // close namespace BloombergLP

#endif
// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
