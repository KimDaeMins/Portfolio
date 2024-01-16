#include "stdafx.h"
#include "WaveAltar.h"
#include "GameInstance.h"
#include "WaveAltarFlame.h"
#include "WaveManager.h"
#include "Monster.h"
#include "Camera_Fly.h"
CWaveAltar::CWaveAltar(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CWaveAltar::CWaveAltar(const CWaveAltar& rhs)
	: CGameObject(rhs)
{
}

HRESULT CWaveAltar::NativeConstruct_Prototype()
{
    return S_OK;
}

HRESULT CWaveAltar::NativeConstruct(void* pArg)
{
	PUSHWAVEALTARDATA Data = *(PUSHWAVEALTARDATA*)pArg;

	if (FAILED(SetUp_Component(_uint(Data.Type))))
		return E_FAIL;

	Create_Status((_uint)DAMEGETYPE_TYPE::NORMAL, 0, 100.f, 100.f);

	m_SpawnType = Data.Type;

	m_pTransformCom->Set_State(STATE_POSITION, _Vector3(Data.Position));
	m_pTransformCom->Rotation(_Vector3(0.f, 1.f, 0.f), XMConvertToRadians(Data.YRotate));
	m_pTransformCom->Set_Scale(Data.Scale);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CWaveAltarFlame::PUSHFLAMEDATA FlamePushData;

	_Vector3 Position = m_pTransformCom->Get_State(STATE_POSITION);
	Position.y += 2.1f;

	FlamePushData.iType = 1;
	FlamePushData.vScale = Vector3(1.f, 1.f, 1.f);
	FlamePushData.vPosition = Position;

	m_pAltarFlame = pGameInstance->Add_GameObjectToLayerAndReturn(L"Layer_WaveAltarBurn", L"Prototype_GameObject_WaveAltarFlame", &FlamePushData);
	RELEASE_INSTANCE(CGameInstance);

	m_pRigid->Set_Host(this);
	//유니티파싱떄 넣어줄것 로테이트,포지션
// 	wstring ColliderName = L"Stair";
// 	m_pRigid->Create_BoxCollider(ColliderName, _Vector3(1.f, 2.f, 1.f), _Vector3(0.f, 2.f, 0.f));
// 	m_pRigid->SetUp_Filtering(tagFilterGroup::OBJECT);	
// 	wstring ColliderName = L"Candle";
// 	m_pRigid->Create_SphereCollider(ColliderName, 0.5f, _Vector3(0.f, 0.5f, 0.f));
// 	m_pRigid->SetUp_Filtering(tagFilterGroup::OBJECT);


	m_pRigid->Set_Transform(m_pTransformCom->Get_WorldMatrix());
	m_pRigid->Attach_Shape();
    return S_OK;
}

_int CWaveAltar::Tick(_float fTimeDelta)
{
	if (!m_bIsComplite)
	{
		if (!m_StartWave)
		{
			if (static_cast<CWaveAltarFlame*>(m_pAltarFlame)->Get_Burn() && m_Burn == false)
			{
				m_Burn = true;
				//타는 코드		
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				pGameInstance->Play(L"gen_prop_int_cathedralArena_lightCandleSequence_01", MULTY, 3, 1);
				RELEASE_INSTANCE(CGameInstance);
				Set_Dissolve(3.f);
			}
			else if(static_cast<CWaveAltarFlame*>(m_pAltarFlame)->Get_Burn() && Get_DissolveEnd())
			{
				m_StartWave = true;		
				m_Burn = false;
				m_pRigid->Detach_Shape();

				m_bIsStartTrigger = ON;
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				pGameInstance->StopSound(SINGLE, BGMSOUND1);
				pGameInstance->Play(L"MST_mus_gauntlet_main_intro", SINGLE, BGMSOUND1);
				pGameInstance->VolumeDown(SINGLE, BGMSOUND1, 0.3f);

				// 카메라 제어
				CCamera_Fly* pCamFly = static_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(TEXT("Layer_Camera")));
				pCamFly->Set_PlayerTargetCamera();
				pCamFly->Set_TargetDistOffset(0.f);
				RELEASE_INSTANCE(CGameInstance);
			}
		}
		else
		{
			CWaveManager* pWaveManager = GET_INSTANCE(CWaveManager);
			if (pWaveManager->Tick(fTimeDelta))
			{
				if (pWaveManager->Get_WaveEnd())
				{
					m_bIsComplite = true;
					CreateReward();

					CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
					if (m_SpawnType == CWaveAltar::POTION)
						pGameInstance->Play(L"gen_prop_int_cathedralArena_healthAppear_", MULTY, 3, 3);
					else
					{
						pGameInstance->StopSound(SINGLE, BGMSOUND1);
						pGameInstance->PlayBGM(L"MST_mus_gauntlet_chill_loop", BGMSOUND1);
						pGameInstance->Play(L"gen_prop_int_cathedralArena_waveCompleted", MULTY, 3, 1);
						pGameInstance->VolumeDown(SINGLE, BGMSOUND1, 0.3f);
					}
					RELEASE_INSTANCE(CGameInstance);
				}
			}
		}
	}

	if (m_StartWave && !m_bIsComplite && m_SpawnType != CWaveAltar::POTION)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		if (!pGameInstance->GetIsPlaying(SINGLE, BGMSOUND1))
		{
			pGameInstance->Play(L"MST_mus_gauntlet_main_loop", SINGLE, BGMSOUND1);
			pGameInstance->VolumeDown(SINGLE, BGMSOUND1, 0.3f);
		}
		RELEASE_INSTANCE(CGameInstance);
	}

	if(m_bIsStartTrigger == ON)
		m_fTimeAcc += fTimeDelta;
	if(m_fTimeAcc >= 2.f)
	{
		WaveStart();
		m_fTimeAcc = 0.f;
		m_bIsStartTrigger = OFF;
	}

	DissolveProcess(fTimeDelta);
    return _int();
}

