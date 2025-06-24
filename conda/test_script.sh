#!/bin/bash
set -euo pipefail

echo "Running Unix test..."

# change dynamic library extension based on OS - so for Linux, dylib for macOS
OS_NAME="$(uname -s)"
LIBRARY_EXTENSION="so"
if [[ "$OS_NAME" == "Darwin" ]]; then
    LIBRARY_EXTENSION="dylib"
fi

# Function to check if a file exists
check_file_exists() {
  local filepath="$1"
  if [[ -f "$filepath" ]]; then
    echo "✅ File exists: $filepath"
  else
    echo "❌ Missing file: $filepath"
    exit 1
  fi
}

# Function to run a binary with -h and check result
run_and_check() {
  local bin="$1"
  "$bin" -h
  if [[ $? -ne 0 ]]; then
    echo "❌ $bin -h failed"
    exit 1
  else
    echo "✅ $bin -h ran successfully"
  fi
}

# existence of files
check_file_exists "$PREFIX/lib/libviewshed.$LIBRARY_EXTENSION"
check_file_exists "$PREFIX/lib/libviewshed.a"
check_file_exists "$PREFIX/lib/cmake/Viewshed/ViewshedTargets.cmake"
check_file_exists "$PREFIX/include/Viewshed/abstractviewshed.h"
check_file_exists "$PREFIX/bin/viewshed"
check_file_exists "$PREFIX/bin/inverseviewshed"
check_file_exists "$PREFIX/bin/viewshedcalculator"

run_and_check "$PREFIX/bin/viewshed"
run_and_check "$PREFIX/bin/inverseviewshed"

# Run GUI app and close it after 1 second
if [[ -x "$PREFIX/bin/viewshedcalculator" ]]; then
  echo "🔍 Launching viewshedcalculator GUI for 1 second..."
  "$PREFIX/bin/viewshedcalculator" &
  app_pid=$!
  sleep 1
  kill $app_pid || true
  echo "✅ viewshedcalculator closed."
fi