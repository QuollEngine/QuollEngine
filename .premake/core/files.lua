-- Load all source and shader files into project
function loadSourceFiles(src, assets)
    assetsPath = assets or "assets"

    files {
        src .. "/**.cpp",
        src .. "/**.h",
        assetsPath .. "/**.vert",
        assetsPath .. "/**.frag"
    }
end
