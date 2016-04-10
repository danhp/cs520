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

/* iload x
 * ldc 0
 * iadd
 * ------>
 * iload x
 *
 */
int simplify_addition_right(CODE **c) {
  int x, k;
  if (is_iload(*c, &x) &&
        is_ldc_int(next(*c), &k) &&
        is_iadd(next(next(*c)))) {
    if (k == 0) {
      return replace(c, 3, makeCODEiload(x, NULL));
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
      return replace(c, 3, makeCODEiload(x, makeCODEiinc(x, k, NULL)));
    }
  }
  return 0;
}

/* ldc x
 * ldc y
 * iadd
 * ----->
 * ldc x + y (if x + y smaller than max int)
 */
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

/* ineg
 * iadd
 * --->
 * isub
 */
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

/* iload x
 * iinc x k
 * astore x
 * ----->
 * iinc x k
 */
int simplify_inc_astore(CODE **c) {
  int x1, x2, x3, k;
  if (is_iload(*c, &x1) &&
      is_iinc(next(*c), &x2, &k) &&
      is_astore(next(next(*c)), &x3) &&
      x1 == x2 && x2 == x3) {
    return replace(c, 2, makeCODEiinc(x1, k, NULL));
  }
  return 0;
}

/* iload x
 * iinc x k
 * istore x
 * ----->
 * iinc x k
 */
int simplify_inc_istore(CODE **c) {
  int x1, x2, x3, k;
  if (is_iload(*c, &x1) &&
      is_iinc(next(*c), &x2, &k) &&
      is_istore(next(next(*c)), &x3) &&
      x1 == x2 && x2 == x3) {
    return replace(c, 2, makeCODEiinc(x1, k, NULL));
  }
  return 0;
}

/* nop
 * --------> (remove the nop)
 * 
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


/* ifeq L1  (or any other type of if condition)
 * iconst_0
 * goto L2
 * L1:
 * iconst_1
 * L2:
 * ifeq L3
 * ---------> (we only do this if L2 and L3 is "uniquelabel")
 * ifeq L1
 * goto L3
 * L1:
 */

/* This is sound because we do not change the stack or the code that gets
 * executed. the additional ifeq L3 removes the previous iconst in the
 * original code. The new code just doesnt add them in the first place.
 * The new code also ends up at the same place as the original code no
 * matter the type of if conditions used.
 * And since we check for uniqueness of L2 and L3, removing or changing
 * them wont break the code.
 */

int simplify_if_then_else(CODE **c)
{
  int l1;
  int c1;
  int l2;
  int l3;
  int c2;
  int l4;
  int l5;
  int t;
  CODE *code;
  t=0;

  if(is_ifeq(*c,&l1)){t=1;}
  else if(is_if_acmpne(*c,&l1)){t=2;}
  else if(is_if_icmpeq(*c,&l1)){t=3;}
  else if(is_if_icmpgt(*c,&l1)){t=4;}
  else if(is_if_icmplt(*c,&l1)){t=5;}
  else if(is_if_icmple(*c,&l1)){t=6;}
  else if(is_if_icmpge(*c,&l1)){t=7;}

  if(t>0
    && is_ldc_int(next(*c),&c1)
    && is_goto(next(next(*c)),&l2)
    && is_label(next(next(next(*c))),&l3)
    && is_ldc_int(next(next(next(next(*c)))),&c2)
    && is_label(next(next(next(next(next(*c))))),&l4)
    && is_ifeq(next(next(next(next(next(next(*c)))))),&l5)
  ){
    if(l1 == l3 && l2 == l4 && c1==0 && c2==1 && uniquelabel(l3) && uniquelabel(l4))
    {
      code = makeCODEgoto(l5,makeCODElabel(l3,NULL));

      if(t==1){return replace(c,7,makeCODEifeq(l1,code));}
      else if(t==2){return replace(c,7,makeCODEif_acmpne(l1,code));}
      else if(t==3){return replace(c,7,makeCODEif_icmpeq(l1,code));}
      else if(t==4){return replace(c,7,makeCODEif_icmpgt(l1,code));}
      else if(t==5){return replace(c,7,makeCODEif_icmplt(l1,code));}
      else if(t==6){return replace(c,7,makeCODEif_icmple(l1,code));}
      else if(t==7){return replace(c,7,makeCODEif_icmpge(l1,code));}
    }
  }
  return 0;
}


#define OPTS 18

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
                           simplify_if_then_else
                          };
