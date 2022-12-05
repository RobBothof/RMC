#include "audio.hpp"
#include "lpd.hpp"
#include "masterControl.hpp"

#include <math.h>
#include <iostream>
#include <stdlib.h>

#include "externals/Externals.h"

jack_client_t *jackclient;

jack_default_audio_sample_t *out1;
jack_default_audio_sample_t *out2;
jack_default_audio_sample_t *out3;
jack_default_audio_sample_t *out4;
jack_default_audio_sample_t *in1;
jack_default_audio_sample_t *in2;

jack_port_t *portOut01;
jack_port_t *portOut02;
jack_port_t *portOut03;
jack_port_t *portOut04;
jack_port_t *portIn01;
jack_port_t *portIn02;

float *output; 
float *input;

int ticks=0;

pd::PdBase pdBase;
pd::Patch patch;
PdObject pdObject;

unsigned int sampleiter;

void lpd::sendBang(const std::__cxx11::string &dest) {
  pdBase.sendBang(dest);
}
void lpd::sendFloat(const std::__cxx11::string &dest, float value) {
  pdBase.sendFloat(dest,value);
}
void lpd::sendList(const std::__cxx11::string &dest,std::vector<MC::polynote> v) {
  if (libpd_start_message(6)) { // request space for 16 elements
    // handle allocation failure, very unlikely in this case
  }
  libpd_add_float(v.size());
  for (int i=0; i< v.size(); i++) {
    if (v[i].voice==0) {
      libpd_add_float(v[i].note); // if empty set default pitch to zero
    } 
  }
  for (int i=0; i< v.size(); i++) {
    if (v[i].voice==1) {
      libpd_add_float(v[i].note); // if empty set default pitch to zero
    } 
  }
  for (int i=0; i< v.size(); i++) {
    if (v[i].voice==2) {
      libpd_add_float(v[i].note); // if empty set default pitch to zero
    } 
  }
  for (int i=0; i< v.size(); i++) {
    if (v[i].voice==3) {
      libpd_add_float(v[i].note); // if empty set default pitch to zero
    } 
  }

  // if (v.size()==0) {
  //   //libpd_add_float(0); // if empty set default pitch to zero
  // } else {
  //   libpd_add_float(v[v.size()-1]); //last note for monophonic mode
  // }
  // for (int i=0;i<v.size();i++) {
  //   libpd_add_float(v[i]);
  // }
  libpd_finish_list(dest.c_str());
}

void lpd::sendList(const std::__cxx11::string &dest,std::vector<int> v) {
  if (libpd_start_message(18)) { // request space for 16 elements
    // handle allocation failure, very unlikely in this case
  }
  libpd_add_float(v.size());
  if (v.size()==0) {
    //libpd_add_float(0); // if empty set default pitch to zero
  } else {
    libpd_add_float(v[v.size()-1]); //last note for monophonic mode
  }
  for (int i=0;i<v.size();i++) {
    libpd_add_float(v[i]);
  }
  libpd_finish_list(dest.c_str());
}


void lpd::receiveMessages() {
  pdBase.receiveMessages(); ///stored in lpd::buffers
  //pdBase.receiveMidi();
}

int audio::shutdown () {
  jack_client_close (jackclient);
}

