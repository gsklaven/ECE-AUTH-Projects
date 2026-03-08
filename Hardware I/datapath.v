`include "alu.v"
`include "regfile.v"

module datapath #(
    parameter [31:0] INITIAL_PC = 32'h00400000
)
(
    input wire clk, 
    input wire rst, 
    input wire [31:0] instr, 
    input wire PCSrc, 
    input wire ALUSrc, 
    input wire RegWrite, 
    input wire MemToReg, 
    input wire [3:0] ALUCtrl,
    input wire loadPC,
    output wire [31:0] PC, 
    output wire Zero,
    output reg [31:0] dAddress, 
    output reg [31:0] dWriteData, 
    input wire [31:0] dReadData, 
    output reg [31:0] WriteBackData
);

  localparam [6:0] IMMEDIATE     = 7'b0010011;
  localparam [6:0] NON_IMMEDIATE = 7'b0110011; 
  localparam [6:0] LW           = 7'b0000011;
  localparam [6:0] SW           = 7'b0100011;
  localparam [6:0] BEQ          = 7'b1100011;
  
  reg [31:0] PC_inter = INITIAL_PC; 
  reg [31:0] immediate;    
  reg [31:0] op2;
  wire [31:0] result_inter;

  wire [31:0] immediate_I = {{20{instr[31]}}, instr[31:20]};
  wire [31:0] immediate_S = {{20{instr[31]}}, instr[31:25], instr[11:7]};
  wire [31:0] immediate_B = {{19{instr[31]}}, instr[31], instr[7], instr[30:25], instr[11:8], 1'b0};

  wire [31:0] readData1, readData2;

  // Register file
  regfile registers(
    .clk(clk),
    .write(RegWrite),
    .readReg1(instr[19:15]),
    .readReg2(instr[24:20]),
    .writeReg(instr[11:7]),
    .writeData(WriteBackData),
    .readData1(readData1),
    .readData2(readData2)
  );

  // ALU
  alu alu_instance(
    .op1(readData1),
    .op2(op2),
    .alu_op(ALUCtrl),
    .zero(Zero),
    .result(result_inter)
  );
  
  assign PC = PC_inter;

  always @(*) begin
    if (instr[6:0] == SW) begin
      immediate = immediate_S;
    end 
    else if (instr[6:0] == BEQ) begin
      immediate = immediate_B;
    end
    else if ( (instr[6:0] == IMMEDIATE) || (instr[6:0] == LW) ) begin
      immediate = immediate_I;
    end
  end

  always @(posedge clk) begin
    if (rst) begin
      PC_inter <= INITIAL_PC; 
    end 
    else if (loadPC) begin
      if (PCSrc) 
        PC_inter <= PC_inter + immediate;
      else
        PC_inter <= PC_inter + 4;
    end
  end

  always @(*) begin
    if (ALUSrc) 
      op2 = immediate;
    else
      op2 = readData2;

    dWriteData = readData2;
    dAddress   = result_inter;

    if (MemToReg) 
      WriteBackData = dReadData;
    else 
      WriteBackData = result_inter;
  end

endmodule