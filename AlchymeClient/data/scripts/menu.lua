function Test()
	print('Hello world!')
end

function ConnectToServer(doc)
	Alchyme.ConnectToServer(
		doc:GetElementById('hostname-in'):GetAttribute('value')
	)
end