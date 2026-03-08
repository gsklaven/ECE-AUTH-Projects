#include <ap_int.h>
#define dimen 16
typedef ap_uint<512> uint512_dt;

extern "C" {
void vadd(const uint512_dt *in1,
          const uint512_dt *in2,
          uint512_dt *out_r,
          int size) {

#pragma HLS INTERFACE m_axi port=in1 offset=slave bundle=gmem
#pragma HLS INTERFACE m_axi port=in2 offset=slave bundle=gmem
#pragma HLS INTERFACE m_axi port=out_r offset=slave bundle=gmem
#pragma HLS INTERFACE s_axilite port=in1 bundle=control
#pragma HLS INTERFACE s_axilite port=in2 bundle=control
#pragma HLS INTERFACE s_axilite port=out_r bundle=control
#pragma HLS INTERFACE s_axilite port=size bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control

    const int total_elements = dimen * dimen;
    const int total_vectors = total_elements / 16;

    int local_in1[total_elements];
    int local_in2[total_elements];
    int local_out[total_elements];

#pragma HLS ARRAY_PARTITION variable=local_in1 cyclic factor=16
#pragma HLS ARRAY_PARTITION variable=local_in2 cyclic factor=16
#pragma HLS ARRAY_PARTITION variable=local_out cyclic factor=16

    read_in1:
    for (int i = 0; i < total_vectors; i++) {
    #pragma HLS PIPELINE II=1
        uint512_dt temp = in1[i];
        for (int j = 0; j < 16; j++) {
        #pragma HLS UNROLL
            local_in1[i * 16 + j] = temp.range(32 * (j + 1) - 1, 32 * j);
        }
    }

    read_in2:
    for (int i = 0; i < total_vectors; i++) {
    #pragma HLS PIPELINE II=1
        uint512_dt temp = in2[i];
        for (int j = 0; j < 16; j++) {
        #pragma HLS UNROLL
            local_in2[i * 16 + j] = temp.range(32 * (j + 1) - 1, 32 * j);
        }
    }

    init_out:
    for (int i = 0; i < total_elements; i++) {
    #pragma HLS PIPELINE II=1
        local_out[i] = 0;
    }

    mult:
    for (int i = 0; i < dimen; i++) {
    #pragma HLS PIPELINE
        for (int j = 0; j < dimen; j++) {
        #pragma HLS UNROLL
            int sum = 0;
            for (int k = 0; k < dimen; k++) {
            #pragma HLS UNROLL
                int a = local_in1[i * dimen + k];
                int b = local_in2[k * dimen + j];
                sum += a * b;
            }
            local_out[i * dimen + j] = sum;
        }
    }

    write_out:
    for (int i = 0; i < total_vectors; i++) {
    #pragma HLS PIPELINE II=1
        uint512_dt temp = 0;
        for (int j = 0; j < 16; j++) {
        #pragma HLS UNROLL
            temp.range(32 * (j + 1) - 1, 32 * j) = local_out[i * 16 + j];
        }
        out_r[i] = temp;
    }
}
}
