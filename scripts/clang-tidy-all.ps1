<#
.SYNOPSIS
    Lint Quoll engine using clang-tidy

.PARAMETER Projects
    Define which projects to lint

    Valid options are: rhi-core, rhi-vulkan, rhi-mock, engine, editor, runtime

.PARAMETER Filter
    File filter. Useful for linting single file during development

.PARAMETER Threads
    Number of threads to use for linting. Default value is all available cores.

.PARAMETER LLVMVersion
    LLVM Version to use for clang-tidy. Default value: $LLVMVersion

.EXAMPLE
    clang-tidy-app.ps1 -Projects engine, editor -Threads 4
#>

param(
    [string[]] [ValidateSet('rhi-core', 'rhi-vulkan', 'rhi-mock', 'engine', 'editor', 'runtime')]
    $Projects = @('rhi-core', 'rhi-vulkan', 'rhi-mock', 'engine', 'editor', 'runtime'),
    [string] $Filter = '*.cpp',
    [uint] $Threads,
    [uint] $LLVMVersion = 16
)

$LintRHICore = $false
$LintRHIVulkan = $false
$LintRHIMock = $false
$LintEngine = $false
$LintEditor = $false
$LintRuntime = $false

foreach ($project in $Projects) {
    if ($project -eq 'rhi-core') {
        $LintRHICore = $true
    }
    
    if ($project -eq 'rhi-vulkan') {
        $LintRHIVulkan = $true
    }
    
    if ($project -eq 'rhi-mock') {
        $LintRHIMock = $true
    }
    
    if ($project -eq 'engine') {
        $LintEngine = $true
    }
    
    if ($project -eq 'editor') {
        $LintEditor = $true
    }
    
    if ($project -eq 'runtime') {
        $LintRuntime = $true
    }
}

& "$PSScriptRoot/vars.ps1"

if (!$Threads) {
    $Threads = $AvailableNumThreads
}

Write-Output "LLVM Version: $LLVMVersion"
Write-Output "Number of threads: $Threads"
Write-Output "---"

$VendorIncludes = $PlatformHeaders + @(
    # TODO: Remove this after msdf-atlas-gen is
    # migrated
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

$global:GlobalExitCode = 0

function Start-Clang-Tidy {
    param (
        $Path,
        $Headers
    )

    $HeaderArgs = Build-Header-Args ($VendorIncludes + $Headers)

    if ($IsLinux) {
        $PlatformPP = "QUOLL_PLATFORM_LINUX";
    }
    elseif ($IsMacOS) {
        $PlatformPP = "QUOLL_PLATFORM_MACOS";
    }
    elseif ($IsWindows) {
        $PlatformPP = "QUOLL_PLATFORM_WINDOWS";
    }

    if ($Threads -gt 1) {
        Get-ChildItem -Recurse -Path $Path -Filter $Filter | ForEach-Object -Parallel {
            if ($IsLinux) {
                Set-Alias -Name clang-tidy -Value clang-tidy-$using:LLVMVersion
            }
    
            clang-tidy --p=file --quiet $_.FullName -- --std=c++20 `
                Create-Header-Paths $using:HeaderArgs -D CRYPTOPP_CXX17_UNCAUGHT_EXCEPTIONS -D $using:PlatformPP
        } -ThrottleLimit $Threads 
    }
    else {
        if ($IsLinux) {
            Set-Alias -Name clang-tidy -Value clang-tidy-$LLVMVersion
        }

        Get-ChildItem -Recurse -Path $Path -Filter $Filter | ForEach-Object {
            clang-tidy --p=file --quiet $_.FullName -- --std=c++20 `
                Create-Header-Paths $HeaderArgs -D CRYPTOPP_CXX17_UNCAUGHT_EXCEPTIONS -D $PlatformPP
            $global:GlobalExitCode = $global:GlobalExitCode -bor $LASTEXITCODE 
        } 
    }
}

if ($LintRHICore) {
    Write-Output "Checking RHICore files"

    Start-Clang-Tidy -Path "rhi/core" -Headers "./engine/lib", "./rhi/core/include", "./rhi/core/include/quoll/rhi", "./platform/base/include"
}

if ($LintRHIVulkan) {
    Write-Output "Checking RHIVulkan files"

    Start-Clang-Tidy -Path "rhi/vulkan" -Headers "./engine/lib", "./rhi/core/include", "./rhi/vulkan/include", "./rhi/vulkan/include/quoll/rhi-vulkan", "./platform/base/include"
}

if ($LintRHIMock) {
    Write-Output "Checking RHIMock files"

    Start-Clang-Tidy -Path "rhi/mock" -Headers "./engine/lib", "./rhi/core/include", "./rhi/mock/include", "./rhi/mock/include/quoll/rhi-mock", "./platform/base/include"
}

if ($LintEngine) {
    Write-Output "Checking Engine files"

    Start-Clang-Tidy -Path "engine/lib" -Headers "./engine/lib", "./rhi/core/include", "./platform/base/include"
}

if ($LintEditor) {
    Write-Output "Checking Editor files"

    Start-Clang-Tidy -Path "editor/lib" -Headers "./editor/lib", "./engine/lib", "./rhi/core/include", "./rhi/vulkan/include", "./platform/base/include"
}

if ($LintRuntime) {
    Write-Output "Checking Runtime files"

    Start-Clang-Tidy -Path "runtime/src" -Headers "./runtime/src", "./engine/lib", "./rhi/core/include", "./rhi/vulkan/include", "./platform/base/include"
}

if ($global:GlobalExitCode -gt 0) {
    exit $global:GlobalExitCode
}
