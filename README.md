# Topl

An experimental C++ graphics engine in the making, using OpenGL 4.4 and DirectX 11 as graphics backends with a Vulkan port in progress. Windows and Linux are the target platforms, with bindings being cross-compiled for Flutter and mobile devices

Topl can be used to create a wide range of rendering applications ranging from animation, modeling, photogrammetry, gaming, and simulations. Audio functionality and FFT analysis coming soon!

### Enviornment

I aim to keep external dependencies to a bare minimum with this project:

1. GLEW to support OpenGL
2. ASSIMP for optional 3D model support
3. miniaudio and kissfft for optional audio playback and analysis

*See BUILDING.md for building instructions*

### Project Structure:

1. **core/** contains main engine components
2. **geometry/** contains functionality used to generate constructs and meshes
3. **support/** contains helper modules for working with files, timing, maths, media, and OS-specific features
4. **Rasteron** submodule for texture functionality and overlayed UI support
5. **Droidl** submodule for porting the engine to the Android platform
6. **demos/** contain demos showcasing the runtime functionality
7. **shaders/** contains pre-built pipeline and shader code 
8. **assets/** contains fonts, images, models, and other resources
9. **scripts/** contains build scripts used in the project