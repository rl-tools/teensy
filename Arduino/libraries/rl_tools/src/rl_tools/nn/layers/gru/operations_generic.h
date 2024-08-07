#include "../../../version.h"
#if (defined(RL_TOOLS_DISABLE_INCLUDE_GUARDS) || !defined(RL_TOOLS_NN_LAYERS_GRU_OPERATIONS_GENERIC_H)) && (RL_TOOLS_USE_THIS_VERSION == 1)
#pragma once
#define RL_TOOLS_NN_LAYERS_GRU_OPERATIONS_GENERIC_H

#include "layer.h"
#include "helper_operations_generic.h"
#include "../../parameters/operations_generic.h"
#include "../../mode.h"

RL_TOOLS_NAMESPACE_WRAPPER_START
namespace rl_tools{
    template <typename DEVICE, typename SPEC>
    void malloc(DEVICE& device, nn::layers::gru::LayerForward<SPEC>& layer){
        malloc(device, layer.weights_input);
        using VIEW_SPEC = tensor::ViewSpec<0, SPEC::HIDDEN_DIM>;
        layer.W_ir = view_range(device, layer.weights_input.parameters, 0*SPEC::HIDDEN_DIM, VIEW_SPEC{});
        layer.W_iz = view_range(device, layer.weights_input.parameters, 1*SPEC::HIDDEN_DIM, VIEW_SPEC{});
        layer.W_in = view_range(device, layer.weights_input.parameters, 2*SPEC::HIDDEN_DIM, VIEW_SPEC{});
        malloc(device, layer.biases_input);
        layer.b_ir = view_range(device, layer.biases_input.parameters, 0*SPEC::HIDDEN_DIM, VIEW_SPEC{});
        layer.b_iz = view_range(device, layer.biases_input.parameters, 1*SPEC::HIDDEN_DIM, VIEW_SPEC{});
        layer.b_in = view_range(device, layer.biases_input.parameters, 2*SPEC::HIDDEN_DIM, VIEW_SPEC{});
        malloc(device, layer.weights_hidden);
        using VIEW_SPEC_DOUBLE = tensor::ViewSpec<0, 2*SPEC::HIDDEN_DIM>;
        layer.W_hrz = view_range(device, layer.weights_hidden.parameters, 0*SPEC::HIDDEN_DIM, VIEW_SPEC_DOUBLE{});
        layer.W_hr = view_range(device, layer.weights_hidden.parameters, 0*SPEC::HIDDEN_DIM, VIEW_SPEC{});
        layer.W_hz = view_range(device, layer.weights_hidden.parameters, 1*SPEC::HIDDEN_DIM, VIEW_SPEC{});
        layer.W_hn = view_range(device, layer.weights_hidden.parameters, 2*SPEC::HIDDEN_DIM, VIEW_SPEC{});
        malloc(device, layer.biases_hidden);
        layer.b_hr = view_range(device, layer.biases_hidden.parameters, 0*SPEC::HIDDEN_DIM, VIEW_SPEC{});
        layer.b_hz = view_range(device, layer.biases_hidden.parameters, 1*SPEC::HIDDEN_DIM, VIEW_SPEC{});
        layer.b_hn = view_range(device, layer.biases_hidden.parameters, 2*SPEC::HIDDEN_DIM, VIEW_SPEC{});

        malloc(device, layer.initial_hidden_state);
        set_all(device, layer.initial_hidden_state.parameters, 0);
    }
    template <typename DEVICE, typename SPEC>
    void malloc(DEVICE& device, nn::layers::gru::LayerBackward<SPEC>& layer){
        malloc(device, static_cast<nn::layers::gru::LayerForward<SPEC>&>(layer));
        malloc(device, layer.n_pre_pre_activation);
        malloc(device, layer.post_activation);
        malloc(device, layer.output);
    }
    template <typename DEVICE, typename SPEC>
    void malloc(DEVICE& device, nn::layers::gru::buffers::Evaluation<SPEC>& buffers){
        malloc(device, buffers.post_activation);
        malloc(device, buffers.n_pre_pre_activation);
    }
    template <typename DEVICE, typename SPEC>
    void init(DEVICE& device, nn::layers::gru::buffers::Backward<SPEC>& buffers){
        using VIEW_SPEC_DOUBLE = tensor::ViewSpec<1, 2*SPEC::HIDDEN_DIM>;
        buffers.buffer_rz = view_range(device, buffers.buffer, 0*SPEC::HIDDEN_DIM, VIEW_SPEC_DOUBLE{});
        using VIEW_SPEC = tensor::ViewSpec<1, SPEC::HIDDEN_DIM>;
        buffers.buffer_r = view_range(device, buffers.buffer, 0*SPEC::HIDDEN_DIM, VIEW_SPEC{});
        buffers.buffer_z = view_range(device, buffers.buffer, 1*SPEC::HIDDEN_DIM, VIEW_SPEC{});
        buffers.buffer_n = view_range(device, buffers.buffer, 2*SPEC::HIDDEN_DIM, VIEW_SPEC{});
    }
    template <typename DEVICE, typename SPEC>
    void malloc(DEVICE& device, nn::layers::gru::buffers::Backward<SPEC>& buffers){
        malloc(device, static_cast<nn::layers::gru::buffers::Evaluation<SPEC>&>(buffers));
        malloc(device, buffers.buffer);
        init(device, buffers);
    }
    template <typename DEVICE, typename SPEC>
    void free(DEVICE& device, nn::layers::gru::buffers::Backward<SPEC>& buffer){
        free(device, static_cast<nn::layers::gru::buffers::Evaluation<SPEC>&>(buffer));
        free(device, buffer.buffer);
    }
    template <typename DEVICE, typename SPEC, typename RNG>
    void init_weights(DEVICE& device, nn::layers::gru::LayerForward<SPEC>& l, RNG& rng){
        // same as in PyTorch
        using T = typename SPEC::T;
        using TI = typename DEVICE::index_t;
        T scale = 1.0 / math::sqrt(device.math, (T)SPEC::HIDDEN_DIM);
        rand(device, l.weights_hidden.parameters, rng, -scale, scale);
        rand(device, l.weights_input.parameters, rng, -scale, scale);
        rand(device, l.biases_hidden.parameters, rng, -scale, scale);
        rand(device, l.biases_input.parameters, rng, -scale, scale);
    }
    template<typename DEVICE, typename SPEC_1, typename SPEC_2, typename SPEC_OUTPUT>
    void multiply_broadcast_accumulate(DEVICE& device, const Tensor<SPEC_1>& t1, const Tensor<SPEC_2>& t2, Tensor<SPEC_OUTPUT>& t_output){
#ifdef RL_TOOLS_ENABLE_TRACY
        ZoneScopedN("gru::multiply_broadcast_accumulate");
#endif
        static_assert(length(typename SPEC_1::SHAPE{}) == 2);
        static_assert(length(typename SPEC_2::SHAPE{}) == 1);
        static_assert(get<0>(typename SPEC_1::SHAPE{}) == get<0>(typename SPEC_OUTPUT::SHAPE{}));
        static_assert(get<1>(typename SPEC_1::SHAPE{}) == get<0>(typename SPEC_2::SHAPE{}));
        static_assert(get<1>(typename SPEC_OUTPUT::SHAPE{}) == get<1>(typename SPEC_1::SHAPE{}));
        using TI = typename DEVICE::index_t;
        using T = typename SPEC_1::T;
        for(TI i=0; i < get<0>(typename SPEC_1::SHAPE{}); i++){
            for(TI j=0; j < get<1>(typename SPEC_1::SHAPE{}); j++){
                T t1_value = get(device, t1, i, j);
                T t2_value = get(device, t2, j);
                T t_output_value = get(device, t_output, i, j);
                set(device, t_output, t1_value * t2_value + t_output_value, i, j);
            }
        }
    }

