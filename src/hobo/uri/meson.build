
prog_python = find_program('python3')

r = run_command(prog_python, 'processuri.py', '--useall')
if r.returncode() != 0
    error('could not generate urischemes.[ch]')
endif

urifiles = ['urischemes.c', 'urischemes.h']

liburischeme = static_library('urischeme', urifiles)
