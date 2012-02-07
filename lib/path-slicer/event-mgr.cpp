#include "util.h"
#include "event-mgr.h"
using namespace tern;

#include "common/util.h"
using namespace llvm;

using namespace std;

char tern::EventMgr::ID = 0;

namespace {
  static RegisterPass<tern::EventMgr> X(
    "event-func",
    "Get functions with event operations",
    false,
    true); // is analysis
}

void EventMgr::getAnalysisUsage(AnalysisUsage &AU) const {
  AU.setPreservesAll();
  CallGraphFP::getAnalysisUsage(AU);
}

void EventMgr::setupEvents(std::vector<llvm::Function *> &eventList) {
  sync_funcs.clear();
  sync_funcs.insert(sync_funcs.begin(), eventList.begin(), eventList.end());
}

// TODO: search in vector may be slow
bool EventMgr::is_sync_function(Function *f) {
  for (size_t i = 0; i < sync_funcs.size(); ++i) {
    if (sync_funcs[i] == f)
      return true;
  }
  return false;
}

void EventMgr::DFS(Function *f) {
  visited.insert(f);
  InstList css = get_call_sites(f);
  for (size_t i = 0; i < css.size(); ++i) {
    Function *caller = css[i]->getParent()->getParent();
    if (visited.count(caller) == 0) {
      parent[caller] = f;
      DFS(caller);
    }
  }
}

void EventMgr::output(const Module &M) const {
  vector<string> func_names;
  FILE *fout;

  func_names.clear();
  for (Module::const_iterator fi = M.begin(); fi != M.end(); ++fi)
    func_names.push_back(fi->getNameStr());
  sort(func_names.begin(), func_names.end());
  fout = fopen("/tmp/all-func.txt", "w");
  for (size_t i = 0; i < func_names.size(); ++i) {
    fprintf(fout, "%s\n", func_names[i].c_str());
  }

  func_names.clear();
  for (DenseSet<Function *>::const_iterator it = visited.begin();
      it != visited.end(); ++it) {
    func_names.push_back((*it)->getNameStr());
  }
  sort(func_names.begin(), func_names.end());
  fout = fopen("/tmp/event-func.txt", "w");
  for (size_t i = 0; i < func_names.size(); ++i) {
    fprintf(fout, "%s\n", func_names[i].c_str());
  }
}

bool EventMgr::mayCallEvent(Function *f) {
  return visited.count(f);
}

bool EventMgr::eventBetween(BranchInst *prevInstr, Instruction *postInstr) {
  Function *func = Util::getFunction(prevInstr);
  BasicBlock *postDomBB = Util::getBasicBlock(postInstr);
  
  /* Flood fill from <bb> until reaching <post_dominator_bb> */
  bbVisited.clear();
  for (Function::iterator bi = func->begin(); bi != func->end(); ++bi)
    bbVisited[bi] = false;
  for (unsigned i = 0; i < prevInstr->getNumSuccessors(); i++)
    DFS(prevInstr->getSuccessor(i), postDomBB);
  /* If any visited BB has a sync operation, the branch cannot be omitted. */
  for (Function::iterator bi = func->begin(); bi != func->end(); ++bi) {
    if (!bbVisited[bi])
      continue;
    // cerr << "Visited BB: " << bi->getNameStr() << endl;
    for (BasicBlock::iterator ii = bi->begin(); ii != bi->end(); ++ii) {
      if (is_call(ii) && !is_intrinsic_call(ii)) {
        vector<Function *> called_funcs = get_called_functions(ii);
        for (size_t i = 0; i < called_funcs.size(); ++i) {
          if (mayCallEvent(called_funcs[i]))
            return true;
        }
      }
    }
  }
  return false;
}

void EventMgr::DFS(BasicBlock *x, BasicBlock *sink) {
  // cerr << "DFS: " << x->getNameStr() << endl;
  if (bbVisited[x])
    return;
  // Stop at the sink -- the post dominator of the branch
  if (x == sink)
    return;
  bbVisited[x] = true;
  for (succ_iterator it = succ_begin(x); it != succ_end(x); ++it) {
    BasicBlock *y = *it;
    DFS(y, sink);
  }
}

void EventMgr::traverse_call_graph(Module &M) {
  visited.clear();
  parent.clear();
  for (size_t i = 0; i < sync_funcs.size(); ++i) {
    DFS(sync_funcs[i]);
  }
}

void EventMgr::print_call_chain(Function *f) {
  /*if (!parent.count(f)) {
    std::cerr << endl;
    return;
  }
  std::cerr << f->getNameStr() << " => ";
  print_call_chain(parent[f]);*/
}

void EventMgr::print_calling_functions(Function *f) {
  /*if (!call_sites.count(f))
    return;
  cerr << f->getNameStr() << ": ";
  vector<Instruction *> &callers = call_sites[f];
  for (size_t i = 0; i < callers.size(); ++i) {
    cerr << callers[i]->getParent()->getParent()->getNameStr() << ' ';
  }
  cerr << endl;*/
}

void EventMgr::stats(const Module &M) const {

}

bool EventMgr::runOnModule(Module &M) {
  CallGraphFP::runOnModule(M);
  traverse_call_graph(M);
  return false;
}

void EventMgr::print(llvm::raw_ostream &O, const Module *M) const {
  output(*M);
  stats(*M);
}

