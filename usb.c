#include <lua.h>
#include <lauxlib.h>
#include <libusb/libusb.h>

/****************************************************************************/

static int lua__usberror(lua_State* L, int usberror)
{
	lua_pushnil(L);
	/*
	switch (usberror)
	{
	case LIBUSB_ERROR_IO: lua_pushstring(L, "io"); break;
	case LIBUSB_ERROR_INVALID_PARAM: lua_pushstring(L, "invalid_param"); break;
	case LIBUSB_ERROR_ACCESS: lua_pushstring(L, "access"); break;
	case LIBUSB_ERROR_NO_DEVICE: lua_pushstring(L, "no_device"); break;
	case LIBUSB_ERROR_NOT_FOUND: lua_pushstring(L, "not_found"); break;
	case LIBUSB_ERROR_BUSY: lua_pushstring(L, "busy"); break;
	case LIBUSB_ERROR_TIMEOUT: lua_pushstring(L, "timeout"); break;
	case LIBUSB_ERROR_OVERFLOW: lua_pushstring(L, "overflow"); break;
	case LIBUSB_ERROR_PIPE: lua_pushstring(L, "pipe"); break;
	case LIBUSB_ERROR_INTERRUPTED: lua_pushstring(L, "interrupted"); break;
	case LIBUSB_ERROR_NO_MEM: lua_pushstring(L, "no_mem"); break;
	case LIBUSB_ERROR_NOT_SUPPORTED: lua_pushstring(L, "not_supported"); break;
	default:
	case LIBUSB_ERROR_OTHER: lua_pushstring(L, "other"); break;
	}
	*/
	lua_pushstring(L, libusb_strerror(usberror));
	lua_pushnumber(L, usberror);
	return 3;
}

/****************************************************************************/

libusb_context* luaL_check_context(lua_State* L, int index)
{
	struct libusb_context* ctx;
	ctx = *(struct libusb_context**)luaL_checkudata(L, index, "libusb_context");
	if (!ctx)
	{
		luaL_argerror(L, index, "context has already been deinitialized");
		return NULL;
	}
	return ctx;
}

void lua_push_context(lua_State* L, libusb_context* value)
{
	libusb_context** udata;
	udata = (libusb_context**)lua_newuserdata(L, sizeof(libusb_context*));
	*udata = value;
	luaL_getmetatable(L, "libusb_context");
	lua_setmetatable(L, -2);
}

libusb_device* luaL_check_device(lua_State* L, int index)
{
	libusb_device* dev;
	dev = *(struct libusb_device**)luaL_checkudata(L, index, "libusb_device");
	if (!dev)
	{
		luaL_argerror(L, index, "device has already been unreferenced");
		return NULL;
	}
	return dev;
}

void lua_push_device(lua_State* L, libusb_device* value)
{
	libusb_device** udata;
	udata = (libusb_device**)lua_newuserdata(L, sizeof(libusb_device*));
	*udata = value;
	luaL_getmetatable(L, "libusb_device");
	lua_setmetatable(L, -2);
}

libusb_device_handle* luaL_check_device_handle(lua_State* L, int index)
{
	libusb_device_handle* handle;
	handle = *(libusb_device_handle**)luaL_checkudata(L, index, "libusb_device_handle");
	if (!handle)
	{
		luaL_argerror(L, index, "device handle has already been closed");
		return NULL;
	}
	return handle;
}

void lua_push_device_handle(lua_State* L, libusb_device_handle* value)
{
	libusb_device_handle** udata;
	udata = (libusb_device_handle**)lua_newuserdata(L, sizeof(libusb_device_handle*));
	*udata = value;
	luaL_getmetatable(L, "libusb_device_handle");
	lua_setmetatable(L, -2);
}

void lua_push_device_descriptor(lua_State* L, struct libusb_device_descriptor* value)
{
	lua_createtable(L, 0, 14);
	lua_pushnumber(L, value->bLength); lua_setfield(L, -2, "bLength");
	lua_pushnumber(L, value->bDescriptorType); lua_setfield(L, -2, "bDescriptorType");
	lua_pushnumber(L, value->bcdUSB); lua_setfield(L, -2, "bcdUSB");
	lua_pushnumber(L, value->bDeviceClass); lua_setfield(L, -2, "bDeviceClass");
	lua_pushnumber(L, value->bDeviceSubClass); lua_setfield(L, -2, "bDeviceSubClass");
	lua_pushnumber(L, value->bDeviceProtocol); lua_setfield(L, -2, "bDeviceProtocol");
	lua_pushnumber(L, value->bMaxPacketSize0); lua_setfield(L, -2, "bMaxPacketSize0");
	lua_pushnumber(L, value->idVendor); lua_setfield(L, -2, "idVendor");
	lua_pushnumber(L, value->idProduct); lua_setfield(L, -2, "idProduct");
	lua_pushnumber(L, value->bcdDevice); lua_setfield(L, -2, "bcdDevice");
	lua_pushnumber(L, value->iManufacturer); lua_setfield(L, -2, "iManufacturer");
	lua_pushnumber(L, value->iProduct); lua_setfield(L, -2, "iProduct");
	lua_pushnumber(L, value->iSerialNumber); lua_setfield(L, -2, "iSerialNumber");
	lua_pushnumber(L, value->bNumConfigurations); lua_setfield(L, -2, "bNumConfigurations");
}

