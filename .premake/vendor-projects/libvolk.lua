project "vendor-libvolk"
    basedir "../../vendor/projects/volk"
    kind "StaticLib"

    files {
        "../../vendor/projects/volk/*.c",
    }

    filter { "system:windows" }
        defines {
            "VK_USE_PLATFORM_WIN32_KHR"
        }

    filter { "system:macosx" }
        defines {
            "VK_USE_PLATFORM_MACOS_MVK",
            "VK_USE_PLATFORM_METAL_EXT"
        }

    filter { "system:linux" }
        defines {
            "VK_USE_PLATFORM_XLIB_KHR"
        }

    filter{}
