#include "..\Public\VIBuffer_Trail.h"

CVIBuffer_Trail::CVIBuffer_Trail()
{
}

CVIBuffer_Trail::CVIBuffer_Trail(const CVIBuffer_Trail & rhs)
	: CVIBuffer(rhs)
	, m_vtxSize (rhs.m_vtxSize)
	, m_maxTrailCnt(rhs.m_maxTrailCnt)
	, m_maxvtxCnt(rhs.m_maxvtxCnt)
	, m_maxtriCnt(rhs.m_maxtriCnt)
	, m_LerpCnt(rhs.m_LerpCnt)
{
}

HRESULT CVIBuffer_Trail::NativeConstruct_Prototype(const _tchar* pShaderFilePath)
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	D3D11_INPUT_ELEMENT_DESC		ElementDesc[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	if (FAILED(Compile_Shader(ElementDesc, 2, pShaderFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Trail::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	m_maxTrailCnt = 50;
	m_LerpCnt = 7;
	m_maxvtxCnt = (_uint)((m_maxTrailCnt - 1) * 2 * m_LerpCnt) + 2;
	m_maxtriCnt = m_maxvtxCnt - 2;

	// --------------------------------------------------------
	// Vertex Buffer Create.
	m_iStride = sizeof(VTXTEX); // 트레일은 위치와 UV 좌표만 갖는다.
	m_iNumVertices = m_maxvtxCnt;
	m_iNumVertexBuffers = 1;
	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));

	m_VBDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_VBDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_VBDesc.MiscFlags = 0;
	m_VBDesc.StructureByteStride = m_iStride;

	m_pVertices = make_shared<void>(new VTXCUBETEX[m_iNumVertices], ArrayDeleter<VTXCUBETEX>);
	ZeroMemory(&m_VBSubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_VBSubresourceData.pSysMem = m_pVertices.get();
	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	// --------------------------------------------------------
	// Index Buffer Create
	m_iNumPrimitive = m_maxtriCnt; // Vertex개수에서 2를 뺀다.
	m_iIndicesSize = sizeof(FACEINDICES16);
	m_iNumIndicesPerFigure = 3;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	ZeroMemory(&m_IBDesc, sizeof(D3D11_BUFFER_DESC));

	m_IBDesc.ByteWidth = m_iIndicesSize * m_iNumPrimitive;
	m_IBDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_IBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_IBDesc.MiscFlags = 0;
	m_IBDesc.StructureByteStride = 0;
	m_pPrimitiveIndices = make_shared<void>(new FACEINDICES16[m_iNumPrimitive], ArrayDeleter<FACEINDICES16>);
	
	m_IBSubresourceData.pSysMem = m_pPrimitiveIndices.get();

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Trail::Render(_uint iPassIndex)
{
	/* 장치에 동시에 여러개의 정점버퍼를 셋팅할 수 있기 때문. */
	ID3D11Buffer* pBuffers[] = {
		m_pVB.Get()
	};

	_uint		iStrides[] = {
		m_iStride
	};

	_uint		iOffset[] = {
		0
	};

	g_DeviceContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pBuffers, iStrides, iOffset);

	g_DeviceContext->IASetIndexBuffer(m_pIB.Get(), m_eIndexFormat, 0);
	g_DeviceContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

	/* 정점셰이더에 입력되는 정점의 구성정보. */
	g_DeviceContext->IASetInputLayout(m_PassesDesc[iPassIndex]->pInputlayout.Get());

	if (FAILED(m_PassesDesc[iPassIndex]->pPass->Apply(0, g_DeviceContext.Get())))
		return E_FAIL;

	g_DeviceContext->DrawIndexed(m_iNumIndicesPerFigure * m_curTriCnt, 0, 0);

	return S_OK;
}

// upposition : 검 위부분 / downposition : 검 아래 부분
void CVIBuffer_Trail::AddNewTrail(const _float3& upposition, const _float3& downposition)
{
	TrailData data(upposition, downposition);
	data.LerpGap.resize(m_LerpCnt);
	trailDatas.emplace_front(data);
}

void CVIBuffer_Trail::Update(_float fTimeDelta, _matrix* pWorldMat)
{
	//사이즈가 1개 이하면 나가자. 삼각형을 못만드니까.
	if (trailDatas.size() <= 1)
		return;

	//일정트레일갯수 이상 늘어나면 삭제.
	TrailData BeforeData = trailDatas.back();
	if (trailDatas.size() > m_maxTrailCnt)
	{
		trailDatas.pop_back();
	}


	D3D11_MAPPED_SUBRESOURCE	SubResourceDataVB;
	D3D11_MAPPED_SUBRESOURCE	SubResourceDataIB;

	g_DeviceContext->Map(m_pVB.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResourceDataVB);
	g_DeviceContext->Map(m_pIB.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResourceDataIB);

	std::size_t dataCnt = trailDatas.size();
	unsigned long index = 0;
	m_UVMaxLength = _float2(0.f, 0.f);
	for (std::size_t i = 0; i < dataCnt; ++i)
	{
		SplineTrailPosition((VTXTEX*)SubResourceDataVB.pData, i, index, pWorldMat);

		if (m_maxvtxCnt <= index)
			break;
	}
	for (_uint i = 0; i < m_LerpCnt; ++i)
	{
		m_UVMaxLength.x -= BeforeData.LerpGap[i].x;
		m_UVMaxLength.y -= BeforeData.LerpGap[i].y;
	}
	_float2 UVGap = {0.f, 0.f};
	_uint	LerpGapIndex = 0;
	_uint	DataIndex = 0;
	for (UINT i = 0; i < index; i += 2)
	{
		((VTXTEX*)SubResourceDataVB.pData)[i].vTexUV = _float2(UVGap.x / m_UVMaxLength.x, 0.f);
		((VTXTEX*)SubResourceDataVB.pData)[i + 1].vTexUV = _float2(UVGap.y / m_UVMaxLength.y, 1.f);

		TrailData FirstData = trailDatas[DataIndex];
		UVGap.x += FirstData.LerpGap[LerpGapIndex].x;
		UVGap.y += FirstData.LerpGap[LerpGapIndex].y;
		++LerpGapIndex;

		if (LerpGapIndex >= m_LerpCnt)
		{
			LerpGapIndex = 0;
			++DataIndex;
		}
	}

	m_curVtxCnt = index;
	m_curTriCnt = m_curVtxCnt - 2;
	for (unsigned long i = 0; i < m_curTriCnt; i += 2)
	{
		((FACEINDICES16*)SubResourceDataIB.pData)[i]._0 = (_ushort)i;
		((FACEINDICES16*)SubResourceDataIB.pData)[i]._1 = _ushort(i + 3);
		((FACEINDICES16*)SubResourceDataIB.pData)[i]._2 = _ushort(i + 1);
		((FACEINDICES16*)SubResourceDataIB.pData)[i + 1]._0 = _ushort(i);
		((FACEINDICES16*)SubResourceDataIB.pData)[i + 1]._1 = _ushort(i + 2);
		((FACEINDICES16*)SubResourceDataIB.pData)[i + 1]._2 = _ushort(i + 3);
	}

	g_DeviceContext->Unmap(m_pVB.Get(), 0);
	g_DeviceContext->Unmap(m_pIB.Get(), 0);
}

// @ index : m_pVB의 index
// @ dataindex : trailDatas의 index
void CVIBuffer_Trail::SplineTrailPosition(OUT VTXTEX* vtx, const size_t& dataindex, unsigned long& index, _matrix *pWorldMat)
{
	_matrix im = XMMatrixInverse(nullptr, *pWorldMat);

	
	size_t iCurIndex = index; // m_pVB

	// trailDatas를 로컬로 변환하자.
	_vector coordTemp1 = XMVector3TransformCoord(XMLoadFloat3(&trailDatas[dataindex].position[0]), im);
	XMStoreFloat3(&vtx[index].vPosition, coordTemp1);
	++index;
	_vector	coordTemp2 = XMVector3TransformCoord(XMLoadFloat3(&trailDatas[dataindex].position[1]), im);
	XMStoreFloat3(&vtx[index].vPosition, coordTemp2);
	++index;

	if (dataindex != 0)
	{
		_float2 LerpGap = { 0.f, 0.f };
		LerpGap.x = XMVectorGetX(XMVector3Length(trailDatas[dataindex - 1].LastLerpPos[0] - XMLoadFloat3(&trailDatas[dataindex].position[0])));
		LerpGap.y = XMVectorGetX(XMVector3Length(trailDatas[dataindex - 1].LastLerpPos[1] - XMLoadFloat3(&trailDatas[dataindex].position[1])));
		m_UVMaxLength.x += LerpGap.x;
		m_UVMaxLength.y += LerpGap.y;
		trailDatas[dataindex - 1].LerpGap[m_LerpCnt - 1] = LerpGap;
	}

	if (m_maxvtxCnt >= index)
		return;

	_vector vLerpPos[2];

	size_t iSize = trailDatas.size();
	
	for (unsigned long j = 1; j < m_LerpCnt; ++j)
	{
		size_t iEditIndexV0 = (dataindex < 1 ? 0 : dataindex - 1);
		size_t iEditIndexV2 = (dataindex + 1 >= iSize ? dataindex : dataindex + 1);
		size_t iEditIndexV3 = (dataindex + 2 >= iSize ? iEditIndexV2 : dataindex + 2);

		vLerpPos[0] = XMVectorCatmullRom(XMLoadFloat3(&trailDatas[iEditIndexV0].position[0]), // Start 이전 정보
			XMLoadFloat3(&trailDatas[dataindex].position[0]),// Start
			XMLoadFloat3(&trailDatas[iEditIndexV2].position[0]), // End
			XMLoadFloat3(&trailDatas[iEditIndexV3].position[0]), // End 이후 정보
			j / float(m_LerpCnt));

		vLerpPos[1] = XMVectorCatmullRom(XMLoadFloat3(&trailDatas[iEditIndexV0].position[1]),
			XMLoadFloat3(&trailDatas[dataindex].position[1]),
			XMLoadFloat3(&trailDatas[iEditIndexV2].position[1]),
			XMLoadFloat3(&trailDatas[iEditIndexV3].position[1]),
			j / float(m_LerpCnt));

		_vector coordTemp = XMVector3TransformCoord(vLerpPos[0], im);
		XMStoreFloat3(&vtx[index].vPosition, coordTemp);
		trailDatas[dataindex].LastLerpPos[0] = XMLoadFloat3(&vtx[index].vPosition);
		++index;
		coordTemp = XMVector3TransformCoord(vLerpPos[1], im);
		XMStoreFloat3(&vtx[index].vPosition, coordTemp);
		trailDatas[dataindex].LastLerpPos[1] = XMLoadFloat3(&vtx[index].vPosition);
		++index;

		trailDatas[dataindex].LerpGap[j - 1].x = XMVectorGetX(XMVector3Length(XMLoadFloat3(&vtx[index - 4].vPosition) - trailDatas[dataindex].LastLerpPos[0]));
		trailDatas[dataindex].LerpGap[j - 1].y = XMVectorGetX(XMVector3Length(XMLoadFloat3(&vtx[index - 3].vPosition) - trailDatas[dataindex].LastLerpPos[1]));
		m_UVMaxLength.x += trailDatas[dataindex].LerpGap[j - 1].x;
		m_UVMaxLength.y += trailDatas[dataindex].LerpGap[j - 1].y;
	}
}


shared_ptr<CVIBuffer_Trail> CVIBuffer_Trail::Create(const _tchar* pShaderFilePath)
{
	shared_ptr<CVIBuffer_Trail> pInstance = make_shared<CVIBuffer_Trail>();

	if (FAILED(pInstance->NativeConstruct_Prototype(pShaderFilePath)))
	{
		MSG_BOX("Failed To Creating CVIBuffer_Trail");
		return nullptr;
	}
	return pInstance;
}

shared_ptr<CComponent> CVIBuffer_Trail::Clone(void* pArg)
{
	shared_ptr<CVIBuffer_Trail> pInstance = make_shared<CVIBuffer_Trail>(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed To Creating CVIBuffer_Cube");
		return nullptr;
	}
	return pInstance;
}