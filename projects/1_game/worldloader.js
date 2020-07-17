// load worlds

const worldconfigs = {
  test: {
    maps : [
      {
	name: "datamap",
	file: "maps/data.txt",
        x: 0,
        y: 0,
	parent: "none",
      },
      {
	name: "mini",
	file: "maps/mini.txt",
        x: -200,
        y: 100,
	parent: "none",
      },
      {
	name: "test",
	file: "maps/test.txt",
        x: -200,
        y: 200,
	parent: "none",
      },
      {
	name: "test2",
	file: "maps/test2.txt",
        x: -200,
        y: 400,
	parent: "none",
      },
    ],
  }
}

async function worldload(name) {
  console.log("loading world: "+name);
  
  // find world config:
  if(!(name in worldconfigs)) {
    console.log("Error: world not found!");
    return {};
  }
  
  var config = worldconfigs[name];
  var world = {
    maps : {},
    rootmap : "none",
  };
  
  for(var i in config.maps) {
    var m = config.maps[i];
    // load this map
    console.log(m);
    var wait = true;
    console.log("go fetch " + m.file);
    var response = await fetch(m.file);
    var txt = await response.text();
    world.maps[m.name] = dataLoadParse(txt);
    world.maps[m.name].x = m.x;
    world.maps[m.name].y = m.y;
    world.maps[m.name].parent = m.parent;
    world.maps[m.name].gl = {
      hasArrays: false,
      needRender: true,
    };
  }
  
  return world;
}


function dataLoadParse(txt) {
  const lines = txt.split("\n");
  //console.log(lines)
  var data = {palette : {}, map : {}, cells : {}, layer : {}, objects: {}};
  var line = 0;
  var state = 0;
  var lastId = -1;
  while(line < lines.length) {
    l = lines[line];
    if(l.length == 0) {line++;continue;}
    switch(l[0]) {
      case "#": {break;}// comment
      case "@": {
        // tag
	if(l === "@palette") {
	  state = 1;
	} else if(l === "@map") {
          state = 2;
	} else if(l === "@cells") {
          state = 3;
	} else if(l === "@datalayer") {
          state = 4;
	} else if(l === "@objects") {
          state = 5;
	} else {
	  console.log("@ not covered",l);
	}
      break;}
      default: {
	switch(state) {
	  case 0: {console.log("before start",l); break;}
	  case 1: {
	    // palette
	    const parts = l.split(",");
	    const id = parseInt(parts[0]);
	    const name = parts[1];
	    const r = parseFloat(parts[2]);
	    const g = parseFloat(parts[3]);
	    const b = parseFloat(parts[4]);
	    const a = parseFloat(parts[5]);
	    data.palette[id] = {id: id, name: name, color: [r,g,b,a]}
          break;}
	  case 2: {
            // map
	    var parts = l.split(",");
	    data.map.nc = parseInt(parts[0]);
	    data.map.dx = parseFloat(parts[1]);
	    data.map.dy = parseFloat(parts[2]);
          break;}
	  case 3: {
            // cells
	    if(l[0] === "*") {// corner
	      const parts = l.substr(1).split(",");
	      const xx = parseFloat(parts[0]);
	      const yy = parseFloat(parts[1]);
	      data.cells[lastId].corners.push([xx,yy]);
	    } else if(l[0] === "-") {// neighbor
	      const parts = l.substr(1).split(",");
	      data.cells[lastId].neighbors = parts.map(i => parseInt(i));
	    } else if(l[0] === "&") {// data layer
	      const parts = l.substr(1).split(",");
	      const id = parseInt(parts[0]);
	      const value = parts[1];
	      data.cells[lastId].layer[id] = value;
	    } else {
	      const parts = l.split(",");
	      lastId = parseInt(parts[0]);
	      const pid = parseInt(parts[1]);
	      const nc = parseInt(parts[2]);
	      const nn = parseInt(parts[3]);
	      const xx = parseFloat(parts[4]);
	      const yy = parseFloat(parts[5]);
	      const cell = {paletteId: pid, x: xx, y: yy, corners: [], neighbors: [], layer : {}};
	      data.cells[lastId] = cell;
	    }
          break;}
	  case 4: {
            // datalayer
	    const parts = l.split(",");
	    const id = parseInt(parts[0]);
	    const name = parts[1];
	    const r = parseFloat(parts[2]);
	    const g = parseFloat(parts[3]);
	    const b = parseFloat(parts[4]);
	    const a = parseFloat(parts[5]);
 	    // ignoring isShow, isEdit, value
	    data.layer[id] = {name: name, color: [r,g,b,a]};
	  break;}
          case 5: {
            // objects
	    if(l[0] === "-") {// dict
       	      const parts = l.substr(1).split(",");
	      const key = parts[0];
	      const val = parts[1];
	      data.objects[lastId].dict[key] = val;
	    } else {
       	      const parts = l.split(",");
	      const id = parseInt(parts[0]);
	      lastId = id;
	      const name = parts[1];
	      const r = parseFloat(parts[2]);
	      const g = parseFloat(parts[3]);
	      const b = parseFloat(parts[4]);
	      const a = parseFloat(parts[5]);
	      const x = parseFloat(parts[6]);
	      const y = parseFloat(parts[7]);
	      data.objects[id] = {id: id, name: name, color: [r,g,b,a], x: x, y: y, dict: {}};
	    }
	  break;}
	}
      break;}
    }
    line++;
  }
  return data;
}



