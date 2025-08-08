BUILD_DIR := build-linux

SRC := src/main.cpp
SRC += externals/cpr/cpr/accept_encoding.cpp externals/cpr/cpr/async.cpp externals/cpr/cpr/auth.cpp externals/cpr/cpr/callback.cpp externals/cpr/cpr/cert_info.cpp externals/cpr/cpr/cookies.cpp externals/cpr/cpr/cprtypes.cpp externals/cpr/cpr/curl_container.cpp externals/cpr/cpr/curlholder.cpp externals/cpr/cpr/error.cpp externals/cpr/cpr/file.cpp externals/cpr/cpr/multipart.cpp externals/cpr/cpr/parameters.cpp externals/cpr/cpr/payload.cpp externals/cpr/cpr/proxies.cpp externals/cpr/cpr/proxyauth.cpp externals/cpr/cpr/session.cpp externals/cpr/cpr/threadpool.cpp externals/cpr/cpr/timeout.cpp externals/cpr/cpr/unix_socket.cpp externals/cpr/cpr/util.cpp externals/cpr/cpr/response.cpp externals/cpr/cpr/redirect.cpp externals/cpr/cpr/interceptor.cpp externals/cpr/cpr/ssl_ctx.cpp externals/cpr/cpr/curlmultiholder.cpp externals/cpr/cpr/multiperform.cpp
OBJ := $(SRC:%=$(BUILD_DIR)/%.o)

INCLUDE := -Isrc -Iexternals/sol2/include/ -Iexternals/lua/src -Iexternals/json/include
INCLUDE += -Iexternals/cpr/include
LDFLAGS := -lraylib -lcurl
CXXFLAGS += $(INCLUDE) -std=c++17
CFLAGS += $(INCLUDE) -DLUA_COMPAT_5_3

LUA_SRC := externals/lua/src/lapi.c externals/lua/src/lcode.c externals/lua/src/lctype.c externals/lua/src/ldebug.c externals/lua/src/ldo.c externals/lua/src/ldump.c externals/lua/src/lfunc.c externals/lua/src/lgc.c externals/lua/src/llex.c externals/lua/src/lmem.c externals/lua/src/lobject.c externals/lua/src/lopcodes.c externals/lua/src/lparser.c externals/lua/src/lstate.c externals/lua/src/lstring.c externals/lua/src/ltable.c externals/lua/src/ltm.c externals/lua/src/lundump.c externals/lua/src/lvm.c externals/lua/src/lzio.c externals/lua/src/lauxlib.c externals/lua/src/lbaselib.c externals/lua/src/lcorolib.c externals/lua/src/ldblib.c externals/lua/src/liolib.c externals/lua/src/lmathlib.c externals/lua/src/loadlib.c externals/lua/src/loslib.c externals/lua/src/lstrlib.c externals/lua/src/ltablib.c externals/lua/src/lutf8lib.c externals/lua/src/linit.c
LUA_OBJ := $(LUA_SRC:%=$(BUILD_DIR)/%.o)

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
