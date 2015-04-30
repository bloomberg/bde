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
        fprintf(stderr, "\nFailed pipe\n");
        printf("\nFailed pipe\n");
        exit(-1);
    }
    union { double result_time; char buf[sizeof(double)]; };
    int pid = fork();
    if (pid < 0) {
        fprintf(stderr, "\nFailed fork\n");
        printf("\nFailed fork\n");
        exit(-1);
    } else if (pid > 0) {  // parent
        close(pipes[1]);
        int got = read(pipes[0], buf, sizeof(buf));
        if (got != sizeof(buf)) {
            fprintf(stderr, "\nFailed read\n");
            printf("\nFailed read\n");
            exit(-1);
        }
        int status = 0;
        waitpid(pid, &status, 0);
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
        exit(0);
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
    alignas(long long) static char pool[1 << 30];
    memset(pool, 1, sizeof(pool));  // Fault in real memory

    double reference = 0.0;

    std::cout << std::setprecision(3);

// vector<int>
// allocator: std::allocator, bound: compile-time
    reference = measure((SA|VEC|INT|CT), csv, 0.0,
            [runs,split]() {
                for (int run: range{0, runs}) {
                    std::vector<int> c;
                    c.reserve(split);
                    for (int elt: range{0, split}) {
                        c.emplace_back(elt);
                        side_effect(&c.back(), 4);
                    }}});

#ifdef TEST
// test
    reference = measure((SA|VEC|INT|CT), csv, 0.0,
        [] { throw std::bad_alloc(); });
    measure((MT|VEC|INT|CT), csv, reference,
        [] { throw std::bad_alloc(); });
    measure((MT|VEC|INT|CT), csv, 1.0,
        [] { throw std::bad_alloc(); });
#endif

// allocator: monotonic, bound: compile-time
    measure((MT|VEC|INT|CT), csv, reference,
            [runs,split]() {
                for (int run: range{0, runs * 128}) {
                    bdlma::BufferedSequentialPool bsp(pool, sizeof(pool));
                    monotonic::vector<int> c(&bsp);
                    c.reserve(split);
                    for (int elt: range{0, split}) {
                        c.emplace_back(elt);
                        side_effect(&c.back(), 4);
                    }}});
#ifdef TEST
    return 1;
#endif

// allocator: monotonic, bound: compile-time, drop
    measure((MTD|VEC|INT|CT), csv, reference,
            [runs,split]() {
                for (int run: range{0, runs * 128}) {
                    bdlma::BufferedSequentialPool bsp(pool, sizeof(pool));
                    monotonic::vector<int>* c =
                            new(bsp) monotonic::vector<int>(&bsp);
                    c->reserve(split);
                    for (int elt: range{0, split}) {
                        c->emplace_back(elt);
                        side_effect(&c->back(), 4);
                    }}});


// allocator: multipool, bound: compile-time
    measure((PL|VEC|INT|CT), csv, reference,
            [runs,split]() {
                for (int run: range{0, runs * 128}) {
                    bdlma::Multipool mp;
                    multipool::vector<int> c(&mp);
                    c.reserve(split);
                    for (int elt: range{0, split}) {
                        c.emplace_back(elt);
                        side_effect(&c.back(), 4);
                    }}});

// allocator: multipool, bound: compile-time, drop
    measure((PLD|VEC|INT|CT), csv, reference,
            [runs,split]() {
                for (int run: range{0, runs * 128}) {
                    bdlma::Multipool mp;
                    multipool::vector<int>* c =
                            new(mp) multipool::vector<int>(&mp);
                    c->reserve(split);
                    for (int elt: range{0, split}) {
                        c->emplace_back(elt);
                        side_effect(&c->back(), 4);
                    }}});

// allocator: multipool/monotonic, bound: compile-time
    measure((PM|VEC|INT|CT), csv, reference,
            [runs,split]() {

                for (int run: range{0, runs * 128}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    bdlma::Multipool mp(&bsa);
                    multipool::vector<int> c(&mp); c.reserve(split);
                    for (int elt: range{0, split}) {
                        c.emplace_back(elt);
                        side_effect(&c.back(), 4);
                    }}});

// allocator: multipool/monotonic, bound: compile-time, drop monotonic
    measure((PMD|VEC|INT|CT), csv, reference,
            [runs,split]() {
                for (int run: range{0, runs * 128}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    bdlma::Multipool* mp =
                            new(bsa) bdlma::Multipool(&bsa);
                    multipool::vector<int>* c =
                            new(*mp) multipool::vector<int>(mp);
                    c->reserve(split);
                    for (int elt: range{0, split}) {
                        c->emplace_back(elt);
                        side_effect(&c->back(), 4);
                    }}});


// allocator: newdelete, bound: run-time
    measure((SA|VEC|INT|RT), csv, reference,
            [runs,split]() {
                bslma::NewDeleteAllocator mfa;
                for (int run: range{0, runs * 128}) {
                    poly::vector<int> c(&mfa); c.reserve(split);
                    for (int elt: range{0, split}) {
                        c.emplace_back(elt); side_effect(&c.back(), 4);
                    }}});

// allocator: monotonic, bound: run-time
    measure((MT|VEC|INT|RT), csv, reference,
            [runs,split]() {
                for (int run: range{0, runs * 128}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    poly::vector<int> c(&bsa); c.reserve(split);
                    for (int elt: range{0, split}) {
                        c.emplace_back(elt);
                        side_effect(&c.back(), 4);
                    }}});

// allocator: monotonic, bound: run-time, drop
    measure((MTD|VEC|INT|RT), csv, reference,
            [runs,split]() {
                for (int run: range{0, runs * 128}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    poly::vector<int>* c = new(bsa) poly::vector<int>(&bsa);
                    c->reserve(split);
                    for (int elt: range{0, split}) {
                        c->emplace_back(elt);
                        side_effect(&c->back(), 4);
                    }}});

// allocator: multipool, bound: run-time
    measure((PL|VEC|INT|RT), csv, reference,
            [runs,split]() {
                for (int run: range{0, runs * 128}) {
                    bdlma::MultipoolAllocator mpa;
                    poly::vector<int> c(&mpa); c.reserve(split);
                    for (int elt: range{0, split}) {
                        c.emplace_back(elt);
                        side_effect(&c.back(), 4);
                    }}});

// allocator: multipool, bound: run-time, drop
    measure((PLD|VEC|INT|RT), csv, reference,
            [runs,split]() {
                for (int run: range{0, runs}) {
                    bdlma::MultipoolAllocator mpa;
                    poly::vector<int>* c = new(mpa) poly::vector<int>(&mpa);
                    c->reserve(split);
                    for (int elt: range{0, split}) {
                        c->emplace_back(elt);
                        side_effect(&c->back(), 4);
                    }}});

// allocator: multipool/monotonic, bound: run-time
    measure((PM|VEC|INT|RT), csv, reference,
            [runs,split]() {
                for (int run: range{0, runs * 128}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    bdlma::MultipoolAllocator mpa(&bsa);
                    poly::vector<int> c(&mpa); c.reserve(split);
                    for (int elt: range{0, split}) {
                        c.emplace_back(elt);
                        side_effect(&c.back(), 4);
                    }}});

// allocator: multipool/monotonic, bound: run-time, drop monotonic
    measure((PMD|VEC|INT|RT), csv, reference,
            [runs,split]() {
                for (int run: range{0, runs * 128}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    bdlma::MultipoolAllocator* mp =
                            new(bsa) bdlma::MultipoolAllocator(&bsa);
                    poly::vector<int>* c = new(*mp) poly::vector<int>(mp);
                    c->reserve(split);
                    for (int elt: range{0, split}) {
                        c->emplace_back(elt);
                        side_effect(&c->back(), 4);
                    }}});


// vector<string>
// allocator: std::allocator, bound: compile-time
    reference = measure((SA|VEC|STR|CT), csv, 0.0,
            [runs,split]() {
                typedef std::vector<std::string> T;
                for (int run: range{0, runs * 128}) {
                    T c; c.reserve(split);
                    for (int elt: range{0, split}) {
                        c.emplace_back(sptr(), slen());
                        side_effect(const_cast<char*>(c.back().data()), 4);
                    }}});

// allocator: monotonic, bound: compile-time
    measure((MT|VEC|STR|CT), csv, reference,
            [runs,split]() {
                typedef monotonic::vector<monotonic::string> T;
                for (int run: range{0, runs * 128}) {
                    bdlma::BufferedSequentialPool bsp(pool, sizeof(pool));
                    T c(&bsp); c.reserve(split);
                    for (int elt: range{0, split}) {
                        c.emplace_back(sptr(), slen());
                        side_effect(const_cast<char*>(c.back().data()), 4);
                    }}});

// allocator: monotonic, bound: compile-time, drop
    measure((MTD|VEC|STR|CT), csv, reference,
            [runs,split]() {
                typedef monotonic::vector<monotonic::string> T;
                for (int run: range{0, runs * 128}) {
                    bdlma::BufferedSequentialPool bsp(pool, sizeof(pool));
                    T* c = new(bsp) T(&bsp); c->reserve(split);
                    for (int elt: range{0, split}) {
                        c->emplace_back(sptr(), slen());
                        side_effect(const_cast<char*>(c->back().data()), 4);
                    }}});


// allocator: multipool, bound: compile-time
    measure((PL|VEC|STR|CT), csv, reference,
            [runs,split]() {
                typedef multipool::vector<multipool::string> T;
                for (int run: range{0, runs * 128}) {
                    bdlma::Multipool mp;
                    T c(&mp); c.reserve(split);
                    for (int elt: range{0, split}) {
                        c.emplace_back(sptr(), slen());
                        side_effect(const_cast<char*>(c.back().data()), 4);
                    }}});

// allocator: multipool, bound: compile-time, drop
    measure((PLD|VEC|STR|CT), csv, reference,
            [runs,split]() {
                typedef multipool::vector<multipool::string> T;
                for (int run: range{0, runs * 128}) {
                    bdlma::Multipool mp;
                    T* c = new(mp) T(&mp); c->reserve(split);
                    for (int elt: range{0, split}) {
                        c->emplace_back(sptr(), slen());
                        side_effect(const_cast<char*>(c->back().data()), 4);
                    }}});

// allocator: multipool/monotonic, bound: compile-time
    measure((PM|VEC|STR|CT), csv, reference,
            [runs,split]() {
                typedef multipool::vector<multipool::string> T;
                for (int run: range{0, runs * 128}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    bdlma::Multipool mp(&bsa);
                    T c(&mp); c.reserve(split);
                    for (int elt: range{0, split}) {
                        c.emplace_back(sptr(), slen());
                        side_effect(const_cast<char*>(c.back().data()), 4);
                    }}});

// allocator: multipool/monotonic, bound: compile-time, drop monotonic
    measure((PMD|VEC|STR|CT), csv, reference,
            [runs,split]() {
                typedef multipool::vector<multipool::string> T;
                for (int run: range{0, runs * 128}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    bdlma::Multipool* mp = new(bsa) bdlma::Multipool(&bsa);
                    T* c = new(*mp) T(mp); c->reserve(split);
                    for (int elt: range{0, split}) {
                        c->emplace_back(sptr(), slen());
                        side_effect(const_cast<char*>(c->back().data()), 4);
                    }}});


// allocator: newdelete, bound: run-time
    measure((SA|VEC|STR|RT), csv, reference,
            [runs,split]() {
                typedef poly::vector<poly::string> T;
                bslma::NewDeleteAllocator mfa;
                for (int run: range{0, runs * 128}) {
                    T c(&mfa); c.reserve(split);
                    for (int elt: range{0, split}) {
                        c.emplace_back(sptr(), slen());
                        side_effect(const_cast<char*>(c.back().data()), 4);
                    }}});

// allocator: monotonic, bound: run-time
    measure((STR|VEC|MT|STR|RT), csv, reference,
            [runs,split]() {
                typedef poly::vector<poly::string> T;
                for (int run: range{0, runs * 128}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    T c(&bsa); c.reserve(split);
                    for (int elt: range{0, split}) {
                        c.emplace_back(sptr(), slen());
                        side_effect(const_cast<char*>(c.back().data()), 4);
                    }}});

// allocator: monotonic, bound: run-time, drop
    measure((MTD|VEC|STR|RT), csv, reference,
            [runs,split]() {
                typedef poly::vector<poly::string> T;
                for (int run: range{0, runs * 128}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    T* c = new(bsa) T(&bsa); c->reserve(split);
                    for (int elt: range{0, split}) {
                        c->emplace_back(sptr(), slen());
                        side_effect(const_cast<char*>(c->back().data()), 4);
                    }}});

// allocator: multipool, bound: run-time
    measure((PL|VEC|STR|RT), csv, reference,
            [runs,split]() {
                typedef poly::vector<poly::string> T;
                for (int run: range{0, runs * 128}) {
                    bdlma::MultipoolAllocator mpa;
                    T c(&mpa); c.reserve(split);
                    for (int elt: range{0, split}) {
                        c.emplace_back(sptr(), slen());
                        side_effect(const_cast<char*>(c.back().data()), 4);
                    }}});

// allocator: multipool, bound: run-time, drop
    measure((PLD|VEC|STR|RT), csv, reference,
            [runs,split]() {
                typedef poly::vector<poly::string> T;
                for (int run: range{0, runs * 128}) {
                    bdlma::MultipoolAllocator mpa;
                    T* c = new(mpa) T(&mpa); c->reserve(split);
                    for (int elt: range{0, split}) {
                        c->emplace_back(sptr(), slen());
                        side_effect(const_cast<char*>(c->back().data()), 4);
                    }}});

// allocator: multipool/monotonic, bound: run-time
    measure((PM|VEC|STR|RT), csv, reference,
            [runs,split]() {
                typedef poly::vector<poly::string> T;
                for (int run: range{0, runs * 128}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    bdlma::MultipoolAllocator mpa(&bsa);
                    T c(&mpa); c.reserve(split);
                    for (int elt: range{0, split}) {
                        c.emplace_back(sptr(), slen());
                        side_effect(const_cast<char*>(c.back().data()), 4);
                    }}});

// allocator: multipool/monotonic, bound: run-time, drop monotonic
    measure((PMD|VEC|STR|RT), csv, reference,
            [runs,split]() {
                typedef poly::vector<poly::string> T;
                for (int run: range{0, runs * 128}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    bdlma::MultipoolAllocator* mp =
                            new(bsa) bdlma::MultipoolAllocator(&bsa);
                    T* c = new(*mp) T(mp); c->reserve(split);
                    for (int elt: range{0, split}) {
                        c->emplace_back(sptr(), slen());
                        side_effect(const_cast<char*>(c->back().data()), 4);
                    }}});


// ================================================================

// unordered_set<int>
// allocator: std::allocator, bound: compile-time
    reference = measure((SA|HASH|INT|CT), csv, 0.0,
            [runs,split]() {
                for (int run: range{0, runs * 128}) {
                    std::unordered_set<int> c(split);
                    for (int elt: range{0, split}) {
                        c.emplace(elt);
                    }}});

// allocator: monotonic, bound: compile-time
    measure((MT|HASH|INT|CT), csv, reference,
            [runs,split]() {
                typedef monotonic::unordered_set<int> T;
                for (int run: range{0, runs * 128}) {
                    bdlma::BufferedSequentialPool bsp(pool, sizeof(pool));
                    T c(split, &bsp);
                    for (int elt: range{0, split}) {
                        c.emplace(elt);
                    }}});

// allocator: monotonic, bound: compile-time, drop
    measure((MTD|HASH|INT|CT), csv, reference,
            [runs,split]() {
                typedef monotonic::unordered_set<int> T;
                for (int run: range{0, runs * 128}) {
                    bdlma::BufferedSequentialPool bsp(pool, sizeof(pool));
                    T* c = new(bsp) T(split, &bsp);
                    for (int elt: range{0, split}) {
                        c->emplace(elt);
                    }}});

// allocator: multipool, bound: compile-time
    measure((PL|HASH|INT|CT), csv, reference,
            [runs,split]() {
                typedef multipool::unordered_set<int> T;
                for (int run: range{0, runs * 128}) {
                    bdlma::Multipool mp;
                    T c(split, &mp);
                    for (int elt: range{0, split}) {
                        c.emplace(elt);
                    }}});

// allocator: multipool, bound: compile-time, drop
    measure((PLD|HASH|INT|CT), csv, reference,
            [runs,split]() {
                typedef multipool::unordered_set<int> T;
                for (int run: range{0, runs * 128}) {
                    bdlma::Multipool mp;
                    T* c = new(mp) T(split, &mp);
                    for (int elt: range{0, split}) {
                        c->emplace(elt);
                    }}});

// allocator: multipool/monotonic, bound: compile-time
    measure((PM|HASH|INT|CT), csv, reference,
            [runs,split]() {
                typedef multipool::unordered_set<int> T;
                for (int run: range{0, runs * 128}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    bdlma::Multipool mp(&bsa);
                    T c(split, &mp);
                    for (int elt: range{0, split}) {
                        c.emplace(elt);
                    }}});

// allocator: multipool/monotonic, bound: compile-time, drop monotonic
    measure((PMD|HASH|INT|CT), csv, reference,
            [runs,split]() {
                typedef multipool::unordered_set<int> T;
                for (int run: range{0, runs * 128}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    bdlma::Multipool* mp = new(bsa) bdlma::Multipool(&bsa);
                    T* c = new(*mp) T(split, mp);
                    for (int elt: range{0, split}) {
                        c->emplace(elt);
                    }}});

// allocator: newdelete, bound: run-time
    measure((SA|HASH|INT|RT), csv, reference,
            [runs,split]() {
                typedef poly::unordered_set<int> T;
                bslma::NewDeleteAllocator mfa;
                for (int run: range{0, runs * 128}) {
                    T c(split, &mfa);
                    for (int elt: range{0, split}) {
                            c.emplace(elt);
                    }}});

// allocator: monotonic, bound: run-time
    measure((MT|HASH|INT|RT), csv, reference,
            [runs,split]() {
                typedef poly::unordered_set<int> T;
                for (int run: range{0, runs * 128}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    T c(split, &bsa);
                    for (int elt: range{0, split}) {
                        c.emplace(elt);
                    }}});

// allocator: monotonic, bound: run-time, drop
    measure((MTD|HASH|INT|RT), csv, reference,
            [runs,split]() {
                typedef poly::unordered_set<int> T;
                for (int run: range{0, runs * 128}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    T* c = new(bsa) T(split, &bsa);
                    for (int elt: range{0, split}) {
                        c->emplace(elt);
                    }}});

// allocator: multipool, bound: run-time
    measure((PL|HASH|INT|RT), csv, reference,
            [runs,split]() {
                typedef poly::unordered_set<int> T;
                for (int run: range{0, runs * 128}) {
                    bdlma::MultipoolAllocator mpa;
                    T c(split, &mpa);
                    for (int elt: range{0, split}) {
                        c.emplace(elt);
                    }}});

// allocator: multipool, bound: run-time, drop
    measure((PLD|HASH|INT|RT), csv, reference,
            [runs,split]() {
                typedef poly::unordered_set<int> T;
                for (int run: range{0, runs * 128}) {
                    bdlma::MultipoolAllocator mpa;
                    T* c = new(mpa) T(split, &mpa);
                    for (int elt: range{0, split}) {
                        c->emplace(elt);
                    }}});

// allocator: multipool/monotonic, bound: run-time
    measure((PM|HASH|INT|RT), csv, reference,
            [runs,split]() {
                typedef poly::unordered_set<int> T;
                for (int run: range{0, runs * 128}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    bdlma::MultipoolAllocator mpa(&bsa);
                    T c(split, &mpa);
                    for (int elt: range{0, split}) {
                        c.emplace(elt);
                    }}});

// allocator: multipool/monotonic, bound: run-time, drop monotonic
    measure((PMD|HASH|INT|RT), csv, reference,
            [runs,split]() {
                typedef poly::unordered_set<int> T;
                for (int run: range{0, runs * 128}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    bdlma::MultipoolAllocator* mp =
                            new(bsa) bdlma::MultipoolAllocator(&bsa);
                    T* c = new(*mp) T(split, mp);
                    for (int elt: range{0, split}) {
                        c->emplace(elt);
                    }}});


// unordered_set<string>
// allocator: std::allocator, bound: compile-time
    reference = measure((SA|HASH|INT|CT), csv, 0.0,
            [runs,split]() {
                typedef std::unordered_set<std::string> T;
                for (int run: range{0, runs * 128}) {
                    T c(split);
                    for (int elt: range{0, split}) {
                        c.emplace(sptr(), slen());
                    }}});

// allocator: monotonic, bound: compile-time
    measure((MT|HASH|STR|CT), csv, reference,
            [runs,split]() {
                typedef monotonic::unordered_set<monotonic::string> T;
                for (int run: range{0, runs * 128}) {
                    bdlma::BufferedSequentialPool bsp(pool, sizeof(pool));
                    T c(split, &bsp);
                    for (int elt: range{0, split}) {
                        c.emplace(sptr(), slen());
                    }}});

// allocator: monotonic, bound: compile-time, drop
    measure((MTD|HASH|STR|CT), csv, reference,
            [runs,split]() {
                typedef monotonic::unordered_set<monotonic::string> T;
                for (int run: range{0, runs * 128}) {
                    bdlma::BufferedSequentialPool bsp(pool, sizeof(pool));
                    T* c = new(bsp) T(split, &bsp);
                    for (int elt: range{0, split}) {
                        c->emplace(sptr(), slen());
                    }}});

// allocator: multipool, bound: compile-time
    measure((PL|HASH|STR|CT), csv, reference,
            [runs,split]() {
                typedef multipool::unordered_set<multipool::string> T;
                for (int run: range{0, runs * 128}) {
                    bdlma::Multipool mp;
                    T c(split, &mp);
                    for (int elt: range{0, split}) {
                        c.emplace(sptr(), slen());
                    }}});

// allocator: multipool, bound: compile-time, drop
    measure((PLD|HASH|STR|CT), csv, reference,
            [runs,split]() {
                typedef multipool::unordered_set<multipool::string> T;
                for (int run: range{0, runs * 128}) {
                    bdlma::Multipool mp;
                    T* c = new(mp) T(split, &mp);
                    for (int elt: range{0, split}) {
                        c->emplace(sptr(), slen());
                    }}});

// allocator: multipool/monotonic, bound: compile-time
    measure((PM|HASH|STR|CT), csv, reference,
            [runs,split]() {
                typedef multipool::unordered_set<multipool::string> T;
                for (int run: range{0, runs * 128}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    bdlma::Multipool mp(&bsa);
                    T c(split, &mp);
                    for (int elt: range{0, split}) {
                        c.emplace(sptr(), slen());
                    }}});

// allocator: multipool/monotonic, bound: compile-time, drop monotonic
    measure((PMD|HASH|STR|CT), csv, reference,
            [runs,split]() {
                typedef multipool::unordered_set<multipool::string> T;
                for (int run: range{0, runs * 128}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    bdlma::Multipool* mp = new(bsa) bdlma::Multipool(&bsa);
                    T* c = new(*mp) T(split, mp);
                    for (int elt: range{0, split}) {
                        c->emplace(sptr(), slen());
                    }}});

// allocator: newdelete, bound: run-time
    measure((SA|HASH|STR|RT), csv, reference,
            [runs,split]() {
                typedef poly::unordered_set<poly::string> T;
                bslma::NewDeleteAllocator mfa;
                for (int run: range{0, runs * 128}) {
                    T c(split, &mfa);
                    for (int elt: range{0, split}) {
                        c.emplace(sptr(), slen());
                    }}});

// allocator: monotonic, bound: run-time
    measure((MT|HASH|STR|RT), csv, reference,
            [runs,split]() {
                typedef poly::unordered_set<poly::string> T;
                for (int run: range{0, runs * 128}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    T c(split, &bsa);
                    for (int elt: range{0, split}) {
                        c.emplace(sptr(), slen());
                    }}});

// allocator: monotonic, bound: run-time, drop
    measure((MTD|HASH|STR|RT), csv, reference,
            [runs,split]() {
                typedef poly::unordered_set<poly::string> T;
                for (int run: range{0, runs * 128}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    T* c = new(bsa) T(split, &bsa);
                    for (int elt: range{0, split}) {
                        c->emplace(sptr(), slen());
                    }}});

// allocator: multipool, bound: run-time
    measure((PL|HASH|STR|RT), csv, reference,
            [runs,split]() {
                typedef poly::unordered_set<poly::string> T;
                for (int run: range{0, runs * 128}) {
                    bdlma::MultipoolAllocator mpa;
                    T c(split, &mpa);
                    for (int elt: range{0, split}) {
                        c.emplace(sptr(), slen());
                    }}});

// allocator: multipool, bound: run-time, drop
    measure((PLD|HASH|STR|RT), csv, reference,
            [runs,split]() {
                typedef poly::unordered_set<poly::string> T;
                for (int run: range{0, runs * 128}) {
                    bdlma::MultipoolAllocator mpa;
                    T* c = new(mpa) T(split, &mpa);
                    for (int elt: range{0, split}) {
                        c->emplace(sptr(), slen());
                    }}});

// allocator: multipool/monotonic, bound: run-time
    measure((PM|HASH|STR|RT), csv, reference,
            [runs,split]() {
                typedef poly::unordered_set<poly::string> T;
                for (int run: range{0, runs * 128}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    bdlma::MultipoolAllocator mpa(&bsa);
                    T c(split, &mpa);
                    for (int elt: range{0, split}) {
                        c.emplace(sptr(), slen());
                    }}});

// allocator: multipool/monotonic, bound: run-time, drop monotonic
    measure((PMD|HASH|STR|RT), csv, reference,
            [runs,split]() {
                typedef poly::unordered_set<poly::string> T;
                for (int run: range{0, runs * 128}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    bdlma::MultipoolAllocator* mp =
                            new(bsa) bdlma::MultipoolAllocator(&bsa);
                    T* c = new(*mp) T(split, mp);
                    for (int elt: range{0, split}) {
                        c->emplace(sptr(), slen());
                    }}});

// ================================================================

// vector<vector<int>>
// allocator: std::allocator, bound: compile-time
    reference = measure((SA|VECVEC|INT|CT), csv, 0.0,
            [runs,split]() {
                typedef std::vector<std::vector<int>> T;
                for (int run: range{0, runs}) {
                    T c; c.reserve(split);
                    c.emplace_back(100, 1);
                    for (int elt: range{0, split}) {
                        c.emplace_back(c.back());
                    }}});

// allocator: monotonic, bound: compile-time
    measure((MT|VECVEC|INT|CT), csv, reference,
            [runs,split]() {
                typedef monotonic::vector<monotonic::vector<int>> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialPool bsp(pool, sizeof(pool));
                    T c(&bsp); c.reserve(split);
                    c.emplace_back(100, 1);
                    for (int elt: range{0, split}) {
                        c.emplace_back(c.back());
                    }}});

