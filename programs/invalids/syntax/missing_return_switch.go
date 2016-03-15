package return_switch

func main() {
    plus(1, 2)
}

func plus(a, b int) int {
    switch a {
    case 1:
        {
            break
        }
        return 1
    default:
        return 1
    }
}
