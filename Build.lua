workspace "Engine"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "Engine"

   filter "system:windows"
      buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus", "/VERBOSE" }

OutputDir = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"

include "Engine/Build-Engine.lua"