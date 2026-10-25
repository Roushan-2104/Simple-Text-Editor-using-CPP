#pragma once

#include <FL/Fl_Text_Display.H>

#include <string>

// Interface for a syntax highlighter (abstraction). The editor window only
// knows this class and calls parse() through a base-class pointer
// (polymorphism), so a highlighter for another language is added by writing
// a new subclass without changing the window.
//
// Styles are single characters 'A', 'B', 'C', ... that index styleTable().
// 'A' is always plain text.
class SyntaxHighlighter {
public:
    virtual ~SyntaxHighlighter() = default;

    // Fills style[i] with the style of text[i] for i in [0, length).
    // startStyle is the style of the character just before `text`
    // (or 'A' at the start of the buffer) so state such as an open block
    // comment can carry over from earlier lines.
    virtual void parse(const char* text, char* style, int length, char startStyle) const = 0;

    virtual const Fl_Text_Display::Style_Table_Entry* styleTable() const = 0;
    virtual int styleCount() const = 0;

    // True if this highlighter should be used for the given file name.
    virtual bool supports(const std::string& filename) const = 0;
};
