{
  'variables': {
    'bsl+bslhdrs_sources': [

    ],
    'bsl+bslhdrs_tests': [

    ],
    'bsl+bslhdrs_tests_paths': [

    ],
    'bsl+bslhdrs_pkgdeps': [
      '../bslscm/bslscm.gyp:bslscm',
      '../bslstl/bslstl.gyp:bslstl',
    ],
  },
  'targets': [
    {
      'target_name': 'bsl+bslhdrs_sources',
      'type': 'none',
      'direct_dependent_settings': {
        'sources': [ '<@(bsl+bslhdrs_sources)' ],
        'include_dirs': [ '.' ],
      },
    },
    {
      'target_name': 'bsl+bslhdrs_tests_build',
      'type': 'none',
      'dependencies': [ '<@(bsl+bslhdrs_tests)' ],
    },
    {
      'target_name': 'bsl+bslhdrs_tests_run',
      'type': 'none',
      'dependencies': [ 'bsl+bslhdrs_tests_build' ],
      'sources': [ '<@(bsl+bslhdrs_tests_paths)' ],
      'rules': [
        {
          'rule_name': 'run_unit_tests',
          'extension': 't',
          'inputs': [ '<@(bsl+bslhdrs_tests_paths)' ],
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
      'target_name': 'bsl+bslhdrs',
      'type': 'none',
      'dependencies': [ '../bsl_deps.gyp:bsl_grpdeps',
                        '<@(bsl+bslhdrs_pkgdeps)',
                        'bsl+bslhdrs_sources', ],
      'export_dependent_settings': [ '<@(bsl+bslhdrs_pkgdeps)' ],
      'direct_dependent_settings': { 'include_dirs': [ '.' ] },
      # Mac OS X empty LD_DYLIB_INSTALL_NAME causes executable and shared
      # libraries linking against dylib to store same path for use at runtime
      'xcode_settings': { 'LD_DYLIB_INSTALL_NAME': '' },
    },

  ],
}
