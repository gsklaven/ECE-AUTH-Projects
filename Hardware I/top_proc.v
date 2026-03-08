`include "datapath.v"

module procedures #(
  parameter [31:0] INITIAL_PC = 32'h00400000
)(
  input clk,
  input rst, 
  input wire [31:0] instr,
  input wire [31:0] dReadData,
  input wire [31:0] PC,
  output wire [31:0] dAddress,
  output wire [31:0] dWriteData,
  output reg MemRead,
  output reg MemWrite,
  output wire [31:0] WriteBackData
);

  wire Zero;
  reg IFStage, IDStage, EXStage, MEMStage, WBStage;
  
  reg [2:0] FSM;
  reg PCSrc;
  reg ALUSrc;
  reg RegWrite;
  reg MemToReg;
  reg loadPC;
  
  reg [3:0] ALUCtrl;
  
  datapath #(.INITIAL_PC(INITIAL_PC)) datapath(
    .clk(clk), 
    .rst(rst), 
    .instr(instr), 
    .PCSrc(PCSrc), 
    .ALUSrc(ALUSrc), 
    .RegWrite(RegWrite), 
    .MemToReg(MemToReg), 
    .ALUCtrl(ALUCtrl), 
    .loadPC(loadPC), 
    .PC(PC), 
    .Zero(Zero), 
    .dAddress(dAddress), 
    .dWriteData(dWriteData), 
    .dReadData(dReadData), 
    .WriteBackData(WriteBackData)
); 
  
  parameter [2:0] IF = 3'b000;
  parameter [2:0] ID = 3'b001;
  parameter [2:0] EX = 3'b010;
  parameter [2:0] MEM = 3'b011;
  parameter [2:0] WB = 3'b100;
    
  wire [6:0] opcode = instr[6:0];
  parameter [6:0] IMMEDIATE = 7'b0010011;
  parameter [6:0] NON_IMMEDIATE = 7'b0110011;
  parameter [6:0] LW = 7'b0000011;
  parameter [6:0] SW = 7'b0100011;
  parameter [6:0] BEQ = 7'b1100011;

  wire [2:0] funct3 = instr[14:12];
  parameter [2:0] ARITHMETIC = 3'b000;
  parameter [2:0] SLT = 3'b010;
  parameter [2:0] XOR = 3'b100;
  parameter [2:0] OR = 3'b110;
  parameter [2:0] AND = 3'b111;
  parameter [2:0] SLL = 3'b001;
  parameter [2:0] SRL = 3'b101;
  parameter [2:0] SRA = 3'b101;
  
  parameter [3:0] ALUOP_AND = 4'b0000;
  parameter [3:0] ALUOP_OR  = 4'b0001;
  parameter [3:0] ALUOP_ADD = 4'b0010;
  parameter [3:0] ALUOP_SUB = 4'b0110;
  parameter [3:0] ALUOP_LT  = 4'b0100;  
  parameter [3:0] ALUOP_SRL = 4'b1000;  
  parameter [3:0] ALUOP_SLL = 4'b1001; 
  parameter [3:0] ALUOP_SRA = 4'b1010;  
  parameter [3:0] ALUOP_XOR = 4'b0101;  

  always @(posedge clk or posedge rst) begin
    if (rst) begin
        FSM <= IF;
        MemRead <= 0;
        MemWrite <= 0;
        RegWrite <= 0;
        MemToReg <= 0;
        loadPC <= 0;
        ALUSrc <= 0;
        PCSrc <= 0;
    end else begin
        case (FSM)
            IF: FSM <= ID;
            ID: FSM <= EX;
            EX: FSM <= MEM;
            MEM: FSM <= WB;
            WB: FSM <= IF;
        endcase
    end
  end

  always @(posedge clk) begin
      case (FSM)
          IF: begin
            {IFStage, IDStage, MEMStage, EXStage, WBStage} = 5'b10000;
            loadPC <= 0;
            // Reset the control signals
            MemRead <= 0;
            MemWrite <= 0;
            RegWrite <= 0;
            MemToReg <= 0;
          end
          ID: {IFStage, IDStage, MEMStage, EXStage, WBStage} = 5'b01000;
          MEM: begin
            {IFStage, IDStage, MEMStage, EXStage, WBStage} = 5'b00100;
            if (opcode == LW)
                MemRead <= 1;
            else if (opcode == SW)
                MemWrite <= 1;
          end
          EX: {IFStage, IDStage, MEMStage, EXStage, WBStage} = 5'b00010;
          WB: begin 
              {IFStage, IDStage, MEMStage, EXStage, WBStage} = 5'b00001;
              loadPC <= 1;
              if (opcode == LW)
                  MemToReg <= 1;

              if (opcode != SW && opcode != BEQ)
                  RegWrite <= 1;
          end
      endcase
  end
        
  always @(*) begin
    if (opcode == NON_IMMEDIATE) begin
      case (funct3)
        ARITHMETIC: begin
          if (instr[30] == 0)
            ALUCtrl = ALUOP_ADD;
          else
            ALUCtrl = ALUOP_SUB;
        end
        SLT: ALUCtrl = ALUOP_LT;
        XOR: ALUCtrl = ALUOP_XOR;
        OR: ALUCtrl = ALUOP_OR;
        SLL: ALUCtrl = ALUOP_SLL;
        AND: ALUCtrl = ALUOP_AND;
        SRL: begin
          if (instr[30] == 0)
            ALUCtrl = ALUOP_SRL;
          else if (instr[30] == 1)
            ALUCtrl = ALUOP_SRA;
        end
      endcase
    end
    else if (opcode == IMMEDIATE) begin
      case (funct3)
            ARITHMETIC : ALUCtrl = ALUOP_ADD;
            SLT : ALUCtrl = ALUOP_LT;
            XOR : ALUCtrl = ALUOP_XOR;
            OR : ALUCtrl = ALUOP_OR;
            AND : ALUCtrl = ALUOP_AND;
            SLL : ALUCtrl = ALUOP_SLL;
            SRL : begin
                if ( instr[30] == 0 )
                    ALUCtrl = ALUOP_SRL;
                else if ( instr[30] == 1 )
                    ALUCtrl = ALUOP_SRA;
            end
        endcase
    end
    else if ( opcode == LW || opcode == SW )
        ALUCtrl = ALUOP_ADD;
    else if ( opcode == BEQ )
        ALUCtrl = ALUOP_SUB;
end

  always @(*) begin
    // ALUSrc  
    if (opcode == LW || opcode == SW || opcode == IMMEDIATE) begin
      ALUSrc <= 1;
    end
    else begin
      ALUSrc <= 0;
    end
    
    // PCSrc
    if (opcode == BEQ) begin
        PCSrc <= Zero ? 1 : 0;
    end
    else begin
        PCSrc <= 0;
    end
  end

endmodule