#include <ncurses.h>

#include <iostream>
#include <string>
#include <vector>

class Interface {
  public:
    virtual int on_key(int &ch) = 0;

    int operator()() {
        int ch = KEY_HOME;
        do {
            clear();
            int out = on_key(ch);
            if (out == -1) {
                continue;
            } else {
                return out;
            }
        } while ((ch = getch()));
        endwin();
        throw std::runtime_error("Exited loop without return!");
    }
};

class SelectInterface : public Interface {
  private:
    int selectedOption;
    int totalOptions;
    std::vector<std::string> stringOptions;
    bool num_mode;

  public:
    int on_key(int &ch) override {
        int numOptions = num_mode ? totalOptions : stringOptions.size();
        
        switch (ch) {
        case KEY_UP:
            selectedOption =
                (selectedOption > 0) ? selectedOption - 1 : numOptions-1;
            break;
        case KEY_DOWN:
            selectedOption =
                (selectedOption < numOptions - 1) ? selectedOption + 1 : 0;
            break;
        }
        // Display options
        printw("Please select an account:\n");
        for (int i = 0; i < numOptions; i++) {
            std::string str = i == selectedOption ? "-> " : "   ";
            if (num_mode) {
                printw((str + "Account %d\n").c_str(), i + 1);
            } else {
                printw((str + stringOptions[i] + "\n").c_str(), i);
            }
            
        }
        switch (ch) {
        case 10: // Enter key
            return selectedOption;
        case 'q':
            return 0; // quit
        default:      // continue loop
            return -1;
        }
    }

    SelectInterface(int options, int selectedOption = 1)
        : selectedOption(selectedOption), totalOptions(options), num_mode(true) {}

    SelectInterface(std::vector<std::string> options, int selectedOption = 1)
        : selectedOption(selectedOption), stringOptions(options), num_mode(false) {}

};

int main() {
    initscr();            // Initialize ncurses
    cbreak();             // Line buffering disabled
    noecho();             // Don't display keypresses
    keypad(stdscr, TRUE); // Enable function keys
    std::vector<std::string> options = {"Opt1", "Opt2"};
    SelectInterface account_select(3);
    int out = account_select(); // Call the functor like a function
    if (out == 0) {
        goto Terminate;
    }
Terminate:
    endwin();
    return 0;
}
