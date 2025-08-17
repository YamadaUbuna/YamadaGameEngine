#pragma once
#include "include/IScene.h"

// ISystemはシステム処理の共通インターフェースです。
// シーン内での初期化や毎フレームの更新処理を行うための抽象クラスです。
// 物理演算やアニメーションなど、ゲームロジックの実装に利用されます。

class IScene;

class ISystem {
public:
    virtual ~ISystem() = default;

    // ロジック処理（物理演算、アニメーションなど）
    virtual void Initialize(IScene& scene) = 0;
    virtual void Update(IScene& scene, float deltaTime) = 0;
};
