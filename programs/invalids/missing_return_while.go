package missing_return_while

func main() {
    plus(1, 2)
}

func plus(a, b int) int {
    for {
        break
        return 1
    }
}
