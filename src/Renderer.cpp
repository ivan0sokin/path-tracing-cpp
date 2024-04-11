#include "Renderer.h"
#include "Utilities.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb-master/stb_image_write.h"

#include "../glm/include/glm/vec2.hpp"
#include "../glm/include/glm/vec3.hpp"
#include "../glm/include/glm/geometric.hpp"

#include <vector>
#include <numeric>
#include <execution>
#include <thread>
#include <cstring>

Renderer::Renderer(int width, int height) noexcept :
    m_Width(width), m_Height(height) {
    m_ImageData = new uint32_t[m_Width * m_Height];
    m_Image = new Image(m_Width, m_Height, m_ImageData);
    m_AccumulationData = new glm::vec4[m_Width * m_Height];

    m_AvailableThreads = std::thread::hardware_concurrency();
    m_UsedThreads = 1;
    m_LinesPerThread = (m_Height + m_UsedThreads - 1) / m_UsedThreads;
}

Renderer::~Renderer() noexcept {
    if (m_Image != nullptr) {
        delete m_Image;
    }
    if (m_ImageData != nullptr) {
        delete[] m_ImageData;
    }
    if (m_AccumulationData != nullptr) {
        delete[] m_AccumulationData;
    }
}

void Renderer::SaveImage(const char *filename) const noexcept {
    stbi_write_png(filename, m_Width, m_Height, 4, m_ImageData, m_Width * 4);
}

void Renderer::OnResize(int width, int height) noexcept {
    if (m_Width == width && m_Height == height) {
        return;
    }
    
    m_Width = width;
    m_Height = height;

    if (m_ImageData != nullptr) {
        delete m_ImageData;
        m_ImageData = new uint32_t[m_Width * m_Height];
    }
    if (m_Image != nullptr) {
        delete m_Image;
        m_Image = new Image(m_Width, m_Height, m_ImageData);
    }
    if (m_AccumulationData != nullptr) {
        delete m_AccumulationData;
        m_AccumulationData = new glm::vec4[m_Width * m_Height];
    }
}

void Renderer::Render(const Camera &camera, const Scene &scene) noexcept {
    m_Camera = &camera;
    m_Scene = &scene;

    if (!m_Accumulate) {
        m_FrameIndex = 1;
    }

    if (m_FrameIndex == 1) {
        memset(m_AccumulationData, 0, m_Width * m_Height * sizeof(glm::vec4));
    }

    float inverseFrameIndex = 1.f / m_FrameIndex;
    float inverseGamma = 1.f / m_Gamma;

    std::vector<std::thread> handles;
    handles.reserve(m_UsedThreads);

    for (int i = 0; i < m_Height; i += m_LinesPerThread) {
        handles.emplace_back([this, i, inverseFrameIndex, inverseGamma]() {
            int nextBlock = i + m_LinesPerThread;
            int limit = nextBlock < m_Height ? nextBlock : m_Height;
            for (int t = i; t < limit; ++t) {
                for (int j = 0; j < m_Width; ++j) {
                    m_AccumulationData[m_Width * t + j] += PixelProgram(t, j);

                    glm::vec4 color = m_AccumulationData[m_Width * t + j];
                    if (color.r != color.r) color.r = 0.f;
                    if (color.g != color.g) color.g = 0.f;
                    if (color.b != color.b) color.b = 0.f;

                    color *= inverseFrameIndex;
                    color = Utilities::CorrectGamma(color, inverseGamma);
                    color = glm::clamp(color, 0.f, 1.f);

                    m_ImageData[m_Width * t + j] = Utilities::ConvertColorToRGBA(color);
                }
            }
        });
    }

    for (auto &handle : handles) {
        handle.join();
    }

    m_Image->UpdateData(m_ImageData);

    if (m_Accumulate) {
        ++m_FrameIndex;
    }
}

