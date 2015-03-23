// $Id: cix-client.cpp,v 1.3 2015-03-12 23:44:16-07 - - $
// Brad Cardello (bcardell)
// Shayan Farmani (sfarmani)
// asg5

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <unordered_map>
using namespace std;

#include <libgen.h>
#include <sys/types.h>
#include <unistd.h>

#include "cix_protocol.h"
#include "logstream.h"
#include "signal_action.h"
#include "sockets.h"

logstream log (cout);

unordered_map<string,cix_command> command_map {
   {"exit", CIX_EXIT},
   {"get" , CIX_GET },
   {"help", CIX_HELP},
   {"ls"  , CIX_LS  },
   {"put" , CIX_PUT },
   {"rm"  , CIX_RM  },
};

void cix_help() {
   static vector<string> help = {
      "exit         - Exit the program.  Equivalent to EOF.",
      "get filename - Copy remote file to local host.",
      "help         - Print help summary.",
      "ls           - List names of files on remote server.",
      "put filename - Copy local file to remote host.",
      "rm filename  - Remove file from remote server.",
   };
   for (const auto& line: help) cout << line << endl;
}

// cix_ls
void cix_ls (client_socket& server) {
   cix_header header;
   header.cix_command = CIX_LS;
   log << "sending header " << header << endl;
   send_packet (server, &header, sizeof header);
   recv_packet (server, &header, sizeof header);
   log << "received header " << header << endl;
   if (header.cix_command != CIX_LSOUT) {
      log << "sent CIX_LS, server did not return CIX_LSOUT" << endl;
      log << "server returned " << header << endl;
   }else {
      char buffer[header.cix_nbytes + 1];
      recv_packet (server, buffer, header.cix_nbytes);
      log << "received " << header.cix_nbytes << " bytes" << endl;
      buffer[header.cix_nbytes] = '\0';
      cout << buffer;
   }
}

// cix_put
void cix_put (client_socket& server, const char* filename) {
   cix_header header;
   header.cix_command = CIX_PUT;
   for (size_t i = 0; i < strlen(filename); i++){
      header.cix_filename[i] = filename[i];
   }
   ifstream ifs;
   ifs.open(header.cix_filename, fstream::in);
   string put_output;
   if (!ifs.good()){
      log << header.cix_filename
          << ": No such file or directory"
          << endl;
      return;
   }
   while (!ifs.eof()){
      char buffer[0x1000];
      ifs.read (buffer, sizeof buffer);
      put_output.append (buffer);
   }
   ifs.close();
   header.cix_nbytes = put_output.size();
   
   log << "sending header " << header << endl;
   send_packet (server, &header, sizeof header);
   send_packet (server, put_output.c_str(), header.cix_nbytes);
   log << "sent " << header.cix_nbytes << " bytes" << endl;
   recv_packet (server, &header, sizeof header);
   log << "received header " << header << endl;
   if (header.cix_command != CIX_ACK) {
      log << "sent CIX_PUT, server did not return CIX_ACK" << endl;
      log << "server returned " << header << endl;
   }
}

// cix_get
void cix_get (client_socket& server, const char* filename) {
   cix_header header;
   header.cix_command = CIX_GET;
   for (size_t i = 0; i < strlen(filename); i++){
      header.cix_filename[i] = filename[i];
   }
   header.cix_nbytes = strlen(filename);
   log << "sending header " << header << endl;
   send_packet(server, &header, sizeof header);
   recv_packet(server, &header, sizeof header);
   log << "received " << header.cix_nbytes << " bytes" << endl;
   
   if (header.cix_command != CIX_FILE) {
      log << "sent CIX_GET, server did not return CIX_FILE" << endl;
      log << "server returned " << header << endl;
   }
   else{
      char buffer[header.cix_nbytes + 1];
      recv_packet(server, buffer, header.cix_nbytes);
      log << "received " << header.cix_nbytes << " bytes" << endl;
      buffer[header.cix_nbytes] = '\0';
      ofstream ofs {header.cix_filename};
      if(ofs.fail()){
         log << header.cix_filename
             << ": ifstream failed: "
             << strerror (errno) << endl;
         return;
      }
      ofs.write (buffer, sizeof buffer);
      ofs.close();
      log << header.cix_filename << " has been created" << endl; 
      memset (header.cix_filename, 0, CIX_FILENAME_SIZE);
      header.cix_nbytes = 0;
   }
}

