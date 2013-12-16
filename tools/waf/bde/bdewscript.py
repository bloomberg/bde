import os
import os.path
import re
import sys

from bdewafconfigure import BdeWafConfigure
from bdewafbuild import BdeWafBuild
from bdeoptions import Uplid, Ufid

from waflib import Context
from waflib import Utils

def options(ctx):
    ctx.load('bdeunittest', tooldir = os.path.join('tools', 'waf', 'bde'))

    from waflib.Tools.compiler_cxx import cxx_compiler
    cxx_compiler['win32'] = ['msvc']
    cxx_compiler['linux'] = ['g++']
    cxx_compiler['darwin'] = ['g++']

    ctx.load('compiler_cxx')

    platform = Utils.unversioned_sys_platform()

    ctx.load('msvs')
    ctx.load('xcode')

    _add_commandline_options(ctx)

def configure(ctx):
    ctx.load('bdeunittest', tooldir = os.path.join('tools', 'waf', 'bde'))

    ufid = _make_ufid_from_options(ctx.options)

    platform = Utils.unversioned_sys_platform()
    if platform == 'win32':
        if '64' in ufid.ufid:
            ctx.options.msvc_targets = 'x64'
        else:
            ctx.options.msvc_targets = 'x86'


    bde_configure = BdeWafConfigure(ctx)
    bde_configure.load_metadata()
    bde_configure.configure_external_libs(ufid)

    ctx.load('compiler_cxx')
    uplid = _make_uplid_from_context(ctx)

    ctx.msg('os_type', uplid.uplid['os_type'])
    ctx.msg('os_name', uplid.uplid['os_name'])
    ctx.msg('cpu_type', uplid.uplid['cpu_type'])
    ctx.msg('os_ver', uplid.uplid['os_ver'])
    ctx.msg('comp_type', uplid.uplid['comp_type'])
    ctx.msg('comp_ver', uplid.uplid['comp_ver'])
    ctx.msg('uplid', uplid)
    ctx.msg('ufid', '_'.join(ufid.ufid))
    ctx.msg('prefix', ctx.options.prefix)

    bde_configure.configure_options(uplid)
    bde_configure.save()


def build(ctx):
    if ctx.cmd == "msvs" or ctx.cmd == "msvs2008":
        if ctx.options.test:
            ctx.waf_command = 'waf.bat --test=%s' % ctx.options.test

        ctx.projects_dir = ctx.srcnode.make_node('.depproj')
        ctx.projects_dir.mkdir()

    bde_build = BdeWafBuild(ctx)
    bde_build.build()


def _make_ufid_from_options(opts):

    env_ufid = os.getenv('BDE_WAF_UFID')
    if env_ufid:
        return Ufid(env_ufid.split('_'))

    if opts.legacy_config:
        return Ufid(opts.legacy_config.split('_'))

    ufid_map = {
        'abi_bits': { '64': '64' },
        'build_type': { 'debug': 'dbg', 'release': 'opt' },
        'assert_level': { 'safe': 'safe', 'safe2': 'safe2' },
        'cpp11': { True: 'cpp11' },
        'noexception': { False: 'exc' },
        'library_type': { 'shared': 'shr' }
        }

    ufid = []
    for opt in ufid_map:
        attr = getattr(opts, opt, None)
        if attr is not None:
            if attr in ufid_map[opt]:
                ufid.append(ufid_map[opt][attr])


    # always use mt
    ufid.append('mt')

    return Ufid(ufid)


def _make_uplid_from_context(ctx):
    platform = Utils.unversioned_sys_platform()

    from bdeoptions import get_linux_osinfo, get_aix_osinfo, get_sunos_osinfo, get_darwin_osinfo, get_windows_osinfo
    osinfo_getters = {
        'linux': get_linux_osinfo,
        'aix': get_aix_osinfo,
        'sunos': get_sunos_osinfo,
        'darwin': get_darwin_osinfo,
        'win32': get_windows_osinfo
        }

    comp_getters = {
        'linux': _get_linux_comp,
        'aix': _get_aix_comp,
        'sunos': _get_sunos_comp,
        'win32': _get_windows_comp,
        'darwin': _get_darwin_comp
        }

    if platform not in osinfo_getters:
        ctx.fatal('Unsupported platform: %s' % platform)

    (os_type, os_name, os_ver) = osinfo_getters[platform](ctx)
    (cpu_type, cxx, cxx_version) = _sanitize_comp(ctx, comp_getters[platform](ctx))

    uplid = Uplid(os_type,
                  os_name,
                  cpu_type,
                  os_ver,
                  cxx,
                  cxx_version)
    return uplid


