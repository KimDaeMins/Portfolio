#include "stdafx.h"
#include "Effect_SoulBright.h"
#include "GameInstance.h"
CSoulBright::CSoulBright(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    :CGameObject(pDevice, pDeviceContext)
{
}

CSoulBright::CSoulBright(const CSoulBright& rhs)
    :CGameObject(rhs)
{
}

HRESULT CSoulBright::NativeConstruct_Prototype()
{
    return S_OK;
}

HRESULT CSoulBright::NativeConstruct(void* pArg)
{
    if (pArg == nullptr)
        return E_FAIL;

    if (FAILED(SetUp_Component()))
        return E_FAIL;

    PUSHSOULBRIGHTDATA Data = *(PUSHSOULBRIGHTDATA*)pArg;

	m_HostObject = Data.HostObject;
	m_vColor = Data.vColor;
	m_StartScale = Data.StartScale;
	m_EndScale = Data.EndScale;
	m_bBillBoard = Data.bBillBoard;

	if (nullptr == m_HostObject)
	{
		m_pTransformCom->Set_State(STATE_POSITION, Data.vPosition);
		m_pTransformCom->LookAtDir(Data.vLookDir, false);
	}

	m_RotateMatrix = _Matrix::CreateRotationZ(XMConvertToRadians(Data.Rotate));
	m_LifeTime = 2.f;
	
    return S_OK;
}

_int CSoulBright::Tick(_float fTimeDelta)
{
	if (m_AccTime < 1.f)
	{
		m_AccTime += fTimeDelta * 2.f;
		if(m_AccTime > 1.f)
			m_AccTime = 1.f;
	}
	m_LifeTime -= fTimeDelta * 2.f;

	if (m_LifeTime < 0.f)
		m_bDead = true;



    return _int();
}

_int CSoulBright::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (m_HostObject)
	{
		if (m_HostObject->Get_Dead())
			m_bDead = true;

		_Vector3 Position = m_HostObject->Get_Transform()->Get_State(STATE_POSITION);
		Position.y += 0.1f;
		m_pTransformCom->Set_State(STATE_POSITION, Position);
	}

	_float Scale = CEasing::Lerp(m_StartScale, m_EndScale, m_AccTime, CEasing::EaseType::easeOutSine);
	m_pTransformCom->Set_Scale(_Vector3(Scale, Scale, Scale));


	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this)))
		return 0;
	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_GLOW, this)))
		return 0;
	// D3D11_SAMPLER_DESC

	RELEASE_INSTANCE(CGameInstance);

    return _int();
}

HRESULT CSoulBright::Render(_uint iRenderGroup)
{
	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (CRenderer::RENDER_NONLIGHT == iRenderGroup)
	{
		m_pVIBufferCom->Render(m_pShaderCom, 22);
	}
	else if (CRenderer::RENDER_GLOW == iRenderGroup)
	{
		m_pVIBufferCom->Render(m_pShaderCom, 22);
	}

	return S_OK;
}

HRESULT CSoulBright::SetUp_Component()
{
	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component_STATIC(TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component_STATIC(TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;
		
	/* For.Com_Texture*/
	if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Texture_StarBurst"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Transform*/
	if (FAILED(__super::Add_Component_STATIC(TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Shader*/
	if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Shader_Rect"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSoulBright::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (m_bBillBoard)
		m_pTransformCom->Set_Billboard();

	m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix");
	m_pShaderCom->Set_RawValue("g_WorldMatrix", &(m_RotateMatrix * m_pTransformCom->Get_WorldMatrix()), sizeof(_float4x4));
	pGameInstance->Bind_Transform_OnShader(TS_VIEW, m_pShaderCom, "g_ViewMatrix");
	pGameInstance->Bind_Transform_OnShader(TS_PROJ, m_pShaderCom, "g_ProjMatrix");

	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;

	m_pShaderCom->Set_RawValue("g_vMtrlDiffuse", &m_vColor, sizeof(_float4));
	_float		fRatio = pow((2.f - m_LifeTime) * 0.5f, 2.f);
	m_pShaderCom->Set_RawValue("g_fRatio", &fRatio, sizeof(_float));

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CSoulBright* CSoulBright::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CSoulBright* pInstance = new CSoulBright(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CSoulBright");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CSoulBright::Clone(void* pArg)
{
	CSoulBright* pInstance = new CSoulBright(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CSoulBright");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSoulBright::Free()
{

	__super::Free();
	// 	Safe_Release(m_pTransformCom);
	// 	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);	
}

