#include "ggml.h"
#include "ggml/ggml-alloc.h"
#include "ggml/ggml-backend.h"

// #define GGML_USE_CUBLAS

#ifdef GGML_USE_CUBLAS
#include "ggml-cuda.h"
#endif

#ifdef GGML_USE_METAL
#include "ggml-metal.h"
#endif

#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <map>
#include <string>
#include <vector>

static void ggml_log_callback_default(ggml_log_level level, const char * text, void * user_data) {
    (void) level;
    (void) user_data;
    fputs(text, stderr);
    fflush(stderr);
}

struct test_model {
    struct ggml_tensor * a;
    struct ggml_tensor * b;
    ggml_backend_t backend = NULL;
    ggml_backend_buffer_t buffer;
    struct ggml_context * ctx;
};

void load_model(test_model & model, bool use_gpu = false) {
    // create data
    int K = 3, IC = 10, OC = 10;
    int IL = 8, N = 1;

    // Initialize adata
    float * adata = new float[K * IC * OC];
    for (int i = 0; i < K * IC * OC; i++) {
        adata[i] = 4.5f;
    }

    // Convert adata to fp16 format
    std::vector<ggml_fp16_t> hadata(K * IC * OC);
    ggml_fp32_to_fp16_row(adata, hadata.data(), K * IC * OC);

    // Initialize bdata
    float * bdata =  new float[IL * IC * N];
    for (int i = 0; i < IL * IC * N; i++) {
        bdata[i] = 2.5f;
    }

    size_t buffer_size = 0;
    {
        buffer_size += K * IC * OC * ggml_type_size(GGML_TYPE_F16); // tensor a
        buffer_size += IL * IC * N * ggml_type_size(GGML_TYPE_F32); // tensor b
        buffer_size += 1024; // overhead
    }

    printf("%s: ggml tensor size    = %d bytes\n", __func__, (int) sizeof(ggml_tensor));
    printf("%s: backend buffer size = %0.2f MB\n", __func__, (buffer_size/ 1024.f/ 1024.f));

    int num_tensors = 2;
    struct ggml_init_params params {
            /*.mem_size   =*/ ggml_tensor_overhead() * num_tensors,
            /*.mem_buffer =*/ NULL,
            /*.no_alloc   =*/ true,
    };

    // initialize the backend
#ifdef GGML_USE_CUBLAS
    if (use_gpu) {
        fprintf(stderr, "%s: using CUDA backend\n", __func__);
        model.backend = ggml_backend_cuda_init(0);
        if (!model.backend) {
            fprintf(stderr, "%s: ggml_backend_cuda_init() failed\n", __func__);
        }
    }
#endif

#ifdef GGML_USE_METAL
    if (use_gpu) {
        fprintf(stderr, "%s: using Metal backend\n", __func__);
        ggml_backend_metal_log_set_callback(ggml_log_callback_default, nullptr);
        model.backend = ggml_backend_metal_init();
        if (!model.backend) {
            fprintf(stderr, "%s: ggml_backend_metal_init() failed\n", __func__);
        }
    }
#endif

    if(!model.backend) {
        // fallback to CPU backend
        model.backend = ggml_backend_cpu_init();
    }

    model.buffer = ggml_backend_alloc_buffer(model.backend, buffer_size);

    // create context
    model.ctx = ggml_init(params);

    // create tensors
    model.a = ggml_new_tensor_3d(model.ctx, GGML_TYPE_F16,  K, IC, OC);
    model.b = ggml_new_tensor_3d(model.ctx, GGML_TYPE_F32, IL, IC, N);

    // create a allocator
    ggml_tallocr alloc = ggml_tallocr_new(model.buffer);

    // alloc memory
    ggml_tallocr_alloc(&alloc, model.a);

    // load data to buffer
    if(ggml_backend_is_cpu(model.backend)) {
        memcpy(model.a->data, hadata.data(), ggml_nbytes(model.a));
    } else {
        ggml_backend_tensor_set(model.a, hadata.data(), 0, ggml_nbytes(model.a));
    }

    // alloc memory
    ggml_tallocr_alloc(&alloc, model.b);

    if(ggml_backend_is_cpu(model.backend)
#ifdef GGML_USE_METAL
                || ggml_backend_is_metal(model.backend)
#endif
    ) {
        memcpy(model.b->data, bdata, ggml_nbytes(model.b));
    } else {
        ggml_backend_tensor_set(model.b, bdata, 0, ggml_nbytes(model.b));
    }
}

struct ggml_cgraph * build_graph(const test_model& model) {
    static size_t buf_size = ggml_tensor_overhead()*GGML_DEFAULT_GRAPH_SIZE + ggml_graph_overhead();
    static std::vector<uint8_t> buf(buf_size);

