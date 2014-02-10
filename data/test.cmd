set output "test.gif";
set terminal gif;
set palette maxcolors 100;
set palette defined (	0 '#ffffff',\
			2 '#000066');
plot "test.dat" matrix with image;
