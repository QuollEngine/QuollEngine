function copyRuntime()
  -- Make sure that runtime is built
  -- before it can be copied to current
  -- project
  links { "Runtime" }

  runtimeTargetName = 'Runtime';
  runtimeTargetLocationRelativeToProjects = '../../workspace/runtime';
  postbuildcommands {
    "{COPYFILE} "..runtimeTargetLocationRelativeToProjects.."/bin/%{cfg.buildcfg}/"..getTargetExtension(runtimeTargetName).." %{cfg.buildtarget.directory}/"
  }
end

project "Runtime"
    basedir "../workspace/runtime"
    kind "ConsoleApp"
    

    includedirs {
        "./src"
    }

    loadSourceFiles{}
    linkDependenciesWith{"LiquidEngine", "LiquidEngineRHIVulkan", "LiquidEngineRHICore", "LiquidEnginePlatformTools" }

    -- This is for development only since runtime
    -- will always be copied from the editor
    copyEngineAssets()
