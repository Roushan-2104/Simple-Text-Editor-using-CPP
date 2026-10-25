#include "EditorWindow.h"

#include "CppHighlighter.h"
#include "Document.h"
#include "SearchReplaceDialog.h"

#include <FL/Fl.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/fl_ask.H>

#include <algorithm>
#include <cstdlib>

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

    searchDialog_ = std::make_unique<SearchReplaceDialog>(*editor_);

    // The window holds the highlighter through a base-class pointer, so a
    // different language only needs a different SyntaxHighlighter subclass.
    highlighter_ = std::make_unique<CppHighlighter>();
    styleBuffer_ = std::make_unique<Fl_Text_Buffer>();
    editor_->highlight_data(styleBuffer_.get(), highlighter_->styleTable(),
                            highlighter_->styleCount(), 'A', nullptr, nullptr);
    doc_.buffer()->add_modify_callback(textModifiedCallback, this);

    doc_.setChangeListener([this] { onDocumentChanged(); });
    onDocumentChanged();
}

EditorWindow::~EditorWindow() {
    doc_.setChangeListener(nullptr);
    doc_.buffer()->remove_modify_callback(textModifiedCallback, this);
    searchDialog_.reset();
    clear();  // delete the editor while the style buffer it uses still exists
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

    menuBar_->add("&Search/&Find...",     FL_COMMAND + 'f',            cb_find,       this);
    menuBar_->add("&Search/Find &Next",   FL_COMMAND + 'g',            cb_findNext,   this, FL_MENU_DIVIDER);
    menuBar_->add("&Search/&Replace...",  FL_COMMAND + 'r',            cb_replace,    this);
    menuBar_->add("&Search/Replace &All", FL_COMMAND + FL_SHIFT + 'r', cb_replaceAll, this);
}

void EditorWindow::onDocumentChanged() {
    updateTitle();
    updateHighlighting();
}

void EditorWindow::updateTitle() {
    copy_label(doc_.getTitle().c_str());
}

void EditorWindow::textModifiedCallback(int pos, int nInserted, int nDeleted, int /*nRestyled*/,
                                        const char* /*deletedText*/, void* data) {
    static_cast<EditorWindow*>(data)->onTextModified(pos, nInserted, nDeleted);
}

void EditorWindow::onTextModified(int pos, int nInserted, int nDeleted) {
    if (nInserted == 0 && nDeleted == 0) return;  // selection change only

    // Keep the style buffer the same length as the text buffer.
    if (nInserted > 0) {
        std::string plain(nInserted, 'A');
        styleBuffer_->replace(pos, pos + nDeleted, plain.c_str());
    } else {
        styleBuffer_->remove(pos, pos + nDeleted);
    }
    if (!highlighting_) return;

    // Re-parse the changed lines, including the final newline. If its style
    // changed, a block comment was opened or closed, so the rest of the
    // text has to be re-parsed too.
    Fl_Text_Buffer* text = doc_.buffer();
    int start = text->line_start(pos);
    int end = std::min(text->line_end(pos + nInserted) + 1, text->length());
    if (restyle(start, end)) restyle(end, text->length());
}

void EditorWindow::updateHighlighting() {
    bool on = highlighter_->supports(doc_.filename());
    if (on == highlighting_) return;
    highlighting_ = on;
    restyleAll();
}

void EditorWindow::restyleAll() {
    int length = doc_.buffer()->length();
    std::string styles(length, 'A');
    if (highlighting_ && length > 0) {
        char* text = doc_.buffer()->text();
        highlighter_->parse(text, &styles[0], length, 'A');
        std::free(text);
    }
    styleBuffer_->text(styles.c_str());
    editor_->redisplay_range(0, length);
}

bool EditorWindow::restyle(int start, int end) {
    if (start >= end) return false;

    char* text = doc_.buffer()->text_range(start, end);
    char* style = styleBuffer_->text_range(start, end);
    char lastBefore = style[end - start - 1];
    char startStyle = (start > 0) ? styleBuffer_->byte_at(start - 1) : 'A';

    highlighter_->parse(text, style, end - start, startStyle);  // polymorphic call
    bool lastChanged = (style[end - start - 1] != lastBefore);

    styleBuffer_->replace(start, end, style);
    editor_->redisplay_range(start, end);
    std::free(text);
    std::free(style);
    return lastChanged;
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
    searchDialog_->hide();  // Fl::run() returns once no window is shown
    hide();
}

// The Edit actions reuse Fl_Text_Editor's own key functions so menu items
// and keyboard shortcuts behave the same way.
void EditorWindow::onUndo()   { Fl_Text_Editor::kf_undo(0, editor_); }
void EditorWindow::onCut()    { Fl_Text_Editor::kf_cut(0, editor_); }
void EditorWindow::onCopy()   { Fl_Text_Editor::kf_copy(0, editor_); }
void EditorWindow::onPaste()  { Fl_Text_Editor::kf_paste(0, editor_); }
void EditorWindow::onDelete() { Fl_Text_Editor::kf_delete(0, editor_); }

void EditorWindow::onFind() {
    searchDialog_->showFind();
}

void EditorWindow::onFindNext() {
    if (searchDialog_->hasSearchText()) searchDialog_->findNext();
    else searchDialog_->showFind();
}

void EditorWindow::onReplace() {
    searchDialog_->showReplace();
}

void EditorWindow::onReplaceAll() {
    if (searchDialog_->hasSearchText()) searchDialog_->replaceAll();
    else searchDialog_->showReplace();
}
