include "io.hy"

let i = 0;
let x = 2 + 1;

let j = 0;
for(i; i < 5; i = i + 1;) {
    j = 0;
    for(j; j < 5; j = j + 1;) {
        if (i ?= j){
            prints("*");
        }else {
            prints(" ");
        }

    }
    println;
}


exit (0);