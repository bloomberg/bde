#!/usr/bin/env python
# encoding: utf-8

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
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ----------------------------- END-OF-FILE ----------------------------------
