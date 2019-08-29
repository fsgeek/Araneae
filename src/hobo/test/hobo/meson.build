deps = [dependency('threads'),
        dependency('uuid'),
        libhobo_dep,
        libmunit_dep,
        dependency('whitedb')
        ]

incdir = include_directories('../munit', '../../lib')

executable('testhobo', 'testhobo.c',
            include_directories : incdir,
            dependencies: deps)
