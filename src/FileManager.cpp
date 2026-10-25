#include "FileManager.h"

#include "Document.h"

#include <FL/Fl_Text_Buffer.H>
#include <FL/fl_utf8.h>

#include <cerrno>
#include <cstdio>
#include <cstring>

bool FileManager::load(const std::string& path, Document& doc, std::string& error) const {
    // Check first so a failed open leaves the current text untouched.
    if (!canOpen(path, "rb", error)) return false;

    doc.beginLoad();
    int result = doc.buffer()->loadfile(path.c_str());
    doc.endLoad();
    if (result != 0) {
        error = "Cannot read '" + path + "'.";
        return false;
    }
    doc.setFilename(path);
    doc.setModified(false);
    return true;
}

bool FileManager::insert(const std::string& path, int pos, Fl_Text_Buffer& buffer,
                         std::string& error) const {
    if (!canOpen(path, "rb", error)) return false;
    if (buffer.insertfile(path.c_str(), pos) != 0) {
        error = "Cannot read '" + path + "'.";
        return false;
    }
    return true;
}

bool FileManager::save(const std::string& path, Document& doc, std::string& error) const {
    if (!canOpen(path, "ab", error)) return false;  // "ab" tests writability without truncating
    if (doc.buffer()->savefile(path.c_str()) != 0) {
        error = "Cannot write '" + path + "'.";
        return false;
    }
    doc.setFilename(path);
    doc.setModified(false);
    return true;
}

bool FileManager::canOpen(const std::string& path, const char* mode, std::string& error) {
    FILE* fp = fl_fopen(path.c_str(), mode);
    if (!fp) {
        error = "Cannot open '" + path + "':\n" + std::strerror(errno);
        return false;
    }
    std::fclose(fp);
    return true;
}
