#!/bin/sh

if [ -z $1 ]; then
    CMD=clang-tidy
else
    CMD=clang-tidy-$1
fi

print_info() {
    echo "\e[34;1m$@\e[0m"
}

ENGINE_FILES=$(find engine/src -type f -name "*.cpp")
EDITOR_FILES=$(find editor -type f -name "*.cpp")
RHI_CORE_FILES=$(find engine/rhi/core -type f -name "*.cpp")
RHI_VULKAN_FILES=$(find engine/rhi/vulkan -type f -name "*.cpp")

print_info "Checking Engine files"
$CMD -header-filter=.* --p=file --quiet $ENGINE_FILES -- --std=c++17 \
    -isystem./vendor/include -isystem./engine/src -isystem./engine/rhi/core/include -isystem/usr/local/include

print_info "Checking RHICore files"
$CMD -header-filter=.* --p=file --quiet $RHI_CORE_FILES -- --std=c++17 \
    -isystem./vendor/include -isystem./engine/src -isystem./engine/rhi/core/include -isystem./engine/rhi/core/include/liquid/rhi -isystem/usr/local/include

print_info "Checking RHIVulkan files"
$CMD -header-filter=.* --p=file --quiet $RHI_VULKAN_FILES -- --std=c++17 \
    -isystem./vendor/include -isystem./engine/src -isystem./engine/rhi/core/include -isystem./engine/rhi/vulkan/include -isystem./engine/rhi/vulkan/include/liquid/rhi-vulkan -isystem/usr/local/include

print_info "Checking Editor files"
$CMD -header-filter=.* --p=file --quiet $EDITOR_FILES -- --std=c++17 \
    -isystem./vendor/include -isystem./engine/src -isystem./engine/rhi/core/include -isystem./engine/rhi/vulkan/include -isystem./engine/platform-tools/include -isystem/usr/local/include
