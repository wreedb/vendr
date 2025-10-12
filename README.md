vendr
=====
A better way to get your software's dependencies.

"Just use git submodules"
-------------------------
No. Git submodules aren't a good way to vendor your dependencies. When you 
depend on something, you're likely depending on a tagged release of the 
repository in question. By default, a git submodule is cloned for it's entire 
depth, which is significantly larger than a repository being fetched at 
one level of 'depth'. Due to the fact that we do not live in a perfect world
where projects offer archives of their work at each release (not automatically 
generated archives made by git(hub/lab) or other forges), I made this small 
program to supplement that.
