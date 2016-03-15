// various allowed assignements
package main

func main() {
    var a, b int
    var x [2]int
    a = 1
    b += a
    a, b = 3, 5
    x[1], x[0] = 0, 2
}
