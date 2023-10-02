FLUTTER_BIN := $(dir $(shell which flutter))
FLUTTER_SDK := $(FLUTTER_BIN)cache/artifacts/engine/linux-arm64/

.PHONY: all
all: glutter flutter

glutter: src/glutter.c src/flutter_embedder.h
	@$(CC) -o $@ $< -L$(FLUTTER_SDK) -lglfw -lflutter_engine -Wl,-rpath=$(FLUTTER_SDK)

.PHONY: flutter
flutter:
	@flutter build bundle

.PHONY: run
run: all
	@./glutter build/flutter_assets $(FLUTTER_SDK)icudtl.dat

.PHONY: clean
clean:
	-@rm glutter
	-@flutter clean

