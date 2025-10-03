// Main.cpp
#include "../DXFramework/System.h"
#include "Scene.h"
#include "TestingScene.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	Scene* scene = new Scene();
//	TestingScene* scene = new TestingScene();
	System* system;

	// Create the system object.
	system = new System(scene, 1200, 675, true, false);

	// Initialize and run the system object.
	system->run();

	// shutdown and release the system object.
	delete system;
	system = 0;

	return 0;
}
