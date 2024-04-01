// UITool.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MFCTool.h"
#include "UITool.h"
#include "afxdialogex.h"


// CUITool 대화 상자입니다.

IMPLEMENT_DYNAMIC(CUITool, CDialog)

CUITool::CUITool(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_UITOOL, pParent)
{

}

CUITool::~CUITool()
{
}

void CUITool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListUIName);
	DDX_Control(pDX, IDC_LIST2, m_ListStaticUI);
	DDX_Control(pDX, IDC_LIST3, m_ListStaticUI);
	DDX_Control(pDX, IDC_LIST4, m_ListDynamicUI);

	DDX_Text(pDX, IDC_EDIT1, m_SUIData.SizeX);
	DDX_Text(pDX, IDC_EDIT12, m_SUIData.SizeY);
	DDX_Text(pDX, IDC_EDIT13, m_SUIData.Left);
	DDX_Text(pDX, IDC_EDIT14, m_SUIData.Top);
	DDX_Text(pDX, IDC_EDIT15, m_SUIData.Alpha);
	DDX_Text(pDX, IDC_EDIT16, m_SUIData.UICount);
	DDX_Text(pDX, IDC_EDIT22, m_SUIData.Rotate);
	DDX_Text(pDX, IDC_EDIT7, m_SUIData.Pass);

	DDX_Text(pDX, IDC_EDIT18, m_DUIData.SizeX);
	DDX_Text(pDX, IDC_EDIT19, m_DUIData.SizeY);
	DDX_Text(pDX, IDC_EDIT20, m_DUIData.Left);
	DDX_Text(pDX, IDC_EDIT21, m_DUIData.Top);
	DDX_Text(pDX, IDC_EDIT17, m_DUIData.UIIndex);
	DDX_Text(pDX, IDC_EDIT2, m_DUIData.WPIndex);
	DDX_Text(pDX, IDC_EDIT6, m_DUIData.UIClass);

	DDX_Control(pDX, IDC_RADIO2, m_ButtonDUIClass[0]);
	DDX_Control(pDX, IDC_RADIO3, m_ButtonDUIClass[1]);
	DDX_Control(pDX, IDC_RADIO4, m_ButtonDUIClass[2]);
	DDX_Control(pDX, IDC_RADIO5, m_ButtonDUIClass[3]);
	DDX_Control(pDX, IDC_RADIO6, m_ButtonDUIClass[4]);

	DDX_Control(pDX, IDC_CHECK1, m_ButtonWPIndexCheck);
}


BEGIN_MESSAGE_MAP(CUITool, CDialog)
	ON_BN_CLICKED(IDC_BUTTON18, &CUITool::OnBnClickedNameLoad)
	ON_BN_CLICKED(IDC_BUTTON7, &CUITool::OnBnClickedStaticDelete)
	ON_BN_CLICKED(IDC_BUTTON11, &CUITool::OnBnClickedStaticLoad)
	ON_BN_CLICKED(IDC_BUTTON1, &CUITool::OnBnClickedStaticClone)
	ON_BN_CLICKED(IDC_BUTTON6, &CUITool::OnBnClickedStaticModify)
	ON_BN_CLICKED(IDC_BUTTON8, &CUITool::OnBnClickedStaticSave)
	ON_BN_CLICKED(IDC_BUTTON17, &CUITool::OnBnClickedDynamicDelete)
	ON_BN_CLICKED(IDC_BUTTON13, &CUITool::OnBnClickedDynamicClone)
	ON_BN_CLICKED(IDC_BUTTON15, &CUITool::OnBnClickedDynamicSave)
	ON_BN_CLICKED(IDC_BUTTON14, &CUITool::OnBnClickedDynamicModify)
	ON_BN_CLICKED(IDC_BUTTON16, &CUITool::OnBnClickedDynamicLoad)
	ON_LBN_SELCHANGE(IDC_LIST2, &CUITool::OnLbnSelchangeStaticUIList1)
	ON_LBN_SELCHANGE(IDC_LIST3, &CUITool::OnLbnSelchangeStaticUIList2)
	ON_LBN_SELCHANGE(IDC_LIST4, &CUITool::OnLbnSelchangeDynamicUIList)

	ON_BN_CLICKED(IDC_RADIO2, &CUITool::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO3, &CUITool::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO4, &CUITool::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO5, &CUITool::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO6, &CUITool::OnBnClickedRadio2)
END_MESSAGE_MAP()


// CUITool 메시지 처리기입니다.


