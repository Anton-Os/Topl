# Checkout https://github.com/PacktPublishing/Learning-Vulkan/blob/master/Chapter%2003/HandShake/CMakeLists.txt

message(STATUS "Attempting auto locate Vulkan using CMake......")
	
# Find Vulkan Path using CMake's Vulkan Module
# This will return Boolean 'Vulkan_FOUND' indicating the status of find as success(ON) or fail(OFF).
# Include directory path - 'Vulkan_INCLUDE_DIRS' and 'Vulkan_LIBRARY' with required libraries.
find_package(Vulkan)

# Try extracting VulkanSDK path from ${Vulkan_INCLUDE_DIRS}
if (NOT ${Vulkan_INCLUDE_DIRS} STREQUAL "")
    set(VULKAN_PATH ${Vulkan_INCLUDE_DIRS})
    STRING(REGEX REPLACE "/Include" "" VULKAN_PATH ${VULKAN_PATH})
endif()
        
if(NOT Vulkan_FOUND)
    # CMake may fail to locate the libraries but could be able to 
    # provide some path in Vulkan SDK include directory variable
    # 'Vulkan_INCLUDE_DIRS', try to extract path from this.
    message(STATUS "Failed to locate Vulkan SDK, retrying again...")
    if(EXISTS "${VULKAN_PATH}")
        message(STATUS "Successfully located the Vulkan SDK: ${VULKAN_PATH}")
    else()
        message(STATUS "Unable to locate Vulkan SDK. Manually specify 'VULKAN_SDK' and 'VULKAN_VERSION'")
    endif()
else()
    message(STATUS "Vulkan SDK Found!")
endif()