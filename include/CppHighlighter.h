#pragma once

#include "SyntaxHighlighter.h"

// Highlights C and C++ source code:
//   A plain text      B line comment      C block comment
//   D string or char  E preprocessor directive
//   F data type       G keyword
class CppHighlighter : public SyntaxHighlighter {
public:
    void parse(const char* text, char* style, int length, char startStyle) const override;

    const Fl_Text_Display::Style_Table_Entry* styleTable() const override;
    int styleCount() const override;

    // .c .cpp .cc .cxx .h .hpp .hh .hxx files, and new unsaved documents.
    bool supports(const std::string& filename) const override;

private:
    static bool isKeyword(const std::string& word);
    static bool isType(const std::string& word);
};
