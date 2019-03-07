#ifndef GAME_H
#define GAME_H

#include <functional>
#include <string>

class Game {
private:
	Game() {};
public:
	Game(Game const&) = delete;
	void operator=(Game const&) = delete;
	static Game& GetInstance() {
		static Game game; 
		return game;
	};

	void SetExitCallback(std::function<void()> exit_callback) { this->exit_callback = exit_callback; };

	void Update(double current_time) {
		delta_time = current_time - _time;
		_time = current_time;
	};

	void Exit(std::string message, bool forced_exit);

	void SetTime(double time) { _time = time; };

	static double delta_time;
private:
	double _time = 0.0;
	std::function<void()> exit_callback = nullptr;
};

#endif // MESH_H