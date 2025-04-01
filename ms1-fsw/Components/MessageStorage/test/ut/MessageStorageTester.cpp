// ======================================================================
// \title  MessageStorageTester.cpp
// \author g83r
// \brief  cpp file for MessageStorage component test harness implementation
// class
// ======================================================================

#include "MessageStorageTester.hpp"

namespace MessageModule {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

MessageStorageTester ::MessageStorageTester()
    : MessageStorageGTestBase("MessageStorageTester",
                              MessageStorageTester::MAX_HISTORY_SIZE),
      component("MessageStorage") {
  this->initComponents();
  this->connectPorts();
}

MessageStorageTester ::~MessageStorageTester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void MessageStorageTester ::toDo() {
  // TODO
}

} // namespace MessageModule
