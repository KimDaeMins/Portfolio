#include "stdafx.h"
#include "..\Public\ChildUIHp.h"


CChildUIHp::CChildUIHp(ID3D11Device * _pDevice, ID3D11DeviceContext * _pDeviceContext)
	:CChildUI(_pDevice, _pDeviceContext)
{
}

CChildUIHp::CChildUIHp(const CChildUIHp & rhs)
	:CChildUI(rhs)
{
}

HRESULT CChildUIHp::NativeConstruct_Prototype(_float _PosX, _float _PosY, _float _SizeX, _float _SizeY)
{
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f)));

	m_SizeX = _SizeX;
	m_SizeY= _SizeY;
	m_fX = _PosX;
	m_fY = _PosY;

	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
	for (int i = 0; i < 3; ++i)
		m_RenderGap[i] = 0.f;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	m_pTarget = ((CPlayer*)pInstance->Get_GameObjectPtr(LEVEL_GAMEPLAY, TEXT("Layer_Player")));
	Safe_AddRef(m_pTarget);


	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_HPBar"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CChildUIHp::NativeConstruct(void * pArg)
{
	return E_NOTIMPL;
}

_int CChildUIHp::Tick(_float fTimeDelta)
{
	return _int();
}

_int CChildUIHp::LateTick(_float fTimeDelta)
{
	if (m_RecorveDelay < 3.f)
		m_RecorveDelay += fTimeDelta;

	m_RenderGap[HP_BLUE] = (_float)m_pTarget->Get_Hp() / (_float)m_pTarget->Get_MaxHp();
	
	if (m_RenderGap[HP_BLUE] < 0)
		m_RenderGap[HP_BLUE] = 0.f;

	if (m_pTarget->Get_Recorve())
		m_RecorveDelay = 0.f;

	if (m_RecorveDelay >= 3.f || m_RenderGap[HP_WHITE] > m_RenderGap[HP_BLUE])
		m_RenderGap[HP_WHITE] = m_RenderGap[HP_BLUE];

	if (m_HitDelay < 2.f)
		m_HitDelay += fTimeDelta;

	if (m_pTarget->Get_Hit())
		m_HitDelay = 0.f;

	if (m_HitDelay >= 2.f || m_RenderGap[HP_BLUE] > m_RenderGap[HP_RED])
		m_RenderGap[HP_RED] = m_RenderGap[HP_BLUE];

	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pInstance->Add_RenderGroup(CRenderer::RENDER_UI, this)))
		return _uint();

	RELEASE_INSTANCE(CGameInstance);
	return _int();
}

HRESULT CChildUIHp::Render(_float fTimeDelta)
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

	for (_uint i = 0; i < HP_END; ++i)
	{
		m_pVIBufferCom->Set_RawValue("g_Gap", &m_RenderGap[i], sizeof(_float));

		if (FAILED(m_pTextureCom->SetUp_OnShader(m_pVIBufferCom, "g_DiffuseTexture", i)))
			return E_FAIL;

		m_pVIBufferCom->Render(3);
	}

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	pGameInstance->Render_Font(TEXT("Font_Bazzi"), L"HP", _float2(m_fX, m_fY + m_SizeY * 0.5f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 20.f);

	wstring String = to_wstring(m_pTarget->Get_Hp());
	String += L" / ";
	String += to_wstring(m_pTarget->Get_MaxHp());

	pGameInstance->Render_Font(TEXT("Font_Bazzi"), String.c_str(), _float2(m_fX + 200.f, m_fY + m_SizeY * 0.5f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 20.f);
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CChildUIHp * CChildUIHp::Create(ID3D11Device * _pDevice, ID3D11DeviceContext * _pDeviceContext, _float _PosX, _float _PosY, _float _SizeX, _float _SizeY)
{
	CChildUIHp* p = new CChildUIHp(_pDevice, _pDeviceContext);

	if (FAILED(p->NativeConstruct_Prototype(_PosX, _PosY, _SizeX, _SizeY)))
	{
		Safe_Release(p);
		return nullptr;
	}
	return p;
}

void CChildUIHp::Free()
{
	Safe_Release(m_pTextureCom);

	Safe_Release(m_pVIBufferCom);

	Safe_Release(m_pTarget);
}

CGameObject * CChildUIHp::Clone(void * pArg)
{
	return nullptr;
}
