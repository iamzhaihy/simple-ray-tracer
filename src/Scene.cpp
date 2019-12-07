#include "Scene.h"

static std::string readFile(std::string filepath) {
    std::ifstream t(filepath);

    if (t.is_open()) {
        std::string str((std::istreambuf_iterator<char>(t)),
                         std::istreambuf_iterator<char>());
        return str;
    } 

    std::cerr << "Failed to open " << filepath << std::endl;
    return "";
}

std::string dirname(std::string filepath) {
    #if defined(WIN32) || defined(_WIN32)
        std::size_t slash = filepath.find("\\");
        while (slash != std::string::npos) {
            filepath.replace(slash, 1, "/");
            slash = filepath.find("\\");
        }
    #endif

    std::size_t pos = filepath.find_last_of("/");

    if (pos == std::string::npos)
        return "";

    return filepath.substr(0, pos);
}

static bool jsonMemberCheck(const rapidjson::Value& val, 
                            std::string memberName, 
                            std::string memberType) {
    
    if (!val.HasMember(memberName.c_str())) 
        return false;

    if (memberType == "object") {
        return val[memberName.c_str()].IsObject();
    } else if (memberType == "string") {
        return val[memberName.c_str()].IsString();
    } else if (memberType == "number") {
        return val[memberName.c_str()].IsNumber();
    } else if (memberType == "array") {
        return val[memberName.c_str()].IsArray();
    } else {
        return false;
    }
}

static glm::vec3 parseVec3(const rapidjson::Value& jsonVec) {
    assert(jsonVec.IsArray());
    assert(jsonVec.Capacity() == 3);
    assert(jsonVec[0].IsNumber() && jsonVec[1].IsNumber() && jsonVec[2].IsNumber());

    return glm::vec3(jsonVec[0].GetFloat(), jsonVec[1].GetFloat(), jsonVec[2].GetFloat());
}

static glm::mat4 parseMat4(const rapidjson::Value& jsonMat) {
    assert(jsonMat.IsArray());
    assert(jsonMat.Capacity() == 4);
    assert(jsonMat[0].Capacity() == 4);
    assert(jsonMat[1].Capacity() == 4);
    assert(jsonMat[2].Capacity() == 4);
    assert(jsonMat[3].Capacity() == 4);

    return glm::mat4(
        jsonMat[0][0].GetFloat(), jsonMat[1][0].GetFloat(), jsonMat[2][0].GetFloat(), jsonMat[3][0].GetFloat(),
        jsonMat[0][1].GetFloat(), jsonMat[1][1].GetFloat(), jsonMat[2][1].GetFloat(), jsonMat[3][1].GetFloat(),
        jsonMat[0][2].GetFloat(), jsonMat[1][2].GetFloat(), jsonMat[2][2].GetFloat(), jsonMat[3][2].GetFloat(),
        jsonMat[0][3].GetFloat(), jsonMat[1][3].GetFloat(), jsonMat[2][3].GetFloat(), jsonMat[3][3].GetFloat()
    );
}

static void parseCamera(const rapidjson::Value& jsonCamera, Camera3D &camera) {
    assert(jsonCamera.IsObject());

    assert(jsonMemberCheck(jsonCamera, "type", "string"));
    assert(jsonMemberCheck(jsonCamera, "focal_length", "number"));
    assert(jsonMemberCheck(jsonCamera, "eye", "array"));
    assert(jsonMemberCheck(jsonCamera, "up", "array"));
    assert(jsonMemberCheck(jsonCamera, "look", "array"));
    assert(jsonMemberCheck(jsonCamera, "height", "number"));
    assert(jsonMemberCheck(jsonCamera, "width", "number"));

    std::string cameraType = jsonCamera["type"].GetString();
    assert(cameraType == "perspective" && "Only perspective cameras are supported");

    float width = jsonCamera["width"].GetFloat();
    float height = jsonCamera["height"].GetFloat();
    float focal_length = jsonCamera["focal_length"].GetFloat();

    float ratio = width/height;
    float fov = 2.0f * std::atan2(0.5f * height, focal_length);

    glm::vec3 eye = parseVec3(jsonCamera["eye"]);
    glm::vec3 up = parseVec3(jsonCamera["up"]);
    glm::vec3 look = parseVec3(jsonCamera["look"]);

    camera.update(focal_length, fov, ratio, eye, up, look);
}

static Material parseMaterial(const rapidjson::Value& jsonMaterial) {
    assert(jsonMaterial.IsObject());
    assert(jsonMemberCheck(jsonMaterial, "ka", "array"));
    assert(jsonMemberCheck(jsonMaterial, "kd", "array"));
    assert(jsonMemberCheck(jsonMaterial, "ks", "array"));
    assert(jsonMemberCheck(jsonMaterial, "km", "array"));
    assert(jsonMemberCheck(jsonMaterial, "name", "string"));
    assert(jsonMemberCheck(jsonMaterial, "phong_exponent", "number"));

    return Material(jsonMaterial["phong_exponent"].GetFloat(),
                    parseVec3(jsonMaterial["ka"]),
                    parseVec3(jsonMaterial["kd"]),
                    parseVec3(jsonMaterial["ks"]),
                    parseVec3(jsonMaterial["km"]));
}

