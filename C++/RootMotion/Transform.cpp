#include "..\Public\Transform.h"
#include "VIBuffer.h"
#include "Model.h"
#include "Navigation.h"
#include "Animator.h"
#include "PipeLine.h"
#include "Shader.h"

CTransform::CTransform(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{
	m_WorldMatrix = {};
	m_WorldMatrix_ForAnimMove = {};
}

CTransform::CTransform(const CTransform & rhs)
	: CComponent(rhs)
	, m_WorldMatrix(rhs.m_WorldMatrix)
	, m_WorldMatrix_ForAnimMove(rhs.m_WorldMatrix_ForAnimMove)
{
}

_vector CTransform::Get_State(STATE eState)
{
	return XMLoadFloat4((_float4*)&m_WorldMatrix.m[eState]);
}

void CTransform::Set_State(STATE eState, _fvector vState)
{
	_float4		vTmp;		

	XMStoreFloat4(&vTmp, vState);
	
	if (eState == STATE_POSITION)
	{
		m_fAccPos += vState  - Get_State(eState);
		vTmp.w = 1.f;
	}

	memcpy(&m_WorldMatrix.m[eState], &vTmp, sizeof(_float4));
}

_float CTransform::Get_Position(AXIS eAxis)
{
	return m_WorldMatrix.m[STATE_POSITION][eAxis];
}

void CTransform::Set_Position(AXIS eAxis, _float fPositionAxis)
{
	_Vector4		vState = Get_State(STATE_POSITION);

	switch (eAxis)
	{
	case AXIS_X:
		vState.x = fPositionAxis;
		break;
	case AXIS_Y:
		vState.y = fPositionAxis;
		break;
	case AXIS_Z:
		vState.z = fPositionAxis;
		break;
	}

	Set_State(STATE_POSITION, vState);
}

void CTransform::Set_TransformDesc(const TRANSFORMDESC & TransformDesc)
{
	m_TransformDesc = TransformDesc;
}

void CTransform::Set_Billboard()
{
	CPipeLine* pPipeLine = (CPipeLine*)GET_INSTANCE(CPipeLine);

	_float3		vScale = Get_Scale_3Dir();

	_matrix		 ViewMatrix;
	ViewMatrix = pPipeLine->Get_Transform(TS_VIEW);
	ViewMatrix = XMMatrixInverse(nullptr, ViewMatrix);
	Set_State(STATE_RIGHT, (_Vector3)ViewMatrix.r[0] * vScale.x);
	Set_State(STATE_UP, (_Vector3)ViewMatrix.r[1] * vScale.y);
	Set_State(STATE_LOOK, (_Vector3)ViewMatrix.r[2] * vScale.z);

	RELEASE_INSTANCE(CPipeLine);
}

void CTransform::Set_YBillboard()
{
	CPipeLine* pPipeLine = (CPipeLine*)GET_INSTANCE(CPipeLine);

	_float3		vScale = Get_Scale_3Dir();

	_matrix		 ViewMatrix;
	ViewMatrix = pPipeLine->Get_Transform(TS_VIEW);
	ViewMatrix = XMMatrixInverse(nullptr, ViewMatrix);
	_Vector3 Right;
	_Vector3 Up = _Vector3(0.f, 1.f, 0.f);
	_Vector3 Look;
	Right.x = XMVectorGetX(ViewMatrix.r[0]);
	Right.z = XMVectorGetZ(ViewMatrix.r[0]);
	Look.x = XMVectorGetX(ViewMatrix.r[2]);
	Look.z = XMVectorGetZ(ViewMatrix.r[2]);
	Set_State(STATE_RIGHT, Right * vScale.x);
	Set_State(STATE_UP, Up * vScale.y);
	Set_State(STATE_LOOK, Look * vScale.z);

	RELEASE_INSTANCE(CPipeLine);
}

void CTransform::Set_Scale(_Vector3 _vScale)
{
	_vector		vRight = Get_State(STATE_RIGHT);
	_vector		vUp = Get_State(STATE_UP);
	_vector		vLook = Get_State(STATE_LOOK);

	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * _vScale.x);
	Set_State(STATE_UP, XMVector3Normalize(vUp) * _vScale.y);
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * _vScale.z);
}

void CTransform::Set_WorldMatrix(physx::PxRigidActor* _Actor)
{
	_Vector3 Scale = Get_Scale_3Dir();
	PxTransform TF = _Actor->getGlobalPose();
	PxMat44 Mat = PxMat44(TF);

	memcpy(&m_WorldMatrix, &Mat, sizeof(_matrix));
	Set_Scale(Scale);
}

