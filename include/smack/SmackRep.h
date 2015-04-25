//
// Copyright (c) 2013 Zvonimir Rakamaric (zvonimir@cs.utah.edu),
//                    Michael Emmi (michael.emmi@gmail.com)
// This file is distributed under the MIT License. See LICENSE for details.
//
#ifndef SMACKREP_H
#define SMACKREP_H

#include "smack/BoogieAst.h"
#include "smack/SmackOptions.h"
#include "smack/DSAAliasAnalysis.h"
#include "llvm/InstVisitor.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/GetElementPtrTypeIterator.h"
#include "llvm/Support/GraphWriter.h"
#include "llvm/Support/Regex.h"
#include <sstream>
#include <set>

namespace smack {

using llvm::Regex;
using llvm::SmallVector;
using llvm::StringRef;
using namespace std;
  
class SmackRep {
public:
  static const string BLOCK_LBL;
  static const string RET_VAR;
  static const string BOOL_VAR;
  static const string FLOAT_VAR;
  static const string PTR_VAR;
  static const string BOOL_TYPE;
  static const string FLOAT_TYPE;
  static const string NULL_VAL;

  static const string ALLOCA;
  static const string MALLOC;
  static const string FREE;
  static const string MEMCPY;

  static const string PTR;
  static const string OBJ;
  static const string OFF;
  static const string PA;
  
  static const string FP;
  
  static const string TRUNC;

  static const string B2P;
  static const string I2P;
  static const string P2I;
  static const string I2B;
  static const string B2I;

  static const string FP2SI;
  static const string FP2UI;
  static const string SI2FP;
  static const string UI2FP;

  static const string ADD;
  static const string SUB;
  static const string MUL;
  static const string SDIV;
  static const string UDIV;
  static const string SREM;
  static const string UREM;
  static const string AND;
  static const string OR;
  static const string XOR;
  static const string LSHR;
  static const string ASHR;
  static const string SHL;

  static const string FADD;
  static const string FSUB;
  static const string FMUL;
  static const string FDIV;
  static const string FREM;

  static const string SGE;
  static const string UGE;
  static const string SLE;
  static const string ULE;
  static const string SLT;
  static const string ULT;
  static const string SGT;
  static const string UGT;
  
  static const string NAND;
  static const string MAX;
  static const string MIN;
  static const string UMAX;
  static const string UMIN;
  
  static const string FFALSE;
  static const string FOEQ;
  static const string FOGE;
  static const string FOGT;
  static const string FOLE;
  static const string FOLT;
  static const string FONE;
  static const string FORD;
  static const string FTRUE;
  static const string FUEQ;
  static const string FUGE;
  static const string FUGT;
  static const string FULE;
  static const string FULT;
  static const string FUNE;
  static const string FUNO;
  
  static const string MEM_OP;
  static const string REC_MEM_OP;
  static const string MEM_OP_VAL;

  static const Expr* NUL;
  
  static const string STATIC_INIT;

  // TODO Make this width a parameter to generate bitvector-based code.
  static const int width;

protected:
  DSAAliasAnalysis* aliasAnalysis;
  vector<string> bplGlobals;
  vector< pair<const llvm::Value*, bool> > memoryRegions;
  std::map< const llvm::Value *, unsigned > regionOfValue;
  const llvm::DataLayout* targetData;
  Program* program;
  int globalsBottom;
  
  vector<const Stmt*> staticInits;
  
  unsigned uniqueFpNum;
  unsigned uniqueUndefNum;

public:
  SmackRep(DSAAliasAnalysis* aa)
    : aliasAnalysis(aa), targetData(aa->getDataLayout()), globalsBottom(0) {
    uniqueFpNum = 0;
    uniqueUndefNum = 0;
  }  

private:
  void addInit(unsigned region, const Expr* addr, const llvm::Constant* val);

  const Expr* pa(const Expr* base, int index, int size);
  const Expr* pa(const Expr* base, const Expr* index, int size);
  const Expr* pa(const Expr* base, const Expr* index, const Expr* size);
  
