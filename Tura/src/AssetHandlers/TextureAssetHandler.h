#pragma once
#include <unordered_map>
#include <memory>
#include <d3d11.h>

class Texture;

class TextureAssetHandler
{
	typedef std::unordered_map<std::wstring, std::shared_ptr<Texture>> TextureMap;

	inline static TextureMap myRegistry{};

public:

	static void Clear();

	static std::shared_ptr<Texture> GetTexture(const std::wstring& aName);

	static bool FindTexture(const std::wstring& aName);
	static bool LoadTexture(const std::wstring& aFileName);
};
