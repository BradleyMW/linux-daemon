/*                                
 * Filename     inotify.cc             
 * Date         4/17/2020           
 * Author       Bradley Wersterfer   
 * Email        bmw170030@utdallas.edu
 * Course       CS 3377.502 Spring 2020
 * Version      1.0                     
 * Copyright    2020, All Rights Reserved
 *                                        
 * Description                             
 *                                          
 *     This file contains various functions using the inotify
 *     facility in order to monitor a pre-defined watch directory
 *     for changes to files. It waits in an infinite while loop
 *     for reading events, blocking until a proper modification
 *     event is found. Once that happens, the change is logged
 *     to an appropriate file and a timestamp is generated.
 *
 *     This timestamp is then appended to a copy of the file, which
 *     is placed in a .versions subfolder in the watched directory.
 *     If the number of these versions exceeds a pre-defined amount,
 *     then the oldest copies are deleted until there are only the
 *     desired number of copies left in the .versions subfolder.
 *     Furthermore, an AES-encrypted secure file (created with the
 *     aid of the Crypto++ library) is copied to the .secure subfolder.
 *     These files have a '?' appended to the end of their timestamp
 *     to signify that they are secure.
 *
 *     All of these pre-defined values are taken from a global
 *     map object, which was used earlier in the program to store
 *     parsed parameters for runtime.
 */

#include <sys/inotify.h>    // inotify functionality
#include <linux/limits.h>   // PATH_MAX
#include <fstream>          // File descriptors
#include <unistd.h>         // close()
#include <cstdlib>          // size_t

// Inclusions for Crypto++ functionality
#include "cryptopp/default.h"  // DefaultEncryptorWithMAC
#include "cryptopp/files.h"    // FileSource

#include "hw5.h"
extern std::map<int, std::string> args;

// Global variable for continuing to watch the directory. They
// are removed by the terminate() function on the proper signals.
int monitor;
size_t bufsiz = sizeof(struct inotify_event) + PATH_MAX + 1;
struct inotify_event *event = (struct inotify_event *) malloc(bufsiz);
int inot_id,
    watch_id;

// This function contains an infinite blocking while loop where the process
// will wait for IN_MODIFY events in the watched directory. When these events
// occur, the process resumes until it is ready to wait for more events.
void inotifyFunc() {

  //  size_t bufsiz = sizeof(struct inotify_event) + PATH_MAX + 1;
  //  struct inotify_event *event = (struct inotify_event *) malloc(bufsiz);

  // Sets up the inotify instance.
  inot_id = inotify_init();
  if(inot_id < 0) {
    logEndl("inotify initialization has failed. Terminating process now.");
    terminate(1);
  }

  // Adds a watch for modifications, creations, and deletions.
  watch_id = inotify_add_watch(inot_id, args[WATCHDIR].c_str(), IN_MODIFY);

  if(watch_id < 0) {
    logEndl("Adding watch to inotify has failed. Terminating process now.");
    terminate(1);
  }
  logEndl("Now watching directory: " + args[WATCHDIR]);

  // Enters an infinite loop until signalled to stop.
  int length;
  monitor = 1;
  while(monitor)
  {
    // Reads in the next event.
    length = read(inot_id, event, bufsiz);
    if(length < 0)
      logEndl("Reading functionality has failed with length: " + ToString(length));

      if(event->len) {
        // Determines what kind of an event should be logged.
        if(event->mask & IN_MODIFY) {

	  // Creates a timestamp and logs the modification.
	  std::string time = generateTimeStamp();
	  if(args[VERBOSE] == "TRUE") {
	    log("Modified " + std::string(event->name) + " at " + time);
	    logEndl("Creating a copy in the .versions subfolder.");
	  } else {
	    log("Modified " + std::string(event->name) + " at " + time);
	  }
	  makeCopy(event->name, time);
	  checkNumVersions(event->name);
	  makeSecureCopy(event->name, time);
        }
      }
  }

  // Signals that the program has exited successfully.
  // Program will never reach this normally.
  terminate(0);
}


