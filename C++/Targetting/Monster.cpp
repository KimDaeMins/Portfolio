#include "stdafx.h"
#include "Monster.h"
#include "GameInstance.h"
#include "Weapon.h"
#include "MonsterHitPoint.h"
#include "Alert.h"
#include "Coin.h"
#include "DropItem.h"
#include "WaterBall.h"
#include "Pong.h"
#include "Effect_SoulBright.h"
CMonster::CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CMonster::CMonster(const CMonster& rhs)
	: CGameObject(rhs)
{
	m_HitFlashDuration = CStopWatch(0.3f);
	m_HitFlashGap = CStopWatch(0.02f);
}

_bool CMonster::Check_Boundary(float length)
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

	dis.Normalize(m_vDir);

	if (magnitude <= length)
		return true;
	else
		return false;
}
_bool CMonster::Check_EyesBoundary(float length, float viewAngle)
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
	forward = XMVector3Normalize(m_pTransformCom->Get_State(Engine::STATE::STATE_LOOK));

	dis.Normalize(dis);

	if (forward.Dot(dis) > cosf(XMConvertToRadians(viewAngle * 0.5f)))
		return true;
	else
		return false;
}
_bool CMonster::Check_EyesBoundary(float viewAngle, _bool reverse)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CGameObject* pTarget = pGameInstance->Get_GameObject(L"Layer_Player");
	RELEASE_INSTANCE(CGameInstance);
	if (!pTarget)
		return false;

	if (reverse)
	{
		_Vector3 position = m_pTransformCom->Get_State(Engine::STATE::STATE_POSITION);
		_Vector3 targetPosition = pTarget->Get_Transform()->Get_State(Engine::STATE::STATE_POSITION);

		_Vector3 dis = targetPosition - position;
		_Vector3 backYard = -1 * XMVector3Normalize(m_pTransformCom->Get_State(Engine::STATE::STATE_LOOK));

		dis.Normalize();

		if (backYard.Dot(dis) > cosf(XMConvertToRadians(viewAngle * 0.5f)))
			return true;
		else
			return false;
	}
	else
	{
		_Vector3 position = m_pTransformCom->Get_State(Engine::STATE::STATE_POSITION);
		_Vector3 targetPosition = pTarget->Get_Transform()->Get_State(Engine::STATE::STATE_POSITION);

		_Vector3 dis = targetPosition - position;
		_Vector3 forward = XMVector3Normalize(m_pTransformCom->Get_State(Engine::STATE::STATE_LOOK));;

		dis.Normalize();

		if (forward.Dot(dis) > cosf(XMConvertToRadians(viewAngle * 0.5f)))
			return true;
		else
			return false;
	}
}
_bool CMonster::Check_Right()
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
	CGameObject* pTarget = pInstance->Get_GameObject(TEXT("Layer_Player"));
	RELEASE_INSTANCE(CGameInstance);

	Vector3 vTargetPos = pTarget->Get_Transform()->Get_State(STATE_POSITION);
	Vector3 vMyPos = m_pTransformCom->Get_State(STATE_POSITION);
	Vector3 look = XMVector3Normalize(m_pTransformCom->Get_State(STATE_LOOK));
	Vector3 vDis = vTargetPos - vMyPos;
	vDis.Normalize();
	//m_pTransformCom->LookAtDir(vDis);

	Vector3 cross = look.Cross(vDis);
	return cross.Dot(Vector3(0, 1, 0)) > 0;
}
_bool CMonster::Check_Foward()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CGameObject* pTarget = pGameInstance->Get_GameObject(L"Layer_Player");
	RELEASE_INSTANCE(CGameInstance);
	if (!pTarget)
		return false;

	_Vector3 position = m_pTransformCom->Get_State(Engine::STATE::STATE_POSITION);
	_Vector3 targetPosition = pTarget->Get_Transform()->Get_State(Engine::STATE::STATE_POSITION);

	_Vector3 dis = targetPosition - position;
	_Vector3 forward;
	forward = XMVector3Normalize(m_pTransformCom->Get_State(Engine::STATE::STATE_LOOK));

	dis.Normalize(dis);

	if (forward.Dot(dis) > 0)
		return true;
	else
		return false;
}
_bool CMonster::Check_Collsion(CGameObject*& pObj)
{
	while (m_pRigid->Get_Collision_Trigger_Enter(&pObj))
	{
		CMonsterHitPoint* pHitPoint = dynamic_cast<CMonsterHitPoint*>(pObj);
		if (pHitPoint != nullptr && pHitPoint->Get_HostObj() == this)
			return false;
		if (pObj->Get_Type() == (_int)OBJECT_TYPE::ATTACK)
			return true;
	}

	return false;
}
CMonster::POSITION_TYPE CMonster::Check_PositionType(_bool horizontal)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CGameObject* pTarget = pGameInstance->Get_GameObject(L"Layer_Player");
	RELEASE_INSTANCE(CGameInstance);
	if (!pTarget)
		return POSITION_TYPE::ERR;
	Vector3 vTargetPos = pTarget->Get_Transform()->Get_State(STATE_POSITION);
	Vector3 vMyPos = m_pTransformCom->Get_State(STATE_POSITION);
	Vector3 look = XMVector3Normalize(m_pTransformCom->Get_State(STATE_LOOK));
	Vector3 vDis = vTargetPos - vMyPos;
	vDis.Normalize();
	if (horizontal)
	{
		float dot = look.Dot(vDis) > 0;
		if (dot > 0)
			return CMonster::POSITION_TYPE::FORWARD;
		else if (dot == 0)
		{
			return CMonster::POSITION_TYPE::STRAIGHT;
		}
		else
			return CMonster::POSITION_TYPE::BACKYARD;
	}
	else
	{
		Vector3 cross = look.Cross(vDis);
		float dot = cross.Dot(Vector3(0, 1, 0));
		if (dot > 0)
		{
			return CMonster::POSITION_TYPE::RIGHT;
		}
		else if (dot == 0)
		{
			return CMonster::POSITION_TYPE::STRAIGHT;
		}
		else
			return CMonster::POSITION_TYPE::LEFT;
	}
}
void CMonster::Set_Dir()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CGameObject* pTarget = pGameInstance->Get_GameObject(TEXT("Layer_Player"), 0);
	RELEASE_INSTANCE(CGameInstance);
	if (!pTarget)
		return;
	Vector3 position = m_pTransformCom->Get_State(Engine::STATE::STATE_POSITION);
	Vector3 targetPosition = pTarget->Get_Transform()->Get_State(Engine::STATE::STATE_POSITION);
	Vector3 dis = targetPosition - position;
	dis.Normalize(m_vDir);
	m_pTransformCom->LookAtDir((m_vDir));
}

