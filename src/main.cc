#include <node.h>
#include <v8.h>
#include <rtl-sdr.h>
#define DEFAULT_ASYNC_BUF_NUMBER 32
#define DEFAULT_BUF_LENGTH (16 * 16384)
static rtlsdr_dev_t *dev;
using namespace v8;

v8::Persistent<v8::Function> dataCallback;
static v8::Local<v8::Context> mycontext;

Handle<Value> DongleCount(const Arguments& args) {
    HandleScope scope;
    int radiodongles  = rtlsdr_get_device_count();
    return scope.Close(Integer::New(radiodongles));
}

Handle<Value> DongleName(const Arguments& args) {
    HandleScope scope;
    if (args.Length() < 1|| !args[0]->IsNumber() )  {
        ThrowException(Exception::TypeError(String::New("Specify by index number (integer) which dongle to read - try zero if in doubt")));
        return scope.Close(Undefined());
    }
    int di = args[0]->NumberValue();
    const char* donglename = rtlsdr_get_device_name(di);
    return scope.Close(String::New(donglename));
}

Handle<Value> DongleString(const Arguments& args) {
    HandleScope scope;
    if (args.Length() < 1|| !args[0]->IsNumber() )  {
        ThrowException(Exception::TypeError(String::New("Specify by index number (integer) which dongle to read - try zero if in doubt")));
        return scope.Close(Undefined());
    }
    char vendor[256], product[256], serial[256];
    vendor[0] = '\0';
    product[0] = '\0';
    serial[0] = '\0';
    int result = rtlsdr_get_device_usb_strings(0, vendor, product, serial);
    Local<Object> obj = Object::New();
    if(result>=0){
        obj->Set(String::NewSymbol("vendor"), String::New(vendor));
        obj->Set(String::NewSymbol("product"), String::New(product));
        obj->Set(String::NewSymbol("serial"), String::New(serial));
    }
    else{
        obj->Set(String::NewSymbol("error"), String::New("Could not read dongle info - is it plugged in?"));
    }
    return scope.Close(obj);
}

Handle<Value> DongleOpen(const Arguments& args) {
    HandleScope scope;
    v8::Local<v8::String> myerror = String::New("");
      if (args.Length() < 1|| !args[0]->IsNumber() )  {
          myerror=String::New("Specify by index number (integer) which dongle to open");
      }
      else{
        int di = args[0]->NumberValue();
        int result = rtlsdr_open(&dev, di);

        if (result < 0){
            ThrowException(Exception::TypeError(String::New("Couldn't open dongle - is it plugged in?")));
        }
      }
     return scope.Close(String::New("Dongle Open"));
}

Handle<Value> DongleClose(const Arguments& args) {
    HandleScope scope;
        v8::Local<v8::String> myerror = String::New("");
        v8::Local<v8::String> mydata = String::New("");
          if (args.Length() < 1|| !args[0]->IsNumber() )  {
              myerror=String::New("Specify by index number (integer) which dongle to close");
          }
         else{
            int di = args[0]->NumberValue();
            int result = rtlsdr_close(dev);
            if ( args[1]-> IsFunction() ){
                Local<Function> cb = Local<Function>::Cast(args[1]);
                if (result<0){
                    myerror=String::New("Error closing dongle - is it plugged in?");
                }
                else{
                    mydata=String::New(rtlsdr_get_device_name(di));
                }
                Local<Value> argv[2] = { myerror, mydata };
                cb->Call(Context::GetCurrent()->Global(), 2, argv);
             }
         }
    return scope.Close(String::New("Dongle Closed"));
}

Handle<Value> DongleGetCenterFreq(const Arguments& args) {
      HandleScope scope;
      int frequency = rtlsdr_get_center_freq(dev);
      return scope.Close(Integer::New(frequency));
}

Handle<Value> DongleSetCenterFreq(const Arguments& args) {
    HandleScope scope;
    if (args.Length() < 1|| !args[0]->IsNumber() )  {
        ThrowException(Exception::TypeError(String::New("Specify a frequency to tune to")));
        return scope.Close(Undefined());
    }
    double frequency = args[0]->NumberValue();
    rtlsdr_set_center_freq(dev,frequency);
    return scope.Close(Integer::New(frequency));
}

Handle<Value> DongleSetSampleRate(const Arguments& args) {
    HandleScope scope;
    if (args.Length() < 1|| !args[0]->IsNumber() )  {
        ThrowException(Exception::TypeError(String::New("Specify a sample rate in HZ")));
        return scope.Close(Undefined());
    }
    double samplerate = args[0]->NumberValue();
    rtlsdr_set_sample_rate(dev,samplerate);
    return scope.Close(Undefined());
}

