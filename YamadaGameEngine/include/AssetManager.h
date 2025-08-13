#pragma once
#include <unordered_map>
#include "include/ModelData.h"
#include <fbxsdk.h>

//�EModelManager�i���邢��AssetManager�j
//���f���iModelComponent�╡�����b�V���̃Z�b�g�j��ǂݍ��݁E�Ǘ��E�L���b�V������N���X
//�t�@�C���ǂݍ��݂͂������ꊇ�ōs���AID�▼�O�Ń��f����ێ�
//���łɓǂݍ��܂�Ă���ꍇ�͍ēǂݍ��݂����A�����̃��f����Ԃ�

class ModelComponent; // �O���錾

class AssetManager {
public:
    // ���f����ǂݍ���œo�^
    bool LoadModel(const std::string& id, const std::wstring& filepath);

    // �o�^�ς݃��f����ID����擾�i�ǂݎ���p�j
    const ModelData* GetModel(const std::string& id) const;

private:
    // ID -> ModelData �̃}�b�v�iModelData�̓��b�V���Ȃǂ������̃N���X�j
    std::unordered_map<std::string, std::unique_ptr<ModelData>> m_models;
    void UploadMeshToGPU(MeshComponent* meshComponent);
    
};

class FbxLoader {
    public:
    // FBX�t�@�C����ǂݍ����ModelData�𐶐�
		bool LoadModel(const std::wstring& filepath, ModelData& outModel);
        void ParseNode(FbxNode* node, ModelData& outModel);
        void ParseMesh(FbxMesh* mesh, ModelData& outModel);
        //std::string GetTexturePath(FbxSurfaceMaterial* material, const char* propName);

private:
   
};

