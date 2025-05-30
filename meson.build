project('otroff', 'c', default_options : [ 'c_std=c17', 'cpp_std=c++17' ])

    roff_src = files(
        'roff/roff1.c',
        'roff/roff2.c',
        'roff/roff3.c',
        'roff/roff4.c',
        'roff/roff5.c',
        'roff/hyphen_utils.c')

        executable('troff', roff_src, c_args : [ '-Wall', '-O2' ])