    struct ggml_init_params params0 = {
        /*.mem_size   =*/ buf_size,
        /*.mem_buffer =*/ buf.data(),
        /*.no_alloc   =*/ true, // the tensors will be allocated later by ggml_gallocr_alloc_graph()
    };

    // create a temporally context to build the graph
    struct ggml_context * ctx0 = ggml_init(params0);

    struct ggml_cgraph  * gf = ggml_new_graph(ctx0);

    int s0 = 1;
    int p0 = 1;
    int d0 = 1;

    // split conv1d in fundamental methods for test unit
    struct ggml_tensor* im2col_0 = ggml_im2col(ctx0, model.a, model.b, s0, 0, p0, 0, d0, 0, false, GGML_TYPE_F16);
    ggml_set_name(im2col_0, "im2col_res_0");
    ggml_build_forward_expand(gf, im2col_0);

    struct ggml_tensor* conv1d_res_0 = ggml_conv_1d(ctx0, model.a, model.b, s0, p0, d0);
    ggml_set_name(conv1d_res_0, "conv1d_res_0");
    ggml_build_forward_expand(gf, conv1d_res_0);





    ggml_tensor *float_32_weights = ggml_cpy(
        ctx0, model.a,
        ggml_new_tensor(ctx0, GGML_TYPE_F32, 4,
                        model.a->ne));

    // split conv1d in fundamental methods for test unit
    struct ggml_tensor* im2col_1 = ggml_im2col(ctx0, float_32_weights, model.b, s0, 0, p0, 0, d0, 0, false, GGML_TYPE_F16);
    ggml_set_name(im2col_1, "im2col_res_1");
    ggml_build_forward_expand(gf, im2col_1);


    struct ggml_tensor* conv1d_res_1 = ggml_conv_1d(ctx0, float_32_weights, model.b, s0, p0, d0);
    ggml_set_name(conv1d_res_1, "conv1d_res_1");
    ggml_build_forward_expand(gf, conv1d_res_1);

    

    // delete the temporally context used to build the graph
    ggml_free(ctx0);
    return gf;
}

struct ggml_cgraph* compute_graph(const test_model & model, ggml_gallocr_t allocr) {
    struct ggml_cgraph * gf = build_graph(model);

    // allocate tensors
    ggml_gallocr_alloc_graph(allocr, gf);
    int n_threads = 1;

    if (ggml_backend_is_cpu(model.backend)) {
        ggml_backend_cpu_set_n_threads(model.backend, n_threads);
    }

#ifdef GGML_USE_METAL
    if (ggml_backend_is_metal(model.backend)) {
        ggml_backend_metal_set_n_cb(model.backend, n_threads);
    }
#endif

    ggml_backend_graph_compute(model.backend, gf);

    //ggml_graph_print(gf);

    return gf;
}

