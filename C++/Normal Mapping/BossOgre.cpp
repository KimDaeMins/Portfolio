#include "stdafx.h"
#include "..\Public\BossOgre.h"
#include "MGuide_Bullet.h"

CBossOgre::CBossOgre(ID3D11Device * _pDevice, ID3D11DeviceContext * _pDeviceContext)
	:CGameObject(_pDevice, _pDeviceContext)
{
}

CBossOgre::CBossOgre(CBossOgre & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBossOgre::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CBossOgre::NativeConstruct(void * _pArg)
{
	//"Prototype_Component_Model_BossSword"
	if(FAILED(SetUp_Component()))
		return E_FAIL;

	m_Hp = 500;
	m_MaxHp = m_Hp;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-492.f, 1.26f, 15.f, 1.f));
	m_pTransformCom->Set_Rotate(XMVectorSet(0.f, 90.f, 0.f, 0.f));
	Create_Collider();

	return S_OK;
}

_int CBossOgre::Tick(_float fTimeDelta)
{
	_bool Finish = m_pModelCom->Get_Finished();
	if (m_IntroPattern)
	{
		if (m_IntroPatternCount == 0)
		{
			if (m_AnimChange)
			{
				m_Attack = false;
				m_AnimChange = false;
				m_AnimIndex = 25;
				Finish = false;
			}

			if (!m_Attack && m_AnimIndex == 27 && m_pModelCom->Get_Frame() > 13)
			{
				//공격객체생성
				m_Attack = true;
			}

			if (Finish)
			{
				if (m_AnimIndex == 29)
				{
					m_eMode = WAIT;
					m_AnimChange = true;
					++m_IntroPatternCount;
				}
				++m_AnimIndex;
			}
		}
		if (m_IntroPatternCount == 1)
		{
			if (m_AnimChange)
			{
				m_Attack = false;
				m_AnimChange = false;
				m_AnimIndex = 38;
				//m_pModelCom->Set_AnimSpeed(1.f);
				Finish = false;
			}

			if (Finish)
			{
				++m_IntroPatternCount;
				m_AnimChange = true;
			}
		}
		if (m_IntroPatternCount == 2)
		{
			if (m_AnimChange)
			{
				m_Attack = false;
				m_AnimChange = false;
				m_AnimIndex = 30;
				Finish = false;
				m_TakkleSpeed = 30.f;
			}

			if (!m_Attack && ((m_AnimIndex == 32 && m_pModelCom->Get_Frame() > 30) || m_AnimIndex == 33))
			{
				//공격객체생성
				m_Attack = true;

				m_AttackDelay += fTimeDelta;

				if (m_AttackDelay > 0.5f* m_DelayBoost)
				{
					m_AttackDelay = 0.f;
					m_Attack = false;
				}
			}

			if (m_AnimIndex > 31 && m_AnimIndex != 33)
			{
				m_TakkleSpeed += 60 * fTimeDelta;
				m_pTransformCom->Move(m_TakkleSpeed, CTransform::MOVE_STRAIGHT, fTimeDelta);
			}
			else if (m_AnimIndex == 33)
			{
				m_TakkleSpeed -= 120 * fTimeDelta;
				if (m_TakkleSpeed < 0.f)
					m_TakkleSpeed = 0.f;
				m_pTransformCom->Move(m_TakkleSpeed, CTransform::MOVE_STRAIGHT, fTimeDelta);
			}

			if (Finish)
			{
				if (m_AnimIndex == 34)
				{
					m_eMode = WAIT;
					m_AnimChange = true;
					m_IntroPattern = false;
					m_IntroPatternCount = 0;
				}
				++m_AnimIndex;
			}
		}
	}
	else if (!m_Dead)
	{
		if (m_eMode == ANGER)
		{
			m_AnimIndex = 36;
			if (Finish)
			{
				m_eMode = WAIT;
				m_AnimChange = true;
				//m_pModelCom->Set_AnimSpeed(1.5f);
				m_DelayBoost = 0.7f;
				m_MotionSpeed = 1.3f;
			}
		}

		if (m_eMode == KICKL)
		{
			if (m_AnimChange)
			{
				m_Attack = false;
				m_AnimChange = false;
				m_AnimIndex = 1;
				Finish = false;
			}

			if (!m_Attack && m_AnimIndex == 3 && m_pModelCom->Get_Frame() > 35)
			{
				//공격객체생성
				m_Attack = true;
			}

			if (Finish)
			{
				if (m_AnimIndex == 5)
				{
					m_eMode = WAIT;
					m_AnimChange = true;
				}
				++m_AnimIndex;
			}
		}
		else if (m_eMode == KICKR)
		{
			if (m_AnimChange)
			{
				m_Attack = false;
				m_AnimChange = false;
				m_AnimIndex = 6;
				Finish = false;
			}

			if (!m_Attack && m_AnimIndex == 8 && m_pModelCom->Get_Frame() > 25)
			{
				//공격객체생성
				m_Attack = true;
			}

			if (Finish)
			{
				if (m_AnimIndex == 10)
				{
					m_eMode = WAIT;
					m_AnimChange = true;
				}
				++m_AnimIndex;
			}
		}
		else if (m_eMode == MISSILE1)//11
		{
			if (m_AnimChange)
			{
				m_Attack = false;
				m_AnimChange = false;
				m_AnimIndex = 11;
				Finish = false;
			}

			if (!m_Attack && m_AnimIndex == 13)
			{
				//공격객체생성
				CMGuide_Bullet::BulletDesc BulletDesc{};
				BulletDesc.GuideCollider = m_pTargetCollider;

				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				_matrix Pivot, Combined, Offset, Matrix;
				Combined = XMLoadFloat4x4(m_pModelCom->Get_CombinedMatrixPtr("RightShoulderParts1"));
				Pivot = XMLoadFloat4x4(&m_pModelCom->Get_PivotMatrix());
				Offset = XMLoadFloat4x4(&m_pModelCom->Get_OffsetMatrix("RightShoulderParts1"));
				Matrix = (Offset * Combined * Pivot) * m_pTransformCom->Get_WorldMatrix();
				memcpy(&BulletDesc.StartDir, &XMVector3Normalize(Matrix.r[2]), sizeof(_float3));
				memcpy(&BulletDesc.StartPos, &Matrix.r[3], sizeof(_float3));
				pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Bullet"), TEXT("Prototype_MGuideBullet"), &BulletDesc);

				Combined = XMLoadFloat4x4(m_pModelCom->Get_CombinedMatrixPtr("RightShoulderParts2"));
				Offset = XMLoadFloat4x4(&m_pModelCom->Get_OffsetMatrix("RightShoulderParts2"));
				Matrix = (Offset * Combined * Pivot) * m_pTransformCom->Get_WorldMatrix();
				memcpy(&BulletDesc.StartDir, &XMVector3Normalize(Matrix.r[2]), sizeof(_float3));
				memcpy(&BulletDesc.StartPos, &Matrix.r[3], sizeof(_float3));
				pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Bullet"), TEXT("Prototype_MGuideBullet"), &BulletDesc);

				Combined = XMLoadFloat4x4(m_pModelCom->Get_CombinedMatrixPtr("LeftShoulderParts2"));
				Offset = XMLoadFloat4x4(&m_pModelCom->Get_OffsetMatrix("LeftShoulderParts2"));
				Matrix = (Offset * Combined * Pivot) * m_pTransformCom->Get_WorldMatrix();
				memcpy(&BulletDesc.StartDir, &XMVector3Normalize(Matrix.r[2]), sizeof(_float3));
				memcpy(&BulletDesc.StartPos, &Matrix.r[3], sizeof(_float3));
				pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Bullet"), TEXT("Prototype_MGuideBullet"), &BulletDesc);

				Combined = XMLoadFloat4x4(m_pModelCom->Get_CombinedMatrixPtr("LeftShoulderParts1"));
				Offset = XMLoadFloat4x4(&m_pModelCom->Get_OffsetMatrix("LeftShoulderParts1"));
				Matrix = (Offset * Combined * Pivot) * m_pTransformCom->Get_WorldMatrix();
				memcpy(&BulletDesc.StartDir, &XMVector3Normalize(Matrix.r[2]), sizeof(_float3));
				memcpy(&BulletDesc.StartPos, &Matrix.r[3], sizeof(_float3));
				pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Bullet"), TEXT("Prototype_MGuideBullet"), &BulletDesc);

				RELEASE_INSTANCE(CGameInstance);

				m_Attack = true;

				m_AttackDelay += fTimeDelta;
				if (m_AttackDelay > 0.7 * m_DelayBoost)
				{
					m_AttackDelay = 0.f;
					m_Attack = false;
				}
			}

			if (Finish)
			{
				if (m_AnimIndex == 14)
				{
					m_eMode = WAIT;
					m_AnimChange = true;
				}
				++m_AnimIndex;
			}
		}
		else if (m_eMode == SLASH1)//20
		{
			if (m_AnimChange)
			{
				m_Attack = false;
				m_AnimChange = false;
				m_AnimIndex = 20;
				Finish = false;
			}

			if (!m_Attack && m_AnimIndex == 22 && m_pModelCom->Get_Frame() > 25)
			{
				//공격객체생성
				m_Attack = true;
			}

			if (Finish)
			{
				if (m_AnimIndex == 24)
				{
					m_eMode = WAIT;
					m_AnimChange = true;
				}
				++m_AnimIndex;
			}
		}
		else if (m_eMode == SLASH2)//25
		{
			if (m_AnimChange)
			{
				m_Attack = false;
				m_AnimChange = false;
				m_AnimIndex = 25;
				Finish = false;
			}

			if (!m_Attack && m_AnimIndex == 27 && m_pModelCom->Get_Frame() > 13)
			{
				//공격객체생성
				m_Attack = true;
			}

			if (Finish)
			{
				if (m_AnimIndex == 29)
				{
					m_eMode = WAIT;
					m_AnimChange = true;
				}
				++m_AnimIndex;
			}
		}
		else if (m_eMode == TACKLE)//30
		{
			if (m_AnimChange)
			{
				m_Attack = false;
				m_AnimChange = false;
				m_AnimIndex = 30;
				Finish = false;
			}

			if (!m_Attack && ((m_AnimIndex == 32 && m_pModelCom->Get_Frame() > 30) || m_AnimIndex == 33))
			{
				//공격객체생성
				m_Attack = true;

				m_AttackDelay += fTimeDelta;
				if (m_AttackDelay > 0.5f* m_DelayBoost)
				{
					m_AttackDelay = 0.f;
					m_Attack = false;
				}
			}

			if (Finish)
			{
				if (m_AnimIndex == 34)
				{
					m_eMode = WAIT;
					m_AnimChange = true;
				}
				++m_AnimIndex;
			}
		}

		if (m_eMode == TURNL)
		{
			if (m_AnimChange)
			{
				Finish = false;
				m_AnimChange = false;
				m_AnimIndex = 36;
			}

			//회전
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), -fTimeDelta);

			if (Finish)
			{
				m_eMode = WAIT;
				m_AnimChange = true;
			}
		}
		else if (m_eMode == TURNR)
		{
			if (m_AnimChange)
			{
				Finish = false;
				m_AnimChange = false;
				m_AnimIndex = 37;
			}

			//회전
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);

			if (Finish)
			{
				m_eMode = WAIT;
				m_AnimChange = true;
			}
		}

		if (m_eMode == WAIT)
		{
			if (m_AnimChange)
			{
				m_Attack = false;
				m_AnimChange = false;
				m_AnimIndex = 38;
				//m_pModelCom->Set_AnimSpeed(1.f);
				Finish = false;
			}

			if (Finish)
			{
				_vector ToTarget = XMVector3Normalize(XMVectorSetY(m_pTargetTransfrom->Get_State(CTransform::STATE_POSITION), 0.f) - XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 0.f));
				_vector Dir = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));

				_float AngleGap = XMConvertToDegrees(acosf(XMVectorGetX(XMVector3Dot(ToTarget, Dir))));
				if (AngleGap > 30.f)
				{
					if (XMVectorGetY(XMVector3Cross(ToTarget, Dir)) > 0)
					{
						m_eMode = TURNL;
					}
					else
					{
						m_eMode = TURNR;
					}
				}
				else
				{
					if (m_IntroPatternCount == TACKLE)
					{
						m_eMode = (MODE)(rand() % TACKLE);
					}
					else
					{
						m_eMode = (MODE)m_IntroPatternCount++;
					}
					if (m_eMode == MODE::MISSILE2)
					{
						m_eMode = MODE::MISSILE1;
					}

					if (m_Hp * 2 < m_MaxHp && m_DelayBoost >= 0.8f)
					{
						m_eMode = MODE::ANGER;
					}
				}
				m_AnimChange = true;
			}
		}
	}
	else
	{
		if (m_AnimChange)
		{
			m_Attack = false;
			m_AnimChange = false;
			m_AnimIndex = 41;
			m_MotionSpeed = 1.f;
			Finish = false;
		}

		if (!m_Attack && m_AnimIndex == 42 && m_pModelCom->Get_Frame() > 147)
		{
			//다운이펙트생성
			m_Attack = true;
		}

		if (Finish)
		{
			if (m_AnimIndex == 43 && !m_pCollider.empty())
			{
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				for (auto& pCollider : m_pCollider)
				{
					pGameInstance->Delete_Collider(TEXT("Boss"), pCollider);
				}
				m_pCollider.clear();
				RELEASE_INSTANCE(CGameInstance);
				/*return EVENT_DEAD;*/
			}
			else
				++m_AnimIndex;
		}
	}
	if (m_AnimIndex != 43)
	{
		m_pModelCom->SetUp_Animation(m_AnimIndex, false, false);

		m_pModelCom->Update_Animation(fTimeDelta * m_MotionSpeed);
	}

	for (_uint i = 0; i < m_pCollider.size(); ++i)
	{
		if (m_pColliderLockOn[i] == true)
		{
			m_pColliderLockOnTransform[i]->Turn(XMVectorSet(1.f, 1.f, 1.f, 0.f), fTimeDelta);
			m_pColliderLockOnTransform[i]->Set_Scale(XMVectorSet(m_pColliderLockOnScale[i], m_pColliderLockOnScale[i], m_pColliderLockOnScale[i], 0.f));

			if (m_pColliderLockOnScale[i] > 15.f)
			{
				m_pColliderLockOnScale[i] -= 3.f * fTimeDelta;
				m_pColliderLockOnCheck[i] = false;
			}
			else
				m_pColliderLockOnCheck[i] = true;

		}
	}

	return _int();
}

