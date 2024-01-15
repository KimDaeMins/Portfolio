#include "stdafx.h"
#include "WandBulletExplosion.h"
#include "GameInstance.h"
#include "BombDust.h"

CWandBulletExplosion::CWandBulletExplosion(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CWandBulletExplosion::CWandBulletExplosion(const CWandBulletExplosion& rhs)
	:CGameObject(rhs)
{
}

HRESULT CWandBulletExplosion::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CWandBulletExplosion::NativeConstruct(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	PUSHWANDBULLETEXPLOSIONDATA Data = *(PUSHWANDBULLETEXPLOSIONDATA*)pArg;

	m_fLifeTime = Data.LifeTime;
	m_StartHalfRadius = Data.StartHalfRadius;
	m_EndHalfRadius = Data.EndHalfRadius;
	m_TimeAcc /= m_fLifeTime;
	Create_Status((_uint)DAMEGETYPE_TYPE::EXPLOSIVE, Data.Damege, 10.f, 10.f);
	//Set_Damege(0.f);
	m_Type = (_uint)OBJECT_TYPE::ENTER_ATTACK;
	m_pTransformCom->Set_State(STATE_POSITION, Data.Position);

	m_Radius = m_StartHalfRadius;

	wstring ColliderName = L"Trigger";
	m_pRigid->Set_Host(this); 
	m_pRigid->Create_SphereCollider(ColliderName, m_StartHalfRadius, _Vector3(0.f, 0.f, 0.f));
	m_pRigid->Attach_Shape();
	m_pRigid->Set_Transform(m_pTransformCom->Get_WorldMatrix());
	m_pRigid->SetUp_Filtering(tagFilterGroup::NONEHITPLAYEROBJECT);
	m_pRigid->Set_Gravity(false);
	m_pRigid->Set_isQuery(ColliderName, false);
	m_pRigid->Set_isSimulation(ColliderName, false);
	m_pRigid->Set_isTrigger(ColliderName, true);
	return S_OK;
}

_int CWandBulletExplosion::Tick(_float fTimeDelta)
{
	m_fLifeTime -= fTimeDelta;


	if (m_fLifeTime < 0.f)
	{
		m_Radius = CEasing::Lerp(m_EndHalfRadius, 0.f, -m_fLifeTime, CEasing::EaseType::easeLiner);
		if (m_fLifeTime < -1.f)
		{
			m_bDead = true;
			m_pRigid->Detach_Shape();
			m_pRigid->Detach_Rigid();
		}
		m_CreateWaitTime += fTimeDelta;
		if (m_CreateWaitTime > 0.1f)
		{
			m_CreateWaitTime = 0.f;
			CBombDust::PUSHBOMBDUSTDATA Data;
			Data.Diffuse = _Vector4(1.f, 1.f, 1.f, 1.f);
			Data.Glow = true;
			Data.MainPos = m_pTransformCom->Get_State(STATE_POSITION);
			Data.NumBombDust = 50;
			Data.SizeGap = 2.f;
			Data.SpeedGap = m_Radius * 0.1f;
			Data.YSpeed = 0.1f;
			Data.StartSize = m_Radius * 0.2f;
			Data.WaitTime = 0.7f;
			Data.WaitTimeGap = 0.5f;
			Data.YDirGap = 0.2f;

			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			pGameInstance->Add_GameObjectToLayer(L"Layer_Effect", L"Prototype_GameObject_BombDustEffect", &Data);
			Data.Glow = false;
			pGameInstance->Add_GameObjectToLayer(L"Layer_Effect", L"Prototype_GameObject_BombDustEffect", &Data);
			RELEASE_INSTANCE(CGameInstance);
		}
	}
	else
	{
		m_BeforeRadious = m_Radius;
		m_Radius = CEasing::Lerp(m_StartHalfRadius, m_EndHalfRadius, 1 - m_fLifeTime * m_TimeAcc, CEasing::EaseType::easeInOutBack);
	}

	wstring Name = L"Trigger";
	m_pRigid->Set_Geometry(Name, m_Radius);
	m_pTransformCom->Set_Scale(_Vector3(m_Radius, m_Radius, m_Radius));
	return _int();
}

_int CWandBulletExplosion::LateTick(_float fTimeDelta)
{
	if (m_bDead)
		return _int();

	if (nullptr == m_pRendererCom)
		return -1;

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this)))
		return 0;
	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_GLOW, this)))
		return 0;

	return _int();
}

HRESULT CWandBulletExplosion::Render(_uint iRenderGroup)
{
	if (CRenderer::RENDER_NONALPHA == iRenderGroup)
	{
		if (FAILED(SetUp_ConstantTable()))
			return E_FAIL;

		_uint	iNumMaterials = m_pModelCom->Get_NumMaterials();

		for (_uint i = 0; i < iNumMaterials; ++i)
		{
			m_pShaderCom->Set_RawValue("g_vMtrlDiffuse", &_Vector4(1.f, 1.f, 1.f, 1.f), sizeof(_float4));
			m_pModelCom->Render(m_pShaderCom, i, 3);
		}
	}

	else if (CRenderer::RENDER_GLOW == iRenderGroup)
	{

		if (FAILED(SetUp_ConstantTable()))
			return E_FAIL;

		_uint	iNumMaterials = m_pModelCom->Get_NumMaterials();

		m_pShaderCom->Set_RawValue("g_vGlowColor_Front", &_Vector4(1.f * m_Radius * 0.2f, 1.f * m_Radius * 0.2f, 0.f, 1.f), sizeof(_float4));
		m_pShaderCom->Set_RawValue("g_vGlowColor_Back", &_Vector4(1.f * m_Radius * 0.2f, 0.f, 0.f, 1.f), sizeof(_float4));
		for (_uint i = 0; i < iNumMaterials; ++i)
		{
			m_pShaderCom->Set_RawValue("g_vMtrlDiffuse", &_Vector4(1.f, 1.f, 1.f, 1.f), sizeof(_float4));
			m_pModelCom->Render(m_pShaderCom, i, 15);
		}
	}


	return S_OK;
}

HRESULT CWandBulletExplosion::SetUp_Component()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component_STATIC(TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;
	

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Shader_Mesh"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component_STATIC(TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Model */
	CRigid::PUSHRIGIDDATA RigidDesc;
	RigidDesc.StaticRigid = true;
	if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Rigid"), TEXT("Com_RigidBody"), (CComponent**)&m_pRigid, &RigidDesc)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Model_MiddleDust"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;



	return S_OK;
}

HRESULT CWandBulletExplosion::SetUp_ConstantTable()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix");
	pGameInstance->Bind_Transform_OnShader(TS_VIEW, m_pShaderCom, "g_ViewMatrix");
	pGameInstance->Bind_Transform_OnShader(TS_PROJ, m_pShaderCom, "g_ProjMatrix");

	m_pShaderCom->Set_RawValue("g_LightViewMatrix", &pGameInstance->Get_LightTransform(TS_VIEW), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_LightProjMatrix", &pGameInstance->Get_LightTransform(TS_PROJ), sizeof(_float4x4));

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CWandBulletExplosion* CWandBulletExplosion::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CWandBulletExplosion* pInstance = new CWandBulletExplosion(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CWandBulletExplosion");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CWandBulletExplosion::Clone(void* pArg)
{
	CWandBulletExplosion* pInstance = new CWandBulletExplosion(*this);


	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CWandBulletExplosion");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CWandBulletExplosion::Free()
{
	__super::Free();
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRigid);
}
