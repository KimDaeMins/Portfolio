#include "stdafx.h"
#include "..\Public\ChildUIBooster.h"
#include "Player.h"


CChildUIBooster::CChildUIBooster(ID3D11Device * _pDevice, ID3D11DeviceContext * _pDeviceContext)
	:CChildUI(_pDevice, _pDeviceContext)
{
}

CChildUIBooster::CChildUIBooster(const CChildUIBooster & rhs)
	: CChildUI(rhs)
{
}

HRESULT CChildUIBooster::NativeConstruct_Prototype(_float _PosX, _float _PosY, _float _SizeX, _float _SizeY)
{
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f)));

	m_SizeX = _SizeX;
	m_SizeY = _SizeY;
	m_fX = _PosX;
	m_fY = _PosY;

	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	m_pTarget = (CPart_Body*)((CPlayer*)pInstance->Get_GameObjectPtr(LEVEL_GAMEPLAY, TEXT("Layer_Player")))->Get_Part(PART_BODY);
	Safe_AddRef(m_pTarget);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BoosterBar"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CChildUIBooster::NativeConstruct(void * pArg)
{
	return E_NOTIMPL;
}

_int CChildUIBooster::Tick(_float fTimeDelta)
{
	return _int();
}

_int CChildUIBooster::LateTick(_float fTimeDelta)
{
	
	m_RenderGap = m_pTarget->Get_BoosterGap();
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pInstance->Add_RenderGroup(CRenderer::RENDER_UI, this)))
		return _uint();

	RELEASE_INSTANCE(CGameInstance);
	return _int();
}

HRESULT CChildUIBooster::Render(_float fTimeDelta)
{
	_float4x4		WorldMatrix, ViewMatrix;

	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());

	m_pVIBufferCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
	WorldMatrix._11 = m_SizeX;
	WorldMatrix._22 = m_SizeY;
	WorldMatrix._41 = m_fX - (g_iWinCX >> 1);
	WorldMatrix._42 = -m_fY + (g_iWinCY >> 1);

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));

	m_pVIBufferCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));

	m_pVIBufferCom->Set_RawValue("g_Gap", &m_RenderGap, sizeof(_float));

	if (m_RenderGap > m_TexChangeGap)
	{
		if (FAILED(m_pTextureCom->SetUp_OnShader(m_pVIBufferCom, "g_DiffuseTexture", 0)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pTextureCom->SetUp_OnShader(m_pVIBufferCom, "g_DiffuseTexture", 1)))
			return E_FAIL;
	}
	m_pVIBufferCom->Render(3);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	pGameInstance->Render_Font(TEXT("Font_Bazzi"), L"Booster", _float2(m_fX - 64.f, m_fY + m_SizeY * 0.5f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 20.f);

	wstring String = to_wstring(m_pTarget->Get_Booster());
	String += L" / ";
	String += to_wstring(m_pTarget->Get_BoosterMax());

	pGameInstance->Render_Font(TEXT("Font_Bazzi"), String.c_str(), _float2(m_fX + 64.f, m_fY + m_SizeY * 0.5f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 20.f);
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CChildUIBooster * CChildUIBooster::Create(ID3D11Device * _pDevice, ID3D11DeviceContext * _pDeviceContext, _float _PosX, _float _PosY, _float _SizeX, _float _SizeY)
{
	CChildUIBooster* p = new CChildUIBooster(_pDevice, _pDeviceContext);

	if (FAILED(p->NativeConstruct_Prototype(_PosX, _PosY, _SizeX, _SizeY)))
	{
		Safe_Release(p);
		return nullptr;
	}
	return p;
}

void CChildUIBooster::Free()
{
	Safe_Release(m_pTextureCom);

	Safe_Release(m_pVIBufferCom);

	Safe_Release(m_pTarget);
}

CGameObject * CChildUIBooster::Clone(void * pArg)
{
	return nullptr;
}
