project "Engine"
   kind "WindowedApp"
   language "C++"
   cppdialect "C++20"
   staticruntime "off"

   files 
   { 
       "src/**.h", 
       "src/**.cpp",
       
        "../Vendor/imgui/*.h",
        "../Vendor/imgui/*.cpp",
        "../Vendor/imgui/backends/*.h",
        "../Vendor/imgui/backends/*.cpp"
   }

   includedirs
   {
      "src/",
      "../Vendor/stb/include",
      "../Vendor/assimp/include",
      "../Vendor/imgui",
      "../Vendor/imgui/backends"
   }

   links
   {
      "d3d12.lib",
      "dxgi.lib",
      "d3dcompiler.lib",
      "../Vendor/assimp/lib/assimp-vc143-mt.lib"
   }

   targetdir ("../Build/" .. OutputDir .. "/%{prj.name}")
   objdir ("../Build/Intermediates/" .. OutputDir .. "/%{prj.name}")
   
   postbuildcommands 
   {
      "{COPYDIR} assets ../Build/" .. OutputDir .. "/%{prj.name}/assets",
      "{COPYDIR} ../Vendor/assimp/bin ../Build/" .. OutputDir .. "/%{prj.name}"
   }

   filter "system:windows"
      systemversion "latest"
      defines { "WINDOWS" }

   filter "configurations:Debug"
      defines { "DEBUG" }
      runtime "Debug"
      symbols "On"
      links { "dxguid.lib" }

   filter "configurations:Release"
      defines { "RELEASE", "NDEBUG" }
      runtime "Release"
      optimize "On"
      symbols "On"

   filter "configurations:Dist"
      defines { "DIST", "NDEBUG" }
      runtime "Release"
      optimize "On"
      symbols "Off"
