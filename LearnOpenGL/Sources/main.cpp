#define _USE_MATH_DEFINES

#include "Window.h"

int main() {

	Window& app = Window::GetInstance();
	
	app.init();

	while(!app.ShouldClose()) { // check if the close button has been triggered for the window 
		app.update();
	}

	app.terminate();

	return 0;

}