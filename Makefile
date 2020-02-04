a:a.cpp GWindow.h.gch GString.o foo.h.gch
	g++ -g a.cpp GString.o -lX11 -o a
	./a
GWindow.h.gch:GWindow.h
	g++ -g GWindow.h
foo.h.gch:foo.h
	g++ foo.h
GString.o:GString.h.gch GString.cpp
	g++ -c GString.h GString.cpp
GString.h.gch:GString.h
	g++ GString.h
run:a
	./a