static Light parseLight(const rapidjson::Value& jsonLight) {
    assert(jsonLight.IsObject());

    assert(jsonMemberCheck(jsonLight, "type", "string"));
    assert(jsonMemberCheck(jsonLight, "color", "array"));

    glm::vec3 lightColor = parseVec3(jsonLight["color"]);

    glm::vec3 lightDiffuse = 1.0f * lightColor;
    glm::vec3 lightAmbient = 0.2f * lightDiffuse;
    glm::vec3 lightSpecular = { 1.0f, 1.0f, 1.0f };

    std::string lightType = jsonLight["type"].GetString();

    if (lightType == "point") {
        assert(jsonMemberCheck(jsonLight, "position", "array"));
        glm::vec3 lightPosition = parseVec3(jsonLight["position"]);

        return Light(LightType::Point,
                     lightPosition,
                     lightAmbient,
                     lightDiffuse,
                     lightSpecular);
    } else if (lightType == "directional") {
        assert(jsonMemberCheck(jsonLight, "direction", "array"));
        glm::vec3 lightDirection = parseVec3(jsonLight["direction"]);

        return Light(LightType::Directional,
                     lightDirection,
                     lightAmbient,
                     lightDiffuse,
                     lightSpecular);
    }
        
    std::cerr << "Light type not supported " << std::endl;
    return Light();
}

Scene::Scene()
        : lights{}
        , objects{} {}

Scene::Scene(float f,
             const std::vector<Light> &l,
             const std::vector<Object3D*> &o)
        : lights{l}
        , objects{o} {}

bool Scene::loadSceneFromJSON(std::string filepath){
    rapidjson::Document document;

    std::string jsonString = readFile(filepath);
    if (jsonString.empty())
        return false;

    rapidjson::ParseResult ok = document.Parse(jsonString.c_str());
    
    if (!ok) {
        std::cerr << "JSON parse error: " << rapidjson::GetParseError_En(ok.Code()) << std::endl;
        return false;
    }

    assert(jsonMemberCheck(document, "camera", "object"));
    assert(jsonMemberCheck(document, "materials", "array"));
    assert(jsonMemberCheck(document, "lights", "array"));
    assert(jsonMemberCheck(document, "objects", "array"));

    const rapidjson::Value& jsonCamera = document["camera"];
    parseCamera(jsonCamera, camera);
    
    std::map<std::string, int> materialDict;
    const rapidjson::Value& jsonMaterials = document["materials"];

    for (rapidjson::SizeType i = 0; i < jsonMaterials.Size(); i++) {
        const rapidjson::Value& jsonMaterial = jsonMaterials[i];

        materials.push_back(parseMaterial(jsonMaterial));
        materialDict[jsonMaterial["name"].GetString()] = i;
    }

    const rapidjson::Value& jsonLights = document["lights"];
    for (rapidjson::SizeType i = 0; i < jsonLights.Size(); i++)
        lights.push_back(parseLight(jsonLights[i]));

    const rapidjson::Value& jsonObjects = document["objects"];

    for (rapidjson::SizeType i = 0; i < jsonObjects.Size(); i++) {
        assert(jsonObjects[i].IsObject());
        const rapidjson::Value& jsonObject = jsonObjects[i];

        assert(jsonMemberCheck(jsonObject, "type", "string"));
        assert(jsonMemberCheck(jsonObject, "material", "string"));

        std::string objectType = jsonObject["type"].GetString();
        std::string objectMaterialName = jsonObject["material"].GetString();

        int objectMaterialIndex = materialDict[objectMaterialName];
        Material *objectMaterial = &(materials[objectMaterialIndex]);

        if (objectType == "plane") {
            assert(jsonMemberCheck(jsonObject, "point", "array"));
            assert(jsonMemberCheck(jsonObject, "normal", "array"));

            glm::vec3 objectPoint = parseVec3(jsonObject["point"]);
            glm::vec3 objectNormal = parseVec3(jsonObject["normal"]);

            objects.push_back(new Plane(objectMaterial,
                                        objectPoint,
                                        objectNormal));
        } else if (objectType == "sphere") {
            assert(jsonMemberCheck(jsonObject, "radius", "number"));
            assert(jsonMemberCheck(jsonObject, "center", "array"));

            float objectRadius = jsonObject["radius"].GetFloat();
            glm::vec3 objectCenter = parseVec3(jsonObject["center"]);

            objects.push_back(new Sphere(objectMaterial,
                                         objectRadius,
                                         objectCenter));
        } else if (objectType == "triangle") {
            assert(jsonMemberCheck(jsonObject, "vertices", "array"));

            objects.push_back(new Triangle(objectMaterial,
                                           parseVec3(jsonObject["vertices"][0]),
                                           parseVec3(jsonObject["vertices"][1]),
                                           parseVec3(jsonObject["vertices"][2])));
        } else if (objectType == "mesh") {
            assert(jsonMemberCheck(jsonObject, "format", "string"));
            assert(jsonMemberCheck(jsonObject, "filename", "string"));

            std::string meshFormat = jsonObject["format"].GetString();
            std::string meshFilepath = dirname(filepath) + "/" + jsonObject["filename"].GetString();

            TriangleMesh *mesh = new TriangleMesh(objectMaterial);

            if (meshFormat == "OFF")
                mesh->readFromOFF(meshFilepath);

            if (jsonMemberCheck(jsonObject, "model-matrix", "array"))
                mesh->transform(parseMat4(jsonObject["model-matrix"]));

            objects.push_back(static_cast<Object3D*>(mesh));
        }

    }

    return true;
}