// bslstl_queue.h                                                     -*-C++-*-
#ifndef INCLUDED_BSLSTL_QUEUE
#define INCLUDED_BSLSTL_QUEUE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

#ifndef INCLUDED_BSLSTL_ALLOCATOR
#include <bslstl_allocator.h>
#endif

#ifndef INCLUDED_BSLSTL_DEQUE
#include <bslstl_deque.h>
#endif

#ifndef INCLUDED_BSLSTL_VECTOR
#include <bslstl_vector.h>
#endif

#ifndef INCLUDED_BSLALG_SWAPUTIL
#include <bslalg_swaputil.h>
#endif

#ifndef INCLUDED_FUNCTIONAL
#include <functional>
#define INCLUDED_FUNCTIONAL
#endif

namespace bsl {

                             // ===========
                             // class queue
                             // ===========
                             

template <class VALUE, class CONTAINER = deque<VALUE> >
class queue {

    // FRIENDS
    template<class VALUE2, class CONTAINER2>
    friend bool operator==(const queue<VALUE2, CONTAINER2>& lhs,
                           const queue<VALUE2, CONTAINER2>& rhs);

    template<class VALUE2, class CONTAINER2>
    friend bool operator!=(const queue<VALUE2, CONTAINER2>& lhs,
                           const queue<VALUE2, CONTAINER2>& rhs);

    template<class VALUE2, class CONTAINER2>
    friend bool operator< (const queue<VALUE2, CONTAINER2>& lhs,
                           const queue<VALUE2, CONTAINER2>& rhs);

    template<class VALUE2, class CONTAINER2>
    friend bool operator> (const queue<VALUE2, CONTAINER2>& lhs,
                           const queue<VALUE2, CONTAINER2>& rhs);

    template<class VALUE2, class CONTAINER2>
    friend bool operator<=(const queue<VALUE2, CONTAINER2>& lhs,
                           const queue<VALUE2, CONTAINER2>& rhs);

    template<class VALUE2, class CONTAINER2>
    friend bool operator>=(const queue<VALUE2, CONTAINER2>& lhs,
                           const queue<VALUE2, CONTAINER2>& rhs);


  protected:

    CONTAINER c;

  public:

    typedef typename CONTAINER::value_type      value_type;
    typedef typename CONTAINER::reference       reference;
    typedef typename CONTAINER::const_reference const_reference;
    typedef typename CONTAINER::size_type       size_type;
    typedef          CONTAINER                  container_type;

    // CREATORS

    explicit queue();
        // TBD

    explicit queue(const CONTAINER& container);
        // TBD

//  explicit queue(CONTAINER&& = CONTAINER());

//  queue(queue&& q);

    queue(const queue& original);

    template <class ALLOCATOR>
    explicit queue(const ALLOCATOR& allocator);
        // TBD

    template <class ALLOCATOR>
    queue(const CONTAINER& container, const ALLOCATOR& allocator);
        // TBD

//  template <class ALLOCATOR>
//  queue(CONTAINER&& container, const ALLOCATOR& allocator);

    template <class ALLOCATOR>
    queue(const queue& original, const ALLOCATOR& allocator);
        // TBD

//  template <class ALLOCATOR>
//  queue(queue&& original, const ALLOCATOR& allocator);

    // MANIPULATORS
    
//  queue& operator=(queue&& rhs);
       
    queue& operator=(const queue& rhs);
        // TBD

    void push(const value_type& value);
        // TBD

//  void push(value_type&& value) { c.push_back(std::move(value)); }
//  template <class... Args> void emplace(Args&&... args)
//  { c.emplace_back(std::forward<Args>(args)...); }

    void pop();
        // TBD

    void swap(queue& other);
        // TBD
        
    // ACCESSORS
    bool empty() const;
        // TBD

    size_type size() const;
        // TBD

    reference front();
        // TBD

    const_reference front() const;
        // TBD

