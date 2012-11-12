-
BDE (Beta)
==========
* [Online Library Documentation]()
* [Quick-Start Guide](https://github.com/bloomberg/bsl/wiki/Getting-Started)

What is BDE?
============
BDE (Basic Development Environment) is a set of C++ software libraries as 
well as development tools and methodology.   Originally developed at 
Bloomberg L.P., BDE is intended to form the foundation for large scale C++
software products. 

The BSL (Basic Standard Library), the first BDE library available with an 
open-source license, provides implementations for portions of the C++ 
standard library, particularly the container types, as well as a suite of
system-utilities, meta-functions, and algorithms needed to build such 
containers.

Goals and Benefits of BDE
=========================
BDE aims to provide libraries that meet superior standards in terms of their
design, documentation, and testing
(see [Mission Statement](https://github.com/bloomberg/bsl/wiki/Mission-Statement)). 
Every component (i.e., .h/.cpp file-pair) provides thorough component, class, and
method level documentation 
(see [Online Library Documentation]())
, including a compilable example for using that type.  Every component also
has a thorough test-driver that tests every public method of that component 
through a comprehensive range of input 
(see [bslstl_map.t.cpp](https://github.com/bloomberg/bsl/blob/master/groups/bsl/bslstl/bslstl_map.t.cpp)).

These libraries, along with the tools and methodology that accompany them, 
are meant to enable developers to create their own C++ software that meets 
similar standards of quality.

The BSL Library
---------------
The BSL library (Basic Standard Library) provides a finely factored collection
of tools that form the foundation for cross-platform component based
development.  The library culminates in the implementation of several standard
containers which take advantage of the features of the rest of the BSL library.
Some of the highlights of BSL include:

* Support for runtime polymorphic memory allocators
  * Provides more efficient application-specific memory-allocation behavior.
  * Provides facilities for testing memory allocation.
  * (see blsma)
* Enhanced type-trait support and trait-aware container facilities (see bslmf)
  * Provides for efficient move-semantics (similar to to C++11 move-semantics)
  * (see blsmf)
* Support for defensive programming through a configurable assert facility
  * Support for testing those defensive checks ("negative testing")
  * (see bsls_assert & bsls_asserttest)
* A facility for testing generic container instantiations with a range of
  troublesome types
  * (see bsltf)
* Implementations for basic atomic operations with flexible memory semantics
  * (see bsls_atomic)

Getting Started
===============
The best place to get started is to read our introduction to building BDE and creating a
"Hello World" application using that built library:

* [Getting started](https://github.com/bloomberg/bsl/wiki/Getting-Started)

Some other introductory material includes:
* The list of platforms on which BDE is supported can be found [here](https://github.com/bloomberg/bsl/wiki/Supported-Platforms)
* Introduction to BDE [memory allocators](https://github.com/bloomberg/bsl/wiki/BDE-Allocator-model)
* Introduction to the [physical organization of the libraries](https://github.com/bloomberg/bsl/wiki/physical-code-organization)
* [BSL and STL](https://github.com/bloomberg/bsl/wiki/BSL-and-STL)
* [Introduction to BDE Coding Standards](https://github.com/bloomberg/bsl/wiki/Introduction-to-BDE-Coding-Standards)

One of the key elements of the BDE libraries is the extensive header
(i.e., component-level) documentation. Doxygen for the BSL library can be
found here: 
[Online Library Documentation](), 
although the documentation in the headers themselves is intended for 
reading on its own (without doxygen).

Community Involvement
=====================
We hope to enlist the software developers who share our goals in extending
the BDE libraries.  One of the key goals of these libraries is to
maintain a high level quality and a high degree of *uniformity* in documentation,
testing, and implementation, so contributors should read over the 
[style conventions](https://github.com/bloomberg/bsl/wiki/Introduction-to-BDE-Coding-Standards) 
used in the libraries, as well as the rigorous testing required for any changes.

Finally, note that the BDE libraries are in active use within Bloomberg, and we
have just started to figure out how to cleanly accept contributions without
breaking internal software that depends on these libraries.  We gratefully accept 
contributions, though integration into an official release may take some 
time as contributions move through our processes.

### Contributor License Agreements

BDE requires all contributors of ideas, code, or documentation to sign a Bloomberg Individual Contributor License Agreement (ICLA).  The purpose of this agreement is to clearly define the terms under which intellectual property has been contributed to BDE and thereby allow us to defend the project should there be a legal dispute regarding the software at some future time.  The e-mail address and name on a signed ICLA must match the e-mail address and name used for git submissions.

Corporations that have employees wishing to contribute to BDE who are covered by employment agreements which assign intellectual property rights must sign a Bloomberg Corporate Contributor License Agreement (CCLA).

* [Individual (ICLA) Agreement](https://github.com/bloomberg/bsl/wiki/Individual-Contributor-License-Agreement-Version-1.0)
* [Corporate (CCLA) Agreement](https://github.com/bloomberg/bsl/wiki/Corporate-Contributor-License-Agreement-Version-1.0)

Future Plans
============
BDE was started in 2001, and has been in production use for over a decade.
BSL is the lowest level BDE library, but as we review higher level libraries
(ensuring they are up to date with our coding standards) additional libraries
will be made available with an open-source license.  Some features of these
additional libraries include:

* Additional concrete allocator implementations.
* Date, time, and calendar types.
* Cross-platform multi-threading & synchronization primitives.
* Efficient thread-pools, event schedulers, and thread-safe container types.
* Network channel pools and session pools.
* Logging infrastructure.

Our immediate focus is an extended a date (and time) facility along with
additional memory-allocators, which we aim to provide in 2013.
