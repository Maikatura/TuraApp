#include "Application.h"

#include "../Framework/DX11.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx11.h"
#include <imgui.h>
#include <stdio.h>          // printf, fprintf
#include <stdlib.h>         // abort
#include <iostream>

// Emedded font
#include <d3d11.h>

#include "ImGui/Roboto-Regular.embed"

extern bool g_ApplicationRunning;

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

//#define IMGUI_UNLIMITED_FRAME_RATE
#ifdef _DEBUG
#define IMGUI_VULKAN_DEBUG_REPORT
#endif

static Tura::Application* s_Instance = nullptr;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


namespace Tura
{

	Application::Application(const ApplicationSpecification& specification)
		: mySpecification(specification)
	{
		s_Instance = this;

		Init((GetSystemMetrics(SM_CXSCREEN) - specification.Width) / 2,
			(GetSystemMetrics(SM_CYSCREEN) - specification.Height) / 2,
			specification.Width,
					specification.Height,
			mySpecification.DebugMode,
			specification.Name);
	}

	Application::~Application()
	{
		Shutdown();

		s_Instance = nullptr;
	}

	Application& Application::Get()
	{
		return *s_Instance;
	}

	void Application::Init(unsigned someX, unsigned someY, unsigned someWidth, unsigned someHeight, bool enableDeviceDebug, std::wstring aName)
	{

		WNDCLASS windowClass = {};
		windowClass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
		windowClass.lpfnWndProc = Application::WinProc;
		windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
		windowClass.lpszClassName = L"TGP";
		RegisterClass(&windowClass);


		myWindowHandle = CreateWindow(
			L"TGP",
			aName.c_str(),
			WS_OVERLAPPEDWINDOW | WS_POPUP | WS_VISIBLE,
			someX,
			someY,
			someWidth,
			someHeight,
			nullptr, nullptr, nullptr,
			this
		);

		myWindowSize.cx = someWidth;
		myWindowSize.cy = someHeight;

		if(!DX11::Init(myWindowHandle, enableDeviceDebug))
		{
			return;
		}

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

		//ImGui::StyleColorsDark();

		ImGui_ImplWin32_Init(Application::Get().GetWindowHandle());
		ImGui_ImplDX11_Init(DX11::Device.Get(), DX11::Context.Get());

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		//io.ConfigViewportsNoAutoMerge = true;
		//io.ConfigViewportsNoTaskBarIcon = true;

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if(io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		// Setup Platform/Renderer backends
		

		// Load default font
		ImFontConfig fontConfig;
		fontConfig.FontDataOwnedByAtlas = false;
		ImFont* robotoFont = io.Fonts->AddFontFromMemoryTTF((void*)g_RobotoRegular, sizeof(g_RobotoRegular), 20.0f, &fontConfig);
		io.FontDefault = robotoFont;
	}

	void Application::Shutdown()
	{
		for(auto& layer : myLayerStack)
			layer->OnDetach();

		myLayerStack.clear();

		ImGui_ImplWin32_Shutdown();
		ImGui_ImplDX11_Shutdown();
		ImGui::DestroyContext();

		g_ApplicationRunning = false;
	}

	
	void Application::SetMinimized(bool aMinimizedState)
	{
		myIsMinimized = aMinimizedState;
	}

	void Application::SetUpdateBuffers(bool aUpdate)
	{
		myWantToResizeBuffers = aUpdate;
	}

	LRESULT CALLBACK Application::WinProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
	{
		// We want to be able to access the Graphics Engine instance from inside this function.
		static Application* graphicsEnginePtr = nullptr;

		if(ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
			return true;

		switch(uMsg)
		{
			case WM_CREATE:
			{
				const CREATESTRUCT* createdStruct = reinterpret_cast<CREATESTRUCT*>(lParam);
				graphicsEnginePtr = static_cast<Application*>(createdStruct->lpCreateParams);
			}

			case WM_SIZE:
			{
				if(DX11::Device != NULL)
				{
					graphicsEnginePtr->SetWindowSize((UINT)LOWORD(lParam), (UINT)HIWORD(lParam));

					switch(wParam)
					{
						case SIZE_MAXIMIZED:
							graphicsEnginePtr->SetUpdateBuffers(true);
							graphicsEnginePtr->SetMinimized(false);
							break;

						case SIZE_MINIMIZED:
							graphicsEnginePtr->SetMinimized(true);
							break;

						case SIZE_RESTORED:
							graphicsEnginePtr->SetUpdateBuffers(true);
							graphicsEnginePtr->SetMinimized(false);
							break;

						case SIZE_MAXSHOW:
							graphicsEnginePtr->SetUpdateBuffers(true);
							graphicsEnginePtr->SetMinimized(false);
							break;
					}
				}
			}

			case WM_EXITSIZEMOVE:
			{
				if(DX11::Device != NULL)
				{
					graphicsEnginePtr->SetUpdateBuffers(true);
				}
			}

			case WM_SYSCOMMAND:
			{
				if((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
				{
					return 0;
				}
				break;
			}



			case WM_DESTROY:
			{
				PostQuitMessage(0);
			}

			case WM_DROPFILES:
			{
				std::cout << "Dropped something\n";
				break;
			}

			case WM_QUIT:
			{
				std::cout << "Test\n";
				break;
			}

			case WM_CLOSE:
			{
				std::cout << "Dropped something\n";
				break;
			}

		}

		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	void Application::Run()
	{
		myRunning = true;


		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		ImGuiIO& io = ImGui::GetIO();

		MSG msg = { 0 };

		// Main loop
		while(myRunning)
		{
			// Poll and handle events (inputs, window resize, etc.)
			// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
			// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
			// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
			// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.

			while(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);

				if(msg.message == WM_QUIT)
				{
					
					myRunning = false;
				}
			}

			if(!myRunning)
			{
				break;
			}
			
			for(auto& layer : myLayerStack)
				layer->OnUpdate(myTimeStep);

			// Start the Dear ImGui frame
			ImGui_ImplWin32_NewFrame();
			ImGui_ImplDX11_NewFrame();
			ImGui::NewFrame();

			if(myWantToResizeBuffers)
			{
				if(DX11::SwapChain)
				{
					DX11::Resize();
				}
				myWantToResizeBuffers = false;
			}


			DX11::BeginFrame({ clear_color.x,clear_color.y, clear_color.z, clear_color.w });


			{
				static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

				// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
				// because it would be confusing to have two docking targets within each others.
				ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
				if(myMenubarCallback)
					window_flags |= ImGuiWindowFlags_MenuBar;

				const ImGuiViewport* viewport = ImGui::GetMainViewport();
				ImGui::SetNextWindowPos(viewport->WorkPos);
				ImGui::SetNextWindowSize(viewport->WorkSize);
				ImGui::SetNextWindowViewport(viewport->ID);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
				window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
				window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

				// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
				// and handle the pass-thru hole, so we ask Begin() to not render a background.
				if(dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
					window_flags |= ImGuiWindowFlags_NoBackground;

				// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
				// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
				// all active windows docked into it will lose their parent and become undocked.
				// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
				// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
				ImGui::Begin("DockSpace Demo", nullptr, window_flags);
				ImGui::PopStyleVar();

				ImGui::PopStyleVar(2);

				// Submit the DockSpace
				ImGuiIO& io = ImGui::GetIO();
				if(io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
				{
					ImGuiID dockspace_id = ImGui::GetID("VulkanAppDockspace");
					ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
				}

				if(myMenubarCallback)
				{
					if(ImGui::BeginMenuBar())
					{
						myMenubarCallback();
						ImGui::EndMenuBar();
					}
				}

				for(auto& layer : myLayerStack)
					layer->OnUIRender();

				ImGui::End();
			}


			// Rendering
			ImGui::Render();
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

			ImGuiIO& io = ImGui::GetIO();
			// Update and Render additional Platform Windows
			if(io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
			}

			float time = GetTime();
			myFrameTime = time - myLastFrameTime;
			myLastFrameTime = time;

			DX11::EndFrame();
		}

	}

	void Application::Close()
	{
		myRunning = false;
	}

	float Application::GetTime()
	{
		return 0.0f;
	}

}
