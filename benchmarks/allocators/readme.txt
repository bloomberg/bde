These are the programs used to produce the results found in N4468,
"On Quantifying Allocation Strategies".  

These programs depend on clang++ and libc++ version 3.6 or later.
They use a patched BDE library built with waf and specific configuration
options, as specified in the Makefile.  The patches needed are on this
branch.  A snapshot of those patches is included here.

To build, 
  $ make

To run benchmarks,
  $ make run

Other targets of interest:
  bde growth locality zation tention
  run-growth run-locality run-zation run-tention

The programs included are:

  growth.cc   - section 7 : Creating/destroying isolated basic data structures
  locality.cc - section 8 : Variation in Locality (long running)
  zation.cc   - section 9 : Variation in Utilization
  tention.cc  - section 10: Variation in Contention

Other files:
  readme-growth.txt - instructions to produce CSV of tables in the paper
  test-growth    - scripts to run the benchmarks as published
  test-locality  - ..
  test-zation
  test-tention
  bde-patches    - snapshot of patches to bde that this depends on

FAQ:

Q1: What about tcmalloc?
A1: In all our tests, tcmalloc was substantially slower than libc malloc.

Q2: What is the effect of building with "-flto"?
A2: The entire source code of the BDE library, ".h" and ".cpp" files alike,
  are visible to the compiler when generating code.  Effectively all code
  used in the program, except libc and parts libc++, are "as if" defined
  inline.

Q3: Why do these depend on recent clang++ and libc++?
A3: The tests, particularly growth.cc and locality.cc, depend on features
  of C++14, including the containers' comprehensive observance of allocator
  traits requirements to direct their memory management.  To our knowledge,
  at the time of this writing libc++ is the only library that meets this
  requirement.  libc++, in turn, is most easily built with clang++.

Q4: "make growth" never finishes, the compiler just sit there.
A4: Link-time optimization on a big program takes a long time.
