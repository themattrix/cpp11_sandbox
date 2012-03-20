
// string helper functions
#include "string.hpp"

namespace mdt
{
   template<> auto string::append<std::string>(std::string &&s, std::string const &t) -> std::string &&
   {
      return std::move(s += t);
   }

   template<> auto string::append<char>(std::string &&s, char const &t) -> std::string &&
   {
      return std::move(s += t);
   }

   template<> auto string::append<uint8_t>(std::string &&s, uint8_t const &t) -> std::string &&
   {
      return std::move(s += static_cast<char>(t));
   }

   auto operator<<(std::string &&s, char const *t) -> std::string &&
   {
      return std::move(s += t);
   }
}

#ifdef MDT_SELF_TEST
#include <iostream>

namespace mdt { namespace test { namespace string
{
   void all()
   {
      std::cout << (make_string << "HELLO, " << "C+" << '+' << 1 << 1) << std::endl;
   }
}}}
#endif