// cix_rm
void cix_rm(const char* filename, client_socket& server){
   cix_header header;
   header.cix_command = CIX_RM;
   for(size_t i = 0; i < strlen(filename); ++i){
      header.cix_filename[i] = filename[i];
   }
   log << "sending header " << header << endl;
   send_packet (server, &header, sizeof header);
   recv_packet (server, &header, sizeof header);
   log << "received header " << header << endl;
   if (header.cix_command != CIX_ACK) {
      log << "sent CIX_RM, server did not return CIX_ACK" << endl;
      log << "server returned " << header << endl;
   }else {
      char buffer[header.cix_nbytes + 1];
      log << "deleted " << header.cix_filename << endl;
      buffer[header.cix_nbytes] = '\0';
      cout << buffer;
   }
}


void usage() {
   cerr << "Usage: " << log.execname() << " [host] [port]" << endl;
   throw cix_exit();
}

bool SIGINT_throw_cix_exit {false};
void signal_handler (int signal) {
   log << "signal_handler: caught " << strsignal (signal) << endl;
   switch (signal) {
      case SIGINT: case SIGTERM: SIGINT_throw_cix_exit = true; break;
      default: break;
   }
}

string trim (const string &str) {
   size_t first = str.find_first_not_of (" \t");
   if (first == string::npos){
      return "";
   }
   size_t last = str.find_last_not_of (" \t");
   string s = str.substr (first, last - first + 1);
   return str.substr (first, last - first + 1);
}

int main (int argc, char** argv) {
   log.execname (basename (argv[0]));
   log << "starting" << endl;
   vector<string> args (&argv[1], &argv[argc]);
   signal_action (SIGINT, signal_handler);
   signal_action (SIGTERM, signal_handler);
   if (args.size() > 2) usage();
   string host = get_cix_server_host (args, 0);
   in_port_t port = get_cix_server_port (args, 1);
   log << to_string (hostinfo()) << endl;
   try {
      log << "connecting to " << host << " port " << port << endl;
      client_socket server (host, port);
      log << "connected to " << to_string (server) << endl;
      for (;;) {
         string line;
         getline (cin, line);
         if (cin.eof()) throw cix_exit();
         if (SIGINT_throw_cix_exit) throw cix_exit();
         log << "command " << line << endl;
         size_t split_pos = line.find_first_of(" ");
         string command = "";
         string file = "";
         if (split_pos != string::npos){
            command = trim (line.substr (0, split_pos));
            file = trim (line.substr (split_pos + 1));
         }
         else{
            command = line;
         }
         const char* file_cpy = file.string::c_str();
         const auto& itor = command_map.find (command);
         cix_command cmd = itor == command_map.end()
                         ? CIX_ERROR : itor->second;
         switch (cmd) {
            case CIX_EXIT:
               throw cix_exit();
               break;
            case CIX_GET:
               cix_get (server, file_cpy);
               break;
            case CIX_HELP:
               cix_help();
               break;
            case CIX_LS:
               cix_ls (server);
               break;
            case CIX_PUT:
               cix_put (server, file_cpy);
               break;
            case CIX_RM:
               cix_rm (file_cpy, server);
               break;
            default:
               log << line << ": invalid command" << endl;
               break;
         }
      }
   }catch (socket_error& error) {
      log << error.what() << endl;
   }catch (cix_exit& error) {
      log << "caught cix_exit" << endl;
   }
   log << "finishing" << endl;
   return 0;
}

