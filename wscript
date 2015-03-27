#!/usr/bin/env python
# encoding: utf-8

# Copy this file to the root directory of a BDE-style source repo to enable
# building it using the waf-based build tool.

import os
import sys

from waflib import Logs
from waflib import Configure

top = '.'
out = 'build'


def _get_tools_path(ctx):

    waf_path = sys.argv[0]

    base = os.path.dirname(waf_path)

    if os.path.isdir(os.path.join(base, 'lib', 'bdebld')):
        return [os.path.join(base, 'lib'),
                os.path.join(base, 'lib', 'legacy')]

    ctx.fatal("BDE waf customizations can not be found under tools/lib in the "
              "path to waf.")


def options(ctx):
    ctx.load('bdebld.waf.wscript', tooldir=_get_tools_path(ctx))


class PreConfigure(Configure.ConfigurationContext):
    cmd = 'configure'

    def __init__(self, **kw):
        global out

        build_dir = os.getenv('BDE_WAF_BUILD_DIR')
        if build_dir:
            out = build_dir

        Logs.debug('config: build dir: ' + out)

        super(PreConfigure, self).__init__(**kw)


def configure(ctx):
    ctx.load('bdebld.waf.wscript', tooldir=_get_tools_path(ctx))


def build(ctx):
    # Preserve backwards compatibility by adding the "legacy" lib to the system
    # path.

    sys.path += _get_tools_path(ctx)
    ctx.load('bdebld.waf.wscript', tooldir=_get_tools_path(ctx))
