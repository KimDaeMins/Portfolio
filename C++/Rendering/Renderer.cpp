#include "..\public\Renderer.h"
#include "GameObject.h"

#include "Target_Manager.h"
#include "Light_Manager.h"
#include "VIBuffer_Rect.h"
#include "Graphic_Device.h"

CRenderer::CRenderer()
	: m_pTarget_Manager(CTarget_Manager::GetInstance())
	, m_pLight_Manager(CLight_Manager::GetInstance())
{
	m_RenderObjects = make_shared<list<shared_ptr<CGameObject>>[]>(RENDER_END);
}

HRESULT CRenderer::NativeConstruct_Prototype()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	_uint		iNumViewports = 1;
	D3D11_VIEWPORT	ViewportDesc;
	ZeroMemory(&ViewportDesc, sizeof(D3D11_VIEWPORT));

	g_DeviceContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_Diffuse"), _uint(ViewportDesc.Width), _uint(ViewportDesc.Height), DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_Normal") , _uint(ViewportDesc.Width), _uint(ViewportDesc.Height), DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_Depth") , _uint(ViewportDesc.Width), _uint(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_Emissive") , _uint(ViewportDesc.Width), _uint(ViewportDesc.Height), DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_Shade") , _uint(ViewportDesc.Width), _uint(ViewportDesc.Height), DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.0f, 0.0f, 0.0f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_Specular") , _uint(ViewportDesc.Width), _uint(ViewportDesc.Height), DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_Bloom_Weith") , _uint(ViewportDesc.Width), _uint(ViewportDesc.Height), DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_Bloom_Height") , _uint(ViewportDesc.Width), _uint(ViewportDesc.Height), DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_LightDepth") , 4096, 4096, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Create_ShadowDepth((_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height)))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Normal"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Depth"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Emissive"))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Specular"))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Bloom_Weith"), TEXT("Target_Bloom_Weith"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Bloom_Height"), TEXT("Target_Bloom_Height"))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_LightDepth"), TEXT("Target_LightDepth"))))
		return E_FAIL;

	if (FAILED(m_pLight_Manager->NativeConstruct()))
		return E_FAIL;


	m_pVIBuffer = CVIBuffer_Rect::Create(TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"));
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	XMStoreFloat4x4(&m_TransformMatrix, XMMatrixIdentity());
	m_TransformMatrix._11 = ViewportDesc.Width;
	m_TransformMatrix._22 = ViewportDesc.Height;
	m_TransformMatrix._41 = 0.0f;
	m_TransformMatrix._42 = 0.0f;

	XMStoreFloat4x4(&m_OrthoMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f)));


#ifdef _DEBUG


	if (FAILED(m_pTarget_Manager->Ready_DebugBuffer(TEXT("Target_Diffuse"), 0, 0, 150, 150)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_DebugBuffer(TEXT("Target_Normal"), 0, 150, 150, 150)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_DebugBuffer(TEXT("Target_Depth"), 0, 300, 150, 150)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_DebugBuffer(TEXT("Target_Emissive"), 0, 450, 150, 150)))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Ready_DebugBuffer(TEXT("Target_LightDepth"), 150, 0, 150, 150)))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Ready_DebugBuffer(TEXT("Target_Shade"), 300, 0, 150, 150)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_DebugBuffer(TEXT("Target_Specular"), 300, 150, 150, 150)))
		return E_FAIL;

#endif // _DEBUG

	_float Weight[8]{ 1.f ,0.8f,0.60f,0.49f,0.39f,0.28f,0.17f,0.09f };
	_float Normalization = Weight[0] 
						+ 2 * (Weight[1] + Weight[2] + Weight[3] + Weight[4] + Weight[5] + Weight[6] + Weight[7]);
	Weight[0] /= Normalization;
	Weight[1] /= Normalization;
	Weight[2] /= Normalization;
	Weight[3] /= Normalization;
	Weight[4] /= Normalization;
	Weight[5] /= Normalization;
	Weight[6] /= Normalization;
	Weight[7] /= Normalization;
	_float TexGap[2]{ 1.f / 1280.f, 1.f / 720.f };


	m_pVIBuffer->Set_RawValue("TexGapX", &TexGap[0], sizeof(_float));
	m_pVIBuffer->Set_RawValue("TexGapY", &TexGap[1], sizeof(_float));
	m_pVIBuffer->Set_RawValue("Weight0", &Weight[0], sizeof(_float));
	m_pVIBuffer->Set_RawValue("Weight1", &Weight[1], sizeof(_float));
	m_pVIBuffer->Set_RawValue("Weight2", &Weight[2], sizeof(_float));
	m_pVIBuffer->Set_RawValue("Weight3", &Weight[3], sizeof(_float));
	m_pVIBuffer->Set_RawValue("Weight4", &Weight[4], sizeof(_float));
	m_pVIBuffer->Set_RawValue("Weight5", &Weight[5], sizeof(_float));
	m_pVIBuffer->Set_RawValue("Weight6", &Weight[6], sizeof(_float));
	m_pVIBuffer->Set_RawValue("Weight7", &Weight[7], sizeof(_float));

	return S_OK;
}

