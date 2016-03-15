package main

func noNewDeclaration(a, b int) {
    a, b := 1, 0
    _ = a
}