_int CWaveAltar::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this)))
		return 0;

	return _int();
}

HRESULT CWaveAltar::Render(_uint iRenderGroup)
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
			// 디졸브용 텍스처 6장 로드해놧다. Textures/Dissolve 확인
			// 텍스처 컴포넌트 만들어서 쉐이더에 던지기용으로 만든다. ( Proto는 Dissolve 해논거있음 0~5번 쓰셈 이미지는 )
			// AnimMesh 기준 Pass 27번이고 던질 값은 m_fSliceAmount 이다. 이거는 그냥 던지셈 사용법1에서 알아서 값 바뀜
			// Pass : AnimMesh(27), AnimMeshEffect(12) Mesh(16), MeshEffect(19), Rect(20)
			m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_EffectTexture", 6);
			m_pShaderCom->Set_RawValue("g_fAlpha", &m_fSliceAmount, sizeof(_float));
			if(i == 0)
				m_pModelCom->Render(m_pShaderCom, i, 16);			
			else
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

_bool CWaveAltar::WaveStart()
{
	CWaveManager* pWaveManager = GET_INSTANCE(CWaveManager);
	if (!pWaveManager->Get_WaveEnd())
		return false;
	switch (m_SpawnType)
	{
	case Client::CWaveAltar::KNIGHT:
	{
		WAVEUSERDATA tData;
		ZeroMemory(&tData, sizeof(tData));
		tData.iModelType = 0;
		tData.fDamage = 30.f;
		tData.fHp = 140.f;
		pWaveManager->Add_Stage(tData, TEXT("Prototype_GameObject_TechKnight"), 1, 0, 1);
		break;
	}
	case Client::CWaveAltar::FROG:
	{
		WAVEUSERDATA tData;
		ZeroMemory(&tData, sizeof(tData));
		tData.iModelType = 0;
		tData.fDamage = 5.f;
		tData.fHp = 30.f;

		pWaveManager->Add_Stage(tData, TEXT("Prototype_GameObject_Frog"), 2, 3.1f, 1);
		ZeroMemory(&tData, sizeof(tData));
		tData.iModelType = 0;
		tData.fDamage = 5.f;
		tData.fHp = 30.f;

		pWaveManager->Add_Stage(tData, TEXT("Prototype_GameObject_FrogSpear"), 1, 0, 1);
		break;
	}
	case Client::CWaveAltar::COWBOT:
	{
		WAVEUSERDATA tData;
		ZeroMemory(&tData, sizeof(tData));
		tData.iModelType = 1;
		tData.fDamage = 5.f;
		tData.fHp = 40.f;
		pWaveManager->Add_Stage(tData, TEXT("Prototype_GameObject_CowBot"), 5, 2.1f, 1);
		pWaveManager->Add_Stage(tData, TEXT("Prototype_GameObject_CowBot"), SPAWN_POSITION_SHAPE::DIAMOND);
		pWaveManager->Add_Stage(tData, TEXT("Prototype_GameObject_CowBot"), SPAWN_POSITION_SHAPE::CYCLE);

		tData.iModelType = 0;
		tData.fDamage = 5.f;
		tData.fHp = 40.f;
		pWaveManager->Add_Stage(tData, TEXT("Prototype_GameObject_CowBot"), SPAWN_POSITION_SHAPE::RECTANGLE);

		tData.iModelType = 2;
		tData.fDamage = 5.f;
		tData.fHp = 100.f;
		pWaveManager->Add_Stage(tData, TEXT("Prototype_GameObject_CowBot"), 1, 5.f, 1);
		break;
	}
	case Client::CWaveAltar::POTION: pWaveManager->Add_ItemStage(77, 0.2f, 1);
		break;
	case Client::CWaveAltar::WIZARD:
	{
		WAVEUSERDATA tData;
		ZeroMemory(&tData, sizeof(tData));
		tData.iModelType = 1;
		tData.fDamage = 5.f;
		tData.fHp = 30.f;

		pWaveManager->Add_Stage(tData, TEXT("Prototype_GameObject_Wizard"), 8, 0.2f, 1);
		break;
	}
	case Client::CWaveAltar::SKULL:
	{
		WAVEUSERDATA tData;
		ZeroMemory(&tData, sizeof(tData));
		tData.iModelType = 0;
		tData.fDamage = 5.f;
		tData.fHp = 30.f;

		pWaveManager->Add_Stage(tData, TEXT("Prototype_GameObject_Skeleton"), 5, 2.1f, 1);

		tData.iModelType = 1;
		tData.fDamage = 100.f;
		tData.fHp = 70.f;
		pWaveManager->Add_Stage(tData, TEXT("Prototype_GameObject_Skeleton"), 1, 3.f, 1);
		break;
	}
	case Client::CWaveAltar::PROBE:
	{
		WAVEUSERDATA tData;
		ZeroMemory(&tData, sizeof(tData));
		tData.fDamage = 5.f;
		tData.fHp = 10.f;

		pWaveManager->Add_Stage(TEXT("Prototype_GameObject_FairyProbe"), 7, 2.1f, 1);
		break;
	}
	default:
		break;
	}
	pWaveManager->Start();
	RELEASE_INSTANCE(CWaveManager);

	return true;
}

