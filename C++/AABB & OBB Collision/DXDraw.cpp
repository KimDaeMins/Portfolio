#include "..\Public\DXDraw.h"
#include "PipeLine.h"
#include "Graphic_Device.h"


CDXDraw::CDXDraw()
{
}

CDXDraw::CDXDraw(const CDXDraw & rhs)
	: CComponent(rhs)
	, m_pInputLayout(rhs.m_pInputLayout)
	, m_pEffect(rhs.m_pEffect)
	, m_pBatch(rhs.m_pBatch)
{
}


HRESULT CDXDraw::NativeConstruct_Prototype()
{
	m_pEffect = make_shared<BasicEffect>(g_Device.Get());
	m_pEffect->SetVertexColorEnabled(true);

	const void*	pShaderByteCodes = nullptr;
	size_t	iShaderByteCodeLength = 0;

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCodes, &iShaderByteCodeLength);

	m_pBatch = make_shared<PrimitiveBatch<VertexPositionColor>>(g_DeviceContext.Get());


	if (FAILED(g_Device->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount
		, pShaderByteCodes, iShaderByteCodeLength, m_pInputLayout.GetAddressOf())))
		return E_FAIL;


	return S_OK;
}

HRESULT CDXDraw::NativeConstruct(void * pArg)
{
	return S_OK;
}
#ifdef _DEBUG
HRESULT CDXDraw::Render()
{
	return S_OK;
}

HRESULT CDXDraw::Render(shared_ptr<BoundingBox> _pBox, _vector _Color)
{
	Renderbegin();
	DX::Draw(m_pBatch.get(), *_pBox, _Color);
	Renderend();

	return S_OK;
}

HRESULT CDXDraw::Render(shared_ptr<BoundingOrientedBox> _pBox, _vector _Color)
{
	Renderbegin();
	DX::Draw(m_pBatch.get(), *_pBox, _Color);
	Renderend();

	return S_OK;
}

HRESULT CDXDraw::Render(shared_ptr<BoundingSphere> _pBox, _vector _Color)
{
	Renderbegin();
	DX::Draw(m_pBatch.get(), *_pBox, _Color);
	Renderend();

	return S_OK;
}
void CDXDraw::Renderbegin()
{
	shared_ptr<CPipeLine>		pPipeLine = GET_INSTANCE(CPipeLine);

	g_DeviceContext->IASetInputLayout(m_pInputLayout.Get());

	m_pEffect->SetWorld(XMMatrixIdentity());
	m_pEffect->SetView((pPipeLine->Get_Transform(CPipeLine::TS_VIEW)));
	m_pEffect->SetProjection((pPipeLine->Get_Transform(CPipeLine::TS_PROJ)));

	m_pEffect->Apply(g_DeviceContext.Get());

	m_pBatch->Begin();
}
void CDXDraw::Renderend()
{
	m_pBatch->End();
}
#endif

shared_ptr<CDXDraw> CDXDraw::Create()
{
	auto		pInstance = make_shared<CDXDraw>();

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CDXDraw");
		return nullptr;
	}

	return pInstance;
}

shared_ptr <CComponent> CDXDraw::Clone(void * pArg)
{
	auto		pInstance = make_shared<CDXDraw>(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CDXDraw");
		return nullptr;
	}

	return pInstance;
}
