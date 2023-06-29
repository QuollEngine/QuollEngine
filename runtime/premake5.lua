function copyRuntime()
  -- Make sure that runtime is built
  -- before it can be copied to current
  -- project
  links { "LiquidRuntime" }

  runtimeTargetName = 'LiquidRuntime';
  runtimeTargetLocationRelativeToProjects = '../../workspace/runtime';
  postbuildcommands {
    "{COPYFILE} "..runtimeTargetLocationRelativeToProjects.."/bin/%{cfg.buildcfg}/"..getTargetExtension(runtimeTargetName).." %{cfg.buildtarget.directory}/"
  }
end

project "LiquidRuntime"
    basedir "../workspace/runtime"
    kind "ConsoleApp"

    includedirs {
        "./src"
    }

    loadSourceFiles{}
    linkDependenciesWith{"LiquidEngine"}
    linkVulkanRHI{}
    linkOptick{}
    linkPlatform()

    -- This is for development only since runtime
    -- will always be copied from the editor
    copyEngineAssets()
