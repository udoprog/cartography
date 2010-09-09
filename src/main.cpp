#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <limits.h>
#include <errno.h>

#include <string>
#include <vector>
#include <stack>
#include <list>
#include <queue>
#include <iostream>
#include <fstream>

#include <png.h>

#include "resource.h"
#include "global.h"
#include "Level.h"
#include "render.h"
#include "Image.h"

int Rotate;
int daynight;
int cave;
int exclude;
int slide;
int flip;
int water;

using namespace std;

//My changes:
//ListFiles rewrited to posix mode
//gui is destroy [oh god]
//only console
//what the hell is wchar_t??
//by Firemark :)

int CWorld = 0;

int cut = 0;

class dirlist {
private:
  queue<string> directories;
  queue<string> files;

public:
  dirlist(string path) {
    directories.push(path);
  }
  
  bool hasnext() {
    if (!files.empty()) {
      return true;
    }

    if (directories.empty()) {
      return false;
    }
    
    // work until you find any files
    while (files.empty() && !directories.empty()) {
      string path = directories.front();
      directories.pop();
      
      DIR *dir = opendir(path.c_str()); 
      
      if (!dir) {
        return false;
      }
      
      dirent *ent; 
    
      while((ent = readdir(dir)) != NULL)
      {
        string temp_str = ent->d_name;

        if (temp_str.compare(".") == 0) {
          continue;
        }
        
        if (temp_str.compare("..") == 0) {
          continue;
        }
        
        if (ent->d_type == DT_DIR) {
          directories.push(path + "/" + temp_str);
        }
        else if (ent->d_type == DT_REG) {
          files.push(path + "/" + temp_str);
        }
      }
      
      closedir(dir);
    }
    
    return !files.empty();
  }

  string next() {
    string next = files.front();
    files.pop();
    return next;
  }
};

