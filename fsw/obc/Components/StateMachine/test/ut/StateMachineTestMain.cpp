// ======================================================================
// \title  StateMachineTestMain.cpp
// \author g83r
// \brief  cpp file for StateMachine component test main function
// ======================================================================

#include "StateMachineTester.hpp"

TEST(Nominal, toDo) {
  StateMachineModule::StateMachineTester tester;
  tester.toDo();
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