def _sanitize_comp(ctx, comp):

    # waf sets CXX to "gcc" for both clang and gcc. This function changes the cxx_name-cxx_version combination for clang
    # to match the existing naming scheme used by uplids, which is "gcc-clang".

    # TODO create and send in a patch to allow c_config.get_cc_version to set a variable to indicate that clang is in
    # use

    (cpu_type, cxx, cxx_version) = comp

    if cxx != 'gcc':
        return comp

    cmd = ctx.env.CXX + ['-dM', '-E', '-']
    env = ctx.env.env or None

    try:
        p = Utils.subprocess.Popen(cmd, stdin=Utils.subprocess.PIPE, stdout=Utils.subprocess.PIPE,
                                   stderr=Utils.subprocess.PIPE, env=env)
        p.stdin.write('\n'.encode())
        out = p.communicate()[0]
    except Exception:
        conf.fatal('Could not determine the compiler version %r' % cmd)


    if not isinstance(out, str):
        out = out.decode(sys.stdout.encoding or 'iso8859-1')

    if out.find("__clang__ 1") < 0:
        return comp

    return (cpu_type, 'gcc', 'clang')

def _get_linux_comp(ctx):
    cpu_type = os.uname()[4]
    return (cpu_type, ctx.env.CXX_NAME, '.'.join(ctx.env.CC_VERSION))


def _get_aix_comp(ctx):
    cpu_type = ctx.cmd_and_log(['uname', '-p']).rstrip()
    cxx_name = ctx.env.CXX_NAME
    if cxx_name == 'xlc++':
        cxx_name = 'xlc'

    return (cpu_type, cxx_name, '.'.join(ctx.env.CC_VERSION))


def _get_sunos_comp(ctx):
    cpu_type = ctx.cmd_and_log(['uname', '-p']).rstrip()
    cxx_name = ctx.env.CXX_NAME
    if cxx_name == 'sun':
        cxx_name = 'cc'

    return (cpu_type, cxx_name, '.'.join(ctx.env.CC_VERSION))


def _get_darwin_comp(ctx):
    cpu_type = os.uname()[4]

    cxx_name = ctx.env.CXX_NAME

    return (cpu_type, ctx.env.CXX_NAME, '.'.join(ctx.env.CC_VERSION))


def _get_windows_comp(ctx):
    env=dict(os.environ)
    env.update(PATH=';'.join(ctx.env['PATH']))
    err = ctx.cmd_and_log(ctx.env['CXX'], output=Context.STDERR, env=env)

    m = re.search(r'Compiler Version ([0-9]+\.[0-9]+).*? for (\S*)', err)
    if m:
        compiler = 'cl'
        compilerversion = m.group(1)
        model = m.group(2)
        if model == '80x86':
            cpu_type = 'x86'
        elif model == 'x64':
            cpu_type = 'amd64'
        else:
            cpu_type = model

    return (cpu_type, compiler, compilerversion)


def _add_commandline_options(ctx):
    configure_opts = (
        (('a', 'abi-bits'),
         {'type': 'choice',
          'default': '32',
          'choices': ('32', '64'),
          'help': "32 or 64 [default: %default]"}),
        (('b', 'build-type'),
         {'type': 'choice',
          'default': 'debug',
          'choices': ('release', 'debug'),
          'help': "the type of build to produce: 'debug' or 'release' [default: %default]"}),
        (('t', 'library-type'),
         {'type': 'choice',
          'default': 'static',
          'choices': ('static', 'shared'),
          'help': "the type of libraries to build: 'shared' or 'static' [default: %default]"}),
        (('assert-level',),
         {'type': 'choice',
          'default': 'none',
          'choices': ('none', 'safe', 'safe2'),
          'help': "bsls_assert level: 'none', 'safe' or 'safe2' [default: %default]"}),
        (('noexception',),
         {'action': 'store_true',
          'default': False,
          'help': 'disable exception support'}),
        (('cpp11',),
         {'action': 'store_true',
          'default': False,
          'help': 'enable C++11 support'}),
        (('t', 'legacy-config'),
         {'type': 'string',
          'default': None,
          'help': 'backwards compatible ufid configuration that can be used with bde_build.pl (e.g., dbg_mt_exc)'}),
        (('debug-opt-keys',),
         {'type': 'string',
          'default': None,
          'help': 'debug rules in the opts files for the specified (comma separated) list of opts keys'})
        )

    configure_group = ctx.get_option_group('configure options')
    build_group = ctx.get_option_group('build and install options')


    def add_opts(grp, opts):
        for opt in opts:
            opt_strings = ['-' + a if len(a) == 1 else '--' + a  for a in opt[0]]
            grp.add_option(*opt_strings, **opt[1])

    add_opts(configure_group, configure_opts)