void CUITool::OnBnClickedNameLoad()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	m_ListUIName.ResetContent();
	
	{
		m_ListUIName.AddString(L"Texture_StaticWaponChoice");
		m_ListUIName.AddString(L"Texture_StaticWeapon");
		m_ListUIName.AddString(L"Texture_StaticHpBar");
		m_ListUIName.AddString(L"Texture_StaticBoosterBar");
		m_ListUIName.AddString(L"Texture_StaticMap");
		m_ListUIName.AddString(L"Texture_StaticSight_4");
		m_ListUIName.AddString(L"Texture_StaticSight_5");
		m_ListUIName.AddString(L"Texture_StaticSight_6");
		m_ListUIName.AddString(L"Texture_StaticSight_7");
		m_ListUIName.AddString(L"Texture_StaticSight_8");

	}
}

void CUITool::OnBnClickedStaticDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_uint Index = (_uint)m_ListStaticUI.GetCurSel();
	if (Index >= (_uint)m_ListStaticUI.GetCount())
		return;
	
	m_SUIData.SizeX = 0.f;
	m_SUIData.SizeY = 0.f;
	m_SUIData.Alpha = 1.f;
	m_SUIData.Left = 0.f;
	m_SUIData.Top = 0.f;
	m_SUIData.UICount = 0;
	m_SUIData.Rotate = 0.f;
	m_SUIData.Pass = 0;

	pGameInstance->Delete_Object(LEVEL_STATIC, SLayer, Index);
	m_ListStaticUI.DeleteString(Index);
	m_pSelectStaticUI = nullptr;


	RELEASE_INSTANCE(CGameInstance);

	UpdateData(FALSE);
}

void CUITool::OnBnClickedStaticClone()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	CString wstrFindName;
	int Index = m_ListUIName.GetCurSel();
	if (Index >= m_ListUIName.GetCount())
		return;
	m_ListUIName.GetText(Index, wstrFindName);

	m_SUIData.Texture_Prototype_Component = wstrFindName.GetString();

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, SLayer, TEXT("Prototype_StaticUI"), &m_SUIData)))
		return;
	m_ListStaticUI.AddString(m_SUIData.Texture_Prototype_Component.c_str());
	RELEASE_INSTANCE(CGameInstance);

	UpdateData(FALSE);
}

void CUITool::OnBnClickedStaticModify()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	if (m_pSelectStaticUI == nullptr)
		return;

	m_pSelectStaticUI->m_SUIData.SizeX = m_SUIData.SizeX;
	m_pSelectStaticUI->m_SUIData.SizeY = m_SUIData.SizeY;
	m_pSelectStaticUI->m_SUIData.Left = m_SUIData.Left;
	m_pSelectStaticUI->m_SUIData.Top = m_SUIData.Top;
	m_pSelectStaticUI->m_SUIData.UICount = m_SUIData.UICount;
	m_pSelectStaticUI->m_SUIData.Alpha = m_SUIData.Alpha;
	m_pSelectStaticUI->m_SUIData.Rotate = m_SUIData.Rotate;
	m_pSelectStaticUI->m_SUIData.Pass = m_SUIData.Pass;
}

