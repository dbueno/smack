#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/CallSite.h"
#include "llvm/ADT/DepthFirstIterator.h"
#include "smack/PruneFunctionPass.h"
#include "llvm/IR/InstIterator.h"

#include <queue>
#include <stack>

using namespace llvm;

namespace smack {
bool PruneFunctionPass::runOnModule(Module &M) {
    errs() << "Hello: ";
    errs().write_escaped(M.getModuleIdentifier()) << '\n';
    bool changed = false;

    // Beginning with main, walk through called functions, recording
    // sink functions.
    std::set<Function*> critical = std::set<Function*>();
    critical.insert(M.getFunction("SSL_library_init"));
    critical.insert(M.getFunction("SSL_CTX_new"));
    critical.insert(M.getFunction("SSL_CTX_ctrl"));
    critical.insert(M.getFunction("SSL_CTX_set_verify"));
    
    std::set<Function*> keepers = std::set<Function*>(); // Functions we'll keep
    for (std::set<Function*>::iterator it = critical.begin(), ie = critical.end(); it != ie; ++it)
        keepers.insert(*it);
    std::set<Function*> seen = std::set<Function*>(); // Any function we've processed
   
    Function *main = M.getFunction("main");

    // Iterative depth first search for paths to critical functions
    std::vector<Function*> stack = std::vector<Function*>();
    stack.push_back(main), seen.insert(main);
    while (stack.size() > 0) {
        Function *F = stack.back();
        for (inst_iterator ii = inst_begin(F), ie = inst_end(F); ii != ie; ++ii) {
            // TODO convert to CallSite
            if (CallInst* callInst = dyn_cast<CallInst>(&*ii)) {
                Function *callee = callInst->getCalledFunction();
                if (!callee) { // Indirect function invocation
                    //errs() << "Warning: Skipping indirect function call: " << *callInst->getCalledValue() << "\n";
                    continue; // just skip it for now
                }
                if (seen.find(callee) != seen.end()) // is seen
                    continue;
                seen.insert(callee);
                if (keepers.find(callee) != keepers.end()) { // reaches a keeper
                    for (std::vector<Function*>::iterator it = stack.begin(), ie = stack.end(); it != ie; ++it)
                        keepers.insert(*it);
                } else {
                    stack.push_back(callee);
                    goto done; // DFS, so immediately explore new unseen callee
                }
            }
        }
        stack.pop_back();
done:
        ;
    }
    //errs() << "Keepers:\n";
    //for (std::set<Function *>::iterator it = keepers.begin(), ie = keepers.end(); it != ie; ++it) {
    //    errs() << (*it)->getName() << "\n";
    //}
    for (Module::iterator it = M.begin(), ie = M.end(); it != ie; ++it) {
        if (keepers.find(it) == keepers.end()) { // not a keeper
            it->deleteBody();
            changed = true;
        }
    }

    errs() << "\n";
    return changed;
}
}

char smack::PruneFunctionPass::ID = 0;
static RegisterPass<smack::PruneFunctionPass> X("prune-nonssl-funcs", "Prune functions that don't reach SSL critical one", false, false);
