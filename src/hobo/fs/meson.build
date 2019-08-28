deps = [dependency('fuse3', version : '>=3.4', required : true),
        dependency('threads'),
        dependency('uuid'),
        libhobo_dep,
        dependency('whitedb')
        ]
        
executable('hobofs', 'hobofs.c',
            include_directories : include_dirs,
            dependencies: deps)
