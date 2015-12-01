
// PwdToolDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PwdTool.h"
#include "PwdToolDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const std::wstring UserDataFile = L"pwd/pwd.dat";

namespace SearchType
{
    const int Keyword = 0;
    const int Id = 1;
    const int Name = 2;
    const int Describe = 3;
    const int Password = 4;
}

namespace EditMode
{
    const int None = -1;
    const int Read = 0;
    const int New = 1;
    const int Edit = 2;
}

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CPwdToolDlg dialog




CPwdToolDlg::CPwdToolDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPwdToolDlg::IDD, pParent)
    , m_pwdId(0)
    , m_pwdName(_T(""))
    , m_pwdPwd(_T(""))
    , m_pwdKeyword(_T(""))
    , m_pwdDescribe(_T(""))
    , m_strSearch(_T(""))
    , m_editMode(EditMode::None)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPwdToolDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDT_ID, m_pwdId);
    DDX_Text(pDX, IDC_EDT_NAME, m_pwdName);
    DDX_Text(pDX, IDC_EDT_PWD, m_pwdPwd);
    DDX_Text(pDX, IDC_EDT_KEYWORD, m_pwdKeyword);
    DDX_Text(pDX, IDC_EDT_DESCRIBE, m_pwdDescribe);
    DDX_Text(pDX, IDC_EDT_SEARCH, m_strSearch);
    DDX_Control(pDX, IDC_COMBO1, m_cobSearchIndex);
    DDX_Control(pDX, IDC_LIST, m_pwdList);
}

BEGIN_MESSAGE_MAP(CPwdToolDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BTN_CANCEL, &CPwdToolDlg::OnClickedBtnCancel)
    ON_BN_CLICKED(IDC_BTN_EDIT, &CPwdToolDlg::OnClickedBtnEdit)
    ON_BN_CLICKED(IDC_BTN_NEW, &CPwdToolDlg::OnClickedBtnNew)
    ON_BN_CLICKED(IDC_BTN_SAVE, &CPwdToolDlg::OnClickedBtnSave)
    ON_BN_CLICKED(IDC_BTN_SEARCH, &CPwdToolDlg::OnClickedBtnSearch)
    ON_NOTIFY(NM_CLICK, IDC_LIST, &CPwdToolDlg::OnNMClickList)
    ON_BN_CLICKED(IDC_BTN_DELETE, &CPwdToolDlg::OnBnClickedBtnDelete)
END_MESSAGE_MAP()


// CPwdToolDlg message handlers

BOOL CPwdToolDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
    
    m_cobSearchIndex.AddString(L"keyword");
    m_cobSearchIndex.AddString(L"id");
    m_cobSearchIndex.AddString(L"name");
    m_cobSearchIndex.AddString(L"describe");
    m_cobSearchIndex.AddString(L"password");
    m_cobSearchIndex.SetCurSel(SearchType::Keyword);

    m_pwdList.InsertColumn(0, L"id", 0, 50);
    m_pwdList.InsertColumn(1, L"keyword", 0, 100);
    m_pwdList.InsertColumn(2, L"username", 0, 100);

    DWORD dwStyle = m_pwdList.GetExtendedStyle(); //获取当前扩展样式
    dwStyle |= LVS_EX_FULLROWSELECT; //选中某行使整行高亮（report风格时）
    dwStyle |= LVS_EX_GRIDLINES; //网格线（report风格时）
    //dwStyle |= LVS_EX_CHECKBOXES; //item前生成checkbox控件
    m_pwdList.SetExtendedStyle(dwStyle); //设置扩展风格

    SetEditMode(EditMode::Read, true);

    OnClickedBtnSearch();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPwdToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPwdToolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPwdToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

bool CPwdToolDlg::LoadPwdFile()
{
    if(!m_pwdMgr.load(UserDataFile))
    {
        MessageBox(L"Open password data failed!", L"ERROR", MB_ICONSTOP);

        return FALSE;
    }

    return TRUE;
}


void CPwdToolDlg::OnClickedBtnCancel()
{
    // TODO: Add your control notification handler code here
    UpdateData(TRUE);

    pwd::Pwd info;
    if(m_pwdMgr.exist(m_pwdId))
    {
        info = m_pwdMgr.get(m_pwdId);
    }

    ViewPwd(info, true);
    SetEditMode(EditMode::Read);
}


