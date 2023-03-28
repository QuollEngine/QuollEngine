#!/bin/sh

if [ -z $1 ]; then
    CMD=clang-tidy
else
    CMD=clang-tidy-$1
fi

shift;

# Lint only specific projects
# by passing additional arguments
# e.g `clang-tidy-all 14 engine editor`
# will only lint engine and editor
# If no arguments are passed, all
# projects will be linted
LINT_ENGINE=1
LINT_RHI_CORE=1
LINT_RHI_VULKAN=1
LINT_RHI_MOCK=1
LINT_EDITOR=1
LINT_RUNTIME=1

if [ $# -gt 0 ]; then
    LINT_ENGINE="0"
    LINT_RHI_CORE="0"
    LINT_RHI_VULKAN="0"
    LINT_RHI_MOCK="0"
    LINT_EDITOR="0"
    LINT_RUNTIME="0"

    while [ $# -gt 0 ]
    do
    if [ $1 = 'engine' ]; then
        LINT_ENGINE=1
    elif [ $1 = 'rhi-core' ]; then
        LINT_RHI_CORE=1
    elif [ $1 = 'rhi-vulkan' ]; then
        LINT_RHI_VULKAN=1
    elif [ $1 = 'rhi-mock' ]; then
        LINT_RHI_MOCK=1
    elif [ $1 = 'editor' ]; then
        LINT_EDITOR=1
    elif [ $1 = 'runtime' ]; then
        LINT_RUNTIME=1
    fi
    shift
    done
fi

print_info() {
    echo "\e[34;1m$@\e[0m"
}

ENGINE_FILES=$(find engine/src -type f -name "*.cpp")
EDITOR_FILES=$(find editor/src -type f -name "*.cpp")
RUNTIME_FILES=$(find runtime/src -type f -name "*.cpp")
RHI_CORE_FILES=$(find engine/rhi/core -type f -name "*.cpp")
RHI_VULKAN_FILES=$(find engine/rhi/vulkan -type f -name "*.cpp")
RHI_MOCK_FILES=$(find engine/rhi/mock -type f -name "*.cpp")
VENDOR_INCLUDES="-isystem/usr/local/include -isystem./vendor/Debug/include -isystem./vendor/Debug/include/msdfgen -isystem./vendor/Debug/include/freetype2"

LINTERROR=0

if [ $LINT_ENGINE -eq 1 ]; then
    print_info "Checking Engine files"
    $CMD -header-filter=.* --p=file --quiet $ENGINE_FILES -- --std=c++17 \
        $VENDOR_INCLUDES -isystem./engine/src -isystem./engine/rhi/core/include -isystem./engine/platform-tools/include

    LINTRET=$?
    LINTERROR=$((LINTERROR | LINTRET))
fi

if [ $LINT_RHI_CORE -eq 1 ]; then
    print_info "Checking RHICore files"
    $CMD -header-filter=.* --p=file --quiet $RHI_CORE_FILES -- --std=c++17 \
        $VENDOR_INCLUDES -isystem./engine/src -isystem./engine/rhi/core/include -isystem./engine/rhi/core/include/liquid/rhi -isystem./engine/platform-tools/include

    LINTRET=$?
    LINTERROR=$((LINTERROR | LINTRET))
fi

if [ $LINT_RHI_VULKAN -eq 1 ]; then
    print_info "Checking RHIVulkan files"
    $CMD -header-filter=.* --p=file --quiet $RHI_VULKAN_FILES -- --std=c++17 \
        $VENDOR_INCLUDES -isystem./engine/src -isystem./engine/rhi/core/include -isystem./engine/rhi/vulkan/include -isystem./engine/rhi/vulkan/include/liquid/rhi-vulkan -isystem./engine/platform-tools/include

    LINTRET=$?
    LINTERROR=$((LINTERROR | LINTRET))
fi

if [ $LINT_RHI_MOCK -eq 1 ]; then
    print_info "Checking RHIMock files"
    $CMD -header-filter=.* --p=file --quiet $RHI_MOCK_FILES -- --std=c++17 \
        $VENDOR_INCLUDES -isystem./engine/src -isystem./engine/rhi/core/include -isystem./engine/rhi/mock/include -isystem./engine/rhi/mock/include/liquid/rhi-mock -isystem./engine/platform-tools/include

    LINTRET=$?
    LINTERROR=$((LINTERROR | LINTRET))
fi

if [ $LINT_EDITOR -eq 1 ]; then
    print_info "Checking Editor files"
    $CMD -header-filter=.* --p=file --quiet $EDITOR_FILES -- --std=c++17 \
        $VENDOR_INCLUDES -isystem./editor/src -isystem./engine/src -isystem./engine/rhi/core/include -isystem./engine/rhi/vulkan/include -isystem./engine/platform-tools/include

    LINTRET=$?
    LINTERROR=$((LINTERROR | LINTRET))
fi

if [ $LINT_RUNTIME -eq 1 ]; then
    print_info "Checking Runtime files"
    $CMD --header-filter=.* --p=file --quiet $RUNTIME_FILES -- --std=c++17 \
        $VENDOR_INCLUDES -isystem./runtime/src -isystem./engine/src -isystem./engine/rhi/core/include -isystem./engine/rhi/vulkan/include -isystem./engine/platform-tools/include

    LINTRET=$?
    LINTERROR=$((LINTERROR | LINTRET))
fi

exit $LINTERROR
