{
  'variables': {
    'bsl+stdhdrs_sources': [

    ],
    'bsl+stdhdrs_tests': [

    ],
    'bsl+stdhdrs_tests_paths': [

    ],
    'bsl+stdhdrs_pkgdeps': [
      '../bsl+bslhdrs/bsl+bslhdrs.gyp:bsl+bslhdrs',
      '../bslscm/bslscm.gyp:bslscm',
      '../bslstl/bslstl.gyp:bslstl',
    ],
  },
  'targets': [
    {
      'target_name': 'bsl+stdhdrs_sources',
      'type': 'none',
      'direct_dependent_settings': {
        'sources': [ '<@(bsl+stdhdrs_sources)' ],
      },
      # one-off workaround bug in Sun Studio C++ requiring abs path to .SUNWCCh
      'conditions': [
        [ 'OS == "solaris"',
          { 'direct_dependent_settings': { 'include_dirs': [ '<!(pwd)' ] } },
          { 'direct_dependent_settings': { 'include_dirs': [ '.' ] } } ],
      ],
    },
    {
      'target_name': 'bsl+stdhdrs_tests_build',
      'type': 'none',
      'dependencies': [ '<@(bsl+stdhdrs_tests)' ],
    },
    {
      'target_name': 'bsl+stdhdrs_tests_run',
      'type': 'none',
      'dependencies': [ 'bsl+stdhdrs_tests_build' ],
      'sources': [ '<@(bsl+stdhdrs_tests_paths)' ],
      'rules': [
        {
          'rule_name': 'run_unit_tests',
          'extension': 't',
          'inputs': [ '<@(bsl+stdhdrs_tests_paths)' ],
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
      'target_name': 'bsl+stdhdrs',
      'type': 'none',
      'dependencies': [ '../bsl_deps.gyp:bsl_grpdeps',
                        '<@(bsl+stdhdrs_pkgdeps)',
                        'bsl+stdhdrs_sources', ],
      'export_dependent_settings': [ '<@(bsl+stdhdrs_pkgdeps)' ],
      # one-off workaround bug in Sun Studio C++ requiring abs path to .SUNWCCh
      'conditions': [
        [ 'OS == "solaris"',
          { 'direct_dependent_settings': { 'include_dirs': [ '<!(pwd)' ] } },
          { 'direct_dependent_settings': { 'include_dirs': [ '.' ] } } ],
      ],
      # Mac OS X empty LD_DYLIB_INSTALL_NAME causes executable and shared
      # libraries linking against dylib to store same path for use at runtime
      'xcode_settings': { 'LD_DYLIB_INSTALL_NAME': '' },
    },

  ],
}
