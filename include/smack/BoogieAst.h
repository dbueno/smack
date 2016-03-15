//
// This file is distributed under the MIT License. See LICENSE for details.
//
#ifndef BOOGIEAST_H
#define BOOGIEAST_H

#include <cassert>
#include <sstream>
#include <string>
#include <vector>
#include <set>

namespace smack {

using namespace std;

class Program;

class Expr {
public:
  enum class ExprKind {
    EBin,
    ECond,
    EFun,
    EBool,
    EInt,
    EBv,
    ENeg,
    ENot,
    EQuant,
    ESel,
    EUpd,
    EVar,
    ECode
  };
  static string kindString(ExprKind kind) {
    switch (kind) {
      case ExprKind::EBin: return "EBin";
      case ExprKind::ECond: return "ECond";
      case ExprKind::EFun: return "EFun";
      case ExprKind::EBool: return "EBool";
      case ExprKind::EInt: return "EInt";
      case ExprKind::EBv: return "EBv";
      case ExprKind::ENeg: return "ENeg";
      case ExprKind::ENot: return "ENot";
      case ExprKind::EQuant: return "EQuant";
      case ExprKind::ESel: return "ESel";
      case ExprKind::EUpd: return "EUpd";
      case ExprKind::EVar: return "EVar";
      case ExprKind::ECode: return "ECode";
    }
  }
  ExprKind getKind() const { return kind; }
  virtual void print(ostream& os) const = 0;
  static const Expr* exists(string v, string t, const Expr* e);
  static const Expr* forall(string v, string t, const Expr* e);
  static const Expr* and_(const Expr* l, const Expr* r);
  static const Expr* cond(const Expr* c, const Expr* t, const Expr* e);
  static const Expr* eq(const Expr* l, const Expr* r);
  static const Expr* lt(const Expr* l, const Expr* r);
  static const Expr* fn(string f, const Expr* x);
  static const Expr* fn(string f, const Expr* x, const Expr* y);
  static const Expr* fn(string f, const Expr* x, const Expr* y, const Expr* z);
  static const Expr* fn(string f, vector<const Expr*> args);
  static const Expr* id(string x);
  static const Expr* impl(const Expr* l, const Expr* r);
  static const Expr* lit(bool b);
  static const Expr* lit(string v);
  static const Expr* lit(unsigned long v);
  static const Expr* lit(long v);
  static const Expr* lit(string v, unsigned w);
  static const Expr* lit(unsigned long v, unsigned w);
  static const Expr* neq(const Expr* l, const Expr* r);
  static const Expr* not_(const Expr* e);
  static const Expr* sel(const Expr* b, const Expr* i);
  static const Expr* sel(string b, string i);

private:
  const ExprKind kind;
protected:
  Expr(ExprKind kind) : kind(kind) {};
};
std::ostream& operator<<(std::ostream& os, const Expr& e);

class BinExpr : public Expr {
public:
  enum Binary { Iff, Imp, Or, And, Eq, Neq, Lt, Gt, Lte, Gte, Sub, Conc,
                Plus, Minus, Times, Div, Mod
              };
private:
  const Binary op;
  const Expr* lhs;
  const Expr* rhs;
public:
  BinExpr(const Binary b, const Expr* l, const Expr* r) : Expr(ExprKind::EBin), op(b), lhs(l), rhs(r) {}
  void print(ostream& os) const;
  const Expr *getLhs() const { return lhs; }
  const Expr *getRhs() const { return rhs; }
  Binary getOp() const { return op; }
};

class CondExpr : public Expr {
  const Expr* cond;
  const Expr* then;
  const Expr* else_;
public:
  CondExpr(const Expr* c, const Expr* t, const Expr* e)
    : Expr(ExprKind::ECond), cond(c), then(t), else_(e) {}
  void print(ostream& os) const;
};

class FunExpr : public Expr {
  string fun;
  vector<const Expr*> args;
public:
  FunExpr(string f, vector<const Expr*> xs) : Expr(ExprKind::EFun), fun(f), args(xs) {}
  const string& getFun() const { return fun; }
  const vector<const Expr*>& getArgs() const { return args; }
  void print(ostream& os) const;
};

class BoolLit : public Expr {
  bool val;
public:
  BoolLit(bool b) : Expr(ExprKind::EBool), val(b) {}
  void print(ostream& os) const;
  bool getVal() const { return val; }
};

class IntLit : public Expr {
  string val;
public:
  IntLit(string v) : Expr(ExprKind::EInt), val(v) {}
  IntLit(unsigned long v) : Expr(ExprKind::EInt) {
    stringstream s;
    s << v;
    val = s.str();
  }
  IntLit(long v) : Expr(ExprKind::EInt) {
    stringstream s;
    s << v;
    val = s.str();
  }
  void print(ostream& os) const;
  string getVal() const { return val; }
};

class BvLit : public Expr {
  string val;
  unsigned width;
public:
  BvLit(string v, unsigned w) : Expr(ExprKind::EBv), val(v), width(w) {}
  BvLit(unsigned long v, unsigned w) : Expr(ExprKind::EBv), width(w) {
    stringstream s;
    s << v;
    val = s.str();
  }
  void print(ostream& os) const;
};

class NegExpr : public Expr {
  const Expr* expr;
public:
  NegExpr(const Expr* e) : Expr(ExprKind::ENeg), expr(e) {}
  void print(ostream& os) const;
};

class NotExpr : public Expr {
  const Expr* expr;
public:
  NotExpr(const Expr* e) : Expr(ExprKind::ENot), expr(e) {}
  void print(ostream& os) const;
  const Expr *getExpr() const { return expr; }
};

class QuantExpr : public Expr {
public:
  enum Quantifier { Exists, Forall };
private:
  Quantifier quant;
  vector< pair<string,string> > vars;
  const Expr* expr;
public:
  QuantExpr(Quantifier q, vector< pair<string,string> > vs, const Expr* e) : Expr(ExprKind::EQuant), quant(q), vars(vs), expr(e) {}
  void print(ostream& os) const;
};

class SelExpr : public Expr {
  const Expr* base;
  vector<const Expr*> idxs;
public:
  SelExpr(const Expr* a, vector<const Expr*> i) : Expr(ExprKind::ESel), base(a), idxs(i) {}
  SelExpr(const Expr* a, const Expr* i) : Expr(ExprKind::ESel), base(a), idxs(vector<const Expr*>(1, i)) {}
  void print(ostream& os) const;
};

class UpdExpr : public Expr {
  const Expr* base;
  vector<const Expr*> idxs;
  const Expr* val;
public:
  UpdExpr(const Expr* a, vector<const Expr*> i, const Expr* v)
    : Expr(ExprKind::EUpd), base(a), idxs(i), val(v) {}
  UpdExpr(const Expr* a, const Expr* i, const Expr* v)
    : Expr(ExprKind::EUpd), base(a), idxs(vector<const Expr*>(1, i)), val(v) {}
  void print(ostream& os) const;
};

class VarExpr : public Expr {
  string var;
public:
  VarExpr(string v) : Expr(ExprKind::EVar), var(v) {}
  string name() const { return var; }
  void print(ostream& os) const;
};

class AttrVal {
public:
  virtual void print(ostream& os) const = 0;
};

class StrVal : public AttrVal {
  string val;
public:
  StrVal(string s) : val(s) {}
  void print(ostream& os) const;
};

class ExprVal : public AttrVal {
  const Expr* val;
public:
  ExprVal(const Expr* e) : val(e) {}
  void print(ostream& os) const;
};

class Attr {
protected:
  string name;
  vector<const AttrVal*> vals;
public:
  Attr(string n, vector<const AttrVal*> vs) : name(n), vals(vs) {}
  void print(ostream& os) const;

