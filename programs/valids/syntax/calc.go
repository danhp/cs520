package main

/* A simple calculator program */
/* To use, just call /*
    compute(op, a, b)
*/

type dummyFloat float64
type exponent dummyFloat

func power(a float64, b float64) float64 {
  var check dummyFloat
  var empty string
  var ret float64 = 1
  var power = exponent(b)

  for power > check {
    ret, power = ret * a, power - 1.0
  }

  return ret
}

func compute(op int, a float64, b float64) float64 {
  switch x := 1; op {
  case 0:
    return a+b
  case 1:
    return a-b
  case 2:
    return a*b
  case 3:
    return a/b
  case 4:
    return power(a,b)
  case 5:
    return -a
  default:
    println("Invalid operator")
    break
  }

  switch {
  default:
      println("hahaha");
  }
  return -1
}

func main() {
  println(",------------,")
  println("| CALCULATOR |")
  println("'------------'")

  println(compute(0,1.,1.), compute(1, 5., 4.))
  println(compute(2,10.,22.0))
  println(compute(3, 19.11, .1))
  println(compute(4, 2., 4.))        /* 2^4 */
  print(compute(4, -10., 0.), "END")     /* negate -10 */
}
