#include "Game.h"
#include <iostream>

double Game::delta_time = 0.0;

void Game::Exit(std::string message, bool forced_exit) {
	
	if (exit_callback != nullptr) exit_callback();

	if (forced_exit) {
		std::cerr << message << std::endl;
		std::getchar();
		std::exit(EXIT_FAILURE);
	}
	else {
		std::cout << message << std::endl;
		std::getchar();
		std::exit(EXIT_SUCCESS);
	}
};