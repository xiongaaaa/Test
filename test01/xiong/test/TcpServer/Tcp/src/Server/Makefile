SRC = HttpServer.cpp HttpClient.cpp HttpResponse.cpp Server.cpp
httpserver:$(SRC)
	g++ $(SRC) -o $@ -I ../xsocket -std=c++11 -lpthread -lxsocket -L ../xsocket -g
	