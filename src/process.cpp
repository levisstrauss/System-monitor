#include <string>
#include <vector>
#include "process.h"
#include "linux_parser.h"
#include <unistd.h>
using std::string;
using std::to_string;
using std::vector;

Process::Process(const int id) :_id(id){}

int Process::Pid() { return _id; }

float Process::CpuUtilization() {
   const int totalTimeActiveSeconds = LinuxParser::ActiveJiffies(_id);
   const int processUpTimeSeconds = LinuxParser::UpTime(_id);
   const int totalTime =  processUpTimeSeconds;
   _cpuUtilization = 1.0 * totalTimeActiveSeconds/ sysconf(_SC_CLK_TCK) / totalTime;
  return _cpuUtilization;
}

string Process::Command() { return LinuxParser::Command(_id); }
string Process::Ram() { return LinuxParser::Ram(_id); }
string Process::User() { return LinuxParser::User(_id); }
long int Process::UpTime() { return LinuxParser::UpTime(_id); }
bool Process::operator>(Process const& a) const {
  return _cpuUtilization > a._cpuUtilization;
}