// allocator: monotonic, bound: compile-time, drop
    measure((MTD|VECVEC|INT|CT), csv, reference,
            [runs,split]() {
                typedef monotonic::vector<monotonic::vector<int>> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialPool bsp(pool, sizeof(pool));
                    T* c = new(bsp) T(&bsp); c->reserve(split);
                    c->emplace_back(100, 1);
                    for (int elt: range{0, split}) {
                        c->emplace_back(c->back());
                    }}});

// allocator: multipool, bound: compile-time
    measure((PL|VECVEC|INT|CT), csv, reference,
            [runs,split]() {
                typedef multipool::vector<multipool::vector<int>> T;
                for (int run: range{0, runs}) {
                    bdlma::Multipool mp;
                    T c(&mp); c.reserve(split);
                    c.emplace_back(100, 1);
                    for (int elt: range{0, split}) {
                        c.emplace_back(c.back());
                    }}});

// allocator: multipool, bound: compile-time, drop
    measure((PLD|VECVEC|INT|CT), csv, reference,
            [runs,split]() {
                typedef multipool::vector<multipool::vector<int>> T;
                for (int run: range{0, runs}) {
                    bdlma::Multipool mp;
                    T* c = new(mp) T(&mp); c->reserve(split);
                    c->emplace_back(100, 1);
                    for (int elt: range{0, split}) {
                        c->emplace_back(c->back());
                    }}});

