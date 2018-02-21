workspace "gmcl_razer"
    configurations { "Debug", "Release" }
    location ( "projects/" .. os.target() )

project "gmcl_razer"
    kind         "SharedLib"
    architecture "x86"
    language     "C++"
    includedirs  "../include/"
    targetdir    "build"
    symbols      "On"
    
    if os.host() == "windows" then targetsuffix "_win32" end
    if os.host() == "macosx" then targetsuffix "_osx"   end
    if os.host() == "linux"  then targetsuffix "_linux" end

    configuration "Debug"
        optimize "Debug"

    configuration "Release"
        optimize "Speed"
        flags    "StaticRuntime"

    files
    {
        "src/**.*",
        "../include/**.*"
    }