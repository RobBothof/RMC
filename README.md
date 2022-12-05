# RMC

The RMC is a complete music improvisation setup build entirely from scratch. It is my personal live improvisation rig and the result of years of experimentation, improvisation with all kinds of d.i.y. instruments and live setups. Inputs feature Japanese arcade controls and high quality pots and faders via a custom low latency input scheme. Processing is done on the UP2 development board with Intel Celeron N3350 CPU, 2 Gb ram, running debian linux in combination with a teensy 3.6 board (for input scanning and led control). Software is developed in C++ in combination with puredata (in the form of libpd) as an audio engine. It used the SDL library for graphics. Dual Midi-Clock output is provided to jam with friends in sync.

This Repository holds the code for this project. It is written in C++ and uses LibPD as an audio engine. It also a includes a couple of custom PureData externals written in C.


![rob-bothof-RMC-00-scaled](https://user-images.githubusercontent.com/10440099/205630137-19375b77-c200-49c6-a073-ec51e47442cf.jpg)


Structure overview:

    main handles initialisation, update and shutdown
    main handles core gfx system
    main handles events, although most events will come from controller

    midiUart.h      -midi via uart serial bus
                    namespace midiUart {
                        int init();
                        void sendClock();
                        void sendClockStart();
                        void shutdown();
                    }
                    
    midiLocal      - midi via rTMidi
                    namespace midiLocal {
                        init()
                        send(byte)
                        send(byte,byte)
                        send(byte,byte,byte)
                        shutdown()
                    }

    filesystem.h    - reading directories
                    namespace fs {
                        extern std::string directoryCurrent;
                        extern std::vector<dirEnty> directoryContents;
                        int getDirectory();
                    
                        todo: 
                        directoryIndex -> counter for current selected entry
                        getSelected();
                    }

    gui.h           - drawing functions
                    namespace gui {
                        renderText
                        renderLoops
                        renderBrowser
                    }

    audio.h         - does audioCallback jack <-> pd / pdobject
                    - implements pd-Object
                    - sending and receiving to pd directly
                    namespace audio {
                        init()
                        audioCallback
                    
                        shutdown()
                    }

                    !! VU and Loop should be part of GUI, global if nessecairy
                
    controller.h    - handles input from teensy
                    - output arcade leds via spi
                    - output leds via i2c
                    namespace controller {
                        extern controlbuffer
                        extern inputbuffer
                        extern spibuffer
                    }

    functions.h     - various tool and helper functions