void CPwdToolDlg::OnClickedBtnEdit()
{
    // TODO: Add your control notification handler code here
    SetEditMode(EditMode::Edit);
}


void CPwdToolDlg::OnClickedBtnNew()
{
    // TODO: Add your control notification handler code here
    pwd::Pwd info;
    ViewPwd(info, true);

    SetEditMode(EditMode::New);
}


void CPwdToolDlg::OnClickedBtnSave()
{
    // TODO: Add your control notification handler code here
    pwd::Pwd info;
    CollectPwd(info, true);

    if(m_editMode == EditMode::New)
    {
        if(m_pwdName.IsEmpty() || m_pwdPwd.IsEmpty() || m_pwdKeyword.IsEmpty())
        {
            MessageBox(L"username or password or keyword must not be null!",
                L"ERROR", MB_ICONSTOP);
            return;
        }

        //add item to pwdmgr
        info.id_ = m_pwdMgr.add(info);

        //add item to list control
        CString strText;
        strText.Format(L"%d", info.id_);

        int iItem = m_pwdList.GetItemCount();
        iItem = m_pwdList.InsertItem(iItem, strText);
        m_pwdList.SetItemText(iItem, 1, info.keyword_.c_str());
        m_pwdList.SetItemText(iItem, 2, info.name_.c_str());

        //refresh view
        ViewPwd(info, true);
    }
    else if(m_editMode == EditMode::Edit)
    {
        m_pwdMgr.modify(info.id_, info);

        int nIndex = FindListItem(info.id_);
        if(nIndex != -1)
			UpdateListItem(nIndex, info);
    }
    else
    {
        return;
    }

    SetEditMode(EditMode::Read);

    //save data to file
    if(!m_pwdMgr.save(UserDataFile))
    {
        MessageBox(L"Failed save data to file!", L"ERROR", MB_ICONSTOP);
    }
}

int CPwdToolDlg::FindListItem(pwd::pwdid pid) const
{
    CString strInfo;
    strInfo.Format(L"%u", pid);

    LVFINDINFO finfo;
	finfo.flags = LVFI_STRING;
    finfo.psz = strInfo.GetBuffer();

    return m_pwdList.FindItem(&finfo);
}

void CPwdToolDlg::AddListItem(const pwd::Pwd & pwd)
{
    int iItem = m_pwdList.GetItemCount();
    InsertListItem(iItem, pwd);
}

void CPwdToolDlg::InsertListItem(int index, const pwd::Pwd & pwd)
{
    CString strText;
    strText.Format(L"%d", pwd.id_);

    index = m_pwdList.InsertItem(index, strText);
    UpdateListItem(index, pwd);
}

void CPwdToolDlg::UpdateListItem(int index, const pwd::Pwd & pwd)
{
    m_pwdList.SetItemText(index, 1, pwd.keyword_.c_str());
    m_pwdList.SetItemText(index, 2, pwd.name_.c_str());
}


void CPwdToolDlg::OnClickedBtnSearch()
{
    // TODO: Add your control notification handler code here

    UpdateData(TRUE);

    m_pwdList.DeleteAllItems();

    if(m_strSearch.IsEmpty())//search all
    {
        CString strText;

        pwd::PwdMap::const_iterator it = m_pwdMgr.begin();
        for(; it != m_pwdMgr.end(); ++it)
        {
            AddListItem(it->second);
        }
    }
    else
    {
        pwd::IdVector ids;
        
        int searchType = m_cobSearchIndex.GetCurSel();
        switch(searchType)
        {
        case SearchType::Keyword:
            m_pwdMgr.searchByKeyword(ids, m_strSearch.GetBuffer());
            break;

        case SearchType::Name:
            m_pwdMgr.searchByName(ids, m_strSearch.GetBuffer());
            break;

        case SearchType::Describe:
            m_pwdMgr.searchByDesc(ids, m_strSearch.GetBuffer());
            break;

        case SearchType::Password:
            m_pwdMgr.searchByPwd(ids, m_strSearch.GetBuffer());
            break;

        case SearchType::Id:
            {
                pwd::pwdid pid = _wtoi(m_strSearch.GetBuffer());
                if(m_pwdMgr.exist(pid))
                    ids.push_back(pid);
                break;
            }

        default:
            break;
        }
        
        for(pwd::IdVector::iterator it = ids.begin();
            it != ids.end(); ++it)
        {
            try
            {
                const pwd::Pwd & info = m_pwdMgr.get(*it);
                AddListItem(info);
            }
            catch(std::exception &)
            {
            }
        }
    }

    UpdateData(FALSE);
}


