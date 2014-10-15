//
// Copyright (c) 2013 Zvonimir Rakamaric (zvonimir@cs.utah.edu),
//                    Michael Emmi (michael.emmi@gmail.com)
// This file is distributed under the MIT License. See LICENSE for details.
//
#include "smack/BoogieAst.h"
#include "llvm/IR/Constants.h"
#include <sstream>

namespace smack {

using namespace std;

unsigned Decl::uniqueId = 0;
static const char *quote = "\"";

const Expr* Expr::and_(const Expr* l, const Expr* r) {
  return new BinExpr(BinExpr::And, l, r);
}

const Expr* Expr::cond(const Expr* c, const Expr* t, const Expr* e) {
  return new CondExpr(c,t,e);
}

const Expr* Expr::eq(const Expr* l, const Expr* r) {
  return new BinExpr(BinExpr::Eq, l, r);
}

const Expr* Expr::lt(const Expr* l, const Expr* r) {
  return new BinExpr(BinExpr::Lt, l, r);
}

const Expr* Expr::fn(string f, const Expr* x) {
  return new FunExpr(f, vector<const Expr*>(1, x));
}

const Expr* Expr::fn(string f, const Expr* x, const Expr* y) {
  vector<const Expr*> ps;
  ps.push_back(x);
  ps.push_back(y);
  return new FunExpr(f, ps);
}

const Expr* Expr::fn(string f, const Expr* x, const Expr* y, const Expr* z) {
  vector<const Expr*> ps;
  ps.push_back(x);
  ps.push_back(y);
  ps.push_back(z);
  return new FunExpr(f, ps);
}

const Expr* Expr::id(string s) {
  return new VarExpr(s);
}

const Expr* Expr::impl(const Expr* l, const Expr* r) {
  return new BinExpr(BinExpr::Imp, l, r);
}

const Expr* Expr::lit(int i) {
  return new LitExpr(i);
}
const Expr* Expr::lit(int i, unsigned w) {
  switch (w) {
  case 0:
    return new LitExpr(i);
  case 8:
    return new LitExpr(LitExpr::Bv8, i);
  case 32:
    return new LitExpr(LitExpr::Bv32, i);
  case 64:
    return new LitExpr(LitExpr::Bv64, i);
  default:
    assert(false && "unexpected integer width.");
  }
}

const Expr* Expr::lit(bool b) {
  return new LitExpr(b);
}

const Expr* Expr::neq(const Expr* l, const Expr* r) {
  return new BinExpr(BinExpr::Neq, l, r);
}

const Expr* Expr::not_(const Expr* e) {
  return new NotExpr(e);
}

const Expr* Expr::sel(const Expr* b, const Expr* i) {
  return new SelExpr(b, i);
}

const Expr* Expr::sel(string b, string i) {
  return new SelExpr(id(b), id(i));
}

const Attr* Attr::attr(string s, vector<const Expr*> vs) {
  vector<const AttrVal*> vals;
  for (unsigned i=0; i<vs.size(); i++)
    vals.push_back(new ExprVal(vs[i]));
  return new Attr(s,vals);
}

const Attr* Attr::attr(string s) {
  return attr(s, vector<const Expr*>());
}

const Attr* Attr::attr(string s, string v) {
  return new Attr(s, vector<const AttrVal*>(1, new StrVal(v)));
}

const Attr* Attr::attr(string s, int v) {
  return attr(s, vector<const Expr*>(1, Expr::lit(v)));
}

const Attr* Attr::attr(string s, string v, int i) {
  vector<const AttrVal*> vals;
  vals.push_back(new StrVal(v));
  vals.push_back(new ExprVal(Expr::lit(i)));
  return new Attr(s, vals);
}

const Attr* Attr::attr(string s, string v, int i, int j) {
  vector<const AttrVal*> vals;
  vals.push_back(new StrVal(v));
  vals.push_back(new ExprVal(Expr::lit(i)));
  vals.push_back(new ExprVal(Expr::lit(j)));
  return new Attr(s, vals);
}

const Stmt* Stmt::annot(vector<const Attr*> attrs) {
  AssumeStmt* s = new AssumeStmt(Expr::lit(true));
  for (unsigned i = 0; i < attrs.size(); i++)
    s->add(attrs[i]);
  return s;
}

const Stmt* Stmt::annot(const Attr* a) {
  return Stmt::annot(vector<const Attr*>(1, a));
}

const Stmt* Stmt::assert_(const Expr* e) {
  return new AssertStmt(e);
}

const Stmt* Stmt::assign(const Expr* e, const Expr* f) {
  return new AssignStmt(vector<const Expr*>(1, e), vector<const Expr*>(1, f));
}

const Stmt* Stmt::assume(const Expr* e) {
  return new AssumeStmt(e);
}

const Stmt* Stmt::assume(const Expr* e, const Attr* a) {
  AssumeStmt* s = new AssumeStmt(e);
  s->add(a);
  return (const AssumeStmt*) s;
}

const Stmt* Stmt::call(string p) {
  return call(p, vector<const Expr*>(), vector<string>());
}

const Stmt* Stmt::call(string p, const Expr* x) {
  return call(p, vector<const Expr*>(1, x), vector<string>());
}

const Stmt* Stmt::call(string p, const Expr* x, string r) {
  return call(p, vector<const Expr*>(1, x), vector<string>(1, r));
}

const Stmt* Stmt::call(string p, const Expr* x, const Expr* y, string r) {
  vector<const Expr*> ps;
  ps.push_back(x);
  ps.push_back(y);
  return call(p, ps, vector<string>(1, r));
}

const Stmt* Stmt::call(string p, vector<const Expr*> ps) {
  return call(p, ps, vector<string>());
}

const Stmt* Stmt::call(string p, vector<const Expr*> ps, vector<string> rs) {
  return call(p, ps, rs, vector<const Attr*>());
}

const Stmt* Stmt::call(string p, vector<const Expr*> ps, vector<string> rs,
  const Attr* attr) {

  return call(p, ps, rs, vector<const Attr*>(1, attr));
}

const Stmt* Stmt::call(string p, vector<const Expr*> ps, vector<string> rs, 
  vector<const Attr*> ax) {

  return new CallStmt(p, ps, rs, ax);
}

const Stmt* Stmt::comment(string s) {
  return new Comment(s);
}

const Stmt* Stmt::goto_(string t) {
  return goto_(vector<string>(1, t));
}

const Stmt* Stmt::goto_(string t, string u) {
  vector<string> ts(2, "");
  ts[0] = t;
  ts[1] = u;
  return goto_(ts);
}

const Stmt* Stmt::goto_(vector<string> ts) {
  return new GotoStmt(ts);
}

const Stmt* Stmt::havoc(string x) {
  return new HavocStmt(vector<string>(1, x));
}

const Stmt* Stmt::return_() {
  return new ReturnStmt();
}

const Stmt* Stmt::skip() {
  return new AssumeStmt(Expr::lit(true));
}

const Stmt* Stmt::code(string s) {
  return new CodeStmt(s);
}

Decl* Decl::typee(string name, string type) {
  return new TypeDecl(name,type);
}
Decl* Decl::axiom(const Expr* e) {
  return new AxiomDecl(e);
}
Decl* Decl::constant(string name, string type) {
  return Decl::constant(name, type, vector<const Attr*>(), false);
}
Decl* Decl::constant(string name, string type, bool unique) {
  return Decl::constant(name, type, vector<const Attr*>(), unique);
}
Decl* Decl::constant(string name, string type, vector<const Attr*> ax, bool unique) {
  return new ConstDecl(name, type, ax, unique);
}
Decl* Decl::variable(string name, string type) {
  return new VarDecl(name, type);
}
Decl* Decl::procedure(Program& p, string name) {
  return procedure(p,name,vector< pair<string,string> >(),vector< pair<string,string> >());
}
Decl* Decl::procedure(Program& p, string name, 
    vector< pair<string,string> > args, vector< pair<string,string> > rets) {
  return new ProcDecl(p,name,args,rets);
}
Decl* Decl::code(string s) {
  return new CodeDecl(s);
}

ostream& operator<<(ostream& os, const Expr& e) {
  e.print(os);
  return os;
}
ostream& operator<<(ostream& os, const Expr* e) {
  e->print(os);
  return os;
}
ostream& operator<<(ostream& os, const AttrVal* v) {
  v->print(os);
  return os;
}
ostream& operator<<(ostream& os, const Attr* a) {
  a->print(os);
  return os;
}
ostream& operator<<(ostream& os, const Stmt* s) {
  s->print(os);
  return os;
}
ostream& operator<<(ostream& os, const Block* b) {
  b->print(os);
  return os;
}
ostream& operator<<(ostream& os, Decl* d) {
  d->print(os);
  return os;
}
ostream& operator<<(ostream& os, Program* p) {
  if (p == 0) {
    os << "<null> Program!\n";
  } else {
    p->print(os);
  }
  return os;
}
ostream& operator<<(ostream& os, Program& p) {
  p.print(os);
  return os;
}

template<class T> void print_seq(ostream& os, vector<T> ts,
                                 string init, string sep, string term) {

  os << init;
  for (typename vector<T>::iterator i = ts.begin(); i != ts.end(); ++i)
    os << (i == ts.begin() ? "" : sep) << *i;
  os << term;
}
template<class T> void print_seq(ostream& os, vector<T> ts, string sep) {
  print_seq<T>(os, ts, "", sep, "");
}
template<class T> void print_seq(ostream& os, vector<T> ts) {
  print_seq<T>(os, ts, "", "", "");
}
template<class T, class C> void print_set(ostream& os, set<T,C> ts,
                                 string init, string sep, string term) {

  os << init;
  for (typename set<T,C>::iterator i = ts.begin(); i != ts.end(); ++i)
    os << (i == ts.begin() ? "" : sep) << *i;
  os << term;
}
template<class T, class C> void print_set(ostream& os, set<T,C> ts, string sep) {
  print_set<T,C>(os, ts, "", sep, "");
}
template<class T, class C> void print_set(ostream& os, set<T,C> ts) {
  print_set<T,C>(os, ts, "", "", "");
}

void BinExpr::print(ostream& os) const {
  // os << "(" << lhs << " ";
  switch (op) {
  case Iff:
    os << "Iff";
    break;
  case Imp:
    os << "Implies";
    break;
  case Or:
    os << "Or";
    break;
  case And:
    os << "And";
    break;
  case Eq:
    os << "Eq";
    break;
  case Neq:
    os << "Neq";
    break;
  case Lt:
    os << "LT";
    break;
  case Gt:
    os << "GT";
    break;
  case Lte:
    os << "LTE";
    break;
  case Gte:
    os << "GTE";
    break;
  case Sub:
    os << "<:";
    break;
  case Conc:
    os << "Concat";
    break;
  case Plus:
    os << "Plus";
    break;
  case Minus:
    os << "Minus";
    break;
  case Times:
    os << "Times";
    break;
  case Div:
    os << "Div";
    break;
  case Mod:
    os << "Mod";
    break;
  }
  os << "(" << lhs << ", " << rhs << ")";
}

void CondExpr::print(ostream &os) const {
    os << "CondExpr(" << cond << ", " << then << ", " << else_ << ")";
}

void FunExpr::print(ostream& os) const {
  os << "FunExpr(\"" << fun << "\", ";
  print_seq<const Expr*>(os, args, "[", ", ", "]");
  os << ")";
}

void LitExpr::print(ostream& os) const {
  switch (lit) {
  case True:
    os << "Bool(True)";
    break;
  case False:
    os << "Bool(False)";
    break;
  case Num:
  case Bv8:
  case Bv32:
  case Bv64:
    os << "Num(" << val << ")";
    break;
  }
}

void NegExpr::print(ostream& os) const {
  os << "NegExpr(" << expr << ")";
}

void NotExpr::print(ostream& os) const {
  os << "NotExpr(" << expr << ")";
}

void QuantExpr::print(ostream& os) const {
  os << "(";
  switch (q) {
  case Forall:
    os << "forall";
    break;
  case Exists:
    os << "exists";
    break;
  }
  os << " -- ToDo: Implement quantified expressions. ";
  os << ")";
}

void SelExpr::print(ostream& os) const {
  os << "SelExpr(" << base << ", ";
  print_seq<const Expr*>(os, idxs, "[", ", ", "]");
  os << ")";
}

void UpdExpr::print(ostream& os) const {
  os << "UpdExpr(\"" << base << "\", [";
  print_seq<const Expr*>(os, idxs, ", ");
  os << "], " << val << ")";
}

void VarExpr::print(ostream& os) const {
  os << "VarExpr(\"" << var << "\")";
}

void StrVal::print(ostream& os) const {
  os << "\"" << val << "\"";
}

void ExprVal::print(ostream& os) const {
  os << val;
}

void Attr::print(ostream& os) const {
  os << "('" << name << "'";
  if (vals.size() > 0)
    print_seq<const AttrVal*>(os, vals, ", [", ", ", "]");
  os << ")";
}

void AssertStmt::print(ostream& os) const {
  os << "AssertStmt(" << expr << ")";
}

void AssignStmt::print(ostream& os) const {
    os << "AssignStmt([";
    print_seq<const Expr*>(os, lhs, ", ");
    os << "], [";
    print_seq<const Expr*>(os, rhs, ", ");
    os << "])";
}

void AssumeStmt::print(ostream& os) const {
  os << "AssumeStmt(";
  os << expr;
  if (attrs.size() > 0)
    print_seq<const Attr*>(os, attrs, ", attrs=[", ", ", "]");
  os << ")";
}

void CallStmt::print(ostream& os) const {
  os << "CallStmt(";
  os << quote << proc << quote;
  print_seq<const Expr*>(os, params, ", [", ", ", "]");
  if (returns.size() > 0)
    print_seq<string>(os, returns, ", rets=[\"", "\", \"", "\"]");
  if (attrs.size() > 0)
    print_seq<const Attr*>(os, attrs, ", attrs=[", ", ", "]");
  os << ")";
}

void Comment::print(ostream& os) const {
  os << "// " << str;
}

void GotoStmt::print(ostream& os) const {
  os << "GotoStmt([";
  print_seq<string>(os, targets, "\"", "\", \"", "\"");
  os << "])";
}

void HavocStmt::print(ostream& os) const {
  os << "HavocStmt([";
  print_seq<string>(os, vars, ", ");
  os << "])";
}

void ReturnStmt::print(ostream& os) const {
  os << "ReturnStmt()";
}

void CodeStmt::print(ostream& os) const {
  os << code;
}

void TypeDecl::print(ostream& os) const {
  os << "type ";
  if (attrs.size() > 0)
    print_seq<const Attr*>(os, attrs, "", " ", " ");
  os << name;
  if (alias != "")
    os << " = " << alias << ";";
  os << ";";
}

void AxiomDecl::print(ostream& os) const {
  os << "AxiomDecl(";
  os << expr;
  if (attrs.size() > 0)
    print_seq<const Attr*>(os, attrs, ", attrs=[", ", ", "]");
  os << ")";
}

void ConstDecl::print(ostream& os) const {
  os << "add_constant(";
  os << quote << name << "\", \"" << type << "\", ";
  os << (unique ? "unique=True" : "unique=False");
  if (attrs.size() > 0)
    print_seq<const Attr*>(os, attrs, ", attrs=[", " ", "]");
  os << ")";
}


void FuncDecl::print(ostream& os) const {
  os << "function " << name;
  if (attrs.size() > 0)
    print_seq<const Attr*>(os, attrs, "", " ", " ");
  for (unsigned i = 0; i < params.size(); i++)
    os << params[i].first << ": " << params[i].second
       << (i < params.size() - 1 ? ", " : "");
  os << ": " << type << " { " << body << " };";
}

void VarDecl::print(ostream &os) const {
    os << "VarDecl(\"" << name << "\", \"" << type << "\"";
    if (attrs.size() > 0)
        print_seq<const Attr*>(os, attrs, ", [", ", ", "]");
    os << ")";
}

void ProcDecl::print(ostream &os) const {
    os << "Procedure(";
  if (attrs.size() > 0)
    print_seq<const Attr*>(os, attrs, "[", ", ", "]");
  os << "\"" << name << "\", ";

  os << "params=[";
  for (unsigned i = 0; i < params.size(); i++)
    os << "(\"" << params[i].first << "\", \"" << params[i].second << "\")"
       << (i < params.size() - 1 ? ", " : "");
  os << "]";
  if (rets.size() > 0) {
    os << ", rets=[";
    for (unsigned i = 0; i < rets.size(); i++)
      os << "(\"" << rets[i].first << "\", \"" << rets[i].second << "\")"
         << (i < rets.size() - 1 ? ", " : "");
    os << "]";
  }
  // if (blocks.size() == 0)
  //   os << ");";

  // if (mods.size() > 0) {
  //   os << endl;
  //   print_seq<string>(os, mods, "  modifies ", ", ", ";");
  // }
  if (requires.size() > 0) {
    os << endl;
    print_seq<const Expr*>(os, requires, ", requires=[", ", ", "]");
  }
  if (ensures.size() > 0) {
    os << endl;
    print_seq<const Expr*>(os, ensures, ", ensures=[", ", ", "]");
  }
  if (blocks.size() > 0) {
    os << ", blocks=[" << endl;
    if (decls.size() > 0)
      print_set<Decl*>(os, decls, "  ", ",\n  ", ",\n");
    print_seq<Block*>(os, blocks, ",\n");
    os << endl << "]";
  }
  os << ")" << endl;
}

void CodeDecl::print(ostream& os) const {
  os << name;
}

void Block::print(ostream &os) const {
    os << "Block(";
    print_seq<const Stmt*>(os, stmts, "[\n", ",\n  ", "]");
    if (name != "")
        os << ", name=\"" << name << "\"";
    os << ")";
}


void Program::print(ostream& os) const {
  os << "# BEGIN SMACK-GENERATED CODE" << endl;
  os << prelude;
  print_set<Decl*>(os, decls, "\n");
  os << endl;
  // os << endl;
  os << "# END SMACK-GENERATED CODE" << endl;
}

}

