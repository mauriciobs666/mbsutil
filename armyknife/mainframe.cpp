#include <ctime>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>

#include "Hash.h"
#include "Thread.h"

#include "mainframe.h"

#include <wx/menu.h>
#include <wx/statusbr.h>
#include <wx/msgdlg.h>
#include <wx/filedlg.h>

//(*InternalHeaders(MainFrame)
#include <wx/bitmap.h>
#include <wx/font.h>
#include <wx/fontenum.h>
#include <wx/fontmap.h>
#include <wx/image.h>
#include <wx/intl.h>
#include <wx/settings.h>
//*)

//(*IdInit(MainFrame)
const long MainFrame::ID_NOTEBOOK1 = wxNewId();
const long MainFrame::ID_PANEL1 = wxNewId();
const long MainFrame::ID_TEXTCTRL2 = wxNewId();
const long MainFrame::ID_STATICTEXT8 = wxNewId();
const long MainFrame::ID_SPINCTRL2 = wxNewId();
const long MainFrame::ID_TEXTCTRL3 = wxNewId();
const long MainFrame::ID_PANEL4 = wxNewId();
const long MainFrame::ID_STATICTEXT1 = wxNewId();
const long MainFrame::ID_TEXTCTRL4 = wxNewId();
const long MainFrame::ID_BUTTON5 = wxNewId();
const long MainFrame::ID_STATICTEXT6 = wxNewId();
const long MainFrame::ID_TEXTCTRL9 = wxNewId();
const long MainFrame::ID_COMBOBOX1 = wxNewId();
const long MainFrame::ID_BUTTON6 = wxNewId();
const long MainFrame::ID_TEXTCTRL8 = wxNewId();
const long MainFrame::ID_PANEL6 = wxNewId();
const long MainFrame::ID_CALENDARCTRL2 = wxNewId();
const long MainFrame::ID_STATICTEXT4 = wxNewId();
const long MainFrame::ID_TEXTCTRL1 = wxNewId();
const long MainFrame::ID_BUTTON2 = wxNewId();
const long MainFrame::ID_TEXTCTRL7 = wxNewId();
const long MainFrame::ID_BUTTON3 = wxNewId();
const long MainFrame::ID_PANEL2 = wxNewId();
const long MainFrame::ID_STATICTEXT2 = wxNewId();
const long MainFrame::ID_TEXTCTRL5 = wxNewId();
const long MainFrame::ID_BUTTON1 = wxNewId();
const long MainFrame::ID_BUTTON4 = wxNewId();
const long MainFrame::ID_TEXTCTRL6 = wxNewId();
const long MainFrame::ID_PANEL3 = wxNewId();
const long MainFrame::ID_STATICTEXT3 = wxNewId();
const long MainFrame::ID_TEXTCTRL10 = wxNewId();
const long MainFrame::ID_BUTTON7 = wxNewId();
const long MainFrame::ID_STATICTEXT5 = wxNewId();
const long MainFrame::ID_SPINCTRL1 = wxNewId();
const long MainFrame::ID_BUTTON8 = wxNewId();
const long MainFrame::ID_TEXTCTRL11 = wxNewId();
const long MainFrame::ID_PANEL5 = wxNewId();
const long MainFrame::ID_TEXTCTRL13 = wxNewId();
const long MainFrame::ID_STATICTEXT7 = wxNewId();
const long MainFrame::ID_TEXTCTRL12 = wxNewId();
const long MainFrame::ID_BUTTON10 = wxNewId();
const long MainFrame::ID_COMBOBOX2 = wxNewId();
const long MainFrame::ID_PANEL7 = wxNewId();
const long MainFrame::ID_PANEL8 = wxNewId();
//*)

using namespace std;

int idMenuQuit = wxNewId();
int idMenuAbout = wxNewId();


