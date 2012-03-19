#ifdef MDT_SELF_TEST

// std::cout, std::endl
#include <iostream>

// mdt::test::output_all()
#include "output.hpp"

// mdt::pending_queue
#include "../util/pending_queue.hpp"

namespace mdt { namespace test
{
   void output_all()
   {
      using namespace mdt::test::pending_queue;

      all();

      std::cout << (passed() ? ">>> passed self-test(s):" : ">>> FAILED self-test(s):") << std::endl;

      if(results().empty())
      {
         std::cout << "   (NONE)" << std::endl;
      }
      else
      {
         for(auto const &i : results())
         {
            std::cout << "   " << i.to_string() << std::endl;
         }
      }
   }
}}

#endif
