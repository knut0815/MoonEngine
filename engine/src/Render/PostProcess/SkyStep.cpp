#include "SkyStep.h"
#include "Libraries/Library.h"
#include "GlobalFuncs/GlobalFuncs.h"

using namespace MoonEngine;

SkyStep::SkyStep(int width, int height):
    _fbo(width, height),
    _width(width),
    _height(height)
{
}


void SkyStep::setup(GLFWwindow * window, Scene * scene)
{
	_renderProgram = Library::ProgramLib->getProgramForName("skydome.program");
	_compositeTexture = Library::TextureLib->getTexture(COMPOSITE_TEXTURE);

	_fbo.addTexture(COMPOSITE_TEXTURE, GL_COLOR_ATTACHMENT0);

    _mainCamera = scene->getMainCamera()->getComponent<Camera>();
    if (_mainCamera == nullptr)
    {
        LOG(ERROR, "No Camera in scene!");
    }

    // ...
    _sphere = GetWorld()->createGameObject();
    _sphere->getTransform().setScale(glm::vec3(500, 500, 500));
    _sphere->addComponent(GetWorld()->createComponent<SimpleTexture>("skycolor"));
    _sphere->addComponent(scene->createComponent<StaticMesh>("sphere.obj", false));

    GetWorld()->addGameObject(_sphere);
}

void SkyStep::render(Scene * scene)
{
	_fbo.bind(GL_FRAMEBUFFER);
	_renderProgram->enable();

    glm::mat4 M = _sphere->getTransform().getMatrix();
    glm::mat4 V = _mainCamera->getView();
    glm::mat4 P = _mainCamera->getProjection();

    glUniformMatrix4fv(_renderProgram->getUniformLocation("P"), 1, GL_FALSE, glm::value_ptr(P));
    glUniformMatrix4fv(_renderProgram->getUniformLocation("V"), 1, GL_FALSE, glm::value_ptr(V));
    glUniformMatrix4fv(_renderProgram->getUniformLocation("M"), 1, GL_FALSE, glm::value_ptr(M));

    // Pass gradient texture
    _sphere->getComponent<SimpleTexture>()->getTexture()->bind(0);
    glUniform1i(_renderProgram->getUniformLocation("skycolor"), 0);

    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Mesh* mesh = _sphere->getComponent<Mesh>();

    mesh->bind();
    mesh->draw();

    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);

    _fbo.DBG_DrawToImgui("Sky");
}