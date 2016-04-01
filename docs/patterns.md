Target language: C++

## Verbatim

- Operators
- Literals
- top-level declarations

# Patterns

## Program structure

- Package declaration: namespace

## Declaration

- Variable declaration:
  - `type id; [id = exp;]`
  - list of decs become multiple separate decs

- typedef

        type point struct { x, y float64 } --> typedef struct { float 64 x; float64 y; } point;

- function declaration

       !!! multiple return values -> wrapper class, list of objects?

## Types

- types

        int, bool   int, bool
        float64     double
        rune        char
        string      std::string

        slices      std::list
            append->push_back
        arrays
        structs

## Statements

- assignment
- declaration
- short var -> expand to regular var dec
- inc/dec -> use +=, -=
- print

        print_stmt      cout << stmt_1 << stmt_2 << ... << stmt_n;
        println_stmt    cout << stmt_1 << endl << stmt_2 << endl << ... << stmt_n;

- return
  - [01] if #exp, nothing to do
- if, else, else if
- switch

        switch x:= f() {
          case x<0: return -x
          default: return x
        }

        becomes

        {                     //x is scoped to the switch
          x = f();
          if(x<0) return -x;  // use if so we can eval exps
          else return x;
        }

- for
  - infinite: `while(0) {  }`
  - "while" loop: `while(exp) {  }`
  - 3-part for: same with '='