void CMonster::Set_LerpDir(float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CGameObject* pTarget = pGameInstance->Get_GameObject(TEXT("Layer_Player"), 0);
	RELEASE_INSTANCE(CGameInstance);
	if (!pTarget)
		return;
	Vector3 position = m_pTransformCom->Get_State(Engine::STATE::STATE_POSITION);
	Vector3 targetPosition = pTarget->Get_Transform()->Get_State(Engine::STATE::STATE_POSITION);
	Vector3 dis = targetPosition - position;
	dis.Normalize(m_vDir);

	float yAngle = atan2f(m_vDir.x, m_vDir.z);
	Matrix worldMat = m_pTransformCom->Get_WorldMatrix();
	Vector3 myAngles = RotationMatrixToRadians(m_pTransformCom->Get_WorldMatrix());
	Quaternion slerp = Quaternion::Slerp(Quaternion::CreateFromYawPitchRoll(myAngles.y, myAngles.x, myAngles.z), Quaternion::CreateFromYawPitchRoll(yAngle, myAngles.x, myAngles.z), fTimeDelta);
	Matrix rotationMatrix = Matrix::CreateFromQuaternion(slerp);
	m_pTransformCom->Set_Rotate(rotationMatrix.Right(), rotationMatrix.Up(), rotationMatrix.Forward());
}

void CMonster::Set_LerpDir2(float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CGameObject* pTarget = pGameInstance->Get_GameObject(TEXT("Layer_Player"), 0);
	RELEASE_INSTANCE(CGameInstance);
	if (!pTarget)
		return;
	Vector3 position = m_pTransformCom->Get_State(Engine::STATE::STATE_POSITION);
	Vector3 targetPosition = pTarget->Get_Transform()->Get_State(Engine::STATE::STATE_POSITION);
	Vector3 dis = targetPosition - position;
	Vector3 look = XMVector3Normalize(m_pTransformCom->Get_State(STATE_LOOK));

	dis.Normalize();
	
	float cosf = look.Dot(dis);
	float acosf = ::acosf(cosf);

	if (acosf > 0.2f)
	{
		Vector3 cross = look.Cross(dis);
		if (cross.Dot(Vector3(0, 1, 0)) > 0)
		{
			m_pTransformCom->Turn(_Vector3(0, 1, 0), fTimeDelta);
		}
		else
		{
			m_pTransformCom->Turn(_Vector3(0, 1, 0), -fTimeDelta);
		}
	}
}