/* 객체생성시에 추가한다(x) */
/* 매 프레임마다 그려져야할 객체를 판명하여 추가하낟. */
HRESULT CRenderer::Add_RenderGroup(RENDER eRenderGroup, shared_ptr<CGameObject> pGameObject)
{
	if (eRenderGroup >= RENDER_END)
		return E_FAIL;

	m_RenderObjects[eRenderGroup].push_back(pGameObject);

	return S_OK;	
}

HRESULT CRenderer::Add_RenderGroup(RENDER eRenderGroup, shared_ptr<CComponent> pDebugComponent)
{
	if (eRenderGroup >= RENDER_END)
		return E_FAIL;

	m_DebugComponent.push_back(pDebugComponent);

	return S_OK;
}

HRESULT CRenderer::Draw(_float fTimeDelta)
{
	if (FAILED(Render_Priority(fTimeDelta)))
		return E_FAIL;
	if (FAILED(Render_NonAlpha(fTimeDelta)))
		return E_FAIL;
	if (FAILED(Render_LightDepth()))
		return E_FAIL;
	if (FAILED(Render_LightAcc(fTimeDelta)))
		return E_FAIL;
	if (FAILED(Render_Bloom()))
		return E_FAIL;
	if (FAILED(Render_Blend()))
		return E_FAIL;
	if (FAILED(Render_NonLight(fTimeDelta)))
		return E_FAIL;
	if (FAILED(Render_Alpha(fTimeDelta)))
		return E_FAIL;
#ifdef _DEBUG
	if (FAILED(Render_DebugComponent(fTimeDelta)))
		return E_FAIL;
#endif
	if (FAILED(Render_UI(fTimeDelta)))
		return E_FAIL;

#ifdef _DEBUG
	m_pTarget_Manager->Render_DebugBuffer(TEXT("MRT_Deferred"), 0);
	m_pTarget_Manager->Render_DebugBuffer(TEXT("MRT_LightDepth"), 0);
	m_pTarget_Manager->Render_DebugBuffer(TEXT("MRT_LightAcc"), 0);
#endif // _DEBUG

	return S_OK;
}

void CRenderer::ClearRenderList()
{
	for (_uint iIndex = 0; iIndex < RENDER_END; ++iIndex)
	{
		m_RenderObjects[iIndex].clear();
	}
}

