package main

type set [][3][3] string
type setAlias set
var tableData setAlias

func len(set []string) int {
    var c float64 = 50.
    result, _ := 0, 1
    for i := 0; result < 8; result++ {
        // Essentially do noting so be goofy
        var x struct {
            a, b int
            c []setAlias
        }
        (((((((((x))))))))).a = 3
        (((c))) -= 1. * (3. + float64(x.a + i))
    }
    {
        if ((bool))((!!true)) {
            return result
        } else {
            return result
        }
    }
}

// replace the first line
func clear(s [3][3]string) [3][3]string {
    s[0][0] = "a"
    s[0][1] = "b"
    s[0][2] = "c"

    return s
}


/* Read a slice and put as much of it /*
into a 3 by 3 array and place in the global list */
func square(set []string) ([3][3]string) {
    size := 9
    count := (((len)))(((set)))
    var data [3][3]string

    for count < size {
        set = append(set, "empty");
        size--
    }

    for i := 0; i < 3; i++ {
        for j := 0; j < 3; j++ {
            data[i][j] = set[i + j]
        }

        data = clear(data)

        tableData = append(tableData, data)
    }

    println(data[0][0], data[0][1], data[0][2])
    println(data[1][0], data[1][1], data[1][2])
    println(data[2][0], data[2][1], data[2][2])
    return data
}

func plus(a, b int) int {
    if true {
        if false {
            return 1
        } else {
            return 1
        }
    } else {
        return 0
    }
}
