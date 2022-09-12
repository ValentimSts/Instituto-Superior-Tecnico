// Bakeoff #2 - Seleção de Alvos Fora de Alcance
// IPM 2021-22, Período 3
// Entrega: até dia 22 de Abril às 23h59 através do Fenix
// Bake-off: durante os laboratórios da semana de 18 de Abril

// p5.js reference: https://p5js.org/reference/


// Database (CHANGE THESE!)
const GROUP_NUMBER = "6-AL"; // Add your group number here as an integer (e.g., 2, 3)
const BAKE_OFF_DAY = false; // Set to 'true' before sharing during the bake-off day


// Target and grid properties (DO NOT CHANGE!)
let PPI, PPCM;
let TARGET_SIZE;
let TARGET_PADDING, MARGIN, LEFT_PADDING, TOP_PADDING;
let continue_button;
let inputArea = { x: 0, y: 0, h: 0, w: 0 }; // Position and size of the user input area


// Metrics
let testStartTime, testEndTime; // time between the start and end of one attempt (54 trials)
let hits = 0; // number of successful selections
let misses = 0; // number of missed selections (used to calculate accuracy)
let database; // Firebase DB


// Study control parameters
let draw_targets = false; // used to control what to show in draw()
let trials = []; // contains the order of targets that activate in the test
let current_trial = 0; // the current trial number (indexes into trials array above)
let attempt = 0; // users complete each test twice to account for practice (attemps 0 and 1)
let fitts_IDs = []; // add the Fitts ID for each selection here (-1 when there is a miss)


// Sidebar parameters (new feature)
let bartime = 0;
let timePerTarget = 0.563;
let decreasePerFrame = 50 / (6 * timePerTarget);
let firstTurn = true;


// Color variables
let hit_background_color = [38, 70, 83];
let miss__background_color = [94, 28, 28];

let background_color = hit_background_color;

// main target colors (target to hit)
let mainTarget_color = [255, 23, 42];
let mainTarget_outline = [241, 250, 238];
let mainTarget_hover = [229, 22, 39];

// colors of the next target to hit
let nextTarget_outline = [241, 250, 238];
let nextTarget_background = [70, 116, 111];

// color of the line to the next target
let lineToTarget_color = [241, 250, 238];

// colors of the remaining targets
let genericTarget_color = [77, 87, 99];

// colors of the cursor
let cursor_color = [255, 159, 28];
let cursor_outline = [0, 0, 0];

// sidebar colors
let sidebar_background = [30, 30, 30];
let sidebar_bar = [251, 133, 0];

let input_area_background = [52, 52, 52];
let input_area_outline = [241, 250, 238];

let aimlock_color = [241, 250, 238];

let default_strokeWeight = 4;


// Target class (position and width)
class Target {
  constructor(x, y, w) {
    this.x = x;
    this.y = y;
    this.w = w;
  }

  isEqual(other) {
    return this.x == other.x &&
           this.y == other.y &&
           this.w == other.w;
  }
}


// cursor class (position only)
class Cursor {
  constructor(x, y) {
    this.x = x;
    this.y = y;
  }
}


// real cursor used to "snap" onto targets
let realCursor = new Cursor(0,0);


// Runs once at the start
function setup() {
  createCanvas(700, 500); // window size in px before we go into fullScreen()
  frameRate(60); // frame rate (DO NOT CHANGE!)

  randomizeTrials(); // randomize the trial order at the start of execution

  textFont("Arial", 18); // font size for the majority of the text
  drawUserIDScreen(); // draws the user start-up screen (student ID and display size)
}


/**
 * **New:**
 * Creates a line from the current target to the next one
 * ---
 * @param {*} curr 
 * @param {*} next 
 */
function draw_line(curr, next) {
  stroke(lineToTarget_color);
  strokeWeight(default_strokeWeight);
  line(curr.x, curr.y, next.x, next.y);
}


/**
 * **New:**
 * Higlights the target if the user is hovering over it.
 * ---
 * @param {*} target The target we're testing
 * @param {*} x The x coord of the virtual cursor
 * @param {*} y The y coord of the virtual cursor
 */
function hovering_target(target, x, y) {

  if (dist(target.x, target.y, x, y) < target.w / 2) {

    fill(mainTarget_hover); // target's color while hovered
    stroke(mainTarget_outline); // target's outline
    strokeWeight(8);

    circle(target.x, target.y, target.w);
  }
}


