OBJS = main.cpp Camera.cpp GeometryManager.cpp GraphicsManager.cpp ImageReader.cpp Mouse_Handler.cpp Planet.cpp SceneGraph.cpp Shader.cpp ShaderManager.cpp Transformation.cpp
GLFLAGS = -lglfw -lGL -lGraphicsMagick++ -o Assignment5 -g
INC = -I/usr/include/GraphicsMagick 
PREFLAGS = -std=c++11 

#GraphicsManager.o: GraphicsManager.cpp
#	g++ $(PREFLAGS) GraphicsManager.cpp $(GLFLAGS)

#Graphic.o: Graphic.cpp 
#	g++ $(PREFLAGS) Graphic.cpp $(GLFLAGS)

#CmdHandler.o: CmdHandler.cpp 
#	g++ $(PREFLAGS) CmdHandler.cpp $(GLFLAGS)

#InputHandler.o: InputHandler.cpp 
#	g++ $(PREFLAGS) InputHandler.cpp $(GLFLAGS)

#SDiamonds.o: SDiamonds.cpp 
#	g++ $(PREFLAGS) SDiamonds.cpp $(GLFLAGS)

assign: $(OBJS)
	g++ $(PREFLAGS) $(OBJS) -g $(GLFLAGS) $(INC)

clean: 
	\rm *.o *~ Assignment5


