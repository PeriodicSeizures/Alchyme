function Test()
	print('Hello world!')
end

function ConnectToServer(doc)
	-- API call
	Alchyme.ConnectToServer(
		doc:GetElementById('hostname-in'):GetAttribute('value')
	)
end

function ForwardPeerInfo(doc)	
	print("Lua ForwardPeerInfo")
	
	local connectingDoc = {}
    for i,d in ipairs(rmlui.contexts["default"].documents) do
	    if d.title == "ConnectingMenu" then
			connectingDoc = d
		end
    end
	
	doc:Hide()
	connectingDoc:Show()
	
	Alchyme.ForwardPeerInfo(
		doc:GetElementById('username-in'):GetAttribute('value'),
		doc:GetElementById('login-key-in'):GetAttribute('value')
	)
	
end
