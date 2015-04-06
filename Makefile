a.out: *.cpp
	$(CXX) $^ -lGLU -lGL -lglut -o $@

clean:
	rm a.out
