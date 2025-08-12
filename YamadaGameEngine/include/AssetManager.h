#pragma once
#include <unordered_map>
#include "include/ModelData.h"

//・ModelManager（あるいはAssetManager）
//モデル（ModelComponentや複数メッシュのセット）を読み込み・管理・キャッシュするクラス
//ファイル読み込みはここが一括で行い、IDや名前でモデルを保持
//すでに読み込まれている場合は再読み込みせず、既存のモデルを返す

class ModelComponent; // 前方宣言

class AssetManager {
public:
    // モデルを読み込んで登録
    bool LoadModel(const std::string& id, const std::wstring& filepath);

    // 登録済みモデルをIDから取得（読み取り専用）
    const ModelData* GetModel(const std::string& id) const;

private:
    // ID -> ModelData のマップ（ModelDataはメッシュなどを持つ実体クラス）
    std::unordered_map<std::string, std::unique_ptr<ModelData>> m_models;
    void InitializeMeshBuffers(MeshComponent* meshComponent);
};


