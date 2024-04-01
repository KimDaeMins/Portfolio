#include "stdafx.h"
#include "..\Public\UIPlayer.h"

#include "ChildUI.h"
#include "ChildUIBooster.h"
#include "ChildUIHp.h"
#include "ChildUIWeapon.h"
CUIPlayer::CUIPlayer(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CUIPlayer::CUIPlayer(const CUIPlayer & rhs)
	:CGameObject(rhs)
{
}

HRESULT CUIPlayer::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CUIPlayer::NativeConstruct(void * pArg)
{
	//여기서 이제 텍스쳐를 로드해야겠지.
	//Why? 로드할거니까.
	//근데 로드를 네이티브 컨스트럭트에서 하는게 맞을까?
	//로드는 프로토타입에서 하는게 맞지않을까? 맞지않다.
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;


	m_UIData = *(UIDATA*)pArg;
	//스트링까지 =연산이 되는지는 모르겠넹 기억이안나
	
	if (FAILED(SetUp_Component()))
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_UIData.Texture_Prototype.c_str(), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f)));

	return S_OK;
}

_int CUIPlayer::Tick(_float fTimeDelta)
{
	return _int();
}

_int CUIPlayer::LateTick(_float fTimeDelta)
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	pInstance->Add_RenderGroup(CRenderer::RENDER_UI, this);

	RELEASE_INSTANCE(CGameInstance);
	if (m_Once)
	{
		CreatChild();
		m_Once = false;
	}

	if (m_ChildUI)
		m_ChildUI->LateTick(fTimeDelta);

	return _int();
}

HRESULT CUIPlayer::Render(_float fTimeDelta)
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	_float4x4		WorldMatrix, ViewMatrix;

	_matrix RotationMatrix;
	RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(m_UIData.Rotate));

	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * m_UIData.SizeX;
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * m_UIData.SizeY;

	vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	vUp = XMVector3TransformNormal(vUp, RotationMatrix);

	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
	memcpy(&WorldMatrix._11, &vRight, sizeof(_float4));
	memcpy(&WorldMatrix._21, &vUp, sizeof(_float4));
	WorldMatrix._41 = m_UIData.PosX - (g_iWinCX >> 1);
	WorldMatrix._42 = -m_UIData.PosY + (g_iWinCY >> 1);

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));

	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());

	m_pVIBufferCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_Alpha", &m_UIData.Alpha, sizeof(_float));
	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pVIBufferCom, "g_DiffuseTexture")))
		return E_FAIL;

	m_pVIBufferCom->Render(m_UIData.Pass);


	if (m_ChildUI)
		m_ChildUI->Render(fTimeDelta);


	return S_OK;
}

void CUIPlayer::CreatChild()
{
	switch (m_UIData.ChildIndex)
	{
	case DUI_WEAPON0:
	case DUI_WEAPON1:
	case DUI_WEAPON2:
	case DUI_WEAPON3:
		m_ChildUI = CChildUIWeapon::Create(m_pDevice, m_pDeviceContext, m_UIData.PosX, m_UIData.PosY, m_UIData.ChildIndex);
		break;
	case DUI_HP:
		m_ChildUI = CChildUIHp::Create(m_pDevice, m_pDeviceContext, m_UIData.PosX, m_UIData.PosY, m_UIData.SizeX, m_UIData.SizeY);
		break;
	case DUI_BOOSTER:
		m_ChildUI = CChildUIBooster::Create(m_pDevice, m_pDeviceContext, m_UIData.PosX, m_UIData.PosY, m_UIData.SizeX, m_UIData.SizeY);
		break;
	case DUI_MAP:
		break;
	default:
		return;
	}
}

HRESULT CUIPlayer::SetUp_Component()
{
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_RectTlqkf"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}


CUIPlayer * CUIPlayer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUIPlayer* p = new CUIPlayer(pDevice, pDeviceContext);

	if (FAILED(p->NativeConstruct_Prototype()))
	{
		Safe_Release(p);
		return nullptr;
	}
	return p;
}

CGameObject * CUIPlayer::Clone(void * pArg)
{
	CUIPlayer* p = new CUIPlayer(*this);

	if (FAILED(p->NativeConstruct(pArg)))
	{
		Safe_Release(p);
		return nullptr;
	}
	return p;
}

void CUIPlayer::Free()
{
	
	Safe_Release(m_ChildUI);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