void CTransform::Mul_World(_Matrix World)
{
	_Matrix matrix = m_WorldMatrix;
	matrix *= World;
	m_WorldMatrix = matrix;
}

HRESULT CTransform::NativeConstruct_Prototype()
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	return S_OK;
}

HRESULT CTransform::NativeConstruct(void * pArg)
{
	if (nullptr != pArg)
	{
		memcpy(&m_TransformDesc, pArg, sizeof(TRANSFORMDESC));
		m_fOriginSpeedPerSec = m_TransformDesc.fSpeedPerSec;
		m_fOriginRotationPerSec = m_TransformDesc.fRotationPerSec;
	}
	else if(nullptr == pArg)
	{
		m_fOriginSpeedPerSec = m_TransformDesc.fSpeedPerSec = 7.f;
		m_fOriginRotationPerSec = m_TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);		
	}

	return S_OK;
}

HRESULT CTransform::Bind_OnShader(CShader* pShader, const char* pConstantName)
{		
	if (nullptr == pShader)
		return E_FAIL;

	return pShader->Set_RawValue(pConstantName, &m_WorldMatrix, sizeof(_float4x4));
}

void CTransform::Move_Dir(_Vector3 Dir, _float fTimeDelta, CNavigation* pNaviCom)
{
	_vector		Position = Get_State(STATE_POSITION);

	Position += Vec3Normalize(Dir) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	if (pNaviCom)
	{
		if (pNaviCom->isMove(Position))
		{
			Set_State(STATE_POSITION, _Vector4(Position, 1.f));
		}
	}
	else
		Set_State(STATE_POSITION, _Vector4(Position, 1.f));
}

void CTransform::Move_Pos(_Vector3 Pos, _float fTimeDelta, CNavigation* pNaviCom)
{
	_Vector3	Position = Get_State(STATE_POSITION);
	_Vector3	Dir = Pos - Position;

	Position += Vec3Normalize(Dir) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	if (pNaviCom)
	{
		if (pNaviCom->isMove(Position))
		{
			Set_State(STATE_POSITION, _Vector4(Position, 1.f));
		}
	}
	else
		Set_State(STATE_POSITION, _Vector4(Position, 1.f));
}

