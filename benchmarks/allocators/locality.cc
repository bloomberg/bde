// t3.cpp

// This is a sketch of a system that should exhibit fragmentation over time.

// ----------------------------------------------------------------------------
// Version 2 has been modified so that if the first argument is negative,
// it and the second arguments are interpreted as (non-negative) powers of
// two where the first argument is determines the overall physical size of the
// problem, and the second determines the size of each subsystem.  The number
// of subsystems is determined as the ratio of physical size to subsystem size.
// The third and fifth parameters are assumed to be integral factors of the
// number of subsystems and physical size, respectively.
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// Version 3 has been modified so that if the third argument is unsigned.
// ----------------------------------------------------------------------------

#define ARENA

//#define VERBOSE

#include "allocont.h"
#include <bdlma_multipoolallocator.h>

#include <iostream>
#include <cstdlib>

#include <list>
#include <vector>

#if defined(STDALLOC)
    template <typename T>
        using List =  std::list<T>;
#elif defined(CTMULTI)
    template <typename T>
        using List =  multipool::list<T>;
#elif defined(RTMULTI) || defined(RTMULTIMONO)
    template <typename T>
        using List =  poly::list<T>;
#endif


class Subsystem {
    // This class simulates a subsystem that might do various work using a
    // node-based container.

#if defined(CTMULTI)
    BloombergLP::bdlma::Multipool d_allocator;
#elif defined(RTMULTI)
    BloombergLP::bdlma::MultipoolAllocator d_allocator;
#elif defined(RTMULTIMONO)
    BloombergLP::bdlma::BufferedSequentialAllocator d_backing;
    BloombergLP::bdlma::MultipoolAllocator d_allocator;
#endif
    List<int> d_data;

    friend std::ostream& operator<<(std::ostream&, const Subsystem&);

  public:
    Subsystem(int initialLength)
        // Create a subsystem having the specified 'initialLength'.
#if defined CTMULTI
    : d_allocator()
    , d_data(&d_allocator)
#elif defined(RTMULTI)
    : d_allocator()
    , d_data(&d_allocator)
#elif defined(RTMULTIMONO)
    : d_backing(new char[initialLength * 16], initialLength * 16)
    , d_allocator(&d_backing)
    , d_data(&d_allocator)
#endif
    {
	for (int i = 0; i < initialLength; ++i) {
	    d_data.push_back(i);
        }
    }

    //void access(int accessCount)  Version 3:
    void access(unsigned accessCount)
        // Ping the system, simulating read/write accesses proportional to the
        // specified 'accessCount'.
    {
	List<int>::iterator it = d_data.begin();

	if (d_data.empty()) {
	    return;  // nothing to access
	}

	//for (int i = 0; i < accessCount; ++i) {  Version 3:
	for (unsigned i = 0; i < accessCount; ++i) {
            if (it == d_data.end()) {
	        it = d_data.begin();
	    }
	    ++*it;
	    ++it;
	}
    }

    void push(int value)
	// Add the specified 'value' to this subsystem.
    {
	d_data.push_back(value);
    }

    void pop()
	// Remove the next value to this subsystem.
    {
	d_data.pop_front();
    }

    int length() const
    {
	return d_data.size();
    }

    bool isEmpty() const
    {
	return d_data.empty();
    }
};

std::ostream& operator<<(std::ostream& stream, const Subsystem& object)
    // Write the contents of the specified 'object' to the specified
    // 'stream' in a single-line, human readable format.
{
    stream << '{';
    for (List<int>::const_iterator it  = object.d_data.begin();
                                        it != object.d_data.end();
				      ++it) {
        stream << ' ' << *it;
    }

    return stream << " }";
}

void print(const char *label, const std::vector<Subsystem *>& array)
{
    std::cout << std::endl << label << ':' << std::endl;

    for (std::size_t i = 0; i < array.size(); ++i) {
	std::cout << *array[i] << std::endl;
    }
}

