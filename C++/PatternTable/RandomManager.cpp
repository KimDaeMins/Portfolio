#include "..\Public\RandomManager.h"

IMPLEMENT_SINGLETON(CRandomManager)

CRandomManager::CRandomManager()
{
}

VOID CRandomManager::Initialize()
{
	gen = mt19937(rd());
	return VOID();
}

__int32 CRandomManager::Range(__int32 min, __int32 max)
{
	uniform_int_distribution<__int32> dis = uniform_int_distribution<__int32>(min, max - 1);
	return dis(gen);
}

float CRandomManager::Range_Float(__int32 min, __int32 max)
{
	return (float)Range(min, max) + Get_Value();
}

int CRandomManager::GetPriority(vector<int>& vecInt)
{
	int sum = 0;
	for (int i = 0; i < vecInt.size(); ++i)
	{
		sum += vecInt[i];
	}

	if (sum <= 0)
		return 0;

	int num = Range(1, sum);

	sum = 0;
	for (int i = 0; i < vecInt.size(); ++i)
	{
		int start = sum;
		sum += vecInt[i];
		if (start < num && num <= sum)
		{
			return i;
		}
	}

	return 0;
}

float CRandomManager::Get_Value()
{
	return (float)Range(0, INT_MAX) / INT_MAX;
}

_bool CRandomManager::Get_Random(_float Percent)
{
	if (Range_Float(0, 100) > Percent)
		return false;

	return true;
}

void CRandomManager::Free()
{

}
