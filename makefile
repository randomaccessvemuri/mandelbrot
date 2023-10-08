LIBS = -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_imgcodecs -lgmp -lgmpxx -lomp
INCLUDE = -I/usr/local/include/opencv4/
SOURCE = computePixel.cpp fundamentals/numbers/complexNumber.cpp

FLAGS = -O3 -std=c++17


build: computePixel.cpp fundamentals/numbers/complexNumber.cpp
	clang++ $(LIBS) $(INCLUDE) -o test $(SOURCE) $(FLAGS)

build_run: $(SOURCE)
	clang++ $(LIBS) $(INCLUDE) -o test $(SOURCE) $(FLAGS)
	./test