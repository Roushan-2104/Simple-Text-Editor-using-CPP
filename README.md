# Simple Text Editor using C++ and FLTK

A desktop text editor written in C++17 with the [FLTK](https://www.fltk.org/) GUI library and designed with object-oriented programming. This is the OOPs course project at IIIT Bhubaneswar (Roushan Sharma, CSE-B, B125103). The full project abstract is in `Text Editor in C++ using FLTK and OOP — Project Abstract.pdf`.

## Features

| Menu   | Item          | Shortcut         |
|--------|---------------|------------------|
| File   | New           | Ctrl+N           |
|        | Open...       | Ctrl+O           |
|        | Insert...     | Ctrl+I           |
|        | Save          | Ctrl+S           |
|        | Save As...    | Ctrl+Shift+S     |
|        | Exit          | Ctrl+Q           |
| Edit   | Undo          | Ctrl+Z           |
|        | Cut           | Ctrl+X           |
|        | Copy          | Ctrl+C           |
|        | Paste         | Ctrl+V           |
|        | Delete        | Del              |
| Search | Find...       | Ctrl+F           |
|        | Find Next     | Ctrl+G           |
|        | Replace...    | Ctrl+R           |
|        | Replace All   | Ctrl+Shift+R     |

- The title bar shows `(modified)` while there are unsaved changes.
- New, Open, Exit and the window's close button ask to **Save**, **Don't Save** or **Cancel** when there are unsaved changes.
- C/C++ syntax highlighting (comments, strings, preprocessor directives, data types, keywords) for `.c .cpp .cc .cxx .h .hpp .hh .hxx` files and new documents.
- Find and replace dialog: Find Next, Replace Next and Replace All. Searches are case-sensitive and wrap around to the start of the text.
- A file name passed on the command line is opened at start-up: `TextEditor.exe samples/sample.cpp`.

## Project layout

```
include/   one header per class
src/       one source file per class, plus main.cpp
samples/   sample.cpp for trying out highlighting and search
docs/      REPORT.md (project report)
Makefile   build with fltk-config
CMakeLists.txt
```

## Building

### Windows (MSYS2)

1. Install [MSYS2](https://www.msys2.org/), for example with `winget install MSYS2.MSYS2`.
2. In the **MSYS2 UCRT64** shell, install the compiler and FLTK:
   ```sh
   pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-fltk make
   ```
3. Build and run from the project folder:
   ```sh
   make
   ./TextEditor.exe
   ```
   The Makefile links FLTK statically, so `TextEditor.exe` also runs outside MSYS2.

### Linux

```sh
sudo apt install g++ make libfltk1.3-dev   # Debian/Ubuntu
make
./TextEditor
```

### CMake (any platform)

```sh
cmake -S . -B build/cmake
cmake --build build/cmake
```

The CMake build links FLTK the way your system's FLTK package provides it. On MSYS2 this is the FLTK DLL, so run the CMake build from the UCRT64 shell, or copy the DLL next to the exe.

## Class design

| Class                 | Inherits from        | Responsibility |
|-----------------------|----------------------|----------------|
| `EditorApp`           | –                    | Starts the program, owns the document and the window, runs the event loop |
| `EditorWindow`        | `Fl_Double_Window`   | Menu bar and `Fl_Text_Editor`; handles menu actions |
| `Document`            | –                    | Text buffer, file name and modified flag; builds the window title |
| `FileManager`         | –                    | Loads, inserts and saves files; reports read and write errors |
| `SearchReplaceDialog` | `Fl_Window`          | Find and replace inputs and buttons |
| `SyntaxHighlighter`   | – (abstract)         | Interface with a pure virtual `parse()` |
| `CppHighlighter`      | `SyntaxHighlighter`  | Styles C/C++ code |

See [docs/REPORT.md](docs/REPORT.md) for the full design, how the program works, and the test results.
