#include "Window.h"
#include "VulkanRenderer.h"

int main(int argc, char* argv[]) {
    Window window("Voxploria");
    VulkanRenderer renderer;

    renderer.Initialize();

    while (window.IsRunning()) {
        window.ProcessEvents();
    }

    renderer.Cleanup();

    return 0;
}
