#pragma once

#include "Base.h"

BEGIN(Engine)

class CGraphic_Device final : public CBase
{
	DECLARE_SINGLETON(CGraphic_Device)

public:
	enum WINMODE { MODE_FULL, MODE_WIN, MODE_END };

public:
	typedef struct tagGraphicDeviceDesc
	{
		HWND			hWnd;
		unsigned int	iWinCX, iWinCY;
		WINMODE			eWinMode;
	}GRAPHICDEVDESC;

public:
	CGraphic_Device();
	virtual ~CGraphic_Device() = default;
public:
	HRESULT Ready_Graphic_Device(HWND hWnd, WINMODE WinMode, _uint iWinCX, _uint iWinCY);
	HRESULT Clear_BackBuffer_View(_float4 vClearColor);
	HRESULT Clear_DepthStencil_View();
	HRESULT Present();


private:	
	//얘네를 전역으로 쓸 생각을 해볼까나...
	/* 메모리 할당. (정점버퍼, 인덱스버퍼, 텍스쳐로드) */
	//ComPtr<ID3D11Device>			m_pDevice;

	/* 바인딩작업. 기능실행 (정점버퍼를 SetStreamSource(), SetIndices(), SetTransform(), SetTexture() */
	//ComPtr<ID3D11DeviceContext>		m_pDeviceContext;
	ComPtr<IDXGISwapChain>			m_pSwapChain;
	// ID3D11ShaderResourceView*
	ComPtr<ID3D11RenderTargetView>		m_pBackBufferRTV ;
	ComPtr<ID3D11DepthStencilView>		m_pDepthStencilView;

private:
	HRESULT Ready_SwapChain(HWND hWnd, WINMODE WinMode, _uint iWinCX, _uint iWinCY);
	HRESULT Ready_BackBufferRenderTargetView();
	HRESULT Ready_DepthStencilRenderTargetView(_uint iWinCX, _uint iWinCY);

public:
	//스마트포인터를 쓰기 전 메모리누수 체크용이였음
	/*virtual void Free() override;*/
};

END