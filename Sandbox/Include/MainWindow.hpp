#pragma once
#include "Otter/Core/Window.hpp"

namespace Sandbox {
	class MainWindow : public Otter::Window
	{
	public:
		MainWindow(glm::vec2 size, std::string title, VkInstance vulkanInstance, bool imGuiAllowed) : Otter::Window(size, title, vulkanInstance, imGuiAllowed) {}

		virtual void OnDrawImGui();
	};

}