/*This source code copyrighted by Rob Bothof (2004-2014) and may not
be redistributed without written permission.*/

#include <functions.hpp>
#include <filesystem.hpp>
//#include <SDL2/SDL.h>

#include <fstream>      // for getTemp
#include <iomanip>      // std:setfill
//#include <stdlib.h>     // atoi 

#include <string>

#include <iostream>



//const std::string notes [12] = {"C-","C#","D-","D#","E-","F-","F#","G-","G#","A-","A#","B-"};

int getTemp() {
    std::string val;
    std::ifstream ifile("/sys/devices/virtual/thermal/thermal_zone0/temp");
    ifile >> val;
    ifile.close();
    return std::stoi(val);
}


// void renderLoops(GPU_Target* r,IOManager *iom, loop &l) {
//     int index = iom->pdObject.loopIndex; 
    
//     if (index < l.lastIndex) {
//       index=768;
//     }

//     for (int i = l.lastIndex; i <index;i++) {
//       float v1 = iom->pdObject.loop1buffer[i];
//       float v2 = iom->pdObject.loop2buffer[i];
//       float v3 = iom->pdObject.loop3buffer[i];
//       float v4 = iom->pdObject.loop4buffer[i];
      
//       if (v1*80 < 1) v1 = 0.015;
//       if (v2*80 < 1) v2 = 0.015;
//       if (v3*80 < 1) v3 = 0.015;
//       if (v4*80 < 1) v4 = 0.015;
          
//       //clear old samples
//       GPU_RectangleFilled(l.target,i,0,i+1,350,l.palette->colors[19]);

//       //draw samples
//       GPU_RectangleFilled(l.target,i,40-v1*40,i+1,40+v1*40,l.palette->colors[8]);
//       GPU_RectangleFilled(l.target,i,130-v2*40,i+1,130+v2*40,l.palette->colors[9]);
//       GPU_RectangleFilled(l.target,i,220-v3*40,i+1,220+v3*40,l.palette->colors[10]);
//       GPU_RectangleFilled(l.target,i,310-v4*40,i+1,310+v4*40,l.palette->colors[11]);
      
//     }
//     if (index==768) {
//       l.lastIndex=0;
//     } else {
//       l.lastIndex=index;
//     }
    
//     //draw writepointer
//     GPU_RectangleFilled(l.target,l.lastIndex,  0,l.lastIndex+1, 80,l.palette->colors[13]);
//     GPU_RectangleFilled(l.target,l.lastIndex, 90,l.lastIndex+1,170,l.palette->colors[13]);
//     GPU_RectangleFilled(l.target,l.lastIndex,180,l.lastIndex+1,260,l.palette->colors[13]);
//     GPU_RectangleFilled(l.target,l.lastIndex,270,l.lastIndex+1,350,l.palette->colors[13]);

// }
// /// ------------------- *** Bitmap Font Rendering *** ------------------- ///

// void renderText(GPU_Target* r, bmpFont &f, int x, int y, std::string text) {
//   //specify rendering regions
//   f.src.w = f.char_w;
//   f.src.h = f.char_h;

//   for( int i = 0; i < text.length(); ++i ) {
//     uint8_t chr = (uint8_t)text[ i ];
//     //set color 0-15 if ascii value is 240-255
//     if (chr > 239) {
//       // i do not specify a default earlier, so color can inherit from a previous render
//       GPU_SetColor(f.t,f.palette->colors[chr-240]);
//     } else {
//       //not a color, render the chars
//       //locate the position in the bitmapfont texture
//       f.src.x = (chr %  f.w) * f.char_w;
//       f.src.y = (chr /  f.w) * f.char_h;
//       GPU_Blit(f.t, &f.src, r, x*f.char_w,y*f.char_h);
//       x++; // advance cursor
//     }
//   }
// }
// void renderDirectory(GPU_Target* r, bmpFont &font, bmpFont &fontBig, int x, int y, int index) {
//     renderText(r, font, x, y, "\xFB     " + fs::directoryCurrent);
//     renderText(r, fontBig, x, y+1, "\xFD\x88\x89");
//     for (unsigned i=index;i<std::min(index+6,(int) fs::directoryContents.size());i++) {
// //      std::cout << directoryContents[i] << std::endl;
//       if (fs::directoryContents[i].isDir) {
//       renderText(r, fontBig, x+2, y+i+1, "\xFC\x80\x81");
//       } else {
//       renderText(r, fontBig, x+2, y+i+1, "\xFA\x86\x87");
//       }
//       renderText(r, fontBig, x+5, y+i+1, fs::directoryContents[i].filename);
//     }
// }

