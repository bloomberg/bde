BDE Libraries
=============

This repository contains the BDE libraries, currently BSL (BDE Standard Library) and BDL (BDE Development Library).  For more information about BSL, BDL, and BDE, follow the links below:

* [BDE wiki](http://github.com/bloomberg/bde/wiki)
* [Quick-Start Guide](http://github.com/bloomberg/bde/wiki/Getting-Started)
* [Online Library Documentation](http://bloomberg.github.com/bde)
* [BDE Mission Statement](http://github.com/bloomberg/bde/wiki/Mission-Statement)

What's New
==========

**October 2015** 

BDE 3.0 is now available.  This  major release introduces important subsystems including calendar types, time-zone utilities, logging facilities, event schedulers, thread-pools, and more. 

See: [BDE 3.0 Open-Source Release](https://github.com/bloomberg/bde/wiki/BDE-3.0---Open-Source-Release)

Basic Build Instructions
========================

BDE uses a build system based on [waf](https://github.com/waf-project/waf), which is
located in the [BDE Tools](https://github.com/bloomberg/bde-tools/)
repository. This build system must be set up before being used:

1. Make sure that python 2.6 - 2.7 are installed on the system.
2. Download [BDE Tools](https://github.com/bloomberg/bde-tools/) and add the
   path `<bde_tools_repo_root>/bin` to the system `PATH` environment
   variable. Make sure the first `waf` command found in `PATH` is a script from that path.

Once the waf based build system has been set up. The following commands can be
used to configure and build the BDE repository:

1. From the root of this source repository, run:

   ```shell
   waf configure
   ```

2. To build the libraries, but not the test drivers, run:

   ```shell
   waf build
   ```

   To also build the test drivers, run:

   ```shell
   waf build --test build
   ```

   To build and run the test drivers, run:

   ```shell
   waf build --test run
   ```

For more details on building the BDE repository, please see the
[Quick Start Guide](http://github.com/bloomberg/bde/wiki/Getting-Started).

For more details on the waf-based build system for building BDE-style
repositories, please see
[its wiki page](https://github.com/bloomberg/bde-tools/wiki/Waf-Build).

License
-------
The BDE libraries are distributed under the Apache License (version 2.0); see the LICENSE file at the top of the source tree for more information.

Question, Comments and Feedback
===============================
If you have questions, comments, suggestions for improvement or any other inquiries regarding BDE, feel free to open an issue
in the [issue tracker](https://github.com/bloomberg/bde/issues).
