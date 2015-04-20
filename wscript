#!/usr/bin/env python
# encoding: utf-8

# Copy this file to the root directory of a BDE-style source repo to enable
# building it using the waf-based build tool.

import os
import sys

top = '.'
out = 'build'


def _get_tools_path(ctx):
    waf_path = sys.argv[0]
    base = os.path.dirname(waf_path)
    if os.path.isdir(os.path.join(base, 'lib', 'bdebld')):
        return os.path.join(base, 'lib')
    ctx.fatal("BDE waf customizations can not be found under tools/lib in the "
              "path to waf.")


def options(ctx):
    ctx.load('bdebld.waf.wscript', tooldir=_get_tools_path(ctx))


def configure(ctx):
    ctx.load('bdebld.waf.wscript', tooldir=_get_tools_path(ctx))


def build(ctx):
    ctx.load('bdebld.waf.wscript', tooldir=_get_tools_path(ctx))
