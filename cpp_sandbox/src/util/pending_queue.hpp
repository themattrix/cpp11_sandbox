#ifndef PENDING_QUEUE_HPP_
#define PENDING_QUEUE_HPP_

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                                                                                                                               ///
///                                                                  Includes                                                                     ///
///                                                                                                                                               ///
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// std::condition_variable()
#include <condition_variable>

// std::function(), std::bind()
#include <functional>

// std::mutex(), std::unique_lock(), std::lock_guard()
#include <mutex>

// std::queue()
#include <queue>

// std::thread()
#include <thread>

// std::move()
#include <utility>

// mdt::wrap()
#include "defer.hpp"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                                                                                                                               ///
///                                                                Public Macros                                                                  ///
///                                                                                                                                               ///
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// local() macro which creates an 'unused' variable for RAII purposes
#define LOCAL_HELPER_3(x, y) x##y
#define LOCAL_HELPER_2(x, y) LOCAL_HELPER_3(x, y)
#define LOCAL_HELPER_1(x) LOCAL_HELPER_2(x, __COUNTER__)
#define local(_args) auto LOCAL_HELPER_1(_unused_)(_args)


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                                                                                                                               ///
///                                                           pending_queue Definition                                                            ///
///                                                                                                                                               ///
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace mdt
{
   /**
    * Thread-safe queue into which elements can be enqueued from any thread, but are processed sequentially by the thread's internal thread.
    */
   template<class T>
   class pending_queue
   {
      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Type Definitions ///
      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

      // stored element type, provided for cases in which the type is difficult to deduce
      public: typedef T element_type;

      // the type of this templated class, provided for cases in which the type is difficult to deduce
      public: typedef pending_queue<element_type> class_type;


      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Functions ///
      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

      // construct a pending queue with the specified call-back
      public: pending_queue(std::function<void(element_type)> callback)
         :
         callback{callback},
         ending{false},
         paused{false}
      {}

      // disallow copying via copy constructor
      public: pending_queue(class_type const &) = delete;

      // disallow copying via assignment operator
      public: class_type & operator=(class_type const &) = delete;

      // move via move constructor
      public: pending_queue(class_type &&other)
         :
         // copy the trivial types
         ending{other.ending},
         paused{other.paused}
      {
         // swap the complex types
         queue.swap(other.queue);
         callback.swap(other.callback);
         thread.swap(other.thread);
      }

      // move via assignment operator
      public: class_type & operator=(class_type &&other)
      {
         // copy the trivial types
         ending = other.ending;
         paused = other.paused;

         // swap the complex types
         queue.swap(other.queue);
         callback.swap(other.callback);
         thread.swap(other.thread);

         return *this;
      }

      // empty destructor
      public: ~pending_queue() {}

      // create a 'defer' instance which will call end() when it goes out of scope
      public: auto go() -> defer
      {
         // execute run() now...
         run();

         // ...store end() for later
         return {std::bind(&class_type::end, this)};
      }

      // start the internal thread for processing queued elements
      private: void run()
      {
         // make this function thread-safe *except with
         std::lock_guard<std::mutex> lock{queue_lock};

         // allow re-starting of the thread
         ending = false;

         // run the process() function in a new thread
         thread = std::thread{&class_type::process, this};
      }

      // process any remaining queued elements, stop the internal thread, and reject new elements
      private: void end()
      {
         {
            // make this function thread-safe
            std::lock_guard<std::mutex> lock{queue_lock};

            // notify the process() function to process remaining elements and exit
            ending = true;
            event.notify_one();
         }

         // wait for the process() thread to exit
         thread.join();
      }

      // wait until all of the currently pending elements have been processed
      public: void sync()
      {
         // ensure that no new elements are added to the queue while we're interacting with it
         std::unique_lock<std::mutex> lock{queue_lock};

         // loop while the queue is not empty
         while(!queue.empty())
         {
            empty.wait(lock);
         }
      }

      // pause or un-pause the queue
      public: void pause(bool pause = true)
      {
         // make this function thread-safe
         std::lock_guard<std::mutex> lock(queue_lock);

         // only take action if the new pause state is a change
         if(paused != pause)
         {
            // assign the new state, and if the new state is un-paused...
            if(!(paused = pause))
            {
               // ...notify the process() thread that it is no longer paused
               event.notify_one();
            }
         }
      }

      // move a new element onto the queue (unless end() has been called, in which case the element will be thrown back to the caller)
      public: void add(element_type element)
      {
         // make this function thread-safe
         std::lock_guard<std::mutex> lock(queue_lock);

         // after end() is called, no additional elements are allowed to be added...
         if(ending)
         {
            // ...so throw the element back to the caller
            throw std::move(element);
         }

         // otherwise, move the element onto the queue...
         queue.push(std::move(element));

         // ...and notify the process() thread that an event has occurred
         event.notify_one();
      }

      // internal element processor running on the thread started by run()
      private: void process()
      {
         while(true)
         {
            // create a temporary element for storing the popped-off element
            element_type element;

            {
               // ensure that no new elements are added to the queue while we're interacting with it
               std::unique_lock<std::mutex> lock{queue_lock};

               // loop until an element has been added to the queue, or while the queue is paused
               while(queue.empty() || (paused && ! ending))
               {
                  // after end() has been called and once the queue is empty...
                  if(ending)
                  {
                     // ...stop processing on this thread
                     return;
                  }

                  // wait for a new element to be added to the queue (or for end() to be called)
                  event.wait(lock);
               }

               // since an element exists in the queue, move it to the temporary element...
               element = std::move(queue.front());

               // ...pop it off the queue...
               queue.pop();
            }

            // ...and move it to the call-back
            callback(std::move(element));

            {
               // ensure that no new elements are added to the queue while we're interacting with it
               std::unique_lock<std::mutex> lock{queue_lock};

               // if the queue is now empty...
               if(queue.empty())
               {
                  // notify the sync() function *after* the callback is called
                  empty.notify_one();
               }
            }
         }
      }


      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Variables ///
      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

      // queue of the pending elements which have yet to be handled by the process() function
      private: std::queue<element_type> queue;

      // supplied by the pending_queue creator, this is called for each element processed by the process() function
      private: std::function<void(element_type)> callback;

      // runs the process() function
      private: std::thread thread;

      // the end() function was called; accept no new input and process the remaining queue items
      private: bool ending;

      // true if the queue is paused (i.e., accepting new input but not processing it)
      private: bool paused;

      // makes 'queue' and 'ending' thread-safe
      private: std::mutex queue_lock;

      // signals each time an item is added to the queue
      private: std::condition_variable event;

      // signals each time the queue is empty
      private: std::condition_variable empty;
   };
}

#ifdef MDT_SELF_TEST
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                                                                                                                               ///
///                                                                    Self-Tests                                                                 ///
///                                                                                                                                               ///
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// std::list
#include <list>

// mdt::test::result
#include "../test/results.hpp"

namespace mdt { namespace test { namespace pending_queue
{
   // run all pending_queue self-tests
   void all();

   // returns true if all tests run so far have passed
   auto passed() -> bool;

   // get the results of the run
   auto results() -> std::list<result> const &;
}}}
#endif

#endif /* PENDING_QUEUE_HPP_ */
