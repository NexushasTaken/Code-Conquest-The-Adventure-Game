BUILD_DIR := build-linux
SRC := src/main.cpp
OBJ := $(SRC:%=$(BUILD_DIR)/%.o)

INCLUDE := -Isrc -Iexternals/sol2/include/ -Iexternals/lua/src -Iexternals/json/include
LDFLAGS := -lraylib -lcurl
CXXFLAGS += $(INCLUDE) -std=c++17
CFLAGS += $(INCLUDE) -DLUA_COMPAT_5_3

LUA_SRC := externals/lua/src/lapi.c externals/lua/src/lcode.c externals/lua/src/lctype.c externals/lua/src/ldebug.c externals/lua/src/ldo.c externals/lua/src/ldump.c externals/lua/src/lfunc.c externals/lua/src/lgc.c externals/lua/src/llex.c externals/lua/src/lmem.c externals/lua/src/lobject.c externals/lua/src/lopcodes.c externals/lua/src/lparser.c externals/lua/src/lstate.c externals/lua/src/lstring.c externals/lua/src/ltable.c externals/lua/src/ltm.c externals/lua/src/lundump.c externals/lua/src/lvm.c externals/lua/src/lzio.c externals/lua/src/lauxlib.c externals/lua/src/lbaselib.c externals/lua/src/lcorolib.c externals/lua/src/ldblib.c externals/lua/src/liolib.c externals/lua/src/lmathlib.c externals/lua/src/loadlib.c externals/lua/src/loslib.c externals/lua/src/lstrlib.c externals/lua/src/ltablib.c externals/lua/src/lutf8lib.c externals/lua/src/linit.c
LUA_OBJ := $(LUA_SRC:%=$(BUILD_DIR)/%.o)

SRC += externals/rlImGui/rlImGui.cpp externals/imgui/imgui.cpp externals/imgui/imgui_draw.cpp externals/imgui/imgui_tables.cpp externals/imgui/imgui_widgets.cpp externals/imgui/imgui_demo.cpp

OBJS := $(OBJ) $(LUA_OBJ)

.PHONE: all
all: $(BUILD_DIR)/$(NAME)

$(BUILD_DIR)/%.cpp.o: %.cpp
	@mkdir -p $(@D)
	g++ -c -o $@ $< $(CXXFLAGS)

$(BUILD_DIR)/%.c.o: %.c
	@mkdir -p $(@D)
	gcc -c -o $@ $< $(CFLAGS)

$(BUILD_DIR)/$(NAME): $(OBJ) $(LUA_OBJ)
	@mkdir -p build
	g++ -o $@ $^ $(CXXFLAGS) $(LDFLAGS)

clean:
	rm -rf $(BUILD_DIR)
