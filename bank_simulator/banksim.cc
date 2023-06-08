#include <ncurses.h>

#include <iostream>
#include <string>
#include <vector>

class Interface {
  public:
    virtual int on_key(int &ch) = 0;

    virtual void pre_key() {};

    virtual void post_key() {};

    int operator()() {
        int ch = KEY_HOME;
        do {
            clear();
            pre_key();
            int out = on_key(ch);
            post_key();
            if (out == -2) {
                continue;
            } else {
                return out;
            }
        } while ((ch = getch()));
        endwin();
        throw std::runtime_error("Exited loop without return!");
    }
};

class VerticalInterface : public Interface {
  private:
    int selectedOption;
    int totalOptions;
    std::vector<std::string> stringOptions;
    bool num_mode;
    std::string prefix;

  public:

    int on_key(int &ch) override {
        int numOptions = num_mode ? totalOptions : stringOptions.size();

        switch (ch) {
        case KEY_UP:
            selectedOption =
                (selectedOption > 0) ? selectedOption - 1 : numOptions - 1;
            break;
        case KEY_DOWN:
            selectedOption =
                (selectedOption < numOptions - 1) ? selectedOption + 1 : 0;
            break;
        }
        // Display options
        
        for (int i = 0; i < numOptions; i++) {
            std::string str = i == selectedOption ? "-> " : "   ";
            if (num_mode) {
                printw((str + prefix + " %d\n").c_str(), i + 1);
            } else {
                printw((str + stringOptions[i] + "\n").c_str(), i);
            }
        }
        switch (ch) {
        case 10: // Enter key
            return selectedOption;
        case 'q':
            return -1; // quit
        default:       // continue loop
            return -2;
        }
    }

    VerticalInterface(int options, int selectedOption = 0, std::string prefix = "Option")
        : selectedOption(selectedOption), totalOptions(options), prefix(prefix),
          num_mode(true) {}

    VerticalInterface(std::vector<std::string> options, int selectedOption = 0, std::string prefix = "Option")
        : selectedOption(selectedOption), stringOptions(options), prefix(prefix),
          num_mode(false) {}
};

class AccountSelectInterface : public VerticalInterface {
    public:
        void pre_key() override {
            printw("Please select an account:\n");
        }
    AccountSelectInterface(int accounts, int selectedAccount = 0)
        : VerticalInterface(accounts, selectedAccount, "Account") {}
};

class LoginSelectInterface : public VerticalInterface {
  private:
    std::string account_text;
  public:
    void pre_key() override { printw(("Welcome to account: " + account_text + "\n").c_str()); }

    LoginSelectInterface(std::vector<std::string> options, std::string account_text = "",
                         int selectedOption = 0)
        : VerticalInterface(options, selectedOption) {}
};

int main() {
    initscr();            // Initialize ncurses
    cbreak();             // Line buffering disabled
    noecho();             // Don't display keypresses
    keypad(stdscr, TRUE); // Enable function keys
    AccountSelectInterface account_select(3);

    int account_num = account_select(); // Call the functor like a function
    if (account_num == -1) {
        endwin();
        return 0;
    }
    LoginSelectInterface login({"Check balance", "Withdraw", "Deposit"}, std::to_string(account_num));
    int login_selection = login();

    endwin();
    return 0;
}
