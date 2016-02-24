// All ways to declare types
package types

type num int

type parens (string)

type point struct {
    x, y float64
}

type emptyStruct struct {}

type ()

type (
    basicOperand int
    point3D struct {
        x, y, z float64
        a int
        _, str string
    }
)

type int_list [19348] int

type string_slice [] somefloat

type _ int

// single line declarations
type (first int; second float64;)
type (first int; second float64)

// check indentations
func main() {
    type a float64
}
