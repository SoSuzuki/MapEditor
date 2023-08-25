#pragma once
#include <vector>
#include <string>
#include "Fbx.h"

namespace Model
{
	struct ModelData
	{
		Fbx* pFbx = nullptr;
		Transform transform;
		std::string fileName;
	};

	/// <summary>
	/// モデルデータをロードする
	/// </summary>
	/// <param name="_fileName">ファイル名</param>
	/// <returns>そのモデルデータに割り当てられた番号</returns>
	int Load(std::string _fileName);

	/// <summary>
	/// 対象モデルのトランスフォームをセットする
	/// </summary>
	/// <param name="_hModel">モデルハンドル</param>
	/// <param name="_t">トランスフォーム</param>
	void SetTransform(int _hModel, Transform _t);

	/// <summary>
	/// FBXのDrawを使って描画
	/// </summary>
	/// <param name="_hModel">モデルハンドル</param>
	void Draw(int _hModel);

	/// <summary>
	/// 任意のモデルを解放
	/// </summary>
	/// <param name="_hModel">モデルハンドル</param>
	void Release();

	/// <summary>
	/// 全てのモデルを解放
	/// </summary>
	void AllRelease();
};
