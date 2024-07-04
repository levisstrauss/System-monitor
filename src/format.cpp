#include <string>
#include <iomanip>
#include "format.h"
using namespace std;
using std::string;

string Format::ElapsedTime(long seconds) {
  long  hours = (seconds / 3600);
  long  r1 = (seconds % 3600) ;
  long minutes = (r1/60);
  long remainder_second = (r1 % 60);
  std::ostringstream stream;
  stream << std::setw(2) << std::setfill('0') << hours << ":"
         << std::setw(2) << std::setfill('0') << minutes << ":"
         << std::setw(2) << std::setfill('0') << remainder_second;
  return stream.str();

}