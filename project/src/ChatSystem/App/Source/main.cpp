#include <wx/frame.h>

#include "wx/app.h"

class ChatApp : public wxApp {
public:
    bool OnInit() override {
        wxFrame* frame = new wxFrame(nullptr, wxID_ANY, "ChatSystem",
            wxDefaultPosition, wxSize(800, 600));
        frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(ChatApp);
