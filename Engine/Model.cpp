#include "Model.h"
#include "Direct3D.h"

namespace Model
{
	std::vector<ModelData*>modelList;

	int Model::Load(std::string _fileName)
	{
		// �\���̂̃C���X�^���X���쐬
		ModelData* pData = new ModelData;

		// ���̃����o�̃t�@�C�����Ɉ�������
		pData->fileName = _fileName;

		// Fbx�I�u�W�F�N�g���쐬���A���[�h
		bool isUsed = false;	//�g�������Ƃ̂��郂�f����

		for (auto& e : modelList)
		{

			if (e != nullptr && e->fileName == _fileName) {
				pData->pFbx = e->pFbx;
				isUsed = true;
				break;
			}
		}

		//�g�������Ƃ̂Ȃ��t�@�C�����J��
		if (isUsed == false) {
			pData->pFbx = new Fbx;
			if (FAILED(pData->pFbx->Load(_fileName))) {
				//�J���Ȃ�����
				SAFE_DELETE(pData->pFbx);
				SAFE_DELETE(pData);
				return -1;
			}
			pData->fileName = _fileName;	//�J����
		}

		// �\���̂̒��g�����܂����̂ŁA���I�z��ɓ˂�����
		modelList.push_back(pData);

		// �ԍ��i�z��̗v�f��-1�j��Ԃ�
		return (int)modelList.size() - 1;
	}

	void Model::SetTransform(int _hModel, Transform _t)
	{	//���f���ԍ��́AmodelList�̃C���f�b�N�X
		modelList[_hModel]->transform = _t;
	}

	void Model::Draw(int _hModel)
	{	//���f���ԍ��́AmodelList�̃C���f�b�N�X
		modelList[_hModel]->pFbx->Draw(modelList[_hModel]->transform);
	}

	void Release()
	{
		bool isReffed = false;	//�Q�Ƃ��ꂽ��
		for (int i = 0; i < modelList.size(); i++)
		{
			for (int j = i + 1; j < modelList.size(); j++)
			{
				if (modelList[i]->pFbx == modelList[j]->pFbx) {	// �ǂ��������Е����Q�Ƃ��Ă�
					isReffed = true;
					break;
				}
			}
			if (isReffed == false) {	// �Q�Ƃ��ĂȂ��Ȃ�delete
				SAFE_DELETE(modelList[i]->pFbx);
			}
			SAFE_DELETE(modelList[i]);
		}
		modelList.clear();	// �������ɓ����Ă�A�h���X������(���������[�N��h��)
	}

	void RayCast(int _hModel, RayCastData& _rayData)
	{	
		//0 ���f���̃g�����X�t�H�[�����J���L�����[�V����
		modelList[_hModel]->transform.Calclation();
		//�@���[���h�s��̋t�s��
		XMMATRIX wInv = XMMatrixInverse(nullptr, modelList[_hModel]->transform.GetWorldMatrix());
		//�A���C�̒ʉߓ_�����߂�(���f����Ԃł̗�̕����x�N�g�������߂�)
		XMVECTOR vPass{ _rayData.start.x + _rayData.dir.x,
						_rayData.start.y + _rayData.dir.y,
						_rayData.start.z + _rayData.dir.z,
						_rayData.start.w + _rayData.dir.w };
		//�BrayData.start�����f����Ԃɕϊ�(�@��������)
		XMVECTOR vStart = XMLoadFloat4(&_rayData.start);
		vStart = XMVector3TransformCoord(vStart, wInv);
		//�C(���_��������x�N�g�������傢�L�΂�����)�ʉߓ_(�A)�ɇ@��������
		vPass = XMVector3TransformCoord(vPass, wInv);
		//�DrayData.dir���B����C�Ɍ������x�N�g���ɂ���(�����Z)
		vPass -= vStart;
		XMStoreFloat4(&_rayData.dir, vPass);
		//�w�肵�����f���ԍ���FBX�Ƀ��C�L���X�g
		modelList[_hModel]->pFbx->RayCast(_rayData);
	}

	//void AllRelease()
	//{
	//	for (int i = 0; i < modelList.size(); i++)
	//	{
	//		if (modelList[i] != nullptr) {
	//			Release(i);
	//		}
	//	}
	//}
};