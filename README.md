# RMC
code for Musical instrument aimed at live performance / improvisation

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
