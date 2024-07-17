#ifndef _MODEL_INSTANCE_H
#define _MODEL_INSTANCE_H

#include "../acceleration/BLAS.h"

class ModelInstance {
public:
    //! Creates model instance from Model, should not be used except by AssetLoader
    ModelInstance(const BVH *bvh, std::function<void()> onCreate, std::function<void()> onDestroy) noexcept;

    ~ModelInstance() noexcept;

    //! Updates transformation matrix of the instance
    constexpr void UpdateTransform() const noexcept {
        Math::Matrix4f translationMatrix = Math::TranslationMatrix(m_Translation);
        Math::Vector3f m_AnglesInRadians(Math::ToRadians(m_AnglesInDegrees.x), Math::ToRadians(m_AnglesInDegrees.y), Math::ToRadians(m_AnglesInDegrees.z));
        Math::Matrix4f rotationMatrix = Math::RotationMatrix(m_AnglesInRadians);
        m_BLAS->SetTransform(translationMatrix * rotationMatrix);
    }

    //! Clones the instance. Returns pointer to ModelInstance of the cloned one
    inline ModelInstance* Clone() const noexcept {
        return new ModelInstance(m_BLAS->GetBVH(), m_OnCreate, m_OnDestroy);
    }

    //! Returns reference to translation Vector3f. GUI convenience
    Math::Vector3f& Translation() noexcept {
        return m_Translation;
    }

    //! Returns reference to angles in degrees Vector3f. GUI convenience 
    Math::Vector3f& Angles() noexcept {
        return m_AnglesInDegrees;
    }

    //! Returns BLAS of the instance
    constexpr BLAS* const GetBLAS() const noexcept {
        return m_BLAS;
    }

private:
    int m_ModelIndex;
    BLAS *m_BLAS;
    std::function<void()> m_OnCreate;
    std::function<void()> m_OnDestroy;

    Math::Vector3f m_Translation;
    Math::Vector3f m_AnglesInDegrees;
};

#endif