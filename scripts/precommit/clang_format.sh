#!/bin/bash

# Get list of staged files with relevant extensions
FILES=$(git diff --cached --name-only | grep -E '\.(cpp|hpp|cc|c|h)$')

if [ -z "$FILES" ]; then
  exit 0
fi

# Run clang-format on each staged file
for file in $FILES; do
  [ -f "$file" ] && clang-format -i "$file"
done