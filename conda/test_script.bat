@echo on

echo ✅ Running Windows test...

🔍 Running GUI test...
if not exist "%PREFIX%/Library/bin/viewshed.dll" exit 1
if not exist "%PREFIX%/Library/lib/viewshed.lib" exit 1
if not exist "%PREFIX%/Library/bin/viewshed.exe" exit 1
if not exist "%PREFIX%/Library/bin/inverseviewshed.exe" exit 1
if not exist "%PREFIX%/Library/lib\viewshed_static.lib" exit 1
if not exist "%PREFIX%/Library/lib/cmake/Viewshed/ViewshedTargets.cmake" exit 1
if not exist "%PREFIX%/Library/include/Viewshed/abstractviewshed.h" exit 1

REM Check that commands can be run successfully
echo 🔍 Running command line test...
"%PREFIX%/Library/bin/viewshed.exe" -h || exit 1
"%PREFIX%/Library/bin/inverseviewshed.exe" -h || exit 1

echo ✅ All tests passed successfully.