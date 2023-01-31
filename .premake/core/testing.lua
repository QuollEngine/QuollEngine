function setupTestingOptions()
    filter { "configurations:Test", "toolset:clang" }
        buildoptions {
            "-fprofile-instr-generate",
            "-fcoverage-mapping"
        }

        linkoptions {
            "-fprofile-instr-generate",
            "-fcoverage-mapping"
        }

    filter { "configurations:Test", "toolset:gcc" }
        buildoptions {
            "-fprofile-arcs",
            "-ftest-coverage"
        }

        linkoptions {
            "-fprofile-arcs",
            "-ftest-coverage"
        }
    filter{}
end
