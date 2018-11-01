size(3cm);

int n = 4;
int width = 4;

draw((0,2)--(0,0), grey+linetype(new real[] {3,3}));
draw((4,2)--(4,0), grey+linetype(new real[] {3,3}));
draw((8,2)--(8,0), grey+linetype(new real[] {3,3}));
draw((12,2)--(12,0), grey+linetype(new real[] {3,3}));
draw((16,2)--(16,0), grey+linetype(new real[] {3,3}));

draw((0,-10)--(0,-12), grey+linetype(new real[] {3,3}));
draw((4,-10)--(4,-12), grey+linetype(new real[] {3,3}));
draw((8,-10)--(8,-12), grey+linetype(new real[] {3,3}));
draw((12,-10)--(12,-12), grey+linetype(new real[] {3,3}));
draw((16,-10)--(16,-12), grey+linetype(new real[] {3,3}));

///////////////////////////
for(int i=0; i<n*width; ++i)
{
  draw((i,3)--(i,2), grey);
}

for(int i=0; i<n; ++i)
{
  draw((i*width,3)--((i+1)*width,3)--((i+1)*width,2)--(width*i,2)--cycle);
}

draw((2,2)--(2,0), Arrow(2));
draw((6,2)--(6,0), Arrow(2));
draw((10,2)--(10,0), Arrow(2));
draw((14,2)--(14,0), Arrow(2));

//////////////////////////
for(int i=0; i<n*width; ++i)
{
  filldraw((i,-1)--(i+1,-1)--(i+1,0)--(i,0)--cycle, gray( (width-i%width)/4 ));
  draw((i,-1)--(i,0), grey);
}

for(int i=0; i<n; ++i)
{
  draw((i*width,-1)--((i+1)*width,-1)--((i+1)*width,0)--(width*i,0)--cycle);
}

///////////////

draw((3.5,-1)--(6.5,-3), Arrow(2));
draw((7.5,-1)--(7.5,-3), Arrow(2));
draw((11.5,-1)--(8.5,-3), Arrow(2));
draw((15.5,-1)--(9.5,-3), Arrow(2));

///////////////
int k = -1;
for(int i=(2*width)-(int)(width/2); i<(2*width)+(int)(width/2); ++i)
{
  filldraw((i,-3)--(i+1,-3)--(i+1,-4)--(i,-4)--cycle, gray( 0.7 ) );
  draw((i,-3)--(i,-4), grey);
}

draw((2*width-2,-3)--((2+1)*width-2,-3)--((2+1)*width-2,-4)--(width*2-2,-4)--cycle);

k = -1;
for(int i=(2*width)-(int)(width/2); i<(2*width)+(int)(width/2); ++i)
{
  filldraw((i,-6)--(i+1,-6)--(i+1,-7)--(i,-7)--cycle, gray( 1 - (i-2*width+width/2)/width ) );
  draw((i,-6)--(i,-7), grey);
}

draw((2*width-2,-6)--((2+1)*width-2,-6)--((2+1)*width-2,-7)--(width*2-2,-7)--cycle);

draw((8,-4)--(8,-6), Arrow(2));


///////////////
for(int i=0; i<n*width; ++i)
{
  filldraw((i,-9)--(i+1,-9)--(i+1,-10)--(i,-10)--cycle, gray( (width-i%width)/4 ));
  draw((i,-9)--(i,-10), grey);
}


for(int i=0; i<n; ++i)
{
  draw((i*width,-9)--((i+1)*width,-9)--((i+1)*width,-10)--(width*i,-10)--cycle);
}

///////////////
for(int i=0; i<n*width; ++i)
{
  filldraw((i, -12)--(i+1,-12)--(i+1,-13)--(i,-13)--cycle, gray( 1 - i/(n*width) ));
  draw((i,-12)--(i,-13), grey);
}

draw((0,-12)--((n)*width,-12)--((n)*width,-13)--(0,-13)--cycle);

draw((6,-10)--(6,-12), Arrow(2));
draw((10,-10)--(10,-12), Arrow(2));
draw((14,-10)--(14,-12), Arrow(2));

///////////////
draw((6.5,-7)--(6,-9), Arrow(2));
draw((7.5,-7)--(10,-9), Arrow(2));
draw((8.5,-7)--(14,-9), Arrow(2));

label( scale(0.5)*"$\oplus$", (6.75,-11));
label( scale(0.5)*"$\oplus$", (10.75,-11));
label( scale(0.5)*"$\oplus$", (14.75,-11));

label( scale(0.25)*"scan", (2.75,1));
label( scale(0.25)*"scan", (6.75,1));
label( scale(0.25)*"scan", (10.75,1));
label( scale(0.25)*"scan", (14.75,1));

label( scale(0.25)*"scan", (8.75,-5));

draw((-1,-2)--(17,-2), grey+linetype(new real[] {4,4}));
draw((-1,-8)--(17,-8), grey+linetype(new real[] {4,4}));

