#include "Renderer.h"
#include "Utilities.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb-master/stb_image_write.h"

#include <vector>
#include <numeric>
#include <execution>
#include <thread>
#include <cstring>

Renderer::Renderer(int width, int height) noexcept :
    m_Width(width), m_Height(height) {
    m_ImageData = new uint32_t[m_Width * m_Height];
    m_Image = new Image(m_Width, m_Height, m_ImageData);
    m_AccumulationData = new Math::Vector4f[m_Width * m_Height];

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
        m_AccumulationData = new Math::Vector4f[m_Width * m_Height];
    }
}

void Renderer::Render(const Camera &camera, const Scene &scene) noexcept {
    m_Camera = &camera;
    m_Scene = &scene;

    if (!m_Accumulate) {
        m_FrameIndex = 1;
    }

    if (m_FrameIndex == 1) {
        memset(m_AccumulationData, 0, m_Width * m_Height * sizeof(Math::Vector4f));
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

                    Math::Vector4f color = m_AccumulationData[m_Width * t + j];
                    if (color.r != color.r) color.r = 0.f;
                    if (color.g != color.g) color.g = 0.f;
                    if (color.b != color.b) color.b = 0.f;

                    color *= inverseFrameIndex;
                    color = Utilities::CorrectGamma(color, inverseGamma);
                    color = Math::Clamp(color, 0.f, 1.f);

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

Math::Vector4f Renderer::PixelProgram(int i, int j) const noexcept {
    Ray ray;
    ray.origin = m_Camera->GetPosition();
    ray.direction = m_Camera->GetRayDirections()[m_Width * i + j];

    Math::Vector3f light(0.f), energy(1.f);
    for (int i = 0; i < m_MaxRayDepth; ++i) {
        HitPayload payload = TraceRay(ray);
        if (payload.t < 0.f) {
            light += energy * m_OnRayMiss(ray);
            break;
        }
        
        const Material &material = payload.material;

        float diffuseRatio = 0.5 * (1.0 - material.metallic);
        float specularRatio = 1 - diffuseRatio;

        Math::Vector3f V = Math::Normalize(-ray.direction);

        Math::Vector3f reflectionDirection;
        if (Utilities::RandomFloatInZeroToOne() < diffuseRatio) {
            reflectionDirection = Utilities::RandomInHemisphere(payload.normal);
        } else {
            Math::Vector3f halfVec;
            {
                Math::Vector2f Xi{Utilities::RandomFloatInZeroToOne(), Utilities::RandomFloatInZeroToOne()};
                Math::Vector3f N = payload.normal;

                float a = material.roughness * material.roughness;

                float phi = Math::Constants::Tau<float> * Xi.x;
                float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a * a - 1.0) * Xi.y));
                float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

                // from spherical coordinates to cartesian coordinates
                Math::Vector3f H;
                H.x = cos(phi) * sinTheta;
                H.y = sin(phi) * sinTheta;
                H.z = cosTheta;

                // from tangent-space vector to world-space sample vector
                Math::Vector3f up = Math::Abs(N.z) < 0.999f ? Math::Vector3f(0.0, 0.0, 1.0) : Math::Vector3f(1.0, 0.0, 0.0);
                Math::Vector3f tangent = Math::Normalize(Math::Cross(up, N));
                Math::Vector3f bitangent = Math::Cross(N, tangent);

                halfVec = tangent * H.x + bitangent * H.y + N * H.z;
                halfVec = Math::Normalize(halfVec);
            }

            reflectionDirection = 2.f * Math::Dot(V, halfVec) * halfVec - V;
        }

        auto DistributionGGX = [](const Math::Vector3f &N, const Math::Vector3f &H, float roughness) {
            float a = roughness * roughness;
            float a2 = a * a;
            float NdotH = Math::Max(Math::Dot(N, H), 0.f);
            float NdotH2 = NdotH * NdotH;

            float nom = a2;
            float denom = (NdotH2 * (a2 - 1.0) + 1.0);
            denom = Math::Constants::Pi<float> * denom * denom;

            return nom / denom;
        };

        auto GeometrySchlickGGX = [](float NdotV, float roughness) {
            float r = (roughness + 1.0);
            float k = (r * r) / 8.0;

            float nom = NdotV;
            float denom = NdotV * (1.0 - k) + k;

            return nom / denom;
        };

        auto GeometrySmith = [&](const Math::Vector3f &N, const Math::Vector3f &V, const Math::Vector3f &L, float roughness) {
            float NdotV = Math::Abs(Math::Dot(N, V));
            float NdotL = Math::Abs(Math::Dot(N, L));
            float ggx2 = GeometrySchlickGGX(NdotV, roughness);
            float ggx1 = GeometrySchlickGGX(NdotL, roughness);

            return ggx1 * ggx2;
        };

        auto FresnelSchlick = [](float cosTheta, const Math::Vector3f &F0) {
            return F0 + (1.f - F0) * pow(1.f - cosTheta, 5.f);
        };

        auto SpecularBRDF = [](float D, float G, const Math::Vector3f &F, const Math::Vector3f &V, const Math::Vector3f &L, const Math::Vector3f &N) {        
            float NdotL = Math::Abs(Math::Dot(N, L));
            float NdotV = Math::Abs(Math::Dot(N, V));
                    
            //specualr
            //Microfacet specular = D * G * F / (4 * NoL * NoV)
            Math::Vector3f nominator = D * G * F;
            float denominator = 4.0 * NdotV * NdotL + 0.000001f;
            Math::Vector3f specularBrdf = nominator / denominator;
            
            return specularBrdf;
        };
        

        auto DiffuseBRDF = [](const Math::Vector3f &albedo) {
            return albedo * Math::Constants::InversePi<float>;
        };

        auto ImportanceSampleGGX_PDF = [](float NDF, float NdotH, float VdotH) {
            //ImportanceSampleGGX pdf
                //pdf = D * NoH / (4 * VoH)
            return NDF * NdotH / (4 * VdotH);
        };

        auto CosinSamplingPDF = [](float NdotL) {
            return NdotL * Math::Constants::InversePi<float>;
        };

        Math::Vector3f L = Math::Normalize(reflectionDirection);
        Math::Vector3f H = Math::Normalize(V + L);

        float NdotL = Math::Abs(Math::Dot(payload.normal, L));
        float NdotH = Math::Abs(Math::Dot(payload.normal, H));
        float VdotH = Math::Abs(Math::Dot(V, H));
        
        float NdotV = Math::Abs(Math::Dot(payload.normal, V));
        
        Math::Vector3f F0 = Math::Vector3f(0.08, 0.08, 0.08);
        F0 = Math::Lerp(F0 * material.specular, material.albedo, material.metallic);

        float NDF = DistributionGGX(payload.normal, H, material.roughness);
        float G = GeometrySmith(payload.normal, V, L, material.roughness);
        Math::Vector3f F = FresnelSchlick(Math::Max(Math::Dot(H, V), 0.f), F0);

        Math::Vector3f kS = F;
        Math::Vector3f kD = 1.f - kS;
        kD *= 1.0 - material.metallic;

        Math::Vector3f specularBrdf = SpecularBRDF(NDF, G, F, V, L, payload.normal);
            
        //hemisphere sampling pdf
        //pdf = 1 / (2 * PI)
        //float speccualrPdf = 1 / (2 * PI);
        
        //ImportanceSampleGGX pdf
        //pdf = D * NoH / (4 * VoH)
        float speccualrPdf = ImportanceSampleGGX_PDF(NDF, NdotH, VdotH);
        
        //diffuse
        //Lambert diffuse = diffuse / PI
        Math::Vector3f diffuseBrdf = DiffuseBRDF(material.albedo);
        //cosin sample pdf = N dot L / PI
        float diffusePdf = CosinSamplingPDF(NdotL);

        Math::Vector3f totalBrdf = (diffuseBrdf * kD + specularBrdf) * NdotL;
        float totalPdf = diffuseRatio * diffusePdf + specularRatio * speccualrPdf;
            
        ray.origin = payload.point + payload.normal * 0.000001f;
        ray.direction = reflectionDirection;
        
        light += material.GetEmission() * energy;
        
        if (totalPdf > 0.0) {
            energy *= totalBrdf / totalPdf;
        }
    }

    return {light.x, light.y, light.z, 1.f};
}

