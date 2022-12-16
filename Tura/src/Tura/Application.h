#pragma once

#include "Layer.h"

#include <string>
#include <vector>
#include <memory>
#include <functional>

#include "Framework/DX11.h"


namespace Tura
{

	struct ApplicationSpecification
	{
		std::wstring Name = L"Tura App";
		uint32_t Width = 1600;
		uint32_t Height = 900;
		bool VSync = false;
		bool DebugMode = false;
	};

	class Application
	{
		SIZE myWindowSize{ 0,0 };
		HWND myWindowHandle{};

		ApplicationSpecification mySpecification;
		bool myRunning = false;
		bool myWantToResizeBuffers = false;
		bool myIsMinimized = false;

		float myTimeStep = 0.0f;
		float myFrameTime = 0.0f;
		float myLastFrameTime = 0.0f;


		std::vector<std::shared_ptr<Layer>> myLayerStack;
		std::function<void()> myMenubarCallback;

	public:
		Application(const ApplicationSpecification& applicationSpecification = ApplicationSpecification());
		~Application();

		static Application& Get();

		void Run();
		void SetMenubarCallback(const std::function<void()>& menubarCallback) { myMenubarCallback = menubarCallback; }

		template<typename T>
		void PushLayer()
		{
			static_assert(std::is_base_of<Layer, T>::value, "Pushed type is not subclass of Layer!");
			myLayerStack.emplace_back(std::make_shared<T>())->OnAttach();
		}

		void PushLayer(const std::shared_ptr<Layer>& layer) { myLayerStack.emplace_back(layer); layer->OnAttach(); }

		void Close();

		float GetTime();
		
		void OnUpdate();

		[[nodiscard]] HWND FORCEINLINE GetWindowHandle() const { return myWindowHandle; }
		[[nodiscard]] SIZE FORCEINLINE GetWindowSize() const { return myWindowSize; }
		[[nodiscard]] void FORCEINLINE SetWindowSize(int aX, int aY) { myWindowSize.cx = aX; myWindowSize.cy = aY; }

	private:

		static LRESULT CALLBACK WinProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);
		void Init(unsigned someX, unsigned someY, unsigned someWidth, unsigned someHeight, bool enableDeviceDebug, std::wstring aName = L"GamerEngine");
		void Shutdown();
		void SetMinimized(bool aMinimizedState);
		void SetUpdateBuffers(bool aUpdate);
	};

	// Implemented by CLIENT
	Application* CreateApplication(int argc, char** argv);
}