Handle<Value> DongleGetSampleRate(const Arguments& args) {
       HandleScope scope;
       int samplerate=rtlsdr_get_sample_rate(dev);
      return scope.Close(Number::New(samplerate));
}

Handle<Value> DongleResetBuffer(const Arguments& args) {
       HandleScope scope;
       rtlsdr_reset_buffer(dev);
       return scope.Close(Undefined());
}

Handle<Value> DongleGetFreqCorrection(const Arguments& args) {
     HandleScope scope;
     int correction = rtlsdr_get_freq_correction(dev);
     return scope.Close(Integer::New(correction));
}

Handle<Value> DongleSetFreqCorrection(const Arguments& args) {
    HandleScope scope;
    if (args.Length() < 1|| !args[0]->IsNumber() )  {
        ThrowException(Exception::TypeError(String::New("Specify by a correction in ppm (int) - call DongleGetFreqCorrection to find out what this value is set to.")));
        return scope.Close(Undefined());
    }
    int correction = args[0]->NumberValue();
    int error = rtlsdr_set_freq_correction(dev, correction);
    if (error < 0){
        ThrowException(Exception::TypeError(String::New("Couldn't set frequency correction on dongle - is it plugged in?")));
    }
    return scope.Close(Integer::New(error));
}

Handle<Value> DongleGetDirectSampling(const Arguments& args) {
       HandleScope scope;
       int sample = rtlsdr_get_direct_sampling(dev);
       return scope.Close(Boolean::New(sample));
}

Handle<Value> DongleDirectSamplingOn(const Arguments& args) {
       HandleScope scope;
       rtlsdr_set_direct_sampling(dev,1);
       return scope.Close(Undefined());
}

Handle<Value> DongleDirectSamplingOff(const Arguments& args) {
       HandleScope scope;
       rtlsdr_set_direct_sampling(dev,0);
       return scope.Close(Undefined());
}

Handle<Value> DongleManualGainOn(const Arguments& args) {
       HandleScope scope;
       int error = rtlsdr_set_tuner_gain_mode(dev,1);
       return scope.Close(Integer::New(error));
}

Handle<Value> DongleManualGainOff(const Arguments& args) {
       HandleScope scope;
       int error = rtlsdr_set_tuner_gain_mode(dev,0);
       return scope.Close(Integer::New(error));
}

Handle<Value> DongleGetTunerGains(const Arguments& args) {
    HandleScope scope;
    int gainvalues = rtlsdr_get_tuner_gains(dev,NULL);
    int gains[gainvalues];
    rtlsdr_get_tuner_gains(dev,gains);
    Handle<Array> array = Array::New(3);
    for (int i=0;i<gainvalues;i++){
        array->Set(i, Integer::New(gains[i]));
     }
    return scope.Close(array);
 }

Handle<Value> DongleGetTunerGain(const Arguments& args) {
     HandleScope scope;
     int gain = rtlsdr_get_tuner_gain(dev);
     return scope.Close(Integer::New(gain));
}

Handle<Value> DongleSetTunerGain(const Arguments& args) {
      HandleScope scope;
      if (args.Length() < 1|| !args[0]->IsNumber() )  {
        ThrowException(Exception::TypeError(String::New("Specify the gain (int)- call DongleGetTunerGains for available settings.")));
        return scope.Close(Undefined());
      }
      int gain = args[0]->NumberValue();
      rtlsdr_set_tuner_gain(dev,gain);
      return scope.Close(Undefined());
}

Handle<Value> DongleTestModeOn(const Arguments& args) {
       HandleScope scope;
       rtlsdr_set_testmode(dev,1);
       return scope.Close(Undefined());
}

Handle<Value> DongleTestModeOff(const Arguments& args) {
       HandleScope scope;
       rtlsdr_set_testmode(dev,0);
       return scope.Close(Undefined());
}

Handle<Value> DongleSet_TunerIfGain(const Arguments& args) {
    HandleScope scope;
    int stage=1;
    int gain=1;
    int error = rtlsdr_set_tuner_if_gain(dev, stage, gain);
    return scope.Close(Integer::New(error));
}
void emit(uv_work_t* req) {
}

void emitradiodata(uv_work_t* req,int x) {
    v8::HandleScope scope;
    Local<Context> mycontext= Context::GetCurrent();
    Handle<Array> array = Array::New(DEFAULT_BUF_LENGTH);
    char* buf=(char *)req->data;
    for (int i=0; i<DEFAULT_BUF_LENGTH; i++){
        int x = (int)buf[i];
        array->Set(i, Integer::New(x));
    }
   Local<Object> obj = Object::New();
   obj->Set(String::NewSymbol("data"),array);
   v8::Local<v8::String> myerror = String::New("");
   Local<Value> argv[2] = { myerror, obj };
   dataCallback->Call(mycontext->Global(), 2, argv);
   scope.Close(Integer::New(0));
}

