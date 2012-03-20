#ifndef STRING_HPP_
#define STRING_HPP_

// std::string
#include <string>

#define make_string std::string()

namespace mdt { namespace string
{
   template<typename T>
   auto append(std::string &&s, T const &t) -> std::string &&
   {
      return std::move(s += std::to_string(t));
   }

   template<> auto append<std::string>(std::string &&s, std::string const &t) -> std::string &&;

   template<> auto append<char>(std::string &&s, char const &t) -> std::string &&;

   template<> auto append<uint8_t>(std::string &&s, uint8_t const &t) -> std::string &&;
}}

namespace mdt
{
   template<typename T>
   auto operator<<(std::string &&s, T const &t) -> std::string &&
   {
      return string::append<T>(std::move(s), t);
   }

   auto operator<<(std::string &&s, char const *t) -> std::string &&;
}

#ifdef MDT_SELF_TEST
namespace mdt { namespace test { namespace string
{
   void all();
}}}
#endif


#endif /* STRING_HPP_ */
