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
///                                                                   Macros                                                                      ///
///                                                                                                                                               ///
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define  ensure(condition, description) \
         [&]{bool r{condition}; current_results.emplace_back(description, r); if(!r && current_status) current_status = r;}()

// ensure that the contents of two primitive-type containers are equal
#define  ensure_equal_containers(a, b, description) \
         ensure(a.size() == b.size() && std::equal(a.begin(), a.end(), b.begin()), description)

// ensure that the contents of two string containers are equal
#define  ensure_equal_string_containers(a, b, description) \
         ensure(a.size() == b.size() && std::equal \
               (a.begin(), a.end(), b.begin(), [](std::string const &x, std::string const &y){return (x.compare(y) == 0);}), description)


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                                                                                                                               ///
///                                                              Static Variables                                                                 ///
///                                                                                                                                               ///
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// records the status of the tests run so far
static bool current_status = true;

// records the current results
static std::list<mdt::test::result> current_results;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                                                                                                                               ///
///                                                               Test Namespaces                                                                 ///
///                                                                                                                                               ///
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
      ensure_equal_string_containers(input, INPUT, "strings: add from a container doesn't change the container");

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
            ensure_equal_string_containers(input, output, "strings: add -> start thread -> sync = flushed output");

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
            ensure_equal_string_containers(input, output, "strings: start thread -> add -> sync = flushed output");

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
            ensure_equal_string_containers(input, output, "strings: start thread -> pause -> add -> un-pause -> sync = flushed output");

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
      ensure_equal_string_containers(input, output, "strings: start thread -> add -> pause -> stop thread = flushed output");

      // clear the output for the next test
      output.clear();

      {
         // start the queue thread
         local(q.go());

         /**
          ** (5) Ensure that re-starting the queue thread from the same parent thread works, and that adding elements, not synchronizing,
          **     and stopping the thread results in the elements being processed.
          **/

         // add some more stuff
         for(auto i : input) q.add(i);

         // end the queue thread
      }

      // ensure that the output is now equal to the input
      ensure_equal_string_containers(input, output, "strings: restart thread -> add -> stop thread = flushed output");
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
            ensure_equal_containers(input, output, "integers: add -> start thread -> sync = flushed output");

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
            ensure_equal_containers(input, output, "integers: start thread -> add -> sync = flushed output");

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
            ensure(output.empty(), "integers: start thread -> pause -> add = no output");

            // un-pause the queue
            q.pause(false);

            // wait until all elements have been processed
            q.sync();

            // ensure that the output is again equal to the input
            ensure_equal_containers(input, output, "integers: start thread -> pause -> add -> un-pause -> sync = flushed output");

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
      ensure_equal_containers(input, output, "integers: start thread -> add -> pause -> stop thread = flushed output");

      // clear the output for the next test
      output.clear();

      {
         // start the queue thread
         local(q.go());

         /**
          ** (5) Ensure that re-starting the queue thread from the same parent thread works, and that adding elements, not synchronizing,
          **     and stopping the thread results in the elements being processed.
          **/

         // add some more stuff
         for(auto i : input) q.add(i);

         // end the queue thread
      }

      // ensure that the output is now equal to the input
      ensure_equal_containers(input, output, "integers: restart thread -> add -> stop thread = flushed output");
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
