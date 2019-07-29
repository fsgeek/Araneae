
project('tnlinux', 'c')

libfuse_dep = dependency('fuse3')
thread_dep = dependency('threads')

deps = [thread_dep, libfuse_dep]

tnlinux_sources = ['tnlinux.c']

executable('tnlinux', 'tnlinux.c', dependencies: deps)

