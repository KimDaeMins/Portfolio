#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Trail final : public CVIBuffer
{
private:
	struct TrailData
	{
		_float3 position[2];
		double timecount = 0.0;
		TrailData(const _float3& upposition, const _float3& downposition)
			: timecount(0.0)
		{
			position[0] = upposition;
			position[1] = downposition;
		}
		vector<_float2> LerpGap;
		_vector LastLerpPos[2];
	};

protected:
	CVIBuffer_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CVIBuffer_Trail(const CVIBuffer_Trail& rhs);
	virtual ~CVIBuffer_Trail() = default;
public:
	virtual HRESULT NativeConstruct_Prototype(const _tchar* pShaderFilePath);
	virtual HRESULT NativeConstruct(void* pArg);

	virtual HRESULT Render(_uint iPassIndex);

public: // From. Trail_DX9
	void AddNewTrail(const _float3& upposition, const _float3& downposition);
	void SplineTrailPosition(OUT VTXTEX* vtx, const size_t& dataindex, unsigned long& index, _matrix* pWorldMat);
	void Update(_float fTimeDelta, _matrix* pWorldMat);
	void Set_Color(_fvector Color) { m_Color = Color; }
	void Bind_Color() {
		__super::Set_RawValue("g_Color", &m_Color, sizeof(_vector));
	}
public:
	unsigned long m_vtxSize = 0;
	unsigned long m_maxTrailCnt = 0;
	unsigned long m_maxvtxCnt = 0;
	unsigned long m_maxtriCnt = 0;
	unsigned long m_curTriCnt = 0;
	unsigned long m_curVtxCnt = 0;
	float m_fDuration = 0.0f;
	float m_fAliveTime = 0.0f;
	float m_fTimerAcc = 0.0f;
	
	_float2 m_UVMaxLength = { 0.f, 0.f };
	size_t m_LerpCnt = 0;
	std::deque<TrailData> trailDatas;
	_vector	m_Color = { 0.f, 0.f, 0.f, 0.f };
public:
	static CVIBuffer_Trail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pShaderFilePath);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END
