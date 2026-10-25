#pragma once

#include <FL/Fl_Double_Window.H>

class Fl_Menu_Bar;
class Fl_Text_Editor;
class Fl_Text_Buffer;
class Fl_Widget;

// Main window: holds the menu bar and the text editor widget and handles
// every menu action. FLTK callbacks must be static, so each one is a static
// member that casts the void* user data back to the window and calls a
// normal member function.
class EditorWindow : public Fl_Double_Window {
public:
    EditorWindow(int w, int h);
    ~EditorWindow() override;

private:
    void buildMenu();

    // File menu
    static void cb_new(Fl_Widget*, void* v)    { static_cast<EditorWindow*>(v)->onNew(); }
    static void cb_open(Fl_Widget*, void* v)   { static_cast<EditorWindow*>(v)->onOpen(); }
    static void cb_insert(Fl_Widget*, void* v) { static_cast<EditorWindow*>(v)->onInsert(); }
    static void cb_save(Fl_Widget*, void* v)   { static_cast<EditorWindow*>(v)->onSave(); }
    static void cb_saveAs(Fl_Widget*, void* v) { static_cast<EditorWindow*>(v)->onSaveAs(); }
    static void cb_exit(Fl_Widget*, void* v)   { static_cast<EditorWindow*>(v)->onExit(); }

    void onNew();
    void onOpen();
    void onInsert();
    void onSave();
    void onSaveAs();
    void onExit();

    Fl_Menu_Bar* menuBar_;
    Fl_Text_Editor* editor_;
    Fl_Text_Buffer* textBuffer_;
};
