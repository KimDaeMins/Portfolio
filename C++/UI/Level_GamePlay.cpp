#include "stdafx.h"
#include "..\public\Level_GamePlay.h"
#include "Level_Loading.h"
#include "GameInstance.h"
#include "Camera_Fly.h"
#include "MeshObject.h"

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CLevel(pDevice, pDeviceContext)
{
}

HRESULT CLevel_GamePlay::NativeConstruct()
{
	if (FAILED(__super::NativeConstruct()))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Object(TEXT("Layer_Object"))))
		return E_FAIL;

	if (FAILED(Load_UI(TEXT("../Bin/Data/BaseUIBatch.dat"))))
		return E_FAIL;

	if (FAILED(Ready_LightDesc()))
		return E_FAIL;


	return S_OK;
}

_int CLevel_GamePlay::Tick(_float fTimeDelta)
{
	if (0 > (__super::Tick(fTimeDelta)))
		return -1;

	m_TimeAcc += fTimeDelta;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (!m_BossZen &&pGameInstance->Get_LayerSize(LEVEL_GAMEPLAY, TEXT("Layer_Monster")) < 2)
	{
		m_BossZen = true;
		HANDLE hFile = CreateFile(TEXT("../Bin/Data/MonsterWave1.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

		if (INVALID_HANDLE_VALUE == hFile)
			return E_FAIL;

		DWORD dwByte = 0;
		DWORD dwStrByte = 0;
		_tchar*	pBuf;
		PUSHOBJECTDATA Info = {};
		while (true)
		{
			wstring ModelName = L"";
			//ModelName
			{
				ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
				if (0 == dwByte)
					break;

				pBuf = new wchar_t[dwStrByte];
				ReadFile(hFile, pBuf, dwStrByte, &dwByte, nullptr);
				ModelName = pBuf;
				if (pBuf)
				{
					delete[] pBuf;
					pBuf = nullptr;
				}
			}

			ReadFile(hFile, &Info.Position, sizeof(_float3), &dwByte, nullptr);
			ReadFile(hFile, &Info.Scale, sizeof(_float3), &dwByte, nullptr);
			ReadFile(hFile, &Info.Rotation, sizeof(_float3), &dwByte, nullptr);
			ReadFile(hFile, &Info.MaxHp, sizeof(_uint), &dwByte, nullptr);
			ReadFile(hFile, &Info.MonReact, sizeof(_bool), &dwByte, nullptr);
			ReadFile(hFile, &Info.GrapAble, sizeof(_bool), &dwByte, nullptr);
			ReadFile(hFile, &Info.DestroyAble, sizeof(_bool), &dwByte, nullptr);

			if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Boss"), TEXT("Prototype_GameObject_BossOgre"), &Info)))
				return E_FAIL;
		}
		CloseHandle(hFile);
	}
	RELEASE_INSTANCE(CGameInstance);


	return _int(0);
}

HRESULT CLevel_GamePlay::Render()
{
	++m_iNumDraw;

	if (m_TimeAcc >= 1.0)
	{
		wsprintf(m_szFPS, TEXT("FPS : %d"), m_iNumDraw);
		SetWindowText(g_hWnd, m_szFPS);

		m_TimeAcc = 0.0;
		m_iNumDraw = 0;
	}
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_LightDesc()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	LIGHTDESC			LightDesc;
	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

	LightDesc.eType = tagLightDesc::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float3(1.f, -1.f, 1.f);

	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;

	LightDesc.eType = tagLightDesc::TYPE_POINT;
	LightDesc.vPosition = _float3(5.0f, 3.0f, 5.f);
	LightDesc.fRadiuse = 5.f;

	LightDesc.vDiffuse = _float4(1.f, 0.1f, 0.1f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.1f, 0.1f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;

	LightDesc.eType = tagLightDesc::TYPE_POINT;
	LightDesc.vPosition = _float3(10.0f, 3.0f, 5.f);
	LightDesc.fRadiuse = 5.f;

	LightDesc.vDiffuse = _float4(0.1f, 1.f, 0.1f, 1.f);
	LightDesc.vAmbient = _float4(0.1f, 1.f, 0.1f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	/* 월드스페이스 상에서의 카메라 상태를 셋팅하자. */
	CCamera::CAMERADESC			CameraDesc;
	ZeroMemory(&CameraDesc, sizeof(CCamera::CAMERADESC));

	CameraDesc.vEye = _float3(0.f, 30.f, -25.0f);
	CameraDesc.vAt = _float3(0.f, 0.f, 0.f);
	CameraDesc.vAxisy = _float3(0.f, 1.f, 0.f);
	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = (_float)g_iWinCX / g_iWinCY;
	CameraDesc.fNear = 0.2f;
	CameraDesc.fFar = 600.f;

	CameraDesc.TransformDesc.fSpeedPerSec = 10.f;
	CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Camera_Fly"), &CameraDesc)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);	

	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Sky"))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);	

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	PUSHWEAPONDESC WeaponDesc = {};
	WeaponDesc.WeaponName = TEXT("Blade");
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Weapon"), TEXT("Prototype_GameObject_Blade"), &WeaponDesc)))
		return E_FAIL;

	WeaponDesc.WeaponName = TEXT("Gun");
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Weapon"), TEXT("Prototype_GameObject_Gun"), &WeaponDesc)))
		return E_FAIL;

	WeaponDesc.WeaponName = TEXT("ArmRocket");
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Weapon"), TEXT("Prototype_GameObject_ArmRocket"), &WeaponDesc)))
		return E_FAIL;

	PUSHDESC Desc = {};
	Desc.ApplyHp = 3000;
	Desc.DamagedCount = 100;
	Desc.DamageRate = 100;
	Desc.MaxGage = 1000;
	Desc.Speed = 50.f;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Inven"), TEXT("Prototype_GameObject_Arm"), &Desc)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Inven"), TEXT("Prototype_GameObject_Body"), &Desc)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Inven"), TEXT("Prototype_GameObject_Head"), &Desc)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Inven"), TEXT("Prototype_GameObject_Leg"), &Desc)))
		return E_FAIL;
	for (_uint i = 0; i < 1; ++i)
	{
		if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Player"))))
			return E_FAIL;
	}

		HANDLE hFile = CreateFile(TEXT("../Bin/Data/MonsterWave0.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

		if (INVALID_HANDLE_VALUE == hFile)
			return E_FAIL;

		DWORD dwByte = 0;
		DWORD dwStrByte = 0;
		_tchar*	pBuf;
		PUSHOBJECTDATA Info = {};
		while (true)
		{
			wstring ModelName = L"";
			wstring PrototypeName = L"Prototype_GameObject_";
			//ModelName
			{
				ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
				if (0 == dwByte)
					break;

				pBuf = new wchar_t[dwStrByte];
				ReadFile(hFile, pBuf, dwStrByte, &dwByte, nullptr);
				ModelName = pBuf;
				if (pBuf)
				{
					delete[] pBuf;
					pBuf = nullptr;
				}
			}
			PrototypeName += ModelName;

			ReadFile(hFile, &Info.Position, sizeof(_float3), &dwByte, nullptr);
			ReadFile(hFile, &Info.Scale, sizeof(_float3), &dwByte, nullptr);
			ReadFile(hFile, &Info.Rotation, sizeof(_float3), &dwByte, nullptr);
			ReadFile(hFile, &Info.MaxHp, sizeof(_uint), &dwByte, nullptr);
			ReadFile(hFile, &Info.MonReact, sizeof(_bool), &dwByte, nullptr);
			ReadFile(hFile, &Info.GrapAble, sizeof(_bool), &dwByte, nullptr);
			ReadFile(hFile, &Info.DestroyAble, sizeof(_bool), &dwByte, nullptr);

			if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), PrototypeName.c_str(), &Info)))
				return E_FAIL;
		}
		CloseHandle(hFile);


		RELEASE_INSTANCE(CGameInstance);



	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Object(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	HANDLE hFile = CreateFile(L"../Bin/Data/MapData3.dat", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	DWORD dwByte = 0;
	DWORD dwStrByte = 0;
	_tchar*	pBuf;
	CMeshObject::OBJECTDESC Info = {};

	while (true)
	{
		//ModelName
		{
			ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
			if (0 == dwByte)
				break;

			pBuf = new wchar_t[dwStrByte];
			ReadFile(hFile, pBuf, dwStrByte, &dwByte, nullptr);
			Info.ModelName = pBuf;
			if (pBuf)
			{
				delete[] pBuf;
				pBuf = nullptr;
			}
		}
		ReadFile(hFile, &Info.Position, sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &Info.Scale, sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &Info.Rotation, sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &Info.MaxHp, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &Info.MonReact, sizeof(_bool), &dwByte, nullptr);
		ReadFile(hFile, &Info.GrapAble, sizeof(_bool), &dwByte, nullptr);
		ReadFile(hFile, &Info.DestroyAble, sizeof(_bool), &dwByte, nullptr);

		pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_MeshObject"), &Info);
	}

	CloseHandle(hFile);

	hFile = CreateFile(L"../Bin/Data/ObjectCollider0.dat", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	dwByte = 0;
	COLLIDERDATA Data = {};
	_uint	ObjectIndex = 0;
	_uint Dummy;
	ReadFile(hFile, &Dummy, sizeof(_uint), &dwByte, nullptr);

	while (true)
	{
		ReadFile(hFile, &Data.isBonecollider, sizeof(_bool), &dwByte, nullptr);
		if (0 == dwByte)
			break;
		ReadFile(hFile, &Data.BoneIndex, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &Data.Type, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &Data.Pivot, sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &Data.Size, sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &Data.Radius, sizeof(_float), &dwByte, nullptr);
		ReadFile(hFile, &ObjectIndex, sizeof(_uint), &dwByte, nullptr);

		//만든 콜라이더를 이 인덱스를 통해 집어넣는다?
		((CMeshObject*)pGameInstance->Get_GameObjectPtr(LEVEL_GAMEPLAY, pLayerTag, ObjectIndex))->Push_Collider(&Data);
	}

	CloseHandle(hFile);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Effect(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Load_UI(const _tchar * pFilePath)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	HANDLE hFile = CreateFile(pFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	DWORD dwByte = 0;
	DWORD dwStrByte = 0;
	_tchar*	pBuf;
	UIDATA Info = {};
	while (true)
	{
		//Texture_Prototype_Component
		{
			ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
			if (0 == dwByte)
				break;

			pBuf = new wchar_t[dwStrByte];
			ReadFile(hFile, pBuf, dwStrByte, &dwByte, nullptr);
			Info.Texture_Prototype = pBuf;
			if (pBuf)
			{
				delete[] pBuf;
				pBuf = nullptr;
			}
		}
		ReadFile(hFile, &Info.SizeX, sizeof(_float), &dwByte, nullptr);
		ReadFile(hFile, &Info.SizeY, sizeof(_float), &dwByte, nullptr);
		ReadFile(hFile, &Info.PosX, sizeof(_float), &dwByte, nullptr);
		ReadFile(hFile, &Info.PosY, sizeof(_float), &dwByte, nullptr);
		ReadFile(hFile, &Info.Alpha, sizeof(_float), &dwByte, nullptr);
		ReadFile(hFile, &Info.ChildIndex, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &Info.Rotate, sizeof(_float), &dwByte, nullptr);
		ReadFile(hFile, &Info.Pass, sizeof(_uint), &dwByte, nullptr);

		if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, TEXT("Layer_UIPlayer"), TEXT("Prototype_UIPlayer"), &Info)))
			return E_FAIL;
	}
	CloseHandle(hFile);
	return S_OK;
	

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CLevel_GamePlay * CLevel_GamePlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLevel_GamePlay*		pInstance = new CLevel_GamePlay(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSG_BOX("Failed to Created CLevel_GamePlay");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GamePlay::Free()
{
	__super::Free();
}
