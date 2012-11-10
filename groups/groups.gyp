{
    'targets': [
        {
            'target_name': 'groups_all',
            'type': 'none',
            'dependencies': [ 'groups_all_libs',
                              'groups_all_tests_build',
                              'groups_all_tests_run' ],
        },
        {
            'target_name': 'groups_all_libs',
            'type': 'none',
            'dependencies': [ 'bsl/bsl.gyp:bsl' ],
        },
        {
            'target_name': 'groups_all_tests_build',
            'type': 'none',
            'dependencies': [ 'bsl/bsl.gyp:bsl_tests_build' ],
        },
        {
            'target_name': 'groups_all_tests_run',
            'type': 'none',
            'dependencies': [ 'bsl/bsl.gyp:bsl_tests_run' ],
        },
    ]
}
