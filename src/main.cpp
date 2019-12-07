// C++ include
#include <limits>
#include <string>
#include <vector>
#include <cstddef>
#include <iostream>

// Image writing library
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "utils.h"

#include "Scene.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

static int MAXRECURSION = 3;
static float EPSILON = 1e-4f;
static float FLOAT_INF = std::numeric_limits<float>::infinity();

struct HitRecord {
    int idx;
    float t;
    float phong;

    glm::vec3 n;
    glm::vec3 ka;
    glm::vec3 kd;
    glm::vec3 ks;
    glm::vec3 km;
};

// hlper functions to do simple string processing
static std::string getDirname(std::string filepath);
static std::string getFileName(std::string filepath);

// returns true if the ray hits any object when t is in [t0, t1]
static bool findIntersections(const std::vector<Object3D*> objs, const glm::vec3 &e, const glm::vec3 &d, float t0, float t1);
// compute the color of a pixel using Blinn-Phong Shading
static glm::vec3 raycolor(const Scene &scene, const glm::vec3 &e, const glm::vec3 &d, float t0, float t1, int recursionDepth);
// find the nearest intersection and record necessary info to compute color
static bool findNearestIntersection(const Scene &scene, const glm::vec3 &e, const glm::vec3 &d, float t0, float t1, HitRecord &rec);

int main(int argc, char *argv[]) {
    Scene scene;
    std::vector<glm::vec4> pixels;

    std::string jsonPath = argc <= 1
                           ? "../data/sphere-and-plane.json"
                           : std::string(argv[1]);

    if (!scene.loadSceneFromJSON(jsonPath)) {
        std::cerr << "Failed to load scene from JSON" << std::endl;
        return -1;
    }

    const std::string filename = getFileName(jsonPath) + ".png";
    std::cout << "Rendering scene defined in " << jsonPath << std::endl;

    Camera3D& camera = scene.camera;
    std::vector<Object3D*> &objects = scene.objects;

    glm::vec3 e = camera.getPosition();

    glm::mat4 inv_view = glm::inverse(camera.getViewMatrix());
    glm::mat4 inv_proj = glm::inverse(camera.getProjectionMatrix());

    int IMAGE_HEIGHT = 720;
    int IMAGE_WIDTH = static_cast<int>(camera.getRatio() * IMAGE_HEIGHT);

    for (int j = 0; j < IMAGE_HEIGHT; j++) {
        for (int i = 0; i < IMAGE_WIDTH; i++) {
            glm::vec4 p_clip = {
                2.0f * (float)i/(float)IMAGE_WIDTH - 1.0f,
                1.0f - 2.0f * (float)j/(float)IMAGE_HEIGHT,
                -1.0f,
                1.0f
            };

            glm::vec4 p_eye = inv_proj * p_clip;
            p_eye = glm::vec4(p_eye.x, p_eye.y, -1.0f, 0.0f);

            glm::vec3 d = glm::normalize(glm::vec3(inv_view * p_eye));

            glm::vec3 L = raycolor(scene, e, d, camera.getFocalLength(), FLOAT_INF, 1);

            if (findIntersections(objects, e, d, camera.getFocalLength(), FLOAT_INF))
                pixels.push_back(glm::vec4(L, 1.0f));
            else
                pixels.push_back(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        } // each col
    } // each row

    write_matrix_to_png(pixels, IMAGE_HEIGHT, IMAGE_WIDTH, filename);
    std::cout << "Image written to " << filename << std::endl;
}

static std::string getDirname(std::string filepath) {
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

static std::string getFileName(std::string filepath) {
    #if defined(WIN32) || defined(_WIN32)
        std::size_t slash = filepath.find("\\");
        while (slash != std::string::npos) {
            filepath.replace(slash, 1, "/");
            slash = filepath.find("\\");
        }
    #endif

    std::size_t pos1 = filepath.find_last_of("/");
    std::size_t pos2 = filepath.find_last_of(".");

    if (pos1 == std::string::npos || pos2 == std::string::npos)
        return "";

    return filepath.substr(pos1 + 1, pos2 - (pos1 + 1));
}

static bool findNearestIntersection(const Scene &scene, const glm::vec3 &e, const glm::vec3 &d, float t0, float t1, HitRecord &rec) {

    bool intersected = false;
    const std::vector<Object3D*> &objs = scene.objects;
    float min_t = std::numeric_limits<float>::infinity();

    for (int i = 0; i < objs.size(); i++) {
        float this_t;
        glm::vec3 this_n;
        bool this_bool = objs[i]->intersectRay(e, d, this_t, this_n);

        if (this_bool && t0 < this_t && this_t < t1 && this_t < min_t) {
            min_t = this_t;
            intersected = true;

            rec.idx = i;
            rec.t = this_t;
            rec.n = this_n;
            rec.ka = objs[i]->material->ka;
            rec.kd = objs[i]->material->kd;
            rec.ks = objs[i]->material->ks;
            rec.km = objs[i]->material->km;
            rec.phong = objs[i]->material->shiness;
        }
    }

    return intersected;
}

static bool findIntersections(const std::vector<Object3D*> objs, const glm::vec3 &e, const glm::vec3 &d, float t0, float t1) {
    for (int k = 0; k < objs.size(); k++) {
        float this_t;
        glm::vec3 this_n;
        bool this_bool = objs[k]->intersectRay(e, d, this_t, this_n);

        if (this_bool && t0 < this_t && this_t < t1)
            return true;
    }

    return false;
}

static glm::vec3 raycolor(const Scene &scene, const glm::vec3 &e, const glm::vec3 &d, float t0, float t1, int recursionDepth) {
    HitRecord rec;
    glm::vec3 color(0.0f, 0.0f, 0.0f);

    const std::vector<Light> &lights = scene.lights;
    const std::vector<Object3D*> &objects = scene.objects;

    bool intersected = findNearestIntersection(scene, e, d, t0, t1, rec);

    if (intersected) {
        glm::vec3 hit = e + rec.t * d;
        glm::vec3 adjustedHit = hit + EPSILON * rec.n;

        for (int j = 0; j < lights.size(); j++) {
            glm::vec3 l(0.0f);
            if (lights[j].type == LightType::Point)
                l = glm::normalize(lights[j].position - hit);
            else if (lights[j].type == LightType::Directional)
                l = glm::normalize(-1.0f * lights[j].direction);

            float tMax = 0.0f;
            if (lights[j].type == LightType::Point)
                tMax = glm::length(lights[j].position - hit);
            else if (lights[j].type == LightType::Directional)
                tMax = FLOAT_INF;

            color += rec.ka * lights[j].ambient;

            if (!findIntersections(objects, adjustedHit, l, 0.0f, tMax)) {
                glm::vec3 v = glm::normalize(e - hit);
                glm::vec3 h = glm::normalize(v + l);

                float diff = std::max(0.0f, glm::dot(rec.n, l));
                glm::vec3 diffuse = lights[j].diffuse * (diff * rec.kd);

                float spec = pow(std::max(0.0f, glm::dot(rec.n, h)), rec.phong);
                glm::vec3 specular = lights[j].specular * (spec * rec.ks);

                color += diffuse + specular;
            }
        }

        if (recursionDepth < MAXRECURSION) {
            glm::vec3 r = glm::reflect(d, rec.n);
            color += rec.km * raycolor(scene, adjustedHit, r, 0.0f, FLOAT_INF, recursionDepth + 1);
        }
    }
    return color;
}