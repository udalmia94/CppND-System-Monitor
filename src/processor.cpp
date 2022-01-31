#include "processor.h"

Processor::Processor() {
  prev_active = LinuxParser::ActiveJiffies();
  prev_jiffies = LinuxParser::Jiffies();
}

// DONE: Return the aggregate CPU utilization
float Processor::Utilization() {
  long total_jiffies = LinuxParser::Jiffies();
  long active_jiffies = LinuxParser::ActiveJiffies();

  long d_jiffies = total_jiffies - prev_jiffies;
  long d_active = active_jiffies - prev_active;

  prev_active = active_jiffies;
  prev_jiffies = total_jiffies;

  return ((float)d_active / d_jiffies);
}