{
  'variables': {
    'bsldoc_sources': [
      'bsldoc_glossary.cpp',
    ],
    'bsldoc_tests': [
      'bsldoc_glossary.t',
    ],
    'bsldoc_tests_paths': [
      '<(PRODUCT_DIR)/bsldoc_glossary.t',
    ],
    'bsldoc_pkgdeps': [
      '../bsls/bsls.gyp:bsls',
      '../bslscm/bslscm.gyp:bslscm',
    ],
  },
  'targets': [
    {
      'target_name': 'bsldoc_sources',
      'type': 'none',
      'direct_dependent_settings': {
        'sources': [ '<@(bsldoc_sources)' ],
        'include_dirs': [ '.' ],
      },
    },
    {
      'target_name': 'bsldoc_tests_build',
      'type': 'none',
      'dependencies': [ '<@(bsldoc_tests)' ],
    },
    {
      'target_name': 'bsldoc_tests_run',
      'type': 'none',
      'dependencies': [ 'bsldoc_tests_build' ],
      'sources': [ '<@(bsldoc_tests_paths)' ],
      'rules': [
        {
          'rule_name': 'run_unit_tests',
          'extension': 't',
          'inputs': [ '<@(bsldoc_tests_paths)' ],
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
      'target_name': 'bsldoc',
      'type': '<(library)',
      'dependencies': [ '../bsl_deps.gyp:bsl_grpdeps',
                        '<@(bsldoc_pkgdeps)',
                        'bsldoc_sources', ],
      'export_dependent_settings': [ '<@(bsldoc_pkgdeps)' ],
      'direct_dependent_settings': { 'include_dirs': [ '.' ] },
      # Mac OS X empty LD_DYLIB_INSTALL_NAME causes executable and shared
      # libraries linking against dylib to store same path for use at runtime
      'xcode_settings': { 'LD_DYLIB_INSTALL_NAME': '' },
    },

    {
      'target_name': 'bsldoc_glossary.t',
      'type': 'executable',
      'dependencies': [ '../bsl_deps.gyp:bsl_grpdeps',
                        '<@(bsldoc_pkgdeps)', 'bsldoc' ],
      'include_dirs': [ '.' ],
      'sources': [ 'bsldoc_glossary.t.cpp' ],
    },
  ],
}
