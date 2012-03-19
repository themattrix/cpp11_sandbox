
// mdt::test::output_all()
#include "test/output.hpp"

auto main() -> int
{
#ifdef MDT_SELF_TEST
   mdt::test::output_all();
#endif
}
