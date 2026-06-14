# DCC Causal Prevention for Falco

[![Status](https://img.shields.io/badge/Status-Hardened--Prototype-blue)](ROADMAP.md)
[![Project](https://img.shields.io/badge/BioOS-Causal--Security-green)](https://metaspace.bio)

## Hardened Architecture: Inline Causal Prevention

This module evolves Falco from a reactive monitoring tool into a **Proactive Causal Prevention System (CPS)**. It implements **Digital Causal Closure (DCC)** directly in the eBPF datapath.

### Hardened Implementation

- **Inline Prevention:** The `falco_dcc_exec_guard` eBPF module uses LSM hooks to return `-EPERM` at the kernel level, physically blocking orphaned processes before they execute.
- **libsinsp Enrichment:** The C++ bridge (`sinsp_bridge.cpp`) has been refactored to perform direct, low-latency `bpf_map_lookup_elem` calls to the `global_dcc_map`.
- **Hardware-Anchored Identity:** Syscalls are verified against hardware-anchored tokens issued by the BioOS kernel, closing the semantic gap.

### Security Guarantees

1. **Fail-Closed Enforcement:** All sensitive syscalls (`execve`, `connect`) are blocked by default if no valid causal lineage is found.
2. **Zero-Latency Prevention:** Decisions are made inline in kernel-space, preventing "detection-to-response" race conditions.
3. **Atomic Veracity:** Tokens are single-use (Anti-Replay) and time-bound (500ms window).

### Scientific Foundation

This implementation is based on the [BioOS Causal Constitution (DOI: 10.5281/zenodo.20384700)](https://doi.org/10.5281/zenodo.20384700).

---
*Verified by MetaSpace BioOS Team | [metaspace.bio](https://metaspace.bio)*
