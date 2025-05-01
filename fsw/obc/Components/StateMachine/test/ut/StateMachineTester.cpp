// ======================================================================
// \title  StateMachineTester.cpp
// \author g83r
// \brief  cpp file for StateMachine component test harness implementation class
// ======================================================================

#include "StateMachineTester.hpp"

namespace StateMachineModule {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

StateMachineTester ::StateMachineTester()
    : StateMachineGTestBase("StateMachineTester",
                            StateMachineTester::MAX_HISTORY_SIZE),
      component("StateMachine") {
  this->initComponents();
  this->connectPorts();
}

StateMachineTester ::~StateMachineTester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void StateMachineTester ::toDo() {
  // TODO
}

} // namespace StateMachineModule