void CMonster::Create_Alert(float offset)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	static_cast<CAlert*>(pGameInstance->Add_GameObjectToLayerAndReturn(TEXT("Layer_Alert"), TEXT("Prototype_GameObject_Effect_Alert"), this))->Set_YOffset(offset);
	RELEASE_INSTANCE(CGameInstance);
}

_int CMonster::Tick(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CTransform* pTrans = (CTransform*)pGameInstance->Get_ComponentPtr(L"Layer_Player", L"Com_Transform");
	_Vector3 Target;
	if(nullptr != pTrans)
		Target = pTrans->Get_State(STATE_POSITION);
	_Vector3 Position = m_pTransformCom->Get_State(STATE_POSITION);
	/*Position = Vec3Normalize(Target - Position);*/
	m_DistanceToPlayer = (Target - Position).Length();

	if (XMVectorGetY(m_pTransformCom->Get_State(STATE_POSITION)) < -200.f)
		m_bDead = true;
	RELEASE_INSTANCE(CGameInstance);
	HitFlashing(fTimeDelta);
	return _int();
}
_int CMonster::LateTick(_float fTimeDelta)
{
	if (m_fEvadingTime >= m_fEvaingMaxTime)
	{
		return _int();
	}
	else
	{
		m_fEvadingTime += fTimeDelta;
		return -1;
	}
}
void CMonster::HitFlashing(_float fTimeDelta)
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

void CMonster::Set_HitFlashing()
{
	m_bHitFlashing = true;
	m_bHitFlashColorToggle = false;
	m_HitFlashDuration.Reset();
	m_HitFlashGap.Reset();
}

void CMonster::Create_Gold(_uint iSmallGoldCount, _uint iMediumGoldCount, _uint iBigGoldCount, _Vector3 vOffset)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	vector<CCoin::COINTYPE>		CoinTable;
	_uint		iCurMediumGold = 0, iCurBigGold = 0;
	_uint		iTotalGoldCount = iSmallGoldCount + iMediumGoldCount + iBigGoldCount;
	_float		fBigGoldRatio = (_float)iBigGoldCount / iTotalGoldCount;
	_float		fMediumGoldRatio = (_float)iMediumGoldCount / iTotalGoldCount;

	for (_uint i = 0; i < iTotalGoldCount; ++i)
	{
		_float		fRandom = pGameInstance->Range_Float(0, 1);
		if (fBigGoldRatio > fRandom && iBigGoldCount > iCurBigGold)
		{
			CoinTable.push_back(CCoin::COINTYPE::BIG);
			++iCurBigGold;
		}
		else if (fMediumGoldRatio > fRandom && iMediumGoldCount > iCurMediumGold)
		{
			CoinTable.push_back(CCoin::COINTYPE::MEDIUM);
			++iCurMediumGold;
		}
		else
		{
			CoinTable.push_back(CCoin::COINTYPE::SMALL);
		}
	}
	_uint i = iTotalGoldCount - 1;
	while (iBigGoldCount > iCurBigGold)
	{
		if (CCoin::COINTYPE::SMALL == CoinTable[i])
		{
			CoinTable[i] = CCoin::COINTYPE::BIG;
			++iCurBigGold;
		}
		--i;

		if (0 == i)
			break;
	}
	while (iMediumGoldCount > iCurMediumGold)
	{
		if (CCoin::COINTYPE::SMALL == CoinTable[i])
		{
			CoinTable[i] = CCoin::COINTYPE::MEDIUM;
			++iCurMediumGold;
		}
		--i;

		if (0 == i)
			break;
	}

	//코인생성
	for (_uint i = 0; i < iTotalGoldCount; ++i)
	{
		_Vector3 Force = _Vector3(0.4f, 0.8f, 0.f);
		_Matrix RotationMatrix = _Matrix::CreateFromAxisAngle(_Vector3(0.f, 1.f, 0.f), (_float)XMConvertToRadians(i * 360.f / iTotalGoldCount));

		Force = _Vector3::Transform(Force, RotationMatrix);
		_Vector3 Position = _Vector3(m_pTransformCom->Get_State(STATE_POSITION)) + vOffset;

		CCoin::PUSHCOINDATA Data;
		Data.Force = Force;
		Data.Position = Position;
		Data.eCoinType = CoinTable[i];
		pGameInstance->Add_GameObjectToLayer(TEXT("Layer_Object"), TEXT("Prototype_GameObject_Coin"), &Data);
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CMonster::Create_Heart()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (pGameInstance->Get_Random(75.f))
		return;

	_Vector3 Force = _Vector3(0.1f, 0.6f, 0.f);
	_Matrix RotationMatrix = _Matrix::CreateFromAxisAngle(_Vector3(0.f, 1.f, 0.f), (_float)XMConvertToRadians(pGameInstance->Range_Float(0, 360)));

	Force = _Vector3::Transform(Force, RotationMatrix);
	_Vector3 Position = _Vector3(m_pTransformCom->Get_State(STATE_POSITION));
	Position.y += 0.1f;

	CDropItem::PUSHDROPITEMDATA Data;
	Data.Force = Force;
	Data.Position = Position;
	Data.eType = CDropItem::HP;
	pGameInstance->Add_GameObjectToLayer(TEXT("Layer_Object"), TEXT("Prototype_GameObject_DropItem"), &Data);
	RELEASE_INSTANCE(CGameInstance);
}

