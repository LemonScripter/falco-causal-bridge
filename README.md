# DCC Causal Prevention for Falco

[![Verified](https://img.shields.io/badge/Verified-Tokyo--Node-green)](VERIFICATION.md)
[![Status](https://img.shields.io/badge/Status-Hardened--Prototype-blue)](ROADMAP.md)
[![Project](https://img.shields.io/badge/BioOS-Causal--Security-green)](https://metaspace.bio)
[![DOI](https://img.shields.io/badge/DOI-10.5281%2Fzenodo.20384700-purple)](https://doi.org/10.5281/zenodo.20384700)

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

### Scientific & Technical Foundation

This implementation is based on the following formal specifications and research:

- **Research Paper:** [The Causal Operating System: Digital Causal Closure for Autonomous Systems](https://doi.org/10.5281/zenodo.20384700) (DOI: 10.5281/zenodo.20384700)
- **Formal Specification:** [BioOS Causal Constitution (PDF)](https://bioos.metaspace.bio/bioos_causal_constitution_en.pdf)

---
*MetaSpace.Bio Logic Project | [metaspace.bio](https://metaspace.bio) | [admin@metaspace.bio](mailto:admin@metaspace.bio)*