HRESULT CRenderer::Render_Priority(_float fTimeDelta)
{
	for (const auto& pGameObject : std::views::all(m_RenderObjects[RENDER_PRIORITY])
		| std::views::filter([](shared_ptr<CGameObject> obj) {return obj != nullptr; }))
	{
		if (FAILED(pGameObject->Render(fTimeDelta)))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CRenderer::Render_NonAlpha(_float fTimeDelta)
{
	if (FAILED(m_pTarget_Manager->Begin_MRT(TEXT("MRT_Deferred"))))
		return E_FAIL;

	for (const auto& pGameObject : std::views::all(m_RenderObjects[RENDER_NONALPHA])
		| std::views::filter([](shared_ptr<CGameObject> obj) {return obj != nullptr; }))
	{
		if (FAILED(pGameObject->Render(fTimeDelta)))
			return E_FAIL;
	}

	if (FAILED(m_pTarget_Manager->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_LightDepth()
{
	if (FAILED(m_pTarget_Manager->Begin_MRT(TEXT("MRT_LightDepth"))))
		return E_FAIL;


	for (const auto& pGameObject : std::views::all(m_RenderObjects[RENDER_NONALPHA])
		| std::views::filter([](shared_ptr<CGameObject> obj) {return obj != nullptr; }))
	{
		if (FAILED(pGameObject->RenderLightDepth()))
			return E_FAIL;
	}


	if (FAILED(m_pTarget_Manager->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_NonLight(_float fTimeDelta)
{

	for (const auto& pGameObject : std::views::all(m_RenderObjects[RENDER_NONALPHA])
		| std::views::filter([](shared_ptr<CGameObject> obj) {return obj != nullptr; }))
	{
		if (FAILED(pGameObject->Render(fTimeDelta)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CRenderer::Render_Alpha(_float fTimeDelta)
{

	m_RenderObjects[RENDER_ALPHA].sort([&](CGameObject* pSour, CGameObject* pDest) 
	{
		return pSour->Get_CamDistance() > pDest->Get_CamDistance();
	});

	for (const auto& pGameObject : std::views::all(m_RenderObjects[RENDER_ALPHA])
		| std::views::filter([](shared_ptr<CGameObject> obj) {return obj != nullptr; }))
	{
		if (FAILED(pGameObject->Render(fTimeDelta)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CRenderer::Render_UI(_float fTimeDelta)
{
	for (const auto& pGameObject : std::views::all(m_RenderObjects[RENDER_UI])
		| std::views::filter([](shared_ptr<CGameObject> obj) {return obj != nullptr; }))
	{
		if (FAILED(pGameObject->Render(fTimeDelta)))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CRenderer::Render_LightAcc(_float fTimeDelta)
{
	/* 장치에는 Target_Shade가 셋팅된다. */
	if (FAILED(m_pTarget_Manager->Begin_MRT(TEXT("MRT_LightAcc"))))
		return E_FAIL;

	/* Target_Shade에 그린다. */
	m_pLight_Manager->Render();

	if (FAILED(m_pTarget_Manager->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Blend()
{
	m_pVIBuffer->Set_RawValue("g_TransformMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&m_TransformMatrix)), sizeof(_float4x4));
	m_pVIBuffer->Set_RawValue("g_ProjMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&m_OrthoMatrix)), sizeof(_float4x4));

	m_pVIBuffer->Set_ShaderResourceView("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Diffuse")));
	m_pVIBuffer->Set_ShaderResourceView("g_ShadeTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Shade")));
	m_pVIBuffer->Set_ShaderResourceView("g_SpecularTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Specular")));

	m_pVIBuffer->Render(3);

	m_pVIBuffer->Set_RawValue("g_TransformMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&m_TransformMatrix)), sizeof(_float4x4));
	m_pVIBuffer->Set_RawValue("g_ProjMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&m_OrthoMatrix)), sizeof(_float4x4));

	m_pVIBuffer->Set_ShaderResourceView("g_EmissiveTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Bloom_Height")));
	m_pVIBuffer->Render(6);

	return S_OK;
}
HRESULT CRenderer::Render_Bloom()
{
	if (FAILED(m_pTarget_Manager->Begin_MRT(TEXT("MRT_Bloom_Weith"))))
		return E_FAIL;

	//먼저그리고 최종적용될녀석에게 알파블랜딩으로 한번더 그려줘야한다.
	m_pVIBuffer->Set_RawValue("g_TransformMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&m_TransformMatrix)), sizeof(_float4x4));
	m_pVIBuffer->Set_RawValue("g_ProjMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&m_OrthoMatrix)), sizeof(_float4x4));

	m_pVIBuffer->Set_ShaderResourceView("g_EmissiveTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Emissive")));

	//5는 이미시브를 받아와서 그려줄거고
	m_pVIBuffer->Render(4);
	if (FAILED(m_pTarget_Manager->End_MRT()))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Begin_MRT(TEXT("MRT_Bloom_Height"))))
		return E_FAIL;

	//먼저그리고 최종적용될녀석에게 알파블랜딩으로 한번더 그려줘야한다.
	m_pVIBuffer->Set_RawValue("g_TransformMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&m_TransformMatrix)), sizeof(_float4x4));
	m_pVIBuffer->Set_RawValue("g_ProjMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&m_OrthoMatrix)), sizeof(_float4x4));
	//6은 그려진 그녀석을 이미시브로 또 받아와서 그려줄거임
	m_pVIBuffer->Set_ShaderResourceView("g_EmissiveTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Bloom_Weith")));
	m_pVIBuffer->Render(5);

	if (FAILED(m_pTarget_Manager->End_MRT()))
		return E_FAIL;

	return S_OK;
}
#ifdef _DEBUG
HRESULT CRenderer::Render_DebugComponent(_float fTimeDelta)
{
	for (const auto& pDebugCom : std::views::all(m_DebugComponent)
		| std::views::filter([](shared_ptr<CComponent> debug) {return debug != nullptr; }))
	{
		if (FAILED(pDebugCom->Render()))
			return E_FAIL;
	}

	m_DebugComponent.clear();
	return S_OK;
}
#endif
shared_ptr<CRenderer> CRenderer::Create()
{
	shared_ptr<CRenderer> pInstance = make_shared<CRenderer>();

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		return nullptr;
	}

	return pInstance;
}