    reference back();
        // TBD
        
    const_reference back() const;
        // TBD
 
};

// FREE FUNCTIONS
template <class VALUE, class CONTAINER>
inline
bool operator==(const queue<VALUE, CONTAINER>& lhs,
                const queue<VALUE, CONTAINER>& rhs);
    // TBD

template <class VALUE, class CONTAINER>
inline
bool operator!=(const queue<VALUE, CONTAINER>& lhs,
                const queue<VALUE, CONTAINER>& rhs);
    // TBD

template <class VALUE, class CONTAINER>
inline
bool operator< (const queue<VALUE, CONTAINER>& lhs,
                const queue<VALUE, CONTAINER>& rhs);
    // TBD

template <class VALUE, class CONTAINER>
inline
bool operator> (const queue<VALUE, CONTAINER>& lhs,
                const queue<VALUE, CONTAINER>& rhs);
    // TBD

template <class VALUE, class CONTAINER>
inline
bool operator>=(const queue<VALUE, CONTAINER>& lhs,
                const queue<VALUE, CONTAINER>& rhs);
    // TBD

template <class VALUE, class CONTAINER>
inline
bool operator<=(const queue<VALUE, CONTAINER>& lhs,
                const queue<VALUE, CONTAINER>& rhs);
    // TBD

template <class VALUE, class CONTAINER>
inline
void swap(queue<VALUE, CONTAINER>& lhs,
          queue<VALUE, CONTAINER>& rhs);
    // TBD

// template <class VALUE, class CONTAINER, class ALLOCATOR>
// struct uses_allocator<queue<VALUE, CONTAINER>, ALLOCATOR>
// : uses_allocator<CONTAINER, ALLOCATOR>::type 
// { 
// };
   

                         // ====================
                         // class priority_queue
                         // ====================
                         
template <class VALUE,
          class CONTAINER  = vector<VALUE>,
          class COMPARATOR = native_std::less<typename CONTAINER::value_type> >
class priority_queue {

  protected:

    CONTAINER c;
    COMPARATOR comp;

  public:

    typedef typename CONTAINER::value_type      value_type;
    typedef typename CONTAINER::reference       reference;
    typedef typename CONTAINER::const_reference const_reference;
    typedef typename CONTAINER::size_type       size_type;
    typedef          CONTAINER                  container_type;

    // CREATORS

    explicit priority_queue();
        // TBD

    priority_queue(const COMPARATOR& comparator,
                   const CONTAINER&  container);
        // TBD
        
//  explicit priority_queue(const COMPARATOR& comparator = COMPARATOR(),
//                                CONTAINER&& container  = CONTAINER ());

    template <class INPUT_ITERATOR>
    priority_queue(INPUT_ITERATOR    first,
                   INPUT_ITERATOR    last);
        // TBD

    template <class INPUT_ITERATOR>
    priority_queue(INPUT_ITERATOR    first,
                   INPUT_ITERATOR    last,
                   const COMPARATOR& comparator,
                   const CONTAINER&  container);

//  template <class INPUT_ITERATOR>
//  priority_queue(INPUT_ITERATOR    first,
//                 INPUT_ITERATOR    last,
//                 const COMPARATOR& comparator = COMPARATOR(),
//                 CONTAINER&&       allocator  = CONTAINER ());

//  priority_queue(priority_queue&&);

    priority_queue(const priority_queue&);

    template <class ALLOCATOR>
    explicit priority_queue(const ALLOCATOR& allocator);
        // TBD

    template <class ALLOCATOR>
    priority_queue(const COMPARATOR& comparator, const ALLOCATOR& allocator);
        // TBD

    template <class ALLOCATOR>
    priority_queue(const COMPARATOR& comparator,
                   const CONTAINER&  container,
                   const ALLOCATOR&  allocator);
        // TBD

//  template <class ALLOCATOR>
//  priority_queue(const COMPARATOR& comparator,
//                       CONTAINER&& container,
//                 const ALLOCATOR&  allocator);
   
