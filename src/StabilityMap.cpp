//std
#include <cstdint>
#include <stdexcept>

//Rigid body
#include "Rigid-body/inc/StabilityMap.hpp"

//Canvas
#include "Canvas/Canvas/inc/API/Loader.hpp"
#include "Canvas/Canvas/inc/Buffers/VAO.hpp"
#include "Canvas/Canvas/inc/Scene/Scene.hpp"
#include "Canvas/Canvas/inc/Objects/Object.hpp"
#include "Canvas/Canvas/inc/Shaders/Shader.hpp"

//object
namespace
{
	class StabilityMapObject : public canvas::objects::Object
	{
	public:
		//constructor
		StabilityMapObject(void);

		//destructor
		~StabilityMapObject(void);

		//draw
		void draw(void) const override;

	private:
		//data
		canvas::buffers::VAO m_vao;
		canvas::shaders::Shader m_shader;
	};

	//constructor
	StabilityMapObject::StabilityMapObject(void) : m_shader{"StabilityMap"}
	{
		return;
	}

	//destructor
	StabilityMapObject::~StabilityMapObject(void)
	{
		return;
	}

	//draw
	void StabilityMapObject::draw(void) const
	{
		m_vao.bind();
		m_shader.bind();
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	}
}

//callbacks
static void callback_size(GLFWwindow* window, int32_t width, int32_t height)
{
	glViewport(0, 0, width, height);
	((StabilityMap*) glfwGetWindowUserPointer(window))->size(width, height);
}
static void callback_keyboard(GLFWwindow* window, int32_t key, int32_t scancode, int32_t action, int32_t mods)
{
	if(key == GLFW_KEY_ESCAPE)
	{
		glfwSetWindowShouldClose(window, true);
	}
}

//constructor
StabilityMap::StabilityMap(void)
{
	setup_glfw();
	setup_scene();
	setup_callbacks();
}

//destructor
StabilityMap::~StabilityMap(void)
{
	delete m_ubo;
	delete m_scene;
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

//start
void StabilityMap::start(void)
{
	while(!glfwWindowShouldClose(m_window))
	{
		//animations
		glfwPollEvents();
		//draw
		m_scene->draw();
		glfwSwapBuffers(m_window);
	}
}
void StabilityMap::size(uint32_t width, uint32_t height)
{
	m_ubo->transfer(0 * sizeof(uint32_t), sizeof(uint32_t), &width);
	m_ubo->transfer(1 * sizeof(uint32_t), sizeof(uint32_t), &height);
}

//setup
void StabilityMap::setup_glfw(void)
{
	//library
	if(glfwInit() != GLFW_TRUE)
	{
		throw std::runtime_error("GLFW initialization failed!");
	}
	//window
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	m_window = glfwCreateWindow(700, 700, "Canvas", nullptr, nullptr);
	//check
	if(!m_window)
	{
		glfwTerminate();
		throw std::runtime_error("GLFW window initialization failed!");
	}
	//context
	glfwMakeContextCurrent(m_window);
	glfwSetWindowUserPointer(m_window, this);
	//v-sync
	glfwSwapInterval(0);
	//functions
	canvas::load_functions();
}
void StabilityMap::setup_scene(void)
{
	//data
	int32_t width, height;
	m_scene = new canvas::Scene;
	m_ubo = new canvas::buffers::UBO;
	canvas::shaders::Shader::path("shd/");
	glfwGetWindowSize(m_window, &width, &height);
	//setup
	m_ubo->bind_base(0);
	m_ubo->allocate(2 * sizeof(uint32_t));
	m_scene->add_object(new StabilityMapObject);
	//callback
	callback_size(m_window, width, height);
}
void StabilityMap::setup_callbacks(void)
{
	glfwSetKeyCallback(m_window, callback_keyboard);
	glfwSetWindowSizeCallback(m_window, callback_size);
}