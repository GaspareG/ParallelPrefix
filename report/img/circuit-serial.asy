size(3cm);

int n = 8;

int width = n;
int height = 7;

for(int i=0; i<=height; ++i)
  draw((-0.25,-i)--(width-.25,-i), gray(0.8)+linetype(new real[] {4,4}));

void op(int level, int l, int r)
{

  draw((l, -(level-1))--(r, -level), gray(0));
  filldraw(circle((r, -level),0.03),black,black+linewidth(1));

  string label = (string) level;
  label( scale(0.5)*label, (-.5,-level));

  string label2 = (string) (level-1);
  label( scale(0.5)*label2, (-.5,-level+1));

}

for(int i=0; i<n; ++i)
{
  string label = (string) i;
  label( scale(0.5)*label, (i,1));
  draw((i, .5)--(i,-height-.5), gray(0.5));
}

// Serial
for(int i=1; i<n; ++i)
  op(i, i-1, i);
/*
// Parallel 1^
op(1, 0, 1);
op(1, 2, 3);
op(1, 4, 5);
op(1, 6, 7);
op(2, 1, 3);
op(2, 5, 7);
op(3, 3, 7);

// Parallel 2^
op(4, 3, 5);
op(5, 1, 2);
op(5, 3, 4);
op(5, 5, 6);*/
