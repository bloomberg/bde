import re
import os
import os.path
import sys

import bdeunittest
from waflib.TaskGen import feature, before_method
from waflib import Utils, Options, Task, Logs


# Support manual dependency specification with the 'depends_on' attribute
@feature('*')
@before_method('process_rule')
def post_the_other(self):
    deps = getattr(self, 'depends_on', [])
    for name in self.to_list(deps):
        other = self.bld.get_tgen_by_name(name)
        other.post()


@feature('bdepc')
@before_method('process_rule')
def make_pc(self):
    """Create a task to generate the pkg-config file."""
    self.create_task('bdepc', None, self.path.find_or_declare(self.target))

class bdepc(Task.Task):

    # replacement parameters: prefix, name, description, name, version, requires.private, name, libs, cflags
    PKGCONFIG_TEMPLATE = '''prefix=%s
libdir=${prefix}/lib
includedir=${prefix}/include/%s

Name: %s
Description: %s
URL: https://github.com/bloomberg/%s
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
                                               group_name,
                                               version,
                                               ' '.join(self.generator.dep),
                                               group_name,
                                               ' '.join(libs),
                                               ' '.join(bld.env[group_name + '_export_cxxflags'])
                                               )
        self.outputs[0].write(pc_source)

# Override the ListContext from waflib.Build to hide internal build targets
from waflib.Build import BuildContext
class ListContext(BuildContext):
    '''lists the targets to execute'''

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


class BdeWafBuild(object):

    def __init__(self, ctx):
        self.ctx = ctx
        self.features = ['cxx']
        self.libtype_features = []

        self.ufid = self.ctx.env['ufid']
        self.external_libs = self.ctx.env['external_libs']
        self.group_dep = self.ctx.env['group_dep']
        self.group_mem = self.ctx.env['group_mem']
        self.group_doc = self.ctx.env['group_doc']
        self.export_groups = self.ctx.env['export_groups']

        self.package_dep = self.ctx.env['package_dep']
        self.package_mem = self.ctx.env['package_mem']
        self.package_pub = self.ctx.env['package_pub']

        self.libtype_features = self.ctx.env['libtype_features']

        self.run_tests = self.ctx.options.test == 'run'
        self.build_tests = self.run_tests or self.ctx.options.test == 'build'

        test_runner_path = self.ctx.path.make_node(os.path.join('tools', 'waf', 'run_unit_tests.py'))
        self.ctx.options.testcmd = '%s %s --verbosity %s --timeout %s %%s' % (sys.executable, test_runner_path.abspath(),
                                                                              self.ctx.options.test_verbosity,
                                                                              self.ctx.options.test_timeout)


    def _build_package(self, package_name, group_node, group_internal_deps, group_external_deps):
        package_node = group_node.make_node(package_name)
        deps = [p + '_src' for p in self.package_dep[package_name]]

        for g in group_internal_deps:
            deps.extend([p + '_lib' for p in self.group_mem[g]])

        cxxflags = self.ctx.env[package_name + '_cxxflags']
        linkflags = self.ctx.env[package_name + '_linkflags']
        libs = self.ctx.env[package_name + '_libs']

        components = self.package_mem[package_name]

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

        if components:
            cpp_files = [component + '.cpp' for component in components]
        else:
            # packages whoes name contains a '+' are special in that their 'mem' files are empty and they do not contain
            # typical bde-style components.  These packages contain either only headers, or contain 'cpp' files that do
            # not have corresponding '.h' nad '.t.cpp' files.

            cpp_files = [x.name for x in package_node.ant_glob('*.cpp')]

        if not cpp_files:
            self.ctx(name            = package_name + '_src',
                     group           = group_node.name,
                     path            = package_node,
                     includes        = '.',
                     export_includes = '.'
                     )
        else:
            self.ctx(name            = package_name + '_src',
                     path            = package_node,
                     group           = group_node.name,
                     source          = cpp_files,
                     features        = self.features,
                     cxxflags        = cxxflags,
                     includes        = '.',
                     export_includes = '.',
                     use             = deps,
                     uselib          = group_external_deps
                     )

            self.ctx(name            = package_name + '_lib',
                     path            = package_node,
                     target          = package_name,
                     group           = group_node.name,
                     features        = self.features + self.libtype_features,
                     linkflags       = linkflags,
                     lib             = libs,
                     use             = package_name + '_src',
                     )

            if self.build_tests:
                test_features = self.features + ['cxxprogram']
                if self.run_tests:
                    test_features = test_features + ['test']

                for c in components:
                    build_test = self.ctx(
                        name         = c + '.t',
                        path         = package_node,
                        group        = group_node.name,
                        source       = c + '.t.cpp',
                        target       = c + '.t',
                        features     = test_features,
                        cxxflags     = cxxflags,
                        linkflags    = linkflags,
                        lib          = libs,
                        includes     = '.',
                        use          = package_name + '_lib',
                        uselib       = group_external_deps
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

    def _build_group(self, group_name, groups_node):
        group_node = groups_node.make_node(group_name)
        deps = set(self.group_dep[group_name])
        internal_deps = deps - self.external_libs
        external_deps = deps & self.external_libs

        # waf uses all uppercase words to identify pkgconfig based dependencies
        external_deps = [l.upper() for l in external_deps]
        packages = self.group_mem[group_name]

        linkflags = self.ctx.env[group_name + '_linkflags']
        libs = self.ctx.env[group_name + '_libs']

        # Note that 'add_group' adds a waf build group, which is not related to a package group
        self.ctx.add_group(group_name)

        for p in packages:
            self._build_package(p, group_node, internal_deps, external_deps)

        if group_name in self.export_groups:
            install_path = os.path.join('${PREFIX}', 'lib')
            self._make_pc_group(group_name, internal_deps, external_deps)
        else:
            install_path = None

        self.ctx(name         = group_name,
                 path         = group_node,
                 group        = group_name,
                 # source       = packages,
                 target       = group_name,
                 features     = self.features + self.libtype_features,
                 linkflags    = linkflags,
                 lib          = libs,
                 use          = list(internal_deps) + [p + '_src' for p in packages] + [p + '_tst' for p in packages],
                 uselib       = external_deps,
                 install_path = install_path,
                 )



    VERSION_CHECK_TEMPLATE = '''
