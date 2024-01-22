#include "stdafx.h"
#include "..\public\Bush.h"

#include "GameInstance.h"
#include "Treetop.h"
#include "Stump.h"

CBush::CBush(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CBush::CBush(const CBush& rhs)
	: CGameObject(rhs)
{
}

HRESULT CBush::NativeConstruct_Prototype()
{

	return S_OK;
}

HRESULT CBush::NativeConstruct(void* pArg)
{
	if (FAILED(SetUp_Component()))
		return E_FAIL;

	m_Type = (_uint)OBJECT_TYPE::OBJECT;

	if (pArg)
	{
		PUSHBUSHDATA Data = *(PUSHBUSHDATA*)pArg;
		m_pTransformCom->Set_State(STATE_POSITION, _Vector4(Data.Position, 1.f));
		m_pTransformCom->Rotation(_Vector3(0.f, 1.f, 0.f), XMConvertToRadians(Data.YRotate));
	}
	else
	{
		m_pTransformCom->Set_State(STATE_POSITION, XMVectorSet(_float(rand() % 20), 0.f, _float(rand() % 20), 1.f));
	}
	MyUp = m_pTransformCom->Get_State(STATE_POSITION);
	MyUp.y += 2.f;

	CRigid::PUSHRIGIDDATA Data;
	Data.StaticRigid = true;
	if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Rigid"), TEXT("Com_RigidBody"), (CComponent**)&m_pRigid, &Data)))
		return E_FAIL;

	wstring ColliderName = L"Rigid";
	m_pRigid->Set_Host(this);
	m_pRigid->Create_BoxCollider(ColliderName, _Vector3(1.f, 2.f ,1.f), _Vector3(0.f , 2.f, 0.f));
	m_pRigid->Attach_Shape();
	m_pRigid->SetUp_Filtering(tagFilterGroup::OBJECT);
	m_pRigid->Set_Transform(m_pTransformCom->Get_WorldMatrix());
	m_pRenderTransformCom->Set_WorldMatrix(m_pTransformCom->Get_WorldMatrix());
	return S_OK;
}	


_int CBush::Tick(_float fTimeDelta)
{
	CGameObject* GameObject = nullptr;


	while (m_pRigid->Get_Collision_Trigger_Enter(&GameObject))
	{
		if (GameObject->Get_Type() == (_uint)OBJECT_TYPE::ATTACK)
		{
			if (GameObject->Get_DamegeType() == (_uint)DAMEGETYPE_TYPE::NORMAL)
			{
				m_bDead = true;
				m_pRigid->Detach_Shape();
			}
		}
	}

	if (m_bDead)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		CTreetop::PUSHTREETOPPDATA TreeTopData;
		TreeTopData.Position = m_pTransformCom->Get_State(STATE_POSITION);
		TreeTopData.LifeTime = 3.f;
		TreeTopData.Force = _Vector3(_float(rand() % 4) / 10.f, 0.2f, _float(rand() % 4) / 10.f);//일단대충랜덤
		TreeTopData.Torque = _Vector3(1.3f, 1.3f, 1.3f);
		TreeTopData.Type = CTreetop::BUSH;

		pGameInstance->Add_GameObjectToLayer(TEXT("Layer_Object"), TEXT("Prototype_GameObject_Treetop"), &TreeTopData);


		CStump::PUSHSTUMPDATA StumpData;
		StumpData.Position = m_pTransformCom->Get_State(STATE_POSITION);
		StumpData.Type = CStump::BUSH;

		pGameInstance->Add_GameObjectToLayer(TEXT("Layer_Object"), TEXT("Prototype_GameObject_Stump"), &StumpData);
		RELEASE_INSTANCE(CGameInstance);
	}

	return _int();
}

