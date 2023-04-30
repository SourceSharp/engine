-- option will affect globally.
option("hl2sdk",
    function()
        set_default("")
        set_description("Determine which Source Engine you want to specify.")
        local version = get_config("hl2sdk")
        after_check(
            function(option)
                local ENGINE_NAME = {
                    ["l4d2"] = "Left 4 Dead 2",
                    ["insurgency"] = "Insurgency",
                    ["csgo"] = "Counter Strike: Global Offensive"
                }
                local sdk_engine = get_config("hl2sdk")
                if sdk_engine == "" or sdk_engine == false or sdk_engine == nil then
                    raise("You must specify \"--hl2sdk\" to define engine version first!")
                end
                local this_engine = ENGINE_NAME[sdk_engine]
                if this_engine == nil then
                    raise(string.format("Unknown Engine SDK version: \"%s\"", sdk_engine))
                end
                utils.cprint(string.format("Source Engine: ${bright green}%s${clear}", ENGINE_NAME[sdk_engine]))
             end
        )
    end
)

-- on_load -> after_load -> on_config -> before_build -> on_build -> after_build
target("sourcesharp",
    function()

        set_arch("x86")
        set_languages("c99", "cxx20")

        on_config(
            function(target)
                local mms_path = os.getenv("MMSOURCE") or os.getenv("METAMOD_SOURCE") or nil
                if mms_path == nil then
                    raise("Unable to find Metamod:Source environment variable. Check \"MMSOURCE\" or \"METAMOD_SOURCE\" first!")
                end
                mms_path = path.join(mms_path)

                target:add("includedirs",
                    path.join(mms_path, "core"),
                    path.join(mms_path, "core", "sourcehook")
                )

                --[[
                    我们通过--hl2sdk确定引擎版本以及后续流程
                    1. 为空时
                        编译所有可用引擎版本
                        只搜索HL2SDK

                    2. 不为空时
                        只编译选定的引擎版本
                        只搜索参数传入的引擎版本, 即HL2SDK_{传入的引擎版本} 或 (Windows specific)HL2SDK-{传入的引擎版本}
                            注: 在Windows中, 如果两个都存在, 默认只接受第一个
                        支持多个引擎版本传入
                        路径搜索遵循如下流程:
                            1. 将假设环境变量所指向的目录就是引擎源文件
                                1. 此时将搜索`${env:HL2SDK_{__engine_version__}}public/tier0`是否存在
                            2. 如果没找到 2.1.1) 所指定的路径, 则根据下文规则搜索源文件
                                遵守Git-branching命名约定 (eg: hl2sdk-csgo, hl2sdk-l4d2, hl2sdk-dota, ...)
                                eg:
                                    预设HL2SDK=D:/hl2sdk, 此时存在csgo, l4d2, dota
                                    此时将会搜索到:
                                            - D:/hl2sdk/hl2sdk-csgo
                                            - D:/hl2sdk/hl2sdk-l4d2
                                            - D:/hl2sdk/hl2sdk-dota
                ]]

                local sdk_engine = get_config("hl2sdk")

                local SDK_TABLE = {
                    ["csgo"] = {env = sdk_engine, code = 21, ext = "2.csgo", path = "csgo"}
                }
                local this_sdk = SDK_TABLE[sdk_engine]

                target:add("defines", string.format("SOURCE_ENGINE=%s", this_sdk.code), "NETWORK_VARS_ENABLED")
                if is_plat("windows") then
                    if (target:has_tool("cxx", "cl")) == false then
                        raise("Only accept MSVC!")
                    end

                    target:add("includedirs", "xmake/msvc")

                    target:add("defines",
                        "WIN32", "_WINDOWS",
                        "_CRT_SECURE_NO_DEPRECATE",
                        "_CRT_SECURE_NO_WARNINGS",
                        "_CRT_NONSTDC_NO_DEPRECATE",
                        "_ITERATOR_DEBUG_LEVEL=0",
                        "COMPILER_MSVC", "COMPILER_MSVC32"
                    )
                    target:add("cflags", "/W3", "/Oy-")
                    target:add("cxxflags",
                        "/EHsc", "/GR-", "/TP", "/wd4819", "/wd4828", "/wd5033", "/wd4005"
                    )
                    target:add("ldflags",
                        "/MACHINE:X86", "kernel32.lib", "user32.lib",
                        "gdi32.lib", "winspool.lib", "comdlg32.lib",
                        "advapi32.lib", "shell32.lib", "ole32.lib",
                        "oleaut32.lib", "uuid.lib", "odbc32.lib", "odbccp32.lib"
                    )
                    target:add("links", "legacy_stdio_definitions")

                    if is_mode("debug") then
                        target:add("cflags", "/MTd", "/Od", "/RTC1")
                        target:add("ldflags", "/NODEFAULTLIB:libcmt")
                        target:add("defines", "DEBUG", "_DEBUG")
                    else
                        target:add("cflags", "/MT")
                    end
                end

                local sdk_path = path.join(os.getenv("HL2SDK"), 'hl2sdk-' .. this_sdk.env)
                utils.cprint("SDK Path: ${bright green}" .. sdk_path .. "${clear} ")

                -- cuz valve's game always have tier0, and tier0 can be found at `public/tier0`
                -- we can simply check whether this folder exists to know whether we are in root sdk folder.
                local ConfigureEngineIncludes = function(sdk_path)
                    target:add("includedirs",
                        path.join(sdk_path, "public"),
                        path.join(sdk_path, "public", "engine"),
                        path.join(sdk_path, "public", "mathlib"),
                        path.join(sdk_path, "public", "vstdlib"),
                        path.join(sdk_path, "public", "tier0"),
                        path.join(sdk_path, "public", "tier1"),
                        path.join(sdk_path, "public", "game", "server"),
                        path.join(sdk_path, "game", "shared"),
                        path.join(sdk_path, "common")
                    )
                    target:add("linkdirs", path.join(sdk_path, "lib", "public"))
                    target:add("links", 'tier0', 'tier1', 'vstdlib', 'mathlib', 'interfaces')
                end

                if os.isdir(path.join(sdk_path, "public", "tier0")) then
                    ConfigureEngineIncludes(sdk_path)
                else
                    sdk_path = path.join(sdk_path, string.format("hl2sdk-%s", sdk_engine))
                    if os.isdir(sdk_path) == false then
                        raise(string.format("Invalid SDK path: \"%s\"", sdk_path))
                    end
                    ConfigureEngineIncludes(sdk_path)
                end

                -- better not hard-coding here, perhaps we could found a way to add it.
                -- 先硬编码
                local ss_dir = path.join(os.getenv("SOURCESHARP"), "runtime", ".deploy")
                utils.cprint("Runtime Path: ${bright green}" .. ss_dir .. "${clear}")
                target:add("includedirs", ss_dir)
                target:add("linkdirs", ss_dir)
                target:add("links", "SourceSharp.Runtime")
            end
        )

        set_kind("shared")
        add_files("src/*.cpp")
        add_includedirs("src")
        add_includedirs("include")

    end
)
