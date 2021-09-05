/*  
 * Filename     parseConfig.cc 
 * Date         4/13/2020     
 * Author       Bradley Wersterfer
 * Email        bmw170030@utdallas.edu
 * Course       CS 3377.502 Spring 2020
 * Version      1.0                   
 * Copyright    2020, All Rights Reserved
 *                                    
 * Description                       
 *                                  
 *     This file uses the rudeconfig library in order to parse
 *     a configuration file for additional parameters to store
 *     in a global map object for this project.
 *
 *     The configuration file name is taken from this map file,
 *     which was already partially populated by TCLAP. Basic
 *     error checking is also performed; if sections or config-
 *     uration variables are missing from the supplied file,
 *     then the program will log an error message and exit.
 *
 *     This file also provides functionality for updating these
 *     configuration parameters via a SIGHUP signal. In that case,
 *     the file is once again read and error-checked. Mutable
 *     parameters are fluidly updated, but if the Password or
 *     WatchDir fields change, then the program logs an error
 *     message and terminates.
 */

#include <cstring>
#include <rude/config.h>
#include <locale>
#include <stdio.h>
#include "hw5.h"
using namespace std;

extern map<int, string> args;

// Parses a configuration file with rudeconfig and stores the results
// in a C++ map for processing.
void parseConfig() {

  // Loads the config file with the name in the given map.
  rude::Config config;
  config.load(args[CONFIG_NAME].c_str());

  // Checks for errors in opening the .conf file.
  if(strcmp(config.getError(), "") != 0) {
    // .conf file was not opened. There is no log file to write to.
    printf("Error: Configuration file \"%s\" was not found.\n", args[CONFIG_NAME].c_str());
    terminate(1);
  }

  // Checks for proper section.
  if(!config.setSection("Parameters", false)) {
    printf("Error: [Parameters] section not found in %s.conf. Terminating now.\n", args[CONFIG_NAME].c_str());
    terminate(1);
  }

  // Checks to make sure that there are no missing definitions.
  if(!config.exists("Verbose")) {
     printf("Error: Verbose definition is missing. Terminating now.\n");
     terminate(1);
  }
  if(!config.exists("LogFile")) {
    printf("Error: LogFile definition is missing. Terminating now.\n");
    terminate(1);
  }
  if(!config.exists("Password")) {
    printf("Error: Password definition is missing. Terminating now.\n");
    terminate(1);
  }
  if(!config.exists("NumVersions")) {
    printf("Error: NumVersions definition is missing. Terminating now.\n");
    terminate(1);
  }
  if(!config.exists("WatchDir")) {
    printf("Error: WatchDir definition is missing. Terminating now.\n");
    terminate(1);
  }

  // Stores configuration values in the global map.
  args[VERBOSE] = config.getStringValue("Verbose");
  args[LOGFILE] = config.getStringValue("LogFile");
  args[PASSWORD] = config.getStringValue("Password");
  args[NUMVERSIONS] = config.getStringValue("NumVersions");
  args[WATCHDIR] = config.getStringValue("WatchDir");

  // If no log file was provided, the default is cs3377dirmond.log.
  if(args[LOGFILE] == "") {
    args[LOGFILE] = "cs3377dirmond.log";
  }

  printf("Successfully parsed %s with rudeconfig...\n", args[CONFIG_NAME].c_str());
  return;
}

// This function is similar to the above, but it prevents changes to immutable fields.
// The configuration arguments will also already be present for usage.
void reparseConfig() {

  rude::Config config;
  config.load(args[CONFIG_NAME].c_str());

  // Checks for errors in opening the config file.
  if(strcmp(config.getError(), "") != 0) {
    // .conf file was not opened. Program must terminate.
    logEndl("Error: " + args[CONFIG_NAME] + " not opened. Terminating now.");
    terminate(1);
  }

  // Checks for the proper section.
  if(!config.setSection("Parameters", false)) {
    logEndl("Error: [Parameters] section not found in .conf file. Terminating now.");
    terminate(1);
  }

  // Checks to make sure that there are no missing definitions.
  if(!config.exists("Verbose")) {
    printf("Error: Verbose definition is missing. Terminating now.\n");
    terminate(1);
  }
  if(!config.exists("LogFile")) {
    printf("Error: LogFile definition is missing. Terminating now.\n");
    terminate(1);
  }
  if(!config.exists("Password")) {
    printf("Error: Password definition is missing. Terminating now.\n");
    terminate(1);
  }
  if(!config.exists("NumVersions")) {
    printf("Error: NumVersions definition is missing. Terminating now.\n");
    terminate(1);
  }
  if(!config.exists("WatchDir")) {
    printf("Error: WatchDir definition is missing. Terminating now.\n");
    terminate(1);
  }

  // Updates parameters that are allowed to change.
  args[VERBOSE] = config.getStringValue("Verbose");
  args[LOGFILE] = config.getStringValue("LogFile");
  args[NUMVERSIONS] = config.getStringValue("NumVersions");
 
  // If no log file was provided, the default is cs3377dirmond.log.
  if(args[LOGFILE] == "") {
    args[LOGFILE] = "cs3377dirmond.log";
  }

  // Updates the current log file.
  closeLog();
  openLogAppend();

  // Password and WatchDir are NOT allowed to change.
  // If either is different, the daemon will terminate
  // to avoid having a different configuration than the
  // intended .conf file.
  if(args[PASSWORD] != config.getStringValue("Password") || args[WATCHDIR] != config.getStringValue("WatchDir")) {
    logEndl("Password or WatchDir fields cannot be changed while running. Terminating now.");
    terminate(1);
  }

  // If verbose, display new configuration.
  if(args[VERBOSE] == "TRUE") {
    logEndl("Process resumed. New configuration is:");
    logConfig();
  }
  return;
}

