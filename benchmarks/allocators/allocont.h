#ifndef ALLOCONT_H_
#define ALLOCONT_H_

#include <list>
#include <forward_list>
#include <deque>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <set>
#include <scoped_allocator>

#include <bsl_memory.h>
#include <bslma_mallocfreeallocator.h>

#include <bdlma_sequentialallocator.h>
#include <bdlma_bufferedsequentialallocator.h>
#include <bdlma_multipoolallocator.h>

template <typename T, typename Pool>
struct pool_adaptor {
    typedef T value_type;
    Pool* pool;
    pool_adaptor() : pool(nullptr) {}
    pool_adaptor(Pool* poo) : pool(poo) {}
    template <typename T2>
        pool_adaptor(pool_adaptor<T2,Pool> other) : pool(other.pool) { }
    T* allocate(size_t sz) {
        char volatile* p = (char*) pool->allocate(sz * sizeof(T));
        *p = '\0';
        return (T*) p;
    }
    void deallocate(void* p, size_t) { pool->deallocate(p); }
};

template <typename T1, typename T2, typename Pool>
    bool operator==(
        pool_adaptor<T1,Pool> const& one,
        pool_adaptor<T2,Pool> const& two)
            { return one.pool == two.pool; }

struct stdalloc {
    using string = std::string;
    template <typename T> using vector = std::vector<T>;
    template <typename T,
              typename H=std::hash<T>, typename Eq=std::equal_to<T>>
        using unordered_set = std::unordered_set<T,H,Eq>;
    template <typename T>
        using allocator = std::allocator<T>;
};

struct mallocfree {

template <typename T>
    using allocator =
        std::scoped_allocator_adaptor<
            pool_adaptor<T,BloombergLP::bslma::MallocFreeAllocator>>;

template <class T>
    using list = std::list<T,allocator<T>>;
template <class T>
    using forward_list = std::forward_list<T,allocator<T>>;
template <class T>
    using deque = std::deque<T,allocator<T>>;
template <class T>
    using vector = std::vector<T,allocator<T>>;

template <class T>
    using basic_string =
        std::basic_string<T,std::char_traits<T>,allocator<T>>;

using string = basic_string<char>;

template <class Key, class T, class Compare = std::less<Key>>
    using map = std::map<
        Key,T,Compare,allocator<std::pair<const Key,T>>>;
template <class Key, class T, class Compare = std::less<Key>>
    using multimap = std::multimap<
        Key,T,Compare,allocator<std::pair<const Key,T>>>;

template <class Key, class Compare = std::less<Key>>
    using set =      std::set<Key,Compare,allocator<Key>>;
template <class Key, class Compare = std::less<Key>>
   using multiset = std::multiset<Key,Compare,allocator<Key>>;

template <class Key, class T,
          class Hash = std::hash<Key>, class Pred = std::equal_to<Key>>
    using unordered_map = std::unordered_map<
        Key,T,Hash,Pred,allocator<std::pair<const Key,T>>>;
template <class Key, class T,
          class Hash = std::hash<Key>, class Pred = std::equal_to<Key>>
    using unordered_multimap = std::unordered_multimap<
        Key,T,Hash,Pred,allocator<std::pair<const Key,T>>>;

template <class Key,
          class Hash = std::hash<Key>, class Pred = std::equal_to<Key>>
    using unordered_set = std::unordered_set<
    Key,Hash,Pred,allocator<Key>>;
template <class Key,
          class Hash = std::hash<Key>, class Pred = std::equal_to<Key>>
    using unordered_multiset = std::unordered_multiset<
        Key,Hash,Pred,allocator<Key>>;
};

struct monotonic {

template <typename T>
    using allocator = std::scoped_allocator_adaptor<
        pool_adaptor<T,BloombergLP::bdlma::BufferedSequentialPool>>;

template <class T>
    using list = std::list<T,allocator<T>>;
template <class T>
    using forward_list = std::forward_list<T,allocator<T>>;
template <class T>
    using deque = std::deque<T,allocator<T>>;
template <class T>
    using vector = std::vector<T,allocator<T>>;

template <class T>
    using basic_string =
        std::basic_string<T,std::char_traits<T>,allocator<T>>;

using string = basic_string<char>;

template <class Key, class T, class Compare = std::less<Key>>
    using map = std::map<
        Key,T,Compare,allocator<std::pair<const Key,T>>>;
template <class Key, class T, class Compare = std::less<Key>>
    using multimap = std::multimap<
        Key,T,Compare,allocator<std::pair<const Key,T>>>;

template <class Key, class Compare = std::less<Key>>
    using set =      std::set<Key,Compare,allocator<Key>>;
template <class Key, class Compare = std::less<Key>>
    using multiset = std::multiset<Key,Compare,allocator<Key>>;

template <class Key, class T,
          class Hash = std::hash<Key>, class Pred = std::equal_to<Key>>
    using unordered_map = std::unordered_map<
        Key,T,Hash,Pred,allocator<std::pair<const Key,T>>>;
template <class Key, class T,
          class Hash = std::hash<Key>, class Pred = std::equal_to<Key>>
    using unordered_multimap = std::unordered_multimap<
        Key,T,Hash,Pred,allocator<std::pair<const Key,T>>>;

template <class Key,
          class Hash = std::hash<Key>, class Pred = std::equal_to<Key>>
    using unordered_set = std::unordered_set<
        Key,Hash,Pred,allocator<Key>>;
template <class Key,
          class Hash = std::hash<Key>, class Pred = std::equal_to<Key>>
    using unordered_multiset = std::unordered_multiset<
        Key,Hash,Pred,allocator<Key>>;
};

