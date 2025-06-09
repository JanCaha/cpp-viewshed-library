#!/bin/bash

set -e  # Exit on any error

FILES=$(git diff --cached --name-only | grep -E '\.(cpp|hpp|cc|c|h)$')

if [ -z "$FILES" ]; then
    exit 0
fi

VERSION=$(grep "project(" CMakeLists.txt | grep -E -o -e "[0-9\.]+")

echo -e "Project version: $VERSION"

sed -i '/version: /c\  version: \"'$VERSION'\"' conda/meta.yaml

CONDA_VERSION=$(grep "version: " conda/meta.yaml | grep -E -o -e "[0-9\.]+")

if [[ "$CONDA_VERSION" != "$VERSION" ]]; then
    echo -e "Version mismatch between CMakeLists.txt and conda/meta.yaml"
    exit 1
fi
