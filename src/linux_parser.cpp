#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.emplace_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// DONE: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  int memtotal;
  int memfree;
  string line;
  string key;
  int value;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "MemTotal:") {
        memtotal = value;
      }
      if (key == "MemFree:") {
        memfree = value;
      }
    }
  }
  return (float)(memtotal - memfree) / memtotal;
}

// DONE: Read and return the system uptime
long LinuxParser::UpTime() {
  long uptime;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    if (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> uptime;
    }
  }
  return uptime;
}

// DONE: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  auto cpu_data = LinuxParser::CpuUtilization();
  long total_jiffies = 0;
  for (int i = 0; i < 10; i++) {
    total_jiffies += stol(cpu_data[i]);
  }
  return total_jiffies;
}

// DONE: Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  long total_time = 0;
  string line, tmp;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    for (int i = 0; i < 22; i++) {
      stream >> tmp;
      if (i == 14 || i == 15) total_time += stol(tmp);
    }
  }
  return total_time;
}

// DONE: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  auto cpu_data = LinuxParser::CpuUtilization();
  long active_jiffies = 0;
  for (int i = 0; i < 10; i++) {
    if (i != 3 && i != 4) active_jiffies += stol(cpu_data[i]);
  }
  return active_jiffies;
}

// DONE: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  auto cpu_data = LinuxParser::CpuUtilization();
  long idle_jiffies = 0;
  for (int i = 0; i < 10; i++) {
    if (i == 3 || i == 4) idle_jiffies += stol(cpu_data[i]);
  }
  return idle_jiffies;
}

// DONE: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  vector<string> cpuinfo{};
  string line, word;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);  // since we know it's the first line
    std::stringstream stream(line);
    while (getline(stream, word, ' ')) {
      if (isdigit(word[0])) {
        cpuinfo.emplace_back(word);
      }
    }
  }

  return cpuinfo;
}

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  int totalprocesses;
  string line;
  string key;
  int value;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "processes") {
        totalprocesses = value;
      }
    }
  }
  return totalprocesses;
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  int runningprocesses;
  string line;
  string key;
  int value;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "procs_running") {
        runningprocesses = value;
      }
    }
  }
  return runningprocesses;
}

// DONE: Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  string command = "";
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  std::getline(stream, command);
  return command;
}

// DONE: Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  long memory = 0;
  string line, key;
  long value;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "VmSize:") {
        memory = value;
      }
    }
  }
  return to_string(memory / 1000);
}

// DONE: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  int uid = 0, value;
  string key, line;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "Uid:") {
        uid = value;
      }
    }
  }
  return to_string(uid);
}

// DONE: Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string target_uid = LinuxParser::Uid(pid);
  string line, user, temp, uid;
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> user >> temp >> uid;
      if (uid == target_uid) {
        return user;
      }
    }
  }
  return "";
}

// DONE: Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  long starttime;
  string line, tmp;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    for (int i = 0; i < 22; i++) {
      stream >> tmp;
    }
  }
  starttime = stol(tmp) / sysconf(_SC_CLK_TCK);
  long system_uptime = LinuxParser::UpTime();
  return (system_uptime - starttime);
}
