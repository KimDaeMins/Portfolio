#include "stdafx.h"
#include "..\Public\ChildUIWeapon.h"
#include "Player.h"

CChildUIWeapon::CChildUIWeapon(ID3D11Device * _pDevice, ID3D11DeviceContext * _pDeviceContext)
	:CChildUI(_pDevice, _pDeviceContext)
{
}

CChildUIWeapon::CChildUIWeapon(const CChildUIWeapon & rhs)
	:CChildUI(rhs)
{
}

HRESULT CChildUIWeapon::NativeConstruct_Prototype(_float _PosX, _float _PosY, _uint _WeaponIndex)
{

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f)));

	m_WeaponIndex = _WeaponIndex;

	m_SizeX[PART_IMAGE] = 38;
	m_SizeY[PART_IMAGE] = 38;
	m_fX[PART_IMAGE] = _PosX - 86.f;
	m_fY[PART_IMAGE] = _PosY;

	m_SizeX[PART_BAR] = 172;
	m_SizeY[PART_BAR] = 22;
	m_fX[PART_BAR] = _PosX + 19.f;
	m_fY[PART_BAR] = _PosY + 8.f;

	m_SizeX[PART_NAME] = 172;
	m_SizeY[PART_NAME] = 16;
	m_fX[PART_NAME] = _PosX + 19.f;
	m_fY[PART_NAME] = _PosY - 11.f;

	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	m_pTargetWeapon = ((CPlayer*)pInstance->Get_GameObjectPtr(LEVEL_GAMEPLAY, TEXT("Layer_Player")))->Get_Weapon(m_WeaponIndex);
	if (m_pTargetWeapon)
		Safe_AddRef(m_pTargetWeapon);
	else
		return S_OK;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	wstring TextureName = TEXT("Prototype_Component_Texture_") + m_pTargetWeapon->Get_Name();

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TextureName.c_str(), TEXT("Com_WeaponTexture"), (CComponent**)&m_pTextureCom[0])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BulletBar"), TEXT("Com_BulletBar"), (CComponent**)&m_pTextureCom[1])))
		return E_FAIL;


	m_UseBullet = m_pTargetWeapon->Get_UseBullet();
	m_pTargetWeapon->Get_NowBullet();
	m_pTargetWeapon->Get_MaxBullet();

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

//¾È¾¸
HRESULT CChildUIWeapon::NativeConstruct(void * pArg)
{
	return S_OK;
}

//¾È¾¸
_int CChildUIWeapon::Tick(_float fTimeDelta)
{
	return _int();
}

_int CChildUIWeapon::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pTargetWeapon)
		return _uint();
	if (!m_UseBullet)
		m_RenderGap = 1.f;
	else
	{
		m_RenderGap = (_float)m_pTargetWeapon->Get_NowBullet() / (_float)m_pTargetWeapon->Get_MaxBullet();

		if (m_RenderGap < 0.f)
			m_RenderGap = 0.f;
	}
	
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pInstance->Add_RenderGroup(CRenderer::RENDER_UI, this)))
		return _uint();
			
	RELEASE_INSTANCE(CGameInstance);
	return _int();
}

