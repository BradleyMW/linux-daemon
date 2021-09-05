/*
 * Filename     parseCmdLine.cc
 * Date         4/13/2020 
 * Author       Bradley Wersterfer 
 * Email        bmw170030@utdallas.edu 
 * Course       CS 3377.502 Spring 2020 
 * Version      1.0             
 * Copyright    2020, All Rights Reserved  
 *                                       
 * Description                          
 *                                     
 *     This file contains the necessary header inclusions
 *     and uses them to create a function that will use
 *     TCLAP to parse the arguments passed to the command
 *     line. It does this with the argc and argv parameters
 *     from the main function, and it stores in a global map
 *     with string representations of the parameters stored
 *     with integer keys.
 */

// Includes the necessary headers for TCLAP.
#include "tclap/CmdLine.h"
#include "tclap/SwitchArg.h"
#include "tclap/UnlabeledValueArg.h"

// Necessary for the enumerated argument keys.
#include "hw5.h"
using namespace std;

// This function uses TCLAP in order to create a map of the command line arguments.
map<int, string> parseCmdLine(int argc, char *argv[]) {
  map<int, string> args;

  // Creates the TCLAP objects necessary for parsing the command line.
  TCLAP::CmdLine cmd("cs3377dirmond Directory Monitor Daemon", ' ', "1.0");
  TCLAP::SwitchArg daemon("d", "daemon", "Run in daemon mode (forks to run as a daemon).", cmd, false);
  TCLAP::UnlabeledValueArg<string> config_name("config_file", "The name of the configuration file. Defaults to cs3377dirmond.conf", false, "cs3377dirmond.conf", "config filename", true);

  // Parses the command line and stores the result in an args map.
  cmd.add(config_name);
  cmd.parse(argc, argv);
  if(daemon.getValue()) {
    args[DAEMON] = "TRUE";
  }
  else {
    args[DAEMON] = "FALSE";
  }
  args[CONFIG_NAME] = config_name.getValue();
  return args;
}