// void loadKeyMap(pattern_editor &e, std::string path) {
//   //clear the keymap table
//   memset(e.kmap, 0, sizeof(e.kmap[0][0]) * 256 * 7);
//   std::ifstream keymapfile(path.c_str());
//   std::string line;
//   std::string word;
//   int i=0;
//   int j=0;
//   if (keymapfile.is_open()) { //if the file is open
//     while (std::getline(keymapfile, line))
//       {
// 	std::stringstream ss(line);
// 	i=0;
// 	while(ss && ss >> word)
// 	  {
// 	    if (word[0] == '#') break;
// 	    if (i==1) {
// 	      j=atoi(word.c_str()); // get scancode
// 	    }
// 	    if ((i > 1) && (i < 9)) {
// 	      e.kmap[j][i-2] = atoi(word.c_str()); // load modifier values
// 	    }
// 	    i++;
// 	  }
//       }
//     keymapfile.close();
//   }
// }

// int getKey(pattern_editor &e, int scancode) {
//   int j=0;
//   if ((SDL_GetModState() & e.mod[5]) > 0) j=6;
//   if ((SDL_GetModState() & e.mod[4]) > 0) j=5;
//   if ((SDL_GetModState() & e.mod[3]) > 0) j=4;
//   if ((SDL_GetModState() & e.mod[2]) > 0) j=3;
//   if ((SDL_GetModState() & e.mod[1]) > 0) j=2;
//   if ((SDL_GetModState() & e.mod[0]) > 0) j=1;
//   return e.kmap[scancode][j];
// }

