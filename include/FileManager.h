#pragma once

#include <string>

class Document;
class Fl_Text_Buffer;

// Loads and saves files. Every method returns false and fills `error` with
// a readable message when the file cannot be read or written.
class FileManager {
public:
    // Replaces the document's text with the file and takes its name.
    bool load(const std::string& path, Document& doc, std::string& error) const;

    // Inserts the file's contents into the buffer at `pos`.
    bool insert(const std::string& path, int pos, Fl_Text_Buffer& buffer, std::string& error) const;

    // Writes the document's text to the file and takes its name.
    bool save(const std::string& path, Document& doc, std::string& error) const;

private:
    static bool canOpen(const std::string& path, const char* mode, std::string& error);
};
