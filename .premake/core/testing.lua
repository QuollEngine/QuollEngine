function setupTestingOptions()
    filter { "toolset:clang" }
        buildoptions {
            "-fprofile-instr-generate",
            "-fcoverage-mapping"
        }

        linkoptions {
            "-fprofile-instr-generate",
            "-fcoverage-mapping"
        }

    filter { "toolset:gcc" }
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
