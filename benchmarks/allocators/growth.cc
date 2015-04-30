
#include <iostream>
#include <iomanip>
#include <memory>
#include <random>
#include <iterator>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <bsl_memory.h>
#include <bslma_testallocator.h>
#include <bslma_newdeleteallocator.h>
#include <bsls_stopwatch.h>

#include <bdlma_sequentialpool.h>
#include <bdlma_sequentialallocator.h>
#include <bdlma_bufferedsequentialallocator.h>
#include <bdlma_multipoolallocator.h>

#include <vector>
#include <string>
#include <unordered_set>
#include <scoped_allocator>
#include "allocont.h"

using namespace BloombergLP;

static const int max_problem_logsize = 30;

void usage(char const* cmd, int result)
{
    std::cerr <<
"usage: " << cmd << " <size> <split> [<csv>]\n"
"    size:  log2 of total element count, 20 -> 1,000,000\n"
"    split: log2 of container size, 10 -> 1,000\n"
"    csv:   if present, produce CSV: <<time>, <description>...>\n"
"    Number of containers used is 2^(size - split)\n"
"    1 <= size <= " << max_problem_logsize << ", 1 <= split <= size\n";
    exit(result);
}

// side_effect() touches memory in a way that optimizers will not optimize away

void side_effect(void* p, size_t len)
{
    static volatile thread_local char* target;
    target = (char*) p;
    memset((char*)target, 0xff, len);
}

// (not standard yet)
struct range {
    int start; int stop;
    struct iter {
        int i;
        bool operator!=(iter other) { return other.i != i; };
        iter& operator++() { ++i; return *this; };
        int operator*() { return i; }
    };
    iter begin() { return iter{start}; }
    iter end() { return iter{stop}; }
};

char trash[1 << 16];  // random characters to construct string keys from

alignas(long long) static char pool[1 << 30];

std::default_random_engine random_engine;
std::uniform_int_distribution<int> pos_dist(0, sizeof(trash) - 1000);
std::uniform_int_distribution<int> length_dist(33, 1000);

char* sptr() { return trash + pos_dist(random_engine); }
size_t slen() {return length_dist(random_engine); }

enum {
    VEC=1<<0, HASH=1<<1,
        VECVEC=1<<2, VECHASH=1<<3,
        HASHVEC=1<<4, HASHHASH=1<<5,
    INT=1<<6, STR=1<<7,
    SA=1<<8, MT=1<<9, MTD=1<<10, PL=1<<11, PLD=1<<12,
        PM=1<<13, PMD=1<<14, CT=1<<15, RT=1<<16
};

char const* const names[] = {
    "vector", "unordered_set", "vector:vector", "vector:unordered_set",
        "unordered_set:vector", "unordered_set:unordered_set",
    "int", "string",
    "new/delete", "monotonic", "monotonic/drop", "multipool", "multipool/drop",
        "multipool/monotonic", "multipool/monotonic/drop",
    "compile-time", "run-time"
};

void print_case(int mask)
{
    for (int i = 8, m = SA; m <= RT; ++i, m <<= 1) {
        if (m & mask) {
            if (m < CT)
                std::cout << "allocator: " << names[i] << ", ";
            else
                std::cout << "bound: " << names[i];
        }}
}
void print_datastruct(int mask)
{
    for (int i = 0, m = VEC; m < SA; ++i, (m <<= 1)) {
        if (m & mask) {
            std::cout << names[i];
            if (m < INT)
                std::cout << ":";
        }}
}

