#include "Model.h"
#include "Direct3D.h"

namespace Model
{
	std::vector<ModelData*>modelList;

	int Model::Load(std::string _fileName)
	{
		// 構造体のインスタンスを作成
		ModelData* pData = new ModelData;

		// そのメンバのファイル名に引数を代入
		pData->fileName = _fileName;

		// Fbxオブジェクトを作成し、ロード
		bool isUsed = false;	//使ったことのあるモデルか

		for (auto& e : modelList)
		{

			if (e != nullptr && e->fileName == _fileName) {
				pData->pFbx = e->pFbx;
				isUsed = true;
				break;
			}
		}

		//使ったことのないファイルを開く
		if (isUsed == false) {
			pData->pFbx = new Fbx;
			if (FAILED(pData->pFbx->Load(_fileName))) {
				//開けなかった
				SAFE_DELETE(pData->pFbx);
				SAFE_DELETE(pData);
				return -1;
			}
			pData->fileName = _fileName;	//開けた
		}

		// 構造体の中身が埋まったので、動的配列に突っ込む
		modelList.push_back(pData);

		// 番号（配列の要素数-1）を返す
		return (int)modelList.size() - 1;
	}

	void Model::SetTransform(int _hModel, Transform _t)
	{	//モデル番号は、modelListのインデックス
		modelList[_hModel]->transform = _t;
	}

	void Model::Draw(int _hModel)
	{	//モデル番号は、modelListのインデックス
		modelList[_hModel]->pFbx->Draw(modelList[_hModel]->transform);
	}

	void Release()
	{
		bool isReffed = false;	//参照されたか
		for (int i = 0; i < modelList.size(); i++)
		{
			for (int j = i + 1; j < modelList.size(); j++)
			{
				if (modelList[i]->pFbx == modelList[j]->pFbx) {	// どっちかが片方を参照してる
					isReffed = true;
					break;
				}
			}
			if (isReffed == false) {	// 参照してないならdelete
				SAFE_DELETE(modelList[i]->pFbx);
			}
			SAFE_DELETE(modelList[i]);
		}
		modelList.clear();	// メモリに入ってるアドレスを消す(メモリリークを防ぐ)
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