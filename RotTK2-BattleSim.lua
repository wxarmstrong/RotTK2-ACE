PadA  = {
{A=true},
{A=false}
}

PadUp  = {
{up=true},
{up=false}
}

PadDown  = {
{down=true},
{down=false}
}

PadLeft  = {
{left=true},
{left=false}
}

PadRight  = {
{right=true},
{right=false}
}

function waitFrames(num)
	for i=1,num,1 do
	emu.frameadvance()
	end
end 

function waitForBytes(byteTable)
	done = false
	while done ~= true do
		done = true
		for k,v in ipairs(byteTable) do
			if memory.readbyte(v[1]) ~= v[2] then
				done = false
				emu.frameadvance()
				break
			end 
		end
	end 
end

function push(button)
	joypad.set(1,button[1])
	waitFrames(2)
	joypad.set(1,button[2])
end

function pushDirection(direction)
	ypos = memory.readbyte(0x7D1B)
	
	if direction == "Up" then
		push(PadUp)
	elseif direction == "UpLeft" then
		push(PadLeft)
	elseif direction == "UpRight" then
		push(PadRight)
	else
		if ypos==0xB then
			if direction == "DownLeft" then
				push(PadLeft)
				waitFrames(1)
				push(PadLeft)
			elseif direction == "DownRight" then
				push(PadRight)
				waitFrames(1)
				push(PadRight)
			end
		else
			push(PadDown)
			waitFrames(1)
			if direction == "DownLeft" then
				push(PadLeft)
			elseif direction == "DownRight" then
				push(PadRight)
			end 
		end
	end
	waitFrames(1)
	push(PadA)
end

function MoveMenu()
	push(PadA)
	waitFrames(24)
end

function MoveNormal(direction)
	MoveMenu()
	push(PadA)
	waitFrames(26)
	pushDirection(direction)
	waitFrames(6)
end  

function AttackMenu()
	push(PadDown)
	waitFrames(12)
	push(PadA)
	waitFrames(25)
end

function AttackNormal(direction)
	AttackMenu()
	push(PadDown)
	waitFrames(6)
	push(PadA)
	waitFrames(21)
	pushDirection(direction)
end 

function Fire(direction)
	AttackMenu()
	push(PadDown)
	waitFrames(50)
	push(PadDown)
	waitFrames(50)
	push(PadA)
	waitFrames(50)
	pushDirection(direction)
end

function Wait()
	push(PadUp)
	waitFrames(11)
	push(PadA)
	waitFrames(12)
	push(PadA)
end

function canMoveEdge(direction)

	xpos = memory.readbyte(0x7D1A)
	ypos = memory.readbyte(0x7D1B)
	
	if (xpos==0x0 and (direction=="UpLeft"  or direction=="DownLeft" ))
	or (xpos==0xC and (direction=="UpRight" or direction=="DownRight")) then
--		emu.print("Cannot move " .. direction .. ": edge of map")
		return false		
	end
	
	if (ypos==0x0) then
		if direction=="Up" then
	--		emu.print("Cannot move " .. direction .. ": edge of map")
			return false
		end
		if (direction=="UpLeft" or direction=="UpRight") and xpos%2==0 then
	--		emu.print("Cannot move " .. direction .. ": edge of map")
			return false			
		end
	end

	if (ypos==0xB) then
		if direction=="Down" then
	--		emu.print("Cannot move " .. direction .. ": edge of map")
			return false
		end
		if (direction=="DownLeft" or direction=="DownLeft") and xpos%2==1 then
	--		emu.print("Cannot move " .. direction .. ": edge of map")
			return false			
		end
	end	
end

mobilityTable = {2,3,3,0,5,3,3}

function canMove(direction)
	xpos = memory.readbyte(0x7D1A)
	ypos = memory.readbyte(0x7D1B)
	
--	emu.print("Now checking to see if movement is possible: " .. direction)

	-- Test for edge of map
	if (canMoveEdge(direction))==false then return false end
--	emu.print("Passed edge of map check")
	
	targetX = xpos
	targetY = ypos
	if (direction=="Up") then targetY = targetY-1
	elseif (direction=="Down") then targetY = targetY+1
	elseif (direction=="UpLeft") then
		targetX = targetX-1
		if (xpos%2==0) then targetY = targetY-1 end
	elseif (direction=="DownLeft") then
		targetX = targetX-1
		if (xpos%2==1) then targetY = targetY+1 end
	elseif (direction=="UpRight") then
		targetX = targetX+1
		if (xpos%2==0) then targetY = targetY-1 end
	elseif (direction=="DownRight") then
		targetX = targetX+1
		if (xpos%2==1) then targetY = targetY+1 end
	end
