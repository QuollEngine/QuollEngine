function setupTestingOptions()
    filter { "configurations:Test", "toolset:clang" }
        buildoptions {
            "-fprofile-instr-generate",
	        "-fcoverage-mapping"
        }

        linkoptions {
    	  "-fprofile-instr-generate"
        }

    filter{}
end
