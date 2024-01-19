#include "stdafx.h"
#include "Dust.h"
#include "GameInstance.h"
CDust::CDust(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CDust::CDust(CGameObject& rhs)
	:CGameObject(rhs)
{
}

CDust::~CDust()
{
}

HRESULT CDust::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CDust::NativeConstruct(void* pArg)
{
	if (false == Get_Pooling_State())
	{
		if (FAILED(SetUp_Component()))
			return E_FAIL;
		Set_Pooling_State(true);

		return S_OK;
	}
	

	if (pArg == nullptr)
		return E_FAIL;

	PUSHDUSTDATA Data = *(PUSHDUSTDATA*)pArg;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_float Rotate = 360.f / Data.NumDust;
	m_SubMatrix.clear();
	m_Times.clear();
	m_Sizes.clear();
	m_State.clear();
	for (_uint i = 0 ; i < Data.NumDust; ++i)
	{
		_Vector3 Position = { Data.Far + pGameInstance->Range_Float(-1, 1) * Data.FarGap, 0.f, 0.f };
		_Matrix RotationRandMatrix = _Matrix::CreateRotationY(XMConvertToRadians(i * Rotate));
		Position = _Vector3::Transform(Position, RotationRandMatrix);
		Position += Data.MainPos;
		_Matrix PushMatrix;
		PushMatrix.Translation(Position);
		//m_SubMatrix.push_back(PushMatrix);

		m_Times.push_back(_Vector4(Data.IncressTime + pGameInstance->Range_Float(-1, 1) * Data.TimeGap,
		Data.WaitTime + pGameInstance->Range_Float(-1, 1) * Data.TimeGap,
		Data.DecressTime + pGameInstance->Range_Float(-1, 1) * Data.TimeGap,
			0.f));

		_float StartSize = Data.StartSize + pGameInstance->Range_Float(-1, 1) * Data.SizeGap;
		m_Sizes.push_back(_Vector2(StartSize,
			Data.EndSize + pGameInstance->Range_Float(-1, 1) * Data.SizeGap));

		m_State.push_back(0);

		PushMatrix = MatrixScaling(PushMatrix, _Vector3(StartSize, StartSize, StartSize));
		m_SubMatrix.push_back(PushMatrix);
	}
	m_Diffuse = Data.Diffuse;
	m_bGlow = Data.bGlow;
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

_int CDust::Tick(_float fTimeDelta)
{
	auto& Matrixiter = m_SubMatrix.begin();
	auto& Timeiter = m_Times.begin();
	auto& Sizeiter = m_Sizes.begin();
	auto& Stateiter = m_State.begin();
	for (; Stateiter != m_State.end();)
	{
		if ((*Stateiter) == 3)
		{
			Matrixiter = m_SubMatrix.erase(Matrixiter);
			Timeiter = m_Times.erase(Timeiter);
			Sizeiter = m_Sizes.erase(Sizeiter);
			Stateiter = m_State.erase(Stateiter);
			continue;
		}
		else if (*Stateiter == 2)
		{
			Timeiter->w += fTimeDelta / Timeiter->z;
			if (Timeiter->w > 1.f)
			{
				Timeiter->w = 1.f;
			}

			_Vector3 Scale;
			Scale.z = Scale.y = Scale.x = CEasing::Lerp(Sizeiter->y, 0.f, Timeiter->w, CEasing::EaseType::easeLiner);
			(*Matrixiter) = MatrixScaling(*Matrixiter, Scale);
			if (Timeiter->w >= 1.f)
			{
				Timeiter->w = 0.f;
				*Stateiter = 3;
			}
			++Stateiter;
			++Matrixiter;
			++Timeiter;
			++Sizeiter;
		}
		else if (*Stateiter == 1)
		{
			Timeiter->w += fTimeDelta / Timeiter->y;
			if (Timeiter->w > 1.f)
			{
				Timeiter->w = 0.f;
				*Stateiter = 2;
			}
			++Stateiter;
			++Matrixiter;
			++Timeiter;
			++Sizeiter;
		}
		else if (*Stateiter == 0)
		{
			Timeiter->w += fTimeDelta / Timeiter->x;
			_Vector3 Scale;
			if (Timeiter->w > 1.f)
			{
				Timeiter->w = 1.f;
			}
			Scale.z = Scale.y = Scale.x = CEasing::Lerp(Sizeiter->x, Sizeiter->y, Timeiter->w, CEasing::EaseType::easeLiner);
			(*Matrixiter) = MatrixScaling(*Matrixiter, Scale);

			if (Timeiter->w >= 1.f)
			{
				Timeiter->w = 0.f;
				*Stateiter = 1;
			}
			++Stateiter;
			++Matrixiter;
			++Timeiter;
			++Sizeiter;
		}
	}

	if (m_State.size() == 0)
	{
		m_bDead = true;
	}
	return _int();
}

_int CDust::LateTick(_float fTimeDelta)
{
	if (m_bDead)
		return _int();

	if (nullptr == m_pRendererCom)
		return -1;

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this)))
		return 0;
	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_GLOW, this)))
		return 0;

	return _int();
}

