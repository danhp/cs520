/*
 * JOOS is Copyright (C) 1997 Laurie Hendren & Michael I. Schwartzbach
 *
 * Reproduction of all or part of this software is permitted for
 * educational or research use on condition that this copyright notice is
 * included in any copy. This software comes with no warranty of any
 * kind. In no event will the authors be liable for any damages resulting from
 * use of this software.
 *
 * email: hendren@cs.mcgill.ca, mis@brics.dk
 */

/* iload x        iload x        iload x
 * ldc 0          ldc 1          ldc 2
 * imul           imul           imul
 * ------>        ------>        ------>
 * ldc 0          iload x        iload x
 *                               dup
 *                               iadd
 */

int simplify_multiplication_right(CODE **c)
{ int x,k;
  if (is_iload(*c,&x) &&
      is_ldc_int(next(*c),&k) &&
      is_imul(next(next(*c)))) {
     if (k==0) return replace(c,3,makeCODEldc_int(0,NULL));
     else if (k==1) return replace(c,3,makeCODEiload(x,NULL));
     else if (k==2) return replace(c,3,makeCODEiload(x,
                                       makeCODEdup(
                                       makeCODEiadd(NULL))));
  }
  return 0;
}

int simplify_multiplication_left(CODE **c) {
  int x, k;
  if (is_ldc_int(*c, &k) &&
        is_iload(next(*c), &x) &&
        is_imul(next(next(*c)))) {
    if (k == 0) return replace(c, 3, makeCODEldc_int(0, NULL));
    else if (k == 1) return replace(c, 3, makeCODEiload(x, NULL));
    else if (k == 2) return replace(c, 3, makeCODEiload(x,
                                          makeCODEdup(
                                          makeCODEiadd(NULL))));
  }
  return 0;
}

/* ldc 1
 * idiv
 * ------>
 * DROP THE INSTUCTIONS
 */
int simplify_division(CODE **c) {
  int k;
  if (is_ldc_int(*c, &k) &&
        is_idiv(next(*c))) {
    if (k == 1) {
      return replace(c, 2, NULL);
    }
  }
  return 0;
}

/* iload x     iload x
 * ldc 0       ldc k
 * iadd        iadd
 * ------>     ----->
 * iload x     iload x
 *             iinc x k
 */
int simplify_addition_right(CODE **c) {
  int x, k;
  if (is_iload(*c, &x) &&
        is_ldc_int(next(*c), &k) &&
        is_iadd(next(next(*c)))) {
    if (k == 0) {
      return replace(c, 3, makeCODEiload(x, NULL));
    } else if (k < 256) {
      return replace(c, 3, makeCODEiload(x, makeCODEiinc(x, 1, NULL)));
    }
  }
  return 0;
}
int simplify_addition_left(CODE **c) {
  int x, k;
  if (is_ldc_int(*c, &k) &&
        is_iload(next(*c), &x) &&
        is_iadd(next(next(*c)))) {
    if (k == 0) {
      return replace(c, 3, makeCODEiload(x, NULL));
    } else if (k < 256){
      return replace(c, 3, makeCODEiinc(x, k, NULL));
    }
  }
  return 0;
}

int simplify_addition_constants(CODE **c) {
  int x, y;
  if (is_ldc_int(*c, &x) &&
        is_ldc_int(next(*c), &y) &&
        is_iadd(next(next(*c)))) {
    if (x + y < 2147483647) {
      return replace(c, 3, makeCODEldc_int(x + y, NULL));
    }
  }
  return 0;
}

int simplify_addition_negative(CODE **c) {
  if (is_ineg(*c) && is_iadd(next(*c))) {
    return replace(c, 2, makeCODEisub(NULL));
  }
  return 0;
}

/* iload x
 * ldc 0
 * isub
 * ------>
 * iload x
 */
int simplify_subtraction_right(CODE **c) {
  int x, k;
  if (is_iload(*c, &x) &&
        is_ldc_int(next(*c), &k) &&
        is_isub(next(next(*c)))) {
    if (k == 0) {
      return replace(c, 3, makeCODEiload(x, NULL));
    }
  }
  return 0;
}
int simplify_subtraction_left(CODE **c) {
  int x, k;
  if (is_ldc_int(*c, &k) &&
        is_iload(next(*c), &x) &&
        is_isub(next(next(*c)))) {
    if (k == 0) {
      return replace(c, 3, makeCODEiload(x, NULL));
    }
  }
  return 0;
}

/* dup
 * astore x
 * pop
 * -------->
 * astore x
 */
int simplify_astore(CODE **c)
{ int x;
  if (is_dup(*c) &&
      is_astore(next(*c),&x) &&
      is_pop(next(next(*c)))) {
     return replace(c,3,makeCODEastore(x,NULL));
  }
  return 0;
}

/* dup
 * istore x
 * pop
 * -------->
 * astore x
 */
int simplify_istore(CODE **c)
{ int x;
  if (is_dup(*c) &&
      is_istore(next(*c),&x) &&
      is_pop(next(next(*c)))) {
     return replace(c,3,makeCODEistore(x,NULL));
  }
  return 0;
}

/* iinc x k
 * astore x
 * ----->
 * iinc x k
 */
int simplify_inc_astore(CODE **c) {
  int x1, x2, x3, k;
  if (is_iload(*c, &x1) &&
      is_iinc(next(*c), &x2, &k) &&
      is_astore(next(next(next(*c))), &x3) &&
      x1 == x2 && x2 == x3) {
    return replace(c, 2, makeCODEiinc(x1, k, NULL));
  }
  return 0;
}

/* iinc x k
 * istore x
 * ----->
 * iinc x k
 */
