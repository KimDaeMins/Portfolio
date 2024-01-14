#include "stdafx.h"
#include "..\public\Chest.h"

#include "GameInstance.h"
CChest::CChest(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CChest::CChest(const CChest& rhs)
	: CGameObject(rhs)
{
}

HRESULT CChest::NativeConstruct_Prototype()
{

	return S_OK;
}

HRESULT CChest::NativeConstruct(void* pArg)
{
	if (FAILED(SetUp_Component()))
		return E_FAIL;

	if (pArg)
	{
		PUSHCHESTDATA Data = *(PUSHCHESTDATA*)pArg;
		m_pTransformCom->Set_State(STATE_POSITION, _Vector4(Data.Position, 1.f));
		m_pTransformCom->Rotation(_Vector3(0.f, 1.f, 0.f), XMConvertToRadians(Data.YRotate));
		m_NumCoin = Data.Gold;
	}
	else
	{
		m_pTransformCom->Set_State(STATE_POSITION, XMVectorSet(_float(rand() % 20), 0.f, _float(rand() % 20), 1.f));
	}

	m_Type = (_uint)OBJECT_TYPE::CHEST;

	m_pModelCom->Set_Animator(m_pAnimatorCom);

	CRigid::PUSHRIGIDDATA Data;
	Data.StaticRigid = true;
	if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Rigid"), TEXT("Com_RigidBody"), (CComponent**)&m_pRigid, &Data)))
		return E_FAIL;

	wstring ColliderName = L"Rigid";
	wstring Trigger = L"Trigger";
	m_pRigid->Set_Host(this);
	m_pRigid->Create_BoxCollider(ColliderName, _Vector3(1.f, 2.f, 0.7f), _Vector3(0.f, 2.f, 0.f));
	m_pRigid->Create_BoxCollider(Trigger, _Vector3(0.5f, 2.f, 0.5f), _Vector3(0.f, 2.f, -1.2f));
	m_pRigid->Set_isSimulation(Trigger, false);
	m_pRigid->Set_isTrigger(Trigger, true);
	m_pRigid->Attach_Shape();
	m_pRigid->SetUp_Filtering(tagFilterGroup::OBJECT);
	m_pRigid->Set_Transform(m_pTransformCom->Get_WorldMatrix());
	m_pRigid->Set_isQuery(ColliderName, false);

	m_pAnimatorCom->SetUp_Animation(0, false);

	return S_OK;
}

_int CChest::Tick(_float fTimeDelta)
{
	CGameObject* GameObject = nullptr;
	if (!AnimStart)
	{
		while (m_pRigid->Get_Collision_Trigger_Stay(&GameObject))
		{
			if (GameObject->Get_Type() == (_uint)OBJECT_TYPE::PLAYER)
			{
				//플레이어의 현재애니메이션이 상자까는 애니메이션이라면?
				if (((CAnimator*)GameObject->Get_ComponentPtr(L"Com_Animator"))->Get_CurrentAnimationIndex() == 28)
				{
					AnimStart = true;
					//내 애니메이션을 바꿔주고 펄스로

					//m_pAnimatorCom->SetUp_Animation(1, true);
					wstring Trigger = L"Trigger";
					m_pRigid->Detach_Shape(Trigger);
				}
			}
		}
	}

	//상호작용이 시작됐다면?
	if (AnimStart)
	{
		m_pAnimatorCom->Update_Animation(fTimeDelta);
		if (!m_ChestOpen && m_pAnimatorCom->Get_CurKeyFrame() >= 30)
		{
			m_ChestOpen = true;
			m_Div = m_NumCoin / 50;
			
			if (m_Div == 0)
				m_Div = 1;

			_uint		iMaxMediumCoin = m_Div * 2;
			_uint		iMaxBigCoin = m_Div;
			_uint		iCurMediumCoin = 0, iCurBigCoin = 0;

			for (_uint i = 0; i < m_Div * 7; ++i)
			{
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				_uint		iRandom = pGameInstance->Random_Range(0, 10);
				if (2 > iRandom && iMaxBigCoin > iCurBigCoin)
				{
					m_CoinTable.push_back(CCoin::COINTYPE::BIG);
					++iCurBigCoin;
				}
				else if (5 > iRandom && iMaxMediumCoin > iCurMediumCoin)
				{
					m_CoinTable.push_back(CCoin::COINTYPE::MEDIUM);
					++iCurMediumCoin;
				}
				else
				{
					m_CoinTable.push_back(CCoin::COINTYPE::SMALL);
				}
				RELEASE_INSTANCE(CGameInstance);
			}
			_uint i = m_Div * 7 - 1;
			while (iMaxBigCoin > iCurBigCoin)
			{
				if (CCoin::COINTYPE::SMALL == m_CoinTable[i])
				{
					m_CoinTable[i] = CCoin::COINTYPE::BIG;
					++iCurBigCoin;
				}
				--i;

				if (0 == i)
					break;
			}
			while (iMaxMediumCoin > iCurMediumCoin)
			{
				if (CCoin::COINTYPE::SMALL == m_CoinTable[i])
				{
					m_CoinTable[i] = CCoin::COINTYPE::MEDIUM;
					++iCurMediumCoin;
				}
				--i;

				if (0 == i)
					break;
			}

			m_NumCoin = m_Div * 7 - 1;
			m_Angle /= m_NumCoin;
		}
		if (m_ChestOpen)
		{
			if(m_NumCoin > 0)
			{
				_Vector3 Force = _Vector3(0.2f, 1.5f, 0.f);
				_Matrix RotationMatrix = _Matrix::CreateFromAxisAngle(_Vector3(0.f, 1.f, 0.f), (_float)XMConvertToRadians(m_Angle * m_NumCoin));

				Force = _Vector3::Transform(Force, RotationMatrix);
				_Vector3 Position = m_pTransformCom->Get_State(STATE_POSITION);

				CCoin::PUSHCOINDATA Data;
				Data.Force = Force;
				Data.Position = Position;
				Data.eCoinType = m_CoinTable[--m_NumCoin];
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				pGameInstance->Add_GameObjectToLayer(TEXT("Layer_Object"), TEXT("Prototype_GameObject_Coin"), &Data);
				RELEASE_INSTANCE(CGameInstance);
			}
		}
	}
	return _int();
}

_int CChest::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;


		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this)))
			return 0;
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DYNAMICSHADOW, this)))
			return 0;




	return _int();
}

HRESULT CChest::Render(_uint iRenderGroup)
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
	}

	else if (CRenderer::RENDER_DYNAMICSHADOW == iRenderGroup)
	{
		if (FAILED(SetUp_ConstantTable()))
			return E_FAIL;

		_uint	iNumMaterials = m_pModelCom->Get_NumMaterials();

		for (_uint i = 0; i < iNumMaterials; ++i)
		{
			m_pModelCom->Render(m_pShaderCom, i, 1);
		}
	}


	return S_OK;
}

HRESULT CChest::SetUp_Component()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component_STATIC(TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component_STATIC(TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Model_Chest"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;
	/* For.Com_Animator */
	if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Animator"), TEXT("Com_Animator"), (CComponent**)&m_pAnimatorCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Shader_AnimMesh"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CChest::SetUp_ConstantTable()
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


CChest* CChest::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CChest* pInstance = new CChest(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CChest");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CChest::Clone(void* pArg)
{
	CChest* pInstance = new CChest(*this);


	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CChest");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChest::Free()
{
	__super::Free();
	// 	Safe_Release(m_pTransformCom);
	// 	Safe_Release(m_pShaderCom);
	Safe_Release(m_pAnimatorCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRigid);
}
