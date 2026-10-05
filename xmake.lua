-- include subprojects
includes("lib/commonlibsse-ng")
add_requires("simpleini")

-- set project
set_project("ManualCrossbowReloading")
set_version("2.1.2")
set_license("GPL-3.0")

-- set defaults
set_languages("c++23")
set_warnings("allextra")

-- add common rules
add_rules("mode.debug", "mode.releasedbg")
add_rules("plugin.vsxmake.autoupdate")

-- define targets
target("ManualCrossbowReloading")
    add_deps("commonlibsse-ng")
    add_packages("simpleini")

    add_rules("commonlibsse-ng.plugin", {
        name = "ManualCrossbowReloading",
        author = "August",
        description = "SKSE64 plugin using CommonLibSSE-NG"
    })

    -- add src files
    add_files("src/**.cpp")
    add_headerfiles("src/**.h")
    add_headerfiles("extern/**.h")
    add_includedirs("src")
    set_pcxxheader("src/pch.h")