int simplify_inc_istore(CODE **c) {
  int x, y, k;
  if (is_iinc(*c, &x, &k) && is_istore(next(*c), &y)) {
    return replace(c, 2, makeCODEiinc(x, k, NULL));
  }
  return 0;
}

/* ireturn
 * nop
 * -------->
 * ireturn
 */
int simplify_nop(CODE **c)
{
  if (is_nop(*c))
  {
    return replace(c,1,NULL);
  }
  return 0;
}

/* Removes labels with 0 references to it */
int simplify_deadlabels(CODE **c) {
  int l;
  if (is_label(*c, &l) && deadlabel(l)) {
    return replace(c,1,NULL);
  }
  return 0;
}

/* goto L1
 * ...
 * L1:
 * goto L2
 * ...
 * L2:
 * --------->
 * goto L2
 * ...
 * L1:    (reference count reduced by 1)
 * goto L2
 * ...
 * L2:    (reference count increased by 1)
 */
int simplify_goto_goto(CODE **c)
{ int l1,l2;
  if (is_goto(*c,&l1) && is_goto(next(destination(l1)),&l2) && l1>l2) {
    droplabel(l1);
    copylabel(l2);
    return replace(c,1,makeCODEgoto(l2,NULL));
  }
  return 0;
}

/* goto L1
 * ...
 * L1:
 * L2:
 * --------->
 * goto L2
 * ...
 * L1:    (reference count reduced by 1)
 * L2:    (reference count increased by 1)
 */
int simplify_goto_label_label(CODE **c)
{ int l1,l2,l3;
  if (is_goto(*c,&l1) &&
      is_label(destination(l1), &l2) &&
      is_label(next(destination(l1)),&l3) &&
      l1>l3) {
    droplabel(l1);
    copylabel(l3);
    return replace(c,1,makeCODEgoto(l3,NULL));
  }
  return 0;
}


/*int simplify_dup_ifne_pop(CODE **c)
{
  int l1;
  if(is_dup(*c)
     && is_ifne(next(*c), &l1)
     && is_pop(next(next(*c)))
  ){
    return replace(c,3,makeCODEifne(l1, NULL));
  }
  return 0;
}*/

/*int simplify_icmplt_then_else(CODE **c)
{
  CODE *c2;
  int i;
  int l1;
  int l2;
  int l3;
  int l4;
  int l5;
  if(is_if_icmplt(*c,&l1)
    && is_goto(next(next(*c)),&l2)
    && is_label(next(next(next(*c))),&l3)
    && is_label(next(next(next(next(next(*c))))),&l4)
    && is_ifeq(next(next(next(next(next(next(*c)))))),&l5)
  ){
    if(l1 == l3 && l2 == l4)
    {
      for(i=0;i<2;i++)
      {
        c = &((*c)->next);
      }
      replace(c,1,makeCODEgoto(l5, NULL));

      for(i=0;i<3;i++)
      {
        c = &((*c)->next);
      }
      return replace(c,2,NULL);
    }
  }
  return 0;
}*/

int simplify_dup_ifeq_pop(CODE **c)
{
  int l1;
  int l2;
  int keepLooping;
  int toReturn;
  toReturn = 0;
  if(is_dup(*c)
     && is_ifeq(next(*c),&l1)
     && is_pop(next((next(*c))))
  ){
printf("one less\n");
     replace(c,3,makeCODEifeq(l1,NULL));
     keepLooping = 1;
     toReturn = 0;
     while(keepLooping)
     {
       c = &((*c)->next);
       if(is_label(*c, &l2))
       {
         if(l1 == l2)
         {
           toReturn = replace(c,2,makeCODElabel(l1,makeCODEdup(NULL)));
           keepLooping = 0;
         }
       }
     }
  }
  return toReturn;
}

/* Don't check cast as they are already done at the typechecker */
int simplify_checkcast(CODE **c) {
  char *args;
  if (is_checkcast(*c, &args)) {
    return replace(c, 1, NULL);
  }
  return 0;
}

/* ldc x
 * ldc k  with x=k
 * ------->
 * ldc x
 */
int simplify_dup_ldc_int(CODE **c) {
  int k,j;
  if (is_ldc_int(*c, &k) &&
      is_ldc_int(next(*c), &j) &&
      k==j) {
    return replace(c,2,makeCODEldc_int(k,NULL));
  }
  return 0;
}

/* iload x
 * iload y, with y=x
 * ------->
 * iload x
 */
int simplify_dup_iload(CODE **c) {
  int x, y ;
  if (is_iload(*c, &x) &&
      is_iload(next(*c), &y) &&
      x==y) {
    return replace(c,2,makeCODEiload(x,NULL));
  }
  return 0;
}

/* aload x
 * aload y, with y=x
 * ------->
 * aload x
 */
int simplify_dup_aload(CODE **c) {
  int x, y;
  if (is_aload(*c, &x) &&
      is_aload(next(*c), &y) &&
      x==y) {
    return replace(c,2,makeCODEaload(x,NULL));
  }
  return 0;
}

#define OPTS 21

OPTI optimization[OPTS] = {simplify_multiplication_right,
                           simplify_multiplication_left,
                           simplify_division,
                           simplify_addition_right,
                           simplify_addition_left,
                           simplify_addition_constants,
                           simplify_addition_negative,
                           simplify_subtraction_right,
                           simplify_subtraction_left,
                           simplify_astore,
                           simplify_istore,
                           simplify_inc_astore,
                           simplify_inc_istore,
                           simplify_deadlabels,
                           simplify_goto_goto,
                           simplify_goto_label_label,
                           simplify_nop,
                           simplify_checkcast,
                           simplify_dup_aload,
                           simplify_dup_iload,
                           simplify_dup_ldc_int
                          };
