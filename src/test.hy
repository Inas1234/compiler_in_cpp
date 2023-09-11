include "io.hy"

let i = 0;
let x = 2 + 1;

let j = 0;
for(i; i < 5; i = i + 1;) {
    j = 0;
    for(j; j ?< i; j = j + 1;) {
        prints("*");

    }
    println;
}


exit (0);