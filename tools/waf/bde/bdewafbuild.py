import re
import os
import os.path
import sys

import bdeunittest
from waflib.TaskGen import feature, after_method, before_method
from waflib import Errors, Utils, Options, Task, Logs

class BdeWafBuild(object):

    def __init__(self, ctx):
        self.ctx = ctx
        self.libtype_features = []

        self.ufid = self.ctx.env['ufid']
        self.external_libs = self.ctx.env['external_libs']
        self.group_dep = self.ctx.env['group_dep']
        self.group_mem = self.ctx.env['group_mem']
        self.group_doc = self.ctx.env['group_doc']
        self.export_groups = self.ctx.env['export_groups']

        self.sa_package_locs = self.ctx.env['sa_package_locs']
        self.group_locs = self.ctx.env['group_locs']

        self.package_dep = self.ctx.env['package_dep']
        self.package_mem = self.ctx.env['package_mem']
        self.package_pub = self.ctx.env['package_pub']
        self.package_dums = self.ctx.env['package_dums']

        self.libtype_features = self.ctx.env['libtype_features']
        self.custom_envs = self.ctx.env['custom_envs']

        self.run_tests = self.ctx.options.test == 'run'
        self.build_tests = self.run_tests or self.ctx.options.test == 'build'

        test_runner_path = os.path.join(os.path.dirname(os.path.dirname(os.path.dirname(os.path.realpath(__file__)))),
                                        'run_unit_tests.py')
        self.ctx.options.testcmd = '%s %s %%s --verbosity %s --timeout %s' % (sys.executable,
                                                                              test_runner_path,
                                                                              self.ctx.options.test_verbosity,
                                                                              self.ctx.options.test_timeout)


    def _build_package_impl(self, package_name, package_node, group_node, components, internal_deps, external_deps,
                            install_path):

        cflags = self.ctx.env[package_name + '_cflags']
        cxxflags = self.ctx.env[package_name + '_cxxflags']
        cincludes = self.ctx.env[package_name + '_cincludes']
        cxxincludes = self.ctx.env[package_name + '_cxxincludes']
        libs = self.ctx.env[package_name + '_libs']
        stlibs = self.ctx.env[package_name + '_stlibs']
        libpaths = self.ctx.env[package_name + '_libpaths']
        linkflags = self.ctx.env[package_name + '_linkflags']

        if package_name in self.package_pub:
            # Some files necessary for compilation are missing from the pub files
            # For example, bsl+stdhdrs/sys/time.h
            # As a work-around, simply export all files listed in the pub files and any missing 'h' or 'SUNWCCh' files

            install_headers = package_node.ant_glob('**/*.h')
            install_headers.extend(package_node.ant_glob('**/*.SUNWCCh'))

            pub_header_names = self.package_pub[package_name]
            pub_headers = [package_node.make_node(h) for h in pub_header_names]

            install_header_paths = [h.abspath() for h in install_headers]

            for ph in pub_headers:
                if not ph.abspath() in install_header_paths:
                    install_headers.append(ph)
        else:
            header_names = [c + '.h' for c in components]
            install_headers = [package_node.make_node(h) for h in header_names]

        path_headers = {}
        for h in install_headers:
            path = os.path.dirname(h.path_from(package_node))
            if not path in path_headers:
                path_headers[path] = []

            path_headers[path].append(h)

        for path in path_headers:
            self.ctx.install_files(os.path.join('${PREFIX}', 'include', group_node.name, path),
                                   path_headers[path])

        cpp_flag = len(package_node.ant_glob('*.cpp')) > 0

        src_extension = '.cpp' if cpp_flag else '.c'

        if cpp_flag:
            features = ['cxx']
        else:
            features = ['c']


        dum_task_gens = []
        if package_name in self.package_dums:

            self.ctx(name = package_name + '.dums',
                     path = package_node,
                     rule = 'cp ${SRC} ${TGT}',
                     source = package_node.make_node(['package', package_name + '.dums']),
                     target = package_name + '_dums.c'
                     )

            self.ctx(name            = package_name + '_dums',
                     path            = package_node,
                     source          = [package_name + '_dums.c'],
                     features        = ['c'],
                     cflags          = cflags,
                     depends_on      = package_name + '.dums',
                     )
            dum_task_gens.append(package_name + '_dums')

        if components:
            cpp_files = [component + src_extension for component in components]
        else:
            # packages whose name contains a '+' are special in that their 'mem' files are empty and they do not contain
            # typical bde-style components.  These packages contain either only headers, or contain 'cpp' files that do
            # not have corresponding '.h' nad '.t.cpp' files.

            # These header-only packages should always have a dummy.cpp file.

            cpp_files = [x.name for x in package_node.ant_glob('*' + src_extension)]

        if not cpp_files:
            self.ctx.fatal('package %s does not contain any components' % package_name)

        self.ctx(name            = package_name + '_lib',
                 target          = package_name,
                 path            = package_node,
                 source          = cpp_files,
                 features        = features + self.libtype_features,
                 cflags          = cflags,
                 cincludes       = cincludes,
                 cxxflags        = cxxflags,
                 cxxincludes     = cxxincludes,
                 linkflags       = linkflags,
                 includes        = [package_node],
                 export_includes = [package_node],
                 use             = internal_deps,
                 uselib          = external_deps,
                 lib             = libs,
                 stlib           = stlibs,
                 cust_libpaths   = libpaths,
                 install_path    = install_path,
                 )

        if self.build_tests:
            test_features = features + ['cxxprogram']
            if self.run_tests:
                test_features = test_features + ['test']

            for c in components:
                build_test = self.ctx(
                    name          = c + '.t',
                    path          = package_node,
                    source        = c + '.t' + src_extension,
                    target        = c + '.t',
                    features      = test_features,
                    cflags        = cflags,
                    cincludes     = cincludes,
                    cxxflags      = cxxflags,
                    cxxincludes   = cxxincludes,
                    linkflags     = linkflags,
                    lib           = libs,
                    stlib         = stlibs,
                    cust_libpaths = libpaths,
                    includes      = [package_node],
                    use           = [package_name + '_lib'] + dum_task_gens,
                    uselib        = external_deps
                    )

        else:
            # Create the same number of task generators to ensure that the generators created with or without tests
            # have the same idx
            for c in components:
                self.ctx(
                    name         = c + '.t',
                    path         = package_node)

        self.ctx(name       = package_name + '_tst',
                 depends_on = [c + '.t' for c in components]
                 )

        self.ctx(name       = package_name,
                 depends_on = [package_name + '_lib', package_name + '_tst']
                 )

    def _build_sa_package(self, package_name):

        # Standard alone packages are architecturally at the same level as package groups, but have the same physical
        # structure as regular packages.  I.e., they can depend directly on other package groups and consititute a UOR
        # (a library) that is on the same hierarchical level as a package group.  Therefore, the metadata for standard
        # alone packages are stored together with package groups.

        package_node = self.ctx.path.make_node(self.sa_package_locs[package_name]).make_node(package_name)
        deps = set(self.group_dep[package_name])
        internal_deps = deps - self.external_libs
        internal_deps = [g + '_lib' for g in internal_deps]
        external_deps = deps & self.external_libs
        # waf uses all uppercase words to identify pkgconfig based dependencies
        external_deps = [l.upper() for l in external_deps]

        components = self.group_mem[package_name]

        if package_name in self.export_groups:
            install_path = os.path.join('${PREFIX}', 'lib')
            self._make_pc_group(package_name, internal_deps, external_deps)
        else:
            install_path = None

        self._build_package_impl(package_name, package_node, package_node, components, internal_deps, external_deps,
                                 install_path)


    def _build_normal_package(self, package_name, group_node, group_internal_deps, group_external_deps):
        package_node = group_node.make_node(package_name)
        deps = [p + '_lib' for p in self.package_dep[package_name]]
        deps.extend([g + '_lib' for g in group_internal_deps])
        components = self.package_mem[package_name]

        self._build_package_impl(package_name, package_node, group_node, components, deps, group_external_deps, None)


    def _build_group(self, group_name):
        group_node = self.ctx.path.make_node(self.group_locs[group_name]).make_node(group_name)
        deps = set(self.group_dep[group_name])
        internal_deps = deps - self.external_libs
        external_deps = deps & self.external_libs

        # waf uses all uppercase words to identify pkgconfig based dependencies
        external_deps = [l.upper() for l in external_deps]
        packages = self.group_mem[group_name]

        linkflags = self.ctx.env[group_name + '_linkflags']
        libs = self.ctx.env[group_name + '_libs']
        stlibs = self.ctx.env[group_name + '_stlibs']
        libpaths = self.ctx.env[group_name + '_libpaths']

        for p in packages:
            self._build_normal_package(p, group_node, internal_deps, external_deps)

        if group_name in self.export_groups:
            install_path = os.path.join('${PREFIX}', 'lib')
            self._make_pc_group(group_name, internal_deps, external_deps)
        else:
            install_path = None

        self.ctx(name            = group_name + '_lib',
                 path            = group_node,
                 target          = group_name,
                 features        = ['cxx'] + self.libtype_features,
                 linkflags       = linkflags,
                 lib             = libs,
                 stlib           = stlibs,
                 cust_libpaths   = libpaths,
                 source          = [p + '_lib' for p in packages],
                 use             = [g + '_lib' for g in internal_deps],
                 uselib          = external_deps,
                 install_path    = install_path,
                 export_includes = packages,
                 )

        self.ctx(name       = group_name,
                 depends_on = [group_name + '_lib']  + [p + '_tst' for p in packages])

    def _make_pc_group(self, group_name, internal_deps, external_deps):

        vc_node = self.ctx.path.make_node('vc');
        version_tag_node = self.ctx.srcnode.find_resource([
                'groups',
                group_name,
                group_name + 'scm',
                group_name + 'scm_versiontag.h'])

        self.ctx(name       = group_name + '.pc',
                 features   = ['bdepc'],
                 path       = vc_node,
                 target     = group_name + '.pc',
                 doc        = self.group_doc[group_name],
                 dep        = self.group_dep[group_name],
                 group_name = group_name
                 )

        self.ctx.install_files(os.path.join('${PREFIX}', 'lib', 'pkgconfig'),
                               [os.path.join(vc_node.relpath(), group_name + '.pc')])


    def build(self):
        print 'Waf: using %d jobs (change with -j)' % self.ctx.options.jobs

        self.ctx.env['env'] = os.environ.copy()
        self.ctx.env['env'].update(self.custom_envs)

        for g in self.group_dep:
            if not g in self.sa_package_locs:
                self._build_group(g)
            else:
                self._build_sa_package(g)

        if self.run_tests:
            self.ctx.add_post_fun(bdeunittest.summary)