void CMonster::Create_Soul()
{
	if (m_DropType == MANA)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		for (_uint i = 0; i < 5; ++i)
		{
			_Vector3 Force = _Vector3(0.6f, 0.4f, 0.f);
			_Matrix RotationMatrix = _Matrix::CreateFromAxisAngle(_Vector3(0.f, 1.f, 0.f), (_float)XMConvertToRadians(pGameInstance->Range_Float(0, 360)));

			Force = _Vector3::Transform(Force, RotationMatrix);
			_Vector3 Position = _Vector3(m_pTransformCom->Get_State(STATE_POSITION));
			Position.y += 0.3f;

			CDropItem::PUSHDROPITEMDATA Data;
			Data.Force = Force;
			Data.Position = Position;
			Data.eType = CDropItem::MP;
			pGameInstance->Add_GameObjectToLayer(TEXT("Layer_Object"), TEXT("Prototype_GameObject_DropItem"), &Data);
		}
		RELEASE_INSTANCE(CGameInstance);
	}
}

void CMonster::DeadEffect(_uint EffectType)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CWaterBall::PUSHWATERBALLDATA WaterBallData;
	CPong::PUSHPONGDATA PongData;
	CSoulBright::PUSHSOULBRIGHTDATA SoulBrightData;
	switch (EffectType)
	{
	case 0:
		pGameInstance->Play(L"en_shared_deathpoof_", MULTY, 5, 3);
		WaterBallData.Diffuse = { 0.5f, 0.75f, 1.f ,1.f };
		WaterBallData.MainPos = m_pTransformCom->Get_State(STATE_POSITION);
		WaterBallData.NumWaterBall = 35;
		if (FAILED(pGameInstance->Add_GameObjectToLayer(L"Layer_Effect", L"Prototype_GameObject_WaterBallEffect", &WaterBallData)))
			MSG_BOX("?");
		break;
	case 1:
		pGameInstance->Play(L"en_shared_deathpoof_", MULTY, 5, 3);
		PongData.Diffuse = { 1.f, 1.f, 1.f, 1.f };
		PongData.MainPos = m_pTransformCom->Get_State(STATE_POSITION);
		PongData.NumPong = 36;
		if (FAILED(pGameInstance->Add_GameObjectToLayer(L"Layer_Effect", L"Prototype_GameObject_PongEffect", &PongData)))
			MSG_BOX("?");
	case 2:
		pGameInstance->Play(L"en_shared_death", MULTY, 5, 1);
		SoulBrightData.HostObject = nullptr;
		SoulBrightData.vColor = _float4(0.78f, 0.24f, 0.651f, 1.f);
		SoulBrightData.StartScale = 0.1f;
		SoulBrightData.EndScale = 6.f;
		SoulBrightData.bBillBoard = true;
		SoulBrightData.vPosition = _Vector4(m_pRigid->Get_ActorCenter(), 1.f);
		pGameInstance->Add_GameObjectToLayer(L"Layer_Effect", L"Prototype_GameObject_SoulBright", &SoulBrightData);
		break;
	default:
		break;
	}
	RELEASE_INSTANCE(CGameInstance);
}

const _float& CMonster::Get_DistanceToPlayer()
{
	// // O: 여기에 return 문을 삽입합니다.
	return m_DistanceToPlayer;
}

_uint CMonster::Get_DropType()
{
	return m_DropType;
}

_uint CMonster::Get_DropLevel()
{
	return m_DropLevel;
}

void CMonster::Set_Drag(_bool Drag)
{
	if (m_bIsDragAble)
	{
		m_bIsDragged = Drag;
	}
}

const _bool& CMonster::Get_Drag()
{
	// // O: 여기에 return 문을 삽입합니다.
	if (m_bIsDragAble)
	{
		return m_bIsDragged;
	}

	return m_bIsDragAble;
}
