ASSIGNMENT 5 OVERVIEW

Controls:
'Space' - Pauses Animation (toggle)
'f'	- Fast Forward's Animation (Hold)

Mouse Controls:
left-click + Move: 	Pan around the xz-plane.  This is a bit disorienting and there's no cap on how far you can go.
right-click + Move:	Orbit around focus point.
scroll wheel:		Zoom in/out.

Known Issues:
- Lighting doesn't work due to issues calculating the Normal.  If I had more time, I could fix it by seperating the Translation and Rotation
	to the world position then apply the rotation to the normal vector only.  The combination of the two cause it to be wrong and if it's not
	positioned into the world properly it only creates a normal based on local rotations and doesn't stay correct in orbit.
- Because of the issues setting up the Normal, the specular highlight loses its place and ends up getting lost in orbit.
- Speed seems a bit slow on Linux,  The ideal speed I wanted worked on windows.  If you want to see that, just set up your windows environment and
	flip the switch in the EnvSpec.h file.

Unsure:
- I think the Axial Rotation is right.  I added that rotation to the scene transform for planets which causes the earth to rotate around the sun
	like it does.  I didn't really understand what was being asked, so I hope that I was correct with that.

Final Notes:
- I really enjoyed this class and I wish it was the only one I had all semester, then I could have spent quality time with the projects and complete
	more that I wanted to do. 
- I'm happy being able to keep all the first 4 assignments in the same program.  Due to time constraints, I didn't attempt the backwards compatibility
	with Assignment 5.
- Thanks for everything and have a good summer!

James

