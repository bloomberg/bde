import copy
import os
import re

from bdeoptions import Options, OptionMask, Uplid, Ufid, RawOptions
from waflib import Context
from waflib import Utils

class BdeWafConfigure(object):

    def __init__(self, ctx):
        self.ctx = ctx

        self.external_libs = set()
        self.export_groups = []
        self.group_dep = {}
        self.group_mem = {}
        self.group_defs = {}
        self.group_opts = {}
        self.group_doc = {}
        self.group_cap = {}

        # Stores the subdirectory under which the stand-alone package is stored
        # e.g. { 'a_comdb2': 'adapters' }  = package 'a_comdb2' is stored under 'adapters'
        # meta-data of stand-alone packages are stored with package groups
        self.sa_package_locs = {}


        # Stores the subdirectory under which the package group is stored.  Almost all package groups currently reside
        # under 'groups', except for 'e_ipc', which resides under 'enterprise'.
        self.group_locs = {}

        self.package_dep = {}
        self.package_mem = {}
        self.package_pub = {}
        self.package_opts = {}
        self.package_cap = {}
        self.package_dums = []

        self.unsupported_groups = set()
        self.unsupported_packages = set()

        self.sl_package_deps = {}
        self.sl_package_locations = {}

        self.group_options = {}
        self.group_export_options = {}
        self.package_options = {}
        self.custom_envs = {}


    REMOVE_COMMENT_RE = re.compile(r'^([^#]*)(#.*)?$')
    @staticmethod
    def _get_meta(node, metadir, metatype):
        metafile = node.make_node([metadir, node.name + '.' + metatype])
        entries = []
        txt = metafile.read()
        for line in txt.splitlines():
            entries.extend(BdeWafConfigure.REMOVE_COMMENT_RE.match(line).group(1).split())

        return entries

    def _get_raw_options(self, node, metadir, metatype):
        metafile = node.make_node([metadir, node.name + '.' + metatype])
        raw_options = RawOptions()
        raw_options.read(metafile.abspath())

        return raw_options.options


    def _parse_group_doc(self, group_node):
        ''' parse the doc of a package group and return (name, description) to be used in the .pc file '''
        name = group_node.name
        doc_node = group_node.make_node(['doc', name + '.txt'])

        try:
            doc = Utils.readf(doc_node.abspath())

            purpose = None
            mnemonic = None
            for line in doc.split('\n'):
                if line.startswith('@PURPOSE'):
                    purpose = line.split(':')[1].strip()

                elif line.startswith('@MNEMONIC'):
                    mnemonic = line.split(':')[1].strip()

                if purpose and mnemonic:
                    return (mnemonic, purpose)
        except:
            pass

        return (name, 'N/A')


    def _load_metadata(self):

        groups_nodes = [x.parent.parent for x in self.ctx.path.ant_glob('groups/*/group/*.mem')]
        enterprise_nodes = [x.parent.parent for x in self.ctx.path.ant_glob('enterprise/*/group/*.mem')]

        group_nodes = groups_nodes + enterprise_nodes

        for g in group_nodes:
            self.group_dep[g.name] = self._get_meta(g, 'group', 'dep')
            self.group_mem[g.name] = self._get_meta(g, 'group', 'mem')
            self.group_defs[g.name] = self._get_raw_options(g, 'group', 'defs')
            self.group_opts[g.name] = self._get_raw_options(g, 'group', 'opts')
            self.group_cap[g.name] = self._get_raw_options(g, 'group', 'cap')
            self.group_doc[g.name] = self._parse_group_doc(g)
            if g.name + 'scm' in g.listdir():
                self.export_groups.append(g.name)

            self.group_locs[g.name] = g.parent.name

        # stand-alone packages behaves like pakcage groups with a single package
        adapter_nodes = [x.parent.parent for x in self.ctx.path.ant_glob('adapters/*/package/*.mem')]
        sa_package_nodes = adapter_nodes

        for s in sa_package_nodes:
            self.group_dep[s.name] = self._get_meta(s, 'package', 'dep')
            self.group_mem[s.name] = self._get_meta(s, 'package', 'mem')
            self.group_defs[s.name] = self._get_raw_options(s, 'package', 'defs')
            self.group_opts[s.name] = self._get_raw_options(s, 'package', 'opts')
            self.group_cap[s.name] = self._get_raw_options(s, 'package', 'cap')
            self.group_doc[s.name] = self._parse_group_doc(s)

            dums_file = s.make_node('package').find_node(s.name + '.dums')
            if dums_file:
                self.package_dums.append(s.name)

            self.export_groups.append(s.name)
            self.sa_package_locs[s.name] = s.parent.name

            # assume that std-alone packages are not headers only and do not have 'pub' files.

        for g in self.group_dep:
            for dep in self.group_dep[g]:
                if dep not in self.group_dep:
                    self.external_libs.add(dep)

        for group_node in group_nodes:
            for package_name in self.group_mem[group_node.name]:
                package_node = group_node.make_node(package_name)
                self.package_dep[package_name] = self._get_meta(package_node, 'package', 'dep')
                self.package_mem[package_name] = self._get_meta(package_node, 'package', 'mem')
                self.package_opts[package_name] = self._get_raw_options(package_node, 'package', 'opts')
                self.package_cap[package_name] = self._get_raw_options(package_node, 'package', 'cap')

                # only header-only packages typically have 'pub' files
                try:
                    self.package_pub[package_name] = self._get_meta(package_node, 'package', 'pub')
                except:
                    pass

                dums_file = package_node.make_node('package').find_node(package_node.name + '.dums')
                if dums_file:
                    self.package_dums.append(package_node.name)


    def configure(self, uplid, ufid):
        self.ctx.msg('os_type', uplid.uplid['os_type'])
        self.ctx.msg('os_name', uplid.uplid['os_name'])
        self.ctx.msg('cpu_type', uplid.uplid['cpu_type'])
        self.ctx.msg('os_ver', uplid.uplid['os_ver'])
        self.ctx.msg('comp_type', uplid.uplid['comp_type'])
        self.ctx.msg('comp_ver', uplid.uplid['comp_ver'])
        self.ctx.msg('uplid', uplid)
        self.ctx.msg('ufid', '_'.join(sorted(list(ufid.ufid))))
        self.ctx.msg('prefix', self.ctx.options.prefix)

        self.load_metadata()
        self.configure_external_libs(ufid)
        self.configure_options(uplid)
        self.save()

    def _levelize_group_dependencies(self, group):
        from collections import defaultdict
        level_map = defaultdict(set)

        def _levelize_groups_impl(group):
            if not group in self.group_dep or not self.group_dep[group]:
                level_map[1].add(group)
                return 1

            children = self.group_dep[group]

            level = 1 + max(_levelize_groups_impl(child) for child in self.group_dep[group])
            level_map[level].add(group)
            return level

        map(lambda x: _levelize_groups_impl(x), self.group_dep[group])

        levels = []
        for level in sorted(level_map.keys()):
            levels.append(level_map[level])

        return levels


    def _evaluate_group_options(self, group):

        # %s_LOCN and BDE_CXXINCLUDES are hard coded in bde_build.pl

        defs = copy.deepcopy(self.default_opts)

        group_node = self.ctx.path.make_node(['groups', group])
        defs.options['%s_LOCN' % group.upper()] = group_node.abspath()

        levels = self._levelize_group_dependencies(group)

        for level in levels:
            for group_dependency in sorted(level):
                if not group_dependency in self.external_libs:
                    defs.read(self.group_defs[group_dependency], self.ctx)
                    defs.read(self.group_cap[group_dependency], self.ctx)

        defs.read(self.group_defs[group], self.ctx)
        defs.read(self.group_cap[group], self.ctx)
        opts = copy.deepcopy(defs)
        opts.read(self.group_opts[group], self.ctx)

        defs.evaluate()

        if defs.options.get('CAPABILITY') == 'NEVER':
            self.unsupported_groups.add(group)
            if not group in self.sa_package_locs:
                self.unsupported_packages |= set(self.group_mem[group])
            return 'skipped (unsupported)'

        self.group_export_options[group] = defs.options

        unsupported_packages = set()
        if not group in self.sa_package_locs:
            for package in self.group_mem[group]:
                p_opts = copy.deepcopy(opts)

                package_node = group_node.make_node(package)

                # hacks to make bst+apache work. bst+apache's opts file references the _LOCN variable.
                p_opts.options['%s_LOCN' % package.upper()] = package_node.abspath()

                # from bde_build.pl 'BDE_CXXINCLUDES = $(SWITCHCHAR)I. $(BDE_CXXINCLUDE) $(PKG_INCLUDES) $(GRP_INCLUDES)'
                p_opts.options['BDE_CXXINCLUDES'] = '$(BDE_CXXINCLUDE)'

                p_opts.read(self.package_opts[package], self.ctx)

                # Ideally, we should also read the capability files of the packages on which this depends, but since
                # bde_build.pl doesn't do this, we don't need to do it for now.
                p_opts.read(self.package_cap[package], self.ctx)
                p_opts.evaluate()

                if p_opts.options.get('CAPABILITY') == 'NEVER':
                    unsupported_packages.add(package)
                else:
                    self.package_options[package] = p_opts.options

        # from bde_build.pl 'BDE_CXXINCLUDES = $(SWITCHCHAR)I. $(BDE_CXXINCLUDE) $(PKG_INCLUDES) $(GRP_INCLUDES)'
        opts.options['BDE_CXXINCLUDES'] = '$(BDE_CXXINCLUDE)'
        opts.evaluate()
        self.group_options[group] = opts.options

        if unsupported_packages:
            self.unsupported_packages |= unsupported_packages
            return 'ok, with some skipped (%s)' % ','.join(unsupported_packages)

        return 'ok'


    def load_metadata(self):
        self.ctx.start_msg('Loading BDE metadata')
        self._load_metadata()
        self.ctx.end_msg('ok')


    def configure_external_libs(self, ufid):
        self.ufid = copy.deepcopy(ufid)

        pkgconfig_args = [ '--libs', '--cflags' ]

        shared_flag =  'shr' in self.ufid.ufid

        if shared_flag:
            self.ufid.ufid.remove('shr')
            self.libtype_features = ['cxxshlib']
        else:
            pkgconfig_args.append('--static')
            self.libtype_features = ['cxxstlib']

        # If the static build is chosen (the default), waf assumes that all libraries queried from pkg-config are to be
        # built statically, which is not true for some libraries. We work around this issue by manually changing the
        # affected libraries to be linked dynamically instead.
        dl_overrides = ['pthread', 'rt', 'nsl', 'socket']

        for lib in self.external_libs:
            self.ctx.check_cfg(package = lib,
                               args = pkgconfig_args,
                               errmsg = "Make sure the path indicated by environment variable 'PKG_CONFIG_PATH' contains '%s.pc'" % lib)

            sl_key = ('stlib_' + lib).upper()
            dl_key = ('lib_' + lib).upper()

            # preserve the order of libraries
            for l in dl_overrides:
                if l in self.ctx.env[sl_key]:
                    if dl_key not in self.ctx.env:
                        self.ctx.env[dl_key] = []

                    self.ctx.env[sl_key].remove(l)
                    self.ctx.env[dl_key].append(l)

            # check_cfg always stores the libpath as dynamic library path instead of static even if the configuration
            # option is set to static.
            if not shared_flag:
                slp_key = ('stlibpath_' + lib).upper()
                dlp_key = ('libpath_' + lib).upper()
                if dlp_key in self.ctx.env:
                    self.ctx.env[slp_key] = self.ctx.env[dlp_key]
                    del self.ctx.env[dlp_key]


    def configure_options(self, uplid):

        self.uplid = uplid
        self.option_mask = OptionMask(self.uplid, self.ufid)

        default_opts_path = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'default.opts')
        raw_options = RawOptions()
        raw_options.read(default_opts_path)

        # At BB, default_internal.opts contains some variables that is required for building bde-bb.
        bde_root = os.environ.get('BDE_ROOT')

        if bde_root:
            default_internal_opts_path = os.path.join(bde_root, 'etc', 'default_internal.opts')
            raw_options.read(default_internal_opts_path)

        debug_opt_keys = self.ctx.options.debug_opt_keys
        if debug_opt_keys:
            debug_opt_keys = debug_opt_keys.split(',')

        self.default_opts = Options(self.option_mask)
        self.default_opts.read(raw_options.options, self.ctx, debug_opt_keys=debug_opt_keys)

        for g in self.group_dep:
            self.ctx.start_msg("Evaluating options for '%s'" % g)
            status_msg = self._evaluate_group_options(g)
            self.ctx.end_msg(status_msg)

        tmp_opts = copy.deepcopy(self.default_opts)
        tmp_opts.evaluate()

        env_variables = ('SET_TMPDIR', 'XLC_LIBPATH')
        setenv_re = re.compile(r'^([^=]+)=(.*)$')
        for e in env_variables:
            if e in tmp_opts.options:
                m = setenv_re.match(tmp_opts.options[e])
                self.custom_envs[m.group(1)] = m.group(2)

    def _parse_ldflags(self, ldflags):
        """
        parse the linker flags into the following components:
        stlib, libs, libpaths, flags
        """

        stlibs = []
        libs = []
        libpaths = []
        flags = []

        shlib_marker = self.ctx.env['SHLIB_MARKER']
        stlib_marker = self.ctx.env['STLIB_MARKER']

        libs_exp = re.compile(self.ctx.env['LIB_ST'].replace('%s', r'([^ =]+)'))

        libpath_exp = re.compile(self.ctx.env['LIBPATH_ST'].replace('%s', r'([^ =]+)'))

        # default to shlibs
        isshlib_flag = True

        for flag in ldflags:
            if flag == shlib_marker:
                isshlib_flag = True
                continue

            if flag == stlib_marker:
                isshlib_flag = False
                continue

            m = libpath_exp.match(flag)
            if m:
                libpaths.append(m.group(1))
                continue

            m = libs_exp.match(flag)
            if m:
                lib = m.group(1)
                if isshlib_flag:
                    libs.append(lib)
                else:
                    stlibs.append(lib)
                continue

            flags.append(flag)

        return (stlibs, libs, libpaths, flags)


    def _parse_cflags(self, cflags):
        includes = []
        flags = []

        inc_exp = re.compile(self.ctx.env['CPPPATH_ST'].replace('%s', r'([^ =]+)'))

        for flag in cflags:
            m = inc_exp.match(flag)
            if m:
                lib = m.group(1)
                includes.append(m.group(1))
                continue

            flags.append(flag)

        return (includes, flags)

    def _get_export_cxxflags(self, cxxflags):
        "only defines is required to be in export flags"
        export_flags = []
        for flag in cxxflags:
            st = flag[:2]
            if st == '-D' or (self.ctx.env.CXX_NAME == 'msvc' and st == '/D'):
                export_flags.append(flag)

        return export_flags

    def _save_group_options(self, group):
        export_options = self.group_export_options[group]
        options = self.group_options[group]

        # First value in the list is the compiler or linker, and so we ignore it to just take the parameters.

        (stlibs, libs, libpaths, linkflags) = self._parse_ldflags(options['CXXLINK'].split()[1:] +
                                                                  options['COMPONENT_BDEBUILD_LDFLAGS'].split())

        self.ctx.env[group + '_export_libs'] = libs
        self.ctx.env[group + '_export_cxxflags'] = self._get_export_cxxflags(export_options['COMPONENT_BDEBUILD_CXXFLAGS'].split())

        self.ctx.env[group + '_libs'] = libs
        self.ctx.env[group + '_stlibs'] = stlibs
        self.ctx.env[group + '_libpaths'] = libpaths
        self.ctx.env[group + '_linkflags'] = linkflags

        if group in self.sa_package_locs:
            (cxxincludes, cxxflags) = self._parse_cflags(options['CXX'].split()[1:] +
                                                         options['COMPONENT_BDEBUILD_CXXFLAGS'].split())
            (cincludes, cflags) = self._parse_cflags(options['CC'].split()[1:] +
                                                     options['COMPONENT_BDEBUILD_CXXFLAGS'].split())

            self.ctx.env[group + '_cxxflags'] = cxxflags
            self.ctx.env[group + '_cxxincludes'] = cxxincludes
            self.ctx.env[group + '_cflags'] = cflags
            self.ctx.env[group + '_cincludes'] = cincludes


    def _save_package_options(self, package):
        options = self.package_options[package]

        (stlibs, libs, libpaths, linkflags) = self._parse_ldflags(options['CXXLINK'].split()[1:] +
                                                                  options['COMPONENT_BDEBUILD_LDFLAGS'].split())

        (cxxincludes, cxxflags) = self._parse_cflags(options['CXX'].split()[1:] +
                                                     options['COMPONENT_BDEBUILD_CXXFLAGS'].split())
        (cincludes, cflags) = self._parse_cflags(options['CC'].split()[1:] +
                                                 options['COMPONENT_BDEBUILD_CFLAGS'].split())

        self.ctx.env[package + '_cxxflags'] = cxxflags
        self.ctx.env[package + '_cxxincludes'] = cxxincludes
        self.ctx.env[package + '_cflags'] = cflags
        self.ctx.env[package + '_cincludes'] = cincludes
        self.ctx.env[package + '_libs'] = libs
        self.ctx.env[package + '_stlibs'] = stlibs
        self.ctx.env[package + '_libpaths'] = libpaths
        self.ctx.env[package + '_linkflags'] = linkflags


    def save(self):
        self.ctx.start_msg('Saving configuration')
        self.ctx.env['ufid'] = self.option_mask.ufid.ufid

        # For visual studio, waf explicitly includes the system header files by setting the 'INCLUD'
        # variable. BSL_OVERRIDE_STD mode requires that the system header files, which contains the standard library, be
        # overridden with custom versions in bsl, so we workaround the issue by moving the system includes to
        # 'INCLUDE_BSL' if it exists. This solution is not perfect, because it doesn't support package groups that
        # doesn't depend on bsl -- this is not a problem for BDE libraries.

        if self.option_mask.uplid.uplid['os_type'] == 'windows' and 'INCLUDES_BSL' in self.ctx.env:
            self.ctx.env['INCLUDES_BSL'].extend(self.ctx.env['INCLUDES'])  # assuming that 'INCLUDES' containly system
                                                                           # headers only
            del self.ctx.env['INCLUDES']

        # ar on aix only processes 32-bit object files by default
        if '64' in self.option_mask.ufid.ufid and self.option_mask.uplid.uplid['comp_type'] == 'xlc':
            self.ctx.env['ARFLAGS'] = ['-rcs', '-X64']

        # Remove unsupported package groups and packages.  We don't need to do dependency analysis because the
        # unsupported sets already contain all transitively unsupported nodes.

        for g in self.unsupported_groups:
            if g in self.export_groups: self.export_groups.remove(g)
            self.group_dep.pop(g, None)
            self.group_mem.pop(g, None)
            self.group_doc.pop(g, None)
            self.sa_package_locs.pop(g, None)
            self.group_locs.pop(g, None)

        for p in self.unsupported_packages:
            self.package_dep.pop(p, None)
            self.package_mem.pop(p, None)
            self.package_pub.pop(p, None)

        for g in self.group_mem:
            self.group_mem[g] = list(set(self.group_mem[g]) - self.unsupported_packages)

        self.ctx.env['external_libs'] = self.external_libs
        self.ctx.env['export_groups'] = self.export_groups
        self.ctx.env['group_dep'] = self.group_dep
        self.ctx.env['group_mem'] = self.group_mem
        self.ctx.env['group_doc'] = self.group_doc

        self.ctx.env['sa_package_locs'] = self.sa_package_locs
        self.ctx.env['group_locs'] = self.group_locs

        self.ctx.env['package_dep'] = self.package_dep
        self.ctx.env['package_mem'] = self.package_mem
        self.ctx.env['package_pub'] = self.package_pub
        self.ctx.env['package_dums'] = self.package_dums
        self.ctx.env['libtype_features'] = self.libtype_features
        self.ctx.env['prefix'] = self.ctx.options.prefix
        self.ctx.env['custom_envs']  = self.custom_envs

        for g in self.group_dep:
            self._save_group_options(g)

        for p in self.package_dep:
            self._save_package_options(p)

        self.ctx.end_msg('ok')

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