// allocator: multipool/monotonic, bound: compile-time
    measure((PM|VECVEC|INT|CT), csv, reference,
            [runs,split]() {
                typedef multipool::vector<multipool::vector<int>> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    bdlma::Multipool mp(&bsa);
                    T c(&mp); c.reserve(split);
                    c.emplace_back(100, 1);
                    for (int elt: range{0, split}) {
                        c.emplace_back(c.back());
                    }}});

// allocator: multipool/monotonic, bound: compile-time, drop monotonic
    measure((PMD|VECVEC|INT|CT), csv, reference,
            [runs,split]() {
                typedef multipool::vector<multipool::vector<int>> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    bdlma::Multipool* mp = new(bsa) bdlma::Multipool(&bsa);
                    T* c = new(*mp) T(mp); c->reserve(split);
                    c->emplace_back(100, 1);
                    for (int elt: range{0, split}) {
                        c->emplace_back(c->back());
                    }}});

// allocator: newdelete, bound: run-time
    measure((SA|VECVEC|INT|RT), csv, reference,
            [runs,split]() {
                typedef poly::vector<poly::vector<int>> T;
                bslma::NewDeleteAllocator mfa;
                for (int run: range{0, runs}) {
                    T c(&mfa); c.reserve(split);
                    c.emplace_back(100, 1);
                    for (int elt: range{0, split}) {
                        c.emplace_back(c.back());
                    }}});

// allocator: monotonic, bound: run-time
    measure((MT|VECVEC|INT|RT), csv, reference,
            [runs,split]() {
                typedef poly::vector<poly::vector<int>> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    T c(&bsa); c.reserve(split);
                    c.emplace_back(100, 1);
                    for (int elt: range{0, split}) {
                        c.emplace_back(c.back());
                    }}});

// allocator: monotonic, bound: run-time, drop
    measure((MTD|VECVEC|INT|RT), csv, reference,
            [runs,split]() {
                typedef poly::vector<poly::vector<int>> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    T* c = new(bsa) T(&bsa); c->reserve(split);
                    c->emplace_back(100, 1);
                    for (int elt: range{0, split}) {
                        c->emplace_back(c->back());
                    }}});

// allocator: multipool, bound: run-time
    measure((PL|VECVEC|INT|RT), csv, reference,
            [runs,split]() {
                typedef poly::vector<poly::vector<int>> T;
                for (int run: range{0, runs}) {
                    bdlma::MultipoolAllocator mpa;
                    T c(&mpa); c.reserve(split);
                    c.emplace_back(100, 1);
                    for (int elt: range{0, split}) {
                        c.emplace_back(c.back());
                    }}});

// allocator: multipool, bound: run-time, drop
    measure((PLD|VECVEC|INT|RT), csv, reference,
            [runs,split]() {
                typedef poly::vector<poly::vector<int>> T;
                for (int run: range{0, runs}) {
                    bdlma::MultipoolAllocator mpa;
                    T* c = new(mpa) T(&mpa); c->reserve(split);
                    c->emplace_back(100, 1);
                    for (int elt: range{0, split}) {
                        c->emplace_back(c->back());
                    }}});

// allocator: multipool/monotonic, bound: run-time
    measure((PM|VECVEC|INT|RT), csv, reference,
            [runs,split]() {
                typedef poly::vector<poly::vector<int>> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    bdlma::MultipoolAllocator mpa(&bsa);
                    T c(&mpa); c.reserve(split);
                    c.emplace_back(100, 1);
                    for (int elt: range{0, split}) {
                        c.emplace_back(c.back());
                    }}});

// allocator: multipool/monotonic, bound: run-time, drop monotonic
    measure((PMD|VECVEC|INT|RT), csv, reference,
            [runs,split]() {
                typedef poly::vector<poly::vector<int>> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    bdlma::MultipoolAllocator* mp =
                            new(bsa) bdlma::MultipoolAllocator(&bsa);
                    T* c = new(*mp) T(mp); c->reserve(split);
                    c->emplace_back(100, 1);
                    for (int elt: range{0, split}) {
                        c->emplace_back(c->back());
                    }}});

// vector<vector<string>>
// allocator: std::allocator, bound: compile-time
    reference = measure((SA|VECVEC|STR|CT), csv, 0.0,
            [runs,split]() {
                typedef std::vector<std::string> T1;
                typedef std::vector<std::vector<std::string>> T;
                for (int run: range{0, runs}) {
                    T c; c.reserve(split);
                    T1 s; s.reserve(100);
                    for (int i : range{0,100})
                        s.emplace_back(sptr(), slen());
                    c.emplace_back(std::move(s));
                    for (int elt: range{0, split}) {
                        c.emplace_back(c.back());
                    }}});

// allocator: monotonic, bound: compile-time
    measure((MT|VECVEC|STR|CT), csv, reference,
            [runs,split]() {
                typedef monotonic::vector<monotonic::string> T1;
                typedef monotonic::vector<T1> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialPool bsp(pool, sizeof(pool));
                    T c(&bsp); c.reserve(split);
                    T1 s(&bsp); s.reserve(100);
                    for (int i : range{0,100})
                        s.emplace_back(sptr(), slen());
                    c.push_back(std::move(s));
                    for (int elt: range{0, split}) {
                        c.emplace_back(c.back());
                    }}});

// allocator: monotonic, bound: compile-time, drop
    measure((MTD|VECVEC|STR|CT), csv, reference,
            [runs,split]() {
                typedef monotonic::vector<monotonic::string> T1;
                typedef monotonic::vector<T1> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialPool bsp(pool, sizeof(pool));
                    T* c = new(bsp) T(&bsp); c->reserve(split);
                    T1 s(&bsp); s.reserve(100);
                    for (int i : range{0,100})
                        s.emplace_back(sptr(), slen());
                    c->push_back(std::move(s));
                    for (int elt: range{0, split}) {
                        c->emplace_back(c->back());
                    }}});

// allocator: multipool, bound: compile-time
    measure((PL|VECVEC|STR|CT), csv, reference,
            [runs,split]() {
                typedef multipool::vector<multipool::string> T1;
                typedef multipool::vector<T1> T;
                for (int run: range{0, runs}) {
                    bdlma::Multipool mp;
                    T c(&mp); c.reserve(split);
                    T1 s(&mp); s.reserve(100);
                    for (int i : range{0,100})
                        s.emplace_back(sptr(), slen());
                    c.push_back(std::move(s));
                    for (int elt: range{0, split}) {
                        c.emplace_back(c.back());
                    }}});

