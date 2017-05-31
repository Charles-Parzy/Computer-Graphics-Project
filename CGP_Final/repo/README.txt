To run the project:
1) cd build
2) cmake ..
3) make
4) cd project
5) ./project

FAQ:
Q: I get an Abort Trap: 6 when running the project ! 
A: We are loading big textures into the GPU, hence you graphic card might not have enough memory to hold that much data. To fix this, please change the line number 56 inside main.cpp:
	
	int water_texture_size = 5000;

This represents the size of the textures that will be loaded into the GPU, a typical value that work with a macbook pro without a dedicated GPU is 5000, but if you can't not run it, please replace it by 1000.