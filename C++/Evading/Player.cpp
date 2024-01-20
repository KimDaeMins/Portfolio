#include "stdafx.h"
#include "..\public\Player.h"


#include "GameInstance.h"
#include "Camera_Fly.h"
#include "Monster.h"
#include "ParticleSystem.h"

#ifdef _DEBUG
#include "ImGui_Manager.h"
#endif // _DEBUG

#pragma region STATE
#include "StateIdle.h"
#include "StateWalk.h"
#include "StateRun.h"
#include "StateRoll.h"
#include "StateHop.h"
#include "StateAttack.h"
#include "StateAttack2.h"
#include "StateAttack3.h" 
#include "StateStickAttack.h" 
#include "StateStickAttack2.h" 
#include "StateShield.h"
#include "StateHit.h"
#include "StateStagger.h"
#include "StateThrowItem.h"
#include "StateChestOpen.h"
#include "StateActiveStoneSwitch.h"
#include "StateItemMotionChange.h"
#include "MonsterHpBar.h"
#include "StateLadder.h"
#include "StateTetherWand.h"
#include "StateSleep.h"
#include "StatePotionEatting.h"
#include "StateWand.h"
#include "StateParry.h"
#include "StateAttackBounce.h"
#include "StateKneel.h"
#include "StateEatting.h"
#include "StateUI.h"
#include "Shop_Item.h"
#include "Shop_UI.h"
#include "StateTeleport.h"
#include "StateGetItem.h"
#include "StateIceDagger.h"
#include "StateCoinToss.h"
#include "StateFalling.h"
#include "StateWellUI.h"
#pragma endregion
#pragma region STATUS_N_UI
#include "PlayerHexBar.h"
#include "PlayerTargetCircle.h"
#include "InteractionUI.h"
#pragma endregion

