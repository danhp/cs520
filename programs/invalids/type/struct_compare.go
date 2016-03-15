package main

func main() {
    var a struct {a int;}
    var b struct {b int;}
    if a == b {
        _ = 0
    }
}