// allocator: multipool, bound: compile-time, drop
    measure((PLD|VECVEC|STR|CT), csv, reference,
            [runs,split]() {
                typedef multipool::vector<multipool::string> T1;
                typedef multipool::vector<T1> T;
                for (int run: range{0, runs}) {
                    bdlma::Multipool mp;
                    T* c = new(mp) T(&mp); c->reserve(split);
                    T1 s(&mp); s.reserve(100);
                    for (int i : range{0,100})
                        s.emplace_back(sptr(), slen());
                    c->push_back(std::move(s));
                    for (int elt: range{0, split}) {
                        c->emplace_back(c->back());
                    }}});

// allocator: multipool/monotonic, bound: compile-time
    measure((PM|VECVEC|STR|CT), csv, reference,
            [runs,split]() {
                typedef multipool::vector<multipool::string> T1;
                typedef multipool::vector<T1> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    bdlma::Multipool mp(&bsa);
                    T c(&mp); c.reserve(split);
                    T1 s(&mp); s.reserve(100);
                    for (int i : range{0,100})
                        s.emplace_back(sptr(), slen());
                    c.push_back(std::move(s));
                    for (int elt: range{0, split}) {
                        c.emplace_back(c.back());
                    }}});
// allocator: multipool/monotonic, bound: compile-time, drop monotonic
    measure((PMD|VECVEC|STR|CT), csv, reference,
            [runs,split]() {
            typedef multipool::vector<multipool::string> T1;
            typedef multipool::vector<T1> T;
            for (int run: range{0, runs}) {
                bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                bdlma::Multipool* mp = new(bsa) bdlma::Multipool(&bsa);
                T* c = new(*mp) T(mp); c->reserve(split);
                T1 s(mp); s.reserve(100);
                for (int i : range{0,100})
                    s.emplace_back(sptr(), slen());
                c->push_back(std::move(s));
                for (int elt: range{0, split}) {
                    c->emplace_back(c->back());
                }}});

// allocator: newdelete, bound: run-time
    measure((SA|VECVEC|STR|RT), csv, reference,
            [runs,split]() {
                typedef poly::vector<poly::string> T1;
                typedef poly::vector<T1> T;
                bslma::NewDeleteAllocator mfa;
                for (int run: range{0, runs}) {
                    T c(&mfa); c.reserve(split);
                    T1 s(&mfa); s.reserve(100);
                    for (int i : range{0,100})
                        s.emplace_back(sptr(), slen());
                    c.push_back(std::move(s));
                    for (int elt: range{0, split}) {
                        c.emplace_back(c.back());
                    }}}); 
// allocator: monotonic, bound: run-time
    measure((MT|VECVEC|STR|RT), csv, reference,
            [runs,split]() {
                typedef poly::vector<poly::string> T1;
                typedef poly::vector<T1> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    T c(&bsa); c.reserve(split);
                    T1 s(&bsa); s.reserve(100);
                    for (int i : range{0,100})
                        s.emplace_back(sptr(), slen());
                    c.push_back(std::move(s));
                    for (int elt: range{0, split}) {
                        c.emplace_back(c.back());
                    }}});

// allocator: monotonic, bound: run-time, drop
    measure((MTD|VECVEC|STR|RT), csv, reference,
            [runs,split]() {
                typedef poly::vector<poly::string> T1;
                typedef poly::vector<T1> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    T* c = new(bsa) T(&bsa); c->reserve(split);
                    T1 s(&bsa); s.reserve(100);
                    for (int i : range{0,100})
                        s.emplace_back(sptr(), slen());
                    c->push_back(std::move(s));
                    for (int elt: range{0, split}) {
                        c->emplace_back(c->back());
                    }}});

// allocator: multipool, bound: run-time
    measure((PL|VECVEC|STR|RT), csv, reference,
            [runs,split]() {
                typedef poly::vector<poly::string> T1;
                typedef poly::vector<T1> T;
                for (int run: range{0, runs}) {
                    bdlma::MultipoolAllocator mpa;
                    T c(&mpa); c.reserve(split);
                    T1 s(&mpa); s.reserve(100);
                    for (int i : range{0,100})
                        s.emplace_back(sptr(), slen());
                    c.push_back(std::move(s));
                    for (int elt: range{0, split}) {
                        c.emplace_back(c.back());
                    }}});

// allocator: multipool, bound: run-time, drop
    measure((PLD|VECVEC|STR|RT), csv, reference,
            [runs,split]() {
                typedef poly::vector<poly::string> T1;
                typedef poly::vector<T1> T;
                for (int run: range{0, runs}) {
                    bdlma::MultipoolAllocator mpa;
                    T* c = new(mpa) T(&mpa); c->reserve(split);
                    T1 s(&mpa); s.reserve(100);
                    for (int i : range{0,100})
                        s.emplace_back(sptr(), slen());
                    c->push_back(std::move(s));
                    for (int elt: range{0, split}) {
                        c->emplace_back(c->back());
                    }}});

// allocator: multipool/monotonic, bound: run-time
    measure((PM|VECVEC|STR|RT), csv, reference,
            [runs,split]() {
                typedef poly::vector<poly::string> T1;
                typedef poly::vector<T1> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    bdlma::MultipoolAllocator mpa(&bsa);
                    T c(&mpa); c.reserve(split);
                    T1 s(&mpa); s.reserve(100);
                    for (int i : range{0,100})
                        s.emplace_back(sptr(), slen());
                    c.push_back(std::move(s));
                    for (int elt: range{0, split}) {
                        c.emplace_back(c.back());
                    }}});

// allocator: multipool/monotonic, bound: run-time, drop monotonic
    measure((PMD|VECVEC|STR|RT), csv, reference,
            [runs,split]() {
                typedef poly::vector<poly::string> T1;
                typedef poly::vector<T1> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    bdlma::MultipoolAllocator* mp =
                            new(bsa) bdlma::MultipoolAllocator(&bsa);
                    T* c = new(*mp) T(mp); c->reserve(split);
                    T1 s(mp); s.reserve(100);
                    for (int i : range{0,100})
                        s.emplace_back(sptr(), slen());
                    c->push_back(std::move(s));
                    for (int elt: range{0, split}) {
                        c->emplace_back(c->back());
                    }}});

// ================================================================

// vector<unordered_set<int>>
// allocator: std::allocator, bound: compile-time
    reference = measure((SA|VECHASH|INT|CT), csv, 0.0,
            [runs,split]() {
                typedef std::unordered_set<int> T1;
                typedef std::vector<T1> T;
                for (int run: range{0, runs}) {
                    T c; c.reserve(split);
                    int in[100]; std::generate(in, in+100, random_engine);
                    T1 s(100); s.insert(in, in+100);
                    c.push_back(std::move(s));
                    for (int elt: range{0, split}) {
                        c.emplace_back(c.back());
                    }}});

// allocator: monotonic, bound: compile-time
    measure((MT|VECHASH|INT|CT), csv, reference,
            [runs,split]() {
                typedef monotonic::unordered_set<int> T1;
                typedef monotonic::vector<T1> T;
                for (int run: range{0, runs}) {
                    int in[100]; std::generate(in, in+100, random_engine);
                    bdlma::BufferedSequentialPool bsp(pool, sizeof(pool));
                    T c(&bsp); c.reserve(split);
                    T1 s(100, &bsp); s.insert(in, in+100);
                    c.push_back(std::move(s));
                    for (int elt: range{0, split}) {
                        c.emplace_back(c.back());
                    }}});

// allocator: monotonic, bound: compile-time, drop
    measure((MTD|VECHASH|INT|CT), csv, reference,
            [runs,split]() {
                typedef monotonic::unordered_set<int> T1;
                typedef monotonic::vector<T1> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialPool bsp(pool, sizeof(pool));
                    int in[100]; std::generate(in, in+100, random_engine);
                    T* c = new(bsp) T(&bsp); c->reserve(split);
                    T1 s(100, &bsp); s.insert(in, in+100);
                    c->push_back(std::move(s));
                    for (int elt: range{0, split}) {
                        c->emplace_back(c->back());
                    }}});

// allocator: multipool, bound: compile-time
    measure((PL|VECHASH|INT|CT), csv, reference,
            [runs,split]() {
                typedef multipool::unordered_set<int> T1;
                typedef multipool::vector<T1> T;
                for (int run: range{0, runs}) {
                    bdlma::Multipool mp;
                    int in[100]; std::generate(in, in+100, random_engine);
                    T c(&mp); c.reserve(split);
                    T1 s(100, &mp); s.insert(in, in+100);
                    c.push_back(std::move(s));
                    for (int elt: range{0, split}) {
                        c.emplace_back(c.back());
                    }}});

// allocator: multipool, bound: compile-time, drop
    measure((PLD|VECHASH|INT|CT), csv, reference,
            [runs,split]() {
                typedef multipool::unordered_set<int> T1;
                typedef multipool::vector<T1> T;
                for (int run: range{0, runs}) {
                    bdlma::Multipool mp;
                    int in[100]; std::generate(in, in+100, random_engine);
                    T* c = new(mp) T(&mp); c->reserve(split);
                    T1 s(100, &mp); s.insert(in, in+100);
                    c->push_back(std::move(s));
                    for (int elt: range{0, split}) {
                        c->emplace_back(c->back());
                    }}});

// allocator: multipool/monotonic, bound: compile-time
    measure((PM|VECHASH|INT|CT), csv, reference,
            [runs,split]() {
                typedef multipool::unordered_set<int> T1;
                typedef multipool::vector<T1> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    bdlma::Multipool mp(&bsa);
                    int in[100]; std::generate(in, in+100, random_engine);
                    T c(&mp); c.reserve(split);
                    T1 s(100, &mp); s.insert(in, in+100);
                    c.push_back(std::move(s));
                    for (int elt: range{0, split}) {
                        c.emplace_back(c.back());
                    }}});

// allocator: multipool/monotonic, bound: compile-time, drop monotonic
    measure((PMD|VECHASH|INT|CT), csv, reference,
            [runs,split]() {
                typedef multipool::unordered_set<int> T1;
                typedef multipool::vector<T1> T;
                for (int run: range{0, runs}) {
                    int in[100]; std::generate(in, in+100, random_engine);
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    bdlma::Multipool* mp = new(bsa) bdlma::Multipool(&bsa);
                    T* c = new(*mp) T(mp); c->reserve(split);
                    T1 s(100, mp); s.insert(in, in+100);
                    c->push_back(std::move(s));
                    for (int elt: range{0, split}) {
                        c->emplace_back(c->back());
                    }}});

// allocator: newdelete, bound: run-time
    measure((SA|VECHASH|INT|RT), csv, reference,
            [runs,split]() {
                typedef poly::unordered_set<int> T1;
                typedef poly::vector<T1> T;
                bslma::NewDeleteAllocator mfa;
                for (int run: range{0, runs}) {
                    int in[100]; std::generate(in, in+100, random_engine);
                    T c(&mfa); c.reserve(split);
                    T1 s(&mfa); s.insert(in, in+100);
                    c.push_back(std::move(s));
                    for (int elt: range{0, split}) {
                        c.emplace_back(c.back());
                    }}});

// allocator: monotonic, bound: run-time
    measure((MT|VECHASH|INT|RT), csv, reference,
            [runs,split]() {
                typedef poly::unordered_set<int> T1;
                typedef poly::vector<T1> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    int in[100]; std::generate(in, in+100, random_engine);
                    T c(&bsa); c.reserve(split);
                    T1 s(100, &bsa); s.insert(in, in+100);
                    c.push_back(std::move(s));
                    for (int elt: range{0, split}) {
                        c.emplace_back(c.back());
                    }}});

// allocator: monotonic, bound: run-time, drop
    measure((MTD|VECHASH|INT|RT), csv, reference,
            [runs,split]() {
                typedef poly::unordered_set<int> T1;
                typedef poly::vector<T1> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    int in[100]; std::generate(in, in+100, random_engine);
                    T* c = new(bsa) T(&bsa); c->reserve(split);
                    T1 s(&bsa); s.insert(in, in+100);
                    c->push_back(std::move(s));
                    for (int elt: range{0, split}) {
                        c->emplace_back(c->back());
                    }}});

