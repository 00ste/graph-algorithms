BUILD_DIR = build

all: $(BUILD_DIR)/graph

$(BUILD_DIR)/graph: graph.c $(BUILD_DIR)
	clang -Wall -Wextra graph.c -o $(BUILD_DIR)/graph

$(BUILD_DIR):
	mkdir $(BUILD_DIR)

clean:
	rm -r build