glm::vec4 Renderer::PixelProgram(int i, int j) const noexcept {
    Ray ray;
    ray.origin = m_Camera->GetPosition();
    ray.direction = m_Camera->GetRayDirections()[m_Width * i + j];

    glm::vec3 light(0.f), energy(1.f);
    for (int i = 0; i < m_MaxRayDepth; ++i) {
        HitPayload payload = TraceRay(ray);
        if (payload.t < 0.f) {
            light += energy * m_OnRayMiss(ray);
            break;
        }


        // const Material &material = m_Scene->materials[materialIndex];
        
        const Material &material = payload.material;

        float diffuseRatio = 0.5 * (1.0 - material.metallic);
        float specularRatio = 1 - diffuseRatio;

        glm::vec3 V = glm::normalize(-ray.direction);

        glm::vec3 reflectionDirection;
        if (Utilities::RandomFloatInZeroToOne() < diffuseRatio) {
            reflectionDirection = Utilities::RandomInHemisphere(payload.normal);
        } else {
            glm::vec3 halfVec;
            {
                glm::vec2 Xi{Utilities::RandomFloatInZeroToOne(), Utilities::RandomFloatInZeroToOne()};
                glm::vec3 N = payload.normal;

                float a = material.roughness * material.roughness;

                float phi = 2.f * std::numbers::pi * Xi.x;
                float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a * a - 1.0) * Xi.y));
                float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

                // from spherical coordinates to cartesian coordinates
                glm::vec3 H;
                H.x = cos(phi) * sinTheta;
                H.y = sin(phi) * sinTheta;
                H.z = cosTheta;

                // from tangent-space vector to world-space sample vector
                glm::vec3 up = abs(N.z) < 0.999f ? glm::vec3(0.0, 0.0, 1.0) : glm::vec3(1.0, 0.0, 0.0);
                glm::vec3 tangent = glm::normalize(glm::cross(up, N));
                glm::vec3 bitangent = glm::cross(N, tangent);

                halfVec = tangent * H.x + bitangent * H.y + N * H.z;
                halfVec = glm::normalize(halfVec);
            }

            reflectionDirection = glm::normalize(2.f * glm::dot(V, halfVec) * halfVec - V);
        }

        auto DistributionGGX = [](const glm::vec3 &N, const glm::vec3 &H, float roughness) {
            float a = roughness * roughness;
            float a2 = a * a;
            float NdotH = std::max(glm::dot(N, H), 0.f);
            float NdotH2 = NdotH * NdotH;

            float nom = a2;
            float denom = (NdotH2 * (a2 - 1.0) + 1.0);
            denom = std::numbers::pi * denom * denom;

            return nom / denom;
        };

        auto GeometrySchlickGGX = [](float NdotV, float roughness) {
            float r = (roughness + 1.0);
            float k = (r * r) / 8.0;

            float nom = NdotV;
            float denom = NdotV * (1.0 - k) + k;

            return nom / denom;
        };

        auto GeometrySmith = [&](const glm::vec3 &N, const glm::vec3 &V, const glm::vec3 &L, float roughness) {
            float NdotV = abs(glm::dot(N, V));
            float NdotL = abs(glm::dot(N, L));
            float ggx2 = GeometrySchlickGGX(NdotV, roughness);
            float ggx1 = GeometrySchlickGGX(NdotL, roughness);

            return ggx1 * ggx2;
        };

        auto FresnelSchlick = [](float cosTheta, const glm::vec3 &F0) {
            return F0 + (1.f - F0) * pow(1.f - cosTheta, 5.f);
        };

        auto SpecularBRDF = [](float D, float G, const glm::vec3 &F, const glm::vec3 &V, const glm::vec3 &L, const glm::vec3 &N) {        
            float NdotL = abs(glm::dot(N, L));
            float NdotV = abs(glm::dot(N, V));
                    
            //specualr
            //Microfacet specular = D * G * F / (4 * NoL * NoV)
            glm::vec3 nominator = D * G * F;
            float denominator = 4.0 * NdotV * NdotL + 0.000001f;
            glm::vec3 specularBrdf = nominator / denominator;
            
            return specularBrdf;
        };
        

        auto DiffuseBRDF = [](const glm::vec3 &albedo) {
            return albedo * (float)std::numbers::inv_pi;
        };

        auto ImportanceSampleGGX_PDF = [](float NDF, float NdotH, float VdotH) {
            //ImportanceSampleGGX pdf
                //pdf = D * NoH / (4 * VoH)
            return NDF * NdotH / (4 * VdotH);
        };

        auto CosinSamplingPDF = [](float NdotL) {
            return NdotL * (float)std::numbers::inv_pi;
        };

        glm::vec3 L = glm::normalize(reflectionDirection);
        glm::vec3 H = glm::normalize(V + L);

        float NdotL = abs(glm::dot(payload.normal, L));
        float NdotH = abs(glm::dot(payload.normal, H));
        float VdotH = abs(glm::dot(V, H));
        
        float NdotV = abs(glm::dot(payload.normal, V));
        
        glm::vec3 F0 = glm::vec3(0.08, 0.08, 0.08);
        F0 = glm::mix(F0 * material.specular, material.albedo, material.metallic);

        float NDF = DistributionGGX(payload.normal, H, material.roughness);
        float G = GeometrySmith(payload.normal, V, L, material.roughness);
        glm::vec3 F = FresnelSchlick(std::max(dot(H, V), 0.f), F0);

        glm::vec3 kS = F;
        glm::vec3 kD = 1.f - kS;
        kD *= 1.0 - material.metallic;

        glm::vec3 specularBrdf = SpecularBRDF(NDF, G, F, V, L, payload.normal);
            
        //hemisphere sampling pdf
        //pdf = 1 / (2 * PI)
        //float speccualrPdf = 1 / (2 * PI);
        
        //ImportanceSampleGGX pdf
        //pdf = D * NoH / (4 * VoH)
        float speccualrPdf = ImportanceSampleGGX_PDF(NDF, NdotH, VdotH);
        
        //diffuse
        //Lambert diffuse = diffuse / PI
        glm::vec3 diffuseBrdf = DiffuseBRDF(material.albedo);
        //cosin sample pdf = N dot L / PI
        float diffusePdf = CosinSamplingPDF(NdotL);

        glm::vec3 totalBrdf = (diffuseBrdf * kD + specularBrdf) * NdotL;
        float totalPdf = diffuseRatio * diffusePdf + specularRatio * speccualrPdf;
            
        ray.origin = payload.point + payload.normal * 0.000001f;
        ray.direction = reflectionDirection;
        
        light += material.GetEmission() * energy;
        
        if (totalPdf > 0.0) {
            energy *= totalBrdf / totalPdf;
        }
    }

    return {light, 1.f};
}

