#include "dyn-instrs.h"
#include "macros.h"
using namespace tern;

using namespace klee;

DynInstr::DynInstr() {
  region = NULL;
  index = SIZE_T_INVALID;
  callingCtx = NULL;
  simCallingCtx = NULL;
}

DynInstr::~DynInstr() {

}

int DynInstr::getTid() {
  return region->getTid();
}

void DynInstr::setIndex(size_t index) {
  this->index = index;
}

size_t DynInstr::getIndex() {
  return index;
}

void DynInstr::setCallingCtx(std::vector<int> *ctx) {
  this->callingCtx = ctx;
}

CallCtx *DynInstr::getCallingCtx() {
  return callingCtx;
}

void DynInstr::setSimCallingCtx(std::vector<int> *ctx) {
  this->simCallingCtx = ctx;
}

CallCtx *DynInstr::getSimCallingCtx() {
  return simCallingCtx;
}

int DynInstr::getOrigInstrId() {
  return -1; // TBD
}

int DynInstr::getMxInstrId() {
  return -1; // TBD
}

std::set<int> *DynInstr::getSimInstrId() {
  return NULL; // TBD
}

void DynInstr::setTaken(bool isTaken, const char *reason) {
  region->setTaken(this, isTaken, reason);
}

bool DynInstr::isTaken() {
  return region->isTaken(this);
}

Instruction *DynInstr::getOrigInstr() {
  return region->getOrigInstr(this);
}

bool DynInstr::isTarget() {
  return isTaken();
}

DynPHIInstr::DynPHIInstr() {

}

DynPHIInstr::~DynPHIInstr() {

}

void DynPHIInstr::setIncomingIndex(unsigned index) {
  incomingIndex = index;
}

unsigned DynPHIInstr::getIncomingIndex() {
  return incomingIndex;
}

DynBrInstr::DynBrInstr() {

}

DynBrInstr::~DynBrInstr() {

}

DynRetInstr::DynRetInstr() {

}

DynRetInstr::~DynRetInstr() {

}

void DynRetInstr::setDynCallInstr(DynInstr *dynInstr) {
  dynCallInstr = dynInstr;
}

DynInstr *DynRetInstr::getDynCallInstr() {
  return dynCallInstr;
}

DynCallInstr::DynCallInstr() {

}

DynCallInstr::~DynCallInstr() {

}

void DynCallInstr::setCalledFunc(llvm::Function *f) {
  calledFunc = f;
}

llvm::Function *DynCallInstr::getCalledFunc() {
  return calledFunc;
}

DynSpawnThreadInstr::DynSpawnThreadInstr() {

}

DynSpawnThreadInstr::~DynSpawnThreadInstr() {

}

void DynSpawnThreadInstr::setChildTid(int childTid) {
  this->childTid = childTid;
}

int DynSpawnThreadInstr::getChildTid() {
  return childTid;
}

DynMemInstr::DynMemInstr() {

}

DynMemInstr::~DynMemInstr() {

}

void DynMemInstr::setConAddr(long conAddr) {
  this->conAddr = conAddr;
}

long DynMemInstr::getConAddr() {
  return conAddr;
}

void DynMemInstr::setSymAddr(ref<klee::Expr> symAddr) {
  this->symAddr = symAddr;
}

ref<klee::Expr> DynMemInstr::getSymAddr() {
  return symAddr;
}

bool DynMemInstr::isAddrSymbolic() {
  return isa<klee::ConstantExpr>(symAddr);
}

