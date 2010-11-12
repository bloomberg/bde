// bdeut_valuebuffer.h             -*-C++-*-
#ifndef INCLUDED_BDEUT_VALUEBUFFER
#define INCLUDED_BDEUT_VALUEBUFFER

//@PURPOSE: Provide a signature-specific function object (functor).
//
//@CLASSES:
//   bdef_Function: signature-specific function object (functor)
//
//@AUTHOR: Ilougino Rocha (irocha)
//
//@SEE_ALSO:
//
//@DESCRIPTION: 

#ifndef INCLUDED_BDES_OBJECTBUFFER
#include <bdes_objectbuffer.h>
#endif

#ifndef INCLUDED_BDEALG_TYPETRAITS
#include <bdealg_typetraits.h>
#endif

#ifndef INCLUDED_BDEALG_SCALARPRIMITIVES
#include <bdealg_scalarprimitives.h>
#endif

class bdema_Allocator;
template <class TYPE, int USES_ALLOCATOR> class bdeut_Value_Buffer;


                        // =================
                        // class bdeut_Value
                        // =================
			       
template <class TYPE>
class bdeut_Value {
    typedef  
	bdeut_Value_Buffer<TYPE,
	                   (int)bdealg_HasTrait<
	                      bdealg_TypeTraitUsesBdemaAllocator>::VALUE>
        Buffer;
    Buffer  d_buffer;
    bool    d_hasValue;
  public:
    // CREATORS
    bdeut_Value(bdema_Allocator *allocator=0);
       // Construct an unset value.
    
    bdeut_Value(const TYPE &original, bdema_Allocator *allocator=0);
       // Construct a value set having the value of the specified original
       // object.

    bdeut_Value(const bdeut_Value<TYPE> &original, 
		bdema_Allocator *allocator=0);
       // Construct a value set having the value of the specified original
       // value.

    ~bdeut_Value();
       // Destroy this value object.  

    // MANIPULATORS
    bdeut_Value& operator=(bdeut_Value<TYPE> const &rhs);

    bdeut_Value& operator=(TYPE const &rhs);

    void clear();

    // ACCESSORS
    TYPE* value();

    const TYPE* value() const;

    operator bool() const;

    TYPE& operator*();

    TYPE const& operator*() const;

    TYPE* operator->();

    const TYPE* operator->() const;
};

                        // ========================
                        // class bdeut_Value_Buffer
                        // ========================

template <class TYPE, int USES_ALLOCATOR>
class bdeut_Value_Buffer {
    bdes_ObjectBuffer<TYPE>  d_buffer;

  public:
    bdeut_Value(bdema_Allocator *allocator=0);

    bdeut_Value(TYPE const &type, bdema_Allocator *allocator=0);

    //MANIPULATORS
    construct();

    construct(TYPE const &type);

    destruct();

    TYPE& object();

    //ACCESSORS

    const TYPE& object() const;

};

template <class TYPE>
struct bdeut_Value_Buffer<TYPE,1> {
    bdes_ObjectBuffer<TYPE>  d_buffer;
    bdema_Allocator         *d_allocator_p;

  public:
    bdeut_Value(bdema_Allocator *allocator=0);

    bdeut_Value(TYPE const &type, bdema_Allocator *allocator=0);

    //MANIPULATORS
    construct();

    construct(TYPE const &type);

    destruct();

    //ACCESSORS
    TYPE& object();

    const TYPE& object() const;
};


                        // -----------------
                        // class bdeut_Value
                        // -----------------

inline bdeut_Value::bdeut_Value(bdema_Allocator *allocator)
: d_buffer(allocator)
, d_hasValue(false)
{
}

inline bdeut_Value::bdeut_Value(TYPE const &original, 
				bdema_Allocator *allocator)
: d_buffer(original, allocator)
, d_hasValue(true)
{

}

inline bdeut_Value::bdeut_Value(bdeut_Value<TYPE> const &original, 
				bdema_Allocator *allocator)
: d_buffer(allocator)
, d_hasValue(false)
{
    if (original.d_hasValue) {
	d_buffer.construct(*original);
	d_hasValue = true;
    }
}

inline bdeut_Value::~bdeut_Value()
{
    if (d_hasValue) d_buffer.destruct();
}

inline bdeut_Value::operator=(bdeut_Value<TYPE> const &rhs)
{
    if (d_hasValue && rhs.d_hasValue) d_buffer.object() = *rhs;
    else if (!d_hasValue && rhs.d_hasValue) {
	d_buffer.construct(rhs);
	d_hasValue = true;
    }
    else if (d_hasValue) {
	d_bufer.destruct();
	d_hasValue = false;
    }
    return *this;
}

