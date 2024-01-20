#include "stdafx.h"
#include "..\public\TetherBeam.h"

#include "GameInstance.h"
#include "FairyProbe.h"
#include "Skeleton.h"
CTetherBeam::CTetherBeam(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CTetherBeam::CTetherBeam(const CTetherBeam& rhs)
	: CGameObject(rhs)
{
}

HRESULT CTetherBeam::NativeConstruct_Prototype()
{

	return S_OK;
}

HRESULT CTetherBeam::NativeConstruct(void* pArg)
{
	if (!pArg)
		return E_FAIL;
	if (FAILED(SetUp_Component()))
		return E_FAIL;

	CTransform::TRANSFORMDESC Desc;
	Desc.fSpeedPerSec = 15.f;
	Desc.fRotationPerSec = XMConvertToRadians(180.f);
	m_pTransformCom->Set_TransformDesc(Desc);

	PUSHTENTAKLEDATA Data = *(PUSHTENTAKLEDATA*)pArg;
	m_pTransformCom->Set_State(STATE_POSITION, _Vector4(Data.LocalPosition, 1.f));
	m_pTransformCom->LookAtDir(_Vector4(Data.Look, 0.f), false);
	m_Type = (_uint)OBJECT_TYPE::OBJ_END;

	m_LookDir = Data.Look;
	m_DragGameObject = Data.DragGameObject;
	m_DragAble = Data.DragAble;
	m_DragScale = Data.Scale;
	m_MaxDragScale = m_DragScale;
	m_TickDiscardScale = m_DragScale / 20.f;
	m_RotationAngle = 0.f;
	m_Detail = 0.f;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	m_Player = pGameInstance->Get_GameObject(L"Layer_Player");
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}


_int CTetherBeam::Tick(_float fTimeDelta)
{
	if (m_DragAble)
	{
		if (m_DragGameObject)
		{
			
			if (dynamic_cast<CSkeleton*>(m_DragGameObject))
			{
				if (static_cast<CSkeleton*>(m_DragGameObject)->Get_ModelType() == 1)
				{
					if (m_DragScale < 3.5f)
					{
						m_DragScale = 3.5f;
						m_bDead = true;
					}
				}
				else
				{
					if (m_DragScale < 3.5f)
					{
						m_DragScale = 3.5f;
						m_bDead = true;
					}
				}
			}
			else
			{
				if (m_DragScale < 2.f)
				{
					m_DragScale = 2.f;
					m_bDead = true;
				}
			}
		}
	}
	else
	{
		if (m_DragScale < 0.f)
		{
			m_DragScale = 0.f;
			m_bDead = true;
		}
	}
	//1. 회전값 없는 이동값만 받아온다.
	CModel* pModel = (CModel*)m_Player->Get_ComponentPtr(L"Com_Model");
	pModel->Update_CombinedTransformationMatrix();
	_Matrix					BoneMatrix = XMLoadFloat4x4(pModel->Get_CombinedMatrixPtr("hand.R"));
	_Matrix					PivotMatrix = pModel->Get_PivotMatrix();
	_Matrix					WorldMatrix = m_Player->Get_Transform()->Get_WorldMatrix();

	_Vector3 Origin = ((BoneMatrix * PivotMatrix) * WorldMatrix).Translation();
	m_pTransformCom->Set_State(STATE_POSITION, _Vector4(Origin, 1.f));


	//2. 회전은 알아서 시킨다.
	m_pTransformCom->LookAtDir(m_LookDir, false);
	//+ x축회전도 시켜야한다 텐타클에맞게

	BoneMatrix = XMLoadFloat4x4(pModel->Get_CombinedMatrixPtr("wand_tendril.008"));
	_Matrix* m_ControlMatrix = m_pModelCom->Get_ControllMatrixPtr("start_bone");
	*m_ControlMatrix = MatrixNormalize(BoneMatrix * PivotMatrix);
	m_ControlMatrix->Translation(_Vector3(0.f, 0.f, 0.f));
	*m_ControlMatrix = (_Matrix)(XMMatrixRotationAxis(_Vector3(0.f, 1.f, 0.f), XMConvertToRadians(90.f)) * XMMatrixRotationAxis(_Vector3(0.f, 0.f, 1.f), XMConvertToRadians(90.f))/* * XMMatrixTranslation(0.f, 0.f, -0.f)*/) * (*m_ControlMatrix);

	m_ControlMatrix->Translation(_Vector3(m_ControlMatrix->_31, m_ControlMatrix->_32, m_ControlMatrix->_33) * -0.5f);

	_Matrix* m_ControlTranslationMatrix = m_pModelCom->Get_ControllTranslationMatrixPtr("end_bone");
	m_ControlTranslationMatrix->Translation(_Vector3(0.f, 0.f, -1.f) * m_DragScale * 100 + _Vector3(0.f, 0.f, 80.f));
	m_pModelCom->Update_CombinedTransformationMatrix();

	if (m_bDead)
	{
		if (m_DragGameObject)
		{
			if (!m_StateUpdate)
			{
				m_StateUpdate = true;
				CStateMachine* pStateMachine = (CStateMachine*)m_DragGameObject->Get_ComponentPtr(L"Com_StateMachine");
				if (pStateMachine)
				{
					pStateMachine->SetCurrentState(0);
				}
			}
			if (m_DragAble)
			{
				wstring ColliderName = L"Rigid";
				((CRigid*)m_DragGameObject->Get_ComponentPtr(L"Com_RigidBody"))->Set_isSimulation(ColliderName, true);
				((CRigid*)m_DragGameObject->Get_ComponentPtr(L"Com_RigidBody"))->Set_Gravity(true);
				m_DragGameObject->Get_Transform()->Set_Y(m_Player->Get_Transform()->Get_Y());
			}
			else
			{
				wstring ColliderName = L"Rigid";
				((CRigid*)m_Player->Get_ComponentPtr(L"Com_RigidBody"))->Set_isSimulation(ColliderName, true);
				((CRigid*)m_Player->Get_ComponentPtr(L"Com_RigidBody"))->Set_Gravity(true);
				m_Player->Get_Transform()->Set_Y(m_DragGameObject->Get_Transform()->Get_Y());
			}
		}
	}
	else if (m_DragGameObject)
	{
		if (m_DragAble)
		{
			if (!m_StateUpdate)
			{
				m_StateUpdate = true;
				CStateMachine* pStateMachine = (CStateMachine*)m_DragGameObject->Get_ComponentPtr(L"Com_StateMachine");
				if (pStateMachine)
				{
					pStateMachine->SetCurrentState(0);
				}
			}
			wstring ColliderName = L"Rigid";
			((CRigid*)m_DragGameObject->Get_ComponentPtr(L"Com_RigidBody"))->Set_isSimulation(ColliderName, false);
			((CRigid*)m_DragGameObject->Get_ComponentPtr(L"Com_RigidBody"))->Set_Gravity(false);
			m_pModelCom->Update_CombinedTransformationMatrix();
			BoneMatrix = XMLoadFloat4x4(m_pModelCom->Get_CombinedMatrixPtr("end_bone"));
			PivotMatrix = m_pModelCom->Get_PivotMatrix();
			WorldMatrix = m_pTransformCom->Get_WorldMatrix();

			_Vector3 BonePos = ((BoneMatrix * PivotMatrix) * WorldMatrix).Translation();
			BonePos.y -= ((CRigid*)m_DragGameObject->Get_ComponentPtr(L"Com_RigidBody"))->Get_LocalPos(ColliderName).y;
			m_DragGameObject->Get_Transform()->Set_State(STATE_POSITION, _Vector4(BonePos, 1.f));
		}
		else
		{
//  			m_ControlTranslationMatrix = m_pModelCom->Get_ControllTranslationMatrixPtr("tentacle_beam_armature");
//  			m_ControlTranslationMatrix->Translation(_Vector3(0.f, 0.f, -1.f) * (m_MaxDragScale - m_DragScale)* 100);

 			wstring ColliderName = L"Rigid";
 			((CRigid*)m_Player->Get_ComponentPtr(L"Com_RigidBody"))->Set_isSimulation(ColliderName, false);
 			((CRigid*)m_Player->Get_ComponentPtr(L"Com_RigidBody"))->Set_Gravity(false);
 			m_pModelCom->Update_CombinedTransformationMatrix();
 
 			BoneMatrix = XMLoadFloat4x4(m_pModelCom->Get_CombinedMatrixPtr("start_bone"));
 			PivotMatrix = m_pModelCom->Get_PivotMatrix();
 			WorldMatrix = m_pTransformCom->Get_WorldMatrix();
 
 			_Vector3 BonePos = ((BoneMatrix * PivotMatrix) * WorldMatrix).Translation();
 			BonePos.y -= ((CRigid*)m_Player->Get_ComponentPtr(L"Com_RigidBody"))->Get_ActorLocalPos().y;
 			m_Player->Get_Transform()->Set_State(STATE_POSITION, _Vector4(_Vector3(m_Player->Get_Transform()->Get_State(STATE_POSITION)) + m_LookDir * m_TickDiscardScale * m_Detail, 1.f));
		}
	}
	return _int();
}

_int CTetherBeam::LateTick(_float fTimeDelta)
{
	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this)))
		return 0;
	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_GLOW, this)))
		return 0;
	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DYNAMICSHADOW, this)))
		return 0;

	return _int();
}