// glm::vec4 Renderer::PixelProgram(int i, int j) const noexcept {
    // Ray ray;
    // ray.origin = m_Camera->GetPosition();
    // ray.direction = m_Camera->GetRayDirections()[m_Width * i + j];

    // glm::vec3 light(0.f);
    // glm::vec3 contribution(1.f);
    // for (int i = 0; i < m_MaxRayDepth; ++i) {
    //     HitPayload payload = TraceRay(ray);

    //     if (payload.t < 0.f) {
    //         light += m_OnRayMiss(ray) * contribution;
    //         break;
    //     }

    //     int materialIndex = -1;
    //     switch (payload.primitive)
    //     {
    //     case Primitive::Sphere:
    //         materialIndex = m_Scene->spheres[payload.objectIndex].materialIndex;
    //         break;
    //     case Primitive::Triangle:
    //         materialIndex = m_Scene->triangles[payload.objectIndex].materialIndex;
    //         break;        
    //     default:
    //         break;
    //     }

    //     const Material &material = m_Scene->materials[materialIndex];
        
    //     if (material.emissionPower != 0.f) {
    //         light += material.GetEmission() * contribution;
    //         break;
    //     }

    //     ray.origin = payload.point + payload.normal * 0.00001f;

    //     if (material.fuzziness > 1.f) {
    //         // ray.direction = glm::normalize(payload.normal + Utilities::RandomUnitVectorFast());

    //         // ray.direction = Utilities::RandomInHemisphereFast(payload.normal);

    //         // if (Utilities::AlmostZero(ray.direction)) {
    //         //     ray.direction = payload.normal;
    //         // } else {
    //         //     ray.direction = glm::normalize(ray.direction);
    //         // }

    //         // glm::vec3 w = payload.normal;
    //         // glm::vec3 a = (glm::abs(w.x) > 0.9) ? glm::vec3(0.f, 1.f, 0.f) : glm::vec3(1.f, 0.f, 0.f);
    //         // glm::vec3 v = glm::normalize(glm::cross(w, a));
    //         // glm::vec3 u = glm::cross(w, v);

    //         // glm::vec3 randomCosineDirection = Utilities::RandomCosineDirection();
    //         // ray.direction = randomCosineDirection.x * u + randomCosineDirection.y * v + randomCosineDirection.z * w;
    //         // ray.direction = glm::normalize(ray.direction);

    //         constexpr float inversePi = std::numbers::inv_pi;
    //         // float cosineThetaOverPi = glm::abs(glm::dot(ray.direction, w)) * inversePi;
    //         // float pdfValue = 0.f > cosineThetaOverPi ? 0.f : cosineThetaOverPi;
    //         float pdfValue = 0.f;
    //         for (int q = 0; q < 1; ++q)
    //         {
    //             const Triangle &triangle = m_Scene->triangles.back();

    //             constexpr float epsilon = std::numeric_limits<float>::epsilon();

    //             glm::vec3 uu = triangle.edges[0];
    //             glm::vec3 vv = triangle.edges[1];
            
    //             glm::vec3 randomInPar;
    //             // do {
    //                 randomInPar = triangle.vertices[0] + Utilities::RandomFloatInZeroToOne() * uu + Utilities::RandomFloatInZeroToOne() * vv;
    //             // } while (signbit(glm::cross(triangle.edges)) == )


    //             ray.direction = randomInPar - ray.origin;

    //             glm::vec3 edge1 = triangle.edges[0];
    //             glm::vec3 edge2 = triangle.edges[1];
    //             glm::vec3 rayCrossEdge2 = glm::cross(ray.direction, edge2);
    //             float determinant = glm::dot(edge1, rayCrossEdge2);

    //             if (glm::abs(determinant) < epsilon) {
    //                 break;
    //             }

    //             float inverseDeterminant = 1.0 / determinant;
    //             glm::vec3 s = ray.origin - triangle.vertices[0];
    //             float u = inverseDeterminant * glm::dot(s, rayCrossEdge2);

    //             if (u < 0.f || u > 1.f) {
    //                 break;
    //             }

    //             glm::vec3 sCrossEdge1 = glm::cross(s, edge1);
    //             float v = inverseDeterminant * glm::dot(ray.direction, sCrossEdge1);

    //             if (v < 0.f || u + v > 1.f) {
    //                 break;
    //             }

    //             float t = inverseDeterminant * glm::dot(edge2, sCrossEdge1);

    //             if (t < 0.f) {
    //                 break;
    //             }

    //             auto distanceSquared = t * t * glm::dot(ray.direction, ray.direction);
    //             auto cosine = abs(glm::dot(ray.direction, payload.normal) / glm::length(ray.direction));

    //             pdfValue = distanceSquared / (cosine * abs(glm::length(glm::cross(triangle.edges[0], triangle.edges[1])) * 0.5f));
    //         }

    //         float scatterPdf = std::max(0.f, glm::dot(payload.normal, ray.direction) * inversePi);

    //         contribution *= material.albedo * scatterPdf / pdfValue;

    //     } else {
    //         ray.direction = glm::normalize(glm::reflect(ray.direction, payload.normal) + material.fuzziness * Utilities::RandomUnitVectorFast());
            
    //         contribution *= material.albedo;
    //     }
    // }

    // return {light, 1.f};

    // Ray ray;
    // ray.origin = m_Camera->GetPosition();
    // ray.direction = m_Camera->GetRayDirections()[m_Width * i + j];

    // return {ColorRecursive(ray, m_MaxRayDepth), 1.f};