void CTransform::Go_Straight(_float fTimeDelta, CNavigation* pNaviCom)
{
	_vector        vPosition = Get_State(STATE_POSITION);
	_vector        vLook = Get_State(STATE_LOOK);

	vPosition += XMVector3Normalize(vLook) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	if (nullptr != pNaviCom)
	{
		if (pNaviCom->isMove(vPosition) == true)
		{
			_float vPosY = pNaviCom->Compute_Height(vPosition);
			vPosition = XMVectorSet(XMVectorGetX(vPosition), vPosY, XMVectorGetZ(vPosition), 1.f);
			Set_State(STATE_POSITION, vPosition);
		}
		else
		{
			if (pNaviCom->FindCurrentCell(vPosition))
			{
				_float vPosY = pNaviCom->Compute_Height(vPosition);			
				vPosition = XMVectorSet(XMVectorGetX(vPosition), vPosY, XMVectorGetZ(vPosition), 1.f);
				Set_State(STATE_POSITION, vPosition);
			}
		}
	}
	else if( nullptr == pNaviCom)
		Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Backward(_float fTimeDelta, CNavigation* pNaviCom)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vLook = Get_State(STATE_LOOK);

	vPosition -= XMVector3Normalize(vLook) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	if (nullptr != pNaviCom)
	{
		if (pNaviCom->isMove(vPosition) == true)
		{
			_float vPosY = pNaviCom->Compute_Height(vPosition);
			vPosition = XMVectorSet(XMVectorGetX(vPosition), vPosY, XMVectorGetZ(vPosition), 1.f);
			Set_State(STATE_POSITION, vPosition);
		}
		else
		{
			if (pNaviCom->FindCurrentCell(vPosition))
			{
				_float vPosY = pNaviCom->Compute_Height(vPosition);
				vPosition = XMVectorSet(XMVectorGetX(vPosition), vPosY, XMVectorGetZ(vPosition), 1.f);
				Set_State(STATE_POSITION, vPosition);
			}
		}
	}
	else if (nullptr == pNaviCom)
		Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Left(_float fTimeDelta, CNavigation* pNaviCom)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vRight = Get_State(STATE_RIGHT);

	vPosition -= XMVector3Normalize(vRight) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	if (nullptr != pNaviCom)
	{
		if (pNaviCom->isMove(vPosition) == true)
		{
			_float vPosY = pNaviCom->Compute_Height(vPosition);
			vPosition = XMVectorSet(XMVectorGetX(vPosition), vPosY, XMVectorGetZ(vPosition), 1.f);
			Set_State(STATE_POSITION, vPosition);
		}
		else
		{
			if (pNaviCom->FindCurrentCell(vPosition))
			{
				_float vPosY = pNaviCom->Compute_Height(vPosition);
				vPosition = XMVectorSet(XMVectorGetX(vPosition), vPosY, XMVectorGetZ(vPosition), 1.f);
				Set_State(STATE_POSITION, vPosition);
			}
		}
	}
	else if (nullptr == pNaviCom)
		Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Right(_float fTimeDelta, CNavigation* pNaviCom)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vRight = Get_State(STATE_RIGHT);

	vPosition += XMVector3Normalize(vRight) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	if (nullptr != pNaviCom)
	{
		if (pNaviCom->isMove(vPosition) == true)
		{
			_float vPosY = pNaviCom->Compute_Height(vPosition);
			vPosition = XMVectorSet(XMVectorGetX(vPosition), vPosY, XMVectorGetZ(vPosition), 1.f);
			Set_State(STATE_POSITION, vPosition);
		}
		else
		{
			if (pNaviCom->FindCurrentCell(vPosition))
			{
				_float vPosY = pNaviCom->Compute_Height(vPosition);
				vPosition = XMVectorSet(XMVectorGetX(vPosition), vPosY, XMVectorGetZ(vPosition), 1.f);
				Set_State(STATE_POSITION, vPosition);
			}
		}
	}
	else if (nullptr == pNaviCom)
		Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Up(_float fTimeDelta, CNavigation* pNaviCom)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vLook = Get_State(STATE_LOOK);
	_vector		vRight = Get_State(STATE_RIGHT);

	_vector		vUp = XMVector3Cross(XMVector3Normalize(vLook), XMVector3Normalize(vRight));

	vPosition += XMVector3Normalize(vUp) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	if (nullptr != pNaviCom)
	{
		if (pNaviCom->isMove(vPosition) == true)
		{
			_float vPosY = pNaviCom->Compute_Height(vPosition);
			vPosition = XMVectorSet(XMVectorGetX(vPosition), vPosY, XMVectorGetZ(vPosition), 1.f);
			Set_State(STATE_POSITION, vPosition);
		}
		else
		{
			if (pNaviCom->FindCurrentCell(vPosition))
			{
				_float vPosY = pNaviCom->Compute_Height(vPosition);
				vPosition = XMVectorSet(XMVectorGetX(vPosition), vPosY, XMVectorGetZ(vPosition), 1.f);
				Set_State(STATE_POSITION, vPosition);
			}
		}
	}
	else if (nullptr == pNaviCom)
		Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Down(_float fTimeDelta, CNavigation* pNaviCom)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vLook = Get_State(STATE_LOOK);
	_vector		vRight = Get_State(STATE_RIGHT);

	_vector		vDown = XMVector3Cross(XMVector3Normalize(vRight), XMVector3Normalize(vLook));

	vPosition += XMVector3Normalize(vDown) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	if (nullptr != pNaviCom)
	{
		if (pNaviCom->isMove(vPosition) == true)
		{
			_float vPosY = pNaviCom->Compute_Height(vPosition);
			vPosition = XMVectorSet(XMVectorGetX(vPosition), vPosY, XMVectorGetZ(vPosition), 1.f);
			Set_State(STATE_POSITION, vPosition);
		}
		else
		{
			if (pNaviCom->FindCurrentCell(vPosition))
			{
				_float vPosY = pNaviCom->Compute_Height(vPosition);
				vPosition = XMVectorSet(XMVectorGetX(vPosition), vPosY, XMVectorGetZ(vPosition), 1.f);
				Set_State(STATE_POSITION, vPosition);
			}
		}
	}
	else if (nullptr == pNaviCom)
		Set_State(STATE_POSITION, vPosition);
}

void CTransform::Rotation(_fvector vAxis, _float fRadian)
{
	_matrix		RotationMatrix;

	RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);

	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * Get_Scale(STATE_RIGHT);
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * Get_Scale(STATE_UP);
	_vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * Get_Scale(STATE_LOOK);

	vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	vUp = XMVector3TransformNormal(vUp, RotationMatrix);
	vLook = XMVector3TransformNormal(vLook, RotationMatrix);

	Set_State(STATE_RIGHT, vRight);
	Set_State(STATE_UP, vUp);
	Set_State(STATE_LOOK, vLook);
}