BEGIN_EVENT_TABLE(MainFrame,wxFrame)
	//(*EventTable(MainFrame)
	EVT_TEXT_ENTER(ID_TEXTCTRL3,MainFrame::OnTextCtrl3TextEnter)
	EVT_BUTTON(ID_BUTTON5,MainFrame::OnSplitOpenFileClick)
	EVT_BUTTON(ID_BUTTON6,MainFrame::OnSplitGoClick)
	EVT_BUTTON(ID_BUTTON2,MainFrame::OnCalendarCalcTimetClick)
	EVT_BUTTON(ID_BUTTON3,MainFrame::OnCalendarCalcDTClick)
	EVT_BUTTON(ID_BUTTON1,MainFrame::OnMD4OpenFileClick)
	EVT_BUTTON(ID_BUTTON4,MainFrame::OnMD4CalcClick)
	EVT_BUTTON(ID_BUTTON7,MainFrame::OnRespawnOpenClick)
	EVT_BUTTON(ID_BUTTON8,MainFrame::OnRespawnGoClick)
	//*)
	EVT_MENU(idMenuQuit, MainFrame::OnQuit)
	EVT_MENU(idMenuAbout, MainFrame::OnAbout)
END_EVENT_TABLE()

MainFrame::MainFrame(wxWindow* parent,wxWindowID id)
{

	//(*Initialize(MainFrame)
	Create(parent,id,_("MBS - Army Knife"),wxDefaultPosition,wxDefaultSize,wxCAPTION|wxTHICK_FRAME|wxSYSTEM_MENU|wxRESIZE_BOX|wxCLOSE_BOX|wxMINIMIZE_BOX,_T(""));
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	Notebook1 = new wxNotebook(this,ID_NOTEBOOK1,wxDefaultPosition,wxSize(379,351),wxNB_BOTTOM|wxNB_MULTILINE,_("ID_NOTEBOOK1"));
	Panel1 = new wxPanel(Notebook1,ID_PANEL1,wxDefaultPosition,wxSize(390,282),wxTAB_TRAVERSAL,_("ID_PANEL1"));
	BoxSizer4 = new wxBoxSizer(wxVERTICAL);
	BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
	eval = new wxTextCtrl(Panel1,ID_TEXTCTRL2,_T(""),wxDefaultPosition,wxSize(297,252),wxTE_MULTILINE|wxTE_READONLY,wxDefaultValidator,_("ID_TEXTCTRL2"));
	if ( 0 ) eval->SetMaxLength(0);
	BoxSizer13 = new wxBoxSizer(wxVERTICAL);
	StaticBoxSizer9 = new wxStaticBoxSizer(wxHORIZONTAL,Panel1,_("Output"));
	StaticText8 = new wxStaticText(Panel1,ID_STATICTEXT8,_("Decimal places:"),wxDefaultPosition,wxDefaultSize,0,_("ID_STATICTEXT8"));
	CalculatorDecPlaces = new wxSpinCtrl(Panel1,ID_SPINCTRL2,_("2"),wxDefaultPosition,wxSize(44,21),0,0,100,0,_("ID_SPINCTRL2"));
	StaticBoxSizer9->Add(StaticText8,0,wxALL|wxALIGN_CENTER,0);
	StaticBoxSizer9->Add(CalculatorDecPlaces,0,wxALL|wxALIGN_CENTER,0);
	BoxSizer13->Add(StaticBoxSizer9,0,wxALL|wxALIGN_CENTER,0);
	BoxSizer3->Add(eval,1,wxALL|wxALIGN_CENTER|wxEXPAND,0);
	BoxSizer3->Add(BoxSizer13,0,wxALL|wxALIGN_CENTER|wxEXPAND,0);
	command = new wxTextCtrl(Panel1,ID_TEXTCTRL3,_T(""),wxDefaultPosition,wxDefaultSize,wxTE_PROCESS_ENTER,wxDefaultValidator,_("ID_TEXTCTRL3"));
	if ( 0 ) command->SetMaxLength(0);
	command->SetFocus();
	BoxSizer4->Add(BoxSizer3,1,wxALL|wxALIGN_CENTER|wxEXPAND,0);
	BoxSizer4->Add(command,0,wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_BOTTOM|wxEXPAND,0);
	Panel1->SetSizer(BoxSizer4);
	BoxSizer4->Fit(Panel1);
	BoxSizer4->SetSizeHints(Panel1);
	Panel4 = new wxPanel(Notebook1,ID_PANEL4,wxDefaultPosition,wxDefaultSize,wxTAB_TRAVERSAL,_("ID_PANEL4"));
	BoxSizer5 = new wxBoxSizer(wxVERTICAL);
	BoxSizer7 = new wxBoxSizer(wxHORIZONTAL);
	StaticText1 = new wxStaticText(Panel4,ID_STATICTEXT1,_("Input file:"),wxDefaultPosition,wxSize(48,13),0,_("ID_STATICTEXT1"));
	SplitFilename = new wxTextCtrl(Panel4,ID_TEXTCTRL4,_T(""),wxDefaultPosition,wxSize(180,21),0,wxDefaultValidator,_("ID_TEXTCTRL4"));
	if ( 0 ) SplitFilename->SetMaxLength(0);
	SplitOpenFile = new wxButton(Panel4,ID_BUTTON5,_("..."),wxDefaultPosition,wxSize(33,23),0,wxDefaultValidator,_("ID_BUTTON5"));
	if (false) SplitOpenFile->SetDefault();
	BoxSizer7->Add(StaticText1,0,wxALL|wxALIGN_CENTER,0);
	BoxSizer7->Add(SplitFilename,1,wxALL|wxALIGN_CENTER|wxEXPAND,0);
	BoxSizer7->Add(SplitOpenFile,0,wxALL|wxALIGN_CENTER,0);
	BoxSizer8 = new wxBoxSizer(wxHORIZONTAL);
	StaticText6 = new wxStaticText(Panel4,ID_STATICTEXT6,_("Part size:"),wxDefaultPosition,wxDefaultSize,0,_("ID_STATICTEXT6"));
	SplitPartsize = new wxTextCtrl(Panel4,ID_TEXTCTRL9,_T(""),wxDefaultPosition,wxSize(114,21),0,wxDefaultValidator,_("ID_TEXTCTRL9"));
	if ( 0 ) SplitPartsize->SetMaxLength(0);
	SplitPartsizeMult = new wxComboBox(Panel4,ID_COMBOBOX1,_T(""),wxDefaultPosition,wxDefaultSize,0,NULL,wxCB_READONLY,wxDefaultValidator,_("ID_COMBOBOX1"));
	SplitPartsizeMult->Append(_("kB"));
	SplitPartsizeMult->Append(_("MB"));
	SplitPartsizeMult->SetSelection(1);
	SplitGo = new wxButton(Panel4,ID_BUTTON6,_("Go"),wxDefaultPosition,wxSize(39,23),0,wxDefaultValidator,_("ID_BUTTON6"));
	if (false) SplitGo->SetDefault();
	BoxSizer8->Add(StaticText6,0,wxALL|wxALIGN_CENTER,0);
	BoxSizer8->Add(SplitPartsize,0,wxALL|wxALIGN_CENTER|wxEXPAND,0);
	BoxSizer8->Add(SplitPartsizeMult,0,wxALL|wxALIGN_CENTER,0);
	BoxSizer8->Add(SplitGo,0,wxALL|wxALIGN_CENTER,0);
	SplitLog = new wxTextCtrl(Panel4,ID_TEXTCTRL8,_T(""),wxDefaultPosition,wxDefaultSize,wxTE_MULTILINE|wxTE_READONLY,wxDefaultValidator,_("ID_TEXTCTRL8"));
	if ( 0 ) SplitLog->SetMaxLength(0);
	BoxSizer5->Add(BoxSizer7,0,wxALL|wxALIGN_CENTER|wxEXPAND,2);
	BoxSizer5->Add(BoxSizer8,0,wxALL|wxALIGN_CENTER|wxEXPAND,2);
	BoxSizer5->Add(SplitLog,1,wxALL|wxALIGN_CENTER|wxEXPAND,2);
	Panel4->SetSizer(BoxSizer5);
	BoxSizer5->Fit(Panel4);
	BoxSizer5->SetSizeHints(Panel4);
	Panel6 = new wxPanel(Notebook1,ID_PANEL6,wxDefaultPosition,wxDefaultSize,0,_("ID_PANEL6"));
	BoxSizer9 = new wxBoxSizer(wxVERTICAL);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxVERTICAL,Panel6,_("Calendar"));
	CalendarCtrl2 = new wxCalendarCtrl(Panel6,ID_CALENDARCTRL2,wxDefaultDateTime,wxDefaultPosition,wxSize(174,137),wxCAL_SHOW_HOLIDAYS|wxCAL_SHOW_SURROUNDING_WEEKS,_("ID_CALENDARCTRL2"));
	BoxSizer10 = new wxBoxSizer(wxHORIZONTAL);
	StaticText4 = new wxStaticText(Panel6,ID_STATICTEXT4,_("Time:"),wxDefaultPosition,wxDefaultSize,0,_("ID_STATICTEXT4"));
	CalendarTime = new wxTextCtrl(Panel6,ID_TEXTCTRL1,_("Not yet..."),wxDefaultPosition,wxDefaultSize,wxTE_READONLY,wxDefaultValidator,_("ID_TEXTCTRL1"));
	if ( 0 ) CalendarTime->SetMaxLength(0);
	CalendarTime->Disable();
	CalendarCalcTimet = new wxButton(Panel6,ID_BUTTON2,_("Calculate time_t"),wxDefaultPosition,wxSize(84,23),0,wxDefaultValidator,_("ID_BUTTON2"));
	if (false) CalendarCalcTimet->SetDefault();
	BoxSizer10->Add(StaticText4,0,wxALL|wxALIGN_CENTER,0);
	BoxSizer10->Add(CalendarTime,1,wxALL|wxALIGN_CENTER,0);
	BoxSizer10->Add(CalendarCalcTimet,0,wxALL|wxALIGN_CENTER,0);
	StaticBoxSizer1->Add(CalendarCtrl2,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP,5);
	StaticBoxSizer1->Add(BoxSizer10,0,wxALL|wxALIGN_CENTER|wxEXPAND,0);
	StaticBoxSizer2 = new wxStaticBoxSizer(wxHORIZONTAL,Panel6,_("time_t"));
	BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
	CalendarTimet = new wxTextCtrl(Panel6,ID_TEXTCTRL7,_T(""),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_("ID_TEXTCTRL7"));
	if ( 0 ) CalendarTimet->SetMaxLength(0);
	CalendarCalcDT = new wxButton(Panel6,ID_BUTTON3,_("Calculate Date"),wxDefaultPosition,wxSize(82,23),0,wxDefaultValidator,_("ID_BUTTON3"));
	if (false) CalendarCalcDT->SetDefault();
	BoxSizer2->Add(CalendarTimet,1,wxALL|wxALIGN_CENTER|wxEXPAND,0);
	BoxSizer2->Add(CalendarCalcDT,0,wxALL|wxALIGN_CENTER|wxEXPAND,0);
	StaticBoxSizer2->Add(BoxSizer2,1,wxALL|wxALIGN_CENTER|wxEXPAND,0);
	BoxSizer9->Add(StaticBoxSizer1,0,wxALL|wxALIGN_CENTER|wxEXPAND,0);
	BoxSizer9->Add(StaticBoxSizer2,0,wxALL|wxALIGN_CENTER|wxEXPAND,0);
	Panel6->SetSizer(BoxSizer9);
	BoxSizer9->Fit(Panel6);
	BoxSizer9->SetSizeHints(Panel6);
	Panel2 = new wxPanel(Notebook1,ID_PANEL2,wxDefaultPosition,wxDefaultSize,0,_("ID_PANEL2"));
	BoxSizer6 = new wxBoxSizer(wxVERTICAL);
	StaticBoxSizer3 = new wxStaticBoxSizer(wxHORIZONTAL,Panel2,_("Input file"));
	StaticText2 = new wxStaticText(Panel2,ID_STATICTEXT2,_("File:"),wxDefaultPosition,wxDefaultSize,0,_("ID_STATICTEXT2"));
	MD4Filename = new wxTextCtrl(Panel2,ID_TEXTCTRL5,_T(""),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_("ID_TEXTCTRL5"));
	if ( 0 ) MD4Filename->SetMaxLength(0);
	MD4OpenFile = new wxButton(Panel2,ID_BUTTON1,_("..."),wxDefaultPosition,wxSize(32,23),0,wxDefaultValidator,_("ID_BUTTON1"));
	if (false) MD4OpenFile->SetDefault();
	MD4OpenFile->SetFocus();
	MD4Calc = new wxButton(Panel2,ID_BUTTON4,_("Go"),wxDefaultPosition,wxSize(32,23),0,wxDefaultValidator,_("ID_BUTTON4"));
	if (false) MD4Calc->SetDefault();
	StaticBoxSizer3->Add(StaticText2,0,wxALL|wxALIGN_CENTER,0);
	StaticBoxSizer3->Add(-1,-1,0);
	StaticBoxSizer3->Add(MD4Filename,1,wxALL|wxALIGN_CENTER|wxEXPAND,0);
	StaticBoxSizer3->Add(MD4OpenFile,0,wxALL|wxALIGN_CENTER|wxEXPAND,0);
	StaticBoxSizer3->Add(MD4Calc,0,wxALL|wxALIGN_CENTER|wxEXPAND,0);
	StaticBoxSizer4 = new wxStaticBoxSizer(wxHORIZONTAL,Panel2,_("Log"));
	MD4Result = new wxTextCtrl(Panel2,ID_TEXTCTRL6,_T(""),wxDefaultPosition,wxSize(169,21),wxTE_MULTILINE|wxTE_READONLY,wxDefaultValidator,_("ID_TEXTCTRL6"));
	if ( 0 ) MD4Result->SetMaxLength(0);
	StaticBoxSizer4->Add(MD4Result,1,wxALL|wxALIGN_CENTER|wxEXPAND,0);
	BoxSizer6->Add(StaticBoxSizer3,0,wxALL|wxALIGN_CENTER|wxEXPAND,0);
	BoxSizer6->Add(StaticBoxSizer4,1,wxALL|wxALIGN_CENTER|wxEXPAND,0);
	Panel2->SetSizer(BoxSizer6);
	BoxSizer6->Fit(Panel2);
	BoxSizer6->SetSizeHints(Panel2);
	Panel3 = new wxPanel(Notebook1,ID_PANEL3,wxDefaultPosition,wxDefaultSize,0,_("ID_PANEL3"));
	BoxSizer11 = new wxBoxSizer(wxVERTICAL);
	StaticBoxSizer5 = new wxStaticBoxSizer(wxHORIZONTAL,Panel3,_("Process"));
	BoxSizer12 = new wxBoxSizer(wxHORIZONTAL);
	StaticText3 = new wxStaticText(Panel3,ID_STATICTEXT3,_("File:"),wxDefaultPosition,wxSize(20,12),0,_("ID_STATICTEXT3"));
	RespawnFileName = new wxTextCtrl(Panel3,ID_TEXTCTRL10,_T(""),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_("ID_TEXTCTRL10"));
	if ( 0 ) RespawnFileName->SetMaxLength(0);
	RespawnOpen = new wxButton(Panel3,ID_BUTTON7,_("..."),wxDefaultPosition,wxSize(34,23),0,wxDefaultValidator,_("ID_BUTTON7"));
	if (false) RespawnOpen->SetDefault();
	StaticText5 = new wxStaticText(Panel3,ID_STATICTEXT5,_("# of times:"),wxDefaultPosition,wxDefaultSize,0,_("ID_STATICTEXT5"));
	RespawnNumber = new wxSpinCtrl(Panel3,ID_SPINCTRL1,_("0"),wxDefaultPosition,wxSize(51,23),0,0,100,0,_("ID_SPINCTRL1"));
	RespawnGo = new wxButton(Panel3,ID_BUTTON8,_("Go"),wxDefaultPosition,wxSize(36,23),0,wxDefaultValidator,_("ID_BUTTON8"));
	if (false) RespawnGo->SetDefault();
	BoxSizer12->Add(StaticText3,0,wxALL|wxALIGN_CENTER,0);
	BoxSizer12->Add(RespawnFileName,1,wxALL|wxALIGN_CENTER|wxEXPAND,0);
	BoxSizer12->Add(RespawnOpen,0,wxALL|wxALIGN_CENTER|wxEXPAND,0);
	BoxSizer12->Add(16,20,0);
	BoxSizer12->Add(StaticText5,0,wxALL|wxALIGN_CENTER,0);
	BoxSizer12->Add(RespawnNumber,0,wxALL|wxALIGN_CENTER|wxEXPAND,0);
	BoxSizer12->Add(RespawnGo,0,wxALL|wxALIGN_CENTER|wxEXPAND,0);
	StaticBoxSizer5->Add(BoxSizer12,1,wxALL|wxALIGN_CENTER|wxEXPAND,0);
	StaticBoxSizer6 = new wxStaticBoxSizer(wxHORIZONTAL,Panel3,_("Log"));
	RespawnLog = new wxTextCtrl(Panel3,ID_TEXTCTRL11,_T(""),wxDefaultPosition,wxDefaultSize,wxTE_MULTILINE|wxTE_READONLY,wxDefaultValidator,_("ID_TEXTCTRL11"));
	if ( 0 ) RespawnLog->SetMaxLength(0);
	StaticBoxSizer6->Add(RespawnLog,1,wxALL|wxALIGN_CENTER|wxEXPAND,0);
	BoxSizer11->Add(StaticBoxSizer5,0,wxALL|wxALIGN_CENTER|wxEXPAND,0);
	BoxSizer11->Add(StaticBoxSizer6,1,wxALL|wxALIGN_CENTER|wxEXPAND,0);
	Panel3->SetSizer(BoxSizer11);
	BoxSizer11->Fit(Panel3);
	BoxSizer11->SetSizeHints(Panel3);
	Panel5 = new wxPanel(Notebook1,ID_PANEL5,wxDefaultPosition,wxDefaultSize,0,_("ID_PANEL5"));
	BoxSizer14 = new wxBoxSizer(wxVERTICAL);
	StaticBoxSizer7 = new wxStaticBoxSizer(wxHORIZONTAL,Panel5,_("Log"));
	TextCtrl13 = new wxTextCtrl(Panel5,ID_TEXTCTRL13,_("nao implementado ainda"),wxDefaultPosition,wxDefaultSize,wxTE_MULTILINE|wxTE_READONLY,wxDefaultValidator,_("ID_TEXTCTRL13"));
	if ( 0 ) TextCtrl13->SetMaxLength(0);
	StaticBoxSizer7->Add(TextCtrl13,1,wxALL|wxALIGN_CENTER|wxEXPAND,0);
	StaticBoxSizer8 = new wxStaticBoxSizer(wxHORIZONTAL,Panel5,_("Server"));
	StaticText7 = new wxStaticText(Panel5,ID_STATICTEXT7,_("Port:"),wxDefaultPosition,wxSize(30,13),0,_("ID_STATICTEXT7"));
	TextCtrl12 = new wxTextCtrl(Panel5,ID_TEXTCTRL12,_T(""),wxDefaultPosition,wxSize(85,23),0,wxDefaultValidator,_("ID_TEXTCTRL12"));
	if ( 0 ) TextCtrl12->SetMaxLength(0);
	Button10 = new wxButton(Panel5,ID_BUTTON10,_("Go"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_("ID_BUTTON10"));
	if (false) Button10->SetDefault();
	ComboBox2 = new wxComboBox(Panel5,ID_COMBOBOX2,_T(""),wxDefaultPosition,wxDefaultSize,0,NULL,0,wxDefaultValidator,_("ID_COMBOBOX2"));
	ComboBox2->Append(_("Echo"));
	ComboBox2->Append(_("Silent"));
	StaticBoxSizer8->Add(StaticText7,0,wxALL|wxALIGN_CENTER,0);
	StaticBoxSizer8->Add(-1,-1,0);
	StaticBoxSizer8->Add(TextCtrl12,0,wxALL|wxALIGN_CENTER|wxEXPAND,0);
	StaticBoxSizer8->Add(-1,-1,0);
	StaticBoxSizer8->Add(Button10,0,wxALL|wxALIGN_CENTER,0);
	StaticBoxSizer8->Add(-1,-1,0);
	StaticBoxSizer8->Add(ComboBox2,0,wxALL|wxALIGN_CENTER,0);
	BoxSizer14->Add(StaticBoxSizer7,1,wxALL|wxALIGN_CENTER|wxEXPAND,0);
	BoxSizer14->Add(StaticBoxSizer8,0,wxALL|wxALIGN_CENTER|wxEXPAND,0);
	Panel5->SetSizer(BoxSizer14);
	BoxSizer14->Fit(Panel5);
	BoxSizer14->SetSizeHints(Panel5);
	Panel7 = new wxPanel(Notebook1,ID_PANEL7,wxDefaultPosition,wxDefaultSize,0,_("ID_PANEL7"));
	Panel8 = new wxPanel(Notebook1,ID_PANEL8,wxDefaultPosition,wxDefaultSize,0,_("ID_PANEL8"));
	Notebook1->AddPage(Panel1,_("Calculator"),false);
	Notebook1->AddPage(Panel4,_("Split"),false);
	Notebook1->AddPage(Panel6,_("Calendar"),false);
	Notebook1->AddPage(Panel2,_("MD4"),false);
	Notebook1->AddPage(Panel3,_("Respawn"),false);
	Notebook1->AddPage(Panel5,_("Echo Server"),false);
	Notebook1->AddPage(Panel7,_("TCP Connect"),false);
	Notebook1->AddPage(Panel8,_("Shutdown"),false);
	BoxSizer1->Add(Notebook1,1,wxALL|wxALIGN_CENTER|wxEXPAND,0);
	this->SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
	Center();
	//*)

	wxMenuBar* mbar = new wxMenuBar();
	wxMenu* fileMenu = new wxMenu(_(""));
	fileMenu->Append(idMenuQuit, _("&Quit\tAlt-F4"), _("Quit the application"));
	mbar->Append(fileMenu, _("&File"));

	wxMenu* helpMenu = new wxMenu(_(""));
	helpMenu->Append(idMenuAbout, _("&About\tF1"), _("Show info about this application"));
	mbar->Append(helpMenu, _("&Help"));

	SetMenuBar(mbar);
}

