function setupTestingOptions()
    filter { "configurations:Test", "toolset:clang" }
        buildoptions {
            "-fprofile-arcs",
	        "-ftest-coverage"
        }

        linkoptions {
    	  "-fprofile-arcs",
	      "-ftest-coverage"
        }

	    links { "gcov" }

    filter{}
end
