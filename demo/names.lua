module(..., package.seeall)

physdes = {}
bias = {}
langid = {}
class = {}
audioterminal = {}
videoterminal = {}
countrycode = {}
vendor = {}
product = {}
subclass = {}
protocol = {}
hid = {}
reporttag = {}
huts = {}
hutus = {}

local function parse(f)
	local linectr = 0
	local lastvendor, lastclass, lastsubclass, lasthut, lastlang

	for buf in f:lines() do
		linectr = linectr + 1
		if buf:match('^[^#].*$') then
			if buf:match('^PHYSDES%s+') then
				local u,cp = buf:match('^PHYSDES%s+(%x+)%s+(.*)$')
				u = u and tonumber(u, 16)
				assert(u and cp, "Invalid PHYSDES type at line "..linectr)
				physdes[u] = cp

			elseif buf:match('^PHY%s+') then
				local u,cp = buf:match('^PHY%s+(%x+)%s+(.*)$')
				u = u and tonumber(u, 16)
				assert(u and cp, "Invalid PHY type at line "..linectr)
				physdes[u] = cp

			elseif buf:match('^BIAS%s+') then
				local u,cp = buf:match('^BIAS%s+(%x+)%s+(.*)$')
				u = u and tonumber(u, 16)
				assert(u and cp, "Invalid BIAS type at line "..linectr)
				bias[u] = cp

			elseif buf:match('^L%s+') then
				local u,cp = buf:match('^L%s+(%x+)%s+(.*)$')
				u = u and tonumber(u, 16)
				assert(u and cp, "Invalid L type at line "..linectr)
				langid[u] = cp
				lasthut,lastclass,lastvendor,lastsubclass = nil
				lastlang = u

			elseif buf:match('^C%s+') then
				local u,cp = buf:match('^C%s+(%x+)%s+(.*)$')
				u = u and tonumber(u, 16)
				assert(u and cp, "Invalid C type at line "..linectr)
				class[u] = cp
				lasthut,lastlang,lastvendor,lastsubclass = nil
				lastclass = u

			elseif buf:match('^AT%s+') then
				local u,cp = buf:match('^AT%s+(%x+)%s+(.*)$')
				u = u and tonumber(u, 16)
				assert(u and cp, "Invalid AT type at line "..linectr)
				audioterminal[u] = cp

			elseif buf:match('^VT%s+') then
				local u,cp = buf:match('^VT%s+(%x+)%s+(.*)$')
				u = u and tonumber(u, 16)
				assert(u and cp, "Invalid VT type at line "..linectr)
				videoterminal[u] = cp

			elseif buf:match('^HCC%s+') then
				local u,cp = buf:match('^HCC%s+(%x+)%s+(.*)$')
				u = u and tonumber(u, 16)
				assert(u and cp, "Invalid HCC type at line "..linectr)
				countrycode[u] = cp

			elseif buf:match('^%x') then
				local u,cp = buf:match('^(%x+)%s+(.*)$')
				u = u and tonumber(u, 16)
				assert(u and cp, "Invalid vendor spec at line "..linectr)
				vendor[u] = cp
				lastvendor = u
				lasthut,lastlang,lastclass,lastsubclass = nil

			elseif buf:match('^\t%x') then
				local u,cp = buf:match('^\t(%x+)%s+(.*)$')
				u = u and tonumber(u, 16)
				assert(u and cp, "Invalid product/subclass spec at line "..linectr)
				
				if lastvendor then
					product[lastvendor] = product[lastvendor] or {}
					product[lastvendor][u] = cp
				elseif lastclass then
					subclass[lastclass] = subclass[lastclass] or {}
					subclass[lastclass][u] = cp
					lastsubclass = u
				elseif lasthut then
					hutus[lasthut * 2^16 + u] = cp
				elseif lastlang then
					langid[lastlang * 2^10 + u] = cp
				else
					error("Product/Subclass spec without prior Vendor/Class spec at line "..linectr)
				end


			elseif buf:match('^\t\t%x') then
				local u,cp = buf:match('^\t\t(%x+)%s+(.*)$')
				u = u and tonumber(u, 16)
				assert(u and cp, "Invalid protocol spec at line "..linectr)

				if lastclass and lastsubclass then
					protocol[lastclass] = protocol[lastclass] or {}
					protocol[lastclass][lastsubclass] = protocol[lastclass][lastsubclass] or {}
					protocol[lastclass][lastsubclass][u] = cp
				end

			elseif buf:match('^HID%s') then
				local u,cp = buf:match('^HID%s+(%x+)%s+(.*)$')
				u = u and tonumber(u, 16)
				assert(u and cp, "Invalid HID type at line "..linectr)
				hid[u] = cp

			elseif buf:match('^HUT%s') then
				local u,cp = buf:match('^HUT%s+(%x+)%s+(.*)$')
				u = u and tonumber(u, 16)
				assert(u and cp, "Invalid HUT type at line "..linectr)
				huts[u] = cp
				lastlang,lastclass,lastvendor,lastsubclass = nil
				lasthut = u

			elseif buf:match('^R%s') then
				local u,cp = buf:match('^R%s+(%x+)%s+(.*)$')
				u = u and tonumber(u, 16)
				assert(u and cp, "Invalid R type at line "..linectr)
				reporttag[u] = cp

			else
				error("Unknown line at line "..linectr)
			end
		end
	end
end

function init(n)
	local f = assert(io.open(n, "r"))
	parse(f)
	f:close()
end

