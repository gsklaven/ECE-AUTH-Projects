module alu(
  input [31:0] op1,
  input [31:0] op2,
  input [3:0] alu_op,
  output reg zero,
  output reg [31:0] result
);
  
  parameter [3:0] ALUOP_AND = 4'b0000;
  parameter [3:0] ALUOP_OR  = 4'b0001;
  parameter [3:0] ALUOP_ADD = 4'b0010;
  parameter [3:0] ALUOP_SUB = 4'b0110;
  parameter [3:0] ALUOP_LT  = 4'b0100;  
  parameter [3:0] ALUOP_SRL = 4'b1000;  
  parameter [3:0] ALUOP_SLL = 4'b1001; 
  parameter [3:0] ALUOP_SRA = 4'b1010;  
  parameter [3:0] ALUOP_XOR = 4'b0101;  
  
  always @(*) begin
    case(alu_op)
      ALUOP_AND: result = op1 & op2;  
      ALUOP_OR: result = op1 | op2;  
      ALUOP_ADD: result = op1 + op2; 
      ALUOP_SUB: result = op1 - op2;  
      ALUOP_LT: result = ($signed(op1) < $signed(op2)) ? 32'b1 : 32'b0;
      ALUOP_SRL: result = op1 >> op2[4:0];  
      ALUOP_SLL: result = op1 << op2[4:0];  
      ALUOP_SRA: result = $unsigned($signed(op1) >>> op2[4:0]);  
      ALUOP_XOR: result = op1 ^ op2;
      default : result = 32'b0;
    endcase
    zero = (result == 32'b0) ? 1 : 0;
  end

endmodule
