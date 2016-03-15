// Various allowed switch statements
package main

func main() {
	i, a := 0, 123
	exp, condition, otherCondition := 0, 0, 1

	switch i++; a {
	}

	switch exp {
	case condition:

	case otherCondition:
	default:
		break;
	}

	// single line
	switch a { case condition: }
}