int audioCallback(jack_nframes_t nframes, void *arg) {
  // get pointers to the output buffers
  //IOManager* iom = static_cast<IOManager*>(arg);

  out1 = (jack_default_audio_sample_t *) jack_port_get_buffer(portOut01, nframes);
  out2 = (jack_default_audio_sample_t *) jack_port_get_buffer(portOut02, nframes);
  out3 = (jack_default_audio_sample_t *) jack_port_get_buffer(portOut03, nframes);
  out4 = (jack_default_audio_sample_t *) jack_port_get_buffer(portOut04, nframes);
  
  in1  = (jack_default_audio_sample_t *) jack_port_get_buffer(portIn01, nframes);
  in2  = (jack_default_audio_sample_t *) jack_port_get_buffer(portIn02, nframes);
  
  ticks = nframes / 64;
  
  for(sampleiter=0; sampleiter<nframes; sampleiter++){
    input[sampleiter*2]=(*in1);
    input[sampleiter*2+1]=(*in2);
    in1++;
    in2++;
  }   
  
  pdBase.processFloat(ticks,(float*) input, (float *) output);

  for(sampleiter=0; sampleiter<nframes; sampleiter++){
    *out1 = output[sampleiter * 4];
    *out2 = output[(sampleiter * 4)+1];
    *out3 = output[(sampleiter * 4)+2];
    *out4 = output[(sampleiter * 4)+3];
    out1++;
    out2++;
    out3++;
    out4++;

  }
  return 0;
}
int audio::loadPatch() {
	pdBase.closePatch(patch);
  std::cout << patch << " closed.." << std::endl;

  // load the patch
  patch = pdBase.openPatch("main.pd", "./pd");
  std::cout << patch << " loaded.." << std::endl;
  return 0;
}
//int audioSetup(IOManager *iom) {
int audio::init() {
  std::cout << "\n**** starting audio\n" << std::endl;

  // create jack client
  if((jackclient = jack_client_open("RMC - Robber Master Control", JackNullOption, NULL)) == NULL){
    std::cerr << "Jack server not running?" << std::endl;
    exit(1);
  }

  int samplerate = jack_get_sample_rate(jackclient);
  int buffersize = jack_get_buffer_size(jackclient);

  std::cout << "\nsamplerate: ";
  std::cout << samplerate;
  std::cout << "\nbuffersize: ";
  std::cout << buffersize;
  std::cout << "\n";

   //setup input/output audio buffers

  output = (float*)malloc(buffersize*4*sizeof(float));
  input  = (float*)malloc(buffersize*2*sizeof(float));
 
  // init pd
  std::cout << "\n**** starting LibPD\n" << std::endl;

//  if(!iom->pd.init(2, 4, samplerate,false)) { //not queued
  if(!pdBase.init(2, 4, samplerate,true)) { //queued
    std::cerr << "Could not init pd" << std::endl;
    exit(1);
  }

  freeverb_tilde_setup();
  vcf_filter_tilde_setup();
  iem_cot4_tilde_setup();
  iem_sqrt4_tilde_setup();
  filter_tilde_setup();

  pdBase.addToSearchPath("pd/lib");
   // receive messages from pd
  pdBase.setReceiver(&pdObject);
  pdBase.setMidiReceiver(&pdObject);

  pdBase.subscribe("cppLoopIndexer");
  pdBase.subscribe("editLeds");
  pdBase.subscribe("noteLeds");
  pdBase.subscribe("bpm");
  pdBase.subscribe("root");

//  iom->pd.subscribe("midiclock");
//  iom->pd.subscribe("midistart");


  audio::loadPatch();

  // send DSP 1 message to pd
  pdBase.computeAudio(true);

  // register jack ports
  portOut01 = jack_port_register(jackclient, "Main_L", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
  portOut02 = jack_port_register(jackclient, "Main_R", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
  portOut03 = jack_port_register(jackclient, "Mon_L", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
  portOut04 = jack_port_register(jackclient, "Mon_R", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
  portIn01 = jack_port_register(jackclient, "Input_L", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
  portIn02 = jack_port_register(jackclient, "Input_R", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);

  // register audio process callback
  jack_set_process_callback(jackclient, audioCallback, 0);

  // go!
  if (jack_activate (jackclient)) {
    std::cout << "Could not activate client";
    exit(1);
  }

  const char **ports;
   
  //Connect ports. 
  int j=0;
  //input

  ports = jack_get_ports (jackclient, NULL, NULL,JackPortIsPhysical|JackPortIsOutput);
  if (ports == NULL) {
    fprintf(stderr, "no physical capture ports\n");
    exit (1);
  } else {
    std::cout << "available capture ports:\n";
    while(ports[j] != NULL) {
      std::cout << ports[j] << "\n";
      j++;
    }
  }
  if (j > 1) {
    if (jack_connect (jackclient, ports[2], jack_port_name (portIn01))) {
      fprintf (stderr, "cannot connect input ports\n");
    }
    if (jack_connect (jackclient, ports[3], jack_port_name (portIn02))) {
      fprintf (stderr, "cannot connect input ports\n");
    }
  }
  free(ports);

  //output
  j=0;
  ports = jack_get_ports (jackclient, NULL, NULL,JackPortIsPhysical|JackPortIsInput);
  if (ports == NULL) {
    fprintf(stderr, "no physical plackback ports\n");
    exit (1);
  } else {
    std::cout << "available playback ports:\n";
    while(ports[j] != NULL) {
      std::cout << ports[j] << "\n";
      j++;
    }
  }

  if (j>3) {
    if (jack_connect (jackclient, jack_port_name (portOut01),ports[0])) {
      fprintf (stderr, "cannot connect port: Main_L\n");
    }
    if (jack_connect (jackclient, jack_port_name (portOut02),ports[1])) {
      fprintf (stderr, "cannot connect port: Main_R\n");
    }
    if (jack_connect (jackclient, jack_port_name (portOut03),ports[2])) {
      fprintf (stderr, "cannot connect port: Mon_L\n");
    }
    if (jack_connect (jackclient, jack_port_name (portOut04),ports[3])) {
      fprintf (stderr, "cannot connect port: Mon_R\n");
    }
  } else {
    fprintf(stderr, "not enough  physical playback ports\n");
    exit(1);
  }

  free(ports);

}


