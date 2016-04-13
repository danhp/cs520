package main

// set the iTHprimeNumber variable to know the iTH prime number.
// make it big to have a long computation time.

func main() {
  var iTHprimeNumber int = 10

  var primeNumbers []int
  var p [3]int
  primeNumbers = append(primeNumbers, 2)

  var sqrt int = 2
  var square int = 4
  var counter int = 1
  var number int = 2
  var lastPrime int = 2
  var i int = 0
  var temp int = 0
  var isPrime int = 1
  var numPrimes int = 1

  for counter < iTHprimeNumber {
    number++

    if number > square {
      sqrt++
      square = sqrt*sqrt
    }

    isPrime = 1
    for i = 0; i<numPrimes; i++ {
      temp = primeNumbers[i]

      if temp > sqrt {
        break
      }

      if number % temp  == 0 {
        isPrime = 0
        break
      }
    }

    if isPrime == 1 {
      primeNumbers = append(primeNumbers, number)
      numPrimes++
      lastPrime = number
      counter++
    }
  }

  println(lastPrime)
}
