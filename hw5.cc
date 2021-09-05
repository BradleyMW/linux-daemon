/*
 * Filename     hw5.cc
 * Date         4/13/2020
 * Author       Bradley Wersterfer
 * Email        bmw170030@utdallas.edu
 * Course       CS 3377.502 Spring 2020
 * Version      1.0                   
 * Copyright    2020, All Rights Reserved
 *                                      
 * Description                         
 * 
 *     This program uses several third party libraries such as
 *     TCLAP and rudeconfig in order to parse desired configuration
 *     settings and set up a global map object of parameters. Once
 *     that is done, it sets up the inotify facility to monitor a
 *     given directory for file modifications. Upon noticing any,
 *     a timestamped copy of the file is generated and stored in a
 *     .versions subfolder and an AES-encrypted copy is stored in a
 *     .secure subfolder. If the number of copies in the .versions
 *     folder exceeds the desired amount from the configuration file,
 *     then all excess copies will be deleted. All possible messages 
 *     are logged to their respective log file.
 *
 *     This program does not contain a built-in AES decryption method.
 *     Instead, I would recommend using cs3377decrypt, an accompanying
 *     program provided in this folder. It can be compiled with the
 *     command make decrypt, and its usage can be checked with the
 *     command cs3377decrypt -h. Encrypted files can be identified by
 *     the '?' appended to the end of their names.
 *
 *     This program is also capable of being run as a daemon in the
 *     background with the -d command line option. In either case,
 *     the program behaves exactly the same and should be terminated
 *     with a SIGINT or SIGTERM signal to gracefully exit. It can also
 *     safely be issued the SIGHUP command to update itself and reread
 *     the original configuration file to update its parameters.
 */

#include <unistd.h> 	// Contains fork()
#include <fstream>      // Handles .pid file
#include <cstring>      // Used for popen()
#include "hw5.h"
using namespace std;

map<int, string> args;

int main(int argc, char *argv[]) {

  // Registers signal catchers for SIGINT, SIGTERM, and SIGHUP.
  signal(SIGINT, signalHandler);
  signal(SIGTERM, signalHandler);
  signal(SIGHUP, signalHandler);

  // Maps out the command line arguments using TCLAP and the
  // configuration file using rudeconfig.
  args = parseCmdLine(argc, argv);
  printf("Successfully parsed the command line with TCLAP...\n");
  parseConfig();

  // Creates pid file if not present. If it is, the program instead exits.
  ofstream pid;
  ifstream pid_exists;
  pid_exists.open("cs3377dirmond.pid");
  if(pid_exists.good()) {
    // .pid file already existed.
    printf("ERROR: Cannot create second daemon.\n");
    return 1;
  }
  else {
    // .pid file did not exist.
    pid.open("cs3377dirmond.pid");
    pid << getpid();
    pid.close();
  }

  // Sets up the log file for further output.
  printf(("Any further messages will be sent to the log file " + args[LOGFILE] + ".\n").c_str());
  openLog();
  logEndl("Original process started with PID: " + ToString(getpid()));

  // Uses the mapped parameters to determine whether it
  // should fork or not.
  if(args[DAEMON] == "TRUE") {
    int forkVal = fork();
    if(forkVal == -1) {
      logEndl("ERROR: fork() failed. Terminating now.");
      return 1;
    }

    // This process is the child (spawned as a daemon).
    else if(forkVal == 0) {
      logEndl("Daemon with PID " + ToString(getpid()) + " created.");
      if(args[VERBOSE] == "TRUE") {
	logEndl("Starting configuration is:");
	logConfig();
      }
    }

    // This process is the parent (ready to exit).
    // It will return before reaching the infinite while loop.
    else {
      return 0;
    }
  }

  // If process reaches this point, it is either the daemon or the
  // main program acting like normal. Proceed with program. After
  // creating any missing subfolders, the process is ready to run.
  createSubfolders();
  inotifyFunc();

  // inotifyFunc() can only be exited via signals, so this
  // return statement will never execute.
  return 0;
}

// Checks to make sure that the proper subfolders are present.
void createSubfolders() {

  // Checks for the .versions folder first.
  FILE* outputCmd;
  string vers_dir = args[WATCHDIR] + "/.versions";
  string cmd = "if [ -d " + vers_dir + " ];\nthen\necho \"WatchDir/.versions exists \" \nelse\nmkdir " + vers_dir + "\nfi";
  outputCmd = popen(cmd.c_str(), "r");
  pclose(outputCmd);

  // Checks for the .secure folder next.
  vers_dir = args[WATCHDIR] + "/.secure";
  cmd = "if [ -d " + vers_dir + " ];\nthen\necho \"WatchDir/.secure exists \" \nelse\nmkdir " + vers_dir + "\nfi";
  outputCmd = popen(cmd.c_str(), "r");
  pclose(outputCmd);
}