MainFrame::~MainFrame()
{
}

void MainFrame::OnQuit(wxCommandEvent& event)
{
	Close();
}

void MainFrame::OnAbout(wxCommandEvent& event)
{
	wxMessageBox(_("©2006 - Mauricio Bieze Stefani"), _("MBS - Army Knife"));
}

/*****************************************************************************
	Calculator
*****************************************************************************/

void MainFrame::OnTextCtrl3TextEnter(wxCommandEvent& event)
{
	ostringstream os;
	os.precision(50);
	string cmd=command->GetValue().c_str();
	if(cmd!="")
	{
		try
		{
			os << setiosflags(ios::fixed) << setprecision(CalculatorDecPlaces->GetValue()) << exp.eval(cmd);
			(*eval) << command->GetValue() << " = " << os.str().c_str() << "\n";
			command->SetValue(os.str().c_str());
		}
		catch(string erro)
		{
			(*eval) << "Erro: " << erro.c_str() << "\n";
		};
		command->SetInsertionPointEnd();
	}
}

/*****************************************************************************
	MD4
*****************************************************************************/

void MainFrame::OnMD4OpenFileClick(wxCommandEvent& event)
{
	wxFileDialog *fdlg=new wxFileDialog(this);
	if(wxID_OK==fdlg->ShowModal())
		MD4Filename->SetValue(fdlg->GetPath());
}

