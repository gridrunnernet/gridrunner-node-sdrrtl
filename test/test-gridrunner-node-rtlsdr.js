var dongle = require('../build/Release/gridrunner-node-sdrrtl');

    exports.countdongles = function(test){
        test.expect(1);
        var donglecount=dongle.DongleCount();
        test.ok((donglecount>0), "Checked for plugged in RTL-SDR compatible dongle");
        test.done();
    };

    exports.opendongle = function(test){
        test.expect(1);
        dongle.DongleOpen(0);
        test.ok(true,"Dongle opened ok");
        test.done();
    };

    exports.setfrequency = function(test){
            test.expect(1);
            var frequency=88910000; //specified in HZ - set to BBC radio 2
            dongle.DongleSetCenterFreq(frequency);
            var setfrequency=dongle.DongleGetCenterFreq();
            test.ok((frequency==setfrequency), "Frequency of dongle can't be set");
            test.done();
    };

    exports.setsamplerate = function(test){
            test.expect(1);
            var samplerate=2048000; //specified in HZ
            dongle.DongleSetSampleRate(samplerate);
            var newrate= dongle.DongleGetSampleRate();
            test.ok((samplerate==newrate), "Sample rate of dongle can't be set");
            test.done();
    };

    exports.setfrequencycorrection = function(test){
        test.expect(1);
        var correction=10; //specified in ppm
        dongle.DongleSetFreqCorrection(correction);
        var actual = dongle.DongleGetFreqCorrection();
        var newrate= dongle.DongleGetSampleRate();
        test.ok((correction==actual), "Frequency correction can't be set");
        test.done();
};

    exports.directsampleingcheck = function(test){
        test.expect(2);
        dongle.DongleDirectSamplingOn();
        test.ok((dongle.DongleGetDirectSampling()), "Couldn't turn on direct sampling");
        dongle.DongleDirectSamplingOff();
        test.ok(!(dongle.DongleGetDirectSampling()), "Couldn't turn off direct sampling");
        test.done();
    };

exports.gaincontolcheck = function(test){
    test.expect(1);
    dongle.DongleManualGainOn();
    var gains = dongle.DongleGetTunerGains();   //returns an array of available gain
    var mygain=gains[gains.length-1]
    dongle.DongleSetTunerGain(mygain);
    var actual = dongle.DongleGetTunerGain();
    test.ok((actual==mygain), "Couldn't set manual gain");
    test.done();
};

exports.directsampleingcheck = function(test){
    test.expect(0);
    //TODO test the test mode ;)
    dongle.DongleTestModeOn();
    dongle.DongleTestModeOff()
    test.done();
};

exports.info = function(test){
   console.log("---------------DONGLE TUNER INFO------------------------");
   console.log(dongle.DongleString(0));
   console.log(dongle.DongleGetTunerType());
   console.log("--------------------------------------------------------");
   test.done();
 }

exports.closedongle = function(test){
    test.expect(1);
    dongle.DongleClose(0,function(err,data){
        if(err){
            console.log(err);
            test.ok(false,"Couldn't close dongle");
            test.done();
        }
        else{
            test.ok(true,"Dongle closed ok "+data);
            test.done();
        }
    });
};

exports.streamtest = function(test){
    test.expect(1);

    var ondata = function(err,data){
        dongle.DongleClose();
        if(err){
            test.ok(false,"Couldn't read data");
        }
        else{
            console.log("HERE");
            test.ok(true,"Called back with data");
            dongle.DongleStopRead();
            dongle.DongleClose();
            test.done();
        }
    };


    dongle.DongleOpen(0);
    var frequency=88910000; //specified in HZ
    dongle.DongleSetCenterFreq(frequency);
    dongle.DongleSetSampleRate(2048000);
    dongle.DongleTestModeOn();
    dongle.DongleResetBuffer();
    console.log(dongle.DongleReadAsync(ondata));


};





