#ifndef WRAP_HPP_
#define WRAP_HPP_

// std::function()
#include <functional>

namespace mdt
{
   /**
    * Simple RAII wrapper class.
    */
   class defer
   {
      // store a function to call later
      public: defer(std::function<void()> f)
         :
         stop{f}
      {}

      // disallow copy constructor
      public: defer(defer const &) = delete;

      // disallow copy via assignment operator
      public: defer & operator=(defer const &) = delete;

      // allow move constructor
      public: defer(defer &&) = default;

      // allow move via assignment operator
      public: defer & operator=(defer &&) = default;

      // run stop(), if it exists
      public: ~defer() throw()
      {
         if(stop) stop();
      }

      // stores a function to run when the wrap object is destroyed
      private: std::function<void()> stop;
   };
}

#endif /* WRAP_HPP_ */
