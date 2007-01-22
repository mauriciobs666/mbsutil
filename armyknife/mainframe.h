#ifndef MAINFRAME_H
#define MAINFRAME_H

#include "Parser.h"

//(*Headers(MainFrame)
#include <wx/button.h>
#include <wx/calctrl.h>
#include <wx/combobox.h>
#include <wx/frame.h>
#include <wx/notebook.h>
#include <wx/panel.h>
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
		static const long ID_NOTEBOOK1;
		static const long ID_PANEL1;
		static const long ID_TEXTCTRL2;
		static const long ID_STATICTEXT8;
		static const long ID_SPINCTRL2;
		static const long ID_TEXTCTRL3;
		static const long ID_PANEL4;
		static const long ID_STATICTEXT1;
		static const long ID_TEXTCTRL4;
		static const long ID_BUTTON5;
		static const long ID_STATICTEXT6;
		static const long ID_TEXTCTRL9;
		static const long ID_COMBOBOX1;
		static const long ID_BUTTON6;
		static const long ID_TEXTCTRL8;
		static const long ID_PANEL6;
		static const long ID_CALENDARCTRL2;
		static const long ID_STATICTEXT4;
		static const long ID_TEXTCTRL1;
		static const long ID_BUTTON2;
		static const long ID_TEXTCTRL7;
		static const long ID_BUTTON3;
		static const long ID_PANEL2;
		static const long ID_STATICTEXT2;
		static const long ID_TEXTCTRL5;
		static const long ID_BUTTON1;
		static const long ID_BUTTON4;
		static const long ID_TEXTCTRL6;
		static const long ID_PANEL3;
		static const long ID_STATICTEXT3;
		static const long ID_TEXTCTRL10;
		static const long ID_BUTTON7;
		static const long ID_STATICTEXT5;
		static const long ID_SPINCTRL1;
		static const long ID_BUTTON8;
		static const long ID_TEXTCTRL11;
		static const long ID_PANEL5;
		static const long ID_TEXTCTRL13;
		static const long ID_STATICTEXT7;
		static const long ID_TEXTCTRL12;
		static const long ID_BUTTON10;
		static const long ID_COMBOBOX2;
		static const long ID_PANEL7;
		static const long ID_PANEL8;
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
