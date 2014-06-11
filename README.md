BDE Libraries
=============

This repository contains the BDE libraries, currently BSL (BDE Standard Library) and BDL (BDE Development Library).  For more information about BSL, BDL, and BDE, follow the links below:

* [BDE wiki](http://github.com/bloomberg/bde/wiki)
* [Quick-Start Guide](http://github.com/bloomberg/bde/wiki/Getting-Started)
* [Online Library Documentation](http://bloomberg.github.com/bde)
* [BDE Mission Statement](http://github.com/bloomberg/bde/wiki/Mission-Statement)

Basic Build Instructions
========================

1. Make sure that python 2.6 - 2.7 are installed on the system.
2. Download [BDE Tools](https://github.com/bloomberg/bde-tools/) and add the
   path `<bde_tools_repo_root>/bin` to the system `PATH` environment
   variable. Make sure the first `waf` command found in `PATH` is a script from that path.
3. From the root of this source repository, run `python waf configure`
4. To build the libraries, but not the test drivers, run:
   `python waf build`
5. To also build the test drivers, run:
   `python waf build --test build`
6. To build and run the test drivers, run:
   `python waf build --test run`

For more details on building the BDE repository, please see the
[Quick Start Guide](http://github.com/bloomberg/bde/wiki/Getting-Started).

For more details on the waf-based build system for building BDE-style
repositories, please see
[its wiki page](https://github.com/bloomberg/bde-tools/wiki/Waf-Build).

License
-------
The BDE libraries are distributed under a simple MIT-style license; see the LICENSE file at the top of the source tree for more information.

Question, Comments and Feedback
===============================
If you have questions, comments, suggestions for improvement or any other inquiries regarding BDE, feel free to open an issue
in the [issue tracker](https://github.com/bloomberg/bde/issues).
