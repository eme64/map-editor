#include "main_gui.hpp"

std::vector<std::string> split(const std::string &s, const char separator =',') {
  std::vector<std::string> res;
  std::istringstream f(s);
  std::string tmp;
  while (getline(f, tmp, separator)) {
      res.push_back(tmp);
  }
  if(s.size()>0 && s[s.size()-1]==separator) {res.push_back("");}
  return res;
}

void MapArea::load(const std::string& fileName) {
  std::cout << "**LOAD**"<< fileName <<"\n";
  std::ifstream myfile;
  myfile.open(fileName);
  if(!myfile.is_open()) {
    std::cout << "File cound not be opened! - abort load.\n";
    return;
  }
  std::stringstream mystream;
  mystream << myfile.rdbuf();
  myfile.close();
  std::string str = mystream.str();
  
  std::vector<std::string> lines = split(str, '\n');
  int state = 0; // 1: palette, 2: map, 3: cells, 4: datalayer
  int line = 0;
  int lastId = -1;
  while(line < lines.size()) {
    std::string &l = lines[line];
    if(l.size()==0){line++;continue;}
    switch(l[0]) {
      case '#': {
        // comment - ignore
      }
      break;
      case '@': {
        // tag - pick state
	if(l.compare("@palette")==0) {
	  state = 1;
	  paletteArea_->clear();
	} else if(l.compare("@map")==0) {
          paletteArea_->repopulate();
	  state = 2;
	} else if(l.compare("@cells")==0) {
	  state = 3;
          updateLayers();
          dataLayerArea_->repopulate();
	} else if(l.compare("@datalayer")==0) {
	  state = 4;
	  dataLayerArea_->clear();
	  dataLayers_.clear();
	} else {
          std::cout << "WARNING: did not know " << l << "\n";
	  return;
	}
      }
      break;
      default: {
        switch(state) {
	  case 0: {
	    std::cout << "Before start: " << l << "\n";
	  break;}
	  case 1: {
	    // palette
	    std::vector<std::string> parts = split(l,',');
            int id = std::atoi(parts[0].c_str());
	    std::string name = parts[1];
	    float r = std::stof(parts[2]);
	    float g = std::stof(parts[3]);
	    float b = std::stof(parts[4]);
	    float a = std::stof(parts[5]);
	    paletteArea_->addItem(id, name, evp::Color(r,g,b,a));
	  break;}
	  case 2: {
	    // map
	    std::vector<std::string> parts = split(l,',');
            size_t nc = std::atoi(parts[0].c_str());
	    float sx = std::stof(parts[1]);
	    float sy = std::stof(parts[2]);
	    vmap->reconfigure(nc,sx,sy);
	  break;}
	  case 3: {
	    // cells
            if(l[0]=='*') {
	      // push corner:
	      std::vector<std::string> parts = split(l.substr(1),',');
	      float xx = std::stof(parts[0]);
	      float yy = std::stof(parts[1]);
	      auto &c = vmap->cells[lastId];
              c.corners.push_back(evp::Point(xx,yy));
	    } else if(l[0]=='-') {
	      // neighbor list:
	      std::vector<std::string> parts = split(l.substr(1),',');
	      auto &c = vmap->cells[lastId];
	      for(auto &n : parts) {
	        c.neighbors.push_back(std::atoi(n.c_str()));
	      }
	    } else if(l[0]=='&') {
	      std::vector<std::string> parts = split(l.substr(1),',');
              int lid = std::atoi(parts[0].c_str());
	      std::string value = parts[1];
	      auto &c = vmap->cells[lastId];
              dataLayers_[lid][lastId] = value;
	    } else {
	      // init info:
	      std::vector<std::string> parts = split(l,',');
	      lastId = std::atoi(parts[0].c_str());
	      int pid = std::atoi(parts[1].c_str());
	      int nc = std::atoi(parts[2].c_str());
	      int nn = std::atoi(parts[3].c_str());
	      float xx = std::stof(parts[4]);
	      float yy = std::stof(parts[5]);
              
	      auto &c = vmap->cells[lastId];
              c.info.paletteId = pid;
              c.info.colFac = 1 - ((float)rand()/(RAND_MAX))*0.2;
	      c.corners.clear();
	      c.corners.reserve(nc);
	      c.neighbors.clear();
	      c.neighbors.reserve(nn);
	      c.pos.x = xx;
	      c.pos.y = yy;
	    }
	  break;}
	  case 4: {
	    // datalayer
	    std::vector<std::string> parts = split(l,',');
            int id = std::atoi(parts[0].c_str());
	    std::string name = parts[1];
	    float r = std::stof(parts[2]);
	    float g = std::stof(parts[3]);
	    float b = std::stof(parts[4]);
	    float a = std::stof(parts[5]);
            int isShow = std::atoi(parts[6].c_str());
            int isEdit = std::atoi(parts[7].c_str());
            std::string value = parts[8];
	    dataLayerArea_->addItem(id, name, evp::Color(r,g,b,a),isShow,isEdit,value);
	  break;}
	
	  default: {
	    std::cout << "Default state: " << l << "\n";
	  break;}
	}
      }
    }
    line++;
  }
  mapColorize();// update map colors
}


