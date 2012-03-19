#ifdef MDT_SELF_TEST

// std::string
#include <string>

// mdt::test::result
#include "results.hpp"

namespace mdt { namespace test
{
   result::result(std::string const &description, bool success)
      :
      description{description},
      success{success}
   {}

   auto result::is_success() const -> bool
   {
      return success;
   }

   auto result::get_description() const -> std::string const &
   {
      return description;
   }

   auto result::to_string() const -> std::string
   {
      std::string s;

      s += (success ? "[success] " : "[FAILURE] ");
      s += description;

      return std::move(s);
   }
}}

#endif
