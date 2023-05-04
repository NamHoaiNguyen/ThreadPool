## A Simple C++ Thread Pool

## Introduction:
A thread pool is a technique that allows developers to exploit the concurrency of modern processors.
Threads are initialized once and remain inactive until some work has to be done. This way we minimize 
the overhead.


The repo is created as a lib which will be used to implement reactor pattern[^1].
This pattern is one of the way to handle C10k problem and applied in many applications(NodeJs, Qt, etc...).
The document about how reactor pattern is applied in NodeJs is attached in below[^2][^3].

## How to build.
Currently, the lib is Linux-centric.

```
// Linux
cd <project-folder>
mkdir build
cd build/
cmake ..
make
```

## How to use.
Create the thread pool very simple.
```
//Create thread pool with 7 threads
ThreadPool pool(7)

//Then initialize pool
pool.init()
```

To shutdown the threadpool, just call
```
pool.shutdown()
```

Put task into thread pool by calling
```
pool.submit(work).
```

Currently, threadpool supports, callable objects(lambda, functor, std::function, etc...), functions, methods.


##Future Work
* Implement for funtions, methods whose parameters are not copyable object.
* Run benchmarks and improve performance.


## References.
[^1]: Reactor Pattern(https://en.wikipedia.org/wiki/Reactor_pattern).
[^2]: (https://nodejs.org/en/docs/guides/dont-block-the-event-loop).
[^3]: (https://www.youtube.com/watch?v=P9csgxBgaZ8&t=408s).
