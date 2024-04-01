#pragma once
#include "Engine_Defines.h"

BEGIN(Engine)

class ENGINE_DLL CBounding :  public enable_shared_from_this<CBounding>
{
public:
	enum TYPE { TYPE_AABB, TYPE_OBB, TYPE_SPHERE, TYPE_END };

private:
	typedef struct OBBDesc
	{
		_float3		vCenter;
		_float3		vExtentDirs[3];
		_float3		vAlignAxis[3];
	}OBBDESC;

public:
	CBounding(_float3 Center, _float3 Extents, _float4 Orientation);
	CBounding(_float3 Center, _float3 Extents);
	CBounding(_float3 Center, _float Radius);

	static const size_t CORNER_COUNT = 8;

private:
	TYPE					m_eType = TYPE_END;
	_float3					m_BaseCenter;
	_float3					m_BaseExtents;
	_float4					m_BaseOrientation;
	_float					m_BaseRadius;

	_float3					m_Center;
	_float3					m_Extents;
	_float4					m_Orientation;
	_float					m_Radius;
	_float					m_CorrectionValue = 1.2f;
public :
	TYPE Get_Type()				{	return m_eType;		}
	_float3 Get_Center()		{	return m_Center;	}
	_float3 Get_Extents()		{	return m_Extents;	}
	_float4 Get_Orientation()	{	return m_Orientation;	}
	_float Get_Radius()			{	return m_Radius;	}

	void Update(_matrix M);


	_bool Collision_Ray(_fvector _Origin, _fvector _Diraction, _float& _Distance);

	_matrix Remove_Rotation(_matrix TransformMatrix);
	
	//상대가 AABB여도 OBBCollision을 해야됨
	//상대가 OBB면 내가 AABB여도 OBBCollision을 해야됨
	//AABB to AABB만 AABBCollision을 해야됨
	//상대 혹은 내가 구형태라면 구충돌을 하는데 Box to Sphere는 따로있을듯?
	_bool Collision(shared_ptr<CBounding> pTargetBounding);

private:
	_bool AABBtoAABB(shared_ptr<CBounding> _Bounding);
	_bool OBBtoOBB(shared_ptr<CBounding> _Bounding);
	_bool OBBtoSphere(shared_ptr<CBounding> _Bounding);
	_bool AABBtoSphere(shared_ptr<CBounding> _Bounding);
	_bool SpheretoSphere(shared_ptr<CBounding> _Bounding);
	static CBounding::OBBDESC Compute_OBBDesc(shared_ptr<CBounding> _Bounding);
};

END

