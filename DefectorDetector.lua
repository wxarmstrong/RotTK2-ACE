function advanceRNG(num)

	RAM = {}

	for regY=5,0,-1 do
		RAM[0x98+regY] = memory.readbyte(0x98+regY)
	end

	RAM[0x9D] = memory.readbyte(0x9D)

	for i=1,num,1 do
		RAM[0x9D] = bit.bor(RAM[0x9D],0x01)

		for regY=5,0,-1 do
			RAM[0x9F+regY] = 0
		end

		for regY=5,0,-1 do
			


			RAM[0x73] = memory.readbyte(0xFC76 + regY)
			RAM[0x6C] = 0x08
			
			while RAM[0x6C] > 0 do
			
				carry = 0;
				
				for regX = 5,0,-1 do
			
					if bit.band(RAM[0x9F+regX],0x80)>0 then
						nextCarry = 1;
					else
						nextCarry = 0;
					end
				
					RAM[0x9F+regX] = bit.band(2*RAM[0x9F+regX]+carry,0xFF)
			
					carry = nextCarry;
			
				end
			
				if bit.band(RAM[0x73],0x80)>0 then
					carry = 1
				else
					carry = 0
				end
				
				RAM[0x73] = bit.band(2*RAM[0x73],0xFF)
				
				if carry~=0 then
					
					carry=0
					
					for regX=5,0,-1 do
					
						
						RAM[0x9F+regX] = RAM[0x9F+regX] + RAM[0x98+regX] + carry
						
						if RAM[0x9F+regX] > 0xFF then
							RAM[0x9F+regX] = bit.band(RAM[0x9F+regX],0xFF)
							carry = 1
						else
							carry = 0
						end
					
					end


				
				end
				
				RAM[0x6C] = RAM[0x6C] - 1
				
			end
			
		end

		carry = 1

		for regX=5,0,-1 do

			regA = XOR(RAM[0x9F+regX],0xFF) + carry
			
			if regA>0xFF then
				regA = bit.band(regA,0xFF)
				carry = 1
			else
				carry = 0
			end
			
			RAM[0x98+regX] = regA
			

		end

		RAM[0x67] = bit.band(RAM[0x98],0x7F)
		RAM[0x66] = RAM[0x99]
	
	end

	return 256*RAM[0x67]+RAM[0x66]
	
end

function getID(gen)
	return memory.readword(gen+0x13)
end

function getFamily(gen)
	address = 0x3285B + 0x09*getID(gen)
	return rom.readbyte(address) + 256*rom.readbyte(address + 1)
end

function getReliable(gen)
	return rom.readbyte(0x32854 + 0x09*getID(gen))
end

function getAmbition(gen)
	return rom.readbyte(0x32856 + 0x09*getID(gen))
end

function isCapable(gen)
	return -1*((bit.band(rom.readbyte(gen+0x02),0x01))-1)
end

function getRatio(num1,num2,num3)
	if num3==0 then return -1 end
	
	RAM = {}
	
	regY=0
	
	RAM[0x0D] = bit.band(num2,0xFF00)/256
	RAM[0x0C] = bit.band(num2,0xFF)
	
	RAM[0x09] = bit.band(num1,0xFF00)/256
	RAM[0x08] = bit.band(num1,0xFF)
	
	regY = 0
	
	RAM[0x0F] = regY
	RAM[0x0E] = regY
	
	--funF1C1()
	RAM[0x0B] = regY
	RAM[0x0A] = regY
	
	--funEF93()
	RAM[0x10] = regY
	RAM[0x11] = regY
	RAM[0x12] = regY
	RAM[0x13] = regY
	
	for regY=0x20,0x01,-1 do
		
		carry = 0
		
		for i=0x13,0x8,-1 do
		
			if bit.band(RAM[i],0x01)>0 then
				nextCarry = 1
			else
				nextCarry = 0
			end
			
			RAM[i] = bit.band(RAM[i],0xFE)/2 + 0x80*carry
			
			carry = nextCarry
			
		end
	
	end
	
	RAM[0x0D] = bit.band(num3,0xFF00)/256
	RAM[0x0C] = bit.band(num3,0xFF)
	
	regY = 0
	
	RAM[0x0F] = regY
	RAM[0x0E] = regY
	
	--funEFC2()
	
		--funEFD0()
		RAM[0x41] = RAM[0x0B]
		RAM[0x40] = bit.bxor(RAM[0x41],RAM[0x0F])
		
		if RAM[0x0B]>80 then
			--funF0F9
			
		end
		
		for i=0x10,0x13,1 do
			RAM[i]=0
		end
		
		for regY=0x20,0x01,-1 do
			
			carry = 0
			
			for i=0x08,0x13,-1 do
			
				if bit.band(RAM[i],0x80)>0 then
					carry = 1
				else
					carry = 0
				end
				
				RAM[i] = 2*bit.band(RAM[i],0x7F)+carry
				
			end
		
		end
		
		carry = 1;
		
		regA = RAM[0x10]
		regA = regA - RAM[0x0C]
		
		if regA<0 then 
			regA = 0x100 + regA 
			carry = 0
		else
			carry = 1
		end
		
		RAM[0x00] = regA
		
		regA = RAM[0x11]
		regA = regA - RAM[0x0D]
		
		if carry == 0 then
			regA = regA - 1
		end
		
		if regA<0 then
			regA = 0x100 + regA
			carry = 0
		else
			carry = 1
		end
		
		RAM[0x01] = regA
		
		regA = RAM[0x12]
		regA = regA - RAM[0x0E]
		
		if carry == 0 then
			regA = regA - 1
		end

		if regA<0 then
			regA = 0x100 + regA
			carry = 0
		else
			carry = 1
		end

		regX = regA
		
		regA = RAM[0x13]
		regA = regA - RAM[0x0F]
		
		if carry == 0 then
			regA = regA - 1
		end

		if regA<0 then
			regA = 0x100 + regA
			carry = 0
		else
			carry = 1
		end

		if carry==0 then
		
			RAM[0x12] = regX
			RAM[0x13] = regA
			
			regA = RAM[0x00]
			RAM[0x10] = regA
			regA = RAM[0x01]
			RAM[0x11] = regA
			RAM[0x08] = bit.band(RAM[0x08] + 1,0xFF)
		
		end
		
	if RAM[0x40]>0x80 then
		--funF0F9
	end
	
	return RAM[0x08] + 256*RAM[0x09]
