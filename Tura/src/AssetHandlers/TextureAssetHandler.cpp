#include <iostream>
#include "TextureAssetHandler.h"

#include "DDSTextureLoader11.h"
#include "WICTextureLoader11.h"

#include "Tura/Texture.h"

#include "Framework/DX11.h"
#include <filesystem>
#include <fstream>


void TextureAssetHandler::Clear()
{
	myRegistry.clear();
}

std::shared_ptr<Texture> TextureAssetHandler::GetTexture(const std::wstring & aName)
{
	auto model = myRegistry.find(aName);
	return model->second;
}

bool TextureAssetHandler::FindTexture(const std::wstring& aName)
{
	auto model = myRegistry.find(aName);
	if (model != myRegistry.end())
	{
		return true;
	}

	LoadTexture(aName);
	model = myRegistry.find(aName);
	if(model != myRegistry.end())
	{
		return true;
	}

	return false;
}

bool TextureAssetHandler::LoadTexture(const std::wstring& aFileName)
{
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	if (const auto It = myRegistry.find(aFileName); It == myRegistry.end())
	{

		std::filesystem::path filename = aFileName;

		Texture result;
		HRESULT createResult = S_FALSE;
		if (filename.extension() == ".dds")
		{
			createResult = DirectX::CreateDDSTextureFromFile(DX11::Device.Get(), aFileName.c_str(),
				result.myTexture.GetAddressOf(),
				result.mySRV.GetAddressOf()
			);
		}
		else
		{
			createResult = DirectX::CreateWICTextureFromFile(DX11::Device.Get(), aFileName.c_str(),
				result.myTexture.GetAddressOf(),
				result.mySRV.GetAddressOf()
			);
		}

		


		std::filesystem::path path = aFileName;
		result.SetName(path.filename());

		if(SUCCEEDED(createResult))
		{
			myRegistry.insert({ aFileName, std::make_shared<Texture>(result) });
		}
	}

	return false;
}
