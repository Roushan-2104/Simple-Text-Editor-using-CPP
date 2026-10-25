#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>

int main(int argc, char** argv) {
    Fl_Double_Window window(800, 600, "Text Editor");
    window.end();
    window.show(argc, argv);
    return Fl::run();
}
