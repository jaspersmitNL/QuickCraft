set(USE_VULKAN ON)
set(USE_D3D12 OFF)

set(DAWN_FETCH_DEPENDENCIES ON)
set(DAWN_BUILD_SAMPLES OFF)
set(TINT_BUILD_CMD_TOOLS OFF)
set(TINT_BUILD_TESTS OFF)
set(TINT_BUILD_IR_BINARY OFF)

set(DAWN_ENABLE_D3D11 OFF)
set(DAWN_ENABLE_D3D12 ${USE_D3D12})
set(DAWN_ENABLE_NULL OFF)
set(DAWN_ENABLE_DESKTOP_GL OFF)
set(DAWN_ENABLE_OPENGLES OFF)
set(DAWN_ENABLE_VULKAN ${USE_VULKAN})
set(TINT_BUILD_SPV_READER OFF)

add_subdirectory("dawn" EXCLUDE_FROM_ALL)


# Does nothing, as this dawn-based distribution of WebGPU is statically linked
function(target_copy_webgpu_binaries Target)
    add_custom_command(
            TARGET ${Target} POST_BUILD
            COMMAND
            "${CMAKE_COMMAND}" -E copy_if_different
            "$<TARGET_FILE_DIR:webgpu_dawn>/$<TARGET_FILE_NAME:webgpu_dawn>"
            "$<TARGET_FILE_DIR:${Target}>"
            COMMENT
            "Copying '$<TARGET_FILE_DIR:webgpu_dawn>/$<TARGET_FILE_NAME:webgpu_dawn>' to '$<TARGET_FILE_DIR:${Target}>'..."
    )
endfunction()