void DongleOnData(unsigned char* buf, uint32_t len, void *ctx) {
  uv_work_t* req = new uv_work_t();
  req->data=buf;
  uv_queue_work(uv_default_loop(), req,emit,emitradiodata);
}

void DongleAfterRead(uv_work_t* req,int x) {
}

void DongleStartRead(uv_work_t* req) {
    rtlsdr_read_async(dev,DongleOnData,dev, DEFAULT_ASYNC_BUF_NUMBER, DEFAULT_BUF_LENGTH);
}

Handle<Value> DongleReadAsync(const Arguments& args) {
    HandleScope scope;
    dataCallback=Persistent<Function>::New(Handle<Function>::Cast(args[0]));
    uv_work_t* req = new uv_work_t();
    req->data = dev;
    uv_queue_work(uv_default_loop(), req, DongleStartRead,DongleAfterRead );
    return scope.Close(String::New("Data read started"));
}

Handle<Value> DongleGetTunerType(const Arguments& args) {
    HandleScope scope;
    enum rtlsdr_tuner tuner = rtlsdr_get_tuner_type(dev);
    const char* tunername;
    switch (tuner)
    {
      case 0:
         tunername="UNKNOWN";
         break;
      case 1:
         tunername="E4000";
         break;
      case 2:
         tunername="FC0012";
         break;
      case 3:
         tunername="FC0013";
         break;
      case 4:
         tunername="FC2580";
         break;
      case 5:
         tunername="R820T";
         break;
      case 6:
         tunername="R828D";
         break;
      default:
         tunername="NEW TYPE";
    }
     return scope.Close(String::New(tunername));
}

Handle<Value> DongleStopRead(const Arguments& args) {
    rtlsdr_cancel_async(dev);
}

void init(Handle<Object> target) {
  NODE_SET_METHOD(target, "DongleCount", DongleCount);
  NODE_SET_METHOD(target, "DongleName", DongleName);
  NODE_SET_METHOD(target, "DongleString", DongleString);
  NODE_SET_METHOD(target, "DongleOpen", DongleOpen);
  NODE_SET_METHOD(target, "DongleClose", DongleClose);
  NODE_SET_METHOD(target, "DongleGetCenterFreq", DongleGetCenterFreq);
  NODE_SET_METHOD(target, "DongleSetCenterFreq", DongleSetCenterFreq);
  NODE_SET_METHOD(target, "DongleGetSampleRate", DongleGetSampleRate);
  NODE_SET_METHOD(target, "DongleSetSampleRate", DongleSetSampleRate);
  NODE_SET_METHOD(target, "DongleResetBuffer", DongleResetBuffer);
  NODE_SET_METHOD(target, "DongleGetFreqCorrection", DongleGetFreqCorrection);
  NODE_SET_METHOD(target, "DongleSetFreqCorrection", DongleSetFreqCorrection);
  NODE_SET_METHOD(target, "DongleGetDirectSampling", DongleGetDirectSampling);
  NODE_SET_METHOD(target, "DongleDirectSamplingOff", DongleDirectSamplingOff);
  NODE_SET_METHOD(target, "DongleDirectSamplingOn", DongleDirectSamplingOn);
  NODE_SET_METHOD(target, "DongleManualGainOn", DongleManualGainOn);
  NODE_SET_METHOD(target, "DongleManualGainOff", DongleManualGainOff);
  NODE_SET_METHOD(target, "DongleAGGOn", DongleManualGainOff);
  NODE_SET_METHOD(target, "DongleGetTunerGains", DongleGetTunerGains);
  NODE_SET_METHOD(target, "DongleGetTunerGain", DongleGetTunerGain);
  NODE_SET_METHOD(target, "DongleSetTunerGain", DongleSetTunerGain);
  NODE_SET_METHOD(target, "DongleTestModeOn", DongleTestModeOn);
  NODE_SET_METHOD(target, "DongleTestModeOff", DongleTestModeOff);
  NODE_SET_METHOD(target, "DongleGetTunerType", DongleGetTunerType);
  NODE_SET_METHOD(target, "DongleReadAsync", DongleReadAsync);
  NODE_SET_METHOD(target, "DongleStopRead", DongleStopRead);
}
NODE_MODULE(gridrunner_node_sdrrtl, init);