// SignalPreview.ixx
//
// NNetWindows

module;

#include <compare>

export module NNetWin32:SignalPreview;

import Types;
import PixFpDimension;
import :TimeGraph;
import NNetModel;

export class SignalPreview : public TimeGraph
{
public:
	SignalPreview
	(
		BaseWindow           const &,
		PixFpDimension<fMicroSecs> &,
		PixFpDimension<mV>         &
	);

	~SignalPreview() final;

private:

	PixFpDimension<mV> & m_vertCoord;

	void DoPaint() final;
	bool OnSize(PIXEL const, PIXEL const) final;

	fPixel getY (fPixel const) const;
	fPixel yVolt(mV     const) const;

	fPixelPoint pixPntVolt(fMicroSecs const, mV const) const;
};