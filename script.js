/**
 * =============================================================================
 * Page Replacement Simulator — Frontend JavaScript
 * =============================================================================
 * Handles:
 *   - Form submission and input validation
 *   - API call to Node.js backend (POST /simulate)
 *   - Rendering step-by-step tables for each algorithm
 *   - Rendering comparison table when "Compare All" is selected
 * =============================================================================
 */

/* ─── DOM References ──────────────────────────────────────────────────────── */
const form        = document.getElementById('sim-form');
const btnRun      = document.getElementById('btn-run');
const btnText     = document.getElementById('btn-text');
const spinner     = document.getElementById('spinner');
const errorMsg    = document.getElementById('error-msg');
const resultsDiv  = document.getElementById('results');
const outputInner = document.getElementById('output-inner');

/* ─── Backend URL ─────────────────────────────────────────────────────────── */
// Change this if your server runs on a different port
const API_URL = 'http://localhost:3000/simulate';

/* ─── Form Submit Handler ─────────────────────────────────────────────────── */
form.addEventListener('submit', async (e) => {
    e.preventDefault();
    await runSimulation();
});

/**
 * Main function: reads form, sends request, renders result.
 */
async function runSimulation() {
    // ── Read inputs ────────────────────────────────────────────────────────
    const pagesInput = document.getElementById('pages').value.trim();
    const framesInput = document.getElementById('frames').value.trim();
    const algorithm   = document.getElementById('algorithm').value;

    // ── Validate inputs ────────────────────────────────────────────────────
    const validationError = validateInputs(pagesInput, framesInput);
    if (validationError) {
        showError(validationError);
        return;
    }

    hideError();

    // ── UI: loading state ──────────────────────────────────────────────────
    setLoading(true);
    resultsDiv.classList.remove('visible');

    try {
        // ── Send request to Node.js server ─────────────────────────────────
        const response = await fetch(API_URL, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({
                algorithm: algorithm,
                frames:    parseInt(framesInput),
                pages:     pagesInput
            })
        });

        const data = await response.json();

        if (!data.success) {
            throw new Error(data.error || 'Simulation failed');
        }

        // ── Render results ─────────────────────────────────────────────────
        renderResults(data.results, algorithm);
        resultsDiv.classList.add('visible');

        // Scroll to results
        resultsDiv.scrollIntoView({ behavior: 'smooth', block: 'start' });

    } catch (err) {
        showError('Could not connect to server. Make sure Node.js server is running on port 3000.\n' + err.message);
    } finally {
        setLoading(false);
    }
}

/* ─── Input Validation ────────────────────────────────────────────────────── */
function validateInputs(pagesStr, framesStr) {
    if (!pagesStr) {
        return 'Please enter a page reference string.';
    }

    // Check each token is a valid integer
    const tokens = pagesStr.split(/\s+/);
    for (const token of tokens) {
        if (isNaN(parseInt(token)) || !/^\d+$/.test(token.trim())) {
            return `Invalid page number: "${token}". Use space-separated integers only.`;
        }
    }

    const frames = parseInt(framesStr);
    if (isNaN(frames) || frames < 1 || frames > 10) {
        return 'Number of frames must be between 1 and 10.';
    }

    return null; // No error
}

/* ─── Render Results ──────────────────────────────────────────────────────── */
/**
 * Clears old output and renders all algorithm result blocks.
 * @param {Array}  results   - Array of parsed result objects
 * @param {string} algorithm - Selected algorithm key
 */
function renderResults(results, algorithm) {
    outputInner.innerHTML = '';

    const isCompare = algorithm === 'all';

    if (isCompare && results.length > 1) {
        // ── Comparison summary table at the top ────────────────────────────
        outputInner.appendChild(buildCompareTable(results));
        outputInner.appendChild(document.createElement('hr')).className = 'divider';
    }

    // ── Individual algorithm step tables ───────────────────────────────────
    results.forEach((result, index) => {
        const block = buildAlgoBlock(result, index);
        outputInner.appendChild(block);
    });
}

/* ─── Build Single Algorithm Block ───────────────────────────────────────── */
function buildAlgoBlock(result, index) {
    const { algorithm, steps, summary, frames } = result;

    const wrapper = document.createElement('div');
    wrapper.className = 'algo-block card';
    wrapper.style.animationDelay = `${index * 0.08}s`;

    // ── Algorithm header ───────────────────────────────────────────────────
    const tagClass = `tag-${algorithm.toLowerCase()}`;
    wrapper.innerHTML = `
        <div class="algo-header">
            <div class="algo-name">
                ${algorithmFullName(algorithm)}
                <span class="algo-tag ${tagClass}">${algorithm}</span>
            </div>
        </div>

        <!-- Stats row -->
        <div class="stats-row">
            <div class="stat-box hits">
                <div class="stat-value">${summary.hits}</div>
                <div class="stat-label">Hits</div>
            </div>
            <div class="stat-box faults">
                <div class="stat-value">${summary.faults}</div>
                <div class="stat-label">Faults</div>
            </div>
            <div class="stat-box total">
                <div class="stat-value">${summary.total}</div>
                <div class="stat-label">Total</div>
            </div>
            <div class="stat-box rate">
                <div class="stat-value">${summary.hitRate.toFixed(1)}%</div>
                <div class="stat-label">Hit Rate</div>
            </div>
        </div>

        <!-- Legend -->
        <div class="legend">
            <div class="legend-item">
                <div class="legend-dot hit"></div> Hit (page already in memory)
            </div>
            <div class="legend-item">
                <div class="legend-dot fault"></div> Fault (page loaded into frame)
            </div>
        </div>
    `;

    // ── Step-by-step table ─────────────────────────────────────────────────
    const tableWrapper = document.createElement('div');
    tableWrapper.className = 'table-wrapper';
    tableWrapper.appendChild(buildStepTable(steps, frames));
    wrapper.appendChild(tableWrapper);

    return wrapper;
}

