#pragma once

#ifdef WL_PLATFORM_WINDOWS

extern Tura::Application* Tura::CreateApplication(int argc, char** argv);
bool g_ApplicationRunning = true;

namespace Tura {

	int Main(int argc, char** argv)
	{
		while (g_ApplicationRunning)
		{
			Tura::Application* app = Tura::CreateApplication(argc, argv);
			app->Run();
			delete app;
		}

		return 0;
	}

}

#ifdef WL_DIST

#include <Windows.h>

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
	return Tura::Main(__argc, __argv);
}

#else

int main(int argc, char** argv)
{
	return Tura::Main(argc, argv);
}

#endif // WL_DIST

#endif // WL_PLATFORM_WINDOWS
