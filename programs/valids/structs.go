package main

func main() {
    type intAlias int
    type s struct {
        a string
    }
    var a struct {
        b, c float64
        a struct {
            a intAlias
            c int
        }
    }
    _ = (((a))).b
}