template <typename Work>
double measure(int mask, bool csv, double reference, Work work)
{
    if (mask & SA)
        std::cout << std::endl;
    if (!csv) {
        if (mask & SA) {
            print_datastruct(mask);
            std::cout << ":\n\n";
        }
        std::cout << "   ";
        print_case(mask);
        std::cout << std::endl;
    }

    int pipes[2];
    int result = pipe(pipes);
    if (result < 0) {
        std::cerr << "\nFailed pipe\n";
        std::cout << "\nFailed pipe\n";
        exit(-1);
    }
    union { double result_time; char buf[sizeof(double)]; };
    int pid = fork();
    if (pid < 0) {
        std::cerr << "\nFailed fork\n";
        std::cout << "\nFailed fork\n";
        exit(-1);
    } else if (pid > 0) {  // parent
        close(pipes[1]);
        int status = 0;
        waitpid(pid, &status, 0);
        if (status == 0) {
            int got = read(pipes[0], buf, sizeof(buf));
            if (got != sizeof(buf)) {
                std::cerr << "\nFailed read\n";
                std::cout << "\nFailed read\n";
                exit(-1);
            }
        } else {
            if (!csv) {
                std::cout << "    (failed)" << std::endl;
            } else {
                std::cout << "(failed), (failed%), ";
                print_datastruct(mask);
                std::cout << ", ";
                print_case(mask);
                std::cout << std::endl;
            }
        }
        close(pipes[0]);
    } else {  // child
        close(pipes[0]);
        bool failed = false;
        bsls::Stopwatch timer;
        try {
            timer.start(true);
            work();
        } catch (std::bad_alloc&) {
            failed = true;
        }
        timer.stop();

        double times[3] = { 0.0, 0.0, 0.0 };
        if (!failed)
            timer.accumulatedTimes(times, times+1, times+2);

        if (!csv) {
            if (!failed) {
                std::cout << "   sys: " << times[0]
                          << " user: " << times[1]
                          << " wall: " << times[2] << ", ";
                if ((mask & (SA|CT)) == (SA|CT)) {
                    std::cout << "(100%)\n";
                } else if (reference == 0.0) {  // reference run failed
                    std::cout << "(N/A%)\n";
                } else {
                    std::cout << (times[2] * 100.)/reference << "%\n";
                }
            } else {
                std::cout << "   (failed)\n";
            }
            std::cout << std::endl;
        } else {
            if (!failed) {
                std::cout << times[2] << ", ";
                if ((mask & (SA|CT)) == (SA|CT)) {
                    std::cout << "(100%), ";
                } else if (reference == 0.0) {  // reference run failed
                    std::cout << "(N/A%), ";
                } else {
                    std::cout << (times[2] * 100.)/reference << "%, ";
                }
            } else {
                std::cout << "(failed), (failed%), ";
            }
            print_datastruct(mask);
            std::cout << ", ";
            print_case(mask);
            std::cout << std::endl;
        }
        result_time = times[2];
        write(pipes[1], buf, sizeof(buf));
        close(pipes[1]);
        exit(failed);
    }

    return ((mask & (SA|CT)) == (SA|CT)) ? result_time : reference;
}

template <typename T>
struct my_hash {
    unsigned long operator()(T const& t)
        { return 1048583 * (1 + reinterpret_cast<unsigned long>(&t)); }
};
template <typename T>
struct my_equal {
    bool operator()(T const& t, T const& u)
        { return &t == &u; }
};

template <
    typename StdCont,
    typename MonoCont,
    typename MultiCont,
    typename PolyCont,
    typename Work>
void apply_allocation_strategies(
    int mask, int runs, int split, bool csv, Work work)
{
    double reference;
// allocator: std::allocator, bound: compile-time
    reference = measure((SA|mask|CT), csv, 0.0,
        [runs,split,work]() {
                for (int run: range{0, runs}) {
                    StdCont c;
                    c.reserve(split);
                    work(c,split);
                }});

// allocator: monotonic, bound: compile-time
    measure((MT|mask|CT), csv, reference,
        [runs,split,work]() {
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialPool bsp(pool, sizeof(pool));
                    MonoCont c(&bsp);
                    c.reserve(split);
                    work(c, split);
                }});

// allocator: monotonic, bound: compile-time, drop
    measure((MTD|mask|CT), csv, reference,
        [runs,split,work]() {
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialPool bsp(pool, sizeof(pool));
                    auto* c = new(bsp) MonoCont(&bsp);
                    c->reserve(split);
                    work(*c, split);
                }});


// allocator: multipool, bound: compile-time
    measure((PL|mask|CT), csv, reference,
        [runs,split,work]() {
                for (int run: range{0, runs}) {
                    bdlma::Multipool mp;
                    MultiCont c(&mp);
                    c.reserve(split);
                    work(c, split);
                }});