@feature('c')
@after_method('propagate_uselib_vars')
@before_method('apply_incpaths')
def append_custom_cincludes(self):
    if hasattr(self, 'cincludes'):
        self.env.INCLUDES.extend(self.cincludes)

    if hasattr(self, 'cust_libpaths'):
        self.env.STLIBPATH.extend(self.cust_libpaths)

@feature('cxx')
@after_method('propagate_uselib_vars')
@before_method('apply_incpaths')
def append_custom_cxxincludes(self):
    if hasattr(self, 'cxxincludes'):
        self.env.INCLUDES.extend(self.cxxincludes)


    if hasattr(self, 'cust_libpaths'):
        self.env.STLIBPATH.extend(self.cust_libpaths)


@feature('*')
@before_method('process_rule')
def post_the_other(self):
    """
    Support manual dependency specification with the 'depends_on' attribute
    """
    deps = getattr(self, 'depends_on', [])
    for name in self.to_list(deps):
        other = self.bld.get_tgen_by_name(name)
        other.post()


@feature('cstlib', 'cshlib', 'cxxstlib', 'cxxshlib', 'fcstlib', 'fcshlib')
@before_method('process_source')
def reuse_lib_objects(self):
    """
    Find sources that are libs; if any are found, extract their object lists and build this lib from the same
    objects. If this occurs, skip the normal process_source step.
    """
    tmp_source = []
    saw_target = False
    task_sources = []

    for source in self.to_list(self.source):
        try:
            y = self.bld.get_tgen_by_name(source)
            saw_target = True
            task_sources.append(y)
        except Errors.WafError:
            tmp_source.append(source)
            continue

    if saw_target and tmp_source:
        raise Errors.WafError("Cannot mix tasks and source files in shlib %s" % self.name)

    if saw_target:
        self.compiled_tasks = []
        for tg in task_sources:
            tg.post()
            for tsk in getattr(tg, 'compiled_tasks', []):
                self.compiled_tasks.append(tsk)

    self.source = tmp_source


