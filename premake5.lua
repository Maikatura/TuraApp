-- premake5.lua
workspace "TuraApp"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "TuraApp"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include "TuraExternal.lua"
include "TuraApp"