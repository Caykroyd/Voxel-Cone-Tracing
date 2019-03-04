#pragma once

class IUpdatable {
	public:
		virtual ~IUpdatable() {};
		virtual void update() {};
		virtual void init() {};
		virtual void destroy() {};

		bool should_init = true;
		bool should_destroy = false;
};
