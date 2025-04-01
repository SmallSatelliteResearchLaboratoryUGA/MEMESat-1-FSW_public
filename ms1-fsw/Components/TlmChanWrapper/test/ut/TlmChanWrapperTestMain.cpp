// ======================================================================
// \title  TlmChanWrapperTestMain.cpp
// \author g83r
// \brief  cpp file for TlmChanWrapper component test main function
// ======================================================================

#include "TlmChanWrapperTester.hpp"

TEST(Nominal, toDo) {
  TlmChanModule::TlmChanWrapperTester tester;
  tester.toDo();
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
