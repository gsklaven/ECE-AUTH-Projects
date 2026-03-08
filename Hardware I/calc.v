`include "alu.v"
`include "calc_enc.v"

module calc(
  input clk,
  input btnc,
  input btnl,
  input btnu,
  input btnr,
  input btnd,
  input [15:0] sw,
  output reg [15:0] led
);
  
  reg [15:0] accumulator;
  wire [31:0] result;
  wire zero;
  wire [3:0] alu_op;
  
  wire signed [31:0] op1, op2;
       
  alu alu_instance (
        .op1(op1),
        .op2(op2),
        .alu_op(alu_op),
        .zero(zero), 
        .result(result)
  );

  calc_enc encoder (
        .btnc(btnc),
        .btnr(btnr),
        .btnl(btnl),
        .alu_op(alu_op)
  );
  
  assign op1 = {{16{accumulator[15]}}, accumulator};
  assign op2 = {{16{sw[15]}}, sw};
  
  always @(posedge clk) begin
    if (btnu) begin
      accumulator <= 16'b0; 
    end
    if (btnd) begin
      accumulator <= result[15:0];
  	end
  end

  always @(*) begin
      led = accumulator;
  end
  
endmodule

  