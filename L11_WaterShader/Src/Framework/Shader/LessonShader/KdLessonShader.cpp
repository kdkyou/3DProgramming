#include "Framework/KdFramework.h"

#include "KdLessonShader.h"

//================================================
// 描画準備
//================================================

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 陰影をつけるオブジェクトの描画の直前処理（不透明な物体やキャラクタの板ポリゴン）
// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// シェーダーのパイプライン変更
// LitShaderで使用するリソースのバッファー設定
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void KdLessonShader::Begin()
{
    // 頂点シェーダーのパイプライン変更
    if (KdShaderManager::Instance().SetVertexShader(m_VS))
    {
        KdShaderManager::Instance().SetInputLayout(m_inputLayout);

		KdShaderManager::Instance().SetVSConstantBuffer(0, m_cb0_Obj.GetAddress());
		KdShaderManager::Instance().SetVSConstantBuffer(1, m_cb1_Mesh.GetAddress());
    }

    // ピクセルシェーダーのパイプライン変更
    if (KdShaderManager::Instance().SetPixelShader(m_PS))
    {
		KdShaderManager::Instance().SetPSConstantBuffer(0, m_cb0_Obj.GetAddress());
		KdShaderManager::Instance().SetPSConstantBuffer(2, m_cb2_Material.GetAddress());
    }
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 陰影ありオブジェクトの描画修了
// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void KdLessonShader::End()
{
}

bool KdLessonShader::Init()
{
    //-------------------------------------
    // 頂点シェーダ
    //-------------------------------------
    {
        // コンパイル済みのシェーダーヘッダーファイルをインクルード
#include "KdLessonShader_VS.shaderInc"

        // 頂点シェーダー作成
        if (FAILED(KdDirect3D::Instance().WorkDev()->CreateVertexShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_VS))) {
            assert(0 && "頂点シェーダー作成失敗");
            Release();
            return false;
        }

		// １頂点の詳細な情報
		std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,			0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM,		0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

        // 頂点入力レイアウト作成
        if (FAILED(KdDirect3D::Instance().WorkDev()->CreateInputLayout(
            &layout[0],				// 入力エレメント先頭アドレス
            layout.size(),			// 入力エレメント数
            &compiledBuffer[0],		// 頂点バッファのバイナリデータ
            sizeof(compiledBuffer),	// 上記のバッファサイズ
            &m_inputLayout))
            ) {
            assert(0 && "CreateInputLayout失敗");
            Release();
            return false;
        }

    }

    //-------------------------------------
    // ピクセルシェーダ
    //-------------------------------------
    {
#include "KdLessonShader_PS.shaderInc"

        if (FAILED(KdDirect3D::Instance().WorkDev()->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_PS))) {
            assert(0 && "ピクセルシェーダー作成失敗");
            Release();
            return false;
        }
    }

	//-------------------------------------
	// 定数バッファ
	//-------------------------------------
	m_cb0_Obj.Create();
	m_cb1_Mesh.Create();
	m_cb2_Material.Create();

    return true;
}

void KdLessonShader::Release()
{
    KdSafeRelease(m_VS);
    KdSafeRelease(m_PS);
    KdSafeRelease(m_inputLayout);

	m_cb0_Obj.Release();
	m_cb1_Mesh.Release();
	m_cb2_Material.Release();
}

void KdLessonShader::DrawMesh(const KdMesh* mesh, const Math::Matrix& mWorld, const std::vector<KdMaterial>& materials,
                              const Math::Vector4& col, const Math::Vector3& emissive)
{
    if (mesh == nullptr) { return; }

    // メッシュの頂点情報転送
    mesh->SetToDevice();

	// 3Dワールド行列転送
	m_cb1_Mesh.Work().mW = mWorld;
	m_cb1_Mesh.Write();

    // 全サブセット
    for (UINT subi = 0; subi < mesh->GetSubsets().size(); subi++)
    {
        // 面が１枚も無い場合はスキップ
        if (mesh->GetSubsets()[subi].FaceCount == 0)continue;

		// マテリアルデータの転送
		const KdMaterial& material = materials[mesh->GetSubsets()[subi].MaterialNo];
		WriteMaterial(material, col, emissive);

        //-----------------------
        // サブセット描画
        //-----------------------
        mesh->DrawSubset(subi);
    }
}

