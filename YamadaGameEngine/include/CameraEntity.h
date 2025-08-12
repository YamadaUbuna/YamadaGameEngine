#pragma once
#include "include/EntityBase.h"
#include "CameraComponent.h"
#include "TransformComponent.h"

class CameraEntity : public EntityBase {
    // EntityBaseの実装をそのまま使うのでGetComponent/AddComponentは不要
    // ここにexampleEntity固有の処理があれば追加
public:

    CameraEntity() {
		AddComponent(std::make_unique<CameraComponent>());
		AddComponent(std::make_unique<TransformComponent>());

		//この下のやつも、専用のSystemを作ってやるべきなので、あとでそうしてくださいね
		TransformComponent* CameraTransform = this->GetComponent<TransformComponent>();
		CameraTransform->SetPosition({ 0.0f, 0.0f, -5.0f });
		CameraTransform->SetRotation({ 0.0f, 0.0f, 0.0f });
    }
	~CameraEntity() override = default; // デストラクタは必要ならオーバーライド
};