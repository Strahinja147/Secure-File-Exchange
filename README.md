# SecureFileExchange – Automated File Protection & Secure Transfer (C++, Win32, Winsock2)

[![Build Solution](https://github.com/Strahinja147/Secure-File-Exchange/actions/workflows/ci.yml/badge.svg)](https://github.com/Strahinja147/Secure-File-Exchange/actions/workflows/ci.yml)

## Overview

Modular C++ security suite developed as a **university project** for real-time file system monitoring, multi-algorithm encryption, and secure peer-to-peer distribution on Windows.  

The project demonstrates low-level bit manipulation, multi-threaded system services, custom protocol design, and network programming using the Windows API. The core objective was to implement cryptographic primitives from scratch to understand data security at the hardware and OS level.

---

## Features

### Real-time File System Monitoring
- Implementation of a **File System Watcher (FSW)** using `ReadDirectoryChangesW`
- Asynchronous monitoring of the `/Target` directory
- Automated encryption trigger upon file creation or modification

### Cryptographic Engine
- **A5/2 Stream Cipher:** Manual implementation of the GSM A5/2 algorithm (both Standard and CTR/Counter modes)
- **Simple Substitution:** Optimized byte-level substitution cipher with $O(1)$ lookup using pre-computed inverse tables
- **Key Derivation (KDF):** Secure session key and Nonce (IV) generation from user passwords via **SHA-256**
- **Data Integrity:** Implementation of **MD5** hashing from scratch to verify file consistency before and after transmission

### Secure Networking (P2P)
- Asynchronous **TCP Server/Client** architecture using **Winsock2**
- Custom transmission protocol: `[Header Length][JSON Header][Payload][Trailer Length][JSON Trailer]`
- Automated "Receive & Decrypt" pipeline for incoming network packages

### Data Serialization & Packaging
- Structured metadata management using **nlohmann/json**
- **Encrypted Envelopes:** Every protected file includes a JSON header (file metadata, algorithm info) and a JSON trailer (MD5 checksums)

---

## Architecture Overview

The project is modular and organized into high-level service components:

- `SecurityService.cpp` – Orchestrates the encryption, packaging, and unpacking logic
- `NetworkService.cpp` – Manages Winsock2 initialization, server loops, and TCP transmission
- `FSWatcher.cpp` – Handles Win32 directory change notifications in a background thread
- `A52.cpp` – Low-level implementation of LFSR registers and majority clocking logic
- `MetadataManager.cpp` – Generates and parses JSON headers and trailers
- `KeyHelper.h` – Derives cryptographic material from session passwords using SHA-256
- `FileManager.cpp` – High-performance C++17 filesystem I/O

**Execution Flow:**

File Detection (FSW) → Metadata Generation → SHA-256 Key Derivation → Encryption (A5/2 / SS) → JSON Packaging → Network Transfer → Integrity Verification (MD5) → Decryption

---

## Technical Stack & APIs Used

- **Language:** C++17
- **Networking:** Winsock2 (Windows Sockets)
- **System API:** Win32 API (File Management, Directory Watching, Threading)
- **Concurrency:** `std::thread`, `std::atomic`, `std::mutex`
- **Serialization:** `nlohmann/json`
- **Hashing:** `picosha2` (SHA-256) and custom MD5

---

## Build & Run

### Requirements
- Windows OS
- Visual Studio 2019/2022 (C++ Desktop Development)

### Build
1. Open `ZastitaInformacija.sln` in Visual Studio.
2. Set configuration to **Release** (x64).
3. Build solution (**Ctrl+Shift+B**).

---

## Example Usage

### Automated Protection (FSW)
- Launch the application and set a **Session Password**.
- Start the File System Watcher (**Option 1**).
- Drag and drop any file (e.g., `document.docx`) into the `/Target` directory.
- The system detects the file and automatically creates `document.docx.bin` in the `/Zasticeni` folder.

### Secure Network Transfer
- **Receiver:** Select **Option 6** and enter a port (e.g., `12345`). The server starts listening in the background.
- **Sender:** Select **Option 7**, input the Receiver's IP, the same port, and the path to the encrypted `.bin` file.
- **Automated Pipeline:** The file is transferred; the receiver's server verifies the MD5 integrity and automatically decrypts the file into the `/Downloads` folder using the shared session password.

---

---

## Purpose

This project focuses on understanding computer security internals and system-level programming through a fully functional C++ implementation. Developed as a university project, it serves to bridge the gap between local system management and network communication.

By implementing cryptographic algorithms like **A5/2** and **MD5** from scratch, the project explores bit-level data manipulation and hardware-inspired logic. It demonstrates how to build a cohesive security pipeline that integrates real-time OS events with asynchronous socket programming.