void MainFrame::OnMD4CalcClick(wxCommandEvent& event)
{
	MD4 mbsmd4;
	MD4Result->SetValue("Calculating file hash... please wait");
	if(0==mbsmd4.arquivo(MD4Filename->GetValue().c_str()))
		MD4Result->SetValue(mbsmd4.toString().c_str());
	else
		MD4Result->SetValue("Error opening file");
}

/*****************************************************************************
	Calendar
*****************************************************************************/

void MainFrame::OnCalendarCalcTimetClick(wxCommandEvent& event)
{
	wxDateTime wxdt=CalendarCtrl2->GetDate();
	structtm.tm_year=wxdt.GetYear()-1900;
	structtm.tm_mon=wxdt.GetMonth();
	structtm.tm_mday=wxdt.GetDay();
	structtm.tm_sec=structtm.tm_min=structtm.tm_hour=structtm.tm_isdst=0;
	timet=mktime(&structtm);
	CalendarTimet->SetValue(int2str(timet).c_str());
}

void MainFrame::OnCalendarCalcDTClick(wxCommandEvent& event)
{
	timet=atol(CalendarTimet->GetValue().c_str());
	CalendarCtrl2->SetDate(timet);
}

/*****************************************************************************
	Split
*****************************************************************************/

void MainFrame::OnSplitOpenFileClick(wxCommandEvent& event)
{
	wxFileDialog *fdlg=new wxFileDialog(this);
	if(wxID_OK==fdlg->ShowModal())
		SplitFilename->SetValue(fdlg->GetPath());
}

