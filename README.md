# Topl

An experimental C++ graphics engine in the making, using OpenGL 4.4 and DirectX 11 as graphics backends with a Vulkan port in progress. Windows and Linux are the target platforms, with bindings being cross-compiled for Flutter and mobile devices

Topl can be used to create a wide range of rendering applications ranging from animation, modeling, photogrammetry, gaming, and simulations. Audio functionality and FFT analysis coming soon!

### Enviornment

I aim to keep external dependencies to a bare minimum with this project:

1. GLEW to support OpenGL
2. ASSIMP for 3D model support
3. miniaudio and kissfft for audio playback and analysis, built by the Soundscape submodule

*See BUILDING.md for building instructions*

### Project Structure:

1. **core/** contains main engine components
2. **Geometron** submodule for meshes, constructs, maths, and physics types
3. **support/** contains helper modules for working with files, timing, media, and OS-specific features
4. **Rasteron** submodule for texture functionality and overlayed UI support
5. **Soundscape** submodule for audio playback, capture, and analysis
6. **Droidl** submodule for porting the engine to the Android platform
7. **demos/** contain demos showcasing the runtime functionality
8. **shaders/** contains pre-built pipeline and shader code 
9. **assets/** contains fonts, images, models, and other resources
10. **scripts/** contains build scripts used in the project