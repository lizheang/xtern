/* Author: Junfeng Yang (junfeng@cs.columbia.edu) -*- Mode: C++ -*- */

#ifndef __TERN_RECORDER_LOGDEFS_H
#define __TERN_RECORDER_LOGDEFS_H

#include <stdint.h>
#include "stdio.h"
#include <boost/static_assert.hpp>
#include "tern/syncfuncs.h"
#include "tern/options.h"

namespace tern {

#define RECORD_SIZE     (32U)  // has to be a # define, not enum
#define INSID_BITS      (29U)
#define REC_TYPE_BITS   (3U)

enum {
  TRUNK_SIZE      = 1024*1024*1024U,   // 1GB
  LOG_SIZE        = 1*TRUNK_SIZE,
  MAX_INLINE_ARGS = 2U,
  MAX_EXTRA_ARGS  = 3U,
  INVALID_INSID   = (unsigned)(-1)
};

enum {
  InsidRecTy      = 0U,
  LoadRecTy       = 1U,
  StoreRecTy      = 2U,
  CallRecTy       = 3U,
  ExtraArgsRecTy  = 4U,
  ReturnRecTy     = 5U,
  SyncRecTy       = 6U,

  LastRecTy       = SyncRecTy
};
BOOST_STATIC_ASSERT(LastRecTy<(1U<<REC_TYPE_BITS));

enum {
  CallIndirect    = 1U,
  CallNoReturn    = 2U,
  CalleeEscape    = 4U
};

#ifdef ENABLE_PACKED_RECORD
#  pragma pack(push)  // push current alignment to stack
#  pragma pack(1)     // set alignment to 1 byte boundary
#endif

struct InsidRec {

protected:
  enum {
    MAX_INSID = (1<<INSID_BITS),
    INVALID_INSID_IN_REC = (INVALID_INSID & ((1<<INSID_BITS)-1))
  };

public:
  unsigned insid  : INSID_BITS;
  unsigned type   : REC_TYPE_BITS;
  bool validInsid() const { return insid != INVALID_INSID_IN_REC; }
  unsigned getInsid() const { return validInsid()? insid : INVALID_INSID; }
  void setInsid(unsigned id) {
    if(id == INVALID_INSID)
      id = INVALID_INSID_IN_REC;
    assert(id < MAX_INSID && "instruction id larger than (1U<<29)!");
    insid = id;
  }
  int numRecForInst() const;
};
BOOST_STATIC_ASSERT(sizeof(InsidRec)<=RECORD_SIZE);

// shared by LoadRec and StoreRec
struct MemRec: public InsidRec {
  long     seq;
  char*    addr;
  uint64_t data;
  char*    getAddr() { return addr; }
  uint64_t getData() { return data; }
};

struct LoadRec: public MemRec {};
BOOST_STATIC_ASSERT(sizeof(LoadRec)<=RECORD_SIZE);

struct StoreRec: public MemRec {};
BOOST_STATIC_ASSERT(sizeof(StoreRec)<=RECORD_SIZE);

/// common prefix of call-related records---not a real record type
struct CallRecPrefix: public InsidRec{
  uint8_t    flags;
  uint8_t    seq;
  short      narg;
};

struct CallRec: public CallRecPrefix {
  int      funcid;
  uint64_t args[MAX_INLINE_ARGS];
  short numArgsInRec(void) const;
};
BOOST_STATIC_ASSERT(sizeof(CallRec)<=RECORD_SIZE);

struct ExtraArgsRec: public CallRecPrefix {
  uint64_t args[MAX_EXTRA_ARGS];
  short numArgsInRec(void) const;
};
BOOST_STATIC_ASSERT(sizeof(ExtraArgsRec)<=RECORD_SIZE);

struct ReturnRec: public CallRecPrefix {
  int      funcid;
  uint64_t data;
};
BOOST_STATIC_ASSERT(sizeof(ReturnRec)<=RECORD_SIZE);

struct SyncRec: public InsidRec {
  short    sync;     // type of sync call
  bool     after;    // before or after the sync call
  bool     timedout; // is the wait timed out?
  int      turn;     // turn no. that this sync occurred
  uint64_t args[MAX_INLINE_ARGS];
};
BOOST_STATIC_ASSERT(sizeof(SyncRec)<=RECORD_SIZE);

#ifdef ENABLE_PACKED_RECORD
#  pragma pack(pop)   // restore original alignment from stack
#endif

static inline int NumExtraArgsRecords(int narg) {
  return (((narg)-MAX_INLINE_ARGS+MAX_EXTRA_ARGS-1)/MAX_EXTRA_ARGS);
}

static inline int NumSyncArgs(short sync) {
  return (sync == syncfunc::pthread_cond_wait)? 2 : 1;
}

static inline int NumRecordsForSync(short sync) {
  switch(sync) {
  case syncfunc::pthread_cond_wait:
  case syncfunc::pthread_barrier_wait:
  case syncfunc::pthread_cond_timedwait:
    return 2;
  }
  return 1;
}

inline int InsidRec::numRecForInst() const {
  switch(type) {
  case CallRecTy:
  case ExtraArgsRecTy:
  case ReturnRecTy:
    return /*CallRec*/ 1
      + NumExtraArgsRecords(((const CallRecPrefix&)*this).narg)
      + /*ReturnRec*/ ((((const CallRecPrefix&)*this).flags&CallNoReturn) == 0);
  case SyncRecTy:
    return NumRecordsForSync(((const SyncRec&)*this).sync);
  default:
    return 1;
  }
}

inline short CallRec::numArgsInRec() const {
  return std::min(narg, (short)MAX_INLINE_ARGS);
}

inline short ExtraArgsRec::numArgsInRec() const {
  short rec_narg = (short)(narg - MAX_INLINE_ARGS - (seq-1) * MAX_EXTRA_ARGS);
  return std::min(rec_narg, (short)MAX_EXTRA_ARGS);
}

static inline int getLogFilename(char *buf, size_t sz,
                                 int tid, const char* ext) {
  return snprintf(buf, sz, "%s/tid-%d-%d%s",
                  options::output_dir.c_str(), getpid(), tid, ext);
}

} // namespace tern

#endif