/// ------------------- *** Pattern Drawing *** ------------------- ///
/* 
void drawPatternEditor(SDL_Renderer* r, bmpFont &f, pattern_editor &e, pattern &p, int playrow) {
  //e.y=playrow;
  int editrow = e.drawrect.y + e.drawrect.h / 2;
  std::string row_index;
  std::string lines;
  std::string playlines;
  std::string index_color;
  int row, baralpha, barcolor, p_row, fontalpha, cursorcolor;
  
  cursorcolor=17;
  
  playlines="\xFE""      ""\x1C""             ""\x1D""  ""\x1C""             ""\x1D""  ""\x1C""             ""\x1D""  ""\x1C""             ""\x1D";
  
  for (int i = e.drawrect.y; i < e.drawrect.y + e.drawrect.h; i ++) {
    
    //row is the current pattern-row at screenposition i
    row = e.y  + i - ( e.drawrect.h / 2  + e.drawrect.y );
    
    ///set some drawing variables for pattern, off-pattern and editbar
    
    //default pattern:
    baralpha=60;
    fontalpha=255;
    barcolor=0;
    lines="\xF0""\xB3""    ""\xB3""               ""\xB3""""\xB3""               ""\xB3""""\xB3""               ""\xB3""""\xB3""               ""\xB3";
    index_color="\xF2";
    p_row=row;
    
    //editbar:
    if (i == editrow) {
      baralpha=110;
      barcolor=5;
      lines="\xF1""\x10""    ""\x11""               ""\x10""""\x11""               ""\x10""""\x11""               ""\x10""""\x11""               ""\x10";
      index_color="\xF3";
    }
    //off-pattern:
    if (row < 0) {
      baralpha=20;
      fontalpha=110;
      p_row=row+p.length;
    }
    if (row >= p.length) {
      baralpha=20;
      fontalpha=110;
      p_row=row-p.length;
    }
    
    ///draw the beat bars or editbar
    if ((i == editrow) || (row % 8 == 0)) {
      SDL_SetTextureColorMod( e.bar, f.color[barcolor][0], f.color[barcolor][1], f.color[barcolor][2]);
      SDL_SetTextureAlphaMod( e.bar, baralpha);
      e.bar_rect.w = e.drawrect.w * f.char_w;
      e.bar_rect.y = i * f.char_h;
      if (i == editrow) {
	e.bar_rect.h = f.char_h;
      } else {
	e.bar_rect.h = f.char_h-1;
      }
      SDL_RenderCopy( r, e.bar, NULL, &e.bar_rect);
    }
    
    if (i == editrow) {
      //1px highlight line at top
      SDL_SetTextureColorMod( e.bar, f.color[3][0], f.color[3][1], f.color[3][2]);
      SDL_SetTextureAlphaMod( e.bar, 50);
      e.bar_rect.w = e.drawrect.w * f.char_w;
      e.bar_rect.y = i * f.char_h-1;
      e.bar_rect.h = 2;
      SDL_RenderCopy( r, e.bar, NULL, &e.bar_rect);
      //1px shadow line at bottom
      SDL_SetTextureColorMod( e.bar, f.color[16][0], f.color[16][1], f.color[16][2]);
      SDL_SetTextureAlphaMod( e.bar, 50);
      e.bar_rect.y = i * f.char_h + f.char_h - 1;
      SDL_RenderCopy( r, e.bar, NULL, &e.bar_rect);
    }
    
    e.bar_rect.h = f.char_h;
    ///draw the lines
    renderText(r, f, 0, i, lines);
    if (row == playrow) {
      renderText(r, f, 0, i, playlines);
    }
    ///draw the line indexes
    SDL_SetTextureAlphaMod( f.t, fontalpha);
    renderText(r, f, 2, i, index_color + toStr(p_row));
    
    ///draw the cursor
    if (i == editrow) {
      //cursor block
      SDL_SetTextureAlphaMod(e.bar, 200);
      SDL_SetTextureColorMod(e.bar, f.color[16][0], f.color[16][1], f.color[16][2]);
      e.bar_rect.y = (e.drawrect.y + e.drawrect.h - (e.drawrect.h + 1) / 2) * f.char_h+2;
      e.bar_rect.w = f.char_w * e.positions[e.x][1]+3;
      e.bar_rect.x = f.char_w * e.positions[e.x][0]-2;
      e.bar_rect.h = f.char_h-4;
      SDL_RenderCopy( r, e.bar, NULL, &e.bar_rect);
      //1px shadow line at top
      SDL_SetTextureAlphaMod( e.bar, 100);
      e.bar_rect.y = (e.drawrect.y + e.drawrect.h - (e.drawrect.h + 1) / 2) * f.char_h+1;
      e.bar_rect.h = 1;
      SDL_RenderCopy( r, e.bar, NULL, &e.bar_rect);
      //1px highlight line at top
      SDL_SetTextureAlphaMod( e.bar, 50);
      SDL_SetTextureColorMod(e.bar, f.color[3][0], f.color[3][1], f.color[3][2]);
      e.bar_rect.y = (e.drawrect.y + e.drawrect.h - (e.drawrect.h + 1) / 2) * f.char_h + f.char_h-2;
      e.bar_rect.h = 1;
      SDL_RenderCopy( r, e.bar, NULL, &e.bar_rect);
      //reset editrect
      e.bar_rect.x = 0;
      e.bar_rect.h = f.char_h;
    }
    
    ///draw actual pattern
    int newval;
    int fx;
    for (int ch = 0; ch < 4; ch++) {
      bool noteoff=false;
      //notes
      newval = p.table[ch*8][p_row];
      if (newval < 254) {
	renderText(r, f, e.positions[ch*9][0], i, "\xF1" + notes[p.table[ch*8][p_row] % 12] + toHex1(p.table[ch*8][p_row] / 12));
      } else {
	if (newval==254) {
	  renderText(r, f, e.positions[ch*9][0], i, "\xF1""\xCD""\xCD""\xCD"" ""\xCD""\xCD"" ""\xCD""\xCD");
	  noteoff=true;
	} else {
	  renderText(r, f, e.positions[ch*9][0], i, "\xF0""\xB7""\xB7""\xB7");
	}
      }
      if (noteoff==false) {
	//instruments
	newval = p.table[ch*8+1][p_row];
	if (newval != 255) {
	  renderText(r, f, e.positions[ch*9+2][0], i, "\xF1" + toStr(p.table[ch*8+1][p_row]));
	} else {
	  renderText(r, f, e.positions[ch*9+2][0], i, "\xF0""\xB7""\xB7");
	}
	//volume
	newval = p.table[ch*8+2][p_row];
	if (newval != 255) {
	  renderText(r, f, e.positions[ch*9+4][0], i, "\xF1" + toStr(p.table[ch*8+2][p_row]));
	} else {
	  renderText(r, f, e.positions[ch*9+4][0], i, "\xF0""\xB7""\xB7");
	}
      }
      //effect
      newval = p.table[ch*8+3][p_row];
      if (newval != 255) {
	renderText(r, f, e.positions[ch*9+6][0], i, "\xF1" + toChar(p.table[ch*8+3][p_row]));
      } else {
	renderText(r, f, e.positions[ch*9+6][0], i, "\xF0""\xB7");
      }
      fx=newval;
      //effect value , uses newval from effect
      newval = p.table[ch*8+4][p_row];
      if (newval != 255) {
	if(fx != 255) {
	  renderText(r, f, e.positions[ch*9+7][0], i, "\xF1" + toStr(p.table[ch*8+4][p_row]));
	} else {
	  renderText(r, f, e.positions[ch*9+7][0], i, "\xF0" + toStr(p.table[ch*8+4][p_row]));
	}
      } else {
	if(fx != 255) {
	  renderText(r, f, e.positions[ch*9+7][0], i, "\xF1""00");
	} else {
	  renderText(r, f, e.positions[ch*9+7][0], i, "\xF0""00");
	}
      }
    }

    //reset font alpha
    SDL_SetTextureAlphaMod ( f.t, 255);

  }
}

/// ------------------- *** Pattern Editing *** ------------------- ///


void editUp(pattern &p, pattern_editor &e) {
  if (e.y > 0) {
    e.y--;
  } else {
    e.y=p.length-1;
  }
}
void editDown(pattern &p, pattern_editor &e) {
  if (e.y < p.length-1) {
    e.y++;
  } else {
    e.y=0;
  }
}
void editLeft(pattern &p, pattern_editor &e) {
  if (e.x > 0) {
    e.x--;
  } else {
    e.x=35;
  }
}
void editRight(pattern &p, pattern_editor &e) {
  if (e.x < 35) {
    e.x++;
  } else {
    e.x=0;
  }
}
void editTABRight(pattern &p, pattern_editor &e) {
  if (e.x < 27) {
    e.x+=9;
  } else {
    e.x-=27;
  }
}
void editTABLeft(pattern &p, pattern_editor &e) {
  if (e.x > 8) {
    e.x-=9;
  } else {
    e.x+=27;
  }
}
void editIns(pattern &p, pattern_editor &e) {
  for (int x = (e.x / 9) * 8; x < (e.x / 9) * 8 + 8; x++) {
    for (int y=p.length-1;y > e.y;y--) {
      p.table[x][y]=p.table[x][y-1];
    }
    p.table[x][e.y]=255; // clear current row
  }
}

void editDel(pattern &p, pattern_editor &e) {
  for (int x = (e.x / 9) * 8; x < (e.x / 9) * 8 + 8; x++) {
    for (int y=e.y; y < p.length-1;y++) {
      p.table[x][y]=p.table[x][y+1];
    }
    p.table[x][p.length-1]=255; // clear current row
  }

}
void editPgUp(pattern &p, pattern_editor &e) {
  if (e.y % 8 == 0) {
    if (e.y == 0) {
      e.y = p.length - 8;
    } else {
      e.y-=8;
    }
  } else {
    e.y=(e.y / 8) * 8;
  }
}
void editPgDown(pattern &p, pattern_editor &e) {
  if (e.y % 8 == 0) {
    if (e.y > p.length - 8) {
      e.y = 0;
    } else {
      e.y+=8;
    }
  } else {
    e.y=(e.y / 8) * 8 + 8;
  }
}

void noteLast(pattern &p, pattern_editor &e) {
  p.table[(e.x/9)*8][e.y]=e.note+e.octave*12;
  instrumentLast(p,e);
  volumeLast(p,e);
}
void noteUp(pattern &p, pattern_editor &e, int part) {
  if (p.table[(e.x/9)*8][e.y] > 253){
    if (p.table[(e.x/9)*8+2][e.y] > 253){
      volumeLast(p,e);
    }
    if (p.table[(e.x/9)*8+1][e.y] > 253){
      instrumentLast(p,e);
    }
    p.table[(e.x/9)*8][e.y]=e.note+e.octave*12;
  } else {
    e.note=p.table[(e.x/9)*8][e.y]%12;
    e.octave=p.table[(e.x/9)*8][e.y]/12;
  }
  if (part == 0) {
    if (p.table[(e.x/9)*8][e.y] < 119) {
      p.table[(e.x/9)*8][e.y]++;
    }
    e.note=p.table[(e.x/9)*8][e.y]%12;
    e.octave=p.table[(e.x/9)*8][e.y]/12;
  } else {
    if (e.octave < 9) {
      e.octave++;
      p.table[(e.x/9)*8][e.y]=e.note+e.octave*12;
    }
  }
}
void noteDown(pattern &p, pattern_editor &e, int part) {
  if (p.table[(e.x/9)*8][e.y] > 253){
    if (p.table[(e.x/9)*8+2][e.y] > 253){
      volumeLast(p,e);
    }
    if (p.table[(e.x/9)*8+1][e.y] > 253){
      instrumentLast(p,e);
    }
    p.table[(e.x/9)*8][e.y]=e.note+e.octave*12;
  } else {
    e.note=p.table[(e.x/9)*8][e.y]%12;
    e.octave=p.table[(e.x/9)*8][e.y]/12;
  }
  if (part == 0) {
    if (p.table[(e.x/9)*8][e.y] > 0) {
      p.table[(e.x/9)*8][e.y]--;
    }
    e.note=p.table[(e.x/9)*8][e.y]%12;
    e.octave=p.table[(e.x/9)*8][e.y]/12;
  } else {
    if (e.octave > 0) {
      e.octave--;
      p.table[(e.x/9)*8][e.y]=e.note+e.octave*12;
    }
  }
}
void noteEdit(pattern &p, pattern_editor &e, int part, int value) {
  if (p.table[(e.x/9)*8][e.y] > 253){
    if (p.table[(e.x/9)*8+2][e.y] > 253){
      volumeLast(p,e);
    }
    if (p.table[(e.x/9)*8+1][e.y] > 253){
      instrumentLast(p,e);
    }
    p.table[(e.x/9)*8][e.y]=e.note+e.octave*12;
  } else {
    e.note=p.table[(e.x/9)*8][e.y]%12;
    e.octave=p.table[(e.x/9)*8][e.y]/12;
  }
  if (value==19) {
    p.table[(e.x/9)*8][e.y]=254;
  } else {
    if (part==0) {
      p.table[(e.x/9)*8][e.y]=value+e.octave*12;
      e.note=value % 12;
    } else {
      if (value >= 0 && value <= 9) {
	e.octave=value;
	p.table[(e.x/9)*8][e.y]=e.note+e.octave*12;
      }
    }
  }
  editDown(p,e);
}
void noteOff(pattern &p, pattern_editor &e) {
  p.table[(e.x/9)*8][e.y] = 254;
  editDown(p,e);
}
void noteDel(pattern &p, pattern_editor &e) {
  p.table[(e.x/9)*8][e.y] = 255;
}


void instrumentLast(pattern &p, pattern_editor &e) {
  p.table[(e.x/9)*8+1][e.y]=e.instrument;
}
void instrumentUp(pattern &p, pattern_editor &e, int part) {
  if (p.table[(e.x/9)*8 + 1][e.y] < 254){
    e.instrument = p.table[(e.x/9)*8+1][e.y];
  }
  if (part == 3) {
    if (e.instrument < 64) e.instrument++;
  }
  if (part == 2) {
    if (e.instrument < 54) {
      e.instrument+=10;
    } else {
      e.instrument=64;
    }
  }
  p.table[(e.x/9)*8+1][e.y]=e.instrument;
}
void instrumentDown(pattern &p, pattern_editor &e, int part) {
  if (p.table[(e.x/9)*8 + 1][e.y] < 254){
    e.instrument = p.table[(e.x/9)*8+1][e.y];
  }
  if (part == 3) {
    if (e.instrument > 0) e.instrument--;
  }
  if (part == 2) {
    if (e.instrument > 10) {
      e.instrument-=10;
    } else {
      e.instrument=0;
    }
  }
  p.table[(e.x/9)*8+1][e.y]=e.instrument;

}
void instrumentEdit(pattern &p, pattern_editor &e, int part, int value) {
  if (p.table[(e.x/9)*8 + 1][e.y] < 254){
    e.instrument = p.table[(e.x/9)*8+1][e.y];
  } else {
    e.instrument = 0;
  }
  if (part == 3) {
    e.instrument=(e.instrument/10)*10 + value;
  }
  if (part == 2) {
    e.instrument=e.instrument%10 + value*10;
  }
  if (e.instrument > 64) {
    e.instrument=64;
  }
  p.table[(e.x/9)*8+1][e.y]=e.instrument;

  if (part == 3) {
    editDown(p,e);
    editLeft(p,e);
  } else {
    editRight(p,e);
  }
}
void instrumentDel(pattern &p, pattern_editor &e) {
  p.table[(e.x/9)*8 + 1][e.y] = 255;
}


void volumeLast(pattern &p, pattern_editor &e) {
  p.table[(e.x/9)*8+2][e.y]=e.volume;
}
void volumeUp(pattern &p, pattern_editor &e, int part) {
  if (p.table[(e.x/9)*8 + 2][e.y] < 254){
    e.volume = p.table[(e.x/9)*8+2][e.y];
  }
  if (part == 5) {
    if (e.volume < 64) e.volume++;
  }
  if (part == 4) {
    if (e.volume < 54) {
      e.volume+=10;
    } else {
      e.volume=64;
    }
  }
  p.table[(e.x/9)*8+2][e.y]=e.volume;
}
void volumeDown(pattern &p, pattern_editor &e, int part) {
  if (p.table[(e.x/9)*8 + 2][e.y] < 254){
    e.volume = p.table[(e.x/9)*8+2][e.y];
  }
  if (part == 5) {
    if (e.volume > 0) e.volume--;
  }
  if (part == 4) {
    if (e.volume > 10) {
      e.volume-=10;
    } else {
      e.volume=0;
    }
  }
  p.table[(e.x/9)*8+2][e.y]=e.volume;
}
void volumeEdit(pattern &p, pattern_editor &e, int part, int value) {
  if (p.table[(e.x/9)*8 + 2][e.y] < 254){
    e.volume = p.table[(e.x/9)*8+2][e.y];
  } else {
    e.volume = 0;
  }
  if (part == 5) {
    e.volume=(e.volume/10)*10 + value;
  }
  if (part == 4) {
    e.volume=e.volume%10 + value*10;
  }
  if (e.volume > 64) {
    e.volume=64;
  }
  p.table[(e.x/9)*8+2][e.y]=e.volume;

  if (part == 5) {
    editDown(p,e);
    editLeft(p,e);
  } else {
    editRight(p,e);
  }

}
void volumeDel(pattern &p, pattern_editor &e) {
  p.table[(e.x/9)*8 + 2][e.y] = 255;
}


void fxLast(pattern &p, pattern_editor &e) {
  p.table[(e.x/9)*8+3][e.y]=e.fx;
  fx_valueLast(p,e);
}
void fxUp(pattern &p, pattern_editor &e) {
  if (p.table[(e.x/9)*8+3][e.y] > 253){
    if (p.table[(e.x/9)*8+4][e.y] > 253){
      fx_valueLast(p,e);
    }
  } else {
    e.fx = p.table[(e.x/9)*8+3][e.y];
  }
  if (e.fx < 25) e.fx++;
  p.table[(e.x/9)*8+3][e.y]=e.fx;
}
void fxDown(pattern &p, pattern_editor &e) {
  if (p.table[(e.x/9)*8+3][e.y] > 253){
    if (p.table[(e.x/9)*8+4][e.y] > 253){
      fx_valueLast(p,e);
    }
  } else {
    e.fx = p.table[(e.x/9)*8+3][e.y];
  }
  if (e.fx >0) e.fx--;
  p.table[(e.x/9)*8+3][e.y]=e.fx;
}
void fxEdit(pattern &p, pattern_editor &e, int value) {
  if (p.table[(e.x/9)*8+3][e.y] > 253){
    if (p.table[(e.x/9)*8+4][e.y] > 253){
      fx_valueLast(p,e);
    }
  }
  e.fx=value;
  p.table[(e.x/9)*8+3][e.y]=e.fx;
  editDown(p,e);
}
void fxDel(pattern &p, pattern_editor &e) {
  p.table[(e.x/9)*8 + 3][e.y] = 255;
}


void fx_valueLast(pattern &p, pattern_editor &e) {
  p.table[(e.x/9)*8+4][e.y]=e.fx_value;
}
void fx_valueUp(pattern &p, pattern_editor &e, int part) {
  if (p.table[(e.x/9)*8 + 4][e.y] < 254){
    e.fx_value = p.table[(e.x/9)*8+4][e.y];
  }
  if (part == 8) {
    if (e.fx_value < 99) e.fx_value++;
  }
  if (part == 7) {
    if (e.fx_value < 89) {
      e.fx_value+=10;
    } else {
      e.fx_value=64;
    }
  }
  p.table[(e.x/9)*8+4][e.y]=e.fx_value;
}
void fx_valueDown(pattern &p, pattern_editor &e, int part) {
  if (p.table[(e.x/9)*8 + 4][e.y] < 254){
    e.fx_value = p.table[(e.x/9)*8+4][e.y];
  }
  if (part == 8) {
    if (e.fx_value > 0) e.fx_value--;
  }
  if (part == 7) {
    if (e.fx_value > 10) {
      e.fx_value-=10;
    } else {
      e.fx_value=0;
    }
  }
  p.table[(e.x/9)*8+4][e.y]=e.fx_value;
}
void fx_valueEdit(pattern &p, pattern_editor &e, int part, int value) {
  if (p.table[(e.x/9)*8 + 4][e.y] < 254){
    e.fx_value = p.table[(e.x/9)*8+4][e.y];
  } else {
    e.fx_value = 0;
  }
  if (part == 8) {
    e.fx_value=(e.fx_value/10)*10 + value;
  }
  if (part == 7) {
    e.fx_value=e.fx_value%10 + value*10;
  }
  if (e.fx_value > 99) {
    e.fx_value=99;
  }
  p.table[(e.x/9)*8+4][e.y]=e.fx_value;
  if (part == 8) {
    editDown(p,e);
    editLeft(p,e);
  } else {
    editRight(p,e);
  }
}
void fx_valueDel(pattern &p, pattern_editor &e) {
  p.table[(e.x/9)*8 + 4][e.y] = 255;
}
 */