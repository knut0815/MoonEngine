#pragma once
#include "GLWrapper/GLFramebuffer.h"
#include "GBuffer.h"
class SSAOBuffers
{
public:
    SSAOBuffers(int width, int height, int samples);
    void bindForSSAO();
    void bindForBlur();
    vector<glm::vec3> getKernel() {
        return _kernel;
    }
    vector<glm::vec3> getNoise()
    {
        return _noise;
    }
    void DBG_DrawToImgui(string guiName);
    void UniformTexture(GLProgram * prog, std::string uniformName, std::string textureName);
private:
    GLFramebuffer _ssaoBuffer;
    GLFramebuffer _ssaoBlurBuffer;
    vector<glm::vec3> _kernel;
    vector<glm::vec3> _noise;
    static float lerp(float a, float b, float f);
    void genNoiseTex();
    GLTexture* _noiseTex;
};