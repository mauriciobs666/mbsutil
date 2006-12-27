#ifndef MAINFRAME_H
#define MAINFRAME_H

#include "Parser.h"

//(*Headers(MainFrame)
#include <wx/button.h>
#include <wx/calctrl.h>
#include <wx/combobox.h>
#include <wx/frame.h>
#include <wx/intl.h>
#include <wx/notebook.h>
#include <wx/panel.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/spinctrl.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
//*)

class MainFrame: public wxFrame
{
	public:

		MainFrame(wxWindow* parent,wxWindowID id = -1);
		virtual ~MainFrame();


		//(*Identifiers(MainFrame)
		enum Identifiers
		{
			ID_NOTEBOOK1 = 0x1000,
			ID_PANEL1,
			ID_TEXTCTRL2,
			ID_STATICTEXT8,
			ID_SPINCTRL2,
			ID_TEXTCTRL3,
			ID_PANEL4,
			ID_STATICTEXT1,
			ID_TEXTCTRL4,
			ID_BUTTON5,
			ID_STATICTEXT6,
			ID_TEXTCTRL9,
			ID_COMBOBOX1,
			ID_BUTTON6,
			ID_TEXTCTRL8,
			ID_PANEL6,
			ID_CALENDARCTRL2,
			ID_STATICTEXT4,
			ID_TEXTCTRL1,
			ID_BUTTON2,
			ID_TEXTCTRL7,
			ID_BUTTON3,
			ID_PANEL2,
			ID_STATICTEXT2,
			ID_TEXTCTRL5,
			ID_BUTTON1,
			ID_BUTTON4,
			ID_TEXTCTRL6,
			ID_PANEL3,
			ID_STATICTEXT3,
			ID_TEXTCTRL10,
			ID_BUTTON7,
			ID_STATICTEXT5,
			ID_SPINCTRL1,
			ID_BUTTON8,
			ID_TEXTCTRL11,
			ID_PANEL5,
			ID_TEXTCTRL13,
			ID_STATICTEXT7,
			ID_TEXTCTRL12,
			ID_BUTTON10,
			ID_COMBOBOX2,
			ID_PANEL7,
			ID_PANEL8
		};
		//*)

	protected:
		//Calculadora

		Expressao exp;

		//Calendario

		time_t timet;
		struct tm structtm;

		//(*Handlers(MainFrame)
		void OnQuit(wxCommandEvent& event);
		void OnAbout(wxCommandEvent& event);
		void OnTextCtrl3TextEnter(wxCommandEvent& event);
		void OnMD4OpenFileClick(wxCommandEvent& event);
		void OnMD4CalcClick(wxCommandEvent& event);
		void OnCalendarCalcTimetClick(wxCommandEvent& event);
		void OnCalendarCalcDTClick(wxCommandEvent& event);
		void OnSplitOpenFileClick(wxCommandEvent& event);
		void OnSplitGoClick(wxCommandEvent& event);
		void OnRespawnGoClick(wxCommandEvent& event);
		void OnRespawnOpenClick(wxCommandEvent& event);
		void OnBotaoKLinitClick(wxCommandEvent& event);
		void OnBotaoKLfimClick(wxCommandEvent& event);
		//*)


		//(*Declarations(MainFrame)
		wxBoxSizer* BoxSizer1;
		wxNotebook* Notebook1;
		wxPanel* Panel1;
		wxBoxSizer* BoxSizer4;
		wxBoxSizer* BoxSizer3;
		wxTextCtrl* eval;
		wxBoxSizer* BoxSizer13;
		wxStaticBoxSizer* StaticBoxSizer9;
		wxStaticText* StaticText8;
		wxSpinCtrl* CalculatorDecPlaces;
		wxTextCtrl* command;
		wxPanel* Panel4;
		wxBoxSizer* BoxSizer5;
		wxBoxSizer* BoxSizer7;
		wxStaticText* StaticText1;
		wxTextCtrl* SplitFilename;
		wxButton* SplitOpenFile;
		wxBoxSizer* BoxSizer8;
		wxStaticText* StaticText6;
		wxTextCtrl* SplitPartsize;
		wxComboBox* SplitPartsizeMult;
		wxButton* SplitGo;
		wxTextCtrl* SplitLog;
		wxPanel* Panel6;
		wxBoxSizer* BoxSizer9;
		wxStaticBoxSizer* StaticBoxSizer1;
		wxCalendarCtrl* CalendarCtrl2;
		wxBoxSizer* BoxSizer10;
		wxStaticText* StaticText4;
		wxTextCtrl* CalendarTime;
		wxButton* CalendarCalcTimet;
		wxStaticBoxSizer* StaticBoxSizer2;
		wxBoxSizer* BoxSizer2;
		wxTextCtrl* CalendarTimet;
		wxButton* CalendarCalcDT;
		wxPanel* Panel2;
		wxBoxSizer* BoxSizer6;
		wxStaticBoxSizer* StaticBoxSizer3;
		wxStaticText* StaticText2;
		wxTextCtrl* MD4Filename;
		wxButton* MD4OpenFile;
		wxButton* MD4Calc;
		wxStaticBoxSizer* StaticBoxSizer4;
		wxTextCtrl* MD4Result;
		wxPanel* Panel3;
		wxBoxSizer* BoxSizer11;
		wxStaticBoxSizer* StaticBoxSizer5;
		wxBoxSizer* BoxSizer12;
		wxStaticText* StaticText3;
		wxTextCtrl* RespawnFileName;
		wxButton* RespawnOpen;
		wxStaticText* StaticText5;
		wxSpinCtrl* RespawnNumber;
		wxButton* RespawnGo;
		wxStaticBoxSizer* StaticBoxSizer6;
		wxTextCtrl* RespawnLog;
		wxPanel* Panel5;
		wxBoxSizer* BoxSizer14;
		wxStaticBoxSizer* StaticBoxSizer7;
		wxTextCtrl* TextCtrl13;
		wxStaticBoxSizer* StaticBoxSizer8;
		wxStaticText* StaticText7;
		wxTextCtrl* TextCtrl12;
		wxButton* Button10;
		wxComboBox* ComboBox2;
		wxPanel* Panel7;
		wxPanel* Panel8;
		//*)

	private:

		DECLARE_EVENT_TABLE()
};

#endif
