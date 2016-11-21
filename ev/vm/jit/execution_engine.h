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

namespace ev { namespace vm { namespace jit {

using namespace llvm;
using namespace llvm::orc;


struct execution_engine_t {
    using obj_linking_layer_t = ObjectLinkingLayer<>;
    using ir_compile_layer_t = IRCompileLayer<obj_linking_layer_t>;
    using module_handle_t =  ir_compile_layer_t::ModuleSetHandleT ;
    using optimize_function_t =  std::function<Module*(Module*)>  ;

    execution_engine_t()
        : m_target_machine(EngineBuilder().selectTarget()),
          m_data_layout(m_target_machine->createDataLayout()),
          m_compile_layer(m_object_layer, SimpleCompiler(*m_target_machine)),
          m_optimize_layer(m_compile_layer,optimize_module)
    {
        llvm::sys::DynamicLibrary::LoadLibraryPermanently(nullptr);
    }

    TargetMachine &  target_machine  () { return *m_target_machine; }

    module_handle_t add_module(Module* module)
    {
        auto resolver = createLambdaResolver(
        // Look back into the JIT itself to find symbols that are part of
        // the same "logical dylib".
        [&](const std::string &name) {
            if (JITSymbol sym = m_optimize_layer.findSymbol(name, false))
            {
                return sym.toRuntimeDyldSymbol();
            }
            return RuntimeDyld::SymbolInfo(nullptr);
        },
        //Search for external symbols in the host process.
        [](const std::string& name) {
            if (auto sym_addr =
                    RTDyldMemoryManager::getSymbolAddressInProcess(name))
                return RuntimeDyld::SymbolInfo(sym_addr, JITSymbolFlags::Exported);
            return RuntimeDyld::SymbolInfo(nullptr);
        });

        // Build a singlton module set to hold our module.
        std::vector<Module*> modules = {module};

        // Add the set to the JIT with the resolver we created above and a newly
        // created SectionMemoryManager.
        return m_optimize_layer.addModuleSet(std::move(modules),
                                             make_unique<SectionMemoryManager>(),
                                             std::move(resolver));
    }

    JITSymbol find_symbol(const std::string& name) {
        std::string mangled_name;
        raw_string_ostream mangled_name_stream(mangled_name);
        Mangler::getNameWithPrefix(mangled_name_stream, name, m_data_layout);
        return m_optimize_layer.findSymbol(mangled_name_stream.str(), true);
    }

    void remove_module(module_handle_t handle) {
        m_optimize_layer.removeModuleSet(handle);
    }

private:

    static Module* optimize_module(Module* module) {
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
        for (auto & func : *module)
            function_pass_manager->run(func);

        return module;
    }
protected:
    static bool init() {
        static const bool i = InitializeNativeTarget();
        return i;
    }
private:

    bool m_init = init();
    std::unique_ptr<TargetMachine> m_target_machine;
    const DataLayout m_data_layout;
    obj_linking_layer_t m_object_layer;
    ir_compile_layer_t m_compile_layer;
    IRTransformLayer<ir_compile_layer_t, optimize_function_t> m_optimize_layer;
};



}}}