HRESULT CDust::Render(_uint iRenderGroup)
{
	if (CRenderer::RENDER_NONALPHA == iRenderGroup)
	{
		for (auto& SubMatrix : m_SubMatrix)
		{
			if (FAILED(SetUp_ConstantTable(SubMatrix)))
				return E_FAIL;

			_uint	iNumMaterials = m_pModelCom->Get_NumMaterials();

			for (_uint i = 0; i < iNumMaterials; ++i)
			{
				m_pShaderCom->Set_RawValue("g_vMtrlDiffuse", &m_Diffuse, sizeof(_float4));
				m_pShaderCom->Set_RawValue("g_vMtrlEmissive", &_float4(0.f, 0.f, 0.f, 1.f), sizeof(_float4));
				m_pModelCom->Render(m_pShaderCom, i, 3);
			}
		}
	}

	else if (CRenderer::RENDER_GLOW == iRenderGroup)
	{
		if (m_bGlow)
		{
			for (auto& SubMatrix : m_SubMatrix)
			{
				if (FAILED(SetUp_ConstantTable(SubMatrix)))
					return E_FAIL;

				_uint	iNumMaterials = m_pModelCom->Get_NumMaterials();

				for (_uint i = 0; i < iNumMaterials; ++i)
				{
					m_pShaderCom->Set_RawValue("g_vMtrlDiffuse", &m_Diffuse, sizeof(_float4));
					m_pModelCom->Render(m_pShaderCom, i, 9);
				}
			}
		}
	}


	return S_OK;
}

HRESULT CDust::SetUp_Component()
{
	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component_STATIC(TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Model_Dust"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Shader_Mesh"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CDust::SetUp_ConstantTable(_Matrix SubMatrix)
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	m_pShaderCom->Set_RawValue("g_WorldMatrix", &SubMatrix, sizeof(_float4x4));
	pGameInstance->Bind_Transform_OnShader(TS_VIEW, m_pShaderCom, "g_ViewMatrix");
	pGameInstance->Bind_Transform_OnShader(TS_PROJ, m_pShaderCom, "g_ProjMatrix");

	m_pShaderCom->Set_RawValue("g_LightViewMatrix", &pGameInstance->Get_LightTransform(TS_VIEW), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_LightProjMatrix", &pGameInstance->Get_LightTransform(TS_PROJ), sizeof(_float4x4));

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}


CDust* CDust::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CDust* pInstance = new CDust(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CDust");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CDust::Clone(void* pArg)
{
	CDust* pInstance = new CDust(*this);


	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CDust");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDust::Free()
{
	__super::Free();
	// 	Safe_Release(m_pTransformCom);
	// 	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}