//Test
#include "UI_Item.h"
#include "WaterBall.h"
#include "Pong.h"
#include "Effect_SoulBright.h"
#include "BombDust.h"

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CPlayer::CPlayer(const CPlayer & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPlayer::NativeConstruct_Prototype()
{	

	return S_OK;
}

HRESULT CPlayer::NativeConstruct(void * pArg)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (false == __super::Get_DontDestroy())
	{
		if (FAILED(SetUp_Component()))
			return E_FAIL;

		m_HitFlashDuration = CStopWatch(1.f);
		m_HitFlashGap = CStopWatch(0.02f);

		m_SPHealWaitTimer = CStopWatch(0.5f);

		//-----------------------------------------------------
		// Create_Status( _DamegeType, _Damege, _Hp, _MaxHp )
		//-----------------------------------------------------

		Create_Status(0, 0.f, 400.f, 400.f);
		Create_Status_Player(400.f, 400.f, 400.f, 400.f);

		//CPlayerHexBar::TYPE eType_ForPlayer = CPlayerHexBar::TYPE_HEALTH;
		//CGameObject* pGameObject = pGameInstance->Get_GameObject(TEXT("Layer_UI_Player_Status"), eType_ForPlayer);
		//if (pGameObject != nullptr)
		//{
		//	static_cast<CPlayerHexBar*>(pGameObject)->SetHostObject(this);
		//}

		//eType_ForPlayer = CPlayerHexBar::TYPE_STAMINA;
		//pGameObject = pGameInstance->Get_GameObject(TEXT("Layer_UI_Player_Status"), eType_ForPlayer);
		//if (pGameObject != nullptr)
		//{
		//	static_cast<CPlayerHexBar*>(pGameObject)->SetHostObject(this);
		//}

		//eType_ForPlayer = CPlayerHexBar::TYPE_MAGIC;
		//pGameObject = pGameInstance->Get_GameObject(TEXT("Layer_UI_Player_Status"), eType_ForPlayer);
		//if (pGameObject != nullptr)
		//{
		//	static_cast<CPlayerHexBar*>(pGameObject)->SetHostObject(this);
		//}

		m_pInventory = GET_INSTANCE(CInventory);
		Safe_AddRef(m_pInventory);
		RELEASE_INSTANCE(CInventory);

		m_Type = (_uint)OBJECT_TYPE::PLAYER;
		m_eState = STATE_IDLE;

		m_pModelCom->Set_Animator(m_pAnimatorCom);
		//m_pAnimatorCom->SetUp_Animation(25);
		m_pAnimatorCom->Set_IsBlending(ON);

		TRANSFORMINFO pTransInfo = *pGameInstance->Get_RowDataToTransform(TEXT("Player"), "Player", DATATYPE_PLAYER);
		CAPSULECOLINFO pCapsuleInfo = *pGameInstance->Get_RowDataToCapsuleCol(TEXT("Player"), "Player", DATATYPE_PLAYER);

		//m_pTransformCom->Set_State(STATE_POSITION, XMVectorSet(pTransInfo.PosX, pTransInfo.PosY, pTransInfo.PosZ, 1.f));
		m_pTransformCom->Set_State(STATE_POSITION, XMVectorSet(0, 0.f, 0, 1.f));
		m_pTransformCom->Set_State(STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));


		//pGameInstance->_PlayBGM(L"BGM.mp3", SOUND_CHANNEL_ID::BGM, 1.f);		

#pragma region RigidSetting

		m_pRigid->Set_LockFlag(CRigid::PR_ROT, CRigid::LOCK_X, true);
		m_pRigid->Set_LockFlag(CRigid::PR_ROT, CRigid::LOCK_Z, true);

		wstring ColliderName = L"Rigid";
		m_pRigid->Set_Host(this);
		//m_pRigid->Create_CapsuleCollider(ColliderName, pCapsuleInfo.Radius, pCapsuleInfo.Height, _Vector3(pCapsuleInfo.CenterX, pCapsuleInfo.CenterY, pCapsuleInfo.CenterZ));
		m_pRigid->Create_CapsuleCollider(ColliderName, 0.5f, pCapsuleInfo.Height, _Vector3(0.f, 0.5f + pCapsuleInfo.Height, 0.f));
		m_pRigid->Attach_Shape();
		m_pRigid->Set_Transform(m_pTransformCom->Get_WorldMatrix());
		m_pRigid->Set_Gravity(true);
		m_pRigid->SetUp_Filtering(tagFilterGroup::PLAYER);
		m_pRigid->Set_Kinematic(false);
		m_pRigid->Set_Mess(1.f);
		m_pRigid->Set_ActorLocalPos(_Vector3(0.f, 0.5f, 0.f));//이게 중간이 아니면 아래로 밀리나?
		m_pRigid->Set_UpDownMove(true);


 		m_pRigid->Set_Material(ColliderName, 1000.f, 1000.f, 0.f);
 		m_pRigid->Set_LockFlag(CRigid::PR_POS, CRigid::LOCK_Y, true);	

#pragma endregion
		//--------------------------------------------------
		// 스테이트 머신에 (호스트)와 (스테이트)를 등록한다
		//--------------------------------------------------
		m_pStateMachine->SetHostObject(this);				 // 호스트 오브젝트를 반드시 등록 해주세요

#pragma region STATUS
		m_pStateMachine->Add_State(STATE_IDLE, CStateIdle::Create(STATE_IDLE));
		m_pStateMachine->Add_State(STATE_WALK, CStateWalk::Create(STATE_WALK));
		m_pStateMachine->Add_State(STATE_RUN, CStateRun::Create(STATE_RUN));
		m_pStateMachine->Add_State(STATE_ROLL, CStateRoll::Create(STATE_ROLL));
		m_pStateMachine->Add_State(STATE_FALL, CStateFalling::Create(STATE_FALL));
		m_pStateMachine->Add_State(STATE_HOP, CStateHop::Create(STATE_HOP));
		m_pStateMachine->Add_State(STATE_ATTACK, CStateAttack::Create(STATE_ATTACK));
		m_pStateMachine->Add_State(STATE_ATTACK2, CStateAttack2::Create(STATE_ATTACK2));
		m_pStateMachine->Add_State(STATE_ATTACK3, CStateAttack3::Create(STATE_ATTACK3));
		m_pStateMachine->Add_State(STATE_STICKATTACK, CStateStickAttack::Create(STATE_STICKATTACK));
		m_pStateMachine->Add_State(STATE_STICKATTACK2, CStateStickAttack2::Create(STATE_STICKATTACK2));
		m_pStateMachine->Add_State(STATE_SHIELD, CStateShield::Create(STATE_SHIELD));
		m_pStateMachine->Add_State(STATE_HIT, CStateHit::Create(STATE_HIT));
		m_pStateMachine->Add_State(STATE_STAGGER, CStateStagger::Create(STATE_STAGGER));
		m_pStateMachine->Add_State(STATE_CHESTOPEN, CStateChestOpen::Create(STATE_CHESTOPEN));
		m_pStateMachine->Add_State(STATE_THROWITEM, CStateThrowItem::Create(STATE_THROWITEM));	
		m_pStateMachine->Add_State(STATE_EATITEM, CStateEatting::Create(STATE_EATITEM));
		m_pStateMachine->Add_State(STATE_ACTIVESTONSWITCH, CStateActiveStoneSwitch::Create(STATE_ACTIVESTONSWITCH));
		m_pStateMachine->Add_State(STATE_ITEMMOTIONCHANGE,CStateItemMotionChange::Create(STATE_ITEMMOTIONCHANGE));
		m_pStateMachine->Add_State(STATE_LADDER, CStateLadder::Create(STATE_LADDER));
		m_pStateMachine->Add_State(STATE_TETHERWAND, CStateTetherWand::Create(STATE_TETHERWAND));
		m_pStateMachine->Add_State(STATE_WAND, CStateWand::Create(STATE_WAND));
		m_pStateMachine->Add_State(STATE_ICEDAGGER, CStateIceDagger::Create(STATE_ICEDAGGER));
		m_pStateMachine->Add_State(STATE_SLEEP, CStateSleep::Create(STATE_SLEEP));
		m_pStateMachine->Add_State(STATE_POTION, CStatePotionEatting::Create(STATE_POTION));
		m_pStateMachine->Add_State(STATE_PARRY, CStateParry::Create(STATE_PARRY));
		m_pStateMachine->Add_State(STATE_ATTACKBOUNCE, CStateAttackBounce::Create(STATE_ATTACKBOUNCE));
		m_pStateMachine->Add_State(STATE_KNEEL, CStateKneel::Create(STATE_KNEEL));		  
		m_pStateMachine->Add_State(STATE_SHOPUI, CStateUI::Create(STATE_SHOPUI));
		m_pStateMachine->Add_State(STATE_TELEPORT, CStateTeleport ::Create(STATE_TELEPORT));
		m_pStateMachine->Add_State(STATE_GETITEM, CStateGetItem::Create(STATE_GETITEM));
		m_pStateMachine->Add_State(STATE_COINTOSS, CStateCoinToss::Create(STATE_COINTOSS));
		m_pStateMachine->Add_State(STATE_WELLUI, CStateWellUI::Create(STATE_WELLUI));

#pragma endregion
	}
	if (pGameInstance->Get_NextLevel() == LEVEL_OVERWORLD || pGameInstance->Get_NextLevel() == LEVEL_OVERWORLDTEST)
	{		
		if(2 > pGameInstance->Get_OverWorldVisitCnt())
			m_pStateMachine->SetCurrentState(STATE_SLEEP);		 // 최초의 상태를 반드시 등록 해주세요
		else
			m_pStateMachine->SetCurrentState(STATE_IDLE);
	}
	else
		m_pStateMachine->SetCurrentState(STATE_IDLE);

	if (pGameInstance->Get_NextLevel() == LEVEL_OVERWORLD || pGameInstance->Get_NextLevel() == LEVEL_OVERWORLDTEST)
	{
		//m_pTransformCom->Set_State(STATE_POSITION, Vector4(0.f, 0.f, 3.f, 1.f));
		if(pGameInstance->Get_PreShop())
		{
			m_pTransformCom->Set_State(STATE_POSITION, Vector4(-55.87f, 41.18f, 186.76f, 1.f)); // 상점 나오는 위치
		}
		else if(2 > pGameInstance->Get_OverWorldVisitCnt())
			m_pTransformCom->Set_State(STATE_POSITION, Vector4(-9.43f, 12.14f, 27.87f, 1.f)); // 시작위치
		else
			m_pTransformCom->Set_State(STATE_POSITION, Vector4(-0.45f, 43.84f, 179.79f, 1.f));// 석상 옆 위치
		//m_pRigid->Set_LockFlag(CRigid::PR_POS, CRigid::LOCK_Y, !m_pRigid->Get_FreezePosition(CRigid::LOCK_Y));
	}
	else if(pGameInstance->Get_NextLevel() == LEVEL_BEACH || pGameInstance->Get_NextLevel() == LEVEL_BEACHTEST)
	{
		m_pTransformCom->Set_State(STATE_POSITION, Vector4(-26.34f, 2.296f, 98.42f, 1.f));
		//m_pRigid->Set_LockFlag(CRigid::PR_POS, CRigid::LOCK_Y, !m_pRigid->Get_FreezePosition(CRigid::LOCK_Y));
	}
	else if (pGameInstance->Get_NextLevel() == LEVEL_LIBRARIAN || pGameInstance->Get_NextLevel() == LEVEL_LIBRARIANTEST)
	{
		m_pTransformCom->Set_State(STATE_POSITION, Vector4(0.f, -40.3f, -44.31f, 1.f));
		//m_pRigid->Set_LockFlag(CRigid::PR_POS, CRigid::LOCK_Y, !m_pRigid->Get_FreezePosition(CRigid::LOCK_Y));
	}
	else if (pGameInstance->Get_NextLevel() == LEVEL_SHOP || pGameInstance->Get_NextLevel() == LEVEL_SHOPTEST)
	{
		//m_pTransformCom->Set_State(STATE_POSITION, Vector4(0.5f, 0.2f, -35.f, 1.f)); //원래 위치
		m_pTransformCom->Set_State(STATE_POSITION, Vector4(0.5f, 0.2f, -35.f, 1.f));
		//m_pRigid->Set_LockFlag(CRigid::PR_POS, CRigid::LOCK_Y, !m_pRigid->Get_FreezePosition(CRigid::LOCK_Y));
	}
	else if(pGameInstance->Get_NextLevel() == LEVEL_SPIDER || pGameInstance->Get_NextLevel() == LEVEL_SPIDERTEST)
	{
		m_pTransformCom->Set_State(STATE_POSITION, Vector4(0.f, -11.9f, 152.f, 1.f));		
	}
	else if (pGameInstance->Get_NextLevel() == LEVEL_WAVE || pGameInstance->Get_NextLevel() == LEVEL_WAVETEST)
	{
		m_pTransformCom->Set_State(STATE_POSITION, Vector4(0.f, 0.f, 0.f, 1.f));
	}
	else if (pGameInstance->Get_NextLevel() == LEVEL_PUZZEL || pGameInstance->Get_NextLevel() == LEVEL_PUZZELTEST)
	{
		m_pTransformCom->Set_State(STATE_POSITION, Vector4(0.68f, 0.03f, -11.83f, 1.f));
	}




	__super::Set_DontDestroy(true);

	RELEASE_INSTANCE(CGameInstance);
	
	return S_OK;
}

