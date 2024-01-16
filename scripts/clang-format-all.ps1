$Directories = @("engine", "editor", "rhi", "runtime", "platform")

$Extensions = "*.cpp", "*.h", "*.vert", ".frag", ".comp", ".glsl"

foreach ($Directory in $Directories) {
    Get-ChildItem -Path $Directory -Recurse -File -Include $Extensions | ForEach-Object -Parallel {
        clang-format -style=file -i $_.FullName 
    } -ThrottleLimit $env:NUMBER_OF_PROCESSORS
}
