#!/usr/bin/env python
# encoding: utf-8

# Copy this file to the root directory of a BDE-style source repo to enable building it using the waf-based build tool.

import os
import sys

from waflib import Logs, Utils
from waflib.Configure import ConfigurationContext

top = '.'
out = 'build'

def _get_tools_path(ctx):

    waf_path = sys.argv[0]

    base = os.path.dirname(waf_path)

    if os.path.isdir(os.path.join(base, 'tools', 'waf', 'bde')):
        return os.path.join(base, 'tools', 'waf', 'bde');

    ctx.fatal("BDE waf customizations can not be found under tools/waf/bde in the path of waf.")


def options(ctx):
    import sys

    # check version numbers here because options() is called before any other command-handling function
    if (sys.hexversion < 0x2060000 or 0x3000000 <= sys.hexversion ):
        ctx.fatal('Pyhon 2.6 or Python 2.7 is required to build BDE using waf.')


    ctx.load('bdewscript', tooldir = _get_tools_path(ctx))


class PreConfigure(ConfigurationContext):
    cmd = 'configure'

    def __init__(self, **kw):
        global out

        build_dir = os.getenv('BDE_WAF_BUILD_DIR')
        if build_dir:
            out = build_dir

        Logs.debug('config: build dir: ' + out)

        super(PreConfigure, self).__init__(**kw)


def configure(ctx):
    ctx.load('bdewscript', tooldir = _get_tools_path(ctx))

def build(ctx):
    ctx.load('bdewscript', tooldir = _get_tools_path(ctx))

# ----------------------------------------------------------------------------
# Copyright (C) 2013-2014 Bloomberg Finance L.P.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to
# deal in the Software without restriction, including without limitation the
# rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
# sell copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
# IN THE SOFTWARE.
# ----------------------------- END-OF-FILE ----------------------------------
