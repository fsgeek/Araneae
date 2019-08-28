libhobo_sources = ['getattr.c',
                   'hobo.c', 'hobo.h',
                   'hobo-list.h',
                   'ino-lookup.c', 'ino-lookup.h',
                   'lookup.c',
                   ]

deps = [dependency('fuse3', version : '>=3.4', required : true),
        dependency('threads'),
        dependency('uuid'),
        ]

libdl = cc.find_library('dl', required: false)
if libdl.found()
   deps += [ libdl ]
endif

libhobo = library('hobo', libhobo_sources, version: meson.project_version(),
                  soversion: '0', include_directories: include_dirs,
                  dependencies: deps, install: false)

pkg = import('pkgconfig')
pkg.generate(libraries: [ '-lfuse', '-lpthread', '-luuid' ],
             libraries_private: '-ldl',
             version: meson.project_version(),
             name: 'hobo',
             description: 'Hobo Filesystem')

libhobo_dep = declare_dependency(include_directories: include_dirs,
                                 link_with: libhobo, 
                                 dependencies: deps)
