#pragma once

// IComponentはすべてのコンポーネントの基底インターフェースです。
// 仮想デストラクタのみを持ち、ポリモーフィズムによる操作を可能にします。
// 継承先で具体的な機能やデータを定義するための土台となります。

class IComponent {
public:
    virtual ~IComponent() = default;
};