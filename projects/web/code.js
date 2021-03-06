var squareRotation = 0.0;

var data = {};

var controlX = 0.5;
var controlY = 0.5;

dataLoad();
//main();

//
// Start here
//
function main() {
  //dataLoad();
  
  // set up canvas
  //
  const canvas = document.querySelector('#glcanvas');
  const gl = canvas.getContext('webgl');
  
  // add event listeners for mouse/touch event:
  canvas.addEventListener("touchmove", function(event) {
     var touches = event.changedTouches;
     for(var i=0; i<touches.length; i++) {
        var id = touches[i].identifier;
        controlMove(id, touches[i].pageX/canvas.width,touches[i].pageY/canvas.height);
     }
  });
  canvas.addEventListener("mousemove", function(event) {
     controlMove(-1, event.pageX/canvas.width,event.pageY/canvas.height);
  });
  
  // If we don't have a GL context, give up now

  if (!gl) {
    alert('Unable to initialize WebGL. Your browser or machine may not support it.');
    return;
  }

  // Vertex shader program

  const vsSource = `
    attribute vec4 aVertexPosition;
    attribute vec4 aVertexColor;
    attribute float aSparkle;

    uniform mat4 uModelViewMatrix;
    uniform mat4 uProjectionMatrix;

    varying lowp vec4 vColor;
    varying lowp vec4 vPos;
    varying highp float vSparkle;
    varying highp float vSparkleX;

    void main(void) {
      gl_Position = uProjectionMatrix * uModelViewMatrix * aVertexPosition;
      vColor = aVertexColor;
      vPos = gl_Position;
      vSparkleX = step(2.0,aSparkle);
      vSparkle = aSparkle*1.0*(1.0-vSparkleX);
    }
  `;

  // Fragment shader program

  const fsSource = `
    varying lowp vec4 vColor;
    varying lowp vec4 vPos;
    varying lowp float vSparkle;
    uniform highp float iOffset;
    varying highp float vSparkleX;
    uniform highp vec2 uControl;
   
    void main(void) {
      highp float dx = uControl.x-vPos.x-0.5;
      highp float dy = -uControl.y*2.0-vPos.y+0.5;
      highp float e = max(-0.05 / (dx*dx + dy*dy),-0.5);
      highp float f = mod(e + vPos.x+vPos.y+0.2*iOffset,1.0)*0.3;
      highp float g = mod(2.0*vPos.x-vPos.y+2.0*iOffset,1.0)*0.05;
      gl_FragColor = mix(
	      //mix(
	      //  vColor,
	        vec4(
	        mod(2.0*f+vColor[0],1.0),
	        mod(3.0*f+vColor[1],1.0),
	        mod(5.0*f+vColor[2],1.0),
	          1),
	      //  vSparkle
	      //),
	      vec4(
	      mod(13.0*g+vColor[0],1.0),
	      mod(17.0*g+vColor[1],1.0),
	      mod(23.0*g+vColor[2],1.0),
	      1),
	      vSparkleX);
    }
  `;

  // Initialize a shader program; this is where all the lighting
  // for the vertices and so forth is established.
  const shaderProgram = initShaderProgram(gl, vsSource, fsSource);

  // Collect all the info needed to use the shader program.
  // Look up which attributes our shader program is using
  // for aVertexPosition, aVevrtexColor and also
  // look up uniform locations.
  const programInfo = {
    program: shaderProgram,
    attribLocations: {
      vertexPosition: gl.getAttribLocation(shaderProgram, 'aVertexPosition'),
      vertexColor: gl.getAttribLocation(shaderProgram, 'aVertexColor'),
      sparkle: gl.getAttribLocation(shaderProgram, 'aSparkle'),
    },
    uniformLocations: {
      projectionMatrix: gl.getUniformLocation(shaderProgram, 'uProjectionMatrix'),
      modelViewMatrix: gl.getUniformLocation(shaderProgram, 'uModelViewMatrix'),
      iOffset: gl.getUniformLocation(shaderProgram, 'iOffset'),
      control: gl.getUniformLocation(shaderProgram, 'uControl'),
    },
  };

  // Here's where we call the routine that builds all the
  // objects we'll be drawing.
  const buffers = dataCellBuffers(gl);//initBuffers(gl);

  var then = 0;

  // Draw the scene repeatedly
  function render(now) {
    now *= 0.001;  // convert to seconds
    const deltaTime = now - then;
    then = now;

    drawScene(canvas,gl, programInfo, buffers, deltaTime);

    requestAnimationFrame(render);
  }
  requestAnimationFrame(render);
}

function controlMove(id, x, y) {
  //console.log(id,x,y);
  controlX = x;
  controlY = y;
}

function dataLoad() {
  fetch("data.txt")
  .then(response => response.text())
  .then((res) => {
    dataLoadParse(res);
    main();
  })
}

function dataLoadParse(txt) {
  const lines = txt.split("\n");
  console.log(lines)
  data = {palette : {}, map : {}, cells : {}, layer : {}, objects: {}};
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
  console.log(data);
}

