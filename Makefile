cpp_cli = client.cpp
cpp_serv = server.cpp

o_cli = $(cpp_cli:.cpp=.o)
o_serv = $(cpp_serv:.cpp=.o)

FLAGS = -std=c++11 -Wall -Wshadow -Werror -O2
LDFLAGS = -lpthread

all: client server

client: $(o_cli)
	c++ -o $@ $^

server: $(o_serv)
	c++ -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	c++ $(FLAGS) -c $< -o $@
	c++ -MM $< > $*.d

-include $(o_cli:.o=.d)
-include $(o_serv:.o=.d)

clean:
	rm -f *.o *.d client server