// Creates a timestamped copy of a file.
void makeCopy(std::string file, std::string time) {

  // Copies the changed file to .versions subfolder.
  FILE *outputCmd;
  std::string cmd = "cp " + args[WATCHDIR] + "/" + file + " " + args[WATCHDIR] + "/.versions/" + file + "." + time;

  // Executes command and closes pipe.
  outputCmd = popen(cmd.c_str(), "r");
  pclose(outputCmd);
}


// Creates an encrypted backup using Crypto++ in the .secure subfolder.
void makeSecureCopy(std::string file, std::string time) {

  // Generates a name and path for the encrypted file.
  std::string originalFile = args[WATCHDIR] + "/" + file;
  std::string encryptedFile = args[WATCHDIR] + "/.secure/" + file + "." + time;

  // Encrypts the file. Adds a '?' to the end to signify that this file is secure.
  CryptoPP::FileSource f(originalFile.c_str(), true, 
            new CryptoPP::DefaultEncryptorWithMAC(args[PASSWORD].c_str(), 
                          new CryptoPP::FileSink(encryptedFile.c_str(), true)));
}


// This function uses popen() and shell commands to create a timestamp string.
std::string generateTimeStamp() {

  // From Makefile: $(shell date +'%Y.%m.%d-%H:%M:%S')
  FILE *outputCmd;
  outputCmd = popen("date +'%Y.%m.%d-%H:%M:%S'", "r");
  if(!outputCmd) {
    logEndl("Error: Timestamp not generated.");
  }

  std::string date = "";
  int buffer_size = 1024;
  char buffer[buffer_size];
  char *line_p;

  // Stores the results of the command in the string date.
  line_p = fgets(buffer, buffer_size, outputCmd);
  while(line_p != NULL) {
    date += line_p;
    line_p = fgets(buffer, buffer_size, outputCmd);
  }
  pclose(outputCmd);
  return date;
}


// Returns the number of files matching the given file name in the .versions subfolder.
void checkNumVersions(std::string file) {

  // The number of files is found by counting the words by line of a list of files found with regex:
  // ls <WatchDir>/.versions/<file>* | wc -l
  FILE* outputCmd;
  outputCmd = popen(("ls " + args[WATCHDIR] + "/.versions/" + file + "* | wc -l").c_str(), "r");
  if(!outputCmd) {
    logEndl("Error: Number of files " + file + " could not be retrieved.");
  }

  int numVersions = 0,
      buffer_size = 1024;
  char buffer[buffer_size];
  char *line_p;

  // Stores the results of the file listing in numVersions for comparison.
  line_p = fgets(buffer, buffer_size, outputCmd);
  while(line_p != NULL) {
    numVersions += std::atoi(line_p);
    line_p = fgets(buffer, buffer_size, outputCmd);
  }
  pclose(outputCmd);
  
  if(args[VERBOSE] == "TRUE") {
    logEndl("Number of Versions: " + ToString(numVersions) + "/" + args[NUMVERSIONS]);
  }


  // Checks to see if anything should be done with the number of versions.
  int numToDelete = numVersions - std::atoi(args[NUMVERSIONS].c_str());
  for( ; numToDelete > 0; numToDelete--) {
    // If the number of versions is still too high, the oldest one must be deleted.
    if(args[VERBOSE] == "TRUE") {
      logEndl("There are too many files being kept in .versions. Deleting the oldest copy.");
    }

    // Finds the last line of a list of the versions sorted newest to oldest.
    outputCmd = popen(("ls " + args[WATCHDIR] + "/.versions/" + file + "* -t | tail -n1").c_str(), "r");
    if(!outputCmd) {
      logEndl("Error: Copies of this file in the .versions folder were not found.");
    }

    // Retrieves the oldest matching file name from the above command.
    std::string oldFile = "";
    line_p = fgets(buffer, buffer_size, outputCmd);
    while(line_p != NULL) {
      oldFile += line_p;
      line_p = fgets(buffer, buffer_size, outputCmd);
    }
    pclose(outputCmd);

    // Removes the file from the system.
    outputCmd = popen(("rm -f " + oldFile).c_str(), "r");
    if(!outputCmd) {
      logEndl("Error: Could not remove the oldest copy from .versions.");
    }
    pclose(outputCmd);
  }
}