    namespace nn::layers::gru{
        namespace multi_step_intermediates_tag_dispatch{ // this is required for MSVC because it complains about the slicker SFINAE-based version
            struct three_dimensional_tag{};
            struct two_dimensional_tag{};

            template <typename SPEC>
            struct dimension_tag{
                using type = typename utils::typing::conditional_t<length(typename SPEC::SHAPE{})==3, three_dimensional_tag, two_dimensional_tag>;
            };
            template <typename DEVICE, typename SPEC>
            auto impl(DEVICE& device, Tensor<SPEC>& tensor, typename DEVICE::index_t step_i, three_dimensional_tag){
                return view(device, tensor, step_i);
            }
            template <typename DEVICE, typename SPEC>
            auto impl(DEVICE& device, Tensor<SPEC>& tensor, typename DEVICE::index_t step_i, two_dimensional_tag){
                return tensor;
            }
        }

        template <typename DEVICE, typename SPEC>
        auto multi_step_intermediates(DEVICE& device, Tensor<SPEC>& tensor, typename DEVICE::index_t step_i) {
            return multi_step_intermediates_tag_dispatch::impl(device, tensor, step_i, typename multi_step_intermediates_tag_dispatch::dimension_tag<SPEC>::type{});
        }

//        template <typename DEVICE, typename SPEC, typename utils::typing::enable_if<length(typename SPEC::SHAPE{}) == 3, int>::type = 0>
//        auto multi_step_intermediates(DEVICE& device, Tensor<SPEC>& tensor, typename DEVICE::index_t step_i){
//            return view(device, tensor, step_i);
//        }
//        template <typename DEVICE, typename SPEC, typename utils::typing::enable_if<length(typename SPEC::SHAPE{}) == 2, int>::type = 0>
//        auto multi_step_intermediates(DEVICE& device, Tensor<SPEC>& tensor, typename DEVICE::index_t step_i){
//            return tensor;
//        }
    }

