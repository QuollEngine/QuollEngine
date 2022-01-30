-- Load all source and shader files into project
function loadSourceFiles()
    files {
        "src/**.cpp",
        "src/**.h",
        "assets/**.vert",
        "assets/**.frag"
    }
end
