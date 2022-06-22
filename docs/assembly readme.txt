cut the box and clean using a box cutter
assemble the box
disassemble the box
mount the main pcb
create the ODMeter parts as in the ODMeter readme file
including mounting the constant  current board and the bracket
mount the stepper on the bracket using BOM 3mm diam screws of 12 or 9?? lenght and siione washers
cut 2 under platform pillows
insert 2 small ball bearings in it
cut the central under platform ballbearing pillow
cut 2 over the box pillows
cut link
cut joint ballbearing part
put the platorm over the box and attach the rod
place the motor link aluminum connector between the link and shaft of the motor
tighten the screws gradually to ensure centered and unrestricted motion of the motor


---
other motors
Solder 2 wires about 15 to 25 cm for both the air pump and the circulation peristaltic pump. Use heat shrink. On the other ends, crimp or solder a two pin conectors. You can twist each pair of wire to look neater unless the are already attached lengthwise. Mark with  red marker or black marker where the + and minus will be for the desired spin direction. The motors can be connecected any way but they spin in different directions and both the air and circulation should be kept in the same direction. You can if needed adjust the flow by connecting the silicone tubes in a reverse position.

Cut a foam pad for the air pump and screw the air motor on it with BOM 3mm screws of ??? length connect the motor cables to the main board
Place the peristaltic pump and screw it with BOM 3mm screws of ??? length connect the motor cables to the main board

Software
Create and save a config file for the old board. Use MOS 1 heater MOS 2 Air, A1B1? Bridge for circulation peristaltic pump
connect cable and load software
start coolTerm and configure port and in options /terminal change to Terminal Mode=Line Mode and  EnterKey Emulation=LF
connect and then type ? to see commands
then copy the appropriate config.json from the source code in memory that should begin with: {"name":"standby","standby":false,"targetTemperature":-1000 etc...
then execute 
createFileWithContent /standby.json Paste_here_the _long _string_of_configuration_from_memory
after that load the protocol by executing:
p load standby.json please note that file commands require / while protocol commands like p do not require directory information as we assume all in the root

PP1 -heater works,  PP2 does not work, A1B1 does not work, buzzer works, rocker ? we don't know

