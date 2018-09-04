// gridBrush.cpp
//

#include "stdafx.h"
#include "assert.h"
#include <unordered_map>
#include "gridPoint.h"
#include "gridRect.h"
#include "grid_model.h"
#include "gridBrush.h"

using namespace std;

bool GridBrush::m_bClassInitialized = false;

Set_Manipulator <short> * GridBrush::m_setMan; 
Add_Manipulator <short> * GridBrush::m_addMan; 
Subt_Manipulator<short> * GridBrush::m_subMan;    
Max_Manipulator <short> * GridBrush::m_maxMan; 
Min_Manipulator <short> * GridBrush::m_minMan; 
Mean_Manipulator<short> * GridBrush::m_meanMan;