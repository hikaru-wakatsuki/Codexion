*This project has been created as part of the 42 curriculum by hwakatsu.*

# Codexion

## Description

Codexion is a concurrency simulation where multiple coders compete for limited USB dongles to compile. Each coder runs as a POSIX thread and must acquire two dongles simultaneously — one in each hand — before compiling. After compiling, they release the dongles and proceed to debug and refactor before attempting to compile again.

The simulation stops either when a coder burns out (fails to start compiling within `time_to_burnout` milliseconds since their last compile) or when all coders have compiled the required number of times.

The project explores thread synchronization, deadlock prevention, starvation avoidance, and fair resource arbitration using FIFO or EDF scheduling policies implemented with a custom priority queue (min-heap).

## Instructions

### Compilation

```bash
make
```

This compiles the project with `-Wall -Wextra -Werror -pthread` and produces the `codexion` executable.

```bash
make clean    # remove object files
make fclean   # remove object files and executable
make re       # full rebuild
```

### Execution

```
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

All arguments are mandatory. Times are in milliseconds. `scheduler` must be exactly `fifo` or `edf`.

**Example:**
```bash
./codexion 5 1200 200 100 100 4 50 edf
```

### Log format

Each state change is printed as:
```
timestamp_in_ms coder_id message
```

Messages: `has taken a dongle`, `is compiling`, `is debugging`, `is refactoring`, `burned out`.

## Blocking cases handled

**Deadlock prevention**

Deadlock arises when every coder holds one dongle and waits for the other (circular wait — one of Coffman's four conditions). This is eliminated by enforcing a global lock ordering: each coder always acquires the dongle with the smaller index first (`set_order`). Since all threads acquire locks in the same order, circular wait cannot occur.

**Starvation prevention**

Under FIFO scheduling, requests are served strictly in arrival order per dongle, ensuring every waiting coder will eventually be served. Under EDF, the coder with the earliest burnout deadline is prioritized, which reduces the risk of any coder missing their deadline when parameters are feasible. A tie-breaker on `arrival_seq` makes EDF fully deterministic.

To avoid starvation at simulation start, the first two rounds of dongle requests use a fixed `arrival_seq` assignment: odd-numbered coders receive `0` and even-numbered coders receive `1`. This ensures odd and even coders are interleaved from the very beginning rather than all competing at the same priority. When the total number of coders is odd, the one dongle shared only by two odd-numbered coders (dongle index 0) uses normal sequential ordering instead.

**Cooldown handling**

After a coder releases a dongle, it enters a cooldown period (`dongle_cooldown` ms) during which it cannot be taken. The `cooldown_until_ms` field is checked atomically inside the dongle's mutex in `can_take`, preventing any coder from acquiring a dongle before the cooldown expires.

**Precise burnout detection**

A dedicated monitor thread polls all coders every 1 ms. It checks `now - last_compile_start_ms >= time_to_burnout` under each coder's `state_mutex`. When burnout is detected, it sets `stop_simulation = true` under `stop_mutex` and logs the burnout message. The 1 ms polling interval guarantees the burnout log is printed within the required 10 ms window.

**Log serialization**

All output goes through `print_log`, which acquires `log_mutex` before calling `printf`. This prevents two messages from interleaving on the same line.

## Thread synchronization mechanisms

**`pthread_mutex_t` usage**

The project uses five categories of mutexes:

- `dongle->mutex` — protects `owner_coder_id`, `cooldown_until_ms`, `local_seq`, and `wait_queue` for each dongle. Acquired in index order (smaller dongle index first) to prevent deadlock.
- `coder->state_mutex` — protects `last_compile_start_ms` and `compile_count` for each coder. Read by the monitor thread and written by the coder thread.
- `sim->stop_mutex` — protects `stop_simulation`. Accessed by the monitor thread (to set it) and by every coder and the monitor (via `is_stopped`).
- `sim->finish_mutex` — protects `finished_count`, incremented when a coder completes all required compiles.
- `sim->log_mutex` — serializes all log output.

**Race condition prevention example**

When a coder checks whether it can take both dongles, it holds both `first->mutex` and `second->mutex` simultaneously before reading any state. Each dongle is checked independently with `can_take`, which verifies priority, ownership, and cooldown atomically under the lock:

```c
pthread_mutex_lock(&first->mutex);
pthread_mutex_lock(&second->mutex);
if (can_take(first, &reqs->first, now, sim)
    && can_take(second, &reqs->second, now, sim))
{
    pop_request(&first->wait_queue, sim);
    pop_request(&second->wait_queue, sim);
    first->owner_coder_id = coder->id;
    second->owner_coder_id = coder->id;
}
pthread_mutex_unlock(&second->mutex);
pthread_mutex_unlock(&first->mutex);
```

`can_take` checks three conditions atomically: the coder has the highest priority in the dongle's wait queue (`no_higher_priority`), the dongle has no current owner, and the cooldown has expired. This prevents two coders from both passing the check and claiming the same dongle.

Since `pop_request` removes the top of the min-heap in O(log n), and priority is already verified by `can_take`, the pop is guaranteed to remove the correct request.

**Thread-safe communication between coders and monitor**

The monitor reads `last_compile_start_ms` under `state_mutex` and writes `stop_simulation` under `stop_mutex`. Coders read `stop_simulation` via `is_stopped()` which also acquires `stop_mutex`. This ensures the stop signal propagates safely without data races.

## Resources

**Concurrency and synchronization**
- POSIX Threads Programming — Blaise Barney, Lawrence Livermore National Laboratory: https://hpc-tutorials.llnl.gov/posix/
- The Little Book of Semaphores — Allen B. Downey: https://greenteapress.com/wp/semaphores/
- Operating Systems: Three Easy Pieces (Concurrency chapters) — Arpaci-Dusseau: https://pages.cs.wisc.edu/~remzi/OSTEP/

**Scheduling algorithms**
- Earliest Deadline First scheduling: https://www.geeksforgeeks.org/operating-systems/earliest-deadline-first-edf-cpu-scheduling-algorithm/
- Priority queue (binary heap): https://www.geeksforgeeks.org/dsa/priority-queue-using-binary-heap/

**How AI was used**

AI was used throughout this project as a design and review assistant. Specific uses included: discussing the dongle acquisition algorithm and deadlock prevention strategy, reviewing mutex usage and identifying potential race conditions, explaining heap operations and the design trade-off between `remove_request` and `pop_request`, discussing the initial odd/even `arrival_seq` ordering to prevent early starvation, suggesting naming improvements for readability, and reviewing the `monitor_routine` logic. All generated suggestions were reviewed, tested, and understood before being incorporated.