  static const Attr* attr(string s);
  static const Attr* attr(string s, string v);
  static const Attr* attr(string s, int v);
  static const Attr* attr(string s, string v, int i);
  static const Attr* attr(string s, string v, int i, int j);
  static const Attr* attr(string s, vector<const Expr*> vs);
};

class Stmt {
public:
  enum class StmtKind {
    Assert,
    Assign,
    Assume,
    Call,
    Comment,
    Goto,
    Havoc,
    Return,
    Code
  };
  const StmtKind kind;
  Stmt(StmtKind kind) : kind(kind) {};

  static const Stmt* annot(vector<const Attr*> attrs);
  static const Stmt* annot(const Attr* a);
  static const Stmt* assert_(const Expr* e);
  static const Stmt* assign(const Expr* e, const Expr* f);
  static const Stmt* assign(vector<const Expr*> lhs, vector<const Expr*> rhs);
  static const Stmt* assume(const Expr* e);
  static const Stmt* assume(const Expr* e, const Attr* attr);
  static const Stmt* call(string p);
  static const Stmt* call(string p, const Expr* x);
  static const Stmt* call(string p, const Expr* x, const Attr* attr);
  static const Stmt* call(string p, const Expr* x, string r);
  static const Stmt* call(string p, const Expr* x, const Expr* y, string r);
  static const Stmt* call(string p, vector<const Expr*> ps);
  static const Stmt* call(string p, vector<const Expr*> ps, vector<string> rs);
  static const Stmt* call(string p, vector<const Expr*> ps, vector<string> rs, const Attr* attr);
  static const Stmt* call(string p, vector<const Expr*> ps, vector<string> rs, vector<const Attr*> ax);
  static const Stmt* comment(string c);
  static const Stmt* goto_(string t);
  static const Stmt* goto_(string t, string u);
  static const Stmt* goto_(vector<string> ts);
  static const Stmt* havoc(string x);
  static const Stmt* return_();
  static const Stmt* return_(const Expr* e);
  static const Stmt* skip();
  static const Stmt* code(string s);
  virtual void print(ostream& os) const = 0;
};
inline std::ostream& operator<<(std::ostream& os, const Stmt& e) { e.print(os); return os; }

inline std::ostream &operator<<(std::ostream &str, Stmt::StmtKind &kind) {
  switch (kind) {
    case Stmt::StmtKind::Assert: str << "Assert"; break;
    case Stmt::StmtKind::Assign: str << "Assign"; break;
    case Stmt::StmtKind::Assume: str << "Assume"; break;
    case Stmt::StmtKind::Call: str << "Call"; break;
    case Stmt::StmtKind::Comment: str << "Comment"; break;
    case Stmt::StmtKind::Goto: str << "Goto"; break;
    case Stmt::StmtKind::Havoc: str << "Havoc"; break;
    case Stmt::StmtKind::Return: str << "Return"; break;
    case Stmt::StmtKind::Code: str << "Code"; break;
  }
  return str;
}

class AssertStmt : public Stmt {
public:
  const Expr* expr;
  AssertStmt(const Expr* e) : Stmt(StmtKind::Assert), expr(e) {}
  void print(ostream& os) const;
};

class AssignStmt : public Stmt {
public:
  vector<const Expr*> lhs;
  vector<const Expr*> rhs;
  AssignStmt(vector<const Expr*> lhs, vector<const Expr*> rhs) : Stmt(StmtKind::Assign), lhs(lhs), rhs(rhs) {}
  void print(ostream& os) const;
};

class AssumeStmt : public Stmt {
public:
  const Expr* expr;
  vector<const Attr*> attrs;
  AssumeStmt(const Expr* e) : Stmt(StmtKind::Assume), expr(e) {}
  void add(const Attr* a) {
    attrs.push_back(a);
  }
  void print(ostream& os) const;
};

class CallStmt : public Stmt {
public:
  string proc;
  vector<const Expr*> params;
  vector<string> returns;
  vector<const Attr*> attrs;
  CallStmt(string p, vector<const Expr*> ps, vector<string> rs, 
    vector<const Attr*> ax)
    : Stmt(StmtKind::Call), proc(p), params(ps), returns(rs), attrs(ax) {}
  void print(ostream& os) const;
};

class Comment : public Stmt {
public:
  string str;
  Comment(string s) : Stmt(StmtKind::Comment), str(s) {}
  void print(ostream& os) const;
};

class GotoStmt : public Stmt {
public:
  vector<string> targets;
  GotoStmt(vector<string> ts) : Stmt(StmtKind::Goto), targets(ts) {}
  void print(ostream& os) const;
};

class HavocStmt : public Stmt {
public:
  vector<string> vars;
  HavocStmt(vector<string> vs) : Stmt(StmtKind::Havoc), vars(vs) {}
  void print(ostream& os) const;
};

class ReturnStmt : public Stmt {
public:
  const Expr* expr;
  ReturnStmt(const Expr* e = nullptr) : Stmt(StmtKind::Return), expr(e) {}
  void print(ostream& os) const;
};

class CodeStmt : public Stmt {
public:
  string code;
  CodeStmt(string s) : Stmt(StmtKind::Code), code(s) {}
  void print(ostream& os) const;
};

class Decl {
  static unsigned uniqueId;
public:
  enum kind { STOR, PROC, FUNC, TYPE, UNNAMED, CODE };
protected:
  unsigned id;
  string name;
  vector<const Attr*> attrs;
  Decl(string n, vector<const Attr*> ax) : id(uniqueId++), name(n), attrs(ax) { }
  Decl(string n) : id(uniqueId++), name(n), attrs(vector<const Attr*>()) { }
public:
  virtual void print(ostream& os) const = 0;
  unsigned getId() const { return id; }
  string getName() const { return name; }
  virtual kind getKind() const = 0;
  void addAttr(const Attr* a) { attrs.push_back(a); }
  