_int CPlayer::Tick(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

#ifdef _DEBUG
	CImGUI_Manager::GetInstance()->Printf(_TEXT("PosX %f"), m_pTransformCom->Get_Position (AXIS_X));
	CImGUI_Manager::GetInstance()->Printf(_TEXT("PosY %f"), m_pTransformCom->Get_Position(AXIS_Y));
	CImGUI_Manager::GetInstance()->Printf(_TEXT("PosZ %f"), m_pTransformCom->Get_Position(AXIS_Z));
#endif // _DEBUG

	if (m_pRigid->Get_FreezePosition(CRigid::LOCK_Y))
		m_pRigid->Set_LinearVelocity(_Vector3(0.f, 0.f, 0.f));
	if (pGameInstance->Key_Down(DIK_B))
	{
		CSoulBright::PUSHSOULBRIGHTDATA SoulBrightData;
		SoulBrightData.HostObject = nullptr;
		SoulBrightData.vColor = _float4(0.92f, 0.93f, 0.651f, 1.f);
		SoulBrightData.StartScale = 4.f;
		SoulBrightData.EndScale = 9.f;
		SoulBrightData.bBillBoard = true;
		SoulBrightData.vPosition = _Vector4(m_pRigid->Get_ActorCenter(), 1.f);
		SoulBrightData.Rotate = pGameInstance->Range_Float(0, 360);
		pGameInstance->Add_GameObjectToLayer(L"Layer_Effect", L"Prototype_GameObject_SoulBright", &SoulBrightData);

		SoulBrightData.vColor = _float4(0.9f, 0.4f, 0.6f, 1.f);
		SoulBrightData.StartScale = 2.f;
		SoulBrightData.EndScale = 6.5f;
		SoulBrightData.Rotate = pGameInstance->Range_Float(0, 360);
		pGameInstance->Add_GameObjectToLayer(L"Layer_Effect", L"Prototype_GameObject_SoulBright", &SoulBrightData);

		CParticleSystem::PARTICLETYPE		eType = CParticleSystem::PARTICLETYPE::BURST_DEFAULT;
		CGameObject* pParticle = pGameInstance->Take_Pooling_GameObject(TEXT("Layer_ParticleSystem"), &eType);
		static_cast<CTransform*>(pParticle->Get_ComponentPtr(TEXT("Com_Transform")))->Set_State(STATE_POSITION, SoulBrightData.vPosition);
		static_cast<CParticleSystem*>(pParticle)->Set_Color(_float3(1.f, 0.83f, 0.92f));
		static_cast<CParticleSystem*>(pParticle)->Set_Duration(1.4f, 0.f, 1.4f);
		static_cast<CParticleSystem*>(pParticle)->Set_GlowColor(_float3(1.f, 0.83f, 0.92f));
		pParticle = pGameInstance->Take_Pooling_GameObject(TEXT("Layer_ParticleSystem"), &eType);
		static_cast<CTransform*>(pParticle->Get_ComponentPtr(TEXT("Com_Transform")))->Set_State(STATE_POSITION, SoulBrightData.vPosition);
		static_cast<CParticleSystem*>(pParticle)->Set_Color(_float3(0.83f, 1.f, 0.83f));
		static_cast<CParticleSystem*>(pParticle)->Set_Duration(1.4f, 0.f, 1.4f);
		static_cast<CParticleSystem*>(pParticle)->Set_GlowColor(_float3(0.83f, 1.f, 0.83f));
		pParticle = pGameInstance->Take_Pooling_GameObject(TEXT("Layer_ParticleSystem"), &eType);
		static_cast<CTransform*>(pParticle->Get_ComponentPtr(TEXT("Com_Transform")))->Set_State(STATE_POSITION, SoulBrightData.vPosition);
		static_cast<CParticleSystem*>(pParticle)->Set_Color(_float3(0.83f, 0.92f, 1.f));
		static_cast<CParticleSystem*>(pParticle)->Set_Duration(1.4f, 0.f, 1.4f);
		static_cast<CParticleSystem*>(pParticle)->Set_GlowColor(_float3(0.83f, 0.92f, 1.f));
		pParticle = pGameInstance->Take_Pooling_GameObject(TEXT("Layer_ParticleSystem"), &eType);
		static_cast<CTransform*>(pParticle->Get_ComponentPtr(TEXT("Com_Transform")))->Set_State(STATE_POSITION, SoulBrightData.vPosition);
		static_cast<CParticleSystem*>(pParticle)->Set_Color(_float3(1.f, 1.f, 0.83f));
		static_cast<CParticleSystem*>(pParticle)->Set_Duration(1.4f, 0.f, 1.4f);
		static_cast<CParticleSystem*>(pParticle)->Set_GlowColor(_float3(1.f, 1.f, 0.83f));
	}
	if (pGameInstance->Key_Down(DIK_N))
	{
		m_pRigid->Set_LockFlag(CRigid::PR_POS, CRigid::LOCK_Y, !m_pRigid->Get_FreezePosition(CRigid::LOCK_Y));
		CWaterBall::PUSHWATERBALLDATA Data;
		Data.Diffuse = { 0.5f, 0.75f, 1.f, 1.f };
		Data.MainPos = m_pTransformCom->Get_State(STATE_POSITION);
		Data.NumWaterBall = 35;
		if (FAILED(pGameInstance->Add_GameObjectToLayer(L"Layer_Effect", L"Prototype_GameObject_WaterBallEffect", &Data)))
			MSG_BOX("?");
	}
	if(pGameInstance->Key_Down(DIK_M))
	{
		CPong::PUSHPONGDATA Data;
		Data.Diffuse = { 1.f, 1.f, 1.f, 1.f };
		Data.MainPos = m_pTransformCom->Get_State(STATE_POSITION);
		Data.NumPong = 36;
		if (FAILED(pGameInstance->Add_GameObjectToLayer(L"Layer_Effect", L"Prototype_GameObject_PongEffect", &Data)))
			MSG_BOX("?");
	}
	if (pGameInstance->Key_Down(DIK_V))
	{
		CBombDust::PUSHBOMBDUSTDATA Data;
		Data.Diffuse = { 1.f, 1.f, 1.f, 1.f };
		Data.MainPos = m_pTransformCom->Get_State(STATE_POSITION);
		Data.NumBombDust = 36;
		Data.Glow = true;
		if (FAILED(pGameInstance->Add_GameObjectToLayer(L"Layer_Effect", L"Prototype_GameObject_BombDustEffect", &Data)))
			MSG_BOX("?");
	}
	if (pGameInstance->Key_Down(DIK_C))
	{
		CBombDust::PUSHBOMBDUSTDATA Data;
		Data.Diffuse = { 1.f, 1.f, 1.f, 1.f };
		Data.MainPos = m_pTransformCom->Get_State(STATE_POSITION);
		Data.NumBombDust = 36;
		if (FAILED(pGameInstance->Add_GameObjectToLayer(L"Layer_Effect", L"Prototype_GameObject_BombDustEffect", &Data)))
			MSG_BOX("?");
	}

	// todo : test
	if (m_Test)
	{
 		m_pInventory->Create_Potion();

// 		CItem* Weapon = (CItem*)pGameInstance->Add_GameObjectToLayerAndReturn(L"Layer_Item", TEXT("Prototype_GameObject_Sword"), nullptr, SORTLAYER_OBJ);
// 		m_pInventory->Add_Weapon(Weapon);
// 		pGameInstance->Add_ObjToDontDestroyLayer(L"Layer_Item", TEXT("Prototype_GameObject_Sword"), Weapon);
// 		m_pInventory->Change_UsingItem(KEYTAG::KEY_J, Weapon);
// 
// 		// Stick
// 		Weapon = (CItem*)pGameInstance->Add_GameObjectToLayerAndReturn(L"Layer_Item", TEXT("Prototype_GameObject_Stick"), nullptr, SORTLAYER_OBJ);
// 		m_pInventory->Add_Weapon(Weapon);
// 		pGameInstance->Add_ObjToDontDestroyLayer(L"Layer_Item", TEXT("Prototype_GameObject_Stick"), Weapon);
// 
// 		// Wand
// 		Weapon = (CItem*)pGameInstance->Add_GameObjectToLayerAndReturn(L"Layer_Item", TEXT("Prototype_GameObject_Wand"), nullptr, SORTLAYER_OBJ);
// 		m_pInventory->Add_Weapon(Weapon);
// 		pGameInstance->Add_ObjToDontDestroyLayer(L"Layer_Item", TEXT("Prototype_GameObject_Wand"), Weapon);
// 
// 		//TetherWand
// 		Weapon = (CItem*)pGameInstance->Add_GameObjectToLayerAndReturn(L"Layer_Item", TEXT("Prototype_GameObject_TetherWand"), nullptr, SORTLAYER_OBJ);
// 		m_pInventory->Add_Weapon(Weapon);
// 		pGameInstance->Add_ObjToDontDestroyLayer(L"Layer_Item", TEXT("Prototype_GameObject_TetherWand"), Weapon);
// 
// 		// IceDagger
// 		Weapon = (CItem*)pGameInstance->Add_GameObjectToLayerAndReturn(L"Layer_Item", TEXT("Prototype_GameObject_IceDagger"), nullptr, SORTLAYER_OBJ);
// 		m_pInventory->Add_Weapon(Weapon);
// 		pGameInstance->Add_ObjToDontDestroyLayer(L"Layer_Item", TEXT("Prototype_GameObject_IceDagger"), Weapon);
// 
//  		m_pInventory->Change_UsingItem(KEYTAG::KEY_L, Weapon);
//  		m_pInventory->Add_ConsumableItem((_uint)CONSUMABLEITEM_TYPE::FIRECRACKER, 15);
//  		m_pInventory->Add_ConsumableItem((_uint)CONSUMABLEITEM_TYPE::FIREBOTTLE, 20);
//  		m_pInventory->Add_ConsumableItem((_uint)CONSUMABLEITEM_TYPE::ICEBOTTLE, 73);
//  		m_pInventory->Add_ConsumableItem((_uint)CONSUMABLEITEM_TYPE::PEPPER, 3);
//  		m_pInventory->Add_ConsumableItem((_uint)CONSUMABLEITEM_TYPE::HERB, 99);
//  		m_pInventory->Add_ConsumableItem((_uint)CONSUMABLEITEM_TYPE::BLUEFRUIT, 12);
// 
//  		m_pInventory->Add_ConsumableItem((_uint)CONSUMABLEITEM_TYPE::PIGGYBANK, 10);
//   		m_pInventory->Add_ConsumableItem((_uint)CONSUMABLEITEM_TYPE::COIN, 67);
// 
// 		//m_pInventory->Change_UsingItem(KEYTAG::KEY_K, 1, 0);
// 
// 		m_pInventory->Add_Geer(GEAR_TYPE::SHIELD);
// 		m_pInventory->Add_Geer(GEAR_TYPE::CAPE);
// 
// 		m_pInventory->Add_Charm(CHARM_TYPE::ROCKETWAND);
// 		m_pInventory->Add_Charm(CHARM_TYPE::CONVERTPOTION);
// 		m_pInventory->Add_Charm(CHARM_TYPE::DROPHP);
		m_pInventory->SizeUp_Charm();

		m_pInventory->Add_PotionMax(3);
		m_pInventory->Add_Gold(1000);
 		m_Test = false;	

		m_pPlayerTargetCircle = (CPlayerTargetCircle*)pGameInstance->Add_GameObjectToLayerAndReturn(L"Layer_UI_Player_Status", TEXT("Prototype_GameObject_PlayerTargetCircle"));
		pGameInstance->Add_ObjToDontDestroyLayer(L"Layer_UI_Player_Status", TEXT("Prototype_GameObject_PlayerTargetCircle"), m_pPlayerTargetCircle);

		CMonsterHpBar::TYPE eType_ForMonster = CMonsterHpBar::TYPE_HEALTH;
		CGameObject* pGameObject = pGameInstance->Take_Pooling_GameObject(TEXT("Layer_UI_Monster_Hp"), &eType_ForMonster);
		pGameInstance->Add_ObjToDontDestroyLayer(TEXT("Layer_UI_Monster_Hp"), TEXT("Prototype_GameObject_MonsterHexBar"), pGameObject);
		if (pGameObject != nullptr)
		{
			m_pMonsterHpBar = (CMonsterHpBar*)pGameObject;
		}

		m_pInteractionUI = (CInteractionUI*)pGameInstance->Add_GameObjectToLayerAndReturn(L"Layer_UI_Player_Status", TEXT("Prototype_GameObject_InteractionUI"), nullptr);
		pGameInstance->Add_ObjToDontDestroyLayer(L"Layer_UI_Player_Status", TEXT("Prototype_GameObject_InteractionUI"), m_pInteractionUI);

	}
	if (pGameInstance->Key_Down(DIK_O))
		Add_PlayerMp(100.f);
	if (pGameInstance->Key_Down(DIK_I))
		m_pInventory->Fill_Potion();
	if (pGameInstance->Key_Down(DIK_U))
		m_pTransformCom->Set_State(STATE_POSITION, XMVectorSetY(m_pTransformCom->Get_State(STATE_POSITION), 10.f));
	if (pGameInstance->Key_Pressing(DIK_EQUALS))
		m_pInventory->Add_Gold(123);


	TargetCheck();
	Key_Check();
	Targetting();



#pragma region HIT
	if (m_EvadingTime > 0.f)
	{
		m_bEvading = true;
		m_EvadingTime -= fTimeDelta;

		if (m_EvadingTime < 0.f)
		{
			m_bEvading = false;
		}
	}
	if (!m_bEvading)
	{
		CGameObject* pGameObject = nullptr;
		_bool CollisionTrigger = false;
		while (m_pRigid->Get_Collision_Trigger_Enter(&pGameObject))
		{
			_uint Type = pGameObject->Get_Type();
			if (Type == (_uint)OBJECT_TYPE::ATTACK)
			{
				Add_PlayerHp(-pGameObject->Get_Damege());
				m_AccDmg += pGameObject->Get_Damege();
				Set_HitFlashing();
				if (m_AccDmg > 15.f)
				{
					Set_Evading(1.f);
					m_AccDmg = 0.f;
					m_pStateMachine->SetCurrentState(CPlayer::STATE_STAGGER);
				}
				else if (m_AccDmg > 5.f)
				{
					Set_Evading(0.4f);
					m_AccDmg = 0.f;
					m_pStateMachine->SetCurrentState(CPlayer::STATE_HIT);
				}
				else
				{
					Set_Evading(0.2f);
					pGameInstance->StopSound(SINGLE, PLAYER);
					pGameInstance->Play(L"pl_gen_hurt_minor", SINGLE, PLAYER, 1);
				}
				/*Set_Hit((_uint)OBJECT_TYPE::ATTACK, pGameObject->Get_Damege());*/
			}
		}
		if (!m_bEvading)
		{
			while (m_pRigid->Get_Collision_Trigger_Stay(&pGameObject))
			{
				_uint Type = pGameObject->Get_Type();
				if (Type == (_uint)OBJECT_TYPE::ATTACK && m_EvadingTime <= 0.f)
				{
					Add_PlayerHp(-pGameObject->Get_Damege());
					m_AccDmg += pGameObject->Get_Damege();
					Set_HitFlashing();
					if (m_AccDmg > 15.f)
					{
						Set_Evading(1.f);
						m_AccDmg = 0.f;
						m_pStateMachine->SetCurrentState(CPlayer::STATE_STAGGER);
					}
					else if (m_AccDmg > 5.f)
					{
						Set_Evading(0.4f);
						m_AccDmg = 0.f;
						m_pStateMachine->SetCurrentState(CPlayer::STATE_HIT);
					}
					else
					{
						Set_Evading(0.2f);
						pGameInstance->StopSound(SINGLE, PLAYER);
						pGameInstance->Play(L"pl_gen_hurt_minor", SINGLE, PLAYER, 1);
					}
					/*Set_Hit((_uint)OBJECT_TYPE::ATTACK, pGameObject->Get_Damege());*/
				}
				if (!CollisionTrigger && (m_pStateMachine->GetCurrentState() == STATE_WALK || m_pStateMachine->GetCurrentState() == STATE_IDLE ) &&
					(Type == (_uint)OBJECT_TYPE::LADDER_UP || Type == (_uint)OBJECT_TYPE::LADDER_DOWN || Type == (_uint)OBJECT_TYPE::CHEST
						|| Type == (_uint)OBJECT_TYPE::TELEPORT || Type == (_uint)OBJECT_TYPE::SHOPITEM || Type == (_uint)OBJECT_TYPE::WELL
					|| Type == (_uint)OBJECT_TYPE::WAVEALTARFLAME || Type == (_uint)OBJECT_TYPE::FOXALTAR))

				{
					m_pInteractionUI->TriggerCollision(pGameObject->Get_Transform()->Get_State(STATE_POSITION), Type);
									
					CollisionTrigger = true;
				}

			}

			if (!CollisionTrigger)
			{
				m_pInteractionUI->CollisionEnd();
			}
		}
	}
#pragma endregion

	HitFlashing(fTimeDelta);
	Check_HealingSP(fTimeDelta);
	m_pStateMachine->Update_CurretState(fTimeDelta);
 	m_pAnimatorCom->Update_Animation(fTimeDelta);

	m_pTransformCom->MoveUsingAnimation(m_pAnimatorCom, nullptr, false, m_UseWorldUpdate);

	//RELEASE_INSTANCE(CGameInstance);
	m_pInventory->LastUseTick(fTimeDelta);

	//if (pGameInstance->Key_Down(DIK_3))
	//	m_pStateMachine->SetCurrentState(STATE_IDLE);

	CPhysicsSystem* pPhysX = GET_INSTANCE(CPhysicsSystem);
	CPhysicsSystem::RAYCASTBUFFER Buffer;
	_Vector3 Origin = m_pTransformCom->Get_State(STATE_POSITION);
	Origin.y += 0.5f;
	
	if (m_pStateMachine->GetCurrentState() != STATE_LADDER  && m_pStateMachine->GetCurrentState() != STATE_FALL && pPhysX->Raycast(Buffer, Origin, _Vector3(0.f, -1.f, 0.f), 0.7f, tagFilterGroup::OBJECT | tagFilterGroup::PLANE, QUERYTYPE::SIMULATION) && Buffer.Normal.y > 0.9f)
	{
		m_pRigid->Set_LockFlag(CRigid::PR_POS, CRigid::LOCK_Y, true);
		m_pTransformCom->Set_State(STATE_POSITION, Buffer.Position);

	}
	else
	{
		m_pRigid->Set_LockFlag(CRigid::PR_POS, CRigid::LOCK_Y, false);
	}
	RELEASE_INSTANCE(CPhysicsSystem);
	RELEASE_INSTANCE(CGameInstance);

	return _int();
}

