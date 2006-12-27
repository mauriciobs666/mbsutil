#include "app.h"

//(*AppHeaders
#include "mainframe.h"
//*)

IMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{

	//(*AppInitialize
	bool wxsOK = true;
	if ( wxsOK )
	{
		MainFrame* MainResource = new MainFrame(0L);
		if ( MainResource ) MainResource->Show();
	}
	//*)

	return wxsOK;
}
