#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera3D {
private:
    float f;             // focal length
    float fov;           // field of view
    float ratio;         // wdith/height

    glm::vec3 u, v, w;   // orthonormal frame (looks at -w)
    glm::vec3 position;  // position of camera (world space)

public:

    Camera3D();

    void update(float _f, 
                float _fov, 
                float _ratio,
                const glm::vec3 &_e,
                const glm::vec3 &_v,
                const glm::vec3 &_w);

    void move(glm::vec3 m);

    void setFOV(float _fov);

    void setRatio(float _ratio);

    void setPosition(glm::vec3 _pos);

    void switchCameraType();

    float getRatio();

    float Camera3D::getFocalLength();
    
    glm::vec3 getPosition();

    glm::vec3 getDirection();

    glm::mat4 getViewMatrix();

    glm::mat4 getProjectionMatrix();
};