// allocator: multipool, bound: run-time
    measure((PL|VECHASH|INT|RT), csv, reference,
            [runs,split]() {
                typedef poly::unordered_set<int> T1;
                typedef poly::vector<T1> T;
                for (int run: range{0, runs}) {
                    bdlma::MultipoolAllocator mpa;
                    int in[100]; std::generate(in, in+100, random_engine);
                    T c(&mpa); c.reserve(split);
                    T1 s(100, &mpa); s.insert(in, in+100);
                    c.push_back(std::move(s));
                    for (int elt: range{0, split}) {
                        c.emplace_back(c.back());
                    }}});

// allocator: multipool, bound: run-time, drop
    measure((PLD|VECHASH|INT|RT), csv, reference,
            [runs,split]() {
                typedef poly::unordered_set<int> T1;
                typedef poly::vector<T1> T;
                for (int run: range{0, runs}) {
                    bdlma::MultipoolAllocator mpa;
                    int in[100]; std::generate(in, in+100, random_engine);
                    T* c = new(mpa) T(&mpa); c->reserve(split);
                    T1 s(&mpa); s.insert(in, in+100);
                    c->push_back(std::move(s));
                    for (int elt: range{0, split}) {
                        c->emplace_back(c->back());
                    }}});

// allocator: multipool/monotonic, bound: run-time
    measure((PM|VECHASH|INT|RT), csv, reference,
            [runs,split]() {
                typedef poly::unordered_set<int> T1;
                typedef poly::vector<T1> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    bdlma::MultipoolAllocator mpa(&bsa);
                    int in[100]; std::generate(in, in+100, random_engine);
                    T c(&mpa); c.reserve(split);
                    T1 s(100, &mpa); s.insert(in, in+100);
                    c.push_back(std::move(s));
                    for (int elt: range{0, split}) {
                        c.emplace_back(c.back());
                    }}});

// allocator: multipool/monotonic, bound: run-time, drop monotonic
    measure((PMD|VECHASH|INT|RT), csv, reference,
            [runs,split]() {
                typedef poly::unordered_set<int> T1;
                typedef poly::vector<T1> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    int in[100]; std::generate(in, in+100, random_engine);
                    bdlma::MultipoolAllocator* mp =
                            new(bsa) bdlma::MultipoolAllocator(&bsa);
                    T* c = new(*mp) T(mp); c->reserve(split);
                    T1 s(100, mp); s.insert(in, in+100);
                    c->push_back(std::move(s));
                    for (int elt: range{0, split}) {
                        c->emplace_back(c->back());
                    }}});


// vector<unordered_set<string>>
// allocator: std::allocator, bound: compile-time
    reference = measure((SA|VECHASH|STR|CT), csv, 0.0,
            [runs,split]() {
                typedef std::unordered_set<std::string> T1;
                typedef std::vector<T1> T;
                for (int run: range{0, runs}) {
                    T c; c.reserve(split);
                    T1 s(100); for (int i : range{0,100})
                              s.emplace(sptr(), slen());
                    c.push_back(std::move(s));
                    for (int elt: range{0, split}) {
                        c.emplace_back(c.back());
                    }}});

// allocator: monotonic, bound: compile-time
    measure((MT|VECHASH|STR|CT), csv, reference,
            [runs,split]() {
                typedef monotonic::unordered_set<monotonic::string> T1;
                typedef monotonic::vector<T1> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialPool bsp(pool, sizeof(pool));
                    T c(&bsp); c.reserve(split);
                    T1 s(100, &bsp);
                    for (int i : range{0,100})
                        s.emplace(sptr(), slen());
                    c.push_back(std::move(s));
                    for (int elt: range{0, split}) {
                        c.emplace_back(c.back());
                    }}});

// allocator: monotonic, bound: compile-time, drop
    measure((MTD|VECHASH|STR|CT), csv, reference,
            [runs,split]() {
                typedef monotonic::unordered_set<monotonic::string> T1;
                typedef monotonic::vector<T1> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialPool bsp(pool, sizeof(pool));
                    T* c = new(bsp) T(&bsp); c->reserve(split);
                    T1 s(128, &bsp); for (int i : range{0, 128})
                                        s.emplace(sptr(), slen());
                    c->push_back(std::move(s));
                    for (int elt: range{0, split}) {
                        c->emplace_back(c->back());
                    }}});

// allocator: multipool, bound: compile-time
    measure((PL|VECHASH|STR|CT), csv, reference,
            [runs,split]() {
                typedef multipool::unordered_set<multipool::string> T1;
                typedef multipool::vector<T1> T;
                for (int run: range{0, runs}) {
                    bdlma::Multipool mp;
                    T c(&mp); c.reserve(split);
                    T1 s(128, &mp);
                    for (int i : range{0, 128}) s.emplace(sptr(), slen());
                    c.push_back(std::move(s));
                    for (int elt: range{0, split}) {
                        c.emplace_back(c.back());
                    }}});

// allocator: multipool, bound: compile-time, drop
    measure((PLD|VECHASH|STR|CT), csv, reference,
            [runs,split]() {
                typedef multipool::unordered_set<multipool::string> T1;
                typedef multipool::vector<T1> T;
                for (int run: range{0, runs}) {
                    bdlma::Multipool mp;
                    T* c = new(mp) T(&mp); c->reserve(split);
                    T1 s(128, &mp);
                    for (int i : range{0, 128})
                        s.emplace(sptr(), slen());
                    c->push_back(std::move(s));
                    for (int elt: range{0, split}) {
                        c->emplace_back(c->back());
                    }}});

// allocator: multipool/monotonic, bound: compile-time
    measure((PM|VECHASH|STR|CT), csv, reference,
            [runs,split]() {
                typedef multipool::unordered_set<multipool::string> T1;
                typedef multipool::vector<T1> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    bdlma::Multipool mp(&bsa);
                    T c(&mp); c.reserve(split);
                    T1 s(128, &mp);
                    for (int i : range{0, 128})
                        s.emplace(sptr(), slen());
                    c.push_back(std::move(s));
                    for (int elt: range{0, split}) {
                        c.emplace_back(c.back());
                    }}});

// allocator: multipool/monotonic, bound: compile-time, drop monotonic
    measure((PMD|VECHASH|STR|CT), csv, reference,
            [runs,split]() {
                typedef multipool::unordered_set<multipool::string> T1;
                typedef multipool::vector<T1> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    bdlma::Multipool* mp = new(bsa) bdlma::Multipool(&bsa);
                    T* c = new(*mp) T(mp); c->reserve(split);
                    T1 s(128, mp);
                    for (int i : range{0, 128})
                        s.emplace(sptr(), slen());
                    c->push_back(std::move(s));
                    for (int elt: range{0, split}) {
                        c->emplace_back(c->back());
                    }}});

// allocator: newdelete, bound: run-time
    measure((SA|VECHASH|STR|RT), csv, reference,
            [runs,split]() {
                typedef poly::unordered_set<poly::string> T1;
                typedef poly::vector<T1> T;
                bslma::NewDeleteAllocator mfa;
                for (int run: range{0, runs}) {
                    T c(&mfa); c.reserve(split);
                    T1 s(128, &mfa);
                    for (int i : range{0, 128})
                        s.emplace(sptr(), slen());
                    c.push_back(std::move(s));
                    for (int elt: range{0, split}) {
                        c.emplace_back(c.back());
                    }}});


// allocator: monotonic, bound: run-time
    measure((MT|VECHASH|STR|RT), csv, reference,
            [runs,split]() {
                typedef poly::unordered_set<poly::string> T1;
                typedef poly::vector<T1> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    T c(&bsa); c.reserve(split);
                    T1 s(128, &bsa);
                    for (int i : range{0, 128})
                        s.emplace(sptr(), slen());
                    c.push_back(std::move(s));
                    for (int elt: range{0, split}) {
                        c.emplace_back(c.back());
                    }}});

// allocator: monotonic, bound: run-time, drop
    measure((MTD|VECHASH|STR|RT), csv, reference,
            [runs,split]() {
                typedef poly::unordered_set<poly::string> T1;
                typedef poly::vector<T1> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    T* c = new(bsa) T(&bsa); c->reserve(split);
                    T1 s(128, &bsa);
                    for (int i : range{0, 128})
                        s.emplace(sptr(), slen());
                    c->push_back(std::move(s));
                    for (int elt: range{0, split}) {
                        c->emplace_back(c->back());
                    }}});

// allocator: multipool, bound: run-time
    measure((PL|VECHASH|STR|RT), csv, reference,
            [runs,split]() {
                typedef poly::unordered_set<poly::string> T1;
                typedef poly::vector<T1> T;
                for (int run: range{0, runs}) {
                    bdlma::MultipoolAllocator mpa;
                    T c(&mpa); c.reserve(split);
                    T1 s(128, &mpa);
                    for (int i : range{0, 128})
                        s.emplace(sptr(), slen());
                    c.push_back(std::move(s));
                    for (int elt: range{0, split}) {
                        c.emplace_back(c.back());
                    }}});

// allocator: multipool, bound: run-time, drop
    measure((PLD|VECHASH|STR|RT), csv, reference,
            [runs,split]() {
                typedef poly::unordered_set<poly::string> T1;
                typedef poly::vector<T1> T;
                for (int run: range{0, runs}) {
                    bdlma::MultipoolAllocator mpa;
                    T* c = new(mpa) T(&mpa); c->reserve(split);
                    T1 s(128, &mpa);
                    for (int i : range{0, 128})
                        s.emplace(sptr(), slen());
                    c->push_back(std::move(s));
                    for (int elt: range{0, split}) {
                        c->emplace_back(c->back());
                    }}});

// allocator: multipool/monotonic, bound: run-time
    measure((PM|VECHASH|STR|RT), csv, reference,
            [runs,split]() {
                typedef poly::unordered_set<poly::string> T1;
                typedef poly::vector<T1> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    bdlma::MultipoolAllocator mpa(&bsa);
                    T c(&mpa); c.reserve(split);
                    T1 s(128, &mpa);
                    for (int i : range{0, 128})
                        s.emplace(sptr(), slen());
                    c.push_back(std::move(s));
                    for (int elt: range{0, split}) {
                        c.emplace_back(c.back());
                    }}});

// allocator: multipool/monotonic, bound: run-time, drop monotonic
    measure((PMD|VECHASH|STR|RT), csv, reference,
            [runs,split]() {
                typedef poly::unordered_set<poly::string> T1;
                typedef poly::vector<T1> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    bdlma::MultipoolAllocator* mp =
                            new(bsa) bdlma::MultipoolAllocator(&bsa);
                    T* c = new(*mp) T(mp); c->reserve(split);
                    T1 s(128, mp);
                    for (int i : range{0, 128})
                        s.emplace(sptr(), slen());
                    c->push_back(std::move(s));
                    for (int elt: range{0, split}) {
                        c->emplace_back(c->back());
                    }}});

// ================================================================

// unordered_set<vector<<int>>
// allocator: std::allocator, bound: compile-time
    reference = measure((SA|HASHVEC|INT|CT), csv, 0.0,
            [runs,split]() {
                typedef std::vector<int> T1;
                typedef std::unordered_set<T1,my_hash<T1>,my_equal<T1>> T;
                for (int run: range{0, runs}) {
                    T c(split);
                    T1 s; s.reserve(128);
                    for (int i: range{0, 128})
                        s.emplace_back(random_engine());
                    c.emplace(std::move(s));
                    for (int elt: range{0, split}) {
                        c.emplace(*c.begin());
                    }}});

