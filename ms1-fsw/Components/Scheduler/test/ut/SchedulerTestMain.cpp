// ======================================================================
// \title  SchedulerTestMain.cpp
// \author g83r
// \brief  cpp file for Scheduler component test main function
// ======================================================================

#include "SchedulerTester.hpp"

TEST(Nominal, toDo) {
  SchedulerModule::SchedulerTester tester;
  tester.toDo();
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
