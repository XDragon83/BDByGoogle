# BDByGoogle

A research-oriented Command & Control (C2) agent that uses **Google Drive as a covert communication channel**, designed to evaluate blind spots in modern EDR and antivirus solutions against cloud-native threats.

> ⚠️ **For authorized penetration testing and security research only.** Do not deploy against systems you do not own or have explicit written permission to test.

---

## Overview

BDByGoogle demonstrates how legitimate cloud infrastructure can be abused as an indirect C2 channel. The agent communicates exclusively through the official **Google Drive API v3** using **OAuth 2.0**, making its traffic indistinguishable from normal cloud application behavior. No direct connection is ever established between the attacker and the target machine.

### How It Works

```
Operator ──► uploads command ──► Google Drive (Source.txt)
                                        │
                                   Agent polls
                                        │
                               executes command
                                        │
                              uploads Result#N.txt
                                        │
Operator ◄── reads output ◄─── Google Drive (Results/)
```

The agent uses a **stack-based state machine** to manage the full lifecycle — authentication, command retrieval, execution, result upload, and cleanup — with automatic error recovery at each stage.

---

## Key Features

- **Zero direct C2 traffic** — all communication routes through `googleapis.com`
- **OAuth 2.0 authentication** with automatic Access Token refresh
- **Out-of-band Refresh Token recovery** via a configurable fallback channel
- **AES-256 encryption** on all payloads before upload
- **Static string obfuscation** via Delta Encoding (no readable strings in `.rdata`)
- **Exponential Backoff with Jitter** to defeat traffic timing analysis
- **Multipart chunked upload** for large command outputs (chunked at 64KB)
- **Dual-layer cleanup** — command file is overwritten then deleted after execution
- **Native Windows tooling only** — HTTP calls made through temporary PowerShell scripts under the OS process, no heavy external HTTP libraries

---

## Evaluation Results

Tested in an isolated lab environment on Windows 10 and 11.

| Security Product | Detection Rate |
|---|---|
| Windows Defender | 0% |
| Kaspersky | 0% |
| Bitdefender | 0% |
| CrowdStrike Falcon (EDR) | 0% |

**VirusTotal:**
- Initial scan: **0 / 72**
- After 72 hours: **4 / 72** (flagged for PowerShell invocation pattern, not network behavior or binary signature)

---

## Project Structure

```
BDByGoogle/
├── main.cpp                # Main loop, stack-based state machine
├── cloudfunctions.cpp      # Google Drive API calls, OAuth 2.0, command execution
├── cloudfunctions.h        # Constants (SOURCE_ID, RESULTS_FOLDER_ID), error codes, headers
├── aes.cpp / aes.h         # AES-256 encryption/decryption module
├── util.cpp / util.h       # Utility helpers
└── Client.py               # Lightweight operator-side script for sending commands
```

---

## Requirements

- Windows 10 / 11 (x64)
- Microsoft Visual Studio 2022 (MSVC x64, C++17)
- A Google Cloud project with Drive API enabled
- OAuth 2.0 credentials (`client_id`, `client_secret`, `refresh_token`)
- Two Google Drive folders: one for commands (`SOURCE_ID`), one for results (`RESULTS_FOLDER_ID`)

**Dependencies:**
- [spdlog](https://github.com/gabime/spdlog) — debug logging only (stripped in Release builds via `#ifdef _DEBUG`)
- AES-256 — bundled, no external dependency

---

## Setup

**1. Google Cloud configuration**

- Create a project at [console.cloud.google.com](https://console.cloud.google.com)
- Enable the **Google Drive API**
- Create OAuth 2.0 credentials (Desktop application type)
- Complete the OAuth flow once to obtain a `refresh_token`

**2. Configure constants in `cloudfunctions.h`**

```cpp
#define CLIENT_ID       "your_client_id"
#define CLIENT_SECRET   "your_client_secret"
#define REFRESH_TOKEN   "your_initial_refresh_token"
#define SOURCE_ID       "google_drive_file_id_for_commands"
#define RESULTS_FOLDER_ID "google_drive_folder_id_for_results"
```

**3. Build**

Open the solution in Visual Studio 2022, select the **Release / x64** configuration, and build. The Debug build enables spdlog output for development.

**4. Operate**

Use `Client.py` to write commands to `Source.txt` in the configured Drive folder and poll the Results folder for output files.

---

## Technical Details

### State Machine

The agent runs a `std::stack<BYTE>`-based state machine with the following states:

| State | Description |
|---|---|
| `STATE_DOWNLOADING` | Fetch command from `Source.txt` |
| `STATE_EXECUTING` | Execute via `_popen()`, capture stdout/stderr |
| `STATE_UPLOADING` | Multipart upload of output to Results folder |
| `STATE_DELETING` | Clear `Source.txt` to signal readiness |
| `STATE_ACCESS_TOKEN_LOST` | Refresh access token using stored refresh token |
| `STATE_REFRESH_TOKEN_LOST` | Retrieve new refresh token from out-of-band backup |
| `STATE_CONNECTION_LOST` | Wait with exponential backoff until network returns |

### String Obfuscation (Delta Encoding)

Sensitive strings (API endpoints, OAuth identifiers, folder IDs) are never stored as plaintext in the binary. Each string is encoded as an integer array where each element is the **delta** (difference) from the previous character's ASCII value:

```
plaintext:  "https://oauth2.googleapis.com/token"
encoded:    { 104, 4, 8, -3, ... }
```

`deltaDecodeA()` and `deltaDecodeW()` reconstruct strings at runtime. This eliminates all recognizable patterns from the `.rdata` section and defeats YARA string-matching rules.

### Jitter

Polling intervals are randomized to prevent traffic timing analysis:

```cpp
Sleep(timeThreshold * 1000 + (rand() % 10) * 100);
```

---

## Defensive Implications

This tool was developed to expose a detection gap that existing security products have not fully addressed. The root causes are:

- **TLS inspection is not applied** to trusted domains like `*.googleapis.com` in most organizations
- **OAuth 2.0 tokens** make requests appear legitimate to firewalls and DLP systems
- **Traffic volume** to Google Drive is too high for anomaly detection to isolate single malicious sessions
- **EDRs focus on process behavior**, not outbound cloud API patterns

Security teams can use this project to extract behavioral signatures, build YARA/Sigma rules, and improve detection of cloud-native C2 channels in their environments.

---

## Disclaimer

This tool is published for **educational and authorized security research purposes only**. The authors are not responsible for any misuse. Always obtain explicit written permission before testing any system. Unauthorized use may violate computer fraud laws in your jurisdiction.

---

## Author

**Mojtaba Jahani**  
Computer Engineering — Shahid Beheshti University  
Spring 2025