all: clean
	$(PEEPDIR)/joosc -O BacktrackSolver.java SudokuSolver.java Benchmark.java

java:
	javac BacktrackSolver.java SudokuSolver.java Benchmark.java

clean:
	rm -rf *.class *.j *~

run:
	java Benchmark < in1
