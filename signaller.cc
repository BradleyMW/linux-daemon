/* 
 * Filename     signaller.cc 
 * Date         4/15/2020   
 * Author       Bradley Wersterfer 
 * Email        bmw170030@utdallas.edu 
 * Course       CS 3377.502 Spring 2020 
 * Version      1.0                    
 * Copyright    2020, All Rights Reserved
 *                                      
 * Description                         
 *                                    
 *     This file contains the signal handling function that
 *     accepts external signals and processes them. On a
 *     SIGHUP, the configuration file is reread and the program
 *     resumes normally. On a SIGINT or a SIGTERM, the process
 *     ends and exits gracefully with the terminate function.
 */

#include <sstream>  // Necessary for fixing to_string() for g++ 4.8
#include "hw5.h"

// Values declared externally that are used for monitoring the program or terminating.
extern std::map<int, std::string> args;
extern int monitor;

void signalHandler (int sig) {

  // Prints out the received signal.
  std::string msg = "Signal (" + ToString(sig) + ") received. ";
  log(msg);

  // Determines how to use the signal.
  switch(sig) {
    // SIGINT or SIGTERM = terminate process.
    case(SIGTERM):
    case(SIGINT):
      // Remove PID file, close logging file, and exit.
      if(args[VERBOSE] == "TRUE") {
	log("Terminating process now.");
      }
      log("\n");

      // Signals to stop waiting for signals.
      monitor = false;
      terminate(0);
      break;

    // SIGHUP = re-read .conf file.
    case(SIGHUP):
      if(args[VERBOSE] == "TRUE") {
	log("Re-reading configuation file.");
      }
      log("\n");

      reparseConfig();
      break;
  }
}


// There seems to be an error with g++ 4.8 and to_string(), 
// so I have defined a compatible method here.
std::string ToString(int n) {
  std::stringstream s;
  s << n;
  return s.str();
}
