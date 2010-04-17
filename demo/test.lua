require 'usb'

local context = assert(usb.init())

print(context)
local devices = assert(context:get_device_list())
for _,device in ipairs(devices) do
	local descriptor = assert(device:get_device_descriptor())
	print(">",
		device:get_bus_number(),
		device:get_device_address(),
		string.format("vid_%04x", descriptor.idVendor),
		string.format("pid_%04x", descriptor.idProduct))
	local handle = device:open()
	if handle then
		print('', handle:get_string_descriptor_ascii(descriptor.iManufacturer, 1024))
		print('', handle:get_string_descriptor_ascii(descriptor.iProduct, 1024))
	--	print('', handle:get_descriptor(3, descriptor.iManufacturer, 255))
--		handle:close()
	end
end

for _,device in ipairs(devices) do
--	device:unref_device()
end

--context:exit()

print("all tests passed successfully")
