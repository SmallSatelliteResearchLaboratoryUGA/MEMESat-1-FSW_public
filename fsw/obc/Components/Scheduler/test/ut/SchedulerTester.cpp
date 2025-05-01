// ======================================================================
// \title  SchedulerTester.cpp
// \author g83r
// \brief  cpp file for Scheduler component test harness implementation class
// ======================================================================

#include "SchedulerTester.hpp"

namespace SchedulerModule {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

SchedulerTester ::SchedulerTester()
    : SchedulerGTestBase("SchedulerTester", SchedulerTester::MAX_HISTORY_SIZE),
      component("Scheduler") {
  this->initComponents();
  this->connectPorts();
}

SchedulerTester ::~SchedulerTester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void SchedulerTester ::toDo() {
  // TODO
}

} // namespace SchedulerModule
