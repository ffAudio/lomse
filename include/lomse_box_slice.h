//---------------------------------------------------------------------------------------
// This file is part of the Lomse library.
// Lomse is copyrighted work (c) 2010-2016. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright notice, this
//      list of conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above copyright notice, this
//      list of conditions and the following disclaimer in the documentation and/or
//      other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
// SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
// TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
// BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
// ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
// DAMAGE.
//
// For any comment, suggestion or feature request, please contact the manager of
// the project at cecilios@users.sourceforge.net
//---------------------------------------------------------------------------------------

#ifndef __LOMSE_BOX_SLICE_H__        //to avoid nested includes
#define __LOMSE_BOX_SLICE_H__

#include "lomse_basic.h"
#include "lomse_gm_basic.h"
//#include <sstream>
//
//using namespace std;

namespace lomse
{

//forward declarations
class GmoBoxSystem;
class GmoBoxSliceInstr;
class ImoInstrument;


//---------------------------------------------------------------------------------------
// GmoBoxSlice: a system column
class GmoBoxSlice : public GmoBox
{
protected:
//    int             m_nAbsMeasure;		//number of this measure (absolute, 1..n)
//	int				m_nNumInSystem;		//number of slice for this system (0..n-1)
//
//    //start and end positions
//    LUnits    m_xStart;
//    LUnits    m_xEnd;
//

public:
    GmoBoxSlice(int nAbsMeasure, ImoObj* pCreatorImo);    //, int nNumInSystem,
			    //LUnits xStart=0, LUnits xEnd=0);
    ~GmoBoxSlice();

//    inline void UpdateSize(LUnits xStart, LUnits xEnd) {
//            m_xStart = xStart;
//            m_xEnd = xEnd;
//        }
//
//	//render
//	void DrawSelRectangle(lmPaper* pPaper);
//
//    //info
//    inline int GetNumMeasure() const { return m_nAbsMeasure; }
//
	//instrument slices
    GmoBoxSliceInstr* add_box_for_instrument(ImoInstrument* pInstr);
//	GmoBoxSliceInstr* GetSliceInstr(int i) const { return (GmoBoxSliceInstr*)m_Boxes[i]; }
//
//    //implementation of virtual methods from base class
//	int GetPageNumber() const;
//
//	//owners and related
//	GmoBoxSystem* GetOwnerSystem() { return m_pBoxSystem; }
//    GmoBoxScore* GetOwnerBoxScore();
//    GmoBoxPage* GetOwnerBoxPage();
//
//    //overrides
//    void SetBottomSpace(LUnits uyValue);

//    TimeUnits GetGridTimeForPosition(LUnits uxPos);
//    void DrawTimeLines(lmPaper* pPaper, wxColour color, LUnits uyTop,
//                       LUnits uyBottom);
    GmoBoxSystem* get_system_box();


};


}   //namespace lomse

#endif    // __LOMSE_BOX_SLICE_H__
