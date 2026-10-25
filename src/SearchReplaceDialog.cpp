#include "SearchReplaceDialog.h"

#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/fl_ask.H>

#include <cstdlib>

SearchReplaceDialog::SearchReplaceDialog(Fl_Text_Editor& editor)
    : Fl_Window(430, 115, "Find and Replace"), editor_(editor) {
    findInput_    = new Fl_Input(85, 10, 335, 25, "Find:");
    replaceInput_ = new Fl_Input(85, 45, 335, 25, "Replace:");

    findButton_       = new Fl_Return_Button(10, 80, 100, 25, "Find Next");
    replaceButton_    = new Fl_Button(115, 80, 100, 25, "Replace Next");
    replaceAllButton_ = new Fl_Button(220, 80, 100, 25, "Replace All");
    closeButton_      = new Fl_Button(325, 80, 95, 25, "Close");
    end();

    findButton_->callback(cb_findNext, this);
    replaceButton_->callback(cb_replaceNext, this);
    replaceAllButton_->callback(cb_replaceAll, this);
    closeButton_->callback(cb_close, this);
    callback(cb_close, this);

    set_non_modal();
}

void SearchReplaceDialog::showFind() {
    show();
    findInput_->take_focus();
}

void SearchReplaceDialog::showReplace() {
    show();
    replaceInput_->take_focus();
}

bool SearchReplaceDialog::hasSearchText() const {
    return findInput_->size() > 0;
}

bool SearchReplaceDialog::findNext() {
    std::string text;
    if (!requireSearchText(text)) return false;

    int found;
    if (!locate(text, found)) return false;
    select(found, found + static_cast<int>(text.size()));
    return true;
}

void SearchReplaceDialog::replaceNext() {
    std::string text;
    if (!requireSearchText(text)) return;
    std::string replacement = replaceInput_->value();

    // Replace the current selection if it is a match, otherwise the next match.
    int start, end;
    bool selectedMatch = false;
    if (buffer()->selection_position(&start, &end)) {
        char* selected = buffer()->selection_text();
        selectedMatch = (text == selected);
        std::free(selected);
    }
    if (!selectedMatch) {
        if (!locate(text, start)) return;
    }

    buffer()->replace(start, start + static_cast<int>(text.size()), replacement.c_str());
    int after = start + static_cast<int>(replacement.size());
    buffer()->unselect();
    editor_.insert_position(after);
    editor_.show_insert_position();

    // Select the following match, if any, so the user can keep replacing.
    int next;
    if (buffer()->search_forward(after, text.c_str(), &next, 1) ||
        buffer()->search_forward(0, text.c_str(), &next, 1))
        select(next, next + static_cast<int>(text.size()));
}

void SearchReplaceDialog::replaceAll() {
    std::string text;
    if (!requireSearchText(text)) return;
    std::string replacement = replaceInput_->value();

    int pos = 0, found, count = 0;
    while (buffer()->search_forward(pos, text.c_str(), &found, 1)) {
        buffer()->replace(found, found + static_cast<int>(text.size()), replacement.c_str());
        pos = found + static_cast<int>(replacement.size());
        ++count;
    }

    if (count == 0) {
        fl_message("No occurrences of '%s' found.", text.c_str());
    } else {
        buffer()->unselect();
        editor_.insert_position(pos);
        editor_.show_insert_position();
        fl_message("Replaced %d occurrence%s.", count, count == 1 ? "" : "s");
    }
}

bool SearchReplaceDialog::locate(const std::string& text, int& found) {
    int start = editor_.insert_position();
    if (buffer()->search_forward(start, text.c_str(), &found, 1)) return true;
    if (start > 0 && buffer()->search_forward(0, text.c_str(), &found, 1)) return true;  // wrap

    fl_message("No occurrences of '%s' found.", text.c_str());
    return false;
}

bool SearchReplaceDialog::requireSearchText(std::string& text) {
    text = findInput_->value();
    if (text.empty()) {
        fl_alert("Please enter the text to find.");
        showFind();
        return false;
    }
    return true;
}

void SearchReplaceDialog::select(int start, int end) {
    buffer()->select(start, end);
    editor_.insert_position(end);
    editor_.show_insert_position();
}

Fl_Text_Buffer* SearchReplaceDialog::buffer() const {
    return editor_.buffer();
}
