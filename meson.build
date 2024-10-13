project('bitstream', 'cpp')

local_incdir = include_directories('include')

cpp = meson.get_compiler('cpp')
gtest_dep = dependency('gtest')

compile_args = ['-Wextra', '-std=c++17', '-Werror']

install_headers('include/bitstream.h', 'include/stdbitstream.h')

install_subdir('include/stdbitstream', install_dir: 'include')

subdir('test')

unit_tests_bin = executable('bitstream-test', unit_test_cpp_files, include_directories: [local_incdir], dependencies: [gtest_dep], cpp_args: compile_args)

run_target(
    'unit-tests',
    command: unit_tests_bin,
    depends: [unit_tests_bin])

include_files = files('include/bitstream.h', 'include/stdbitstream.h')

clangtidy = find_program('clang-tidy', required: false)

if clangtidy.found()
    run_target(
        'lint',
        command: [
            'scripts/clang-tidy.sh',
            clangtidy.path(),
            meson.source_root(),
            meson.build_root(),
            unit_test_cpp_files + include_files, 
        ])
endif