void churn(std::vector<Subsystem *> *system, int churnCount)
{
    for (int c = 0; c < churnCount; ++c) {
        size_t k;
        do {
	    k = rand() % system->size();
        }
        while ((*system)[k]->isEmpty());

        // std::cout << "pop k = " << k << std::endl;

        (*system)[k]->pop();

        k = c % system->size();

        // std::cout << "push k = " << k << std::endl;

        (*system)[k]->push(-9);
    }

#ifdef VERBOSE
    print("configuration after churn", *system);
#endif
}

int main(int argc, const char *argv[])
{
    int numSubsystems = argc > 1 ? atoi(argv[1]) : 4;
    int initialLength = argc > 2 ? atoi(argv[2]) : 20;
    // int accessCount   = argc > 3 ? atoi(argv[3]) : 18;  // Version 3:
    unsigned accessCount   = argc > 3 ? atoi(argv[3]) : 18;
    int churnCount    = argc > 4 ? atoi(argv[4]) : 10;
    int iterations    = argc > 5 ? atoi(argv[5]) : 3;


#ifdef VERBOSE
    std::cout << std::endl
              << "numSubsystems = " << numSubsystems << std::endl
              << "initialLength = " << initialLength << std::endl
	      << "accessCount   = " << accessCount << std::endl
              << "churnCount    = " << churnCount << std::endl
              << "iterations    = " << iterations << std::endl;
#else
    std::cout << std::endl
              << "nS = " << numSubsystems << '\t'
              << "iL = " << initialLength << '\t'
	      << "aC = " << accessCount << '\t'
              << "cC = " << churnCount << '\t'
              << "it = " << iterations << std::endl;
#endif

// ----------------------------------------------------------------------------
// Version 2:

    if (numSubsystems < 0) {

        numSubsystems = 1 << (-numSubsystems - initialLength);

        initialLength = 1 << initialLength;

        accessCount *= initialLength;

        churnCount *= numSubsystems * initialLength;

        std::cout << "nS = " << numSubsystems << '\t'
                  << "iL = " << initialLength << '\t'
	          << "aC = " << accessCount << '\t'
                  << "cC = " << churnCount << '\t'
                  << "it = " << iterations << std::endl;
    }

// ----------------------------------------------------------------------------

#ifdef VERBOSE
    std::cout << std::endl
              << "Create an array to hold 'numSubsystems' = "
              << numSubsystems
              << std::endl;
#endif

    std::vector<Subsystem *> array;
    array.reserve(numSubsystems);

#ifdef VERBOSE
    std::cout << std::endl
              << "Populate each subsystem to have 'initialLength' = "
              << initialLength
              << std::endl;
#endif

    for (int i = 0; i < numSubsystems; ++i) {
	array.push_back(new Subsystem(initialLength));
    }

#ifdef VERBOSE
    print("Initial Configuration", array);
#endif


    if (churnCount >= 0) {
        churn(&array, churnCount);
    }


    for (int n = 0; n < iterations; ++n) {

#ifdef VERBOSE
        std::cout << std::endl << "***** iteration " << n << std::endl;
#endif

        for (int s = 0; s < numSubsystems; ++s) {

	    array[s]->access(accessCount);

        }

#ifdef VERBOSE
	print("configuration after access", array);
#endif

    }

    if (churnCount < 0) {
        churn(&array, -churnCount);
    }


    int minLen = 0;
    int maxLen = 0;

    for (int i = 0; i < numSubsystems; ++i) {
        int len = array[i]->length();
        if (0 == i) {
            minLen = maxLen = len;
        } else if (len > maxLen) {
            maxLen = len;
        } else if (len < minLen) {
            minLen = len;
        }
    }

    std::cout << std::endl
              <<   "minLen = " << minLen
              << ", maxLen = " << maxLen
              << ", diff = " << maxLen - minLen << std::endl;
}


