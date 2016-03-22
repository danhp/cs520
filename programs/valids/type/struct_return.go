package main

func f() struct {a, b int;} {
    var a struct {a, b int;}
    a.a = 1
    return a
}
func main() {
    print(f().a)
}
