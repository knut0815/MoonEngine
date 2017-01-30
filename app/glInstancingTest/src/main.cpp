#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "MoonEngine.h"


using namespace MoonEngine;


int main(int argc, char **argv) {

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow * window = glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	Logger::SetLogLevel(INFO);


	MoonEngineCfg cfg;
	cfg.assetPath = "resources";

	std::shared_ptr<EngineApp> app = std::make_shared<EngineApp>(window, cfg);

	Scene * scene = new Scene();

	//Game Objects
	std::shared_ptr<GameObject> cameraObj = std::make_shared<GameObject>();



	//Camera setup
	Camera * cam = scene->createComponent<Camera>(3.1415 / 3, 800.0 / 600.0, 0.1, 50);
	cameraObj->addComponent(cam);
	cameraObj->addComponent(scene->createComponent<FirstPersonController>());
	//cameraObj->getTransform().translate(glm::vec3(0,2,5));
	//cameraObj->getTransform().rotate(glm::vec3(-M_PI/6,0,0));
	scene->addGameObject(cameraObj);


	//Ground
	Transform groundTransform;
	groundTransform.setScale(glm::vec3(5, 1, 5));
	std::shared_ptr<GameObject> groundObject = std::make_shared<GameObject>(groundTransform);
	groundObject->addComponent(scene->createComponent<StaticMesh>("quad.obj", true));
	groundObject->addComponent(scene->createComponent<Material>(glm::vec3(0.2, 0.8, 0.2), "default.program"));
	scene->addGameObject(groundObject);

	//Boxes
	

	float accumTime;
	int lastUpdateTime;
	scene->addCustomUpdate([&](float dt) {
		//ImGui::ShowTestWindow();
		// accumTime += dt;
		// if((int)accumTime > lastUpdateTime)
		// {
		// 	LOG(GAME, "FPS: " + std::to_string(1.0/dt));
		// 	LOG(GAME, "Active Objects: " + std::to_string(scene->getGameObjects().size()));

		// 	lastUpdateTime = (int)accumTime;
		// }

	});



	ProgramRenderer * renderer = new ProgramRenderer();
	app->run(scene, renderer);

	delete scene;
	delete renderer;


	return 0;

}