void CUITool::OnBnClickedStaticSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFileDialog Dlg(FALSE, L"dat", L"*.dat", OFN_OVERWRITEPROMPT);
	TCHAR szFilePath[MAX_PATH]{};
	GetCurrentDirectory(MAX_PATH, szFilePath);//절대경로뽑기
	PathRemoveFileSpec(szFilePath);//뒤에하나지우기
	lstrcat(szFilePath, L"\\Client\\Bin\\Data");//Data폴더라고 설정한거
	Dlg.m_ofn.lpstrInitialDir = szFilePath; //디폴트를 Data폴더로 설정
	if (IDOK == Dlg.DoModal())
	{
		CString wstrFilePath = Dlg.GetPathName(); //켜져서 내가 누르고 열기한녀석 경로 반환
		HANDLE hFile = CreateFile(wstrFilePath.GetString(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

		if (INVALID_HANDLE_VALUE == hFile)
			return;

		DWORD dwByte = 0;
		DWORD dwStrByte = 0;
		_uint Index = 0;
		CGameInstance* pGamseInstance = GET_INSTANCE(CGameInstance);

		while (true)
		{
			CStaticUI* Object = (CStaticUI*)pGamseInstance->Get_GameObjectPtr(LEVEL_STATIC, SLayer, Index++);
			if (Object == nullptr)
				break;

			dwStrByte = DWORD(sizeof(wchar_t) * (Object->m_SUIData.Texture_Prototype_Component.size() + 1));
			WriteFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
			WriteFile(hFile, Object->m_SUIData.Texture_Prototype_Component.c_str(), dwStrByte, &dwByte, nullptr);

			WriteFile(hFile, &Object->m_SUIData.SizeX, sizeof(_float), &dwByte, nullptr);
			WriteFile(hFile, &Object->m_SUIData.SizeY, sizeof(_float), &dwByte, nullptr);
			WriteFile(hFile, &Object->m_SUIData.Left, sizeof(_float), &dwByte, nullptr);
			WriteFile(hFile, &Object->m_SUIData.Top, sizeof(_float), &dwByte, nullptr);
			WriteFile(hFile, &Object->m_SUIData.Alpha, sizeof(_float), &dwByte, nullptr);
			WriteFile(hFile, &Object->m_SUIData.UICount, sizeof(_uint), &dwByte, nullptr);
			WriteFile(hFile, &Object->m_SUIData.Rotate, sizeof(_float), &dwByte, nullptr);
			WriteFile(hFile, &Object->m_SUIData.Pass, sizeof(_uint), &dwByte, nullptr);
		}
		CloseHandle(hFile);
		RELEASE_INSTANCE(CGameInstance);
	}
}

void CUITool::OnBnClickedStaticLoad()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFileDialog Dlg(TRUE, L"dat", L"*.dat", OFN_OVERWRITEPROMPT);
	TCHAR szFilePath[MAX_PATH]{};
	GetCurrentDirectory(MAX_PATH, szFilePath);
	PathRemoveFileSpec(szFilePath);
	lstrcat(szFilePath, L"\\Client\\Bin\\Data");
	Dlg.m_ofn.lpstrInitialDir = szFilePath;
	if (IDOK == Dlg.DoModal())
	{
		CString wstrFilePath = Dlg.GetPathName();
		HANDLE hFile = CreateFile(wstrFilePath.GetString(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

		if (INVALID_HANDLE_VALUE == hFile)
			return;

		DWORD dwByte = 0;
		DWORD dwStrByte = 0;
		wchar_t* pBuf = nullptr;
		SUIDATA Info;
		{
			m_ListStaticUI.ResetContent();
			m_SUIData.SizeX = 0.f;
			m_SUIData.SizeY = 0.f;
			m_SUIData.Left = 0.f;
			m_SUIData.Top = 0.f;
			m_SUIData.Alpha = 1.f;
			m_SUIData.UICount = 0;
			m_SUIData.Rotate = 0.f;
		}
		CGameInstance* pGamseInstance = GET_INSTANCE(CGameInstance);
		const _tchar* Prototype_Tag = TEXT("Prototype_StaticUI");
		while (true)
		{	
			//Texture_Prototype_Component
			{
				ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
				if (0 == dwByte)
					break;

				pBuf = new wchar_t[dwStrByte];
				ReadFile(hFile, pBuf, dwStrByte, &dwByte, nullptr);
				Info.Texture_Prototype_Component = pBuf;
				if (pBuf)
				{
					delete[] pBuf;
					pBuf = nullptr;
				}
			}
			ReadFile(hFile, &Info.SizeX, sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &Info.SizeY, sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &Info.Left, sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &Info.Top, sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &Info.Alpha, sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &Info.UICount, sizeof(_uint), &dwByte, nullptr);
			ReadFile(hFile, &Info.Rotate, sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &Info.Pass, sizeof(_uint), &dwByte, nullptr);

			pGamseInstance->Add_GameObjectToLayer(LEVEL_STATIC, SLayer, Prototype_Tag, &Info);
			m_ListStaticUI.AddString(Info.Texture_Prototype_Component.c_str());
		}
		CloseHandle(hFile);
		RELEASE_INSTANCE(CGameInstance);
	}
	UpdateData(FALSE);
}

void CUITool::OnBnClickedDynamicDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_uint Index = (_uint)m_ListStaticUI.GetCurSel();
	if (Index >= (_uint)m_ListStaticUI.GetCount())
		return;

	m_DUIData.SizeX = 0.f;
	m_DUIData.SizeY = 0.f;
	m_DUIData.Left = 0.f;
	m_DUIData.Top = 0.f;
	m_DUIData.UIIndex = 99;
	m_DUIData.UIClass = 99;
	m_DUIData.WPIndex = 99;

	for (int i = 0; i < 5; ++i)
	{
		m_ButtonDUIClass[i].SetCheck(FALSE);
	}
	m_ButtonWPIndexCheck.SetCheck(FALSE);

	pGameInstance->Delete_Object(LEVEL_STATIC, DLayer, Index);
	m_ListDynamicUI.DeleteString(Index);
	m_pSelectDynamicUI = nullptr;


	RELEASE_INSTANCE(CGameInstance);

	UpdateData(FALSE);
}

void CUITool::OnBnClickedDynamicClone()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	for (_uint i = 0; i < 5; ++i)
	{
		if (m_ButtonDUIClass[i].GetCheck())
			m_DUIData.UIClass = i;
	}
	if (m_DUIData.UIClass == 99)
		return;


	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, DLayer, TEXT("Prototype_DynamicUI"), &m_DUIData)))
		return;
	m_ListDynamicUI.AddString(TEXT("DynamicUI"));
	RELEASE_INSTANCE(CGameInstance);

	UpdateData(FALSE);
}

