-- Define HL2SDK table first.
local HL2SDK_TABLE = {
    ["cs2"] = {env = "HL2SDK_CS2", code = 21, macro = "CS2"}
}
-- I think xmake should have a way to take out specific table, but passing in params.
-- If you want to use xmake to gen cmake, then probably you should add CMakeLists.txt to gitignore.
-- But probably we could xmake build directly?

local hl2sdk_path = path.join(os.getenv("HL2SDK"), "hl2sdk-csgo") -- further works required
local mms_path = os.getenv("MMSOURCE") or os.getenv("METAMOD_SOURCE") or nil

target("sourcesharp", 
    function()
        set_languages("c99", "cxx20")

        set_kind("shared")
        add_files("src/*.cpp")
        add_includedirs("src")
        add_includedirs("include")

        -- better not hard-coding here, perhaps we could found a way to add it.
        add_includedirs("D:/SourceSharp/runtime/Core/bin/x86/Release/net7.0/win-x86")
        add_linkdirs("D:/SourceSharp/runtime/Core/bin/x86/Release/net7.0/win-x86")
        add_links("SourceSharp.Runtime")

        -- msvc
        if is_mode("debug") then
            add_cflags("/MTd")
            add_ldflags("/NODEFAULTLIB:libcmt")
        else
            add_cflags("/MT")
        end

        add_defines("_CRT_SECURE_NO_DEPRECATE", "_CRT_SECURE_NO_WARNINGS", "_CRT_NONSTDC_NO_DEPRECATE", "_ITERATOR_DEBUG_LEVEL=0")
        add_cflags("/W3")
        add_cxxflags("/EHsc", "/GR-", "/TP")
        add_ldflags("/MACHINE:X86", "kernel32.lib", "user32.lib",
                "gdi32.lib", "winspool.lib", "comdlg32.lib",
                "advapi32.lib", "shell32.lib", "ole32.lib",
                "oleaut32.lib", "uuid.lib", "odbc32.lib", "odbccp32.lib")

        if is_mode("debug") then
            add_cflags("/Od", "/RTC1")
        end

        add_cflags("/Oy-")

        -- Optimization

        -- Debugging
        if is_mode("debug") then
            add_defines("DEBUG", "_DEBUG")
        end

        -- Windows
        add_defines("WIN32", "_WINDOWS")

        -- Include all necessary directories.
        
        -- metamod
        add_includedirs(
            path.join(mms_path, "core"),
            path.join(mms_path, "core", "sourcehook")
        )
        -- hl2sdk
        add_includedirs(
            path.join(hl2sdk_path, "public"),
            path.join(hl2sdk_path, "public", "engine"),
            path.join(hl2sdk_path, "public", "mathlib"),
            path.join(hl2sdk_path, "public", "vstdlib"),
            path.join(hl2sdk_path, "public", "tier0"),
            path.join(hl2sdk_path, "public", "tier1"),
            path.join(hl2sdk_path, "public", "game", "server"),
            path.join(hl2sdk_path, "game", "shared"),
            path.join(hl2sdk_path, "common")
        )
        -- cs2 define is 21? iirc this is used for csgo
        -- nvm, my advice is remove everything of old stuff
        add_defines("SOURCE_ENGINE=21")

        -- MSVC
        add_defines("COMPILER_MSVC", "COMPILER_MSVC32")

        add_defines("NETWORK_VARS_ENABLED")

        add_linkdirs(path.join(hl2sdk_path, "lib", "public"))
        add_links('tier0', 'tier1', 'vstdlib', 'mathlib', 'interfaces')

        -- win need
        add_links('legacy_stdio_definitions')

    end
)
