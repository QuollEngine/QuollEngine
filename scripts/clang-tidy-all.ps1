param($p1, $p2, $p3, $p4, $p5, $p6)

$LintRHICore=$true
$LintRHIVulkan=$true
$LintRHIMock=$true
$LintEngine=$true
$LintEditor=$true
$LintRuntime=$true

if ($PSBoundParameters.Count -gt 0) {
    $LintRHICore=$false
    $LintRHIVulkan=$false
    $LintRHIMock=$false
    $LintEngine=$false
    $LintEditor=$false
    $LintRuntime=$false

    $PSBoundParameters.Values | ForEach-Object {
        if ($_ -eq 'rhi-core') {
            $LintRHICore=$true
        }

        if ($_ -eq 'rhi-vulkan') {
            $LintRHIVulkan=$true
        }

        if ($_ -eq 'rhi-mock') {
            $LintRHIMock=$true
        }

        if ($_ -eq 'engine') {
            $LintEngine=$true
        }

        if ($_ -eq 'editor') {
            $LintEditor=$true
        }

        if ($_ -eq 'runtime') {
            $LintRuntime=$true
        }
    }
}

$VulkanSDK=Join-Path -Path $Env:VULKAN_SDK -ChildPath "Include"

$VendorIncludes=@(
  $VulkanSDK
  './vcpkg_installed/x64-windows-static/include',
  './vendor/Debug/include'
  './vendor/Debug/include/msdfgen'
)

function Create-Header-Args {
    param (
        $Headers
    )
    
    $Headers | ForEach-Object {
        -join('-isystem', $_)
    }
}

function Clang-Tidy {
    param (
        $Path,
        $Headers
    )

    $HeaderArgs = Create-Header-Args ($VendorIncludes + $Headers)

    Get-ChildItem -Recurse -Path $Path -Filter "*.cpp" | ForEach-Object -Parallel {
        clang-tidy '-header-filter'=.* --p=file --quiet $_.FullName -- --std=c++20 `
            Create-Header-Paths $using:HeaderArgs -D CRYPTOPP_CXX17_UNCAUGHT_EXCEPTIONS
    } -ThrottleLimit $env:NUMBER_OF_PROCESSORS
}

if ($LintRHICore) {
    echo "Checking RHICore files"

    Clang-Tidy -Path "engine/rhi/core" -Headers "./engine/src", "./engine/rhi/core/include", "./engine/rhi/core/include/quoll/rhi", "./platform/base/include"
}

if ($LintRHIVulkan) {
    echo "Checking RHIVulkan files"

    Clang-Tidy -Path "engine/rhi/vulkan" -Headers "./engine/src", "./engine/rhi/core/include", "./engine/rhi/vulkan/include", "./engine/rhi/vulkan/include/quoll/rhi-vulkan", "./platform/base/include"
}

if ($LintRHIMock) {
    echo "Checking RHIMock files"

    Clang-Tidy -Path "engine/rhi/mock" -Headers "./engine/src", "./engine/rhi/core/include", "./engine/rhi/mock/include", "./engine/rhi/mock/include/quoll/rhi-mock", "./platform/base/include"
}

if ($LintEngine) {
    echo "Checking Engine files"

    Clang-Tidy -Path "engine/src" -Headers "./engine/src", "./engine/rhi/core/include", "./platform/base/include"
}

if ($LintEditor) {
    echo "Checking Editor files"

    Clang-Tidy -Path "editor/src" -Headers "./editor/src", "./engine/src", "./engine/rhi/core/include", "./engine/rhi/vulkan/include", "./platform/base/include"
}

if ($LintRuntime) {
    echo "Checking Runtime files"

    Clang-Tidy -Path "runtime/src" -Headers "./runtime/src", "./engine/src", "./engine/rhi/core/include", "./engine/rhi/vulkan/include", "./platform/base/include"
}
