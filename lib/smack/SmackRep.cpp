//
// Copyright (c) 2013 Zvonimir Rakamaric (zvonimir@cs.utah.edu),
//                    Michael Emmi (michael.emmi@gmail.com)
// This file is distributed under the MIT License. See LICENSE for details.
//
#include "smack/SmackRep.h"
#include "smack/SmackOptions.h"

#include <iostream>

namespace smack {

const string SmackRep::BLOCK_LBL = "$bb";
const string SmackRep::RET_VAR = "$r";
const string SmackRep::BOOL_VAR = "$b";
const string SmackRep::FLOAT_VAR = "$f";
const string SmackRep::PTR_VAR = "$p";
const string SmackRep::BOOL_TYPE = "bool";
const string SmackRep::FLOAT_TYPE = "float";
const string SmackRep::NULL_VAL = "$NULL";

const string SmackRep::ALLOCA = "$alloca";
const string SmackRep::MALLOC = "$malloc";
const string SmackRep::FREE = "$free";
const string SmackRep::MEMCPY = "$memcpy";

const string SmackRep::PTR = "$ptr";
const string SmackRep::OBJ = "$obj";
const string SmackRep::OFF = "$off";
const string SmackRep::PA = "$pa";

const string SmackRep::FP = "$fp";

const string SmackRep::TRUNC = "$trunc";

const string SmackRep::B2P = "$b2p";
const string SmackRep::I2P = "$i2p";
const string SmackRep::P2I = "$p2i";
const string SmackRep::I2B = "$i2b";
const string SmackRep::B2I = "$b2i";

const string SmackRep::FP2SI = "$fp2si";
const string SmackRep::FP2UI = "$fp2ui";
const string SmackRep::SI2FP = "$si2fp";
const string SmackRep::UI2FP = "$ui2fp";

const string SmackRep::ADD = "$add";
const string SmackRep::SUB = "$sub";
const string SmackRep::MUL = "$mul";
const string SmackRep::SDIV = "$sdiv";
const string SmackRep::UDIV = "$udiv";
const string SmackRep::SREM = "$srem";
const string SmackRep::UREM = "$urem";
const string SmackRep::AND = "$and";
const string SmackRep::OR = "$or";
const string SmackRep::XOR = "$xor";
const string SmackRep::LSHR = "$lshr";
const string SmackRep::ASHR = "$ashr";
const string SmackRep::SHL = "$shl";

const string SmackRep::NAND = "$nand";
const string SmackRep::MAX = "$max";
const string SmackRep::MIN = "$min";
const string SmackRep::UMAX = "$umax";
const string SmackRep::UMIN = "$umin";

const string SmackRep::FADD = "$fadd";
const string SmackRep::FSUB = "$fsub";
const string SmackRep::FMUL = "$fmul";
const string SmackRep::FDIV = "$fdiv";
const string SmackRep::FREM = "$frem";

const string SmackRep::SGE = "$sge";
const string SmackRep::UGE = "$uge";
const string SmackRep::SLE = "$sle";
const string SmackRep::ULE = "$ule";
const string SmackRep::SLT = "$slt";
const string SmackRep::ULT = "$ult";
const string SmackRep::SGT = "$sgt";
const string SmackRep::UGT = "$ugt";

const string SmackRep::FFALSE = "$ffalse";
const string SmackRep::FOEQ = "$foeq";
const string SmackRep::FOGE = "$foge";
const string SmackRep::FOGT = "$fogt";
const string SmackRep::FOLE = "$fole";
const string SmackRep::FOLT = "$folt";
const string SmackRep::FONE = "$fone";
const string SmackRep::FORD = "$ford";
const string SmackRep::FTRUE = "$ftrue";
const string SmackRep::FUEQ = "$fueq";
const string SmackRep::FUGE = "$fuge";
const string SmackRep::FUGT = "$fugt";
const string SmackRep::FULE = "$fule";
const string SmackRep::FULT = "$fult";
const string SmackRep::FUNE = "$fune";
const string SmackRep::FUNO = "$funo";

// used for memory model debugging
const string SmackRep::MEM_OP = "$mop";
const string SmackRep::REC_MEM_OP = "boogie_si_record_mop";
const string SmackRep::MEM_OP_VAL = "$MOP";

const Expr* SmackRep::NUL = Expr::id(NULL_VAL);

const string SmackRep::STATIC_INIT = "$static_init";

const int SmackRep::width = 0;

// TODO Do the following functions belong here ?

string EscapeString(string s) {
  string Str(llvm::DOT::EscapeString(s));
  for (unsigned i = 0; i != Str.length(); ++i)
    switch (Str[i]) {
    case '\01':
      Str[i] = '_';
      break;
    case '@':
      Str[i] = '.';
      break;
    }
  return Str;
}

Regex BPL_KW(
  "^(bool|int|false|true|old|forall|exists|requires|modifies|ensures|invariant|free"
  "|unique|finite|complete|type|const|function|axiom|var|procedure"
  "|implementation|where|returns|assume|assert|havoc|call|return|while"
  "|break|goto|if|else|div)$");
Regex SMACK_NAME(".*__SMACK_.*");
Regex PROC_MALLOC_FREE("^(malloc|free_)$");
Regex PROC_IGNORE("^("
  "llvm\\.memcpy\\..*|llvm\\.memset\\..*|llvm\\.dbg\\..*|"
  "__SMACK_code|__SMACK_decl|__SMACK_top_decl"
")$");

bool isBplKeyword(string s) {
  return BPL_KW.match(s);
}

bool SmackRep::isSmackName(string n) {
  return SMACK_NAME.match(n);
}

bool SmackRep::isSmackGeneratedName(string n) {
  return n.size() > 0 && n[0] == '$';
}

bool SmackRep::isMallocOrFree(llvm::Function* f) {
  return PROC_MALLOC_FREE.match(id(f));
}

bool SmackRep::isIgnore(llvm::Function* f) {  
  return PROC_IGNORE.match(id(f));
}

bool SmackRep::isInt(const llvm::Type* t) {
  return t->isIntegerTy();
}

bool SmackRep::isInt(const llvm::Value* v) {
  return isInt(v->getType());
}

bool SmackRep::isBool(llvm::Type* t) {
  return t->isIntegerTy(1);
}

bool SmackRep::isBool(const llvm::Value* v) {
  return isBool(v->getType());
}

bool SmackRep::isFloat(llvm::Type* t) {
  return t->isFloatingPointTy();
}

bool SmackRep::isFloat(llvm::Value* v) {
  return isFloat(v->getType());
}

string SmackRep::type(llvm::Type* t) {
  if (isBool(t))
    return BOOL_TYPE;
  else if (isFloat(t))
    return FLOAT_TYPE;
  else
    return getPtrType();
}

string SmackRep::type(llvm::Value* v) {
  return type(v->getType());
}

unsigned SmackRep::storageSize(llvm::Type* t) {
  return targetData->getTypeStoreSize(t);
}

unsigned SmackRep::fieldOffset(llvm::StructType* t, unsigned fieldNo) {
  return targetData->getStructLayout(t)->getElementOffset(fieldNo);
}

string SmackRep::memReg(unsigned idx) {
  stringstream s;
  s << "$M." << idx;
  return s.str();
}

const Expr* SmackRep::mem(const llvm::Value* v) {
  return mem( getRegion(v), expr(v) );
}

const Expr* SmackRep::mem(unsigned region, const Expr* addr) {
  return Expr::sel( Expr::id(memReg(region)), addr );
}

unsigned SmackRep::getRegion(const llvm::Value* v) {
  unsigned r;

  for (r=0; r<memoryRegions.size(); ++r)
    if (!aliasAnalysis->isNoAlias(v, memoryRegions[r].first))
      break;

  if (r == memoryRegions.size())
    memoryRegions.push_back(make_pair(v,false));

  memoryRegions[r].second = memoryRegions[r].second || aliasAnalysis->isAlloced(v);

  regionOfValue[v] = r;

  return r;
}

bool SmackRep::isExternal(const llvm::Value* v) {
  return v->getType()->isPointerTy() && !memoryRegions[getRegion(v)].second;
}

void SmackRep::collectRegions(llvm::Module &M) {
  RegionCollector rc(*this);

  for (llvm::Module::iterator func = M.begin(), e = M.end();
       func != e; ++func) {

    for (llvm::Function::iterator block = func->begin();
        block != func->end(); ++block) {

      rc.visit(*block);
    }
  }
}

const Expr* SmackRep::trunc(const llvm::Value* v, llvm::Type* t) {
  assert(t->isIntegerTy() && "TODO: implement truncate for non-integer types.");
  const Expr* e = expr(v);

  return isBool(t)
    ? Expr::fn(I2B,e)
    : Expr::fn(TRUNC,e,lit(t->getPrimitiveSizeInBits()));
}
const Expr* SmackRep::zext(const llvm::Value* v, llvm::Type* t) {
  return isBool(v->getType()) ? b2p(v) : expr(v);
}
const Expr* SmackRep::sext(const llvm::Value* v, llvm::Type* t) {
  return isBool(v->getType()) ? b2p(v) : expr(v);
}
const Expr* SmackRep::fptrunc(const llvm::Value* v, llvm::Type* t) {
  return expr(v);  
}
const Expr* SmackRep::fpext(const llvm::Value* v, llvm::Type* t) {
  return expr(v);
}
const Expr* SmackRep::fp2ui(const llvm::Value* v) {
  return Expr::fn(FP2UI, expr(v));
}
const Expr* SmackRep::fp2si(const llvm::Value* v) {
  return Expr::fn(FP2SI, expr(v));
}
const Expr* SmackRep::ui2fp(const llvm::Value* v) {
  return Expr::fn(UI2FP, expr(v));
}
const Expr* SmackRep::si2fp(const llvm::Value* v) {
  return Expr::fn(SI2FP, expr(v));
}
const Expr* SmackRep::p2i(const llvm::Value* v) {
  return Expr::fn(P2I, expr(v));
}
const Expr* SmackRep::i2p(const llvm::Value* v) {
  return Expr::fn(I2P, expr(v));
}
const Expr* SmackRep::bitcast(const llvm::Value* v, llvm::Type* t) {
  return expr(v);
}

const Stmt* SmackRep::alloca(llvm::AllocaInst& i) {  
  const Expr* size = 
    Expr::fn(MUL,lit(storageSize(i.getAllocatedType())),lit(i.getArraySize()));
                       
  return Stmt::call(ALLOCA,size,id(&i));
}

const Stmt* SmackRep::memcpy(const llvm::MemCpyInst& mci) {
  int dstRegion = getRegion(mci.getOperand(0));
  int srcRegion = getRegion(mci.getOperand(1));

  program->addDecl(memcpyProc(dstRegion,srcRegion));

  stringstream name;
  name << "$memcpy." << dstRegion << "." << srcRegion;
  vector<const Expr*> args;
  for (unsigned i = 0; i < mci.getNumOperands() - 1; i++)
    args.push_back(expr(mci.getOperand(i)));
  return Stmt::call(name.str(),args);
}

const Stmt* SmackRep::memset(const llvm::MemSetInst& msi) {
  int region = getRegion(msi.getOperand(0));

  program->addDecl(memsetProc(region));

  stringstream name;
  vector<const Expr*> args;
  name << "$memset." << region;
  for (unsigned i = 0; i < msi.getNumOperands() - 1; i++)
    args.push_back(expr(msi.getOperand(i)));
  return Stmt::call(name.str(),args);
}

const Expr* SmackRep::pa(const Expr* base, int index, int size) {
  return pa(base, Expr::lit(index), Expr::lit(size));
}
const Expr* SmackRep::pa(const Expr* base, const Expr* index, int size) {
  return pa(base, index, Expr::lit(size));
}
const Expr* SmackRep::pa(const Expr* base, const Expr* index, const Expr* size) {
  return Expr::fn(PA, base, index, size);
}
const Expr* SmackRep::b2p(const llvm::Value* v) {
  return Expr::fn(B2P, expr(v));
}
const Expr* SmackRep::i2b(const llvm::Value* v) {
  return Expr::fn(I2B, expr(v));
}
const Expr* SmackRep::b2i(const llvm::Value* v) {
  return Expr::fn(B2I, expr(v));
}

const Expr* SmackRep::undef() {
  stringstream s;
  s << "$u." << uniqueUndefNum++;
  return Expr::id(s.str());
}

string SmackRep::id(const llvm::Value* v) {
  string name;

  if (v->hasName()) {
    name = v->getName().str();

  } else {
    assert(false && "expected named value.");

    // OLD NAME-HANDLING CODE
    // llvm::raw_string_ostream ss(name);
    // ss << *v;
    // name = name.substr(name.find("%"));
    // name = name.substr(0, name.find(" "));
  }
  name = EscapeString(name);

  if (isBplKeyword(name))
    name = name + "_";

  return name;
}

const Expr* SmackRep::lit(const llvm::Value* v) {
  if (const llvm::ConstantInt* ci = llvm::dyn_cast<const llvm::ConstantInt>(v)) {
    if (ci->getBitWidth() == 1)
      return Expr::lit(!ci->isZero());

    uint64_t val = ci->getLimitedValue();
    if (width > 0 && ci->isNegative())
      return Expr::fn(SUB, Expr::lit(0, width), Expr::lit(-val, width));
    else
      return Expr::lit(val, width);

  } else if (llvm::isa<const llvm::ConstantFP>(v)) {

    // TODO encode floating point
    return Expr::fn(FP,Expr::lit((int) uniqueFpNum++));

  } else if (llvm::isa<llvm::ConstantPointerNull>(v))
    return Expr::lit(0, width);

  else
    return expr(v);
  // assert( false && "value type not supported" );
}

const Expr* SmackRep::lit(unsigned v) {
  // TODO why doesn't this one do the thing with negative as well?
  return Expr::lit(v, width);
}

const Expr* SmackRep::ptrArith(
  const llvm::Value* p, vector<llvm::Value*> ps, vector<llvm::Type*> ts) {

  assert(ps.size() > 0 && ps.size() == ts.size());

  const Expr* e = expr(p);

  for (unsigned i = 0; i < ps.size(); i++) {
    if (llvm::StructType* st = llvm::dyn_cast<llvm::StructType>(ts[i])) {

      assert(ps[i]->getType()->isIntegerTy()
             && ps[i]->getType()->getPrimitiveSizeInBits() == 32
             && "Illegal struct idx");

      // Get structure layout information...
      unsigned fieldNo =
        llvm::cast<llvm::ConstantInt>(ps[i])->getZExtValue();

      // Add in the offset, as calculated by the
      // structure layout info...
      e = pa(e, fieldOffset(st, fieldNo), 1);

    } else {
      llvm::Type* et =
        llvm::cast<llvm::SequentialType>(ts[i])->getElementType();
      e = pa(e, lit(ps[i]), storageSize(et));
    }
  }

  return e;
}

const Expr* SmackRep::expr(const llvm::Value* v) {
  using namespace llvm;

  if (const GlobalValue* g = dyn_cast<const GlobalValue>(v)) {
    assert(g->hasName());
    return Expr::id(id(v));

  } else if (v->hasName())
    return Expr::id(id(v));

  else if (const Constant* constant = dyn_cast<const Constant>(v)) {

    if (const ConstantExpr* constantExpr = dyn_cast<const ConstantExpr>(constant)) {

      if (constantExpr->getOpcode() == Instruction::GetElementPtr) {

        vector<llvm::Value*> ps;
        vector<llvm::Type*> ts;
        llvm::gep_type_iterator typeI = gep_type_begin(constantExpr);
        for (unsigned i = 1; i < constantExpr->getNumOperands(); i++, ++typeI) {
          ps.push_back(constantExpr->getOperand(i));
          ts.push_back(*typeI);
        }
        return ptrArith(constantExpr->getOperand(0), ps, ts);

      } else if (constantExpr->getOpcode() == Instruction::BitCast)

        // TODO: currently this is a noop instruction
        return expr(constantExpr->getOperand(0));

      else if (constantExpr->getOpcode() == Instruction::IntToPtr)

        // TODO test this out, formerly Expr::id("$UNDEF");
        return i2p(constantExpr->getOperand(0));

      else if (constantExpr->getOpcode() == Instruction::PtrToInt)

        // TODO test this out, formerly Expr::id("$UNDEF");
        return p2i(constantExpr->getOperand(0));

      else if (Instruction::isBinaryOp(constantExpr->getOpcode()))
        return op(constantExpr);

      else {
        DEBUG(errs() << "VALUE : " << *v << "\n");
        assert(false && "constant expression of this type not supported");
      }

    } else if (const ConstantInt* ci = dyn_cast<const ConstantInt>(constant)) {
      if (ci->getBitWidth() == 1)
        return Expr::lit(!ci->isZero());

      else return lit(ci);
      
    } else if (const ConstantFP* cf = dyn_cast<const ConstantFP>(constant)) {
      return lit(cf);

    } else if (constant->isNullValue())
      return lit((unsigned)0);

    else if (isa<UndefValue>(constant))
      return undef();

    else {
      DEBUG(errs() << "VALUE : " << *v << "\n");
      assert(false && "this type of constant not supported");
    }

  } else {
    DEBUG(errs() << "VALUE : " << *v << "\n");
    assert(false && "value of this type not supported");
  }
}

string SmackRep::getString(const llvm::Value* v) {
  if (const llvm::ConstantExpr* constantExpr = llvm::dyn_cast<const llvm::ConstantExpr>(v))
    if (constantExpr->getOpcode() == llvm::Instruction::GetElementPtr)
      if (const llvm::GlobalValue* cc = llvm::dyn_cast<const llvm::GlobalValue>(constantExpr->getOperand(0)))
        if (const llvm::ConstantDataSequential* cds = llvm::dyn_cast<const llvm::ConstantDataSequential>(cc->getOperand(0)))
            return cds ->getAsCString();
  return "";
}

const Expr* SmackRep::op(const llvm::User* v) {
  using namespace llvm;
  unsigned opcode;
  string op;

  if (const BinaryOperator* bo = dyn_cast<const BinaryOperator>(v))
    opcode = bo->getOpcode();

  else if (const ConstantExpr* ce = dyn_cast<const ConstantExpr>(v))
    opcode = ce->getOpcode();

  else assert(false && "unexpected operator value.");

  switch (opcode) {
    using llvm::Instruction;

  // Integer operations
  case Instruction::Add:
    op = ADD;
    break;
  case Instruction::Sub:
    op = SUB;
    break;
  case Instruction::Mul:
    op = MUL;
    break;
  case Instruction::SDiv:
    op = SDIV;
    break;
  case Instruction::UDiv:
    op = UDIV;
    break;
  case Instruction::SRem:
    op = SREM;
    break;
  case Instruction::URem:
    op = UREM;
    break;
  case Instruction::And:
    op = AND;
    break;
  case Instruction::Or:
    op = OR;
    break;
  case Instruction::Xor:
    op = XOR;
    break;
  case Instruction::LShr:
    op = LSHR;
    break;
  case Instruction::AShr:
    op = ASHR;
    break;
  case Instruction::Shl:
    op = SHL;
    break;

  // Floating point operations
  case Instruction::FAdd:
    op = FADD;
    break;
  case Instruction::FSub:
    op = FSUB;
    break;
  case Instruction::FMul:
    op = FMUL;
    break;
  case Instruction::FDiv:
    op = FDIV;
    break;
  case Instruction::FRem:
    op = FREM;
    break;
      
  default:
    assert(false && "unexpected predicate.");
  }
  llvm::Value
  *l = v->getOperand(0),
   *r = v->getOperand(1);

  const Expr* e = Expr::fn(op,
                           (isBool(l) ? b2i(l) : expr(l)),
                           (isBool(r) ? b2i(r) : expr(r)));

  return isBool(v) ? Expr::fn("$i2b",e) : e;
}

const Expr* SmackRep::pred(llvm::CmpInst& ci) {
  const Expr* e = NULL;
  string o;
  const Expr
  *l = expr(ci.getOperand(0)),
   *r = expr(ci.getOperand(1));

  switch (ci.getPredicate()) {
    using llvm::CmpInst;

  // integer comparison
  case CmpInst::ICMP_EQ:
    e = Expr::eq(l, r);
    break;
  case CmpInst::ICMP_NE:
    e = Expr::neq(l, r);
    break;
  case CmpInst::ICMP_SGE:
    o = SGE;
    break;
  case CmpInst::ICMP_UGE:
    o = UGE;
    break;
  case CmpInst::ICMP_SLE:
    o = SLE;
    break;
  case CmpInst::ICMP_ULE:
    o = ULE;
    break;
  case CmpInst::ICMP_SLT:
    o = SLT;
    break;
  case CmpInst::ICMP_ULT:
    o = ULT;
    break;
  case CmpInst::ICMP_SGT:
    o = SGT;
    break;
  case CmpInst::ICMP_UGT:
    o = UGT;
    break;

  // floating point comparison 
  case CmpInst::FCMP_FALSE:
    o = FFALSE;
    break;
  case CmpInst::FCMP_OEQ:
    o = FOEQ;
    break;
  case CmpInst::FCMP_OGE:
    o = FOGE;
    break;
  case CmpInst::FCMP_OGT:
    o = FOGT;
    break;
  case CmpInst::FCMP_OLE:
    o = FOLE;
    break;
  case CmpInst::FCMP_OLT:
    o = FOLT;
    break;
  case CmpInst::FCMP_ONE:
    o = FONE;
    break;
  case CmpInst::FCMP_ORD:
    o = FORD;
    break;
  case CmpInst::FCMP_TRUE:
    o = FTRUE;
    break;
  case CmpInst::FCMP_UEQ:
    o = FUEQ;
    break;
  case CmpInst::FCMP_UGE:
    o = FUGE;
    break;
  case CmpInst::FCMP_UGT:
    o = FUGT;
    break;
  case CmpInst::FCMP_ULE:
    o = FULE;
    break;
  case CmpInst::FCMP_ULT:
    o = FULT;
    break;
  case CmpInst::FCMP_UNE:
    o = FUNE;
    break;
  case CmpInst::FCMP_UNO:
    o = FUNO;
    break;
  default:
    assert(false && "unexpected predicate.");
  }

  return e == NULL ? Expr::fn(o, l, r) : e;
}

string indexedName(string name, int idx) {
  stringstream idxd;
  idxd << name << "#" << idx;
  return idxd.str();
}

ProcDecl* SmackRep::proc(llvm::Function* f, int nargs) {
  vector< pair<string,string> > args, rets;

  int i = 0;
  for (llvm::Function::arg_iterator
       arg = f->arg_begin(), e = f->arg_end(); arg != e; ++arg, ++i) {
    string name;
    if (arg->hasName()) {
      name = id(arg);
    } else {
      name = indexedName("p",i);
      arg->setName(name);
    }
    
    args.push_back(make_pair(name, type(arg->getType()) ));
  }
  
  for (; i < nargs; i++)
    args.push_back(make_pair(indexedName("p",i), getPtrType()));

  if (!f->getReturnType()->isVoidTy())
    rets.push_back(make_pair(RET_VAR,type(f->getReturnType())));

  return (ProcDecl*) Decl::procedure(
    *program,
    f->isVarArg() ? indexedName(id(f),nargs) : id(f), 
    args, 
    rets
  );
}

const Expr* SmackRep::arg(llvm::Function* f, unsigned pos, llvm::Value* v) {
  return (f && f->isVarArg() && isFloat(v)) ? fp2si(v) : expr(v);
}

const Stmt* SmackRep::call(llvm::Function* f, llvm::CallInst& ci) {

  assert(f && "Call encountered unresolved function.");
  
  string name = id(f);
  vector<const Expr*> args;
  vector<string> rets;
  
  for (unsigned i = 0; i < ci.getNumOperands() - 1; i++)
    args.push_back(arg(f, i, ci.getOperand(i)));
  
  if (!ci.getType()->isVoidTy())
    rets.push_back(id(&ci));

  if (name == "malloc") {
    assert(args.size() == 1);
    return Stmt::call(MALLOC, args[0], rets[0]);

  } else if (name == "free_") {
    assert(args.size() == 1);
    return Stmt::call(FREE, args[0]);

  } else if (f->isVarArg() || (f->isDeclaration() && !isSmackName(name))) {
    
    Decl* p = proc(f,args.size());
    program->addDecl(p);
    return Stmt::call(p->getName(), args, rets);
    
  } else {
    return Stmt::call(name, args, rets);
  }
}

string SmackRep::code(llvm::CallInst& ci) {
  
  llvm::Function* f = ci.getCalledFunction();
  assert(f && "Inline code embedded in unresolved function.");
  
  string fmt = getString(ci.getOperand(0));
  assert(!fmt.empty() && "__SMACK_code: missing format string.");
  
  string s = fmt;
  for (unsigned i=1; i<ci.getNumOperands()-1; i++) {
    const Expr* a = arg(f, i, ci.getOperand(i));
    string::size_type idx = s.find('@');
    assert(idx != string::npos && "__SMACK_code: too many arguments.");
    
    ostringstream ss;

    if (s.find("{@}") == idx-1) {
      a->print(ss);
      s = s.replace(idx-1,3,ss.str());
      
    } else {
      a->print(ss);
      s = s.replace(idx,1,ss.str());
    }      
  }
  return s;
}

string SmackRep::getPrelude() {
  ostringstream s;
  s << endl;
  s << "# Memory region declarations";
  s << ": " << memoryRegions.size() << endl;
  for (unsigned i=0; i<memoryRegions.size(); ++i) {
    s << "add_memory_region(\"" << memReg(i) 
      << "\", \"" << getPtrType() << "\", \"" << getPtrType() << "\")" << endl;
  }
  for (std::map<const llvm::Value*, unsigned >::iterator
        IT = regionOfValue.begin(), IE = regionOfValue.end(); IT != IE; ++IT) {
      const llvm::Value *v = IT->first;
      const Expr *e = expr(v);
      s << "add_ptr_to_region(\"" << memReg(IT->second) << "\", ";
      e->print(s);
      s << ")" << endl;
  }
  s << endl;

  if (uniqueUndefNum > 0) {
    s << "# Undefined values" << endl;
    for (unsigned i=0; i<uniqueUndefNum; i++)
      s << "add_undef('$u." << i << "', '" << getPtrType() << "')" << endl;  
    s << endl;
  }

  s << "add_constant(\"$GLOBALS_BOTTOM\", \"int\")\n";

  s << "add_axiom(Eq(\"$GLOBALS_BOTTOM\", Num(" << globalsBottom << ")))" << endl;

  return s.str();
}

void SmackRep::addBplGlobal(string name) {
  bplGlobals.push_back(name);
}

vector<string> SmackRep::getModifies() {
  vector<string> mods;
  for (vector<string>::iterator i = bplGlobals.begin(); i != bplGlobals.end(); ++i)
    mods.push_back(*i);
  for (unsigned i=0; i<memoryRegions.size(); ++i)
    mods.push_back(memReg(i));
  return mods;
}

unsigned SmackRep::numElements(const llvm::Constant* v) {
  using namespace llvm;
  if (const ArrayType* at = dyn_cast<const ArrayType>(v->getType()))
    return at->getNumElements();
  else
    return 1;
}

void SmackRep::addInit(unsigned region, const llvm::Value* addr, const llvm::Constant* val) {
  addInit(region, expr(addr), val);
}

void SmackRep::addInit(unsigned region, const Expr* addr, const llvm::Constant* val) {
  using namespace llvm;

  if (isInt(val)) {
    staticInits.push_back( Stmt::assign(mem(region,addr), expr(val)) );
    
  } else if (isa<PointerType>(val->getType())) {
    staticInits.push_back( Stmt::assign(mem(region,addr), expr(val)) );

  } else if (ArrayType* at = dyn_cast<ArrayType>(val->getType())) {

    for (unsigned i = 0; i < at->getNumElements(); i++) {
      const Constant* elem = val->getAggregateElement(i);
      addInit( region, pa(addr,i,storageSize(at->getElementType())), elem );
    }

  } else if (StructType* st = dyn_cast<StructType>(val->getType())) {
    for (unsigned i = 0; i < st->getNumElements(); i++) {
      const Constant* elem = val->getAggregateElement(i);
      addInit( region, pa(addr,fieldOffset(st,i),1), elem );
    }

  } else {
    errs() << "addr: " << addr << " val: " << val << ", " << val->getType() << "\n";
    assert (false && "Unexpected static initializer.");
  }
}

bool SmackRep::hasStaticInits() {
  return staticInits.size() > 0;
}

Decl* SmackRep::getStaticInit() {
  ProcDecl* proc = (ProcDecl*) Decl::procedure(*program, STATIC_INIT);
  Block* b = new Block(*proc);
  for (unsigned i=0; i<staticInits.size(); i++)
    b->addStmt(staticInits[i]);
  b->addStmt(Stmt::return_());
  proc->addBlock(b);
  return proc;
}
Regex STRING_CONSTANT("^\\.str[0-9]*$");

vector<Decl*> SmackRep::globalDecl(const llvm::Value* v) {
  using namespace llvm;
  vector<Decl*> decls;
  vector<const Attr*> ax;
  string name = id(v);

  if (const GlobalVariable* g = dyn_cast<const GlobalVariable>(v)) {
    if (g->hasInitializer()) {
      const Constant* init = g->getInitializer();
      unsigned numElems = numElements(init);
      unsigned size;

      // NOTE: all global variables have pointer type in LLVM
      if (g->getType()->isPointerTy()) {
        PointerType *t = (PointerType*) g->getType();

        // in case we can determine the size of the element type ...
        if (t->getElementType()->isSized())
          size = storageSize(t->getElementType());

        // otherwise (e.g. for function declarations), use a default size
        else
          size = 1024;

      } else
        size = storageSize(g->getType());

      globalsBottom -= size;

      if (!g->hasName() || !STRING_CONSTANT.match(g->getName().str())) {
        if (numElems > 1)
          ax.push_back(Attr::attr("count",numElems));

        decls.push_back(Decl::axiom(Expr::eq(Expr::id(name),Expr::lit(globalsBottom))));
        addInit(getRegion(g), g, init);

        // Expr::fn("$slt",
        //     Expr::fn(SmackRep::ADD, Expr::id(name), Expr::lit(1024)),
        //     Expr::lit(globalsBottom)) ));
      } else if (STRING_CONSTANT.match(g->getName().str())) {
          addInit(getRegion(g), g, init);
      }

    } else {
      decls.push_back(Decl::axiom(declareIsExternal(Expr::id(name))));
    }
  }
  decls.push_back(Decl::constant(name, getPtrType(), ax, true));
  return decls;
}

const Expr* SmackRep::declareIsExternal(const Expr* e) {
  return Expr::fn("$isExternal",e);
}

string SmackRep::getPtrType() {
  return "int";
}

string SmackRep::memcpyProc(int dstReg, int srcReg) {
  stringstream s;

  if (SmackOptions::MemoryModelImpls) {
    s << "procedure $memcpy." << dstReg << "." << srcReg;
    s << "(dest: int, src: int, len: int, align: int, isvolatile: bool)" << endl;
    s << "modifies " << memReg(dstReg) << ";" << endl;
    s << "{" << endl;
    s << "  var $oldSrc: [" << getPtrType() << "] " << getPtrType() << ";" << endl;
    s << "  var $oldDst: [" << getPtrType() << "] " << getPtrType() << ";" << endl;
    s << "  $oldSrc := " << memReg(srcReg) << ";" << endl;
    s << "  $oldDst := " << memReg(dstReg) << ";" << endl;
    s << "  havoc " << memReg(dstReg) << ";" << endl;
    s << "  assume (forall x:int :: dest <= x && x < dest + len ==> "
      << memReg(dstReg) << "[x] == $oldSrc[src - dest + x]);" << endl;
    s << "  assume (forall x:int :: !(dest <= x && x < dest + len) ==> "
      << memReg(dstReg) << "[x] == $oldDst[x]);" << endl;
    s << "}" << endl;
  } else {
    s << "procedure $memcpy." << dstReg << "." << srcReg;
    s << "(dest: int, src: int, len: int, align: int, isvolatile: bool);" << endl;
    s << "modifies " << memReg(dstReg) << ";" << endl;
    s << "ensures (forall x:int :: dest <= x && x < dest + len ==> "
      << memReg(dstReg) << "[x] == old(" << memReg(srcReg) << ")[src - dest + x]);" 
      << endl;
    s << "ensures (forall x:int :: !(dest <= x && x < dest + len) ==> "
      << memReg(dstReg) << "[x] == old(" << memReg(dstReg) << ")[x]);" << endl;
  }

  return s.str();
}

string SmackRep::memsetProc(int dstReg) {
  stringstream s;

  if (SmackOptions::MemoryModelImpls) {
    s << "procedure $memset." << dstReg;
    s << "(dest: int, val: int, len: int, align: int, isvolatile: bool)" << endl;
    s << "modifies " << memReg(dstReg) << ";" << endl;
    s << "{" << endl;
    s << "  var $oldDst: [" << getPtrType() << "] " << getPtrType() << ";" << endl;
    s << "  $oldDst := " << memReg(dstReg) << ";" << endl;
    s << "  havoc " << memReg(dstReg) << ";" << endl;
    s << "  assume (forall x:int :: dest <= x && x < dest + len ==> "
      << memReg(dstReg) << "[x] == val);" << endl;
    s << "  assume (forall x:int :: !(dest <= x && x < dest + len) ==> "
      << memReg(dstReg) << "[x] == $oldDst[x]);" << endl;
    s << "}" << endl;
  } else {
      s << "Procedure(\"$memset." << dstReg << "\", params=[('dest', 'int'), ('val', 'int'), ('len', 'int'), ('align', 'int'), ('isvolatile', 'bool')])";
    //s << "procedure $memset." << dstReg;
    //s << "(dest: int, val: int, len: int, align: int, isvolatile: bool);" << endl;
    //s << "modifies " << memReg(dstReg) << ";" << endl;
    //s << "ensures (forall x:int :: dest <= x && x < dest + len ==> "
    //  << memReg(dstReg) << "[x] == val);"
    //  << endl;
    //s << "ensures (forall x:int :: !(dest <= x && x < dest + len) ==> "
    //  << memReg(dstReg) << "[x] == old(" << memReg(dstReg) << ")[x]);" << endl;
  }

  return s.str();
}

} // namespace smack