void MainFrame::OnSplitGoClick(wxCommandEvent& event)
{
	const unsigned long BUFSIZE=1024;
	double dsize;
	dsize=atof(SplitPartsize->GetValue().c_str());
	dsize*=1024;
	if(SplitPartsizeMult->GetValue()=="MB")
		dsize*=1024;
	unsigned long size=(unsigned long)(dsize);
	if(size>BUFSIZE)
	{
		SplitLog->AppendText(("Selected part size: "+int2str(size)+" bytes\n").c_str());
		ifstream arqin;
		string filename=SplitFilename->GetValue().c_str();
		SplitLog->AppendText(("Opening input file "+filename+"...").c_str());
		arqin.open(filename.c_str(),ios::binary | ios::in);
		if(arqin.good())
		{
			SplitLog->AppendText("ok\n");
			char *temp=new char[BUFSIZE];
			if(temp!=NULL)
			{
				ofstream arqout;
				unsigned long ofcount=0;
				unsigned long ofsize=0;
				bool ofopen=false;
				bool error=false;
				unsigned long readcount;
				while(!arqin.eof() & !error)
				{
					arqin.read(temp,BUFSIZE);
					readcount=arqin.gcount();
					if(readcount>0)
					{
						if(!ofopen)
						{
							SplitLog->AppendText("Writing "+SplitFilename->GetValue()+"."+int2str(ofcount).c_str()+"...");
							arqout.open(SplitFilename->GetValue()+"."+int2str(ofcount).c_str(),ios::binary | ios::out);
							if(arqout.good())
								ofopen=true;
							else
							{
								error=true;
								break;
							}
							ofsize=0;
						}
						arqout.write(temp,readcount);
						if(arqout.good())
							ofsize+=readcount;
						else
						{
							error=true;
							break;
						}
						if(ofsize>=size)
						{
							if(error)
								SplitLog->AppendText("error\n");
							else
								SplitLog->AppendText("ok\n");
							arqout.close();
							ofopen=false;
							ofcount++;
						}
					}
				}
				if(ofopen)
				{
					if(error)
						SplitLog->AppendText("error\n");
					else
						SplitLog->AppendText("ok\n");
					arqout.close();
				}
				SplitLog->AppendText("Finished!\n");
				delete temp;
			}
			else
				SplitLog->AppendText("ARGHHH! Memory allocation error!\n");
		}
		else
			SplitLog->AppendText("Error!\n");
	}
	else
		SplitLog->AppendText("Invalid size\n");
}

/*****************************************************************************
	Respawn
*****************************************************************************/

class Respawn : public Thread
{
	public:
		Respawn(int n, string cmd);
		virtual void run();
	private:
		int falta;
		string comando;
};

Respawn::Respawn(int n, string cmd)
{
	falta=n;
	comando=cmd;
}

void Respawn::run()
{
	if(0==falta)
		while(ativa)
			system(comando.c_str());

	for(;falta>0;falta--)
		if(ativa)
			system(comando.c_str());
		else
			break;
}

void MainFrame::OnRespawnGoClick(wxCommandEvent& event)
{
	static Respawn *r=NULL;
	if(RespawnGo->GetLabel()=="Go")
	{
		r=new Respawn(RespawnNumber->GetValue(),RespawnFileName->GetValue().c_str());
		r->resume();
		RespawnGo->SetLabel("Stop");
	}
	else
	{
		r->executando(false);
		while(r->executando());
		delete r;
		RespawnGo->SetLabel("Go");
	}
}

void MainFrame::OnRespawnOpenClick(wxCommandEvent& event)
{
	wxFileDialog *fdlg=new wxFileDialog(this);
	if(wxID_OK==fdlg->ShowModal())
		RespawnFileName->SetValue(fdlg->GetPath());
}