// allocator: monotonic, bound: compile-time
    measure((MT|HASHVEC|INT|CT), csv, reference,
            [runs,split]() {
                typedef monotonic::vector<int> T1;
                typedef monotonic::unordered_set<T1,my_hash<T1>,my_equal<T1>> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialPool bsp(pool, sizeof(pool));
                    T c(split, &bsp);
                    T1 s(&bsp); s.reserve(128);
                    for (int i: range{0, 128})
                        s.emplace_back(random_engine());
                    c.emplace(std::move(s));
                    for (int elt: range{0, split}) {
                        c.emplace(*c.begin());
                    }}});

// allocator: monotonic, bound: compile-time, drop
    measure((MTD|HASHVEC|INT|CT), csv, reference,
            [runs,split]() {
                typedef monotonic::vector<int> T1;
                typedef monotonic::unordered_set<T1,my_hash<T1>,my_equal<T1>> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialPool bsp(pool, sizeof(pool));
                    T* c = new(bsp) T(split, &bsp);
                    T1 s(&bsp); s.reserve(128);
                    for (int i: range{0, 128})
                        s.emplace_back(random_engine());
                    c->emplace(std::move(s));
                    for (int elt: range{0, split}) {
                        c->emplace(*c->begin());
                    }}});

// allocator: multipool, bound: compile-time
    measure((PL|HASHVEC|INT|CT), csv, reference,
            [runs,split]() {
                typedef multipool::vector<int> T1;
                typedef multipool::unordered_set<T1,my_hash<T1>,my_equal<T1>> T;
                for (int run: range{0, runs}) {
                    bdlma::Multipool mp;
                    T c(split, &mp);
                    T1 s(&mp); s.reserve(128);
                    for (int i: range{0, 128})
                        s.emplace_back(random_engine());
                    c.emplace(std::move(s));
                    for (int elt: range{0, split}) {
                        c.emplace(*c.begin());
                    }}});

// allocator: multipool, bound: compile-time, drop
    measure((PLD|HASHVEC|INT|CT), csv, reference,
            [runs,split]() {
                typedef multipool::vector<int> T1;
                typedef multipool::unordered_set<T1,my_hash<T1>,my_equal<T1>> T;
                for (int run: range{0, runs}) {
                    bdlma::Multipool mp;
                    T* c = new(mp) T(split, &mp);
                    T1 s(&mp); s.reserve(128);
                    for (int i: range{0, 128})
                        s.emplace_back(random_engine());
                    c->emplace(std::move(s));
                    for (int elt: range{0, split}) {
                        c->emplace(*c->begin());
                    }}});

// allocator: multipool/monotonic, bound: compile-time
    measure((PM|HASHVEC|INT|CT), csv, reference,
            [runs,split]() {
                typedef multipool::vector<int> T1;
                typedef multipool::unordered_set<T1,my_hash<T1>,my_equal<T1>> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    bdlma::Multipool mp(&bsa);
                    T c(split, &mp);
                    T1 s(&mp); s.reserve(128);
                    for (int i: range{0, 128})
                        s.emplace_back(random_engine());
                    c.emplace(std::move(s));
                    for (int elt: range{0, split}) {
                        c.emplace(*c.begin());
                    }}});

// allocator: multipool/monotonic, bound: compile-time, drop monotonic
    measure((PMD|HASHVEC|INT|CT), csv, reference,
            [runs,split]() {
                typedef multipool::vector<int> T1;
                typedef multipool::unordered_set<T1,my_hash<T1>,my_equal<T1>> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    bdlma::Multipool* mp = new(bsa) bdlma::Multipool(&bsa);
                    T* c = new(*mp) T(split, mp);
                    T1 s(mp); s.reserve(128);
                    for (int i: range{0, 128})
                        s.emplace_back(random_engine());
                    c->emplace(std::move(s));
                    for (int elt: range{0, split}) {
                        c->emplace(*c->begin());
                    }}});


// allocator: newdelete, bound: run-time
    measure((SA|HASHVEC|INT|RT), csv, reference,
            [runs,split]() {
                typedef poly::vector<int> T1;
                typedef poly::unordered_set<T1,my_hash<T1>,my_equal<T1>> T;
                bslma::NewDeleteAllocator mfa;
                for (int run: range{0, runs}) {
                    T c(split, &mfa);
                    T1 s(&mfa); s.reserve(128);
                    for (int i: range{0, 128})
                        s.emplace_back(random_engine());
                    c.emplace(std::move(s));
                    for (int elt: range{0, split}) {
                        c.emplace(*c.begin());
                    }}});

// allocator: monotonic, bound: run-time
    measure((MT|HASHVEC|INT|RT), csv, reference,
            [runs,split]() {
                typedef poly::vector<int> T1;
                typedef poly::unordered_set<T1,my_hash<T1>,my_equal<T1>> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    T c(split, &bsa);
                    T1 s(&bsa); s.reserve(128);
                    for (int i: range{0, 128})
                        s.emplace_back(random_engine());
                    c.emplace(std::move(s));
                    for (int elt: range{0, split}) {
                        c.emplace(*c.begin());
                    }}});

// allocator: monotonic, bound: run-time, drop
    measure((MTD|HASHVEC|INT|RT), csv, reference,
            [runs,split]() {
                typedef poly::vector<int> T1;
                typedef poly::unordered_set<T1,my_hash<T1>,my_equal<T1>> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    T* c = new(bsa) T(split, &bsa);
                    T1 s(&bsa); s.reserve(128);
                    for (int i: range{0, 128})
                        s.emplace_back(random_engine());
                    c->emplace(std::move(s));
                    for (int elt: range{0, split}) {
                        c->emplace(*c->begin());
                    }}});

// allocator: multipool, bound: run-time
    measure((PL|HASHVEC|INT|RT), csv, reference,
            [runs,split]() {
                typedef poly::vector<int> T1;
                typedef poly::unordered_set<T1,my_hash<T1>,my_equal<T1>> T;
                for (int run: range{0, runs}) {
                    bdlma::MultipoolAllocator mpa;
                    T c(split, &mpa);
                    T1 s(&mpa); s.reserve(128);
                    for (int i: range{0, 128})
                        s.emplace_back(random_engine());
                    c.emplace(std::move(s));
                    for (int elt: range{0, split}) {
                        c.emplace(*c.begin());
                    }}});

// allocator: multipool, bound: run-time, drop
    measure((PLD|HASHVEC|INT|RT), csv, reference,
            [runs,split]() {
                typedef poly::vector<int> T1;
                typedef poly::unordered_set<T1,my_hash<T1>,my_equal<T1>> T;
                for (int run: range{0, runs}) {
                    bdlma::MultipoolAllocator mpa;
                    T* c = new(mpa) T(split, &mpa);
                    T1 s(&mpa); s.reserve(128);
                    for (int i: range{0, 128})
                        s.emplace_back(random_engine());
                    c->emplace(std::move(s));
                    for (int elt: range{0, split}) {
                        c->emplace(*c->begin());
                    }}});

// allocator: multipool/monotonic, bound: run-time
    measure((PM|HASHVEC|INT|RT), csv, reference,
            [runs,split]() {
                typedef poly::vector<int> T1;
                typedef poly::unordered_set<T1,my_hash<T1>,my_equal<T1>> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    bdlma::MultipoolAllocator mpa(&bsa);
                    T c(split, &mpa);
                    T1 s(&mpa); s.reserve(128);
                    for (int i: range{0, 128})
                        s.emplace_back(random_engine());
                    c.emplace(std::move(s));
                    for (int elt: range{0, split}) {
                        c.emplace(*c.begin());
                    }}});

// allocator: multipool/monotonic, bound: run-time, drop monotonic
    measure((PMD|HASHVEC|INT|RT), csv, reference,
            [runs,split]() {
                typedef poly::vector<int> T1;
                typedef poly::unordered_set<T1,my_hash<T1>,my_equal<T1>> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    bdlma::MultipoolAllocator* mp =
                            new(bsa) bdlma::MultipoolAllocator(&bsa);
                    T* c = new(*mp) T(split, mp);
                    T1 s(mp); s.reserve(128);
                    for (int i: range{0, 128})
                        s.emplace_back(random_engine());
                    c->emplace(std::move(s));
                    for (int elt: range{0, split}) {
                        c->emplace(*c->begin());
                    }}});

// unordered_set<vector<string>>
// allocator: std::allocator, bound: compile-time
    reference = measure((SA|HASHVEC|STR|CT), csv, 0.0,
            [runs,split]() {
                typedef std::vector<std::string> T1;
                typedef std::unordered_set<T1,my_hash<T1>,my_equal<T1>> T;
                for (int run: range{0, runs}) {
                    T c(split);
                    T1 s; s.reserve(128);
                    for (int i: range{0, 128})
                        s.emplace_back(sptr(), slen());
                    c.emplace(std::move(s));
                    for (int elt: range{0, split}) {
                        c.emplace(*c.begin());
                    }}});

// allocator: monotonic, bound: compile-time
    measure((MT|HASHVEC|STR|CT), csv, reference,
            [runs,split]() {
                typedef monotonic::vector<monotonic::string> T1;
                typedef monotonic::unordered_set<T1,my_hash<T1>,my_equal<T1>> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialPool bsp(pool, sizeof(pool));
                    T c(split, &bsp);
                    T1 s(&bsp); s.reserve(128);
                    for (int i : range{0, 128})
                        s.emplace_back(sptr(), slen());
                    c.emplace(std::move(s));
                    for (int elt: range{0, split}) {
                        c.emplace(*c.begin());
                    }}});

// allocator: monotonic, bound: compile-time, drop
    measure((MTD|HASHVEC|STR|CT), csv, reference,
            [runs,split]() {
                typedef monotonic::vector<monotonic::string> T1;
                typedef monotonic::unordered_set<T1,my_hash<T1>,my_equal<T1>> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialPool bsp(pool, sizeof(pool));
                    T* c = new(bsp) T(split, &bsp);
                    T1 s(&bsp); s.reserve(128);
                    for (int i : range{0, 128})
                        s.emplace_back(sptr(), slen());
                    c->emplace(std::move(s));
                    for (int elt: range{0, split}) {
                        c->emplace(*c->begin());
                    }}});

// allocator: multipool, bound: compile-time
    measure((PL|HASHVEC|STR|CT), csv, reference,
            [runs,split]() {
                typedef multipool::vector<multipool::string> T1;
                typedef multipool::unordered_set<T1,my_hash<T1>,my_equal<T1>> T;
                for (int run: range{0, runs}) {
                    bdlma::Multipool mp;
                    T c(split, &mp);
                    T1 s(&mp); s.reserve(128);
                    for (int i : range{0, 128})
                        s.emplace_back(sptr(), slen());
                    c.emplace(std::move(s));
                    for (int elt: range{0, split}) {
                        c.emplace(*c.begin());
                    }}});

// allocator: multipool, bound: compile-time, drop
    measure((PLD|HASHVEC|STR|CT), csv, reference,
            [runs,split]() {
                typedef multipool::vector<multipool::string> T1;
                typedef multipool::unordered_set<T1,my_hash<T1>,my_equal<T1>> T;
                for (int run: range{0, runs}) {
                    bdlma::Multipool mp;
                    T* c = new(mp) T(split, &mp);
                    T1 s(&mp); s.reserve(128);
                    for (int i : range{0, 128})
                        s.emplace_back(sptr(), slen());
                    c->emplace(std::move(s));
                    for (int elt: range{0, split}) {
                        c->emplace(*c->begin());
                    }}});

