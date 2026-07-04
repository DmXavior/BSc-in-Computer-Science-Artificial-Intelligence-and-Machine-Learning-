#include <iostream>
#include "menu.h"
#include "weatherManager.h"
int main() {
    int choice;

    while (true) {
        showMenu();
        std::cout << "Enter your choice: ";
        std::cin >> choice;
        if (choice == 7) break; // Exit on choice 7
        handleMenuChoice(choice);
    }

    return 0;
}