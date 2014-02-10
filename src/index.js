var dongle = require('../build/Release/gridrunner-node-sdrrtl');

dongle.DongleOpen(0,function(err,data,myobject){
    if(err){
        console.log(err);
    }
    else{
    console.log(data);
    }
});

var frequency=88910000; //specified in HZ
dongle.DongleSetCenterFreq(frequency);
dongle.DongleSetSampleRate(2048000);
dongle.DongleTestModeOn();
dongle.DongleResetBuffer();
console.log(dongle.DongleReadAsync());