/**
 * **New:**
 * Draws the main target (the one the user should hit).
 * ---
 * @param {*} target The target to hit
 */
function draw_mainTarget(target) {
  // Highlights the target the user should be trying to select
  fill(mainTarget_color); // target's color
  stroke(mainTarget_outline); // color of the target's outline
  strokeWeight(default_strokeWeight); // width of the outline

  // Draws the targets
  circle(target.x, target.y, target.w);
}


/**
 * **New:**
 * Draws the next target to hit.
 *  - if the next target is the same as the current one
 *    indicates it accordingly
 * ---
 * @param {*} target 
 * @param {*} nextTarget 
 */
function draw_nexTarget(target, nextTarget) {

  // creates a highlight on the next target
  stroke(nextTarget_outline); // color of the target's outline
  strokeWeight(default_strokeWeight); // width of the outline

  // if the next target is the same we're currently on
  if (target.isEqual(nextTarget)) {
    fill(color(background_color));
    
    drawingContext.setLineDash([15, 15]);
    circle(nextTarget.x, nextTarget.y, nextTarget.w + 30);
    drawingContext.setLineDash([0, 0]);
  }
  else {
    fill(nextTarget_background);

    drawingContext.setLineDash([15, 15]);
    circle(nextTarget.x, nextTarget.y, nextTarget.w);
    drawingContext.setLineDash([0, 0]);
  }
}


/**
 * **New:**
 * Draws a sidebar on the left of the screen that decreases
 * with time.
 */
function draw_sidebar() {
  strokeWeight(default_strokeWeight);

  // sidebar background color
  fill(sidebar_background);
  rect(110, 280, 40, 500, 20);

  // sidebar "sliding bar" color
  fill(sidebar_bar);
  rect(110, 280, 40, 500 - bartime, 20);
}


/**
 * **New:**
 * Draws a square box sorrounding the target.
 * ---
 * @param {*} target Base target
 */
function draw_area_around_target(target) {
  strokeWeight(default_strokeWeight);
  stroke(10, 10, 10);
  noFill();
  // draws a square around the target
  square(target.x - TARGET_SIZE, target.y - TARGET_SIZE, target.w * 2);
}


function draw_user_instructions() {
  // starts drawing the instructions above the user input area
  let Yspacing = TARGET_SIZE * 2;
  let instructionsY = inputArea.y - TARGET_SIZE*1.5;
  let instructionsX = inputArea.x + TARGET_SIZE;

  let textX = instructionsX + TARGET_SIZE;

  // draws the main target
  fill(mainTarget_color); 
  stroke(mainTarget_outline); 
  strokeWeight(default_strokeWeight);

  circle(instructionsX, instructionsY, TARGET_SIZE);

  fill(mainTarget_outline);
  strokeWeight(0);
  textSize(20);
  text('Target to hit', textX, instructionsY);
  instructionsY -= Yspacing;

  //draws the next target
  fill(nextTarget_background);
  stroke(nextTarget_outline);
  strokeWeight(default_strokeWeight);

  drawingContext.setLineDash([15, 15]);
  circle(instructionsX, instructionsY, TARGET_SIZE);
  drawingContext.setLineDash([0, 0]);

  fill(mainTarget_outline);
  strokeWeight(0);
  textSize(20);
  text('Next target to hit', textX, instructionsY);
  instructionsY -= Yspacing;

  strokeWeight(0);
  textSize(30);
  text('Red background means you missed the target', textX - 130, instructionsY + 50);

  instructionsY = inputArea.y - TARGET_SIZE*1.5;
  instructionsX = inputArea.x + TARGET_SIZE*5;
  textX = instructionsX + TARGET_SIZE;

  // draws the double target
  fill(color(background_color));
  stroke(nextTarget_outline);
  strokeWeight(default_strokeWeight);

  drawingContext.setLineDash([15, 15]);
  circle(instructionsX, instructionsY, TARGET_SIZE + 30);
  drawingContext.setLineDash([0, 0]);

  fill(mainTarget_color); 
  stroke(mainTarget_outline); 
  strokeWeight(default_strokeWeight);

  circle(instructionsX, instructionsY, TARGET_SIZE);

  fill(mainTarget_outline);
  strokeWeight(0);
  textSize(20);
  text('Hit twice', textX, instructionsY);
}


