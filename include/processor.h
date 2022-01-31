#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "linux_parser.h"

class Processor {
 public:
  Processor();
  float Utilization();  // Done: See src/processor.cpp

  // DONE: Declare any necessary private members
 private:
  long prev_jiffies;
  long prev_active;
};

#endif