/* ─── Build Step-by-Step Table ────────────────────────────────────────────── */
function buildStepTable(steps, nFrames) {
    const table = document.createElement('table');
    table.className = 'step-table';

    // ── Header row ─────────────────────────────────────────────────────────
    const thead = table.createTHead();
    const headRow = thead.insertRow();

    const headers = ['Step', 'Page'];
    for (let i = 0; i < nFrames; i++) {
        headers.push(`Frame ${i + 1}`);
    }
    headers.push('Status');

    headers.forEach(h => {
        const th = document.createElement('th');
        th.textContent = h;
        headRow.appendChild(th);
    });

    // ── Data rows ──────────────────────────────────────────────────────────
    const tbody = table.createTBody();

    steps.forEach((step, stepIndex) => {
        const tr = tbody.insertRow();
        tr.className = step.status === 'HIT' ? 'row-hit' : 'row-fault';

        // Step number
        addCell(tr, stepIndex + 1, 'step-num');

        // Page number
        const pageCell = addCell(tr, '', 'page-cell');
        const pageSpan = document.createElement('span');
        pageSpan.className = 'page-num';
        pageSpan.textContent = step.page;
        pageCell.appendChild(pageSpan);

        // Frame cells
        step.frames.forEach(f => {
            const td = tr.insertCell();
            td.className = 'frame-cell';

            if (f === null || f === undefined) {
                td.innerHTML = '<span class="frame-empty">—</span>';
            } else {
                td.textContent = f;
                // Highlight the page that was just loaded on a FAULT
                if (step.status === 'FAULT' && f === step.page) {
                    td.classList.add('active');
                }
            }
        });

        // Status badge
        const statusTd = addCell(tr, '', 'status-cell');
        const badge = document.createElement('span');
        badge.className = step.status === 'HIT' ? 'badge badge-hit' : 'badge badge-fault';
        badge.textContent = step.status;
        statusTd.appendChild(badge);
    });

    return table;
}

/* ─── Build Comparison Table ──────────────────────────────────────────────── */
function buildCompareTable(results) {
    const card = document.createElement('div');
    card.className = 'card';
    card.innerHTML = `<div class="card-title">Algorithm Comparison</div>`;

    const tableWrapper = document.createElement('div');
    tableWrapper.className = 'table-wrapper';

    const table = document.createElement('table');
    table.className = 'compare-table';

    // Header
    const thead = table.createTHead();
    const headRow = thead.insertRow();
    ['Algorithm', 'Hits', 'Faults', 'Total', 'Hit Rate', 'Fault Rate'].forEach(h => {
        const th = document.createElement('th');
        th.textContent = h;
        headRow.appendChild(th);
    });

    // Find best (lowest faults)
    const minFaults = Math.min(...results.map(r => r.summary.faults));
    const maxHitRate = Math.max(...results.map(r => r.summary.hitRate));

    // Body
    const tbody = table.createTBody();
    results.forEach(result => {
        const s = result.summary;
        const tr = tbody.insertRow();

        if (s.faults === minFaults) tr.className = 'best-row';

        addCell(tr, algorithmFullName(result.algorithm));

        const hitsCell = tr.insertCell();
        hitsCell.style.color = 'var(--hit-color)';
        hitsCell.style.fontWeight = '700';
        hitsCell.textContent = s.hits;

        const faultsCell = tr.insertCell();
        faultsCell.style.color = 'var(--fault-color)';
        faultsCell.style.fontWeight = '700';
        faultsCell.textContent = s.faults;

        addCell(tr, s.total);

        const rateCell = tr.insertCell();
        rateCell.style.color = 'var(--gold)';
        rateCell.style.fontWeight = '700';
        rateCell.textContent = s.hitRate.toFixed(2) + '%';
        if (s.hitRate === maxHitRate) rateCell.className = 'best-cell';

        addCell(tr, (100 - s.hitRate).toFixed(2) + '%');
    });

    tableWrapper.appendChild(table);
    card.appendChild(tableWrapper);
    return card;
}

/* ─── Utilities ───────────────────────────────────────────────────────────── */

/** Add a td cell with optional class and text */
function addCell(tr, text, className = '') {
    const td = tr.insertCell();
    if (className) td.className = className;
    td.textContent = text;
    return td;
}

/** Human-readable algorithm names */
function algorithmFullName(algo) {
    const names = {
        'FIFO':    'First-In First-Out',
        'LRU':     'Least Recently Used',
        'OPTIMAL': 'Optimal (Bélády\'s)'
    };
    return names[algo.toUpperCase()] || algo;
}

/** Show loading spinner on button */
function setLoading(isLoading) {
    btnRun.disabled = isLoading;
    if (isLoading) {
        btnRun.classList.add('loading');
        spinner.style.display = 'inline-block';
        btnText.textContent = 'Running...';
    } else {
        btnRun.classList.remove('loading');
        spinner.style.display = 'none';
        btnText.textContent = 'Run Simulation';
    }
}

/** Show error message */
function showError(msg) {
    errorMsg.textContent = msg;
    errorMsg.classList.add('visible');
}

/** Hide error message */
function hideError() {
    errorMsg.classList.remove('visible');
}

/* ─── Live input formatting ───────────────────────────────────────────────── */
// Auto-clean pages input: replace commas/multiple spaces with single space
document.getElementById('pages').addEventListener('blur', (e) => {
    e.target.value = e.target.value
        .replace(/,/g, ' ')           // Replace commas with spaces
        .replace(/\s+/g, ' ')         // Collapse multiple spaces
        .trim();
});
