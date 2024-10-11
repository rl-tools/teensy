#include "../../../version.h"
#if (defined(RL_TOOLS_DISABLE_INCLUDE_GUARDS) || !defined(RL_TOOLS_NN_LAYERS_DENSE_PERSIST_CODE_H)) && (RL_TOOLS_USE_THIS_VERSION == 1)
#pragma once
#define RL_TOOLS_NN_LAYERS_DENSE_PERSIST_CODE_H
#include "layer.h"
#include "../../../containers/matrix/persist_code.h"
#include <sstream>
#include "../../../persist/code.h"
#include "../../../containers/matrix/persist_code.h"
#include "../../../nn/capability/persist_code.h"

RL_TOOLS_NAMESPACE_WRAPPER_START
namespace rl_tools {
    namespace nn::layers::dense::persist{
        template<nn::activation_functions::ActivationFunction ACTIVATION_FUNCTION>
        auto get_activation_function_string(){
            static_assert(ACTIVATION_FUNCTION == nn::activation_functions::ActivationFunction::IDENTITY ||
                          ACTIVATION_FUNCTION == nn::activation_functions::ActivationFunction::RELU ||
                          ACTIVATION_FUNCTION == nn::activation_functions::ActivationFunction::GELU ||
                          ACTIVATION_FUNCTION == nn::activation_functions::ActivationFunction::TANH ||
                          ACTIVATION_FUNCTION == nn::activation_functions::ActivationFunction::FAST_TANH ||
                          ACTIVATION_FUNCTION == nn::activation_functions::ActivationFunction::SIGMOID);

            if constexpr (ACTIVATION_FUNCTION == nn::activation_functions::ActivationFunction::IDENTITY){
                return "RL_TOOLS""_NAMESPACE_WRAPPER ::rl_tools::nn::activation_functions::ActivationFunction::IDENTITY";
            } else if constexpr (ACTIVATION_FUNCTION == nn::activation_functions::ActivationFunction::RELU){
                return "RL_TOOLS""_NAMESPACE_WRAPPER ::rl_tools::nn::activation_functions::ActivationFunction::RELU";
            } else if constexpr (ACTIVATION_FUNCTION == nn::activation_functions::ActivationFunction::TANH){
                return "RL_TOOLS""_NAMESPACE_WRAPPER ::rl_tools::nn::activation_functions::ActivationFunction::TANH";
            } else if constexpr (ACTIVATION_FUNCTION == nn::activation_functions::ActivationFunction::FAST_TANH){
                return "RL_TOOLS""_NAMESPACE_WRAPPER ::rl_tools::nn::activation_functions::ActivationFunction::FAST_TANH";
            } else if constexpr (ACTIVATION_FUNCTION == nn::activation_functions::ActivationFunction::SIGMOID){
                return "RL_TOOLS""_NAMESPACE_WRAPPER ::rl_tools::nn::activation_functions::ActivationFunction::SIGMOID";
            }
        }
//        auto get_shape_factory_string(nn::layers::dense::DefaultInputShapeFactory){
//            return "RL_TOOLS""_NAMESPACE_WRAPPER ::rl_tools::nn::layers::dense::DefaultInputShapeFactory";
//        }
//        template <typename TI, TI SEQUENCE_LENGTH>
//        auto get_shape_factory_string(nn::layers::dense::SequenceInputShapeFactory<TI, SEQUENCE_LENGTH>){
//            return std::string("RL_TOOLS""_NAMESPACE_WRAPPER ::rl_tools::nn::layers::dense::SequenceInputShapeFactory<") + containers::persist::get_type_string<TI>() + ", " + std::to_string(SEQUENCE_LENGTH) + ">";
//        }
    }
    namespace nn::layers::dense::persist_code{
        template<typename DEVICE, typename SPEC>
            rl_tools::persist::Code finish(DEVICE& device, nn::layers::dense::LayerForward<SPEC> &layer, std::string name, rl_tools::persist::Code input, bool const_declaration=false, typename DEVICE::index_t indent=0){
            using TI = typename DEVICE::index_t;
            std::stringstream indent_ss;
            for(TI i=0; i < indent; i++){
                indent_ss << "    ";
            }
            std::string ind = indent_ss.str();
            using TI = typename DEVICE::index_t;
            std::stringstream ss, ss_header;
            ss_header << input.header;
            ss_header << "#include <rl_tools/nn/layers/dense/layer.h>\n";
            ss << input.body;
            std::string T_string = containers::persist::get_type_string<typename SPEC::T>();
            std::string TI_string = containers::persist::get_type_string<typename SPEC::TI>();
            ss << ind << "namespace " << name << " {\n";
            ss << ind << "    using CONFIG = " << "RL_TOOLS""_NAMESPACE_WRAPPER ::rl_tools::nn::layers::dense::Configuration<"
                << T_string << ", "
                << TI_string << ", "
                << SPEC::OUTPUT_DIM << ", "
                << nn::layers::dense::persist::get_activation_function_string<SPEC::ACTIVATION_FUNCTION>() << ", "
                << "RL_TOOLS""_NAMESPACE_WRAPPER ::rl_tools::nn::layers::dense::DefaultInitializer<" << T_string << ", " << TI_string << ">, "
                << get_type_string_tag(device, typename SPEC::PARAMETER_GROUP{})
                << ">; \n";
            ss << ind << "    " << "using TEMPLATE = RL_TOOLS""_NAMESPACE_WRAPPER ::rl_tools::nn::layers::dense::BindConfiguration<CONFIG>;" << "\n";
            ss << ind << "    " << "using INPUT_SHAPE = RL_TOOLS""_NAMESPACE_WRAPPER ::rl_tools::tensor::Shape<" << TI_string << ", " << get<0>(typename SPEC::INPUT_SHAPE{}) << ", " << get<1>(typename SPEC::INPUT_SHAPE{}) << ", " << get<2>(typename SPEC::INPUT_SHAPE{}) << ">;\n";
            ss << ind << "    " << "using CAPABILITY = " << to_string(typename SPEC::CAPABILITY{}) << ";" << "\n";
            ss << ind << "    " << "using TYPE = RL_TOOLS""_NAMESPACE_WRAPPER ::rl_tools::nn::layers::dense::Layer<CONFIG, CAPABILITY, INPUT_SHAPE>;" << "\n";
            std::string initializer_list;
            if constexpr(SPEC::CAPABILITY::TAG == nn::LayerCapability::Forward){
                initializer_list = "{weights::parameters, biases::parameters}";
            }
            else{
                if constexpr(SPEC::CAPABILITY::TAG == nn::LayerCapability::Backward){
                    initializer_list = "{{weights::parameters, biases::parameters}, pre_activations::container}";
                }
                else{
                    if constexpr(SPEC::CAPABILITY::TAG == nn::LayerCapability::Gradient){
                        initializer_list = "{{{weights::parameters, biases::parameters}, pre_activations::container}, output::container}";
                    }
                    else{
                        utils::assert_exit(device, false, "Unknown capability");
                    }
                }
            }
            ss << ind << "    " << (const_declaration ? "const " : "") << "TYPE module = " << initializer_list << ";\n";
            ss << ind << "    " << "template <typename MODEL>" << "\n";
            ss << ind << "    " << "constexpr MODEL create(){" << "\n";
            ss << ind << "    " << "    return MODEL" << initializer_list << ";" << "\n";
            ss << ind << "    " << "}" << "\n";
            ss << ind << "}\n";


            return {ss_header.str(), ss.str()};
        }
    }
    template<typename DEVICE, typename SPEC>
    persist::Code save_code_split(DEVICE& device, nn::layers::dense::LayerForward<SPEC> &layer, std::string name, bool const_declaration=false, typename DEVICE::index_t indent=0, bool finish=true){
        using TI = typename DEVICE::index_t;
        std::stringstream indent_ss;
        for(TI i=0; i < indent; i++){
            indent_ss << "    ";
        }
        std::string ind = indent_ss.str();
        using TI = typename DEVICE::index_t;
        std::stringstream ss, ss_header;
        ss << ind << "namespace " << name << " {\n";
        auto weights = save_code_split(device, layer.weights, "weights", const_declaration, indent+1);
        ss_header << weights.header;
        ss << weights.body;
        auto biases = save_code_split(device, layer.biases, "biases", const_declaration, indent+1);
        ss_header << biases.header;
        ss << biases.body;
        ss << ind << "}\n";
        if(finish){
            return nn::layers::dense::persist_code::finish(device, layer, name, {ss_header.str(), ss.str()}, const_declaration, indent);
        }
        else{
            return {ss_header.str(), ss.str()};
        }
    }
    template<typename DEVICE, typename SPEC>
    persist::Code save_code_split(DEVICE& device, nn::layers::dense::LayerBackward<SPEC> &layer, std::string name, bool const_declaration=false, typename DEVICE::index_t indent=0, bool finish=true){
        using TI = typename DEVICE::index_t;
        std::stringstream indent_ss;
        for(TI i=0; i < indent; i++){
            indent_ss << "    ";
        }
        std::string ind = indent_ss.str();
        using TI = typename DEVICE::index_t;
        std::stringstream ss, ss_header;
        auto previous = save_code_split(device, static_cast<nn::layers::dense::LayerForward<SPEC>&>(layer), name, const_declaration, indent, false);
        ss_header << previous.header;
        ss << previous.body;
        ss << ind << "namespace " << name << " {\n";
        auto pre_activations = save_code_split(device, layer.pre_activations, "pre_activations", const_declaration, indent+1);
        ss_header << pre_activations.header;
        ss << pre_activations.body;
        ss << ind << "}\n";
        if(finish){
            return nn::layers::dense::persist_code::finish(device, layer, name, {ss_header.str(), ss.str()}, const_declaration, indent);
        }
        else{
            return {ss_header.str(), ss.str()};
        }
    }