    template<typename DEVICE, typename LAYER_SPEC, typename INPUT_SPEC, typename OUTPUT_SPEC, typename POST_ACTIVATION_SPEC, typename N_PRE_PRE_ACTIVATION_SPEC, typename RNG, typename MODE = nn::mode::Default>
    void evaluate(DEVICE& device, const nn::layers::gru::LayerForward<LAYER_SPEC>& layer, const Tensor<INPUT_SPEC>& input, Tensor<POST_ACTIVATION_SPEC>& post_activation, Tensor<N_PRE_PRE_ACTIVATION_SPEC>& n_pre_pre_activation, Tensor<OUTPUT_SPEC>& output, RNG& rng, const nn::Mode<MODE>& mode = nn::Mode<nn::mode::Default>{}){
        using T = typename LAYER_SPEC::T;
        using TI = typename DEVICE::index_t;
        constexpr TI SEQUENCE_LENGTH = get<0>(typename INPUT_SPEC::SHAPE{});
        constexpr TI BATCH_SIZE = get<1>(typename INPUT_SPEC::SHAPE{});
        // you can either pass post_activation and n_pre_pre_activation as intermediate tensors with steps (e.g. for "forward" to maintain the state for the backward pass) or in a purely iterative fashion
        static_assert(nn::layers::gru::check_input_output<LAYER_SPEC, INPUT_SPEC, OUTPUT_SPEC>, "Input and output spec not matching");
        static_assert(length(typename POST_ACTIVATION_SPEC::SHAPE{}) == 3 || length(typename POST_ACTIVATION_SPEC::SHAPE{}) == 2);
        static_assert(length(typename N_PRE_PRE_ACTIVATION_SPEC::SHAPE{}) == 3 || length(typename N_PRE_PRE_ACTIVATION_SPEC::SHAPE{}) == 2);
        constexpr bool MULTI_STEP_INTERMEDIATES = length(typename POST_ACTIVATION_SPEC::SHAPE{}) == 3;
        static_assert(MULTI_STEP_INTERMEDIATES == (length(typename N_PRE_PRE_ACTIVATION_SPEC::SHAPE{}) == 3), "N_PRE_PRE_ACTIVATION_SPEC must have the same number of dimensions as POST_ACTIVATION_SPEC");
        static_assert(get<length(typename POST_ACTIVATION_SPEC::SHAPE{})-1>(typename POST_ACTIVATION_SPEC::SHAPE{}) == 3*LAYER_SPEC::HIDDEN_DIM);
        static_assert(get<length(typename POST_ACTIVATION_SPEC::SHAPE{})-2>(typename POST_ACTIVATION_SPEC::SHAPE{}) == BATCH_SIZE);
        static_assert(get<length(typename N_PRE_PRE_ACTIVATION_SPEC::SHAPE{})-1>(typename N_PRE_PRE_ACTIVATION_SPEC::SHAPE{}) == LAYER_SPEC::HIDDEN_DIM);
        static_assert(get<length(typename N_PRE_PRE_ACTIVATION_SPEC::SHAPE{})-2>(typename N_PRE_PRE_ACTIVATION_SPEC::SHAPE{}) == BATCH_SIZE);


        for(TI step_i=0; step_i < SEQUENCE_LENGTH; ++step_i){
#ifdef RL_TOOLS_ENABLE_TRACY
            ZoneScopedN("gru::evaluate_step");
#endif
            auto input_step = view(device, input, step_i);
            auto post_activation_step = nn::layers::gru::multi_step_intermediates(device, post_activation, step_i);
            auto n_pre_pre_activation_step = nn::layers::gru::multi_step_intermediates(device, n_pre_pre_activation, step_i);
            auto output_step = view(device, output, step_i);

            auto rz_post_activation = view_range(device, post_activation_step, 0*LAYER_SPEC::HIDDEN_DIM, tensor::ViewSpec<1, 2*LAYER_SPEC::HIDDEN_DIM>{});
            auto r_post_activation  = view_range(device, post_activation_step, 0*LAYER_SPEC::HIDDEN_DIM, tensor::ViewSpec<1, LAYER_SPEC::HIDDEN_DIM>{});
            auto z_post_activation  = view_range(device, post_activation_step, 1*LAYER_SPEC::HIDDEN_DIM, tensor::ViewSpec<1, LAYER_SPEC::HIDDEN_DIM>{});
            auto n_post_activation  = view_range(device, post_activation_step, 2*LAYER_SPEC::HIDDEN_DIM, tensor::ViewSpec<1, LAYER_SPEC::HIDDEN_DIM>{});

            if(step_i == 0){
                nn::layers::gru::helper::matrix_multiply_broadcast_transpose_bias(device, layer.weights_hidden.parameters, layer.initial_hidden_state.parameters, layer.biases_hidden.parameters, post_activation_step);
            }
            else{
                auto output_previous_step = view(device, output, step_i-1);
                nn::layers::gru::helper::matrix_multiply_transpose_bias(device, layer.weights_hidden.parameters, output_previous_step, layer.biases_hidden.parameters, post_activation_step);
            }

            copy(device, device, n_post_activation, n_pre_pre_activation_step);
            set_all(device, n_post_activation, 0);

            nn::layers::gru::helper::matrix_multiply_transpose_bias_accumulate(device, layer.weights_input.parameters, input_step, layer.biases_input.parameters, post_activation_step);
            if(LAYER_SPEC::FAST_TANH){
                fast_sigmoid(device, rz_post_activation);
            }
            else{
                sigmoid(device, rz_post_activation);
            }
            multiply_accumulate(device, n_pre_pre_activation_step, r_post_activation, n_post_activation);
            if constexpr(LAYER_SPEC::FAST_TANH){
                fast_tanh(device, n_post_activation);
            }
            else{
                tanh(device, n_post_activation);
            }
            one_minus(device, z_post_activation, output_step);
            multiply(device, n_post_activation, output_step);
            if(step_i == 0){
                multiply_broadcast_accumulate(device, z_post_activation, layer.initial_hidden_state.parameters, output_step);
            }
            else{
                auto output_previous_step = view(device, output, step_i-1);
                multiply_accumulate(device, z_post_activation, output_previous_step, output_step);
            }
        }
    }
    template<typename DEVICE, typename LAYER_SPEC, typename INPUT_SPEC, typename OUTPUT_SPEC, typename RNG, typename MODE = nn::mode::Default>
    void evaluate(DEVICE& device, const nn::layers::gru::LayerForward<LAYER_SPEC>& layer, const Tensor<INPUT_SPEC>& input, Tensor<OUTPUT_SPEC>& output, nn::layers::gru::buffers::Evaluation<LAYER_SPEC>& buffers, RNG& rng, const nn::Mode<MODE>& mode = nn::Mode<nn::mode::Default>{}){
        evaluate(device, layer, input, buffers.post_activation, buffers.n_pre_pre_activation, output, rng, mode);
    }