_int CPlayer::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;



	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this)))
		return 0;
	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DEPTH_SILHOUETTE, this)))
		return 0;
	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DYNAMICSHADOW, this)))
		return 0;
	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this)))
		return 0;
	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHA, this)))
		return 0;
	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_GLOW, this)))
		return 0;

	// TODO : 위치값 콘솔로 테스트
		//cout << XMVectorGetX(m_pTransformCom->Get_State(STATE_POSITION)) << ", " << 
		//	XMVectorGetY(m_pTransformCom->Get_State(STATE_POSITION)) << ", " <<
		//	XMVectorGetZ(m_pTransformCom->Get_State(STATE_POSITION)) << endl;
	m_pRigid->Set_AngularVelocity(_Vector3(0.f, 0.f, 0.f));
	m_pInventory->LastUseLateTick(fTimeDelta);
	return _int();
}

HRESULT CPlayer::Render(_uint iRenderGroup)
{
	if (!m_bIsRendering)
		return S_OK;

	if (CRenderer::RENDER_NONALPHA == iRenderGroup)
	{
		if (FAILED(SetUp_ConstantTable()))
			return E_FAIL;

		/* 장치에 월드변환 행렬을 저장한다. */
		_uint	iNumMaterials = m_pModelCom->Get_NumMaterials();

		for (_uint i = 0; i < iNumMaterials; ++i)
		{
			m_pModelCom->Set_ShaderResourceView(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);
			m_pModelCom->Render(m_pShaderCom, i, 23);
		}
// 		else
// 		{
// 			_uint	iNumMaterials = m_pModelCom->Get_NumMaterials();
// 
// 			for (_uint i = 0; i < iNumMaterials; ++i)
// 			{
// 				m_pModelCom->Render(m_pShaderCom, i, 31);
// 			}
// 		}
	}

	else if (CRenderer::RENDER_NONLIGHT == iRenderGroup)
	{
		if (FAILED(SetUp_ConstantTable()))
			return E_FAIL;

		_uint	iNumMaterials = m_pModelCom->Get_NumMaterials();

		for (_uint i = 0; i < iNumMaterials; ++i)
		{
			if (m_bHitFlashing)
			{
				if (false == m_bHitFlashColorToggle)
				{
					m_pModelCom->Render(m_pShaderCom, i, 9);				// 피격 효과
				}
				else
				{
					m_pShaderCom->Set_RawValue("g_vMtrlDiffuse", &_float4(0.f, 0.f, 0.f, 1.f), sizeof(_float4));
					m_pModelCom->Render(m_pShaderCom, i, 18);
				}
			}

			// 맵 오브젝트 뒤 실루엣
			m_pShaderCom->Set_RawValue("g_vMtrlDiffuse", &_float4(0.25f, 0.3f, 0.35f, 1.f), sizeof(_float4));
			m_pModelCom->Render(m_pShaderCom, i, 16);
		}
	}

//  	else if (CRenderer::RENDER_DEPTH_SILHOUETTE == iRenderGroup)
//  	{
//  		if (FAILED(SetUp_ConstantTable()))
//  			return E_FAIL;
//  
//  		/* 장치에 월드변환 행렬을 저장한다. */
//  		_uint	iNumMaterials = m_pModelCom->Get_NumMaterials();
//  
//  		for (_uint i = 0; i < iNumMaterials; ++i)
//  		{
// 			m_pModelCom->Render(i, 7);			// 비교용 depth 저장
//  		}
//  	}

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

	m_pInventory->LastUseRender(iRenderGroup);

	return S_OK;
}