void KdLessonShader::DrawModel(const KdModelData& rModel, const Math::Matrix& mWorld,
                               const Math::Color& colRate, const Math::Vector3& emissive)
{
    auto& dataNodes = rModel.GetOriginalNodes();

	//オブジェクト単位の情報を転送 授業用
	if (m_dirtyCBObj)
	{
		m_cb0_Obj.Write();
	}

    // 全描画用メッシュノードを描画
    for (auto& nodeIdx : rModel.GetDrawMeshNodeIndices())
    {
        // 描画
        DrawMesh(dataNodes[nodeIdx].m_spMesh.get(), dataNodes[nodeIdx].m_worldTransform * mWorld,
            rModel.GetMaterials(), colRate, emissive);
    }

	//定数に変更があった場合は自動的に初期状態に戻す
	if (m_dirtyCBObj)
	{
		ResetCBObject();
	}
}


// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// モデルワークを描画（ダイナミック(アニメーションをする)なモデルに対応
// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// データに所属する全ての描画用メッシュをワークの3D行列に従って描画する
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void KdLessonShader::DrawModel(KdModelWork& rModel, const Math::Matrix& mWorld,
	const Math::Color& colRate, const Math::Vector3& emissive)
{
	if (!rModel.IsEnable()) { return; }

	const std::shared_ptr<KdModelData>& data = rModel.GetData();

	// データがないときはスキップ
	if (data == nullptr) { return; }

	if (rModel.NeedCalcNodeMatrices())
	{
		rModel.CalcNodeMatrices();
	}

	// オブジェクト単位の情報転送
	if (m_dirtyCBObj)
	{
		m_cb0_Obj.Write();
	}

	auto& workNodes = rModel.GetNodes();
	auto& dataNodes = data->GetOriginalNodes();

	// 全描画用メッシュノードを描画
	for (auto& nodeIdx : data->GetDrawMeshNodeIndices())
	{
		//授業用
		if (GetEnableOutLineDraw())
		{
			//表面をカリング(非表示)にするラスタライザステートをセットする
			KdShaderManager::Instance().ChangeRasterizerState(KdRasterizerState::CullFront);
		}

		// 描画
		DrawMesh(dataNodes[nodeIdx].m_spMesh.get(), workNodes[nodeIdx].m_worldTransform * mWorld,
			data->GetMaterials(), colRate, emissive);
	
		if (GetEnableOutLineDraw())
		{
			//表面をカリング(非表示)にするラスタライザステートをセットする
			KdShaderManager::Instance().ChangeRasterizerState(KdRasterizerState::CullBack);
		}

	}

	// 定数に変更があった場合は自動的に初期状態に戻す
	if (m_dirtyCBObj)
	{
		ResetCBObject();
	}
}



// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 描画用マテリアル情報の転送
// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// それぞれのマテリアルの影響倍率値とテクスチャを設定
// BaseColor：基本色 / Emissive：自己発光色 / Metalic：金属性(テカテカ) / Roughness：粗さ(材質の色の反映度)
// テクスチャは法線マップ以外は未設定なら白1ピクセルのシステムテクスチャを指定
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void KdLessonShader::WriteMaterial(	const KdMaterial& material, const Math::Vector4& colRate, 
									const Math::Vector3& emiRate)
{
	//-----------------------
	// マテリアル情報を定数バッファへ書き込む
	//-----------------------
	m_cb2_Material.Work().BaseColor = material.m_baseColorRate * colRate;
	m_cb2_Material.Work().Emissive	= material.m_emissiveRate * emiRate;
	m_cb2_Material.Work().Metallic	= material.m_metallicRate;
	m_cb2_Material.Work().Roughness = material.m_roughnessRate;
	m_cb2_Material.Write();

	//-----------------------
	// テクスチャセット
	//-----------------------
	ID3D11ShaderResourceView* srvs[4];

	srvs[0] = material.m_baseColorTex ? material.m_baseColorTex->WorkSRView() : KdDirect3D::Instance().GetWhiteTex()->WorkSRView();
	srvs[1] = material.m_metallicRoughnessTex ? material.m_metallicRoughnessTex->WorkSRView() : KdDirect3D::Instance().GetWhiteTex()->WorkSRView();
	srvs[2] = material.m_emissiveTex ? material.m_emissiveTex->WorkSRView() : KdDirect3D::Instance().GetWhiteTex()->WorkSRView();
	srvs[3] = material.m_normalTex ? material.m_normalTex->WorkSRView() : KdDirect3D::Instance().GetNormalTex()->WorkSRView();

	// セット
	KdDirect3D::Instance().WorkDevContext()->PSSetShaderResources(0, _countof(srvs), srvs);
}

//授業用
//定数バッファを初期状態に戻す
void KdLessonShader::ResetCBObject()
{
	m_cb0_Obj.Work() = cbObject();
	m_cb0_Obj.Write();

	m_dirtyCBObj = false;
}