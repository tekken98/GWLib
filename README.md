# GWLib
Graphic Window Lib c++

#不可恢复的操作，慎用，一般用来减少仓库体积
git filter-branch --index-filter 'git rm --cached --ignore-unmatch <fileName>'
rm -rf .git/refs/original/*
git reflog expire --expire=now --all
git fsck --full --unreachable
git repack -A -d
git gc --aggressive --prune=now
