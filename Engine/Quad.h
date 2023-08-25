#pragma once
#include <DirectXMath.h>
#include <vector>
#include "Direct3D.h"
#include "Camera.h"
#include "Texture.h"
#include "Transform.h"

using namespace DirectX;


//コンスタントバッファー
struct CONSTANT_BUFFER
{
	XMMATRIX	matWVP;
	XMMATRIX	matNormal;	//matWから改名
};

//頂点情報
struct VERTEX
{
	XMVECTOR position;	//位置
	XMVECTOR uv;		//UV座標
	XMVECTOR normal;	//法線
};

class Quad
{
protected:
	ID3D11Buffer* pVertexBuffer_;	//頂点バッファ
	ID3D11Buffer* pIndexBuffer_;	//インデックスバッファ
	ID3D11Buffer* pConstantBuffer_;	//コンスタントバッファ

	Texture* pTexture_;
	
	int vertexNum_;				//頂点数
	std::vector<VERTEX>vertices3_;	//頂点情報
	
	int indexNum_;		//インデックス数
	std::vector<int>index_;	//インデックス情報


public:
	Quad();
	virtual ~Quad();
	HRESULT Initialize();
	void Draw(Transform& transform);
	void Release();

	ID3D11Buffer* GetVertexBuffer() { return pVertexBuffer_; }
	ID3D11Buffer* GetIndexBuffer() { return pIndexBuffer_; }
	ID3D11Buffer* GetConstantBuffer() { return pConstantBuffer_; }

	

private:
	//---------Initializeから呼ばれる関数---------
	virtual void InitVertexData();	//頂点情報の準備
	HRESULT CreateVertexBuffer();	//頂点バッファを作成

	virtual void InitIndexData();	//インデックス情報の準備
	HRESULT CreateIndexBuffer();	//インデックスバッファを作成

	HRESULT CreateConstantBuffer();	//コンスタントバッファを作成

	HRESULT LoadTexture();			//テクスチャをロード

	//---------Draw関数から呼ばれる関数---------
	void PassDataToCB(DirectX::XMMATRIX worldMatrix);	//コンスタントバッファに情報を渡す
	void SetBufferToPipeline();


};
