/**
 * =============================================================================
 * Page Replacement Simulator - Node.js Backend Server
 * =============================================================================
 * This server acts as a bridge between the HTML frontend and the compiled C
 * program. It receives requests from the browser, calls the C executable,
 * and returns the structured output to be displayed in the GUI.
 *
 * How it works:
 *   Browser → POST /simulate → server.js → exec(./simulator) → parse → JSON
 *
 * Setup:
 *   1. npm install express cors
 *   2. node server.js
 *   3. Open http://localhost:3000 in browser
 * =============================================================================
 */

const express = require('express');
const cors    = require('cors');
const { exec } = require('child_process');
const path    = require('path');

const app  = express();
const PORT = 3000;

/* ─── Middleware ──────────────────────────────────────────────────────────── */
app.use(cors());                          // Allow cross-origin requests
app.use(express.json());                  // Parse JSON request bodies
app.use(express.static(path.join(__dirname, 'gui'))); // Serve GUI files

/* ─── Path to compiled C binary ──────────────────────────────────────────── */
const SIMULATOR_PATH = path.join(__dirname, 'backend', 'simulator');

/* ─── POST /simulate ──────────────────────────────────────────────────────── */
/**
 * Endpoint called by the frontend when user clicks "Run Simulation".
 *
 * Request body (JSON):
 *   { algorithm: "fifo"|"lru"|"optimal"|"all", frames: 3, pages: "1 2 3 4 1" }
 *
 * Response (JSON):
 *   { success: true, results: [ { algorithm, steps, summary } ] }
 */
app.post('/simulate', (req, res) => {
    const { algorithm, frames, pages } = req.body;

    /* ── Input Validation ─────────────────────────────────────────────────── */
    if (!algorithm || !frames || !pages) {
        return res.status(400).json({ success: false, error: 'Missing required fields' });
    }

    // Validate algorithm name (whitelist to prevent injection)
    const validAlgos = ['fifo', 'lru', 'optimal', 'all'];
    if (!validAlgos.includes(algorithm.toLowerCase())) {
        return res.status(400).json({ success: false, error: 'Invalid algorithm' });
    }

    // Validate frames is a positive integer
    const frameCount = parseInt(frames);
    if (isNaN(frameCount) || frameCount < 1 || frameCount > 10) {
        return res.status(400).json({ success: false, error: 'Frames must be 1-10' });
    }

    // Clean and validate pages string (only digits and spaces)
    const pagesClean = pages.trim().replace(/\s+/g, ' ');
    const pageList   = pagesClean.split(' ');

    if (pageList.length === 0 || pageList.some(p => isNaN(parseInt(p)))) {
        return res.status(400).json({ success: false, error: 'Invalid page reference string' });
    }

    /* ── Build command ────────────────────────────────────────────────────── */
    // e.g.: ./backend/simulator fifo 3 1 2 3 4 1 2 5
    const command = `"${SIMULATOR_PATH}" ${algorithm.toLowerCase()} ${frameCount} ${pagesClean}`;

    /* ── Execute C program ────────────────────────────────────────────────── */
    exec(command, { timeout: 5000 }, (error, stdout, stderr) => {
        if (error) {
            console.error('Exec error:', error.message);
            console.error('Stderr:', stderr);
            return res.status(500).json({
                success: false,
                error: 'Simulator execution failed: ' + (stderr || error.message)
            });
        }

        /* ── Parse C output ───────────────────────────────────────────────── */
        try {
            const results = parseSimulatorOutput(stdout);
            res.json({ success: true, results });
        } catch (parseError) {
            console.error('Parse error:', parseError.message);
            res.status(500).json({ success: false, error: 'Failed to parse simulator output' });
        }
    });
});

/* ─── parseSimulatorOutput ────────────────────────────────────────────────── */
/**
 * Parses the structured output from the C simulator.
 *
 * C output format:
 *   FRAMES|3
 *   PAGES|7
 *   ALGO|FIFO
 *   STEP|1|FAULT|1,-,-
 *   STEP|2|FAULT|1,2,-
 *   ...
 *   SUMMARY|2|5|7|28.57
 *   ---           ← separator (only in "all" mode)
 *   ALGO|LRU
 *   ...
 *
 * Returns array of result objects (one per algorithm).
 */
function parseSimulatorOutput(raw) {
    const lines   = raw.trim().split('\n');
    const results = [];

    let current   = null; // Current algorithm block being parsed
    let frames    = 0;
    let totalPages = 0;

    for (const line of lines) {
        const trimmed = line.trim();
        if (!trimmed) continue;

        if (trimmed.startsWith('FRAMES|')) {
            frames = parseInt(trimmed.split('|')[1]);

        } else if (trimmed.startsWith('PAGES|')) {
            totalPages = parseInt(trimmed.split('|')[1]);

        } else if (trimmed.startsWith('ALGO|')) {
            // Start a new algorithm block
            if (current) results.push(current);
            current = {
                algorithm: trimmed.split('|')[1],
                frames: frames,
                totalPages: totalPages,
                steps: [],
                summary: null
            };

        } else if (trimmed.startsWith('STEP|') && current) {
            // Parse: STEP|page|HIT_OR_FAULT|f0,f1,f2
            const parts      = trimmed.split('|');
            const page       = parseInt(parts[1]);
            const status     = parts[2]; // "HIT" or "FAULT"
            const frameState = parts[3].split(',').map(f => f === '-' ? null : parseInt(f));

            current.steps.push({ page, status, frames: frameState });

        } else if (trimmed.startsWith('SUMMARY|') && current) {
            // Parse: SUMMARY|hits|faults|total|hit_rate
            const parts = trimmed.split('|');
            current.summary = {
                hits:     parseInt(parts[1]),
                faults:   parseInt(parts[2]),
                total:    parseInt(parts[3]),
                hitRate:  parseFloat(parts[4])
            };

        } else if (trimmed === '---') {
            // Divider between algorithms in "all" mode
            if (current) {
                results.push(current);
                current = null;
            }
        }
    }

    // Push the last block
    if (current) results.push(current);

    return results;
}

/* ─── Start Server ────────────────────────────────────────────────────────── */
app.listen(PORT, () => {
    console.log('');
    console.log('╔════════════════════════════════════════╗');
    console.log('║   Page Replacement Simulator Server    ║');
    console.log('╠════════════════════════════════════════╣');
    console.log(`║   Running at: http://localhost:${PORT}     ║`);
    console.log('║   Press Ctrl+C to stop                 ║');
    console.log('╚════════════════════════════════════════╝');
    console.log('');
});
