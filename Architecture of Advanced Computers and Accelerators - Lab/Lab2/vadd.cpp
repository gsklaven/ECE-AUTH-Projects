#define dimen 16
#define LM 4
#define LN 4
#define LP 4

#define M (1 << LM)
#define N (1 << LN)
#define P (1 << LP)

extern "C" {
void vadd(int in1[dimen*dimen], // Read-Only Vector 1
          int in2[dimen*dimen], // Read-Only Vector 2
          int out_r[dimen*dimen],     // Output Result
          int size                 // Size in integer
          ) {

#pragma HLS INTERFACE m_axi port = in1 offset = slave bundle = gmem
#pragma HLS INTERFACE m_axi port = in2 offset = slave bundle = gmem
#pragma HLS INTERFACE m_axi port = out_r offset = slave bundle = gmem
#pragma HLS INTERFACE s_axilite port = in1 bundle = control
#pragma HLS INTERFACE s_axilite port = in2 bundle = control
#pragma HLS INTERFACE s_axilite port = out_r bundle = control
#pragma HLS INTERFACE s_axilite port = size bundle = control
#pragma HLS INTERFACE s_axilite port = return bundle = control

   int local_in1[dimen][dimen];
   int local_in2[dimen][dimen];
   int local_out[dimen][dimen];

  read1:
  for (int i = 0; i < dimen; i++) {
    for (int j = 0; j < dimen; j++) {
      local_in1[i][j] = in1[i * dimen + j];
    }
  }

  read2:
  for (int i = 0; i < dimen; i++) {
    for (int j = 0; j < dimen; j++) {
      local_in2[i][j] = in2[i * dimen + j];
    }
  }

  vadd:
      #pragma HLS ARRAY_PARTITION variable=local_in1 type=cyclic factor=dimen
      #pragma HLS ARRAY_PARTITION variable=local_in2 type=cyclic factor=dimen
      #pragma HLS ARRAY_PARTITION variable=local_out type=cyclic factor=dimen
          for (int i = 0; i < dimen; i++) {
                #pragma HLS PIPELINE II=1
              for (int j = 0; j < dimen; j++) {
                #pragma HLS PIPELINE II=1
                  local_out[i][j] = 0;
                  for (int k = 0; k < dimen; k++) {
                    #pragma HLS UNROLL
                      local_out[i][j]  += local_in1[i][k] * local_in2[k][j];
                  }
              }
          }

  write:
  for (int i = 0; i < dimen; i++) {
    for (int j = 0; j < dimen; j++) {
      out_r[i * dimen + j] = local_out[i][j];
    }
  }
}
}