void CWaveAltar::CreateReward()
{
	CWaveManager* pWaveManager = GET_INSTANCE(CWaveManager);
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	switch (m_SpawnType)
	{
	case Client::CWaveAltar::KNIGHT:
		pGameInstance->Add_GameObjectToLayer(L"Layer_Reward", L"Prototype_GameObject_TechKinghtReward");
		break;
	case Client::CWaveAltar::FROG:
		break;
	case Client::CWaveAltar::COWBOT:
		break;
	case Client::CWaveAltar::POTION:
		break;
	case Client::CWaveAltar::WIZARD:
		break;
	case Client::CWaveAltar::SKULL:
		break;
	case Client::CWaveAltar::PROBE:
		break;
	default:
		break;
	}
	RELEASE_INSTANCE(CWaveManager);
	RELEASE_INSTANCE(CGameInstance);
}

CWaveAltar* CWaveAltar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CWaveAltar* pInstance = new CWaveAltar(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CWavealtar");
		Safe_Release(pInstance);
	}

	return pInstance;
}

HRESULT CWaveAltar::SetUp_Component(_uint iModelType)
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component_STATIC(TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;
	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component_STATIC(TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;
	/* For.Com_Model*/

	switch (iModelType)
	{
	case 0:
	{

		if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Model_WaveAlter_Knight"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	}
	case 1:
	{
		if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Model_WaveAlter_Frog"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	}
	case 2:
	{
		if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Model_WaveAlter_CowBot"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	}
	case 3:
	{
		if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Model_WaveAlter_Postion"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	}
	case 4:
	{
		if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Model_WaveAlter_Wizard"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	}
	case 5:
	{
		if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Model_WaveAlter_Skull"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	}
	case 6:
	{
		if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Model_WaveAlter_Probe"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	}
	default:
		break;
	}

	/* For.Com_Shader*/
	if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Shader_Mesh"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	CRigid::PUSHRIGIDDATA Data;
	Data.StaticRigid = true;
	if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Rigid"), TEXT("Com_RigidBody"), (CComponent**)&m_pRigid, &Data)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Texture_Dissolve"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

    return S_OK;
}

HRESULT CWaveAltar::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix");
	pGameInstance->Bind_Transform_OnShader(TS_VIEW, m_pShaderCom, "g_ViewMatrix");
	pGameInstance->Bind_Transform_OnShader(TS_PROJ, m_pShaderCom, "g_ProjMatrix");


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CGameObject* CWaveAltar::Clone(void* pArg)
{
	CWaveAltar* pInstance = new CWaveAltar(*this);


	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CWavealtar");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWaveAltar::Free()
{
	__super::Free();
	// 	Safe_Release(m_pTransformCom);
	// 	Safe_Release(m_pShaderCom);	
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRigid);
	Safe_Release(m_pTextureCom);
}
