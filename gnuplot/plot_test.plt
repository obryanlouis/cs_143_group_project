set terminal svg enhanced size 1000 1000 fname "Times" fsize 36
set object 1 rectangle from screen 0,0 to screen 1,1 fillcolor rgb"white" behind
set output "gnuplot/plot.svg"
set title "A test plot"
set xlabel "x"
set ylabel "y"
plot "gnuplot/test.dat" using 1:2 title "" with lines
