#include "Platform.hpp"

#include "vulkan/Topl_Renderer_Vulkan.hpp"

int main(int argc, char** argv){
	Platform* platform = new Platform(argv[0], "Hello Vulkan");
	platform->createWindow();

	Topl_Renderer_Vulkan renderer = Topl_Renderer_Vulkan(platform->getParentWindow());

	delete(platform);
    return 0;
}