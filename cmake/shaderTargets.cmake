file(GLOB gl4_shaders ${SHADERS_DIR}/glsl/*.glsl)
add_custom_target(GL4_ShaderEdit SOURCES ${gl4_shaders})
add_dependencies(GL4_ShaderEdit GL4_ENGINE)

if(WIN32)
	file(GLOB drx11_shaders ${SHADERS_DIR}/hlsl/*.hlsl)
	add_custom_target(DX11_ShaderEdit SOURCES ${drx11_shaders})
	add_dependencies(DX11_ShaderEdit DX11_ENGINE)
endif()

if(VK_FOUND) # comiling shaders for vulkan
	# function(compileShader shaderFile shaderType)
	#	get_filename_component(shader_name ${shaderFile} NAME)
	#	message(STATUS "Compiling ${shaderType} shader ${shader_name}")

	#	string(FIND ${shader_name} shaderType isShaderType)
	#	message("${isShaderType}")
	#	if(NOT isShaderType EQUAL -1)
	#		message("Compiling ${shaderType} shader ${shaderFile}")
	#		exec_program(
	#			${glslc_exec} 
	#			${CMAKE_BINARY_DIR} 
	#			ARGS "-fshader-stage=vertex ${shaderFile} -o ${shader_name}.spv"
	#		)
	#	endif()
	# endfunction()

	message(STATUS "Searching inside ${VULKAN_PATH}/Bin/")

    find_file(glslc_exec "glslc" "glslc.exe" PATHS "${VULKAN_PATH}/Bin/") # for compiling shader code into bytecode
    # find_file(glslc_exec "glslangValidator" "glslangValidator.exe" PATHS "${VULKAN_PATH}/Bin/") # for compiling shader code into bytecode
	if(NOT ${glslc_exec} STREQUAL "glslc_exec-NOTFOUND")
		message("${glslc_exec} executing")
        foreach(shader IN ITEMS ${gl4_shaders})
			get_filename_component(shader_name ${shader} NAME)

			string(FIND ${shader_name} "Vert" isVertexShader)
			if(NOT isVertexShader EQUAL -1)
				message("Compiling vertex shader ${shader_name} inside of ${shader}")
				# exec_program(${glslc_exec} ${CMAKE_CURRENT_BINARY_DIR} ARGS "-fshader-stage=vertex ${shader} -o ${shader_name}.spv")
                # exec_program(${glslc_exec} ${SHADERS_DIR}/spirv ARGS "-fshader-stage=vertex ${shader} -o ${shader_name}.spv")
                exec_program(${glslc_exec} ${SHADERS_DIR}/spirv ARGS "-fshader-stage=vertex ${shader} -o ${shader_name}.spv")
			endif()
			
			string(FIND ${shader_name} "Frag" isFragShader)
			if(NOT isFragShader EQUAL -1)
				message("Compiling fragment shader ${shader_name} inside of ${shader}")
				# exec_program(${glslc_exec} ${CMAKE_CURRENT_BINARY_DIR} ARGS "-fshader-stage=frag ${shader} -o ${shader_name}.spv")
                exec_program(${glslc_exec} ${SHADERS_DIR}/spirv ARGS "-fshader-stage=frag ${shader} -o ${shader_name}.spv")
			endif()

            string(FIND ${shader_name} "Geo" isGeomShader)
			if(NOT isGeomShader EQUAL -1)
				message("Compiling geometry shader ${shader_name} inside of ${shader}")
				# exec_program(${glslc_exec} ${CMAKE_CURRENT_BINARY_DIR} ARGS "-fshader-stage=geom ${shader} -o ${shader_name}.spv")
                exec_program(${glslc_exec} ${SHADERS_DIR}/spirv ARGS "-fshader-stage=geom ${shader} -o ${shader_name}.spv")
			endif()

            string(FIND ${shader_name} "Tess" isTessShader)
			if(NOT isTessShader EQUAL -1)
				string(FIND ${shader_name} "Eval" isTessEvalShader)
                if(NOT isTessEvalShader EQUAL -1)
                    message("Compiling tesselation evaluation shader ${shader_name} inside of ${shader}")
                    # exec_program(${glslc_exec} ${CMAKE_CURRENT_BINARY_DIR} ARGS "-fshader-stage=tesseval ${shader} -o ${shader_name}.spv")
                    exec_program(${glslc_exec} ${SHADERS_DIR}/spirv ARGS "-fshader-stage=tesseval ${shader} -o ${shader_name}.spv")
                else()
                    message("Compiling tesselation control shader ${shader_name} inside of ${shader}")
                    # exec_program(${glslc_exec} ${CMAKE_CURRENT_BINARY_DIR} ARGS "-fshader-stage=tesscontrol ${shader} -o ${shader_name}.spv")
                    exec_program(${glslc_exec} ${SHADERS_DIR}/spirv ARGS "-fshader-stage=tesscontrol ${shader} -o ${shader_name}.spv")
                endif()
			endif()
        endforeach()
    endif()
endif()

configure_file("${CMAKE_SOURCE_DIR}/support/support_def.h.in" "${CMAKE_SOURCE_DIR}/support/support_def.h")