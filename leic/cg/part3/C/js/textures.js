///////////////////////
/* GENERATE TEXTURES */
///////////////////////

/* GLOBAL VARIABLES */
var canvas, context, gradient, colors, texture;

var numFlowers = 200;
var numStars = 300;


/**
 * Creates the texture used for the scenes's ground (flower field). 
 */
function generateFlowerFieldTexture() {
  canvas = document.createElement('canvas');
  context = canvas.getContext('2d');
  canvas.width = 512;
  canvas.height = 512;

  // Draw a dark green background.
  context.fillStyle = '#6a994e';
  context.fillRect(0, 0, canvas.width, canvas.height);

  // Draw white, yeallow, purple and light blue circles (flowers).
  colors = ['#FFFFFF', '#FFFFAA', '#FFAAFF', '#AAFFFF'];

  for (let i = 0; i < numFlowers; i++) {
    var x = Math.random() * canvas.width;
    var y = Math.random() * canvas.height;
    var radius = Math.random() * 10 + 2;
    var color = colors[Math.floor(Math.random() * colors.length)];

    context.beginPath();
    context.arc(x, y, radius, 0, 2 * Math.PI, false);
    context.fillStyle = color;
    context.fill();
}
  
  texture = new THREE.CanvasTexture(canvas);
  texture.wrapS = THREE.RepeatWrapping;
  texture.wrapT = THREE.RepeatWrapping;
  texture.repeat.set(4, 4);

  return texture;
}

/**
 * Creates the texture used for the scenes's skydome (starry sky).
 */
function generateStarrySkyTexture() {
  canvas = document.createElement('canvas');
  context = canvas.getContext('2d');
  canvas.width = 512;
  canvas.height = 512;
  
  // Draws a dark blue to dark violet linear gradient background.
  gradient = context.createLinearGradient(0, 0, 0, canvas.height);
  gradient.addColorStop(0, '#001845');
  gradient.addColorStop(1, '#240046');
  context.fillStyle = gradient;
  context.fillRect(0, 0, canvas.width, canvas.height);

  for (let i = 0; i < numStars; i++) {
    var x = Math.random() * canvas.width;
    var y = Math.random() * canvas.height;
    var radius = Math.random() * 2 + 0.5;

    context.beginPath();
    context.arc(x, y, radius, 0, 2 * Math.PI, false);
    context.fillStyle = '#FFFFFF';
    context.fill();
  }

  texture = new THREE.CanvasTexture(canvas);
  texture.wrapS = THREE.RepeatWrapping;
  texture.wrapT = THREE.RepeatWrapping;
  texture.repeat.set(10, 2);

  return texture;  
}
