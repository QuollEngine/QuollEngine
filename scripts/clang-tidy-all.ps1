param($p1, $p2, $p3, $p4, $p5, $p6)

$LintRHICore = $true
$LintRHIVulkan = $true
$LintRHIMock = $true
$LintEngine = $true
$LintEditor = $true
$LintRuntime = $true

if ($PSBoundParameters.Count -gt 0) {
    $LintRHICore = $false
    $LintRHIVulkan = $false
    $LintRHIMock = $false
    $LintEngine = $false
    $LintEditor = $false
    $LintRuntime = $false

    $PSBoundParameters.Values | ForEach-Object {
        if ($_ -eq 'rhi-core') {
            $LintRHICore = $true
        }

        if ($_ -eq 'rhi-vulkan') {
            $LintRHIVulkan = $true
        }

        if ($_ -eq 'rhi-mock') {
            $LintRHIMock = $true
        }

        if ($_ -eq 'engine') {
            $LintEngine = $true
        }

        if ($_ -eq 'editor') {
            $LintEditor = $true
        }

        if ($_ -eq 'runtime') {
            $LintRuntime = $true
        }
    }
}

$VulkanSDK = Join-Path -Path $Env:VULKAN_SDK -ChildPath "Include"

$VendorIncludes = @(
    $VulkanSDK
    './vcpkg_installed/x64-windows-static/include',
    './vcpkg_installed/x64-windows-static/include/msdfgen',
    './vendor/Debug/include'
)

function Build-Header-Args {
    param (
        $Headers
    )
    
    $Headers | ForEach-Object {
        -join ('-isystem', $_)
    }
}

function Start-Clang-Tidy {
    param (
        $Path,
        $Headers
    )

    $HeaderArgs = Build-Header-Args ($VendorIncludes + $Headers)

    Get-ChildItem -Recurse -Path $Path -Filter "*.cpp" | ForEach-Object -Parallel {
        clang-tidy '-header-filter'=.* --p=file --quiet $_.FullName -- --std=c++20 `
            Create-Header-Paths $using:HeaderArgs -D CRYPTOPP_CXX17_UNCAUGHT_EXCEPTIONS
    } -ThrottleLimit $env:NUMBER_OF_PROCESSORS
}

if ($LintRHICore) {
    Write-Output "Checking RHICore files"

    Start-Clang-Tidy -Path "rhi/core" -Headers "./engine/src", "./rhi/core/include", "./rhi/core/include/quoll/rhi", "./platform/base/include"
}

if ($LintRHIVulkan) {
    Write-Output "Checking RHIVulkan files"

    Start-Clang-Tidy -Path "rhi/vulkan" -Headers "./engine/src", "./rhi/core/include", "./rhi/vulkan/include", "./rhi/vulkan/include/quoll/rhi-vulkan", "./platform/base/include"
}

if ($LintRHIMock) {
    Write-Output "Checking RHIMock files"

    Start-Clang-Tidy -Path "rhi/mock" -Headers "./engine/src", "./rhi/core/include", "./rhi/mock/include", "./rhi/mock/include/quoll/rhi-mock", "./platform/base/include"
}

if ($LintEngine) {
    Write-Output "Checking Engine files"

    Start-Clang-Tidy -Path "engine/src" -Headers "./engine/src", "./rhi/core/include", "./platform/base/include"
}

if ($LintEditor) {
    Write-Output "Checking Editor files"

    Start-Clang-Tidy -Path "editor/src" -Headers "./editor/src", "./engine/src", "./rhi/core/include", "./rhi/vulkan/include", "./platform/base/include"
}

if ($LintRuntime) {
    Write-Output "Checking Runtime files"

    Start-Clang-Tidy -Path "runtime/src" -Headers "./runtime/src", "./engine/src", "./rhi/core/include", "./rhi/vulkan/include", "./platform/base/include"
}