  const Expr* b2p(const llvm::Value* v);
  const Expr* i2b(const llvm::Value* v);
  const Expr* b2i(const llvm::Value* v);

public:
  void setProgram(Program* p) { program = p; }
  
  bool isSmackName(string n);
  bool isSmackGeneratedName(string n);
  bool isMallocOrFree(llvm::Function* f);
  bool isIgnore(llvm::Function* f);
  bool isInt(const llvm::Type* t);
  bool isInt(const llvm::Value* v);
  bool isBool(llvm::Type* t);
  bool isBool(const llvm::Value* v);
  bool isFloat(llvm::Type* t);
  bool isFloat(llvm::Value* v);

  unsigned storageSize(llvm::Type* t);
  unsigned fieldOffset(llvm::StructType* t, unsigned fieldNo);
  
  unsigned getRegion(const llvm::Value* v);
  string memReg(unsigned i);
  bool isExternal(const llvm::Value* v);
  void collectRegions(llvm::Module &M);

  virtual string type(llvm::Type* t);
  virtual string type(llvm::Value* v);
  
  const Expr* mem(const llvm::Value* v);
  const Expr* mem(unsigned region, const Expr* addr);  

  string id(const llvm::Value* v);
  const Expr* undef();
  const Expr* lit(const llvm::Value* v);
  const Expr* lit(unsigned v);
  const Expr* ptrArith(const llvm::Value* p, vector<llvm::Value*> ps,
                       vector<llvm::Type*> ts);
  const Expr* expr(const llvm::Value* v);
  string getString(const llvm::Value* v);
  const Expr* op(const llvm::User* v);
  const Expr* pred(llvm::CmpInst& ci);
  
  const Expr* arg(llvm::Function* f, unsigned pos, llvm::Value* v);
  const Stmt* call(llvm::Function* f, llvm::CallInst& ci);
  string code(llvm::CallInst& ci);
  ProcDecl* proc(llvm::Function* f, int n);
  
  virtual const Expr* trunc(const llvm::Value* v, llvm::Type* t);
  virtual const Expr* zext(const llvm::Value* v, llvm::Type* t);
  virtual const Expr* sext(const llvm::Value* v, llvm::Type* t);
  virtual const Expr* fptrunc(const llvm::Value* v, llvm::Type* t);
  virtual const Expr* fpext(const llvm::Value* v, llvm::Type* t);
  virtual const Expr* fp2ui(const llvm::Value* v);
  virtual const Expr* fp2si(const llvm::Value* v);
  virtual const Expr* ui2fp(const llvm::Value* v);
  virtual const Expr* si2fp(const llvm::Value* v);
  virtual const Expr* p2i(const llvm::Value* v);
  virtual const Expr* i2p(const llvm::Value* v);
  virtual const Expr* bitcast(const llvm::Value* v, llvm::Type* t);

  virtual const Stmt* alloca(llvm::AllocaInst& i);
  virtual const Stmt* memcpy(const llvm::MemCpyInst& msi);
  virtual const Stmt* memset(const llvm::MemSetInst& msi);
  
  virtual vector<Decl*> globalDecl(const llvm::Value* g);
  virtual void addBplGlobal(string name);
  virtual vector<string> getModifies();
  unsigned numElements(const llvm::Constant* v);
  void addInit(unsigned region, const llvm::Value* addr, const llvm::Constant* val);
  bool hasStaticInits();
  Decl* getStaticInit();
  virtual string getPtrType();
  virtual string getPrelude();

  virtual const Expr* declareIsExternal(const Expr* e);

  virtual string memcpyProc(int dstReg, int srcReg);
  virtual string memsetProc(int dstReg);
};

class RegionCollector : public llvm::InstVisitor<RegionCollector> {
private:
  SmackRep& rep;

public:
  RegionCollector(SmackRep& r) : rep(r) {}
  void visitAllocaInst(llvm::AllocaInst& i) { rep.getRegion(&i); }
  void visitCallInst(llvm::CallInst& i) {
    if (i.getType()->isPointerTy())
      rep.getRegion(&i);
  }
};

}

#endif // SMACKREP_H

