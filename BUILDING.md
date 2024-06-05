# Building

Build and Install most recent version of Rasteron project:
https://github.com/Anton-Os/Rasteron.git

Configure Topl project in CMake, set cache variable Rasteron_Install_Path to the install directory used in the Rasteron project above.

Navigate to the Topl build directory and invoke the commands:

1. cmake .
2. cmake --build . --target Assimp
3. cmake --build .