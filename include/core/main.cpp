#include"Application.hpp"

int main(int argc, char *argv[])
{
  std::unique_ptr<APPLICATION> test=std::make_unique<Application>();
  test->run();
  while(1)
  {};

return 0;
}