# Patch ccroot.propagate_uselib_vars so that libraries can be repeated.
# This is required to support cyclic dependencies and bde-bb.
def propagate_uselib_vars(self):
    """
    Process uselib variables for adding flags. For example, the following target::

        def build(bld):
            bld.env.AFLAGS_aaa = ['bar']
            from waflib.Tools.ccroot import USELIB_VARS
            USELIB_VARS['aaa'] = set('AFLAGS')

            tg = bld(features='aaa', aflags='test')

    The *aflags* attribute will be processed and this method will set::

            tg.env.AFLAGS = ['bar', 'test']
    """

    _vars = self.get_uselib_vars()
    env = self.env

    for x in _vars:
        y = x.lower()
        env.append_value(x, self.to_list(getattr(self, y, [])))

    for x in self.features:
        for var in _vars:
            compvar = '%s_%s' % (var, x)
            env.append_value(var, env[compvar])

    for x in self.to_list(getattr(self, 'uselib', [])):
        for v in _vars:
            env.append_value(v, env[v + '_' + x])

from waflib.TaskGen import task_gen
from waflib.Tools import ccroot
setattr(task_gen, ccroot.propagate_uselib_vars.__name__, propagate_uselib_vars)


from waflib.Build import BuildContext
class ListContext(BuildContext):
    """
    lists the targets to execute
    Override the ListContext from waflib.Build to hide internal build targets
    """

    cmd = 'list'
    def execute(self):
        """
        See :py:func:`waflib.Context.Context.execute`.
        """
        self.restore()
        if not self.all_envs:
            self.load_envs()

        self.recurse([self.run_dir])
        self.pre_build()

        # display the time elapsed in the progress bar
        self.timer = Utils.Timer()

        for g in self.groups:
            for tg in g:
                try:
                    f = tg.post
                except AttributeError:
                    pass
                else:
                    f()

        try:
            # force the cache initialization
            self.get_tgen_by_name('')
        except Exception:
            pass
        lst = list(self.task_gen_cache_names.keys())
        lst.sort()

        hidden_suffixes = ['_lib', '_src', '_tst']
        for k in lst:
            if len(k) > 4 and (k[-4:] in hidden_suffixes):
                continue

            Logs.pprint('GREEN', k)