struct libusb_config_descriptor* luaL_check_config_descriptor(lua_State* L, int index)
{
	struct libusb_config_descriptor* config;
	config = *(struct libusb_config_descriptor**)luaL_checkudata(L, index, "libusb_config_descriptor");
	if (!config)
	{
		luaL_argerror(L, index, "configuration descriptor has already been freed");
		return NULL;
	}
	return config;
}

void lua_push_config_descriptor(lua_State* L, struct libusb_config_descriptor* value)
{
	struct libusb_config_descriptor** udata;
	udata = (struct libusb_config_descriptor**)lua_newuserdata(L, sizeof(struct libusb_config_descriptor*));
	*udata = value;
	luaL_getmetatable(L, "libusb_config_descriptor");
	lua_setmetatable(L, -2);
}

/****************************************************************************/

#define BINDING(f) static int lua__libusb_##f(lua_State* L)
#define BIND(f) {#f, lua__libusb_##f},

BINDING(init)
{
	libusb_context* context;
	int result;
	
	result = libusb_init(&context);
	if (result != LIBUSB_SUCCESS)
		return lua__usberror(L, result);
	
	lua_push_context(L, context);
	return 1;
}

static int lua__libusb_context__gc(lua_State* L)
{
	libusb_context** ctx;
	
	ctx = (libusb_context**)lua_touserdata(L, 1);
	if (*ctx)
	{
		libusb_exit(*ctx);
		*ctx = 0;
	}
	return 0;
}

BINDING(exit)
{
	luaL_check_context(L, 1);
	return lua__libusb_context__gc(L);
}

BINDING(get_device_list)
{
	libusb_context* ctx;
	libusb_device** list;
	ssize_t result, i;
	
	ctx = luaL_check_context(L, 1);
	
	result = libusb_get_device_list(ctx, &list);
	if (result < 0)
		return lua__usberror(L, (int)result);
	
	lua_createtable(L, (int)result, 0);
	for (i=0; i<result; ++i)
	{
		lua_push_device(L, list[i]);
		lua_rawseti(L, -2, (int)i+1);
	}
	libusb_free_device_list(list, 0);	
	return 1;
}

BINDING(get_bus_number)
{
	libusb_device* dev;
	uint8_t result;
	
	dev = luaL_check_device(L, 1);
	
	result = libusb_get_bus_number(dev);
	
	lua_pushnumber(L, result);
	return 1;
}

BINDING(get_device_address)
{
	libusb_device* dev;
	uint8_t result;
	
	dev = luaL_check_device(L, 1);
	
	result = libusb_get_device_address(dev);
	
	lua_pushnumber(L, result);
	return 1;
}

BINDING(get_max_packet_size)
{
	libusb_device* dev;
	unsigned char endpoint;
	int result;
	
	dev = luaL_check_device(L, 1);
	endpoint = (unsigned char)luaL_checknumber(L, 2); /* :FIXME: handle overflow */
	
	result = libusb_get_max_packet_size(dev, endpoint);
	if (result < 0)
		return lua__usberror(L, result);
	
	lua_pushnumber(L, result);
	return 1;
}

BINDING(get_max_iso_packet_size)
{
	libusb_device* dev;
	unsigned char endpoint;
	int result;
	
	dev = luaL_check_device(L, 1);
	endpoint = (unsigned char)luaL_checknumber(L, 2); /* :FIXME: handle overflow */
	
	result = libusb_get_max_iso_packet_size(dev, endpoint);
	if (result < 0)
		return lua__usberror(L, result);
	
	lua_pushnumber(L, result);
	return 1;
}

static int lua__libusb_device__gc(lua_State* L)
{
	libusb_device** dev;
	
	dev = (libusb_device**)lua_touserdata(L, 1);
	if (*dev)
	{
		libusb_unref_device(*dev);
		*dev = 0;
	}
	return 0;
}

