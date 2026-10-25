#include "EditorWindow.h"

#include "Document.h"

#include <FL/Fl.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Text_Editor.H>

namespace {
const int kMenuHeight = 25;
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
}

void EditorWindow::updateTitle() {
    copy_label(doc_.getTitle().c_str());
}

void EditorWindow::onNew() {
    doc_.clear();
}

void EditorWindow::onOpen()   {}
void EditorWindow::onInsert() {}
void EditorWindow::onSave()   {}
void EditorWindow::onSaveAs() {}

void EditorWindow::onExit() {
    hide();
}
