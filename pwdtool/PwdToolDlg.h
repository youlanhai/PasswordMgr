
// PwdToolDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "../PwdLib/PwdMgr.h"


// CPwdToolDlg dialog
class CPwdToolDlg : public CDialogEx
{
// Construction
public:
	CPwdToolDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_PWDTOOL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON       m_hIcon;
    int         m_editMode;
    pwd::PwdMgr m_pwdMgr;

    void SetEditMode(int mode, bool force = false);
    void ViewPwd(const pwd::Pwd & pwd, bool updateView);
    void CollectPwd(pwd::Pwd & pwd, bool updateView);

    int FindListItem(pwd::pwdid pid) const;
    void AddListItem(const pwd::Pwd & pwd);
    void InsertListItem(int index, const pwd::Pwd & pwd);
    void UpdateListItem(int index, const pwd::Pwd & pwd);

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    UINT m_pwdId;
    CString m_pwdName;
    CString m_pwdPwd;
    CString m_pwdKeyword;
    CString m_pwdDescribe;
    CString m_strSearch;
    CComboBox m_cobSearchIndex;
    CListCtrl m_pwdList;

    afx_msg void OnClickedBtnCancel();
    afx_msg void OnClickedBtnEdit();
    afx_msg void OnClickedBtnNew();
    afx_msg void OnClickedBtnSave();
    afx_msg void OnClickedBtnSearch();
    afx_msg void OnNMClickList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBnClickedBtnDelete();

    bool LoadPwdFile();
};
