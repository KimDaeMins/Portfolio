#include "..\Public\VIBuffer_Rect.h"

CVIBuffer_Rect::CVIBuffer_Rect()
{
}

CVIBuffer_Rect::CVIBuffer_Rect(const CVIBuffer_Rect & rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Rect::NativeConstruct_Prototype(const _tchar* pShaderFilePath)
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	m_iStride = sizeof(VTXTEX);
	m_iNumVertices = 4;
	m_iNumVertexBuffers = 1;

	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));

	m_VBDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_VBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.CPUAccessFlags = 0;
	m_VBDesc.MiscFlags = 0;
	m_VBDesc.StructureByteStride = m_iStride;

	m_pVertices = make_shared<void>(new VTXTEX[m_iNumVertices], ArrayDeleter<VTXTEX>);

	((VTXTEX*)m_pVertices.get())[0].vPosition = _float3(-0.5f, 0.5f, 0.f);
	((VTXTEX*)m_pVertices.get())[0].vTexUV = _float2(0.0f, 0.f);

	((VTXTEX*)m_pVertices.get())[1].vPosition = _float3(0.5f, 0.5f, 0.f);
	((VTXTEX*)m_pVertices.get())[1].vTexUV = _float2(1.0f, 0.f);

	((VTXTEX*)m_pVertices.get())[2].vPosition = _float3(0.5f, -0.5f, 0.f);
	((VTXTEX*)m_pVertices.get())[2].vTexUV = _float2(1.0f, 1.f);

	((VTXTEX*)m_pVertices.get())[3].vPosition = _float3(-0.5f, -0.5f, 0.f);
	((VTXTEX*)m_pVertices.get())[3].vTexUV = _float2(0.0f, 1.f);

	ZeroMemory(&m_VBSubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_VBSubresourceData.pSysMem = m_pVertices.get();

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	m_iNumPrimitive = 2;
	m_iIndicesSize = sizeof(FACEINDICES16);
	m_iNumIndicesPerFigure = 3;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	ZeroMemory(&m_IBDesc, sizeof(D3D11_BUFFER_DESC));

	m_IBDesc.ByteWidth = m_iIndicesSize * m_iNumPrimitive;
	m_IBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_IBDesc.CPUAccessFlags = 0;
	m_IBDesc.MiscFlags = 0;
	m_IBDesc.StructureByteStride = 0;

	m_pPrimitiveIndices = make_shared<void>(new FACEINDICES16[m_iNumPrimitive], ArrayDeleter<FACEINDICES16>);
	((FACEINDICES16*)m_pPrimitiveIndices.get())[0]._0 = 0;
	((FACEINDICES16*)m_pPrimitiveIndices.get())[0]._1 = 1;
	((FACEINDICES16*)m_pPrimitiveIndices.get())[0]._2 = 2;

	((FACEINDICES16*)m_pPrimitiveIndices.get())[1]._0 = 0;
	((FACEINDICES16*)m_pPrimitiveIndices.get())[1]._1 = 2;
	((FACEINDICES16*)m_pPrimitiveIndices.get())[1]._2 = 3;

	ZeroMemory(&m_IBSubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_IBSubresourceData.pSysMem = m_pPrimitiveIndices.get();

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	

	/*
	LPCSTR SemanticName;
	UINT SemanticIndex;
	DXGI_FORMAT Format;
	UINT InputSlot;
	UINT AlignedByteOffset;
	D3D11_INPUT_CLASSIFICATION InputSlotClass;
	UINT InstanceDataStepRate;
	*/

	D3D11_INPUT_ELEMENT_DESC		ElementDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};	

	if (FAILED(Compile_Shader(ElementDesc, 2, pShaderFilePath)))
		return E_FAIL;

	

	return S_OK;
}

HRESULT CVIBuffer_Rect::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	return S_OK;
}

shared_ptr<CVIBuffer_Rect> CVIBuffer_Rect::Create(const _tchar* pShaderFilePath)
{
	shared_ptr<CVIBuffer_Rect>	pInstance = make_shared<CVIBuffer_Rect>();

	if (FAILED(pInstance->NativeConstruct_Prototype(pShaderFilePath)))
	{
		MSG_BOX("Failed To Creating CVIBuffer_Rect");
	}
	return pInstance;
}

shared_ptr<CComponent> CVIBuffer_Rect::Clone(void * pArg)
{
	shared_ptr<CVIBuffer_Rect>	pInstance = make_shared<CVIBuffer_Rect>(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed To Creating CVIBuffer_Rect");
	}
	return pInstance;
}
