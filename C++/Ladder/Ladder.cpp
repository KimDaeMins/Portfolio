#include "stdafx.h"
#include "..\public\Ladder.h"

#include "GameInstance.h"
#include "Player.h"

CLadderZone::CLadderZone(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CLadderZone::CLadderZone(const CLadderZone& rhs)
	: CGameObject(rhs)
{
}

HRESULT CLadderZone::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CLadderZone::NativeConstruct(void* pArg)
{
	if (!pArg)
		return E_FAIL;
	if (FAILED(SetUp_Component()))
		return E_FAIL;

	PUSHLADDERPARTDATA Data = *(PUSHLADDERPARTDATA*)pArg;
	
	CRigid::PUSHRIGIDDATA Desc;
	Desc.StaticRigid = true;
	if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Rigid"), TEXT("Com_RigidBody"), (CComponent**)&m_pRigid, &Desc)))
		return E_FAIL;

	m_pRigid->Set_Host(this);

	m_pRigid->Set_Transform(m_pTransformCom->Get_WorldMatrix());

	_Vector3 LookPosition;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CRigid* PlayerRigid = (CRigid*)pGameInstance->Get_ComponentPtr(L"Layer_Player", L"Com_RigidBody");
	wstring ColliderName = L"Rigid";
	RELEASE_INSTANCE(CGameInstance);
	_Matrix RotationMatrix = RotationMatrix.CreateRotationY(XMConvertToRadians(Data.Rotate));

	if (Data.Up)
	{
		m_Type = (_uint)OBJECT_TYPE::LADDER_UP;

		LookPosition = { -0.5f, - Data.Extents.y + 0.1f  ,0.f };
		LookPosition = _Vector3::Transform(LookPosition, RotationMatrix);
		Data.Position = { Data.Position.x - LookPosition.x, Data.Position.y , Data.Position.z - LookPosition.z };
	}
	else
	{
		m_Type = (_uint)OBJECT_TYPE::LADDER_DOWN;
		LookPosition = { 1.49f, Data.Extents.y + 0.01f + PlayerRigid->Get_Radius(ColliderName) + PlayerRigid->Get_Height(ColliderName) ,0.f};
		LookPosition = _Vector3::Transform(LookPosition, RotationMatrix);
		Data.Position = { Data.Position.x - LookPosition.x , Data.Position.y , Data.Position.z - LookPosition.z };
	}
	//혹시라도 잘 안맞으면 직접 수정보는게 나을듯 if문안에 넣고.
	ColliderName = L"Trigger";
	m_pTransformCom->Set_State(STATE_POSITION, _Vector4(Data.Position, 1.f));
	m_pRigid->Create_BoxCollider(ColliderName, Data.Extents, LookPosition, _Vector3(0.f, XMConvertToRadians(Data.Rotate), 0.f));
	m_pRigid->Attach_Shape();
	m_pRigid->SetUp_Filtering(tagFilterGroup::OBJECT);
	m_pRigid->Set_isQuery(ColliderName, false);
	m_pRigid->Set_isSimulation(ColliderName, false);
	m_pRigid->Set_isTrigger(ColliderName, true);

	return S_OK;
}

_int CLadderZone::Tick(_float fTimeDelta)
{

	return _int();
}

_int CLadderZone::LateTick(_float fTimeDelta)
{
	return _int();
}

HRESULT CLadderZone::Render(_uint iRenderGroup)
{
	return S_OK;
}

HRESULT CLadderZone::SetUp_Component()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component_STATIC(TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	return S_OK;
}



CLadderZone* CLadderZone::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLadderZone* pInstance = new CLadderZone(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CLadderZone");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CLadderZone::Clone(void* pArg)
{
	CLadderZone* pInstance = new CLadderZone(*this);


	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CLadderZone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLadderZone::Free()
{

	__super::Free();
	// 	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRigid);
}
