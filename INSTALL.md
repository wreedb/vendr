<!--
SPDX-FileCopyrightText: 2025 Will Reed
SPDX-License-Identifier: GPL-3.0-or-later
-->
Build Requirements
========
Vendr uses the [Meson](https://mesonbuild.com) build system, and by extension 
also requires [Ninja](https://ninja-build.org).

A compiler with support for at least C++23 is also needed, such as:
- [GCC](https://gcc.gnu.org) >= version 14
- [Clang](https://clang.llvm.org) >= version 18

#### NOTE for Clang users:
You will likely want to add the following argument, as `#include <print>` may not be supported without it.
```
-D cpp_args=-stdlib=libc++
```

Building
========
```sh
git clone https://github.com/wreedb/vendr.git
cd vendr

meson setup build --buildtype=release # --prefix=/my/prefix
# optionally: meson configure build
meson compile -C build
meson install -C build

# to check the installation and see available options
vendr --help
vendr --version
```

Runtime Dependencies
====================
Unless linked statically at build time, the following shared libraries will 
required at runtime:
- [libtomlplusplus](https://github.com/marzer/tomlplusplus)
- [libcurl](https://curl.se/libcurl)
- [libgit2](https://github.com/libgit2/libgit2)

Pre-built Executables and Packages
==================================
You may also find standalone executables on the [releases](https://github.com/wreedb/vendr/releases) page. 
Currently these are only provided for the `AMD64` or (`x86_64` if you 
really love underscores) architecture. Their names will be something 
similar to `vendr-<version>-<arch>-static`.

Files named `vendr-<version>-<arch>.package.tar.*` contain the executable, manual 
pages and shell completions.

Source-only archives named `vendr-<version>.source.tar.*` are also available.
