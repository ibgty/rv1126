#include "Application.hpp"

int main()
{
  Application test_demo;
  test_demo.run();
  while(1)
  {
    // std::this_thread::sleep_for(std::chrono::seconds(10000));
  }; 
  return 0;
}