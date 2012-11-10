{
  'variables': {
    'bslim_sources': [
      'bslim_printer.cpp',
    ],
    'bslim_tests': [
      'bslim_printer.t',
    ],
    'bslim_tests_paths': [
      '<(PRODUCT_DIR)/bslim_printer.t',
    ],
    'bslim_pkgdeps': [
      '../bsl+bslhdrs/bsl+bslhdrs.gyp:bsl+bslhdrs',
      '../bsl+stdhdrs/bsl+stdhdrs.gyp:bsl+stdhdrs',
      '../bslalg/bslalg.gyp:bslalg',
      '../bslma/bslma.gyp:bslma',
      '../bslmf/bslmf.gyp:bslmf',
      '../bsls/bsls.gyp:bsls',
      '../bslscm/bslscm.gyp:bslscm',
      '../bslstl/bslstl.gyp:bslstl',
    ],
  },
  'targets': [
    {
      'target_name': 'bslim_sources',
      'type': 'none',
      'direct_dependent_settings': {
        'sources': [ '<@(bslim_sources)' ],
        'include_dirs': [ '.' ],
      },
    },
    {
      'target_name': 'bslim_tests_build',
      'type': 'none',
      'dependencies': [ '<@(bslim_tests)' ],
    },
    {
      'target_name': 'bslim_tests_run',
      'type': 'none',
      'dependencies': [ 'bslim_tests_build' ],
      'sources': [ '<@(bslim_tests_paths)' ],
      'rules': [
        {
          'rule_name': 'run_unit_tests',
          'extension': 't',
          'inputs': [ '<@(bslim_tests_paths)' ],
          'outputs': [ '<(INTERMEDIATE_DIR)/<(RULE_INPUT_ROOT).t.ran' ],
          'action': [ '<(python_path)', '<(DEPTH)/tools/run_unit_tests.py',
                      '<(RULE_INPUT_PATH)',
                      '<@(_outputs)',
                      '--abi=<(ABI_bits)',
                      '--lib=<(library)'
          ],
          'msvs_cygwin_shell': 0,
        },
      ],
    },
    {
      'target_name': 'bslim',
      'type': '<(library)',
      'dependencies': [ '../bsl_deps.gyp:bsl_grpdeps',
                        '<@(bslim_pkgdeps)',
                        'bslim_sources', ],
      'export_dependent_settings': [ '<@(bslim_pkgdeps)' ],
      'direct_dependent_settings': { 'include_dirs': [ '.' ] },
      # Mac OS X empty LD_DYLIB_INSTALL_NAME causes executable and shared
      # libraries linking against dylib to store same path for use at runtime
      'xcode_settings': { 'LD_DYLIB_INSTALL_NAME': '' },
    },

    {
      'target_name': 'bslim_printer.t',
      'type': 'executable',
      'dependencies': [ '../bsl_deps.gyp:bsl_grpdeps',
                        '<@(bslim_pkgdeps)', 'bslim' ],
      'include_dirs': [ '.' ],
      'sources': [ 'bslim_printer.t.cpp' ],
    },
  ],
}
