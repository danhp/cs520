package main

func main() {
    x, y, z := 123, 1234, "test"

    {
        print(x)
        x := 2
        println("x: ", x);
        print(y)
    }

    print(z)
}
