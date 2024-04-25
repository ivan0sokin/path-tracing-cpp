#include "Mesh.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "../../tinyobjloader/tiny_obj_loader.h"

Mesh::Mesh(const char *pathToFile, const char *materialDirectory) noexcept {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warning;
    std::string error;
    tinyobj::LoadObj(&attrib, &shapes, &materials, &warning, &error, pathToFile, materialDirectory);
    
    if (!error.empty()) {
        fprintf(stderr, "Failed to load %s. Error occured: %s\n", pathToFile, error.c_str());
    }

    if (!warning.empty()) {
        fprintf(stderr, "Warning while loading %s: %s\n", pathToFile, warning.c_str());
    }

    for (size_t s = 0; s < shapes.size(); s++) {
        // if (shapes.size() > 1) {
            // fprintf(stderr, "wtf?????");
            // exit(-1);
        // }
        // Loop over faces(polygon)

        fprintf(stdout, "shape name: %s\n", shapes[s].name.c_str());

        m_Triangles.reserve(shapes[s].mesh.num_face_vertices.size());

        fprintf(stdout, "vertices: %d\n", shapes[s].mesh.num_face_vertices.size());

        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            int fv = shapes[s].mesh.num_face_vertices[f];

            if (fv != 3) {
                fprintf(stderr, "ALARM GOIDA!\n");
                exit(-1);
            }

            Math::Vector3f vertices[3];

            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; v++) {
            // access to vertex
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                tinyobj::real_t vx = attrib.vertices[3*idx.vertex_index+0];
                tinyobj::real_t vy = attrib.vertices[3*idx.vertex_index+1];
                tinyobj::real_t vz = attrib.vertices[3*idx.vertex_index+2];

                vertices[v] = {vx, vy, vz};

                // tinyobj::real_t nx = attrib.normals[3*idx.normal_index+0];
                // tinyobj::real_t ny = attrib.normals[3*idx.normal_index+1];
                // tinyobj::real_t nz = attrib.normals[3*idx.normal_index+2];
                // tinyobj::real_t tx = attrib.texcoords[2*idx.texcoord_index+0];
                // tinyobj::real_t ty = attrib.texcoords[2*idx.texcoord_index+1];
                // Optional: vertex colors
                // tinyobj::real_t red = attrib.colors[3*idx.vertex_index+0];
                // tinyobj::real_t green = attrib.colors[3*idx.vertex_index+1];
                // tinyobj::real_t blue = attrib.colors[3*idx.vertex_index+2];
            }
            index_offset += fv;

            // m_Triangles.emplace_back(vertices[0], vertices[1], vertices[2], 0);

            m_Min = Math::Min(Math::Min(vertices[0], vertices[1]), Math::Min(vertices[2], m_Min));
            m_Max = Math::Max(Math::Max(vertices[0], vertices[1]), Math::Max(vertices[2], m_Max));

            // per-face material
            shapes[s].mesh.material_ids[f];
        }
    }
}

void Mesh::Hit(const Ray &ray, float tMin, float tMax, HitPayload &payload) const noexcept {
    m_Root->Hit(ray, tMin, tMax, payload);
}

