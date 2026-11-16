 MSH Shell Test Plan

**Project:** MSH — Minimal Unix Shell  
**Course:** ICS 2305 — Systems Programming  
**Student:** PHILLIP kIMONYI  
**Student ID:** SCT212-0057/2024


## 1. Lecturer's Required Tests

| No. | Input / Action | Expected Behaviour | Actual Result |
|---|---|---|---|
| 1 | Press Enter on a blank line | New prompt appears without an error | To be recorded
 | 2 | `echo hello world` | Prints hello world; next status is 0 
mu| | To be recorded
 | 3 | `ls /nonexistent` | ls reports an error; status is non-zero | To be recorded
 | 4 | `nosuchcommand` | Error message appears; 
| status is 127 | To be recorded
 | 5 | `cd /tmp`, then `pwd` | Displays /tmp | To be recorded
 | 6 | `sleep 3 &` | Prompt returns immediately and PID is 
| displayed | To be recorded
 | 7 | `echo hi > f.txt`, then `cat f.txt` | File contains hi | To be recorded
 | 8 | `sleep 100`, then press Ctrl+C | Sleep 
| stops; shell survives; status is 130 | To be recorded
 | 9 | Press Ctrl+D at the prompt | Shell exits cleanly | To be recorded |

## 2. Additional Student Tests

| No. | Input / Action | Expected Behaviour | Actual Result |
|---|---|---|---|
| 10 | Run `cd` without an argument, then `pwd` | Shell changes to the HOME directory | To be recorded |
| 11 | Run `false`, then `true` | Prompt first shows status 1, then status 0 | To be recorded |
| 12 | Run `echo first > f.txt`, then `echo second > f.txt`, then `cat f.txt` | File is truncated; it contains only second | To be recorded |
| 13 | Run `sleep 2 &`, then `sleep 4` | Background process finishes and is reaped while foreground command runs | To be recorded |

## 3. Test Execution Notes

- Run the tests using the compiled `msh` executable.
- Record the actual result after performing each test.
- Do not mark a test as passed unless the observed behaviour matches the expected behaviour.
- Record any failures and corrections made.
