#include "Object3D.h"

Material::Material()
        : shiness{0.0f}
        , ka{glm::vec3(0.0f)}
        , kd{glm::vec3(0.0f)}
        , ks{glm::vec3(0.0f)}
        , km{glm::vec3(0.0f)} {}

Material::Material(float p,
                   const glm::vec3 &a,
                   const glm::vec3 &d,
                   const glm::vec3 &s,
                   const glm::vec3 &m)
        : shiness{p}
        , ka{glm::vec3(a)}
        , kd{glm::vec3(d)}
        , ks{glm::vec3(s)}
        , km{glm::vec3(m)} {}

Plane::Plane()
        : normal{0.0f,  1.0f, 0.0f}
        , center{0.0f, -1.0f, 0.0f} {}

Plane::Plane(const glm::vec3 &c, const glm::vec3 &n)
        : center{c}
        , normal{n} {}

Plane::Plane(Material* m, const glm::vec3 &c, const glm::vec3 &n)
        : Object3D{m}
        , center{c}
        , normal{n} {}

bool Plane::intersectRay(const glm::vec3 &e,
                         const glm::vec3 &d,
                         float &t,
                         glm::vec3 &n) {

    float denom = glm::dot(d, normal);

    if (fabs(denom) > 0) {
        glm::vec3 diff = center - e;
        float this_t = glm::dot(diff, normal)/denom;

        if (this_t > 0) {
            n = normal;
            t = this_t;
            return true;
        }
    }

    return false;
}

Sphere::Sphere()
        : radius{0.0f}
        , center{0.0f} {}

Sphere::Sphere(float r)
        : radius{r}
        , center{0.0f} {}

Sphere::Sphere(float r, const glm::vec3 &c)
        : radius{r}
        , center{c} {}

Sphere::Sphere(Material *m, float r, const glm::vec3 &c)
        : Object3D{m}
        , radius{r}
        , center{c} {}

bool Sphere::intersectRay(const glm::vec3 &e,
                          const glm::vec3 &d,
                          float &t,
                          glm::vec3 &n) {

    bool intersected = false;

    float A = glm::dot(d, d);
    float B = glm::dot(2.0f * d, e - center);
    float C = glm::dot(e - center, e - center) - radius*radius;

    float discrimininant = B*B - 4*A*C;

    if (discrimininant >= 0) {
        float t1 = (-1 * B + sqrt(discrimininant)) / (2 * A);
        float t2 = (-1 * B - sqrt(discrimininant)) / (2 * A);

        t = t1 < t2 ? t1 : t2;

        glm::vec3 intersection = e + t * d;
        n = glm::normalize(intersection - center);

        intersected = true;
    }

    return intersected;
}

Triangle::Triangle()
        : a{0.0f}
        , b{0.0f}
        , c{0.0f} {}

Triangle::Triangle(const glm::vec3 &va,
                   const glm::vec3 &vb,
                   const glm::vec3 &vc)
        : a{va}
        , b{vb}
        , c{vc} {}

Triangle::Triangle(Material *m,
                   const glm::vec3 &va,
                   const glm::vec3 &vb,
                   const glm::vec3 &vc)
        : Object3D{m}
        , a{va}
        , b{vb}
        , c{vc} {}

void Triangle::transform(const glm::mat4 &model) {
    a = glm::vec3(model * glm::vec4(a, 1.0f));
    b = glm::vec3(model * glm::vec4(b, 1.0f));
    c = glm::vec3(model * glm::vec4(c, 1.0f));
}

bool Triangle::intersectRay(const glm::vec3 &e,
                            const glm::vec3 &d,
                            float &t,
                            glm::vec3 &n) {

    float xa = a.x, ya = a.y, za = a.z;
    float xb = b.x, yb = b.y, zb = b.z;
    float xc = c.x, yc = c.y, zc = c.z;
    float xd = d.x, yd = d.y, zd = d.z;
    float xe = e.x, ye = e.y, ze = e.z;

    glm::mat3 A = {
        xa-xb, xa-xc, xd,
        ya-yb, ya-yc, yd,
        za-zb, za-zc, zd
    };

    // compute t
    glm::mat3 M1 = {
        xa-xb, xa-xc, xa-xe,
        ya-yb, ya-yc, ya-ye,
        za-zb, za-zc, za-ze
    };

    t = glm::determinant(M1)/glm::determinant(A);
    if (std::isnan(t) || t == -INFINITY || t == INFINITY)
        return false;

    // compute gamma
    glm::mat3 M2 = {
        xa-xb, xa-xe, xd,
        ya-yb, ya-ye, yd,
        za-zb, za-ze, zd
    };

    float gamma = glm::determinant(M2)/glm::determinant(A);

    if (gamma < 0.0f || gamma > 1.0f)
        return false;

    // compute beta
    glm::mat3 M3 = {
        xa-xe, xa-xc, xd,
        ya-ye, ya-yc, yd,
        za-ze, za-zc, zd
    };

    float beta = glm::determinant(M3)/glm::determinant(A);

    if (beta < 0.0f || beta > 1.0f-gamma)
        return false;

    n = glm::normalize(glm::cross(c-a, b-a));

    return true;
}

