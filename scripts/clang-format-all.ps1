param(
    [string] [ValidateSet('format', 'verify')]
    $Op = 'format',
    [uint] $Threads,
    [uint] $LLVMVersion = 16
)

$Directories = @("engine", "editor", "rhi", "runtime", "platform")
$Extensions = "*.cpp", "*.h", "*.vert", ".frag", ".comp", ".glsl"

& "$PSScriptRoot/vars.ps1"

if (!$Threads) {
    $Threads = $AvailableNumThreads
}

foreach ($Directory in $Directories) {
    Get-ChildItem -Path $Directory -Recurse -File -Include $Extensions | ForEach-Object -Parallel {
        if ($IsLinux) {
            Set-Alias -Name clang-format -Value clang-format-$using:LLVMVersion
        }

        if ($Op -eq 'verify') {
            clang-format -style=file -n -Werror $_.FullName
        }
        else {
            clang-format -style=file -i $_.FullName
        }
    } -ThrottleLimit $Threads
}