--	emu.print("Your current coordinates are " .. xpos .. "," .. ypos)
--	emu.print("Your target coordinates are " .. targetX .. "," .. targetY)
	
	--Test for unpassable terrain
	currentOffset = xpos + 0xD*ypos
	targetOffset = targetX + 0xD*targetY
	
	terrainType = memory.readbyte(0x7D58+targetOffset)
	tileFlags = memory.readbyte(0x7DF4+targetOffset)
	
	if ( (terrainType==3)
	or (bit.band(tileFlags,0xF) > 0)
	or (bit.band(tileFlags,0x20) > 0) )  then
--		emu.print("Performing diagnostic type 1:")
		-- f_924F(curX, curY, targetX, targetY)
		if terrainType ~=1 then
--			emu.print("Failed test 1: 7D58 table check")
			return false
		end
		
		if bit.band(tileFlags,0xF)==0 then return false end
		current_bit = 0
		t_7DF4_copy = memory.readbyte(0x7DF4+currentOffset)
		if (t_7DF4_copy ~= 0) then
			while (t_7DF4_copy~=1) do
				t_7DF4_copy = bit.rshift(t_7DF4_copy,1)
				current_bit = current_bit+1;
			end
		end
		target_bit = 0;
		t_7DF4_copy = memory.readbyte(0x7DF4+targetOffset)
		if (t_7DF4_copy ~= 0) then
			while (t_7DF4_copy~=1) do
				t_7DF4_copy = bit.rshift(t_7DF4_copy,1)
				target_bit = target_bit+1;
			end
		end
		searchSpot = 0x7D1E + 3*current_bit
		found = false
		idx = 0
		while idx~=3 do
			if memory.readbyte(searchSpot+idx)==target_bit then
				found=true
				break
			end
			idx = idx + 1
		end
		if found==false then return false end
		-- if fun_9A1E(curX,curY,targX,targY)==false then return false end
	else
	--	emu.print("Performing diagnostic type 2:")
		mobility = memory.readbyte(activeGeneral + 0x11)
		mobilityCost = mobilityTable[terrainType+1]
		if (mobilityCost>mobility) then return false end
		if (mobility-mobilityCost>=2) then return false end
	end
	return true
end

function canAttack(direction)
	xpos = memory.readbyte(0x7D1A)
	ypos = memory.readbyte(0x7D1B)
	
--	emu.print("Now checking to see if movement is possible: " .. direction)

	-- Test for edge of map
	if (canMoveEdge(direction))==false then return false end
--	emu.print("Passed edge of map check")
end

function canFire(direction)
	xpos = memory.readbyte(0x7D1A)
	ypos = memory.readbyte(0x7D1B)
	
--	emu.print("Now checking to see if movement is possible: " .. direction)

	-- Test for edge of map
	if (canMoveEdge(direction))==false then return false end
--	emu.print("Passed edge of map check")
end

function toNextTurn()
--	emu.print("toNextTurn()")
	waitFrames(200) -- Skip new Day Screen
	push(PadA)
	waitFrames(60)
	while (memory.readword(0x7D2C)~=activeGeneral) do
		emu.frameadvance()
	end
--	emu.print("It is now active general's turn")
	waitFrames(50)
end

function deepcopy(orig)
    local orig_type = type(orig)
    local copy
    if orig_type == 'table' then
        copy = {}
        for orig_key, orig_value in next, orig, nil do
            copy[deepcopy(orig_key)] = deepcopy(orig_value)
        end
        setmetatable(copy, deepcopy(getmetatable(orig)))
    else -- number, string, boolean, etc
        copy = orig
    end
    return copy
end

DirectionTypes= {"Up", "UpLeft", "UpRight", "Down", "DownLeft", "DownRight"}
ActionTypes = {"MoveNormal","Fire","Wait"}

bestTurns = 999
bestActionList = {}
activeGeneral = nil

save_array = {}

