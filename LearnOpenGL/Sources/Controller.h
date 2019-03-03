#pragma once

#include "IUpdatable.h"

class Controller : public IUpdatable {
	public:
		virtual ~Controller() {};
		virtual void update() {};
};
