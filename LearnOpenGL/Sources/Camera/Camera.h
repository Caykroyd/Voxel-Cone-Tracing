/*
MIT License

Copyright(c) 2016 Fredrik Präntare

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#pragma once

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include "Shader/ShaderProperty.h"
#include "Shader/ShaderProgram.h"

/// <summary> A camera base class. </summary>
class Camera : public ShaderProperty {
	public:
		glm::vec3 up = { 0,1,0 }, rotation = { 0,0,-1 }, position = { 0,0,0 };
		glm::mat4 viewMatrix;
		const glm::mat4 & getProjectionMatrix() const;
		void setProjectionMatrix(glm::mat4 projectionMatrix);
		virtual void updateViewMatrix();
		glm::vec3 right() { return glm::normalize(-glm::cross(up, rotation)); }
		glm::vec3 forward() { return glm::normalize(rotation); }
		bool projectionMatrixHasChanged = true;
		bool dirtyProjectionMatrix = true;

		void SendToShader(ShaderProgram& shader) override {
			glm::mat4 projectionMatrix = getProjectionMatrix();//computeProjectionMatrix();
			shader.set("M_Projection", projectionMatrix); // Compute the projection matrix of the camera and pass it to the GPU program

			//updateViewMatrix();
		
			shader.set("M_View", viewMatrix);//computeViewMatrix();
			shader.set("M_View_1t", glm::transpose(glm::inverse(viewMatrix)));
		}
	protected:
		Camera(glm::mat4 _projectionMatrix) : projectionMatrix(_projectionMatrix) {};
		glm::mat4 projectionMatrix;
};