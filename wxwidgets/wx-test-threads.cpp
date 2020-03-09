#include <thread>
#include <wx/wx.h>
#include <wx/sizer.h>

#include "elem_plot_utils.h"

DECLARE_EVENT_TYPE(wxEVT_ELEM_UPDATE, -1)

DEFINE_EVENT_TYPE(wxEVT_ELEM_UPDATE)

std::thread *worker_thread = nullptr;

class BasicDrawPane : public wxPanel
{
public:
    BasicDrawPane(wxFrame* parent);
    
    void paintEvent(wxPaintEvent & evt);
    void onElemUpdate(wxCommandEvent& evt);
    void render(wxDC& dc);
   
    DECLARE_EVENT_TABLE()
private:
    int paint_x, paint_y;
};


class MyApp: public wxApp
{
    bool OnInit();
    
    wxFrame *frame;
    BasicDrawPane * drawPane;
};

IMPLEMENT_APP(MyApp)

void WorkerThreadStart(BasicDrawPane *panel) {
    elem::utils::Sleep(3);
    for (int i = 0; i < 40; i++) {
        elem::utils::Sleep(1);
        wxCommandEvent event(wxEVT_ELEM_UPDATE, wxID_ANY);
        panel->GetEventHandler()->AddPendingEvent(event);
    }
}

bool MyApp::OnInit()
{
    wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    frame = new wxFrame((wxFrame *)NULL, -1,  wxT("Hello wxDC"), wxPoint(50,50), wxSize(800,600));
    
    drawPane = new BasicDrawPane( (wxFrame*) frame );
    sizer->Add(drawPane, 1, wxEXPAND);
    
    frame->SetSizer(sizer);
    frame->SetAutoLayout(true);

    frame->Show();

    worker_thread = new std::thread{WorkerThreadStart, drawPane};
    return true;
} 

BEGIN_EVENT_TABLE(BasicDrawPane, wxPanel)
EVT_COMMAND(wxID_ANY, wxEVT_ELEM_UPDATE, BasicDrawPane::onElemUpdate)
EVT_PAINT(BasicDrawPane::paintEvent)
END_EVENT_TABLE()

BasicDrawPane::BasicDrawPane(wxFrame* parent) :
wxPanel(parent), paint_x(100), paint_y(100)
{
}

void BasicDrawPane::paintEvent(wxPaintEvent & evt)
{
    wxPaintDC dc(this);
    render(dc);
}

void BasicDrawPane::onElemUpdate(wxCommandEvent& evt)
{
    wxClientDC dc(this);
    dc.SetBrush(*wxGREEN_BRUSH); // green filling
    dc.SetPen( wxPen( wxColor(255,0,0), 5 ) ); // 5-pixels-thick red outline
    dc.DrawCircle( wxPoint(paint_x, paint_y), 25 /* radius */ );
    paint_x += 50;
    if (paint_x > 800) {
        paint_x = 100;
        paint_y += 50;
    }
}

void BasicDrawPane::render(wxDC&  dc)
{
    // draw some text
    dc.DrawText(wxT("Testing"), 40, 60); 
    
    // draw a rectangle
    dc.SetBrush(*wxBLUE_BRUSH); // blue filling
    dc.SetPen( wxPen( wxColor(255,175,175), 10 ) ); // 10-pixels-thick pink outline
    dc.DrawRectangle( 300, 100, 400, 200 );
    
    // draw a line
    dc.SetPen( wxPen( wxColor(0,0,0), 3 ) ); // black line, 3 pixels thick
    dc.DrawLine( 300, 100, 700, 300 ); // draw line across the rectangle
    
    // Look at the wxDC docs to learn how to draw other stuff
}
