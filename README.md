
# Cube Drop Game

DirectX 12 を用いて制作した3Dゲームです。  

ECSを使った設計で拡張性を確保し、新しいコンポーネントやシステムを追加しやすい構造を意識しました。
リソース管理やメモリ解放のタイミングを明確化し、安全に扱えるよう設計しました。
Update処理をシステムごとに分離し、責務を分割することで可読性と保守性を高めました。
将来的にネットワーク対応や物理エンジンの導入など、機能を追加しても影響範囲を最小限に抑えられるよう意識しました。

実行動画
https://www.youtube.com/watch?v=nRz4UWf_oA4

## 特徴
- DirectX 12 を使用した独自レンダリング
- ECS(Entity Component System)による拡張性の高い設計
- 当たり判定やカメラ制御を自作で実装

## 開発環境
- OS: Windows 10 Home 64bit
- 言語: C++20
- API: DirectX 12, HLSL
- IDE: Visual Studio 2022
- SDK: Windows 10 SDK (10.0.20348.0)

## 操作方法
- `Space` : キューブを落とす
