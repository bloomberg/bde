#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>

#include <algorithm>
#include <iostream>
#include <string>
#include <bdlma_sequentialallocator.h>
#include <bdlma_multipoolallocator.h>
#include <bslma_newdeleteallocator.h>
#include <bsl_vector.h>

using namespace BloombergLP;

#define ITER 7

int W;
int N;
int S;

void *f1(void *arg) {
    for (int i = 0; i < N; ++i) {
        char *p = static_cast<char *>(::operator new(S));
        ++(*p);
        ::operator delete(p);
    }
    return 0;
}

void *f2(void *arg) {
    bslma::NewDeleteAllocator allocator;

    for (int i = 0; i < N; ++i) {
        char *p = static_cast<char *>(allocator.allocate(S));
        ++(*p);
        allocator.deallocate(p);
    }
    return 0;
}

void *f3(void *arg) {
    bdlma::SequentialPool allocator;

    for (int i = 0; i < N; ++i) {
        char *p = static_cast<char *>(allocator.allocate(S));
        ++(*p);
        //allocator.deallocate(p);
    }
    return 0;
}

void *f5(void *arg) {
    bdlma::SequentialAllocator allocator;

    for (int i = 0; i < N; ++i) {
        char *p = static_cast<char *>(allocator.allocate(S));
        ++(*p);
        allocator.deallocate(p);
    }
    return 0;
}

void *f7(void *arg) {
    bdlma::Multipool allocator;

    for (int i = 0; i < N; ++i) {
        char *p = static_cast<char *>(allocator.allocate(S));
        ++(*p);
        allocator.deallocate(p);
    }
    return 0;
}

void *f9(void *arg) {
    bdlma::MultipoolAllocator allocator;

    for (int i = 0; i < N; ++i) {
        char *p = static_cast<char *>(allocator.allocate(S));
        ++(*p);
        allocator.deallocate(p);
    }
    return 0;
}

void *f11(void *arg) {
    bdlma::SequentialAllocator alloc;
    bdlma::Multipool allocator(&alloc);

    for (int i = 0; i < N; ++i) {
        char *p = static_cast<char *>(allocator.allocate(S));
        ++(*p);
        allocator.deallocate(p);
    }
    return 0;
}

void *f13(void *arg) {
    bdlma::SequentialAllocator alloc;
    bdlma::MultipoolAllocator allocator(&alloc);

    for (int i = 0; i < N; ++i) {
        char *p = static_cast<char *>(allocator.allocate(S));
        ++(*p);
        allocator.deallocate(p);
    }
    return 0;
}

double testOnce(void *(*f)(void *)) {
    timespec start;
    timespec stop;

    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int iter = 0; iter < 10; ++iter) {
        bsl::vector<pthread_t> id;
        id.resize(W);
    
        for (int i = 0; i < W; ++i) {
            pthread_create(&id[i], 0, f, 0);
        }
        for (int i = 0; i < W; ++i) {
            pthread_join(id[i], 0);
        }
    }

    
    clock_gettime(CLOCK_MONOTONIC, &stop);
    int64_t rv = static_cast<int64_t>(stop.tv_sec - start.tv_sec) * 1000000000LL
                    + (stop.tv_nsec - start.tv_nsec);
    return (static_cast<double>(rv) / 1000000000.0L);
}

double test(void *(*f)(void *)) {
    if (ITER == 1) {
        return testOnce(f);
    }

    std::vector<double> v;
    for (int i = 0; i < ITER; ++i) {
        v.push_back(testOnce(f));
    }

    if (ITER < 7) {
        double t = 0;
        for (unsigned i = 0; i < v.size(); ++i) {
            t += v[i];
        }
        return t / v.size();
    }


    std::sort(v.begin(), v.end());

    if (ITER < 14) {
        double t = 0;
        v.pop_back();
        for (unsigned i = 1; i < v.size(); ++i) {
            t += v[i];
        }
    }

    double t = 0;
    v.pop_back();
    v.pop_back();
    for (unsigned i = 2; i < v.size(); ++i) {
        t += v[i];
    }
    return t / (v.size() - 2);
}

int main(int argc, char *argv[]) {
    N = argc > 1 ? atoi(argv[1]) : 1;
    S = argc > 2 ? atoi(argv[2]) : 1;
    W = argc > 3 ? atoi(argv[3]) : 1;

    // allocator index: 1, 2, 3, 5, 7, 9, 11, 13

    printf("%i,%i,%i", N, S, W);

    N = 1 << N;
    S = 1 << S;

    double rv = -1.0;
    double firstRV = -1.0;

    rv = test(f1);
    firstRV = rv;
    printf(",%0.3lfs", rv);

    rv = test(f2);
    printf(",%0.0lf", 100.0 * rv / firstRV);

    rv = test(f3);
    printf(",%0.0lf", 100.0 * rv / firstRV);

    rv = test(f5);
    printf(",%0.0lf", 100.0 * rv / firstRV);

    rv = test(f7);
    printf(",%0.0lf", 100.0 * rv / firstRV);

    rv = test(f9);
    printf(",%0.0lf", 100.0 * rv / firstRV);

    rv = test(f11);
    printf(",%0.0lf", 100.0 * rv / firstRV);

    rv = test(f13);
    printf(",%0.0lf", 100.0 * rv / firstRV);

    printf("\n");
    return 0;
}
