#include <linux/bpf.h>
#include <bpf/libbpf.h>
#include <bpf/bpf.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include "sinsp.h"
#include "sinsp_int.h"

/*
 * libsinsp Causal Bridge (User Space / C++) - Hardened
 * 
 * This component enriches Falco's sinsp_evt with real-time DCC metadata.
 * It performs direct eBPF map lookups to verify the causal status 
 * of a system call before it is processed by the rules engine.
 */

namespace falco_dcc {

    class CausalEnrichment {
    public:
        CausalEnrichment() : m_map_fd(-1) {
            // Hardened: Loading the pinned map from the BPF filesystem
            m_map_fd = bpf_obj_get("/sys/fs/bpf/falco/global_dcc_map");
            if (m_map_fd < 0) {
                std::cerr << "DCC Warning: Failed to open causal map at /sys/fs/bpf/falco/global_dcc_map" << std::endl;
            }
        }

        ~CausalEnrichment() {
            if (m_map_fd >= 0) {
                close(m_map_fd);
            }
        }

        /**
         * @brief Enriches the system event with causal context.
         */
        void enrich(sinsp_evt* evt) {
            if (m_map_fd < 0) {
                evt->set_custom_field("causal.verified", false);
                return;
            }

            uint32_t pid = evt->get_thread_info()->m_pid;
            
            // Perform real-time kernel state verification
            bool verified = verify_causality_state(pid);
            
            evt->set_custom_field("causal.verified", verified);
            evt->set_custom_field("causal.origin", verified ? "verified_hardware_irq" : "autonomous_orphaned_call");
        }

    private:
        int m_map_fd;

        struct dcc_token {
            uint64_t timestamp;
            uint32_t intent_id;
            uint32_t age_limit_ns;
            uint8_t  consumed;
        };

        bool verify_causality_state(uint32_t pid) {
            struct dcc_token token;
            
            // Atomic lookup in the kernel-anchored map
            if (bpf_map_lookup_elem(m_map_fd, &pid, &token) < 0) {
                return false; // Fail-Closed: No token = No causality
            }

            // Logic matching the kernel-side dcc_bridge.bpf.c
            uint64_t now = get_nanoseconds(); 
            if (now - token.timestamp > token.age_limit_ns) return false;
            if (token.consumed) return false;

            return true;
        }

        uint64_t get_nanoseconds() {
            struct timespec ts;
            clock_gettime(CLOCK_MONOTONIC, &ts);
            return (uint64_t)ts.tv_sec * 1000000000ULL + ts.tv_nsec;
        }
    };

} // namespace falco_dcc