#include <%sscm_versiontag.h>
#include <stdio.h>

int main()
{
    printf("%%d.%%d", %s_VERSION_MAJOR, %s_VERSION_MINOR);
}
'''

    def _make_pc_group(self, group_name, internal_deps, external_deps):

        def make_vc_source(task):

            vc_source = BdeWafBuild.VERSION_CHECK_TEMPLATE % (group_name.lower(),
                                                              group_name.upper(),
                                                              group_name.upper())

            task.outputs[0].write(vc_source)


        vc_node = self.ctx.path.make_node('vc');
        version_tag_node = self.ctx.srcnode.find_resource([
                'groups',
                group_name,
                group_name + 'scm',
                group_name + 'scm_versiontag.h'])

        if False:
            vcSourceTask = self.ctx(name   = group_name + '_vc.cpp',
                                    path   = vc_node,
                                    rule   = make_vc_source,
                                    target = group_name + '_vc.cpp'
                                    )
            vcSourceTask.post();

            vcExeTask = self.ctx(name      = group_name + '_vc',
                                 path      = vc_node,
                                 source    = vcSourceTask.tasks[0].outputs[0],
                                 target    = group_name + '_vc',
                                 features  = self.features + ['cxxprogram'],
                                 cxxflags  = self.ctx.env[group_name + 'scm_cxxflags'],
                                 linkflags = self.ctx.env[group_name + 'scm_linkflags'],
                                 use       = list(internal_deps) + [group_name + 'scm'],
                                 uselib    = external_deps
                                 )
            vcExeTask.post();
            self.ctx(name   = group_name + '.pc',
                     path   = vc_node,
                     rule   = make_pc,
                     source = vcExeTask.tasks[1].outputs[0],
                     target = group_name + '.pc',
                     )
        else:
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
        groups_node = self.ctx.path.make_node('groups')

        for g in self.group_dep:
            self._build_group(g, groups_node)

        if self.run_tests:
            self.ctx.add_post_fun(bdeunittest.summary)