BVHNode* fuck(std::vector<const Shapes::Triangle*> &objects, int low, int high) noexcept {
    if (low + 1 == high) {
        return new BVHNode(objects[low]);
    }
    
    std::sort(objects.begin() + low, objects.begin() + high, [](const Shapes::Triangle *a, const Shapes::Triangle *b){
        return a->vertices[0].x + a->vertices[1].x + a->vertices[2].x < b->vertices[0].x + b->vertices[1].x + b->vertices[2].x;
    });

    std::vector<AABB> left(high - low + 1);
    left[0] = AABB::Empty();
    for (int i = low; i < high; ++i) {
        left[i - low + 1] = AABB(left[i - low], objects[i]->GetBoundingBox());
    }

    std::vector<AABB> right(high - low + 1);
    right[high - low] = AABB::Empty();
    for (int i = high - 1; i >= low; --i) {
        right[i - low] = AABB(objects[i]->GetBoundingBox(), right[i - low + 1]);
    }

    float minX = Math::Constants::Infinity<float>;
    int minXIndex = -1;
    for (int i = low; i < high; ++i) {
        float f = left[i - low + 1].GetSurfaceArea() * (float)(i - low) + right[i - low + 1].GetSurfaceArea() * (float)((high - low) - (i - low));
        if (f < minX) {
            minX = f;
            minXIndex = i;
        }
    }

    std::sort(objects.begin() + low, objects.begin() + high, [](const Shapes::Triangle *a, const Shapes::Triangle *b){
        return a->vertices[0].y + a->vertices[1].y + a->vertices[2].y < b->vertices[0].y + b->vertices[1].y + b->vertices[2].y;
    });

    left[0] = AABB::Empty();
    for (int i = low; i < high; ++i) {
        left[i - low + 1] = AABB(left[i - low], objects[i]->GetBoundingBox());
    }

    right[high - low] = AABB::Empty();
    for (int i = high - 1; i >= low; --i) {
        right[i - low] = AABB(objects[i]->GetBoundingBox(), right[i - low + 1]);
    }

    float minY = Math::Constants::Infinity<float>;
    int minYIndex = -1;
    for (int i = low; i < high; ++i) {
        float f = left[i - low + 1].GetSurfaceArea() * (float)(i - low) + right[i - low + 1].GetSurfaceArea() * (float)((high - low) - (i - low));
        if (f < minY) {
            minY = f;
            minYIndex = i;
        }
    }

    std::sort(objects.begin() + low, objects.begin() + high, [](const Shapes::Triangle *a, const Shapes::Triangle *b){
        return a->vertices[0].z + a->vertices[1].z + a->vertices[2].z < b->vertices[0].z + b->vertices[1].z + b->vertices[2].z;
    });

    left[0] = AABB::Empty();
    for (int i = low; i < high; ++i) {
        left[i - low + 1] = AABB(left[i - low], objects[i]->GetBoundingBox());
    }

    right[high - low] = AABB::Empty();
    for (int i = high - 1; i >= low; --i) {
        right[i - low] = AABB(objects[i]->GetBoundingBox(), right[i - low + 1]);
    }

    float minZ = Math::Constants::Infinity<float>;
    int minZIndex = -1;
    for (int i = low; i < high; ++i) {
        float f = left[i - low + 1].GetSurfaceArea() * (float)(i - low) + right[i - low + 1].GetSurfaceArea() * (float)((high - low) - (i - low));
        if (f < minZ) {
            minZ = f;
            minZIndex = i;
        }
    }

    int mid = -1;
    if (minX < minY && minX < minZ) {
        mid = minXIndex + 1;
        std::sort(objects.begin() + low, objects.begin() + high, [](const Shapes::Triangle *a, const Shapes::Triangle *b){
            return a->vertices[0].x + a->vertices[1].x + a->vertices[2].x < b->vertices[0].x + b->vertices[1].x + b->vertices[2].x;
        });
    } else if (minY < minX && minY < minZ) {
        mid = minYIndex + 1;
        std::sort(objects.begin() + low, objects.begin() + high, [](const Shapes::Triangle *a, const Shapes::Triangle *b){
            return a->vertices[0].y + a->vertices[1].y + a->vertices[2].y < b->vertices[0].y + b->vertices[1].y + b->vertices[2].y;
        });
    } else {
        mid = minZIndex + 1;
        std::sort(objects.begin() + low, objects.begin() + high, [](const Shapes::Triangle *a, const Shapes::Triangle *b){
            return a->vertices[0].z + a->vertices[1].z + a->vertices[2].z < b->vertices[0].z + b->vertices[1].z + b->vertices[2].z;
        });
    }

    BVHNode *lft = fuck(objects, low, mid);
    BVHNode *rgt = fuck(objects, mid, high);

    return new BVHNode(lft, rgt);
}

BVHNode* Mesh::MakeHierarchy() const noexcept {
    std::vector<const Shapes::Triangle*> triangles;
    for (auto &trig : m_Triangles) {
        triangles.push_back(&trig);
    }

    return fuck(triangles, 0, (int)m_Triangles.size());
}

int Mesh::GetMaterialIndex() const noexcept {
    return 0;
}

AABB Mesh::GetBoundingBox() const noexcept {
    return AABB(m_Min - 0.001f, m_Max + 0.001f);
}