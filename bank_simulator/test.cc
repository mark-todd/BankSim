#include <iostream>
#include <string>
#include <ncurses.h>

class Interface {
    public:
        virtual int on_key(int& ch) = 0;

        int operator()() {
            int ch = KEY_HOME;
            do {
                clear();
                int out = on_key(ch);
                if(out == -1) {
                    continue;
                } else{
                    return out;
                }
            } while (
                (ch = getch())
            );
            endwin();
            throw std::runtime_error("Exited loop without return!");
        }
};

class SelectInterface : public Interface {
    private:
        int selectedOption;
        int totalOptions;

    public:
        int on_key(int& ch) override {
            switch (ch) {
                case KEY_UP:
                    selectedOption = (selectedOption > 1) ? selectedOption - 1 : totalOptions;
                case KEY_DOWN:
                    selectedOption = (selectedOption < totalOptions) ? selectedOption + 1 : 1;
            }
            // Display options
            printw("Please select an account:\n");
            for (int i = 1; i <= totalOptions; i++) {
                std::string str = i == selectedOption ? "-> " : "   ";
                printw((str + "Account %d\n").c_str(), i);
            }
            switch (ch) {
                case 10:  // Enter key
                    return selectedOption;
                case 'q':
                    return 0; // quit
                default: // continue loop
                    return -1;
            }
        }

        SelectInterface(int totalOptions, int selectedOption = 1) : selectedOption(selectedOption), totalOptions(totalOptions) {}
};

int main() {
    initscr();  // Initialize ncurses
    cbreak();              // Line buffering disabled
    noecho();              // Don't display keypresses
    keypad(stdscr, TRUE);  // Enable function keys

    SelectInterface account_select(3);
    int out = account_select();  // Call the functor like a function
    if (out ==0){
        goto Terminate;
    }
    Terminate:
    endwin();
    return 0;
}
