@echo on

echo ✅ Running Windows test...

echo 🔍 Running files checks...

if not exist "%PREFIX%/Library/bin/viewshed.dll" exit 1
if not exist "%PREFIX%/Library/lib/viewshed.lib" exit 1
if not exist "%PREFIX%/Library/bin/viewshed.exe" exit 1
if not exist "%PREFIX%/Library/bin/inverseviewshed.exe" exit 1
if not exist "%PREFIX%/Library/lib\viewshed_static.lib" exit 1
if not exist "%PREFIX%/Library/lib/cmake/Viewshed/ViewshedTargets.cmake" exit 1
if not exist "%PREFIX%/Library/include/Viewshed/abstractviewshed.h" exit 1

echo ✅ All tests passed successfully.