int write_image(settings_t *s, const char *filename, int width, int height, Image *img, const char *title)
{
   int code = 0;
   FILE *fp;
   png_structp png_ptr = NULL;
   png_infop info_ptr = NULL;
   png_bytep row = NULL;

   fp = fopen(filename, "wb");

   if (fp == NULL) {
      code = 1;
      goto finalise;
   }

   png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
   if (png_ptr == NULL) {
      code = 1;
      goto finalise;
   }

   info_ptr = png_create_info_struct(png_ptr);

   if (info_ptr == NULL) {
      code = 1;
      goto finalise;
   }

   if (setjmp(png_jmpbuf(png_ptr))) {
      code = 1;
      goto finalise;
   }

   png_init_io(png_ptr, fp);

   png_set_IHDR(png_ptr, info_ptr, width, height,
         8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
         PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
    
   if (title != NULL) {
      png_text title_text;
      title_text.compression = PNG_TEXT_COMPRESSION_NONE;
      title_text.key = (char *)"Title";
      title_text.text = (char *)title;
      png_set_text(png_ptr, info_ptr, &title_text, 1);
   }

   png_write_info(png_ptr, info_ptr);

   row = (png_bytep) malloc(3 * width * sizeof(png_byte));

   int x, y;
   for (y=0 ; y<height ; y++) {
      for (x=0 ; x<width ; x++) {
        Color c = img->d[y*width + x];
        row[0 + x*3] = c.r;
        row[1 + x*3] = c.g;
        row[2 + x*3] = c.b;
      }

      png_write_row(png_ptr, row);
   }

   png_write_end(png_ptr, NULL);

finalise:
   if (fp != NULL) {
     fclose(fp);
   }
    
   if (info_ptr != NULL) {
     png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
   }

   if (png_ptr != NULL) {
     png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
   }

   if (row != NULL) {
     free(row);
   }

   return code;
}

int save_txt(string txtname, int cc, Level &foo)
{
    ofstream values(txtname.c_str());

    if(!values.is_open()) {
      return 1;
    }

  values << "Map surface is: " << cc * 16 * 16 << " square meters" << endl;
  values << "Block amounts:" << endl;
  values << endl;
  values << "Air: " << foo.count[0] << endl;
  values << "Stone: " << foo.count[1] << endl;
  values << "Grass: " << foo.count[2] << endl;
  values << "Dirt: " << foo.count[3] << endl;
  values << "Snow: " << foo.count[78] << endl;
  values << endl;
  values << "Water: " << foo.count[8] + foo.count[9] << endl;
  values << "Ice: " << foo.count[79] << endl;
  values << "Lava: " << foo.count[10] + foo.count[11] << endl;
  values << endl;
  values << "Obisidian: " << foo.count[49] << endl;
  values << endl;
  values << "Trunk: " << foo.count[17] << endl;
  values << "Leaves: " << foo.count[18] << endl;
  values << "Wood: " << foo.count[5] << endl;
  values << "Cactus: " << foo.count[81] << endl;
  values << endl;
  values << "Sand: " << foo.count[12] << endl;
  values << "Gravel: " << foo.count[13] << endl;
  values << "Clay: " << foo.count[82] << endl;
  values << endl;
  values << "Gold Ore: " << foo.count[14] << endl;
  values << "Iron Ore: " << foo.count[15] << endl;
  values << "Coal Ore: " << foo.count[16] << endl;
  values << "Diamond Ore: " << foo.count[56] << endl;
  values << "Redstone: " << foo.count[73] + foo.count[74] << endl;
  values << endl;
  values << "Cobble: " << foo.count[4] << endl;
  values << "Glass: " << foo.count[20] << endl;
  values << "Cloth: " << foo.count[35] << endl;
  values << "Gold: " << foo.count[41] << endl;
  values << "Iron: " << foo.count[42] << endl;
  values << "Diamond: " << foo.count[57] << endl;
  values << endl;
  values << "Farmland: " << foo.count[60] << endl;
  values << "Crops: " << foo.count[59] << endl;
  values << "Reed: " << foo.count[83] << endl;
  values << "Torch: " << foo.count[50] << endl;
  values << "CraftTable: " << foo.count[58] << endl;
  values << "Chest: " << foo.count[54] << endl;
  values << "Furnace: " << foo.count[61] + foo.count[62] << endl;
  values << "Wooden Doors: " << foo.count[64] / 2 << endl;
  values << "Iron Doors: " << foo.count[71] << endl;
  values << "Signs: " << foo.count[63] + foo.count[68] << endl;
  values << "Ladder: " << foo.count[65] << endl;
  values << "Railtracks: " << foo.count[66] << endl;
  values << "Wooden Stairs: " << foo.count[53] << endl;
  values << "Rock Stairs: " << foo.count[67] << endl;
  values << "Lever: " << foo.count[69] << endl;
  values << "Buttons: " << foo.count[77] << endl;
  values << "Pressure Plates: " << foo.count[70] + foo.count[72] << endl;
  values << "Redstone Powder: " << foo.count[55] << endl;
  values << "Redstone Torches: " << foo.count[75] + foo.count[76] << endl;
  values << endl;
    values.close();
    return 0;
}

Color *setup_colors() {
  Color *colors = new Color[256];
  
  colors[0] = Color(255,255,255,0);
  colors[1] = Color(120,120,120,255);
  colors[2] = Color(117,176,73,255);
  colors[3] = Color(134,96,67,255);
  colors[4] = Color(115,115,115,255);
  colors[48] = Color(115,115,115,255);
  colors[5] = Color(157,128,79,255);
  colors[6] = Color(120,120,120,0);
  colors[7] = Color(84,84,84,255);
  colors[8] = Color(38,92,255,51);
  colors[9] = Color(38,92,255,51);
  colors[10] = Color(255,90,0,255);
  colors[11] = Color(255,90,0,255);
  colors[12] = Color(218,210,158,255);
  colors[13] = Color(136,126,126,255);
  colors[14] = Color(143,140,125,255);
  colors[15] = Color(136,130,127,255);
  colors[16] = Color(115,115,115,255);
  colors[17] = Color(102,81,51,255);
  colors[18] = Color(60,192,41,100);
  colors[20] = Color(255,255,255,64); //glass
  //colors[21] = Color(222,50,50,255);
  //colors[22] = Color(222,136,50,255);
  //colors[23] = Color(222,222,50,255);
  //colors[24] = Color(136,222,50,255);
  //colors[25] = Color(50,222,50,255);
  //colors[26] = Color(50,222,136,255);
  //colors[27] = Color(50,222,222,255);
  //colors[28] = Color(104,163,222,255);
  //colors[29] = Color(120,120,222,255);
  //colors[30] = Color(136,50,222,255);
  //colors[31] = Color(174,74,222,255);
  //colors[32] = Color(222,50,222,255);
  //colors[33] = Color(222,50,136,255);
  //colors[34] = Color(77,77,77,255);
  colors[35] = Color(222,222,222,255); //Color(143,143,143,255); 
  //colors[36] = Color(222,222,222,255);
  colors[37] = Color(255,0,0,255);
  colors[38] = Color(255,255,0,0);
  colors[41] = Color(232,245,46,255);
  colors[42] = Color(191,191,191,255);
  colors[43] = Color(200,200,200,255);
  colors[44] = Color(200,200,200,255);
  colors[45] = Color(170,86,62,255);
  colors[46] = Color(160,83,65,255);
  colors[49] = Color(26,11,43,255);
  colors[50] = Color(245,220,50,200);
  colors[51] = Color(255,170,30,200);
  //colors[52] = Color(245,220,50,255); unnecessary afaik
  colors[53] = Color(157,128,79,255);
  colors[54] = Color(125,91,38,255);
  //colors[55] = Color(245,220,50,255); unnecessary afaik
  colors[56] = Color(129,140,143,255);
  colors[57] = Color(45,166,152,255);
  colors[58] = Color(114,88,56,255);
  colors[59] = Color(146,192,0,255);
  colors[60] = Color(95,58,30,255);
  colors[61] = Color(96,96,96,255);
  colors[62] = Color(96,96,96,255);
  colors[63] = Color(111,91,54,255);
  colors[64] = Color(136,109,67,255);
  colors[65] = Color(181,140,64,32);
  colors[66] = Color(150,134,102,180);
  colors[67] = Color(115,115,115,255);
  colors[71] = Color(191,191,191,255);
  colors[73] = Color(131,107,107,255);
  colors[74] = Color(131,107,107,255);
  colors[75] = Color(181,140,64,32);
  colors[76] = Color(255,0,0,200);
  colors[78] = Color(255,255,255,255);
  colors[79] = Color(83,113,163,51);
  colors[80] = Color(250,250,250,255);
  colors[81] = Color(25,120,25,255);
  colors[82] = Color(151,157,169,255);
  colors[83] = Color(193,234,150,255);

  return colors;
}

void do_work(settings_t *s, string path, string out) {
  string txtname = out + ".txt";
  string pngname = out + ".png";
  
  cout << "world: " << path << " " << endl;
  cout << "png: " << pngname << " " << endl;
  cout << "txt: " << txtname << " " << endl;
  
    vector<string> files;
  list<render> renderblocks;

  int cc = 0;

  Color *colors = setup_colors();
  
  Level foo(colors);
  
  cout << "Unpacking and drawing... " << flush;

  dirlist listing(path);
  
  while (listing.hasnext()) {
    string p = listing.next();

    cout << p << endl;
    
    const render *temp = foo.LoadLevelFromFile(s, p.c_str(), s->slide, s->water, cut);
    
    if(temp->isgood){
      cc++;
      renderblocks.push_back(*temp);
    }
  }

  delete [] colors;
  
  cout << "done!" << endl;
  
  int minx = INT_MAX;
  int miny = INT_MAX;
  int maxx = INT_MIN;
  int maxy = INT_MIN;

  for (list<render>::iterator block=renderblocks.begin(); block!=renderblocks.end(); block++){
    if(abs(block->x) < 65536 && abs(block->y) < 65536) {
      if(block->x < minx) {
        minx = block->x;
      }
      
      if(block->y < miny) {
        miny = block->y;
      }
    
      if(block->x > maxx) {
        maxx = block->x;
      }

      if(block->y > maxy) {
        maxy = block->y;
      }
    }
  }
  
  int imageheight, imagewidth;

  Image *image;

  if(slide == 70) {
    int cxx = ((((maxx-minx))+(maxy-miny)))*0.5;
    int cyy = (((maxx-minx))+(maxy-miny))*0.5;

    imageheight = (cxx)*32 + 288;
    imagewidth = (cyy)*32 + 288;
  }
  else
  {
    imageheight = (maxx-minx)*16 + 144;
    imagewidth = (maxy-miny)*16 + 144;
  }

  if(slide == 70) {
    image = new Image(imageheight, imagewidth+128);
  }
  else if(slide == 69) {
    image = new Image(imageheight, imagewidth);
  }
  else {
    image = new Image(imageheight, imagewidth);
  }

  renderblocks.sort();
  renderblocks.reverse();
  
  for (list<render>::iterator rit=renderblocks.begin(); rit!=renderblocks.end(); rit++) {
    int xb = rit->x;
    int yb = rit->y;

    int xx = (xb-minx)*16;
    int yy = (yb-miny)*16;

    if(s->slide == 70){
      if(s->flip == 1) {
        yy = abs((yb-miny)-(maxy-miny))*16;
      }

      if(Rotate == 0) {
        xx = abs((xb-minx)-(maxx-minx))*16;
      }

      int temx = xx;
      int temy = yy;
      xx = imageheight/2+(temx)-(temy);
      yy = (temx)+(temy)+128;
    }

    if(slide == 69) {
      if(s->flip == 1){
        if(s->Rotate == 0){
          yy = abs((yb-miny)-(maxy-miny))*16;
        } else {
          xx = (yb-miny)*16;
          yy = abs((xb-minx)-(maxx-minx))*16;
        }
      } else {
        if(s->Rotate == 1) {
          xx = (yb-miny)*16;
          yy = (xb-minx)*16;
        }
      }
    }

    if(xx >= 0 && yy >= 0 && xx < 40000 && yy < 40000){
      if(slide == 70){
        for(int xo = 0;xo < 33; xo++){
          for(int yo = 0;yo < 160;yo++){
            int x = xo;
            int y = yo;
            image->SetPixel(x+xx,y+yy-128, Blend(image->GetPixel(x+xx,y+yy-128), rit->Q->GetPixel(x,y), 128));
          }
        }
      } else {
        for(int xo = 0;xo < 16;xo++){
          for(int yo = 0;yo < 16+(slide > 68)*128;yo++){
            int x = xo;
            int y = yo;

            if(slide == 69) {
              image->SetPixel(x+xx,y+yy, Blend(image->GetPixel(x+xx,y+yy), rit->Q->GetPixel(x,y), 128));
            } else if(slide == 70) {
              image->SetPixel(x+xx,y+yy-128, Blend(image->GetPixel(x+xx,y+yy-128), rit->Q->GetPixel(x,y), 128));
            } else {
              image->SetPixel(x+xx, y+yy, rit->Q->GetPixel(x,y));
            }
          }
        }
      }
    }
  }

  cout << "Saving image " << pngname << "... " << flush;
  
  if (write_image(s, pngname.c_str(), imagewidth, imageheight, image, "Title stuff") != 0) {
    cout << "failed! " << strerror(errno) << endl;
    exit(1);
  }

  cout << "done!" << endl;

  delete image;
  renderblocks.clear();

  cout << "Saving txt " << txtname << "... " << flush;

  if (save_txt(txtname, cc, foo) != 0) {
    cout << "failed!" << endl;
      exit(1);
    }

    cout << "done!" << endl;
}

void do_help() {
  cout << "Usage: cart5 <world-directory> <output> [options]" << endl;
  cout << "Valid options:" << endl
    << "W - water; C - cave mode" << endl
    << "R - rotate; F - flip  " << endl
    << "D - day; d -day/night; N - night " << endl
    << "Hc - heightcolor; Hg - heightgray; Or - Ore " << endl
    << "Ob - oblique; Oa - oblique angle" << endl
    << "E - exclude " << endl << ":" << flush;
}

int main(int argc, char *argv[]){
  string flag = "";

  if (argc < 3) {
    do_help();
    exit(1);
  }

  string path(argv[1]);
  string out(argv[2]);
  
  cout << "Cartography rewritten for linux by Firemark [pozdrawiam halp]" << std::endl;

  settings_t *settings = new settings_t();
  settings->slide = -1;
  settings->water = 0;
  settings->cave = 0;
  settings->Rotate = 0;
  settings->flip = 0;
  settings->daynight = 0;

  for (int i = 3; i < argc; i++) {
    string opt(argv[i]);
    
    if (opt.compare("W") == 0) {
      settings->water = 1;
    }
  
    else if ( opt.compare("C") == 0) {
      settings->cave = 1;
    }
    else if ( opt.compare("R") == 0) {
      settings->Rotate = 1;
    }
    else if ( opt.compare("F") == 0) {
      settings->flip = 1;
    }
    
    else if ( opt.compare("D") == 0)
      settings->daynight = 0;
    else if ( opt.compare("d") == 0)
      settings->daynight = 1;
    else if ( opt.find("n") == 0)
      settings->daynight = 2;
    else if ( opt.compare("Hc") == 0)
      settings->slide = -3;
    else if ( opt.compare("Hg") == 0)
      settings->slide = -4;
    else if ( opt.compare("Or") == 0)
      settings->slide = -5;
    else if ( opt.compare("Ob") == 0)
      //--yeah funny
      // -- i lolled aswell
      settings->slide = 69;
    else if ( opt.compare("Oa") == 0)
      settings->slide = 70;
    else if ( opt.compare("E") == 0) {
      cout << "Write number to exclude [0-128]" << endl << ":" << flush;
      cin >> exclude;
    }
    else {
      cerr << "Unkown option: " << opt << endl;
      exit(1);
    }
  }
  
  slide =   settings->slide;
  water =   settings->water;
  cave =     settings->cave;
  Rotate =   settings->Rotate;
  flip =     settings->flip;
  daynight =   settings->daynight;

  do_work(settings, path, out);
  
  return 0;
};
