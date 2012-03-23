#ifdef MDT_SELF_TEST

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                                                                                                                               ///
///                                                                  Includes                                                                     ///
///                                                                                                                                               ///
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// std::equal()
#include <algorithm>

// std::cout, std::endl
#include <iostream>

// std::list
#include <list>

// std::string
#include <string>

// std::vector
#include <vector>

// mdt::pending_queue
#include "pending_queue.hpp"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                                                                                                                               ///
///                                                               Test Namespaces                                                                 ///
///                                                                                                                                               ///
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace mdt { namespace test
{
   // returns true if the two containers have identical contents
   template<typename Ta, typename Tb>
   auto equal_containers(Ta const &a, Tb const &b) -> bool
   {
      return (a.size() == b.size() && std::equal(a.begin(), a.end(), b.begin()));
   }
}}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////// pending_queue<std::string> tests ///
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// std::string tests for pending_queue
namespace mdt { namespace test { namespace pending_queue { namespace strings
{
   const std::vector<std::string> INPUT
   {
      "testing", "this", "rather", "fine", "looking", "pending_queue"
   };

   static auto all() -> test::result
   {
      test::result result("std::string tests");

      std::vector<std::string> input(INPUT);
      std::list  <std::string> output;

      // create a new pending queue which appends to 'output'
      mdt::pending_queue<std::string> o([&](std::string s){output.push_back(std::move(s));});

      // ensure that the queue can be moved
      auto q = std::move(o);

      // add some stuff before starting the queue thread
      for(auto i : input) q.add(i);

      // ensure that the output queue is empty (to verify that the call-back is not called synchronously)
      result << test::result{"asynchronous call-back", output.empty()};

      // ensure that the input has not changed contents
      result << test::result{"add from a container doesn't change the container", equal_containers(input, INPUT)};

      {
         // start the queue thread
         local(q.go());

         /**
          ** (1) Ensure that adding stuff to the pending queue before the queue's thread is started, then starting and synchronizing the queue,
          **     results in the output queue being equal to the input queue.
          **/
         {
            // wait until all elements have been processed
            q.sync();

            // ensure that the output is now equal to the input
            result << test::result{"add -> start thread -> sync = flushed output", equal_containers(input, output)};

            // clear the output for the next test
            output.clear();
         }

         /**
          ** (2) Ensure that adding stuff to the pending queue after the queue's thread is started, then synchronizing the queue, results in the
          **     output queue being equal to the input queue.
          **/
         {
            // add some more stuff while not paused and while thread is running
            for(auto i : input) q.add(i);

            // wait until all elements have been processed
            q.sync();

            // ensure that the output is again equal to the input
            result << test::result{"start thread -> add -> sync = flushed output", equal_containers(input, output)};

            // clear the output for the next test
            output.clear();
         }

         /**
          ** (3) Ensure that adding stuff to the pending queue while the queue is paused, then un-pausing and synchronizing the queue, results in
          **     the output queue being equal to the input queue.
          **/
         {
            // pause the queue
            q.pause();

            // add some more stuff while paused
            for(auto i : input) q.add(i);

            // ensure that the output queue has not had anything added to it yet
            result << test::result{"start thread -> pause -> add = no output", output.empty()};

            // un-pause the queue
            q.pause(false);

            // wait until all elements have been processed
            q.sync();

            // ensure that the output is again equal to the input
            result << test::result{"start thread -> pause -> add -> un-pause -> sync = flushed output", equal_containers(input, output)};

            // clear the output for the next test
            output.clear();
         }

         /**
          ** (4) Ensure that stopping a paused queue while elements are still present in it results in the elements being processed.
          **/
         {
            // pause the queue
            q.pause();

            // add some more stuff while paused
            for(auto i : input) q.add(i);
         }

         // end the queue thread
      }

      // ensure that the output is again equal to the input
      result << test::result{"start thread -> add -> pause -> stop thread = flushed output", equal_containers(input, output)};

      // clear the output for the next test
      output.clear();

      /**
       ** (5) Ensure that re-starting the queue thread from the same parent thread works, and then adding elements, not synchronizing,
       **     and stopping the thread results in the elements being processed.
       **/
      {
         // start the queue thread
         local(q.go());

         // add some more stuff
         for(auto i : input) q.add(i);

         // end the queue thread
      }

      // ensure that the output is now equal to the input
      result << test::result{"restart thread -> add -> stop thread = flushed output", equal_containers(input, output)};

      // clear the output for the next test
      output.clear();

      /**
       ** (6) Ensure that stopped queues throw back the inserted element.
       **/
      try
      {
         // attempt to add some more stuff
         for(auto i : input) q.add(i);

         result << test::result{"adding to a stopped queue should have thrown an exception", false};
      }
      catch(std::string const &e)
      {
         result << test::result{"adding to a stopped queue should throw the first element added after stopping", e == input[0]};
      }
      catch(...)
      {
         result << test::result{"caught wrong exception type (should be 'std::string')", false};
      }

      return result;
   }
}}}}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// pending_queue<int> tests ///
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// integer tests for pending_queue
namespace mdt { namespace test { namespace pending_queue { namespace ints
{
   const std::vector<int> INPUT
   {
      0, 1, 2, 3, 4, 5, 6, 7, 8, 9
   };

   static auto all() -> test::result
   {
      test::result result("integer tests");

      std::vector<int> input(INPUT);
      std::list  <int> output;

      // create a new pending queue which appends to 'output'
      mdt::pending_queue<int> o([&](int i){output.push_back(i);});

      // ensure that the queue can be moved
      auto q = std::move(o);

      // add some stuff before starting the queue thread
      for(auto i : input) q.add(i);

      // ensure that the output queue is empty (to verify that the call-back is not called synchronously)
      result << test::result{"asynchronous call-back", output.empty()};

      // ensure that the input has not changed contents
      result << test::result{"adding from a container doesn't change the container", equal_containers(input, INPUT)};

      /**
       ** (1) Ensure that starting and stopping the queue thread acts like a synchronization.
       **/
      {
         // start and end the queue thread
         local(q.go());
      }

      // ensure that the output is now equal to the input
      result << test::result{"restart thread -> add -> stop thread = flushed output", equal_containers(input, output)};

      // clear the output for the next test
      output.clear();

      /**
       ** (2) Ensure that stopped queues throw back the inserted element.
       **/
      try
      {
         // attempt to add some more stuff
         for(auto i : input) q.add(i);

         result << test::result{"adding to a stopped queue should have thrown an exception", false};
      }
      catch(int e)
      {
         result << test::result{"adding to a stopped queue should throw the first element added after stopping", e == input[0]};
      }
      catch(...)
      {
         result << test::result{"caught wrong exception type (should be 'int')", false};
      }

      return result;
   }
}}}}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// test interface ///
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace mdt { namespace test { namespace pending_queue
{
   // run all pending queue tests
   auto all() -> result
   {
      // create the base result then append the string and integer tests as children
      return result{"pending_queue tests"} << strings::all() << ints::all();
   }
}}}

#endif
