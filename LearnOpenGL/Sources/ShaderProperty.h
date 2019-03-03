#pragma once

#include "ShaderProgram.h"

class ShaderProperty {
	public:
		virtual ~ShaderProperty() {};
		virtual void SendToShader(ShaderProgram& shader) const {};
};