_int CBossOgre::LateTick(_float fTimeDelta)
{
	if (__super::LateTick(fTimeDelta) < 0)
		return -1;

	_bool Finish = m_pModelCom->Get_Finished();

	if (Finish)
	{
		//트랜스폼 매트릭스의 포지션을 루트노드의 포지션으로 바꿔줌
		//지금 머리안돌아감ㅋㅋ
		_matrix		OffsetMatrix = XMLoadFloat4x4(&m_OffsetMatrix);
		_matrix		CombinedTransformationMatrix = XMLoadFloat4x4(m_pBoneMatrix);
		_matrix		PivotMatrix = XMLoadFloat4x4(&m_PivotMatrix);

		_float4x4  World;
		XMStoreFloat4x4(&World, ((OffsetMatrix * CombinedTransformationMatrix * PivotMatrix) * m_pTransformCom->Get_WorldMatrix()));

		_vector Vector;
		memcpy(&Vector, World.m[3], sizeof(_vector)); 
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, Vector);
	}
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this)))
		return 0;


	if (m_Hp <= 0 && !m_Dead)
	{
		m_Dead = true;
		m_AnimChange = true;
	}

	RELEASE_INSTANCE(CGameInstance);

	return _int();
}

HRESULT CBossOgre::Render(_float fTimeDelta)
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	m_pTransformCom->Bind_OnShader(m_pModelCom, "g_WorldMatrix");
	pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_VIEW, m_pModelCom, "g_ViewMatrix");
	pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_PROJ, m_pModelCom, "g_ProjMatrix");

	RELEASE_INSTANCE(CGameInstance);

	_uint		iNumMaterials = m_pModelCom->Get_NumMaterials();


	for (_uint i = 0; i < iNumMaterials; ++i)
	{
		_uint Pass = 0;
		m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", i, aiTextureType_DIFFUSE);
		if (!FAILED(m_pModelCom->Set_ShaderResourceView("g_NormalTexture", i, aiTextureType_NORMALS)))
		{
			++Pass;
			if (!FAILED(m_pModelCom->Set_ShaderResourceView("g_EmissiveTexture", i, aiTextureType_EMISSIVE)))
				++Pass;
		}
		m_pModelCom->Render(i, Pass);
	}

	for (_uint i = 0; i < m_pCollider.size(); ++i)
	{
		if (m_pColliderLockOn[i] == true)
		{
			CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

			_matrix Matrix = XMMatrixIdentity();
			memcpy(&Matrix.r[3], &m_pCollider[i]->Get_CenterPos(m_pCollider[i]->Get_Type()), sizeof(_float3));
			m_pLockOnBoxModelCom->Set_RawValue("g_WorldMatrix", &XMMatrixTranspose(Matrix), sizeof(_matrix));
			pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_VIEW, m_pLockOnBoxModelCom, "g_ViewMatrix");
			pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_PROJ, m_pLockOnBoxModelCom, "g_ProjMatrix");
			m_pLockOnBoxModelCom->Set_RawValue("LockOnCheck", &m_pColliderLockOnCheck[i], sizeof(_bool));
			m_pLockOnBoxTexture->SetUp_OnShader(m_pLockOnBoxModelCom, "g_DiffuseTexture");
			m_pLockOnBoxModelCom->Render(1);
			RELEASE_INSTANCE(CGameInstance);
		}
	}
	return S_OK;
}

