#include "DeferredRenderer.h"


using namespace MoonEngine;


DeferredRenderer::DeferredRenderer(int width, int height):
_mainCamera(nullptr),
_gBuffer(width, height),
_positionTex(0),
_colorTex(1),
_normalTex(2),
_textureTex(3),
_depthStencilTex(4)
{
    // renderQuad = MeshCreator::CreateQuad(glm::vec2(-1,1), glm::vec2(1,1));
    assert(_colorTex.init(GLTextureConfiguration(width,height,GL_RGB,GL_RGB,GL_UNSIGNED_BYTE)));
    assert(_depthStencilTex.init(GLTextureConfiguration(width,height,GL_DEPTH24_STENCIL8,GL_DEPTH_STENCIL,GL_UNSIGNED_INT_24_8)));
    _gBuffer.addTexture("color",_colorTex,GL_COLOR_ATTACHMENT0);
    _gBuffer.addTexture("depthStencil",_depthStencilTex,GL_DEPTH_STENCIL_ATTACHMENT);

}

void DeferredRenderer::setup(Scene * scene)
{
    _mainCamera = scene->findGameObjectWithComponent<Camera>()->getComponent<Camera>();
    if (_mainCamera == nullptr)
    {
        LOG(ERROR, "No Camera in scene!");
    }
    //Swing through all rendering components and load their programs.

    glClearColor(0.2f, 0.2f, 0.6f, 1.0f);
    glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
}


void DeferredRenderer::render(Scene * scene)
{

	vector<std::shared_ptr<GameObject>>forwardObjects;
        
	geometryPass(scene);
    
    // ImGui::Begin("Framebuffer");
    // {
    // 	ImGui::Image((void*)(_colorTex.getTextureId()),ImVec2(256,256));
    // 	ImGui::Image((void*)(_gBuffer.getTexture("_depthStencilTex")),ImVec2(128,128));
    // }
    // ImGui::End();
    //Debug show textures
    //Library::TextureLib->Debug_ShowAllTextures();
    GLVertexArrayObject::Unbind();
}

vector<std::shared_ptr<GameObject>> DeferredRenderer::geometryPass(Scene * scene)
{
	GLProgram* activeProgram = nullptr;
	const MeshInfo* mesh = nullptr;
	Material* mat = nullptr;
	vector<std::shared_ptr<GameObject>> forwardObjects;
	
	_gBuffer.bind();
	glm::mat4 V = _mainCamera->getView();
	glm::mat4 P = _mainCamera->getProjection();
	
	// Only the geometry pass writes to the depth buffer
	glDepthMask(GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (std::shared_ptr<GameObject> obj : scene->getRenderableGameObjects())
	{
		mat = obj->getComponent<Material>();
		mesh = obj->getComponent<Mesh>()->getMesh();

		if (mat->isForward()) {
			forwardObjects.push_back(obj);
			continue;
		}

		glm::mat4 M = obj->getTransform().getMatrix();
		
		
		//sets the materials geometry shader as active
		mat->setActiveProgram(0);
		mat->bind();
		mesh->bind();

		if (activeProgram != mat->getProgram()) {
			activeProgram = mat->getProgram();
			activeProgram->enable();

			//No assumptions about the geometry stage is made beyond a P, V, and M Uniforms
			glUniformMatrix4fv(activeProgram->getUniformLocation("P"), 1, GL_FALSE, glm::value_ptr(P));
			glUniformMatrix4fv(activeProgram->getUniformLocation("V"), 1, GL_FALSE, glm::value_ptr(V));
			glUniformMatrix4fv(activeProgram->getUniformLocation("M"), 1, GL_FALSE, glm::value_ptr(M));

			//Optional Uniforms are checked here, and bound if found
			if (activeProgram->hasUniform("tint")) {
				glm::vec3 tint = mat->getTint();
				glUniform3f(activeProgram->getUniformLocation("tint"), tint.x, tint.y, tint.z);
			}
			if (activeProgram->hasUniform("iGLobalTime")) {
				glUniform1f(activeProgram->getUniformLocation("iGlobalTime"), scene->getGlobalTime());
			}
			if (activeProgram->hasUniform("N")) {
				glm::mat3 N = glm::mat3(glm::transpose(glm::inverse(V * M)));
				glUniformMatrix3fv(activeProgram->getUniformLocation("N"), 1, GL_FALSE, glm::value_ptr(N));
			}

		}

		if (obj->getComponent<InstanceMesh>() != nullptr)
		{
			glDrawElementsInstanced(
				GL_TRIANGLES,
				mesh->numTris,
				GL_UNSIGNED_SHORT,
				mesh->indexDataOffset,
				obj->getComponent<InstanceMesh>()->_numOfInstances
			);
		}
		else
		{
			glDrawElementsBaseVertex(
				GL_TRIANGLES,
				mesh->numTris,
				GL_UNSIGNED_SHORT,
				mesh->indexDataOffset,
				mesh->baseVertex
			);
		}
		mat->unbind();
	}
	//sets the framebuffer back to the default(0)
	GLFramebuffer::Unbind();
	//sets the mesh (VAO) back to 0
	GLVertexArrayObject::Unbind();
}

void DeferredRenderer::lightingPass(Scene * scene)
{
}


void DeferredRenderer::shutdown()
{
}


