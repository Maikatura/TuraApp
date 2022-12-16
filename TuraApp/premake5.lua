project "TuraApp"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++17"
   targetdir "../bin/"
   staticruntime "off"

   files { "src/**.h", "src/**.cpp", "src/**.hpp"  }
   debugdir "../bin/"
   

   includedirs
   {
      "./src",
	  "../vendor/imgui",

      "../Tura/src"
   }
   

    links
    {
		"ImGui",
        "Tura",
		"d3d11.lib",
		"dxguid.lib"
    }

   objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")

   filter "system:windows"
      systemversion "latest"
      defines { "WL_PLATFORM_WINDOWS" }

   filter "configurations:Debug"
      defines { "WL_DEBUG" }
      runtime "Debug"
      symbols "On"

   filter "configurations:Release"
      defines { "WL_RELEASE" }
      runtime "Release"
      optimize "On"
      symbols "On"

   filter "configurations:Dist"
      kind "WindowedApp"
      defines { "WL_DIST" }
      runtime "Release"
      optimize "On"
      symbols "Off"