HRESULT CBossOgre::RenderLightDepth()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	m_pTransformCom->Bind_OnShader(m_pModelCom, "g_WorldMatrix");
	pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_VIEW, m_pModelCom, "g_ViewMatrix");
	pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_PROJ, m_pModelCom, "g_ProjMatrix");
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

CComponent * CBossOgre::Get_Component(const _uint ComNumber)
{
	switch (ComNumber)
	{
	case 0:
		return m_pTransformCom;
	case 1:
		return m_pModelCom;
	}

	return nullptr;
}

HRESULT CBossOgre::SetUp_Component()
{
	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 2.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(24.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_BossOgre"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	m_pTargetTransfrom = (CTransform*)pGameInstance->Get_ComponentPtr(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform"));
	m_pTargetCollider = (CCollider*)pGameInstance->Get_ComponentPtr(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Collider0"));
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, TEXT("Layer_BossWeapon"), TEXT("Prototype_GameObject_BossSword"), this)))
		return E_FAIL;


	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cube"), TEXT("Com_VIBuffer"), (CComponent**)&m_pLockOnBoxModelCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_LockOn"), TEXT("Com_Texture"), (CComponent**)&m_pLockOnBoxTexture)))
		return E_FAIL;
	 
	m_pBoneMatrix = m_pModelCom->Get_CombinedMatrixPtr("Scene_Root");
	m_OffsetMatrix = m_pModelCom->Get_OffsetMatrix("Scene_Root");
	m_PivotMatrix = m_pModelCom->Get_PivotMatrix();

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}


