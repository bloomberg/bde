#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <stdint.h>

#include <algorithm>
#include <iostream>
#include <string>
#include <bdlma_sequentialallocator.h>
#include <bdlma_multipoolallocator.h>
#include <bslma_newdeleteallocator.h>
#include <bsl_vector.h>

using namespace BloombergLP;

bool assumeFail = false;

#define ITER 7

int64_t totalAllocation;
int64_t activeAllocation;
int64_t allocationSize;


struct directAllocator {
    void *allocate(size_t s) {  return ::operator new(s);  }
    void deallocate(void *p) {  ::operator delete(p);  }
};

template <class T>
double testOnce(T& allocator) {
    timespec start;
    timespec stop;

    bsl::vector<void *> active;
    active.resize(activeAllocation);
    
    clock_gettime(CLOCK_MONOTONIC, &start);

    int64_t i;
    for (i = 0; i < totalAllocation && i < activeAllocation; ++i) {
        active[i] = allocator.allocate(allocationSize);
        ++(*static_cast<char *>(active[i]));
    }
    for (; i < totalAllocation; ++i) {
        int64_t j = i % activeAllocation;
        allocator.deallocate(active[j]);
        active[j] = allocator.allocate(allocationSize);
        ++(*static_cast<char *>(active[j]));
    }
    for (i = 0; i < totalAllocation && i < activeAllocation; ++i) {
        allocator.deallocate(active[i]);
    }

    clock_gettime(CLOCK_MONOTONIC, &stop);
    int64_t rv = static_cast<int64_t>(stop.tv_sec - start.tv_sec) * 1000000000LL
                    + (stop.tv_nsec - start.tv_nsec);
    return (static_cast<double>(rv) / 1000000000.0L);
}

template <>
double testOnce(bdlma::SequentialPool& allocator) {
    timespec start;
    timespec stop;

    bsl::vector<void *> active;
    active.resize(activeAllocation);
    
    clock_gettime(CLOCK_MONOTONIC, &start);

    int64_t i;
    for (i = 0; i < totalAllocation && i < activeAllocation; ++i) {
        active[i] = allocator.allocate(allocationSize);
        ++(*static_cast<char *>(active[i]));
    }
    for (; i < totalAllocation; ++i) {
        int64_t j = i % activeAllocation;
        active[j] = allocator.allocate(allocationSize);
        ++(*static_cast<char *>(active[j]));
    }
    for (i = 0; i < totalAllocation && i < activeAllocation; ++i) {
    }

    clock_gettime(CLOCK_MONOTONIC, &stop);
    int64_t rv = static_cast<int64_t>(stop.tv_sec - start.tv_sec) * 1000000000LL
                    + (stop.tv_nsec - start.tv_nsec);
    return (static_cast<double>(rv) / 1000000000.0L);
}

template <class T1, class T2>
double test() {
    if (ITER == 1) {
        T2 alloc;
        T1 allocator(&alloc);
        return testOnce(allocator);
    }

    std::vector<double> v;
    for (int i = 0; i < ITER; ++i) {
        T2 alloc;
        T1 allocator(&alloc);
        v.push_back(testOnce(allocator));
    }

    if (ITER < 7) {
        double t = 0;
        for (unsigned i = 0; i < v.size(); ++i) {
            t += v[i];
        }
        return t / v.size();
    }


    std::sort(v.begin(), v.end());

    double t = 0;
    v.pop_back();
    for (unsigned i = 1; i < v.size(); ++i) {
        t += v[i];
    }
    return t / (v.size() - 1);
}

template <class T1>
double test() {
    if (ITER == 1) {
        T1 allocator;
        return testOnce(allocator);
    }

    std::vector<double> v;
    for (int i = 0; i < ITER; ++i) {
        T1 allocator;
        v.push_back(testOnce(allocator));
    }

    if (ITER < 7) {
        double t = 0;
        for (unsigned i = 0; i < v.size(); ++i) {
            t += v[i];
        }
        return t / v.size();
    }


    std::sort(v.begin(), v.end());

    double t = 0;
    v.pop_back();
    for (unsigned i = 1; i < v.size(); ++i) {
        t += v[i];
    }
    return t / (v.size() - 1);
}

int main(int argc, char *argv[]) {
    int totalSize = argc > 1 ? atoi(argv[1]) : 10;
    int activeSize = argc > 2 ? atoi(argv[2]) : 6;
    int blockSize = argc > 3 ? atoi(argv[3]) : 4;

    totalAllocation = 1LL << (totalSize - blockSize);
    activeAllocation =  1LL << (activeSize - blockSize);
    allocationSize =  1LL << blockSize;


    printf("%i,%i,%i", totalSize, activeSize, blockSize);

    double firstRV;
    for (int allocatorIndex = 0; allocatorIndex < 8; ++allocatorIndex) {
        double rv = -1.0;
        switch (allocatorIndex) {
          case 0: {
            rv = test<directAllocator>();
            firstRV = rv;
            printf(",%0.3lfs", rv);
          } break;
          case 1: {
            rv = test<bslma::NewDeleteAllocator>();
            printf(",%0.0lf", 100.0 * rv / firstRV);
          } break;
          case 2: {
            if (assumeFail && totalSize >= 32) printf(",fail");
            else {
                rv = test<bdlma::SequentialPool>();
                printf(",%0.0lf", 100.0 * rv / firstRV);
            }
          } break;
          case 3: {
            if (assumeFail && totalSize >= 32) printf(",fail");
            else {
                rv = test<bdlma::SequentialAllocator>();
                printf(",%0.0lf", 100.0 * rv / firstRV);
            }
          } break;
          case 4: {
            rv = test<bdlma::Multipool>();
            printf(",%0.0lf", 100.0 * rv / firstRV);
          } break;
          case 5: {
            rv = test<bdlma::MultipoolAllocator>();
            printf(",%0.0lf", 100.0 * rv / firstRV);
          } break;
          case 6: {
            if (assumeFail && totalSize >= 32 && blockSize >= 13) {
                printf(",fail");
            }
            else {
                rv = test<bdlma::Multipool, bdlma::SequentialAllocator>();
                printf(",%0.0lf", 100.0 * rv / firstRV);
            }
          } break;
          case 7: {
            if (assumeFail && totalSize >= 32 && blockSize >= 13) {
                printf(",fail");
            }
            else {
                rv = test<bdlma::MultipoolAllocator,
                          bdlma::SequentialAllocator>();
                printf(",%0.0lf", 100.0 * rv / firstRV);
            }
          } break;
        }
    }
    printf("\n");
    return 0;
}
