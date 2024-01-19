#include "stdafx.h"
#include "..\public\IceDaggerLighting.h"

#include "GameInstance.h"
#include "WandBulletExplosion.h"
#include "Thunder.h"
CIceDaggerLighting::CIceDaggerLighting(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CIceDaggerLighting::CIceDaggerLighting(const CIceDaggerLighting& rhs)
	: CGameObject(rhs)
{
}

HRESULT CIceDaggerLighting::NativeConstruct_Prototype()
{

	return S_OK;
}

HRESULT CIceDaggerLighting::NativeConstruct(void* pArg)
{
	if (!pArg)
		return E_FAIL;
	if (FAILED(SetUp_Component()))
		return E_FAIL;

	CTransform::TRANSFORMDESC Desc;
	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(180.f);
	m_pTransformCom->Set_TransformDesc(Desc);

	ICELIGHTINGDATA Data = *(ICELIGHTINGDATA*)pArg;

	Create_Status((_uint)DAMEGETYPE_TYPE::ICEDAGGER, Data.Damege);
	m_pTransformCom->Set_State(STATE_POSITION, _Vector4(Data.Position, 1.f));
	m_pTransformCom->LookAtDir(_Vector4(Data.Look, 0.f), false);
	m_Type = (_uint)OBJECT_TYPE::OBJ_END;

	CRigid::PUSHRIGIDDATA RigidDesc;
	RigidDesc.StaticRigid = TRUE;
	if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Rigid"), TEXT("Com_RigidBody"), (CComponent**)&m_pRigid, &RigidDesc)))
		return E_FAIL;

	wstring ColliderName = L"Rigid";
	m_pRigid->Set_Host(this);
	m_pRigid->Create_SphereCollider(ColliderName, 0.3f, _Vector3(0.f, 0.f, 3.f));
	m_pRigid->Attach_Shape();
	m_pRigid->Set_Transform(m_pTransformCom->Get_WorldMatrix());
	m_pRigid->SetUp_Filtering(tagFilterGroup::NONEHITPLAYEROBJECT);
	m_pRigid->Set_Gravity(false);
	m_pRigid->Set_isQuery(ColliderName, false);
	m_pRigid->Set_isSimulation(ColliderName, false);
	m_pRigid->Set_isTrigger(ColliderName, true);

	//m_pTransformCom->Turn(m_pTransformCom->Get_State(STATE_LOOK), Data.Number * 5.f);
	m_pRenderTransform->Set_Scale(_Vector3(0.15f, 0.15f, 0.04f));
	return S_OK;
}


_int CIceDaggerLighting::Tick(_float fTimeDelta)
{
	CGameObject* pGameObject = nullptr;
	
	if(m_fFadeAmount <= 1.0f)
		m_fFadeAmount += fTimeDelta;
	_float Speed  = CEasing::Lerp(5, 80, m_fFadeAmount, CEasing::EaseType::easeInOutQuint);
	m_pTransformCom->Set_SpeedPerSec(Speed);

	//---------------------------------------------
	// 여기서 충돌 체크를 한다
	//---------------------------------------------
	while (!m_bFirstCollision && m_bDead == false && m_pRigid->Get_Collision_Trigger_Enter(&pGameObject))
	{
		if (pGameObject->Get_Type() == (_uint)OBJECT_TYPE::MONSTER)
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

			pGameInstance->Play(L"pl_itm_wep_techbow_impact_", MULTY, 16, 3, m_pTransformCom);

			m_bFirstCollision = true;
			m_FirstTouch = pGameObject;
			m_Rendering = false;
			RELEASE_INSTANCE(CGameInstance);
			
		}
		else if (pGameObject->Get_Type() == (_uint)OBJECT_TYPE::OBJ_END || pGameObject->Get_Type() == (_uint)OBJECT_TYPE::CHEST
			|| pGameObject->Get_Type() == (_uint)OBJECT_TYPE::FLOOR || pGameObject->Get_Type() == (_uint)OBJECT_TYPE::OBJECT
			|| pGameObject->Get_Type() == (_uint)OBJECT_TYPE::SHIELD || pGameObject->Get_Type() == (_uint)OBJECT_TYPE::SHOPITEM
			|| pGameObject->Get_Type() == (_uint)OBJECT_TYPE::STATICOBJECT || pGameObject->Get_Type() == (_uint)OBJECT_TYPE::TELEPORT
			|| pGameObject->Get_Type() == (_uint)OBJECT_TYPE::WALL || pGameObject->Get_Type() == (_uint)OBJECT_TYPE::WELL)
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

			pGameInstance->Play(L"pl_itm_wep_techbow_impact_", MULTY, 16, 3, m_pTransformCom);
			m_bDead = true;
			RELEASE_INSTANCE(CGameInstance);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// 여기서 근처에 있는 몬스터들 정보를 수집한다 ( docs.microsoft.com/ko-kr/troubleshoot/developer/visualstudio/cpp/libraries/stl-priority-queue-class-custom-type )
	//------------------------------------------------------------------------------------------------------------------------------------------------------------------
	if (!m_bFirstCollision)
	{
		m_LifeTime += fTimeDelta;
		
		if (m_LifeTime < 0.f)
			m_bDead = true;

		m_pTransformCom->Go_Straight(fTimeDelta);
	}
	else if (m_bFirstCollision && m_bCollectMonster == false)
	{
		
		list<CGameObject*> MonsterList;
		ListInput(&MonsterList, TEXT("Layer_Wave"));
		ListInput(&MonsterList, TEXT("Layer_Monster"));
		ListInput(&MonsterList, TEXT("Layer_Boss"));

		const _uint	ChainMax = 3;
		vector<CGameObject*> NearObject;
		NearObject.resize(ChainMax);
		_Vector3 Position = m_FirstTouch->Get_Transform()->Get_State(STATE_POSITION);
		_float Distances[ChainMax];

		for (_uint i = 0; i < ChainMax; ++i)
			Distances[i] = 30.f;

		for (auto iter = MonsterList.begin(); iter != MonsterList.end(); ++iter)
		{
			//최초터치는 리스트에서 제거
			if ((*iter) == m_FirstTouch)
			{
				MonsterList.erase(iter);
				break;
			}
		}

		//거리를 구해서 가까운녀석의 데이터를 집어넣는다.
		for (auto& Object : MonsterList)
		{
			_Vector3 Distance = Position - (_Vector3)Object->Get_Transform()->Get_State(STATE_POSITION);

			//최대 Distance인 녀석을 구한다.
			_uint DistanceIndex = 0;
			if (Distances[0] < Distances[1])
			{
				++DistanceIndex;
			}
			if (Distances[1] < Distances[2])
			{
				++DistanceIndex;
			}

			//최대 Distance인 녀석보다 거리가 가까운녀석이 있다면 데이터를 교체한다.
			if (Distance.Length() < Distances[DistanceIndex])
			{
				Distances[DistanceIndex] = Distance.Length();
				NearObject[DistanceIndex] = Object;
			}
		}
		//벡터 깔끔하게 정리
		{
			auto& iter = NearObject.begin();
			vector<CGameObject*> shirink_vector;
			for (; iter != NearObject.end() && *iter != nullptr; ++iter)
			{
				shirink_vector.push_back(*iter);
			}
			NearObject.swap(shirink_vector);
		}
		//넣은녀석은 리스트에서 정리
		{
			for (auto& Object : NearObject)
			{
				for (auto iter = MonsterList.begin(); iter != MonsterList.end(); ++iter)
				{
					if ((*iter) == Object)
					{
						MonsterList.erase(iter);
						break;
					}
				}
			}
		}
		//맵에넣는다
		m_ChainMap.emplace(m_FirstTouch, NearObject);
		//세컨드벡터를 기준으로 그들의자식을 뽑아서 넣는다
		if (NearObject.size() != 0)
		{
			while (true)
			{
				//amb_base_thunder_near_01
				map<CGameObject*, vector<CGameObject*>> BackUpMap = NextSearch(&MonsterList, NearObject);

				NearObject.clear();
				for (auto& Pair : BackUpMap)
				{
					m_ChainMap.emplace(Pair);
					for (auto& Object : Pair.second)
					{
						NearObject.push_back(Object);
					}
				}

				if (NearObject.size() == 0)
					break;
			}
		}

		m_KeyObject.push_back(m_FirstTouch);
		m_bCollectMonster = true;
		m_pRigid->Detach_Shape();
		m_pRigid->Detach_Rigid();
	}
	else
	{
		m_AttackDelay += fTimeDelta;
		auto& iter = m_KeyObject.begin();
		for (; iter != m_KeyObject.end();)
		{
			if ((*iter)->Get_Dead())
			{
				iter = m_KeyObject.erase(iter);
			}
			else
				++iter;
		}
		if (m_AttackDelay > 0.6f)
		{
			_float Sound = 0.f;
			vector<CGameObject*> NextObject;
			m_AttackDelay = 0.f;
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			for (auto& GameObject : m_KeyObject)
			{
				Sound += 0.05f;
				CThunder::PUSHTHUNDERDATA Data;
				Data.Position = GameObject->Get_Transform()->Get_State(STATE_POSITION);
				Data.Damege = GameObject->Get_Damege();
				if (FAILED(pGameInstance->Add_GameObjectToLayer(L"Layer_Effect", L"Prototype_GameObject_Thunder", &Data)))
					MSG_BOX("야발!");

				vector<CGameObject*> SecondObjects = m_ChainMap.find(GameObject)->second;
				for (auto& SecondObject : SecondObjects)
				{
					NextObject.push_back(SecondObject);
				}
			}
			_uint EID = pGameInstance->Play(L"amb_base_thunder_near_", MULTY, 1, 7);
			pGameInstance->VolumeUp(MULTY, EID, Sound);
			m_KeyObject.clear();
			for (auto& GameObject : NextObject)
			{
				m_KeyObject.push_back(GameObject);
			}
			NextObject.clear();
			RELEASE_INSTANCE(CGameInstance);
		}
	}
	return _int();
}

_int CIceDaggerLighting::LateTick(_float fTimeDelta)
{

	if (m_bDead)
	{
		m_pRigid->Detach_Shape();
		m_pRigid->Detach_Rigid();
	}

	if (!m_Rendering)
		return _int();
	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this)))
		return 0;
	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_GLOW, this)))
		return 0;
	m_pRenderTransform->Set_State(STATE_POSITION, m_pTransformCom->Get_State(STATE_POSITION));
	return _int();
}

