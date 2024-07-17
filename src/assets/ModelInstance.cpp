#include "ModelInstance.h"
#include "AssetLoader.h"

ModelInstance::ModelInstance(const BVH *bvh, std::function<void()> onCreate, std::function<void()> onDestroy) noexcept :
    m_BLAS(new BLAS(bvh)), m_OnCreate(onCreate), m_OnDestroy(onDestroy) {
    m_OnCreate();
}

ModelInstance::~ModelInstance() noexcept {
    m_OnDestroy();
}