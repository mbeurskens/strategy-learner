/*!
 * turtle.cpp
 *
 * Manage shared memory connection and inference for each turtle on a separate thread.
 *
 *  Created on: Jul 16, 2020
 *      Author: Mickey Beurskens
 */

#ifndef INFERENCE_TURTLE_H
#define INFERENCE_TURTLE_H

#include "config_loader.h"
#include "base_model.h"
#include "info_shared_memory.h"
#include "timer.h"

class Turtle{
private:
    bool joined;    // True if turtle created shared memory and the client joined the shared memory server
    bool left;      // True if turtle joined first, and has now closed its shared memory server again
    bool finish;    // If flag is set, finish current run
    bool model_load_error;
    int t_id;       // Current turtle ID.
    const char* model_path; // Path to pytorch model
    const char* vpg_policy_model_path;  // Path to pytorch model
    const char* vpg_value_model_path;   // Path to pytorch model
    const char* algorithm;  // Type of algorithm used
    bool greedy_mode_flag;  // Set inference mode to greedy if true
    ConfigLoader configs;

    bool ModelPathValid();

    turtle_info LoadTurtleInfo(const char* settings_file_path);

    void RunInferenceModel(bool &exit_flag, InfoShm &info_server, BaseModel &model, Timer &time);

    void RunLearnedMode(bool &exit_flag, InfoShm &info_server);

    void RunManualMode(bool &exit_flag, InfoShm &info_server);

    void ConnectToTurtle(bool &exit_flag, bool &local_exit_flag, InfoShm &info_server, const char* settings_file_path);

public:

    /*!
     * Constructor
     * @param turtle_id Turtle ID of turtle to connect to.
     */
    Turtle(int turtle_id);

    /*!
     * Constructor for threading.
     * @param t Current instance of turtle class.
     * @param turtle_id Turtle ID of turtle to connect to.
     */
    Turtle(const Turtle &t, int turtle_id);

    /*!
     * Thread for turtle with turtle ID t_id that starts info server and ends when turtle
     * disconnects or exit flag is set.
     * @param exit_flag Pointer to flag which is true when inference running needs to stop.
     * @param settings_file_path File path to settings file.
     */
    void Run(bool &exit_flag, const char* settings_file_path);

    /*!
     * Returns joined status.
     * @return True if turtle created shared memory and the client joined the shared memory server.
     */
    bool GetJoined();

    /*!
     * Returns left status.
     * @return True if turtle joined first, and has now closed its shared memory server again.
     */
    bool GetLeft();

    /*!
     * Returns whether model load error has occurred.
     * @return True if inference model load error occurred.
     */
    bool GetLoadError();

    /*!
     * Set the finish flag. Finish current threaded run.
     * @param flag True is run needs to finish.
     */
    void SetFinish(bool flag);
};

#endif //INFERENCE_TURTLE_H
