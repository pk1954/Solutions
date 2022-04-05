// InputConnector.h
//
// NNetModel

#pragma once

#include "NobType.h"
#include "Nob.h"
#include "tHighlightType.h"
//#include "SignalGenerator.h"
#include "IoConnector.h"

class Param;
class IoNeuron;
class DrawContext;

using std::unique_ptr;

class InputConnector: public IoConnector
{
public:

	static bool TypeFits(NobType const type) { return type.IsInputConnectorType(); }

	explicit InputConnector(Param &, vector<IoNeuron *> &&);

	~InputConnector() final = default;

	void Prepare() final { /* */ };
	void AppendMenuItems(AddMenuFunc const &) const final;

	NobIoMode GetIoMode() const final { return NobIoMode::input; }

	void DrawExterior(DrawContext const &, tHighlight const) const final;
};