int main(void)
{
    ggml_time_init();

    test_model model;
    load_model(model, true);

    ggml_gallocr_t allocr = NULL;

    {
        allocr = ggml_gallocr_new(ggml_backend_get_default_buffer_type(model.backend));

        //create the worst case graph for memory usage estimation
        struct ggml_cgraph * gf = build_graph(model);

        // compute the required memory
        ggml_gallocr_reserve(allocr, gf);
        size_t mem_size = ggml_gallocr_get_buffer_size(allocr, 0);
        fprintf(stderr, "%s: compute buffer size: %.2f MB\n", __func__, mem_size/1024.0f/1024.0f);
    }

    struct ggml_cgraph * gf_res = compute_graph(model, allocr);

     struct ggml_tensor * im2col_res_0 = NULL;
    struct ggml_tensor * conv1d_res_0 = NULL;

    for(int i = 0; i < gf_res->n_nodes; i++) {
        if(strcmp(ggml_get_name(gf_res->nodes[i]), "im2col_res_0") == 0) {
            im2col_res_0 = gf_res->nodes[i];
        } else if(strcmp(ggml_get_name(gf_res->nodes[i]), "conv1d_res_0") == 0) {
            conv1d_res_0 = gf_res->nodes[i];
        }
    }

    uint16_t* im2col_data_0 = new uint16_t[ggml_nelements(im2col_res_0)];
    float* conv2d_data_0 = new float[ggml_nelements(conv1d_res_0)];

    ggml_backend_tensor_get(im2col_res_0, im2col_data_0, 0, ggml_nbytes(im2col_res_0));
    ggml_backend_tensor_get(conv1d_res_0, conv2d_data_0, 0, ggml_nbytes(conv1d_res_0));

    struct ggml_tensor * im2col_res_1 = NULL;
    struct ggml_tensor * conv1d_res_1 = NULL;

    for(int i = 0; i < gf_res->n_nodes; i++) {
        if(strcmp(ggml_get_name(gf_res->nodes[i]), "im2col_res_1") == 0) {
            im2col_res_1 = gf_res->nodes[i];
        } else if(strcmp(ggml_get_name(gf_res->nodes[i]), "conv1d_res_1") == 0) {
            conv1d_res_1 = gf_res->nodes[i];
        }
    }

    uint16_t* im2col_data_1 = new uint16_t[ggml_nelements(im2col_res_1)];
    float* conv2d_data_1 = new float[ggml_nelements(conv1d_res_1)];

    ggml_backend_tensor_get(im2col_res_1, im2col_data_1, 0, ggml_nbytes(im2col_res_1));
    ggml_backend_tensor_get(conv1d_res_1, conv2d_data_1, 0, ggml_nbytes(conv1d_res_1));

    const int n_conv1d_test = 80;
    const int n_im2col_test = 240;

    float expected_conv1d[n_conv1d_test] = {
        225.00f, 337.50f, 337.50f, 337.50f, 337.50f, 337.50f, 337.50f, 225.00f,
        225.00f, 337.50f, 337.50f, 337.50f, 337.50f, 337.50f, 337.50f, 225.00f,
        225.00f, 337.50f, 337.50f, 337.50f, 337.50f, 337.50f, 337.50f, 225.00f,
        225.00f, 337.50f, 337.50f, 337.50f, 337.50f, 337.50f, 337.50f, 225.00f,
        225.00f, 337.50f, 337.50f, 337.50f, 337.50f, 337.50f, 337.50f, 225.00f,
        225.00f, 337.50f, 337.50f, 337.50f, 337.50f, 337.50f, 337.50f, 225.00f,
        225.00f, 337.50f, 337.50f, 337.50f, 337.50f, 337.50f, 337.50f, 225.00f,
        225.00f, 337.50f, 337.50f, 337.50f, 337.50f, 337.50f, 337.50f, 225.00f,
        225.00f, 337.50f, 337.50f, 337.50f, 337.50f, 337.50f, 337.50f, 225.00f,
        225.00f, 337.50f, 337.50f, 337.50f, 337.50f, 337.50f, 337.50f, 225.00f
    };
    // first im2col test

    uint16_t expected_im2col[n_conv1d_test] = {
        0, 16640, 16640, 0, 16640, 16640, 0, 16640,
        16640, 0, 16640, 16640, 0, 16640, 16640, 0,
        16640, 16640, 0, 16640, 16640, 0, 16640, 16640,
        0, 16640, 16640, 0, 16640, 16640, 16640, 16640,
        16640, 16640, 16640, 16640, 16640, 16640, 16640, 16640,
        16640, 16640, 16640, 16640, 16640, 16640, 16640, 16640,
        16640, 16640, 16640, 16640, 16640, 16640, 16640, 16640,
        16640, 16640, 16640, 16640, 16640, 16640, 16640, 16640,
        16640, 16640, 16640, 16640, 16640, 16640, 16640, 16640,
        16640, 16640, 16640, 16640, 16640, 16640, 16640, 16640
    };

    printf("\nPerforming test:\n");

    bool passed = true;
    for(int i = 0; i < n_conv1d_test; i++) {
        if(
            im2col_data_0[i] != expected_im2col[i]) {
            passed = false;
            break;
        }
    }

    printf("ggml_im2col (%d): %s\n", (int) ggml_nelements(im2col_res_0), passed && (ggml_nelements(im2col_res_0) == n_im2col_test) ? "\033[32mPASSED\033[0m" : "\033[31mFAILED\033[0m");

    passed = true;
    for(int i = 0; i < n_conv1d_test; i++) {
        if(conv2d_data_0[i] != expected_conv1d[i]) {
            passed = false;
            break;
        }
    }

    printf("ggml_conv1d (%d): %s\n", (int) ggml_nelements(conv1d_res_0), passed && (ggml_nelements(conv1d_res_0) == n_conv1d_test) ? "\033[32mPASSED\033[0m" : "\033[31mFAILED\033[0m");

    passed = true;
    for(int i = 0; i < n_conv1d_test; i++) {
        if(
            im2col_data_1[i] != expected_im2col[i]) {
            passed = false;
            break;
        }
    }

    printf("ggml_im2col (%d): %s\n", (int) ggml_nelements(im2col_res_1), passed && (ggml_nelements(im2col_res_1) == n_im2col_test) ? "\033[32mPASSED\033[0m" : "\033[31mFAILED\033[0m");

    passed = true;
    for(int i = 0; i < n_conv1d_test; i++) {
        if(conv2d_data_1[i] != expected_conv1d[i]) {
            passed = false;
            break;
        }
    }

    printf("ggml_conv1d (%d): %s\n", (int) ggml_nelements(conv1d_res_1), passed && (ggml_nelements(conv1d_res_1) == n_conv1d_test) ? "\033[32mPASSED\033[0m" : "\033[31mFAILED\033[0m");


    ggml_free(model.ctx);

    ggml_backend_buffer_free(model.buffer);
    ggml_backend_free(model.backend);
    ggml_gallocr_free(allocr);
    return 0;
}
