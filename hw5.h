/*                                              
 * Filename     hw5.h                           
 * Date         4/13/2020                         
 * Author       Bradley Wersterfer                 
 * Email        bmw170030@utdallas.edu              
 * Course       CS 3377.502 Spring 2020              
 * Version      1.0                                   
 * Copyright    2020, All Rights Reserved              
 *                                                      
 * Description                                           
 *                                                        
 *     This header file contains various inclusions used by
 *     several of the individual source files in the project,
 *     along with a global enumerated key for the map object
 *     of parameters shared between the files. Finally, it
 *     declares function prototypes by their respective file.
 */

#ifndef hw5
#define hw5

#include <string>
#include <map>
#include <stdlib.h>
#include <csignal>

// The map object used to store parameters and configurations.
// Defined globally for this project to assist access.
enum args_keys {DAEMON = 0, CONFIG_NAME, VERBOSE, LOGFILE, PASSWORD, NUMVERSIONS, WATCHDIR};

// Defined in parseCmdLine.cc
std::map<int, std::string> parseCmdLine(int argc, char **argv);

// Defined in parseConfig.cc
void parseConfig();
void reparseConfig();
std::string toupper(std::string str);

// Defined in signaller.cc
int signalReady();
void signalHandler(int sig);
std::string ToString(int n);

// Defined in logger.cc
void openLog();
void openLogAppend();
void log(std::string msg);
void logEndl(std::string msg);
void logConfig();
void closeLog();
void terminate(int fail);

// Defined in inotify.cc
void inotifyFunc();
void makeCopy(std::string file, std::string time);
void makeSecureCopy(std::string file, std::string time);
std::string generateTimeStamp();
void checkNumVersions(std::string file);

// Defined in hw5.cc
void createSubfolders();

#endif