  static Decl* typee(string name, string type);
  static Decl* axiom(const Expr* e);
  static Decl* function(string name, vector< pair<string,string> > args, string type, const Expr* e);
  static Decl* constant(string name, string type);
  static Decl* constant(string name, string type, bool unique);
  static Decl* constant(string name, string type, vector<const Attr*> ax, bool unique);
  static Decl* variable(string name, string type);
  static Decl* procedure(Program& p, string name);
  static Decl* procedure(Program& p, string name,
    vector< pair<string,string> > args, vector< pair<string,string> > rets);
  static Decl* code(string s);
};
inline std::ostream& operator<<(std::ostream& os, const Decl& e) { e.print(os); return os; }

struct DeclCompare {
  bool operator()(Decl* a, Decl* b) {
    assert(a && b);    
    if (a->getKind() == b->getKind() && a->getKind() != Decl::UNNAMED)
      return a->getName() < b->getName();
    else if (a->getKind() == b->getKind())
      return a->getId() < b->getId();
    else
      return a->getKind() < b->getKind();
  }
};

class TypeDecl : public Decl {
  string alias;
public:
  TypeDecl(string n, string t) : Decl(n), alias(t) {}
  kind getKind() const { return TYPE; }
  void print(ostream& os) const;
  string getAlias() const { return alias; };
};

class AxiomDecl : public Decl {
  const Expr* expr;
  static int uniqueId;
public:
  AxiomDecl(const Expr* e) : Decl(""), expr(e) {}
  kind getKind() const { return UNNAMED; }
  void print(ostream& os) const;
  const Expr* getExpr() const { return expr; }
};

class ConstDecl : public Decl {
  string type;
  bool unique;
public:
  ConstDecl(string n, string t, vector<const Attr*> ax, bool u) : Decl(n,ax), type(t), unique(u) {}
  kind getKind() const { return STOR; }
  void print(ostream& os) const;
};

class FuncDecl : public Decl {
  vector< pair<string, string> > params;
  string type;
  const Expr* body;
public:
  FuncDecl(string n, vector<const Attr*> ax, vector< pair<string, string> > ps,
    string t, const Expr* b)
    : Decl(n,ax), params(ps), type(t), body(b) {}
  kind getKind() const { return FUNC; }
  void print(ostream& os) const;
};

class VarDecl : public Decl {
  string type;
public:
  VarDecl(string n, string t) : Decl(n), type(t) {}
  kind getKind() const { return STOR; }
  void print(ostream& os) const;
  inline string getType() const { return type; }
};

class Block {
  string name;
public:
  vector<const Stmt*> stmts;

