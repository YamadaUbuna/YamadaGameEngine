#pragma once

#include <fbxsdk.h>
#include <vector>
#include <unordered_map>
#include <wrl/client.h>

#include "include/MeshComponent.h"
#include <DirectXMath.h>

using Microsoft::WRL::ComPtr;
using namespace DirectX;

enum class FBX_TEXTURE_TYPE
{
	FBX_DIFFUSE,
	FBX_NORMAL,
	FBX_SPECUAR,
	FBX_FALLOFF,
	FBX_REFLECTIONMAP,
	FBX_UNKNOWN
};

class MeshContainer
{
protected:

	FbxMesh* m_mesh;
	FBXDataContainer* m_animeFbxCont;

	bool m_uniqueMesh = false;

	const char* m_meshNodeName;
	int	m_parentNodeId;

public:
	std::wstring m_MaterialId = L"";
	std::wstring m_MeshId = L"";

	UINT	m_vertexCount = 0;

	std::vector<FbxVertex>	m_vertexData;
	std::vector<ULONG>		m_indexData;

	//頂点データの最大値と最小値　これは判定作成時に使う
	XMFLOAT3	m_vtxMin;
	XMFLOAT3	m_vtxMax;

	~MeshContainer();

	void SetFbxMesh(FbxMesh* mesh);

	const char* GetMeshNodeName()
	{
		return m_meshNodeName;
	}

	FbxMesh* GetFbxMesh()
	{
		return m_mesh;
	}

	void SetUniqueFlag(bool mesh)
	{
		m_uniqueMesh = mesh;
	}

};


class FBXDataContainer
{
private:
	std::vector<const char*> m_nodeNameList;
	std::vector<std::unique_ptr<MeshContainer>> m_pMeshContainer;
	std::unordered_map<std::wstring, std::unique_ptr<MaterialContainer>> m_pMaterialContainer;

	//頂点データの最大値と最小値　このオブジェクトに読み込んだ全メッシュの中での総合値
	XMFLOAT3	m_vtxTotalMin;
	XMFLOAT3	m_vtxTotalMax;

	FBX_TEXTURE_TYPE GetTextureType(const fbxsdk::FbxBindingTableEntry& entryTable);

	HRESULT ReadFbxToMeshContainer(const std::wstring id, FbxMesh* pMesh);
	HRESULT LoadMaterial(const std::wstring id, FbxSurfaceMaterial* material);

	HRESULT LoadTextureFromMaterial(const std::wstring matName, const std::wstring id, FBX_TEXTURE_TYPE texType, const FbxProperty* fbxProp);


};

