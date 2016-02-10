#include "weed.h"

void weedPROGRAM(PROGRAM *program) {
  if (program->top_decl != NULL)
    weedTOP_DECL(program->top_decl);
}

void weedTOP_DECL(TOP_DECL *decl) {
  if (decl->next) weedTOP_DECL(decl->next);

  switch(decl->kind) {
    case top_varK:
      weedTOPvar(decl->val.varT);
      break;
    case top_typeK:
      break;
    case top_funcK:
      weedTOPfunc(decl->val.funcT);
      break;
    default:
      return; /* nothing to do */
  }
}

void  weedTOPvar(VAR_DECL *decl) {
  if (decl) {
    if (decl->next) weedTOPvar(decl->next);
    if (decl->exp) weedVARdecl(decl->id, decl->exp, decl->loc);
  }
}

/* #id = #exp */
void  weedVARdecl(ID *id, EXP *exp, YYLTYPE loc) {
  while (id && exp) {
    id = id->next;
    exp = exp->next;
  }

  if (!(id == NULL && exp == NULL))
    printError("Number of ids doesn't match number of expressions", loc);
}

void weedTOPfunc(FUNC_DECL *func)
{
  /* weedFUNC_SIGN(func->signature); */
  printf("in top func %s", func->name);
  weedSTMT(func->body);
}

void weedSTMT(STMT *stmt) {
    printf("%u", stmt->kind);
    if (stmt->next) {
      printf("has next");
      weedSTMT(stmt->next);
    }
  switch(stmt->kind) {
    case switchK:
      weedSTMTswitch(stmt->val.switchS.body);
      break;
    default:
      printf("Default");
      break;
  }
}

/* only 1 default case */
void weedSTMTswitch(CASE_DECL *body) {
  printf("weeding");
  /* count number of default cases */
  int numDefault = 0;
  while (body) {
    if ((body)->kind == defaultK)
      numDefault += 1;
    body = body->next;
  }

  if (numDefault > 1)
    printError("Too many default cases", body->loc);
}
