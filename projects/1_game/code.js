// ideas:
// - allow multiple maps -> objects relative / absolute position
// - camera: zoom range, rotaton/pos relative to last clicked object?
// - place objects: gui

// Have some mapping from data -> gl/game data


// World definition:
// list of maps and objects
// tree structure throuth parent relation
var world;
worldload("test").then(
  w => {
    world = w;
    console.log(world);
    main();
  }
);

// ------------------------------------ CODE BELOW:

var squareRotation = 0.0;

var data = {};

var controlX = 0.5;
var controlY = 0.5;

//
// Start here
//
function main() {
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

  // enalbe AlphaBlend
  gl.enable(gl.BLEND);
  gl.blendFunc(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA);
  
  // Vertex shader program

  const vsSource = `
    attribute vec4 aVertexPosition;
    attribute vec4 aVertexColor;

    uniform mat4 uModelViewMatrix;
    uniform mat4 uProjectionMatrix;

    varying lowp vec4 vColor;

    void main(void) {
      gl_Position = uProjectionMatrix * uModelViewMatrix * aVertexPosition;
      vColor = aVertexColor;
    }
  `;

  // Fragment shader program

  const fsSource = `
    varying lowp vec4 vColor;
   
    void main(void) {
      gl_FragColor = vColor;
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
  worldRender(gl);
  //const buffers = dataCellBuffers(gl);

  var then = 0;

  // Draw the scene repeatedly
  function render(now) {
    now *= 0.001;  // convert to seconds
    const deltaTime = now - then;
    then = now;

    //drawScene(canvas,gl, programInfo, buffers, deltaTime);
    drawWorld(canvas,gl, programInfo, deltaTime);

    requestAnimationFrame(render);
  }
  requestAnimationFrame(render);
}

function controlMove(id, x, y) {
  //console.log(id,x,y);
  controlX = x;
  controlY = y;
}

function worldRender(gl) {
  console.log("render world...");
  for(var n in world.maps) {
    console.log(n);
    var m = world.maps[n];
    
    if(m.gl.needRender) {
      m.gl.needRender = false;
      
      if(m.gl.hasArray) {
        // free old arrays.
	console.log("free old arrays")
        // TODO
      }
      // create new arrays.
      console.log("create new arrays")
      m.gl.buffers = mapBuffers(gl, m);
      m.gl.hasArray = true;
    }
  }
  
  console.log("render world done.");
}

function mapBuffers(gl, m) {
  var positions = [];
  var colors = [];
  //var lid = -1;
  //for(var i in data.layer) {
  //  const name = data.layer[i].name;
  //  if(name === "sparkle") {lid = i;}
  //  console.log(name,i);
  //}
  for(var cid in m.cells) {
    const cell = m.cells[cid];
    const col = m.palette[cell.paletteId].color;
    const r0 = 1-Math.random()*0.2;
    //var spark = parseFloat(cell.layer[lid]);
    //if(!spark) {spark = 0.0;}
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
      //sparkle.push(spark);
      //sparkle.push(spark);
      //sparkle.push(spark);
    }
  }
  //console.log(sparkle)

  const positionBuffer = gl.createBuffer();
  gl.bindBuffer(gl.ARRAY_BUFFER, positionBuffer);
  gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(positions), gl.STATIC_DRAW);

  const colorBuffer = gl.createBuffer();
  gl.bindBuffer(gl.ARRAY_BUFFER, colorBuffer);
  gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(colors), gl.STATIC_DRAW);

  //const sparkleBuffer = gl.createBuffer();
  //gl.bindBuffer(gl.ARRAY_BUFFER, sparkleBuffer);
  //gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(sparkle), gl.STATIC_DRAW);


  return {
    position: positionBuffer,
    color: colorBuffer,
    //sparkle: sparkleBuffer,
    num: positions.length/2,
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
// Draw the world.
//
function drawWorld(canvas, gl, programInfo, deltaTime) {
  resizeCanvas(canvas,gl);

  gl.clearColor(0.0, 0.0, 0.0, 1.0);  // Clear to black, fully opaque
  gl.clearDepth(1.0);                 // Clear everything
  gl.enable(gl.DEPTH_TEST);           // Enable depth testing
  gl.depthFunc(gl.LEQUAL);            // Near things obscure far things

  // Clear the canvas before we start drawing on it.
  gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

  // create projection matrix
  const aspect = gl.canvas.clientWidth / gl.canvas.clientHeight;
  const rzoom = 1000;
  const xf = aspect*rzoom;
  const xf0 = 0.5*(1-aspect)*rzoom;
  const xf1 = xf0+xf;
  const yf = rzoom;
  const projectionMatrix = mat4.create();
  mat4.ortho(projectionMatrix,
             xf0, xf1, yf, 0, 0.1, 100);
  
  // process each map now:
  for(var n in world.maps) {
    var m = world.maps[n];
    
    const modelViewMatrix = mat4.create();
    mat4.translate(modelViewMatrix,     // destination matrix
                 modelViewMatrix,     // matrix to translate
                 [m.x,m.y, -6]);  // amount to translate
  
    // Tell WebGL how to pull out the positions from the position
    // buffer into the vertexPosition attribute
    {
      const numComponents = 2;
      const type = gl.FLOAT;
      const normalize = false;
      const stride = 0;
      const offset = 0;
      gl.bindBuffer(gl.ARRAY_BUFFER, m.gl.buffers.position);
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
      gl.bindBuffer(gl.ARRAY_BUFFER, m.gl.buffers.color);
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

    gl.useProgram(programInfo.program);
    // Tell WebGL to use our program when drawing

    // Set the shader uniforms
    gl.uniformMatrix4fv(
        programInfo.uniformLocations.projectionMatrix,
        false,
        projectionMatrix);
    gl.uniformMatrix4fv(
        programInfo.uniformLocations.modelViewMatrix,
        false,
        modelViewMatrix);
    //gl.uniform1f(
    //    programInfo.uniformLocations.iOffset,
    //    squareRotation);
    //let ctrl = vec2.create();
    //ctrl[0] = controlX;
    //ctrl[1] = controlY;
    //gl.uniform2fv(
    //    programInfo.uniformLocations.control,
    //    ctrl);
 
    {
      const offset = 0;
      const vertexCount = m.gl.buffers.num;
      gl.drawArrays(gl.TRIANGLES, offset, vertexCount);
    }

    // Draw objects of this map:
    for(var i in m.objects) {
      var obj = m.objects[i];
      //console.log("draw object " + obj.name);
      //console.log(obj);
      drawDot(gl,projectionMatrix,obj.x+m.x,obj.y+m.y);
    }
  }

  // Update the rotation for the next draw
  squareRotation += deltaTime;
}

var drawDotBuffer;
var drawDotProgramInfo;

// Draw dot for some object. Probably replace this later on.
function drawDot(gl,projectionMatrix,x,y) {
  if(!drawDotBuffer) {
    console.log("create drawDotBuffer");
    var positions = [
	    -10,-10,
	    10,-10,
	    -10,10,
	    10,-10,
	    10,10,
	    -10,10,
    ];
    drawDotBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, drawDotBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(positions), gl.STATIC_DRAW);
  
    // Vertex shader program

    const vsSource = `
      attribute vec4 aVertexPosition;

      uniform mat4 uModelViewMatrix;
      uniform mat4 uProjectionMatrix;

      varying lowp vec4 vPos;
      
      void main(void) {
        gl_Position = uProjectionMatrix * uModelViewMatrix * aVertexPosition;
	vPos = aVertexPosition;
      }
    `;

    // Fragment shader program

    const fsSource = `
      varying lowp vec4 vPos;
     
      void main(void) {
        lowp float dd = vPos.x*vPos.x + vPos.y*vPos.y;
	lowp float res = step(dd,100.0);
	gl_FragColor = mix(vec4(0,0,0,0),vec4(1,1,1,1),res);
      }
    `;

    // Initialize a shader program; this is where all the lighting
    // for the vertices and so forth is established.
    const shaderProgram = initShaderProgram(gl, vsSource, fsSource);

    // Collect all the info needed to use the shader program.
    // Look up which attributes our shader program is using
    // for aVertexPosition, aVevrtexColor and also
    // look up uniform locations.
    drawDotProgramInfo = {
      program: shaderProgram,
      attribLocations: {
        vertexPosition: gl.getAttribLocation(shaderProgram, 'aVertexPosition'),
      },
      uniformLocations: {
        projectionMatrix: gl.getUniformLocation(shaderProgram, 'uProjectionMatrix'),
        modelViewMatrix: gl.getUniformLocation(shaderProgram, 'uModelViewMatrix'),
      },
    };
  }
  
  const modelViewMatrix = mat4.create();
  mat4.translate(modelViewMatrix,     // destination matrix
               modelViewMatrix,     // matrix to translate
               [x,y, -2]);  // amount to translate
  
  // Tell WebGL how to pull out the positions from the position
  // buffer into the vertexPosition attribute
  {
    const numComponents = 2;
    const type = gl.FLOAT;
    const normalize = false;
    const stride = 0;
    const offset = 0;
    gl.bindBuffer(gl.ARRAY_BUFFER, drawDotBuffer);
    gl.vertexAttribPointer(
        drawDotProgramInfo.attribLocations.vertexPosition,
        numComponents,
        type,
        normalize,
        stride,
        offset);
    gl.enableVertexAttribArray(
        drawDotProgramInfo.attribLocations.vertexPosition);
  }
  gl.useProgram(drawDotProgramInfo.program);
  gl.uniformMatrix4fv(
        drawDotProgramInfo.uniformLocations.projectionMatrix,
        false,
        projectionMatrix);
  gl.uniformMatrix4fv(
    drawDotProgramInfo.uniformLocations.modelViewMatrix,
    false,
    modelViewMatrix);
  {
    const offset = 0;
    const vertexCount = 6;
    gl.drawArrays(gl.TRIANGLES, offset, vertexCount);
  }
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

