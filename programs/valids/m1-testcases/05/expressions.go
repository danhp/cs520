/* Structs. */

package main

var x, y, i = 0, 0, 1

var a [10] int

func f() int {
	return 100
}

var b, c = 3, 4

func g() bool {
	return true
}

func h() bool {
	return false
}

func main() {
	a[i] = x

	_ = +x
	_ = 23 + 3 * a[i]
	_ = x <= f()
	_ = ^b >> c
	_ = g() || h()
	_ = x == y+1 && c > 0
}
