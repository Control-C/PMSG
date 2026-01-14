# PMSG – Communication Protocol

PMSG uses a **simple JSON-based protocol** over BLE, Serial, or other links.

---

## Message Structure
```json
{
  "p": "pmsg",
  "v": 1,
  "t": "cmd | ack | evt",
  "id": "COMMAND_NAME",
  "seq": 12,
  "arg": "optional"
}