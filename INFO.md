# INFO

[Puppetware](https://github.com/Anton-Os/Puppetware) and [Voxelator](https://github.com/Anton-Os/Voxelator.git) are my ideas built over Topl. Animation, modeling, photogrammetry, gaming, and simulation projects are all candidates for uses.

Project Directory Structure:

1. **core/** contains main engine components, both renderer-specific and generalized. Renderer implementations are split into the rendering source file, (Topl_Renderer_X.cpp) which handles data and drawing, and the pipeline source file, (Topl_Pipeline_X.cpp) which handles shader compilation and linking. The Scene implementation is split into statics source file, (Topl_Scene_Statics.cpp) which handles world actors, lighting, cameras, and texturing properties, and the dynamics source file, (Topl_Scene_Dynamics.cpp) which handles movement, links, and anchors used in physics calculations. The Factory and App implementation simplify the process of working with graphics APIs and creating demos.
2. **geometry/** contains functionality used to generate 2D and 3D data. Geo_RenderObjects implementation is defined in Geometry.hpp and is split into primitive types such as triangles, cones, boxes, and spheres. Actors are defined in Geo_Actor.hpp and assign positioning and orientation data to its underlying render object. Geo_Tree is defined in Geo_Tree.hpp and groups actors together, split into tree types such as such as grids, chains, and 3D models.
3. **helper/** contains helper modules for working with files, timing, and system-specific functionality. Utility and calculating functions are included as well, many defined inside ValueGen.hpp
4. **demos/** contain demos showcasing the Topl functionality and include smaller tests as well.
5. **shaders/** contains both hlsl and glsl shaders and common .hpp file that can be used independent of graphics API
6. **assets/** contains fonts, images, models, and other resources
7. **cmake/** contains build scripts used in the project