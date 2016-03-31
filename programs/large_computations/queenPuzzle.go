package queenPuzzle

// This is the classic queens problem. Set the chess grid size (in main class) to a number and the program will
// place that many queens on the chessboard in a way that they wont be attacking eachother.

type point struct {
  x,y int
}

var queens []point
var chessGridSize int

//returns 1 if everything is good
func queenRecusion(index int) int {
  if index >= chessGridSize {
    return 1
  }

  var i int
  var j int
  var p point
  var allgood int
  var p2 point
  var solutionFound = 0

  for i := 0; i<chessGridSize; i++ {
    p = queens[i]
    p.x = i

    allgood = 1
    for j := 0; j<index; j++ {
      p2 = queens[j]
      if p.y == p2.y || p2.y - p.y == p2.x - p.x || p2.y - p.y == p.x - p2.x {
        allgood = 0
        break
      }
    }

    if allgood {
      if queenRecursion(index+1) {
        print("(")
        print(p.x)
        print(",")
        print(p.y)
        println(")")
        solutionFound = 1
      }
    }
  }

  return solutionFound
}

func main() {
  chessGridSize = 8

  var i int
  for i := 0; i<chessGridSize; i++ {
    var p point
    p.x = 0
    p.y = i
    queens = append(queens, p)
  }

  queenRecustion(0)
}
