#include "stdafx.h"
#include "..\public\SpecialChest.h"

#include "GameInstance.h"
#include "Inventory.h"
#include "Player.h"
#include "UI_Item.h"
CSpecialChest::CSpecialChest(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CSpecialChest::CSpecialChest(const CSpecialChest& rhs)
	: CGameObject(rhs)
{
}

HRESULT CSpecialChest::NativeConstruct_Prototype()
{

	return S_OK;
}

HRESULT CSpecialChest::NativeConstruct(void* pArg)
{
	if (FAILED(SetUp_Component()))
		return E_FAIL;

	if (pArg)
	{
		PUSHSPECIALCHESTDATA Data = *(PUSHSPECIALCHESTDATA*)pArg;
		m_pTransformCom->Set_State(STATE_POSITION, _Vector4(Data.Position, 1.f));
		m_pTransformCom->Rotation(_Vector3(0.f, 1.f, 0.f), XMConvertToRadians(Data.YRotate));
		m_ItemType = Data.ItemType;
		m_NumItem = Data.NumItem;
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

_int CSpecialChest::Tick(_float fTimeDelta)
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
			CInventory* pInventory = GET_INSTANCE(CInventory);
			CItem* Weapon = nullptr;
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			switch (m_ItemType)
			{
			case Client::CSpecialChest::FIRECRACKER:
			case Client::CSpecialChest::FIREBOTTLE:
			case Client::CSpecialChest::REDFRUIT:
			case Client::CSpecialChest::BLUEFRUIT:
			case Client::CSpecialChest::HERB:
			case Client::CSpecialChest::BAIT:
			case Client::CSpecialChest::COIN:
			case Client::CSpecialChest::ICEBOTTLE:
			case Client::CSpecialChest::PIGGYBANK:
			case Client::CSpecialChest::PEPPER:
				pInventory->Add_ConsumableItem(m_ItemType, m_NumItem);
				break;
			case Client::CSpecialChest::POTION:
				pInventory->Add_PotionMax(1);
				break;
			case Client::CSpecialChest::STICK:
				Weapon = (CItem*)pGameInstance->Add_GameObjectToLayerAndReturn(L"Layer_Item", TEXT("Prototype_GameObject_Stick"), nullptr, SORTLAYER_OBJ);
				pInventory->Add_Weapon(Weapon);
				pGameInstance->Add_ObjToDontDestroyLayer(L"Layer_Item", TEXT("Prototype_GameObject_Stick"), Weapon);
				break;
			case Client::CSpecialChest::SWORD:
				Weapon = (CItem*)pGameInstance->Add_GameObjectToLayerAndReturn(L"Layer_Item", TEXT("Prototype_GameObject_Sword"), nullptr, SORTLAYER_OBJ);
				pInventory->Add_Weapon(Weapon);
				pGameInstance->Add_ObjToDontDestroyLayer(L"Layer_Item", TEXT("Prototype_GameObject_Sword"), Weapon);
				break;
			case Client::CSpecialChest::ICEDAGGER:
				Weapon = (CItem*)pGameInstance->Add_GameObjectToLayerAndReturn(L"Layer_Item", TEXT("Prototype_GameObject_IceDagger"), nullptr, SORTLAYER_OBJ);
				pInventory->Add_Weapon(Weapon);
				pGameInstance->Add_ObjToDontDestroyLayer(L"Layer_Item", TEXT("Prototype_GameObject_IceDagger"), Weapon);
				break;
			case Client::CSpecialChest::WAND:
				Weapon = (CItem*)pGameInstance->Add_GameObjectToLayerAndReturn(L"Layer_Item", TEXT("Prototype_GameObject_Wand"), nullptr, SORTLAYER_OBJ);
				pInventory->Add_Weapon(Weapon);
				pGameInstance->Add_ObjToDontDestroyLayer(L"Layer_Item", TEXT("Prototype_GameObject_Wand"), Weapon);
				break;
			case Client::CSpecialChest::TETHERWAND:
				Weapon = (CItem*)pGameInstance->Add_GameObjectToLayerAndReturn(L"Layer_Item", TEXT("Prototype_GameObject_TetherWand"), nullptr, SORTLAYER_OBJ);
				pInventory->Add_Weapon(Weapon);
				pGameInstance->Add_ObjToDontDestroyLayer(L"Layer_Item", TEXT("Prototype_GameObject_TetherWand"), Weapon);
				break;
			case Client::CSpecialChest::CAPE:
				pInventory->Add_Geer(GEAR_TYPE::CAPE);
				break;
			case Client::CSpecialChest::SHIELD:
				pInventory->Add_Geer(GEAR_TYPE::SHIELD);
				break;
			case Client::CSpecialChest::CHARM_CONVERTPOTION:
				pInventory->Add_Charm(CHARM_TYPE::CONVERTPOTION);
				break;
			case Client::CSpecialChest::CHARM_DROPHP:
				pInventory->Add_Charm(CHARM_TYPE::DROPHP);
				break;
			case Client::CSpecialChest::CHARM_ROCKETWAND:
				pInventory->Add_Charm(CHARM_TYPE::ROCKETWAND);
				break;
			case Client::CSpecialChest::POTIONPIECE:
				pInventory->Add_PotionPiece(m_NumItem);
				break;
			default:
				break;
			}
			CUI_Item::PUSHUIITEMDATA Data;
			Data.iItemNum = m_ItemType;
			Data.iItemCnt = m_NumItem;
			static_cast<CStateMachine*>(pGameInstance->Get_ComponentPtr(L"Layer_Player", L"Com_StateMachine"))->SetCurrentState(CPlayer::STATE_GETITEM, &Data);
			RELEASE_INSTANCE(CGameInstance);
			RELEASE_INSTANCE(CInventory);
			m_ChestOpen = true;
		}
		if (m_ChestOpen)
		{
		}
	}
	return _int();
}

_int CSpecialChest::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;


	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this)))
		return 0;
	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DYNAMICSHADOW, this)))
		return 0;




	return _int();
}

HRESULT CSpecialChest::Render(_uint iRenderGroup)
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

HRESULT CSpecialChest::SetUp_Component()
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

HRESULT CSpecialChest::SetUp_ConstantTable()
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


CSpecialChest* CSpecialChest::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CSpecialChest* pInstance = new CSpecialChest(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CSpecialChest");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CSpecialChest::Clone(void* pArg)
{
	CSpecialChest* pInstance = new CSpecialChest(*this);


	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CSpecialChest");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSpecialChest::Free()
{
	__super::Free();
	// 	Safe_Release(m_pTransformCom);
	// 	Safe_Release(m_pShaderCom);
	Safe_Release(m_pAnimatorCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRigid);
}
