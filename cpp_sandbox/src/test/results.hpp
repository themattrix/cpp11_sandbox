#ifdef MDT_SELF_TEST

#ifndef RESULTS_HPP_
#define RESULTS_HPP_

namespace mdt { namespace test
{
   class result
   {
      // store a success or fail result tied to a description of the test
      public: result(std::string const &description, bool success = true);

      // return true if this is a success result
      public: auto is_success() const -> bool;

      // return the test description
      public: auto get_description() const -> std::string const &;

      // return a nicely-formatted string representation of this instance for display purposes
      public: auto to_string() const -> std::string;

      // records the test description
      private: std::string description;

      // records the test result
      private: bool success;
   };
}}

#endif /* RESULTS_HPP_ */

#endif
