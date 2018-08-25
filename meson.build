project('bitstream', 'cpp')

local_incdir = include_directories('include')

cpp = meson.get_compiler('cpp')
gtest_dep = dependency('gtest')

compile_args = ['-Wextra', '-std=c++17', '-Werror']

install_headers('include/bitstream.h', 'include/stdbitstream.h')

subdir('test')

tests = executable('bitstream-test', unit_test_cpp_files, include_directories: [local_incdir], dependencies: [gtest_dep], cpp_args: compile_args)