// allocator: multipool, bound: compile-time, drop
    measure((PLD|mask|CT), csv, reference,
        [runs,split,work]() {
                for (int run: range{0, runs}) {
                    bdlma::Multipool mp;
                    auto* c = new(mp) MultiCont(&mp);
                    c->reserve(split);
                    work(*c, split);
                }});

// allocator: multipool/monotonic, bound: compile-time
    measure((PM|mask|CT), csv, reference,
        [runs,split,work]() {
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    bdlma::Multipool mp(&bsa);
                    MultiCont c(&mp);
                    c.reserve(split);
                    work(c, split);
                }});

// allocator: multipool/monotonic, bound: compile-time, drop monotonic
    measure((PMD|mask|CT), csv, reference,
        [runs,split,work]() {
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    bdlma::Multipool* mp =
                            new(bsa) bdlma::Multipool(&bsa);
                    auto* c = new(*mp) MultiCont(mp);
                    c->reserve(split);
                    work(*c, split);
                }});


// allocator: newdelete, bound: run-time
    measure((SA|mask|RT), csv, reference,
        [runs,split,work]() {
                bslma::NewDeleteAllocator mfa;
                for (int run: range{0, runs}) {
                    PolyCont c(&mfa);
                    c.reserve(split);
                    work(c, split);
                }});

// allocator: monotonic, bound: run-time
    measure((MT|mask|RT), csv, reference,
        [runs,split,work]() {
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    PolyCont c(&bsa);
                    c.reserve(split);
                    work(c, split);
                }});

// allocator: monotonic, bound: run-time, drop
    measure((MTD|mask|RT), csv, reference,
        [runs,split,work]() {
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    auto* c = new(bsa) PolyCont(&bsa);
                    c->reserve(split);
                    work(*c, split);
                }});

// allocator: multipool, bound: run-time
    measure((PL|mask|RT), csv, reference,
        [runs,split,work]() {
                for (int run: range{0, runs}) {
                    bdlma::MultipoolAllocator mpa;
                    PolyCont c(&mpa);
                    c.reserve(split);
                    work(c, split);
                }});

// allocator: multipool, bound: run-time, drop
    measure((PLD|mask|RT), csv, reference,
        [runs,split,work]() {
                for (int run: range{0, runs}) {
                    bdlma::MultipoolAllocator mpa;
                    auto* c = new(mpa) PolyCont(&mpa);
                    c->reserve(split);
                    work(*c, split);
                }});

// allocator: multipool/monotonic, bound: run-time
    measure((PM|mask|RT), csv, reference,
        [runs,split,work]() {
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    bdlma::MultipoolAllocator mpa(&bsa);
                    PolyCont c(&mpa);
                    c.reserve(split);
                    work(c, split);
                }});

// allocator: multipool/monotonic, bound: run-time, drop monotonic
    measure((PMD|mask|RT), csv, reference,
        [runs,split,work]() {
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    bdlma::MultipoolAllocator* mp =
                            new(bsa) bdlma::MultipoolAllocator(&bsa);
                    auto* c = new(*mp) PolyCont(mp);
                    c->reserve(split);
                    work(*c, split);
                }});
}

