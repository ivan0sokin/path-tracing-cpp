#ifndef _MODEL_INSTANCE_H
#define _MODEL_INSTANCE_H

#include "../acceleration/BLAS.h"

class ModelInstance {
public:
    ModelInstance(int modelIndex, const BVH *bvh) noexcept;

    ~ModelInstance() noexcept;

    inline void Update() const noexcept {
        Math::Matrix4f translationMatrix = Math::TranslationMatrix(m_Translation);
        Math::Vector3f m_AnglesInRadians(Math::ToRadians(m_Angles.x), Math::ToRadians(m_Angles.y), Math::ToRadians(m_Angles.z));
        Math::Matrix4f rotationMatrix = Math::RotationMatrix(m_AnglesInRadians);
        m_BLAS->SetTransform(translationMatrix * rotationMatrix);
    }

    inline ModelInstance* Clone() const noexcept {
        return new ModelInstance(m_ModelIndex, m_BLAS->GetBVH());
    }

    Math::Vector3f& Translation() noexcept {
        return m_Translation;
    }

    Math::Vector3f& Angles() noexcept {
        return m_Angles;
    }

    constexpr BLAS* const GetBLAS() const noexcept {
        return m_BLAS;
    }

private:
    int m_ModelIndex;
    BLAS *m_BLAS;

    Math::Vector3f m_Translation;
    Math::Vector3f m_Angles;
};

#endif