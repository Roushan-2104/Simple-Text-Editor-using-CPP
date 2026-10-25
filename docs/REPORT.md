# Project Report: Text Editor in C++ using FLTK and OOP

**Name:** Roushan Sharma  **Branch:** CSE-B  **Roll No.:** B125103
**Subject:** Object-Oriented Programming (OOPs)  **Institute:** IIIT Bhubaneswar

## 1. Introduction

This project is a desktop text editor written in C++17 with the FLTK (Fast Light Toolkit) GUI library. Users can create, open, insert, edit and save plain text files through a menu bar and keyboard shortcuts. The editor supports cut, copy, paste, delete and undo, and find, find next, replace next and replace all. It tracks unsaved changes, shows "(modified)" in the title bar, asks the user to save before discarding changes, and highlights C/C++ source code.

A small editor like this could be written with global variables for the text buffer, the file name and the modified flag, plus free C-style callback functions. Every function would then depend on shared global state. Instead, this project puts each piece of data and behaviour inside a class with one responsibility.

## 2. Tools and environment

| Item       | Choice |
|------------|--------|
| Language   | C++17 |
| GUI library | FLTK 1.4.5 (`Fl_Double_Window`, `Fl_Menu_Bar`, `Fl_Text_Editor`, `Fl_Text_Buffer`, `Fl_Native_File_Chooser`) |
| Compiler   | GCC 16.2 (MSYS2 UCRT64) |
| Build      | `Makefile` with `fltk-config`, and `CMakeLists.txt` |
| Version control | Git, one commit per development phase |
| Platform   | Windows 11 (the code also builds on Linux) |

## 3. Object-oriented design

| Class | Inherits from | Responsibility | Files |
|-------|---------------|----------------|-------|
| `EditorApp` | – | Owns the `Document` and the `EditorWindow`, opens a command-line file, runs `Fl::run()` | `EditorApp.h/.cpp` |
| `EditorWindow` | `Fl_Double_Window` | Builds the menu bar and editor widget, handles every menu action, asks to save unsaved changes, keeps the style buffer up to date | `EditorWindow.h/.cpp` |
| `Document` | – | Owns the `Fl_Text_Buffer`, file name and modified flag; builds the title; notifies a listener when the name or modified state changes | `Document.h/.cpp` |
| `FileManager` | – | Loads, inserts and saves files and turns failures into readable error messages | `FileManager.h/.cpp` |
| `SearchReplaceDialog` | `Fl_Window` | Find and replace inputs; Find Next, Replace Next, Replace All, Close | `SearchReplaceDialog.h/.cpp` |
| `SyntaxHighlighter` | – (abstract) | Interface: pure virtual `parse()`, `styleTable()`, `styleCount()`, `supports()` | `SyntaxHighlighter.h` |
| `CppHighlighter` | `SyntaxHighlighter` | Styles C/C++ comments, strings, directives, types and keywords | `CppHighlighter.h/.cpp` |

### Ownership

```
EditorApp
 ├── Document                    (declared first, so it is destroyed last)
 │    └── Fl_Text_Buffer         (the text)
 └── EditorWindow : Fl_Double_Window
      ├── Fl_Menu_Bar
      ├── Fl_Text_Editor         (shows Document's buffer)
      ├── FileManager
      ├── SearchReplaceDialog : Fl_Window
      ├── SyntaxHighlighter*  →  CppHighlighter
      └── Fl_Text_Buffer         (style buffer)
```

### The four pillars of OOP

- **Encapsulation.** `Document` keeps the buffer, file name and modified flag private. Other classes use only `isModified()`, `setModified()`, `setFilename()`, `getTitle()`, `clear()` and so on. The modified flag is set from inside `Document` by a modify callback on its own buffer, so no other class can forget to set it.
- **Inheritance.** `EditorWindow` extends `Fl_Double_Window`, `SearchReplaceDialog` extends `Fl_Window`, and `CppHighlighter` extends `SyntaxHighlighter`.
- **Abstraction.** `SyntaxHighlighter` states *what* a highlighter does (parse text into styles, give a style table, say which files it handles) without saying *how*. `EditorWindow` depends only on this interface.
- **Polymorphism.** `EditorWindow` holds a `std::unique_ptr<SyntaxHighlighter>` and calls `highlighter_->parse(...)` and `highlighter_->supports(...)` through it. These are virtual calls that run `CppHighlighter`'s code. A Python or Java highlighter would be a new subclass, and the window would not change.

### Design challenge: static FLTK callbacks

FLTK callbacks must be plain functions with the signature `void (Fl_Widget*, void*)`. Each class uses a static member function that casts the `void*` user data back to the object and calls a normal member function, so all the logic stays inside the class:

```cpp
static void cb_save(Fl_Widget*, void* v) { static_cast<EditorWindow*>(v)->onSave(); }
...
menuBar_->add("&File/&Save", FL_COMMAND + 's', cb_save, this);
```

The same pattern is used for the text buffer's modify callbacks in `Document` and `EditorWindow`, and for the buttons in `SearchReplaceDialog`.

## 4. How the program works

