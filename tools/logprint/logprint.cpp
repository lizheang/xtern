/* Author: Junfeng Yang (junfeng@cs.columbia.edu) */
#include "llvm/Module.h"
#include "llvm/LLVMContext.h"
#include "llvm/PassManager.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/IRReader.h"
#include "llvm/Support/CommandLine.h"

#include "common/id-manager/IDTagger.h"
#include "common/id-manager/IDManager.h"
#include "common/instr/instrutil.h"
#include "recorder/access/logaccess.h"

using namespace std;
using namespace llvm;
using namespace tern;

static cl::opt<std::string>
InputFilename(cl::Positional, cl::desc("<input raw log file>"),
    cl::init("-"), cl::value_desc("filename"));

static cl::opt<std::string>
BcFilename("bc", cl::desc("input .bc filename"),
           cl::value_desc(".bc filename"));

static cl::opt<std::string>
FuncsFilename("funcs", cl::desc("input .funcs filename, which maps function"\
                                " names to function IDs in the raw log"),
           cl::value_desc(".funcs filename"));

static cl::opt<bool>
PrintRaw("r", cl::init(false),
         cl::desc("Print the raw log in addition to the instruction log"));

static cl::opt<bool>
Details("v", cl::init(false),
         cl::desc("Verbose mode; print disassembled LLVM instructions"));

static bool endsWith(const string& str, const char* suffix) {
  size_t len = str.length();
  size_t suffix_len = strlen(suffix);
  return str.compare(len-suffix_len, suffix_len, suffix) == 0;
}

int main(int argc, char **argv) {
  cl::ParseCommandLineOptions(argc, argv,
      "Given a raw log file, print the instruction log"\
      " computed based on the raw log\n");

  LLVMContext &Context = getGlobalContext();
  SMDiagnostic Err;
  auto_ptr<Module> M;

  if(endsWith(BcFilename, ".ll"))
    M.reset(ParseAssemblyFile(BcFilename, Err, Context));
  else
    M.reset(ParseIRFile(BcFilename, Err, Context));
  if (M.get() == 0) {
    Err.Print(argv[0], errs());
    return 1;
  }

  PassManager Passes;
  IDManager   *IDM = new IDManager;
  Passes.add(IDM);
  Passes.run(*M.get());

  if(FuncsFilename.empty()) { // set default .funcs file
    FuncsFilename = BcFilename;
    if(endsWith(FuncsFilename, ".ll")
       || endsWith(FuncsFilename, ".bc"))
      FuncsFilename = FuncsFilename.substr(0, FuncsFilename.length() - 3);
    if(endsWith(FuncsFilename, "-record")
      || endsWith(FuncsFilename, "-replay"))
      FuncsFilename = FuncsFilename.substr(0, FuncsFilename.length() - 7);
    else if(endsWith(FuncsFilename, "-analysis"))
      FuncsFilename = FuncsFilename.substr(0, FuncsFilename.length() - 9);
    FuncsFilename += ".funcs";
  }

  setIDManager(IDM);
  InstLog::setFuncMap(FuncsFilename.c_str(), *M.get());

  if(PrintRaw) {
    auto_ptr<RawLog> raw(new RawLog(InputFilename.c_str()));
    for(RawLog::iterator ri=raw->begin(); ri!=raw->end(); ++ri)
      outs() << *ri << "\n";
  }

  InstLogBuilder b;
  auto_ptr<InstLog> log(b.create(InputFilename.c_str()));

  for(InstLog::iterator i = log->begin(); i != log->end(); ++i)
    log->printExecutedInst(outs(), *i, Details) << "\n";
}
