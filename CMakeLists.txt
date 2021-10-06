project(Engine)

add_subdirectory(Libraries/SDL2)

file(GLOB_RECURSE SOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/Src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Libraries/glad/src/gl.c"
    )

list(FILTER SOURCE_FILES EXCLUDE REGEX "DX11")

add_compile_definitions(SDL_MAIN_HANDLED)

add_library(Engine STATIC)
target_sources(Engine PUBLIC "${SOURCE_FILES}")

target_link_libraries(Engine PUBLIC SDL2)

target_include_directories(Engine
PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/Include"
    "${CMAKE_CURRENT_SOURCE_DIR}/Libraries/glad/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/Libraries/SDL2/include"
    )