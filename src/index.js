var dongle = require('../build/Release/gridrunner-node-sdrrtl');

var ondata = function(err,data){
    if(err){
        console.log(err);
    }
    else{
        console.log(data);
    }
};

dongle.DongleOpen(0);
var frequency=88910000; //specified in HZ
dongle.DongleSetCenterFreq(frequency);
dongle.DongleSetSampleRate(2048000);
dongle.DongleTestModeOn();
dongle.DongleResetBuffer();
console.log(dongle.DongleReadAsync(ondata));