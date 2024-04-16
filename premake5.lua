include("include/blf")

workspace "schetool"
	configurations { "Debug", "Release" }
	startproject "schetool"
	
-- Clean Function --
newaction {
	trigger = "clean",
	description = "clean build remnants",
	execute = function ()
		print("Cleaning build files and directories")
		os.rmdir("./obj")
		os.rmdir("./schetool")
		os.rmdir("./Debug")
		os.rmdir("./Release")
		os.rmdir("./bin")
		---- libraries
		print("Cleaning libraries")
		-- BlurLevelFormat
		print("Cleaning BlurLevelFormat (blf)")
		os.rmdir("./include/blf/bin")
		os.rmdir("./include/blf/obj")
		os.remove("./include/blf/Makefile")
		os.remove("./include/blf/BlurLevelFormat.make")
		os.remove("include/zlib/zlib.vcxproj")
		os.remove("schetool.pdb")
		-- zlib
		print("Cleaning zlib")
		os.remove("include/blf/blf.vcxproj")
		--os.remove("./include/zlib/Makefile") TODO
	end
}

project "schetool"
	location "schetool"
	
	filter "configurations:Debug"
		kind "ConsoleApp"
		defines { "DEBUG" }
		symbols "On"
	filter "configurations:Release"
		kind "WindowedApp"
		defines { "NDEBUG" }
		optimize "On"
	filter {}
		
	language "C++"
	cppdialect "C++17"
	staticruntime "on"
	--architecture "x86_64"
	
	targetdir "."
	objdir "obj/%{cfg.buildcfg}"
	
	filter { "system:windows", "action:gmake2" }
		linkoptions{ "-m64", "-static", "-mtune=native", "-lpthread", "-static-libstdc++", "-static-libgcc", "-lstdc++" }
	filter {}
	
	files 
	{ 
		"source/**.cpp",
		"include/*.h"
	}
	
	includedirs 
	{ 
		"include/", 
		"include/blf/include",
		"include/blf/include/blf",
		"include/glad/include", 
		"include/glad/include/glad",
		"include/glew",
		"include/glfw/include", 
		"include/glm",
		"include/imgui",
		"include/rectpack2D/include",
		"include/zlib"
	}
	
	defines
	{
		"GLFW_INCLUDE_NONE"
	}
	
	links
	{
		"BlurLevelFormat",
		"glad",
		"glfw",
		"imgui",
		"zlib",
	}

	filter "system:not linux"
		links
		{
			"gdi32"
		}

	filter "system:linux"
		targetextension "_bin"
		links
		{
			"X11",
		}

group "Dependencies"

externalproject "BlurLevelFormat"
   location "include/blf"
   kind "StaticLib"
   language "C++"

project "glad"
	location "include/glad"
	kind "StaticLib"
	language "C"
	staticruntime "on"
	
	targetdir "bin/%{cfg.buildcfg}"
	objdir "obj/%{cfg.buildcfg}"

	files 
	{ 
		"%{prj.location}/src/glad.c",
		"%{prj.location}/include/KHR/khrplatform.h"
	}
	
	includedirs 
	{ 
		"%{prj.location}/include",
		"%{prj.location}/include/KHR"
	}

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"

project "glfw"
	location "include/glfw"
	kind "StaticLib"
	language "C"
	staticruntime "on"

	targetdir "bin/%{cfg.buildcfg}"
	objdir "obj/%{cfg.buildcfg}"
	
	files
	{
		"%{prj.location}/src/context.c",
		"%{prj.location}/src/egl_context.c",
		"%{prj.location}/src/egl_context.h",
		"%{prj.location}/src/init.c",
		"%{prj.location}/src/input.c",
		"%{prj.location}/src/internal.h",
		"%{prj.location}/src/monitor.c",
		"%{prj.location}/src/osmesa_context.c",
		"%{prj.location}/src/osmesa_context.h",
		"%{prj.location}/src/vulkan.c",
		"%{prj.location}/src/window.c"
	}

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"
		
	filter "system:windows"
		files
		{
			"%{prj.location}/src/wgl_context.c",
			"%{prj.location}/src/wgl_context.h",
			"%{prj.location}/src/win32_init.c",
			"%{prj.location}/src/win32_joystick.c",
			"%{prj.location}/src/win32_joystick.h",
			"%{prj.location}/src/win32_monitor.c",
			"%{prj.location}/src/win32_platform.h",
			"%{prj.location}/src/win32_thread.c",
			"%{prj.location}/src/win32_time.c",
			"%{prj.location}/src/win32_window.c"
		}

		defines 
		{ 
			"_GLFW_WIN32",
			"_CRT_SECURE_NO_WARNINGS"
		}
	
	filter "system:linux"
		files
		{
			"%{prj.location}/src/glx_context.c",
			"%{prj.location}/src/glx_context.h",
			"%{prj.location}/src/linux_joystick.c",
			"%{prj.location}/src/linux_joystick.h",
			"%{prj.location}/src/posix_time.c",
			"%{prj.location}/src/posix_time.h",
			"%{prj.location}/src/posix_thread.c",
			"%{prj.location}/src/posix_thread.h",
			"%{prj.location}/src/x11_init.c",
			"%{prj.location}/src/x11_monitor.c",
			"%{prj.location}/src/x11_platform.h",
			"%{prj.location}/src/x11_window.c",
			"%{prj.location}/src/xkb_unicode.c",
			"%{prj.location}/src/xkb_unicode.h"
		}

		defines 
		{ 
			"_GLFW_X11"
		}
		
	filter "system:macosx"
		files
		{
			"%{prj.location}/src/cocoa_init.m",
			"%{prj.location}/src/cocoa_joystick.m",
			"%{prj.location}/src/cocoa_joystick.h",
			"%{prj.location}/src/cocoa_monitor.m",
			"%{prj.location}/src/cocoa_platform.h",
			"%{prj.location}/src/cocoa_time.c",
			"%{prj.location}/src/cocoa_window.m",
			"%{prj.location}/src/nsgl_context.m",
			"%{prj.location}/src/nsgl_context.h",
			"%{prj.location}/src/posix_thread.c",
			"%{prj.location}/src/posix_thread.h"
		}

		defines
		{ 
			"_GLFW_COCOA"
		}
		
project "imgui"
	location "libs/imgui"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir "bin/%{cfg.buildcfg}"
	objdir "obj/%{cfg.buildcfg}"
	
	files 
	{ 
		"%{prj.location}/*.cpp"
	}
	
	includedirs
	{ 
		"../../include/imgui/",
		"include/imgui",
		"include/glad/include",
		"include/glfw/include/" 
	}

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"
		
project "zlib"
	location "include/zlib"
	kind "StaticLib"
	language "C"
	staticruntime "on"

	targetdir "bin/%{cfg.buildcfg}"
	objdir "obj/%{cfg.buildcfg}"

	files
	{
		"%{prj.location}/adler32.c",
		"%{prj.location}/compress.c",
		"%{prj.location}/crc32.c",
		"%{prj.location}/deflate.c",
		"%{prj.location}/gzclose.c",
		"%{prj.location}/gzlib.c",
		"%{prj.location}/gzread.c",
		"%{prj.location}/gzwrite.c",
		"%{prj.location}/inflate.c",
		"%{prj.location}/infback.c",
		"%{prj.location}/inftrees.c",
		"%{prj.location}/inffast.c",
		"%{prj.location}/trees.c",
		"%{prj.location}/uncompr.c",
		"%{prj.location}/zutil.c"
	}

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"