HRESULT CIceDaggerLighting::Render(_uint iRenderGroup)
{
	if (CRenderer::RENDER_NONLIGHT == iRenderGroup)
	{
		for (_uint i = 0; i < 5; ++i)
		{
			if (FAILED(SetUp_ConstantTable()))
				return E_FAIL;

			/* 장치에 월드변환 행렬을 저장한다. */
			_uint	iNumMaterials = m_pModelCom->Get_NumMaterials();

			for (_uint i = 0; i < iNumMaterials; ++i)
			{
				//m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", i, aiTextureType_DIFFUSE);

				m_pModelCom->Render(m_pShaderCom, i, 3);
			}
		}
	}

	else if (CRenderer::RENDER_GLOW == iRenderGroup)
	{
		for (_uint i = 0; i < 5; ++i)
		{
			if (FAILED(SetUp_ConstantTable()))
				return E_FAIL;

			/* 장치에 월드변환 행렬을 저장한다. */
			_uint	iNumMaterials = m_pModelCom->Get_NumMaterials();

			for (_uint i = 0; i < iNumMaterials; ++i)
			{
				//m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", i, aiTextureType_DIFFUSE);

				m_pModelCom->Render(m_pShaderCom, i, 5);
			}
		}
	}

	return S_OK;
}

void CIceDaggerLighting::ListInput(list<CGameObject*>* _pList, const _tchar* LayerName)
{

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	list<class CGameObject*>* pGameObjectList = pGameInstance->Get_ObjectList(LayerName);
	if (pGameObjectList != nullptr)
	{
		for (list<class CGameObject*>::iterator iter = pGameObjectList->begin(); iter != pGameObjectList->end(); ++iter)
		{
			_pList->push_back(*iter);
		}
	}
	RELEASE_INSTANCE(CGameInstance);
}

