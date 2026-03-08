module regfile #(
    parameter DATAWIDTH = 32
)(
    input clk,
    input [4:0] readReg1,
    input [4:0] readReg2,
    input [4:0] writeReg,
    input [DATAWIDTH-1:0] writeData,
    input write,
    output reg [DATAWIDTH-1:0] readData1,
    output reg [DATAWIDTH-1:0] readData2
);

    reg [DATAWIDTH-1:0] registers [31:0];
    integer i;

    initial begin
        for (i = 0; i < 32; i = i + 1) begin
            registers[i] = 0;
        end
    end

  always @(posedge clk) begin
        if (write) begin
            registers[writeReg] <= writeData;
        end
      
        readData1 <= registers[readReg1];
        readData2 <= registers[readReg2];
    
     	if (write && (writeReg == readReg1)) begin
            readData1 <= writeData;
        end
        if (write && (writeReg == readReg2)) begin
            readData2 <= writeData; 
        end
    end

endmodule