function dataCellBuffers(gl) {
  var positions = [];
  var colors = [];
  var sparkle = [];
  var lid = -1;
  for(var i in data.layer) {
    const name = data.layer[i].name;
    if(name === "sparkle") {lid = i;}
    console.log(name,i);
  }
  for(var cid in data.cells) {
    const cell = data.cells[cid];
    const col = data.palette[cell.paletteId].color;
    const r0 = 1-Math.random()*0.2;
    var spark = parseFloat(cell.layer[lid]);
    if(!spark) {spark = 0.0;}
    for(var c=2;c<cell.corners.length;c++) {
      positions.push(cell.corners[0][0]);
      positions.push(cell.corners[0][1]);
      positions.push(cell.corners[c-1][0]);
      positions.push(cell.corners[c-1][1]);
      positions.push(cell.corners[c][0]);
      positions.push(cell.corners[c][1]);
      colors.push(r0*col[0],r0*col[1],r0*col[2],col[3]);
      colors.push(r0*col[0],r0*col[1],r0*col[2],col[3]);
      colors.push(r0*col[0],r0*col[1],r0*col[2],col[3]);
      sparkle.push(spark);
      sparkle.push(spark);
      sparkle.push(spark);
    }
  }
  console.log(sparkle)

  const positionBuffer = gl.createBuffer();
  gl.bindBuffer(gl.ARRAY_BUFFER, positionBuffer);
  gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(positions), gl.STATIC_DRAW);

  const colorBuffer = gl.createBuffer();
  gl.bindBuffer(gl.ARRAY_BUFFER, colorBuffer);
  gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(colors), gl.STATIC_DRAW);

  const sparkleBuffer = gl.createBuffer();
  gl.bindBuffer(gl.ARRAY_BUFFER, sparkleBuffer);
  gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(sparkle), gl.STATIC_DRAW);


  return {
    position: positionBuffer,
    color: colorBuffer,
    sparkle: sparkleBuffer,
    num: positions.length/2,
  };
}

//
// initBuffers
//
// Initialize the buffers we'll need. For this demo, we just
// have one object -- a simple two-dimensional square.
//
function initBuffers(gl) {

  // Create a buffer for the square's positions.

  const positionBuffer = gl.createBuffer();

  // Select the positionBuffer as the one to apply buffer
  // operations to from here out.

  gl.bindBuffer(gl.ARRAY_BUFFER, positionBuffer);

  // Now create an array of positions for the square.

  const positions = [
     1.0,  1.0,
    -1.0,  1.0,
     1.0, -1.0,
     1.0, -1.0,
    -1.0,  1.0,
    -1.0, -1.0,
  ];

  // Now pass the list of positions into WebGL to build the
  // shape. We do this by creating a Float32Array from the
  // JavaScript array, then use it to fill the current buffer.

  gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(positions), gl.STATIC_DRAW);

  // Now set up the colors for the vertices

  const colors = [
    1.0,  1.0,  1.0,  1.0,    // white
    1.0,  0.0,  0.0,  1.0,    // red
    0.0,  1.0,  0.0,  1.0,    // green
    0.0,  1.0,  0.0,  1.0,    // green
    1.0,  0.0,  0.0,  1.0,    // red
    0.0,  0.0,  1.0,  1.0,    // blue
  ];

  const colorBuffer = gl.createBuffer();
  gl.bindBuffer(gl.ARRAY_BUFFER, colorBuffer);
  gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(colors), gl.STATIC_DRAW);

  return {
    position: positionBuffer,
    color: colorBuffer,
    num: 6,
  };
}

function resizeCanvas(canvas,gl) {
  var width = canvas.clientWidth;
  var height = canvas.clientHeight;
  if (canvas.width != width ||
      canvas.height != height) {
    canvas.width = width;
    canvas.height = height;
    gl.viewport(0, 0, gl.canvas.width, gl.canvas.height);
  }
}

