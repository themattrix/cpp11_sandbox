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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////// static variables and helper functions ///
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace mdt { namespace test
{
   // records the status of the tests run so far
   static bool current_status = true;

   // records the current results
   static std::list<mdt::test::result> current_results;

   // set a failure result
   static void failure(std::string const &description)
   {
      // construct a new failure result at the back of current_results
      current_results.emplace_back(description, false);

      // the overall testing status has now failed
      current_status = false;
   }

   // set a success result
   static void success(std::string const &description)
   {
      // construct a new success result at the back of the current_reuslts
      current_results.emplace_back(description, true);
   }

   // if condition is true, add a 'success' result, otherwise add a 'failure'
   static void ensure(bool condition, std::string const &description)
   {
      if(condition) success(description);
      else          failure(description);
   }

   // ensure that the contents of two containers are equal
   template<typename Ta, typename Tb>
   static void ensure_equal_containers(Ta const &a, Tb const &b, std::string const &description)
   {
      ensure(a.size() == b.size() && std::equal(a.begin(), a.end(), b.begin()), description);
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

   static void all()
   {
      std::vector<std::string> input(INPUT);
      std::list  <std::string> output;

      mdt::pending_queue<std::string> q([&](std::string s){output.push_back(std::move(s));});

      // add some stuff before starting the queue thread
      for(auto i : input) q.add(i);

      // ensure that the output queue is empty (to verify that the call-back is not called synchronously)
      ensure(output.empty(), "strings: asynchronous call-back");

      // ensure that the input has not changed contents
      ensure_equal_containers(input, INPUT, "strings: add from a container doesn't change the container");

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
            ensure_equal_containers(input, output, "strings: add -> start thread -> sync = flushed output");

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
            ensure_equal_containers(input, output, "strings: start thread -> add -> sync = flushed output");

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
            ensure(output.empty(), "strings: start thread -> pause -> add = no output");

            // un-pause the queue
            q.pause(false);

            // wait until all elements have been processed
            q.sync();

            // ensure that the output is again equal to the input
            ensure_equal_containers(input, output, "strings: start thread -> pause -> add -> un-pause -> sync = flushed output");

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
      ensure_equal_containers(input, output, "strings: start thread -> add -> pause -> stop thread = flushed output");

      // clear the output for the next test
      output.clear();

      /**
       ** (5) Ensure that re-starting the queue thread from the same parent thread works, and that adding elements, not synchronizing,
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
      ensure_equal_containers(input, output, "strings: restart thread -> add -> stop thread = flushed output");

      // clear the output for the next test
      output.clear();

      /**
       ** (6) Ensure that stopped queues throw back the inserted element.
       **/
      try
      {
         // attempt to add some more stuff
         for(auto i : input) q.add(i);

         failure("strings: adding to a stopped queue should have thrown an exception");
      }
      catch(std::string const &e)
      {
         ensure(e == input[0], "strings: adding to a stopped queue should throw the first element added after stopping");
      }
      catch(...)
      {
         failure("strings: caught wrong exception type (should be 'std::string')");
      }
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

   static void all()
   {
      std::vector<int> input(INPUT);
      std::list  <int> output;

      mdt::pending_queue<int> q([&](int i){output.push_back(i);});

      // add some stuff before starting the queue thread
      for(auto i : input) q.add(i);

      // ensure that the output queue is empty (to verify that the call-back is not called synchronously)
      ensure(output.empty(), "integers: asynchronous call-back");

      // ensure that the input has not changed contents
      ensure_equal_containers(input, INPUT, "integers: adding from a container doesn't change the container");

      /**
       ** (1) Ensure that starting and stopping the queue thread acts like a synchronization.
       **/
      {
         // start the queue thread
         local(q.go());

         // end the queue thread
      }

      // ensure that the output is now equal to the input
      ensure_equal_containers(input, output, "integers: restart thread -> add -> stop thread = flushed output");

      // clear the output for the next test
      output.clear();

      /**
       ** (2) Ensure that stopped queues throw back the inserted element.
       **/
      try
      {
         // attempt to add some more stuff
         for(auto i : input) q.add(i);

         failure("integers: adding to a stopped queue should have thrown an exception");
      }
      catch(int e)
      {
         ensure(e == input[0], "integers: adding to a stopped queue should throw the first element added after stopping");
      }
      catch(...)
      {
         failure("integers: caught wrong exception type (should be 'int')");
      }
   }
}}}}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// test interface ///
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace mdt { namespace test { namespace pending_queue
{
   void all()
   {
      // test the pending_queue with a bunch of strings
      strings::all();

      // test the pending_queue with a bunch of integers
      ints::all();
   }

   auto passed() -> bool
   {
      return current_status;
   }

   auto results() -> std::list<result> const &
   {
      return current_results;
   }
}}}

#endif
