
// std::cout, std::endl
#include <iostream>

// mdt::test::output_all()
#include "test/run_tests.hpp"

auto main() -> int
{
#ifdef MDT_SELF_TEST
   std::cout << mdt::test::run_all().to_string() << std::endl;
#endif
}
