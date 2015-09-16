#ifndef PRUNEFUNCTIONPASS_H
#define PRUNEFUNCTIONPASS_H

#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/CallSite.h"
#include "llvm/ADT/DepthFirstIterator.h"
#include "smack/PruneFunctionPass.h"

using namespace llvm;

namespace smack {
    class PruneFunctionPass : public ModulePass {
    public:
        static char ID;
        PruneFunctionPass() : ModulePass(ID) {}
        bool runOnModule(Module &M) override;
    };
}

#endif
