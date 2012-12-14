function onRendezVousUnknown(peerId)
	--for i,v in cumulus.servers:ipairs() do
	--	NOTE("onRendezVousUnknown " .. v.publicAddress)
	--end
	NOTE("clients "..cumulus.clients.count)
	return cumulus.servers
	--bytes = string.byte(peerId)
	--string = ""
	--for i = 1,32 do
	--	string = string + string.format("%02x", bytes[i])
	--end
	--NOTE(string)
	--NOTE("NOT FOUND")
end

function onStart(path)
	--NOTE("started....")
	NOTE("clients "..cumulus.clients.count)
end

function onManage()
	--NOTE("clients "..cumulus.clients.count)
	--NOTE("servers "..cumulus.servers.count)
	--NOTE("groups "..cumulus.groups.count)
	--NOTE("publications "..cumulus.publications.count)
	--for i,v in cumulus.servers:ipairs() do
	--	NOTE(v.address)
	--end
end

function onConnection(client)
	NOTE("onConnection ".. client.id .. "--" .. client.address)
	NOTE("clients "..cumulus.clients.count)
end

--index = 0
--function onHandshake(address,path,properties,attempts)
--	NOTE("onHandshake " .. address .. "--" .. path .. "--" .. attempts)
--	NOTE("clients "..cumulus.clients.count)
--	index = index + 1
--	if index > cumulus.servers.count then index=1 end
--	return cumulus.servers(index)
--end

