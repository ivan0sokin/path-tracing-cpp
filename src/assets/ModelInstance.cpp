#include "ModelInstance.h"
#include "AssetLoader.h"

ModelInstance::ModelInstance(int modelIndex, const BVH *bvh) noexcept :
    m_ModelIndex(modelIndex), m_BLAS(new BLAS(bvh)) {
    AssetLoader::Instance().IncreaseInstanceCount(m_ModelIndex);
}

ModelInstance::~ModelInstance() noexcept {
    AssetLoader::Instance().DecreaseInstanceCount(m_ModelIndex);
}