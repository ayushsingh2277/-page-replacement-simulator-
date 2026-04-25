# Page Replacement Algorithm Simulator
## OS Project — C + Python + HTML/CSS/JS

```
page_replacement_simulator/
├── c_core/
│   ├── page_replacement.h      ← Header: data structures & function prototypes
│   ├── page_replacement.c      ← Core logic: FIFO, LRU, Optimal algorithms
│   └── Makefile                ← Build the standalone C binary
│
├── python_server/
│   ├── page_server.py          ← Flask REST API server (bridges C ↔ Frontend)
│   └── requirements.txt        ← Python dependencies
│
└── web/
    └── index.html              ← Full HTML/CSS/JS frontend (standalone)
```

---

## How to Run

### Option A — Just open the frontend (quickest)
```bash
cd web/
open index.html          # macOS
xdg-open index.html      # Linux
# Or simply double-click index.html in your file manager
```
The simulator runs entirely in the browser. The JS faithfully mirrors the C algorithms.

---

### Option B — Run the C binary directly (console output)
```bash
cd c_core/
make          # Compile
make run      # Run with built-in test case
```

You can edit `main()` in `page_replacement.c` to change the reference string and frame count.

---

### Option C — Run the Python Flask server + frontend
```bash
# Install Python dependencies
cd python_server/
pip install -r requirements.txt

# Start the server
python page_server.py

# Then open the frontend (it will auto-connect to the server)
open ../web/index.html
```

Server runs at: `http://localhost:5000`
API endpoint: `POST /simulate`

**Example API call:**
```bash
curl -X POST http://localhost:5000/simulate \
  -H "Content-Type: application/json" \
  -d '{"ref_string":[7,0,1,2,0,3,0,4,2,3],"frames":3,"algorithms":["FIFO","LRU","Optimal"]}'
```

---

## Algorithms Implemented

### FIFO — First In, First Out
- Tracks insertion order using a queue
- Evicts the page that has been in memory the longest
- Simple but suffers from Bélády's Anomaly

### LRU — Least Recently Used
- Tracks timestamp of last access for each frame
- Evicts the page not used for the longest time
- Better approximation to optimal; no Bélády's Anomaly

### Optimal (OPT)
- Looks ahead in the reference string
- Evicts the page whose next use is farthest in the future
- Theoretical minimum page faults; impossible in real systems
- Used as a benchmark to evaluate other algorithms

---

## Data Structures (C)

```c
typedef struct {
    int page_ref;               // Page referenced at this step
    int frame_state[MAX_FRAMES];// Snapshot of all frames
    int frame_count;            // Number of frames
    int is_fault;               // 1 = page fault, 0 = hit
    int replaced_page;          // Evicted page (EMPTY if none)
} StepInfo;

typedef struct {
    StepInfo *steps;            // Array of all steps (malloc'd)
    int total_refs;             // Reference string length
    int page_faults;            // Total faults
    double hit_ratio;           // Hit percentage (0–100)
} SimResult;
```

---

## Frontend Features

- **Interactive reference string input** with quick presets
- **Frame count slider** (1–8 frames)
- **Algorithm toggle** — run any combination of FIFO / LRU / Optimal
- **Metric cards** — faults count, hit ratio bar per algorithm
- **Bar chart** — visual comparison of fault counts
- **Step table** — complete frame-by-frame state with fault/hit badges
- **Analysis summary** — best algorithm, savings, optimal bound

---

## Sample Output (Reference: 7 0 1 2 0 3 0 4 2 3 0 3 2 1 2 0 1 7 0 1 | Frames: 3)

| Algorithm | Page Faults | Hit Ratio |
|-----------|-------------|-----------|
| FIFO      | 15          | 25.00%    |
| LRU       | 12          | 40.00%    |
| **Optimal**   | **9**       | **55.00%** |

---

## Technologies

| Component | Technology | Purpose |
|-----------|-----------|---------|
| Core logic | **C (C99)** | FIFO, LRU, Optimal algorithms |
| Server | **Python + Flask** | REST API bridge |
| Frontend | **HTML5 + CSS3 + Vanilla JS** | UI, visualization, animation |
| Build | **GCC + Makefile** | Compile C binary |

---

*Compile the C binary, open index.html, and start simulating!*
