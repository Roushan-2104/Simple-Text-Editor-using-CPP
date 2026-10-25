#include "Document.h"

#include <FL/Fl_Text_Buffer.H>

Document::Document()
    : buffer_(new Fl_Text_Buffer()) {
    buffer_->add_modify_callback(modifyCallback, this);
}

Document::~Document() {
    buffer_->remove_modify_callback(modifyCallback, this);
    delete buffer_;
}

void Document::setModified(bool modified) {
    if (modified_ == modified) return;
    modified_ = modified;
    notify();
}

void Document::setFilename(const std::string& filename) {
    filename_ = filename;
    notify();
}

std::string Document::getTitle() const {
    std::string name = "Untitled";
    if (hasFilename()) {
        std::string::size_type slash = filename_.find_last_of("/\\");
        name = (slash == std::string::npos) ? filename_ : filename_.substr(slash + 1);
    }
    if (modified_) name += " (modified)";
    return name + " - Text Editor";
}

void Document::clear() {
    beginLoad();
    buffer_->text("");
    endLoad();
    filename_.clear();
    modified_ = false;
    notify();
}

void Document::beginLoad() {
    loading_ = true;
    buffer_->canUndo(0);
}

void Document::endLoad() {
    buffer_->canUndo(1);
    loading_ = false;
}

void Document::modifyCallback(int /*pos*/, int nInserted, int nDeleted, int /*nRestyled*/,
                              const char* /*deletedText*/, void* data) {
    static_cast<Document*>(data)->onTextModified(nInserted, nDeleted);
}

void Document::onTextModified(int nInserted, int nDeleted) {
    if (loading_) return;
    if (nInserted > 0 || nDeleted > 0) setModified(true);
}

void Document::notify() {
    if (listener_) listener_();
}