void CPwdToolDlg::OnNMClickList(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: Add your control notification handler code here
    *pResult = 0;

    if(pNMItemActivate->iItem < 0)
        return;

    CString text = m_pwdList.GetItemText(pNMItemActivate->iItem, 0);
    pwd::pwdid id = _wtoi(text.GetBuffer());
    try
    {
        pwd::Pwd pwd = m_pwdMgr.get(id);
        ViewPwd(pwd, true);

        SetEditMode(EditMode::Read);
    }
    catch(std::exception & e)
    {
        CString msg(e.what());
        MessageBox(msg, L"ERROR", MB_ICONWARNING);
    }
}

void CPwdToolDlg::SetEditMode(int mode, bool force)
{
    if(mode == m_editMode) return;
    m_editMode = mode;

    GetDlgItem(IDC_BTN_NEW)->EnableWindow(
        m_editMode == EditMode::Read);

    GetDlgItem(IDC_BTN_DELETE)->EnableWindow(
        m_editMode == EditMode::Read);

    GetDlgItem(IDC_BTN_EDIT)->EnableWindow(
        m_editMode == EditMode::Read);

    GetDlgItem(IDC_BTN_CANCEL)->EnableWindow(
        m_editMode == EditMode::New || m_editMode == EditMode::Edit);

    GetDlgItem(IDC_BTN_SAVE)->EnableWindow(
        m_editMode == EditMode::New || m_editMode == EditMode::Edit);

    bool readOnly = m_editMode != EditMode::New && m_editMode != EditMode::Edit;
    
    ((CEdit*)GetDlgItem(IDC_EDT_NAME))->SetReadOnly(readOnly);
    ((CEdit*)GetDlgItem(IDC_EDT_PWD))->SetReadOnly(readOnly);
    ((CEdit*)GetDlgItem(IDC_EDT_KEYWORD))->SetReadOnly(readOnly);
    ((CEdit*)GetDlgItem(IDC_EDT_DESCRIBE))->SetReadOnly(readOnly);
}

void CPwdToolDlg::ViewPwd(const pwd::Pwd & pwd, bool updateView)
{
    m_pwdId = pwd.id_;
    m_pwdName = pwd.name_.c_str();
    m_pwdPwd = pwd.pwd_.c_str();
    m_pwdKeyword = pwd.keyword_.c_str();
    m_pwdDescribe = pwd.desc_.c_str();

    if(updateView) UpdateData(FALSE);
}

void CPwdToolDlg::CollectPwd(pwd::Pwd & pwd, bool updateView)
{
    if(updateView) UpdateData(TRUE);

    pwd.id_ = m_pwdId;
    pwd.name_ = m_pwdName.GetBuffer();
    pwd.pwd_ = m_pwdPwd.GetBuffer();
    pwd.keyword_ = m_pwdKeyword.GetBuffer();
    pwd.desc_ = m_pwdDescribe.GetBuffer();
}

void CPwdToolDlg::OnBnClickedBtnDelete()
{
    // TODO: Add your control notification handler code here

    UpdateData(TRUE);
    if(!m_pwdMgr.exist(m_pwdId))
    {
        MessageBox(L"the password info doesn't exist!", L"ERROR", MB_ICONSTOP);
        return;
    }

    CString strInfo;
    strInfo.Format(L"ensure to delete %u-(%s)?", m_pwdId, m_pwdName.GetBuffer());
    if(IDOK == MessageBox(strInfo, L"INFO", MB_OKCANCEL))
    {
        //delete item from pwdmgr
        m_pwdMgr.del(m_pwdId);
        
        //delete item from list control
        int nIndex = FindListItem(m_pwdId);
        if(nIndex != -1)
			m_pwdList.DeleteItem(nIndex);

        //save to file
        if(!m_pwdMgr.save(UserDataFile))
        {
            MessageBox(L"Failed save data to file!", L"ERROR", MB_ICONSTOP);
        }

        //clear view
        ViewPwd(pwd::Pwd(), true);
    }
}