#pragma region FUNC
void CPlayer::TepOnOff(_bool OnOff)
{
	m_Tep = OnOff;
}

void CPlayer::Create_Status_Player(_float _Sp, _float _MaxSp, _float _Mp, _float _MaxMp)
{
	m_pStatus_Player = new STATUSPLAYER(_Sp, _MaxSp, _Mp, _MaxMp);
}

void CPlayer::Set_PlayerHp(_float Hp)
{
	Set_Hp(Hp);

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CPlayerHexBar*		pPlayerHPBar = static_cast<CPlayerHexBar*>(pGameInstance->Get_GameObject(L"Layer_UI_Player_Status", 0, SORTLAYER_UI));

	if (nullptr != pPlayerHPBar)
		pPlayerHPBar->Set_Point((_int)Hp);

	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::Add_PlayerHp(_float Hp)
{
	Add_Hp(Hp);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CPlayerHexBar* pPlayerHPBar = static_cast<CPlayerHexBar*>(pGameInstance->Get_GameObject(L"Layer_UI_Player_Status", 0, SORTLAYER_UI));

	if (nullptr != pPlayerHPBar)
		pPlayerHPBar->Set_Point((_int)Get_Hp());

	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::Set_PlayerMaxHp(_float MaxHp)
{
	Set_MaxHp(MaxHp);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CPlayerHexBar* pPlayerHPBar = static_cast<CPlayerHexBar*>(pGameInstance->Get_GameObject(L"Layer_UI_Player_Status", 0, SORTLAYER_UI));

	if (nullptr != pPlayerHPBar)
		pPlayerHPBar->Set_MaxPoint((_int)MaxHp);

	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::Add_PlayerMaxHp(_float MaxHp)
{
	Add_MaxHp(MaxHp);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CPlayerHexBar* pPlayerHPBar = static_cast<CPlayerHexBar*>(pGameInstance->Get_GameObject(L"Layer_UI_Player_Status", 0, SORTLAYER_UI));

	if (nullptr != pPlayerHPBar)
		pPlayerHPBar->Set_MaxPoint((_int)Get_MaxHp());

	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::Set_PlayerSp(_float Sp)
{
	m_pStatus_Player->Sp = Sp;

	if (m_bLowSp)
	{
		if (0.f >= m_pStatus_Player->Sp)
			m_bLowSp = true;
	}
	else
	{
		if (200.f <= m_pStatus_Player->Sp)
			m_bLowSp = false;
	}

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CPlayerHexBar* pPlayerSPBar = static_cast<CPlayerHexBar*>(pGameInstance->Get_GameObject(L"Layer_UI_Player_Status", 1, SORTLAYER_UI));

	if (nullptr != pPlayerSPBar)
		pPlayerSPBar->Set_Point((_int)Sp);

	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::Add_PlayerSp(_float Sp)
{
	m_pStatus_Player->Sp = Clamp(m_pStatus_Player->Sp + Sp, 0.f, m_pStatus_Player->MaxSp);

	if (false == m_bLowSp)
	{
		if (0.f >= m_pStatus_Player->Sp)
			m_bLowSp = true;
	}
	else
	{
		if (200.f <= m_pStatus_Player->Sp)
			m_bLowSp = false;
	}
	

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CPlayerHexBar* pPlayerSPBar = static_cast<CPlayerHexBar*>(pGameInstance->Get_GameObject(L"Layer_UI_Player_Status", 1, SORTLAYER_UI));

	if (nullptr != pPlayerSPBar)
		pPlayerSPBar->Set_Point((_int)m_pStatus_Player->Sp);

	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::Set_PlayerMaxSp(_float MaxSp)
{
	m_pStatus_Player->MaxSp = MaxSp;
	m_pStatus_Player->Sp = min(m_pStatus_Player->Sp, m_pStatus_Player->MaxSp);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CPlayerHexBar* pPlayerSPBar = static_cast<CPlayerHexBar*>(pGameInstance->Get_GameObject(L"Layer_UI_Player_Status", 1, SORTLAYER_UI));

	if (nullptr != pPlayerSPBar)
		pPlayerSPBar->Set_MaxPoint((_int)MaxSp);

	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::Add_PlayerMaxSp(_float MaxSp)
{
	m_pStatus_Player->MaxSp = max(0.f, m_pStatus_Player->MaxSp + MaxSp);
	m_pStatus_Player->Sp = min(m_pStatus_Player->Sp, m_pStatus_Player->MaxSp);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CPlayerHexBar* pPlayerSPBar = static_cast<CPlayerHexBar*>(pGameInstance->Get_GameObject(L"Layer_UI_Player_Status", 1, SORTLAYER_UI));

	if (nullptr != pPlayerSPBar)
		pPlayerSPBar->Set_MaxPoint((_int)m_pStatus_Player->MaxSp);

	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::Set_PlayerMp(_float Mp)
{
	m_pStatus_Player->Mp = Mp;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CPlayerHexBar* pPlayerMPBar = static_cast<CPlayerHexBar*>(pGameInstance->Get_GameObject(L"Layer_UI_Player_Status", 2, SORTLAYER_UI));

	if (nullptr != pPlayerMPBar)
		pPlayerMPBar->Set_Point((_int)Mp);

	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::Add_PlayerMp(_float Mp)
{
	m_pStatus_Player->Mp = Clamp(m_pStatus_Player->Mp + Mp, 0.f, m_pStatus_Player->MaxMp);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CPlayerHexBar* pPlayerMPBar = static_cast<CPlayerHexBar*>(pGameInstance->Get_GameObject(L"Layer_UI_Player_Status", 2, SORTLAYER_UI));

	if (nullptr != pPlayerMPBar)
		pPlayerMPBar->Set_Point((_int)m_pStatus_Player->Mp);

	// 아이템슬롯 mp 변경 : tabui -> itemslot

	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::Set_PlayerMaxMp(_float MaxMp)
{
	m_pStatus_Player->MaxMp = MaxMp;
	m_pStatus_Player->Mp = min(m_pStatus_Player->Mp, m_pStatus_Player->MaxMp);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CPlayerHexBar* pPlayerMPBar = static_cast<CPlayerHexBar*>(pGameInstance->Get_GameObject(L"Layer_UI_Player_Status", 2, SORTLAYER_UI));

	if (nullptr != pPlayerMPBar)
		pPlayerMPBar->Set_MaxPoint((_int)MaxMp);

	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::Add_PlayerMaxMp(_float MaxMp)
{
	m_pStatus_Player->MaxMp = max(0.f, m_pStatus_Player->MaxMp + MaxMp);
	m_pStatus_Player->Mp = min(m_pStatus_Player->Mp, m_pStatus_Player->MaxMp);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CPlayerHexBar* pPlayerMPBar = static_cast<CPlayerHexBar*>(pGameInstance->Get_GameObject(L"Layer_UI_Player_Status", 2, SORTLAYER_UI));

	if (nullptr != pPlayerMPBar)
		pPlayerMPBar->Set_MaxPoint((_int)m_pStatus_Player->MaxMp);

	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::Set_Evading(_float EvadingTime)
{
	if (EvadingTime <= 0.f)
	{
		m_EvadingTime = 0.f;
		m_bEvading = false;
	}
	else
	{
		m_bEvading = true;
		m_EvadingTime = EvadingTime;
	}
}

_bool CPlayer::Get_Evading()
{
	return m_bEvading;
}

_float CPlayer::Get_EvadingTime()
{
	return m_EvadingTime;
}

void CPlayer::Check_HealingSP(_float fTimeDelta)
{
	if (!m_bSpHealing)			// sp 회복중이 아님 (sp가 max일때/행동이 끝난지 얼마 안되었을때/행동중일 때)
	{
		if (!(STATE_IDLE == m_pStateMachine->GetCurrentState() ||
			STATE_WALK == m_pStateMachine->GetCurrentState() ||
			STATE_RUN == m_pStateMachine->GetCurrentState() ||
			STATE_SHIELD == m_pStateMachine->GetCurrentState()))
		{
			m_bSpHealing = false;
			m_SPHealWaitTimer.Reset();
		}
		else if (m_pStatus_Player->MaxSp > m_pStatus_Player->Sp)
		{
			m_SPHealWaitTimer.Add_Time(fTimeDelta);

			if (m_SPHealWaitTimer.Check_End())
			{
				m_bSpHealing = true;
			}
		}
	}
	else						// sp 회복중 (행동이 끝나고 일정시간이 지났을 때)
	{
		if (STATE_IDLE == m_pStateMachine->GetCurrentState() ||
			STATE_WALK == m_pStateMachine->GetCurrentState())
			Add_PlayerSp(200.f * fTimeDelta);
		else if (STATE_RUN == m_pStateMachine->GetCurrentState() ||
			STATE_SHIELD == m_pStateMachine->GetCurrentState())
			Add_PlayerSp(50.f * fTimeDelta);

		if (m_pStatus_Player->MaxSp > m_pStatus_Player->Sp)
			m_bSpHealing = false;
	}
}


void CPlayer::Set_Hit(_uint DamegeType, _float Damege)
{
	Set_Evading(0.5f);
	Set_HitFlashing();
	Add_PlayerHp(-Damege);
	if (Damege < 20.f)
		m_pStateMachine->SetCurrentState(CPlayer::STATE_HIT);
	else
		m_pStateMachine->SetCurrentState(CPlayer::STATE_STAGGER);
}

void CPlayer::Targetting()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Down(DIK_LSHIFT))
	{
		//// todo : 카메라 방향 세팅 테스트
		//CCamera_Fly* pCamera = dynamic_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(TEXT("Layer_Camera")));
		//_Vector3 vCameraLook = pCamera->GetLook();
		//if (nullptr != pCamera)
		//{
		//	_Vector3 vRotationAxis = _Vector3(0.f, 1.f, 0.f).Cross(vCameraLook);
		//	vRotationAxis.Normalize();
		//	_Vector4 vQuaternion = Quaternion::CreateFromAxisAngle(vRotationAxis, XMConvertToRadians(20.f));
		//	pCamera->Set_DirOffsetQuarternion(vQuaternion);
		//}

		_float Distance = m_TargetMaxDistance;
		list<CGameObject*>* plist = pGameInstance->Get_ObjectList(L"Layer_Monster");
		if (plist)
		{
			for (auto& GameObject : *plist)
			{
				if (Distance > ((CMonster*)GameObject)->Get_DistanceToPlayer() && !GameObject->Get_Dead() && GameObject->Get_Hp() > 0.f)
				{
					Distance = ((CMonster*)GameObject)->Get_DistanceToPlayer();
					m_Target = GameObject;
					m_TargetLook = true;
				}
				else if (Distance > ((CMonster*)GameObject)->Get_DistanceToPlayer() && GameObject->Get_Hp() == -100.f)
				{
					_float CheckDistance = ((CMonster*)GameObject)->Get_DistanceToPlayer();
					CheckDistance -= 3.f;
					if (CheckDistance > 0)
					{
						Distance = ((CMonster*)GameObject)->Get_DistanceToPlayer();
						m_Target = GameObject;
						m_TargetLook = true;
					}
				}
			}
		}
		plist = pGameInstance->Get_ObjectList(L"Layer_Boss");
		if (plist)
		{
			for (auto& GameObject : *plist)
			{
				if (Distance > ((CMonster*)GameObject)->Get_DistanceToPlayer() && !GameObject->Get_Dead())
				{
					Distance = ((CMonster*)GameObject)->Get_DistanceToPlayer();
					m_Target = GameObject;
					m_TargetLook = true;
				}
			}
		}
		plist = pGameInstance->Get_ObjectList(L"Layer_Wave");
		if (plist)
		{
			for (auto& GameObject : *plist)
			{
				if (Distance > ((CMonster*)GameObject)->Get_DistanceToPlayer() && !GameObject->Get_Dead() && GameObject->Get_Hp() > 0.f)
				{
					Distance = ((CMonster*)GameObject)->Get_DistanceToPlayer();
					m_Target = GameObject;
					m_TargetLook = true;
				}
			}
		}
	}
	else if(pGameInstance->Key_Up(DIK_LSHIFT))
	{
		//// todo : 카메라 방향 세팅 테스트
		//CCamera_Fly* pCamera = dynamic_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(TEXT("Layer_Camera")));
		//if (nullptr != pCamera)
		//{
		//	pCamera->Set_DirOffsetQuarternion(_Vector4(0.f, 0.f, 0.f, 1.f));
		//}


		m_Target = nullptr;
		m_TargetLook = false;

		// todo : 타겟팅 리셋 테스트
		Reset_TargettingCamera();
	}


	if (nullptr != m_Target)//타겟이 리지드가 없을리가있나? 없다.
	{
		wstring Rigid = L"Rigid";
		_Vector3 ShapeCenter =
			((CRigid*)m_Target->Get_ComponentPtr(L"Com_RigidBody"))->Get_ActorPos() +
			((CRigid*)m_Target->Get_ComponentPtr(L"Com_RigidBody"))->Get_LocalPos(Rigid);
		m_pPlayerTargetCircle->SetTargetPos(_Vector4(ShapeCenter, 1.f), true);

		_uint TargetFilter = ((CRigid*)m_Target->Get_ComponentPtr(L"Com_RigidBody"))->Get_Filter(Rigid, 0);
		if(TargetFilter != (_uint)tagFilterGroup::BOSS && m_Target->Get_Type() != (_uint)OBJECT_TYPE::SHIELD && m_pMonsterHpBar != nullptr)
			m_pMonsterHpBar->SetHostObject(m_Target);
	}
	else
	{
		m_pPlayerTargetCircle->SetTargetPos(_Vector4(0.f, 0.f, 0.f, 0.f), false);

		if (m_pMonsterHpBar != nullptr)
			m_pMonsterHpBar->SetHostObject(nullptr);
	}

	RELEASE_INSTANCE(CGameInstance)
}

void CPlayer::TargetCheck()
{
	if (m_TargetLook)
	{
		if (m_Target->Get_Dead())
		{
			m_TargetLook = false;
			m_Target = nullptr;
			Reset_TargettingCamera();
		}
		else if (m_Target->Get_Hp() <= 0.f && m_Target->Get_Hp() != -100.f)
		{
			m_TargetLook = false;
			m_Target = nullptr;
			Reset_TargettingCamera();
		}
	}
}

_bool CPlayer::TargetLook()
{

	if (m_TargetLook)//타겟을 찝었다면?
	{
		if (m_Target->Get_Dead())
		{
			m_TargetLook = false;
			m_Target = nullptr;

			// todo : 타겟팅 리셋 테스트
			Reset_TargettingCamera();

			return false;
		}

		_Vector4 TargetPos = m_Target->Get_Transform()->Get_State(STATE_POSITION);
		_Vector4 MyPos = m_pTransformCom->Get_State(STATE_POSITION);
		_Vector3 ToTarget = TargetPos - MyPos;

		m_pTransformCom->LookAtDir(ToTarget);

		// todo : 타겟팅 카메라이동 테스트
		Set_TargettingCamera(TargetPos, MyPos);

		if (ToTarget.Length() > m_TargetMaxDistance)
		{
			m_Target = nullptr;
			m_TargetLook = false;

			// todo : 타겟팅 리셋 테스트
			Reset_TargettingCamera();
		}
	}

	return m_TargetLook;
}

_bool CPlayer::TargetLookLerp(_float fTimeDelta)
{
	if (m_TargetLook)//타겟을 찝었다면?
	{
		if (m_Target->Get_Dead())
		{
			m_TargetLook = false;
			m_Target = nullptr;

			// todo : 타겟팅 리셋 테스트
			Reset_TargettingCamera();

			return false;
		}
		Vector4 position = m_pTransformCom->Get_State(STATE_POSITION);
		Vector4 targetPosition = m_Target->Get_Transform()->Get_State(STATE_POSITION);
		Vector3 dis = targetPosition - position;
		_Vector3 Dir;
		dis.Normalize(Dir);

		float yAngle = atan2f(Dir.x, Dir.z);
		Vector3 myAngles = RotationMatrixToRadians(m_pTransformCom->Get_WorldMatrix());
		Quaternion slerp = Quaternion::Slerp(Quaternion::CreateFromYawPitchRoll(myAngles.y, myAngles.x, myAngles.z), Quaternion::CreateFromYawPitchRoll(yAngle, myAngles.x, myAngles.z), fTimeDelta * 10.f);
		Matrix rotationMatrix = Matrix::CreateFromQuaternion(slerp);
		m_pTransformCom->Set_Rotate(rotationMatrix.Right(), rotationMatrix.Up(), rotationMatrix.Forward());

		// todo : 타겟팅 카메라이동 테스트
		Set_TargettingCamera(targetPosition, position);

		if (dis.Length() > m_TargetMaxDistance)
		{
			m_Target = nullptr;
			m_TargetLook = false;

			// todo : 타겟팅 리셋 테스트
			Reset_TargettingCamera();
		}
	}

	return m_TargetLook;
}

const _bool& CPlayer::Get_KeepTargetLook()
{
	// // O: 여기에 return 문을 삽입합니다.
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (m_TargetLook)
	{
		if (pGameInstance->Key_Pressing(DIK_LSHIFT))
		{
			m_TargetLook = true;
		}
		else
		{
			m_TargetLook = false;

			// todo : 타겟팅 리셋 테스트
			Reset_TargettingCamera();
		}
	}
	RELEASE_INSTANCE(CGameInstance);
	return m_TargetLook;
}

CGameObject* CPlayer::Get_Target()
{
	return m_Target;
}

void CPlayer::Set_TargettingCamera(_Vector4 vTargetPos, _Vector4 vMyPos)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CCamera_Fly* pCamera = static_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(TEXT("Layer_Camera")));

	if (nullptr == pCamera)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	_Vector3 vCameraLook = pCamera->GetLook();
	_Vector3 vCameraLookNoY = vCameraLook;
	vCameraLookNoY.y = 0;
	vCameraLookNoY.Normalize();
	_Vector3 vDistance = vTargetPos - vMyPos;
	_Vector3 vViewportRight = _Vector3(0.f, 1.f, 0.f).Cross(vCameraLook);
	vViewportRight.Normalize();
	_Vector3 vViewportUp = vCameraLook.Cross(vViewportRight);
	vViewportUp.Normalize();
	_float fDistanceCheckVertical = vDistance.Dot(vViewportRight);
	fDistanceCheckVertical = abs(fDistanceCheckVertical - Clamp(fDistanceCheckVertical, -16.f, 16.f)) * 0.66f; // -16~16 : 카메라 멀어짐이 없는 최소범위, 0.66 : 범위 밖에서 카메라 멀어지는 속도
	_float fDistanceCheckHorizontal = vDistance.Dot(vViewportUp);
	fDistanceCheckHorizontal = abs(fDistanceCheckHorizontal - Clamp(fDistanceCheckHorizontal, -8.f, 8.f)) * 1.25f;
	//_Vector3 vDistanceOffset = -vCameraLook * (fDistanceCheckVertical + fDistanceCheckHorizontal);		// 대상과 멀어지면 카메라도 멀어진다
	//_Vector3 vOffset = vDistance / 3.f + vDistanceOffset;

	//pCamera->Set_Offset(vOffset);

	pCamera->Set_Focusing(vMyPos + vDistance / 3.f, fDistanceCheckVertical + fDistanceCheckHorizontal);

	RELEASE_INSTANCE(CGameInstance)
}

void CPlayer::Reset_TargettingCamera()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CCamera_Fly* pCamera = static_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(TEXT("Layer_Camera")));
	if (nullptr != pCamera)
	{
		//pCamera->Set_Offset(_Vector3(0.f, 0.f, 0.f));
		pCamera->Reset_Focusing();
	}
	RELEASE_INSTANCE(CGameInstance)
}

_bool CPlayer::Check_EyesBoundary(float length, float viewAngle, bool reverse)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CGameObject* pTarget = pGameInstance->Get_GameObject(L"Layer_Player", 0);
	RELEASE_INSTANCE(CGameInstance);
	if (!pTarget)
		return false;

	_Vector3 position = m_pTransformCom->Get_State(Engine::STATE::STATE_POSITION);
	_Vector3 targetPosition = pTarget->Get_Transform()->Get_State(Engine::STATE::STATE_POSITION);

	_Vector3 dis = targetPosition - position;
	float magnitude = dis.Length();
	if (magnitude >= length)
		return false;

	_Vector3 forward;
	if (reverse)
	{
		forward = -1 * XMVector3Normalize(m_pTransformCom->Get_State(Engine::STATE::STATE_LOOK));
	}
	else
		forward = XMVector3Normalize(m_pTransformCom->Get_State(Engine::STATE::STATE_LOOK));

	dis.Normalize(dis);

	if (forward.Dot(dis) > cosf(XMConvertToRadians(viewAngle * 0.5f)))
		return true;
	else
		return false;
}


void CPlayer::Key_Check()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// todo : 피격 테스트
	if (pGameInstance->Key_Down(DIK_H) && STATE_HIT != m_pStateMachine->GetCurrentState())
	{
		//Add_PlayerHp(-5.f);
		//m_pStateMachine->SetCurrentState(CPlayer::STATE_HIT);

		Set_Hit((_uint)OBJECT_TYPE::ATTACK, 5);
	}

	_Vector3		vMoveDir = _Vector3(0.f);

	if (pGameInstance->Key_Pressing(DIK_W))
	{
		vMoveDir.z = 1.f;
		m_bMoving = true;
	}
	else if (pGameInstance->Key_Pressing(DIK_S))
	{
		vMoveDir.z = -1.f;
		m_bMoving = true;
	}

	if (pGameInstance->Key_Pressing(DIK_A))
	{
		vMoveDir.x = -1.f;
		m_bMoving = true;
	}
	else if (pGameInstance->Key_Pressing(DIK_D))
	{
		vMoveDir.x = 1.f;
		m_bMoving = true;
	}


	CCamera_Fly* pCam = static_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(TEXT("Layer_Camera")));
	_vector		vCamLook = {};
	if(nullptr != pCam)
		vCamLook = pCam->GetLook();
	_vector		vCamLookNoY = XMVector3Normalize(XMVectorSet(XMVectorGetX(vCamLook), 0.f, XMVectorGetZ(vCamLook), 0.f));
	_vector		vCamLookNoYRev = XMVector3Normalize(vCamLookNoY) * -1.f;

	if (0 < vMoveDir.x)
	{
		if (0 < vMoveDir.z)
		{
			_vector		vCamLookNoYRevRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vCamLookNoYRev));
			_vector		vCamLookNoYRevRightvCamLookNoYRev = XMVector3Normalize(vCamLookNoYRevRight + vCamLookNoYRev) * -1.f;

			m_vMoveTargetDir = vCamLookNoYRevRightvCamLookNoYRev;
		}
		else if (0 > vMoveDir.z)
		{
			_vector		vCamLookNoYRevRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vCamLookNoYRev));
			_vector		vCamLookNoYRevRightMvCamLook = XMVector3Normalize(vCamLookNoYRevRight * -1.f + vCamLookNoYRev);

			m_vMoveTargetDir = vCamLookNoYRevRightMvCamLook;
		}
		else
		{
			_vector		vCamLookNoYRevRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vCamLookNoYRev);

			m_vMoveTargetDir = vCamLookNoYRevRight * -1.f;
		}
	}
	else if (0 > vMoveDir.x)
	{
		if (0 < vMoveDir.z)
		{
			_vector		vCamLookNoYRevRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vCamLookNoYRev));
			_vector		vCamLookNoYRevRightvCamLookNoY = XMVector3Normalize(vCamLookNoYRevRight + vCamLookNoY);

			m_vMoveTargetDir = vCamLookNoYRevRightvCamLookNoY;
		}
		else if (0 > vMoveDir.z)
		{
			_vector		vCamLookNoYRevRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vCamLookNoYRev));
			_vector		vCamLookNoYRevCamLookNoYRevRight = XMVector3Normalize(vCamLookNoYRev + vCamLookNoYRevRight);

			m_vMoveTargetDir = vCamLookNoYRevCamLookNoYRevRight;
		}
		else
		{
			_vector		vCamLookNoYRevRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vCamLookNoYRev);

			m_vMoveTargetDir = vCamLookNoYRevRight;
		}
	}
	else if (0 > vMoveDir.z)
	{
		m_vMoveTargetDir = vCamLookNoYRev;
	}
	else if (0 < vMoveDir.z)
	{
		m_vMoveTargetDir = vCamLookNoY;
	}
	else
	{
		m_vMoveTargetDir = _Vector3(0.f);
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::HitFlashing(_float fTimeDelta)
{
	if (m_bHitFlashing)
	{
		m_HitFlashDuration.Add_Time(fTimeDelta);

		if (m_HitFlashDuration.Check_End())
		{
			m_bHitFlashing = false;
			m_bHitFlashColorToggle = false;
			m_HitFlashDuration.Reset();
			m_HitFlashGap.Reset();
		}

		m_HitFlashGap.Add_Time(fTimeDelta);

		if (m_HitFlashGap.Check_End())
		{
			m_bHitFlashColorToggle = !m_bHitFlashColorToggle;
			m_HitFlashGap.Reset();
		}
	}
}

void CPlayer::Set_HitFlashing()
{
	m_bHitFlashing = true;
	m_bHitFlashColorToggle = false;
	m_HitFlashDuration.Reset();
	m_HitFlashGap.Reset();
}

#pragma endregion
HRESULT CPlayer::SetUp_Component()
{
	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 6.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(45.0f);

	if (FAILED(__super::Add_Component_STATIC(TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component_STATIC(TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Model_Player"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;
// 
// 	/* For.Com_Navi */
// 	if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Navigation"), TEXT("Com_Navi"), (CComponent**)&m_pNaviCom)))
// 		return E_FAIL;
	
	/* For.Com_StateMachine */
	if (FAILED(__super::Add_Component(TEXT("Prototype_Component_StateMachine"), TEXT("Com_StateMachine"), (CComponent**)&m_pStateMachine)))
		return E_FAIL;

	/* For.Com_Animator */
	if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Animator"), TEXT("Com_Animator"), (CComponent**)&m_pAnimatorCom)))
		return E_FAIL;

	/* For.Com_RigidBody */
	CRigid::PUSHRIGIDDATA Data;
	if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Rigid"), TEXT("Com_RigidBody"), (CComponent**)&m_pRigid)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Shader_AnimMesh"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);	

	m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix");
	pGameInstance->Bind_Transform_OnShader(TS_VIEW, m_pShaderCom, "g_ViewMatrix");
	pGameInstance->Bind_Transform_OnShader(TS_PROJ, m_pShaderCom, "g_ProjMatrix");

	m_pShaderCom->Set_RawValue("g_LightViewMatrix", &pGameInstance->Get_LightTransform(TS_VIEW), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_LightProjMatrix", &pGameInstance->Get_LightTransform(TS_PROJ), sizeof(_float4x4));

	pGameInstance->Bind_Transform_OnShader(TS_VIEW, m_pShaderCom, "g_ViewMatrixInverse", true);
	pGameInstance->Bind_Transform_OnShader(TS_PROJ, m_pShaderCom, "g_ProjMatrixInverse", true);

	m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4));
	

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CPlayer * CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CPlayer*		pInstance = new CPlayer(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CFork");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayer::Clone(void* pArg)
{
	CPlayer*		pInstance = new CPlayer(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	Safe_Release(m_pInventory);

	__super::Free();
	// 	Safe_Release(m_pTransformCom);
	// 	Safe_Release(m_pShaderCom);
	
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRigid);
	Safe_Release(m_pStateMachine);
	Safe_Release(m_pAnimatorCom);

	//-----------------------------------------------------------------------------------
	// m_pStatus_Player 얘도 CBase를 상속받지 않기 때문에 이렇게 Release 해줘야 합니다~
	//-----------------------------------------------------------------------------------
	Safe_Delete(m_pStatus_Player);	
}
