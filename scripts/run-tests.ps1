param([string] [ValidateSet('lcov', 'html')] $Coverage)

$TestExecutables = @(
  "./workspace/engine-test/bin/Test/QuollEngineTest"
  "./workspace/editor-test/bin/Test/QuollEditorTest"
)

Remove-Item -Force -Recurse coverage/ -ErrorAction SilentlyContinue
New-Item -Force -Path coverage/ -ItemType Directory | Out-Null

foreach ($Executable in $TestExecutables) {
    $Parent = Split-Path $Executable -Parent
    $Name = Split-Path $Executable -Leaf
    $ProfRaw = Join-Path -Path $Parent -ChildPath "default.profraw"

    Push-Location $Parent
    & "./$Name"
    Pop-Location

    if ($Coverage) {
        $OutputBasePath = Join-Path -Path "coverage" -ChildPath $Name
        $OutputProfData = $OutputBasePath + ".profdata"
        $OutputCoverageInfo = $OutputBasePath + ".info"
        llvm-profdata-16 merge -sparse $ProfRaw -o $OutputProfData
        llvm-cov-16 export $Executable -instr-profile $OutputProfData -format lcov >> $OutputCoverageInfo

        if ($Coverage -eq 'html') {
          genhtml $OutputCoverageInfo --output-directory coverage/html --prefix $(Get-Location) --quiet
        }
    }
}
