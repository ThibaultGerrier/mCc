project('mCc', 'c',
    default_options: [
        'buildtype=debug',
        'c_std=c11',
        'cpp_std=c++14',
        'warning_level=2'
    ],
    meson_version: '>=0.44.0'
)

# ------------------------------------------------------------------ GENERATORS

flex = find_program('flex')
lgen = generator(flex,
                 output: ['@BASENAME@.c', '@BASENAME@.h'],
                 arguments: [ '--outfile=@OUTPUT0@',
                              '--header-file=@OUTPUT1@',
                              '@INPUT@' ])

bison = find_program('bison')
pgen = generator(bison,
                 output: ['@BASENAME@.tab.c', '@BASENAME@.tab.h'],
                 arguments: [ '-Wall',
                              '--report=all',
                              '--output=@OUTPUT0@',
                              '--defines=@OUTPUT1@',
                              '@INPUT@' ])

# --------------------------------------------------------------------- LIBRARY

mCc_inc = include_directories('include')

mCc_src = [ 'src/ast.c',
            'src/ast_print.c',
            'src/ast_visit.c',
            'src/tac.c',
            'src/ast_symbol_table.c',
            'src/ast_type_checking.c',
            'src/error_manager.c',
            'src/symbol_table.c',
            'src/ast_function_return_check.c',
            lgen.process('src/scanner.l'),
            pgen.process('src/parser.y') ]

mCc_lib = library('mCc', mCc_src,
                  c_args: ['-D_POSIX_C_SOURCE=200809L ', '-Wno-unused'],
                  include_directories: mCc_inc)
mCc_lib_test = library('mCc_test', mCc_src,
                  c_args: ['-D_POSIX_C_SOURCE=200809L', '-D_UNIT_TESTS__', '-Wno-unused'],
                  include_directories: mCc_inc)

# ----------------------------------------------------------------- EXECUTABLES

mCc_exes = [ 'mCc', 'mC_to_dot' ]

foreach exe : mCc_exes
    executable(exe, 'src/bin/' + exe + '.c',
               include_directories: mCc_inc,
               link_with: mCc_lib)
endforeach

# ----------------------------------------------------------------- TEST EXEC

mCc_test_exes = [ ['mCc_test', 'mCc'], ['mC_to_dot_test', 'mC_to_dot'] ]

foreach exe : mCc_test_exes
    executable(exe[0], 'src/bin/' + exe[1] + '.c',
               include_directories: mCc_inc,
               link_with: mCc_lib_test)
endforeach

# ------------------------------------------------------------------ UNIT TESTS

gtest = dependency('gtest', fallback: ['gtest', 'gtest_main_dep'])

mCc_uts = [ 'parser', 'symbol_table', 'return_check', 'type_checking', 'tac' ]

foreach ut : mCc_uts
    t = executable('ut_' + ut.underscorify(), 'test/' + ut + '.cpp',
                   include_directories: mCc_inc,
                   link_with: mCc_lib_test,
                   dependencies: gtest)

    test(ut, t)
endforeach

# ------------------------------------------------------------------ BENCHMARKS

mCc_benchs = [ ['parser/binary_op',         '100000'],
               ['parser/nested_expression', '100000'] ]

foreach bench : mCc_benchs
    name = bench[0]
    reps = bench[1]

    b = executable('bench_' + name.underscorify(), 'benchmark/' + name + '.c',
                   include_directories: mCc_inc,
                   link_with: mCc_lib)

    benchmark(name + ': ' + reps, b, args: [reps])
endforeach

# --------------------------------------------------------------------- DOXYGEN

doxygen = find_program('doxygen', required: false)

if doxygen.found()
        doxyconf = configuration_data()
        doxyconf.set('ROOT', meson.source_root())
        doxyconf.set('BUILDDIR', meson.build_root())

        doxyfile = configure_file(input: '.doxyfile',
                                  output: 'doxyfile',
                                  configuration: doxyconf)

        html_doc = run_target('doc', command: [doxygen, doxyfile])
endif
