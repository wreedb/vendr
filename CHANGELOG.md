<!--
SPDX-FileCopyrightText: 2025 Will Reed
SPDX-License-Identifier: GPL-3.0-or-later
-->
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

## [0.2.1] - 2025-10-20

### Added
- chore: add vendr.toml to project root
- chore(misc/packaging): add Arch Linux PKGBUILD
- chore(misc): add ignore-file-list for cloc tool

### Changed
- build: create meson wrap for libcpr, update gitignore for subprojects/cpr
- build: change ifdef to check SYSTEM_ARGSHXX instead of VENDORED_ARGSHXX
- docs(install): remove libcpr as an explicit dependency, meson wrap handles it
- fix(src/vendr): vendr::get will create leading directories in file.path, avoiding filesystem_error throws
- chore(version): bump to 0.2.1

## [Unreleased] - 2025-10-21

### Changed
- build(meson): adjust libcpr system detection
- build(meson): update license_files project argument
- chore(packaging): update Arch Linux PKGBUILD to reflect 0.2.1 release

### Removed
- docs(license): moved LICENSE from licenses/LICENSE.md to project root

## [Unreleased] - 2025-10-22

### Added
- build(flake): add nix flake definition
- build(just): add justfile for utility

### Changed
- build(meson): replace libcpr find_library() with regular dependency() call
- chore(meta): update editorconfig and gitignore for nix files
- chore(editorconfig): specify no final newline on 'version' file
- fix(http): display received MB when server doesn't specify Content-Length instead of 0%
- fix(http): check for Content-Length header field before downloading
- build(pkgconfig): make pkg-config file relocatable with '${pcfiledir}'
- build(meson): add install-docs and docdir options, for where to install examples and licenses
- build(flake): consolidate duplicate input lists, strip installed executable
- chore(editorconfig): no final newline by default, override where actually needed/desired

## [Unreleased] - 2025-10-23

### Changed
- build(flake): use gcc 15 for flake stdenv, consistent with my host machine
- chore(gitignore): add compile_commands.json to ignore for clang-tidy
- chore(src): clean unnecessary include of cstring
- feat(src): add -m,--manual option to open than man(1) page for vendr
- docs(readme): add section about similar software
- chore: add SPDX license sections
- refactor(src): replace useColor ternaries with helper function calls
- docs(install): add instructions trying vendr with nix
- refactor(src): replace ternaries in vendr::log functions, move verbose/useColor out of log ns
- chore(src): all strings wrapped for gettext in vendr.hpp

### Added
- build(packaging): added '-git' variant of PKGBUILD which follows repository HEAD
- feat(i18n): header for i18n support with gettext
- chore(licenses): add libcpr license

## [Unreleased] - 2025-10-24

### Changed
- chore(packaging): add gettext to PKGBUILD makedepends

## [Unreleased] - 2025-10-25

### Added
- build(i18n): add meson install script for translations

### Changed
- build(meson): add options for installing translation files
- build(just): added gettext rule for updating translations