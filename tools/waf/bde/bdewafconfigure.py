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

        self.package_dep = {}
        self.package_mem = {}
        self.package_pub = {}
        self.package_opts = {}

        self.sl_package_deps = {}
        self.sl_package_locations = {}

        self.group_options = {}
        self.group_export_options = {}
        self.package_options = {}


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

        group_nodes = [x.parent.parent for x in self.ctx.path.ant_glob('groups/*/group/*.mem')]

        for g in group_nodes:
            self.group_dep[g.name] = self._get_meta(g, 'group', 'dep')
            self.group_mem[g.name] = self._get_meta(g, 'group', 'mem')
            self.group_defs[g.name] = self._get_raw_options(g, 'group', 'defs')
            self.group_opts[g.name] = self._get_raw_options(g, 'group', 'opts')
            self.group_doc[g.name] = self._parse_group_doc(g)
            if g.name + 'scm' in g.listdir():
                self.export_groups.append(g.name)

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

                # only header only packages typically have 'pub' files
                try:
                    self.package_pub[package_name] = self._get_meta(package_node, 'package', 'pub')
                except:
                    pass


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

        defs.read(self.group_defs[group], self.ctx)
        opts = copy.deepcopy(defs)
        opts.read(self.group_opts[group], self.ctx)

        defs.evaluate()
        self.group_export_options[group] = defs.options

        for package in self.group_mem[group]:
            p_opts = copy.deepcopy(opts)

            package_node = group_node.make_node(package)

            # hacks to make bst+apache work. bst+apache's opts file references the _LOCN variable.
            p_opts.options['%s_LOCN' % package.upper()] = package_node.abspath()

            # from bde_build.pl 'BDE_CXXINCLUDES = $(SWITCHCHAR)I. $(BDE_CXXINCLUDE) $(PKG_INCLUDES) $(GRP_INCLUDES)'
            p_opts.options['BDE_CXXINCLUDES'] = '$(BDE_CXXINCLUDE)'

            p_opts.read(self.package_opts[package], self.ctx)
            p_opts.evaluate()

            self.package_options[package] = p_opts.options

        opts.evaluate()
        self.group_options[group] = opts.options


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
        dl_overrides = set(['pthread', 'rt'])

        for lib in self.external_libs:
            self.ctx.check_cfg(package = lib,
                               args = pkgconfig_args,
                               errmsg = "Make sure the path indicated by environment variable 'PKG_CONFIG_PATH' contains '%s.pc'" % lib)

            sl_key = ('stlib_' + lib).upper()
            dl_key = ('lib_' + lib).upper()

            sl_set = set(self.ctx.env[sl_key])
            self.ctx.env[sl_key] = list(sl_set - dl_overrides)
            # dl_set = set(self.ctx.env[dl_key])
            self.ctx.env[dl_key] = list(sl_set & dl_overrides)

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


        debug_opt_keys = self.ctx.options.debug_opt_keys
        if debug_opt_keys:
            debug_opt_keys = debug_opt_keys.split(',')

        self.default_opts = Options(self.option_mask)
        self.default_opts.read(raw_options.options, self.ctx, debug_opt_keys=debug_opt_keys)

        for g in self.group_dep:
            self.ctx.start_msg("Evaluating options for '%s'" % g)
            self._evaluate_group_options(g)
            self.ctx.end_msg('ok')


    def _get_libs(self, ldflags):
        libs = []
        flags = []
        expression = re.compile(self.ctx.env['LIB_ST'].replace('%s', r'([^ =]+)'))

        # import pdb; pdb.set_trace()
        for flag in ldflags:
            m = expression.match(flag)
            if m:
                lib = m.group(1)
                if not lib in libs:
                    libs.append(m.group(1))
            else:
                flags.append(flag)

        return (libs, flags)

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

        # First value in the list is the compiler, and so we ignore it to just take the parameters.
        linkflags = options['CXXLINK'].split()[1:]
        (libs, flags) = self._get_libs(options['COMPONENT_BDEBUILD_LDFLAGS'].split())
        linkflags.extend(flags)

        self.ctx.env[group + '_export_libs'] = libs
        self.ctx.env[group + '_export_cxxflags'] = self._get_export_cxxflags(export_options['COMPONENT_BDEBUILD_CXXFLAGS'].split())
        self.ctx.env[group + '_libs'] = libs
        self.ctx.env[group + '_linkflags'] = linkflags


    def _save_package_options(self, package):
        options = self.package_options[package]

        # First value in the list is the compiler, and so we ignore it to just take the parameters.
        cxxflags = options['CXX'].split()[1:]
        cflags = options['CC'].split()[1:]
        linkflags = options['CXXLINK'].split()[1:]
        (libs, flags) = self._get_libs(options['COMPONENT_BDEBUILD_LDFLAGS'].split())
        linkflags.extend(flags)

        self.ctx.env[package + '_cxxflags'] = cxxflags + options['COMPONENT_BDEBUILD_CXXFLAGS'].split()
        self.ctx.env[package + '_cflags'] = cflags + options['COMPONENT_BDEBUILD_CFLAGS'].split()
        self.ctx.env[package + '_libs'] = libs
        self.ctx.env[package + '_linkflags'] = linkflags


    def save(self):
        self.ctx.start_msg('Saving configuration')
        self.ctx.env['ufid'] = self.option_mask.ufid.ufid

        self.ctx.env['external_libs'] = self.external_libs
        self.ctx.env['export_groups'] = self.export_groups
        self.ctx.env['group_dep'] = self.group_dep
        self.ctx.env['group_mem'] = self.group_mem
        self.ctx.env['group_doc'] = self.group_doc

        self.ctx.env['package_dep'] = self.package_dep
        self.ctx.env['package_mem'] = self.package_mem
        self.ctx.env['package_pub'] = self.package_pub
        self.ctx.env['libtype_features'] = self.libtype_features
        self.ctx.env['prefix'] = self.ctx.options.prefix

        for g in self.group_dep:
            self._save_group_options(g)

        for p in self.package_dep:
            self._save_package_options(p)

        self.ctx.end_msg('ok')
