#pragma once

#include "FileManager.h"

#include <FL/Fl_Double_Window.H>

#include <memory>
#include <string>

class Document;
class SearchReplaceDialog;
class Fl_Menu_Bar;
class Fl_Text_Editor;
class Fl_Widget;

// Main window: holds the menu bar and the text editor widget and handles
// every menu action. FLTK callbacks must be static, so each one is a static
// member that casts the void* user data back to the window and calls a
// normal member function.
class EditorWindow : public Fl_Double_Window {
public:
    EditorWindow(int w, int h, Document& doc);
    ~EditorWindow() override;

    // Loads a file into the document; shows an error dialog on failure.
    bool openFile(const std::string& path);

private:
    void buildMenu();
    void updateTitle();

    // Asks to save unsaved changes. Returns false if the user cancels.
    bool checkSave();
    bool save();
    bool saveAs();
    bool saveTo(const std::string& path);
    // Shows a native file dialog; returns an empty string on cancel.
    std::string chooseFile(const char* title, bool forSaving);

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

    // Edit menu
    static void cb_undo(Fl_Widget*, void* v)   { static_cast<EditorWindow*>(v)->onUndo(); }
    static void cb_cut(Fl_Widget*, void* v)    { static_cast<EditorWindow*>(v)->onCut(); }
    static void cb_copy(Fl_Widget*, void* v)   { static_cast<EditorWindow*>(v)->onCopy(); }
    static void cb_paste(Fl_Widget*, void* v)  { static_cast<EditorWindow*>(v)->onPaste(); }
    static void cb_delete(Fl_Widget*, void* v) { static_cast<EditorWindow*>(v)->onDelete(); }

    void onUndo();
    void onCut();
    void onCopy();
    void onPaste();
    void onDelete();

    // Search menu
    static void cb_find(Fl_Widget*, void* v)       { static_cast<EditorWindow*>(v)->onFind(); }
    static void cb_findNext(Fl_Widget*, void* v)   { static_cast<EditorWindow*>(v)->onFindNext(); }
    static void cb_replace(Fl_Widget*, void* v)    { static_cast<EditorWindow*>(v)->onReplace(); }
    static void cb_replaceAll(Fl_Widget*, void* v) { static_cast<EditorWindow*>(v)->onReplaceAll(); }

    void onFind();
    void onFindNext();
    void onReplace();
    void onReplaceAll();

    Document& doc_;
    FileManager files_;
    Fl_Menu_Bar* menuBar_;
    Fl_Text_Editor* editor_;
    std::unique_ptr<SearchReplaceDialog> searchDialog_;
};
