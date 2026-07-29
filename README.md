<h1 align="center">
    <img src="./assets/logo.png" width="350px" alt="CVFS Logo">
</h1>

<h1 align="center">
Customized Virtual File System (CVFS)
</h1>

<p align="center">
  <i>An ext4-inspired virtual file system built from scratch in C, emulating core UNIX file system internals entirely in user space.</i>
</p>

<h4 align="center">
  <img src="https://img.shields.io/badge/language-C-00599C?style=flat-square&logo=c&logoColor=white" />
  <img src="https://img.shields.io/badge/platform-Linux-FCC624?style=flat-square&logo=linux&logoColor=black" />
  <img src="https://img.shields.io/badge/system-POSIX-blue?style=flat-square" />
  <img src="https://img.shields.io/badge/topic-Systems%20Programming-darkred?style=flat-square" />
  <img src="https://img.shields.io/badge/interface-CLI-black?style=flat-square" />
  <img src="https://img.shields.io/github/last-commit/aaditya-jagtap-2205/cvfs?style=flat-square" />
  <img src="https://img.shields.io/github/repo-size/aaditya-jagtap-2205/cvfs?style=flat-square" />
  <img src="https://img.shields.io/github/languages/top/aaditya-jagtap-2205/cvfs?style=flat-square" />
</h4>

---

# Introduction

CVFS (Customized Virtual File System) is a systems programming project that recreates the core internals of a UNIX-style file system — superblock, inodes, and a per-process file descriptor table — entirely from scratch in C, without touching the host OS's actual file system.

The project demonstrates practical understanding of:

- File system internals (superblock, inode table, free-space tracking)
- File descriptor management
- User-space simulation of kernel data structures
- Low-level buffer and memory management
- Permission modeling (read / write / read+write)
- Command-driven shell design

All commands are implemented and tested in a Linux development environment using GCC and standard C.

---

# Implemented Commands

| Command  | Description |
|---|---|
| `creat`  | Create a new file with a given permission |
| `write`  | Write data into an existing file |
| `read`   | Read data from an existing file |
| `stat`   | Display metadata about a file |
| `unlink` | Delete an existing file |
| `ls`     | List all files currently in the file system |
| `man`    | Display the manual page for a command |
| `help`   | Display all available commands |
| `clear`  | Clear the CVFS terminal |
| `exit`   | Terminate CVFS |

> Additional commands and features are continuously being improved.

---

# Core Concepts Used

- Superblock & Inode Table design
- Disk Inode List Block (DILB)
- User File Descriptor Table (UFDT)
- File Permission Modeling
- Free-Space Management
- Buffer-backed File Storage
- Command Parsing & Dispatch
- Error Handling via Custom Error Codes

---

# Filesystem Structures Used

```c
struct BootBlock    // boot-time metadata
struct SuperBlock    // total / free inode tracking
struct Inode          // per-file metadata + data buffer
struct FileTable      // per-open-file read/write offsets
struct UAREA          // per-process file descriptor table (UFDT)
```

---

# Project Structure

```bash
cvfs/
├── src/
│   └── cvfs.c
│
├── cvfs.h
│
├── assets/
│   └── logo.png
│
├── Makefile
└── README.md
```

---

# Building the Project

## Clone the repository

```bash
git clone https://github.com/aaditya-jagtap-2205/cvfs.git
cd cvfs
```

---

## Compile

```bash
gcc -Wall -Wextra -o cvfs src/cvfs.c
```

Or compile using:

```bash
make
```

---

# Running CVFS

```bash
./cvfs
```

Example session:

```bash
CVFS: > creat test.txt 3
File gets succesfully created with fd: 3

CVFS: > write 3 HelloCVFS
Number of bytes written: 9

CVFS: > read 3 20
Data read from file: HelloCVFS
Number of bytes read: 9

CVFS: > stat test.txt
Inode Number     : 1
File Name        : test.txt
File Size        : 50
Actual File Size : 9
Permission       : 3
Reference Count  : 1

CVFS: > unlink test.txt
File deleted successfully

CVFS: > exit
Thank you for using CVFS
```

---

# Development Environment

| Component | Details |
|---|---|
| OS | Linux |
| Compiler | GCC |
| Standard | C11 |
| Shell | Bash |

---

# Future Improvements

- `close()` command with reference-count based file closing
- `open()` for reopening existing files without re-creating them
- Directory support (nested paths, not just a flat namespace)
- Persistent storage (currently in-memory only, resets on exit)
- Improved command option parsing
- Unit tests for individual filesystem operations

---

# Learning Outcomes

This project helped strengthen understanding of:

- File system internals (inodes, superblocks, free-space management)
- UNIX file descriptor model
- User-space vs kernel-space responsibilities
- Structuring a C project with headers and source separation
- Debugging pointer-heavy, state-driven C programs

---

# License

This project is licensed under the MIT License.
