package return_switch

func main() {
    plus(1, 2)
}

func plus(a, b int) int {
    switch a {
    case 1:
        return 1
        break
    default:
        return 1
    }
}
