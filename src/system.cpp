#include <algorithm>
#include <cstddef>
#include <set>
#include <string>
#include <vector>
#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

using namespace std;
using std::set;
using std::size_t;
using std::string;
using std::vector;
using std::sort;

System::System() { cpu_ = Processor();}

Processor& System::Cpu() { return cpu_; }

vector<Process>& System::Processes() {
  set<int> uniqueId;
  for (Process process : processes_){
    uniqueId.insert(process.Pid());
  }

  std::vector<int> pids = LinuxParser::Pids();
  for(int pid : pids){
    if (uniqueId.find(pid) == uniqueId.end()){
      processes_.emplace_back(Process(pid));
    }
  }

  for(Process& process : processes_){
    process.CpuUtilization();
  }


  sort(processes_.begin(), processes_.end(), greater<Process>());
  return processes_;
}

std::string System::Kernel() { return LinuxParser::Kernel(); }
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }
std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }
long int System::UpTime() { return LinuxParser::UpTime(); }