#include "EditorWindow.h"

#include "Document.h"

#include <FL/Fl.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/fl_ask.H>

namespace {
const int kMenuHeight = 25;
const char* kFileFilter =
    "Text and C/C++ Files\t*.{txt,c,cpp,cc,cxx,h,hpp,hh}\n"
    "All Files\t*";
}

EditorWindow::EditorWindow(int w, int h, Document& doc)
    : Fl_Double_Window(w, h), doc_(doc) {
    menuBar_ = new Fl_Menu_Bar(0, 0, w, kMenuHeight);
    buildMenu();

    editor_ = new Fl_Text_Editor(0, kMenuHeight, w, h - kMenuHeight);
    editor_->buffer(doc_.buffer());
    editor_->textfont(FL_COURIER);
    editor_->textsize(14);
    end();

    resizable(editor_);
    callback(cb_exit, this);  // window close button behaves like File > Exit

    doc_.setChangeListener([this] { updateTitle(); });
    updateTitle();
}

EditorWindow::~EditorWindow() {
    doc_.setChangeListener(nullptr);
}

void EditorWindow::buildMenu() {
    menuBar_->add("&File/&New",        FL_COMMAND + 'n',            cb_new,    this);
    menuBar_->add("&File/&Open...",    FL_COMMAND + 'o',            cb_open,   this);
    menuBar_->add("&File/&Insert...",  FL_COMMAND + 'i',            cb_insert, this, FL_MENU_DIVIDER);
    menuBar_->add("&File/&Save",       FL_COMMAND + 's',            cb_save,   this);
    menuBar_->add("&File/Save &As...", FL_COMMAND + FL_SHIFT + 's', cb_saveAs, this, FL_MENU_DIVIDER);
    menuBar_->add("&File/E&xit",       FL_COMMAND + 'q',            cb_exit,   this);

    menuBar_->add("&Edit/&Undo",       FL_COMMAND + 'z',            cb_undo,   this, FL_MENU_DIVIDER);
    menuBar_->add("&Edit/Cu&t",        FL_COMMAND + 'x',            cb_cut,    this);
    menuBar_->add("&Edit/&Copy",       FL_COMMAND + 'c',            cb_copy,   this);
    menuBar_->add("&Edit/&Paste",      FL_COMMAND + 'v',            cb_paste,  this);
    menuBar_->add("&Edit/&Delete",     FL_Delete,                   cb_delete, this);
}

void EditorWindow::updateTitle() {
    copy_label(doc_.getTitle().c_str());
}

bool EditorWindow::openFile(const std::string& path) {
    std::string error;
    if (!files_.load(path, doc_, error)) {
        fl_alert("%s", error.c_str());
        return false;
    }
    editor_->insert_position(0);
    editor_->show_insert_position();
    return true;
}

bool EditorWindow::checkSave() {
    if (!doc_.isModified()) return true;

    int choice = fl_choice("The current file has unsaved changes.\n"
                           "Would you like to save them?",
                           "Cancel", "Save", "Don't Save");
    if (choice == 1) return save();  // false if the save was cancelled or failed
    return choice == 2;              // 0 (Cancel or Escape) keeps the text
}

bool EditorWindow::save() {
    if (!doc_.hasFilename()) return saveAs();
    return saveTo(doc_.filename());
}

bool EditorWindow::saveAs() {
    std::string path = chooseFile("Save File As", true);
    if (path.empty()) return false;
    return saveTo(path);
}

bool EditorWindow::saveTo(const std::string& path) {
    std::string error;
    if (!files_.save(path, doc_, error)) {
        fl_alert("%s", error.c_str());
        return false;
    }
    return true;
}

std::string EditorWindow::chooseFile(const char* title, bool forSaving) {
    Fl_Native_File_Chooser chooser;
    chooser.title(title);
    chooser.filter(kFileFilter);
    if (forSaving) {
        chooser.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
        chooser.options(Fl_Native_File_Chooser::SAVEAS_CONFIRM | Fl_Native_File_Chooser::NEW_FOLDER);
        if (doc_.hasFilename()) chooser.preset_file(doc_.filename().c_str());
    } else {
        chooser.type(Fl_Native_File_Chooser::BROWSE_FILE);
    }

    switch (chooser.show()) {
        case 0:  return chooser.filename();
        case -1: fl_alert("File dialog error:\n%s", chooser.errmsg()); return "";
        default: return "";  // cancelled
    }
}

void EditorWindow::onNew() {
    if (!checkSave()) return;
    doc_.clear();
}

void EditorWindow::onOpen() {
    if (!checkSave()) return;
    std::string path = chooseFile("Open File", false);
    if (!path.empty()) openFile(path);
}

void EditorWindow::onInsert() {
    std::string path = chooseFile("Insert File", false);
    if (path.empty()) return;

    std::string error;
    if (!files_.insert(path, editor_->insert_position(), *doc_.buffer(), error))
        fl_alert("%s", error.c_str());
}

void EditorWindow::onSave() {
    save();
}

void EditorWindow::onSaveAs() {
    saveAs();
}

void EditorWindow::onExit() {
    if (!checkSave()) return;
    hide();
}

// The Edit actions reuse Fl_Text_Editor's own key functions so menu items
// and keyboard shortcuts behave the same way.
void EditorWindow::onUndo()   { Fl_Text_Editor::kf_undo(0, editor_); }
void EditorWindow::onCut()    { Fl_Text_Editor::kf_cut(0, editor_); }
void EditorWindow::onCopy()   { Fl_Text_Editor::kf_copy(0, editor_); }
void EditorWindow::onPaste()  { Fl_Text_Editor::kf_paste(0, editor_); }
void EditorWindow::onDelete() { Fl_Text_Editor::kf_delete(0, editor_); }
