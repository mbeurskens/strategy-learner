/*!
 * main.cpp
 *
 * Main file for the strategy learner inference library.
 *
 *  Created on: Jul 14, 2020
 *      Author: Mickey Beurskens
 */


#include <thread>
#include <iostream>
#include "inference.h"

static void show_main_menu() {
    std::cout << "\n";
    std::cout << "Welcome to strategy learner inference. What would you like to do?\n";
    std::cout << "\t A. Start automatic inference\n";
    std::cout << "\t Q. Quit strategy learner inference\n";
    std::cout << "\n";
}

static void wait_for_quit() {
    char user_input;
    while (true) {
        std::cout << "Go back to main menu at any time by pressing Q\n";
        std::cin >> user_input;
        if (user_input == 'q' || user_input == 'Q') {
            break;
        }
    }
}

static void run_automatic_mode() {
    bool exit_flag = false;
    Inference inf;

    std::thread thread_obj(&Inference::Run, &inf, std::ref(exit_flag), "/home/robocup/svn/trunk/src/Turtle2/Strategy/"
                                                                       "src/STP/strategy_learner/inference/settings/"
                                                                       "inference_settings.json");

    wait_for_quit();
    exit_flag = true;

    if (thread_obj.joinable()){
        thread_obj.join();
    }
}

static bool run_main_menu() {
    char user_input;
    show_main_menu();

    std::cin >> user_input;
    if (user_input == 'q' || user_input == 'Q') {
        std::cout << "Goodbye\n";
        return false;
    } else if (user_input == 'a' || user_input == 'A') {
        run_automatic_mode();
        return true;
    } else {
        std::cout << "That is not an option currently, try again\n";
        return true;
    }
}

int main(int argc, char* argv[]) {
    bool continue_menu = true;
    while(continue_menu){
        continue_menu = run_main_menu();
    }
    return 0;
}
