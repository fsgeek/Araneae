libhobo_sources = [
                   'access.c',
                   'destroy.c',
                   'getattr.c',
                   'hobo.c', 'hobo.h',
                   'hobo-list.h',
                   'hobodb.c', 'hobodb.h',
                   'init.c',
                   'ino-lookup.c', 'ino-lookup.h',
                   'lookup.c',
                   'open.c',
                   'opendir.c',
                   'readdir.c',
                   'releasedir.c',
                   ]

deps = [dependency('fuse3', version : '>=3.4', required : true),
        dependency('threads'),
        dependency('uuid'),
        dependency('whitedb'),
        ]

libdl = cc.find_library('dl', required: false)
if libdl.found()
   deps += [ libdl ]
endif

libhobo = library('hobo', libhobo_sources, version: meson.project_version(),
                  soversion: '0', include_directories: include_dirs,
                  c_args : '-fms-extensions',
                  dependencies: deps, install: false)

pkg = import('pkgconfig')
pkg.generate(libraries: [ '-lfuse', '-lpthread', '-luuid' , '-lwhitedb'],
             libraries_private: '-ldl',
             version: meson.project_version(),
             name: 'hobo',
             description: 'Hobo Filesystem')

libhobo_dep = declare_dependency(include_directories: include_dirs,
                                 link_with: libhobo, 
                                 dependencies: deps)
