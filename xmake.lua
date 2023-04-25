local hl2sdk_path = "D:/source/hl2sdk-csgo"
local mms_path = "D:/source/metamod-source"

target("sourcesharp")
    set_languages("c99", "cxx20")

    set_kind("shared")
    add_files("src/*.cpp")
    add_includedirs("src")
    add_includedirs("include")

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

    -- HL2SDK
    add_includedirs(mms_path .. "/core", mms_path .. "/core/sourcehook")
    add_includedirs(hl2sdk_path .. "/public")
    add_includedirs(hl2sdk_path .. "/public/engine")
    add_includedirs(hl2sdk_path .. "/public/mathlib")
    add_includedirs(hl2sdk_path .. "/public/vstdlib")
    add_includedirs(hl2sdk_path .. "/public/tier0")
    add_includedirs(hl2sdk_path .. "/public/tier1")
    add_includedirs(hl2sdk_path .. "/public/game/server")
    add_includedirs(hl2sdk_path .. "/public/toolframework")
    add_includedirs(hl2sdk_path .. "/game/shared")
    add_includedirs(hl2sdk_path .. "/common")
    add_defines("SOURCE_ENGINE=21")

        -- MSVC
    add_defines("COMPILER_MSVC", "COMPILER_MSVC32")

    add_defines("NETWORK_VARS_ENABLED")

    add_linkdirs(hl2sdk_path .. "/lib/public")
    add_links('tier0', 'tier1', 'vstdlib', 'mathlib', 'interfaces')

    -- win need
    add_links('legacy_stdio_definitions')

target_end()