map<CGameObject*, vector<CGameObject*>> CIceDaggerLighting::NextSearch(list<CGameObject*>* MonsterList, vector<CGameObject*> SecondObject)
{
	map<CGameObject*, vector<CGameObject*>> BackupMap;

	vector <vector<CGameObject*>> NearObject;
	NearObject.resize(SecondObject.size());

	for (auto& Object : *MonsterList)
	{
		vector<_float> PairDistances;
		vector<_uint> Indices;
		//30보다 가까운녀석을 벡터에 담아둔다
		for (_uint i = 0; i < (_uint)SecondObject.size(); ++i)
		{
			_Vector3 Distance = SecondObject[i]->Get_Transform()->Get_State(STATE_POSITION) - Object->Get_Transform()->Get_State(STATE_POSITION);

			if (Distance.Length() < 30.f)
			{
				PairDistances.push_back(Distance.Length());
				Indices.push_back(i);
			}
		}

		//벡터에 아무것도 담기지 않았다면 리턴한다.
		if (Indices.size() == 0)
			continue;

		//가까운순서대로 정렬한다
		for (_uint i = 0; i < PairDistances.size() - 1; ++i)
		{
			for (_uint j = 0; j < PairDistances.size() - 1 - i; ++j)
			{
				if (PairDistances[j] > PairDistances[j + 1])
				{
					_float Temp = PairDistances[j];
					PairDistances[j] = PairDistances[j + 1];
					PairDistances[j + 1] = Temp;

					_uint IndexTemp = Indices[j];
					Indices[j] = Indices[j + 1];
					Indices[j + 1] = IndexTemp;
				}
			}
		}
		
		//정렬된녀석의 첫번째 인덱스의 벡터사이즈가 제일 작거나 같고, 그 사이즈가 3미만이라면 거기에 푸쉬백.
		_uint Size = 3;
		_uint InputIndex = 9999;
		for (_uint i = 0; i < (_uint)Indices.size(); ++i)
		{
			_uint NearObjectSize = (_uint)NearObject[Indices[i]].size();
			if (Size > NearObjectSize)
			{
				Size = NearObjectSize;
				InputIndex = Indices[i];
			}
		}
		//만약 아무것도 넣지못했다면 전부 다 차있다는뜻. 제일처음녀석의 세컨드벡터의 디스탄스들을 비교, 가장낮은녀석을 집어넣는다
		//->빠진녀석은? 문제가있다. 왜? 다른녀석기준으론 그녀석이 여태까지 뽑은애보다 가까울수가 있으니까.
		if (InputIndex == 9999)
		{
			CGameObject* pGameObject = nullptr;
			for (_uint j = 0; j < (_uint)Indices.size(); ++j)
			{
				_uint NearIndex = 9999;
				_float CheckDistance = PairDistances[j];
				for (_uint i = 0; i < (_uint)NearObject[Indices[j]].size(); ++i)
				{
					_Vector3 Distance = NearObject[Indices[j]][i]->Get_Transform()->Get_State(STATE_POSITION) - SecondObject[Indices[j]]->Get_Transform()->Get_State(STATE_POSITION);

					if (CheckDistance <= Distance.Length())
					{
						CheckDistance = Distance.Length();
						NearIndex = i;
					}
				}
				if (NearIndex == 9999)
					continue;
				pGameObject = NearObject[Indices[j]][NearIndex];
				NearObject[Indices[j]][NearIndex] = Object;
				break;
			}
			while (pGameObject)
			{
				CGameObject* Temp = nullptr;
				//변경이 일어났다면 그녀석을 기준으로 모든오브젝트가 돌아서 거리를 비교한다 
				for (_uint i = 0; i < (_uint)SecondObject.size(); ++i)
				{
					for (_uint j = 0; j < (_uint)NearObject[i].size(); ++j)
					{
						if (NearObject[i][j] == pGameObject)
							continue;

						//NearObject랑 SecondObject간의 거리가 pGameInstance랑 secondObject간의 거리보다 크다면 교체를 해야함
						_Vector3 Distance = NearObject[i][j]->Get_Transform()->Get_State(STATE_POSITION) - SecondObject[i]->Get_Transform()->Get_State(STATE_POSITION);
						_Vector3 Distance2 = pGameObject->Get_Transform()->Get_State(STATE_POSITION) - SecondObject[i]->Get_Transform()->Get_State(STATE_POSITION);
						if (Distance2.Length() < Distance.Length())
						{
							Temp = NearObject[i][j];
							NearObject[i][j] = pGameObject;
							pGameObject = Temp;
							break;
						}
					}
					if (Temp)
						break;
				}
				if (Temp == nullptr)
				{
					pGameObject = nullptr;
				}
			}
		}
		else
		{
			NearObject[InputIndex].push_back(Object);
		}
	}

	auto iter = MonsterList->begin();
	for (; iter != MonsterList->end();)
	{
		_bool Delete = false;
		for (_uint i = 0; i < NearObject.size(); ++i)
		{
			for (_uint j = 0; j < NearObject[i].size(); ++j)
			{
				if (*iter == NearObject[i][j])
				{
					iter = MonsterList->erase(iter);
					Delete = true;
					break;
				}
			}
			if (Delete)
				break;
		}
		if (Delete)
			continue;

		++iter;
	}

	for (_uint i = 0; i < NearObject.size(); ++i)
	{
		BackupMap.emplace(SecondObject[i], NearObject[i]);
	}

	return BackupMap;
}