//
// Draw the scene.
//
function drawScene(canvas, gl, programInfo, buffers, deltaTime) {
  resizeCanvas(canvas,gl);

  gl.clearColor(0.0, 0.0, 0.0, 1.0);  // Clear to black, fully opaque
  gl.clearDepth(1.0);                 // Clear everything
  gl.enable(gl.DEPTH_TEST);           // Enable depth testing
  gl.depthFunc(gl.LEQUAL);            // Near things obscure far things

  // Clear the canvas before we start drawing on it.

  gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

  ///  // Create a perspective matrix, a special matrix that is
  ///  // used to simulate the distortion of perspective in a camera.
  ///  // Our field of view is 45 degrees, with a width/height
  ///  // ratio that matches the display size of the canvas
  ///  // and we only want to see objects between 0.1 units
  ///  // and 100 units away from the camera.

  ///  const fieldOfView = 45 * Math.PI / 180;   // in radians
  ///  const aspect = gl.canvas.clientWidth / gl.canvas.clientHeight;
  ///  const zNear = 0.1;
  ///  const zFar = 100.0;
  ///  const projectionMatrix = mat4.create();

  ///  // note: glmatrix.js always has the first argument
  ///  // as the destination to receive the result.
  ///  mat4.perspective(projectionMatrix,
  ///                   fieldOfView,
  ///                   aspect,
  ///                   zNear,
  ///                   zFar);
  const aspect = gl.canvas.clientWidth / gl.canvas.clientHeight;
  const rzoom = 1000;
  const xf = aspect*rzoom;
  const xf0 = 0.5*(1-aspect)*rzoom;
  const xf1 = xf0+xf;
  const yf = rzoom;
  const projectionMatrix = mat4.create();
  mat4.ortho(projectionMatrix,
             xf0, xf1, yf, 0, 0.1, 100);

  // Set the drawing position to the "identity" point, which is
  // the center of the scene.
  const modelViewMatrix = mat4.create();

  // Now move the drawing position a bit to where we want to
  // start drawing the square.

  mat4.translate(modelViewMatrix,     // destination matrix
                 modelViewMatrix,     // matrix to translate
                 [-0.0, 0.0, -6.0]);  // amount to translate
  //mat4.rotate(modelViewMatrix,  // destination matrix
  //            modelViewMatrix,  // matrix to rotate
  //            squareRotation,   // amount to rotate in radians
  //            [0, 0, 1]);       // axis to rotate around

  // Tell WebGL how to pull out the positions from the position
  // buffer into the vertexPosition attribute
  {
    const numComponents = 2;
    const type = gl.FLOAT;
    const normalize = false;
    const stride = 0;
    const offset = 0;
    gl.bindBuffer(gl.ARRAY_BUFFER, buffers.position);
    gl.vertexAttribPointer(
        programInfo.attribLocations.vertexPosition,
        numComponents,
        type,
        normalize,
        stride,
        offset);
    gl.enableVertexAttribArray(
        programInfo.attribLocations.vertexPosition);
  }

  // Tell WebGL how to pull out the colors from the color buffer
  // into the vertexColor attribute.
  {
    const numComponents = 4;
    const type = gl.FLOAT;
    const normalize = false;
    const stride = 0;
    const offset = 0;
    gl.bindBuffer(gl.ARRAY_BUFFER, buffers.color);
    gl.vertexAttribPointer(
        programInfo.attribLocations.vertexColor,
        numComponents,
        type,
        normalize,
        stride,
        offset);
    gl.enableVertexAttribArray(
        programInfo.attribLocations.vertexColor);
  }
  
  {
    const numComponents = 1;
    const type = gl.FLOAT;
    const normalize = false;
    const stride = 0;
    const offset = 0;
    gl.bindBuffer(gl.ARRAY_BUFFER, buffers.sparkle);
    gl.vertexAttribPointer(
        programInfo.attribLocations.sparkle,
        numComponents,
        type,
        normalize,
        stride,
        offset);
    gl.enableVertexAttribArray(
        programInfo.attribLocations.sparkle);
  }

  // Tell WebGL to use our program when drawing

  gl.useProgram(programInfo.program);

  // Set the shader uniforms

  gl.uniformMatrix4fv(
      programInfo.uniformLocations.projectionMatrix,
      false,
      projectionMatrix);
  gl.uniformMatrix4fv(
      programInfo.uniformLocations.modelViewMatrix,
      false,
      modelViewMatrix);
  gl.uniform1f(
      programInfo.uniformLocations.iOffset,
      squareRotation);
  let ctrl = vec2.create();
  ctrl[0] = controlX;
  ctrl[1] = controlY;
  gl.uniform2fv(
      programInfo.uniformLocations.control,
      ctrl);
 
  {
    const offset = 0;
    const vertexCount = buffers.num;
    gl.drawArrays(gl.TRIANGLES, offset, vertexCount);
  }

  // Update the rotation for the next draw

  squareRotation += deltaTime;
}

//
// Initialize a shader program, so WebGL knows how to draw our data
//
function initShaderProgram(gl, vsSource, fsSource) {
  const vertexShader = loadShader(gl, gl.VERTEX_SHADER, vsSource);
  const fragmentShader = loadShader(gl, gl.FRAGMENT_SHADER, fsSource);

  // Create the shader program

  const shaderProgram = gl.createProgram();
  gl.attachShader(shaderProgram, vertexShader);
  gl.attachShader(shaderProgram, fragmentShader);
  gl.linkProgram(shaderProgram);

  // If creating the shader program failed, alert

  if (!gl.getProgramParameter(shaderProgram, gl.LINK_STATUS)) {
    alert('Unable to initialize the shader program: ' + gl.getProgramInfoLog(shaderProgram));
    return null;
  }

  return shaderProgram;
}

//
// creates a shader of the given type, uploads the source and
// compiles it.
//
function loadShader(gl, type, source) {
  const shader = gl.createShader(type);

  // Send the source to the shader object

  gl.shaderSource(shader, source);

  // Compile the shader program

  gl.compileShader(shader);

  // See if it compiled successfully

  if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
    alert('An error occurred compiling the shaders: ' + gl.getShaderInfoLog(shader));
    gl.deleteShader(shader);
    return null;
  }

  return shader;
}