    template <class ALLOCATOR>
    priority_queue(const priority_queue& original, const ALLOCATOR& allocator);
        // TBD

//  template <class ALLOCATOR>
//  priority_queue(priority_queue&& original, const ALLOCATOR& allocator);


    // MANIPULATORS

//  priority_queue& operator=(priority_queue&& rhs);

    priority_queue& operator=(const priority_queue& rhs);

    void push(const value_type& value);
        // TBD

//  void push(value_type&& value);
   
//  template <class... Args>
//  void emplace(Args&&... args);
   
    void pop();
        // TBD
   
    void swap(priority_queue& other);
        // TBD

    // ACCESSORS
    bool empty() const;
        // TBD

    size_type size() const;
        // TBD
        
    const_reference top() const;
        // TBD
};

// FREE FUNCTIONS

template <class VALUE, class CONTAINER, class COMPARATOR>
void swap(priority_queue<VALUE, CONTAINER, COMPARATOR>& lhs,
          priority_queue<VALUE, CONTAINER, COMPARATOR>& rhs);

// template <class VALUE,
//           class CONTAINER,
//           class COMPARATOR,
//           class ALLOCATOR>
// struct
// uses_allocator<priority_queue<VALUE, CONTAINER, COMPARATOR>, ALLOCATOR>
// : uses_allocator<CONTAINER, ALLOCATOR>::type
// {
// };

// ==========================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ===========================================================================

                              // -----------
                              // class queue
                              // -----------

// CREATORS
template <class VALUE, class CONTAINER>
inline
queue<VALUE, CONTAINER>::queue()
{
}

template <class VALUE, class CONTAINER>
inline
queue<VALUE, CONTAINER>::queue(const CONTAINER& container)
: c(container)
{
}

template <class VALUE, class CONTAINER>
inline
queue<VALUE, CONTAINER>::queue(const queue& original)
: c(original.c)
{
}

template <class VALUE, class CONTAINER>
template <class ALLOCATOR>
inline
queue<VALUE, CONTAINER>::queue(const ALLOCATOR& allocator)
: c(allocator)
{
}

template <class VALUE, class CONTAINER>
template <class ALLOCATOR>
inline
queue<VALUE, CONTAINER>::queue(const CONTAINER& container,
                               const ALLOCATOR& allocator)
: c(container, allocator)
{
}

template <class VALUE, class CONTAINER>
template <class ALLOCATOR>
inline
queue<VALUE, CONTAINER>::queue(const queue&     queue,
                               const ALLOCATOR& allocator)
: c(queue.c, allocator)
{
}

// MANIPULATORS

template <class VALUE, class CONTAINER>
inline
queue<VALUE, CONTAINER>&
queue<VALUE, CONTAINER>::operator=(const queue& rhs)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(this != &rhs)) {
        queue other(rhs);
        this->swap(other);
    }
    return *this;
}

template <class VALUE, class CONTAINER>
inline
void queue<VALUE, CONTAINER>::push(const value_type& value)
{
    c.push_back(value);
}

template <class VALUE, class CONTAINER>
inline
void queue<VALUE, CONTAINER>::pop()
{
    c.pop_front();
}

template <class VALUE, class CONTAINER>
inline
void queue<VALUE, CONTAINER>::swap(queue& q)
{
    BloombergLP::bslalg_SwapUtil::swap(&c, &q.c);
}

// ACCESSORS
template <class VALUE, class CONTAINER>
inline
bool queue<VALUE, CONTAINER>::empty() const
{ 
    return c.empty();
}

template <class VALUE, class CONTAINER>
inline
typename queue<VALUE, CONTAINER>::size_type
queue<VALUE, CONTAINER>::size() const
{
    return c.size();
}

template <class VALUE, class CONTAINER>
inline
typename queue<VALUE, CONTAINER>::reference
queue<VALUE, CONTAINER>::front()
{
    return c.front();
}

