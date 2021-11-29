--print("Hello, Lua!")
--print(rmlui ~= nil and "not nil" or "is nil")

local mainScript = {
	onEnable = nil,
	onDisable = nil,
}

function mainScript.onEnable() 
	print("Main script successfully enabled!")
	
	rmlui:LoadFontFace("fonts/OpenSans-Regular.ttf")
	local con = rmlui.contexts["default"]
	local uiMainMenu = con:LoadDocument("ui/main-menu.rml")
    uiMainMenu:Show()
	
	
	
	--Alchyme.ConnectToServer("localhost", 8001)
end

-- Register this for routine/scheduled event
Alchyme.RegisterScript(mainScript)

mainScript = nil -- why nullify?
				-- probably to safe memory 
				-- since is no longer needed in this scape