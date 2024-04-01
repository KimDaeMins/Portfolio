#include "stdafx.h"
#include "..\public\Player.h"

#include "Part_Arm.h"
#include "Part_Body.h"
#include "Part_Leg.h"
#include "Part_Head.h"
#include "Gun.h"
#include "Blade.h"
#include "ArmRocket.h"
#include "Camera_Fly.h"

CPlayer::CPlayer(ID3D11Device * _pDevice, ID3D11DeviceContext * _pDeviceContext)
	:CGameObject(_pDevice, _pDeviceContext)
{
}

CPlayer::CPlayer(const CPlayer & rhs)
	: CGameObject(rhs)
	, m_Parts(rhs.m_Parts)
{
}

HRESULT CPlayer::NativeConstruct_Prototype()
{

	return S_OK;
}

HRESULT CPlayer::NativeConstruct(void* _pArg)
{
	if (FAILED(__super::NativeConstruct(_pArg)))
		return E_FAIL;

	SetUp_Component();
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	m_Parts.resize(PART_END);

	m_Parts[PART_ARM] = (CPart*)pGameInstance->Get_GameObjectPtr(LEVEL_GAMEPLAY, TEXT("Layer_Inven"), 0);
	m_Parts[PART_BODY] = (CPart*)pGameInstance->Get_GameObjectPtr(LEVEL_GAMEPLAY, TEXT("Layer_Inven"), 1);
	m_Parts[PART_HEAD] = (CPart*)pGameInstance->Get_GameObjectPtr(LEVEL_GAMEPLAY, TEXT("Layer_Inven"), 2);
	m_Parts[PART_LEG] = (CPart*)pGameInstance->Get_GameObjectPtr(LEVEL_GAMEPLAY, TEXT("Layer_Inven"), 3);

	RELEASE_INSTANCE(CGameInstance);
	//50, 90
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));


	//������ �÷��̾�� �ι� �ȸ���ϱ�
	{
	m_pModelCom->MaterialInput(0, 0, TEXT("../Bin/Resources/Meshes/Arsenal/AS_Body_000_M.tga"));
	m_pModelCom->MaterialInput(0, 3, TEXT("../Bin/Resources/Meshes/Arsenal/AS_Body_000_ORM.tga"));
	m_pModelCom->MaterialInput(0, 4, TEXT("../Bin/Resources/Meshes/Arsenal/AS_Body_000_E.tga"));
	m_pModelCom->MaterialInput(0, 6, TEXT("../Bin/Resources/Meshes/Arsenal/AS_Body_000_N.tga"));

	m_pModelCom->MaterialInput(1, 0, TEXT("../Bin/Resources/Meshes/Arsenal/AS_WeaponRack_000_M.tga"));
	m_pModelCom->MaterialInput(1, 3, TEXT("../Bin/Resources/Meshes/Arsenal/AS_WeaponRack_000_ORM.tga"));
	m_pModelCom->MaterialInput(1, 4, TEXT("../Bin/Resources/Meshes/Arsenal/AS_WeaponRack_000_E.tga"));
	m_pModelCom->MaterialInput(1, 6, TEXT("../Bin/Resources/Meshes/Arsenal/AS_WeaponRack_000_N.tga"));

	m_pModelCom->MaterialInput(2, 0, TEXT("../Bin/Resources/Meshes/Arsenal/AS_Hip_000_M.tga"));
	m_pModelCom->MaterialInput(2, 3, TEXT("../Bin/Resources/Meshes/Arsenal/AS_Hip_000_ORM.tga"));
	m_pModelCom->MaterialInput(2, 4, TEXT("../Bin/Resources/Meshes/Arsenal/AS_Hip_000_E.tga"));
	m_pModelCom->MaterialInput(2, 6, TEXT("../Bin/Resources/Meshes/Arsenal/AS_Hip_000_N.tga"));

	m_pModelCom->MaterialInput(3, 0, TEXT("../Bin/Resources/Meshes/Arsenal/AS_Arm_000_M.tga"));
	m_pModelCom->MaterialInput(3, 3, TEXT("../Bin/Resources/Meshes/Arsenal/AS_Arm_000_ORM.tga"));
	m_pModelCom->MaterialInput(3, 4, TEXT("../Bin/Resources/Meshes/Arsenal/AS_Arm_000_E.tga"));
	m_pModelCom->MaterialInput(3, 6, TEXT("../Bin/Resources/Meshes/Arsenal/AS_Arm_000_N.tga"));

	m_pModelCom->MaterialInput(4, 0, TEXT("../Bin/Resources/Meshes/Arsenal/AS_Leg_000_M.tga"));
	m_pModelCom->MaterialInput(4, 3, TEXT("../Bin/Resources/Meshes/Arsenal/AS_Leg_000_ORM.tga"));
	m_pModelCom->MaterialInput(4, 4, TEXT("../Bin/Resources/Meshes/Arsenal/AS_Leg_000_E.tga"));
	m_pModelCom->MaterialInput(4, 6, TEXT("../Bin/Resources/Meshes/Arsenal/AS_Leg_000_N.tga"));

	m_pModelCom->MaterialInput(5, 0, TEXT("../Bin/Resources/Meshes/Arsenal/AS_Head_000_M.tga"));
	m_pModelCom->MaterialInput(5, 3, TEXT("../Bin/Resources/Meshes/Arsenal/AS_Head_000_ORM.tga"));
	m_pModelCom->MaterialInput(5, 4, TEXT("../Bin/Resources/Meshes/Arsenal/AS_Head_000_E.tga"));
	m_pModelCom->MaterialInput(5, 6, TEXT("../Bin/Resources/Meshes/Arsenal/AS_Head_000_N.tga"));
}
	//���׸���������!


	Create_Collider();
	return S_OK;
}

