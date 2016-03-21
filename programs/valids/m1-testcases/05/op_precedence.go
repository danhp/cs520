/* Operator precidence */
package main

func main() {
	x, y, z = 1, 2, 3
	_ = x / y * z //Should be the same as (x / y) * z
}