// }

HitPayload Renderer::TraceRay(const Ray &ray) const noexcept {
    Primitive primitive = Primitive::None;
    int objectIndex = -1;
    float closestT = std::numeric_limits<float>::max();

    int objectCount = (int)m_Scene->spheres.size();
    for (int i = 0; i < objectCount; ++i) {
        const Sphere &sphere = m_Scene->spheres[i];

        glm::vec3 centerToOrigin = ray.origin - sphere.center;
        
        float a = glm::dot(ray.direction, ray.direction);
        float k = glm::dot(centerToOrigin, ray.direction);
        float c = glm::dot(centerToOrigin, centerToOrigin) - sphere.radiusSquared;
        float discriminant = k * k - a * c;
        
        if (discriminant < 0.0) {
            continue;
        }

        float t = (-k - glm::sqrt(discriminant)) / a;

        if (t > 0.f && t < closestT) {
            closestT = t;
            primitive = Primitive::Sphere;
            objectIndex = i;
        }
    }

    objectCount = (int)m_Scene->triangles.size();
    for (int i = 0; i < objectCount; ++i) {
        const Triangle &triangle = m_Scene->triangles[i];

        constexpr float epsilon = std::numeric_limits<float>::epsilon();

        glm::vec3 edge1 = triangle.edges[0];
        glm::vec3 edge2 = triangle.edges[1];
        glm::vec3 rayCrossEdge2 = glm::cross(ray.direction, edge2);
        float determinant = glm::dot(edge1, rayCrossEdge2);

        if (glm::abs(determinant) < epsilon) {
            continue;
        }

        float inverseDeterminant = 1.0 / determinant;
        glm::vec3 s = ray.origin - triangle.vertices[0];
        float u = inverseDeterminant * glm::dot(s, rayCrossEdge2);

        if (u < 0.f || u > 1.f) {
            continue;
        }

        glm::vec3 sCrossEdge1 = glm::cross(s, edge1);
        float v = inverseDeterminant * glm::dot(ray.direction, sCrossEdge1);

        if (v < 0.f || u + v > 1.f) {
            continue;
        }

        float t = inverseDeterminant * glm::dot(edge2, sCrossEdge1);

        if (t > 0.f && t < closestT) {
            closestT = t;
            primitive = Primitive::Triangle;
            objectIndex = i;
        }
    }

    if (objectIndex < 0) {
        return Miss(ray);
    }

    return ClosestHit(ray, closestT, primitive, objectIndex);
}

HitPayload Renderer::ClosestHit(const Ray &ray, float t, Primitive primitive, int objectIndex) const noexcept {
    HitPayload payload;
    payload.t = t;
    payload.point = ray.origin + ray.direction * t;

    switch (primitive) {
    case Primitive::Sphere: {
        const Sphere &sphere = m_Scene->spheres[objectIndex];
        payload.normal = (payload.point - sphere.center) * sphere.inverseRadius;
        payload.material = m_Scene->materials[sphere.materialIndex];
        break;       
    }
    case Primitive::Triangle: {
        const Triangle &triangle = m_Scene->triangles[objectIndex];
        payload.normal = triangle.normal;
        if (glm::dot(ray.direction, payload.normal) > 0) {
            payload.normal = -payload.normal;
        }
        payload.normal = glm::normalize(payload.normal);
        payload.material = m_Scene->materials[triangle.materialIndex];
        break;
    }
    default:
        break;
    }
    
    return payload;
}

HitPayload Renderer::Miss(const Ray &ray) const noexcept {
    HitPayload payload;
    payload.t = -1.f;
    return payload;
}