TriangleMesh::TriangleMesh()
        : AABB { std::numeric_limits<float>::infinity(),
                -std::numeric_limits<float>::infinity(),
                 std::numeric_limits<float>::infinity(),
                -std::numeric_limits<float>::infinity(),
                 std::numeric_limits<float>::infinity(),
                -std::numeric_limits<float>::infinity()} {}

TriangleMesh::TriangleMesh(Material* m)
        : Object3D{m}
        , AABB { std::numeric_limits<float>::infinity(),
                -std::numeric_limits<float>::infinity(),
                 std::numeric_limits<float>::infinity(),
                -std::numeric_limits<float>::infinity(),
                 std::numeric_limits<float>::infinity(),
                -std::numeric_limits<float>::infinity()} {}

void TriangleMesh::recomputeAABB() {
    for (unsigned i = 0; i < vertices.size(); i++) {
        const glm::vec3 &v = vertices[i];

        AABB.xMin = std::min(AABB.xMin, v.x);
        AABB.xMax = std::max(AABB.xMax, v.x);

        AABB.yMin = std::min(AABB.yMin, v.y);
        AABB.yMax = std::max(AABB.yMax, v.y);

        AABB.zMin = std::min(AABB.zMin, v.z);
        AABB.zMax = std::max(AABB.zMax, v.z);
    }
}

bool TriangleMesh::intersectAABB(const glm::vec3 &e, const glm::vec3 &d) {
    float tmin = (AABB.xMin - e.x) / d.x;
    float tmax = (AABB.xMax - e.x) / d.x;

    if (tmin > tmax)
        std::swap(tmin, tmax);

    float tymin = (AABB.yMin - e.y) / d.y;
    float tymax = (AABB.yMax - e.y) / d.y;

    if (tymin > tymax)
        std::swap(tymin, tymax);

    if ((tmin > tymax) || (tymin > tmax))
        return false;

    if (tymin > tmin)
        tmin = tymin;

    if (tymax < tmax)
        tmax = tymax;

    float tzmin = (AABB.zMin - e.z) / d.z;
    float tzmax = (AABB.zMax - e.z) / d.z;

    if (tzmin > tzmax)
        std::swap(tzmin, tzmax);

    if ((tmin > tzmax) || (tzmin > tmax))
        return false;

    if (tzmin > tmin)
        tmin = tzmin;

    if (tzmax < tmax)
        tmax = tzmax;

    return true;
}

void TriangleMesh::transform(const glm::mat4 &model) {
    for (int i = 0; i < triangles.size(); i++)
        triangles[i].transform(model);
    for (int j = 0; j < vertices.size(); j++)
        vertices[j] = glm::vec3(model * glm::vec4(vertices[j], 1.0f));
    recomputeAABB();
}

bool TriangleMesh::readFromOFF(std::string filename) {
    std::fstream inFile(filename);

    if (!inFile) {
        std::cerr << "Faied to open " << filename << std::endl;
        return false;
    }

    std::string firstLine;
    inFile >> firstLine;

    if (firstLine != "OFF") {
        std::cerr << "Invalid File format." << std::endl;
        return false;
    }

    int nVertices, nFaces, nEdges;
    inFile >> nVertices >> nFaces >> nEdges;

    triangles.reserve(nFaces);
    vertices.reserve(nVertices);

    // read vertices
    for (int i = 0; i < nVertices; i++) {
        float x, y, z;
        inFile >> x >> y >> z;
        vertices.emplace_back(glm::vec3(x, y, z));
    }

    int temp = 0;
    // read faces
    for (int j = 0; j < nFaces; j++) {
        int aIndex, bIndex, cIndex;
        inFile >> temp >> aIndex >> bIndex >> cIndex;
        glm::vec3 a(vertices[aIndex].x, vertices[aIndex].y, vertices[aIndex].z);
        glm::vec3 b(vertices[bIndex].x, vertices[bIndex].y, vertices[bIndex].z);
        glm::vec3 c(vertices[cIndex].x, vertices[cIndex].y, vertices[cIndex].z);

        triangles.emplace_back(Triangle(a, b, c));
    }

    recomputeAABB();

    inFile.close();
    return true;
}


bool TriangleMesh::intersectRay(const glm::vec3 &e,
                                const glm::vec3 &d,
                                float &t,
                                glm::vec3 &n) {

    if (!intersectAABB(e, d))
        return false;

    bool intersected = false;
    float min_t = std::numeric_limits<float>::infinity();

    for (int i = 0; i < triangles.size(); i++) {
        float this_t;
        glm::vec3 this_n;
        bool this_bool = triangles[i].intersectRay(e, d, this_t, this_n);

        if (this_bool && this_t < min_t) {
            t = this_t;
            n = this_n;
            min_t = this_t;
            intersected = true;
        }
    }

    return intersected;
}
