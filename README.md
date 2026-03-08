# hw-1

This repository contains two Verilog hardware design projects:

## 1. FPGA Calculator (`calc.v`)

A button-driven calculator targeting FPGA boards (e.g., Basys3). It uses an ALU and a button encoder to perform arithmetic and logical operations on 16-bit values displayed via switches and LEDs.

- **`calc.v`** – Top-level calculator module with accumulator, button inputs, and LED output.
- **`alu.v`** – 32-bit ALU supporting AND, OR, ADD, SUB, SLT, XOR, SLL, SRL, and SRA operations.
- **`calc_enc.v`** – Button encoder that maps FPGA button inputs to ALU operation codes.
- **`tb_calc.v`** – Testbench for the calculator module.

## 2. Multi-Cycle RISC-V Processor (`top_proc.v`)

A multi-cycle processor implementing a subset of the RISC-V instruction set (RV32I). Control is managed by a 5-stage Finite State Machine (IF → ID → EX → MEM → WB).

- **`top_proc.v`** – Top-level processor module with FSM-based control unit.
- **`datapath.v`** – Datapath including the PC, register file, ALU, and immediate generation.
- **`regfile.v`** – 32×32-bit register file with forwarding support.
- **`tb_top_proc.v`** – Testbench for the processor.
- **`FSM.excalidraw` / `fsm.png`** – FSM diagram illustrating the processor's control flow.

### Supported Instructions
- **R-type**: `ADD`, `SUB`, `AND`, `OR`, `XOR`, `SLT`, `SLL`, `SRL`, `SRA`
- **I-type**: `ADDI`, `ANDI`, `ORI`, `XORI`, `SLTI`, `SLLI`, `SRLI`, `SRAI`, `LW`
- **S-type**: `SW`
- **B-type**: `BEQ`