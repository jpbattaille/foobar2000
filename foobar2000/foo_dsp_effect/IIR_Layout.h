#pragma once

#include "afx.h"
#include "afxwin.h"
// IIR_Layout dialog

class IIR_Layout : public CDialogEx
{
	DECLARE_DYNAMIC(IIR_Layout)

public:
	IIR_Layout(CWnd* pParent = nullptr);   // standard constructor
	virtual ~IIR_Layout();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_IIR_LAYOUT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
