#include "DeferredRenderer.h"


using namespace MoonEngine;


DeferredRenderer::DeferredRenderer(int width, int height):
mainCamera(nullptr),
_gBuffer(width, height),
_colorTex(0),
_depthStencilTex(1)
{
    // renderQuad = MeshCreator::CreateQuad(glm::vec2(-1,1), glm::vec2(1,1));
    assert(_colorTex.init(GLTextureConfiguration(width,height,GL_RGB,GL_RGB,GL_UNSIGNED_BYTE)));
    assert(_depthStencilTex.init(GLTextureConfiguration(width,height,GL_DEPTH24_STENCIL8,GL_DEPTH_STENCIL,GL_UNSIGNED_INT_24_8)));
    _gBuffer.addTexture("color",_colorTex,GL_COLOR_ATTACHMENT0);
    _gBuffer.addTexture("depthStencil",_depthStencilTex,GL_DEPTH_STENCIL_ATTACHMENT);

}

void DeferredRenderer::setup(Scene * scene)
{
    mainCamera = scene->findGameObjectWithComponent<Camera>()->getComponent<Camera>();
    if (mainCamera == nullptr)
    {
        LOG(ERROR, "No Camera in scene!");
    }
    //Swing through all rendering components and load their programs.

    glClearColor(0.2f, 0.2f, 0.6f, 1.0f);
    glEnable(GL_DEPTH_TEST);

	//Setup the GBuffer



}


void DeferredRenderer::render(Scene * scene)
{

	GLProgram * activeProgram = nullptr;

	vector<std::shared_ptr<GameObject>>forwardObjects;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 V = mainCamera->getView();
    glm::mat4 P = mainCamera->getProjection();
	
	for (std::shared_ptr<GameObject> obj : scene->getRenderableGameObjects())
	{
		Material * mat = obj->getComponent<Material>();


		if (mat->isForward()) {
			forwardObjects.push_back(obj);
			continue;
		}
		
		glm::mat4 M = obj->getTransform().getMatrix();
		glm::mat3 N = glm::mat3(glm::transpose(glm::inverse(V * M)));
		



		const MeshInfo * mesh = obj->getComponent<Mesh>()->getMesh();
		mesh->bind();
		mat->bind();
	}
    //GLFramebuffer::Unbind();


    // ImGui::Begin("Framebuffer");
    // {
    // 	ImGui::Image((void*)(framebufferColorTexture.getTextureId()),ImVec2(256,256));
    // 	ImGui::Image((void*)(renderToFB.getTexture("depthStencil")),ImVec2(128,128));
    // }
    // ImGui::End();
    //Debug show textures
    //Library::TextureLib->Debug_ShowAllTextures();
    GLVertexArrayObject::Unbind();
}

void DeferredRenderer::geometryPass(Scene * scene)
{
}

void DeferredRenderer::lightingPass(Scene * scene)
{
}


void DeferredRenderer::shutdown()
{
}