void CUITool::OnBnClickedDynamicModify()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	if (m_pSelectStaticUI == nullptr)
		return;

	m_pSelectDynamicUI->m_DUIData.SizeX = m_DUIData.SizeX;
	m_pSelectDynamicUI->m_DUIData.SizeY = m_DUIData.SizeY;
	m_pSelectDynamicUI->m_DUIData.Left = m_DUIData.Left;
	m_pSelectDynamicUI->m_DUIData.Top = m_DUIData.Top;
	m_pSelectDynamicUI->m_DUIData.UIIndex = m_DUIData.UIIndex;
	m_pSelectDynamicUI->m_DUIData.WPIndex = m_DUIData.WPIndex;

	for (_uint i = 0; i < 5; ++i)
	{
		if (m_ButtonDUIClass[i].GetCheck())
			m_pSelectDynamicUI->m_DUIData.UIClass = i;
	}
}

void CUITool::OnBnClickedDynamicSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFileDialog Dlg(FALSE, L"dat", L"*.dat", OFN_OVERWRITEPROMPT);
	TCHAR szFilePath[MAX_PATH]{};
	GetCurrentDirectory(MAX_PATH, szFilePath);//절대경로뽑기
	PathRemoveFileSpec(szFilePath);//뒤에하나지우기
	lstrcat(szFilePath, L"\\Client\\Bin\\Data");//Data폴더라고 설정한거
	Dlg.m_ofn.lpstrInitialDir = szFilePath; //디폴트를 Data폴더로 설정
	if (IDOK == Dlg.DoModal())
	{
		CString wstrFilePath = Dlg.GetPathName(); //켜져서 내가 누르고 열기한녀석 경로 반환
		HANDLE hFile = CreateFile(wstrFilePath.GetString(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

		if (INVALID_HANDLE_VALUE == hFile)
			return;

		DWORD dwByte = 0;
		DWORD dwStrByte = 0;
		_uint Index = 0;
		CGameInstance* pGamseInstance = GET_INSTANCE(CGameInstance);

		while (true)
		{
			CDynamicUI* Object = (CDynamicUI*)pGamseInstance->Get_GameObjectPtr(LEVEL_STATIC, DLayer, Index++);
			if (Object == nullptr)
				break;

			WriteFile(hFile, &Object->m_DUIData.SizeX, sizeof(_float), &dwByte, nullptr);
			WriteFile(hFile, &Object->m_DUIData.SizeY, sizeof(_float), &dwByte, nullptr);
			WriteFile(hFile, &Object->m_DUIData.Left, sizeof(_float), &dwByte, nullptr);
			WriteFile(hFile, &Object->m_DUIData.Top, sizeof(_float), &dwByte, nullptr);
			WriteFile(hFile, &Object->m_DUIData.UIClass, sizeof(_uint), &dwByte, nullptr);
			WriteFile(hFile, &Object->m_DUIData.UIIndex, sizeof(_uint), &dwByte, nullptr);
			WriteFile(hFile, &Object->m_DUIData.WPIndex, sizeof(_uint), &dwByte, nullptr);
		}
		CloseHandle(hFile);
		RELEASE_INSTANCE(CGameInstance);
	}
}

void CUITool::OnBnClickedDynamicLoad()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFileDialog Dlg(TRUE, L"dat", L"*.dat", OFN_OVERWRITEPROMPT);
	TCHAR szFilePath[MAX_PATH]{};
	GetCurrentDirectory(MAX_PATH, szFilePath);
	PathRemoveFileSpec(szFilePath);
	lstrcat(szFilePath, L"\\Client\\Bin\\Data");
	Dlg.m_ofn.lpstrInitialDir = szFilePath;
	if (IDOK == Dlg.DoModal())
	{
		CString wstrFilePath = Dlg.GetPathName();
		HANDLE hFile = CreateFile(wstrFilePath.GetString(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

		if (INVALID_HANDLE_VALUE == hFile)
			return;

		DWORD dwByte = 0;
		DUIDATA Info;
		{
			m_ListDynamicUI.ResetContent();
			m_DUIData.SizeX = 0.f;
			m_DUIData.SizeY = 0.f;
			m_DUIData.Left = 0.f;
			m_DUIData.Top = 0.f;
			m_DUIData.UIIndex = 99;
			m_DUIData.UIClass = 99;
			m_DUIData.WPIndex = 99;
			for (int i = 0; i < 5; ++i)
			{
				m_ButtonDUIClass[i].SetCheck(FALSE);
			}
			m_ButtonWPIndexCheck.SetCheck(FALSE);
		}
		CGameInstance* pGamseInstance = GET_INSTANCE(CGameInstance);
		const _tchar* Prototype_Tag = TEXT("Prototype_DynamicUI");
		while (true)
		{
			ReadFile(hFile, &Info.SizeX, sizeof(_float), &dwByte, nullptr);
			if (dwByte == 0)
				break;
			ReadFile(hFile, &Info.SizeY, sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &Info.Left, sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &Info.Top, sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &Info.UIClass, sizeof(_uint), &dwByte, nullptr);
			ReadFile(hFile, &Info.UIIndex, sizeof(_uint), &dwByte, nullptr);
			ReadFile(hFile, &Info.WPIndex, sizeof(_uint), &dwByte, nullptr);

			pGamseInstance->Add_GameObjectToLayer(LEVEL_STATIC, DLayer, Prototype_Tag, &Info);
			m_ListDynamicUI.AddString(TEXT("DynamicUI"));
			//이거 다이인덱스를 스트링화시키는거 찾아서 넣으면될듯
		}
		CloseHandle(hFile);
		RELEASE_INSTANCE(CGameInstance);
	}
	UpdateData(FALSE);
}

void CUITool::OnLbnSelchangeStaticUIList1()
{
	UpdateData(TRUE);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	_uint Index = (_uint)m_ListStaticUI.GetCurSel();
	if (Index >= (_uint)m_ListStaticUI.GetCount())
		return;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	m_pSelectStaticUI = (CStaticUI*)pGameInstance->Get_GameObjectPtr(LEVEL_STATIC, SLayer, Index);
	if (m_pSelectStaticUI == nullptr)
		return;

	m_SUIData = m_pSelectStaticUI->m_SUIData;

	m_DUIData.UIIndex = Index;

	RELEASE_INSTANCE(CGameInstance);

	UpdateData(FALSE);
}

void CUITool::OnLbnSelchangeStaticUIList2()
{
	UpdateData(TRUE);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	_uint Index = (_uint)m_ListStaticUI.GetCurSel();
	if (Index >= (_uint)m_ListStaticUI.GetCount())
		return;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	m_pSelectStaticUI = (CStaticUI*)pGameInstance->Get_GameObjectPtr(LEVEL_STATIC, SLayer, Index);
	if (m_pSelectStaticUI == nullptr)
		return;

	m_SUIData = m_pSelectStaticUI->m_SUIData;

	m_DUIData.UIIndex = Index;

	RELEASE_INSTANCE(CGameInstance);

	UpdateData(FALSE);
}

void CUITool::OnLbnSelchangeDynamicUIList()
{
	UpdateData(TRUE);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	_uint Index = (_uint)m_ListDynamicUI.GetCurSel();
	if (Index >= (_uint)m_ListDynamicUI.GetCount())
		return;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	m_pSelectDynamicUI = (CDynamicUI*)pGameInstance->Get_GameObjectPtr(LEVEL_STATIC, DLayer, Index);
	if (m_pSelectDynamicUI == nullptr)
		return;

	m_DUIData = m_pSelectDynamicUI->m_DUIData;

	for (int i = 0; i < 5; ++i)
	{
		m_ButtonDUIClass[i].SetCheck(FALSE);
	}

	if(m_DUIData.UIClass < 5)
		m_ButtonDUIClass[m_DUIData.UIClass].SetCheck(TRUE);

	if (m_DUIData.UIClass >= 2)
		m_ButtonWPIndexCheck.SetCheck(TRUE);
	else
		m_ButtonWPIndexCheck.SetCheck(FALSE);

	RELEASE_INSTANCE(CGameInstance);

	UpdateData(FALSE);
}

void CUITool::OnBnClickedRadio1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_ButtonWPIndexCheck.SetCheck(FALSE);

	UpdateData(FALSE);
}

void CUITool::OnBnClickedRadio2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_ButtonWPIndexCheck.SetCheck(TRUE);

	UpdateData(FALSE);
}
