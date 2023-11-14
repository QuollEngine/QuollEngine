-- Setup toolset specific options
function setupToolsetOptions()
    filter { "system:linux or system:macosx" }
        toolset "clang"
    filter {}

    filter { "system:macosx" }
        xcodebuildsettings {
            ["MACOSX_DEPLOYMENT_TARGET"] = "12.0",
            ["ONLY_ACTIVE_ARCH"] = "YES",
            ["ENABLE_TESTABILITY"] = "YES",
            ["ARCHS"] = "$(ARCHS_STANDARD)",
            ["CLANG_ENABLE_OBJC_WEAK"] = "YES",
            ["CLANG_WARN_BLOCK_CAPTURE_AUTORELEASING"] = "YES",
            ["CLANG_WARN_BOOL_CONVERSION"] = "YES",
            ["CLANG_WARN_COMMA"] = "YES",
            ["CLANG_WARN_CONSTANT_CONVERSION"] = "YES",
            ["CLANG_WARN_DEPRECATED_OBJC_IMPLEMENTATIONS"] = "YES",
            ["CLANG_WARN_EMPTY_BODY"] = "YES",
            ["CLANG_WARN_ENUM_CONVERSION"] = "YES",
            ["CLANG_WARN_INFINITE_RECURSION"] = "YES",
            ["CLANG_WARN_INT_CONVERSION"] = "YES",
            ["CLANG_WARN_NON_LITERAL_NULL_CONVERSION"] = "YES",
            ["CLANG_WARN_OBJC_IMPLICIT_RETAIN_SELF"] = "YES",
            ["CLANG_WARN_OBJC_LITERAL_CONVERSION"] = "YES",
            ["CLANG_WARN_QUOTED_INCLUDE_IN_FRAMEWORK_HEADER"] = "YES",
            ["CLANG_WARN_RANGE_LOOP_ANALYSIS"] = "YES",
            ["CLANG_WARN_STRICT_PROTOTYPES"] = "YES",
            ["CLANG_WARN_SUSPICIOUS_MOVE"] = "YES",
            ["CLANG_WARN_UNREACHABLE_CODE"] = "YES",
            ["CLANG_WARN__DUPLICATE_METHOD_MATCH"] = "YES",
            ["GCC_WARN_64_TO_32_BIT_CONVERSION"] = "YES",
            ["GCC_WARN_ABOUT_RETURN_TYPE"] = "YES",
            ["GCC_WARN_UNDECLARED_SELECTOR"] = "YES",
            ["GCC_WARN_UNINITIALIZED_AUTOS"] = "YES",
            ["GCC_WARN_UNUSED_FUNCTION"] = "YES",
            ["GCC_WARN_UNUSED_VARIABLE"] = "YES",
            ["GCC_NO_COMMON_BLOCKS"] = "YES",
            ["ENABLE_STRICT_OBJC_MSGSEND"] = "YES",
        }

    filter { "system:macosx", "configurations:Debug or configurations:Test" }
       xcodebuildsettings {
            ["CLANG_ENABLE_OBJC_WEAK"] = "YES",
            ["CODE_SIGN_IDENTITY"] = "-"
        }

    -- Sets working directory for Visual Studio
    -- so that, applications use the same path
    -- as if we were to open exe file directly
    filter { "toolset:msc-*" }
        debugdir "$(TargetDir)"
        flags { "MultiProcessorCompile" }
        defines { "_CRT_SECURE_NO_WARNINGS" }

    filter{}

    -- Disable edit and continue feature
    -- in all toolset
    editandcontinue "Off"
end
