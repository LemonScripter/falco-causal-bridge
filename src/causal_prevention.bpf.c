/*
 * Falco DCC Causal Prevention Module (Kernel Space)
 * 
 * This eBPF program extends Falco's modern eBPF probe with Inline Prevention.
 * It uses Digital Causal Closure (DCC) to block syscalls that lack 
 * a verified causal trigger.
 */

#include <linux/bpf.h>
#include <linux/ptrace.h>
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>

char LICENSE[] SEC("license") = "GPL";

struct dcc_token {
    __u64 timestamp;
    __u32 intent_id;
    __u32 age_limit_ns;
    __u8  consumed;
};

/* Shared map with the global DCC engine */
struct {
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, 10240);
    __type(key, __u32);   /* PID */
    __type(value, struct dcc_token);
} global_dcc_map SEC(".maps");

/* 
 * Inline Causal Validator 
 * Blocks the execution if no valid causal link is found.
 */
static __always_inline int check_causality(__u32 pid, __u32 required_intent) {
    struct dcc_token *token;
    __u64 now = bpf_ktime_get_ns();

    token = bpf_map_lookup_elem(&global_dcc_map, &pid);
    if (!token) return 0; // No causality found

    if (now - token->timestamp > token->age_limit_ns) return 0; // Expired
    if (token->consumed) return 0; // Replay attempt
    if (token->intent_id != required_intent) return 0; // Intent mismatch

    token->consumed = 1; // Atomic closure
    return 1;
}

/* LSM Hook for Process Execution Prevention */
SEC("lsm/bprm_check_security")
int BPF_PROG(falco_dcc_exec_guard, struct linux_binprm *bprm) {
    __u32 pid = bpf_get_current_pid_tgid() >> 32;

    /* Intent ID for Process Spawn = 0x200 */
    if (!check_causality(pid, 0x200)) {
        /* 
         * Physically prevent the execution. 
         * This transforms Falco from IDS (detection) to IPS (prevention).
         */
        return -1; // -EPERM
    }

    return 0; // Allow
}
