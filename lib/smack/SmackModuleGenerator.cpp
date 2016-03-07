//
// This file is distributed under the MIT License. See LICENSE for details.
//
#define DEBUG_TYPE "smack-mod-gen"
#include "smack/SmackModuleGenerator.h"
#include "smack/SmackOptions.h"
#include "smack/Contracts.h"
#include "llvm/LinkAllPasses.h"
#include "llvm/PassManager.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/PrettyStackTrace.h"
#include "llvm/Support/Signals.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/ToolOutputFile.h"
#include "llvm/Support/raw_ostream.h"
#include "assistDS/StructReturnToPointer.h"
#include "assistDS/SimplifyExtractValue.h"
#include "assistDS/SimplifyInsertValue.h"
#include "smack/PruneFunctionPass.h"
#include "smack/BplFilePrinter.h"
#include "smack/DSAAliasAnalysis.h"

namespace smack {

llvm::RegisterPass<SmackModuleGenerator> X("smack", "SMACK generator pass");
char SmackModuleGenerator::ID = 0;

void SmackModuleGenerator::generateProgram(llvm::Module& m) {

  Naming naming;
  SmackRep rep(&getAnalysis<DSAAliasAnalysis>(), naming, program);
  rep.collectRegions(m);
  
  
  DEBUG(errs() << "Analyzing globals...\n");

  for (llvm::Module::const_global_iterator
       x = m.global_begin(), e = m.global_end(); x != e; ++x)
    program.addDecls(rep.globalDecl(x));
  
  program.addDecl(rep.getStaticInit());

  DEBUG(errs() << "Analyzing functions...\n");

  for (llvm::Module::iterator func = m.begin(), e = m.end();
       func != e; ++func) {

    // TODO: Implement function pointers of vararg functions properly
    // if (!func->isVarArg())
    program.addDecls(rep.globalDecl(func));

    ProcDecl* proc = rep.proc(func);
    if (!func->isDeclaration() && proc->getName() != "__SMACK_decls")
      program.addDecl(proc);

    // TODO this will cover the cases of malloc, memcpy, memset, …
    if (func->isDeclaration()) {
      program.addDecls(rep.decl(func));
      continue;
    }

    if (!func->isDeclaration() && !func->empty()
        && !func->getEntryBlock().empty()) {

      DEBUG(errs() << "Analyzing function: " << naming.get(*func) << "\n");

      Slices slices;
      ContractsExtractor ce(rep, *proc, naming, slices);
      SmackInstGenerator igen(rep, *proc, naming, slices);

      naming.enter();
      DEBUG(errs() << "Extracting contracts for " << naming.get(*func) << " from ");
      DEBUG(errs() << *func << "\n");
      ce.visit(func);
      DEBUG(errs() << "\n");

      DEBUG(errs() << "Generating body for " << naming.get(*func) << " from ");
      DEBUG(errs() << *func << "\n");
      igen.visit(func);
      DEBUG(errs() << "\n");
      naming.leave();

      // First execute static initializers, in the main procedure.
      if (naming.get(*func) == "main") {
        proc->insert(Stmt::call(SmackRep::INIT_FUNCS));
        proc->insert(Stmt::call(SmackRep::STATIC_INIT));
      } else if (naming.get(*func).substr(0, 18)  == "__SMACK_init_func_")
        rep.addInitFunc(func);

      DEBUG(errs() << "Finished analyzing function: " << naming.get(*func) << "\n\n");
    }

    // MODIFIES
    // ... to do below, after memory splitting is determined.
  }

  program.addDecl(rep.getInitFuncs());

  // MODIFIES
  vector<ProcDecl*> procs = program.getProcs();
  for (unsigned i=0; i<procs.size(); i++) {
    
    if (procs[i]->hasBody()) {
      procs[i]->addMods(rep.getModifies());
    
    } else {
      vector< pair<string,string> > rets = procs[i]->getRets();
      for (vector< pair<string,string> >::iterator r = rets.begin();
          r != rets.end(); ++r) {
        
        // TODO should only do this for returned POINTERS.
        // procs[i]->addEnsures(rep.declareIsExternal(Expr::id(r->first)));
      }
    }
  }
  
  // NOTE we must do this after instruction generation, since we would not 
  // otherwise know how many regions to declare.
  program.appendPrelude(rep.getPrelude());
  program.addPreludeDecls(rep.getTypeDecls());
  program.addPreludeDecls(rep.getMemoryRegionDecls());
  program.addPreludeDecls(rep.getAxiomDecls());
}

SmackModuleGenerator *runSmack(string input, llvm::ModulePass *actionPass) {
  string OutputFilename{};
  llvm::llvm_shutdown_obj shutdown;  // calls llvm_shutdown() on exit
  //llvm::cl::ParseCommandLineOptions(argc, argv, "SMACK - LLVM bitcode to Boogie transformation\n");

  //llvm::sys::PrintStackTraceOnErrorSignal();
  //llvm::PrettyStackTraceProgram PSTP(argc, argv);
  llvm::EnableDebugBuffering = true;

  if (OutputFilename.empty()) {
    OutputFilename = "a.bpl";
  }
 
  std::string error_msg;
  llvm::SMDiagnostic err;
  llvm::LLVMContext &context = llvm::getGlobalContext();  
  std::unique_ptr<llvm::Module> module;
  std::unique_ptr<llvm::tool_output_file> output;
 
  module.reset(llvm::ParseIRFile(StringRef(input), err, context));
  if (module.get() == 0) {
    if (llvm::errs().has_colors()) llvm::errs().changeColor(llvm::raw_ostream::RED);
    llvm::errs() << "error: " << "Bitcode was not properly read; " << err.getMessage() << "\n";
    if (llvm::errs().has_colors()) llvm::errs().resetColor();
    return nullptr;
  }
 
  output.reset(new llvm::tool_output_file(OutputFilename.c_str(), error_msg, llvm::sys::fs::F_None));
  if (!error_msg.empty()) {
    if (llvm::errs().has_colors()) llvm::errs().changeColor(llvm::raw_ostream::RED);
    llvm::errs() << "error: " << error_msg << "\n";
    if (llvm::errs().has_colors()) llvm::errs().resetColor();
    return nullptr;
  }
 
  ///////////////////////////////
  // initialise and run passes //
  ///////////////////////////////

  llvm::PassManager pass_manager;
  llvm::PassRegistry &Registry = *llvm::PassRegistry::getPassRegistry();
  llvm::initializeAnalysis(Registry);
 
  // add an appropriate DataLayout instance for the module
  const llvm::DataLayout *dl = 0;
  const std::string &moduleDataLayout = module.get()->getDataLayoutStr();
  assert (!moduleDataLayout.empty());
  dl = new llvm::DataLayout(moduleDataLayout);
  if (dl) pass_manager.add(new llvm::DataLayoutPass(*dl));

  //pass_manager.add(new smack::PruneFunctionPass());
  pass_manager.add(llvm::createAggressiveDCEPass());
  pass_manager.add(llvm::createGlobalDCEPass());
  pass_manager.add(llvm::createLowerSwitchPass());
  pass_manager.add(llvm::createCFGSimplificationPass());
  pass_manager.add(llvm::createInternalizePass());
  pass_manager.add(llvm::createPromoteMemoryToRegisterPass());
  pass_manager.add(new llvm::StructRet());
  pass_manager.add(new llvm::SimplifyEV());
  pass_manager.add(new llvm::SimplifyIV());
  pass_manager.add(new smack::SmackModuleGenerator());
  //pass_manager.add(new smack::BplFilePrinter(output->os()));
  pass_manager.add(actionPass);
  pass_manager.run(*module.get());

  output->keep();

  return nullptr;
}

} // namespace smack