    template<typename DEVICE, typename LAYER_SPEC, typename INPUT_SPEC, typename RNG, typename MODE = nn::mode::Default>
    void forward(DEVICE& device, nn::layers::gru::LayerBackward<LAYER_SPEC>& layer, const Tensor<INPUT_SPEC>& input, nn::layers::gru::buffers::Evaluation<LAYER_SPEC>& buffers, RNG& rng, const nn::Mode<MODE>& mode = nn::Mode<nn::mode::Default>{}){
        evaluate(device, layer, input, layer.post_activation, layer.n_pre_pre_activation, layer.output, rng, mode);
    }
    template<typename DEVICE, typename SPEC_FACTOR, typename SPEC_1, typename SPEC_2, typename SPEC_OUTPUT>
    void multiply_subtract_broadcast(DEVICE& device, Tensor<SPEC_FACTOR>& factor, Tensor<SPEC_1>& t1, Tensor<SPEC_2>& t2, Tensor<SPEC_OUTPUT>& t_output) {
#ifdef RL_TOOLS_ENABLE_TRACY
        ZoneScopedN("gru::multiply_subtract_broadcast");
#endif
        // broadcast t1 along first dimension
        static_assert(length(typename SPEC_FACTOR::SHAPE{}) == 2);
        static_assert(get<0>(typename SPEC_FACTOR::SHAPE{}) == get<0>(typename SPEC_2::SHAPE{}));
        static_assert(get<1>(typename SPEC_FACTOR::SHAPE{}) == get<1>(typename SPEC_2::SHAPE{}));
        static_assert(length(typename SPEC_1::SHAPE{}) == 1);
        static_assert(length(typename SPEC_2::SHAPE{}) == 2);
        static_assert(get<0>(typename SPEC_2::SHAPE{}) == get<0>(typename SPEC_OUTPUT::SHAPE{}));
        static_assert(get<0>(typename SPEC_1::SHAPE{}) == get<1>(typename SPEC_2::SHAPE{}));
        static_assert(get<1>(typename SPEC_OUTPUT::SHAPE{}) == get<1>(typename SPEC_2::SHAPE{}));
        using TI = typename DEVICE::index_t;
        using T = typename SPEC_1::T;
        for(TI i=0; i < get<0>(typename SPEC_2::SHAPE{}); i++){
            for(TI j=0; j < get<1>(typename SPEC_2::SHAPE{}); j++){
                T factor_value = get(device, factor, i, j);
                T t1_value = get(device, t1, j);
                T t2_value = get(device, t2, i, j);
                set(device, t_output, factor_value*(t1_value - t2_value), i, j);
            }
        }
    }
    namespace tensor::ternary_operations{
        template <typename T>
        T multiply_subtract(T factor, T a, T b){
            return factor * (a-b);
        }
    }
    template<typename DEVICE, typename SPEC_FACTOR, typename SPEC_1, typename SPEC_2, typename SPEC_OUT>
    void multiply_subtract(DEVICE& device, Tensor<SPEC_FACTOR>& factor, Tensor<SPEC_1>& t1, Tensor<SPEC_2>& t2, Tensor<SPEC_OUT>& result){
#ifdef RL_TOOLS_ENABLE_TRACY
        ZoneScopedN("gru::multiply_subtract");
#endif
        ternary_operation(device, tensor::Operation<tensor::ternary_operations::multiply_subtract<typename SPEC_1::T>, tensor::OperationEmptyParameter>{}, factor, t1, t2, result);
    }
    template<typename DEVICE, typename SPEC_1, typename SPEC_2, typename SPEC_OUT>
    void matrix_multiply_broadcast_accumulate(DEVICE& device, Tensor<SPEC_1>& t1, Tensor<SPEC_2>& t2, Tensor<SPEC_OUT>& result){
#ifdef RL_TOOLS_ENABLE_TRACY
        ZoneScopedN("gru::matrix_multiply_broadcast_accumulate");
#endif
        static_assert(length(typename SPEC_1::SHAPE{}) == 2);
        static_assert(length(typename SPEC_2::SHAPE{}) == 1);
        static_assert(length(typename SPEC_OUT::SHAPE{}) == 2);
        static_assert(get<0>(typename SPEC_1::SHAPE{}) == get<0>(typename SPEC_OUT::SHAPE{}));
        static_assert(get<0>(typename SPEC_2::SHAPE{}) == get<1>(typename SPEC_OUT::SHAPE{}));
        using T = typename SPEC_1::T;
        using TI = typename DEVICE::index_t;
        for(TI row_i=0; row_i < get<0>(typename SPEC_1::SHAPE{}); ++row_i){
            for(TI col_j=0; col_j < get<0>(typename SPEC_2::SHAPE{}); ++col_j){
                T acc = get(device, result, row_i, col_j);
                T t2_value = get(device, t2, col_j);
                for(TI k=0; k < get<1>(typename SPEC_1::SHAPE{}); ++k){
                    acc += get(device, t1, row_i, k) * t2_value;
                }
                set(device, result, acc, row_i, col_j);
            }
        }
    }
    template<typename DEVICE, typename SPEC_1, typename SPEC_2, typename SPEC_OUT>
    void matrix_multiply_accumulate_reduce(DEVICE& device, const Tensor<SPEC_1>& t1, const Tensor<SPEC_2>& t2, Tensor<SPEC_OUT>& result){
#ifdef RL_TOOLS_ENABLE_TRACY
        ZoneScopedN("gru::matrix_multiply_accumulate_reduce");
#endif
        static_assert(length(typename SPEC_1::SHAPE{}) == 2);
        static_assert(length(typename SPEC_2::SHAPE{}) == 2);
        static_assert(length(typename SPEC_OUT::SHAPE{}) == 1);
        static_assert(get<1>(typename SPEC_1::SHAPE{}) == get<0>(typename SPEC_2::SHAPE{}));
        static_assert(get<1>(typename SPEC_2::SHAPE{}) == get<0>(typename SPEC_OUT::SHAPE{}));
        using T = typename SPEC_1::T;
        using TI = typename DEVICE::index_t;
        for(TI row_i=0; row_i < get<0>(typename SPEC_1::SHAPE{}); ++row_i){
            for(TI col_j=0; col_j < get<1>(typename SPEC_2::SHAPE{}); ++col_j){
                T acc = get(device, result, col_j);
                for(TI k=0; k < get<1>(typename SPEC_1::SHAPE{}); ++k){
                    acc += get(device, t1, row_i, k) * get(device, t2, k, col_j);
                }
                set(device, result, acc, col_j);
            }
        }
    }
    template<typename DEVICE, typename SPEC_1, typename SPEC_2, typename SPEC_OUTPUT>
    void multiply_accumulate_reduce(DEVICE& device, Tensor<SPEC_1>& t1, Tensor<SPEC_2>& t2, Tensor<SPEC_OUTPUT>& t_output){
#ifdef RL_TOOLS_ENABLE_TRACY
        ZoneScopedN("gru::multiply_accumulate_reduce");
#endif
        static_assert(length(typename SPEC_1::SHAPE{}) == 2);
        static_assert(length(typename SPEC_2::SHAPE{}) == 2);
        static_assert(length(typename SPEC_OUTPUT::SHAPE{}) == 1);
        static_assert(get<0>(typename SPEC_1::SHAPE{}) == get<0>(typename SPEC_2::SHAPE{}));
        static_assert(get<1>(typename SPEC_1::SHAPE{}) == get<1>(typename SPEC_2::SHAPE{}));
        static_assert(get<1>(typename SPEC_2::SHAPE{}) == get<0>(typename SPEC_OUTPUT::SHAPE{}));
        using T = typename SPEC_1::T;
        using TI = typename DEVICE::index_t;
        for(TI row_i=0; row_i < get<0>(typename SPEC_1::SHAPE{}); ++row_i){
            for(TI col_j=0; col_j < get<1>(typename SPEC_1::SHAPE{}); ++col_j){
                T increment = get(device, t1, row_i, col_j) * get(device, t2, row_i, col_j);
                set(device, t_output, get(device, t_output, col_j) + increment, col_j);
            }
        }
    }
    template<typename DEVICE, typename SPEC>
    void zero_gradient(DEVICE& device, nn::layers::gru::LayerGradient<SPEC>& layer) {
        zero_gradient(device, layer.weights_input);
        zero_gradient(device, layer.biases_input);
        zero_gradient(device, layer.weights_hidden);
        zero_gradient(device, layer.biases_hidden);
        zero_gradient(device, layer.initial_hidden_state);
    }

