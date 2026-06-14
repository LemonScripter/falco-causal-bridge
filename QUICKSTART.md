# Quickstart: DCC Causal Prevention for Falco

This guide provides a 3-step environment to verify the **Digital Causal Closure (DCC)** logic for Falco.

## Prerequisites
- **G++** (C++11 or later)
- **Linux** (for kernel enforcement) or **macOS** (for logic demo)

## Step 1: Build the Logic Proof
```bash
git clone https://github.com/LemonScripter/falco-causal-bridge.git
cd falco-causal-bridge
make build
```

## Step 2: Run the Automated Proof
Execute the reproduction script to see the "Inline Prevention" logic in action:
```bash
./reproduce.sh
```

## Step 3: Production Deployment
For live prevention, deploy the eBPF module on a BioOS node. It will hook into the Linux Security Module (LSM) layer to block unauthorized syscalls at zero-latency.

## Verification Scenarios
- **Verified:** Processes initiated through a hardware-anchored intent are allowed to proceed.
- **Fail-Closed:** Any process missing a verified causal lineage is physically blocked from execution (`-EPERM`).

---
*Production-Grade Research Prototype by MetaSpace BioOS Team*
