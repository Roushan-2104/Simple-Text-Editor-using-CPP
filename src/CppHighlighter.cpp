#include "CppHighlighter.h"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <iterator>

namespace {

const Fl_Text_Display::Style_Table_Entry kStyles[] = {
    {FL_BLACK,        FL_COURIER,        14, 0, 0},  // A plain text
    {FL_DARK_GREEN,   FL_COURIER_ITALIC, 14, 0, 0},  // B line comment
    {FL_DARK_GREEN,   FL_COURIER_ITALIC, 14, 0, 0},  // C block comment
    {FL_DARK_RED,     FL_COURIER,        14, 0, 0},  // D string or char
    {FL_DARK_MAGENTA, FL_COURIER,        14, 0, 0},  // E preprocessor directive
    {FL_DARK_CYAN,    FL_COURIER_BOLD,   14, 0, 0},  // F data type
    {FL_DARK_BLUE,    FL_COURIER_BOLD,   14, 0, 0},  // G keyword
};

// Both lists must stay sorted for std::binary_search.
const char* const kKeywords[] = {
    "alignas", "alignof", "and", "asm", "break", "case", "catch", "class",
    "const_cast", "constexpr", "continue", "default", "delete", "do",
    "dynamic_cast", "else", "enum", "explicit", "export", "extern", "false",
    "final", "for", "friend", "goto", "if", "inline", "namespace", "new",
    "noexcept", "not", "nullptr", "operator", "or", "override", "private",
    "protected", "public", "reinterpret_cast", "return", "sizeof",
    "static_assert", "static_cast", "struct", "switch", "template", "this",
    "throw", "true", "try", "typedef", "typeid", "typename", "union", "using",
    "virtual", "while",
};

const char* const kTypes[] = {
    "auto", "bool", "char", "char16_t", "char32_t", "const", "double", "float",
    "int", "int16_t", "int32_t", "int64_t", "int8_t", "long", "map", "mutable",
    "register", "short", "signed", "size_t", "static", "std", "string",
    "uint16_t", "uint32_t", "uint64_t", "uint8_t", "unsigned", "vector", "void",
    "volatile", "wchar_t",
};

bool lessThan(const char* a, const char* b) { return std::strcmp(a, b) < 0; }

bool contains(const char* const* begin, const char* const* end, const std::string& word) {
    return std::binary_search(begin, end, word.c_str(), lessThan);
}

bool isWordStart(char c) { return std::isalpha(static_cast<unsigned char>(c)) || c == '_'; }
bool isWordChar(char c)  { return std::isalnum(static_cast<unsigned char>(c)) || c == '_'; }

}  // namespace

void CppHighlighter::parse(const char* text, char* style, int length, char startStyle) const {
    bool inBlockComment = (startStyle == 'C');
    bool atLineStart = true;  // only whitespace so far on this line
    int i = 0;

    while (i < length) {
        char c = text[i];
        char next = (i + 1 < length) ? text[i + 1] : '\0';

        if (inBlockComment) {
            if (c == '*' && next == '/') {
                style[i++] = 'C';
                inBlockComment = false;
            }
            style[i++] = 'C';
            continue;
        }

        if (c == '\n') {
            style[i++] = 'A';
            atLineStart = true;
            continue;
        }

        if (c == '/' && next == '/') {
            while (i < length && text[i] != '\n') style[i++] = 'B';
            continue;
        }

        if (c == '/' && next == '*') {
            style[i++] = 'C';
            style[i++] = 'C';
            inBlockComment = true;
            atLineStart = false;
            continue;
        }

        if (c == '"' || c == '\'') {
            // A string ends at its closing quote or, if unterminated, at the end of the line.
            style[i++] = 'D';
            while (i < length && text[i] != '\n') {
                char d = text[i];
                style[i++] = 'D';
                if (d == '\\' && i < length && text[i] != '\n') style[i++] = 'D';
                else if (d == c) break;
            }
            atLineStart = false;
            continue;
        }

        if (c == '#' && atLineStart) {
            // Directive runs to the end of the line or the start of a comment.
            while (i < length && text[i] != '\n') {
                if (text[i] == '/' && i + 1 < length && (text[i + 1] == '/' || text[i + 1] == '*')) break;
                style[i++] = 'E';
            }
            atLineStart = false;
            continue;
        }

        if (isWordStart(c)) {
            int start = i;
            while (i < length && isWordChar(text[i])) ++i;
            std::string word(text + start, i - start);
            char s = isKeyword(word) ? 'G' : isType(word) ? 'F' : 'A';
            std::memset(style + start, s, i - start);
            atLineStart = false;
            continue;
        }

        if (isWordChar(c)) {
            // A number such as 0x1f or 10u: keep its letters out of word matching.
            while (i < length && isWordChar(text[i])) style[i++] = 'A';
            atLineStart = false;
            continue;
        }

        if (c != ' ' && c != '\t' && c != '\r') atLineStart = false;
        style[i++] = 'A';
    }
}

const Fl_Text_Display::Style_Table_Entry* CppHighlighter::styleTable() const {
    return kStyles;
}

int CppHighlighter::styleCount() const {
    return static_cast<int>(std::size(kStyles));
}

bool CppHighlighter::supports(const std::string& filename) const {
    if (filename.empty()) return true;  // new documents are highlighted as C++

    std::string::size_type dot = filename.find_last_of('.');
    std::string::size_type slash = filename.find_last_of("/\\");
    if (dot == std::string::npos || (slash != std::string::npos && dot < slash)) return false;

    std::string ext = filename.substr(dot + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(),
                   [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });

    static const char* const kExtensions[] = {"c", "cc", "cpp", "cxx", "h", "hh", "hpp", "hxx"};
    return contains(std::begin(kExtensions), std::end(kExtensions), ext);
}

bool CppHighlighter::isKeyword(const std::string& word) {
    return contains(std::begin(kKeywords), std::end(kKeywords), word);
}

bool CppHighlighter::isType(const std::string& word) {
    return contains(std::begin(kTypes), std::end(kTypes), word);
}
