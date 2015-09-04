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
    libdir = os.path.join(os.path.dirname(os.path.dirname(os.path.realpath(
        waf_path))), 'lib', 'python')
    if os.path.isdir(os.path.join(libdir, 'bdebuild')):
        return libdir
    ctx.fatal('BDE waf customizations can not be found. '
              'Make sure that you are running the waf executable located in '
              'bde-oss-tools/bin.')


def options(ctx):
    ctx.load('bdebuild.waf.wscript', tooldir=_get_tools_path(ctx))


def configure(ctx):
    ctx.load('bdebuild.waf.wscript', tooldir=_get_tools_path(ctx))


def build(ctx):
    ctx.load('bdebuild.waf.wscript', tooldir=_get_tools_path(ctx))
