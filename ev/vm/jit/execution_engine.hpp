#pragma once

#include "llvm/ADT/STLExtras.h"

#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/RuntimeDyld.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/ExecutionEngine/Orc/CompileUtils.h"
#include "llvm/ExecutionEngine/Orc/JITSymbol.h"
#include "llvm/ExecutionEngine/Orc/IRCompileLayer.h"
#include "llvm/ExecutionEngine/Orc/IRTransformLayer.h"
#include "llvm/ExecutionEngine/Orc/LambdaResolver.h"
#include "llvm/ExecutionEngine/Orc/ObjectLinkingLayer.h"

#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Mangler.h"

#include "llvm/Support/DynamicLibrary.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/TargetSelect.h"

#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

namespace ev {
namespace vm {
namespace jit {
using namespace llvm;
using namespace llvm::orc;

struct execution_engine {
    using obj_linking_layer = ObjectLinkingLayer<>;
    using ir_compile_layer = IRCompileLayer<obj_linking_layer>;
    using module_handle = ir_compile_layer::ModuleSetHandleT;
    using optimize_function = std::function<Module*(Module*)>;

    execution_engine()
        : m_target_machine(EngineBuilder().selectTarget()),
          m_data_layout(m_target_machine->createDataLayout()),
          m_compile_layer(m_object_layer, SimpleCompiler(*m_target_machine)),
          m_optimize_layer(m_compile_layer, optimize_module)
    {
        if (!m_init) {
            throw std::runtime_error("Failed to init the JIT engine");
        }
        llvm::sys::DynamicLibrary::LoadLibraryPermanently(nullptr);
    }

    TargetMachine& target_machine()
    {
        return *m_target_machine;
    }
    module_handle add(std::vector<Module*>&& modules)
    {
        auto resolver = createLambdaResolver(
            // Look back into the JIT itself to find symbols that are part of
            // the same "logical dylib".
            [&](const std::string& name) {
                if (JITSymbol sym = m_optimize_layer.findSymbol(name, false)) {
                    return sym.toRuntimeDyldSymbol();
                }
                return RuntimeDyld::SymbolInfo(nullptr);
            },
            // Search for external symbols in the host process.
            [](const std::string& name) {
                if (auto sym_addr = RTDyldMemoryManager::getSymbolAddressInProcess(name))
                    return RuntimeDyld::SymbolInfo(sym_addr, JITSymbolFlags::Exported);
                return RuntimeDyld::SymbolInfo(nullptr);
            });

        // Add the set to the JIT with the resolver we created above and a newly
        // created SectionMemoryManager.
        return m_optimize_layer.addModuleSet(
            std::move(modules), make_unique<SectionMemoryManager>(), std::move(resolver));
    }

    void remove(module_handle handle)
    {
        m_optimize_layer.removeModuleSet(handle);
    }

    std::string mangle(const std::string& symbol) const
    {
        std::string mangled_name;
        raw_string_ostream mangled_name_stream(mangled_name);
        Mangler::getNameWithPrefix(mangled_name_stream, symbol, m_data_layout);
        return mangled_name;
    }

    JITSymbol find_symbol(const std::string& name)
    {
        return m_optimize_layer.findSymbol(mangle(name), true);
    }

private:
    static Module* optimize_module(Module* module)
    {
        // Create a function pass manager.
        auto function_pass_manager = std::make_unique<legacy::FunctionPassManager>(module);

        // Add some optimizations.
        function_pass_manager->add(createInstructionCombiningPass());
        function_pass_manager->add(createReassociatePass());
        function_pass_manager->add(createGVNPass());
        function_pass_manager->add(createCFGSimplificationPass());
        function_pass_manager->doInitialization();

        // Run the optimizations over all functions in the module being added to
        // the JIT.
        for (auto& func : *module) function_pass_manager->run(func);

        return module;
    }

protected:
    static bool init()
    {
        static const bool i =  // the init functions return false if success
            !InitializeNativeTarget() && !InitializeNativeTargetAsmPrinter() &&
            !InitializeNativeTargetAsmParser();

        return i;
    }

private:
    bool m_init = init();
    std::unique_ptr<TargetMachine> m_target_machine;
    const DataLayout m_data_layout;
    obj_linking_layer m_object_layer;
    ir_compile_layer m_compile_layer;
    IRTransformLayer<ir_compile_layer, optimize_function> m_optimize_layer;
};
}
}
}
