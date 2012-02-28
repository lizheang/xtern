

/* Flags to identify whether a dynamic instruction is taken, and reasons of taken. */

/* Reasons of taken by handling important events (synch calls or other important function calls). */
DEF(TEST_TARGET, TestTarget)
DEF(TAKEN_EVENT, EventTarget)

DEF(TAKEN_RACE, RaceTarget)                 /* real races. */
/* Reasons of taken by handling inter-thread phase. */
/* Reasons of taken by handling instruction -instruction may race in inter-thread phase. */
DEF(INTER_INSTR2, InterThreadTarget)
DEF(INTER_LOAD_TGT, InterThreadTarget)
DEF(INTER_STORE_TGT, InterThreadTarget)
// TBD

/* Reasons of taken by handling branch -instruction may race in inter-thread phase. */
DEF(INTER_BR_INSTR, InterThreadTarget)
// TBD

/* Reasons of taken by handling branch -branch may race in inter-thread phase. */
DEF(INTER_BR_BR, InterThreadTarget)
// TBD

DEF(INTER_PHASE_MAX, InterThreadTarget)                         /* End of inter-thread phase. */



/* Starting target of checkers in directed symbolic execution project. */
DEF(CHECKER_IMPORTANT, CheckerTarget)
DEF(CHECKER_ERROR, CheckerTarget)
// TBD



/*********************************************************************/
/* Base of intra-thread phase. This is also the ending of all targets (any number bigger than this
must not be a target). */
/*********************************************************************/



/* Reasons of taken by handling intra-thread phase. */
/* Reasons of taken by handling alloca instructions. */
DEF(INTRA_ALLOCA, IntraThread)

/* Reasons of taken by handling PHI instructions. */
DEF(INTRA_PHI, IntraThread)
DEF(INTRA_PHI_BR_CTRL_DEP, IntraThread)
// TBD

/* Reasons of taken by handling branch instructions. */
DEF(INTRA_BR_N_POSTDOM, IntraThread)
DEF(INTRA_BR_EVENT_BETWEEN, IntraThread)
DEF(INTRA_BR_WR_BETWEEN, IntraThread)
// TBD

/* Reasons of taken by handling return instructions. */
DEF(INTRA_RET_REG_OW, IntraThread) /* OVERWRITE. */
DEF(INTRA_RET_CALL_EVENT, IntraThread) /* Calling event only. */
DEF(INTRA_RET_WRITE_FUNC, IntraThread) /* Write func only. */
DEF(INTRA_RET_BOTH, IntraThread) /* Both calling event and writing func. */
// TBD

/* Reasons of taken by handling call instructions. */
DEF(INTRA_EXT_CALL_REG_OW, IntraThread)
DEF(INTRA_EXT_CALL_MOD_LIVE, IntraThread)
// TBD

/* Reasons of taken by handling load instructions. */
DEF(INTRA_LOAD_OW, IntraThread)
// TBD

/* Reasons of taken by handling store instructions. */
DEF(INTRA_STORE_OW, IntraThread) /* OVERWRITE. */
DEF(INTRA_STORE_ALIAS, IntraThread)
// TBD

/* Reasons of taken by handling all other non memory instructions. */
DEF(INTRA_NON_MEM, IntraThread)
// TBD