    template<typename DEVICE, typename SPEC, typename OPTIMIZER>
    void update(DEVICE& device, nn::layers::gru::LayerGradient<SPEC>& layer, OPTIMIZER& optimizer){
        update(device, layer.weights_input, optimizer);
        update(device, layer.biases_input, optimizer);
        update(device, layer.weights_hidden, optimizer);
        update(device, layer.biases_hidden, optimizer);
        update(device, layer.initial_hidden_state, optimizer);
    }

    template<typename DEVICE, typename SPEC, typename OPTIMIZER>
    void _reset_optimizer_state(DEVICE& device, nn::layers::gru::LayerGradient<SPEC>& layer, OPTIMIZER& optimizer) {
        _reset_optimizer_state(device, layer.weights_input, optimizer);
        _reset_optimizer_state(device, layer.biases_input, optimizer);
        _reset_optimizer_state(device, layer.weights_hidden, optimizer);
        _reset_optimizer_state(device, layer.biases_hidden, optimizer);
        _reset_optimizer_state(device, layer.initial_hidden_state, optimizer);
    }

    template<typename DEVICE, typename SPEC>
    void one_minus(DEVICE& device, Tensor<SPEC>& t){
        using T = typename SPEC::T;
        using PARAMETER = T;
        tensor::Operation<tensor::unary_operations::one_minus<DEVICE, PARAMETER, T>, PARAMETER> op;
        unary_operation(device, op, t);
    }
    template<typename DEVICE, typename SPEC, typename SPEC_OUTPUT>
    void one_minus(DEVICE& device, Tensor<SPEC>& t, Tensor<SPEC_OUTPUT>& output){
        using T = typename SPEC::T;
        using PARAMETER = T;
        tensor::Operation<tensor::unary_operations::one_minus<DEVICE, PARAMETER, T>, PARAMETER> op;
        unary_operation(device, op, t, output);
    }

    namespace tensor::ternary_operations{
        template <typename T>
        T multiply_one_minus_times_d_tanh_post_activation(T factor, T one_minus, T tanh_post_activation){
            return factor * (1-one_minus) * (1-tanh_post_activation*tanh_post_activation);
        }
    }

    template<typename DEVICE, typename SPEC_FACTOR, typename SPEC_OM, typename SPEC_TANH, typename SPEC_RESULT>
    void multiply_one_minus_times_d_tanh_post_activation(DEVICE& device, Tensor<SPEC_FACTOR>& factor, Tensor<SPEC_OM>& one_minus, Tensor<SPEC_TANH>& tanh_post_activation, Tensor<SPEC_RESULT>& result){
        using T = typename SPEC_FACTOR::T;
        using PARAMETER = T;
        tensor::Operation<tensor::ternary_operations::multiply_one_minus_times_d_tanh_post_activation<T>, PARAMETER> op;
        ternary_operation(device, op, factor, one_minus, tanh_post_activation, result);
    }
    namespace tensor::binary_operations{
        template <typename T>
        T multiply_d_sigmoid_post_activation(T factor, T post_activation, OperationEmptyParameter){
            return factor * post_activation * (1-post_activation);
        }
    }
    template<typename DEVICE, typename SPEC_FACTOR, typename SPEC_PA, typename SPEC_RESULT>
    void multiply_d_sigmoid_post_activation(DEVICE& device, Tensor<SPEC_FACTOR>& factor, Tensor<SPEC_PA>& pre_activation, Tensor<SPEC_RESULT>& result){
        using T = typename SPEC_FACTOR::T;
        tensor::Operation<tensor::binary_operations::multiply_d_sigmoid_post_activation<T>, tensor::OperationEmptyParameter> op;
        binary_operation(device, op, factor, pre_activation, result);
    }

