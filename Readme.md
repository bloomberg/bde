BDE Libraries
=============

This repository contains the BDE libraries, currently BSL (Basic Standard
Library), BDL (Basic Development Library), BAL (Basic Application Library), and
BBL (Basic Business Library).

For more information about BDE libraries follow the links below:

Open Source
-----------

* [Online Library Documentation](https://bloomberg.github.io/bde)
* [BDE Build Tools](https://bloomberg.github.io/bde-tools/)
* [BDE Build Howto](https://bloomberg.github.io/bde/library_information/build.html)


Bloomberg LP (internal)
-----------------------

* [Online Library Documentation](https://bde.bloomberg.com/bde)
* [BDE Build Tools](https://bde.bloomberg.com/bde-tools/)
* [BDE Build Howto](https://bde.bloomberg.com/bde/library_information/build.html)


Basic Build Instructions (Open Source)
======================================

BDE uses a build system based on [cmake](https://cmake.org), which is located
in the BDE Tools repository.

The following commands can be used to configure and build the BDE repository:
1. Clone the `bde` resporitory and add `bde-tools` to your `PATH`:
   ```shell
   $ git clone https://github.com/bloomberg/bde.git
   $ git clone https://github.com/bloomberg/bde-tools.git
   $ export PATH=$PWD/bde-tools/bin:$PATH      # add bde-tools to the 'PATH'
   $ cd bde
   ```

2. From the root of this source repository, run:

   ```shell
   $ export BDE_CMAKE_BUILD_DIR=$PWD/_build   # configure the build directory
   $ cmake_build.py configure -u dbg_exc_mt_64_cpp14
   ```

3. To build the libraries, but not the test drivers, run:

   ```shell
   $ cmake_build.py build
   ```

   To also build the test drivers, run:

   ```shell
   $ cmake_build.py build --test build
   ```

   To build and run the test drivers, run:

   ```shell
   $ cmake_build.py build --test run
   ```

Basic Build Instructions (internal)
===================================

BDE uses a build system based on [cmake](https://cmake.org), which is located
in the BDE Tools repository.

The following commands can be used to configure and build the BDE repository:
1. Clone the `bde` resporitory and add `bde-tools` to your `PATH`:
   ```shell
   $ git clone bbgithub:bde/bde
   $ git clone bbgithub:bde/bde-tools
   $ export PATH=$PWD/bde-tools/bin:$PATH      # add bde-tools to the 'PATH'
   $ cd bde
   ```

2. From the root of this source repository, run:

   ```shell
   $ export BDE_CMAKE_BUILD_DIR=$PWD/_build   # configure the build directory
   $ cmake_build.py configure -u dbg_exc_mt_64_cpp14
   ```

3. To build the libraries, but not the test drivers, run:

   ```shell
   $ cmake_build.py build
   ```

   To also build the test drivers, run:

   ```shell
   $ cmake_build.py build --test build
   ```

   To build and run the test drivers, run:

   ```shell
   $ cmake_build.py build --test run
   ```

For more details on building the BDE repository, please see the Build Examples
on BDE Build Tools).

License
=======

The BDE libraries are distributed under the Apache License (version 2.0); see
the LICENSE file at the top of the source tree for more information.