  Block() : name("") {}
  Block(string n) : name(n) {}
  void print(ostream& os) const;
  void insert(const Stmt* s) {
    stmts.insert(stmts.begin(), s);
  }
  void addStmt(const Stmt* s) {
    stmts.push_back(s);
  }
  string getName() {
    return name;
  }
};

class CodeContainer {
protected:
  Program& prog;
  CodeContainer(Program& p) : prog(p) {}
public:
  set<Decl*,DeclCompare> decls;
  vector<Block*> blocks;
  vector<string> mods;

  Program& getProg() const {
    return prog;
  }
  void addDecl(Decl* d) {
    decls.insert(d);
  }
  void insert(const Stmt* s) {
    blocks.front()->insert(s);
  }
  void addBlock(Block* b) {
    blocks.push_back(b);
  }
  bool hasBody() {
    return decls.size() > 0 || blocks.size() > 0;
  }
  void addMod(string m) {
    mods.push_back(m);
  }
  void addMods(vector<string> ms) {
    for (unsigned i = 0; i < ms.size(); i++)
      addMod(ms[i]);
  }
  virtual bool isProc() { return false; }
};

class CodeExpr : public Expr, public CodeContainer {
public:
  CodeExpr(Program& p) : Expr(ExprKind::ECode), CodeContainer(p) {}
  void print(ostream& os) const;
};

class ProcDecl : public Decl, public CodeContainer {
public:
  vector< pair<string,string> > params;
  vector< pair<string,string> > rets;
  vector<const Expr*> requires;
  vector<const Expr*> ensures;
  
