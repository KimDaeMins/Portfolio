#include "stdafx.h"
#include "..\public\Level_Librarian.h"
#include "Level_Loading.h"
#include "GameInstance.h"
#include "Camera_Fly.h"
#include "PlayerHexBar.h"
#include "Chest.h"
#include "MonsterHpBar.h"
#include "TabUI.h"
#include "TabUI_CharmSelect.h"
#include "UI_Boss_Announce.h"
#include "DragAbleObject.h"
#include "Ladder.h"
#include "Librarian.h"
CLevel_Librarian::CLevel_Librarian(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CLevel(pDevice, pDeviceContext)
{
}

HRESULT CLevel_Librarian::NativeConstruct()
{
	if (FAILED(Ready_Layer_UI_Monster_Hp(TEXT("Layer_UI_Monster_Hp"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_PoolingObjects()))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI_Boss_Hp(TEXT("Layer_UI_Boss_Hp"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_ParticleSystem(TEXT("Layer_ParticleSystem"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_LightDesc()))
		return E_FAIL;

	if (FAILED(Ready_Renderer()))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

// 	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
// 		return E_FAIL;
// 
// 	if (FAILED(Ready_Layer_UI_Player_Status(TEXT("Layer_UI_Player_Status"))))
// 		return E_FAIL;
// 
// 	if (FAILED(Ready_Layer_UI_Inventory(TEXT("Layer_UI_Inventory"))))
// 		return E_FAIL;

	//	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
	//		return E_FAIL;

	if (FAILED(Ready_Layer_Boss(TEXT("Layer_Boss"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Map(TEXT("Layer_Map"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Effect_ATZThunder(TEXT("Layer_Effect_ATZThunder"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Object(TEXT("Layer_Object"))))
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	pGameInstance->Toggle_Fading_Level();
	pGameInstance->StopAll();

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

_int CLevel_Librarian::Tick(_float fTimeDelta)
{
	if (0 > (__super::Tick(fTimeDelta)))
		return -1;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	pGameInstance->Set_ActiveDOF(OFF);

	CCamera_Fly* pCam = (CCamera_Fly*)pGameInstance->Get_GameObject(TEXT("Layer_Camera"));

	if (true == pGameInstance->Get_FadeInEnd() && m_bOn == false)
	{
		//pCam->Set_BossCinematicCam();
		m_bOn = true;
	}


	if (pGameInstance->Key_Down(DIK_END))
	{
		pGameInstance->Set_Fading_Level(true, 1.f);
	}

	if (true == pGameInstance->Get_FadeOutEnd_Level())
	{
		if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pDeviceContext, LEVEL_SPIDER))))
			return -1;
	}

	if (false == m_bCinematicStart)
	{
		// 시네마틱 체크
		CTransform* pPlayerTransform = static_cast<CTransform*>(pGameInstance->Get_ComponentPtr(TEXT("Layer_Player"), TEXT("Com_Transform")));
		if (nullptr != pPlayerTransform && -8.f < XMVectorGetZ(pPlayerTransform->Get_State(STATE_POSITION)))
		{
			pCam->Set_LibrarianCinematicCam();
			m_bCinematicStart = true;
		}
	}
	else if (false == m_bCinematicEnd)
	{
		m_fCinematicTimeAcc += fTimeDelta;
		if (false == m_bLibrarianTurn && 6.5f < m_fCinematicTimeAcc)
		{
			// set librarian turn
			CLibrarian* pLibrarian = (CLibrarian*)pGameInstance->Get_GameObject(TEXT("Layer_Boss"));
			if (nullptr != pLibrarian)
				pLibrarian->Set_CinematicTurn();
			m_bLibrarianTurn = true;
		}

		if (true == m_bLibrarianTurn && 10.f < m_fCinematicTimeAcc)
		{
			// set librarian attack
			CLibrarian* pLibrarian = (CLibrarian*)pGameInstance->Get_GameObject(TEXT("Layer_Boss"));
			if (nullptr != pLibrarian)
			{
				pLibrarian->Begin_InitAttack();
				// set bosscam
				pCam->Set_BossSpinCamera(75.f, true);
				pCam->Set_DegreeX(30.f);
				pCam->Set_TargetDist(100.f);
				pCam->Set_DegreeXFocusOffset(0.f);
				pCam->Set_StaticDegreeYOffset(0.f);
			}
			m_bCinematicEnd = true;
		}
	}

	RELEASE_INSTANCE(CGameInstance);

	return _int(0);
}

HRESULT CLevel_Librarian::Render()
{
	SetWindowText(g_hWnd, TEXT("Librarian Test Level"));

	return S_OK;
}

HRESULT CLevel_Librarian::Ready_LightDesc()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	pGameInstance->Clear_Light();

	LIGHTDESC            LightDesc;
	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

	LightDesc.eType = tagLightDesc::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float3(1.f, 1.f, -1.f);

	LightDesc.vPosition = _float3(3.f, -2.f, 3.f);

	LightDesc.vDiffuse = _float4(0.2f, 0.2f, 0.2f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(0.f, 0.15f, 0.1f, 1.f);

	LightDesc.fFarRadius = 200.f;

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;

	LightDesc.eType = tagLightDesc::TYPE_POINT;
	LightDesc.vPosition = _float3(20.0f, -20.0f, 0.f);
	LightDesc.fRadiuse = 100.f;

	LightDesc.vDiffuse = _float4(0.f, 0.2f, 0.1f, 1.f);
	LightDesc.vAmbient = _float4(0.f, 0.f, 0.f, 1.f);
	LightDesc.vSpecular = _float4(0.f, 0.f, 0.f, 1.f);

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;

	LightDesc.vPosition = _float3(-20.0f, -20.0f, 0.f);
	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;

	LightDesc.vPosition = _float3(0.f, -20.0f, 20.f);
	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;

	LightDesc.vPosition = _float3(0.f, -20.0f, -20.f);
	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Librarian::Ready_Renderer()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// renderer 값 조절 테스트

	pGameInstance->Set_DOFFocusDistance(25.f);
	pGameInstance->Set_DOFFocusRange(280.f);
	pGameInstance->Set_HDRMiddleGrey(0.72f);
	pGameInstance->Set_ActiveDOF(OFF);
	pGameInstance->Set_ActiveHDR(ON);
	pGameInstance->Set_ActiveSSAO(ON);
	pGameInstance->Set_CurrentLUTTag(TEXT("Daytime_vivid"));

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Librarian::Ready_Layer_Camera(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/* 월드스페이스 상에서의 카메라 상태를 셋팅하자. */
	CCamera::CAMERADESC			CameraDesc;
	ZeroMemory(&CameraDesc, sizeof(CCamera::CAMERADESC));

	CameraDesc.vEye = _float3(0.f, 15.f, -10.0f);
	CameraDesc.vAt = _float3(0.f, 0.f, 0.f);
	CameraDesc.vAxisy = _float3(0.f, 1.f, 0.f);
	CameraDesc.fFovy = XMConvertToRadians(15.0f);
	CameraDesc.fAspect = (_float)g_iWinCX / g_iWinCY;
	CameraDesc.fNear = 0.2f;
	CameraDesc.fFar = 300.f;

	CameraDesc.TransformDesc.fSpeedPerSec = 30.f;
	CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	CCamera_Fly* cam = static_cast<CCamera_Fly*>(pGameInstance->Add_GameObjectToLayerAndReturn(pLayerTag, TEXT("Prototype_GameObject_Camera_Fly"), &CameraDesc));
	pGameInstance->Set_ListenerTransform(cam->Get_Transform());
	pGameInstance->Set_ListenDistance(1000.f);

	cam->Set_TargetDist(100.f); // 타겟으로부터 거리 FOV 줄여놔서 Dist 높음
	cam->Set_StaticDegreeY(45.f); // 보는각도(y축 회전)
	cam->Set_DegreeX(30.f); //얼마나 높이서 볼건지

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Librarian::Ready_Layer_BackGround(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/* For.Terrain */
	if (FAILED(pGameInstance->Add_GameObjectToLayer(pLayerTag, TEXT("Prototype_GameObject_Sky"))))
		return E_FAIL;



	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Librarian::Ready_Layer_Player(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// 	if (FAILED(pGameInstance->Add_GameObjectToLayer(pLayerTag, TEXT("Prototype_GameObject_Player"))))
	// 		return E_FAIL;

	CGameObject* pObj = pGameInstance->Add_GameObjectToLayerAndReturn(pLayerTag, TEXT("Prototype_GameObject_Player"));
	pGameInstance->Add_ObjToDontDestroyLayer(pLayerTag, TEXT("Prototype_GameObject_Player"), pObj);

	// 	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Sword"))))
	// 		return E_FAIL;


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Librarian::Ready_Layer_Map(const _tchar* pLayerTag)
{

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_GameObjectToLayer(pLayerTag, TEXT("Prototype_GameObject_Map"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectToLayer(pLayerTag, TEXT("Prototype_GameObject_StaticWall"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectToLayer(pLayerTag, TEXT("Prototype_GameObject_StaticGround"))))
		return E_FAIL;

	// 	if (FAILED(pGameInstance->Add_GameObjectToLayer(pLayerTag, TEXT("Prototype_GameObject_StaticWall"))))
	// 		return E_FAIL;


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Librarian::Ready_Layer_Monster(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	//	if (FAILED(pGameInstance->Add_GameObjectToLayer(pLayerTag, TEXT("Prototype_GameObject_Scavenger_Rifle"))))
	//		return E_FAIL;
	//	if (FAILED(pGameInstance->Add_GameObjectToLayer(pLayerTag, TEXT("Prototype_GameObject_CowBot"))))
	//		return E_FAIL;
// 	if (FAILED(pGameInstance->Add_GameObjectToLayer(pLayerTag, TEXT("Prototype_GameObject_Bat_Head"))))
// 		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(pLayerTag, TEXT("Prototype_GameObject_Librarian"))))
		return E_FAIL;
	// 	if (FAILED(pGameInstance->Add_GameObjectToLayer(pLayerTag, TEXT("Prototype_GameObject_Bat"))))
	// 		return E_FAIL;
	// 	if (FAILED(pGameInstance->Add_GameObjectToLayer(pLayerTag, TEXT("Prototype_GameObject_Blob"))))
	// 		return E_FAIL;
	// 	if (FAILED(pGameInstance->Add_GameObjectToLayer(pLayerTag, TEXT("Prototype_GameObject_Turret"))))
	// 		return E_FAIL;
		// 	if (FAILED(pGameInstance->Add_GameObjectToLayer(pLayerTag, TEXT("Prototype_GameObject_Frog"))))
	// 		return E_FAIL;
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}
HRESULT CLevel_Librarian::Ready_Layer_Effect(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_Pooling_GameObjectToLayer(pLayerTag, TEXT("Prototype_GameObject_Effect_Shockwave"), 1)))
		return E_FAIL;



	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Librarian::Ready_Layer_Object(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	TRANSFORMINFO Data;

	_uint OBJSize = pGameInstance->Get_TableSize(TEXT("LibrarianObject"), DATATYPE_OBJECT);

	for (_uint i = 1; i < OBJSize + 1; ++i)
	{
		memcpy(&Data, pGameInstance->Get_RowDataToTransform(TEXT("LibrarianObject"), i, DATATYPE_OBJECT), sizeof(TRANSFORMINFO));

		CDragAbleObject::PUSHMONSTERDATA MonsterData;
		MonsterData.Position = { Data.PosX, Data.PosY, Data.PosZ };
		MonsterData.YRotate = Data.RotY;
		if (FAILED(pGameInstance->Add_GameObjectToLayer(L"Layer_Monster", L"Prototype_GameObject_DragAbleObject", &MonsterData)))
			return E_FAIL;
	}


	OBJSize = pGameInstance->Get_TableSize(TEXT("LibrarianLadder"), DATATYPE_OBJECT);

	for (_uint i = 1; i < OBJSize + 1; ++i)
	{
		memcpy(&Data, pGameInstance->Get_RowDataToTransform(TEXT("LibrarianLadder"), i, DATATYPE_OBJECT), sizeof(TRANSFORMINFO));

		CLadderZone::PUSHLADDERPARTDATA LadderData;
		LadderData.Position = { Data.PosX, Data.PosY, Data.PosZ };
		LadderData.Extents = { 1.f, 0.5f, 0.5f };
		LadderData.Up = true;
		LadderData.Rotate = 90.f;
		if (FAILED(pGameInstance->Add_GameObjectToLayer(pLayerTag, L"Prototype_GameObject_LadderZone", &LadderData)))
			return E_FAIL;

		LadderData.Position = { Data.PosX, Data.PosY + Data.ScaleY * 0.5f, Data.PosZ };
		LadderData.Extents = { 1.1f, 0.5f, 0.5f };
		LadderData.Up = false;
		LadderData.Rotate = 90.f;
		if (FAILED(pGameInstance->Add_GameObjectToLayer(pLayerTag, L"Prototype_GameObject_LadderZone", &LadderData)))
			return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);

	if (FAILED(Load_Chest(pLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Librarian::Ready_Layer_UI_Monster_Hp(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CMonsterHpBar::TYPE eType_ForMonster = CMonsterHpBar::TYPE_HEALTH;

	if (FAILED(pGameInstance->Add_Pooling_GameObjectToLayer(pLayerTag, TEXT("Prototype_GameObject_MonsterHexBar"), 20, &eType_ForMonster)))
		return E_FAIL;


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Librarian::Ready_Layer_UI_Boss_Hp(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	BOSSTYPE		BossType = BOSSTYPE::LIBRARIAN;

	if (FAILED(pGameInstance->Add_GameObjectToLayer(pLayerTag, TEXT("Prototype_GameObject_BossHpBar"), &BossType)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Layer_UI_Boss_Announce"), TEXT("Prototype_GameObject_UI_Boss_Announce"), &BossType)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Librarian::Ready_Layer_UI_Player_Status(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	//-------------------------------------
	// 플레이어 BAR 관련 UI
	//-------------------------------------
	CPlayerHexBar::TYPE eType_ForPlayer;

	// TODO : UI LayerOrder == 4
	eType_ForPlayer = CPlayerHexBar::TYPE_HEALTH;
	CGameObject* pObj = pGameInstance->Add_GameObjectToLayerAndReturn(pLayerTag, TEXT("Prototype_GameObject_PlayerHexBar"), &eType_ForPlayer, SORTLAYER_UI, 4);
	if (nullptr == pObj)
		return E_FAIL;
	pGameInstance->Add_ObjToDontDestroyLayer(pLayerTag, TEXT("Prototype_GameObject_PlayerHexBar"), pObj, SORTLAYER_UI);

	eType_ForPlayer = CPlayerHexBar::TYPE_STAMINA;
	pObj = pGameInstance->Add_GameObjectToLayerAndReturn(pLayerTag, TEXT("Prototype_GameObject_PlayerHexBar"), &eType_ForPlayer, SORTLAYER_UI, 4);
	if (nullptr == pObj)
		return E_FAIL;
	pGameInstance->Add_ObjToDontDestroyLayer(pLayerTag, TEXT("Prototype_GameObject_PlayerHexBar"), pObj, SORTLAYER_UI);

	eType_ForPlayer = CPlayerHexBar::TYPE_MAGIC;
	pObj = pGameInstance->Add_GameObjectToLayerAndReturn(pLayerTag, TEXT("Prototype_GameObject_PlayerHexBar"), &eType_ForPlayer, SORTLAYER_UI, 4);
	if (nullptr == pObj)
		return E_FAIL;
	pGameInstance->Add_ObjToDontDestroyLayer(pLayerTag, TEXT("Prototype_GameObject_PlayerHexBar"), pObj, SORTLAYER_UI);


	//-------------------------------------
	// 플레이어 탈진 관련 UI
	//-------------------------------------

	pObj = pGameInstance->Add_GameObjectToLayerAndReturn(pLayerTag, TEXT("Prototype_GameObject_PlayerSweat"));
	if (nullptr == pObj)
		return E_FAIL;
	pGameInstance->Add_ObjToDontDestroyLayer(pLayerTag, TEXT("Prototype_GameObject_PlayerSweat"), pObj);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Librarian::Ready_Layer_UI_Inventory(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// TabUI Index : 0
	// 인덱스 편하게찾으려고 레이어 나눴더니 체력바가 tabui보다 먼저 그려지는 현상, 체력바 깨져보이는현상 등의 문제 있음
	CGameObject* pTabUI = nullptr;
	pTabUI = pGameInstance->Add_GameObjectToLayerAndReturn(pLayerTag, TEXT("Prototype_GameObject_TabUI"), nullptr, SORTLAYER_UI, 5); // TODO : UI LayerOrder == 5
	if (nullptr == pTabUI)
		return E_FAIL;
	pGameInstance->Add_ObjToDontDestroyLayer(pLayerTag, TEXT("Prototype_GameObject_TabUI"), pTabUI, SORTLAYER_UI);

	// 1
	CGameObject* pObj = nullptr;
	pObj = pGameInstance->Add_GameObjectToLayerAndReturn(pLayerTag, TEXT("Prototype_GameObject_TabUI_Equipable"), nullptr, SORTLAYER_UI, 5);
	if (nullptr == pObj)
		return E_FAIL;
	pGameInstance->Add_ObjToDontDestroyLayer(pLayerTag, TEXT("Prototype_GameObject_TabUI_Equipable"), pObj, SORTLAYER_UI);

	static_cast<CTabUI*>(pTabUI)->Set_UI_Equipable(pObj);

	// 2
	pObj = pGameInstance->Add_GameObjectToLayerAndReturn(pLayerTag, TEXT("Prototype_GameObject_TabUI_Consumable"), nullptr, SORTLAYER_UI, 5);
	if (nullptr == pObj)
		return E_FAIL;
	pGameInstance->Add_ObjToDontDestroyLayer(pLayerTag, TEXT("Prototype_GameObject_TabUI_Consumable"), pObj, SORTLAYER_UI);

	static_cast<CTabUI*>(pTabUI)->Set_UI_Consumable(pObj);

	// 3
	pObj = pGameInstance->Add_GameObjectToLayerAndReturn(pLayerTag, TEXT("Prototype_GameObject_TabUI_Gear"), nullptr, SORTLAYER_UI, 5);
	if (nullptr == pObj)
		return E_FAIL;
	pGameInstance->Add_ObjToDontDestroyLayer(pLayerTag, TEXT("Prototype_GameObject_TabUI_Gear"), pObj, SORTLAYER_UI);

	static_cast<CTabUI*>(pTabUI)->Set_UI_Gear(pObj);

	// 4
	pObj = pGameInstance->Add_GameObjectToLayerAndReturn(pLayerTag, TEXT("Prototype_GameObject_TabUI_Selector"), nullptr, SORTLAYER_UI, 5);
	if (nullptr == pObj)
		return E_FAIL;
	pGameInstance->Add_ObjToDontDestroyLayer(pLayerTag, TEXT("Prototype_GameObject_TabUI_Selector"), pObj, SORTLAYER_UI);

	static_cast<CTabUI*>(pTabUI)->Set_UI_Selector(pObj);

	// 5
	pObj = pGameInstance->Add_GameObjectToLayerAndReturn(pLayerTag, TEXT("Prototype_GameObject_PlayerPotionBar"), nullptr, SORTLAYER_UI, 5);
	if (nullptr == pObj)
		return E_FAIL;
	pGameInstance->Add_ObjToDontDestroyLayer(pLayerTag, TEXT("Prototype_GameObject_PlayerPotionBar"), pObj, SORTLAYER_UI);

	// 6
	pObj = pGameInstance->Add_GameObjectToLayerAndReturn(pLayerTag, TEXT("Prototype_GameObject_PlayerItemSlot"), nullptr, SORTLAYER_UI, 5);
	if (nullptr == pObj)
		return E_FAIL;
	pGameInstance->Add_ObjToDontDestroyLayer(pLayerTag, TEXT("Prototype_GameObject_PlayerItemSlot"), pObj, SORTLAYER_UI);

	static_cast<CTabUI*>(pTabUI)->Set_UI_ItemSlot(pObj);

	// 7
	pObj = pGameInstance->Add_GameObjectToLayerAndReturn(pLayerTag, TEXT("Prototype_GameObject_TabUI_Collect"), nullptr, SORTLAYER_UI, 5);
	if (nullptr == pObj)
		return E_FAIL;
	pGameInstance->Add_ObjToDontDestroyLayer(pLayerTag, TEXT("Prototype_GameObject_TabUI_Collect"), pObj, SORTLAYER_UI);

	static_cast<CTabUI*>(pTabUI)->Set_UI_Collect(pObj);

	// 8
	pObj = pGameInstance->Add_GameObjectToLayerAndReturn(pLayerTag, TEXT("Prototype_GameObject_TabUI_Charm"), nullptr, SORTLAYER_UI, 5);
	if (nullptr == pObj)
		return E_FAIL;
	pGameInstance->Add_ObjToDontDestroyLayer(pLayerTag, TEXT("Prototype_GameObject_TabUI_Charm"), pObj, SORTLAYER_UI);

	static_cast<CTabUI*>(pTabUI)->Set_UI_Charm(pObj);

	// 9
	CGameObject* pTabUI_CharmSelect = pGameInstance->Add_GameObjectToLayerAndReturn(pLayerTag, TEXT("Prototype_GameObject_TabUI_CharmSelect"), nullptr, SORTLAYER_UI, 5);
	if (nullptr == pTabUI_CharmSelect)
		return E_FAIL;
	pGameInstance->Add_ObjToDontDestroyLayer(pLayerTag, TEXT("Prototype_GameObject_TabUI_CharmSelect"), pTabUI_CharmSelect, SORTLAYER_UI);

	static_cast<CTabUI*>(pTabUI)->Set_UI_CharmSelect(pTabUI_CharmSelect);

	// 10
	pObj = pGameInstance->Add_GameObjectToLayerAndReturn(pLayerTag, TEXT("Prototype_GameObject_CharmSelector"), nullptr, SORTLAYER_UI, 5);
	if (nullptr == pObj)
		return E_FAIL;
	pGameInstance->Add_ObjToDontDestroyLayer(pLayerTag, TEXT("Prototype_GameObject_CharmSelector"), pObj, SORTLAYER_UI);

	static_cast<CTabUI_CharmSelect*>(pTabUI_CharmSelect)->Set_Selector(pObj);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Librarian::Ready_Layer_Effect_ATZThunder(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_Pooling_GameObjectToLayer(pLayerTag, TEXT("Prototype_GameObject_LibrarianATZThunder"), 20)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Librarian::Ready_Layer_ParticleSystem(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_Pooling_GameObjectToLayer(pLayerTag, TEXT("Prototype_GameObject_Particle_System"), 3))) // Arg 던져서 어떤 파티클 타입으로될지.
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Librarian::Ready_Layer_Boss(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_GameObjectToLayer(pLayerTag, TEXT("Prototype_GameObject_Librarian"))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Librarian::Ready_Layer_PoolingObjects()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_Pooling_GameObjectToLayer(L"Layer_DustEffect", L"Prototype_GameObject_DustEffect", 20)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Librarian::Load_Chest(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_uint ChestSize = pGameInstance->Get_TableSize(TEXT("LibSpChest"), DATATYPE_OBJECT);
	PUSHSPECIALCHESTDATA ChestPushData;

	for (_uint i = 1; i < ChestSize + 1; ++i)
	{
		ZeroMemory(&ChestPushData, sizeof(PUSHSPECIALCHESTDATA));

		ChestPushData = *pGameInstance->Get_RowDataToSpecialChestData(TEXT("LibSpChest"), i, DATATYPE_OBJECT);

		if (FAILED(pGameInstance->Add_GameObjectToLayer(pLayerTag, TEXT("Prototype_GameObject_SpecialChest"), &ChestPushData)))
			return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CLevel_Librarian* CLevel_Librarian::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLevel_Librarian* pInstance = new CLevel_Librarian(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSG_BOX("Failed to Created CLevel_Librarian");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Librarian::Free()
{
	__super::Free();
}
