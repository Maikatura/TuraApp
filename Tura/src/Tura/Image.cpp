#include "Image.h"

#include "imgui.h"

#include "Application.h"

#define STB_IMAGE_IMPLEMENTATION
#include <iostream>

namespace Tura
{

	namespace Utils
	{

		

	}

	Image::Image(const std::wstring& path)
		: m_Filepath(path)
	{

		if (TextureAssetHandler::FindTexture(path))
		{
			myTexture = TextureAssetHandler::GetTexture(path);
			myReadyToUse = true;
		}
	}

	Image::Image(uint32_t width, uint32_t height, const void* data)
		: m_Width(width), m_Height(height)
	{
		
	}

	Image::~Image()
	{
		Release();
	}

	void Image::AllocateMemory(uint64_t size)
	{
		
	}

	void Image::Release()
	{
		
	}

	void Image::SetData(const void* data)
	{
		
	}

	bool Image::ReadyToUse()
	{
		if (myReadyToUse && ImGui::IsItemVisible())
		{
			return true;
		}

		return false;
	}

	void Image::Resize(uint32_t width, uint32_t height)
	{

	}

}