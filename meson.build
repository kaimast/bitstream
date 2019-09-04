project('bitstream', 'cpp')

local_incdir = include_directories('include')

cpp = meson.get_compiler('cpp')
gtest_dep = dependency('gtest')

compile_args = ['-Wextra', '-std=c++17', '-Werror']

install_headers('include/bitstream.h', 'include/stdbitstream.h')

install_subdir('include/stdbitstream', install_dir: 'include')

subdir('test')

tests = executable('bitstream-test', unit_test_cpp_files, include_directories: [local_incdir], dependencies: [gtest_dep], cpp_args: compile_args)

clangtidy = find_program('clang-tidy', required: false)

if clangtidy.found()
    tidy_files = unit_test_cpp_files

    run_target(
        'tidy',
        command: [
            'scripts/clang-tidy.sh',
            clangtidy.path(),
            meson.source_root(),
            meson.build_root(),
            tidy_files
        ],
    depends: [tests])
endif

