# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.1.0] - 2025-10-11

### Changed
- meson.build: write define for PROJECT_VERSION
- src/main.cpp: static const for project version

## [0.2.0] - 2025-10-19

### Removed
- meson.version: moved to 'version'

### Added
- src/help.hpp: separate generic helper and vendr::log definitions
- version: moved from 'meson.version'
- misc/completions: added shell completions for bash, fish and zsh
- misc/vendr.pc.in: pkg-config file
- doc/vendr.1.scd.in: section 1 manpage for vendr
- doc/vendr.toml.5.scd.in: section 5 manpage for vendr.toml
- CONTRIBUTING.md: document contribution terms
- examples/: vendr.toml format examples for reference and testing
- src/args.hxx: vendored, license in licenses/third-party
- src/toml.hpp: vendored, license in licenses/third-party

### Changed
- meson.options: added 'static-cpp' for -static-libstdc++ -static-libgcc
- src/log.hpp: NO_COLOR standard compliance
- src/main.cpp: NO_COLOR standard compliance
- src/vendr.hpp: NO_COLOR standard compliance
- src/main.cpp: replace usage of boost program_options with args.hxx
- src/vendr.hpp: add http/https downloading with libcurl/CPR
- src/vendr.hpp: vendr::file struct for file download entries
- src/vendr.hpp: vendr::targets struct for repos+files
- src/vendr.hpp: vendr::serializeToml now returns -> vendr::targets
- meson.build: removed boost_dep
- meson.build: added libcurl and libcpr deps
- meson.options: added 'static-libcurl' for -l:libcurl.a
- meson.options: add options for manpages, pkg-config file and shell completions
- INSTALL.md: update install procedure and dependency requirements
