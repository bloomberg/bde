{
    'targets': [
        {
            'target_name': 'all',
            'type': 'none',
            'dependencies': [ 'all_libs', 'all_tests_build', 'all_tests_run' ],
        },
        {
            'target_name': 'all_libs',
            'type': 'none',
            'dependencies': [ 'groups/groups.gyp:groups_all_libs' ],
        },
        {
            'target_name': 'all_tests_build',
            'type': 'none',
            'dependencies': [ 'groups/groups.gyp:groups_all_tests_build' ],
        },
        {
            'target_name': 'all_tests_run',
            'type': 'none',
            'dependencies': [ 'groups/groups.gyp:groups_all_tests_run' ],
        },
    ]
}
