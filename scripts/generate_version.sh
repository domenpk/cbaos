#!/bin/sh

ver="$(git rev-parse --short HEAD)"
test -n "$(git status -s .)" && ver="${ver}+"

cat > include/version.h << EOF
#ifndef VERSION_H_
#define VERSION_H_

#define VERSION "$ver"

#endif
EOF