HRESULT CIceDaggerLighting::SetUp_Component()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component_STATIC(TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component_STATIC(TEXT("Prototype_Component_Transform"), TEXT("Com_RenderTransform"), (CComponent**)&m_pRenderTransform)))
		return E_FAIL;

	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component_STATIC(TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Model_Lighting_IceDaggerLighting"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Shader*/
	if (FAILED(__super::Add_Component(TEXT("Prototype_Component_Shader_Mesh_Effect"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CIceDaggerLighting::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;
	m_pRenderTransform->Set_Scale(_Vector3(0.2f, 0.2f, 0.04f));
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_Matrix RotMat = _Matrix::CreateFromYawPitchRoll(XMConvertToRadians(pGameInstance->Range_Float(0, 360)), XMConvertToRadians(pGameInstance->Range_Float(0, 360)), XMConvertToRadians(pGameInstance->Range_Float(0, 360)));
	m_pRenderTransform->Set_Rotate(RotMat.Right(), RotMat.Up(), RotMat.Forward());
	m_pRenderTransform->Bind_OnShader(m_pShaderCom, "g_WorldMatrix");
	pGameInstance->Bind_Transform_OnShader(TS_VIEW, m_pShaderCom, "g_ViewMatrix");
	pGameInstance->Bind_Transform_OnShader(TS_PROJ, m_pShaderCom, "g_ProjMatrix");

	m_pShaderCom->Set_RawValue("g_vMtrlDiffuse", &_float4(1.f, 1.f, 1.f, 1.f), sizeof(_float4));
	m_pShaderCom->Set_RawValue("g_vGlowColor_Front", &_float4(0.f, 0.f, 0.2f, 1.f), sizeof(_float4));
	m_pShaderCom->Set_RawValue("g_vGlowColor_Back", &_float4(0.f, 0.f, 0.0f, 1.f), sizeof(_float4));

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}


CIceDaggerLighting* CIceDaggerLighting::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CIceDaggerLighting* pInstance = new CIceDaggerLighting(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CChest");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CIceDaggerLighting::Clone(void* pArg)
{
	CIceDaggerLighting* pInstance = new CIceDaggerLighting(*this);


	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CGrass");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CIceDaggerLighting::Free()
{
	__super::Free();
	// 	Safe_Release(m_pTransformCom);
	// 	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRigid);
	Safe_Release(m_pRenderTransform);

// 	if (!m_NearMonsterList.empty())
// 		m_NearMonsterList.clear();
}
