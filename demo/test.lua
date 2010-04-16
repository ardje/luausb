require 'usb'

local context = assert(usb.init())

print(context)
local devices = assert(context:get_device_list())
for _,device in ipairs(devices) do
	local descriptor = assert(device:get_device_descriptor())
	local handle = device:open()
	if handle then
		print(">",
			assert(device:get_bus_number()),
			assert(device:get_device_address()),
			assert(device:get_max_packet_size()),
			string.format("vid_%04x", descriptor.idVendor), assert(handle:get_string_descriptor(descriptor.iVendor, 0)),
			string.format("pid_%04x", descriptor.idProduct), assert(handle:get_string_descriptor(descriptor.iProduct, 0)))
	end
end

context:exit()

print("all tests passed successfully")
