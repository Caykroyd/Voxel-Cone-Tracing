#define _USE_MATH_DEFINES

#include "App.h"

int main() {

	App& app = App::getInstance();
	
	app.init();

	while(!app.shouldClose()) { // check if the close button has been triggered for the window 
		app.update();
	}

	app.terminate();

	return 0;

}