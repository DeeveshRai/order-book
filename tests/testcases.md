# Order Book V1 Test Suite

# CLI INSTRUCTIONS:

- To use the CLI:
Input 1 (as its own input) to the CLI, this will change the mode to order recieving
To Move out of the order recieving mode of the CLI type `exit` into the CLI
To Move into the cancel mode of the CLI input 2 (as its own input to the CLI)
You can move directly from cancel to order recieving by inputting 1

TO RUN THE ORDERBOOK:
.\orderbook.exe

ORDER RULES:
- MUST BE CAPITALISED 

LIMIT ORDERS:
- MUST FOLLOW THE STRUCTURE -> LIMIT BUY/SELL QTY PRICE

MARKET ORDERS:
- MUST FOLOOW THE STRUCTURE -> MARKET BUY/SELL QTY

CANCELLATION:
- cancel id
- IDS can be retrieved from the CLI output of adding orders, when an order node is added, it will be outputted with all order info into the CLI

# COPILOT INSTRUCTIONS:

- You are to try EVERY test case within this file, and construct a testresults.md file
- In this testresults.md file say which tests passed and failed with no extra information

## Test 1 - Exact Match

### Input

```text
LIMIT BUY 100 10
LIMIT SELL 100 10
```

### Expected

```text
Buy book empty
Sell book empty
```

---

## Test 2 - Partial Fill Buy Survives

### Input

```text
LIMIT BUY 100 10
LIMIT SELL 50 10
```

### Expected

```text
BUY 50 @ 10 remains
```

---

## Test 3 - Partial Fill Sell Survives

### Input

```text
LIMIT SELL 100 10
LIMIT BUY 50 10
```

### Expected

```text
SELL 50 @ 10 remains
```

---

## Test 4 - Multi-Level Market Buy

### Input

```text
LIMIT SELL 100 10
LIMIT SELL 100 11
LIMIT SELL 100 12

MARKET BUY 250
```

### Expected

```text
10 level removed
11 level removed
12 level remains with qty 50
```

---

## Test 5 - Multi-Level Market Sell

### Input

```text
LIMIT BUY 100 12
LIMIT BUY 100 11
LIMIT BUY 100 10

MARKET SELL 250
```

### Expected

```text
12 level removed
11 level removed
10 level remains with qty 50
```

---

## Test 6 - FIFO Priority (Sell Side)

### Input

```text
LIMIT SELL 100 10
LIMIT SELL 90 10
LIMIT SELL 80 10

MARKET BUY 150
```

### Expected

```text
First order removed
Second order qty = 40
Third order untouched
```

---

## Test 7 - FIFO Priority (Buy Side)

### Input

```text
LIMIT BUY 100 10
LIMIT BUY 90 10
LIMIT BUY 80 10

MARKET SELL 150
```

### Expected

```text
First order removed
Second order qty = 40
Third order untouched
```

---

## Test 8 - Head Removal

### Input

```text
LIMIT BUY 100 10
LIMIT BUY 90 10

MARKET SELL 100
```

### Expected

```text
Head advances to second order
Remaining qty = 90
```

---

## Test 9 - Crossing Multiple Levels

### Input

```text
LIMIT BUY 100 10
LIMIT BUY 100 11

LIMIT SELL 150 10
```

### Expected

```text
BUY 100 @ 11 consumed
BUY 50 @ 10 consumed

Remaining:
BUY 50 @ 10
```

---

## Test 10 - Non-Crossing Order

### Input

```text
LIMIT BUY 100 10
LIMIT BUY 100 11

LIMIT SELL 150 11
```

### Expected

```text
BUY 100 @ 11 consumed

Remaining:
BUY 100 @ 10
SELL 50 @ 11
```

---

## Test 11 - Market Buy Empty Book

### Input

```text
MARKET BUY 100
```

### Expected

```text
No crash
Book unchanged
```

---

## Test 12 - Market Sell Empty Book

### Input

```text
MARKET SELL 100
```

### Expected

```text
No crash
Book unchanged
```

---

## Test 13 - Single Node Cancellation

### Input

```text
LIMIT BUY 100 10

CANCEL <id>
```

### Expected

```text
Buy book empty
Price level removed
ID removed
```

---

## Test 14 - Head Cancellation

### Input

```text
LIMIT BUY 100 10
LIMIT BUY 90 10

CANCEL <first id>
```

### Expected

```text
Second order becomes head
```

---

## Test 15 - Tail Cancellation

### Input

```text
LIMIT BUY 100 10
LIMIT BUY 90 10

CANCEL <second id>
```

### Expected

```text
First order remains head and tail
```

---

## Test 16 - Middle Cancellation

### Input

```text
LIMIT BUY 100 10
LIMIT BUY 90 10
LIMIT BUY 80 10

CANCEL <middle id>
```

### Expected

```text
100 -> 80 linkage preserved
```

---

## Test 17 - Price Level Removal

### Input

```text
LIMIT BUY 100 10
LIMIT SELL 100 10
```

### Expected

```text
Price level 10 erased
No dead levels remain
```

---

## Test 18 - Mixed Chaos Test

### Input

```text
LIMIT BUY 100 10
LIMIT BUY 90 10
LIMIT BUY 80 10

LIMIT SELL 50 10

CANCEL <middle id>

LIMIT SELL 60 10

LIMIT BUY 120 10

MARKET SELL 170

LIMIT SELL 100 11

MARKET BUY 200

```

### Expected

```text
No crashes
No invalid head/tail states
No inactive nodes referenced by price storage
ID map consistent
```

---

# Invariants To Verify After Every Test

```text
Head reachable from level
Tail reachable from level

Head != -1 => Tail != -1
Tail != -1 => Head != -1

No inactive node referenced by price storage

All prev/next links consistent

No price level exists with zero active orders

ID map never references inactive nodes
```
