#pragma once
#include "include/IComponent.h"

// �����L���[�u�̏�ԊǗ��R���|�[�l���g�B
// ���������ǂ����̃t���O�Ɨ������x��ێ�����B

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