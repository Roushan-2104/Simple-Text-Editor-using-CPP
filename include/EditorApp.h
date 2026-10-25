#pragma once

#include <memory>

class EditorWindow;

// Starts the program: owns the document and the main window and runs
// FLTK's event loop.
class EditorApp {
public:
    EditorApp();
    ~EditorApp();

    int run(int argc, char** argv);

private:
    std::unique_ptr<EditorWindow> window_;
};
