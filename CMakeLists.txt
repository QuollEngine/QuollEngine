cmake_minimum_required(VERSION 3.27)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/.cmake" ${CMAKE_MODULE_PATH})

set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
set(VCPKG_TARGET_TRIPLET "x64-windows-static")
set(VCPKG_INSTALLED_DIR "${PROJECT_SOURCE_DIR}/vcpkg_installed")

project(QuollEngine LANGUAGES CXX)
file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS engine/src/*.cpp engine/src/*.h)

find_package(glm CONFIG REQUIRED)
find_package(Freetype REQUIRED)

add_library(QuollEngine STATIC ${SOURCE_FILES})
target_include_directories(QuollEngine PUBLIC engine/src/)
target_compile_features(QuollEngine PUBLIC cxx_std_20)
target_link_libraries(QuollEngine PRIVATE glm::glm)