void PaletteArea::save(std::ofstream &myfile) {
  myfile << "@palette\n";
  myfile << "#id,name,r,g,b,a\n";
  for(auto it : pname) {
    myfile << it.first <<","<<it.second<<",";
    evp::Color c = pcolor[it.first];
    myfile << c.r << "," << c.g << "," << c.b << "," << c.a << "\n";
  }
}

void DataLayerArea::save(std::ofstream &myfile) {
  myfile << "@datalayer\n";
  myfile << "#id,name,r,g,b,a,isShow,isEdit,defaultValue\n";
  for(auto it : lname) {
    myfile << it.first <<","<<it.second<<",";
    evp::Color c = lcolor[it.first];
    myfile << c.r << "," << c.g << "," << c.b << "," << c.a << ",";
    myfile << (int)lshow[it.first] << ",";
    myfile << (int)ledit[it.first] << ",";
    myfile << lvalue[it.first] << "\n";
  }
}

void MapArea::save(const std::string& fileName) {
  std::cout << "**SAVE**"<< fileName <<"\n";
  
  std::ofstream myfile;
  myfile.open(fileName);
  paletteArea_->save(myfile);
  // ------------------------- begin map

  myfile << "@map\n";
  
  myfile << "#num_cells,spread_x,spread_y\n";
  myfile << vmap->num_cells << ",";
  myfile << vmap->spread_x << ",";
  myfile << vmap->spread_y << "\n";
  
  dataLayerArea_->save(myfile);
  
  myfile << "@cells\n";
  myfile << "#id,paletteId,n_corners,n_neighbors,posx,posy\n";
  myfile << "# *cornerx,cornery\n";
  myfile << "# -neighborlist\n";
  myfile << "# &datalayerId,value\n";

  for(size_t i = 0; i<vmap->num_cells; i++) {
    auto &c = vmap->cells[i];
    myfile << i << "," << c.info.paletteId << ",";
    myfile << c.corners.size() << "," << c.neighbors.size() << ",";
    myfile << c.pos.x << "," << c.pos.y << "\n";

    for(int k=0;k<c.corners.size();k++) {
      myfile << "*" << c.corners[k].x << "," << c.corners[k].y << "\n";
    }
      
    myfile << "-";
    for(int k=0;k<c.neighbors.size();k++) {
      if(k>0){myfile << ",";}
      myfile << c.neighbors[k];
    }
    myfile << "\n";

    for(auto &it : dataLayers_) {
      if(it.second[i].size() > 0) {
        myfile << "&" << it.first << "," << it.second[i] << "\n";
      }
    }
  }

  // ------------------------- close
  myfile.close();
}

