// All ways to declare types
package types

type num int

type parens (string)

type point struct {
    x, y float64
}

type ()

type (
    basicOperand int
    point3D struct {
        x, y, z float64
    }
)

type int_list [19348] int

type string_slice [] float

// single line declarations
type (first int; second float64;)
type (first int; second float64)
