#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "MoonEngine.h"
#include "LevelEditor/LevelLoader.h"

using namespace MoonEngine;


int main(int argc, char ** argv)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    float windowWidth = 800.0f, windowHeight = 600.0f;
    GLFWwindow * window = glfwCreateWindow(windowWidth, windowHeight, "ShatteredMoon", nullptr, nullptr);
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

    Logger::SetLogLevel(GAME);
    std::shared_ptr<EngineApp> app = std::make_shared<EngineApp>(window);
    Scene * scene = new Scene();


    std::shared_ptr<GameObject> cameraObj = std::make_shared<GameObject>();

    Camera * cam = scene->createComponent<Camera>( 3.1415/3, 800.0/600.0, 0.1, 1000);
    FirstPersonController * ctrl = scene->createComponent<FirstPersonController>(5);
    cameraObj->addComponent(ctrl);
    cameraObj->addComponent(cam);

    cameraObj->getTransform().translate(glm::vec3(0,150,-5));
    scene->addGameObject(cameraObj);


    //Game Objects
//    Transform playerTransform = Transform();
//    playerTransform.setPosition(
//            glm::vec3(-32.623940, 20.913505554199219,-101.991371));
//
//    std::shared_ptr<GameObject> cameraObj = std::make_shared<GameObject>(playerTransform);
//
//    playerTransform.setPosition(
//            glm::vec3(-52.623940, 12.913505554199219,-101.991371));
//    std::shared_ptr<GameObject> playerObj = std::make_shared<GameObject>(playerTransform);
//    playerObj->addComponent(scene->createComponent<ThirdPersonCharacterController>(4.1));
//
//    stringmap textures({{"diffuse", "wolf.tga"}});
//
//    playerObj->addComponent(scene->createComponent<StaticMesh>("wolf.obj", false));
//    playerObj->addComponent(scene->createComponent<Material>(glm::vec3(0.2, 0.2, 0.2), "geom.program", textures));
//    playerObj->addComponent(scene->createComponent<BoxCollider>());
//
//    //playerObj->getTransform().setPosition(glm::vec3(0, 0.5, 0));
//    playerObj->getTransform().setScale(glm::vec3(0.2, 0.2, 0.2));
//    playerObj->addTag(T_Player);
//
//    scene->addGameObject(playerObj);
//
//    //Camera setup
//    Camera * cam = scene->createComponent<Camera>(3.1415 / 3, windowWidth / windowHeight, 0.1, 1200);
//    cameraObj->addComponent(cam);
//    cameraObj->addComponent(scene->createComponent<ThirdPersonOrbitalController>());
//    cameraObj->getTransform().translate(glm::vec3(0, 5, 5));
//    //cameraObj->getTransform().rotate(glm::vec3(-M_PI/6,0,0));
//    scene->addGameObject(cameraObj);


    //Ground
    Transform groundTransform;
    groundTransform.setScale(glm::vec3(5, 150, 5));
    std::shared_ptr<GameObject> groundObject = std::make_shared<GameObject>(groundTransform);
    groundObject->addComponent(scene->createComponent<StaticMesh>("quad.obj", true));
    groundObject->addComponent(scene->createComponent<Material>(glm::vec3(0.2, 0.8, 0.2), "geom.program"));
    scene->addGameObject(groundObject);

    LevelLoader levelLoader;
    levelLoader.LoadLevel("scenedata.json", scene);
    stringmap cube_texture({{"diffuse", "cube"}});


    //Lights
    Transform lightTransform;
    lightTransform.setPosition(glm::vec3(6, 4, 1));
    std::shared_ptr<GameObject> pointLight = make_shared<GameObject>(lightTransform);
    pointLight->addComponent(scene->createComponent<PointLight>(pointLight->getTransform().getPosition(), COLOR_PURPLE, 0.2f, 0.2f));
    pointLight->getComponent<PointLight>()->setRange(10);
    //scene->addGameObject(pointLight);

    lightTransform.setPosition(glm::vec3(-5, 3, 1));
    pointLight = make_shared<GameObject>(lightTransform);
    pointLight->addComponent(scene->createComponent<PointLight>(pointLight->getTransform().getPosition(), COLOR_WHITE, 0.2f, 0.2f));
    pointLight->getComponent<PointLight>()->setRange(10);
    //scene->addGameObject(pointLight);

    lightTransform.setPosition(glm::vec3(4, 3, -5));
    pointLight = make_shared<GameObject>(lightTransform);
    pointLight->addComponent(scene->createComponent<PointLight>(pointLight->getTransform().getPosition(), COLOR_CYAN, 0.2f, 0.2f));
    pointLight->getComponent<PointLight>()->setRange(10);
    //scene->addGameObject(pointLight);


    std::shared_ptr<GameObject> dirLight = make_shared<GameObject>();
    dirLight->addComponent(scene->createComponent<DirLight>(glm::vec3(-1, -1, -1), COLOR_WHITE, 0.1f, 0.5f));
    scene->addGameObject(dirLight);


    //Terrain
    //Preload canyon 32f texture
    EngineApp::GetAssetLibrary().TextureLib->getTexture("grandCanyon",".png",true);

    stringmap canyon_texture(
            {{"heightmap", "grandCanyon"},
                    {"diffuse"  , "cube"},
                    {"canyonTint","canyonTint"}});

    CDLODQuadtree::CreateInfo createInfo;
    //ImplicitHeightmapSource heightSource(256,256,[](int, int){return 0;});
    TextureHeightmapSource texSource("resources/textures","grandCanyon",".png");
    createInfo.source = &texSource;
    createInfo.leafNodeSize = 2;
    createInfo.LODLevelCount = 5;
    MapDimensions mapDims;

    mapDims.size = glm::vec3(1000,200,1000);
    mapDims.minCoords = glm::vec3(0,0,0);
    mapDims.minCoords = -mapDims.size/2.0f;
    mapDims.minCoords.y = 0;
    createInfo.dimensions = mapDims;
    std::shared_ptr<GameObject> terrainObject = std::make_shared<GameObject>(Transform());
    terrainObject->addComponent(scene->createComponent<Terrain>(createInfo));
    terrainObject->addComponent(scene->createComponent<Material>(glm::vec3(0.2,0.2,0.2), "terrain_geom_deferred.program",canyon_texture));
    scene->addGameObject(terrainObject);


    Transform tran;
    tran.setPosition(glm::vec3(0.0, 150.0, 0.0));
    tran.setScale(glm::vec3(5, 5, 5));

    stringmap sun = {{"billboard", "cube"}};

    std::shared_ptr<GameObject> sunBillboard = std::make_shared<GameObject>(tran);
    sunBillboard->addComponent(scene->createComponent<StaticMesh>("quad", false));
    sunBillboard->addComponent(scene->createComponent<Material>(glm::vec3(1.0, 1.0, 1.0), "billboard.program", sun,true));
    scene->addGameObject(sunBillboard);


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
        //LOG(GAME, "SUN: " + std::to_string(scene->getGlobalLightDir().x) + " " + std::to_string(scene->getGlobalLightDir().y));
        Library::TextureLib->Debug_ShowAllTextures();

    });

    DeferredRenderer * renderer = new DeferredRenderer(width, height, "phong_point_deferred.program", "phong_dir_deferred.program");
    app->run(scene, renderer);

    delete scene;
    delete renderer;

    return 0;

}