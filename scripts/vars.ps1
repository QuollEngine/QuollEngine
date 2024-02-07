if ($IsLinux) {
    $Global:PlatformHeaders = @(
        '/usr/local/include',
        './vcpkg_installed/x64-linux/include',
        './vcpkg_installed/x64-linux/include/msdfgen'
    )
    $Global:AvailableNumThreads = $(nproc)
}
elseif ($IsWindows) {
    $Global:VulkanSDK = Join-Path -Path $Env:VULKAN_SDK -ChildPath "Include"
    $Global:PlatformHeaders = @(
        $VulkanSDK,
        './vcpkg_installed/x64-windows-static/include',
        './vcpkg_installed/x64-windows-static/include/msdfgen'
    )
    $Global:AvailableNumThreads = $Env:NUMBER_OF_PROCESSORS
}
