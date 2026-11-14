# Building

Rasteron, Geometron and Soundscape are built from the sources inside of their submodules, fetch them first:

    git submodule update --init Rasteron Geometron Soundscape

Configure the Topl project in CMake, then navigate to the Topl build directory and invoke the commands:

1. cmake --build .
2. cmake --build .

The first build covers the dependencies, which are built and installed into the INSTALL directory of the build:

- **Rasteron** is built from its submodule with every option enabled (`SUPPORT_FONT_BAKING`, `SUPPORT_MEDIA_EXPORT`), alongside zlib, libpng, libtiff, libjpeg, FreeType and FFmpeg
- **Soundscape** is built from its submodule, alongside miniaudio and kissfft
- **GLEW** and **Assimp** are fetched and built

The second build configures again on its own, locates the dependencies and builds the engine and demos. Run `cmake .` by hand if the engine targets are still missing. A network connection is needed for the first build.

**Geometron** has no build step of its own, GEOLIB is compiled from its sources and the engine includes its headers directly.

### Options

| Option | Default | Description |
| --- | --- | --- |
| `SUPPORT_TEXTURES` | ON | Texturing and UI, builds Rasteron |
| `SUPPORT_MODELS` | ON | 3D model loading and exporting for Geometron, builds Assimp |
| `SUPPORT_AUDIO` | ON | Audio playback, capture and analysis, builds Soundscape |
| `SUPPORT_MIDI` | OFF | MIDI plugin of Soundscape, defines `TOPL_ENABLE_MIDI` |
| `SUPPORT_PHYSICS` | ON | Physics calculations |

Changing an option that adds a dependency takes two builds again, the first one for the dependency. Build directories from before the submodules keep their cached values, so set `SUPPORT_MODELS` by hand or start from an empty directory.

### Working on the submodules

Sources inside of Rasteron and Soundscape are tracked, a change rebuilds and installs the project with the next build of the engine. Their targets are `Rasteron_Source` and `Soundscape_Source`, each builds inside of `PROJECTS/<name>/build`. Geometron sources are part of GEOLIB and rebuild like any other source.

Rasteron and Soundscape write `support/support_def.h` into their own source directories when they configure, as Topl does, so the submodules show those files as modified.
