#include <string>
#include <iomanip>

#include "format.h"

using std::string;

// DONE: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
  int hours = seconds / 3600;
  seconds = seconds % 3600;
  int minutes = seconds / 60;
  seconds = seconds % 60;
  string output;
  std::stringstream stream;
  stream << std::setw(2) << std::setfill('0') << hours << ":" << std::setw(2)
         << std::setfill('0') << minutes << ":" << std::setw(2)
         << std::setfill('0') << seconds;
  return stream.str();
}