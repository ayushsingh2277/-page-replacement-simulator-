# Page Replacement Algorithm Simulator

> A modular C-based simulator for **FIFO**, **LRU**, and **Optimal** page replacement algorithms — featuring both a CLI version and a new GUI web-based interface.

---

## Table of Contents

1. [Project Overview](#project-overview)
2. [CLI Version (Original)](#cli-version-original)
3. [Project Structure](#project-structure)
4. [How to Compile & Run (CLI)](#how-to-compile--run-cli)
5. [GUI Version (Enhancement)](#gui-version-enhancement)
6. [Team & Contributions](#team--contributions)

---

## Project Overview

This project simulates three classic **page replacement algorithms** used in Operating Systems virtual memory management:

| Algorithm | Strategy |
|-----------|----------|
| **FIFO** | Replace the page that has been in memory the longest |
| **LRU** | Replace the page that was least recently accessed |
| **Optimal** | Replace the page that won't be used for the longest time |

The simulator accepts a **page reference string** and a **frame count**, then outputs step-by-step simulation results including total page faults and hit rate.

---

## CLI Version (Original)

The original project is implemented in modular C with a clean separation of concerns:

```
module1_algorithms/    — Core algorithm logic (FIFO, LRU, Optimal)
module2_display/       — Output formatting and table display
module3_ui/            — User input handling (terminal UI)
main.c                 — Entry point
Makefile               — Build configuration
```

> ⚠️ **These files are not modified.** All original code, commit history, and team contributions are preserved exactly as-is.

---

## Project Structure

```
page-replacement-simulator/
│
├── module1_algorithms/     ← (original, untouched)
├── module2_display/        ← (original, untouched)
├── module3_ui/             ← (original, untouched)
├── main.c                  ← (original, untouched)
├── Makefile                ← (original, untouched)
│
├── gui/                    ← NEW: Web-based frontend
│   ├── index.html          — Main HTML page
│   ├── style.css           — Dark modern styling
│   └── script.js           — Frontend logic & API calls
│
├── backend/                ← NEW: Single-file C for GUI integration
│   └── main.c              — FIFO + LRU + Optimal in one file
│
├── server.js               ← NEW: Node.js bridge server
└── README.md               ← This file
```

---

## How to Compile & Run (CLI)

```bash
# From the project root
make

# Run the CLI simulator
./simulator
```

---

## GUI Version (Enhancement)

The GUI is a **new addition** to this project. It provides a browser-based interface for the same algorithms without touching any existing code.

### Architecture

```
Browser (HTML/CSS/JS)
       ↓  POST /simulate  (JSON)
  Node.js server.js
       ↓  exec()
  backend/simulator  (compiled C)
       ↓  structured text output
  server.js parses output
       ↓  JSON response
  Browser renders results
```

### What's New

- `backend/main.c` — A **single-file** standalone C implementation that merges all three algorithms. This was written specifically for easy integration with the web backend. The original modular code is unchanged.
- `server.js` — A lightweight Node.js Express server that receives requests from the frontend, calls the C binary as a subprocess, and returns parsed JSON.
- `gui/` — A dark-themed, fully responsive web UI.

### Setup Instructions

#### Step 1 — Compile the C backend

```bash
# From the project root
gcc -o backend/simulator backend/main.c -lm

# On Windows (MinGW):
gcc -o backend/simulator.exe backend/main.c
```

#### Step 2 — Install Node.js dependencies

```bash
npm install express cors
```

#### Step 3 — Start the server

```bash
node server.js
```

You should see:
```
╔════════════════════════════════════════╗
║   Page Replacement Simulator Server   ║
╠════════════════════════════════════════╣
║   Running at: http://localhost:3000    ║
╚════════════════════════════════════════╝
```

#### Step 4 — Open in browser

Navigate to: **http://localhost:3000**

### Using the GUI

1. Enter a **page reference string** — space-separated integers, e.g. `1 2 3 4 1 2 5 1 2 3 4 5`
2. Set the **number of frames** (1–10)
3. Choose an algorithm: FIFO, LRU, Optimal, or **Compare All**
4. Click **Run Simulation**

The output shows:
- Step-by-step table with each page access
- 🟢 Green rows = Cache **HIT** (page already in memory)
- 🔴 Red rows = Page **FAULT** (page loaded into a frame)
- Summary: total hits, faults, and hit rate percentage
- Comparison table (when "Compare All" is selected)

### Testing the C backend directly

```bash
# FIFO with 3 frames
./backend/simulator fifo 3 1 2 3 4 1 2 5 1 2 3 4 5

# LRU with 4 frames
./backend/simulator lru 4 1 2 3 4 5 1 2 3 4 5

# Compare all algorithms
./backend/simulator all 3 7 0 1 2 0 3 0 4 2 3 0 3 2
```

---

## Team & Contributions

> All original commits from team members are preserved. The GUI enhancement was added as new files only — no existing files were modified.

---

*This project was developed as part of an Operating Systems course. The GUI enhancement demonstrates integration between systems-level C programming and modern web technologies.*
