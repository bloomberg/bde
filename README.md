BDE Libraries
=============

This repository contains the BDE libraries, currently BSL (Basic Standard
Library), BDL (Basic Development Library), BAL (Basic Application Library), and
BBL (Basic Business Library).  For more information about BDE libraries
follow the links below:

* [Online Library Documentation](http://bde.bloomberg.com/bde-docs)
* [BDE build tools](https://bde.bloomberg.com/bde-tools/)

Basic Build Instructions
========================

BDE uses a build system based on [cmake](https://cmake.org), which is located
in the [BDE Tools](https://bbgithub.dev.bloomberg.com/bde/bde-tools/)
repository. 

The following commands can be used to configure and build the BDE repository:

1. From the root of this source repository, run:

   ```shell
   $ eval `bde_build_env.py -t dbg_exc_mt_64_cpp11`
   $ cmake_build.py configure
   ```

2. To build the libraries, but not the test drivers, run:

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

For more details on building the BDE repository, please see the
[Build Examples](https://bde.bloomberg.com/bde-tools/cmake/examples.html).

License
-------
The BDE libraries are distributed under the Apache License (version 2.0); see
the LICENSE file at the top of the source tree for more information.
