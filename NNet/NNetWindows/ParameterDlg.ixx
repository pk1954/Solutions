// ParameterDlg.ixx
//
// NNetWindows

module;

#include <d2d1.h>
#include <compare>
#include <memory>
#include <string>
#include <Windows.h>

export module NNetWin32:ParameterDialog;

import Direct2D;
import BaseDialog;
import NNetModel;

using std::unique_ptr;
using std::wstring;

export class ParameterDialog : public BaseDialog
{
public:
	ParameterDialog();
	~ParameterDialog() final;

	void Start(HWND const);
	void Stop();
	void SetModelInterface(NNetModelWriterInterface* const);
	void Notify(bool const) final;
	void PaintGraphics() final;

private:
	static int const LEFT_SPACE       {  16 };
	static int const HORZ_SPACE       {   8 };
	static int const NAME_WIDTH       { 100 };
	static int const EDIT_WIDTH       {  60 };
	static int const UNIT_WIDTH       {  40 };
	static int const VERT_SPACE       {  10 };
	static int const VERT_BLOCK_SPACE {  30 };
	static int const HEIGHT           {  16 };

	IDWriteTextFormat        * m_pTextFormatHeader { nullptr };
	NNetModelWriterInterface * m_pNMWI             { nullptr };

	fPixel m_fPixPosVert;      // helper for paintHeader

	HWND m_hwndPulseFreqMax   { nullptr };
	HWND m_hwndPeakVoltage    { nullptr };
	HWND m_hwndThreshold      { nullptr };
	HWND m_hwndSynapseDelay   { nullptr };
	HWND m_hwndPulseWidth     { nullptr };
	HWND m_hwndTimeResolution { nullptr };
	HWND m_hwndFilterSize     { nullptr };
	HWND m_hwndPulseSpeed     { nullptr };

	void resetParameter(HWND const, ParamType::Value const) const;
	void applyParameter(HWND const, ParamType::Value const);
	HWND addParameter  (HWND const, ParamType::Value const, int &);
	void refreshParameters();
	void applyParameters();
	void paintHeader(int const, wstring const &);

	ParameterDialog             (ParameterDialog const &);  // noncopyable class 
	ParameterDialog & operator= (ParameterDialog const &);  // noncopyable class 

	bool OnCommand(WPARAM const, LPARAM const, PixelPoint const = PixelPoint::NULL_VAL()) final;
	bool UserProc(UINT const, WPARAM const, LPARAM const) final;

	void AddColorCtlMenu(HMENU const) final {}
};
