/* 
 * Filename     logger.cc 
 * Date         4/15/2020 
 * Author       Bradley Wersterfer 
 * Email        bmw170030@utdallas.edu 
 * Course       CS 3377.502 Spring 2020 
 * Version      1.0                    
 * Copyright    2020, All Rights Reserved 
 *                                       
 * Description                          
 *                                     
 *     This file contains various logging functions that
 *     use the main parameter map to determine which file
 *     to log to. Because the logging file is opened and closed
 *     each time, output is simply appended to whatever the
 *     current log file is (supporting a change of log files in
 *     the configuration file).
 *
 *     There is also a function provided to reset the logging file,
 *     meant for use at the beginning of the program to generate a
 *     new log file if one is not already present.
 */

#include "hw5.h"
#include <fstream>
#include <sys/inotify.h>    // Closing inotify
#include <unistd.h>         // close()

using namespace std;
extern map<int, string> args;

// The log must be defined globally so that it can be closed
// by the signal handler functions.
ofstream logf;
extern int inot_id, watch_id;
extern struct inotify_event *event;

// Either opens log fresh for program start or in appending mode for SIGHUP signal.
void openLog() {
  logf.open(args[LOGFILE].c_str());
}
void openLogAppend() {
  logf.open(args[LOGFILE].c_str(), ios_base::app);
}

// Logs messages to the intended output file.
void log(string msg) {
  logf << msg << flush;
}
void logEndl(string msg) {
  logf << msg << endl;
}

// This function logs all of the current configuration parameters
// for debugging purposes. It can also be used when the Verbose
// parameter is marked as true.
void logConfig() {
  logf << endl << "Daemon: " << args[DAEMON] << endl;
  logf << "ConfigName: " << args[CONFIG_NAME] << endl;
  logf << "Verbose: " << args[VERBOSE] << endl;
  logf << "LogFile: " << args[LOGFILE] << endl;
  logf << "NumVersions: " << args[NUMVERSIONS] << endl;
  logf << "Password: " << args[PASSWORD] << endl;
  logf << "WatchDir: " << args[WATCHDIR] << endl << endl;
}

// This provides a way for the log to be closed gracefully on exit.
void closeLog() {
  logf.close();
}

// This function handles a graceful exit in a variety of situations after the log file
// has been created.
void terminate(int fail) {

  // Removes the .pid file from the system.
  FILE *outputCmd;
  outputCmd = popen("rm -f cs3377dirmond.pid", "r");
  if(!outputCmd) {
    logEndl("ERROR: cs3377dirmond.pid could not be removed.");
  }
  pclose(outputCmd);

  // Logs that the process is terminating. If verbose, includes ending signal.
  if(args[VERBOSE] == "TRUE") {
    if(fail) {
      log("Program terminated on a 1 (failure).\n");
    }
    else
      log("Program terminated on a 0 (success).\n");
  }
  else {
    // If output is not verbose, simply terminate the program.
    log("Program terminated.\n");
  }

  // Removes watch and closes inotify instance.
  inotify_rm_watch(inot_id, watch_id);
  close(inot_id);
  delete event;

  // Log file is closed and program terminates.
  closeLog();
  exit(fail);
}
