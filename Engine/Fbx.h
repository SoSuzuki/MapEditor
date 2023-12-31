#pragma once

#include <d3d11.h>
#include <fbxsdk.h>
#include <string>
#include <vector>
#include "Direct3D.h"
#include "Transform.h"
#include "Camera.h"
//#include "Texture.h"

#pragma comment(lib, "LibFbxSDK-Md.lib")
#pragma comment(lib, "LibXml2-Md.lib")
#pragma comment(lib, "zlib-Md.lib")

using namespace Direct3D;

class Texture;

struct RayCastData
{
	XMFLOAT4 start;
	XMFLOAT4 dir;
	bool hit;
	float dist;

	RayCastData() { dist = 99999.0f; }
};

class Fbx
{
private:
	//マテリアル
	struct MATERIAL
	{
		Texture* pTexture;
		XMFLOAT4	diffuse;
	};

	struct CONSTANT_BUFFER
	{
		XMMATRIX	matWVP;
		XMMATRIX	matNormal;
		XMFLOAT4	diffuseColor;
		int		isTextured;
	};
	
	struct VERTEX
	{
		XMVECTOR position;	//位置
		XMVECTOR uv;		//UV座標
		XMVECTOR normal;	//法線
	};


	VERTEX* pVertices_;
	int** ppIndex_;

	int vertexCount_;	//頂点数
	int polygonCount_;	//ポリゴン数
	int materialCount_;	//マテリアルの個数
	std::vector<int>indexCount_;
	//int* indexCount_;

	ID3D11Buffer* pVertexBuffer_;
	ID3D11Buffer** pIndexBuffer_;
	ID3D11Buffer* pConstantBuffer_;
	MATERIAL* pMaterialList_;

	//Texture* pTexture_;

	void InitVertex(fbxsdk::FbxMesh* mesh);
	void InitIndex(fbxsdk::FbxMesh* mesh);
	void InitConstantBuffer();
	void InitMaterial(fbxsdk::FbxNode* pNode);

	//---------Draw関数から呼ばれる関数---------
	//void PassDataToCB(DirectX::XMMATRIX worldMatrix);	//コンスタントバッファに情報を渡す
	//void SetBufferToPipeline();

public:

	Fbx();
	~Fbx();

	HRESULT Load(std::string fileName);
	void Draw(Transform& transform);
	void Release();

	void RayCast(RayCastData& _rayData);
};

