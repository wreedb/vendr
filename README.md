<!--
SPDX-FileCopyrightText: 2025 Will Reed
SPDX-License-Identifier: GPL-3.0-or-later
-->
vendr
=====
A better way to manage your projects' dependencies.

"Just use git submodules"
-------------------------
No. Git submodules aren't a good way to vendor your dependencies. When you 
depend on something, you're likely depending on a tagged release of the 
repository in question. By default, a git submodule is cloned for it's entire 
depth, which is significantly larger than a repository being fetched at 
one level of 'depth'. See the [Similar Software](#similar-software) section for more detail.

---
Usage
=====
To use vendr, simply create a file in your repo called `vendr.toml`, with contents like such:
```toml
[[repos]]
name = "tomlplusplus"
url = "https://github.com/marzer/tomlplusplus.git"
path = "deps/tomlplusplus"
tag = "v3.4.0"

[[repos]]
name = "vendr"
url = "https://github.com/wreedb/vendr.git"
path = "deps/vendr"
tag = "0.1.0"

[[files]]
name = "cpr"
url = "https://github.com/libcpr/cpr/archive/refs/tags/1.12.0.tar.gz"
path = "deps/cpr-1.12.0.tar.gz"
```
Then by invoking `vendr`, by default it will clone all listed repositories 
to the specified paths, at the specified tag. You can specify a single 
`name` from the file with `-n/--name` to fetch if you know which you you 
need at any given time. By default the depth of the clone is set to `1`; 
if you want to do a full clone of the repo's entire git history, set the 
depth to `0` like so:
```toml
[[repos]]
name = "foo"
# ...
depth = 0
```

---
Installation
------------
See [INSTALL.md](./INSTALL.md)

---
License
-------
Vendr is licensed under the GNU GPL version 3 (or later); see the [license file](./LICENSE.md) 
for more information.

---
Contributing
------------
For contribution terms and guidelines, see the [contributing document](./CONTRIBUTING.md)

---
Similar software
----------------
- **Git submodules**:
Mainly meant to be used when you are developing multiple repositories together, embedding one within another. 
They require explicit initialization and can be fragile when switching branches. They are often overkill for 
vendoring purposes due to their history being cloned in full.

- **Git subtree**:
Similar to Git submodules with more granular control, however still intermingles the subproject with your main
project's history, which is often not particularly desired for dependencies you may be using at a tagged release.

- **[Git subrepo](https://github.com/ingydotnet/git-subrepo)**:
A more elegant alternative to submodules and subtrees, though it is still limited in scope to git repositories 
by design.

- **Direct (manual) vendoring**:
Either adding vendored code to your project directly, or making use of shell scripts to add them after cloning. 
This can be very simple, but requires manual updates and history tracking.

**Vendr** tries to bridge the gap by allowing you to declare your dependencies as tagged git repositories which are 
fetched shallow by default, as well as regular HTTP(S) downloads for regular single files or source archives.