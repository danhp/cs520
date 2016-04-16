package main

func main() {
    var (
        a int
        b float64
        c rune
        d string
        e bool
        f [3][2] struct {a int}
        g struct {a, b float64; c struct {a int;};}
    )
}