inline bdeut_Value::operator=(TYPE const &rhs)
{
    if (d_hasValue) d_buffer.object() = rhs;
    else {
	d_buffer.construct(rhs);
	d_hasValue = true;
    }
    return *this;
}

void bdeut_Value::clear()
{
    if (d_hasValue) {
	d_buffer.destruct();
	d_hasValue = false;
    }
}

inline TYPE* bdeut_Value::value()
{
    return d_hasValue ? &d_buffer.object() : 0;
}

inline const TYPE* bdeut_Value::value() const
{
    return d_hasValue ? &d_buffer.object() : 0;
}

inline bdeut_Value::operator bool() const
{
    return value();
}

inline TYPE& bdeut_Value::operator*()
{
    return *value();
}

inline TYPE const& bdeut_Value::operator*() const
{
    return *value();
}

inline TYPE* bdeut_Value::operator->()
{
    return value();
}

inline const TYPE* bdeut_Value::operator->() const
{
    return value();
}

                        // ------------------------
                        // class bdeut_Value_Buffer
                        // ------------------------

template <class TYPE,int USES_ALLOCATOR>
inline bdeut_Value_Buffer<TYPE,USES_ALLOCATOR>::bdeut_Value(
						    bdema_Allocator *allocator)
{
}

template <class TYPE,int USES_ALLOCATOR>
inline bdeut_Value_Buffer<TYPE,USES_ALLOCATOR>::bdeut_Value(
						    TYPE const &original, 
						    bdema_Allocator *allocator)
{
    bdealg_ScalarPrimitives::copyConstruct(&d_buffer.object(), original, 0);
}

template <class TYPE,int USES_ALLOCATOR>
inline bdeut_Value_Buffer<TYPE,USES_ALLOCATOR>::construct()
{		  
    bdealg_ScalarPrimitives::defaultConstruct(&d_buffer.object(), 0);
}

template <class TYPE,int USES_ALLOCATOR>
inline bdeut_Value_Buffer<TYPE,USES_ALLOCATOR>::construct(TYPE const &original)
{		  
    bdealg_ScalarPrimitives::copyConstruct(&d_buffer.object(), original, 0);
}

template <class TYPE,int USES_ALLOCATOR>
inline bdeut_Value_Buffer<TYPE,USES_ALLOCATOR>::destruct()
{		  
    bdealg_ScalarPrimitives::destruct(&d_buffer.object(), 0);
}

template <class TYPE,int USES_ALLOCATOR>
inline TYPE& bdeut_Value_Buffer<TYPE,USES_ALLOCATOR>::object()
{					   
    return d_buffer.object();
}

template <class TYPE,int USES_ALLOCATOR>
inline const TYPE& bdeut_Value_Buffer<TYPE,USES_ALLOCATOR>::object() const
{				
    return d_buffer.object();		 
}

template <class TYPE>
inline bdeut_Value_Buffer<TYPE,1>::bdeut_Value(bdema_Allocator *allocator)
: d_allocator_p(bdema_Default::allocator(allocator))
{
    bdealg_ScalarPrimitives::defaultConstruct(&d_buffer.object(), 
					      d_allocator_p);
}


template <class TYPE>
inline bdeut_Value_Buffer<TYPE,1>::bdeut_Value(TYPE const &type, 
					       bdema_Allocator *allocator)
: d_allocator_p(bdema_Default::allocator(allocator))
{
    bdealg_ScalarPrimitives::copyConstruct(&d_buffer.object(), original, 
					   d_allocator_p);
}

template <class TYPE>
inline bdeut_Value_Buffer<TYPE,1>::construct()
{		  
    bdealg_ScalarPrimitives::defaultConstruct(&d_buffer.object(), 
					      d_allocator_p);
}

template <class TYPE>
inline bdeut_Value_Buffer<TYPE,1>::construct(TYPE const &type)
{		  
    bdealg_ScalarPrimitives::copyConstruct(&d_buffer.object(), original, 
					   d_allocator_p);
}

template <class TYPE>
inline bdeut_Value_Buffer<TYPE,1>::destruct()
{		  
    bdealg_ScalarPrimitives::destruct(&d_buffer.object(), 0);
}

template <class TYPE>
inline TYPE& bdeut_Value_Buffer<TYPE,1>::object()
{					   
    return d_buffer.object();
}

template <class TYPE>			 
inline const TYPE& bdeut_Value_Buffer<TYPE,1>::object() const
{				
    return d_buffer.object();		 
}

#endif
