#include "Engine.h"


/**
* Sample application for using omen game engine
* This application creates a window and waits for user to close the window.
**/
int main(int argc, char *argv[]) 
{
    try
    {
        // Create omen engine instance and get pointer to it
        omen::Engine* engine = omen::Engine::instance();

        // Create a new window of size 640x480
        const std::unique_ptr<omen::Window>& window = engine->createWindow(640, 480);

        // Create main loop and wait until window has been closed
        while (!window->shouldClose()) {
            engine->update();
        }
    }
    catch (std::runtime_error& e)
    {
        std::cout << "Oops.. Something unexpected happened:\n " << e.what() << std::endl;
    }

	return 0;
}