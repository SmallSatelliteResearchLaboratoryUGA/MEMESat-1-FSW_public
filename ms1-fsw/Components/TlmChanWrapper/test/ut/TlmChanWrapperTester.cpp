// ======================================================================
// \title  TlmChanWrapperTester.cpp
// \author g83r
// \brief  cpp file for TlmChanWrapper component test harness implementation
// class
// ======================================================================

#include "TlmChanWrapperTester.hpp"

namespace TlmChanModule {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

TlmChanWrapperTester ::TlmChanWrapperTester()
    : TlmChanWrapperGTestBase("TlmChanWrapperTester",
                              TlmChanWrapperTester::MAX_HISTORY_SIZE),
      component("TlmChanWrapper") {
  this->initComponents();
  this->connectPorts();
}

TlmChanWrapperTester ::~TlmChanWrapperTester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void TlmChanWrapperTester ::toDo() {
  // TODO
}

} // namespace TlmChanModule
