#include "Camera3D.h"

Camera3D::Camera3D()
        : f{1.0f}
        , fov{45.0f}
        , ratio{1.0f}
        , position{0.0f}
        , u{glm::vec3(0.0f)}
        , v{glm::vec3(0.0f)}
        , w{glm::vec3(0.0f)} {}

void Camera3D::update(float _f, 
                      float _fov, 
                      float _ratio,
                      const glm::vec3 &_e,
                      const glm::vec3 &_v,
                      const glm::vec3 &_w) {
    f = _f;
    fov = _fov;
    ratio = _ratio;
    position = _e;
    v = glm::normalize(_v);
    w = -1.0f * glm::normalize(_w);
    u = glm::cross(v, w);    
}

void Camera3D::move(glm::vec3 m) {
    position += m;
}

void Camera3D::setFOV(float _fov) {
    fov = _fov;
}

void Camera3D::setRatio(float _ratio) {
    ratio = _ratio;
}

void Camera3D::setPosition(glm::vec3 _pos) {
    position = _pos;
}

float Camera3D::getRatio() {
    return ratio;
}

float Camera3D::getFocalLength() {
    return f;
}

glm::vec3 Camera3D::getDirection() {
    return -w;
}

glm::vec3 Camera3D::getPosition() {
    return position;
}

glm::mat4 Camera3D::getViewMatrix() {
    return glm::lookAt(position, -w, v);
}

glm::mat4 Camera3D::getProjectionMatrix() {
    return glm::perspective(fov, ratio, f, 100.0f);
}