    template<bool CALCULATE_D_INPUT, typename DEVICE, typename LAYER_SPEC, typename INPUT_SPEC, typename D_OUTPUT_SPEC, typename D_INPUT_SPEC, typename MODE = nn::mode::Default>
    void _backward(DEVICE& device, nn::layers::gru::LayerGradient<LAYER_SPEC>& layer, const Tensor<INPUT_SPEC>& input, Tensor<D_OUTPUT_SPEC>& d_output, Tensor<D_INPUT_SPEC>& d_input, nn::layers::gru::buffers::Backward<LAYER_SPEC>& buffers, typename DEVICE::index_t step_i, const nn::Mode<MODE>& mode = nn::Mode<nn::mode::Default>{}){
#ifdef RL_TOOLS_ENABLE_TRACY
        ZoneScopedN("gru::_backward_step");
#endif
        // call with backward<false> to disable d_input calculation
        // warning: this modifies d_output!
        static_assert(tensor::same_dimensions<typename decltype(layer.output)::SPEC, D_OUTPUT_SPEC>());
        static_assert(tensor::same_dimensions<INPUT_SPEC, D_INPUT_SPEC>());
        static_assert(nn::layers::gru::check_input_output<LAYER_SPEC, INPUT_SPEC, typename decltype(layer.output)::SPEC>, "Input and output spec not matching");
        using TI = typename DEVICE::index_t;
        auto input_step = view(device, input, step_i);
        auto n_pre_pre_activation_step = view(device, layer.n_pre_pre_activation, step_i);
        auto post_activation_step = view(device, layer.post_activation, step_i);
        auto d_output_step = view(device, d_output, step_i);
        auto d_input_step = view(device, d_input, step_i);


        auto rz_post_activation = view_range(device, post_activation_step, 0*LAYER_SPEC::HIDDEN_DIM, tensor::ViewSpec<1, 2*LAYER_SPEC::HIDDEN_DIM>{});
        auto r_post_activation = view_range(device, post_activation_step, 0*LAYER_SPEC::HIDDEN_DIM, tensor::ViewSpec<1, LAYER_SPEC::HIDDEN_DIM>{});
        auto z_post_activation = view_range(device, post_activation_step, 1*LAYER_SPEC::HIDDEN_DIM, tensor::ViewSpec<1, LAYER_SPEC::HIDDEN_DIM>{});
        auto n_post_activation = view_range(device, post_activation_step, 2*LAYER_SPEC::HIDDEN_DIM, tensor::ViewSpec<1, LAYER_SPEC::HIDDEN_DIM>{});

        if(step_i == 0){
            multiply_subtract_broadcast(device, d_output_step, layer.initial_hidden_state.parameters, n_post_activation, buffers.buffer_z);
            auto d_output_previous_step = layer.initial_hidden_state.gradient;
            multiply_accumulate_reduce(device, d_output_step, z_post_activation, d_output_previous_step);
        }
        else{
            auto output_previous_step = view(device, layer.output, step_i-1);
            multiply_subtract(device, d_output_step, output_previous_step, n_post_activation, buffers.buffer_z);
            auto d_output_previous_step = view(device, d_output, step_i-1);
            multiply_accumulate(device, d_output_step, z_post_activation, d_output_previous_step);
        }
        multiply_one_minus_times_d_tanh_post_activation(device, d_output_step, z_post_activation, n_post_activation, buffers.buffer_n);
        multiply(device, buffers.buffer_n, n_pre_pre_activation_step, buffers.buffer_r);
        multiply_d_sigmoid_post_activation(device, buffers.buffer_rz, rz_post_activation, buffers.buffer_rz);
        auto buffer_transpose = permute(device, buffers.buffer, tensor::PermutationSpec<1, 0>{});
        static_assert(decltype(buffer_transpose)::SPEC::SIZE == decltype(buffers.buffer)::SPEC::SIZE);
        matrix_multiply_accumulate(device, buffer_transpose, input_step, layer.weights_input.gradient);

        reduce_sum<true>(device, buffer_transpose, layer.biases_input.gradient);
        auto b_irz_grad = view_range(device, layer.biases_input.gradient, 0*LAYER_SPEC::HIDDEN_DIM, tensor::ViewSpec<0, 2*LAYER_SPEC::HIDDEN_DIM>{});
        auto b_hrz_grad = view_range(device, layer.biases_hidden.gradient, 0*LAYER_SPEC::HIDDEN_DIM, tensor::ViewSpec<0, 2*LAYER_SPEC::HIDDEN_DIM>{});
        copy(device, device, b_irz_grad, b_hrz_grad);

        if constexpr(CALCULATE_D_INPUT){
            matrix_multiply(device, buffers.buffer, layer.weights_input.parameters, d_input_step);
        }

        multiply(device, r_post_activation, buffers.buffer_n);

        if(step_i == 0){
            matrix_multiply_broadcast_accumulate(device, buffer_transpose, layer.initial_hidden_state.parameters, layer.weights_hidden.gradient);
            auto d_output_previous_step = layer.initial_hidden_state.gradient;
            matrix_multiply_accumulate_reduce(device, buffers.buffer, layer.weights_hidden.parameters, d_output_previous_step);
        }
        else{
            auto output_previous_step = view(device, layer.output, step_i-1);
            matrix_multiply_accumulate(device, buffer_transpose, output_previous_step, layer.weights_hidden.gradient);
            auto d_output_previous_step = view(device, d_output, step_i-1);
            matrix_multiply_accumulate(device, buffers.buffer, layer.weights_hidden.parameters, d_output_previous_step);
        }

        auto b_hn_grad = view_range(device, layer.biases_hidden.gradient, 2*LAYER_SPEC::HIDDEN_DIM, tensor::ViewSpec<0, LAYER_SPEC::HIDDEN_DIM>{});
        auto buffer_n_transpose = permute(device, buffers.buffer_n, tensor::PermutationSpec<1, 0>{});
        reduce_sum<true>(device, buffer_n_transpose, b_hn_grad);
    }
    template<bool CALCULATE_D_INPUT, typename DEVICE, typename LAYER_SPEC, typename INPUT_SPEC, typename D_OUTPUT_SPEC, typename D_INPUT_SPEC, typename MODE = nn::mode::Default>
    void _backward(DEVICE& device, nn::layers::gru::LayerGradient<LAYER_SPEC>& layer, const Tensor<INPUT_SPEC>& input, Tensor<D_OUTPUT_SPEC>& d_output, Tensor<D_INPUT_SPEC>& d_input, nn::layers::gru::buffers::Backward<LAYER_SPEC>& buffers, const nn::Mode<MODE>& mode = nn::Mode<nn::mode::Default>{}){
        using TI = typename DEVICE::index_t;
        for(TI step_i=LAYER_SPEC::SEQUENCE_LENGTH-1; true; step_i--){
            _backward<CALCULATE_D_INPUT>(device, layer, input, d_output, d_input, buffers, step_i);
            if(step_i == 0){
                break;
            }
        }
    }
    template<typename DEVICE, typename LAYER_SPEC, typename INPUT_SPEC, typename D_OUTPUT_SPEC, typename D_INPUT_SPEC, typename MODE = nn::mode::Default>
    void backward_full(DEVICE& device, nn::layers::gru::LayerGradient<LAYER_SPEC>& layer, const Tensor<INPUT_SPEC>& input, Tensor<D_OUTPUT_SPEC>& d_output, Tensor<D_INPUT_SPEC>& d_input, nn::layers::gru::buffers::Backward<LAYER_SPEC>& buffers, typename DEVICE::index_t step_i, const nn::Mode<MODE>& mode = nn::Mode<nn::mode::Default>{}){
        _backward<true>(device, layer, input, d_output, d_input, buffers, step_i);
    }
    template<typename DEVICE, typename LAYER_SPEC, typename INPUT_SPEC, typename D_OUTPUT_SPEC, typename MODE = nn::mode::Default>
    void backward(DEVICE& device, nn::layers::gru::LayerGradient<LAYER_SPEC>& layer, const Tensor<INPUT_SPEC>& input, Tensor<D_OUTPUT_SPEC>& d_output, nn::layers::gru::buffers::Backward<LAYER_SPEC>& buffers, typename DEVICE::index_t step_i, const nn::Mode<MODE>& mode = nn::Mode<nn::mode::Default>{}){
        using T = typename LAYER_SPEC::T;
        using TI = typename DEVICE::index_t;
        Tensor<tensor::Specification<T, TI, typename INPUT_SPEC::SHAPE>> d_input_dummy; // not allocated, pointer should be optimized away because it is not used
        _backward<false>(device, layer, input, d_output, d_input_dummy, buffers, step_i);
    }
    template<typename DEVICE, typename LAYER_SPEC, typename INPUT_SPEC, typename D_OUTPUT_SPEC, typename D_INPUT_SPEC, typename MODE = nn::mode::Default>
    void backward_full(DEVICE& device, nn::layers::gru::LayerGradient<LAYER_SPEC>& layer, const Tensor<INPUT_SPEC>& input, Tensor<D_OUTPUT_SPEC>& d_output, Tensor<D_INPUT_SPEC>& d_input, nn::layers::gru::buffers::Backward<LAYER_SPEC>& buffers, const nn::Mode<MODE>& mode = nn::Mode<nn::mode::Default>{}){
#ifdef RL_TOOLS_ENABLE_TRACY
        ZoneScopedN("gru::backward_full");
#endif
        _backward<true>(device, layer, input, d_output, d_input, buffers);
    }
    template<typename DEVICE, typename LAYER_SPEC, typename INPUT_SPEC, typename D_OUTPUT_SPEC, typename MODE = nn::mode::Default>
    void backward(DEVICE& device, nn::layers::gru::LayerGradient<LAYER_SPEC>& layer, const Tensor<INPUT_SPEC>& input, Tensor<D_OUTPUT_SPEC>& d_output, nn::layers::gru::buffers::Backward<LAYER_SPEC>& buffers, const nn::Mode<MODE>& mode = nn::Mode<nn::mode::Default>{}){
        using T = typename LAYER_SPEC::T;
        using TI = typename DEVICE::index_t;
        Tensor<tensor::Specification<T, TI, typename INPUT_SPEC::SHAPE>> d_input_dummy; // not allocated, pointer should be optimized away because it is not used
        _backward<false>(device, layer, input, d_output, d_input_dummy, buffers);
    }

