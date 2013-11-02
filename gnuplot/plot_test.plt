set terminal svg enhanced size 500 1000 fname "Times" fsize 12
#set object 1 rectangle from screen 0,0 to screen 1,1 fillcolor rgb"white" behind
#set output "gnuplot/plot.svg"
set output "plot.svg"
set multiplot layout 2, 1
set title "A test plot"
set xlabel "x label"
set ylabel "y label"
set ytics scale 10
set xtics scale 5
#plot "gnuplot/test.dat" using 1:2 title "" with lines
plot "test.dat" using 1:2 title "data" with points
set title "A sine wave"
plot sin(x)
unset multiplot
