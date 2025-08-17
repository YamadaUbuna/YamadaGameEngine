#pragma once
#include "include/IComponent.h"

// 落下キューブの状態管理コンポーネント。
// 落下中かどうかのフラグと落下速度を保持する。

class DropCubeComponent :public IComponent {
public:
	DropCubeComponent() = default;
	~DropCubeComponent() = default;

	const bool GetDroping() const { return droping; }
	void SetDroping(bool drop) { droping = drop; }

	const float GetDropSpeed() const { return speed; }
	void SetDropSpeed(float dSpeed) { speed = dSpeed; }

private:
	bool droping = false;
	float speed = -3.0f;
};