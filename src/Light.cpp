#include "Light.h"

Light::Light()
        : type{LightType::Point}
        , ambient{0.0f}
        , diffuse{0.0f}
        , specular{0.0f}
        , position{0.0f}
        , direction{0.0f} {}

Light::Light(LightType t, const glm::vec3 &v)
        : type{t}
        , ambient{0.0f}
        , diffuse{0.0f}
        , specular{0.0f}
        , position{0.0f}
        , direction{0.0f} {
    if (t == LightType::Point)
        position = v;
    else if (t == LightType::Directional)
        direction = v;
}

Light::Light(LightType t,
             const glm::vec3 &v,
             const glm::vec3 &a,
             const glm::vec3 &d,
             const glm::vec3 &s)
        : type{t}
        , ambient{a}
        , diffuse{d}
        , specular{s}
        , position{0.0f}
        , direction{0.0f} {
    if (t == LightType::Point)
        position = v;
    else if (t == LightType::Directional)
        direction = v;
}

void Light::setPosition(float tx, float ty, float tz) {
    position = { tx, ty , tz };
}