end

function recruitSpecial(gen)
	activeTrust = memory.readbyte(activeNation + 0x06)
	genInt = memory.readbyte(gen + 0x04)
	genCharm = memory.readbyte(gen + 0x06)
	
	genLoyal = memory.readbyte(gen + 0x08)
	
	num1 = (100 - genLoyal)
	num2 = activeTrust + genInt + genCharm
	num3 = 200
	
	return math.floor( 0.8*math.floor( (num1*num2)/num3 ) )
end

function recruitHorse(gen)
	activeTrust = memory.readbyte(activeNation + 0x06)
	genWar = memory.readbyte(gen + 0x05)
	genCharm = memory.readbyte(gen + 0x06)
	
	genLoyal = memory.readbyte(gen + 0x08)
	
	num1 = 100-genLoyal
	num2 = activeTrust + (3*genWar - genCharm)
	num3 = 200
	
	return math.floor( (num1*num2)/num3 )
end

function recruitGold(gen)
	genRel = getReliable(gen)
	genAmb = getAmbition(gen)
	genCharm = memory.readbyte(gen + 0x06)
	genLoyal = memory.readbyte(gen + 0x08)
	
	num1 = (100 - genLoyal)
	num2 = 2*(100-genRel)+genAmb-genCharm
	num3 = 200
	
	return math.floor( (num1*num2)/num3 )
end

function recruitLetter(gen)

	genLoyal = memory.readbyte(gen + 0x08)
	activeTrust = memory.readbyte(activeNation + 0x06)
	
	num1 = 100-genLoyal

	num3 = 200
	
	num2 = 3*activeTrust
	
	for i,v in pairs(myGen) do
		if isCapable(gen) then
			curCharm = memory.readbyte(v + 0x06)
			if v==activeRuler then
				curCharm = math.floor(0.8*curCharm)
			end
				if 3*activeTrust+curCharm > num2 then num2=3*activeTrust+curCharm end
			end
		end
		
	return math.floor( (num1*num2)/num3 )
end

rng = advanceRNG(1)%100

emu.print("RNG seed: " .. rng)

activeProv = memory.readword(0x782F)
activeNation = 0x71D4 + 0x22*memory.readbyte(activeProv + 0x0E)
activeRuler = memory.readword(activeNation)

myGen = {}

curGen = memory.readword(activeProv+2)

while curGen~=0 do
	table.insert(myGen,curGen)
	curGen = memory.readword(curGen)
end

for i=1,41,1 do
	curProv = 0x73F4 + 0x19*(i-1)
	curNation = 0x71D4 + 0x22*memory.readbyte(curProv + 0x0E)
	curRuler = memory.readword(curNation)
	if curNation ~= activeNation then
	
		
		thisGen = {}
		curThisGen = memory.readword(curProv+2)
		
		while curThisGen~=0 do
			table.insert(thisGen,curThisGen)
			curThisGen = memory.readword(curThisGen)
		end
		
		for k,v in pairs(thisGen) do
			if getFamily(curRuler) ~= getFamily(v) 
			--and k==1
			then
				
				fail = 0;
				
				thisOneLoyal = memory.readbyte(v+0x08)
				
				if thisOneLoyal~=100 then
				
					emu.print( "Province " .. i .. " General " .. k .. ": ".. string.format("%x",v))
					
					if isCapable(activeRuler) then
						if recruitSpecial(v) > rng then
							emu.print("Can be recruited w/ Special Attention" .. " (" .. recruitSpecial(v) .. ")")
						elseif recruitSpecial(v) > 0 then
				--			emu.print("Special Attention: Must have a RNG seed of " .. recruitSpecial(v)-1 .. " or lower")
						else
							fail = fail + 1;
						end
					else
						fail = fail + 1;
					end
					
					if recruitHorse(v) > rng then
						emu.print("Can be recruited w/ Horse" .. " (" .. recruitHorse(v) .. ")")
					elseif recruitHorse(v) > 0 then
			--			emu.print("Horse: Must have a RNG seed of " .. recruitHorse(v)-1 .. " or lower")
					else
						fail = fail + 1;					
					end
					
					if recruitGold(v) > rng then
						emu.print("Can be recruited w/ Gold" .. " (" .. recruitGold(v) .. ")")
					elseif recruitGold(v) > 0 then
			--			emu.print("Gold: Must have a RNG seed of " .. recruitGold(v)-1 .. " or lower")
					else
						fail = fail + 1;					
					end
					
					
					if recruitLetter(v) > rng then
						emu.print("Can be recruited w/ Letter" .. " (" .. recruitLetter(v) .. ")")
					elseif recruitLetter(v) > 0 then
			--			emu.print("Letter: Must have a RNG seed of " .. recruitLetter(v)-1 .. " or lower")
					else
						fail = fail + 1;					
					end
					
					if fail==4 then
						emu.print("CANNOT BE RECRUITED WITHOUT STAT CHANGES")
					end
					
				end
			end
		end
	end
end