#pragma once

#include <map>
#include <cmath>
#include <string>
#include <vector>
#include <cstddef>
#include <fstream>
#include <iostream>

#include "Light.h"
#include "Camera3D.h"
#include "Object3D.h"

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/error/en.h"


class Scene {
public:
    Camera3D camera;
    std::vector<Material> materials;
    std::vector<Light> lights;
    std::vector<Object3D*> objects;

    Scene();
    Scene(float f, const std::vector<Light> &l, const std::vector<Object3D*> &o);

    bool loadSceneFromJSON(std::string filepath);
};
