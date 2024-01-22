#include "stdafx.h"
#include "..\public\Treetop.h"

#include "GameInstance.h"

CTreetop::CTreetop(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CTreetop::CTreetop(const CTreetop& rhs)
	: CGameObject(rhs)
{
}

HRESULT CTreetop::NativeConstruct_Prototype()
{

	return S_OK;
}

HRESULT CTreetop::NativeConstruct(void* pArg)
{

	if (pArg == nullptr)
		return E_FAIL;

	PUSHTREETOPPDATA Data = *(PUSHTREETOPPDATA*)pArg;
	m_LifeTime = Data.LifeTime;

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	m_Type = (_uint)OBJECT_TYPE::OBJECT;


	wstring ColliderName = L"Rigid";
	m_pTransformCom->Set_State(STATE_POSITION, _Vector4(Data.Position, 1.f));
	if (Data.Type == CTreetop::GRASS)
	{
		if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Model_Grass_Clipping"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		//얘는 콜라이더 안달거임 왜냐? 쓸모없거든~
	}
	else
	{
		if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Model_Bush_Clipping"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		m_pRigid->Create_BoxCollider(ColliderName, _Vector3(0.5f, 0.5f, 0.5f), _Vector3(0.f, 0.5f, 0.f));
		m_pRigid->Set_ActorLocalPos(_Vector3(0.f, 0.5f, 0.f));
		m_pRigid->Set_Material(ColliderName, 0.7f, 0.7f, 1.f);//작게튕기니까 이게맞을듯~
		m_pRigid->Attach_Shape();
	}

	m_pRigid->Set_Host(this);
	m_pRigid->Set_Transform(m_pTransformCom->Get_WorldMatrix());
	m_pRigid->Set_Gravity(true);
	m_pRigid->SetUp_Filtering(tagFilterGroup::OBJECT);
	m_pRigid->Set_Mess(0.1f);
	m_pRigid->Set_isQuery(ColliderName, false);

	m_pRigid->Add_Force(Data.Force , CRigid::FORCEMODE::IMPULSE);
	m_pRigid->Add_Torque(Data.Torque, CRigid::FORCEMODE::IMPULSE);
	return S_OK;
}


_int CTreetop::Tick(_float fTimeDelta)
{
	m_LifeTime -= fTimeDelta;

	if (m_LifeTime < 0.f)
		m_bDead = true;

	if (m_LifeTime <= 0.5f)//이렇게해도 이쁘긴할듯?
		m_pTransformCom->Set_Scale(_Vector3(m_LifeTime * 2.f, m_LifeTime * 2.f, m_LifeTime * 2.f));

	return _int();
}

_int CTreetop::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;


	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this)))
		return 0;
	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DYNAMICSHADOW, this)))
		return 0;


	return _int();
}

HRESULT CTreetop::Render(_uint iRenderGroup)
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
			m_pModelCom->Set_ShaderResourceView(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS);

			m_pModelCom->Render(m_pShaderCom, i, 0);
		}
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

// HRESULT CTreetop::Render_Shadow()
// {
// 	if (FAILED(SetUp_ConstantTable()))
// 		return E_FAIL;
// 
// 	_uint	iNumMaterials = m_pModelCom->Get_NumMaterials();
// 
// 	for (_uint i = 0; i < iNumMaterials; ++i)
// 	{
// 		m_pModelCom->Render(i, 2);
// 	}
// 
// 	return S_OK;
// }

HRESULT CTreetop::SetUp_Component()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component_STATIC(TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component_STATIC(TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;


	if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Rigid"), TEXT("Com_RigidBody"), (CComponent**)&m_pRigid)))
		return E_FAIL;

	/* For.Com_Shader*/
	if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Shader_Mesh"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTreetop::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
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


CTreetop* CTreetop::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CTreetop* pInstance = new CTreetop(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CTreetop");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CTreetop::Clone(void* pArg)
{
	CTreetop* pInstance = new CTreetop(*this);


	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CTreetop");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTreetop::Free()
{

	__super::Free();
	// 	Safe_Release(m_pTransformCom);
	// 	Safe_Release(m_pShaderCom);	
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRigid);
}
