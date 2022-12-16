#include "Tura/Application.h"
#include "Tura/EntryPoint.h"
#include <imgui.h>
#include "Tura/Image.h"


Tura::Application* Tura::CreateApplication(int argc, char** argv)
{
	Tura::ApplicationSpecification spec;
	spec.Name = L"TuraApp";

	Tura::Application* app = new Tura::Application(spec);



	app->SetMenubarCallback([app]()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
			{
				app->Close();
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			ImGui::EndMenu();
		}
	});
	return app;
}