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
	//�X�L���A�j���p�f�[�^�폜
	//m_skinCount = 0; //�ƌ����Ă��f�X�g���N�^���Ȃ̂ŏ����I�ȈӖ��͂Ȃ��B
	//ToDo: �����܂�

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

	//�m�[�h��
	int nodecount = pMesh->GetNodeCount();

	//MeshContainer�Ɍ��f�[�^�|�C���^�ۑ�
	meshCont->SetFbxMesh(pMesh);

	// VertexBuffer
	// ���ꂪ���_�f�[�^�{�̂����ǂ��A���̓{�[���_���܂ނ̂Łu�R���g���[���|�C���g�v
	FbxVector4* controllPoints = nullptr;

	// IndexBuffer
	// �C���f�b�N�X�f�[�^
	int* indices = nullptr;

	// ���_��
	int vertexCount;
	// �R���g���[���|�C���g��
	int contCount;

	//ToDo 08:���b�V����{�f�[�^�ǂݍ��� 
	// VertexBuffer
	controllPoints = pMesh->GetControlPoints();
	// IndexBuffer
	// ���\�b�h�����킩��h���B�R���g���[���|�C���g�̒�����|���S�����_�̃C���f�b�N�X���X�g
	indices = pMesh->GetPolygonVertices();
	// ���_��
	vertexCount = pMesh->GetPolygonVertexCount();
	// �R���g���[���|�C���g�� ������g���̂̓X�L���A�j���̎�
	contCount = pMesh->GetControlPointsCount();
	//ToDo 08: �����܂�

	meshCont->m_vertexData.clear();
	meshCont->m_vertexData.resize(vertexCount);

	//���b�V�����Ƃ̓����蔻��G���A�l������
	meshCont->m_vtxMin.x = FLT_MAX;
	meshCont->m_vtxMin.y = FLT_MAX;
	meshCont->m_vtxMin.z = FLT_MAX;

	meshCont->m_vtxMax.x = FLT_MIN;
	meshCont->m_vtxMax.y = FLT_MIN;
	meshCont->m_vtxMax.z = FLT_MIN;

	for (int i = 0; i < vertexCount; i++)
	{
		// Vertex�f�[�^��������
		ZeroMemory(&meshCont->m_vertexData[i], sizeof(FbxVertex));

		//ToDo 09:���_�f�[�^��FBX���玩�O�̒��_�o�b�t�@�� 
		// �C���f�b�N�X�o�b�t�@���璸�_�ԍ����擾
		int index = indices[i];
		// ���_���W���X�g������W���擾����
		meshCont->m_vertexData[i].position.x = (float)controllPoints[index][0];
		meshCont->m_vertexData[i].position.y = (float)controllPoints[index][1];
		meshCont->m_vertexData[i].position.z = (float)controllPoints[index][2];
		//ToDo 9:�����܂�

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

	//�ۑ�����min��max��S�̂�min�Amax�Ɣ�r���čX�V
	if (m_vtxTotalMax.x < meshCont->m_vtxMax.x) m_vtxTotalMax.x = meshCont->m_vtxMax.x;
	if (m_vtxTotalMax.y < meshCont->m_vtxMax.y) m_vtxTotalMax.y = meshCont->m_vtxMax.y;
	if (m_vtxTotalMax.z < meshCont->m_vtxMax.z) m_vtxTotalMax.z = meshCont->m_vtxMax.z;

	if (m_vtxTotalMin.x > meshCont->m_vtxMin.x) m_vtxTotalMin.x = meshCont->m_vtxMin.x;
	if (m_vtxTotalMin.y > meshCont->m_vtxMin.y) m_vtxTotalMin.y = meshCont->m_vtxMin.y;
	if (m_vtxTotalMin.z > meshCont->m_vtxMin.z) m_vtxTotalMin.z = meshCont->m_vtxMin.z;

	FbxStringList uvset_names;
	// UVSet�̖��O���X�g���擾
	// �}���`�e�N�X�`���̏ꍇ�AUV����������A���ꂼ��ɖ��O�����Ă���B
	pMesh->GetUVSetNames(uvset_names);

	FbxArray<FbxVector2> uv_buffer;

	// UVSet�̖��O����UVSet���擾����
	// ����̓}���`�e�N�X�`���ɂ͑Ή����Ȃ��̂ōŏ��̖��O����
	// �}���`�e�N�X�`�����g�����b�V���ɂ�InputLayout���V�F�[�_���ς�邩�璍��
	pMesh->GetPolygonVertexUVs(uvset_names.GetStringAt(0), uv_buffer);

	for (int i = 0; i < uv_buffer.Size(); i++)
	{
		FbxVector2& uv = uv_buffer[i];

		//ToDo 10:UV�f�[�^�����O���_�f�[�^�ɔ��f 
		// ��{�I�Ƀo�b�t�@�̒l�����̂܂܎g�p����񂾂��ǂ��E�E�E
		meshCont->m_vertexData[i].uv.x = (float)uv[0];
		meshCont->m_vertexData[i].uv.y = (float)(1.0 - uv[1]); //FBX��V�͂Ђ�����Ԃ��Ă���̂�1.0
		//ToDo 10: �����܂�
	}


	//index�̍쐬
	meshCont->m_indexData.clear();
	meshCont->m_indexData.resize(vertexCount);

	//�|���S�������擾
	int length = pMesh->GetPolygonCount();

	//�|���S�����Ń��[�v
	for (int i = 0; i < length; i++)
	{
		//ToDo 11:�C���f�b�N�X�f�[�^�̍쐬 
		int baseIndex = i * 3; //�|���S���J�E���g���R���C���f�b�N�X�̐擪
		//�f�[�^���E��n�łЂ�����Ԃ��Ă���ꍇ�́A�����ŃC���f�b�N�X�̈�ڂƎO�ڂ̒l�Ђ�����Ԃ��Ηǂ�
		//���ǁA����͂Ђ�����Ԃ��Ȃ��B�A�j���ƃZ�b�g�ŕϊ����\�b�h�łЂ�����Ԃ��B����͌�̍u�`�ŁB
		meshCont->m_indexData[baseIndex] = static_cast<ULONG>(baseIndex);
		meshCont->m_indexData[baseIndex + 1] = static_cast<ULONG>(baseIndex + 1);
		meshCont->m_indexData[baseIndex + 2] = static_cast<ULONG>(baseIndex + 2);
		//ToDo 11:�����܂� 
	}

	//�쐬�����C���f�b�N�X�f�[�^�ŃV�F�[�_���\�[�X���쐬
	//MyAccessHub::getMyGameEngine()->GetMeshManager()->AddIndexBuffer(id, meshCont->m_indexData.data(), sizeof(ULONG), vertexCount);

	auto vbResource = Renderer::GetInstance().CreateDefaultBuffer(
		meshCont->m_vertexData.data(),
		sizeof(FbxVertex) * vertexCount,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
	);
	meshComponent->SetVertexBuffer(vbResource);


	//�@�����d�v�Ȃ񂾂��ǂ��A�Ȃ񂾂������悤�ȗ���Ȃ̂�ToDo�͏ȗ�
	//�@��
	FbxArray<FbxVector4> normals;
	// �@�����X�g�̎擾
	pMesh->GetPolygonVertexNormals(normals);

	//�@���o�^
	for (int i = 0; i < normals.Size(); i++)
	{
		meshCont->m_vertexData[i].normal.x = normals[i][0];
		meshCont->m_vertexData[i].normal.y = normals[i][1];
		meshCont->m_vertexData[i].normal.z = normals[i][2];
	}

	//���_�J���[
	{

		// ���_�J���[�f�[�^�̐����m�F
		int color_count = pMesh->GetElementVertexColorCount();

		// ���_�J���[�f�[�^�̎擾
		// �{���͂�����color_count�����[�v�ŉ񂵂āE�E�E�ƌ������ꂪ��������B
		// ����͊ȗ����̈�Diffuse�����ōs���̂ōŏ��̃J���[�����擾
		FbxGeometryElementVertexColor* vertex_colors = pMesh->GetElementVertexColor(0);

		if (color_count == 0 || vertex_colors == nullptr)
		{
			//���̒��_�f�[�^�ɂ͐F�ݒ肪�Ȃ��̂őS�Ĕ��ŕ␳
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

			//���_�J���[�̓J���[�ݒ胂�[�h�Œl�̎������ς��B�Q�[���͊�{���ړ��͒l���ƌo���I�ɂ͎v���B
			if (mapping_mode == FbxLayerElement::eByPolygonVertex)
			{
				if (reference_mode == FbxLayerElement::eIndexToDirect)
				{
					// ���_�J���[�o�b�t�@�擾
					FbxLayerElementArrayTemplate<FbxColor>& colors = vertex_colors->GetDirectArray();
					// ���_�J���[�C���f�b�N�X�o�b�t�@�擾
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

	//���_���ۑ�
	meshCont->m_vertexCount = vertexCount;

	//ToDo :SkinAnime09
	//�{�[���A�X�L���̓ǂݍ���
	int skinCount = pMesh->GetDeformerCount(FbxDeformer::eSkin);
	if (skinCount > 0)
	{
		// �X�L���A�j��������̂�MeshMatrix��BoneMatrix���K�v�ɂȂ�B
		// ����bone�B�������͍H���ł���
		meshCont->InitSkinList(skinCount);
		std::vector< std::vector< std::pair<UINT, float> > > tempWeightVect;
		std::vector< std::vector<FbxSkinAnimeParams> > skinWeights;
		skinWeights.resize(skinCount);
		//�X�L���̐��������[�v
		for (int skinloop = 0; skinloop < skinCount; skinloop++)
		{
			//�X�L���̃E�F�C�g�f�[�^�ۑ��z�񏉊���
			tempWeightVect.clear();
			tempWeightVect.resize(contCount); //�S�R���g���[���|�C���g���̉e���l�X���b�g���m��
			//�X�L�����擾�B�����FBXSDK�̎g�����ł����Ȃ��̂ŁA�������A�Ɗo���Ă�����OK�B
			FbxSkin* pSkin = (FbxSkin*)pMesh->GetDeformer(skinloop, FbxDeformer::eSkin);
			skinWeights[skinloop].clear();
			skinWeights[skinloop].resize(contCount); //Weight�f�[�^��Vertex�ł͂Ȃ�ControllPoint�p
			//�X�L���̒��Ɋ�̃N���X�^�[������̂����擾�B�Ԑړ_�Ƃ��̉e���_�̏W�����u�N���X�^�[�v�ƌĂԂ��B
			int clusterCount = pSkin->GetClusterCount();
			//�N���X�^�[�̐��������[�v
			for (int clusterloop = 0; clusterloop < clusterCount; clusterloop++)
			{
				FbxCluster* pCluster = pSkin->GetCluster(clusterloop); //�N���X�^�[����擾
				//�Ԑړ_��ID�ԍ����擾�BGetClusterId���ŐV�����Ԑړ_�������ꍇ�͏�����Ԃ̋t�s����ۑ��B
				int clIndex = GetClusterId(pCluster);
				//���̊Ԑړ_���e����^����R���g���[���|�C���g�i���_�j�̐�
				int pointCnt = pCluster->GetControlPointIndicesCount();
				//���̊Ԑړ_���e����^����R���g���[���|�C���g�̃C���f�b�N�X�z��
				int* pPointArray = pCluster->GetControlPointIndices();
				//���̊Ԑړ_���e����^����R���g���[���|�C���g�̃E�F�C�g�z��A�C���f�b�N�X�Ƒg�B
				double* pPointWeights = pCluster->GetControlPointWeights();
				//���_�{�[���e���p�����[�^�ǂݍ���
				for (int pointloop = 0; pointloop < pointCnt; pointloop++)
				{
					//�R���g���[���|�C���g�̃C���f�b�N�X�ԍ�
					int ctrlIdx = pPointArray[pointloop];
					//���̃R���g���[���|�C���g�ɗ^����E�F�C�g�l
					double boneWeight = pPointWeights[pointloop];
					//�Ƃ肠�����R���g���[���|�C���g���Ƃ�Weight�f�[�^�����W
					//����͈�̒��_�ɍő�W�܂ŃE�F�C�g�f�[�^��������d�l�B
					tempWeightVect[ctrlIdx].push_back({ clIndex, static_cast<float>(boneWeight) });
				}
			} //for clusterLoop
			//���W����ControllPoint���Ƃ�Weight�f�[�^�𒸓_�p�f�[�^�ɐ��`
			for (int cntLoop = 0; cntLoop < contCount; cntLoop++)
			{
				int wCnt = tempWeightVect[cntLoop].size(); //���̒��_�ɓo�^���ꂽ�Ԑړ_�f�[�^�̐�
				FbxSkinAnimeParams* skinparam = &skinWeights[skinloop][cntLoop];//���_�f�[�^�p�e���x�o�b�t�@
				//�o�^�����ő�̂W�ɑ���Ȃ����͂O�Ŗ��߂Ă���
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
		//��������vertex�f�[�^���쐬
		std::vector<FbxSkinAnimeVertex> skinVertex;
		skinVertex.clear();
		skinVertex.resize(vertexCount);
		for (int i = 0; i < vertexCount; i++) //���[�v��i�͒��_�̃C���f�b�N�X�ԍ�
		{
			int index = indices[i]; //skinWeights��ControllPoint�p�Ȃ̂Œ��_�ԍ�����ControllPoint�pID�����o��
			skinVertex[i].vertex = meshCont->m_vertexData[i];//���_�f�[�^�͂��̂܂܃X���C�h
			skinVertex[i].skinvalues = skinWeights[0][index];//����Ή��X�L���͂P�B�e���l�z���ControllPoint�
		}
		MyAccessHub::getMyGameEngine()->GetMeshManager()->
			AddVertexBuffer(id, skinVertex.data(), sizeof(FbxSkinAnimeVertex), vertexCount);
	} //if skinCount>0
	else
	{
		// �X�P���^�����b�V���Ȃ� �}���`���b�V���łȂ��Ȃ�V���v���ɏo���邯�ǂ��E�E�E
		if (pMesh->GetScene()->GetSrcObjectCount<FbxMesh>() > 1)
		{
			// �{�[���͂Ȃ����ǃ}���`���b�V���B���b�V���̊��m�[�h���{�[���Ƃ��Ďg��
			// ���b�V���S�̂��ό`�����ɓ��������ɓ����悤�ȕ��̏ꍇ�i��Ƃ��j�͂��̍\���ɂȂ��Ă���̂Œ��ӁB
			std::vector<FbxSkinAnimeVertex> skinVertex;
			skinVertex.clear();
			skinVertex.resize(vertexCount);
			UINT boneId = GetClusterId(pMesh->GetNode());
			for (int i = 0; i < vertexCount; i++)
			{
				skinVertex[i].vertex = meshCont->m_vertexData[i];
				skinVertex[i].skinvalues.indices0 = { boneId, 0, 0, 0 };//���Node��ID���Z�b�g���āE�E�E
				skinVertex[i].skinvalues.indices1 = { 0, 0, 0, 0 };
				skinVertex[i].skinvalues.weight0 = { 1.0f, 0, 0, 0 };//Weight�i�e���x�j��1.0f�i100%)�ɂ���B
				skinVertex[i].skinvalues.weight1 = { 0, 0, 0, 0 };
			}
			MyAccessHub::getMyGameEngine()->GetMeshManager()->
				AddVertexBuffer(id, skinVertex.data(), sizeof(FbxSkinAnimeVertex), vertexCount);
		}
		else
		{
			//�{�[�����Ȃ��}���`���b�V���ł��Ȃ��B�V���v���ȃX�^�e�B�b�N���b�V���BSkeDome�Ƃ��B
			MyAccessHub::getMyGameEngine()->GetMeshManager()->
				AddVertexBuffer(id, meshCont->m_vertexData.data(), sizeof(FbxVertex), vertexCount);
		}
	} //if (skinCount > 0) else
	//ToDo : �����܂�

	//�X�L���A�j��������Ƃ����̑O�ɕ��򂪓���̂ō��͉�
	//MyAccessHub::getMyGameEngine()->GetMeshManager()->AddVertexBuffer(id, meshCont->m_vertexData.data(), sizeof(FbxVertex), vertexCount);

	//ToDo 12:�o���オ����MeshContainer�I�u�W�F�N�g��m_pMeshContainer�ɒǉ��B
	//���̎��AmeshCont�ϐ���unique_ptr�Ȃ̂�move���Ȃ���push�o���Ȃ����ɒ��ӁB
	m_pMeshContainer.push_back(move(meshCont));
	//ToDo 12:�����܂�

	return hr;
}
