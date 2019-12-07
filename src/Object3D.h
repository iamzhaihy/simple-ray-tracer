#pragma once

#include <cmath>
#include <limits>
#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Material {
public:
    float shiness;
    glm::vec3 ka;
    glm::vec3 kd;
    glm::vec3 ks;
    glm::vec3 km;

    Material();
    Material(float p,
             const glm::vec3 &a,
             const glm::vec3 &d,
             const glm::vec3 &s,
             const glm::vec3 &m);
};

class Object3D {
public:
    Material *material;
    Object3D(Material *m = new Material()): material{m} {}
    virtual bool intersectRay(const glm::vec3 &e, const glm::vec3 &d, float &t, glm::vec3 &n) = 0;
};

class Plane :public Object3D {
public:
    glm::vec3 normal;
    glm::vec3 center;

    Plane();
    Plane(const glm::vec3 &c,
          const glm::vec3 &n);
    Plane(Material* m,
          const glm::vec3 &c,
          const glm::vec3 &n);

    bool intersectRay(const glm::vec3 &e, const glm::vec3 &d, float &t, glm::vec3 &n);
};

class Sphere: public Object3D {
public:
    float radius;
    glm::vec3 center;

    Sphere();
    Sphere(float r);
    Sphere(float r, const glm::vec3 &c);
    Sphere(Material* m, float r, const glm::vec3 &c);

    bool intersectRay(const glm::vec3 &e, const glm::vec3 &d, float &t, glm::vec3 &n);
};

class Triangle : public Object3D {
public:
    glm::vec3 a;
    glm::vec3 b;
    glm::vec3 c;

    Triangle();
    Triangle(const glm::vec3 &va, const glm::vec3 &vb, const glm::vec3 &vc);
    Triangle(Material* m, const glm::vec3 &va, const glm::vec3 &vb, const glm::vec3 &vc);

    void transform(const glm::mat4 &model);

    bool intersectRay(const glm::vec3 &e, const glm::vec3 &d, float &t, glm::vec3 &n);
};

class TriangleMesh : public Object3D {
private:
    struct BoundingVolume {
        float xMin, xMax;
        float yMin, yMax;
        float zMin, zMax;
    };

    struct BoundingVolume AABB;

    void recomputeAABB();
    bool intersectAABB(const glm::vec3 &e, const glm::vec3 &d);

public:
	std::vector <Triangle> triangles;
    std::vector <glm::vec3> vertices;

    TriangleMesh();
    TriangleMesh(Material* m);

    void transform(const glm::mat4 &model);

    bool readFromOFF(std::string filename);
    bool intersectRay(const glm::vec3 &e, const glm::vec3 &d, float &t, glm::vec3 &n);
};
