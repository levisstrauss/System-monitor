#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <numeric>


#include "linux_parser.h"
using std::stof;
using std::string;
using std::to_string;
using std::vector;
using std::accumulate;
using std::stol;



template <typename T>
T findValueByKey(std::string const &keyFilter, std::string const &filename) {
  std::string line, key;
  T value;
  std::ifstream stream(LinuxParser::kProcDirectory + filename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == keyFilter) {
          return value;
        }
      }
    }
  }
  return value;
}


template <typename T>
T getValueOfFile(std::string const &filename) {
  std::string line;
  T value;
  std::ifstream stream(LinuxParser::kProcDirectory + filename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> value;
  }
  return value;
};


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


vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization() {
  string memTotal = "MemTotal:";
  string memFree = "MemFree:";
  float Total = findValueByKey<float>(memTotal, kMeminfoFilename);
  float Free = findValueByKey<float>(memFree, kMeminfoFilename);
  return (Total - Free) / Total;
}

long LinuxParser::UpTime() {
  string line;
  long upTime = 0;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream lineStream(line);
    lineStream >> upTime;
  }
  return upTime;
}

long LinuxParser::Jiffies() {
  return (LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies());
}


long LinuxParser::ActiveJiffies(int pid) {
  string active_jiffies, line;
  long uptime = 0, startTime = 0, currentTime = 0, currentSecondTime = 0;
  std::ifstream fileStream(kProcDirectory + to_string(pid) + kStatFilename);
  if(fileStream.is_open()){
    std::getline(fileStream, line);
    std::istringstream lineStream(line);
    for(int index = 0; index <13; index++){
      lineStream >> active_jiffies;
    }
    lineStream >> uptime >> startTime >> currentTime >> currentSecondTime;
  }
  return uptime + startTime + currentTime + currentSecondTime;
}


long LinuxParser::ActiveJiffies() {
  vector<string> cpuUtilization = LinuxParser::CpuUtilization();
  long user = stol(cpuUtilization[CPUStates::kUser_]);
  long nice = stol(cpuUtilization[CPUStates::kNice_]);
  long system = stol(cpuUtilization[CPUStates::kSystem_]);
  long irq = stol(cpuUtilization[CPUStates::kIRQ_]);
  long softIrq = stol(cpuUtilization[CPUStates::kSoftIRQ_]);
  long steal = stol(cpuUtilization[CPUStates::kSteal_]);
  return user + nice + system + irq + softIrq + steal;
}



long LinuxParser::IdleJiffies() {
  vector<string> cpuUtilization = LinuxParser::CpuUtilization();
  long idle = stol(cpuUtilization[CPUStates::kIdle_]);
  long iowait = stol(cpuUtilization[CPUStates::kIOwait_]);
  return idle + iowait;
}

vector<string> LinuxParser::CpuUtilization() {
  vector<string> cpu;
  string line, dataType;
  std::ifstream fileStream(kProcDirectory + kStatFilename);
  if (fileStream.is_open()){
    while (std::getline(fileStream, line)){
      std::istringstream lineStream(line);
      while(lineStream >> dataType){
        if (dataType == "cpu"){
          while(lineStream >> dataType){
            cpu.push_back(dataType);
          }
          return cpu;
        }
      }
    }
  }
  return cpu;
}


int LinuxParser::TotalProcesses() {
  string line, key;
  int value = 0;
  int totalProcesses = 0;
  std::ifstream fileStream(kProcDirectory + kStatFilename);
  if (fileStream.is_open()){
    while (std::getline(fileStream, line)){
      std::istringstream lineStream(line);
      while (lineStream >> key >> value) {
        if (key == "processes"){
          totalProcesses = value;
          break;
        }
      }
    }
  }
  return totalProcesses;

}


int LinuxParser::RunningProcesses() {
  int runningProcesses = 0, value = 0;
  string line, key;
  std::ifstream fileStream(kProcDirectory + kStatFilename);
  if (fileStream.is_open()){
    while (std::getline(fileStream, line)){
      std::istringstream lineStream(line);
      while (lineStream >> key >> value){
        if (key == "procs_running"){
          runningProcesses = value;
          break;
        }
      }

   }
  }
 return runningProcesses;
}


string LinuxParser::Command(int pid) {
  return std::string(getValueOfFile<std::string>(std::to_string(pid) + kCmdlineFilename));
}

// I have used VmRSS instead of VmSize
string LinuxParser::Ram(int pid) {
  string line, key, value;
  int vmRss = 0;
  std::ifstream fileStream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (fileStream.is_open()){
    while (std::getline(fileStream, line)){
      std::istringstream lineStream(line);
      while(lineStream >> key >> value) {
        if (key == "VmRSS:") {
          vmRss = std::stoi(value);
          vmRss = vmRss / 1000;
          return std::to_string(vmRss);
        }
      }
    }
  }
  return {};
}

string LinuxParser::Uid(int pid) {
  string line, key, value;
  std::ifstream fileStream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (fileStream.is_open()){
    while (std::getline(fileStream, line)){
      std::istringstream lineStream(line);
      lineStream >> key >> value;
      if(key == "Uid:"){
        return value;
      }
    }
  }
  return {};
}


string LinuxParser::User(int pid) {
  string uid, line, userName, password;
  std::ifstream fileStream(kPasswordPath);
  if(fileStream.is_open()){
    while(std::getline(fileStream, line)){
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream lineStream(line);
      lineStream >> userName >> password >> uid;
      if(uid == LinuxParser::Uid(pid)){
        return userName;
      }
    }
  }
  return userName;
}

long LinuxParser::UpTime(int pid) {
  long startTime;
  string line, skipped;
  std::ifstream fileStream(kProcDirectory + to_string(pid) + kStatFilename);
  if (fileStream.is_open()){
    std::getline(fileStream, line);
    std::istringstream lineStream(line);
    for(int i = 0; i < 21; i++){
      lineStream >> skipped;
    }
    lineStream >> startTime;
    return LinuxParser::UpTime() - startTime / sysconf(_SC_CLK_TCK);
  }
  return 0;
}