HRESULT CTetherBeam::Render(_uint iRenderGroup)
{
	if (CRenderer::RENDER_NONLIGHT == iRenderGroup)
	{
		if (FAILED(SetUp_ConstantTable()))
			return E_FAIL;

		_uint	iNumMaterials = m_pModelCom->Get_NumMaterials();

		for (_uint i = 0; i < iNumMaterials; ++i)
		{
			m_pShaderCom->Set_RawValue("g_vTentacleColor_Near", &_float4(0.25f, 0.f, 1.f, 1.f), sizeof(_float4));
			m_pShaderCom->Set_RawValue("g_vTentacleColor_Mid", &_float4(1.f, 0.f, 1.f, 1.f), sizeof(_float4));
			m_pShaderCom->Set_RawValue("g_vTentacleColor_Far", &_float4(1.f, 1.f, 1.f, 1.f), sizeof(_float4));
			m_pModelCom->Render(m_pShaderCom, i, 10);
		}

		// edge
		for (_uint i = 0; i < iNumMaterials; ++i)
		{
			m_pShaderCom->Set_RawValue("g_vMtrlDiffuse", &_float4(1.f, 0.f, 1.f, 1.f), sizeof(_float4));
			m_pModelCom->Render(m_pShaderCom, i, 11);
		}

	}

	else if (CRenderer::RENDER_GLOW == iRenderGroup)
	{
		if (FAILED(SetUp_ConstantTable()))
			return E_FAIL;

		_uint	iNumMaterials = m_pModelCom->Get_NumMaterials();

		for (_uint i = 0; i < iNumMaterials; ++i)
		{
			m_pShaderCom->Set_RawValue("g_vTentacleColor_Near", &_float4(0.f, 0.f, 0.f, 1.f), sizeof(_float4));
			m_pShaderCom->Set_RawValue("g_vTentacleColor_Mid", &_float4(0.f, 0.f, 0.f, 1.f), sizeof(_float4));
			m_pShaderCom->Set_RawValue("g_vTentacleColor_Far", &_float4(0.f, 0.f, 0.f, 1.f), sizeof(_float4));
			m_pModelCom->Render(m_pShaderCom, i, 10);
		}

		// edge
		for (_uint i = 0; i < iNumMaterials; ++i)
		{
			m_pShaderCom->Set_RawValue("g_vMtrlDiffuse", &_float4(0.5f, 0.f, 1.f, 1.f), sizeof(_float4));
			m_pModelCom->Render(m_pShaderCom, i, 11);
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

_float CTetherBeam::DragObject(_float KeyFrameMove)
{
	if (m_DragGameObject == nullptr)
	{
		m_bDead = true;
		return 0.f;
	}
	m_Detail = KeyFrameMove;

	return m_DragScale -= KeyFrameMove * m_TickDiscardScale;
}

HRESULT CTetherBeam::SetUp_Component()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component_STATIC(TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component_STATIC(TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Model_TetherBeam"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Shader*/
	if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Shader_AnimMesh_Effect"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTetherBeam::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix			WorldMatrix = XMMatrixScaling(1.f, 1.f, 1.f) * m_pTransformCom->Get_WorldMatrix();
	//m_pTransformCom->Bind_OnShader(m_pModelCom, "g_WorldMatrix");
	m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	pGameInstance->Bind_Transform_OnShader(TS_VIEW, m_pShaderCom, "g_ViewMatrix");
	pGameInstance->Bind_Transform_OnShader(TS_PROJ, m_pShaderCom, "g_ProjMatrix");

	m_pShaderCom->Set_RawValue("g_LightViewMatrix", &pGameInstance->Get_LightTransform(TS_VIEW), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_LightProjMatrix", &pGameInstance->Get_LightTransform(TS_PROJ), sizeof(_float4x4));

	_float		fOutLineWidth = 0.05f;
	m_pShaderCom->Set_RawValue("fOutLineWidth", &fOutLineWidth, sizeof(_float));
// 
// 	m_pModelCom->Set_RawValue("g_vMtrlDiffuse", &_float4(0.98f, 0.66f, 0.86f, 1.f), sizeof(_float4));
// 	m_pModelCom->Set_RawValue("g_vGlowColor_Front", &_float4(2.4f, 0.6f, 3.f, 1.f), sizeof(_float4));
// 	m_pModelCom->Set_RawValue("g_vGlowColor_Back", &_float4(1.52f, 0.f, 2.96f, 4.f), sizeof(_float4));

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}


CTetherBeam* CTetherBeam::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CTetherBeam* pInstance = new CTetherBeam(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CTetherBeam");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CTetherBeam::Clone(void* pArg)
{
	CTetherBeam* pInstance = new CTetherBeam(*this);


	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CTetherBeam");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTetherBeam::Free()
{
	__super::Free();
	// 	Safe_Release(m_pTransformCom);
	// 	Safe_Release(m_pShaderCom);	
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRigid);
}