HRESULT CChildUIWeapon::Render(_float fTimeDelta)
{
	if (nullptr == m_pTargetWeapon)
		return S_OK;

	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	_float4x4		WorldMatrix, ViewMatrix;

	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());

	m_pVIBufferCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));


	{
		XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
		WorldMatrix._11 = m_SizeX[PART_IMAGE];
		WorldMatrix._22 = m_SizeY[PART_IMAGE];
		WorldMatrix._41 = m_fX[PART_IMAGE] - (g_iWinCX >> 1);
		WorldMatrix._42 = -m_fY[PART_IMAGE] + (g_iWinCY >> 1);

		XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));

		m_pVIBufferCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));

		if (FAILED(m_pTextureCom[0]->SetUp_OnShader(m_pVIBufferCom, "g_DiffuseTexture")))
			return E_FAIL;

		m_pVIBufferCom->Render(4);
	}
	{
		XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
		WorldMatrix._11 = m_SizeX[PART_BAR];
		WorldMatrix._22 = m_SizeY[PART_BAR];
		WorldMatrix._41 = m_fX[PART_BAR] - (g_iWinCX >> 1);
		WorldMatrix._42 = -m_fY[PART_BAR] + (g_iWinCY >> 1);

		XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));

		m_pVIBufferCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
		m_pVIBufferCom->Set_RawValue("g_Gap", &m_RenderGap, sizeof(_float));

		if (FAILED(m_pTextureCom[1]->SetUp_OnShader(m_pVIBufferCom, "g_DiffuseTexture")))
			return E_FAIL;

		m_pVIBufferCom->Render(3);
		if (m_WeaponIndex == 2)
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			wstring String = to_wstring(m_pTargetWeapon->Get_SpareBullet());
			_float2 Pos = { m_fX[PART_BAR], m_fY[PART_BAR] };
			Pos.x -= m_SizeX[PART_BAR] * 0.5f;
			Pos.y -= m_SizeY[PART_BAR] * 0.5f;
			pGameInstance->Render_Font(TEXT("Font_Bazzi"), String.c_str(), Pos, XMVectorSet(1.f, 1.f, 1.f, 1.f), m_SizeY[PART_BAR]);
			RELEASE_INSTANCE(CGameInstance);
		}
		else if (m_UseBullet)
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			wstring String = to_wstring(m_pTargetWeapon->Get_NowBullet());
			String += L"       / ";
			String += to_wstring(m_pTargetWeapon->Get_SpareBullet());

			_float2 Pos = { m_fX[PART_BAR], m_fY[PART_BAR] };
			Pos.x -= m_SizeX[PART_BAR] * 0.5f;
			Pos.y -= m_SizeY[PART_BAR] * 0.5f;
			pGameInstance->Render_Font(TEXT("Font_Bazzi"), String.c_str(), Pos, XMVectorSet(1.f, 1.f, 1.f, 1.f), m_SizeY[PART_BAR]);
			RELEASE_INSTANCE(CGameInstance);
		}}
	{
		XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
		WorldMatrix._11 = m_SizeX[PART_NAME];
		WorldMatrix._22 = m_SizeY[PART_NAME];
		WorldMatrix._41 = m_fX[PART_NAME] - (g_iWinCX >> 1);
		WorldMatrix._42 = -m_fY[PART_NAME] + (g_iWinCY >> 1);

		XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));

		m_pVIBufferCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));

		//if (FAILED(m_pTextureCom[0]->SetUp_OnShader(m_pVIBufferCom, "g_DiffuseTexture")))
		//	return E_FAIL;

		//m_pVIBufferCom->Render(0);

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		_float2 Pos = { m_fX[PART_NAME], m_fY[PART_NAME] };
		Pos.x -= m_SizeX[PART_NAME] * 0.5f;
		Pos.y -= m_SizeY[PART_NAME] * 0.5f;

		wstring String = m_pTargetWeapon->Get_Name();
		pGameInstance->Render_Font(TEXT("Font_Bazzi"), String.c_str(), Pos, XMVectorSet(1.f, 1.f, 1.f, 1.f), m_SizeY[PART_NAME]);
		RELEASE_INSTANCE(CGameInstance);
	}

	return S_OK;
}

CChildUIWeapon * CChildUIWeapon::Create(ID3D11Device * _pDevice, ID3D11DeviceContext * _pDeviceContext, _float _PosX, _float _PosY, _uint _WeaponIndex)
{

	CChildUIWeapon* p = new CChildUIWeapon(_pDevice, _pDeviceContext);

	if (FAILED(p->NativeConstruct_Prototype(_PosX, _PosY, _WeaponIndex)))
	{
		Safe_Release(p);
		return nullptr;
	}
	return p;
}

void CChildUIWeapon::Free()
{
	Safe_Release(m_pTextureCom[0]);
	Safe_Release(m_pTextureCom[1]);

	Safe_Release(m_pVIBufferCom);

	Safe_Release(m_pTargetWeapon);
}

CGameObject * CChildUIWeapon::Clone(void * pArg)
{
	return nullptr;
}
