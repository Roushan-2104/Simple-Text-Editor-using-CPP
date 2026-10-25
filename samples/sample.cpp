// sample.cpp - open this file to check syntax highlighting.
#include <iostream>
#include <string>
#define GREETING "Hello"

/* A block comment
   that spans several lines. */
class Counter {
public:
    explicit Counter(int start) : value_(start) {}
    void increment() { ++value_; }
    int value() const { return value_; }

private:
    int value_;
};

int main() {
    Counter counter(0);
    for (int i = 0; i < 3; ++i) counter.increment();

    std::string name = "world";
    char quote = '\'';
    double ratio = 0x1f / 2.0;  // numbers stay plain

    if (counter.value() == 3) {
        std::cout << GREETING << ", " << name << quote << "!\n";
    }
    return 0;
}