BINDING(unref_device)
{
	luaL_check_device(L, 1);
	return lua__libusb_device__gc(L);
}

BINDING(open)
{
	libusb_device* dev;
	libusb_device_handle* handle;
	int result;
	
	dev = luaL_check_device(L, 1);
	
	result = libusb_open(dev, &handle);
	if (result != 0)
		return lua__usberror(L, result);
	
	lua_push_device_handle(L, handle);
	return 1;
}

BINDING(open_device_with_vid_pid)
{
	libusb_context* ctx;
	uint16_t vendor_id;
	uint16_t product_id;
	libusb_device_handle* result;
	
	ctx = luaL_check_context(L, 1);
	vendor_id = (uint16_t)luaL_checknumber(L, 2); /* :FIXME: handle overflow */
	product_id = (uint16_t)luaL_checknumber(L, 3); /* :FIXME: handle overflow */
	
	result = libusb_open_device_with_vid_pid(ctx, vendor_id, product_id);
	
	if (result)
		lua_push_device_handle(L, result);
	else
		lua_pushnil(L);
	return 1;
}

static int lua__libusb_device_handle__gc(lua_State* L)
{
	libusb_device_handle** dev_handle;
	
	dev_handle = (libusb_device_handle**)lua_touserdata(L, 1);
	if (*dev_handle)
	{
		libusb_close(*dev_handle);
		*dev_handle = 0;
	}
	return 0;
}

BINDING(close)
{
	luaL_check_device(L, 1);
	return lua__libusb_device_handle__gc(L);
}

BINDING(get_device)
{
	libusb_device_handle* dev_handle;
	libusb_device* result;
	
	dev_handle = luaL_check_device_handle(L, 1);
	
	result = libusb_get_device(dev_handle);
	
	libusb_ref_device(result);
	lua_push_device(L, result);
	return 1;
}

BINDING(get_configuration)
{
	libusb_device_handle* dev;
	int config;
	int result;
	
	dev = luaL_check_device_handle(L, 1);
	
	result = libusb_get_configuration(dev, &config);
	if (result != 0)
		return lua__usberror(L, result);
	
	lua_pushnumber(L, config);
	return 1;
}

BINDING(set_configuration)
{
	libusb_device_handle* dev;
	int configuration;
	int result;
	
	dev = luaL_check_device_handle(L, 1);
	configuration = (int)luaL_checknumber(L, 2); /* :FIXME: handle overflow */
	
	result = libusb_set_configuration(dev, configuration);
	if (result != 0)
		return lua__usberror(L, result);
	
	lua_pushboolean(L, 1);
	return 1;
}

BINDING(claim_interface)
{
	libusb_device_handle* dev;
	int interface_number;
	int result;
	
	dev = luaL_check_device_handle(L, 1);
	interface_number = (int)luaL_checknumber(L, 2); /* :FIXME: handle overflow */
	
	result = libusb_claim_interface(dev, interface_number);
	if (result != 0)
		return lua__usberror(L, result);
	
	lua_pushboolean(L, 1);
	return 1;
}

BINDING(release_interface)
{
	libusb_device_handle* dev;
	int interface_number;
	int result;
	
	dev = luaL_check_device_handle(L, 1);
	interface_number = (int)luaL_checknumber(L, 2); /* :FIXME: handle overflow */
	
	result = libusb_release_interface(dev, interface_number);
	if (result != 0)
		return lua__usberror(L, result);
	
	lua_pushboolean(L, 1);
	return 1;
}

BINDING(set_interface_alt_setting)
{
	libusb_device_handle* dev;
	int interface_number;
	int alternate_setting;
	int result;
	
	dev = luaL_check_device_handle(L, 1);
	interface_number = (int)luaL_checknumber(L, 2); /* :FIXME: handle overflow */
	alternate_setting = (int)luaL_checknumber(L, 3); /* :FIXME: handle overflow */
	
	result = libusb_set_interface_alt_setting(dev, interface_number, alternate_setting);
	if (result != 0)
		return lua__usberror(L, result);
	
	lua_pushboolean(L, 1);
	return 1;
}

BINDING(clear_halt)
{
	libusb_device_handle* dev;
	unsigned char endpoint;
	int result;
	
	dev = luaL_check_device_handle(L, 1);
	endpoint = (unsigned char)luaL_checknumber(L, 2); /* :FIXME: handle overflow */
	
	result = libusb_clear_halt(dev, endpoint);
	if (result != 0)
		return lua__usberror(L, result);
	
	lua_pushboolean(L, 1);
	return 1;
}