template <class VALUE, class CONTAINER>
inline
typename queue<VALUE, CONTAINER>::const_reference
queue<VALUE, CONTAINER>::front() const
{
    return c.front();
}

template <class VALUE, class CONTAINER>
inline
typename queue<VALUE, CONTAINER>::reference
queue<VALUE, CONTAINER>::back()
{
    return c.back();
}

template <class VALUE, class CONTAINER>
inline
typename queue<VALUE, CONTAINER>::const_reference
queue<VALUE, CONTAINER>::back() const
{
    return c.back();
}

// FREE OPERATORS
template <class VALUE, class CONTAINER>
inline
bool operator==(const queue<VALUE, CONTAINER>& lhs,
                const queue<VALUE, CONTAINER>& rhs)
{
    return lhs.c == rhs.c;
}

template <class VALUE, class CONTAINER>
inline
bool operator< (const queue<VALUE, CONTAINER>& lhs,
                const queue<VALUE, CONTAINER>& rhs)
{
    return lhs.c < rhs.c;
}

template <class VALUE, class CONTAINER>
inline
bool operator!=(const queue<VALUE, CONTAINER>& lhs,
                const queue<VALUE, CONTAINER>& rhs)
{
    return lhs.c != rhs.c;
}

template <class VALUE, class CONTAINER>
inline
bool operator> (const queue<VALUE, CONTAINER>& lhs,
                const queue<VALUE, CONTAINER>& rhs)
{
    return lhs.c > rhs.c;
}

template <class VALUE, class CONTAINER>
inline
bool operator>=(const queue<VALUE, CONTAINER>& lhs,
                const queue<VALUE, CONTAINER>& rhs)
{
    return lhs.c >= rhs.c;
}

template <class VALUE, class CONTAINER>
inline
bool operator<=(const queue<VALUE, CONTAINER>& lhs,
                const queue<VALUE, CONTAINER>& rhs)
{
    return lhs.c <= rhs.c;
}

template <class VALUE, class CONTAINER>
inline
void swap(queue<VALUE, CONTAINER>& lhs,
          queue<VALUE, CONTAINER>& rhs)
{
    lhs.swap(rhs);
}


                         // --------------------
                         // class priority_queue
                         // --------------------
                         
template <class VALUE, class CONTAINER, class COMPARATOR>
inline
priority_queue<VALUE, CONTAINER, COMPARATOR>::priority_queue()
{
}
                        
template <class VALUE, class CONTAINER, class COMPARATOR>
inline
priority_queue<VALUE, CONTAINER, COMPARATOR>::priority_queue(
                                                  const COMPARATOR& comparator,
                                                  const CONTAINER&  container)
: c(container)
, comp(comparator)
{
    make_heap(c.begin(), c.end(), comp);
}

template <class VALUE, class CONTAINER, class COMPARATOR>
template <class INPUT_ITERATOR>
inline
priority_queue<VALUE, CONTAINER, COMPARATOR>::priority_queue(
                                                       INPUT_ITERATOR    first,
                                                       INPUT_ITERATOR    last)
{
    insert(c.end(), first, last);
    make_heap(c.begin(), c.end(), comp);
}

template <class VALUE, class CONTAINER, class COMPARATOR>
template <class INPUT_ITERATOR>
inline
priority_queue<VALUE, CONTAINER, COMPARATOR>::priority_queue(
                                                  INPUT_ITERATOR    first,
                                                  INPUT_ITERATOR    last,
                                                  const COMPARATOR& comparator,
                                                  const CONTAINER&  container)
: c(container)
, comp(comparator)
{
    insert(c.end(), first, last);
    make_heap(c.begin(), c.end(), comp);
}

template <class VALUE, class CONTAINER, class COMPARATOR>
inline
priority_queue<VALUE, CONTAINER, COMPARATOR>::priority_queue(
                                                const priority_queue& original)