/**
 * **New:**
 * Creates a new cursor that snaps onto the trget closest
 * to the virtual cursor.
 * ---
 * @param {*} x The x coord of the virtual cursor
 * @param {*} y The y coord of the virtual cursor
 */
function aimlock_target(x, y) {
  let target = false;

  // loops over all the targets to find if the cursor is inside
  // the area of any of them.
  for (let i = 0; i < 18; ++i) {
    target = getTargetBounds(i);

    if (inside_target_area(target, x, y)) {
      // "snaps" the real cursor onto the target
      realCursor.x = target.x;
      realCursor.y = target.y;
      break;
    }

    target = false;
  }

  if (target) {
    strokeWeight(default_strokeWeight);
    stroke(aimlock_color);
    noFill();
    circle(target.x, target.y, 0.65 * PPCM);
  }
}


// Runs every frame and redraws the screen
function draw() {
  // decreases the sidebar (redrawing it every fram) -> 60 fps
  if (bartime + decreasePerFrame <= 500 && !firstTurn) {
    bartime += decreasePerFrame;
  }

  if (draw_targets) {
    // The user is interacting with the 6x3 target grid
    background(background_color);

    strokeWeight(0);
    // Print trial count at the top left-corner of the canvas
    fill(color(255, 255, 255));
    textAlign(LEFT);
    text("Trial " + (current_trial + 1) + " of " + trials.length, 50, 20);

    draw_sidebar();

    // Draw all 18 targets
    for (var i = 0; i < 18; i++) {
        drawTarget(i);
    }

    // finds the current, next and previous targets
    let target = getTargetBounds(trials[current_trial]);
    let nextTarget = getTargetBounds(trials[current_trial+1]);

    draw_line(target, nextTarget);

    // redraws the main target so the line is drawn under it
    draw_mainTarget(target);

    // Draw the user input area
    drawInputArea();

    let x = map(mouseX, inputArea.x, inputArea.x + inputArea.w, 0, width);
    let y = map(mouseY, inputArea.y, inputArea.y + inputArea.h, 0, height);
    
    // as long as the virtual cursor isn't inside any of the targets'
    // areas, the real and virtual cursor are the same
    realCursor.x = x;
    realCursor.y = y;

    aimlock_target(x, y);

    hovering_target(target, x, y);

    draw_user_instructions();
    
    strokeWeight(4);
    // cursor color
    fill(cursor_color);
    stroke(cursor_outline);
    circle(x, y, 0.65 * PPCM);
  }
}


/**
 * **New:**
 * Prints the FITTS ids at the end of each attempt.
 */
function print_fitts_IDS() {

  for (let i = 0; i < trials.length; ++i) {
    let text_X = i < trials.length / 2 ? width / 3 : (2 * width) / 3;
    let text_Y = 280 + 20 * (i % (trials.length / 2));

    let value;

    if (i === 0) {
      value = '---';
    }
    else if (fitts_IDs[i] < 0) {
      value = 'MISSED';
    }
    else {
      value = fitts_IDs[i];
    }

    text("Target " + (i+1) + ": " + str(value), text_X, text_Y);
  }
}


