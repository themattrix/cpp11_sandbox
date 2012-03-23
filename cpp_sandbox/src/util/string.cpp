// string helper functions
#include "string.hpp"

namespace mdt
{
   auto operator<<(std::string &&s, char const *t) -> std::string &&
   {
      return std::move(s += t);
   }

   auto operator<<(std::string &s, char const *t) -> std::string &
   {
      return (s += t);
   }

   auto operator<<(std::string &&s, std::string const &t) -> std::string &&
   {
      return std::move(s += t);
   }

   auto operator<<(std::string &s, std::string const &t) -> std::string &
   {
      return (s += t);
   }

   auto operator<<(std::string &&s, char t) -> std::string &&
   {
      return std::move(s += t);
   }

   auto operator<<(std::string &s, char t) -> std::string &
   {
      return (s += t);
   }

   auto operator<<(std::string &&s, uint8_t t) -> std::string &&
   {
      return std::move(s += static_cast<char>(t));
   }

   auto operator<<(std::string &s, uint8_t t) -> std::string &
   {
      return (s += static_cast<char>(t));
   }
}

#ifdef MDT_SELF_TEST
#include <iostream>

namespace mdt { namespace test { namespace string
{
   static const std::string TEST_STRING{"HELLO, C++11"};

   auto all() -> result
   {
      // explicitly specify this operator so that compiler can find it
      using mdt::operator<<;

      // establish a new result group
      test::result result{"string concatenation tests"};

      /**
       * (1) Ensure that lvalue concatenation works.
       */
      {
         // establish an lvalue
         std::string lvalue;

         // append to the lvalue
         lvalue << "HELLO, C" << '+' << static_cast<uint8_t>('+') << 1 << std::string("1");

         // ensure that the lvalue contains the desired string
         result << test::result{"lvalue concatenation", lvalue == TEST_STRING};
      }

      /**
       * (2) Ensure that rvalue concatenation works.
       */
      {
         // ensure that the rvalue contains the desired string
         result << test::result
         {
            "rvalue concatenation", (std::string() << "HELLO, C" << '+' << static_cast<uint8_t>('+') << 1 << std::string("1")) == TEST_STRING
         };
      }

      return result;
   }
}}}
#endif
