# INFO

Project Structure:

1. **core/** contains main engine components. The Renderer (Topl_Renderer) is split into rendering and pipeline source files specific to the graphics library. The Scene (Topl_Scene) is split into static implementation (geometry, lighting, cameras, and texturing) and dynamics implementation (physcis, movement, links). The Factory (Topl_Factory) and App (Topl_Main) respectively handle working across graphics libraries and creating functional demos.
2. **geometry/** contains functionality used to generate geometries. Geo_Renderableect (Geometry) is split into primitive types such as triangles, cones, boxes, and spheres. Actor (Geo_Actor) assigns positioning and orientation data. Geo_Construct (Geo_Construct) groups actors together, with tree types such as such as grids, chains, and imported 3D models.
3. **helper/** contains helper modules for working with files, timing, and OS-specific functionality. Utility and calculation functions are included as well, some defined inside ValueGen
4. **demos/** contain demos showcasing the Topl functionality and include smaller tests as well.
5. **shaders/** contains both hlsl and glsl shaders and common  that can be used across graphics libraries
6. **assets/** contains fonts, images, models, and other resources
7. **cmake/** contains build scripts used in the project