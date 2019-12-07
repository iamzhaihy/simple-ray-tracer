#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class LightType {
    Point,
    Directional
};

class Light {
public:
    LightType type;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    glm::vec3 position;
    glm::vec3 direction;

    Light();

    Light(LightType t, 
          const glm::vec3 &v);
    Light(LightType t, 
          const glm::vec3 &v, 
          const glm::vec3 &a, 
          const glm::vec3 &d, 
          const glm::vec3 &s);

    void setPosition(float tx, float ty, float tz);
};