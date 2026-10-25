#include "EditorApp.h"

#include "EditorWindow.h"

#include <FL/Fl.H>

EditorApp::EditorApp()
    : window_(std::make_unique<EditorWindow>(800, 600, document_)) {}

EditorApp::~EditorApp() = default;

int EditorApp::run(int argc, char** argv) {
    // An optional file name on the command line is opened at start-up.
    if (argc > 1) window_->openFile(argv[1]);
    window_->show();
    return Fl::run();
}
