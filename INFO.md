# INFO

[Pixel-Puppets](https://github.com/Anton-Os/Pixel-Puppets.git) and [Voxelator](https://github.com/Anton-Os/Voxelator.git) are my ideas built over Topl, but there are many more functionalities I hope to implement using the engine: animation, modeling, photogrammetry, gaming, and simulation projects are all candidates.

Project Directory Structure:

1. **core/** contains main engine components, both renderer-specific and generalized. Renderer implementations are split into the rendering source file, (Topl_Renderer_X.cpp) which handles data and drawing, and the pipeline source file, (Topl_Pipeline_X.cpp) which handles shader compilation and linking. The Scene implementation is split into statics source file, (Topl_Scene_Statics.cpp) which handles world actors, lighting, cameras, and texturing properties, and the dynamics source file, (Topl_Scene_Dynamics.cpp) which handles movement, links, and anchors used in physics calculations. The App implementation simplifies the process of creating a demo and maintaining a rendering loop with updates. Additionally, Maths and Physics headers are included as well as a Images header which wraps around types in the [Rasteron](https://github.com/Anton-Os/Rasteron) project.
2. **geometry/** contains functionality used to generate 2D and 3D data. Render Objects are defined in the Geometry implementation which houses vertices and vertex attributtes, split into shape types such as triangles, cones, boxes, and spheres. Actors are defined in the Geo_Actor header and assign positioning and orientation data to the underlying render object. A tree defined in the Geo_Tree object groups actors together, split into tree types such as such as grids, chains, and 3D models. The Sprite_Table implementation simplifies grouping together and displaying images on multiple rectangular render objects. The ShapesGen implementation is included to generate special and iterative Render Objects, as well as modifying preexisting ones.
3. **helper/** contains helper modules for working with files, timing, and system-specific inputs and functionality. The ValueGen implementation is included to perform mathematical calculations with respect to memory operations, color assignment, linear algebra, and more.
4. **demos/** contain demos showcasing the Topl functionality and include smaller tests as well. Modern demos should be written utilizing the App implmentation.
5. **shaders/** contains both hlsl and glsl shaders as well as a unified include directory containing classes that generate a unified vertex layout and uniform block variables that are passed to the rendering backend.
6. **assets/** contains fonts, images, models, and other resources