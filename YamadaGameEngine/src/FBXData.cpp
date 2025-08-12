#include "include\pch.h"
#include "include/FBXData.h"
#include <include/Renderer.h>
#include <include/PipelineManager.h>
#include <include/MeshComponent.h>
using namespace std;

MeshContainer::~MeshContainer()
{
	m_vertexData.clear();
	m_indexData.clear();

	//ToDo: SkinAnime05
	//スキンアニメ用データ削除
	//m_skinCount = 0; //と言ってもデストラクタ内なので処理的な意味はない。
	//ToDo: ここまで

	//if (m_uniqueMesh)
	//{
	//	auto meshMng = MyAccessHub::getMyGameEngine()->GetMeshManager();
	//	meshMng->removeVertexBuffer(m_MeshId, true);
	//}
}

void MeshContainer::SetFbxMesh(FbxMesh* mesh)
{
	m_mesh = mesh;
	m_meshNodeName = m_mesh->GetNode(0)->GetName();
}

HRESULT FBXDataContainer::ReadFbxToMeshContainer(const std::wstring id, FbxMesh* pMesh)
{
	HRESULT hr = S_OK;

	unique_ptr<MeshContainer> meshCont = make_unique<MeshContainer>();

	//ノード数
	int nodecount = pMesh->GetNodeCount();

	//MeshContainerに元データポインタ保存
	meshCont->SetFbxMesh(pMesh);

	// VertexBuffer
	// これが頂点データ本体だけども、実はボーン点も含むので「コントロールポイント」
	FbxVector4* controllPoints = nullptr;

	// IndexBuffer
	// インデックスデータ
	int* indices = nullptr;

	// 頂点数
	int vertexCount;
	// コントロールポイント数
	int contCount;

	//ToDo 08:メッシュ基本データ読み込み 
	// VertexBuffer
	controllPoints = pMesh->GetControlPoints();
	// IndexBuffer
	// メソッド名がわかり辛い。コントロールポイントの中からポリゴン頂点のインデックスリスト
	indices = pMesh->GetPolygonVertices();
	// 頂点数
	vertexCount = pMesh->GetPolygonVertexCount();
	// コントロールポイント数 これを使うのはスキンアニメの時
	contCount = pMesh->GetControlPointsCount();
	//ToDo 08: ここまで

	meshCont->m_vertexData.clear();
	meshCont->m_vertexData.resize(vertexCount);

	//メッシュごとの当たり判定エリア値初期化
	meshCont->m_vtxMin.x = FLT_MAX;
	meshCont->m_vtxMin.y = FLT_MAX;
	meshCont->m_vtxMin.z = FLT_MAX;

	meshCont->m_vtxMax.x = FLT_MIN;
	meshCont->m_vtxMax.y = FLT_MIN;
	meshCont->m_vtxMax.z = FLT_MIN;

	for (int i = 0; i < vertexCount; i++)
	{
		// Vertexデータを初期化
		ZeroMemory(&meshCont->m_vertexData[i], sizeof(FbxVertex));

		//ToDo 09:頂点データをFBXから自前の頂点バッファへ 
		// インデックスバッファから頂点番号を取得
		int index = indices[i];
		// 頂点座標リストから座標を取得する
		meshCont->m_vertexData[i].position.x = (float)controllPoints[index][0];
		meshCont->m_vertexData[i].position.y = (float)controllPoints[index][1];
		meshCont->m_vertexData[i].position.z = (float)controllPoints[index][2];
		//ToDo 9:ここまで

		//max & min check
		if (meshCont->m_vertexData[i].position.x < meshCont->m_vtxMin.x)
		{
			meshCont->m_vtxMin.x = meshCont->m_vertexData[i].position.x;
		}
		else if (meshCont->m_vertexData[i].position.x > meshCont->m_vtxMax.x)
		{
			meshCont->m_vtxMax.x = meshCont->m_vertexData[i].position.x;
		}

		if (meshCont->m_vertexData[i].position.y < meshCont->m_vtxMin.y)
		{
			meshCont->m_vtxMin.y = meshCont->m_vertexData[i].position.y;
		}
		else if (meshCont->m_vertexData[i].position.y > meshCont->m_vtxMax.y)
		{
			meshCont->m_vtxMax.y = meshCont->m_vertexData[i].position.y;
		}

		if (meshCont->m_vertexData[i].position.z < meshCont->m_vtxMin.z)
		{
			meshCont->m_vtxMin.z = meshCont->m_vertexData[i].position.z;
		}
		else if (meshCont->m_vertexData[i].position.z > meshCont->m_vtxMax.z)
		{
			meshCont->m_vtxMax.z = meshCont->m_vertexData[i].position.z;
		}
	}

	//保存したminとmaxを全体のmin、maxと比較して更新
	if (m_vtxTotalMax.x < meshCont->m_vtxMax.x) m_vtxTotalMax.x = meshCont->m_vtxMax.x;
	if (m_vtxTotalMax.y < meshCont->m_vtxMax.y) m_vtxTotalMax.y = meshCont->m_vtxMax.y;
	if (m_vtxTotalMax.z < meshCont->m_vtxMax.z) m_vtxTotalMax.z = meshCont->m_vtxMax.z;

	if (m_vtxTotalMin.x > meshCont->m_vtxMin.x) m_vtxTotalMin.x = meshCont->m_vtxMin.x;
	if (m_vtxTotalMin.y > meshCont->m_vtxMin.y) m_vtxTotalMin.y = meshCont->m_vtxMin.y;
	if (m_vtxTotalMin.z > meshCont->m_vtxMin.z) m_vtxTotalMin.z = meshCont->m_vtxMin.z;

	FbxStringList uvset_names;
	// UVSetの名前リストを取得
	// マルチテクスチャの場合、UVも複数あり、それぞれに名前がついている。
	pMesh->GetUVSetNames(uvset_names);

	FbxArray<FbxVector2> uv_buffer;

	// UVSetの名前からUVSetを取得する
	// 今回はマルチテクスチャには対応しないので最初の名前だけ
	// マルチテクスチャを使うメッシュにはInputLayoutもシェーダも変わるから注意
	pMesh->GetPolygonVertexUVs(uvset_names.GetStringAt(0), uv_buffer);

	for (int i = 0; i < uv_buffer.Size(); i++)
	{
		FbxVector2& uv = uv_buffer[i];

		//ToDo 10:UVデータを自前頂点データに反映 
		// 基本的にバッファの値をそのまま使用するんだけども・・・
		meshCont->m_vertexData[i].uv.x = (float)uv[0];
		meshCont->m_vertexData[i].uv.y = (float)(1.0 - uv[1]); //FBXのVはひっくり返っているので1.0
		//ToDo 10: ここまで
	}


	//indexの作成
	meshCont->m_indexData.clear();
	meshCont->m_indexData.resize(vertexCount);

	//ポリゴン数を取得
	int length = pMesh->GetPolygonCount();

	//ポリゴン数でループ
	for (int i = 0; i < length; i++)
	{
		//ToDo 11:インデックスデータの作成 
		int baseIndex = i * 3; //ポリゴンカウント＊３がインデックスの先頭
		//データが右手系でひっくり返っている場合は、ここでインデックスの一つ目と三つ目の値ひっくり返せば良い
		//けど、今回はひっくり返さない。アニメとセットで変換メソッドでひっくり返す。それは後の講義で。
		meshCont->m_indexData[baseIndex] = static_cast<ULONG>(baseIndex);
		meshCont->m_indexData[baseIndex + 1] = static_cast<ULONG>(baseIndex + 1);
		meshCont->m_indexData[baseIndex + 2] = static_cast<ULONG>(baseIndex + 2);
		//ToDo 11:ここまで 
	}

	//作成したインデックスデータでシェーダリソースを作成
	//MyAccessHub::getMyGameEngine()->GetMeshManager()->AddIndexBuffer(id, meshCont->m_indexData.data(), sizeof(ULONG), vertexCount);

	auto vbResource = Renderer::GetInstance().CreateDefaultBuffer(
		meshCont->m_vertexData.data(),
		sizeof(FbxVertex) * vertexCount,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
	);
	meshComponent->SetVertexBuffer(vbResource);


	//法線も重要なんだけども、なんだか同じような流れなのでToDoは省略
	//法線
	FbxArray<FbxVector4> normals;
	// 法線リストの取得
	pMesh->GetPolygonVertexNormals(normals);

	//法線登録
	for (int i = 0; i < normals.Size(); i++)
	{
		meshCont->m_vertexData[i].normal.x = normals[i][0];
		meshCont->m_vertexData[i].normal.y = normals[i][1];
		meshCont->m_vertexData[i].normal.z = normals[i][2];
	}

	//頂点カラー
	{

		// 頂点カラーデータの数を確認
		int color_count = pMesh->GetElementVertexColorCount();

		// 頂点カラーデータの取得
		// 本当はここでcolor_countをループで回して・・・と言う流れが発生する。
		// 今回は簡略化の為Diffuseだけで行うので最初のカラーだけ取得
		FbxGeometryElementVertexColor* vertex_colors = pMesh->GetElementVertexColor(0);

		if (color_count == 0 || vertex_colors == nullptr)
		{
			//この頂点データには色設定がないので全て白で補正
			for (int i = 0; i < vertexCount; i++)
			{
				meshCont->m_vertexData[i].color.x = 1.0f;
				meshCont->m_vertexData[i].color.y = 1.0f;
				meshCont->m_vertexData[i].color.z = 1.0f;
				meshCont->m_vertexData[i].color.w = 1.0f;
			}
		}
		else
		{
			FbxLayerElement::EMappingMode mapping_mode = vertex_colors->GetMappingMode();
			FbxLayerElement::EReferenceMode reference_mode = vertex_colors->GetReferenceMode();

			//頂点カラーはカラー設定モードで値の取り方が変わる。ゲームは基本直接入力値だと経験的には思う。
			if (mapping_mode == FbxLayerElement::eByPolygonVertex)
			{
				if (reference_mode == FbxLayerElement::eIndexToDirect)
				{
					// 頂点カラーバッファ取得
					FbxLayerElementArrayTemplate<FbxColor>& colors = vertex_colors->GetDirectArray();
					// 頂点カラーインデックスバッファ取得
					FbxLayerElementArrayTemplate<int>& indeces = vertex_colors->GetIndexArray();
					for (int i = 0; i < indeces.GetCount(); i++)
					{
						int id = indeces.GetAt(i);
						FbxColor color = colors.GetAt(id);
						meshCont->m_vertexData[i].color.x = (float)color.mAlpha;
						meshCont->m_vertexData[i].color.y = (float)color.mRed;
						meshCont->m_vertexData[i].color.z = (float)color.mGreen;
						meshCont->m_vertexData[i].color.w = (float)color.mBlue;
					}
				}
			}
		}

	}

	meshCont->m_MeshId = id;

	FbxLayerElementMaterial* elMat = pMesh->GetElementMaterial(0);
	int matIndex = elMat->GetIndexArray().GetAt(0);
	FbxSurfaceMaterial* srfMat = pMesh->GetNode()->GetSrcObject<FbxSurfaceMaterial>(matIndex);
	if (srfMat != nullptr)
	{
		wchar_t namebuff[64] = {};
		auto mtname = srfMat->GetName();
		size_t conv = 0;
		mbstowcs_s(&conv, namebuff, mtname, strlen(mtname));
		meshCont->m_MaterialId = std::wstring(namebuff);
	}
	else
	{
		meshCont->m_MaterialId = L"";
	}

	//頂点数保存
	meshCont->m_vertexCount = vertexCount;

	//ToDo :SkinAnime09
	//ボーン、スキンの読み込み
	int skinCount = pMesh->GetDeformerCount(FbxDeformer::eSkin);
	if (skinCount > 0)
	{
		// スキンアニメがあるのでMeshMatrixとBoneMatrixが必要になる。
		// 次にbone。こっちは工数でかい
		meshCont->InitSkinList(skinCount);
		std::vector< std::vector< std::pair<UINT, float> > > tempWeightVect;
		std::vector< std::vector<FbxSkinAnimeParams> > skinWeights;
		skinWeights.resize(skinCount);
		//スキンの数だけループ
		for (int skinloop = 0; skinloop < skinCount; skinloop++)
		{
			//スキンのウェイトデータ保存配列初期化
			tempWeightVect.clear();
			tempWeightVect.resize(contCount); //全コントロールポイント分の影響値スロットを確保
			//スキンを取得。これはFBXSDKの使い方でしかないので、こうやる、と覚えておけばOK。
			FbxSkin* pSkin = (FbxSkin*)pMesh->GetDeformer(skinloop, FbxDeformer::eSkin);
			skinWeights[skinloop].clear();
			skinWeights[skinloop].resize(contCount); //WeightデータはVertexではなくControllPoint用
			//スキンの中に幾つのクラスターがあるのかを取得。間接点とその影響点の集合を「クラスター」と呼ぶぞ。
			int clusterCount = pSkin->GetClusterCount();
			//クラスターの数だけループ
			for (int clusterloop = 0; clusterloop < clusterCount; clusterloop++)
			{
				FbxCluster* pCluster = pSkin->GetCluster(clusterloop); //クラスターを一つ取得
				//間接点のID番号を取得。GetClusterId内で新しい間接点だった場合は初期状態の逆行列も保存。
				int clIndex = GetClusterId(pCluster);
				//この間接点が影響を与えるコントロールポイント（頂点）の数
				int pointCnt = pCluster->GetControlPointIndicesCount();
				//この間接点が影響を与えるコントロールポイントのインデックス配列
				int* pPointArray = pCluster->GetControlPointIndices();
				//この間接点が影響を与えるコントロールポイントのウェイト配列、インデックスと組。
				double* pPointWeights = pCluster->GetControlPointWeights();
				//頂点ボーン影響パラメータ読み込み
				for (int pointloop = 0; pointloop < pointCnt; pointloop++)
				{
					//コントロールポイントのインデックス番号
					int ctrlIdx = pPointArray[pointloop];
					//そのコントロールポイントに与えるウェイト値
					double boneWeight = pPointWeights[pointloop];
					//とりあえずコントロールポイントごとにWeightデータを収集
					//今回は一つの頂点に最大８つまでウェイトデータを入れられる仕様。
					tempWeightVect[ctrlIdx].push_back({ clIndex, static_cast<float>(boneWeight) });
				}
			} //for clusterLoop
			//収集したControllPointごとのWeightデータを頂点用データに成形
			for (int cntLoop = 0; cntLoop < contCount; cntLoop++)
			{
				int wCnt = tempWeightVect[cntLoop].size(); //この頂点に登録された間接点データの数
				FbxSkinAnimeParams* skinparam = &skinWeights[skinloop][cntLoop];//頂点データ用影響度バッファ
				//登録数が最大の８に足らない分は０で埋めておく
				for (; wCnt < 8; wCnt++)
				{
					tempWeightVect[cntLoop].push_back({ 0, 0.0f });
				}
				skinparam->indices0 = { tempWeightVect[cntLoop][0].first, tempWeightVect[cntLoop][1].first,
tempWeightVect[cntLoop][2].first, tempWeightVect[cntLoop][3].first };
				skinparam->indices1 = { tempWeightVect[cntLoop][4].first, tempWeightVect[cntLoop][5].first,
				tempWeightVect[cntLoop][6].first, tempWeightVect[cntLoop][7].first };
				skinparam->weight0 = { tempWeightVect[cntLoop][0].second, tempWeightVect[cntLoop][1].second,
				tempWeightVect[cntLoop][2].second, tempWeightVect[cntLoop][3].second };
				skinparam->weight1 = { tempWeightVect[cntLoop][4].second, tempWeightVect[cntLoop][5].second,
				tempWeightVect[cntLoop][6].second, tempWeightVect[cntLoop][7].second };
			} //for cntLoop
		} //for skinLoop
		//結合したvertexデータを作成
		std::vector<FbxSkinAnimeVertex> skinVertex;
		skinVertex.clear();
		skinVertex.resize(vertexCount);
		for (int i = 0; i < vertexCount; i++) //ループのiは頂点のインデックス番号
		{
			int index = indices[i]; //skinWeightsがControllPoint用なので頂点番号からControllPoint用IDを取り出す
			skinVertex[i].vertex = meshCont->m_vertexData[i];//頂点データはそのままスライド
			skinVertex[i].skinvalues = skinWeights[0][index];//今回対応スキンは１つ。影響値配列はControllPoint基準
		}
		MyAccessHub::getMyGameEngine()->GetMeshManager()->
			AddVertexBuffer(id, skinVertex.data(), sizeof(FbxSkinAnimeVertex), vertexCount);
	} //if skinCount>0
	else
	{
		// スケルタルメッシュなし マルチメッシュでないならシンプルに出来るけども・・・
		if (pMesh->GetScene()->GetSrcObjectCount<FbxMesh>() > 1)
		{
			// ボーンはないけどマルチメッシュ。メッシュの基底ノードをボーンとして使う
			// メッシュ全体が変形せずに同じ方向に動くような物の場合（顔とか）はこの構造になっているので注意。
			std::vector<FbxSkinAnimeVertex> skinVertex;
			skinVertex.clear();
			skinVertex.resize(vertexCount);
			UINT boneId = GetClusterId(pMesh->GetNode());
			for (int i = 0; i < vertexCount; i++)
			{
				skinVertex[i].vertex = meshCont->m_vertexData[i];
				skinVertex[i].skinvalues.indices0 = { boneId, 0, 0, 0 };//基底NodeのIDをセットして・・・
				skinVertex[i].skinvalues.indices1 = { 0, 0, 0, 0 };
				skinVertex[i].skinvalues.weight0 = { 1.0f, 0, 0, 0 };//Weight（影響度）を1.0f（100%)にする。
				skinVertex[i].skinvalues.weight1 = { 0, 0, 0, 0 };
			}
			MyAccessHub::getMyGameEngine()->GetMeshManager()->
				AddVertexBuffer(id, skinVertex.data(), sizeof(FbxSkinAnimeVertex), vertexCount);
		}
		else
		{
			//ボーンもなくマルチメッシュでもない。シンプルなスタティックメッシュ。SkeDomeとか。
			MyAccessHub::getMyGameEngine()->GetMeshManager()->
				AddVertexBuffer(id, meshCont->m_vertexData.data(), sizeof(FbxVertex), vertexCount);
		}
	} //if (skinCount > 0) else
	//ToDo : ここまで

	//スキンアニメが来るとここの前に分岐が入るので今は仮
	//MyAccessHub::getMyGameEngine()->GetMeshManager()->AddVertexBuffer(id, meshCont->m_vertexData.data(), sizeof(FbxVertex), vertexCount);

	//ToDo 12:出来上がったMeshContainerオブジェクトをm_pMeshContainerに追加。
	//この時、meshCont変数はunique_ptrなのでmoveしないとpush出来ない事に注意。
	m_pMeshContainer.push_back(move(meshCont));
	//ToDo 12:ここまで

	return hr;
}
