/**
 * \file      EdgeApp.cpp
 * \brief     EdgeApp wxWidgets application file.
 * \details   This file is part of student project. Some parts of code may be
 *            influenced by various examples found on internet.
 * \author    resset <silentdemon@gmail.com>
 * \date      2006-2012
 * \copyright GNU General Public License, http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 */

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#   include <wx/wx.h>
#endif
#include <wx/image.h>

#include "EdgeApp.h"
#include "CannyEdgeDetector.h"

BEGIN_EVENT_TABLE(EdgeImageFrame, wxFrame)
	EVT_ERASE_BACKGROUND(EdgeImageFrame::OnEraseBackground)
	EVT_PAINT(EdgeImageFrame::OnPaint)
END_EVENT_TABLE()

EdgeImageFrame::EdgeImageFrame(wxFrame *parent, const wxBitmap& bitmap, wxString title)
	: wxFrame(parent, wxID_ANY, title,
	          wxDefaultPosition, wxDefaultSize,
	          wxCAPTION | wxSYSTEM_MENU | wxCLOSE_BOX), m_bitmap(bitmap)
{
	SetClientSize(bitmap.GetWidth(), bitmap.GetHeight());
}

void EdgeImageFrame::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
}

void EdgeImageFrame::OnPaint(wxPaintEvent& WXUNUSED(event))
{
	wxPaintDC dc(this);
	dc.DrawBitmap(m_bitmap, 0, 0, true);
}

BEGIN_EVENT_TABLE(EdgeAppCanvas, wxScrolledWindow)
	EVT_PAINT(EdgeAppCanvas::OnPaint)
END_EVENT_TABLE()

EdgeAppCanvas::EdgeAppCanvas(wxWindow *parent, wxWindowID id,
	                         const wxPoint &pos, const wxSize &size)
	: wxScrolledWindow(parent, id, pos, size, wxSUNKEN_BORDER)
{
	SetBackgroundColour(* wxWHITE);
}

void EdgeAppCanvas::OnPaint(wxPaintEvent &WXUNUSED(event))
{
}

void EdgeAppCanvas::loadImage(wxImage image)
{
	wxPaintDC dc(this);
	PrepareDC(dc);

	if (image.IsOk()) {
		dc.DrawBitmap(image.Rescale(295, 235), 0, 0);
	}
}

enum
{
	ID_QUIT  = wxID_EXIT,
	ID_ABOUT = wxID_ABOUT,
	ID_NEW   = 100,
	ID_WXBUTTON_CANNY = 1001
};

IMPLEMENT_DYNAMIC_CLASS(EdgeAppFrame, wxFrame)

BEGIN_EVENT_TABLE(EdgeAppFrame, wxFrame)
	EVT_MENU   (ID_ABOUT, EdgeAppFrame::OnAbout)
	EVT_MENU   (ID_QUIT, EdgeAppFrame::OnQuit)
	EVT_MENU   (ID_NEW, EdgeAppFrame::OnOpenFile)
	EVT_BUTTON (ID_WXBUTTON_CANNY, EdgeAppFrame::WxButtonCannyClick)
END_EVENT_TABLE()

EdgeAppFrame::EdgeAppFrame()
	: wxFrame((wxFrame *)NULL, wxID_ANY, _T("Edge detection app"),
	          wxPoint(20, 20), wxSize(400, 300))
{
	wxMenu *menuImage = new wxMenu;
	menuImage->Append(ID_NEW, _T("&Open image\tCtrl-O"));
	menuImage->Append(ID_ABOUT, _T("&About..."));
	menuImage->AppendSeparator();
	menuImage->Append(ID_QUIT, _T("&Quit\tCtrl-Q"));

	wxMenuBar *menu_bar = new wxMenuBar();
	menu_bar->Append(menuImage, _T("&File"));

	SetMenuBar(menu_bar);

	image = NULL;

	WxButtonCanny = new wxButton(this, ID_WXBUTTON_CANNY, wxT("Canny"), wxPoint(5, 5), wxSize(75, 25), 0, wxDefaultValidator, wxT("WxButtonCanny"));

	SetIcon(wxNullIcon);
	Center();

	ea_canvas = new EdgeAppCanvas(this, wxID_ANY, wxPoint(85, 0), wxSize(295, 235));
}

void EdgeAppFrame::OnOpenFile(wxCommandEvent &WXUNUSED(event))
{
	wxString filename = wxFileSelector(_T("Choose image file"));
	if (!filename) {
		return;
	}

	if (!image.LoadFile(filename)) {
		wxLogError(_T("Cannot open '%s'."), filename.c_str());
		return;
	}

	if (image.IsOk()) {
		ea_canvas->loadImage(image);
	}
}

void EdgeAppFrame::OnAbout(wxCommandEvent &WXUNUSED(event))
{
	(void) wxMessageBox(_T("EdgeApp - edge detection\n")
	                    _T("resset 2006 - 2012"),
	                    _T("About..."), wxICON_INFORMATION | wxOK);
}

void EdgeAppFrame::OnQuit(wxCommandEvent &WXUNUSED(event))
{
	Close(true);
}

void EdgeAppFrame::WxButtonCannyClick(wxCommandEvent& WXUNUSED(event))
{
	if (image.IsOk()) {
		CannyEdgeDetector *canny = new CannyEdgeDetector();

		(new EdgeImageFrame(this,
		                    wxBitmap(wxImage(image.GetWidth(),
		                                     image.GetHeight(),
		                                     canny->ProcessImage(image.GetData(),
		                                                         image.GetWidth(),
		                                                         image.GetHeight(),
		                                                         1.0f, 15, 21),
		                                     true)),
		                    _T("Canny"))
		)->Show();
	}
}

IMPLEMENT_APP(EdgeApp);

bool EdgeApp::OnInit()
{
	wxInitAllImageHandlers();

	wxFrame *frame = new EdgeAppFrame();
	frame->Show(true);

	return true;
}