HitPayload Renderer::TraceRay(const Ray &ray) const noexcept {
    Primitive primitive = Primitive::None;
    int objectIndex = -1;
    float closestT = std::numeric_limits<float>::max();

    int objectCount = (int)m_Scene->spheres.size();
    for (int i = 0; i < objectCount; ++i) {
        const Sphere &sphere = m_Scene->spheres[i];

        Math::Vector3f centerToOrigin = ray.origin - sphere.center;
        
        float a = Math::Dot(ray.direction, ray.direction);
        float k = Math::Dot(centerToOrigin, ray.direction);
        float c = Math::Dot(centerToOrigin, centerToOrigin) - sphere.radiusSquared;
        float discriminant = k * k - a * c;
        
        if (discriminant < 0.0) {
            continue;
        }

        float t = (-k - Math::Sqrt(discriminant)) / a;

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

        Math::Vector3f edge1 = triangle.edges[0];
        Math::Vector3f edge2 = triangle.edges[1];
        Math::Vector3f rayCrossEdge2 = Math::Cross(ray.direction, edge2);
        float determinant = Math::Dot(edge1, rayCrossEdge2);

        if (Math::Abs(determinant) < epsilon) {
            continue;
        }

        float inverseDeterminant = 1.0 / determinant;
        Math::Vector3f s = ray.origin - triangle.vertices[0];
        float u = inverseDeterminant * Math::Dot(s, rayCrossEdge2);

        if (u < 0.f || u > 1.f) {
            continue;
        }

        Math::Vector3f sCrossEdge1 = Math::Cross(s, edge1);
        float v = inverseDeterminant * Math::Dot(ray.direction, sCrossEdge1);

        if (v < 0.f || u + v > 1.f) {
            continue;
        }

        float t = inverseDeterminant * Math::Dot(edge2, sCrossEdge1);

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
        if (Math::Dot(ray.direction, payload.normal) > 0) {
            payload.normal = -payload.normal;
        }
        payload.normal = Math::Normalize(payload.normal);
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