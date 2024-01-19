#include "stdafx.h"
#include "BombDust.h"
#include "GameInstance.h"

CBombDust::CBombDust(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CBombDust::CBombDust(CGameObject& rhs)
	:CGameObject(rhs)
{
}

CBombDust::~CBombDust()
{
}

HRESULT CBombDust::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CBombDust::NativeConstruct(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	PUSHBOMBDUSTDATA Data = *(PUSHBOMBDUSTDATA*)pArg;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	m_NumBombDust = Data.NumBombDust;
	m_Diffuse = Data.Diffuse;
	m_Glow = Data.Glow;
	_float Angle = 360.f / m_NumBombDust;
	for (_uint i = 0; i < m_NumBombDust; ++i)
	{
		//°¸¸¸Å­Æ÷Áö¼ÇÀüºÎ·£´ý
		_Vector3 Dir = { 1.f, 0.f, 0.f };
		_Matrix RotationMatrix = _Matrix::CreateRotationY(XMConvertToRadians(pGameInstance->Range_Float(-10, 10) + Angle * i));
		Dir = _Vector3::Transform(Dir, RotationMatrix);
		Dir *= pGameInstance->Range_Float(0, 1) * Data.SpeedGap;
		Dir.y = Data.YSpeed + pGameInstance->Range_Float(0, 1) * Data.YDirGap;
		m_Dir.push_back(Dir);

		_float Size = Data.StartSize + pGameInstance->Range_Float(-1, 1) * Data.SizeGap;
		m_Sizes.push_back(Size);

		m_WaitTime.push_back(_Vector3(Data.WaitTime + pGameInstance->Range_Float(0, 1) * Data.WaitTimeGap, 0.f, 0.f));

		_Matrix PushMatrix;
		PushMatrix = MatrixScaling(PushMatrix, _Vector3(Size, Size, Size));
		PushMatrix.Translation(Data.MainPos + ZeroY(Dir) * 3.f);
		m_SubMatrix.push_back(PushMatrix);
	}
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

_int CBombDust::Tick(_float fTimeDelta)
{
	auto& Matrixiter = m_SubMatrix.begin();
	auto& Timeiter = m_WaitTime.begin();
	auto& Sizeiter = m_Sizes.begin();
	auto& Diriter = m_Dir.begin();
	m_TimeAcc += fTimeDelta * 3.f;

	for (; Sizeiter != m_Sizes.end();)
	{
		if (Timeiter->z >= 1.f)
		{
			Matrixiter = m_SubMatrix.erase(Matrixiter);
			Timeiter = m_WaitTime.erase(Timeiter);
			Sizeiter = m_Sizes.erase(Sizeiter);
			Diriter = m_Dir.erase(Diriter);
			continue;
		}
		else
		{
			if (Timeiter->x > Timeiter->y)
			{
				Timeiter->y += fTimeDelta;
			}
			else
			{
				Timeiter->z += fTimeDelta * 1.5f;
				if (Timeiter->z > 1.f)
				{
					Timeiter->z = 1.f;
				}
				_Vector3 Scale;
				Scale.x = Scale.y = Scale.z = CEasing::Lerp(*Sizeiter, 0.f, Timeiter->z, CEasing::EaseType::easeOutQuad);
				*Matrixiter = MatrixScaling(*Matrixiter, Scale);
			}

			if (m_TimeAcc >= 1.f)
			{
				Diriter->y -= 0.001f;
				m_TimeAcc = 1.f;
			}
			_Vector3 Position;
			Position.x = CEasing::Lerp(Diriter->x, 0.f, m_TimeAcc, CEasing::EaseType::easeLiner);
			Position.z = CEasing::Lerp(Diriter->z, 0.f, m_TimeAcc, CEasing::EaseType::easeLiner);
			Position.y = Diriter->y;

			Matrixiter->Translation(Matrixiter->Translation() + Position);

			++Diriter;
			++Matrixiter;
			++Timeiter;
			++Sizeiter;
		}
	}

	if (m_Sizes.size() == 0)
	{
		m_bDead = true;
	}
	return _int();
}

_int CBombDust::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this)))
		return 0;
	if (m_Glow)
	{
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_GLOW, this)))
			return 0;
	}

	return _int();
}

HRESULT CBombDust::Render(_uint iRenderGroup)
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
				//m_pModelCom->Set_ShaderResourceView(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);
				aiColor4D		aiMtrlDiffuse = m_pModelCom->Get_MtrlDiffuse(i, aiTextureType_DIFFUSE) * 0.75f;
				m_pShaderCom->Set_RawValue("g_vMtrlDiffuse", &m_Diffuse, sizeof(_float4));
				m_pModelCom->Render(m_pShaderCom, i, 3);
			}
		}
	}
	else if (CRenderer::RENDER_GLOW == iRenderGroup)
	{
		for (auto& SubMatrix : m_SubMatrix)
		{
			if (FAILED(SetUp_ConstantTable(SubMatrix)))
				return E_FAIL;

			_uint	iNumMaterials = m_pModelCom->Get_NumMaterials();

			m_pShaderCom->Set_RawValue("g_vGlowColor_Front", &_Vector4(1.f, 1.f, 0.f, 1.f), sizeof(_float4));
			m_pShaderCom->Set_RawValue("g_vGlowColor_Back", &_Vector4(1.f, 0.f, 0.f, 1.f), sizeof(_float4));
			for (_uint i = 0; i < iNumMaterials; ++i)
			{
				m_pShaderCom->Set_RawValue("g_vMtrlDiffuse", &m_Diffuse, sizeof(_float4));
				m_pModelCom->Render(m_pShaderCom, i, 15);
			}
		}
	}

	return S_OK;
}

HRESULT CBombDust::SetUp_Component()
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

HRESULT CBombDust::SetUp_ConstantTable(_Matrix SubMatrix)
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


CBombDust* CBombDust::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CBombDust* pInstance = new CBombDust(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CBombDust");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CBombDust::Clone(void* pArg)
{
	CBombDust* pInstance = new CBombDust(*this);


	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CBombDust");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBombDust::Free()
{
	__super::Free();
	// 	Safe_Release(m_pTransformCom);
	// 	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}