// Print and save results at the end of 54 trials
function printAndSavePerformance() {
  // DO NOT CHANGE THESE!
  let accuracy = parseFloat(hits * 100) / parseFloat(hits + misses);
  let test_time = (testEndTime - testStartTime) / 1000;
  let time_per_target = nf(test_time / parseFloat(hits + misses), 0, 3);
  let penalty = constrain((parseFloat(95) - parseFloat(hits * 100) / parseFloat(hits + misses)) * 0.2, 0, 100);
  let target_w_penalty = nf(test_time / parseFloat(hits + misses) + penalty, 0, 3);
  let timestamp = day() + "/" + month() + "/" + year() + "  " + hour() + ":" + minute() + ":" + second();

  
  background(color(0, 0, 0)); // clears screen
  fill(color(255, 255, 255)); // set text fill color to white
  text(timestamp, 10, 20); // display time on screen (top-left corner)

  textAlign(CENTER);
  text("Attempt " + (attempt + 1) + " out of 2 completed!", width / 2, 60);
  text("Hits: " + hits, width / 2, 100);
  text("Misses: " + misses, width / 2, 120);
  text("Accuracy: " + accuracy + "%", width / 2, 140);
  text("Total time taken: " + test_time + "s", width / 2, 160);
  text("Average time per target: " + time_per_target + "s", width / 2, 180);
  text("Average time for each target (+ penalty): " + target_w_penalty + "s", width / 2, 220);

  // Print Fitts IDS (one per target, -1 if failed selection, optional)
  text('Fitts Index of Performance', width / 2, 260);
  print_fitts_IDS();

  // Saves results (DO NOT CHANGE!)
  let attempt_data = {
    project_from: GROUP_NUMBER,
    assessed_by: student_ID,
    test_completed_by: timestamp,
    attempt: attempt,
    hits: hits,
    misses: misses,
    accuracy: accuracy,
    attempt_duration: test_time,
    time_per_target: time_per_target,
    target_w_penalty: target_w_penalty,
    fitts_IDs: fitts_IDs,
  };

  // Send data to DB (DO NOT CHANGE!)
  if (BAKE_OFF_DAY) {
    // Access the Firebase DB

    if (attempt === 0) {
      firebase.initializeApp(firebaseConfig);

      database = firebase.database();
    }

    // Add user performance results
    let db_ref = database.ref("G" + GROUP_NUMBER);

    db_ref.push(attempt_data);
  }
}


/**
 * **New:**
 * Checks if the virtual cursor is inside the target area.
 *  - This area isn't the target circle but the whole "square"
 *    sorrounding it.
 * ---
 * @param {*} target The target we're checking
 * @param {*} x The x coord of our virtual mouse
 * @param {*} y The y coord of out virtual mouse
 * ---
 * @returns True if the cursor is inside the target area, False otherwise.
 */
function inside_target_area(target, x, y) {
  const targetArea = [
    (target.x - target.w), (target.x + target.w), // left and right bounds
    (target.y - target.w), (target.y + target.w), // top and bottom bounds
  ];

  return x >= targetArea[0] && x <= targetArea[1] &&
         y >= targetArea[2] && y <= targetArea[3];
}


/**
 * **New:**
 * Uses Fitts' law to calculate the FITTS performance IDs.
 * ---
 * @param {*} target The target that got hit
 * ---
 * @returns Fitts' performance ID.
 */
function get_fitts_performanceID(current_trial) {

  let prevTarget = getTargetBounds(trials[current_trial-1]);
  let target = getTargetBounds(trials[current_trial]);

  let distance = dist(prevTarget.x, prevTarget.y, target.x, target.y);

  fid = Math.log(distance/target.w + 1) / Math.log(2)

  return fid.toFixed(3);
}


// Mouse button was pressed - lets test to see if hit was in the correct target
function mousePressed() {
  // Only look for mouse releases during the actual test
  // (i.e., during target selections)

  if (draw_targets) {
    // Get the location and size of the target the user should be trying to select
    let target = getTargetBounds(trials[current_trial]);

    // Check to see if the virtual cursor is inside the target bounds,
    // increasing either the 'hits' or 'misses' counters
    if (insideInputArea(mouseX, mouseY)) {

      let virtual_x = realCursor.x;
      let virtual_y = realCursor.y;

      circle(target.x, target.y, target.w);
      
      if (dist(target.x, target.y, virtual_x, virtual_y) < target.w/2) {
        hits++;
        // when the user hits the target, the sidebar resets
        bartime = 0;
        firstTurn = false;

        fitts_IDs.push(get_fitts_performanceID(current_trial));
        // updates the background color as the user hit the right target
        background_color = hit_background_color;
      }
      else {
        misses++;
        firstTurn = false;
        fitts_IDs.push(-1);
        // updates the background color as the user hit the wrong target
        background_color = miss__background_color;
      }

      current_trial++; // Move on to the next trial/target
    }

    // Check if the user has completed all 54 trials
    if (current_trial === trials.length) {
      testEndTime = millis();

      draw_targets = false; // Stop showing targets and the user performance results

      strokeWeight(0);
      printAndSavePerformance(); // Print the user's results on-screen and send these to the DB

      attempt++;

      // If there's an attempt to go create a button to start this
      if (attempt < 2) {
        continue_button = createButton("START 2ND ATTEMPT");
        continue_button.mouseReleased(continueTest);
        continue_button.position(width / 2 - continue_button.size().width / 2, height / 2 - continue_button.size().height / 2);
      }

    }
    else if (current_trial === 1) {
      testStartTime = millis();
    }
  }
}


