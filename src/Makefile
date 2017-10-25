CC = g++
CXXFLAGS = -std=c++11 -Wall -O2
SOURCE_FILES =  serv.o http.o event.o  conf.o

serv : $(SOURCE_FILES)
	$(CC) -o serv $(SOURCE_FILES) $(CXXFLAGS)

%.o:%.cpp
	$(CC) -c $(CXXFLAGS) -o $@ $<

clean:
	rm *.o
	rm serv