function BattleSim()
	emu.speedmode("turbo")
	activeGeneral = memory.readword(0x7D2C)
	bestTurns = 999;
	blankActionList = {}
	
	save_array[0] = savestate.create()
	savestate.save(save_array[0])
	savestate.persist(save_array[0])					
	
	SimTurn(blankActionList)	
	
	savestate.load(save_array[0])
	
	emu.print("Simulation over")
	if (#bestActionList>0) then emu.print(bestActionList)
		else emu.print("No path found")
	end
	emu.print(save_array)
	emu.speedmode("normal")
end

winX = 0x6
winY = 0x6
maxTurns = 20

function winCondition()
	xpos = memory.readbyte(0x7D1A)
	ypos = memory.readbyte(0x7D1B)
	emu.print("Testing win conditions")
	emu.print("Xpos: " .. xpos)
	emu.print("Target: " .. winX)
	emu.print("Ypos: " .. ypos)
	emu.print("Target: " .. winY)	
	if xpos==winX and ypos==winY then return true
	else return false
	end
end

function loseCondition()
	if memory.readbyte(0x7D1C)>maxTurns then return true
	else return false
	end
end

function SimTurn(ActionList)
	local turnNum = #ActionList + 1
	local curAction = {}
	local newActionList = {}
	
	
	emu.print("Now simulating turn #" .. turnNum)
	emu.print(ActionList)
	
	save_array[turnNum] = savestate.create()
	savestate.save(save_array[turnNum])
	savestate.persist(save_array[turnNum])
	
	if (winCondition()==true) then          -- Win condition
		emu.print("Solution found!")
		if #ActionList < bestTurns then
			emu.print("New best solution found!!")
			bestActionList = deepcopy(ActionList)
			bestTurns = #ActionList
		end
		emu.print("Stepping back...")
		savestate.load(save_array[turnNum-1])
		return 
	end
	
	if (loseCondition()==true) then     -- Lose condition
		emu.print("Lose condition hit")
		emu.print("Stepping back...")
		savestate.load(save_array[turnNum-1])
		return
	end

	if #ActionList >= bestTurns-1 then
		emu.print("Can't be better than current best. Pruning...")
		savestate.load(save_array[turnNum-1])
		return
	end
	
	local Directions = {}
	xpos = memory.readbyte(0x7D1A)
	ypos = memory.readbyte(0x7D1B)
	
	if (xpos==winX) then
		if     (ypos>winY) then
			Directions = {"Up","UpLeft","UpRight","Down","DownLeft","DownRight"}
		elseif (ypos<winY) then
			Directions = {"Down","DownLeft","DownRight","Up","UpLeft","UpRight"}
		end
	elseif (xpos<winX) then
		if     (ypos==winY) then
			Directions = {"UpRight","DownRight","Up","Down","UpLeft","DownLeft"}
		elseif (ypos<winY) then
			Directions = {"DownRight","Down","UpRight","Up","DownLeft","UpLeft"}
		elseif (ypos>winY) then
			Directions = {"UpRight","Up","DownRight","UpLeft","Down","DownLeft"}
		end
	elseif (xpos>winX) then
		if     (ypos==winY) then
			Directions = {"UpLeft","DownLeft","Up","Down","UpRight","DownRight"}
		elseif (ypos<winY) then
			Directions = {"DownLeft","Down","UpLeft","Up","DownRight","UpRight"}
		elseif (ypos>winY) then
			Directions = {"UpLeft","Up","DownLeft","UpRight","Down","DownRight"}
		end	
	end 

	for actionKey,action in ipairs(ActionTypes) do
	
		emu.print("Testing action: " .. action)
		if action=="Wait" then
			curAction = {"Wait"}
			newActionList = deepcopy(ActionList)
			Wait()
			toNextTurn()
			emu.print("Previous action list: ")
			emu.print(newActionList)
			newActionList[#newActionList+1] = curAction
			emu.print("New action list: ")
			emu.print(newActionList)
			SimTurn(newActionList)
			emu.print("We've now walked back to turn " .. turnNum)
			emu.print(ActionList)
		else
			if action=="Wait" then
				curAction = {"Wait"}
				newActionList = deepcopy(ActionList)
				Wait()
				toNextTurn()
				newActionList[#newActionList+1] = curAction
				SimTurn(newActionList)
				emu.print("We've now walked back to turn " .. turnNum)
				emu.print(ActionList)				
			else
				for directionKey,direction in ipairs(Directions) do
					emu.print("Testing direction: " .. direction)
					
					if action == "Fire" then
						curAction = {"Fire", direction}
						newActionList = deepcopy(ActionList)
						Fire(direction)
						toNextTurn()
						newActionList[#newActionList+1] = curAction
						SimTurn(newActionList)
						emu.print("We've now walked back to turn " .. turnNum)
						emu.print(ActionList)
					end
					
					if action == "MoveNormal" and canMove(direction)==true then
						curAction = {"MoveNormal", direction}
						newActionList = deepcopy(ActionList)
						MoveNormal(direction)
						toNextTurn()
						newActionList[#newActionList+1] = curAction
						SimTurn(newActionList)
						emu.print("We've now walked back to turn " .. turnNum)
						emu.print(ActionList)
					end
				end
			end
		end
	end
	savestate.load(save_array[turnNum-1])
	emu.print("Done trying moves for this turn.")
	emu.print("Rolling back to turn " .. turnNum-1)
	
end

 BattleSim()