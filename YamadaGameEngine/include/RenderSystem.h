#pragma once
#include "include/ISystem.h"
#include "include/MeshComponent.h"
#include "include/MaterialComponent.h"
#include "include/TransformComponent.h"
#include "include/ModelComponent.h"
#include "include/CameraComponent.h"

#include <DirectXMath.h>

// RenderSystem��ISystem���������A�V�[�����̃����_�����O������S�����܂��B
// �G���e�B�e�B�̃��f���A�}�e���A���A�g�����X�t�H�[���R���|�[�l���g���擾���A
// �����̒萔�o�b�t�@���쐬�E�X�V����������Renderer�ɕ`����˗����܂��B
// Direct3D12�p�̒萔�o�b�t�@�쐬�ƍX�V�̃w���p�[�֐����܂݂܂��B

class RenderSystem :
    public ISystem
{
public:
    void Initialize(IScene& scene) override;
    void Update(IScene& scene, float deltaTime) override;

private:
    HRESULT CreateAndUpdateCB(Microsoft::WRL::ComPtr<ID3D12Resource>& cb, const void* data, size_t size);
    HRESULT UpdateTransformCBs(TransformComponent* transform, CameraComponent* camera);
    HRESULT UpdateMaterialCB(MaterialComponent* material);
};

