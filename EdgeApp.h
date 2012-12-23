#ifndef _EDGEAPP_H_
#define _EDGEAPP_H_

class EdgeImageFrame : public wxFrame
{
	public:
		EdgeImageFrame(wxFrame *parent, const wxBitmap& bitmap, wxString title);
		void OnEraseBackground(wxEraseEvent& WXUNUSED(event));
		void OnPaint(wxPaintEvent& WXUNUSED(event));

	private:
		wxBitmap m_bitmap;
		DECLARE_EVENT_TABLE()
};

class EdgeAppCanvas: public wxScrolledWindow
{
	public:
		wxBitmap image_original;
		wxBitmap image_detected;

		EdgeAppCanvas(wxWindow *parent, wxWindowID, const wxPoint &pos, const wxSize &size);
		void OnPaint(wxPaintEvent &event);
		void loadImage(wxImage image);

	private:
		DECLARE_EVENT_TABLE()
};

class EdgeAppFrame: public wxFrame
{
	public:
		EdgeAppCanvas *ea_canvas;

		EdgeAppFrame();
		void OnOpenFile(wxCommandEvent &event);
		void OnAbout(wxCommandEvent &event);
		void OnQuit(wxCommandEvent &event);

	private:
		wxImage image;
		wxButton *WxButtonCanny;
		void WxButtonCannyClick(wxCommandEvent& event);

	private:
		DECLARE_DYNAMIC_CLASS(EdgeAppFrame)
		DECLARE_EVENT_TABLE()
};

class EdgeApp : public wxApp
{
	public:
		virtual bool OnInit();
};

DECLARE_APP(EdgeApp)

#endif // #ifndef _EDGEAPP_H_
