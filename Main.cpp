#include "Window.h"
#include "VulkanRenderer.h"
#include "Logger.h"


int main(int argc, char* argv[]) {
    try {
        Window window("Voxploria");
        VulkanRenderer renderer;

        Logger::Log("Runtime", "test");

        renderer.Initialize();

        while (window.IsRunning()) {
            window.ProcessEvents();
        }

        renderer.Cleanup();
    }
    catch (const std::exception& e) {
        Logger::Log("Runtime", std::string(e.what()));
        std::cerr << "An error occurred: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
