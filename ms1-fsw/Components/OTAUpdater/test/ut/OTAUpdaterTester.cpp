// ======================================================================
// \title  OTAUpdaterTester.cpp
// \author g83r
// \brief  cpp file for OTAUpdater component test harness implementation class
// ======================================================================

#include "OTAUpdaterTester.hpp"

namespace UpdaterModule {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

OTAUpdaterTester ::OTAUpdaterTester()
    : OTAUpdaterGTestBase("OTAUpdaterTester",
                          OTAUpdaterTester::MAX_HISTORY_SIZE),
      component("OTAUpdater") {
  this->initComponents();
  this->connectPorts();
}

OTAUpdaterTester ::~OTAUpdaterTester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void OTAUpdaterTester ::toDo() {
  // TODO
}

} // namespace UpdaterModule
