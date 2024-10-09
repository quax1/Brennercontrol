#!/bin/bash
echo -n "Checking if there are uncommited changes... "
git status
echo nicht committed:   $(git diff-index HEAD --| wc -l)

 git log --pretty=format:'#define GIT_INFO_PRESENT%n static const char* GIT_INFO = "Version Information=[%H,%d]\r\n"; %n static const char* GIT_HASH = "%H"; ' -n 1 > ../gitcommit.h
# git log --pretty=format:'#define GIT_INFO_PRESENT%n static const char* GIT_HASH = "%H"; ' -n 1 > ../gitcommit.h
# git log --pretty=format:'#define GIT_INFO_PRESENT%n// static const char* GIT_INFO = "Version Information=[%H,%d]\r\n"; %n static const char* GIT_HASH = "%H"; ' -n 1 > ../gitcommit.h
echo >> ../gitcommit.h

# echo ' static const char* GIT_UNCOMMITTED     = "'$(git diff-index HEAD --| wc -l)'"; ' >> ../gitcommit.h
echo ' static const byte GIT_UNCOMMITTED_BYTE = '$(git diff-index HEAD --| wc -l)'; ' >> ../gitcommit.h         
echo >> ../gitcommit.h

hashvalue=$(git log --pretty=format:'%H' -n 1)
hid=$(echo $hashvalue | cut -b1-7)

echo ' static const char* GIT_ID = "'$hid'";'   >> ../gitcommit.h
echo >> ../gitcommit.h