  ProcDecl(Program& p, string n, vector< pair<string,string> > ps, vector< pair<string,string> > rs) 
    : Decl(n), CodeContainer(p), params(ps), rets(rs) {}
  kind getKind() const { return PROC; }
  void addParam(string x, string t) {
    params.push_back(make_pair(x, t));
  }
  void addRet(string x, string t) {
    rets.push_back(make_pair(x, t));
  }
  vector< pair<string,string> > getRets() {
    return rets;
  }
  void addRequires(const Expr* e) {
    requires.push_back(e);
  }
  void addEnsures(const Expr* e) {
    ensures.push_back(e);
  }
  bool isProc() { return true; }
  void print(ostream& os) const;
};

class CodeDecl : public Decl {
public:
  CodeDecl(string s) : Decl(s) {}
  kind getKind() const { return CODE; }
  void print(ostream& os) const;
};

class Program {
  // TODO While I would prefer that a program is just a set or sequence of
  // declarations, putting the Prelude in a CodeDeclaration does not work,
  // and I do not yet understand why; see below. --mje
public:
  string prelude;
  set<Decl*,DeclCompare> decls;
  vector<Decl*> preludeDecls;
  Program() {}
  void print(ostream& os) const;
  void addPreludeDecls(vector<Decl*> decls) {
    for (auto decl : decls) {
      preludeDecls.push_back(decl);
    }
  }
  void addDecl(Decl* d) {
    decls.insert(d);
  }
  void addDecl(string s) {
    // TODO Why does this break to put the prelude string inside of a CodeDecl?
    decls.insert( Decl::code(s) );
  }
  void appendPrelude(string s) {
    prelude += s;
  }
  void addDecls(vector<Decl*> ds) {
    for (unsigned i = 0; i < ds.size(); i++)
      addDecl(ds[i]);
  }
  void addDecls(vector<string> ds) {
    for (unsigned i=0; i < ds.size(); i++)
      addDecl(ds[i]);
  }
  vector<ProcDecl*> getProcs() {
    vector<ProcDecl*> procs;
    for (set<Decl*>::iterator i = decls.begin(); i != decls.end(); ++i)
      if ((*i)->getKind() == Decl::PROC)
        procs.push_back((ProcDecl*) (*i));
    return procs;
  }
};
}

#endif // BOOGIEAST_H

