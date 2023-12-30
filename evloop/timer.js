var sec = 0;

function timer() {
    print(++sec, "seconds");
    if (sec < 5)
        setTimeout(timer, 1000);
    else
        print("done");
}

print("Starting timer");
setTimeout(timer, 1000);
