# Socket Programming Assignment: Concurrent Math Evaluation Server

## Overview

This project implements a **TCP-based client–server system in C++** that evaluates arithmetic expressions sent by clients (e.g., `22+44`, `3*5`) and returns the computed result.

The primary objective of this assignment is to **demonstrate and compare multiple server-side concurrency models** used in real-world network programming. The project incrementally evolves from a simple blocking server to advanced, scalable architectures capable of handling many concurrent clients efficiently.

Four distinct server implementations are provided, each showcasing a different design philosophy and trade-off in operating systems and networking.

---

## Project Structure

```
.
├── server1.cpp        # Iterative Server (Single-process, Blocking)
├── server2.cpp        # Multi-Process Server (fork-based concurrency)
├── server3.cpp        # Multi-Threaded Server (std::thread-based)
├── server4.cpp        # I/O Multiplexing Server (select-based)
├── client.cpp         # TCP client for interactive testing
├── calculator.h       # Interface for expression evaluation logic
├── calculator.cpp     # Implementation of arithmetic parsing and evaluation
├── Makefile           # Unified build automation
└── README.md          # Project documentation
```

Each server file is self-contained, allowing isolated experimentation with a specific concurrency model while reusing common client and calculator logic.

---

## Compilation & Build System

A Makefile is provided to ensure consistent compilation across all components.

### Build All Targets

```bash
make
```

This command compiles all server implementations and the client:

- `server1`
- `server2`
- `server3`
- `server4`
- `client`

### Clean Generated Files

```bash
make clean
```

Removes all compiled binaries and intermediate artifacts.

---

## Server Architectures Explained

### Server 1: Iterative (Blocking) Server

**Source File:** `server1.cpp`

**Architecture:**
Single-process, single-threaded implementation.

**Concurrency Model:**
None (serial execution).

**Behavior:**
The server handles exactly one client at a time. While a client is connected, all other incoming clients must wait until the current session terminates.

**Key OS Concepts:**

- Blocking `accept()`
- Blocking `recv()`

**Use Case:**
Educational baseline for understanding blocking I/O and control flow.

---

### Server 2: Multi-Process Server

**Source File:** `server2.cpp`

**Architecture:**
Concurrent server using `fork()`.

**Concurrency Model:**
Process-based parallelism.

**Behavior:**
Each client connection is handled by a separate child process. The parent process immediately returns to listening for new connections.

**Key OS Concepts:**

- Process creation (`fork()`)
- Copy-on-write memory semantics
- Signal handling (`SIGCHLD`)

**Advantages:**

- Strong isolation — a crash in one client handler does not affect others.

**Limitations:**

- Higher context-switching and memory usage compared to threads.

---

### Server 3: Multi-Threaded Server

**Source File:** `server3.cpp`

**Architecture:**
Single process with multiple threads.

**Concurrency Model:**
Thread-based parallelism using `std::thread`.

**Behavior:**
Each client connection is handled by a detached worker thread, allowing the main thread to continue accepting new connections immediately.

**Key OS Concepts:**

- Shared address space
- Shared file descriptor table
- Lightweight context switching

**Advantages:**

- Faster than process-based servers due to shared memory.

**Limitations:**

- Requires careful synchronization in more complex scenarios; a crash may terminate the entire server.

---

### Server 4: I/O Multiplexing Server

**Source File:** `server4.cpp`

**Architecture:**
Single-process, event-driven server.

**Concurrency Model:**
I/O multiplexing using `select()`.

**Behavior:**
A single thread monitors multiple client sockets simultaneously and only performs `recv()` when data is available.

**Key OS Concepts:**

- `select()` system call
- `FD_SET`, `FD_ISSET`
- Non-blocking I/O (`O_NONBLOCK`)

**Advantages:**

- Very efficient for large numbers of idle or low-traffic connections.

**Limitations:**

- Constrained by `FD_SETSIZE` (typically 1024 file descriptors).

---

## How to Run

### Step 1: Launch a Server

Choose a server model and start it on a desired port (e.g., 8080):

```bash
./server1 8080   # Iterative
./server2 8080   # Multi-Process
./server3 8080   # Multi-Threaded
./server4 8080   # I/O Multiplexing
```

### Step 2: Run the Client

In a separate terminal window:

```bash
./client 127.0.0.1 8080
```

Multiple clients can be launched concurrently to test server behavior.

---

## Interaction Example

```
Enter message: 10+20
Server Reply: 30

Enter message: 5*5
Server Reply: 25
```

Expressions are evaluated on the server side and results are returned synchronously.

---

## Technical Design Details

### Communication Protocol

**Transport Layer:** TCP (`SOCK_STREAM`)

**Message Format:** Plain-text expressions terminated by newline (`\n`)

**Framing Strategy:** The server buffers incoming bytes until a newline delimiter is detected.

### Parsing Strategy

1. Accumulate received bytes into a buffer.
2. Detect `\n` as a message boundary.
3. Extract the complete expression.
4. Pass it to `Calculator::calculate()`.
5. Send the computed result followed by `\n`.

---

## Comparison of Server Models

| Server | Concurrency Model | Scalability | Memory Usage | Strengths             | Limitations                |
| ------ | ----------------- | ----------- | ------------ | --------------------- | -------------------------- |
| S1     | Iterative         | Low         | Minimal      | Simple, predictable   | Blocks all other clients   |
| S2     | Multi-Process     | High        | High         | Strong isolation      | Expensive context switches |
| S3     | Multi-Threaded    | High        | Medium       | Fast, shared memory   | Crash affects all clients  |
| S4     | I/O Multiplexing  | Very High   | Low          | Efficient, no locking | `select()` FD limit        |

---

## Troubleshooting Guide

### 1. Address Already in Use

**Reason:** Port is in `TIME_WAIT` state.

**Solution:** The server enables `SO_REUSEADDR`. If the issue persists, wait briefly or use a different port.

### 2. Client Hangs (Server 1)

**Reason:** Another client is currently being served.

**Solution:** Disconnect the existing client or switch to a concurrent server.

### 3. Too Many Connections (Server 4)

**Reason:** `select()` is limited by `FD_SETSIZE`.

**Solution:** For production systems, use `poll()`, `epoll()`, or `kqueue()`.

---

## License & Usage

This project is developed for academic and educational purposes as part of a Socket Programming assignment. It is intended to demonstrate operating system concepts, network protocols, and concurrency models.