BINDING(reset_device)
{
	libusb_device_handle* dev;
	int result;
	
	dev = luaL_check_device_handle(L, 1);
	
	result = libusb_reset_device(dev);
	if (result != 0)
		return lua__usberror(L, result);
	
	lua_pushboolean(L, 1);
	return 1;
}

BINDING(kernel_driver_active)
{
	libusb_device_handle* dev;
	int interface;
	int result;
	
	dev = luaL_check_device_handle(L, 1);
	interface = (int)luaL_checknumber(L, 2); /* :FIXME: handle overflow */
	
	result = libusb_kernel_driver_active(dev, interface);
	if (result < 0)
		return lua__usberror(L, result);
	
	lua_pushboolean(L, result);
	return 1;
}

BINDING(detach_kernel_driver)
{
	libusb_device_handle* dev;
	int interface;
	int result;
	
	dev = luaL_check_device_handle(L, 1);
	interface = (int)luaL_checknumber(L, 2); /* :FIXME: handle overflow */
	
	result = libusb_detach_kernel_driver(dev, interface);
	if (result != 0)
		return lua__usberror(L, result);
	
	lua_pushboolean(L, 1);
	return 1;
}

BINDING(attach_kernel_driver)
{
	libusb_device_handle* dev;
	int interface;
	int result;
	
	dev = luaL_check_device_handle(L, 1);
	interface = (int)luaL_checknumber(L, 2); /* :FIXME: handle overflow */
	
	result = libusb_attach_kernel_driver(dev, interface);
	if (result != 0)
		return lua__usberror(L, result);
	
	lua_pushboolean(L, 1);
	return 1;
}

BINDING(get_device_descriptor)
{
	libusb_device* dev;
	struct libusb_device_descriptor desc;
	int result;
	
	dev = luaL_check_device(L, 1);
	
	result = libusb_get_device_descriptor(dev, &desc);
	if (result != 0)
		return lua__usberror(L, result);
	
	lua_push_device_descriptor(L, &desc);
	return 1;
}

BINDING(get_active_config_descriptor)
{
	libusb_device* dev;
	struct libusb_config_descriptor* config;
	int result;
	
	dev = luaL_check_device(L, 1);
	
	result = libusb_get_active_config_descriptor(dev, &config);
	if (result != 0)
		return lua__usberror(L, result);
	
	lua_push_config_descriptor(L, config);
	return 1;
}

BINDING(get_config_descriptor)
{
	libusb_device* dev;
	uint8_t config_index;
	struct libusb_config_descriptor* config;
	int result;
	
	dev = luaL_check_device(L, 1);
	config_index = (uint8_t)luaL_checknumber(L, 2); /* :FIXME: handle overflow */
	
	result = libusb_get_config_descriptor(dev, config_index, &config);
	if (result != 0)
		return lua__usberror(L, result);
	
	lua_push_config_descriptor(L, config);
	return 1;
}

BINDING(get_config_descriptor_by_value)
{
	libusb_device* dev;
	uint8_t bConfigurationValue;
	struct libusb_config_descriptor* config;
	int result;
	
	dev = luaL_check_device(L, 1);
	bConfigurationValue = (uint8_t)luaL_checknumber(L, 2); /* :FIXME: handle overflow */
	
	result = libusb_get_config_descriptor_by_value(dev, bConfigurationValue, &config);
	if (result != 0)
		return lua__usberror(L, result);
	
	lua_push_config_descriptor(L, config);
	return 1;
}

static int lua__libusb_config_descriptor__gc(lua_State* L)
{
	struct libusb_config_descriptor** config;
	
	config = (struct libusb_config_descriptor**)lua_touserdata(L, 1);
	if (*config)
	{
		libusb_free_config_descriptor(*config);
		*config = 0;
	}
	return 0;
}

BINDING(free_config_descriptor)
{
	luaL_check_config_descriptor(L, 1);
	return lua__libusb_config_descriptor__gc(L);
}

BINDING(get_string_descriptor_ascii)
{
	libusb_device_handle* dev;
	uint8_t desc_index;
	unsigned char* data;
	int length;
	int result;
	
	dev = luaL_check_device_handle(L, 1);
	desc_index = (uint8_t)luaL_checknumber(L, 2); /* :FIXME: handle overflow */
	
	length = 4*1024;
	data = lua_newuserdata(L, length);
	
	result = libusb_get_string_descriptor_ascii(dev, desc_index, data, length);
	if (result < 0)
		return lua__usberror(L, result);
	
	lua_pushlstring(L, data, result);
	return 1;
}