void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
	_matrix		RotationMatrix;

	RotationMatrix = XMMatrixRotationAxis(vAxis, m_TransformDesc.fRotationPerSec * fTimeDelta);
// 	Quaternion::CreateFromAxisAngle();
// 	Quaternion::Slerp();

	_vector		vRight = Get_State(STATE_RIGHT);
	_vector		vUp = Get_State(STATE_UP);
	_vector		vLook = Get_State(STATE_LOOK);

	vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	vUp = XMVector3TransformNormal(vUp, RotationMatrix);
	vLook = XMVector3TransformNormal(vLook, RotationMatrix);

	Set_State(STATE_RIGHT, vRight);
	Set_State(STATE_UP, vUp);
	Set_State(STATE_LOOK, vLook);
}


void CTransform::Set_Rotate(_fvector Right, _fvector Up, _fvector Look)
{
	_vector		vRight = Right;
	_vector		vUp = Up;
	_vector		vLook = Look;
	Set_State(STATE_RIGHT, vRight * Get_Scale(STATE_RIGHT));
	Set_State(STATE_UP, vUp * Get_Scale(STATE_UP));
	Set_State(STATE_LOOK, vLook * Get_Scale(STATE_LOOK));
}

void CTransform::LookAt(_fvector vTargetPos)
{
	_vector		vPosition = Get_State(STATE_POSITION);

	_vector		vLook = vTargetPos - vPosition;
	_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	_vector		vUp = XMVector3Cross(vLook, vRight);	

	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * Get_Scale(STATE_RIGHT));
	Set_State(STATE_UP, XMVector3Normalize(vUp) * Get_Scale(STATE_UP));
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * Get_Scale(STATE_LOOK));

}

void CTransform::LookAtSlerp(_fvector vTargetPos, _float fTimeDelta, _bool bIgnoreY)
{
	_vector		vPosition = Get_State(STATE_POSITION);

	_Vector3	vLook = vTargetPos - vPosition;
	if (vLook.y < -0.99f)
		vLook.y = -0.99f;
	if (bIgnoreY)
		vLook.y = _Vector3(Get_State(STATE_LOOK)).y;
	vLook = XMVector3Normalize(vLook);
	_Vector3	vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	_Vector3	vUp = XMVector3Cross(vLook, vRight);

	_float fDotResult = vLook.Dot(Get_State(STATE_LOOK));

	if (0.f < fDotResult)
	{
		_Vector3	vNewRight = XMQuaternionSlerp(Get_State(STATE_RIGHT), XMVector3Normalize(vRight) * Get_Scale(STATE_RIGHT), fTimeDelta);
		_Vector3	vNewUp = XMQuaternionSlerp(Get_State(STATE_UP), XMVector3Normalize(vUp) * Get_Scale(STATE_UP), fTimeDelta);
		_Vector3	vNewLook = XMQuaternionSlerp(Get_State(STATE_LOOK), XMVector3Normalize(vLook) * Get_Scale(STATE_LOOK), fTimeDelta);

		if (0.f < vNewUp.y)
		{
			Set_State(STATE_RIGHT, vNewRight);
			Set_State(STATE_UP, vNewUp);
			Set_State(STATE_LOOK, vNewLook);
		}
	}
	else
	{
		if (vRight.Dot(Get_State(STATE_RIGHT)) < 0.f)
		{
			if(0.f < vRight.Dot(Get_State(STATE_LOOK)))
				Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), -fTimeDelta);
			else
				Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);
		}
		else
		{
			if (0.f < vRight.Dot(Get_State(STATE_LOOK)))
				Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);
			else
				Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), -fTimeDelta);
		}
	}
}

void CTransform::LookAtDir(_fvector vDir, _bool YLock)
{
	if (XMVectorGetX(XMVector3Length(vDir)) == 0.f)
		return;

	if (YLock)
	{
		_vector		vPosition = Get_State(STATE_POSITION);

		_vector		vLook = XMVectorSet(XMVectorGetX(vDir), m_WorldMatrix._32, XMVectorGetZ(vDir), 0.f);
		_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
		_vector		vUp = XMVector3Cross(vLook, vRight);

		Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * Get_Scale(STATE_RIGHT));
		Set_State(STATE_UP, XMVector3Normalize(vUp) * Get_Scale(STATE_UP));
		Set_State(STATE_LOOK, XMVector3Normalize(vLook) * Get_Scale(STATE_LOOK));
	}
	else
	{
		_vector		vPosition = Get_State(STATE_POSITION);

		_vector		vLook = vDir;
		_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
		_vector		vUp = XMVector3Cross(vLook, vRight);

		Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * Get_Scale(STATE_RIGHT));
		Set_State(STATE_UP, XMVector3Normalize(vUp) * Get_Scale(STATE_UP));
		Set_State(STATE_LOOK, XMVector3Normalize(vLook) * Get_Scale(STATE_LOOK));
	}

}

