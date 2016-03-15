package main

func plus() int {
    var a struct{b int;}
    a.b = 1
    return (a).a
}

func main () {
    _ = 0
}