    template<typename DEVICE, typename SPEC>
    persist::Code save_code_split(DEVICE& device, nn::layers::dense::LayerGradient<SPEC> &layer, std::string name, bool const_declaration=false, typename DEVICE::index_t indent=0){
        using TI = typename DEVICE::index_t;
        std::stringstream indent_ss;
        for(TI i=0; i < indent; i++){
            indent_ss << "    ";
        }
        std::string ind = indent_ss.str();
        using TI = typename DEVICE::index_t;
        std::stringstream ss, ss_header;
        auto previous = save_code_split(device, static_cast<nn::layers::dense::LayerBackward<SPEC>&>(layer), name, const_declaration, indent, false);
        ss_header << previous.header;
        ss << previous.body;
        ss << ind << "namespace " << name << " {\n";
        auto output = save_code_split(device, layer.output, "output", const_declaration, indent+1);
        ss_header << output.header;
        ss << output.body;
        ss << ind << "}\n";
        return nn::layers::dense::persist_code::finish(device, layer, name, {ss_header.str(), ss.str()}, const_declaration, indent);
    }

    template<typename DEVICE, typename SPEC>
    std::string save_code(DEVICE& device, nn::layers::dense::LayerForward<SPEC> &layer, std::string name, bool const_declaration=false, typename DEVICE::index_t indent=0){
        auto code = save_code_split(device, layer, name, const_declaration, indent);
        return code.header + code.body;
    }
}
RL_TOOLS_NAMESPACE_WRAPPER_END

#endif