    template <typename SOURCE_DEVICE, typename TARGET_DEVICE, typename SOURCE_SPEC, typename TARGET_SPEC>
    void copy(SOURCE_DEVICE& source_device, TARGET_DEVICE& target_device, const nn::layers::gru::LayerForward<SOURCE_SPEC>& source, nn::layers::gru::LayerForward<TARGET_SPEC>& target){
        copy(source_device, target_device, source.weights_input, target.weights_input);
        copy(source_device, target_device, source.biases_input, target.biases_input);
        copy(source_device, target_device, source.weights_hidden, target.weights_hidden);
        copy(source_device, target_device, source.biases_hidden, target.biases_hidden);
        copy(source_device, target_device, source.initial_hidden_state, target.initial_hidden_state);
    }
    template <typename SOURCE_DEVICE, typename TARGET_DEVICE, typename SOURCE_SPEC, typename TARGET_SPEC>
    void copy(SOURCE_DEVICE& source_device, TARGET_DEVICE& target_device, const nn::layers::gru::LayerBackward<SOURCE_SPEC>& source, nn::layers::gru::LayerBackward<TARGET_SPEC>& target){
        copy(source_device, target_device, static_cast<const nn::layers::gru::LayerForward<SOURCE_SPEC>&>(source), static_cast<nn::layers::gru::LayerForward<TARGET_SPEC>&>(target));
        copy(source_device, target_device, source.post_activation, target.post_activation);
        copy(source_device, target_device, source.n_pre_pre_activation, target.n_pre_pre_activation);
        copy(source_device, target_device, source.output, target.output);
    }
    template <typename SOURCE_DEVICE, typename TARGET_DEVICE, typename SOURCE_SPEC, typename TARGET_SPEC>
    void copy(SOURCE_DEVICE& source_device, TARGET_DEVICE& target_device, const nn::layers::gru::LayerGradient<SOURCE_SPEC>& source, nn::layers::gru::LayerGradient<TARGET_SPEC>& target){
        copy(source_device, target_device, static_cast<const nn::layers::gru::LayerForward<SOURCE_SPEC>&>(source), static_cast<nn::layers::gru::LayerForward<TARGET_SPEC>&>(target));
    }

