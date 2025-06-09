#!/bin/bash

set -e  # Exit on any error

FILES=$(git diff --cached --name-only | grep -E '^CMakeLists.txt$')

if [ -z "$FILES" ]; then
    exit 0
fi

VERSION=$(grep "project(" CMakeLists.txt | grep -E -o -e "[0-9\.]+")

echo -e "Project version: $VERSION"

# update conda package version
CONDA_VERSION=$(grep "version: " conda/meta.yaml | grep -E -o -e "[0-9\.]+")

if [[ "$CONDA_VERSION" != "$VERSION" ]]; then
    sed -i '/version: /c\  version: \"'$VERSION'\"' conda/meta.yaml
fi

# update README.md with the new version
README_VERSION=$(grep "version = " README.md | grep -E -o -e "[0-9\.]+")

if [[ "$README_VERSION" != "$VERSION" ]]; then
    YEAR=$(date +%Y)
    ISO_DATE=$(date +%Y-%m-%d)
    sed -i '/version = {/c\  version = {'$VERSION'}' README.md
    sed -i '/year    = {/c\  year    = {'$YEAR'}' README.md
    sed -i '/date    = {/c\  date    = {'$ISO_DATE'}' README.md
fi