void CTransform::MoveUsingAnimation(CAnimator* pAnimator, CNavigation* pNaviCom, _bool bNeedNormalizePivot, _bool bNeedWorldUpdata)
{
	if (pAnimator->Get_IsBlending() || !pAnimator->Get_UseDiff())
		return;

	if (bNeedWorldUpdata || pAnimator->Get_CurKeyFrame() == 0)
		m_WorldMatrix_ForAnimMove = m_WorldMatrix;

	_Matrix Pivot =  pAnimator->Get_PivotMatrix();
	Pivot.Translation(_Vector3(0.f, 0.f, 0.f));
	if(bNeedNormalizePivot)
		Pivot = _Matrix::Normalize(Pivot);
	_Matrix DiffMatrix = XMLoadFloat4x4(&pAnimator->GetDiffMatrix());
	DiffMatrix.Translation(_Vector3::Transform(DiffMatrix.Translation(), Pivot));
	DiffMatrix._41 = DiffMatrix._41 * pAnimator->Get_MoveUsingAnimDir();
	DiffMatrix._43 = DiffMatrix._43 * pAnimator->Get_MoveUsingAnimDir();

	_Matrix RotationMatrix = m_WorldMatrix_ForAnimMove;
	RotationMatrix.Translation(_Vector3(0.f, 0.f, 0.f));
	
	_Matrix Translation = _Matrix::CreateTranslation(_Vector3::Transform(DiffMatrix.Translation(), RotationMatrix));
	DiffMatrix.Translation(_Vector3(0.f, 0.f, 0.f));
	m_WorldMatrix = DiffMatrix * m_WorldMatrix * Translation;

	//------------------------------
	// 과거 알고리즘 ( 삭제 예정 )
	//------------------------------
	//_matrix DiffMatrix	= XMLoadFloat4x4(&pAnimator->GetDiffMatrix());
	//// fDir값을 받아오는 대신 animator에 멤버변수로 저장하는 방식으로 수정
	//_Vector3 MoveRange = DiffMatrix.r[3];
	//// 여기서 분기를 한다 ( xz는 같이, y는 따로 )
	//_vector vYMove = MoveRange.y * Get_State(STATE_UP);
	//MoveRange.y = 0.f;
	//_vector vDiffScalar = MoveRange.Length() * Get_State(STATE_LOOK) * pAnimator->Get_MoveUsingAnimDir();
	//_vector vPosition	= Get_State(STATE_POSITION) + vDiffScalar + vYMove;

	//if (nullptr != pNaviCom)
	//{
	//	if (pNaviCom->isMove(vPosition) == true)
	//	{
	//		_float vPosY = pNaviCom->Compute_Height(vPosition);
	//		vPosition = XMVectorSet(XMVectorGetX(vPosition), vPosY, XMVectorGetZ(vPosition), 1.f);
	//		Set_State(STATE_POSITION, vPosition);
	//	}
	//	else
	//	{
	//		if (pNaviCom->FindCurrentCell(vPosition))
	//		{
	//			_float vPosY = pNaviCom->Compute_Height(vPosition);
	//			vPosition = XMVectorSet(XMVectorGetX(vPosition), vPosY, XMVectorGetZ(vPosition), 1.f);
	//			Set_State(STATE_POSITION, vPosition);
	//		}
	//	}
	//}
	//else if (nullptr == pNaviCom)
	//	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Set_Y(_float Y)
{
	m_WorldMatrix._42 = Y;
}

_float CTransform::Get_Y()
{
	return m_WorldMatrix._42;
}



CTransform * CTransform::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CTransform*		pInstance = new CTransform(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CTransform");
		Safe_Release(pInstance);
	}

	return pInstance; 
}

CComponent * CTransform::Clone(void * pArg)
{
	CTransform*		pInstance = new CTransform(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CTransform");
		Safe_Release(pInstance);
	}

	return pInstance; 
}

void CTransform::Free()
{
	__super::Free();
}
