/*!
 * inference.h
 *
 * Class to perform inference on a separate thread.
 *
 *  Created on: Jun 23, 2020
 *      Author: Mickey Beurskens
 */

#ifndef INFERENCE_INFERENCE_H
#define INFERENCE_INFERENCE_H

#include <thread>
#include <vector>
#include "turtle.h"

class Inference{
private:

    static bool TurtleJoined(std::vector<Turtle> &turtles);

    static bool TurtlesJoinedHaveLeft(std::vector<Turtle> &turtles);

    static bool TurtleWithLoadError(std::vector<Turtle> &turtles);

    static void FinishTurtleRuns(std::vector<Turtle> &turtles);

    static void WaitForTurtles(std::vector<Turtle> &turtles, bool &exit_flag);

    static std::vector<Turtle> GetTurtles();

    static std::vector<std::thread> InitializeTurtleThreads(std::vector<Turtle> &turtles, bool &exit_flag);

    static void StopTurtleThreads(std::vector<std::thread> &threads);

    static turtle_info GetRunTurtleInfo(ConfigLoader config);

public:

    /*!
     * Initialize neural network inference.
     */
    Inference();

    /*!
     * Initialize neural network inference.
     * @param inf Inference class instance for use with thread library.
     */
    Inference(const Inference &inf);

    /*!
     * Run inference until exit flag is triggered.
     * @param exit_flag Pointer to flag which is true when inference running needs to stop.
     * @paragraph model_file_path Absolute file path to json config file.
     */
    void Run(bool &exit_flag, const char* config_file_path);
};

#endif /* INFERENCE_INFERENCE_H_ */
