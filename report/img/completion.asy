import graph;

// picture pic;
real m = 4;
real xsize=160*m, ysize=90*m;
int p;

scale(Log, Log);

size(xsize,ysize,IgnoreAspect);
/***********************************************************************/
////////////////////////////////////// block_STL
real[] p1 = {21,22,23,23,24,22,26,35,49,119};
pair[] f1 = {(0,0.),(0,0.),(0,0.),(0,0),(0,0),(0,0),(0,0)};
for(int i=0, p=1048576; i<10; ++i, p *= 2) f1[i] = (p, p1[i]);
draw(graph(f1),"block\_stl", marker(scale(1.5mm)*polygon(3),grey,FillDraw(gray(0.7)),above=false));

////////////////////////////////////// block_FF
real[] p2 = {68,67,67,69,67,70,84,90,108,201};
pair[] f2 = {(0,0.),(0,0.),(0,0.),(0,0),(0,0),(0,0),(0,0)};
for(int i=0, p=1048576; i<10; ++i, p *= 2) f2[i] = (p, p2[i]);
draw(graph(f2),"block\_ff", marker(scale(1.5mm)*polygon(4),grey,FillDraw(gray(0.7)),above=false));

////////////////////////////////////// block_OMP
real[] p3 = {2,3,4,6,5,7,14,24,46,117};
pair[] f3 = {(0,0.),(0,0.),(0,0.),(0,0),(0,0),(0,0),(0,0)};
for(int i=0, p=1048576; i<10; ++i, p *= 2) f3[i] = (p, p3[i]);
draw(graph(f3),"block\_omp", marker(scale(1.5mm)*polygon(5),grey,FillDraw(gray(0.7)),above=false));

////////////////////////////////////// block_CILK
real[] p4 = {33,28,32,37,52,67,91,116,215,286};
pair[] f4 = {(0,0.),(0,0.),(0,0.),(0,0),(0,0),(0,0),(0,0)};
for(int i=0, p=1048576; i<10; ++i, p *= 2) f4[i] = (p, p4[i]);

draw(graph(f4),"block\_cilk", marker(scale(1.5mm)*polygon(6),grey,FillDraw(gray(0.7)),above=false));
//////////////////////////////////////

/***********************************************************************/
////////////////////////////////////// circuit_STL
real[] p5 = {96,115,136,178,198,212,256,301,379,594};
pair[] f5 = {(0,0.),(0,0.),(0,0.),(0,0),(0,0),(0,0),(0,0)};
for(int i=0, p=1048576; i<10; ++i, p *= 2) f5[i] = (p, p5[i]);
draw(graph(f5),"circuit\_stl", marker(scale(1.5mm)*polygon(3),grey,FillDraw(gray(0)),above=false));

////////////////////////////////////// circuit_FF
real[] p6 = {80,80,82,87,96,106,137,182,273,574};
pair[] f6 = {(0,0.),(0,0.),(0,0.),(0,0),(0,0),(0,0),(0,0)};
for(int i=0, p=1048576; i<10; ++i, p *= 2) f6[i] = (p, p6[i]);
draw(graph(f6),"circuit\_ff", marker(scale(1.5mm)*polygon(4),grey,FillDraw(gray(0)),above=false));

////////////////////////////////////// circuit_OMP
real[] p7 = {1,2,4,7,17,33,57,126,236,513};
pair[] f7 = {(0,0.),(0,0.),(0,0.),(0,0),(0,0),(0,0),(0,0)};
for(int i=0, p=1048576; i<10; ++i, p *= 2) f7[i] = (p, p7[i]);
draw(graph(f7),"circuit\_omp", marker(scale(1.5mm)*polygon(5),grey,FillDraw(gray(0)),above=false));

////////////////////////////////////// circuit_CILK
real[] p8 = {56,81,82,105,111,114,184,217,310,617};
pair[] f8 = {(0,0.),(0,0.),(0,0.),(0,0),(0,0),(0,0),(0,0)};
for(int i=0, p=1048576; i<10; ++i, p *= 2) f8[i] = (p, p8[i]);
draw(graph(f8),"circuit\_cilk", marker(scale(1.5mm)*polygon(6),grey,FillDraw(gray(0)),above=false));

scale(Log, Log);

xaxis("n",BottomTop,LeftTicks(DefaultFormat,new real[] {1048576, 2097152,4194304,8388608,16777216,33554432,67108864,134217728,268435456,536870912}));

yaxis("$T_{C_{p}}$",LeftRight,RightTicks(DefaultFormat, new real[] {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024} ));

add(currentpicture, legend(),point(SE),10N+10W,UnFill);

scale(Log, Log);

