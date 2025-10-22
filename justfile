# SPDX-FileCopyrightText: 2025 Will Reed
# SPDX-License-Identifier: GPL-3.0-or-later

project-name := "vendr"
project-version := `cat version`
current-date := `date +%Y-%m-%d`
stage-dir := "{{project-name}}-{{project-version}}"

_default:
    @just -l

[group("clean")]
clean-flake:
    -rm -f result

[group("build")]
@build-flake: clean-flake
    nix build

[group("clean")]
@clean-build:
    -rm -rf .build

[group("build")]
@build-debug: clean-build
    meson setup .build \
        --prefix=/ \
        --buildtype=debug
    meson compile -C .build

[group("build")]
@build-release: clean-build
    meson setup .build \
        --prefix=/usr \
        --buildtype=release \
        --strip
    meson compile -C .build

[group("devel")]
@dev:
    meson compile -C .build

[group("info")]
@summary:
    echo -e "\033[34msummary\033[0m:"
    echo -e "  \033[34mname\033[0m:" {{project-name}}
    echo -e "  \033[34mversion\033[0m:" {{project-version}}
    echo -e "  \033[34mdate\033[0m:" {{current-date}}