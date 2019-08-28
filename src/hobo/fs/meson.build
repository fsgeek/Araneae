deps = [dependency('fuse3', version : '>=3.4', required : true),
        dependency('threads'),
        libhobo_dep,
        ]
        
executable('hobofs', 'hobofs.c',
            include_directories : include_dirs,
            dependencies: deps)
