FLUTTER_BIN := $(shell which flutter 2>/dev/null)
FLUTTER_SDK := $(dir $(FLUTTER_BIN))cache/artifacts/engine/linux-arm64/

ifndef FLUTTER_BIN
    $(error `flutter` not found)
endif

.PHONY: all
all: glutter bundle

glutter: src/glutter.c src/flutter_embedder.h
	@$(CC) -o $@ $< -L$(FLUTTER_SDK) -lglfw -lflutter_engine -Wl,-rpath=$(FLUTTER_SDK)

.PHONY: bundle
bundle: 
	@flutter build bundle

.PHONY: run
run: all
	@./glutter build/flutter_assets $(FLUTTER_SDK)icudtl.dat

.PHONY: clean
clean:
	-@rm glutter
	-@flutter clean
