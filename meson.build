project(
  'vendr', 'cpp',
  meson_version: '>= 1.6.1',
  default_options: ['cpp_std=c++23'],
  license: 'GPL-3.0-or-later',
  license_files: 'LICENSE',
  version: files('meson.version')
)

config_h = configuration_data()
cxx = meson.get_compiler('cpp')

if cxx.get_id() == 'clang'
  add_project_arguments('-stdlib=libc++', language: 'cpp')
  add_project_link_arguments('-lc++', '-lc++abi', language: 'cpp')
endif

vendored_inc_dir = 'deps/stage/include'
vendored_lib_dir = 'deps/stage/lib'

if get_option('boost-po-static')
  boost_dep = cxx.find_library(
    'boost_program_options',
    required: true,
    static: true)
else
  boost_dep = dependency(
    'boost',
    method: 'cmake',
    modules: ['program_options'],
    required: true,
    static: false)
endif

toml_dep = dependency(
  'tomlplusplus',
  required: true,
  static: get_option('tomlpp-static'))

libgit2_dep = dependency(
    'libgit2',
    required: true,
    static: get_option('libgit2-static'))

zlib_dep = dependency('zlib', static: get_option('zlib-static'))
mbedtls_dep = dependency('mbedtls', static: get_option('mbedtls-static'))
mbedcrypto_dep = dependency('mbedcrypto', static: get_option('mbedcrypto-static'))
mbedx509_dep = dependency('mbedx509', static: get_option('mbedx509-static'))

bindeps = [
  boost_dep,
  toml_dep,
  libgit2_dep
]

if get_option('libgit2-static')
  bindeps += [zlib_dep, mbedtls_dep, mbedcrypto_dep, mbedx509_dep]
endif

config_h.set10('TOML_HEADER_ONLY', get_option('tomlpp-header-only'))
config_h.set10('TOML_SHARED_LIB', get_option('tomlpp-static'))

config_h.set_quoted('PROJECT_VERSION', meson.project_version())

configure_file(
  output: 'config.hpp',
  configuration: config_h)

bin = executable(
  'vendr',
  'src/main.cpp',
  dependencies: bindeps,
  install: true
)
