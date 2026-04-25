#!/usr/bin/env python3
"""
page_server.py — Flask API server for Page Replacement Simulator
Bridges the HTML/JS frontend with the compiled C binary.

Usage:
    pip install flask
    python page_server.py

Endpoints:
    POST /simulate   { ref_string: [...], frames: N, algorithms: [...] }
    GET  /health
"""

import os
import sys
import json
import ctypes
import subprocess
import tempfile
from flask import Flask, request, jsonify, send_from_directory
from flask_cors import CORS

# ── Config ──────────────────────────────────────────────────
BASE_DIR   = os.path.dirname(os.path.abspath(__file__))
C_DIR      = os.path.join(BASE_DIR, "../c_core")
C_SRC      = os.path.join(C_DIR, "page_replacement.c")
C_BIN      = os.path.join(C_DIR, "page_sim")
PORT       = 5000
MAX_FRAMES = 10
MAX_REF    = 100

app = Flask(__name__, static_folder="../web", static_url_path="")

@app.route("/")
def home():
    return send_from_directory("../web", "index.html")

CORS(app)
  # allow requests from the HTML frontend

# ── Build C binary if needed ─────────────────────────────────
def ensure_c_binary():
    if not os.path.exists(C_BIN):
        print("Building C binary...", flush=True)
        result = subprocess.run(
            ["gcc", "-Wall", "-O2", "-std=c99", "-o", C_BIN, C_SRC, "-lm"],
            capture_output=True, text=True
        )
        if result.returncode != 0:
            print("C compilation failed:\n", result.stderr)
            sys.exit(1)
        print("C binary built successfully.")

# ── Pure-Python fallback algorithms ──────────────────────────
# (used when C binary is unavailable; mirrors the C logic exactly)

EMPTY = -1

def _fifo(ref, frames):
    f = [EMPTY] * frames
    q = []
    steps = []
    faults = 0
    for page in ref:
        fault = page not in f
        replaced = EMPTY
        if fault:
            faults += 1
            if EMPTY in f:
                idx = f.index(EMPTY)
                f[idx] = page
                q.append(page)
            else:
                replaced = q.pop(0)
                idx = f.index(replaced)
                f[idx] = page
                q.append(page)
        steps.append({
            "page_ref": page,
            "frame_state": f[:],
            "is_fault": fault,
            "replaced_page": replaced
        })
    hits = len(ref) - faults
    return {"steps": steps, "page_faults": faults,
            "hit_ratio": round(hits / len(ref) * 100, 2)}

def _lru(ref, frames):
    f = [EMPTY] * frames
    last = {}
    steps = []
    faults = 0
    for i, page in enumerate(ref):
        fault = page not in f
        replaced = EMPTY
        if fault:
            faults += 1
            if EMPTY in f:
                idx = f.index(EMPTY)
                f[idx] = page
            else:
                lru_page = min(last, key=lambda p: last[p] if p in f else float('inf'))
                idx = f.index(lru_page)
                replaced = f[idx]
                f[idx] = page
                if replaced in last:
                    del last[replaced]
        last[page] = i
        steps.append({
            "page_ref": page,
            "frame_state": f[:],
            "is_fault": fault,
            "replaced_page": replaced
        })
    hits = len(ref) - faults
    return {"steps": steps, "page_faults": faults,
            "hit_ratio": round(hits / len(ref) * 100, 2)}

def _optimal(ref, frames):
    f = [EMPTY] * frames
    steps = []
    faults = 0
    for i, page in enumerate(ref):
        fault = page not in f
        replaced = EMPTY
        if fault:
            faults += 1
            if EMPTY in f:
                idx = f.index(EMPTY)
                f[idx] = page
            else:
                future = {}
                for p in f:
                    try:
                        future[p] = ref.index(p, i + 1)
                    except ValueError:
                        future[p] = float('inf')
                victim = max(future, key=lambda p: future[p])
                replaced = victim
                f[f.index(victim)] = page
        steps.append({
            "page_ref": page,
            "frame_state": f[:],
            "is_fault": fault,
            "replaced_page": replaced
        })
    hits = len(ref) - faults
    return {"steps": steps, "page_faults": faults,
            "hit_ratio": round(hits / len(ref) * 100, 2)}

ALGO_MAP = {"FIFO": _fifo, "LRU": _lru, "Optimal": _optimal}

# ── Routes ───────────────────────────────────────────────────

@app.route("/health", methods=["GET"])
def health():
    return jsonify({"status": "ok", "port": PORT})

@app.route("/simulate", methods=["POST"])
def simulate():
    data = request.get_json(force=True)

    ref = data.get("ref_string", [])
    frames = int(data.get("frames", 3))
    algos  = data.get("algorithms", ["FIFO", "LRU", "Optimal"])

    # Validate
    if not ref or len(ref) > MAX_REF:
        return jsonify({"error": "ref_string must be 1–100 pages"}), 400
    if not (1 <= frames <= MAX_FRAMES):
        return jsonify({"error": "frames must be 1–10"}), 400
    if not all(isinstance(p, int) and 0 <= p <= 99 for p in ref):
        return jsonify({"error": "pages must be integers 0–99"}), 400

    results = {}
    for algo in algos:
        if algo not in ALGO_MAP:
            return jsonify({"error": f"Unknown algorithm: {algo}"}), 400
        results[algo] = ALGO_MAP[algo](ref, frames)

    return jsonify({
        "ref_string": ref,
        "frames": frames,
        "results": results
    })

# ── Main ─────────────────────────────────────────────────────

if __name__ == "__main__":
    ensure_c_binary()
    print(f"🚀  Page Replacement Simulator API running at http://localhost:{PORT}")
    print(f"    Open web/index.html in your browser.")
    app.run(host="0.0.0.0", port=PORT, debug=True)
