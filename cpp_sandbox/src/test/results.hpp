#ifndef RESULTS_HPP_
#define RESULTS_HPP_

// std::list
#include <list>

// std::string
#include <string>

// std::unique_ptr
#include <memory>

namespace mdt { namespace test
{
   /**
    * Interface for constructing a nested hierarchy of results.
    */
   class result
   {
      // create an empty result
      public: result() = default;

      // store a success or fail result tied to a description of the test
      public: result(std::string description, bool success = true);

      // default move via constructor operator
      public: result(result &&) = default;

      // default move via assignment operator
      public: result & operator=(result &&) = default;

      // disable copy via constructor operator
      public: result(result const &) = delete;

      // disable copy via assignment operator
      public: result & operator=(result const &) = delete;

      // default destructor
      public: ~result() = default;

      // insert a result as the last sibling and return a reference to the inserted result
      public: auto append_sibling(result &&next) -> result &;

      // insert a result as the last child of this result and return a reference to the inserted result
      public: auto append_child(result &&child) -> result &;

      // returns false if this is an empty result
      public: explicit operator bool() const;

      // return true if this is a success result (if this result has children, then the status is the combined status of all children)
      public: auto is_success() const -> bool;

      // return the test description
      public: auto get_description() const -> std::string const &;

      // return a nicely-formatted string representation of this instance for display purposes
      public: auto to_string() const -> std::string;

      // for internal use: append this string, with the given indentation, to the input string
      private: void to_string(std::string &input, size_t indentation) const;

      // forward-declaration of the class containing the private data
      private: class private_data;

      // owns the private data
      private: std::shared_ptr<private_data> data;
   };

   // append a child to this object and return the parent (unlike append_child() which returns the child)
   auto operator<<(result &parent, result &&child) -> result &;

   // same as above but allows for the parent to be an rvalue
   auto operator<<(result &&parent, result &&child) -> result &&;
}}

#endif /* RESULTS_HPP_ */
