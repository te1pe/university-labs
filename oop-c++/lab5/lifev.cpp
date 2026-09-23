#include "lifev.h"
#include <chrono>
#include <thread>

namespace {
    const std::string HELP_MESSAGE =
        " dump <filename>  сохранить вселенную в файл (формат Life 1.06)\n"
        " tick <n>         рассчитать n итераций и напечатать результат (n = 1 по умолчанию)\n"
        " t <n>            то же самое, короткая запись\n"
        " help             распечатать справку о командах\n"
        " exit             завершить игру\n";

    // очистка экрана и возврат курсора в левый верхний угол
    const std::string CLEAR_SCREEN = "\033[2J\033[H";

    const int FRAME_DELAY_MS = 300;
}

void LifeView::printHelp() {
    std::cout << HELP_MESSAGE << std::endl;
}

void LifeView::printError(const std::string& message) {
    std::cout << "Error: " << message << std::endl;
}

void LifeView::printWarning(const std::string& message) {
    std::cout << "Warning: " << message << std::endl;
}

void LifeView::printFrame(const LifeModel& model, int step, int total) {
    const Field& field = model.getField();
    const int side = field.getSize();

    std::cout << CLEAR_SCREEN;
    std::cout << "Universe: " << model.getName() << std::endl;
    std::cout << "Rule: " << model.getRule() << std::endl;
    std::cout << "Iteration: " << model.getIteration()
              << " (step " << step << " of " << total << ")" << std::endl;

    std::cout << '+' << std::string(side * 2, '-') << '+' << std::endl;
    for (int row = 0; row < side; ++row) {
        std::cout << '|';
        for (int column = 0; column < side; ++column) {
            std::cout << (field.getState(row, column) ? "* " : "  ");
        }
        std::cout << '|' << std::endl;
    }
    std::cout << '+' << std::string(side * 2, '-') << '+' << std::endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(FRAME_DELAY_MS));
}
