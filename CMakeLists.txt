project(Engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

add_subdirectory(Libraries/SDL2)
# add_subdirectory(Libraries/SPIRV-Reflect)

file(GLOB_RECURSE SOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/Src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Src/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Libraries/glad/src/gl.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/Libraries/SPIRV-Reflect/spirv_reflect.c"
    )

if(NOT WIN32 OR NOT D3D)
    list(FILTER SOURCE_FILES EXCLUDE REGEX "DX11")
endif()

add_compile_definitions(SDL_MAIN_HANDLED)
# add_compile_definitions(VULKAN_HPP_DISABLE_ENHANCED_MODE)

add_library(Engine STATIC)
target_sources(Engine PUBLIC "${SOURCE_FILES}")

target_link_libraries(Engine PUBLIC SDL2)
target_link_libraries(Engine PUBLIC vulkan)
if(WIN32 AND D3D)
    target_link_libraries(Engine PUBLIC d3d11 dxgi dxguid)
endif()

target_include_directories(Engine
PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/Include"
    "${CMAKE_CURRENT_SOURCE_DIR}/Libraries/glad/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/Libraries/SDL2/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/Libraries/SPIRV-Reflect"
    "${CMAKE_CURRENT_SOURCE_DIR}/Libraries/SPIRV-Reflect/include"
    )