_int CPlayer::Tick(_float fTimeDelta)
{
	if (m_Dead)
		return EVENT_DEAD;

	if (0 > __super::Tick(fTimeDelta))
		return -1;

	if (!m_Once)
	{
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
		//50, 90
		m_Weapons.resize(4);
		m_Weapons[0] = (CWeapon*)pGameInstance->Get_GameObjectPtr(LEVEL_GAMEPLAY, TEXT("Layer_Weapon"), 0);
		m_Weapons[0]->Weapon_Equip(HAND_L);
		m_Weapons[1] = (CWeapon*)pGameInstance->Get_GameObjectPtr(LEVEL_GAMEPLAY, TEXT("Layer_Weapon"), 1);
		m_Weapons[1]->Weapon_Equip(HAND_R);
		m_Weapons[2] = (CWeapon*)pGameInstance->Get_GameObjectPtr(LEVEL_GAMEPLAY, TEXT("Layer_Weapon"), 2);
		m_Weapons[2]->Weapon_Equip(HAND_L);

		RELEASE_INSTANCE(CGameInstance);
		m_Once = true;
	}


	Key_Input(fTimeDelta);

	return _int();
}

_int CPlayer::LateTick(_float fTimeDelta)
{
	if (0 > __super::LateTick(fTimeDelta))
		return -1;


	if (m_BodyState == NOATTACK && m_LegState == IDLE)
	{
		m_LegLoopAnim = true;
		if (m_SpaceState == SPACE_AIR)
			m_LegAnimationIndex = AIRWAIT_1;
		else
			m_LegAnimationIndex = WAIT;
	}
	if (m_BodyState == MOVEATTACK && m_LegState == IDLE)
	{
		//�ִϸ��̼��� ��ģ�� but �Ⱦִϸ��̼��� ���� �Ⱦִϸ��̼� ��->��ü���۳�
		//MoveAttack �� NoAttack���� �ٲ�� í�� Wait���·� �ٲ��.
		if (m_LeftArmUpdate)
			m_LegAnimationIndex = m_LeftArmAnimationIndex;
		else if (m_RightArmUpdate)
			m_LegAnimationIndex = m_RightArmAnimationIndex;
	}
	m_pModelCom->SetUp_Animation(m_LegAnimationIndex, m_isBlending , m_LegLoopAnim);
	m_pModelCom->SetUp_ArmAction(m_LeftArmAnimationIndex, m_RightArmAnimationIndex, m_LeftArmUpdate, m_RightArmUpdate);
	m_pModelCom->Update_Animation(fTimeDelta);
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);

	if (m_SpaceState == SPACE_LAND && !m_Jump)
	{
		_vector		vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(vPosition, 1.25f));
	}

	RELEASE_INSTANCE(CGameInstance);
	return _int();
}

_int CPlayer::SetTick(_float fTimeDelta)
{
	if (!m_Parts.size())
		return _int();
	//�������� ������ �ν��� ����, �׼ǻ����ϋ� �ν��� ����
	if (m_LegState == IDLE || m_LegState == ACTION)
	{
		dynamic_cast<CPart_Body*>(m_Parts[PART_BODY])->BoosterEnd();
	}
	//�ν��� ���� ��������
	m_Booster = dynamic_cast<CPart_Body*>(m_Parts[PART_BODY])->Get_BoosterUse();


	//�ö��̻����ε� ���ö��̰� false -> SPACE_LAND�ιٲ�
	if (m_SpaceState == SPACE_AIR)
	{
		if (!dynamic_cast<CPart_Body*>(m_Parts[PART_BODY])->Get_Fly())
		{
			m_SpaceState = SPACE_LAND;
			m_Jump = true;
			m_LegLoopAnim = true;
			m_JumpMotionState = MOTION_ED;
			m_LegAnimationIndex = JUMP_DOWN_LP;
			m_LegState = MOVE_SLIDE;
			m_JumpPower = 0.f;
			m_isBlending = false;
		}
	}

	if (!m_pModelCom->Get_isBlending())
	{
		m_BeforeLegState = m_LegState;
		m_BeforeLegMotionState = m_LegMotionState;
	}

	return _int();
}

HRESULT CPlayer::Render(_float fTimeDelta)
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	if (0 > __super::Render(fTimeDelta))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	_uint		iNumMaterials = m_pModelCom->Get_NumMaterials();

	for (_uint i = 0; i < iNumMaterials; ++i)
	{
		m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", i, aiTextureType_DIFFUSE);
		m_pModelCom->Set_ShaderResourceView("g_NormalTexture", i, aiTextureType_NORMALS);
		m_pModelCom->Set_ShaderResourceView("g_EmissiveTexture", i, aiTextureType_EMISSIVE);

		m_pModelCom->Render(i, 2);
	}
	return S_OK;
}

HRESULT CPlayer::RenderLightDepth()
{
	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	pGameInstance->Bind_LightMatrix_OnShader(CLight_Manager::TYPE_VIEW, m_pModelCom, "g_LightViewMatrix");
	pGameInstance->Bind_LightMatrix_OnShader(CLight_Manager::TYPE_PROJ, m_pModelCom, "g_LightProjMatrix");
	RELEASE_INSTANCE(CGameInstance);

	_uint	iNumMaterials = m_pModelCom->Get_NumMaterials();

	for (_uint i = 0; i < iNumMaterials; ++i)
	{
		m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", i, aiTextureType_DIFFUSE);

		m_pModelCom->Render(i, 5);
	}

	return S_OK;
}

void CPlayer::Key_Input(_float fTimeDelta)
{
	//Q�� ������ ���� �� �ִ�. �� ���´���� �ִ°�쿡��?
	QAttack(fTimeDelta);
	//��ü�׼��� �������� �ִϸ��̼��� ���ߴ� Action ���¸� ������ �������� ���밡��
	if (m_BodyState != ATTACK && m_LegMotionState != MOTION_ST)
	{
		Key_Input_BodyAction(fTimeDelta);
	}
	//���������Ʈ
	WeaponAction(fTimeDelta);
	Chase(fTimeDelta);
	if (m_LegState != ACTION)
	{
		//ȸ��, ������, ���ǵ� �Ѱ�
		Jumping(fTimeDelta);
		Key_Input_Move(fTimeDelta);
		if(!m_Jump)
		{
			Speed_Setting(fTimeDelta);
			Move(fTimeDelta);
			Turn(fTimeDelta);
			MoveControl(fTimeDelta);
		}
	}
}

