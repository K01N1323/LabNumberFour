#include <iostream>
#include "ui/Menu.h"
#include "tests/Tests.h"

int main() {
    int choice = -1;
    while (choice != 0) {
        std::cout << "\n\n";
        std::cout << "      ГЛАВНОЕ МЕНЮ ЛР-4   \n";
        std::cout << "\n";
        std::cout << "1. Запустить модульные тесты\n";
        std::cout << "2. Запустить консольный интерфейс (UI)\n";
        std::cout << "0. Выход\n";
        std::cout << "Ваш выбор: ";
        std::cin >> choice;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(32767, '\n');
            continue;
        }

        switch (choice) {
            case 1:
                RunAllTests();
                break;
            case 2: {
                ConsoleInterface ui;
                ui.Run();
                break;
            }
            case 0:
                std::cout << "Выход из программы...\n";
                break;
            default:
                std::cout << "Неверный выбор. Попробуйте снова.\n";
        }
    }
    return 0;
}
