{
  'targets': [
    {
      'target_name': 'bsl_grpdeps',
      'type': 'none',
      'dependencies': [

      ],
      # XXX: when generating .gyp from bde metadata, need to special-case
      'conditions': [  # (bsls_timeutil uses POSIX realtime API clock_gettime())
        [ 'OS == "linux"', { 'link_settings': { 'libraries': [ '-lrt' ] } } ],
      ],
    },
  ],
}