@feature('bdepc')
@before_method('process_rule')
def make_pc(self):
    """Create a task to generate the pkg-config file."""
    self.create_task('bdepc', None, self.path.find_or_declare(self.target))

class bdepc(Task.Task):

    # replacement parameters: prefix, group_name, name, description, version, requires.private, name, libs, cflags
    PKGCONFIG_TEMPLATE = '''prefix=%s
libdir=${prefix}/lib
includedir=${prefix}/include/%s

Name: %s
Description: %s
URL: https://github.com/bloomberg/bde
Version: %s
Requires:
Requires.private: %s
Libs: -L${libdir} -l%s %s
Libs.private:
Cflags: -I${includedir} %s
'''

    def signature(self):
        # Add prefix as part of the signature, so that the .pc file will be regenerated when the prefix changes
        self.hcode = Options.options.prefix
        ret = super(bdepc, self).signature()
        return ret

    def run(self):
        bld = self.generator.bld
        group_name = self.generator.group_name

        # if task.inputs and 'cxxstlib' in self.libtype_features:
        #     version = self.ctx.cmd_and_log(task.inputs[0].abspath()).strip()
        # TODO: have to use LD_LIBRARY_PATH for linking against 'so' files
        # else:
        version = ''

        libs = [bld.env['LIB_ST'] % l for l in bld.env[group_name + '_export_libs']]

        pc_source = self.PKGCONFIG_TEMPLATE % (Options.options.prefix,
                                               group_name,
                                               self.generator.doc[0],
                                               self.generator.doc[1],
                                               version,
                                               ' '.join(self.generator.dep),
                                               group_name,
                                               ' '.join(libs),
                                               ' '.join(bld.env[group_name + '_export_cxxflags'])
                                               )
        self.outputs[0].write(pc_source)


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
