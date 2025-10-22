project(
  'vendr',
  'cpp',
  meson_version: '>= 1.6.1',
  license: 'GPL-3.0-or-later',
  license_files: 'LICENSE.md',
  version: files('version'),
  default_options: ['cpp_std=c++23'],
)

cmake = import('cmake')
config_h = configuration_data()
cxx = meson.get_compiler('cpp')

manpages_feat = get_option('man-pages')
scdoc = find_program('scdoc', required: manpages_feat)

date_month = run_command('date', '+%B', check: true).stdout().strip()
date_year = run_command('date', '+%Y', check: true).stdout().strip()

if scdoc.found()
  conf_manpage = configuration_data()
  conf_manpage.set('MONTH', date_month)
  conf_manpage.set('YEAR', date_year)
  conf_manpage.set('VERSION', meson.project_version())

  vendr_1 = configure_file(
    input: 'doc/vendr.1.scd.in',
    output: 'vendr.1.scd',
    configuration: conf_manpage,
  )

  vendr_toml_5 = configure_file(
    input: 'doc/vendr.toml.5.scd.in',
    output: 'vendr.toml.5.scd',
    configuration: conf_manpage,
  )

  custom_target(
    'vendr.1',
    input: vendr_1,
    output: 'vendr.1',
    command: [scdoc],
    feed: true,
    capture: true,
    install: true,
    install_dir: get_option('mandir') / 'man1',
  )

  custom_target(
    'vendr.toml.5',
    input: vendr_toml_5,
    output: 'vendr.toml.5',
    command: [scdoc],
    feed: true,
    capture: true,
    install: true,
    install_dir: get_option('mandir') / 'man5',
  )
endif

# default is true, but easy to disable if undesired.
if get_option('install-docs')
  opt_docdir = ''

  if get_option('docdir') == ''
    opt_docdir = 'share' / 'doc' / meson.project_name() + '-' + meson.project_version()
  else
    opt_docdir = get_option('docdir')
  endif

  install_data(['LICENSE.md', 'CHANGELOG.md', 'README.md'], install_dir: opt_docdir)
  install_subdir('licenses', install_dir: opt_docdir)
  install_subdir('examples', install_dir: opt_docdir)
endif
  
if get_option('gen-pkgconfig')
  conf_pc = configuration_data()
  conf_pc.set('VERSION', meson.project_version())
  conf_pc.set('YEAR', date_year)

  n = configure_file(
    input: 'misc/vendr.pc.in',
    output: 'vendr.pc',
    configuration: conf_pc,
    install: true,
    install_dir: get_option('datadir') / 'pkgconfig',
  )

endif

completions = get_option('shell-completions')

if 'bash' in completions
  install_data(
    'misc/completions/vendr.bash',
    install_dir: get_option('datadir') / 'bash-completion/completions',
  )
endif

if 'fish' in completions
  install_data(
    'misc/completions/vendr.fish',
    install_dir: get_option('datadir') / 'fish/vendor_completions.d',
  )
endif

if 'zsh' in completions
  install_data(
    'misc/completions/_vendr',
    install_dir: get_option('datadir') / 'zsh/site-functions',
  )
endif

if get_option('static-cpp')
  add_project_link_arguments('-static-libgcc', '-static-libstdc++', language: 'cpp')
endif

deps = []

# check system for libcpr; its uncommon
# to have so we fallback to cmake subproject, using
# the system curl to link it.
system_cpr = dependency('cpr', required: false, static: get_option('static-libcpr'))

if system_cpr.found()
  cpr_dep = system_cpr
else
  cpr_cm_opts = cmake.subproject_options()
  cpr_cm_opts.set_install(false)
  cpr_cm_opts.add_cmake_defines({
    'CPR_USE_SYSTEM_CURL': true,
    'BUILD_SHARED_LIBS': false # never build shared when vendoring libcpr
  })
  cpr_proj = cmake.subproject('cpr', options: cpr_cm_opts)
  cpr_dep = cpr_proj.dependency('cpr')
endif

curl_dep = dependency('libcurl', required: true, static: get_option('static-libcurl'))

# ignore these warnings from tomlplusplus when using the header-only version
if get_option('tomlpp-header-only')
  add_project_arguments('-Wno-deprecated-literal-operator', language: 'cpp')
  config_h.set('TOML_HEADER_ONLY', 1)
else
  config_h.set10('TOML_SHARED_LIB', get_option('static-tomlpp'))
  toml_dep = dependency('tomlplusplus', required: true, static: get_option('static-tomlpp'))
  deps += toml_dep
endif

libgit2_dep = dependency(
  'libgit2',
  method: 'pkg-config',
  required: true,
  static: get_option('static-libgit2'),
)

deps += libgit2_dep
deps += cpr_dep
deps += curl_dep

# finalize/write the config.hpp
config_h.set_quoted('PROJECT_VERSION', meson.project_version())
config_h.set10('SYSTEM_ARGSHXX', get_option('system-argshxx'))
configure_file(output: 'config.hpp', configuration: config_h)

bin = executable('vendr', 'src/main.cpp', install: true, dependencies: deps)