CBossOgre * CBossOgre::Create(ID3D11Device * _pDevice, ID3D11DeviceContext * _pDeviceContext)
{
	auto p = new CBossOgre(_pDevice, _pDeviceContext);

	if (FAILED(p->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create CBossOgre");
		Safe_Release(p);
	}
	return p;
}

CGameObject* CBossOgre::Clone(void * pArg)
{
	auto p = new CBossOgre(*this);

	if (FAILED(p->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CBossOgre");
		Safe_Release(p);
	}
	return p;
}

void CBossOgre::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelCom);
	for (auto& pCollider : m_pCollider)
		Safe_Release(pCollider);
}

void CBossOgre::LockonMonster(_float LockonDistance, _uint Index)
{
	m_pColliderLockOn[Index] = true;
	m_pCollider[Index]->Set_Distance(LockonDistance);
	return;
}


void CBossOgre::UnLock(_uint Index)
{
	m_pColliderLockOn[Index] = false;
	m_pColliderLockOnScale[Index] = 20.f;
}

HRESULT CBossOgre::Create_Collider()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	HANDLE hFile = CreateFile(L"../Bin/Data/BossOrgeCollider.dat", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	DWORD dwByte = 0;
	DWORD dwStrByte = 0;
	char* pBuf = nullptr;
	CGameInstance* pGamseInstance = GET_INSTANCE(CGameInstance);

	ReadFile(hFile, &m_NumCollider, sizeof(_uint), &dwByte, nullptr);

	m_pCollider.resize(m_NumCollider);
	m_ColliderStrings.resize(m_NumCollider);
	m_pColliderLockOn.resize(m_NumCollider);
	m_pColliderLockOnScale.resize(m_NumCollider);
	m_pColliderLockOnCheck.resize(m_NumCollider);
	m_pColliderLockOnTransform.resize(m_NumCollider);
	m_LockonTransformString.resize(m_NumCollider);
	CTransform::TRANSFORMDESC TransformDesc;
	TransformDesc.fRotationPerSec = XMConvertToRadians(60.0f);
	for (_uint i = 0; i < m_NumCollider; ++i)
	{
		m_LockonTransformString[i] = L"Com_LockOnTransform" + to_wstring(i);
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), m_LockonTransformString[i].c_str(), (CComponent**)&m_pColliderLockOnTransform[i], &TransformDesc)))
			return E_FAIL;
	}
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

		//본정보넣고
		if (Data.isBonecollider)
		{
			Data.m_pBoneMatrix = m_pModelCom->Get_CombinedMatrixPtr(Data.BoneIndex);
			Data.m_OffsetMatrix = m_pModelCom->Get_OffsetMatrix(Data.BoneIndex);
			Data.m_PivotMatrix = m_pModelCom->Get_PivotMatrix();
		}
		Data.m_pHostObject = this;

		//콜라이더생성
		wstring ComponentName = L"Com_Collider";
		ComponentName += to_wstring(i);
		m_ColliderStrings[i] = ComponentName;
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider"), m_ColliderStrings[i].c_str(), (CComponent**)&m_pCollider[i], &Data)))
			return E_FAIL;

		//넣기
		pGameInstance->Add_Collider(TEXT("Boss"), m_pCollider[i]);
		//++i
		++i;
	}
	CloseHandle(hFile);
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}
