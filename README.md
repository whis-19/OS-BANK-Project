# OS-BANK Project

OS-BANK is an operating system simulation project designed to demonstrate core operating system concepts such as inter-process communication (IPC), process scheduling, paging, threading, mutex locks, synchronization, and error handling. This project provides an educational platform to understand and explore fundamental operating system mechanisms.

## Features

### Core Functionalities
- **Inter-Process Communication (IPC):** Seamless communication between processes.
- **Process Scheduling:** Efficient scheduling algorithms to manage processes.
- **Paging:** Simulated memory management with paging.
- **Threading:** Multi-threading support for concurrent execution.
- **Mutex Locks and Synchronization:** Ensuring safe access to shared resources.
- **Error Handling:** Robust mechanisms to handle runtime errors gracefully.

## Getting Started

### Prerequisites
- C programming language
- GCC compiler
- Git for version control

### Clone the Repository
```bash
git clone https://github.com/whis-19/OS-BANK-Project.git
cd OS-BANK-Project
```

### Build and Run
1. **Build the project:**
   ```bash
   gcc -o os-bank bank.c
   gcc -o client customer.c
   ```

2. **Run the executable:**
   ```bash
   ./os-bank
   ./client
   ```


### Contributing
Contributions are welcome! Please fork the repository, create a new branch, and submit a pull request with detailed descriptions of your changes.

## Credits
This project is a collaborative effort, and we extend our gratitude to the following contributors:

- **Basic Structure and IPC**
  - [Faizan Tariq](https://github.com/FaizanTariq109)

- **Scheduling, Paging, and Threading**
  - [whis-19](https://github.com/whis-19/)

- **Mutex Locks, Synchronization, and Error Handling**
  - [Muhammad Waleed](https://github.com/Waleed-SE)

## Acknowledgments
We appreciate all contributors and reviewers who helped shape this project. Special thanks to the open-source community for their tools and libraries.