void apply_containers(int runs, int split, bool csv)
{
    apply_allocation_strategies<
            std::vector<int>,monotonic::vector<int>,
            multipool::vector<int>,poly::vector<int>>(
        VEC|INT, runs * 128, split, csv,
        [] (auto& c, int elems) {
            for (int elt: range{0, elems}) {
                c.emplace_back(elt);
                side_effect(&c.back(), 4);
        }});

    apply_allocation_strategies<
            std::vector<std::string>,
            monotonic::vector<monotonic::string>,
            multipool::vector<multipool::string>,
            poly::vector<poly::string>>(
        VEC|STR, runs * 128, split, csv,
        [] (auto& c, int elems) {
            for (int elt: range{0, elems}) {
                c.emplace_back(sptr(), slen());
                side_effect(const_cast<char*>(c.back().data()), 4);
        }});

    apply_allocation_strategies<
            std::unordered_set<int>,monotonic::unordered_set<int>,
            multipool::unordered_set<int>,poly::unordered_set<int>>(
        HASH|INT, runs * 128, split, csv,
        [] (auto& c, int elems) {
            for (int elt: range{0, elems})
                c.emplace(elt);
        });

    apply_allocation_strategies<
            std::unordered_set<std::string>,
            monotonic::unordered_set<monotonic::string>,
            multipool::unordered_set<multipool::string>,
            poly::unordered_set<multipool::string>>(
        HASH|STR, runs * 128, split, csv,
        [] (auto& c, int elems) {
            for (int elt: range{0, elems})
                c.emplace(sptr(), slen());
        });

    apply_allocation_strategies<
            std::vector<std::vector<int>>,
            monotonic::vector<monotonic::vector<int>>,
            multipool::vector<multipool::vector<int>>,
            poly::vector<poly::vector<int>>>(
        VECVEC|INT, runs, split, csv,
        [] (auto& c, int elems) {
            c.emplace_back(128, 1);
            for (int elt: range{0, elems})
                c.emplace_back(c.back());
        });

    apply_allocation_strategies<
            std::vector<std::vector<std::string>>,
            monotonic::vector<monotonic::vector<monotonic::string>>,
            multipool::vector<multipool::vector<multipool::string>>,
            poly::vector<poly::vector<poly::string>>>(
        VECVEC|STR, runs, split, csv,
        [split] (auto& c, int elems) {
            typename std::decay<decltype(c)>::type::value_type s;
            s.reserve(128);
            for (int i : range{0,128})
                s.emplace_back(sptr(), slen());
            c.emplace_back(std::move(s));
            for (int elt: range{0, split})
                c.emplace_back(c.back());
        });

    apply_allocation_strategies<
            std::vector<std::unordered_set<int>>,
            monotonic::vector<monotonic::unordered_set<int>>,
            multipool::vector<multipool::unordered_set<int>>,
            poly::vector<poly::unordered_set<int>>>(
        VECHASH|INT, runs, split, csv,
        [split] (auto& c, int elems) {
            int in[128]; std::generate(in, in+128, random_engine);
            typename std::decay<decltype(c)>::type::value_type s(128);
            s.insert(in, in+128);
            c.push_back(std::move(s));
            for (int elt: range{0, split})
                c.emplace_back(c.back());
        });

    apply_allocation_strategies<
            std::vector<std::unordered_set<std::string>>,
            monotonic::vector<monotonic::unordered_set<monotonic::string>>,
            multipool::vector<multipool::unordered_set<multipool::string>>,
            poly::vector<poly::unordered_set<poly::string>>>(
        VECHASH|STR, runs, split, csv,
        [split] (auto& c, int elems) {
            typename std::decay<decltype(c)>::type::value_type s(128);
            for (int i : range{0,128})
                s.emplace(sptr(), slen());
            c.push_back(std::move(s));
            for (int elt: range{0, split})
                c.emplace_back(c.back());
        });

    apply_allocation_strategies<
            std::unordered_set<std::vector<int>,
                    my_hash<std::vector<int>>,
                    my_equal<std::vector<int>>>,
            monotonic::unordered_set<monotonic::vector<int>,
                    my_hash<monotonic::vector<int>>,
                    my_equal<monotonic::vector<int>>>,
            multipool::unordered_set<multipool::vector<int>,
                    my_hash<multipool::vector<int>>,
                    my_equal<multipool::vector<int>>>,
            poly::unordered_set<poly::vector<int>,
                    my_hash<poly::vector<int>>,
                    my_equal<poly::vector<int>>>>(
        HASHVEC|INT, runs, split, csv,
        [split] (auto& c, int elems) {
            typename std::decay<decltype(c)>::type::key_type s;
            s.reserve(128);
            for (int i: range{0, 128})
                s.emplace_back(random_engine());
            c.emplace(std::move(s));
            for (int elt: range{0, split})
                c.emplace(*c.begin());
        });

    apply_allocation_strategies<
            std::unordered_set<std::vector<std::string>,
                my_hash<std::vector<std::string>>,
                my_equal<std::vector<std::string>>>,
            monotonic::unordered_set<monotonic::vector<monotonic::string>,
                my_hash<monotonic::vector<monotonic::string>>,
                my_equal<monotonic::vector<monotonic::string>>>,
            multipool::unordered_set<multipool::vector<multipool::string>,
                my_hash<multipool::vector<multipool::string>>,
                my_equal<multipool::vector<multipool::string>>>,
            poly::unordered_set<poly::vector<poly::string>,
                my_hash<poly::vector<poly::string>>,
                my_equal<poly::vector<poly::string>>>>(
        HASHVEC|STR, runs, split, csv,
        [split] (auto& c, int elems) {
            typename std::decay<decltype(c)>::type::key_type s;
            s.reserve(128);
            for (int i: range{0, 128})
                s.emplace_back(sptr(), slen());
            c.emplace(std::move(s));
            for (int elt: range{0, split})
                c.emplace(*c.begin());
        });

    apply_allocation_strategies<
            std::unordered_set<std::unordered_set<int>,
                    my_hash<std::unordered_set<int>>,
                    my_equal<std::unordered_set<int>>>,
            monotonic::unordered_set<
                monotonic::unordered_set<int>,
                    my_hash<monotonic::unordered_set<int>>,
                    my_equal<monotonic::unordered_set<int>>>,
            multipool::unordered_set<
                multipool::unordered_set<int>,
                    my_hash<multipool::unordered_set<int>>,
                    my_equal<multipool::unordered_set<int>>>,
            poly::unordered_set<
                poly::unordered_set<int>,
                    my_hash<poly::unordered_set<int>>,
                    my_equal<poly::unordered_set<int>>>>(
        HASHHASH|INT, runs, split, csv,
        [split] (auto& c, int elems) {
            typename std::decay<decltype(c)>::type::key_type s(128);
            for (int i: range{0, 128})
                s.emplace(random_engine());
            c.emplace(std::move(s));
            for (int elt: range{0, split})
                c.emplace(*c.begin());
        });

    apply_allocation_strategies<
            std::unordered_set<std::unordered_set<std::string>,
                    my_hash<std::unordered_set<std::string>>,
                    my_equal<std::unordered_set<std::string>>>,
            monotonic::unordered_set<
                monotonic::unordered_set<monotonic::string>,
                    my_hash<monotonic::unordered_set<monotonic::string>>,
                    my_equal<monotonic::unordered_set<monotonic::string>>>,
            multipool::unordered_set<
                multipool::unordered_set<multipool::string>,
                    my_hash<multipool::unordered_set<multipool::string>>,
                    my_equal<multipool::unordered_set<multipool::string>>>,
            poly::unordered_set<
                poly::unordered_set<poly::string>,
                    my_hash<poly::unordered_set<poly::string>>,
                    my_equal<poly::unordered_set<poly::string>>>>(
        HASHHASH|STR, runs, split, csv,
        [split] (auto& c, int elems) {
            typename std::decay<decltype(c)>::type::key_type s(128);
            for (int i: range{0, 128})
                s.emplace(sptr(), slen());
            c.emplace(std::move(s));
            for (int elt: range{0, split})
                c.emplace(*c.begin());
        });
}


int main(int ac, char** av)
{
    std::ios::sync_with_stdio(false);
    if (ac != 3 && ac != 4)
        usage(*av, 1);
    int logsize = atoi(av[1]);
    int logsplit = atoi(av[2]);
    if (logsize < 1 || logsize > max_problem_logsize)
        usage(*av, 2);
    if (logsplit < 1 || logsplit > logsize)
        usage(*av, 3);
    bool csv = (ac == 4);

    if (!csv) {
        std::cout << "Total # of objects = 2^" << logsize
                  << ", # elements per container = 2^" << logsplit
                  << ", # rounds = 2^" << logsize - logsplit << "\n";
    }

    int size = 1 << logsize;
    int split = 1 << logsplit;
    int runs = size / split;

    std::uniform_int_distribution<char> char_dist('!', '~');
    for (char& c : trash)
        c = char_dist(random_engine);

    // The actual storage
    memset(pool, 1, sizeof(pool));  // Fault in real memory

    std::cout << std::setprecision(3);

    apply_containers(runs, split, csv);

    return 0;
}