1. `main()` creates an `EditorApp` and calls `run()`.
2. `EditorApp` creates the `Document` and then the `EditorWindow`. If a file name was given on the command line, it is opened. The window is shown and `Fl::run()` waits for input.
3. Each menu choice or shortcut runs a static callback, which calls a member function of `EditorWindow`.
4. Any change to the text calls `Document`'s modify callback, which sets the modified flag and tells the window to update the title to "(modified)".
5. Before **New**, **Open**, **Exit** or the window's close button, `checkSave()` asks *Save / Don't Save / Cancel* when there are unsaved changes. **Save** saves first (and stops if saving fails or is cancelled), **Don't Save** continues, and **Cancel** leaves everything as it was.

### Syntax highlighting

`Fl_Text_Display` can draw text using a second *style buffer* that holds one style letter per character ('A' plain, 'B' line comment, 'C' block comment, 'D' string, 'E' directive, 'F' type, 'G' keyword). The style letters index the style table that `CppHighlighter` provides.

When the text changes, `EditorWindow::onTextModified()` first inserts or removes the same number of characters in the style buffer, so both buffers stay the same length. It then re-parses only the changed lines. Each call to `parse()` gets the style of the character just before the range, so it knows whether it starts inside a `/* ... */` comment. If the style of the last newline in the range changed, a block comment was opened or closed, and the rest of the text is re-parsed as well. Highlighting is switched on only for files whose extension `CppHighlighter::supports()`, and for new documents.

### Search and replace

`SearchReplaceDialog` is non-modal, so the user can keep editing while it is open. **Find Next** searches forward from the cursor and wraps to the start; the match is selected and scrolled into view. **Replace Next** replaces the selected match (or finds the next one first) and selects the following match. **Replace All** replaces every match from the start and reports how many it replaced. An empty search box gives a warning.

## 5. Development phases

Each phase ended with a program that compiled and ran, and was committed to Git.

| Phase | Work | Commit |
|-------|------|--------|
| 1. Setup | Toolchain, Makefile, CMake, blank window | `Phase 1: Setup` |
| 2. Main window | `EditorApp`, `EditorWindow`, menu bar, text widget | `Phase 2: Main window` |
| 3. Document | `Document`, modified flag, title | `Phase 3: Document class` |
| 4. File handling | `FileManager`, New/Open/Insert/Save/Save As, save prompt | `Phase 4: File handling` |
| 5. Edit features | Cut, copy, paste, delete, undo | `Phase 5: Edit features` |
| 6. Search | `SearchReplaceDialog` | `Phase 6: Search` |
| 7. Highlighting | `SyntaxHighlighter`, `CppHighlighter`, style buffer | `Phase 7: Highlighting` |
| 8. Test and report | Testing, Escape-key fix, README, this report | `Phase 8: Test and report` |

## 6. Testing

The editor was run on Windows 11 and driven with simulated keystrokes, mouse clicks and screenshots. The results:

| # | Test | Result |
|---|------|--------|
| 1 | Start with no file: title is "Untitled - Text Editor" | Pass |
| 2 | Typing changes the title to "(modified)"; New (Ctrl+N) clears it | Pass |
| 3 | Open a file from the command line: title shows the file name | Pass |
| 4 | Edit, then Save (Ctrl+S and File > Save): file is written, "(modified)" removed | Pass |
| 5 | Open a file that does not exist: "Cannot open … No such file or directory" alert, editor stays usable | Pass |
| 6 | Exit (Ctrl+Q) with unsaved changes: Save / Don't Save / Cancel prompt | Pass |
| 7 | Close button with unsaved changes → Cancel: window stays open, text kept | Pass |
| 8 | Close button → Don't Save: program exits, file unchanged | Pass |
| 9 | Close button → Save: file saved, program exits | Pass |
| 10 | Escape key in the editor does not trigger the exit prompt (found in testing, fixed) | Pass |
| 11 | Edit menu: cut, paste twice, delete, undo give the expected text | Pass |
| 12 | Replace All "cat" → "CAT" in "cat dog cat bird cat": 3 replacements | Pass |
| 13 | Replace Next with the cursor at the end wraps to the first match, then the second | Pass |
| 14 | Find Next with an empty search box: "Please enter the text to find." | Pass |
| 15 | `samples/sample.cpp`: comments, strings, directives, types and keywords highlighted | Pass |
| 16 | Deleting `/*` restyles the following line as code; typing it again restores the comment | Pass |
| 17 | A `.txt` file is not highlighted | Pass |
| 18 | `make` build compiles with no warnings (`-Wall -Wextra`); CMake build also compiles and links | Pass |

Not tested automatically (needs clicking through the native file dialog): Save As to a read-only folder. `FileManager::save()` checks that the file can be opened for writing first and reports the system error message, the same path as test 5.

## 7. Conclusion

The editor meets all seven objectives in the project abstract: a File/Edit/Search menu bar with shortcuts; new, open, insert, save and save as; cut, copy, paste, delete and undo; a find and replace dialog; unsaved-change tracking and prompts; C/C++ syntax highlighting with a separate style buffer; and a class structure that shows encapsulation, inheritance, abstraction and polymorphism. Because the code is split into classes with one responsibility each, the parts can be changed independently. For example, file handling could change without touching the window, and a new highlighter could be added without touching either.

## 8. Future enhancements

- Multiple documents in tabs
- Line numbers and a status bar showing the cursor position
- Highlighters for more languages (Python, Java) as new `SyntaxHighlighter` subclasses
- Case-insensitive and whole-word search options (search is currently always case-sensitive)
- Recent files list and auto-save
