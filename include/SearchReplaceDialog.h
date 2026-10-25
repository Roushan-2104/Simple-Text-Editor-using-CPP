#pragma once

#include <FL/Fl_Window.H>

#include <string>

class Fl_Button;
class Fl_Input;
class Fl_Return_Button;
class Fl_Text_Buffer;
class Fl_Text_Editor;
class Fl_Widget;

// Non-modal dialog with find and replace inputs. Searches the text shown in
// the editor it was created for. Searches are case-sensitive and wrap around
// to the start of the text.
class SearchReplaceDialog : public Fl_Window {
public:
    explicit SearchReplaceDialog(Fl_Text_Editor& editor);

    void showFind();     // show the dialog with the find field focused
    void showReplace();  // show the dialog with the replace field focused

    bool hasSearchText() const;

    bool findNext();
    void replaceNext();
    void replaceAll();

private:
    static void cb_findNext(Fl_Widget*, void* v)    { static_cast<SearchReplaceDialog*>(v)->findNext(); }
    static void cb_replaceNext(Fl_Widget*, void* v) { static_cast<SearchReplaceDialog*>(v)->replaceNext(); }
    static void cb_replaceAll(Fl_Widget*, void* v)  { static_cast<SearchReplaceDialog*>(v)->replaceAll(); }
    static void cb_close(Fl_Widget*, void* v)       { static_cast<SearchReplaceDialog*>(v)->hide(); }

    // Finds the search text from the cursor, wrapping to the start.
    // Returns false (after telling the user) if it is empty or not found.
    bool locate(const std::string& text, int& found);
    bool requireSearchText(std::string& text);
    void select(int start, int end);
    Fl_Text_Buffer* buffer() const;

    Fl_Text_Editor& editor_;
    Fl_Input* findInput_;
    Fl_Input* replaceInput_;
    Fl_Return_Button* findButton_;
    Fl_Button* replaceButton_;
    Fl_Button* replaceAllButton_;
    Fl_Button* closeButton_;
};
