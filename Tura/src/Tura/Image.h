#pragma once

#include <memory>
#include <string>
#include <wrl/client.h>

#include "Texture.h"

class Texture;

namespace Tura
{

	enum class ImageFormat
	{
		None = 0,
		RGBA,
		RGBA32F
	};

	class Image
	{
	public:
		Image(const std::wstring& path);
		Image(uint32_t width, uint32_t height, const void* data = nullptr);
		~Image();

		void SetData(const void* data);

		bool ReadyToUse();

		Microsoft::WRL::ComPtr<struct ID3D11ShaderResourceView> Get() const { return myTexture->GetSRV(); }

		void Resize(uint32_t width, uint32_t height);

		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }
	private:
		void AllocateMemory(uint64_t size);
		void Release();
	private:
		uint32_t m_Width = 0, m_Height = 0;

		std::shared_ptr<Texture> myTexture;

		bool myReadyToUse = false;
		std::wstring m_Filepath;
	};

}



