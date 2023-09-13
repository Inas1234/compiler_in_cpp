fn pow(let x, let y) -> {
    let resultpow = 1;
    let i = 0;
    for (i; i < y; i = i + 1;) {
        resultpow = resultpow * x;
    }

    return resultpow;
}

fn sqrt(let x) -> {
    if (x < 0){
        printf("Error: negative number");
    }

    let guess = x;

    let j = 0;
    for(j; j < 10; j = j + 1;){
        guess = guess / 2 + x / guess / 2;
    }

    return guess;

}