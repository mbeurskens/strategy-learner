/*!
 * inference.cpp
 *
 * Class to perform inference on a separate thread.
 *
 *  Created on: Jun 23, 2020
 *      Author: Mickey Beurskens
 */

#include <thread>
#include <iostream>
#include "inference.h"
#include "turtle.h"

Inference::Inference() {}

Inference::Inference(const Inference &inf)
{
    Inference();
}

void Inference::Run(bool &exit_flag, const char* config_file_path)
{
    // Keep trying to connect to turtles until quit command is called
    while (!exit_flag){
        std::vector<Turtle> turtles = GetTurtles();
        std::vector<std::thread> threads = InitializeTurtleThreads(turtles, exit_flag);
        WaitForTurtles(turtles, exit_flag);
        StopTurtleThreads(threads);
        std::cout << "\tConnections terminated\n";
    }
}

bool Inference::TurtleJoined(std::vector<Turtle> &turtles)
{
    for (std::vector<Turtle>::iterator turtle = turtles.begin(); turtle != turtles.end(); ++turtle){
        if (turtle->GetJoined()) {
            return true;
        }
    }
    return false;
}

bool Inference::TurtlesJoinedHaveLeft(std::vector<Turtle> &turtles)
{
    for (std::vector<Turtle>::iterator turtle = turtles.begin(); turtle != turtles.end(); ++turtle){
        if (turtle->GetJoined() != turtle->GetLeft()) {
            return false;
        }
    }
    return true;
}

bool Inference::TurtleWithLoadError(std::vector<Turtle> &turtles) {
    for (std::vector<Turtle>::iterator turtle = turtles.begin(); turtle != turtles.end(); ++turtle){
        if (turtle->GetLoadError()) {
            return true;
        }
    }
    return false;
}

void Inference::FinishTurtleRuns(std::vector<Turtle> &turtles)
{
    for (std::vector<Turtle>::iterator turtle = turtles.begin(); turtle != turtles.end(); ++turtle){
        turtle->SetFinish(true);
    }
}

void Inference::WaitForTurtles(std::vector<Turtle> &turtles, bool &exit_flag) {
    while (!exit_flag && !TurtleJoined(turtles) && !TurtleWithLoadError(turtles)){}
    while (!exit_flag && !TurtlesJoinedHaveLeft(turtles) && !TurtleWithLoadError(turtles)){}
    FinishTurtleRuns(turtles);
}

std::vector<Turtle> Inference::GetTurtles() {
    // Create turtle objects and threads for each turtle and try to connect to server
    std::vector<Turtle> turtles = {Turtle(2), Turtle(3)};
    return turtles;
}

std::vector<std::thread> Inference::InitializeTurtleThreads(std::vector<Turtle> &turtles, bool &exit_flag) {
    std::vector<std::thread> threads;
    for (int i = 0; i < turtles.size(); ++i){
        threads.push_back(std::thread(&Turtle::Run, &turtles[i], std::ref(exit_flag),
                                      "/home/robocup/svn/trunk/src/Turtle2/Strategy/src/STP/strategy_learner/inference"
                                      "/settings/inference_settings.json"));
    }
    return threads;
}

void Inference::StopTurtleThreads(std::vector<std::thread> &threads) {
    for (std::vector<std::thread>::iterator thread = threads.begin(); thread != threads.end(); ++thread){
        if (thread->joinable()) {
            thread->join();
        }
    }
}
