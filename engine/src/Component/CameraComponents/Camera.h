#pragma once

#include "Component/Component.h"
#include "glm/glm.hpp"

namespace MoonEngine
{

    class Camera: public Component
    {
    public:
        Camera(float fov, float aspect, float near, float far);

        /**
         * Initialize the camera
         */
        void start();

        /**
         * Get the current projection matrix for this camera
         * @return the projection matrix
         */
        const glm::mat4 & getProjection();

        /**
         * Get the view matrix for this matrix
         * @return the view matrix
         */
        const glm::mat4 & getView();

        void getPlanes(glm::vec4 planes[6]);

        float getFOV() const;

		float getFar() const;

		float getNear() const;

        float getAspect() const;

        /**
         * Set the field of view of this camera
         * @param fov the new FOV
         */
        void setFOV(float fov);

        void update(float dt);

        std::shared_ptr<Component> clone() const;

    private:
        float _aspect;
        float _near;
        float _far;
        float _fov;
        bool _vNeedsUpdate;
        glm::mat4 _P;
        glm::mat4 _V;

    };
}