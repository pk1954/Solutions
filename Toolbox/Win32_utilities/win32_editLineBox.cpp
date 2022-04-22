// win32_refreshRate.cpp
//
// Win32_utilities

#include "stdafx.h"
#include "win32_util.h"
#include "win32_stdDialogBox.h"
#include "win32_util_resource.h"
#include "win32_editLineBox.h"

using std::wstring;
using std::to_wstring;
using std::bit_cast;

EditLineBox::EditLineBox
(
	wstring       & wstrValue,
	wstring const & wstrTitle,
	wstring const & wstrUnit
)
  : m_wstrValue(wstrValue),
    m_wstrUnit (wstrUnit),
    m_wstrTitle(wstrTitle)
{}

bool EditLineBox::Show(HWND const hwndParent)
{
	return StdDialogBox::Show(hwndParent, IDD_STD_EDIT_DIALOG);
}

bool EditLineBox::OnOK(HWND const hDlg)
{
	HWND hwndEditCtl { GetDlgItem(hDlg, IDD_EDIT_CTL) };
	bool bOK { Util::Evaluate(hwndEditCtl, m_wstrValue) };
	if (bOK)
		EndDialog(hDlg, IDOK);
	else 
		SetFocus(hwndEditCtl);
	return bOK;
}

void EditLineBox::OnInitDlg(HWND const hDlg, WPARAM const wParam, LPARAM const lParam)
{
	::SetWindowText(hDlg, m_wstrTitle.c_str());
	Util::SetEditField(GetDlgItem(hDlg, IDD_EDIT_CTL), m_wstrValue.c_str());
	::SetWindowText(GetDlgItem(hDlg, IDC_STATIC), m_wstrUnit.c_str());
	SendMessage(hDlg, DM_SETDEFID, IDOK, 0);
	SendMessage(GetDlgItem(hDlg, IDCANCEL), BM_SETSTYLE, BS_PUSHBUTTON, 0);
}
