#pragma once
#include "include/EntityBase.h"
#include "CameraComponent.h"
#include "TransformComponent.h"

// カメラ用エンティティクラス。
// CameraComponentとTransformComponentを持ち、初期位置と向きを設定する。

class CameraEntity : public EntityBase {
public:

    CameraEntity() {
		AddComponent(std::make_unique<CameraComponent>());
		AddComponent(std::make_unique<TransformComponent>());

		TransformComponent* CameraTransform = this->GetComponent<TransformComponent>();
		CameraTransform->SetPosition({ 0.0f, 3.0f, -6.0f });
		CameraTransform->SetRotation({ 0.0f, 0.0f, 0.0f });
		DirectX::XMFLOAT3 target = { 0.0f, 0.0f, 0.0f }; // 本来見たい場所
		DirectX::XMFLOAT3 up = { 0.0f, 1.0f, 0.0f }; // ワールドの上方向

		// 少し下を向くようにターゲットを下にずらす
		target.y -= 90.0f; 

		CameraTransform->LookAt(target, up);

    }
	~CameraEntity() override = default; 
};