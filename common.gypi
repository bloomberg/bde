{
    # - nesting 'variables' sections; see chromium/src/build/common.gypi
    # - suffix '%' provides default values if variable not already set
    'variables': {
     'variables': {
      'variables': {
        'variables': {
          'conditions': [
            # default to GNU gcc/ld if not Windows (and not otherwise set below)
            [ 'OS == "win"',      { 'library%':      'static_library',
                                    'compiler_tag%': 'msvs',
                                    'linker_tag%':   'msvs' },
                                  { 'library%':      'static_library',
                                    'compiler_tag%': 'gcc',
                                    'linker_tag%':   'gnu_ld' } ],
            [ 'OS == "solaris"',  { 'compiler_tag%': 'sunstudio',
                                    'linker_tag%':   'solaris_ld' } ],
            [ 'OS == "aix"',      { 'compiler_tag%': 'xlC',
                                    'linker_tag%':   'aix_ld' } ],
          ],
          'defines_base': [ '_REENTRANT',          # thread-aware (not optional)
                            '_THREAD_SAFE', ],     # thread-aware (not optional)
          'ABI_bits%': 64,                         # default to 64-bit build
          'dbg_safe%': 0,                          # default dbg safe mode off
        },
        # copy variables (including conditionally-set variables) out one scope
        'library%': '<(library)',
        'compiler_tag%': '<(compiler_tag)',
        'linker_tag%': '<(linker_tag)',
        'defines_base': [ '<@(defines_base)', ],
        'ABI_bits%': '<(ABI_bits)',
        'dbg_safe%': '<(dbg_safe)',

        'conditions': [
          # (option: large conditions might be split out to separate includes,
          #          e.g. [ 'xxx == "value"', { 'includes': [ '...' ] } ]  )
          # (additional compiler- and linker-specific flags can be found in
          #  tools/gyp/pylib/gyp/generator/make.py)

          # GNU gcc
          [ 'compiler_tag == "gcc"', {
            'cflags_base%': [ '-pipe', # uncomment -std= to specify compiler std
                             #'-std=c++0x', # gcc 4.6
                             #'-std=c++11', # gcc 4.7
                              '-pthread', ],       # thread aware (not optional)
            'cflags_opt%': [ '-O2', '-fno-strict-aliasing', ],
            'cflags_c_debug%':  [ '-g' ],
            'cflags_cc_debug%': [ '-g' ],
            'cflags_cc_exc%': [ '-fexceptions', ],
            'cflags_c_extra%':  [ ],
            'cflags_cc_extra%': [ ],
            'cflags_diags%': [ '-Wall', '-Wextra',
                               '-Wcast-align', '-Wcast-qual',
                               '-Wformat-security', '-Wformat-y2k',
                               '-Winit-self', '-Wno-long-long',
                               '-Wno-unknown-pragmas', '-Wpacked',
                               '-Wpointer-arith', '-Wwrite-strings',
                               '-Wno-unused-value', '-Wno-char-subscripts',
                               '-Wsign-compare', '-Wparentheses',
                               '-Wvolatile-register-var',
                               '-fdiagnostics-show-option',
                               # requires gcc 4.3 (RHEL 5 ships 4.1.2)
                               #'-Wlogical-op', '-Wstrict-overflow', '-Wvla',
                             ],
            'conditions': [
              # require at least 686 instruction set on IA-32 x86 systems
              [ 'ABI_bits == 32 and OS != "win" and "<!(uname -m|sed \"s/i.86/x86/\"|sed \"s/x86_64/x86/\")"=="x86"',
                               { 'cflags_abi_32%': [ '-m32 -march=i686' ], } ],
              # set 32-bit or 64-bit compilation flags
              [ 'OS != "aix"', { 'cflags_abi_32%': [ '-m32' ],
                                 'cflags_abi_64%': [ '-m64' ], },
                               { 'cflags_abi_32%': [ '-maix32' ],
                                 'cflags_abi_64%': [ '-maix64' ], } ],
            ],
            'cflags_PIC%': [ '-fPIC' ],
          }], # compiler_tag == "gcc"


          [ 'linker_tag == "gnu_ld"', {
            'ldflags_base%': [ '-pthread',         # thread aware (not optional)
                               # linker optimizations (requires newer GNU ld)
                               # (info: 'man ld')
                               #'-Wl,-O1,--hash-style=both',
                               #'-Wl,--as-needed',
                               # (compile -fdata-sections -ffunction-sections)
                               #'-Wl,--gc-sections', 
                               '-Wl,-z,noexecstack', ],
            'ldflags_abi_32%': [ '-m32' ],
            'ldflags_abi_64%': [ '-m64' ],
            'ldflags_PIC%': [ '-fPIC' ],
          }], # linker_tag == "gnu_ld"


          # IBM Visual Age xlC
          [ 'compiler_tag == "xlC"', {
            'defines_base': [ '__unix',
                              '__VACPP_MULTI__',   # thread aware (or use xlC_r)
                              # (optional) see "xlC streams optimization" below
                              #'__NOLOCK_ON_INPUT', '__NOLOCK_ON_OUTPUT',
                            ],
            'cflags_base%': [#'-qlanglvl=extc1x',#uncomment to spec compiler std
                              '-qthreaded',        # thread aware (not optional)
                              '-qfuncsect', ], 
            'cflags_opt%': [ '-O2' ],
            'cflags_c_debug%':  [ '-g' ], #(XXX: debug+inlining is complicated)
            'cflags_cc_debug%': [ '-g' ], #(XXX: debug+inlining is complicated)
            'cflags_cc_exc%': [ '-qeh', '-qlanglvl=newexcp', ],
            'cflags_c_extra%':  [ ],
            'cflags_cc_extra%': [ '-qrtti=all', ],
            'cflags_diags%': [ ],  # XXX: TODO warnings/diagnostics
            'cflags_abi_32%': [ '-q32', '-qdebug=nparseasm' ],
            'cflags_abi_64%': [ '-q64' ],
            'cflags_PIC%': [ '-qpic=large' ],
          }], # compiler_tag == "xlC"


          [ 'linker_tag == "aix_ld"', {
            'ldflags_base%': [ '-qthreaded',       # thread aware (not optional)
                               '-qtwolink',
                               '-bweaklocal' ],    # linker opt; 'man ld'
            'ldflags_abi_32%': [ '-q32 -b32 -brtl' ],
            'ldflags_abi_64%': [ '-q64 -b64 -brtl' ],
            'ldflags_PIC%': [ ],
          }], # linker_tag == "aix_ld"


          # Oracle Sun Studio
          [ 'compiler_tag == "sunstudio"', {
            'cflags_base%': [ '-mt', ],            # thread aware (not optional)
            'cflags_opt%': [ '-O' ],
            'cflags_c_debug%':  [ '-g' ],
            'cflags_cc_debug%': [ '-g0' ],         # debug and inlining
            'cflags_cc_exc%': [ '-features=except', ],
            'cflags_c_extra%':  [ ],
            'cflags_cc_extra%': [ ],
            'cflags_diags%': [ ],  # XXX: TODO warnings/diagnostics
            'cflags_abi_32%': [ '-m32' ],
            'cflags_abi_64%': [ '-m64' ],
            'cflags_PIC%': [ '-KPIC' ],
          }], # compiler_tag == "sunstudio"


          [ 'linker_tag == "solaris_ld"', {
            'ldflags_base%': [ '-mt', ],           # thread aware (not optional)
            'ldflags_abi_32%': [ '-m32' ],
            'ldflags_abi_64%': [ '-m64' ],
            'ldflags_PIC%': [ ],
          }], # linker_tag == "solaris_ld"


          # MS Visual Studio
          # XXX: ??? do we need /MT for linking, too ???
          # XXX: ??? if so, need to fix configs and conditions further below ???
          # (FYI: must omit /RTC1 for optimized debug since /RTC1 disables opt)
          [ 'compiler_tag == "msvs"', {
            'defines_base': [ 'BDE_HIDE_COMMON_WINDOWS_WARNINGS', #BDE-specific
                              'NOGDI', 'NOMINMAX',
                              '_CRT_SECURE_NO_DEPRECATE',
                              '_SCL_SECURE_NO_DEPRECATE', ],
            'cflags_base%': [ '/nologo',
                              '/MT',               # thread aware (not optional)
                              # Note: consumer must use /Za with /Oi if
                              # required for for floating point precision
                              '/Oi',               # generate intrinsic funcs
                            ],
            # http://msdn.microsoft.com/en-us/library/1deeycx5%28v=vs.80%29.aspx
            'cflags_opt%': [ '/O2','/Ob1','/GS-','/GT', ],
            'cflags_c_debug%':  [ '/Zi', '/RTC1' ],
            'cflags_cc_debug%': [ '/Zi', '/RTC1' ],
            'cflags_cc_exc%': [ '/EHsc', ],     # /EH (Exception Handling Model)
            'cflags_c_extra%':  [ ],
            'cflags_cc_extra%': [ ],
            'cflags_diags%': [ ],  # XXX: TODO warnings/diagnostics
            'cflags_abi_32%': [ ],
            'cflags_abi_64%': [ ],
            'cflags_PIC%':  [ ],
          }], # compiler_tag == "msvs"


          [ 'linker_tag == "msvs"', {
            'ldflags_base%': [ '/MT', ],           # thread aware (not optional)
            'ldflags_abi_32%': [ ],
            'ldflags_abi_64%': [ ],
            'ldflags_PIC%': [ ],
          }], # linker_tag == "msvs"


          [ 'OS == "solaris"', {
            'defines_base': [ '_POSIX_PTHREAD_SEMANTICS', ],
          }],


          [ 'OS == "mac"', {
            'defines_base': [
              '__unix',              # xcodebuild/clang does not define __unix
              '__CLANG_GNUC__=4',
              '__CLANG_GNUC_MINOR__=2',
              '__CLANG_GNUC_PATCHLEVEL__=1',
            ],
            'cflags_abi_32=': [ 'i386' ],
            'cflags_abi_64=': [ 'x86_64' ],
          }],

        ],
      },
      # copy variables (including conditionally-set variables) out one scope
      'library%': '<(library)',
      'ABI_bits%': '<(ABI_bits)',
      'dbg_safe%': '<(dbg_safe)',
      'compiler_tag%': '<(compiler_tag)',
      'linker_tag%': '<(linker_tag)',
      'defines_base': [ '<@(defines_base)' ],
      'cflags_base%':  [ '<@(cflags_base)' ],
      'ldflags_base%': [ '<@(ldflags_base)' ],
      'cflags_opt%':   [ '<@(cflags_opt)' ],
      'cflags_c_debug%': [ '<@(cflags_c_debug)' ],
      'cflags_cc_debug%': [ '<@(cflags_cc_debug)' ],
      'cflags_cc_exc%': [ '<@(cflags_cc_exc)' ],
      'cflags_c_extra%': [ '<@(cflags_c_extra)' ],
      'cflags_cc_extra%': [ '<@(cflags_cc_extra)' ],
      'cflags_diags%': [ '<@(cflags_diags)' ],
      'cflags_abi_32%': [ '<@(cflags_abi_32)' ],
      'cflags_abi_64%': [ '<@(cflags_abi_64)' ],
      'ldflags_abi_32%': [ '<@(ldflags_abi_32)' ],
      'ldflags_abi_64%': [ '<@(ldflags_abi_64)' ],
      'cflags_PIC%':  [ '<@(cflags_PIC)' ],
      'ldflags_PIC%': [ '<@(ldflags_PIC)' ],

      'conditions': [
        [ 'ABI_bits == 64', {
          'cflags_abi%':  [ '<@(cflags_abi_64)' ],
          'ldflags_abi%': [ '<@(ldflags_abi_64)' ],
        }, { # ABI_bits == 32
          'cflags_abi%':  [ '<@(cflags_abi_32)' ],
          'ldflags_abi%': [ '<@(ldflags_abi_32)' ],
        }],
      ],

     },
     # copy variables (including conditionally-set variables) out one scope
     'library%': '<(library)',
     'ABI_bits%': '<(ABI_bits)',
     'dbg_safe%': '<(dbg_safe)',
     'compiler_tag%': '<(compiler_tag)',
     'linker_tag%': '<(linker_tag)',
     'defines_base': [ '<@(defines_base)' ],
     'cflags_base%':  [ '<@(cflags_base)' ],
     'ldflags_base%': [ '<@(ldflags_base)' ],
     'cflags_opt%':   [ '<@(cflags_opt)' ],
     'cflags_c_debug%': [ '<@(cflags_c_debug)' ],
     'cflags_cc_debug%': [ '<@(cflags_cc_debug)' ],
     'cflags_cc_exc%': [ '<@(cflags_cc_exc)' ],
     'cflags_c_extra%': [ '<@(cflags_c_extra)' ],
     'cflags_cc_extra%': [ '<@(cflags_cc_extra)' ],
     'cflags_diags%': [ '<@(cflags_diags)' ],
     'cflags_abi%':  [ '<@(cflags_abi)' ],
     'ldflags_abi%': [ '<@(ldflags_abi)' ],
     'cflags_PIC%':  [ '<@(cflags_PIC)' ],
     'ldflags_PIC%': [ '<@(ldflags_PIC)' ],

    }, # variables

    'target_defaults': {
        'default_configuration': 'Release',
        'defines': [
            '<@(defines_base)',
            'BDE_BUILD_TARGET_EXC',
            'BDE_BUILD_TARGET_MT',
        ],
        'configurations': {
            # (as needed) define cflags_c, cflags_cc, cflags_objc, cflags_objcc
            #  (in addition to cflags) for flags specific to C or C++, not both
            'Release': { 'defines':   [ 'BDE_BUILD_TARGET_OPT',
                                        'NDEBUG', ], # no-op <assert.h>
                         'cflags':    [ '<@(cflags_abi)',
                                        '<@(cflags_base)',
                                        '<@(cflags_opt)',
                                        '<@(cflags_diags)', ],
                         'cflags_c':  [ '<@(cflags_c_extra)', ],
                         'cflags_cc': [ '<@(cflags_cc_exc)',
                                        '<@(cflags_cc_extra)', ],
                         'ldflags':   [ '<@(ldflags_abi)',
                                        '<@(ldflags_base)' ],
                         'msvs_settings': {
                           'VCCLCompilerTool': {
                             'Optimization': '2',
                             'PreprocessorDefinitions': ['NDEBUG'],
                           },
                         },
                       },
            'Debug':   { 'defines':   [ 'BDE_BUILD_TARGET_DBG', ],
                         'cflags':    [ '<@(cflags_abi)',
                                        '<@(cflags_base)',
                                        '<@(cflags_diags)', ],
                         'cflags_c':  [ '<@(cflags_c_debug)',
                                        '<@(cflags_c_extra)', ],
                         'cflags_cc': [ '<@(cflags_cc_exc)',
                                        '<@(cflags_cc_debug)',
                                        '<@(cflags_cc_extra)', ],
                         'ldflags':   [ '<@(ldflags_abi)',
                                        '<@(ldflags_base)' ],
                         'msvs_settings': {
                           'VCCLCompilerTool': {
                             'Optimization': '0',
                           },
                         },
                       },
        },

        'conditions': [

            [ 'dbg_safe == 1', {
              'defines': [ 'BDE_BUILD_TARGET_SAFE',
                           'BDE_DONT_ALLOW_TRANSITIVE_INCLUDES', ],
            }],

            [ 'OS == "mac"', {
                'configurations': {
                    'Release': {
                        'xcode_settings': { 
                          'GCC_OPTIMIZATION_LEVEL': '2',
                          'GCC_GENERATE_DEBUGGING_SYMBOLS': 'NO',
                        },
                    },
                    'Debug': {
                        'xcode_settings': {
                          'GCC_OPTIMIZATION_LEVEL': '0',
                          'GCC_GENERATE_DEBUGGING_SYMBOLS': 'YES',
                          'STRIP_INSTALLED_PRODUCT': 'NO',
                        },
                    },
                },
                'xcode_settings': {
                  # This build system is not yet capable of producing fat binaries, so only
                  # one architecture will be set in ARCHS below.
                  'ARCHS': [ '<@(cflags_abi)', ],
                  'INSTALL_PATH': '@rpath',
                  'WARNING_CFLAGS': [ '<@(cflags_diags)' ],
                  'GCC_ENABLE_CPP_RTTI': 'YES',
                  # omit -fvisibility=hidden
                  'GCC_INLINES_ARE_PRIVATE_EXTERN': 'NO',
                  'GCC_SYMBOLS_PRIVATE_EXTERN': 'NO',
                },
            }],
            [ 'compiler_tag == "msvs"', {
            # http://msdn.microsoft.com/en-us/library/1deeycx5%28v=vs.80%29.aspx
              'cflags_cc': [ '/EHsc', ],  # /EH (Exception Handling Model)
              'conditions': [
                [ 'library == "shared_library"', {
                  'configurations': {
                    # http://msdn.microsoft.com/en-us/library/2kzt1wy3.aspx
                    'Release': {
                      'cflags_base!': [ '/MT', ],  # thread aware (not optional)
                      'cflags_base':  [ '/MD', ],  # thread aware (not optional)
                    },
                    'Debug': {
                      'cflags_base!': [ '/MT', ],  # thread aware (not optional)
                      'cflags_base':  [ '/MDd', ], # thread aware (not optional)
                    },
                  },
                }, { # library == "static_library"
                  'configurations': {
                    'Debug': {
                      'cflags_base!': [ '/MT', ],  # thread aware (not optional)
                      'cflags_base':  [ '/MTd', ], # thread aware (not optional)
                    },
                  },
                }],
              ],
            }],
            [ 'linker_tag == "msvs"', {
              'configurations': {
                'Debug': { 'ldflags_base': [ '/debug', ], },
              },
            }],
        ],

        'target_conditions': [
            [ '_type == "shared_library"', {
              'cflags':  [ '<@(cflags_PIC)' ],
              'ldflags': [ '<@(ldflags_PIC)' ],
              'conditions': [
                [ 'OS == "win"', { 'defines': [ '_USRDLL', '_WINDLL', ], } ],
              ],
            }],
        ],

    }, # target_defaults

    'conditions': [

      # (start paths with $(empty) to trick make.gyp to not $(abspath ...) it)
      # (different compilers require mods in gyp/pylib/gyp/generator/make.py)
      # (see gyp/pylib/gyp/generator/* for tokens affecting gyp make output)

      # GNU gcc
      [ 'compiler_tag == "gcc"', {
        'make_global_settings': [
          ['CC', '$(empty)gcc'],
          ['CXX', '$(empty)g++'],
          ['LINK', '$(CXX)'],
          ['CC.host', '$(CC)'],
          ['CXX.host', '$(CXX)'],
          ['LINK.host', '$(LINK)'],
        ],
      }], # compiler_tag == "gcc"

      # IBM Visual Age xlC
      [ 'compiler_tag == "xlC"', {
        'make_global_settings': [
          ['CC', '$(empty)xlc_r'],
          ['CXX', '$(empty)xlC_r'],
          ['LINK', '$(CXX)'],
          ['CC.host', '$(CC)'],
          ['CXX.host', '$(CXX)'],
          ['LINK.host', '$(LINK)'],
          # (AIX ar 32/64 compat; alternatively, set ar -X64 only for 64-bit)
          ['ARFLAGS.host', '$(empty)-X32_64 -crsT' ],
          ['ARFLAGS.target', '$(empty)-X32_64 -crsT' ],
        ],
      }], # compiler_tag == "xlC"

      # Oracle Sun Studio
      [ 'compiler_tag == "sunstudio"', {
        'make_global_settings': [
          ['CC', '$(empty)cc'],
          ['CXX', '$(empty)CC'],
          ['LINK', '$(CXX)'],
          ['CC.host', '$(CC)'],
          ['CXX.host', '$(CXX)'],
          ['LINK.host', '$(LINK)'],
        ],
      }], # compiler_tag == "sunstudio"

    ], # conditions
}

# Note: xlC streams optimization
# http://pic.dhe.ibm.com/infocenter/comphelp/v121v141/index.jsp?topic=%2Fcom.ibm.xlcpp121.aix.doc%2Fproguide%2Fthreadsafe_streams.html&resultof%3D%2522%255f%255f%254e%254f%254c%254f%2543%254b%255f%254f%254e%255f%2549%254e%2550%2555%2554%2522%2520
# Ensuring thread safety of stream objects
# "... However, if you disable locking on input or output objects, you must provide the appropriate locking mechanisms in your source code if stream objects are shared between threads. If you do not, the behavior is undefined, with the possibility of data corruption or application crash. ..."

# Warning: when building on AIX on NFS, AIX still has problem with race mkdir
#   and failing even when directory successfully created with mkdir -p when
#   there is lots of parallelism.  Just re-execute build and it will continue.
#   Rinse and repeat.  Better: build on local (non-NFS) volume.
