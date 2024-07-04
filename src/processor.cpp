#include "processor.h"
#include "linux_parser.h"

float Processor::Utilization() {
  const long jiffies = LinuxParser::Jiffies();
  const long activeJiffies = LinuxParser::ActiveJiffies();
  if (_lastActiveJiffies == 0 && _lastJiffies == 0) {
    _lastActiveJiffies = activeJiffies;
    _lastJiffies = jiffies;
    return (_lastActiveJiffies / _lastJiffies);
  }
  const float diff_active_jiffies = (activeJiffies - _lastActiveJiffies);
  const float  diff_jiffies = jiffies - _lastJiffies;
  _lastActiveJiffies = activeJiffies;
  _lastJiffies = jiffies;
  if (diff_active_jiffies == 0) {
    return 0.0;
  }else {
    return (diff_active_jiffies / diff_jiffies);
  }
}