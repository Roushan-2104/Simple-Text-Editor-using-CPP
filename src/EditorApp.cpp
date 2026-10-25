#include "EditorApp.h"

#include "EditorWindow.h"

#include <FL/Fl.H>

EditorApp::EditorApp()
    : window_(std::make_unique<EditorWindow>(800, 600)) {}

EditorApp::~EditorApp() = default;

int EditorApp::run(int /*argc*/, char** /*argv*/) {
    window_->show();
    return Fl::run();
}
