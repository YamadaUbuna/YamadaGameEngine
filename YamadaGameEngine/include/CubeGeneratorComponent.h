#pragma once

#include "include/IComponent.h"

// キューブ生成管理コンポーネント。
// 一定時間ごとに確率でキューブ生成判定を行う。
// 生成間隔（SpawnInterval）と出現確率（Appearance）を設定可能。
// 連続非生成を防ぐロジックを持つ。

class CubeGeneratorComponent : public IComponent {
public:

    CubeGeneratorComponent() = default;
    ~CubeGeneratorComponent() = default;

    void UpdateTimer(float deltaTime) { m_timer += deltaTime; }

    bool ShouldSpawn() {
        if (m_timer >= m_spawnInterval) {
            m_timer -= m_spawnInterval;

            // appearance に基づく確率判定
            bool spawn = (static_cast<float>(rand()) / RAND_MAX) <= m_appearance;

            // 連続で生成なしを防止
            if (!spawn && !m_lastSpawned) spawn = true;

            m_lastSpawned = spawn; // 今回の結果を保存
            return spawn;
        }
        return false;
    }

    void SetSpawn(float spawn) { m_spawnInterval = spawn; }
    const float GetSpawn() const{ return m_spawnInterval; }
    
    void SetAppearance(float appearance) { m_appearance = appearance; }
    const float GetAppearance() const { return m_appearance; }

private:
    float m_spawnInterval = 1.0f;
    float m_timer;
    float m_appearance = 0.6f;
    bool m_lastSpawned;     // 前回生成フラグ
};
