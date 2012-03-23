#ifdef MDT_SELF_TEST

// assert()
#include <cassert>

// mdt::test::run_all()
#include "run_tests.hpp"

// mdt::test::result
#include "results.hpp"

// mdt::pending_queue
#include "../util/pending_queue.hpp"

// string helper functions
#include "../util/string.hpp"

namespace mdt { namespace test
{
   class result::private_data
   {
      public: private_data(std::string description, bool success) : description{description}, success{success} {}

      // description of the result or result group
      public: std::string description;

      // success status of the result
      public: bool success;

      // the first child result, if any
      public: result child;

      // the next result, if any
      public: result next;
   };

   result::result(std::string description, bool success)
      :
      // create the private data block with the description and success status filled out
      data(std::make_shared<private_data>(description, success))
   {}

   auto result::append_sibling(result &&next) -> result &
   {
      assert(data);

      // return a reference to the next node after adding it
      return(data->next ? data->next.append_sibling(std::move(next)) : (data->next = std::move(next)));
   }

   auto result::append_child(result &&child) -> result &
   {
      assert(data);

      // set success to false if the new child is already false
      if(!child.is_success()) data->success = false;

      // return a reference to the child node after adding it
      return(data->child ? data->child.append_sibling(std::move(child)) : (data->child = std::move(child)));
   }

   result::operator bool() const
   {
      // returns true if the private data instance exits
      return !!data;
   }

   auto result::is_success() const -> bool
   {
      assert(data);

      // return the success status
      return(data->success);
   }

   auto result::get_description() const -> std::string const &
   {
      assert(data);

      // return the description of the result
      return(data->description);
   }

   auto result::to_string() const -> std::string
   {
      // create a string to hold the result
      std::string result;

      // populate the string with every result
      to_string(result, 0);

      // return the result via move
      return result;
   }

   void result::to_string(std::string &input, size_t indentation) const
   {
      // if the private data doesn't exist, this result doesn't add to the input
      if(!data)
      {
         return;
      }

      // upper-case "FAILURE" is used to stand out against the lower-case "success" because it's more important
      input += '[';
      input += (data->success ? "success" : "FAILURE");
      input += "] ";

      // add the dotted line (if required) to the indented result description, leaving a space on both sides
      if(indentation > 1)
      {
         input += std::string(indentation - 1, '.');
         input += ' ';
      }

      // add the description
      input += data->description;

      // note that because of this, even the final line will be empty
      input += '\n';

      // add the same data for each child
      if(data->child)
      {
         data->child.to_string(input, indentation + 3);
      }

      // add the same data for the next sibling
      if(data->next)
      {
         data->next.to_string(input, indentation);
      }
   }

   auto operator<<(result &parent, result &&child) -> result &
   {
      // move the child into the parent and return a reference to the parent
      parent.append_child(std::move(child));
      return parent;
   }

   auto operator<<(result &&parent, result &&child) -> result &&
   {
      // move the child into the parent and move the parent back to the caller
      parent.append_child(std::move(child));
      return std::move(parent);
   }
}}

namespace mdt { namespace test
{
   auto run_all() -> result
   {
      // test all mdt namespace utilities and return the results
      return result{"mdt utility tests"}
             << test::pending_queue::all()
             << test::string::all();
   }
}}

#endif