BINDING(get_descriptor)
{
	libusb_device_handle* dev;
	uint8_t desc_type;
	uint8_t desc_index;
	unsigned char* data;
	int length;
	int result;
	
	dev = luaL_check_device_handle(L, 1);
	
	result = libusb_get_descriptor(dev, desc_type, desc_index, data, length);
}

BINDING(get_string_descriptor)
{
	libusb_device_handle* dev;
	uint8_t desc_index;
	uint16_t langid;
	unsigned char* data;
	int length;
	int result;
	
	dev = luaL_check_device_handle(L, 1);
	desc_index = (uint8_t)luaL_checknumber(L, 2);
	langid = (uint16_t)luaL_checknumber(L, 3);
	
	length = 4*1024;
	data = lua_newuserdata(L, length);
	
	result = libusb_get_string_descriptor(dev, desc_index, langid, data, length);
	if (result < 0)
		return lua__usberror(L, result);
	
	/* :TODO: convert to utf-8 */
	lua_pushlstring(L, data, length);
	return 1;
}

/****************************************************************************/

static luaL_Reg functions[] = {
	BIND(init)
	{0, 0},
};

static struct luaL_Reg libusb_context__metamethods[] = {
	{"__gc", lua__libusb_context__gc},
	{0, 0},
};

static struct luaL_Reg libusb_context__methods[] = {
	BIND(exit)
	BIND(get_device_list)
	BIND(open_device_with_vid_pid)
	{0, 0},
};

static struct luaL_Reg libusb_device__metamethods[] = {
	{"__gc", lua__libusb_device__gc},
	{0, 0},
};

static struct luaL_Reg libusb_device__methods[] = {
	BIND(get_bus_number)
	BIND(get_device_address)
	BIND(get_max_packet_size)
	BIND(get_max_iso_packet_size)
	BIND(unref_device)
	BIND(open)
	BIND(get_device_descriptor)
	BIND(get_active_config_descriptor)
	BIND(get_config_descriptor)
	BIND(get_config_descriptor_by_value)
	{0, 0},
};

static struct luaL_Reg libusb_device_handle__metamethods[] = {
	{"__gc", lua__libusb_device_handle__gc},
	{0, 0},
};

static struct luaL_Reg libusb_device_handle__methods[] = {
	BIND(close)
	BIND(get_device)
	BIND(get_configuration)
	BIND(set_configuration)
	BIND(claim_interface)
	BIND(release_interface)
	BIND(set_interface_alt_setting)
	BIND(clear_halt)
	BIND(reset_device)
	BIND(kernel_driver_active)
	BIND(detach_kernel_driver)
	BIND(attach_kernel_driver)
	BIND(get_string_descriptor_ascii)
	BIND(get_string_descriptor)
	{0, 0},
};

static struct luaL_Reg libusb_config_descriptor__metamethods[] = {
	{"__gc", lua__libusb_config_descriptor__gc},
	{0, 0},
};

static struct luaL_Reg libusb_config_descriptor__methods[] = {
	BIND(free_config_descriptor)
	{0, 0},
};

/****************************************************************************/

LUAMOD_API int luaopen_module(lua_State* L)
{
	luaL_register(L, lua_tostring(L, 1), functions);
	lua_pop(L, 1);
	/* context */
	luaL_newmetatable(L, "libusb_context");
	luaL_register(L, 0, libusb_context__metamethods);
	lua_newtable(L);
	luaL_register(L, 0, libusb_context__methods);
	lua_setfield(L, -2, "__index");
	lua_pop(L, 1);
	/* device */
	luaL_newmetatable(L, "libusb_device");
	luaL_register(L, 0, libusb_device__metamethods);
	lua_newtable(L);
	luaL_register(L, 0, libusb_device__methods);
	lua_setfield(L, -2, "__index");
	lua_pop(L, 1);
	/* device handle */
	luaL_newmetatable(L, "libusb_device_handle");
	luaL_register(L, 0, libusb_device_handle__metamethods);
	lua_newtable(L);
	luaL_register(L, 0, libusb_device_handle__methods);
	lua_setfield(L, -2, "__index");
	lua_pop(L, 1);
	/* config descriptor */
	luaL_newmetatable(L, "libusb_config_descriptor");
	luaL_register(L, 0, libusb_config_descriptor__metamethods);
	lua_newtable(L);
	luaL_register(L, 0, libusb_config_descriptor__methods);
	lua_setfield(L, -2, "__index");
	lua_pop(L, 1);
	return 0;
}

