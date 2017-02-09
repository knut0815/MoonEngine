#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "MoonEngine.h"


using namespace MoonEngine;


int main(int argc, char **argv) {

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	float windowWidth = 800.0f, windowHeight = 600.0f;
	GLFWwindow * window = glfwCreateWindow(windowWidth, windowHeight, "LearnOpenGL", nullptr, nullptr);
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
	if(glewInit() != GLEW_OK)
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




    std::shared_ptr<GameObject> playerObj = std::make_shared<GameObject>();

	playerObj->addComponent(scene->createComponent<ThirdPersonCharacterController>(2.1));

    stringmap textures({{"diffuse", "penguin"}});

    playerObj->addComponent(scene->createComponent<StaticMesh>("penguin.obj", false));
    playerObj->addComponent(scene->createComponent<Material>(glm::vec3(0.2, 0.2, 0.2), "phong.program", textures));
    playerObj->addComponent(scene->createComponent<BoxCollider>());

    playerObj->getTransform().setPosition(glm::vec3(0, 20, 0));
    playerObj->getTransform().setScale(glm::vec3(0.2, 0.2, 0.2));
    playerObj->addTag(T_Player);

    scene->addGameObject(playerObj);

    //Camera setup
    Camera * cam = scene->createComponent<Camera>(3.1415 / 3, windowWidth / windowHeight, 0.1, 1000);
    cameraObj->addComponent(cam);
    cameraObj->addComponent(scene->createComponent<ThirdPersonOrbitalController>());
    cameraObj->getTransform().translate(glm::vec3(0, 100, 0));
    //cameraObj->getTransform().rotate(glm::vec3(-M_PI/6,0,0));
    scene->addGameObject(cameraObj);


	std::shared_ptr<GameObject> boxObject = std::make_shared<GameObject>(Transform());
	boxObject->addComponent(scene->createComponent<StaticMesh>("cube.obj", false));
	boxObject->addComponent(scene->createComponent<Material>(glm::vec3(0.9, 0.5, 0.5), "phong.program"));
	boxObject->addComponent(scene->createComponent<BoxCollider>());
	//scene->addGameObject(boxObject);

	CDLODQuadtree::CreateInfo createInfo;
	//ImplicitHeightmapSource heightSource(256,256,[](int, int){return 0;});
	TextureHeightmapSource texSource("resources","canyonlands",".png");
	createInfo.source = &texSource;
	createInfo.leafNodeSize = 16;
	createInfo.LODLevelCount = 4;
	MapDimensions mapDims;
	
	mapDims.size = glm::vec3(100,20,100);
	mapDims.minCoords = glm::vec3(0,0,0);
	//mapDims.minCoords = -mapDims.size/2.0f;
	//mapDims.minCoords.y = 0;
	createInfo.dimensions = mapDims;
	std::shared_ptr<GameObject> terrainObject = std::make_shared<GameObject>(Transform());
	terrainObject->addComponent(scene->createComponent<Terrain>(createInfo));
	
	//Preload canyon 32f texture
	EngineApp::GetAssetLibrary().TextureLib->getTexture("canyonlands",0,".png",true);
	
	stringmap canyon_texture(
            {{"heightmap", "canyonlands"}});
    
	terrainObject->addComponent(scene->createComponent<Material>(glm::vec3(0.2,0.2,0.2), "terrain.program",canyon_texture));
	scene->addGameObject(terrainObject);
	
	float accumTime;
	int lastUpdateTime;
	scene->addCustomUpdate([&](float dt){
		if(Keyboard::key(GLFW_KEY_L))
		{
			if(Keyboard::isKeyToggled(GLFW_KEY_L))
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
			else
			{
				glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
			}
		}
		Library::TextureLib->Debug_ShowAllTextures();
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
