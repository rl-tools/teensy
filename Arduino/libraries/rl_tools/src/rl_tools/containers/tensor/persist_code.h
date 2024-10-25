#include "../../version.h"
#if (defined(RL_TOOLS_DISABLE_INCLUDE_GUARDS) || !defined(RL_TOOLS_CONTAINERS_TENSOR_PERSIST_CODE_H)) && (RL_TOOLS_USE_THIS_VERSION == 1)
#pragma once
#define RL_TOOLS_CONTAINERS_TENSOR_PERSIST_CODE_H

#include "../../persist/code.h"
#include "../../utils/generic/typing.h"
#include "../matrix/persist_code.h"
#include <sstream>

RL_TOOLS_NAMESPACE_WRAPPER_START
namespace rl_tools{
    template<typename DEVICE, typename SPEC>
    persist::Code save_code_split(DEVICE& device, Tensor<SPEC>& tensor, std::string name, bool const_declaration=false, typename DEVICE::index_t indent=0){
        using T = typename SPEC::T;
        using TI = typename DEVICE::index_t;
        static_assert(utils::typing::is_same_v<containers::persist::STORAGE_TYPE, unsigned char>);
        static_assert(sizeof(T) % sizeof(containers::persist::STORAGE_TYPE) == 0);
        std::stringstream indent_ss;
        for(TI i=0; i < indent; i++){
            indent_ss << "    ";
        }
        std::string ind = indent_ss.str();
        std::stringstream ss_header;
        ss_header << "// NOTE: This code export assumes that the endianness of the target platform is the same as the endianness of the source platform\n";
        ss_header << "#include <rl_tools/containers/tensor/tensor.h>\n";
        std::stringstream ss;
        ss << ind << "namespace " << name << " {\n";
        ss << ind << "    static_assert(sizeof(" << containers::persist::get_type_string<containers::persist::STORAGE_TYPE>() << ") == 1);\n";
        ss << ind << "    alignas(" << containers::persist::get_type_string<T>() << ") " << (const_declaration ? "const " : "") << containers::persist::get_type_string<containers::persist::STORAGE_TYPE>() << " memory[] = {";

        auto m = matrix_view(device, tensor);
        using MATRIX_SPEC = typename decltype(m)::SPEC;
        bool first = true;
        for(TI i=0; i < MATRIX_SPEC::ROWS; i++){
            for(TI j=0; j < MATRIX_SPEC::COLS; j++){
                auto value = get(m, i, j);
                auto* ptr = reinterpret_cast<containers::persist::STORAGE_TYPE*>(&value);
                for(TI k=0; k < sizeof(T); k++){
                    if(!first){
                        ss << ", ";
                    }
                    ss << (int)ptr[k];
                    first = false;
                }
            }
        }
        ss << "};\n";
        ss << ind << "    using SHAPE = RL_TOOLS""_NAMESPACE_WRAPPER ::rl_tools::tensor::Shape<" << containers::persist::get_type_string<TI>() << ", ";
        for(TI dim_i=0; dim_i < length(typename SPEC::SHAPE{}); dim_i++){
            if(dim_i > 0){
                ss << ", ";
            }
            ss << get(device, typename SPEC::SHAPE{}, dim_i);
        }
        ss << ">;\n";
        ss << ind << "    using SPEC = RL_TOOLS""_NAMESPACE_WRAPPER ::rl_tools::tensor::Specification<";
        ss << containers::persist::get_type_string<T>() << ", ";
        ss << containers::persist::get_type_string<TI>() << ", ";
        ss << "SHAPE, ";
        constexpr bool DYNAMIC_ALLOCATION = true; // it is not dynamically allocated but the statically allocated exported arrays are assigned to the pointer of the tensor. This is easier than initializing a static array inside the tensor directly
        ss << (DYNAMIC_ALLOCATION ? "true" : "false") << ", ";
        ss << "RL_TOOLS""_NAMESPACE_WRAPPER ::rl_tools::tensor::RowMajorStride<SHAPE>, ";
        constexpr bool CONST = true;
        ss << (CONST ? "true" : "false");
        ss << ">;\n";
        ss << ind << "    using CONTAINER_TYPE = RL_TOOLS""_NAMESPACE_WRAPPER ::rl_tools::Tensor<SPEC>;\n";
        ss << ind << "    " << (const_declaration ? "const " : "") << "CONTAINER_TYPE container = {(" << containers::persist::get_type_string<T>() << "*)" << "memory}; \n";
        ss << ind << "}\n";
        return {ss_header.str(), ss.str()};
    }
    template<typename DEVICE, typename SPEC>
    std::string save_code(DEVICE& device, Tensor<SPEC>& m, std::string name, bool const_declaration, typename DEVICE::index_t indent=0){
        auto code = save_code_split(device, m, name, const_declaration, indent);
        return code.header + code.body;
    }
}
RL_TOOLS_NAMESPACE_WRAPPER_END
#endif
