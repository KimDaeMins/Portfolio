#include "..\public\PhysicsLayer.h"

void CPhysicsLayer::Create_FilterLayer(const _uint& FilterSize)
{
	if (m_FilterLayer.size() != 0)
		return;
	m_FilterLayer.resize(FilterSize);
}

void CPhysicsLayer::Delete_FilterLayer()
{
	m_FilterLayer.clear();
}

void CPhysicsLayer::Delete_FilterGroup(const _uint& FilterGroup, const _uint& FilterGroups)
{
	PxU32 i = FilterGroup;
	PxU32 Index = 0;
	while (i >>= 1)
	{
		++Index;
	}
	if (Index >= m_FilterLayer.size())
		return;
	PxU32 Input = FilterGroups;
	m_FilterLayer[Index] ^= m_FilterLayer[Index] & Input;
}

void CPhysicsLayer::Add_FilterGroup(const _uint& FilterGroup, const _uint& FilterGroups)
{
	PxU32 i = FilterGroup;
	PxU32 Index = 0;
	while (i >>= 1)
	{
		++Index;
	}
	if (Index >= m_FilterLayer.size())
		return;
	PxU32 Input = FilterGroups;
	m_FilterLayer[Index] |= Input;
}

void CPhysicsLayer::Set_FilterGroups(const _uint& FilterGroup, const _uint& FilterGroups)
{
	PxU32 i = FilterGroup;
	PxU32 Index = 0;
	while (i >>= 1)
	{
		++Index;
	}
	if (Index >= m_FilterLayer.size())
		return;
	PxU32 Input = FilterGroups;
	m_FilterLayer[Index] = Input;
}

PxU32 CPhysicsLayer::Get_FilterGroups(const _uint& FilterGroup)
{
	PxU32 i = FilterGroup;
	PxU32 Index = 0;
	while (i >>= 1)
	{
		++Index;
	}

	return m_FilterLayer[Index];
}

void CPhysicsLayer::Free()
{
	Delete_FilterLayer();
}

CPhysicsLayer* CPhysicsLayer::Create()
{
	CPhysicsLayer* p = new CPhysicsLayer();

	return p;
}
