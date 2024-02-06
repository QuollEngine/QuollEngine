param($p0)

$Tool = Join-Path -Path $(Get-Location) -ChildPath "scripts/llvm-cov.sh"

Remove-Item -Force -Recurse coverage/ -ErrorAction SilentlyContinue
New-Item -Force -Path coverage/ -ItemType Directory | Out-Null
lcov -c --directory workspace -o coverage/report.info --gcov-tool $Tool
lcov -r coverage/report.info "*vendor*" "*vcpkg_installed*" "/usr*" "*engine/tests*" "*editor/tests*" -o coverage/report.info

if ($p0 -eq 'html') {
    genhtml coverage/report.info --output-directory coverage/ --prefix $(Get-Location)
}
