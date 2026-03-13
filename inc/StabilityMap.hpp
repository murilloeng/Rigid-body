#pragma once

//std
#include <cstdint>

//glfw
#include <GLFW/glfw3.h>

//Canvas
#include "Canvas/Canvas/inc/Buffers/UBO.hpp"
#include "Canvas/Canvas/inc/Scene/Scene.hpp"

class StabilityMap
{
public:
	//constructors
	StabilityMap(void);

	//destructor
	~StabilityMap(void);

	//window
	void start(void);
	void size(uint32_t, uint32_t);

private:
	//setup
	void setup_glfw(void);
	void setup_scene(void);
	void setup_callbacks(void);

	//data
	GLFWwindow* m_window;
	canvas::Scene* m_scene;
	canvas::buffers::UBO* m_ubo;
};