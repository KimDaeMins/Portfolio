#include "..\Public\Target_Manager.h"
#include "RenderTarget.h"
#include "VIBuffer_Rect.h"

IMPLEMENT_SINGLETON(CTarget_Manager)

CTarget_Manager::CTarget_Manager()
{

}

ComPtr<ID3D11ShaderResourceView> CTarget_Manager::Get_SRV(const _tchar * pTargetTag)
{
	shared_ptr<CRenderTarget> pRenderTarget = Find_RenderTarget(pTargetTag);
	if (nullptr == pRenderTarget)
		return nullptr;

	return pRenderTarget->Get_SRV();	
}

HRESULT CTarget_Manager::Add_RenderTarget(const _tchar * pRenderTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT eFormat, _float4 vClearColor)
{
	if (nullptr != Find_RenderTarget(pRenderTargetTag))
		return E_FAIL;

	shared_ptr<CRenderTarget>		pRenderTarget = CRenderTarget::Create(iWidth, iHeight, eFormat, vClearColor);
	if (nullptr == pRenderTarget)
		return E_FAIL;


	m_RenderTargets.emplace(pRenderTargetTag, pRenderTarget);

	return S_OK;
}

HRESULT CTarget_Manager::Add_MRT(const _tchar * pMRTTag, const _tchar * pRenderTargetTag)
{
	shared_ptr<CRenderTarget>		pRenderTarget = Find_RenderTarget(pRenderTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	list<shared_ptr<CRenderTarget>>*		pMRTList = Find_MRT(pMRTTag);

	if (nullptr == pMRTList)
	{
		list<shared_ptr<CRenderTarget>>		MRTList;
		MRTList.push_back(pRenderTarget);
		m_MRTs.emplace(pMRTTag, MRTList);
	}
	else
		pMRTList->push_back(pRenderTarget);

	return S_OK;
}

HRESULT CTarget_Manager::Begin_MRT(const _tchar * pMRTTag)
{
	/* 지정한 mrt에 있는 렌더타겟들을 장치에 바인드한다. */
	list<shared_ptr<CRenderTarget>>*		pMRTList = Find_MRT(pMRTTag);

	if (nullptr == pMRTList)
		return E_FAIL;

	/* 장치에 보관되어있던 백버퍼, 깊이스텐실버퍼를 얻어와서 멤버변수에 보관한다.  */
	/* 왜? 새로운 타겟들을 장치에 바인드하면 백버퍼를 잃어버리니까. */
	g_DeviceContext->OMGetRenderTargets(1, m_pBackBufferView.GetAddressOf(), m_pDepthStencilView.GetAddressOf());

	ID3D11RenderTargetView*		pRenderTargets[8] = { nullptr };

	_uint		iIndex = 0;

	for (auto& pRenderTarget : *pMRTList)
	{
		pRenderTarget->Clear();
		pRenderTargets[iIndex++] = pRenderTarget->Get_RTV().Get();
	}

	if (lstrcmpW(L"MRT_LightDepth", pMRTTag))
		g_DeviceContext->OMSetRenderTargets(pMRTList->size(), pRenderTargets, m_pDepthStencilView.Get());
	else
	{
		//얘의 랜더타겟을 저장해야하나 고민. 단순적용인지 저장인지
		g_DeviceContext->OMSetRenderTargets(pMRTList->size(), pRenderTargets, m_pShadowDepthStencilView.Get());
		Clear_ShadowDepth();
	}
	return S_OK;
}

HRESULT CTarget_Manager::End_MRT()
{
	if (nullptr == m_pBackBufferView ||
		nullptr == m_pDepthStencilView)
		return E_FAIL;


	ID3D11ShaderResourceView* null[] = { nullptr, nullptr };
	g_DeviceContext->PSSetShaderResources(0, 2, null);
	g_DeviceContext->OMSetRenderTargets(1, m_pBackBufferView.GetAddressOf(), m_pDepthStencilView.Get());

	return S_OK;
}

HRESULT CTarget_Manager::Create_ShadowDepth(_uint iWidth, _uint iHeight)
{
	ComPtr<ID3D11Texture2D>		pDepthStencilTexture = nullptr;

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = iWidth;
	TextureDesc.Height = iHeight;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(g_Device->CreateTexture2D(&TextureDesc, nullptr, pDepthStencilTexture.GetAddressOf())))
		return E_FAIL;

	/* RenderTarget */
	/* ShaderResource */
	/* DepthStencil */

	if (FAILED(g_Device->CreateDepthStencilView(pDepthStencilTexture.Get(), nullptr, m_pShadowDepthStencilView.GetAddressOf())))
		return E_FAIL;

	return S_OK;
}
HRESULT CTarget_Manager::Clear_ShadowDepth()
{
	g_DeviceContext->ClearDepthStencilView(m_pShadowDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	return S_OK;
}
#ifdef _DEBUG

HRESULT CTarget_Manager::Ready_DebugBuffer(const _tchar * pTargetTag, _uint iLTX, _uint iLTY, _uint iSizeX, _uint iSizeY)
{
	if(nullptr == m_pVIBuffer)
	{
		m_pVIBuffer = CVIBuffer_Rect::Create(TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"));
		if (nullptr == m_pVIBuffer)
			return E_FAIL;
	}

	shared_ptr<CRenderTarget>		pRenderTarget = Find_RenderTarget(pTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Ready_DebugBuffer(iLTX, iLTY, iSizeX, iSizeY);	
}

HRESULT CTarget_Manager::Render_DebugBuffer(const _tchar * pMRTTag, _uint iPassIndex)
{
	list<shared_ptr<CRenderTarget>>*		pMRTList = Find_MRT(pMRTTag);
	if (nullptr == pMRTList)
		return E_FAIL;

	for (auto& pRenderTarget : *pMRTList)
		pRenderTarget->Render_DebugBuffer(m_pVIBuffer, iPassIndex);

	return S_OK;
}
#endif // _DEBUG

shared_ptr<CRenderTarget> CTarget_Manager::Find_RenderTarget(const _tchar * pRenderTargetTag)
{
	if (m_RenderTargets.contains(pRenderTargetTag))
		return m_RenderTargets.at(pRenderTargetTag);

	return nullptr;
}

list<shared_ptr<CRenderTarget>>* CTarget_Manager::Find_MRT(const _tchar * pMRTTag)
{
	if (m_MRTs.contains(pMRTTag))
		return &m_MRTs.at(pMRTTag);

	return nullptr;
}