// Draw target on-screen
function drawTarget(i) {
  // Get the location and size for target (i)

  let target = getTargetBounds(i);
  let nextTarget = getTargetBounds(trials[current_trial+1]);

  // Does not draw a border if this is not the target the user should be trying to select
  noStroke();
  fill(genericTarget_color);

  // Draws the targets
  if (trials[current_trial+1] !== i) {
    circle(target.x, target.y, target.w);
  }

  // Check whether this target is the target the user should be trying to select
  if (trials[current_trial] === i) {
    // draws the next target to hit
    draw_nexTarget(target, nextTarget);
    
    // draws the current target
    draw_mainTarget(target);
  }

  draw_area_around_target(target);
}


// Returns the location and size of a given target
function getTargetBounds(i) {

  var x = parseInt(LEFT_PADDING) + parseInt((i % 3) * (TARGET_SIZE + TARGET_PADDING) + MARGIN);
  var y = parseInt(TOP_PADDING) + parseInt(Math.floor(i / 3) * (TARGET_SIZE + TARGET_PADDING) + MARGIN);

  return new Target(x, y, TARGET_SIZE);
}


// Evoked after the user starts its second (and last) attempt
function continueTest() {
  // Re-randomize the trial order

  shuffle(trials, true);
  current_trial = 0;
  print("trial order: " + trials);

  // Resets performance variables
  hits = 0;
  misses = 0;
  fitts_IDs = [];

  bartime = 0;
  firstTurn = true;

  continue_button.remove();

  // Shows the targets again
  draw_targets = true;

  testStartTime = millis();
}


// Is invoked when the canvas is resized (e.g., when we go fullscreen)
function windowResized() {
  resizeCanvas(windowWidth, windowHeight);

  let display = new Display({ diagonal: display_size }, window.screen);

  // DO NOT CHANGE THESE!
  PPI = display.ppi; // calculates pixels per inch
  PPCM = PPI / 2.54; // calculates pixels per cm
  TARGET_SIZE = 1.5 * PPCM; // sets the target size in cm, i.e, 1.5cm
  TARGET_PADDING = 1.5 * PPCM; // sets the padding around the targets in cm
  MARGIN = 1.5 * PPCM; // sets the margin around the targets in cm

  // Sets the margin of the grid of targets to the left of the canvas (DO NOT CHANGE!)
  LEFT_PADDING = width / 3 - TARGET_SIZE - 1.5 * TARGET_PADDING - 1.5 * MARGIN;

  // Sets the margin of the grid of targets to the top of the canvas (DO NOT CHANGE!)
  TOP_PADDING = height / 2 - TARGET_SIZE - 3.5 * TARGET_PADDING - 1.5 * MARGIN;

  // Defines the user input area (DO NOT CHANGE!)
  inputArea = {
    x: width / 2 + 2 * TARGET_SIZE,
    y: height / 2,
    w: width / 3,
    h: height / 3,
  };

  // Starts drawing targets immediately after we go fullscreen
  draw_targets = true;
}


/**
 * **New:**
 * Draws a the targets to hit on the user input area.
 * ---
 * @param {*} i The target index
 */
function drawTargetInsideInputArea(i){
  target = getTargetBounds(i);

  x = map(target.x, 0, width, inputArea.x, inputArea.x + inputArea.w);
  y = map(target.y, 0, height, inputArea.y, inputArea.y + inputArea.h);

  size = target.w * (inputArea.w / height);
  
  noStroke();
  fill(genericTarget_color);

  if (trials[current_trial] === i) {
    fill(color(mainTarget_color));

    if (trials[current_trial + 1] === i) {
      strokeWeight(default_strokeWeight);
      stroke(color(mainTarget_outline));
    }
  }
  else if (trials[current_trial + 1] === i) {
    fill(color(nextTarget_background));
  }

  rectMode(CENTER);
  rect(x, y, size, size);
  rectMode(CORNER);
}


// Responsible for drawing the input area
function drawInputArea() {
  fill(input_area_background);
  stroke(input_area_outline);
  strokeWeight(2);
  rect(inputArea.x, inputArea.y, inputArea.w, inputArea.h, 20);

  for (let i = 0; i < 18; i++) {
    drawTargetInsideInputArea(i);
  }
}