// allocator: multipool/monotonic, bound: compile-time
    measure((PM|HASHVEC|STR|CT), csv, reference,
            [runs,split]() {
                typedef multipool::vector<multipool::string> T1;
                typedef multipool::unordered_set<T1,my_hash<T1>,my_equal<T1>> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    bdlma::Multipool mp(&bsa);
                    T c(split, &mp);
                    T1 s(&mp); s.reserve(128);
                    for (int i : range{0, 128})
                        s.emplace_back(sptr(), slen());
                    c.emplace(std::move(s));
                    for (int elt: range{0, split}) {
                        c.emplace(*c.begin());
                    }}});

// allocator: multipool/monotonic, bound: compile-time, drop monotonic
    measure((PMD|HASHVEC|STR|CT), csv, reference,
            [runs,split]() {
                typedef multipool::vector<multipool::string> T1;
                typedef multipool::unordered_set<T1,my_hash<T1>,my_equal<T1>> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    bdlma::Multipool* mp = new(bsa) bdlma::Multipool(&bsa);
                    T* c = new(*mp) T(split, mp);
                    T1 s(mp); s.reserve(128);
                    for (int i : range{0, 128})
                        s.emplace_back(sptr(), slen());
                    c->emplace(std::move(s));
                    for (int elt: range{0, split}) {
                        c->emplace(*c->begin());
                    }}});

// allocator: newdelete, bound: run-time
    measure((SA|HASHVEC|STR|RT), csv, reference,
            [runs,split]() {
                typedef poly::vector<poly::string> T1;
                typedef poly::unordered_set<T1,my_hash<T1>,my_equal<T1>> T;
                bslma::NewDeleteAllocator mfa;
                for (int run: range{0, runs}) {
                    T c(split, &mfa);
                    T1 s(&mfa); s.reserve(128);
                    for (int i : range{0, 128})
                        s.emplace_back(sptr(), slen());
                    c.emplace(std::move(s));
                    for (int elt: range{0, split}) {
                        c.emplace(*c.begin());
                    }}});

// allocator: monotonic, bound: run-time
    measure((MT|HASHVEC|STR|RT), csv, reference,
            [runs,split]() {
                typedef poly::vector<poly::string> T1;
                typedef poly::unordered_set<T1,my_hash<T1>,my_equal<T1>> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    T c(split, &bsa);
                    T1 s(&bsa); s.reserve(128);
                    for (int i : range{0, 128})
                        s.emplace_back(sptr(), slen());
                    c.emplace(std::move(s));
                    for (int elt: range{0, split}) {
                        c.emplace(*c.begin());
                    }}});

// allocator: monotonic, bound: run-time, drop
    measure((MTD|HASHVEC|STR|RT), csv, reference,
            [runs,split]() {
                typedef poly::vector<poly::string> T1;
                typedef poly::unordered_set<T1,my_hash<T1>,my_equal<T1>> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    T* c = new(bsa) T(split, &bsa);
                    T1 s(&bsa); s.reserve(128);
                    for (int i : range{0, 128})
                        s.emplace_back(sptr(), slen());
                    c->emplace(std::move(s));
                    for (int elt: range{0, split}) {
                        c->emplace(*c->begin());
                    }}});

// allocator: multipool, bound: run-time
    measure((PL|HASHVEC|STR|RT), csv, reference,
            [runs,split]() {
                typedef poly::vector<poly::string> T1;
                typedef poly::unordered_set<T1,my_hash<T1>,my_equal<T1>> T;
                for (int run: range{0, runs}) {
                    bdlma::MultipoolAllocator mpa;
                    T c(split, &mpa);
                    T1 s(&mpa); s.reserve(128);
                    for (int i : range{0, 128})
                        s.emplace_back(sptr(), slen());
                    c.emplace(std::move(s));
                    for (int elt: range{0, split}) {
                        c.emplace(*c.begin());
                    }}});

// allocator: multipool, bound: run-time, drop
    measure((PLD|HASHVEC|STR|RT), csv, reference,
            [runs,split]() {
                typedef poly::vector<poly::string> T1;
                typedef poly::unordered_set<T1,my_hash<T1>,my_equal<T1>> T;
                for (int run: range{0, runs}) {
                    bdlma::MultipoolAllocator mpa;
                    T* c = new(mpa) T(split, &mpa);
                    T1 s(&mpa); s.reserve(128);
                    for (int i : range{0, 128})
                        s.emplace_back(sptr(), slen());
                    c->emplace(std::move(s));
                    for (int elt: range{0, split}) {
                        c->emplace(*c->begin());
                    }}});

// allocator: multipool/monotonic, bound: run-time
    measure((PM|HASHVEC|STR|RT), csv, reference,
            [runs,split]() {
                typedef poly::vector<poly::string> T1;
                typedef poly::unordered_set<T1,my_hash<T1>,my_equal<T1>> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    bdlma::MultipoolAllocator mpa(&bsa);
                    T c(split, &mpa);
                    T1 s(&mpa); s.reserve(128);
                    for (int i : range{0, 128})
                        s.emplace_back(sptr(), slen());
                    c.emplace(std::move(s));
                    for (int elt: range{0, split}) {
                        c.emplace(*c.begin());
                    }}});

// allocator: multipool/monotonic, bound: run-time, drop monotonic
    measure((PMD|HASHVEC|STR|RT), csv, reference,
            [runs,split]() {
                typedef poly::vector<poly::string> T1;
                typedef poly::unordered_set<T1,my_hash<T1>,my_equal<T1>> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    bdlma::MultipoolAllocator* mp =
                            new(bsa) bdlma::MultipoolAllocator(&bsa);
                    T* c = new(*mp) T(split, mp);
                    T1 s(mp); s.reserve(128);
                    for (int i : range{0, 128})
                        s.emplace_back(sptr(), slen());
                    c->emplace(std::move(s));
                    for (int elt: range{0, split}) {
                        c->emplace(*c->begin());
                    }}});

// ================================================================

// unordered_set<unordered_set<<int>>
// allocator: std::allocator, bound: compile-time
    reference = measure((SA|HASHHASH|INT|CT), csv, 0.0,
            [runs,split]() {
                typedef std::unordered_set<int> T1;
                typedef std::unordered_set<T1,my_hash<T1>,my_equal<T1>> T;
                for (int run: range{0, runs}) {
                    T c(split);
                    T1 s(128);
                    for (int i: range{0, 128})
                        s.emplace(random_engine());
                    c.emplace(std::move(s));
                    for (int elt: range{0, split}) {
                        c.emplace(*c.begin());
                    }}});

// allocator: monotonic, bound: compile-time
    measure((MT|HASHHASH|INT|CT), csv, reference,
            [runs,split]() {
                typedef monotonic::unordered_set<int> T1;
                typedef monotonic::unordered_set<T1,my_hash<T1>,my_equal<T1>> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialPool bsp(pool, sizeof(pool));
                    T c(split, &bsp);
                    T1 s(128, &bsp);
                    for (int i: range{0, 128})
                        s.emplace(random_engine());
                    c.emplace(std::move(s));
                    for (int elt: range{0, split}) {
                        c.emplace(*c.begin());
                    }}});

// allocator: monotonic, bound: compile-time, drop
    measure((MTD|HASHHASH|INT|CT), csv, reference,
            [runs,split]() {
                typedef monotonic::unordered_set<int> T1;
                typedef monotonic::unordered_set<T1,my_hash<T1>,my_equal<T1>> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialPool bsp(pool, sizeof(pool));
                    T* c = new(bsp) T(split, &bsp);
                    T1 s(128, &bsp);
                    for (int i: range{0, 128})
                        s.emplace(random_engine());
                    c->emplace(std::move(s));
                    for (int elt: range{0, split}) {
                        c->emplace(*c->begin());
                    }}});

// allocator: multipool, bound: compile-time
    measure((PL|HASHHASH|INT|CT), csv, reference,
            [runs,split]() {
                typedef multipool::unordered_set<int> T1;
                typedef multipool::unordered_set<T1,my_hash<T1>,my_equal<T1>> T;
                for (int run: range{0, runs}) {
                    bdlma::Multipool mp;
                    T c(split, &mp);
                    T1 s(128, &mp);
                    for (int i: range{0, 128})
                        s.emplace(random_engine());
                    c.emplace(std::move(s));
                    for (int elt: range{0, split}) {
                        c.emplace(*c.begin());
                    }}});

// allocator: multipool, bound: compile-time, drop
    measure((PLD|HASHHASH|INT|CT), csv, reference,
            [runs,split]() {
                typedef multipool::unordered_set<int> T1;
                typedef multipool::unordered_set<T1,my_hash<T1>,my_equal<T1>> T;
                for (int run: range{0, runs}) {
                    bdlma::Multipool mp; 
                    T* c = new(mp) T(split, &mp);
                    T1 s(128, &mp);
                    for (int i: range{0, 128})
                        s.emplace(random_engine());
                    c->emplace(std::move(s));
                    for (int elt: range{0, split}) {
                        c->emplace(*c->begin());
                    }}});

// allocator: multipool/monotonic, bound: compile-time
    measure((PM|HASHHASH|INT|CT), csv, reference,
            [runs,split]() {
                typedef multipool::unordered_set<int> T1;
                typedef multipool::unordered_set<T1,my_hash<T1>,my_equal<T1>> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    bdlma::Multipool mp(&bsa);
                    T c(split, &mp);
                    T1 s(128, &mp);
                    for (int i: range{0, 128})
                        s.emplace(random_engine());
                    c.emplace(std::move(s));
                    for (int elt: range{0, split}) {
                        c.emplace(*c.begin());
                    }}});

// allocator: multipool/monotonic, bound: compile-time, drop monotonic
    measure((PMD|HASHHASH|INT|CT), csv, reference,
            [runs,split]() {
                typedef multipool::unordered_set<int> T1;
                typedef multipool::unordered_set<T1,my_hash<T1>,my_equal<T1>> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    bdlma::Multipool* mp = new(bsa) bdlma::Multipool(&bsa);
                    T* c = new(*mp) T(split, mp);
                    T1 s(128, mp);
                    for (int i: range{0, 128})
                        s.emplace(random_engine());
                    c->emplace(std::move(s));
                    for (int elt: range{0, split}) {
                        c->emplace(*c->begin());
                    }}});

// allocator: newdelete, bound: run-time
    measure((SA|HASHHASH|INT|RT), csv, reference,
            [runs,split]() {
                typedef poly::unordered_set<int> T1;
                typedef poly::unordered_set<T1,my_hash<T1>,my_equal<T1>> T;
                bslma::NewDeleteAllocator mfa;
                for (int run: range{0, runs}) {
                    T c(split, &mfa);
                    T1 s(128, &mfa);
                    for (int i: range{0, 128})
                        s.emplace(random_engine());
                    c.emplace(std::move(s));
                    for (int elt: range{0, split}) {
                        c.emplace(*c.begin());
                    }}});

// allocator: monotonic, bound: run-time
    measure((MT|HASHHASH|INT|RT), csv, reference,
            [runs,split]() {
                typedef poly::unordered_set<int> T1;
                typedef poly::unordered_set<T1,my_hash<T1>,my_equal<T1>> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    T c(split, &bsa);
                    T1 s(128, &bsa);
                    for (int i: range{0, 128})
                        s.emplace(random_engine());
                    c.emplace(std::move(s));
                    for (int elt: range{0, split}) {
                        c.emplace(*c.begin());
                    }}});

// allocator: monotonic, bound: run-time, drop
    measure((MTD|HASHHASH|INT|RT), csv, reference,
            [runs,split]() {
                typedef poly::unordered_set<int> T1;
                typedef poly::unordered_set<T1,my_hash<T1>,my_equal<T1>> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    T* c = new(bsa) T(split, &bsa);
                    T1 s(128, &bsa);
                    for (int i: range{0, 128})
                        s.emplace(random_engine());
                    c->emplace(std::move(s));
                    for (int elt: range{0, split}) {
                        c->emplace(*c->begin());
                    }}});

