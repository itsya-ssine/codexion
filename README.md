*This project has been created as part of the 42 curriculum by yelmajdo.*

# Codexion

## Description

Codexion is a concurrency simulation inspired by the classic dining philosophers
problem, reskinned around a coding theme. A group of **coders** sit around a
circular co-working hub and cycle endlessly through three states: **compiling**,
**debugging**, and **refactoring**. Compiling requires exclusive access to two
shared **USB dongles** at once (the coder's left and right dongle), and there are
exactly as many dongles as coders, arranged so that each dongle sits between two
neighbouring coders.

Each coder is implemented as a POSIX thread. A dedicated **monitor** thread
independently watches every coder's deadline and stops the whole simulation the
moment any coder **burns out** — i.e. fails to start a new compile within
`time_to_burnout` milliseconds of their previous one. The simulation also stops
successfully once every coder has compiled at least `number_of_compiles_required`
times.

The goal of the project is to correctly implement thread synchronization for a
shared, limited resource: avoiding deadlock, avoiding starvation, respecting a
mandatory cooldown period on each dongle, and supporting two different
arbitration policies (`fifo` and `edf`) for deciding who gets a contested
dongle — all without a single global variable.

## Instructions

### Build

```sh
make
```

This produces a `codexion` binary at the project root. Other available rules:

```sh
make clean   # remove object files
make fclean  # remove object files and the binary
make re      # fclean + all
```

The project compiles with `cc -Wall -Wextra -Werror -pthread` and has no external
dependencies beyond the POSIX threads library.

### Run

```sh
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug \
           time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

| Argument                      | Meaning                                                                 |
|--------------------------------|--------------------------------------------------------------------------|
| `number_of_coders`             | Number of coder threads (also the number of dongles).                    |
| `time_to_burnout` (ms)         | Max time a coder may go without starting a new compile before burning out.|
| `time_to_compile` (ms)         | Duration of the compiling phase (requires both dongles held).            |
| `time_to_debug` (ms)           | Duration of the debugging phase.                                         |
| `time_to_refactor` (ms)        | Duration of the refactoring phase.                                       |
| `number_of_compiles_required`  | Simulation stops successfully once every coder reaches this compile count.|
| `dongle_cooldown` (ms)         | Time a dongle stays unavailable after being released.                    |
| `scheduler`                    | Arbitration policy for contested dongles: `fifo` or `edf`.               |

Example:

```sh
./codexion 5 800 200 100 100 4 50 edf
```

All arguments are mandatory and strictly validated: negative numbers,
non-integers, a zero coder count, or a scheduler value other than `fifo`/`edf`
are all rejected with an error message and a non-zero exit code.

### Output

Every state change is printed as `timestamp_in_ms coder_id event`, e.g.:

```
0 1 has taken a dongle
1 1 has taken a dongle
1 1 is compiling
201 1 is debugging
401 1 is refactoring
```

## Resources

- *The Little Book of Semaphores* — Allen B. Downey (dining philosophers, deadlock
  avoidance, starvation freedom).
- POSIX Threads Programming — the classic LLNL pthreads tutorial, used as a
  reference for `pthread_mutex_t`, `pthread_cond_t`, and `pthread_cond_timedwait`
  semantics.
- The `man` pages for `pthread_create`, `pthread_mutex_lock`,
  `pthread_cond_timedwait`, and `gettimeofday`.
- Wikipedia — "Dining philosophers problem" and "Earliest deadline first
  scheduling", for the theoretical background behind deadlock avoidance and the
  EDF arbitration policy.

**AI usage:** An AI assistant was used to write this readme.

## Blocking cases handled

- **Deadlock prevention (Coffman's conditions):** all four coders never acquire
  their two dongles in a fixed global order. Instead, coders alternate their
  acquisition order by parity of their id (even-numbered coders take their left
  dongle first, odd-numbered coders take their right dongle first). This is the
  classic asymmetric-ordering fix for the dining philosophers problem: it breaks
  the circular-wait condition, so a cycle of coders each holding one dongle and
  waiting on the next can never form.
- **Starvation prevention:** both `fifo` and `edf` arbitration are implemented
  with a custom binary heap per dongle, so waiting coders are served in a
  well-defined order (arrival order for `fifo`, nearest burnout deadline for
  `edf`) instead of being granted the dongle arbitrarily by whichever thread
  the OS happens to wake up first.
- **Cooldown handling:** a dongle records `released_at` when it is put back on
  the table; any coder trying to reacquire it must wait, via
  `pthread_cond_timedwait`, until `released_at + dongle_cooldown` has passed,
  even if it is otherwise first in line.
- **Precise burnout detection:** a separate monitor thread polls every coder's
  `last_compile_start` at a fine-grained interval, independent of the coders'
  own execution, so a burnout is detected and logged within a few milliseconds
  of the actual deadline (well inside the 10 ms tolerance), regardless of what
  the coder threads themselves are doing at that instant.
- **Log serialization:** every log line, including the burnout announcement
  from the monitor, is printed while holding a single dedicated logging mutex,
  so two messages can never interleave mid-line.

## Thread synchronization mechanisms

- **`pthread_mutex_t coder_mutex` (one per coder):** protects a coder's own
  `last_compile_start` and `compile_count` fields, which are read by both the
  coder's own thread and the monitor thread concurrently.
- **`pthread_mutex_t mutex` + `pthread_cond_t cond` (one per dongle):** protect
  `is_available`, `released_at`, and the dongle's private wait-queue (heap).
  A coder wanting a dongle pushes itself onto that dongle's heap under the
  mutex, then waits on the condition variable (via `pthread_cond_wait` or
  `pthread_cond_timedwait` when a cooldown is still running) until it is both
  at the front of the heap **and** the cooldown has elapsed. `dongle_release`
  updates `is_available`/`released_at` and calls `pthread_cond_broadcast` so
  every waiter re-checks the condition — this is what lets the heap-based
  arbitration policy (fifo/edf) decide who actually proceeds, rather than
  whichever thread the OS wakes up first.
- **`pthread_mutex_t fifo_mutex`:** protects a single monotonically increasing
  counter used to timestamp requests for the `fifo` policy, guaranteeing a
  strict, race-free arrival order across all coder threads.
- **`pthread_mutex_t log_mutex`:** serializes every `printf` call describing a
  state change, so log lines from different coder threads and the monitor
  thread are never interleaved.
- **`pthread_mutex_t stop_mutex`:** guards the single `simulation_over` flag
  that every coder thread and the monitor thread check to know when to exit;
  it is the only piece of state that determines a clean, coordinated shutdown.
- **`pthread_mutex_t start_mutex`:** held by the main thread while all coder
  threads are being created, so that no coder can start acting on
  `last_compile_start` before every thread exists and the simulation's
  official `start_time` has been recorded — this avoids a race where an early
  thread could begin timing itself against a `start_time` that hasn't been
  set yet.

Race conditions are avoided by the rule that every field that is read by more
than one thread (`is_available`, `released_at`, `last_compile_start`,
`compile_count`, `simulation_over`) is only ever read or written while holding
its associated mutex, and thread-safe communication between coders and the
monitor happens exclusively through those mutex-protected fields plus the
condition variables' broadcast wake-ups — no coder ever calls into another
coder's code directly, and coders never communicate with each other, only with
shared dongle and simulation state.