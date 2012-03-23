#ifndef STRING_HPP_
#define STRING_HPP_

// std::string
#include <string>

namespace mdt
{
   // append a C-string to an rvalue-std::string
   auto operator<<(std::string &&s, char const *t) -> std::string &&;

   // append a C-string to an lvalue-std::string
   auto operator<<(std::string &s, char const *t) -> std::string &;

   // append a std::string to an rvalue-std::string
   auto operator<<(std::string &&s, std::string const &t) -> std::string &&;

   // append a std::string to an lvalue-std::string
   auto operator<<(std::string &s, std::string const &t) -> std::string &;

   // append a character to an rvalue-std::string
   auto operator<<(std::string &&s, char t) -> std::string &&;

   // append a character to an lvalue-std::string
   auto operator<<(std::string &s, char t) -> std::string &;

   // append a character to an rvalue-std::string
   auto operator<<(std::string &&s, uint8_t t) -> std::string &&;

   // append a character to an lvalue-std::string
   auto operator<<(std::string &s, uint8_t t) -> std::string &;

   // append a generic type to an rvalue-std::string
   template<typename T> auto operator<<(std::string &&s, T const &t) -> std::string &&
   {
      return std::move(s += std::to_string(t));
   }

   // append a generic type to an lvalue-std::string
   template<typename T> auto operator<<(std::string &s, T const &t) -> std::string &
   {
      return (s += std::to_string(t));
   }
}

#ifdef MDT_SELF_TEST
#include "../test/results.hpp"

namespace mdt { namespace test { namespace string
{
   // run all string tests
   auto all() -> result;
}}}
#endif


#endif /* STRING_HPP_ */
