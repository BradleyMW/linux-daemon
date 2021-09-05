/*                 
 * Filename     decrypt.cc 
 * Date         4/20/2020 
 * Author       Bradley Wersterfer   
 * Email        bmw170030@utdallas.edu 
 * Course       CS 3377.502 Spring 2020 
 * Version      1.0                    
 * Copyright    2020, All Rights Reserved 
 *                                       
 * Description                          
 *                                     
 *     This program is meant to accompany the main cs3377dirmond
 *     program for CS3377's fifth homework project. It can be
 *     compiled with that accompanying Makefile with make decrypt,
 *     or it can be compiled by hand as shown below.
 *
 *     This program uses the Crypto++ library to decrypt a file
 *     with the AES algorithms. It only accepts one form of command
 *     line arguments: ./cs3377decrypt <password> <infile> <outfile>,
 *     where the password, infile, and outfile are all strings.
 *
 *     TCLAP is used to parse the command line and generate usage
 *     statements in the case of incorrect calls.
 *
 */

// Recommended to either compile with make decrypt or to use this command: 
//    g++ -o cs3377decrypt -I ~/include/ decrypt.cc -static -L /home/012/b/bm/bmw170030/lib -lcryptopp

// Inclusions for TCLAP functionality
#include "tclap/CmdLine.h"
#include "tclap/UnlabeledValueArg.h"

// Inclusions for Crypto++ functionality
#include "cryptopp/default.h"  // DefaultDecryptorWithMAC
#include "cryptopp/files.h"    // FileSource

// General inclusions for this program.
#include <string>
#include <iostream>

// This program has the usage:
// cs3377decrypt <password> <infile> <outfile>
int main(int argc, char *argv[]) {
  
  // Creates the TCLAP objects necessary for parsing the command line.
  TCLAP::CmdLine cmd("cs3377decrypt AES Decryption Service", ' ', "1.0");
  TCLAP::UnlabeledValueArg<std::string> pass("password", "The password used to decrypt the file. Must match the password used to encrypt the file.", true, "error", "password string", false);
  TCLAP::UnlabeledValueArg<std::string> in("infile", "The encrypted file name. Recommended to provide full path.", true, "error", "infile name", false);
  TCLAP::UnlabeledValueArg<std::string> out("outfile", "The output file. Will receive the decrypted text.", true, "error", "outfile name", false);

  // Parses the command line and stores the result in an args map.
  cmd.add(pass);
  cmd.add(in);
  cmd.add(out);
  cmd.parse(argc, argv);

  // Decrypt the file <inname> and store output in <outname> file.
  CryptoPP::FileSource f(in.getValue().c_str(), true, 
     new CryptoPP::DefaultDecryptorWithMAC(pass.getValue().c_str(), 
        new CryptoPP::FileSink(out.getValue().c_str(), true)));

  // Terminates the program.
  return 0;
}
