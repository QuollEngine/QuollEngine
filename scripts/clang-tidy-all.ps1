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
    [string[]] [ValidateSet('rhi-core', 'rhi-vulkan', 'rhi-mock', 'engine', 'editor', 'runtime', 'qui')]
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
$LintQui = $false

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

    if ($project -eq 'qui') {
        $LintQui = $true;        
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

$global:FinalExitCode = 0
$global:RunspacePool = [runspacefactory]::CreateRunspacePool(1, $Threads - 1)

function Start-Clang-Tidy {
    param (
        $Path,
        $Headers,
        $Operation
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

    $Files = Get-ChildItem -Recurse -Path $Path -Filter $Filter
    $Processed = [ref] 0

    $Jobs = $Files | ForEach-Object {
        $Pwsh = [powershell]::Create().AddScript({
                param(
                    $File,
                    $PlatformDefines,
                    $HeaderArgs,
                    $LLVMVersion,
                    $Processed
                )

                if ($IsLinux) {
                    Set-Alias -Name clang-tidy -Value clang-tidy-$LLVMVersion
                }

                $Output = clang-tidy --p=file --quiet $File.FullName -- --std=c++20 `
                    Create-Header-Paths $HeaderArgs -D CRYPTOPP_CXX17_UNCAUGHT_EXCEPTIONS -D $PlatformDefines 2>$null
                $Code = $LASTEXITCODE

                if ($Output) {
                    $CurrentDir = (Get-Location).Path;
                    $Output = $Output.Replace("$CurrentDir/", "")
                } 

                $null = [System.Threading.Interlocked]::Increment($Processed) 

                @{
                    Code  = $Code
                    Error = $Output
                }
            }).AddArgument($_).AddArgument($PlatformPP).AddArgument($HeaderArgs).AddArgument($LLVMVersion).AddArgument($Processed)

        $Pwsh.RunspacePool = $global:RunspacePool

        return @{
            Pwsh   = $Pwsh
            Handle = $Pwsh.BeginInvoke()
        }
    }

    while ($Processed.Value -lt $Files.Count) {
        $Completed = ($Processed.Value / $Files.Count) * 100
        Write-Progress -CurrentOperation $Operation -PercentComplete $Completed -Activity "Linting $Operation..." -Status "$($Processed.Value) of $($Files.Count)"
        Start-Sleep -Milliseconds 1000
    }

    do {} until ($Jobs.Handle.IsCompleted)
        
    Write-Progress -CurrentOperation $Operation -Activity "Linting $Operation..." -Completed

    $Results = $Jobs | ForEach-Object {
        $Res = $_.Pwsh.EndInvoke($_.Handle)
        $_.Pwsh.Dispose()
        $Res[0]
    }

    $Errors = New-Object System.Collections.ArrayList
    $ExitCode = 0
    foreach ($Res in $Results) {
        $ExitCode = $ExitCode -bor $Res.Code
        if ($Res.Code -gt 0) {
            $Errors.Add($Res.Error)
        }
    }

    $global:FinalExitCode = $global:FinalExitCode -bor $ExitCode

    return @{
        Code   = $ExitCode
        Errors = $Errors
    }
}

function Start-Lint-Project {
    param (
        $Project,
        $Path,
        $Headers
    )

    $Result = Start-Clang-Tidy -Path $Path -Headers $Headers -Operation $Project
    Write-Progress -CurrentOperation $Operation -Completed -Activity "Linting $Project..."

    if ($Result.Code -gt 0) {
        Write-Host "Linting $Project... " -NoNewLine
        Write-Host "Error" -ForegroundColor Red
        foreach ($Error in $Result.Errors) {
            Write-Host $Error
        }
    }
    else {
        Write-Host "Linting $Project files... " -NoNewLine
        Write-Host "Done" -ForegroundColor Green
    }
}

$global:RunspacePool.Open()
if ($LintRHICore) {
    Start-Lint-Project -Project "RHICore" -Path "rhi/core" -Headers "./engine/lib", "./rhi/core/include", "./rhi/core/include/quoll/rhi", "./platform/base/include"
}

if ($LintRHIVulkan) {
    Start-Lint-Project -Project "RHIVulkan" -Path "rhi/vulkan" -Headers "./engine/lib", "./rhi/core/include", "./rhi/vulkan/include", "./rhi/vulkan/include/quoll/rhi-vulkan", "./platform/base/include"
}

if ($LintRHIMock) {
    Start-Lint-Project -Project "RHIMock" -Path "rhi/mock" -Headers "./engine/lib", "./rhi/core/include", "./rhi/mock/include", "./rhi/mock/include/quoll/rhi-mock", "./platform/base/include"
}

if ($LintEngine) {
    Start-Lint-Project -Project "Engine" -Path "engine/lib" -Headers "./engine/lib", "./rhi/core/include", "./platform/base/include"
}

if ($LintQui) {
    Start-Lint-Project -Project "Qui" -Path "engine/lib/quoll/qui" -Headers "./engine/lib", "./rhi/core/include", "./platform/base/include"
}

if ($LintEditor) {
    Start-Lint-Project -Project "Editor" -Path "editor/lib" -Headers "./editor/lib", "./engine/lib", "./rhi/core/include", "./rhi/vulkan/include", "./platform/base/include"
}

if ($LintRuntime) {
    Start-Lint-Project -Project "Runtime" -Path "runtime/src" -Headers "./runtime/src", "./engine/lib", "./rhi/core/include", "./rhi/vulkan/include", "./platform/base/include"
}

$global:RunspacePool.Close()
$global:RunspacePool.Dispose()

if ($global:FinalExitCode -gt 0) {
    exit $global:FinalExitCode
} 
