#pragma once
#include "include/IComponent.h"
#include "include/d3dx12.h"

#include <vector>
#include <unordered_map>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;
using namespace DirectX;

// MeshComponentは、頂点・インデックスデータおよび対応するGPUリソースを管理するコンポーネントです。
// 頂点構造体FbxVertexを用い、位置・法線・UV・色を保持します。
// DirectX 12のバッファとビュー情報を扱い、描画に必要な情報を提供します。
// マテリアルIDとの関連付けにも対応しています。


struct FbxVertex
{
    XMFLOAT3 position;
    XMFLOAT3 normal;
    XMFLOAT2 uv;
    XMFLOAT4 color;
};

class MeshComponent : public IComponent
{
public:
    MeshComponent() = default;
    ~MeshComponent() = default;

    void SetVertices(const std::vector<FbxVertex>& verts) { vertices = verts; }
    void SetIndices(const std::vector<uint32_t>& inds) { indices = inds; }

    const std::vector<FbxVertex>& GetVertices() const { return vertices; }
    const std::vector<uint32_t>& GetIndices() const { return indices; }

    // ゲッター
    ID3D12Resource* GetVertexBuffer() const {
        return vertexBuffer.Get();
    }

    ID3D12Resource* GetIndexBuffer() const {
        return indexBuffer.Get();
    }

    const D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView() const {
        return vertexBufferView;
    }

    const D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() const {
        return indexBufferView;
    }

    // セッター
    void SetVertexBuffer(const Microsoft::WRL::ComPtr<ID3D12Resource>& buffer) {
        vertexBuffer = buffer;
    }

    void SetIndexBuffer(const Microsoft::WRL::ComPtr<ID3D12Resource>& buffer) {
        indexBuffer = buffer;
    }

    void SetVertexBufferView(const D3D12_VERTEX_BUFFER_VIEW& view) {
        vertexBufferView = view;
    }

    void SetIndexBufferView(const D3D12_INDEX_BUFFER_VIEW& view) {
        indexBufferView = view;
    }

    void SetMaterialId(const std::string& id) { materialId = id; }
    const std::string& GetMaterialId() const { return materialId; }

private:
    std::vector<FbxVertex> vertices;
    std::vector<uint32_t> indices;

    Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer;
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
    D3D12_INDEX_BUFFER_VIEW indexBufferView;

    std::string materialId;
};
