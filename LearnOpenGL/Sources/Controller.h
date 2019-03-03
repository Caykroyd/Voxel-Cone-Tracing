#pragma once

#include "IUpdatable.h"

class Controller {
	public:
		virtual ~Controller() {};
		virtual void update() {};
};
