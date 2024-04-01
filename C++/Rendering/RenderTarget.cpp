#include "..\Public\RenderTarget.h"
#include "VIBuffer_Rect.h"

CRenderTarget::CRenderTarget()
{
}

HRESULT CRenderTarget::NativeConstruct(_uint iWidth, _uint iHeight, DXGI_FORMAT eFormat, _float4 vClearColor)
{
	D3D11_TEXTURE2D_DESC		TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));	

	TextureDesc.Width = iWidth;
	TextureDesc.Height = iHeight;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = eFormat;

	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.SampleDesc.Quality = 0;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	if (FAILED(g_Device->CreateTexture2D(&TextureDesc, nullptr, m_pTexture.GetAddressOf())))
		return E_FAIL;

	D3D11_SHADER_RESOURCE_VIEW_DESC			SRVDesc;
	ZeroMemory(&SRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

	SRVDesc.Format = eFormat;
	SRVDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;

	if (FAILED(g_Device->CreateShaderResourceView(m_pTexture.Get(), &SRVDesc, m_pSRV.GetAddressOf())))
		return E_FAIL;

	D3D11_RENDER_TARGET_VIEW_DESC			RTVDesc;
	ZeroMemory(&RTVDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));

	RTVDesc.Format = eFormat;
	RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;		
	RTVDesc.Texture2D.MipSlice = 0;

	if (FAILED(g_Device->CreateRenderTargetView(m_pTexture.Get(), &RTVDesc, m_pRTV.GetAddressOf())))
		return E_FAIL;

	m_vClearColor = vClearColor;

	return S_OK;
}

HRESULT CRenderTarget::Clear()
{
	g_DeviceContext->ClearRenderTargetView(m_pRTV.Get(), (_float*)&m_vClearColor);

	return S_OK;
}

#ifdef _DEBUG
HRESULT CRenderTarget::Ready_DebugBuffer(_uint iLTX, _uint iLTY, _uint iSizeX, _uint iSizeY)
{
	_float			fCenterX, fCenterY;

	fCenterX = iLTX + iSizeX * 0.5f;
	fCenterY = iLTY + iSizeY * 0.5f;

	D3D11_VIEWPORT		ViewportDesc;

	_uint			iNumViewport = 1;

	g_DeviceContext->RSGetViewports(&iNumViewport, &ViewportDesc);

	XMStoreFloat4x4(&m_TransformMatrix, XMMatrixIdentity());
	m_TransformMatrix._11 = _float(iSizeX);
	m_TransformMatrix._22 = _float(iSizeY);
	m_TransformMatrix._41 = fCenterX - (ViewportDesc.Width * 0.5f);
	m_TransformMatrix._42 = -fCenterY + (ViewportDesc.Height * 0.5f);

	XMStoreFloat4x4(&m_OrthoMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f)));

	return S_OK;
}
HRESULT CRenderTarget::Render_DebugBuffer(shared_ptr<CVIBuffer_Rect> pVIBuffer, _uint iPassIndex)
{
	pVIBuffer->Set_RawValue("g_TransformMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&m_TransformMatrix)), sizeof(_float4x4));
	pVIBuffer->Set_RawValue("g_ProjMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&m_OrthoMatrix)), sizeof(_float4x4));
	pVIBuffer->Set_ShaderResourceView("g_TargetTexture", m_pSRV.Get());

	pVIBuffer->Render(iPassIndex);

	return S_OK;
}
#endif // _DEBUG

shared_ptr<CRenderTarget> CRenderTarget::Create(_uint iWidth, _uint iHeight, DXGI_FORMAT eFormat, _float4 vClearColor)
{
	shared_ptr<CRenderTarget>	pInstance = make_shared<CRenderTarget>();

	if (FAILED(pInstance->NativeConstruct(iWidth, iHeight, eFormat, vClearColor)))
	{
		MSG_BOX("Failed To Creating CRenderTarget");
		return nullptr;
	}
	return pInstance;
}