void CPlayer::Key_Input_Move(_float fTimeDelta)
{
	if (m_pModelCom->Get_isBlending())
		return;
	//���������� �ִϸ��̼ǋ�����.. //�׼ǻ��¿��� ���ʿ� �ȵ����ϱ� �׳� �̻��·� �̿��Ѵ�.
	m_MoveInput = false;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CTransform* CamRotation = (CTransform*)pGameInstance->Get_ComponentPtr(LEVEL_GAMEPLAY, TEXT("Layer_Camera"), TEXT("Com_Rotation_Transform"));
	_float4x4 RotationMatrix;
	XMStoreFloat4x4(&RotationMatrix, CamRotation->Get_WorldMatrix());
	RotationMatrix._12 = 0;
	RotationMatrix._21 = 0;
	RotationMatrix._22 = 1;
	RotationMatrix._23 = 0;
	RotationMatrix._32 = 0;
	RotationMatrix._33 = RotationMatrix._11;

	m_pTransformCom->Set_Rotate(RotationMatrix);
	//�ν��� �̰� �����̵� �����̵� ����.
	//�ν��ʹ� ������ �� ���� �޽�
	if (pGameInstance->Get_KeyDown(DIK_LSHIFT))
	{
		m_Booster = dynamic_cast<CPart_Body*>(m_Parts[PART_BODY])->Booster();
	}
	else if (pGameInstance->Get_KeyUp(DIK_LSHIFT))
	{
		dynamic_cast<CPart_Body*>(m_Parts[PART_BODY])->BoosterEnd();
		m_Booster = false;
	}

	//�ӽ��ڵ�
	if (m_Booster)
		m_Run = true;
	else
		m_Run = false;

	//���� ����
	if (pGameInstance->Get_KeyDown(DIK_SPACE) && m_SpaceState == SPACE_LAND && m_Jump)
	{
		if (dynamic_cast<CPart_Body*>(m_Parts[PART_BODY])->Fly())
		{
			m_SpaceState = SPACE_AIR;
			m_LegAnimationIndex = AIRWAIT_1;
			m_JumpMotionState = MOTION_NONE;
			m_isBlending = true;
			m_LegLoopAnim = true;
			m_LegState = IDLE;
			m_Jump = false;
		}
	}
	else if (pGameInstance->Get_KeyDown(DIK_LCONTROL) && m_SpaceState == SPACE_AIR)
	{
		dynamic_cast<CPart_Body*>(m_Parts[PART_BODY])->Fly_End();
		m_Jump = true;
		m_SpaceState = SPACE_LAND;
		m_LegLoopAnim = true;
		m_JumpMotionState = MOTION_ED;
		m_LegAnimationIndex = JUMP_DOWN_LP;
		m_LegState = MOVE_SLIDE;
		m_JumpPower = 0.f;
		m_isBlending = false;
	}

	//���ٿ� ���� �⺻ AIR���¿����� ����
	if (m_SpaceState == SPACE_AIR)
	{
		if (pGameInstance->Get_KeyPressing(DIK_SPACE))
		{
			m_LegState = MOVE_UP;
			m_MoveInput = true;
			//���⼭ y�� ���� �־��ش�.
			m_pTransformCom->Move(CTransform::MOVE_UP, fTimeDelta);
		}
	}

	//���� ����
	if (pGameInstance->Get_KeyDown(DIK_SPACE) && m_SpaceState == SPACE_LAND && m_Parts[PART_LEG]->Get_PartUseAble())
	{
		m_Jump = true;
		//���߿������ؾߵ�pp
		m_LegState = MOVE_SLIDE;
		m_LegMotionState = MOTION_NONE;
	}


	//pp �����̵� ��ø��ϰԸ���
	if (m_BeforeLegMotionState != MOTION_ED && !m_Jump)
	{
		if (pGameInstance->Get_KeyPressing(DIK_W))
		{
			if (pGameInstance->Get_KeyPressing(DIK_D))
			{
				m_LegState = MOVE_RF;
			}
			else if (pGameInstance->Get_KeyPressing(DIK_A))
			{
				m_LegState = MOVE_LF;
			}
			else
			{
				m_LegState = MOVE_F;
			}
			m_MoveInput = true;
		}
		else if (pGameInstance->Get_KeyPressing(DIK_S))
		{
			if (pGameInstance->Get_KeyPressing(DIK_D))
			{
				m_LegState = MOVE_RB;
			}
			else if (pGameInstance->Get_KeyPressing(DIK_A))
			{
				m_LegState = MOVE_LB;
			}
			else
			{
				m_LegState = MOVE_B;
			}
			m_MoveInput = true;
		}
		else if (pGameInstance->Get_KeyPressing(DIK_D))
		{
			m_LegState = MOVE_R;
			m_MoveInput = true;
		}
		else if (pGameInstance->Get_KeyPressing(DIK_A))
		{
			m_LegState = MOVE_L;
			m_MoveInput = true;
		}
	}
	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::Key_Input_BodyAction(_float fTimeDelta)
{
	if (m_pModelCom->Get_isBlending())
		return;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Get_KeyPressing(CInput_Device::MSB_LBUTTON))
	{
		m_LAttack = true;
	}
	if (pGameInstance->Get_KeyPressing(CInput_Device::MSB_RBUTTON))
	{
		m_RAttack = true;
	}
	if (pGameInstance->Get_KeyDown(DIK_R))
	{
		m_Weapons[0]->Reloading();
		m_Weapons[1]->Reloading();
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::Speed_Setting(_float fTimeDelta)
{
	if (m_LegState == MOVE_SLIDE)
	{
		if (m_MoveSpeed >= 3.f)
			m_MoveSpeed -= 170.f * fTimeDelta;
		else
			m_MoveSpeed = 0.f;
	}
	else if (m_LegState < MOVE_B)
	{
		m_MoveSpeed = 0.f;
		dynamic_cast<CPart_Leg*>(m_Parts[PART_LEG])->AccelEnd();
	}
	else if (m_Booster)
	{
		m_MoveSpeed = dynamic_cast<CPart_Body*>(m_Parts[PART_BODY])->Get_BoosterSpeed();
		dynamic_cast<CPart_Leg*>(m_Parts[PART_LEG])->AccelMax();
	}
	else if (m_SpaceState == SPACE_LAND && m_Jump == false)
	{
		m_MoveSpeed = dynamic_cast<CPart_Leg*>(m_Parts[PART_LEG])->Get_MoveSpeed();
		dynamic_cast<CPart_Leg*>(m_Parts[PART_LEG])->Accel(fTimeDelta);
	}
	else if (m_SpaceState == SPACE_AIR)
	{
		m_MoveSpeed = dynamic_cast<CPart_Body*>(m_Parts[PART_BODY])->Get_FlySpeed();
	}
	else //(��������) �̵��Ұ�.
	{
		m_MoveSpeed = 0.f;
		dynamic_cast<CPart_Leg*>(m_Parts[PART_LEG])->AccelEnd();

		//m_MoveSpeed = dynamic_pointer_cast<CPart_Leg>(m_Parts[PART_LEG])->Get_MoveSpeed();
		//dynamic_pointer_cast<CPart_Leg>(m_Parts[PART_LEG])->Accel();
	}

	m_pTransformCom->Set_Speed(m_MoveSpeed);
}

void CPlayer::Move(_float fTimeDelta)
{
	if (m_SpaceState == SPACE_AIR)
	{
		switch (m_LegState)
		{
		case MOVE_B: case MOVE_RB: case MOVE_LB:
			m_pTransformCom->Move(CTransform::MOVE_BACKWARD, fTimeDelta);
			m_LegState = MOVE_B;
			break;
		case MOVE_F: case MOVE_LF: case MOVE_RF:
			m_pTransformCom->Move(CTransform::MOVE_STRAIGHT, fTimeDelta);
			m_LegState = MOVE_F;
			break;
		case MOVE_L:
			m_pTransformCom->Move(CTransform::MOVE_LEFT, fTimeDelta);
			m_LegState = MOVE_L;
			break;
		case MOVE_R:
			m_pTransformCom->Move(CTransform::MOVE_RIGHT, fTimeDelta);
			m_LegState = MOVE_R;
			break;
		default:
			break;
		}
	}
	else if (!m_Run)
	{
		switch (m_LegState)
		{
		case MOVE_B: case MOVE_RB: case MOVE_LB:
			m_pTransformCom->Move(CTransform::MOVE_BACKWARD, fTimeDelta);
			m_LegState = MOVE_B;
			break;
		case MOVE_F: case MOVE_LF: case MOVE_RF:
			m_pTransformCom->Move(CTransform::MOVE_STRAIGHT, fTimeDelta);
			m_LegState = MOVE_F;
			break;
		case MOVE_L:
			m_pTransformCom->Move(CTransform::MOVE_LEFT, fTimeDelta);
			m_LegState = MOVE_L;
			break;
		case MOVE_R:
			m_pTransformCom->Move(CTransform::MOVE_RIGHT, fTimeDelta);
			m_LegState = MOVE_R;
			break;
		default:
			break;
		}
	}
	else
	{
		switch (m_LegState)
		{
		case MOVE_B:
			m_pTransformCom->Move(CTransform::MOVE_BACKWARD, fTimeDelta);
			break;
		case MOVE_F:
			m_pTransformCom->Move(CTransform::MOVE_STRAIGHT, fTimeDelta);
			break;
		case MOVE_L:
			m_pTransformCom->Move(CTransform::MOVE_LEFT, fTimeDelta);
			break;
		case MOVE_LB:
			m_pTransformCom->Move(CTransform::MOVE_LB, fTimeDelta);
			break;
		case MOVE_LF:
			m_pTransformCom->Move(CTransform::MOVE_LF, fTimeDelta);
			break;
		case MOVE_R:
			m_pTransformCom->Move(CTransform::MOVE_RIGHT, fTimeDelta);
			break;
		case MOVE_RB:
			m_pTransformCom->Move(CTransform::MOVE_RB, fTimeDelta);
			break;
		case MOVE_RF:
			m_pTransformCom->Move(CTransform::MOVE_RF, fTimeDelta);
			break;
		default:
			break;
		}
	}
}

void CPlayer::Turn(_float fTimeDelta)
{
	if (m_BeforeLegMotionState == MOTION_LP)
	{
		if (m_Run && m_SpaceState == SPACE_LAND)
		{
			if (m_BeforeLegState == MOVE_B && m_LegState == MOVE_F)
			{
				m_LegMotionState = MOTION_ST;
				m_LegAnimationIndex = RUNSTEP_BTOF;
				m_LegLoopAnim = false;
			}
			else if (m_BeforeLegState == MOVE_F && m_LegState == MOVE_B)
			{
				m_LegMotionState = MOTION_ST;
				m_LegAnimationIndex = RUNSTEP_FTOB;
				m_LegLoopAnim = false;
			}
			else if (m_BeforeLegState == MOVE_R && m_LegState == MOVE_L)
			{
				m_LegMotionState = MOTION_ST;
				m_LegAnimationIndex = RUNSTEP_RTOL;
				m_LegLoopAnim = false;
			}
			else if (m_BeforeLegState == MOVE_L && m_LegState == MOVE_R)
			{
				m_LegMotionState = MOTION_ST;
				m_LegAnimationIndex = RUNSTEP_LTOR;
				m_LegLoopAnim = false;
			}
			else if (m_BeforeLegState == MOVE_RB && m_LegState == MOVE_LF)
			{
				m_LegMotionState = MOTION_ST;
				m_LegAnimationIndex = RUNSTEP_RBTOLF;
				m_LegLoopAnim = false;
			}
			else if (m_BeforeLegState == MOVE_RF && m_LegState == MOVE_LB)
			{
				m_LegMotionState = MOTION_ST;
				m_LegAnimationIndex = RUNSTEP_RFTOLB;
				m_LegLoopAnim = false;
			}
			else if (m_BeforeLegState == MOVE_LB && m_LegState == MOVE_RF)
			{
				m_LegMotionState = MOTION_ST;
				m_LegAnimationIndex = RUNSTEP_LBTORF;
				m_LegLoopAnim = false;
			}
			else if (m_BeforeLegState == MOVE_LF && m_LegState == MOVE_RB)
			{
				m_LegMotionState = MOTION_ST;
				m_LegAnimationIndex = RUNSTEP_LFTORB;
				m_LegLoopAnim = false;
			}
		}
		else if (m_SpaceState == SPACE_AIR)
		{
			if (m_BeforeLegState == MOVE_R && m_LegState == MOVE_L)
			{
				m_LegMotionState = MOTION_ST;
				m_LegAnimationIndex = AIRSTEP_RTOL;
				m_LegLoopAnim = false;
			}
			else if (m_BeforeLegState == MOVE_L && m_LegState == MOVE_R)
			{
				m_LegMotionState = MOTION_ST;
				m_LegAnimationIndex = AIRSTEP_LTOR;
				m_LegLoopAnim = false;
			}
		}
	}
}

void CPlayer::MoveControl(_float fTimeDelta)
{
	//�⺻������ LegAction = false;
	//�̋� ST, LP, ED, HI , NONE ��ȯ

	_bool Anim_End = m_pModelCom->Get_Finished();
	//�̵������� ����� ������?
	if (Anim_End && m_LegMotionState == MOTION_ED)
	{
		m_LegState = IDLE;
		m_LegMotionState = MOTION_NONE;
		m_LegLoopAnim = true;
		m_isBlending = true;
		//�ִϸ��̼�����(IDLE���°� �ǵ���)
		switch (m_SpaceState)
		{
		case SPACE_LAND:
			m_LegAnimationIndex = WAIT;
			break;
		case SPACE_AIR:
			m_LegAnimationIndex = AIRWAIT_1;
			break;
		}
	}

	//�̵����۸���� + �ִԳ� or �̵������ߴµ� �����¸�?
	if ((Anim_End && m_LegMotionState == MOTION_ST) || (m_BeforeLegMotionState == MOTION_NONE && m_Run))
	{
		m_LegMotionState = MOTION_LP;
		m_isBlending = false;
		m_LegLoopAnim = true;
	}
	if (m_LegMotionState == MOTION_LP)
	{
		//�ִϸ��̼�����
		m_LegLoopAnim = true;
		m_isBlending = true;
		if (m_SpaceState == SPACE_AIR)
		{
			//AirRun
			if (m_Run)
			{
				switch (m_LegState)
				{
				case MOVE_B: case MOVE_RB: case MOVE_LB:
					m_LegAnimationIndex = AIRRUN_B_LP;
					break;
				case MOVE_F: case MOVE_LF: case MOVE_RF:
					m_LegAnimationIndex = AIRRUN_F_LP;
					break;
				case MOVE_L:
					m_LegAnimationIndex = AIRRUN_L_LP;
					break;
				case MOVE_R:
					m_LegAnimationIndex = AIRRUN_R_LP;
					break;
				default:
					break;
				}
			}
			//AirWalk
			else
			{
				switch (m_LegState)
				{
				case MOVE_B: case MOVE_RB: case MOVE_LB:
					m_LegAnimationIndex = AIRWALK_B_LP;
					break;
				case MOVE_F: case MOVE_LF: case MOVE_RF:
					m_LegAnimationIndex = AIRWALK_F_LP;
					break;
				case MOVE_L:
					m_LegAnimationIndex = AIRWALK_L_LP;
					break;
				case MOVE_R:
					m_LegAnimationIndex = AIRWALK_R_LP;
					break;
				case MOVE_UP:
				case MOVE_DOWN:
					m_LegAnimationIndex = AIRRISING_LP;
					break;

				default:
					break;
				}
			}
		}
		else
		{
			//Run
			if (m_Run)
			{
				switch (m_LegState)
				{
				case MOVE_B:
					m_LegAnimationIndex = RUN_B_LP;
					break;
				case MOVE_F:
					m_LegAnimationIndex = RUN_F_LP;
					break;
				case MOVE_L:
					m_LegAnimationIndex = RUN_L_LP;
					break;
				case MOVE_LB:
					m_LegAnimationIndex = RUN_LB_LP;
					break;
				case MOVE_LF:
					m_LegAnimationIndex = RUN_LF_LP;
					break;
				case MOVE_R:
					m_LegAnimationIndex = RUN_R_LP;
					break;
				case MOVE_RB:
					m_LegAnimationIndex = RUN_RB_LP;
					break;
				case MOVE_RF:
					m_LegAnimationIndex = RUN_RF_LP;
					break;
				default:
					break;
				}
			}
			//Walk
			else
			{
				switch (m_LegState)
				{
				case MOVE_B: case MOVE_RB: case MOVE_LB:
					m_LegAnimationIndex = WALK_B;
					break;
				case MOVE_F: case MOVE_LF: case MOVE_RF:
					m_LegAnimationIndex = WALK_F;
					break;
				case MOVE_L:
					m_LegAnimationIndex = WALK_L;
					break;
				case MOVE_R:
					m_LegAnimationIndex = WALK_R;
					break;
				default:
					break;
				}
			}
		}
	}

	//ST�� ������ LP�� �ƴµ� Ű���忡�� ���� ������
	if (!m_MoveInput && m_LegMotionState == MOTION_LP)
	{
		//�ȴ����϶�
		m_LegMotionState = MOTION_ED;
		m_LegLoopAnim = false;

		if (m_LegAnimationIndex >= WALK_B && m_LegAnimationIndex <= WALK_R)
		{
			m_LegAnimationIndex = WALK_ED;
		}
		//AIR�����϶�
		else if (m_SpaceState == SPACE_AIR)
		{
			if (m_LegState == MOVE_B)
				m_LegAnimationIndex = AIRWALK_B_ED;
			else if (m_LegState == MOVE_F)
				m_LegAnimationIndex = AIRWALK_F_ED;
			else if (m_LegState == MOVE_L)
				m_LegAnimationIndex = AIRWALK_L_ED;
			else if (m_LegState == MOVE_R)
				m_LegAnimationIndex = AIRWALK_R_ED;
			else if (m_LegState == MOVE_UP || m_LegState == MOVE_DOWN)
			{
				m_LegState = IDLE;
				m_LegMotionState = MOTION_NONE;
				m_LegLoopAnim = true;
				m_isBlending = true;
				m_LegAnimationIndex = AIRWAIT_1;
			}
		}
		//�޸�������϶�
		else
		{
			--m_LegAnimationIndex;
		}
	}

	//�����̵����� ������ �ȱ�, �⺻AirWalk
	//�ٵ� ST���¿��� �ν��͸� ���� �� �ٷ� �� LP�� �Ѿ ����?
	if (m_LegMotionState == MOTION_NONE && m_MoveInput)
	{
		m_LegMotionState = MOTION_ST;
		m_LegLoopAnim = false;
		m_isBlending = true;
		if (m_SpaceState == SPACE_AIR)
		{
			switch (m_LegState)
			{
			case MOVE_B: case MOVE_RB: case MOVE_LB:
				m_LegAnimationIndex = AIRWALK_B_ST;
				break;
			case MOVE_F: case MOVE_LF: case MOVE_RF:
				m_LegAnimationIndex = AIRWALK_F_ST;
				break;
			case MOVE_L:
				m_LegAnimationIndex = AIRWALK_L_ST;
				break;
			case MOVE_R:
				m_LegAnimationIndex = AIRWALK_R_ST;
				break;
			case MOVE_UP:
			case MOVE_DOWN:
				m_LegAnimationIndex = AIRRISING_LP;
				m_LegLoopAnim = true;
				m_LegMotionState = MOTION_LP;
				break;
			}
		}
		else if (m_SpaceState == SPACE_LAND)
		{
			switch (m_LegState)
			{
			case MOVE_B: case MOVE_RB: case MOVE_LB:
				m_LegAnimationIndex = WALK_B;
				break;
			case MOVE_F: case MOVE_LF: case MOVE_RF:
				m_LegAnimationIndex = WALK_F;
				break;
			case MOVE_L:
				m_LegAnimationIndex = WALK_L;
				break;
			case MOVE_R:
				m_LegAnimationIndex = WALK_R;
				break;
			default:
				break;
			}
		}
	}

}

void CPlayer::WeaponAction(_float fTimeDelta)
{
	_bool Left_Anim_End = false;
	_bool Right_Anim_End = false;
	_uint Left_Anim_Frame = 0;
	_uint Right_Anim_Frame = 0;
	_uint Anim_Index = 0;

	//�ִϸ��̼� ���� �����ӱ��ϱ� �� �ִϸ��̼��� �������� �Ǵ�
	{
		if (m_LeftArmUpdate)
		{
			Left_Anim_End = m_pModelCom->Get_Finished(CModel::ARM_LEFT);
			Left_Anim_Frame = m_pModelCom->Get_Frame(CModel::ARM_LEFT);
		}
		else
		{
			Left_Anim_End = m_pModelCom->Get_Finished();
			Left_Anim_Frame = m_pModelCom->Get_Frame();
		}
		if (m_RightArmUpdate)
		{
			Right_Anim_End = m_pModelCom->Get_Finished(CModel::ARM_RIGHT);
			Right_Anim_Frame = m_pModelCom->Get_Frame(CModel::ARM_RIGHT);
		}
		else
		{
			Right_Anim_End = m_pModelCom->Get_Finished();
			Right_Anim_Frame = m_pModelCom->Get_Frame();
		}
	}
	m_Weapons[0]->Reloading(fTimeDelta, Left_Anim_End, &m_LeftArmAnimationIndex, &m_LegLoopAnim, &m_LeftArmUpdate, HAND::HAND_L);
	m_Weapons[1]->Reloading(fTimeDelta, Right_Anim_End, &m_RightArmAnimationIndex, &m_LegLoopAnim, &m_RightArmUpdate, HAND::HAND_R);

	if (m_LAttack && m_RAttack && false)
	{

	}
	else
	{
		if (m_LAttack)
		{
			if (m_Weapons[0] != nullptr)
				m_Weapons[0]->WeaponUpdate(fTimeDelta, Left_Anim_End, Left_Anim_Frame,&m_LegLoopAnim, &m_Chase, &m_WeaponMotion, &m_LeftArmAnimationIndex, &m_LeftArmUpdate, HAND_L);
			else
				Punch(&m_LeftArmAnimationIndex);

			if (m_WeaponMotion)
			{
				m_RAttack = false;
				m_LegAnimationIndex = m_LeftArmAnimationIndex;
				m_isBlending = false;
				if (m_LeftArmAnimationIndex == 1000)
				{
					m_BodyState = NOATTACK;
					m_LAttack = false;
					m_WeaponMotion = false;
					m_LegLoopAnim = true;
					if (m_Jump)
					{
						m_JumpMotionState = MOTION_ED;
						m_LegAnimationIndex = JUMP_DOWN_LP;
						m_LegState = MOVE_SLIDE;
						m_JumpPower = 0.f;
						m_isBlending = false;
					}
					else
					{
						m_LegState = IDLE;
						m_BodyMotionState = MOTION_NONE;
						m_isBlending = true;
					}
				}
			}
			else if (m_LeftArmAnimationIndex == 1000)
			{
				m_LAttack = false;
				m_BodyState = NOATTACK;
			}
		}
		else
		{
			if (m_Weapons[0] != nullptr)
				m_Weapons[0]->Reset();
		}
		
		if (m_RAttack)
		{
			if (m_Weapons[1] != nullptr)
 				m_Weapons[1]->WeaponUpdate(fTimeDelta, Right_Anim_End, Right_Anim_Frame, &m_LegLoopAnim, &m_Chase, &m_WeaponMotion, &m_RightArmAnimationIndex, &m_RightArmUpdate, HAND_R);
			else
				Punch(&m_RightArmAnimationIndex);

			//������ ����� ���� ��
			if (m_WeaponMotion)
			{
				m_LAttack = false;
				m_LegAnimationIndex = m_RightArmAnimationIndex;
				m_isBlending = false;
				if (m_RightArmAnimationIndex == 1000)
				{
					m_BodyState = NOATTACK;
					m_RAttack = false;
					m_WeaponMotion = false;
					m_LegLoopAnim = true;
					if (m_Jump)
					{
						m_JumpMotionState = MOTION_ED;
						m_LegAnimationIndex = JUMP_DOWN_LP;
						m_LegState = MOVE_SLIDE;
						m_JumpPower = 0.f;
						m_isBlending = false;
					}
					else
					{
						m_LegState = IDLE;
						m_BodyMotionState = MOTION_NONE;
						m_isBlending = true;
					}
				}
			}
			else if (m_RightArmAnimationIndex == 1000)
			{
				m_BodyState = NOATTACK;
				m_RAttack = false;
			}
		}
		else
		{
			if (m_Weapons[1] != nullptr)
				m_Weapons[1]->Reset();
		}
	}
	//�̵������� �����ѳ༮������ ����. �����ߴٸ� �̶�� ������ �ʿ��߾���
	if (m_LAttack || m_RAttack)
	{
		if (m_WeaponMotion)
		{
			m_LegState = ACTION;
			m_BodyState = ATTACK;
			m_LegMotionState = MOTION_NONE;
			m_RightArmUpdate = false;
			m_LeftArmUpdate = false;
		}
		else
		{
			m_BodyState = MOVEATTACK;
		}
	}
}

void CPlayer::Punch(_uint* AnimationIndex)
{
	m_WeaponMotion = true;

	_bool	Anim_End = m_pModelCom->Get_Finished();

	//���ʰ���
	if (m_BodyMotionState == MOTION_NONE)
	{
		m_BodyMotionState = MOTION_LP;
		m_LegLoopAnim = false;
		Anim_End = false;
		if (m_SpaceState == SPACE_AIR)
			*AnimationIndex = PUNCH_F_LP;
		else if (m_LAttack)
			*AnimationIndex = PUNCH_L_LP;
		else if (m_RAttack)
			*AnimationIndex = PUNCH_R_LP;
	}

	//��������
	if (m_BodyMotionState == MOTION_ED && Anim_End)
	{
		*AnimationIndex = 1000;
		m_BodyMotionState = MOTION_NONE;
	}

	//������
	if (m_BodyMotionState == MOTION_LP && Anim_End)
	{
		m_BodyMotionState = MOTION_ED;
		m_LegLoopAnim = false;
		--(*AnimationIndex);
	}

}

void CPlayer::Chase(_float fTImeDelta)
{
	if (m_ChaseTime >= 0.25f)
	{
		m_Chase = false;
	}

	if (m_Chase)
	{
		m_ChaseTime += fTImeDelta;
		//�߰��Ѵ�
		_float Speed = XMVectorGetX(XMVector3Length(m_LockOnDD));
		Speed = (Speed - 13.f) * 4.f;
		m_pTransformCom->DirMove(XMVector3Normalize(m_LockOnDD), fTImeDelta, Speed);

		_vector Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		if (XMVectorGetY(Pos) < 1.26f)
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(Pos, 1.26f));
	}
	else
		m_ChaseTime = 0.f;
}

void CPlayer::Jumping(_float fTImeDelta)
{
	if (m_Jump)
	{
		_bool	Anim_End = m_pModelCom->Get_Finished();
		if (m_JumpMotionState == MOTION_NONE)
		{
			m_isBlending = false;
			m_LegAnimationIndex = JUMP_ST;
			m_JumpMotionState = MOTION_ST;
			m_LegLoopAnim = false;
		}
		if (m_JumpMotionState == MOTION_PLUS && Anim_End)
		{
			m_Jump = false;
			m_JumpMotionState = MOTION_NONE;
			m_isBlending = false;
			m_LegLoopAnim = true;
			m_LegState = IDLE;
		}

		if (m_JumpMotionState == MOTION_ST && Anim_End)
		{
			m_JumpMotionState = MOTION_LP;
			m_LegAnimationIndex = JUMPUP_LP;
			m_isBlending = false;
			m_LegLoopAnim = false;
			Anim_End = false;
		}
		//LP����
		if (m_JumpMotionState == MOTION_LP)
		{
			_float JumpPower = 35.f;
			m_JumpPower = JumpPower - m_pModelCom->Get_Frame() * 0.2f;

			m_pTransformCom->Move(m_JumpPower, CTransform::MOVE_UP, fTImeDelta);

			if (Anim_End)
			{
				m_LegAnimationIndex = JUMPUPTODOWN;
				m_JumpMotionState = MOTION_HI;
				m_isBlending = false;
				m_LegLoopAnim = false;
				Anim_End = false;

			}
		}

		if (m_JumpMotionState == MOTION_HI)
		{
			if (Anim_End)
			{
				m_LegAnimationIndex = JUMP_DOWN_LP;
				m_JumpMotionState = MOTION_ED;
				m_isBlending = false;
				m_LegLoopAnim = true;
			}
		}

		if (m_JumpMotionState == MOTION_ED)
		{
			if (m_JumpPower <= -35.f)
				m_JumpPower = -35.f;
			else
				m_JumpPower -= 20.f * fTImeDelta;

			m_pTransformCom->Move(m_JumpPower, CTransform::MOVE_UP, fTImeDelta);
			
			CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
	
			_vector		vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			if (1.26f > XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
			{
				m_LegAnimationIndex = JUMP_ED;
				m_JumpMotionState = MOTION_PLUS;
				m_isBlending = false;
				m_LegLoopAnim = false;
				m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(vPosition, 1.26f));
			}

			RELEASE_INSTANCE(CGameInstance);
		}
	}
}

void CPlayer::QAttack(_float fTimeDelta)
{
	
	_bool Anim_End = m_pModelCom->Get_Finished();

	if (m_Weapons[2] != nullptr )
	{
		if (!m_QAttack)
		{
			//Q�� ������ ���� �� �ִ�. ���´���� �ִ°�쿡��.
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

			CCamera_Fly* pCamera = (CCamera_Fly*)pGameInstance->Get_GameObjectPtr(LEVEL_GAMEPLAY, TEXT("Layer_Camera"));
			CCollider* pObject = pCamera->LockOn_Front();

			if (pGameInstance->Get_KeyDown(DIK_Q) && pObject != nullptr)
			{
				m_QAttack = true;

				if (m_BodyState == NOATTACK && m_LegState == IDLE)
				{
					m_BodyState = ATTACK;
					m_LegState = ACTION;
					m_BodyMotion = true;
					m_LegLoopAnim = false;
					Anim_End = false;
				}
			}
			RELEASE_INSTANCE(CGameInstance);
		}

		if (m_QAttack)
		{

			_bool	boolDefault = false;
			_uint	uintDefault = false;

			_uint   Anim_Frame = m_pModelCom->Get_Frame();
			_uint	Anim_Index = m_LegAnimationIndex;

			m_Weapons[2]->WeaponUpdate(fTimeDelta, Anim_End, Anim_Frame, &m_BodyMotion, &boolDefault, &boolDefault, &Anim_Index, &boolDefault, HAND_MAX);

			if (m_BodyMotion)
			{
				m_LegAnimationIndex = Anim_Index;
				m_isBlending = false;

				if (Anim_Index == 1000)
				{
					m_BodyState = NOATTACK;
					m_QAttack = false;
					m_BodyMotion = false;
					m_LegLoopAnim = true;
					if (m_Jump)
					{
						m_JumpMotionState = MOTION_ED;
						m_LegAnimationIndex = JUMP_DOWN_LP;
						m_LegState = MOVE_SLIDE;
						m_JumpPower = 0.f;
						m_isBlending = false;
					}
					else
					{
						m_LegState = IDLE;
						m_isBlending = true;
					}
				}
			}
			else if (Anim_Index == 1000)
			{
				m_QAttack = false;
			}
		}
	}
}

CWeapon * CPlayer::Get_Weapon(_uint _Index)
{
	return m_Weapons[_Index];
}

CPart * CPlayer::Get_Part(_uint _Index)
{
	return m_Parts[_Index];
}

_bool CPlayer::Get_Hit()
{
	return m_Hit;
}

_bool CPlayer::Get_Recorve()
{
	return m_Recorve;
}

void CPlayer::Set_LockOnDD(_fvector _vector)
{
	m_LockOnDD = _vector;
}

void CPlayer::Add_Bullet(_uint _NumBullet)
{
	for (auto& aa : m_Weapons)
	{
		if (aa != nullptr && aa->Get_UseBullet())
		{
			aa->Add_SpareBullet(_NumBullet);
		}
	}
}

HRESULT CPlayer::SetUp_Component()
{
	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 7.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_AimTransform"), (CComponent**)&m_pAimTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Player"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::SetUp_ConstantTable()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	m_pTransformCom->Bind_OnShader(m_pModelCom, "g_WorldMatrix");
	pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_VIEW, m_pModelCom, "g_ViewMatrix");
	pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_PROJ, m_pModelCom, "g_ProjMatrix");

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}
CPlayer* CPlayer::Create(ID3D11Device * _pDevice, ID3D11DeviceContext * _pDeviceContext)
{
	auto p = new CPlayer(_pDevice, _pDeviceContext);

	if (FAILED(p->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create CPlayer");
		Safe_Release(p);
	}
	return p;
}

CGameObject* CPlayer::Clone(void * pArg)
{
	auto p = new CPlayer(*this);

	if (FAILED(p->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CPlayer");
		Safe_Release(p);
	}
	return p;
}

void CPlayer::Free()
{
	__super::Free();

	for (auto Part : m_Parts)
		Safe_Release(Part);

	Safe_Release(m_pAimTransformCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelCom);
	for (auto& pCollider : m_pCollider)
		Safe_Release(pCollider);
	m_Parts.clear();

}

HRESULT CPlayer::Create_Collider()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	HANDLE hFile = CreateFile(L"../Bin/Data/PlayerCollider.dat", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	DWORD dwByte = 0;
	DWORD dwStrByte = 0;
	char* pBuf = nullptr;
	CGameInstance* pGamseInstance = GET_INSTANCE(CGameInstance);

	ReadFile(hFile, &m_NumCollider, sizeof(_uint), &dwByte, nullptr);

	m_pCollider.resize(m_NumCollider);
	m_ColliderStrings.resize(m_NumCollider);
	_uint i = 0;
	COLLIDERDATA Data;
	while (true)
	{
		ReadFile(hFile, &Data.isBonecollider, sizeof(_bool), &dwByte, nullptr);
		if (0 == dwByte)
			break;
		ReadFile(hFile, &Data.BoneIndex, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &Data.Type, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &Data.Pivot, sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &Data.Size, sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &Data.Radius, sizeof(_float), &dwByte, nullptr);

		//�������ְ�
		if (Data.isBonecollider)
		{
			Data.m_pBoneMatrix = m_pModelCom->Get_CombinedMatrixPtr(Data.BoneIndex);
			Data.m_OffsetMatrix = m_pModelCom->Get_OffsetMatrix(Data.BoneIndex);
			Data.m_PivotMatrix = m_pModelCom->Get_PivotMatrix();
		}
		Data.m_pHostObject = this;
		//�ݶ��̴����� ���������� ����ְ��� �������
		wstring ComponentName = L"Com_Collider";
		wstring Count = to_wstring(i);
		wstring Result = ComponentName + Count;

		m_ColliderStrings[i] = Result;

		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider"), m_ColliderStrings[i].c_str(), (CComponent**)&m_pCollider[i], &Data)))
			return E_FAIL;

		pGameInstance->Add_Collider(TEXT("Player"), m_pCollider[i]);

		++i;
	}
	CloseHandle(hFile);
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}