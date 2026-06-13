# DCC Causal Prevention for Falco

## Overview
The **DCC Causal Prevention** module is a professional extension for the Falco ecosystem. It evolves Falco from a detection-only System (IDS) into a proactive **Causal Prevention System (CPS)** by integrating **Digital Causal Closure (DCC)** principles directly into the eBPF drivers and the `libsinsp` inspection library.

## The Problem: Reactive Latency
Falco is excellent at detecting anomalous behavior, but its prevention model is traditionally reactive. By the time an alert is processed by a response engine (like Falco Talon), the malicious syscall has often already been executed.

## The Solution: Inline Causal Enforcement
This module leverages **eBPF LSM hooks** to perform inline, nanosecond-latency verification of system calls. 
- **Causal Validation:** Every `execve`, `connect`, or `bpf` operation is verified against a hardware-anchored Causal Token.
- **Physical Blocking:** If no valid causal chain is found in the `global_dcc_map`, the kernel driver returns `-EPERM` *before* the operation completes.

## Scientific Background
This integration is based on the following formal research:
- [The Causal Operating System: Digital Causal Closure for Autonomous Systems](https://doi.org/10.5281/zenodo.20384700)
- [BioOS Causal Constitution (PDF)](https://bioos.metaspace.bio/bioos_causal_constitution_en.pdf)

## Components
- **`causal_prevention.bpf.c`**: eBPF driver patch for inline blocking.
- **`sinsp_bridge.cpp`**: C++ enrichment bridge for `libsinsp` to expose causal metadata to Falco rules.
- **`verify_falco.py`**: Logic verification suite ensuring 100% prevention accuracy.

## Upstreaming Proposal
We propose a `matchCausal` condition for Falco YAML rules, allowing administrators to define policies that physically prevent execution of orphaned workloads.

---
*Created by MetaSpace BioOS | [metaspace.bio](https://metaspace.bio) | [admin@metaspace.bio](mailto:admin@metaspace.bio)*