struct multipool {

template <typename T>
    using allocator = std::scoped_allocator_adaptor<
        pool_adaptor<T,BloombergLP::bdlma::Multipool>>;

template <class T>
    using list = std::list<T,allocator<T>>;
template <class T>
    using forward_list = std::forward_list<T,allocator<T>>;
template <class T>
    using deque = std::deque<T,allocator<T>>;
template <class T>
    using vector = std::vector<T,allocator<T>>;

template <class T>
    using basic_string =
        std::basic_string<T,std::char_traits<T>,allocator<T>>;

using string = basic_string<char>;

template <class Key, class T, class Compare = std::less<Key>>
    using map = std::map<
        Key,T,Compare,allocator<std::pair<const Key,T>>>;
template <class Key, class T, class Compare = std::less<Key>>
    using multimap = std::multimap<
        Key,T,Compare,allocator<std::pair<const Key,T>>>;

template <class Key, class Compare = std::less<Key>>
    using set =      std::set<Key,Compare,allocator<Key>>;
template <class Key, class Compare = std::less<Key>>
    using multiset = std::multiset<Key,Compare,allocator<Key>>;

template <class Key, class T,
          class Hash = std::hash<Key>, class Pred = std::equal_to<Key>>
    using unordered_map = std::unordered_map<
        Key,T,Hash,Pred,allocator<std::pair<const Key,T>>>;
template <class Key, class T,
          class Hash = std::hash<Key>, class Pred = std::equal_to<Key>>
    using unordered_multimap = std::unordered_multimap<
        Key,T,Hash,Pred,allocator<std::pair<const Key,T>>>;

template <class Key,
          class Hash = std::hash<Key>, class Pred = std::equal_to<Key>>
    using unordered_set = std::unordered_set<
        Key,Hash,Pred,allocator<Key>>;
template <class Key,
          class Hash = std::hash<Key>, class Pred = std::equal_to<Key>>
    using unordered_multiset = std::unordered_multiset<
        Key,Hash,Pred,allocator<Key>>;
};

struct poly {

template <typename T>
    using allocator = std::scoped_allocator_adaptor<bsl::allocator<T>>;

template <class T>
    using list = std::list<T,allocator<T>>;
template <class T>
    using forward_list = std::forward_list<T,allocator<T>>;
template <class T>
    using deque = std::deque<T,allocator<T>>;
template <class T>
    using vector = std::vector<T,allocator<T>>;

template <class T>
    using basic_string =
        std::basic_string<T,std::char_traits<T>,allocator<T>>;

using string = basic_string<char>;

template <class Key, class T, class Compare = std::less<Key>>
    using map = std::map<
        Key,T,Compare,allocator<std::pair<const Key,T>>>;
template <class Key, class T, class Compare = std::less<Key>>
    using multimap = std::multimap<
        Key,T,Compare,allocator<std::pair<const Key,T>>>;

template <class Key, class Compare = std::less<Key>>
    using set =      std::set<Key,Compare,allocator<Key>>;
template <class Key, class Compare = std::less<Key>>
    using multiset = std::multiset<Key,Compare,allocator<Key>>;

template <class Key, class T,
          class Hash = std::hash<Key>, class Pred = std::equal_to<Key>>
    using unordered_map = std::unordered_map<
        Key,T,Hash,Pred,allocator<std::pair<const Key,T>>>;
template <class Key, class T,
          class Hash = std::hash<Key>, class Pred = std::equal_to<Key>>
    using unordered_multimap = std::unordered_multimap<
        Key,T,Hash,Pred,allocator<std::pair<const Key,T>>>;

template <class Key,
          class Hash = std::hash<Key>, class Pred = std::equal_to<Key>>
    using unordered_set = std::unordered_set<
        Key,Hash,Pred,allocator<Key>>;
template <class Key,
          class Hash = std::hash<Key>, class Pred = std::equal_to<Key>>
    using unordered_multiset = std::unordered_multiset<
        Key,Hash,Pred,allocator<Key>>;
};

#endif
