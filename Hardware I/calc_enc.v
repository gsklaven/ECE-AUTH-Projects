module calc_enc(
    input wire btnl, btnc, btnr,
  	output wire [3:0] alu_op
);
  	
  	not U0(btnc_not, btnc);
  	not U1(btnl_not, btnl);
  	not U2(btnr_not, btnr);
  
    and A1 (m0, btnc_not, btnr); 
    and A2 (m1, btnl, btnr);
  	or O1 (alu_op[0], m0, m1);
  	
    and A3 (m2, btnl_not, btnc);
    and A4 (m3, btnc, btnr_not);
    or O2 (alu_op[1], m2, m3);
  
    and A5 (m4, btnc, btnr); 
    and A6 (m5, btnl, btnc_not);
  	and A7 (m6, m5, btnr_not);
  	or O3 (alu_op[2], m4, m6);
  
    and A8 (m7, btnl, btnc_not);
    and A9 (m8, m7, btnr);
  	and A10 (m9, btnl, btnc);
    and A11 (m10, m9, btnr_not);
    or O4 (alu_op[3], m10, m8);
  
endmodule

  