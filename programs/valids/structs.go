package main

func main() {
    type intAlias int
    var a struct {
        a struct {
            a intAlias
            c int
        }
        b, c float64
    }
    _ = (((a))).b
}
