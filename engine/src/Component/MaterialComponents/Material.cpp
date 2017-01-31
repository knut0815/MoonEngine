#include "Material.h"
#include "Libraries/Library.h"
#include "thirdparty/imgui/imgui.h"
using namespace MoonEngine;

Material::Material(glm::vec3 tint, std::string program, stringmap textures):
	Component(),
	_tint(tint),
	_texture_unit(0),
	_textures(std::unordered_map<string, GLTexture *>())
{
	_programPtr = Library::ProgramLib->getProgramForName(program);
	if(_programPtr == nullptr)
	{
		_programPtr = Library::ProgramLib->getProgramForName("default.program");
	}


	for (auto &texture: textures) {
        // uniform name <=> texture
		_textures[texture.first] = Library::TextureLib->getTexture(texture.second, _texture_unit++);
        /* Add the uniforms */
        //_programPtr->getUniformLocation(materialPropertyNames[texture.first]);
	}

	_samplerPtr = Library::SamplerLib->getSampler("default");
}

const glm::vec3 & Material::getTint() const
{
	return _tint;
}

void Material::setTint(glm::vec3 newTint)
{
	_tint = newTint;
}


GLProgram * Material::getProgram() const
{
	return _programPtr;
}


std::shared_ptr<Component> Material::clone() const
{
	return std::make_shared<Material>(*this);
}

void Material::bind() {
	//todo bind uniforms
	for (auto &_texture: _textures) {
		ImGui::Begin("Textures");
		{
			ImGui::Image((void*)(_texture.second->getTextureId()),ImVec2(256,256));
		}
		ImGui::End();
		LOG(GAME,"Texture Unit: "  + std::to_string(_texture.second->getTextureId()));

        _texture.second->bind();
        _texture.second->bindSampler(_samplerPtr);
        glUniform1i(_programPtr->getUniformLocation(_texture.first), _texture.second->getUnit());
    }
}

void Material::unbind() {
	//todo unbind uniforms
	for (auto &texture: _textures) {
		texture.second->unbind();
		texture.second->unbindSampler();
	}
}