: c(original.c)
, comp(original.comp)
{
}

template <class VALUE, class CONTAINER, class COMPARATOR>
template <class ALLOCATOR>
inline
priority_queue<VALUE, CONTAINER, COMPARATOR>::priority_queue(
                                                    const ALLOCATOR& allocator)
: c(allocator)
, comp(native_std::less<typename CONTAINER::value_type>())
{
}

template <class VALUE, class CONTAINER, class COMPARATOR>
template <class ALLOCATOR>
inline
priority_queue<VALUE, CONTAINER, COMPARATOR>::priority_queue(
                                                  const COMPARATOR& comparator,
                                                  const ALLOCATOR&  allocator)
: c(allocator)
, comp(comparator)
{
}

template <class VALUE, class CONTAINER, class COMPARATOR>
template <class ALLOCATOR>
inline
priority_queue<VALUE, CONTAINER, COMPARATOR>::priority_queue(
                                                  const COMPARATOR& comparator,
                                                  const CONTAINER&  container,
                                                  const ALLOCATOR&  allocator)
: c(container, allocator)
, comp(comparator)
{
}

   
template <class VALUE, class CONTAINER, class COMPARATOR>
template <class ALLOCATOR>
inline
priority_queue<VALUE, CONTAINER, COMPARATOR>::priority_queue(
                                               const priority_queue& original,
                                               const ALLOCATOR&      allocator)
: c(original.c, allocator)
, comp(original.comp)
{
}


// MANIPULATORS

template <class VALUE, class CONTAINER, class COMPARATOR>
inline
priority_queue<VALUE, CONTAINER, COMPARATOR>&
priority_queue<VALUE, CONTAINER, COMPARATOR>::operator=(
                                                     const priority_queue& rhs)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(this != &rhs)) {
        priority_queue other(rhs);
        this->swap(other);
    }
    return *this;
}

template <class VALUE, class CONTAINER, class COMPARATOR>
inline
void priority_queue<VALUE, CONTAINER, COMPARATOR>::push(
                                                       const value_type& value)
{
    c.push_back(value);
    push_heap(c.begin(), c.end(), comp);
}

template <class VALUE, class CONTAINER, class COMPARATOR>
inline
void priority_queue<VALUE, CONTAINER, COMPARATOR>::pop()
{
    pop_heap(c.begin(), c.end(), comp);
    c.pop_back();
}

template <class VALUE, class CONTAINER, class COMPARATOR>
inline
void priority_queue<VALUE, CONTAINER, COMPARATOR>::swap(
                                                         priority_queue& other) 
{
    BloombergLP::bslalg_SwapUtil::swap(&c   , &other.c   );
    BloombergLP::bslalg_SwapUtil::swap(&comp, &other.comp);
} 

// ACCESSORS
template <class VALUE, class CONTAINER, class COMPARATOR>
inline
bool priority_queue<VALUE, CONTAINER, COMPARATOR>::empty() const
{
    return c.empty();
}

template <class VALUE, class CONTAINER, class COMPARATOR>
inline
typename priority_queue<VALUE, CONTAINER, COMPARATOR>::size_type
priority_queue<VALUE, CONTAINER, COMPARATOR>::size() const
{
    return c.size();
}

template <class VALUE, class CONTAINER, class COMPARATOR>
inline
typename priority_queue<VALUE, CONTAINER, COMPARATOR>::const_reference
priority_queue<VALUE, CONTAINER, COMPARATOR>::top() const
{
    return c.front();
}

// FREE FUNCTIONS
template <class VALUE, class CONTAINER, class COMPARATOR>
void swap(priority_queue<VALUE, CONTAINER, COMPARATOR>& lhs,
          priority_queue<VALUE, CONTAINER, COMPARATOR>& rhs)
{
    lhs.swap(rhs);
}

}  // close namespace bsl

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