_int CBush::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;


	CGameObject* GameObject = nullptr;
	_bool		Pushed = false;
	_Vector3 MyPos = m_pTransformCom->Get_State(STATE_POSITION);
	while (m_pRigid->Get_Collision_Simulation_Stay(&GameObject))
	{
		if (GameObject->Get_Type() == (_uint)OBJECT_TYPE::PLAYER || GameObject->Get_Type() == (_uint)OBJECT_TYPE::MONSTER)
		{
			CTransform* TargetTransform = GameObject->Get_Transform();
			//내 Extents (사방이같음) 객체내부 호출이니까 판단할필요 없어보임
			wstring Name = L"Rigid";
			_float Extent = m_pRigid->Get_Extents(Name).x;
			_Vector3 TargetPos = TargetTransform->Get_State(STATE_POSITION);
			//이거 이동량으로 수정 -> 플레이어가 부쉬보다 늦게돼서 문제가생김 -> 플레이어를 강제적으로 거기다 놓는다고 했었나?
			_Vector3 TargetMove = TargetTransform->Get_AccPos();
			MyUp = MyPos;
			MyUp.y += 1.f;
			if (TargetPos.z > MyPos.z + Extent || TargetPos.z < MyPos.z - Extent)
			{
				MyUp.z += TargetMove.z;
				Pushed = true;
			}
			if (TargetPos.x > MyPos.x + Extent || TargetPos.x < MyPos.x - Extent)
			{
				MyUp.x += TargetMove.x;
				Pushed = true;
			}
		}
	}


	if (!Pushed)
	{
		_Vector3 Dir = MyPos - MyUp;
		Dir.y = 0;

		if (Dir.Length() < 0.1f)
		{
			MyUp.x = MyPos.x;
			MyUp.z = MyPos.z;
		}
		else
		{
			MyUp += Vec3Normalize(Dir) * fTimeDelta;
		}
	}
	//이거 if문 필요하면 하고..
	_Vector3 Up = MyUp - MyPos;
	_Vector3 Right = Up.Cross(_Vector3(0.f, 0.f, 1.f));
	_Vector3 Look = Right.Cross(Up);
	m_pRenderTransformCom->Set_Rotate(Vec3Normalize(Right), Vec3Normalize(Up), Vec3Normalize(Look));


	if (1)
	{
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this)))
			return 0;
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DYNAMICSHADOW, this)))
			return 0;
	}


	return _int();
}

HRESULT CBush::Render(_uint iRenderGroup)
{
	if (CRenderer::RENDER_NONALPHA == iRenderGroup)
	{
		if (FAILED(SetUp_ConstantTable()))
			return E_FAIL;

		/* 장치에 월드변환 행렬을 저장한다. */
		_uint	iNumMaterials = m_pModelCom->Get_NumMaterials();

		for (_uint i = 0; i < iNumMaterials; ++i)
		{
			m_pModelCom->Set_ShaderResourceView(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);

			m_pModelCom->Render(m_pShaderCom, i, 0);
		}
		//
		//#ifdef _DEBUG
		//	m_pAABBCom->Render();
		//	m_pOBBCom->Render();
		//#endif // _DEBUG
	}

	else if (CRenderer::RENDER_DYNAMICSHADOW == iRenderGroup)
	{
		if (FAILED(SetUp_ConstantTable()))
			return E_FAIL;

		_uint	iNumMaterials = m_pModelCom->Get_NumMaterials();

		for (_uint i = 0; i < iNumMaterials; ++i)
		{
			m_pModelCom->Render(m_pShaderCom, i, 2);
		}
	}


	return S_OK;
}

//HRESULT CBush::Render_Shadow()
//{
//	if (FAILED(SetUp_ConstantTable()))
//		return E_FAIL;
//
//	_uint	iNumMaterials = m_pModelCom->Get_NumMaterials();
//
//	for (_uint i = 0; i < iNumMaterials; ++i)
//	{
//		m_pModelCom->Render(i, 2);
//	}
//
//	return S_OK;
//}

HRESULT CBush::SetUp_Component()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component_STATIC(TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(__super::Add_Component_STATIC(TEXT("Prototype_Component_Transform"), TEXT("Com_RenderTransform"), (CComponent**)&m_pRenderTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component_STATIC(TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Model_Bush_Base"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Shader_Mesh"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBush::SetUp_ConstantTable()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	m_pRenderTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix");
	pGameInstance->Bind_Transform_OnShader(TS_VIEW, m_pShaderCom, "g_ViewMatrix");
	pGameInstance->Bind_Transform_OnShader(TS_PROJ, m_pShaderCom, "g_ProjMatrix");

	m_pShaderCom->Set_RawValue("g_LightViewMatrix", &pGameInstance->Get_LightTransform(TS_VIEW), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_LightProjMatrix", &pGameInstance->Get_LightTransform(TS_PROJ), sizeof(_float4x4));

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}


CBush* CBush::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CBush* pInstance = new CBush(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CBush");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CBush::Clone(void* pArg)
{
	CBush* pInstance = new CBush(*this);


	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CBush");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBush::Free()
{

	__super::Free();
	// 	Safe_Release(m_pTransformCom);
	// 	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRenderTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRigid);
	

}
