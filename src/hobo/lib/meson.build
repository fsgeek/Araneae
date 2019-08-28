libhobo_sources = ['hobo.c', 'hobo.h' ]

deps = [ thread_dep ]

libfuse = cc.find_library('libfuse', required: true)
deps += [libfuse]

libdl = cc.find_library('dl', required: false)
if libdl.found()
   deps += [ libdl ]
endif

libhobo = library('hobo', libhobo_sources, version: meson.project_version(),
                  soversion: '0', include_directories: include_dirs,
                  dependencies: deps, install: false)

pkg = import('pkgconfig')
pkg.generate(libraries: [ '-lfuse', '-lpthread' ],
             libraries_private: '-ldl',
             version: meson.project_version(),
             name: 'hobo',
             description: 'Hobo Filesystem')

libhobo_dep = declare_dependency(include_directories: include_dirs,
                                 link_with: libhobo, 
                                 dependencies: deps)
