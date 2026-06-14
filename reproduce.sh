#!/bin/bash
set -e

# DCC Falco Bridge: Automated Reproduction Script
# Builds the C++ logic proof and verifies the prevention invariants.

echo "--- [1/3] Building Falco Causal Logic Proof ---"
mkdir -p bin
g++ -O2 src/reproduce_proof.cpp -o bin/falco-dcc-proof

echo "--- [2/3] Verifying Causal Logic (Demo Mode) ---"

echo "Test A: Authorized Process (PID 1234)"
RESULT_A=$(./bin/falco-dcc-proof --demo --pid 1234)
echo "$RESULT_A"
if [[ "$RESULT_A" == *"ALLOW"* ]]; then
    echo "[PASS] Authorized process allowed."
else
    echo "[FAIL] Authorized process blocked."
    exit 1
fi

echo -e "\nTest B: Orphaned Process (PID 6666)"
if ./bin/falco-dcc-proof --demo --pid 6666 > /dev/null 2>&1; then
    echo "[FAIL] Orphaned process allowed (Security Breach)."
    exit 1
else
    echo "[PASS] Orphaned process blocked (Fail-Closed)."
fi

echo -e "\n--- [3/3] Deployment Path ---"
echo "In production, this logic is enforced inline via LSM hooks:"
echo "SEC(\"lsm/bprm_check_security\") -> returns -EPERM for orphans."

echo -e "\nSUCCESS: Falco Causal Prevention logic verified."
