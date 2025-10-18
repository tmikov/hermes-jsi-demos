# evloop

This example builds on the [hf-runner](../hf-runner) example. It adds a simple event
loop, which allows the JavaScript code to register callbacks using setTimeout() and
setImmediate(). It also enables micro-task support, which in turn enables WeakRef
support.

## Usage

```
evloop <js-file> [<shared-lib> ...]
```

## How the Event Loop Works

The event loop implementation is split between JavaScript (jslib.js.inc) and C++ (evloop.cpp), with the task queue maintained entirely in JavaScript.

### Architecture Overview

**JavaScript Side (jslib.js.inc):**
- Maintains a sorted array of macro tasks (`tasks[]`)
- Each task contains: `{id, fn, deadline, args}`
- Tasks are kept sorted by deadline for efficient scheduling
- Provides `setTimeout()`, `clearTimeout()`, `setImmediate()`, and `clearImmediate()` global functions
- Exposes two helper functions to C++:
  - `peek()`: Returns the deadline of the next task, or -1 if queue is empty
  - `run(currentTime)`: Executes the next task if its deadline has passed

**C++ Side (evloop.cpp):**
- Implements the main event loop using `std::chrono` for timing
- Manages the runtime lifecycle and microtask queue
- Coordinates with JavaScript task queue via the helper functions

### Event Loop Execution Flow

1. **Initialization** (lines 138-155 in evloop.cpp):
   - Enable microtask queue in runtime config: `withMicrotaskQueue(true)`
   - Evaluate jslib.js.inc to install setTimeout/setImmediate and get helper functions
     - Note: jslib.js.inc is a text file containing JavaScript wrapped in a C++ raw string literal `R"(...)"`
     - It's included directly into the C++ source via `#include "jslib.js.inc"` and assigned to a `const char*`
     - This embeds the JavaScript library code directly into the compiled binary
   - Initialize event loop's current time with first `run()` call
   - Load any native libraries
   - Execute the main JavaScript file

2. **Main Event Loop** (lines 168-191 in evloop.cpp):
   ```cpp
   while ((nextTimeMs = peekMacroTask.call(*runtime).getNumber()) >= 0) {
       // Get current time
       // If next task is in the future, sleep until then
       // Run the next macro task
       // Drain microtasks
   }
   ```

3. **Task Scheduling** (lines 23-35 in jslib.js.inc):
   - `setTimeout(fn, ms, ...args)` creates a task with deadline = currentTime + ms
   - Task is inserted into the sorted `tasks[]` array by deadline
   - Returns a unique task ID for cancellation

4. **Task Execution** (lines 15-21 in jslib.js.inc):
   - C++ calls `run(currentTime)` with the actual time
   - JavaScript updates its `curTime` variable
   - If the first task's deadline has passed, it's removed from the queue and executed
   - After macro task execution, C++ calls `runtime->drainMicrotasks()`

### Task Queue Management

**Insertion (setTimeout):**
Tasks are inserted in O(n) time to maintain sorted order by deadline:
```javascript
for (i = 0; i < tasks.length; ++i) {
    if (tasks[i].deadline > task.deadline) {
        break;
    }
}
tasks.splice(i, 0, task);
```

**Cancellation (clearTimeout):**
Searches linearly through the queue and removes the task by ID:
```javascript
for (var i = 0; i < tasks.length; i++) {
    if (tasks[i].id === id) {
        tasks.splice(i, 1);
        break;
    }
}
```

**Execution (runMacroTask):**
Always executes from the front of the queue (earliest deadline):
```javascript
if (tasks.length && tasks[0].deadline <= tm) {
    var task = tasks.shift();
    task.fn.apply(undefined, task.args);
}
```

### Microtasks vs Macrotasks

- **Macrotasks**: setTimeout/setImmediate callbacks, managed by JavaScript queue
- **Microtasks**: Promise callbacks, managed by Hermes runtime
- After each macrotask execution, `runtime->drainMicrotasks()` runs all pending microtasks before the next macrotask
- This ensures proper Promise resolution timing and enables WeakRef support

### Timing and Sleep

The C++ event loop:
- Uses `std::chrono::steady_clock` for monotonic time
- Calculates sleep duration: `nextTimeMs - currentTimeMs`
- Sleeps using `std::this_thread::sleep_until()` for precise wakeup
- Re-queries time after sleeping to account for scheduling delays

### setImmediate Implementation

`setImmediate()` is implemented as `setTimeout(fn, 0, ...args)`, meaning:
- Task deadline is set to current time (no delay)
- Still queued as a macrotask, not executed synchronously
- Will run in the next iteration of the event loop
- Microtasks scheduled during current execution will run first

### Example

See `timer.js` for a simple example that prints seconds 1-5 using setTimeout.
