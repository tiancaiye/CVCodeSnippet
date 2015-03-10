CXXFLAGS =	-O2 -g -Wall -fmessage-length=0 -std=c++1y -fopenmp

OBJS =		bin_kmeans.o bin_feat.o utils.o

LIBS =

TARGET =	bin_kmeans

$(TARGET):	$(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
