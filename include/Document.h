#pragma once

#include <functional>
#include <string>

class Fl_Text_Buffer;

// Stores the text buffer, the file name and the modified flag, and builds
// the window title. All three are private; other classes go through the
// public methods only (encapsulation).
class Document {
public:
    Document();
    ~Document();

    Document(const Document&) = delete;
    Document& operator=(const Document&) = delete;

    Fl_Text_Buffer* buffer() const { return buffer_; }

    bool isModified() const { return modified_; }
    void setModified(bool modified);

    const std::string& filename() const { return filename_; }
    void setFilename(const std::string& filename);
    bool hasFilename() const { return !filename_.empty(); }

    // "name.cpp (modified) - Text Editor", or "Untitled - Text Editor".
    std::string getTitle() const;

    // Empties the text and forgets the file name (File > New).
    void clear();

    // Text changes between beginLoad() and endLoad() do not mark the
    // document as modified and cannot be undone.
    void beginLoad();
    void endLoad();

    // Called whenever the file name or the modified flag changes.
    void setChangeListener(std::function<void()> listener) { listener_ = std::move(listener); }

private:
    static void modifyCallback(int pos, int nInserted, int nDeleted, int nRestyled,
                               const char* deletedText, void* data);
    void onTextModified(int nInserted, int nDeleted);
    void notify();

    Fl_Text_Buffer* buffer_;
    std::string filename_;
    bool modified_ = false;
    bool loading_ = false;
    std::function<void()> listener_;
};