// allocator: multipool, bound: run-time
    measure((PL|HASHHASH|INT|RT), csv, reference,
            [runs,split]() {
                typedef poly::unordered_set<int> T1;
                typedef poly::unordered_set<T1,my_hash<T1>,my_equal<T1>> T;
                for (int run: range{0, runs}) {
                    bdlma::MultipoolAllocator mpa;
                    T c(split, &mpa);
                    T1 s(128, &mpa);
                    for (int i: range{0, 128})
                        s.emplace(random_engine());
                    c.emplace(std::move(s));
                    for (int elt: range{0, split}) {
                        c.emplace(*c.begin());
                    }}});

// allocator: multipool, bound: run-time, drop
    measure((PLD|HASHHASH|INT|RT), csv, reference,
            [runs,split]() {
                typedef poly::unordered_set<int> T1;
                typedef poly::unordered_set<T1,my_hash<T1>,my_equal<T1>> T;
                for (int run: range{0, runs}) {
                    bdlma::MultipoolAllocator mpa;
                    T* c = new(mpa) T(split, &mpa);
                    T1 s(128, &mpa);
                    for (int i: range{0, 128})
                        s.emplace(random_engine());
                    c->emplace(std::move(s));
                    for (int elt: range{0, split}) {
                        c->emplace(*c->begin());
                    }}});

// allocator: multipool/monotonic, bound: run-time
    measure((PM|HASHHASH|INT|RT), csv, reference,
            [runs,split]() {
                typedef poly::unordered_set<int> T1;
                typedef poly::unordered_set<T1,my_hash<T1>,my_equal<T1>> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    bdlma::MultipoolAllocator mpa(&bsa);
                    T c(split, &mpa);
                    T1 s(128, &mpa);
                    for (int i: range{0, 128})
                        s.emplace(random_engine());
                    c.emplace(std::move(s));
                    for (int elt: range{0, split}) {
                        c.emplace(*c.begin());
                    }}});

// allocator: multipool/monotonic, bound: run-time, drop monotonic
    measure((PMD|HASHHASH|INT|RT), csv, reference,
            [runs,split]() {
                typedef poly::unordered_set<int> T1;
                typedef poly::unordered_set<T1,my_hash<T1>,my_equal<T1>> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    bdlma::MultipoolAllocator* mp =
                            new(bsa) bdlma::MultipoolAllocator(&bsa);
                    T* c = new(*mp) T(split, mp);
                    T1 s(128, mp);
                    for (int i: range{0, 128})
                        s.emplace(random_engine());
                    c->emplace(std::move(s));
                    for (int elt: range{0, split}) {
                        c->emplace(*c->begin());
                    }}});

// unordered_set<unordered_set<string>>
// allocator: std::allocator, bound: compile-time
    reference = measure((SA|HASHHASH|STR|CT), csv, 0.0,
            [runs,split]() {
                typedef std::unordered_set<std::string> T1;
                typedef std::unordered_set<T1,my_hash<T1>,my_equal<T1>> T;
                for (int run: range{0, runs}) {
                    T c(split);
                    T1 s(128);
                    for (int i: range{0, 128})
                        s.emplace(sptr(), slen());
                    c.emplace(std::move(s));
                    for (int elt: range{0, split}) {
                        c.emplace(*c.begin());
                    }}});

// allocator: monotonic, bound: compile-time
    measure((MT|HASHHASH|STR|CT), csv, reference,
            [runs,split]() {
                typedef monotonic::unordered_set<monotonic::string> T1;
                typedef monotonic::unordered_set<T1,my_hash<T1>,my_equal<T1>> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialPool bsp(pool, sizeof(pool));
                    T c(split, &bsp);
                    T1 s(128, &bsp);
                    for (int i : range{0, 128})
                        s.emplace(sptr(), slen());
                    c.emplace(std::move(s));
                    for (int elt: range{0, split}) {
                        c.emplace(*c.begin());
                    }}});

// allocator: monotonic, bound: compile-time, drop
    measure((MTD|HASHHASH|STR|CT), csv, reference,
            [runs,split]() {
                typedef monotonic::unordered_set<monotonic::string> T1;
                typedef monotonic::unordered_set<T1,my_hash<T1>,my_equal<T1>> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialPool bsp(pool, sizeof(pool));
                    T* c = new(bsp) T(split, &bsp);
                    T1 s(128, &bsp);
                    for (int i : range{0, 128})
                        s.emplace(sptr(), slen());
                    c->emplace(std::move(s));
                    for (int elt: range{0, split}) {
                        c->emplace(*c->begin());
                    }}});


// allocator: multipool, bound: compile-time
    measure((PL|HASHHASH|STR|CT), csv, reference,
            [runs,split]() {
                typedef multipool::unordered_set<multipool::string> T1;
                typedef multipool::unordered_set<T1,my_hash<T1>,my_equal<T1>> T;
                for (int run: range{0, runs}) {
                    bdlma::Multipool mp;
                    T c(split, &mp);
                    T1 s(128, &mp);
                    for (int i : range{0, 128})
                        s.emplace(sptr(), slen());
                    c.emplace(std::move(s));
                    for (int elt: range{0, split}) {
                        c.emplace(*c.begin());
                    }}});

// allocator: multipool, bound: compile-time, drop
    measure((PLD|HASHHASH|STR|CT), csv, reference,
            [runs,split]() {
                typedef multipool::unordered_set<multipool::string> T1;
                typedef multipool::unordered_set<T1,my_hash<T1>,my_equal<T1>> T;
                for (int run: range{0, runs}) {
                    bdlma::Multipool mp;
                    T* c = new(mp) T(split, &mp);
                    T1 s(128, &mp);
                    for (int i : range{0, 128})
                        s.emplace(sptr(), slen());
                    c->emplace(std::move(s));
                    for (int elt: range{0, split}) {
                        c->emplace(*c->begin());
                    }}});

// allocator: multipool/monotonic, bound: compile-time
    measure((PM|HASHHASH|STR|CT), csv, reference,
            [runs,split]() {
                typedef multipool::unordered_set<multipool::string> T1;
                typedef multipool::unordered_set<T1,my_hash<T1>,my_equal<T1>> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    bdlma::Multipool mp(&bsa);
                    T c(split, &mp);
                    T1 s(128, &mp);
                    for (int i : range{0, 128})
                        s.emplace(sptr(), slen());
                    c.emplace(std::move(s));
                    for (int elt: range{0, split}) {
                        c.emplace(*c.begin());
                    }}});

// allocator: multipool/monotonic, bound: compile-time, drop monotonic
    measure((PMD|HASHHASH|STR|CT), csv, reference,
            [runs,split]() {
                typedef multipool::unordered_set<multipool::string> T1;
                typedef multipool::unordered_set<T1,my_hash<T1>,my_equal<T1>> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    bdlma::Multipool* mp = new(bsa) bdlma::Multipool(&bsa);
                    T* c = new(*mp) T(split, mp);
                    T1 s(128, mp);
                    for (int i : range{0, 128})
                        s.emplace(sptr(), slen());
                    c->emplace(std::move(s));
                    for (int elt: range{0, split}) {
                        c->emplace(*c->begin());
                    }}});


// allocator: newdelete, bound: run-time
    measure((SA|HASHHASH|STR|RT), csv, reference,
            [runs,split]() {
                typedef poly::unordered_set<poly::string> T1;
                typedef poly::unordered_set<T1,my_hash<T1>,my_equal<T1>> T;
                bslma::NewDeleteAllocator mfa;
                for (int run: range{0, runs}) {
                    T c(split, &mfa);
                    T1 s(128, &mfa);
                    for (int i : range{0, 128})
                        s.emplace(sptr(), slen());
                    c.emplace(std::move(s));
                    for (int elt: range{0, split}) {
                        c.emplace(*c.begin());
                    }}});


// allocator: monotonic, bound: run-time
    measure((MT|HASHHASH|STR|RT), csv, reference,
            [runs,split]() {
                typedef poly::unordered_set<poly::string> T1;
                typedef poly::unordered_set<T1,my_hash<T1>,my_equal<T1>> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    T c(split, &bsa);
                    T1 s(128, &bsa);
                    for (int i : range{0, 128})
                        s.emplace(sptr(), slen());
                    c.emplace(std::move(s));
                    for (int elt: range{0, split}) {
                        c.emplace(*c.begin());
                    }}});

// allocator: monotonic, bound: run-time, drop
    measure((MTD|HASHHASH|STR|RT), csv, reference,
            [runs,split]() {
                typedef poly::unordered_set<poly::string> T1;
                typedef poly::unordered_set<T1,my_hash<T1>,my_equal<T1>> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    T* c = new(bsa) T(split, &bsa);
                    T1 s(128, &bsa);
                    for (int i : range{0, 128})
                        s.emplace(sptr(), slen());
                    c->emplace(std::move(s));
                    for (int elt: range{0, split}) {
                        c->emplace(*c->begin());
                    }}});

// allocator: multipool, bound: run-time
    measure((PL|HASHHASH|STR|RT), csv, reference,
            [runs,split]() {
                typedef poly::unordered_set<poly::string> T1;
                typedef poly::unordered_set<T1,my_hash<T1>,my_equal<T1>> T;
                for (int run: range{0, runs}) {
                    bdlma::MultipoolAllocator mpa;
                    T c(split, &mpa);
                    T1 s(128, &mpa);
                    for (int i : range{0, 128})
                        s.emplace(sptr(), slen());
                    c.emplace(std::move(s));
                    for (int elt: range{0, split}) {
                        c.emplace(*c.begin());
                    }}});

// allocator: multipool, bound: run-time, drop
    measure((PLD|HASHHASH|STR|RT), csv, reference,
            [runs,split]() {
                typedef poly::unordered_set<poly::string> T1;
                typedef poly::unordered_set<T1,my_hash<T1>,my_equal<T1>> T;
                for (int run: range{0, runs}) {
                    bdlma::MultipoolAllocator mpa;
                    T* c = new(mpa) T(split, &mpa);
                    T1 s(128, &mpa);
                    for (int i : range{0, 128})
                        s.emplace(sptr(), slen());
                    c->emplace(std::move(s));
                    for (int elt: range{0, split}) {
                        c->emplace(*c->begin());
                    }}});

// allocator: multipool/monotonic, bound: run-time
    measure((PM|HASHHASH|STR|RT), csv, reference,
            [runs,split]() {
                typedef poly::unordered_set<poly::string> T1;
                typedef poly::unordered_set<T1,my_hash<T1>,my_equal<T1>> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    bdlma::MultipoolAllocator mpa(&bsa);
                    T c(split, &mpa);
                    T1 s(128, &mpa);
                    for (int i : range{0, 128})
                        s.emplace(sptr(), slen());
                    c.emplace(std::move(s));
                    for (int elt: range{0, split}) {
                        c.emplace(*c.begin());
                    }}});

// allocator: multipool/monotonic, bound: run-time, drop monotonic
    measure((PMD|HASHHASH|STR|RT), csv, reference,
            [runs,split]() {
                typedef poly::unordered_set<poly::string> T1;
                typedef poly::unordered_set<T1,my_hash<T1>,my_equal<T1>> T;
                for (int run: range{0, runs}) {
                    bdlma::BufferedSequentialAllocator bsa(pool, sizeof(pool));
                    bdlma::MultipoolAllocator* mp =
                            new(bsa) bdlma::MultipoolAllocator(&bsa);
                    T* c = new(*mp) T(split, mp);
                    T1 s(128, mp);
                    for (int i : range{0, 128})
                        s.emplace(sptr(), slen());
                    c->emplace(std::move(s));
                    for (int elt: range{0, split}) {
                        c->emplace(*c->begin());
                    }}});

    return 0;
}
