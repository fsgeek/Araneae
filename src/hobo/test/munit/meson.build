libmunit_sources = ['munit.c', 'munit.h']

deps = [dependency('threads'),
        dependency('uuid'),
        dependency('whitedb'),
        ]

libmunit = library('munit', libmunit_sources, version: meson.project_version(),
                   soversion: '0', include_directories: include_dirs,
                   dependencies: deps, install: false)


pkg = import('pkgconfig')
pkg.generate(libraries: [ '-lpthread',],
             version: meson.project_version(),
             name: 'munit',
             description: 'munit test harness')

libmunit_dep = declare_dependency(include_directories: include_dirs,
                                  link_with: libmunit, 
                                  dependencies: deps)
