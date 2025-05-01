// ======================================================================
// \title  EpsUartTester.cpp
// \author g83r
// \brief  cpp file for EpsUart component test harness implementation class
// ======================================================================

#include "EpsUartTester.hpp"

namespace EpsModule {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

EpsUartTester ::EpsUartTester()
    : EpsUartGTestBase("EpsUartTester", EpsUartTester::MAX_HISTORY_SIZE),
      component("EpsUart") {
  this->initComponents();
  this->connectPorts();
}

EpsUartTester ::~EpsUartTester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void EpsUartTester ::toDo() {
  // TODO
}

} // namespace EpsModule
