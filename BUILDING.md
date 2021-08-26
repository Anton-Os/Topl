# Building

Build and Install most recent version of Rasteron project:
https://github.com/Anton-Os/Rasteron.git

Configure Topl project in CMake, set cache variable Rasteron_Install_Path to the install directory used in the Rasteron project above.

Navigate to the Topl build directory and invoke the commands:
cmake .
cmake --build . --target Assimp
cmake --build .