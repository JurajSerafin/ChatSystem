#include <wx/frame.h>

#include "wx/app.h"

#include "tech_demo.h"

class ChatAppDemo : public wxApp {
public:
    bool OnInit() override {
        
        show_window();

        return true;
    }

    static void show_window()
    {
        wxFrame* frame = new wxFrame(nullptr, wxID_ANY, "ChatSystem",
            wxDefaultPosition, wxSize(800, 600));
        frame->Show(true);
    }
};

wxIMPLEMENT_APP(ChatAppDemo);