    template <typename DEVICE, typename SPEC>
    void reset_forward_state(DEVICE& device, rl_tools::nn::layers::gru::LayerForward<SPEC>& l) { }

    template <typename DEVICE, typename SPEC>
    void reset_forward_state(DEVICE& device, rl_tools::nn::layers::gru::LayerBackward<SPEC>& l) {
        set_all(device, l.post_activation, 0);
        set_all(device, l.n_pre_pre_activation, 0);
        set_all(device, l.output, 0);
        reset_forward_state(device, static_cast<rl_tools::nn::layers::gru::LayerForward<SPEC>&>(l));
    }

    template <typename DEVICE, typename SPEC_1, typename SPEC_2>
    typename SPEC_1::T abs_diff(DEVICE& device, const rl_tools::nn::layers::gru::LayerForward<SPEC_1>& l1, const rl_tools::nn::layers::gru::LayerForward<SPEC_2>& l2) {
        typename SPEC_1::T diff = 0;
        diff += abs_diff(device, l1.weights_input, l2.weights_input);
        diff += abs_diff(device, l1.biases_input, l2.biases_input);
        diff += abs_diff(device, l1.weights_hidden, l2.weights_hidden);
        diff += abs_diff(device, l1.biases_hidden, l2.biases_hidden);
        diff += abs_diff(device, l1.initial_hidden_state, l2.initial_hidden_state);
        return diff;
    }
    template <typename DEVICE, typename SPEC_1, typename SPEC_2>
    typename SPEC_1::T abs_diff(DEVICE& device, const rl_tools::nn::layers::gru::LayerBackward<SPEC_1>& l1, const rl_tools::nn::layers::gru::LayerBackward<SPEC_2>& l2) {
        using T = typename SPEC_1::T;
        T diff = abs_diff(device, static_cast<const rl_tools::nn::layers::gru::LayerForward<SPEC_1>&>(l1), static_cast<const rl_tools::nn::layers::gru::LayerForward<SPEC_2>&>(l2));
        diff += abs_diff(device, l1.post_activation, l2.post_activation);
        diff += abs_diff(device, l1.n_pre_pre_activation, l2.n_pre_pre_activation);
        diff += abs_diff(device, l1.output, l2.output);
        return diff;
    }
    template <typename DEVICE, typename SPEC_1, typename SPEC_2>
    typename SPEC_1::T abs_diff(DEVICE& device, const rl_tools::nn::layers::gru::LayerGradient<SPEC_1>& l1, const rl_tools::nn::layers::gru::LayerGradient<SPEC_2>& l2) {
        return abs_diff(device, static_cast<const rl_tools::nn::layers::gru::LayerBackward<SPEC_1>&>(l1), static_cast<const rl_tools::nn::layers::gru::LayerBackward<SPEC_2>&>(l2));
    }

    template <typename DEVICE, typename SPEC>
    void free(DEVICE& device, nn::layers::gru::LayerForward<SPEC>& layer){
        free(device, layer.weights_input);
        free(device, layer.biases_input);
        free(device, layer.weights_hidden);
        free(device, layer.biases_hidden);
        free(device, layer.initial_hidden_state);
    }
    template <typename DEVICE, typename SPEC>
    void free(DEVICE& device, nn::layers::gru::LayerBackward<SPEC>& layer){
        free(device, static_cast<nn::layers::gru::LayerForward<SPEC>&>(layer));
        free(device, layer.n_pre_pre_activation);
        free(device, layer.post_activation);
        free(device, layer.output);
    }
    template <typename DEVICE, typename SPEC>
    void free(DEVICE& device, nn::layers::gru::buffers::Evaluation<SPEC>& buffers){
        free(device, buffers.post_activation);
        free(device, buffers.n_pre_pre_activation);
    }
    template <typename SPEC>
    auto output(nn::layers::gru::LayerBackward<SPEC>& layer){
        return layer.output;
    }
}
RL_TOOLS_NAMESPACE_WRAPPER_END

#endif