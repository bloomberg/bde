{
  'variables': {
    'bslscm_sources': [
      'bslscm_version.cpp',
      'bslscm_versiontag.cpp',
    ],
    'bslscm_tests': [
      'bslscm_version.t',
      'bslscm_versiontag.t',
    ],
    'bslscm_tests_paths': [
      '<(PRODUCT_DIR)/bslscm_version.t',
      '<(PRODUCT_DIR)/bslscm_versiontag.t',
    ],
    'bslscm_pkgdeps': [
      '../bsls/bsls.gyp:bsls',
    ],
  },
  'targets': [
    {
      'target_name': 'bslscm_sources',
      'type': 'none',
      'direct_dependent_settings': {
        'sources': [ '<@(bslscm_sources)' ],
        'include_dirs': [ '.' ],
      },
    },
    {
      'target_name': 'bslscm_tests_build',
      'type': 'none',
      'dependencies': [ '<@(bslscm_tests)' ],
    },
    {
      'target_name': 'bslscm_tests_run',
      'type': 'none',
      'dependencies': [ 'bslscm_tests_build' ],
      'sources': [ '<@(bslscm_tests_paths)' ],
      'rules': [
        {
          'rule_name': 'run_unit_tests',
          'extension': 't',
          'inputs': [ '<@(bslscm_tests_paths)' ],
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
      'target_name': 'bslscm',
      'type': '<(library)',
      'dependencies': [ '../bsl_deps.gyp:bsl_grpdeps',
                        '<@(bslscm_pkgdeps)',
                        'bslscm_sources', ],
      'export_dependent_settings': [ '<@(bslscm_pkgdeps)' ],
      'direct_dependent_settings': { 'include_dirs': [ '.' ] },
      # Mac OS X empty LD_DYLIB_INSTALL_NAME causes executable and shared
      # libraries linking against dylib to store same path for use at runtime
      'xcode_settings': { 'LD_DYLIB_INSTALL_NAME': '' },
    },

    {
      'target_name': 'bslscm_version.t',
      'type': 'executable',
      'dependencies': [ '../bsl_deps.gyp:bsl_grpdeps',
                        '<@(bslscm_pkgdeps)', 'bslscm' ],
      'include_dirs': [ '.' ],
      'sources': [ 'bslscm_version.t.cpp' ],
    },
    {
      'target_name': 'bslscm_versiontag.t',
      'type': 'executable',
      'dependencies': [ '../bsl_deps.gyp:bsl_grpdeps',
                        '<@(bslscm_pkgdeps)', 'bslscm' ],
      'include_dirs': [ '.' ],
      'sources': [ 'bslscm_versiontag.t.cpp' ],
    },
  ],
}
