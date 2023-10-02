.PHONY: all
all: glutter flutter

glutter: src/glutter.c src/flutter_embedder.h
	@$(CC) -o $@ $< -Lsrc -lglfw -lflutter_engine -Wl,-rpath=\$$ORIGIN/src

.PHONY: flutter
flutter:
	@flutter build bundle

.PHONY: run
run:
	@./glutter build/flutter_assets src/icudtl.dat

.PHONY: clean
clean:
	-@rm glutter
	-@flutter clean

