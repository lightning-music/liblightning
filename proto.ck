// a prototype for lightning-server written in ChucK
function void playBuf(string file, float gain, float speed) {
    SndBuf buf => dac;
    file => buf.read;
    0 => buf.pos;
    gain => buf.gain;
    speed => buf.rate;
    buf.length() => now;
}

function void go(Event play, string file) {
    while (1) {
        play => now;
        spork ~ playBuf(file, Math.random2f(0.01, 0.5), Math.random2f(0.5, 1.5));
    }
}

Event e;
spork ~ go(e, me.arg(0));
while (1) {
    e.signal();
    200::ms => now;
}
