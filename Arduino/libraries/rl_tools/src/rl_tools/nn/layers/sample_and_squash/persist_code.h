#include "../../../version.h"
#if (defined(RL_TOOLS_DISABLE_INCLUDE_GUARDS) || !defined(RL_TOOLS_NN_LAYERS_SAMPLE_AND_SQUASH_PERSIST_CODE_H)) && (RL_TOOLS_USE_THIS_VERSION == 1)
#pragma once
#define RL_TOOLS_NN_LAYERS_SAMPLE_AND_SQUASH_PERSIST_CODE_H
#include "layer.h"
#include "../../../containers/matrix/persist_code.h"
#include <sstream>
#include "../../../persist/code.h"
#include "../../../containers/matrix/persist_code.h"
#include "../../../nn/capability/persist_code.h"

RL_TOOLS_NAMESPACE_WRAPPER_START
namespace rl_tools {
    namespace nn::layers::sample_and_squash::persist_code{
        template<typename DEVICE, typename SPEC>
            rl_tools::persist::Code finish(DEVICE& device, nn::layers::sample_and_squash::LayerForward<SPEC> &layer, std::string name, rl_tools::persist::Code input, bool const_declaration=false, typename DEVICE::index_t indent=0){
            using TI = typename DEVICE::index_t;
            std::stringstream indent_ss;
            for(TI i=0; i < indent; i++){
                indent_ss << "    ";
            }
            std::string ind = indent_ss.str();
            using TI = typename DEVICE::index_t;
            std::stringstream ss, ss_header;
            ss_header << input.header;
            ss_header << "#include <rl_tools/nn/layers/sample_and_squash/layer.h>\n";
            ss << input.body;
            ss << ind << "namespace " << name << " {\n";
            ss << ind << "    using SPEC = " << "RL_TOOLS""_NAMESPACE_WRAPPER ::rl_tools::nn::layers::sample_and_squash::Specification<"
               << containers::persist::get_type_string<typename SPEC::T>() << ", "
               << containers::persist::get_type_string<typename SPEC::TI>() << ", "
               << SPEC::DIM
               << ">; \n";
            ss << ind << "    " << "template <typename CAPABILITY>" << "\n";
            ss << ind << "    " << "using TEMPLATE = RL_TOOLS""_NAMESPACE_WRAPPER ::rl_tools::nn::layers::sample_and_squash::Layer<CAPABILITY, SPEC>;" << "\n";
            ss << ind << "    " << "using CAPABILITY = " << to_string(typename SPEC::CAPABILITY{}) << ";" << "\n";
            ss << ind << "    " << "using TYPE = RL_TOOLS""_NAMESPACE_WRAPPER ::rl_tools::nn::layers::sample_and_squash::Layer<CAPABILITY, SPEC>;" << "\n";
            std::string initializer_list;
            if constexpr(SPEC::CAPABILITY::TAG == nn::LayerCapability::Forward){
                initializer_list = "{}";
            }
            else{
                if constexpr(SPEC::CAPABILITY::TAG == nn::LayerCapability::Backward){
                    initializer_list = "{{}, pre_squashing::container, noise::container}";
                }
                else{
                    if constexpr(SPEC::CAPABILITY::TAG == nn::LayerCapability::Gradient){
                        initializer_list = "{{{}, pre_squashing::container, noise::container}, log_probabilities::container, output::container, log_alpha::parameters}";
                    }
                    else{
                        utils::assert_exit(device, false, "Unknown capability");
                    }
                }
            }
            ss << ind << "    " << (const_declaration ? "const " : "") << "TYPE module = " << initializer_list << ";\n";
            ss << ind << "}\n";


            return {ss_header.str(), ss.str()};
        }
    }
    template<typename DEVICE, typename SPEC>
    persist::Code save_code_split(DEVICE& device, nn::layers::sample_and_squash::LayerForward<SPEC> &layer, std::string name, bool const_declaration=false, typename DEVICE::index_t indent=0, bool finish=true){
        if(finish){
            return nn::layers::sample_and_squash::persist_code::finish(device, layer, name, {"", ""}, const_declaration, indent);
        }
        else{
            return {"", ""};
        }
    }
    template<typename DEVICE, typename SPEC>
    persist::Code save_code_split(DEVICE& device, nn::layers::sample_and_squash::LayerBackward<SPEC> &layer, std::string name, bool const_declaration=false, typename DEVICE::index_t indent=0, bool finish=true){
        using TI = typename DEVICE::index_t;
        std::stringstream indent_ss;
        for(TI i=0; i < indent; i++){
            indent_ss << "    ";
        }
        std::string ind = indent_ss.str();
        using TI = typename DEVICE::index_t;
        std::stringstream ss, ss_header;
        auto previous = save_code_split(device, static_cast<nn::layers::sample_and_squash::LayerForward<SPEC>&>(layer), name, const_declaration, indent, false);
        ss_header << previous.header;
        ss << previous.body;
        ss << ind << "namespace " << name << " {\n";
        {
            auto pre_activations = save_code_split(device, layer.pre_squashing, "pre_squashing", const_declaration, indent+1);
            ss_header << pre_activations.header;
            ss << pre_activations.body;
        }
        {
            auto pre_activations = save_code_split(device, layer.noise, "noise", const_declaration, indent+1);
            ss_header << pre_activations.header;
            ss << pre_activations.body;
        }
        ss << ind << "}\n";
        if(finish){
            return nn::layers::sample_and_squash::persist_code::finish(device, layer, name, {ss_header.str(), ss.str()}, const_declaration, indent);
        }
        else{
            return {ss_header.str(), ss.str()};
        }
    }

    template<typename DEVICE, typename SPEC>
    persist::Code save_code_split(DEVICE& device, nn::layers::sample_and_squash::LayerGradient<SPEC> &layer, std::string name, bool const_declaration=false, typename DEVICE::index_t indent=0){
        using TI = typename DEVICE::index_t;
        std::stringstream indent_ss;
        for(TI i=0; i < indent; i++){
            indent_ss << "    ";
        }
        std::string ind = indent_ss.str();
        using TI = typename DEVICE::index_t;
        std::stringstream ss, ss_header;
        auto previous = save_code_split(device, static_cast<nn::layers::sample_and_squash::LayerBackward<SPEC>&>(layer), name, const_declaration, indent, false);
        ss_header << previous.header;
        ss << previous.body;
        ss << ind << "namespace " << name << " {\n";
        {
            auto output = save_code_split(device, layer.log_alpha, "log_alpha", const_declaration, indent+1);
            ss_header << output.header;
            ss << output.body;
        }
        {
            auto output = save_code_split(device, layer.log_probabilities, "log_probabilities", const_declaration, indent+1);
            ss_header << output.header;
            ss << output.body;
        }
        {
            auto output = save_code_split(device, layer.output, "output", const_declaration, indent+1);
            ss_header << output.header;
            ss << output.body;
        }
        ss << ind << "}\n";
        return nn::layers::sample_and_squash::persist_code::finish(device, layer, name, {ss_header.str(), ss.str()}, const_declaration, indent);
    }

    template<typename DEVICE, typename SPEC>
    std::string save_code(DEVICE& device, nn::layers::sample_and_squash::LayerForward<SPEC> &layer, std::string name, bool const_declaration=false, typename DEVICE::index_t indent=0){
        auto code = save_code_split(device, layer, name, const_declaration, indent);
        return code.header + code.body;
    }
}
RL_TOOLS_NAMESPACE_WRAPPER_END

#endif
