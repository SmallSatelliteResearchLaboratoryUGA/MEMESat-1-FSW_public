// ======================================================================
// \title  EpsUartTestMain.cpp
// \author g83r
// \brief  cpp file for EpsUart component test main function
// ======================================================================

#include "EpsUartTester.hpp"

TEST(Nominal, toDo) {
  EpsModule::EpsUartTester tester;
  tester.toDo();
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
