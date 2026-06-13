import unittest
import time

# Falco DCC Prevention Logic Verification
# Ensures 100% pass rate for inline prevention invariants.

class TestFalcoDCCPrevention(unittest.TestCase):
    def setUp(self):
        self.dcc_map = {}
        self.INTENT_EXEC = 0x200
        self.CAUSALITY_WINDOW_NS = 500 * 1000 * 1000 # 500ms

    def issue_token(self, pid, intent):
        self.dcc_map[pid] = {
            "ts": time.time_ns(),
            "intent": intent,
            "consumed": False
        }

    def falco_ebpf_check(self, pid, intent):
        # Simulation of bprm_check_security LSM hook logic
        now = time.time_ns()
        if pid not in self.dcc_map:
            return -1 # EPERM (Block)
        
        token = self.dcc_map[pid]
        if now - token["ts"] > self.CAUSALITY_WINDOW_NS:
            return -1 # EPERM (Block)
            
        if token["consumed"]:
            return -1 # EPERM (Block)
            
        if token["intent"] != intent:
            return -1 # EPERM (Block)
            
        token["consumed"] = True
        return 0 # Allow

    def test_prevent_unauthorized_exec(self):
        # Autonomous execution without token must be blocked
        pid = 9999
        result = self.falco_ebpf_check(pid, self.INTENT_EXEC)
        self.assertEqual(result, -1)

    def test_allow_verified_exec(self):
        # Execution with fresh token must be allowed
        pid = 9999
        self.issue_token(pid, self.INTENT_EXEC)
        result = self.falco_ebpf_check(pid, self.INTENT_EXEC)
        self.assertEqual(result, 0)

    def test_prevent_replay_exec(self):
        # Token cannot be used twice for execution
        pid = 9999
        self.issue_token(pid, self.INTENT_EXEC)
        self.assertEqual(self.falco_ebpf_check(pid, self.INTENT_EXEC), 0)
        self.assertEqual(self.falco_ebpf_check(pid, self.INTENT_EXEC), -1)

if __name__ == "__main__":
    print("--- Running Falco DCC Prevention Logic Tests ---")
    unittest.main()
