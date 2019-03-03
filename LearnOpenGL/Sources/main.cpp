#define _USE_MATH_DEFINES

#include "App.h"

int main() {

	Window& app = Window::getInstance();
	
	app.init();

	while(!app.shouldClose()) { // check if the close button has been triggered for the window 
		app.update();
	}

	app.terminate();

	return 0;

}