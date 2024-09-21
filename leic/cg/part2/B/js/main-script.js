//////////////////////
/* GLOBAL VARIABLES */
//////////////////////

var camera, scene, renderer;
var geometry, mesh;

// Materials array, used to store all the materials used in the scene,
// so that their wireframe can be easily toggles off.
var materials = [];

// Array of objects containing all the main objects (parents) 
// that are used in the scene.
var mainObjects = [];

// Colors used in the scene.
var black = new THREE.Color(0x000000);
var grey = new THREE.Color(0x808080);
var red = new THREE.Color(0xff0000);
var darkBlue = new THREE.Color(0x00008b);
var yellow = new THREE.Color(0xffff00);
var lightGrey = new THREE.Color(0xd3d3d3);
var darkRed = new THREE.Color(0x8b0000);
var lightBlue = new THREE.Color(0xadd8e6);

//
var trailer;
var robot;

// Flags used to know if the trailer animation (moving the trailer
// to its connection point) is ongoing, and if the robot is currently
// in truck mode (transformed).
var trailerAnimation = false;
var distanceTrailerX;
var distanceTrailerZ;
var stillAboard = false;

var rSeg = 30;

var obj1BoundBox;
var obj2BoundBox;
var obj1Size = new THREE.Vector3();
var obj2Size = new THREE.Vector3();


/////////////////////
/* CREATE SCENE(S) */
/////////////////////

function createScene(){
    'use strict';

    scene = new THREE.Scene();

    scene.add(new THREE.AxisHelper(100));
    scene.background = new THREE.Color('skyblue');

    createTrailer(20, 4, -30);
    createRobot(0, 0, 0);
}


//////////////////////
/* CREATE CAMERA(S) */
//////////////////////

function createCamera(camera_id) {
    'use strict';

    // If the trailer animation is currently happening (trailer
    // snapping onto the truck), switching between cameras is disabled.
    if (!trailerAnimation) {

        var x, y, z;

        var viewSize = 100;
        var aspectRatio = window.innerWidth / window.innerHeight;

        if (camera_id != 5) {
            camera = new THREE.OrthographicCamera(-aspectRatio*viewSize/2, aspectRatio*viewSize/2,
                                    viewSize/2, -viewSize/2, -1000, 1000);
        }
        else {
            camera = new THREE.PerspectiveCamera(70, window.innerWidth/window.innerHeight, 
                                            1, 1000);
        }

        switch (camera_id) {
            case 1:
                // Front view.
                x = 0; y = 0; z = 50;
                break;
            case 2:
                // Side view (right).
                x = 50; y = 0; z = 0;
                break;
            case 3:
                // Top view.
                x = 0; y = 50; z = 0;
                break;

            case 4:
                // Isometric perspective.
                x = 5; y = 5; z = 5;
                break;
            case 5:
                // Isometric perspective.
                x = -50; y = 50; z = 50;
                break;
        }
        
        camera.position.x = x;
        camera.position.y = y;
        camera.position.z = z;

        camera.lookAt(scene.position);
    }
}


/////////////////////
/* CREATE LIGHT(S) */
/////////////////////

////////////////////////
/* CREATE OBJECT3D(S) */
////////////////////////

/**
 * Creates mesh for the given geometry in the specified position.
 * Adds the mesh's material to the materials array.
 * 
 *  - Returns the created mesh.
 */
function createMesh(geometry, x, y, z, color) {
    'use strict';

    // Create a material for the box, and push it to the materials array.
    var material = new THREE.MeshBasicMaterial({color: color, wireframe: true});
    materials.push(material);

    mesh = new THREE.Mesh(geometry, material);
    mesh.position.set(x, y, z);

    return mesh;
}

/**
 * Creates a Box object with the given dimensions and position,
 * and adds it as a child node to 'obj'.
 * 
 *  - Returns the created object if the 'returnMesh' flag is set to true.
 */
function addBox(obj, width, height, depth, x, y, z, returnMesh, color) {
    'use strict';

    geometry = new THREE.BoxGeometry(width, height, depth);
    
    var mesh = createMesh(geometry, x, y, z, color);
    obj.add(mesh);

    if (returnMesh) {
        return mesh;
    }
}

/**
 * Creates a Cone object with the given dimensions and position,
 * and adds it as a child node to 'obj'.
 * 
 * - Returns the created object if the 'returnMesh' flag is set to true.
 * 
 */
function addCone(obj, radius, height, rSeg, x, y, z, returnMesh, color) {
    'use strict';

    geometry = new THREE.ConeGeometry(radius, height, rSeg);
    
    var mesh = createMesh(geometry, x, y, z, color);
    obj.add(mesh);

    if (returnMesh) {
        return mesh;
    }
}

/**
 * Creates a Cylinder object with the given dimensions and position,
 * and adds it as a child node to 'obj'.
 * In this project's context, cylinders are mostly used to represent wheels,
 * so they will only be rotated 90 degrees on the z axis if the 'rotate' flag
 * is set to true.
 * 
 *  - Returns the created object if the 'returnMesh' flag is set to true.
 */

function addCylinder(obj, radius, height, rSeg, x, y, z, returnMesh, rotate, color) {
    'use strict';

    geometry = new THREE.CylinderGeometry(radius, radius, height, rSeg);
    
    var mesh = createMesh(geometry, x, y, z, color);
    
    if (rotate) {
        mesh.rotation.z = 0.5*Math.PI; // Rotate 90 degrees on the z axis.
    }

    obj.add(mesh);

    if (returnMesh) {
        return mesh;
    }
}


/* ================= TRAILER ====================*/

/**
 * Creates a trailer object in the given position.
 */
function createTrailer(x, y, z) {
    'use strict'

    // Parent object for the trailer.
    trailer = new THREE.Object3D();
    trailer.userData = { right: 0, left: 0, up: 0, down: 0 };

    // Create a material for the trailer, and push it to the materials array.
    var material = new THREE.MeshBasicMaterial({wireframe: true});
    materials.push(material);

    // Dimensions for the trailer.
    var wContainer = 8, hContainer = 14, dContainer = 30;
    var wBase = 6, hBase = 4, dBase = 13;
    var wJoint = 2, hJoint = 1, dJoint = 2;
    var rWheel = 1.5, hWheel = 1, rSeg = 32;

    var container = addBox(trailer, wContainer, hContainer, dContainer, 0, 0, 0, true, grey);
    var containerBase = addBox(container, wBase, hBase, dBase,
                            0, -(hContainer+hBase)/2, -dBase/2, true, darkBlue);
    // Trailer's connection point.
    addBox(trailer, wJoint, hJoint, dJoint, 0, (hJoint-hContainer)/2, (dContainer+dJoint)/2, false, darkBlue);
    
    // Create and add the wheels to the trailer.
    addCylinder(containerBase, rWheel, hWheel, rSeg, (wBase+hWheel)/2, -hBase/2, -2*rWheel, false, true, black);
    addCylinder(containerBase, rWheel, hWheel, rSeg, -(wBase+hWheel)/2, -hBase/2, -2*rWheel, false, true, black);
    addCylinder(containerBase, rWheel, hWheel, rSeg, (wBase+hWheel)/2, -hBase/2, 2*rWheel, false, true, black);
    addCylinder(containerBase, rWheel, hWheel, rSeg, -(wBase+hWheel)/2, -hBase/2, 2*rWheel, false, true, black);

    scene.add(trailer);

    trailer.position.x = x;
    trailer.position.y = y;
    trailer.position.z = z;

}


/* ================= ROBOT ====================*/

/**
 * Creates the robot's head object in the given position 
 * and adds it as a child node to 'obj'.
 */
function createRobotHead(obj, x, y, z) {
    'use strict';
    
    // Parent object for the head.
    var head_obj = new THREE.Object3D();
    
    // Create a material for the head, and push it to the materials array.
    var material = new THREE.MeshBasicMaterial({wireframe: true});
    materials.push(material);

    // Head parts dimensions.
    var wHead = 3, hHead = 3, dHead = 3;
    var wEye = 1, hEye = 0.5, dEye = 0.5;
    var rEar = 0.25, hEar = 1.5;

    var headPivot = new THREE.Object3D();
    head_obj.add(headPivot);
    headPivot.position.set(x, y-0.5, z+4);

    var head = addBox(headPivot, wHead, hHead, dHead, 0, 2, 0, true, darkBlue);

    // Create and add the eyes/ears to the head.
    addBox(head, wEye, hEye, dEye, -wHead/2+wEye/2, 0.5, wHead/2+dEye/2, false, lightGrey); // right eye
    addBox(head, wEye, hEye, dEye, wHead/2-wEye/2, 0.5, wHead/2+dEye/2, false, lightGrey); // left eye
    addCone(head, rEar, hEar, rSeg, -wHead/2+rEar/2, hHead/2+hEar/2, 0, false, darkBlue); // left ear
    addCone(head, rEar, hEar, rSeg, wHead/2-rEar/2, hHead/2+hEar/2, 0, false, darkBlue); // right ear

    head_obj.userData = {rotateClockwise: 0, rotateCounterClockwise: 0, pivot: headPivot};
    
    mainObjects.push({name: 'head', obj: head_obj, truckMode: false });
    
    obj.add(head_obj);
    return head_obj;
}

/**
 * Creates the robot's arm object in the given position 
 * and adds it as a child node to 'obj'.
 * If the 'left' flag is set to true, the arm will be created on the left side,
 * otherwise it will be created on the right side.
 */
function createRobotArm(obj, x, y, z, left) {
    'use strict';

    var arm_obj = new THREE.Object3D();

    // Create a material for the arm, and push it to the materials array.
    var material = new THREE.MeshBasicMaterial({wireframe: true});
    materials.push(material);

    // Arm parts dimensions.
    var wArm = 2, hArm = 6, dArm = 2;
    var wForearm = 7, hForearm = 2, dForearm = 2;
    var wElbow = 2, hElbow = 2, dElbow = 2;
    var rElbowJoint = 1, hElbowJoint = 2, rSeg = 30;
    var rWrist = 0.5, hWrist = 0.5;
    var rPipe = 1.5/2, hPipe = 4;
    var rPipeSmall = 0.25, hPipeSmall = 2.5;

    // Truck default position for the arm.
    var armTargetPosition;

    // ARM:
    if (left) {
        var arm = addBox(arm_obj, wArm, hArm, dArm, x+wArm/2, y, z, true, red);
        var pipe = addCylinder(arm, rPipe, hPipe, rSeg, wArm/2+rPipe, 0, 0, true, false, lightGrey);
        addCylinder(pipe, rPipeSmall, hPipeSmall, rSeg, 0, (hPipe+hPipeSmall)/2, 0, false, false, lightGrey);

        // Arm's target position when the robot is in truck mode.
        armTargetPosition = new THREE.Vector3(arm_obj.position.x - wArm,
                                              arm_obj.position.y,
                                              arm_obj.position.z-3-dArm/2);
    }
    else {
        var arm = addBox(arm_obj, wArm, hArm, dArm, x-wArm/2, y, z, true, red);
        var pipe = addCylinder(arm, rPipe, hPipe, rSeg, -wArm/2-rPipe, 0, 0, true, false, lightGrey);
        addCylinder(pipe, rPipeSmall, hPipeSmall, rSeg, 0, (hPipe+hPipeSmall)/2, 0, false, false, lightGrey);

        // Arm's target position when the robot is in truck mode.
        armTargetPosition = new THREE.Vector3(arm_obj.position.x + wArm,
                                              arm_obj.position.y,
                                              arm_obj.position.z-3-dArm/2);
    }
    
    // ELBOW:
    var elbowJoint = addCylinder(arm, rElbowJoint, hElbowJoint, rSeg, 0, -hArm/2-rElbowJoint, 0, true, true, grey);
    addBox(elbowJoint, wElbow, hElbow, dElbow, 0, 0, -wElbow/2-rElbowJoint, false, red);

    elbowJoint.userData = { angle: 0, minAngle: 0, maxAngle: -Math.PI/2 , rotateClockwise: 0, rotateCounterClockwise: 0 };

    // FOREARM:
    var forearm = addBox(elbowJoint, wForearm, hForearm, dForearm, -wForearm/2, 0, 0, true, red);
    addCylinder(forearm, rWrist, hWrist, rSeg, -wForearm/2-hWrist/2, 0, 0, false, true, yellow); // wrist.


    // Store the arm and elbow joint objects' information in the mainObjects array.
    if (left) { 
        mainObjects.push({name: 'leftArm', obj: arm_obj, truckMode: false });
        mainObjects.push({name: 'leftElbowJoint', obj: elbowJoint, truckMode: false });
    }
    else {
        mainObjects.push({name: 'rightArm', obj: arm_obj, truckMode: false });
        mainObjects.push({name: 'rightElbowJoint', obj: elbowJoint, truckMode: false });
    }

    var limRobot = new THREE.Vector3();
    arm_obj.getWorldPosition(limRobot);
    arm_obj.userData = { limRobot: limRobot, limTruck: armTargetPosition, right: 0, left: 0, up: 0, down: 0 };

    obj.add(arm_obj);
    return arm_obj;
}

/**
 * Creates the robot's leg object in the given position 
 * and adds it as a child node to 'obj'.
 * If the 'left' flag is set to true, the leg will be created on the left side,
 * otherwise it will be created on the right side.
 */
function createRobotLeg(obj, x, y, z, left) {
    'use strict';

    var leg_obj = new THREE.Object3D();

    // Create a material for the leg, and push it to the materials array.
    var material = new THREE.MeshBasicMaterial({wireframe: true});
    materials.push(material);

    // Torso parts dimensions.
    var rTube = 0.5, hTube = 3 , rSeg = 32;
    var rWheel = 1.5, hWheel = 1;
    var wFoot = 3, hFoot = 2, dFoot = 4;
    var wLowLeg = 3, hLowLeg = 12, dLowLeg = 4;
    var wLeg = 2, hLeg = 3, dLeg = 3;

    var leg = addBox(leg_obj, wLeg, hLeg, dLeg, x, y-hLeg/2, z, true, grey);
    var LowLeg = addBox(leg, wLowLeg, hLowLeg, dLowLeg, 0, -(hLowLeg+hLeg)/2, 0, true, darkBlue);

    // Create a pivot point for the foot.
    let footPivot = new THREE.Object3D();
    LowLeg.add(footPivot);
    footPivot.position.set(x, y-hLowLeg/2+hFoot, z);

    var foot = addBox(footPivot, wFoot, hFoot, dFoot, 0, hFoot/2, dLowLeg/2, true, darkBlue);
    foot.userData = {rotateClockwise: 0, rotateCounterClockwise: 0, pivot: footPivot};

    if (left) {
        mainObjects.push({name: 'leftFoot', obj: foot, truckMode: false });
    }
    else {
        mainObjects.push({name: 'rightFoot', obj: foot, truckMode: false });
    }

    if (left) { // wheels
        addCylinder(LowLeg, rWheel, hWheel, rSeg, wLowLeg/2, hWheel/2, dLowLeg/2, false, true, black);
        addCylinder(LowLeg, rWheel, hWheel, rSeg, wLowLeg/2, (-3/4)*dLowLeg, dLowLeg/2, false, true, black);
        addCylinder(LowLeg, rTube, hTube, rSeg, wLowLeg/2+rTube, hLowLeg/2-hTube/2, -hTube/3 + dLowLeg/2 , false, false, lightGrey); // Pipe.

        // Store the leg object's information in the mainObjects array.
        mainObjects.push({name: 'leftLeg', obj: leg_obj, truckMode: false });
    }
    else {
        addCylinder(LowLeg, rWheel, hWheel, rSeg, -wLowLeg/2, hWheel/2, dLowLeg/2, false, true, black); 
        addCylinder(LowLeg, rWheel, hWheel, rSeg, -wLowLeg/2, (-3/4)*dLowLeg, dLowLeg/2, false, true, black);
        addCylinder(LowLeg, rTube, hTube, rSeg, -wLowLeg/2-rTube, hLowLeg/2-hTube/2, -hTube/3 + dLowLeg/2 , false, false, lightGrey); // Pipe.

        // Store the leg object's information in the mainObjects array.
        mainObjects.push({name: 'rightLeg', obj: leg_obj, truckMode: false });
    }

    leg_obj.userData = {rotateClockwise: 0, rotateCounterClockwise: 0};
    obj.add(leg_obj);
    return leg_obj;
}

/**
 * Creates the robot's torso object in the given position 
 * and adds it as a child node to 'obj'.
 */
function createRobotTorso(obj, x, y, z) {
    'use strict';

    var torso = new THREE.Object3D();

    // Create a material for the torso, and push it to the materials array.
    var material = new THREE.MeshBasicMaterial({wireframe: true});
    materials.push(material);

    // Torso parts dimensions.
    var wChest = 8, hChest = 6, dChest = 6;
    var wSideWindow = 8.01, hSideWindow = 3, dSideWindow = 1;
    var wFrontWindow = 3, hFrontWindow = 3, dFrontWindow = 1;
    var wWaist = 4, hWaist = 2, dWaist = 6;
    var wHip = 6, hHip = 2, dHip = 6;
    var rWheel = 1.5, hWheel = 1, rSeg = 32;
    var wNeck =  6, hNeck = 0.5, dNeck = 5;

    var chest = addBox(torso, wChest, hChest, dChest, x, y, z, true, red);
    // Create and add the robot's windows to the chest.
    addBox(chest, wSideWindow, hSideWindow, dSideWindow, 0, 0, 2*dSideWindow, false, lightBlue);
    addBox(chest, wFrontWindow, hFrontWindow, dFrontWindow, 3*wFrontWindow/5, 0, 2.5*dFrontWindow, false, lightBlue);
    addBox(chest, wFrontWindow, hFrontWindow, dFrontWindow, -3*wFrontWindow/5, 0, 2.5*dFrontWindow, false, lightBlue);

    // Create and add the neck to the chest.
    addBox(chest, wNeck, hNeck, dNeck, 0, (hChest+hNeck)/2, dNeck/2-dChest/2, false, red);

    var waist = addBox(chest, wWaist, hWaist, dWaist, 0, -(hChest+hWaist)/2, 0, true, red);
    var hip = addBox(waist, wHip, hHip, dHip, 0, -(hWaist+hHip)/2, 0, true, grey);

    // Create and add the robot's hip wheels to the torso.
    addCylinder(hip, rWheel, hWheel, rSeg, (wHip+hWheel)/2, -hHip/2, 0, false, true, black);
    addCylinder(hip, rWheel, hWheel, rSeg, -(wHip+hWheel)/2, -hHip/2, 0, false, true, black);
    
    /////////////////////////////
    // REMAINING ROBOT MEMBERS //
    /////////////////////////////

    // HEAD.
    var head = createRobotHead(torso, 0, hChest/2+hNeck, -dNeck);
    
    // ARMS.
    var leftArm = createRobotArm(torso, wChest/2, 0, 0, true);
    var rightArm = createRobotArm(torso, -wChest/2, 0, 0, false);
    
    // Create Pivots for the robot's legs.
    var rightLegPivot = new THREE.Object3D();
    var leftLegPivot = new THREE.Object3D();
    hip.add(rightLegPivot);
    hip.add(leftLegPivot);

    // Positions the pivots in the correct position.
    rightLegPivot.position.set(wHip/4, hHip/2, 0);
    leftLegPivot.position.set(-wHip/4, hHip/2, 0);

    // LEGS.
    var leftLeg = createRobotLeg(rightLegPivot, 0, -hHip, 0, true);
    var rightLeg = createRobotLeg(leftLegPivot, 0, -hHip, 0, false);

    obj.add(torso);
    return torso;
}

/**
 * Creates a robot object in the given position.
 */
function createRobot(x, y, z) {
    'use strict'

    // Parent object for the robot.
    robot = new THREE.Object3D();

    // Create a material for the robot, and push it to the materials array.
    var material = new THREE.MeshBasicMaterial({color: 0xff0000, wireframe: true});
    materials.push(material);

    var torso = createRobotTorso(robot, 0, 0, 0);

    scene.add(robot);

    robot.position.x = x;
    robot.position.y = y;
    robot.position.z = z;
}


//////////////////////
/* CHECK COLLISIONS */
//////////////////////
function checkCollisions() {
    'use strict';

    var trailerw = 8;
    var trailerh = 19.5;
    var trailerd = 30;

    // Calculate the minimum and maximum coordinates of each object
    const truckMinX = -5.5;
    const truckMaxX = 5.5;
    const truckMinY = -8.5;
    const truckMaxY = 4.5;
    const truckMinZ = -21;
    const truckMaxZ = 3;

    const trailerMinX = trailer.position.x - trailerw / 2;
    const trailerMaxX = trailer.position.x + trailerw / 2;
    const trailerMinY = trailer.position.y - trailerh / 2;
    const trailerMaxY = trailer.position.y + trailerh / 2;
    const trailerMinZ = trailer.position.z - trailerd / 2;
    const trailerMaxZ = trailer.position.z + trailerd / 2;

    // Check for overlap on each axis
    const overlapX = truckMinX <= trailerMaxX && truckMaxX >= trailerMinX;
    const overlapY = truckMinY <= trailerMaxY && truckMaxY >= trailerMinY;
    const overlapZ = truckMinZ <= trailerMaxZ && truckMaxZ >= trailerMinZ;

    // Return true if there is overlap on all three axes, indicating a collision
    return overlapX && overlapY && overlapZ;
}

///////////////////////
/* HANDLE COLLISIONS */
///////////////////////
function handleCollisions(){
    'use strict';

    trailerAnimation = true;
}

////////////
/* UPDATE */
////////////
function update() {
    'use strict';

    var trailerTargetPosition = new THREE.Vector3(0, 4, -22);
    var speed = 0.05;
    
    if (trailerAnimation) { // after a collision was detected
        if (trailer.position.x != trailerTargetPosition.x) {
            trailer.position.x -= distanceTrailerX * speed;
            distanceTrailerX -= distanceTrailerX * speed;
        }
        if (trailer.position.z != trailerTargetPosition.z) {
            trailer.position.z -=  distanceTrailerZ * speed;
            distanceTrailerZ -= distanceTrailerZ * speed;
        }
        if ((distanceTrailerX < 0.02 && distanceTrailerX > 0) || (distanceTrailerX > -0.02 && distanceTrailerX < 0)) {
            distanceTrailerX = 0;
        }
        if ((distanceTrailerZ < 0.02 && distanceTrailerZ > 0) || (distanceTrailerZ > -0.02 && distanceTrailerZ < 0)) {
            distanceTrailerZ = 0;
        }
        if (distanceTrailerZ == 0 && distanceTrailerX == 0) {
            trailerAnimation = false;
            stillAboard = true;
        }
    }
    else if (!stillAboard && checkTruckMode() && checkCollisions()) {
        distanceTrailerX = (trailer.position.x - trailerTargetPosition.x);
        distanceTrailerZ = (trailer.position.z - trailerTargetPosition.z);

        handleCollisions();
    }
    else if (stillAboard && !checkCollisions(trailer, robot)) {
        stillAboard = false;
    }
    else {
        // Update the trailer:
        moveTrailer();

        // Update the robot:
        // -> Head:
        moveHead();

        // -> Arms:
        moveArms();

        // -> Legs:
        moveLegs();

        // -> Feet:
        moveFeet();

    }
}

/////////////
/* DISPLAY */
/////////////

function render() {
    'use strict';

    renderer.render(scene, camera);
}


////////////////////////////////
/* INITIALIZE ANIMATION CYCLE */
////////////////////////////////

function init() {
    'use strict';

    var default_camera = 4;

    renderer = new THREE.WebGLRenderer({antialias: true});
    renderer.setSize(window.innerWidth, window.innerHeight);
    document.body.appendChild(renderer.domElement);

    createScene();
    createCamera(default_camera);

    render();

    window.addEventListener("keydown", onKeyDown);
    window.addEventListener("keyup", onKeyUp);
    window.addEventListener("resize", onResize);
}


/////////////////////
/* ANIMATION CYCLE */
/////////////////////

function animate() {
    'use strict';

    update();
    
    render();

    requestAnimationFrame(animate);
}


////////////////////////////
/* RESIZE WINDOW CALLBACK */
////////////////////////////

function onResize() { 
    'use strict';

    renderer.setSize(window.innerWidth, window.innerHeight);

    if (window.innerHeight > 0 && window.innerWidth > 0) {
        camera.aspect = window.innerWidth / window.innerHeight;
        camera.updateProjectionMatrix();
    }
}


///////////////////////
/* KEY DOWN CALLBACK */
///////////////////////

function onKeyDown(e) {
    'use strict';

    switch (e.keyCode) {
        // Change between cameras.
        case 49: //1
            createCamera(1);
            break;
        case 50: //2
            createCamera(2);
            break;
        case 51: //3
            createCamera(3);
            break;
        case 52: //4
            createCamera(4);
            break;
        case 53: //5
            createCamera(5);
            break;


        // Toggle wireframe.
        case 54: //6
            toggleWireframe();
            break;
            

        // Move the trailer.
        case 37: //left arrow
            trailer.userData.left = 1;
            break;
        case 38: //up arrow
            trailer.userData.up = 1;
            break;
        case 39: //right arrow  
            trailer.userData.right = 1;
            break;
        case 40: //down arrow
            trailer.userData.down = 1;
            break;
            

        // Toggle rotations and movements.
        // Feet:
        case 81: //q [to ROBOT]
            setFeetUserData(true, false);
            break;
        case 65: //a [to TRUCK]
            setFeetUserData(false, false);
            break;

        // Legs:
        case 87: //w [to ROBOT]
            setLegsUserData(true, false);
            break;
        case 83: //s [to TRUCK]
            setLegsUserData(false, false);
            break;

        // ARMS:
        case 69: //e [to ROBOT]
            setArmsUserData(false, false);
            break;
        case 68: //d [to TRUCK]
            setArmsUserData(true, false);
            break;

        // HEAD:
        case 82: //r [to ROBOT]
            setHeadUserData(false, false);
            break;
        case 70: //f [to TRUCK]
            setHeadUserData(true, false);
            break;
    }

}


///////////////////////
/* KEY UP CALLBACK */
///////////////////////

function onKeyUp(e){
    'use strict';

    switch (e.keyCode) {
        
        // Reset the trailer.
        case 37: //left arrow
            trailer.userData.left = 0;
            break;
        case 38: //up arrow
            trailer.userData.up = 0;
            break;
        case 39: //right arrow  
            trailer.userData.right = 0;
            break;
        case 40: //down arrow
            trailer.userData.down = 0;
            break;

        // Reset the robot.
        // Feet:
        case 81: //q [to ROBOT]
            setFeetUserData(true, true);
            break;
        case 65: //a [to TRUCK]
            setFeetUserData(false, true);
            break;

        // Legs:
        case 87: //w [to ROBOT]
            setLegsUserData(true, true);
            break;
        case 83: //s [to TRUCK]
            setLegsUserData(false, true);
            break;

        // ARMS:
        case 69: //e [to ROBOT]
            setArmsUserData(false, true);
            break;
        case 68: //d [to TRUCK]
            setArmsUserData(true, true);
            break;

        // HEAD:
        case 82: //r [to ROBOT]
            setHeadUserData(false, true);
            break;
        case 70: //f [to TRUCK]
            setHeadUserData(true, true);
            break;
    }
}

function toggleWireframe(){
    'use strict';

    for(var i = 0; i < materials.length; i++){
        materials[i].wireframe = !materials[i].wireframe;
    }
}


//////////////////////
/* HELPER FUNCTIONS */
//////////////////////

/**
 * Returns the object with the given name. If it
 * doesn't exist, returns null.
 */
function getMainObject(name) {
    'use strict';

    for (var i = 0; i < mainObjects.length; i++) {
        if (mainObjects[i].name == name) {
            return mainObjects[i].obj;
        }
    }

    return null;
}

/**
 * 
 */
function rotateObject(name, point, axis, rotation) {
    'use strict';

    var obj = getMainObject(name);
    if (obj) {

        let newPos = new THREE.Vector3().copy(obj.position).sub(point);
        let originalPos = new THREE.Vector3().copy(obj.position);

        // Move the object to the center.
        obj.position.set(newPos.x, newPos.y, newPos.z);
        
        // Rotates the object.
        switch (axis) {
            case 'x':
                obj.rotation.x += rotation;
                break;
            case 'y':
                obj.rotation.y += rotation;
                break;
            case 'z':
                obj.rotation.z += rotation;
                break;
        }
        
        // Returns the object to its original position.
        obj.position.set(originalPos.x, originalPos.y, originalPos.z);
    }
}

/**
 * Moves the trailer according to the keys pressed.
 */
function moveTrailer() {
    'use strict';

    let speed = 0.2;

    trailer.position.x += speed * trailer.userData.right;
    trailer.position.x -= speed * trailer.userData.left;
    trailer.position.z += speed * trailer.userData.down;
    trailer.position.z -= speed * trailer.userData.up;
}

/**
 * Rotates the robot's head according to the keys pressed.
 */
function moveHead() {
    'use strict';

    // Main objects for the head.
    let head = getMainObject('head');

    let delta = 0.1;
    // Min and max angles of rotation.
    let maxAngle = 0, minAngle = -Math.PI;

    let headSpeed = delta*(head.userData.rotateClockwise - head.userData.rotateCounterClockwise);

    // Clamps the head's rotation to be between the min and max angles.
    head.userData.pivot.rotation.x = THREE.MathUtils.clamp(head.userData.pivot.rotation.x - headSpeed, minAngle, maxAngle);

    if (head.userData.pivot.rotation.x == minAngle) {
        head.truckMode = true;
    }
    else {
        head.truckMode = false;
    }
}



function moveArms() {
    'use strict';

    // Main objects for the forearms.
    let rightElbowJoint = getMainObject('rightElbowJoint');
    let leftElbowJoint = getMainObject('leftElbowJoint');

    let delta = 0.1;
    // Min and max angles of rotation.
    let maxAngle = 0, minAngle = -Math.PI/2;

    let rightElbowSpeed = delta*(rightElbowJoint.userData.rotateClockwise - rightElbowJoint.userData.rotateCounterClockwise);
    let leftElbowSpeed = delta*(leftElbowJoint.userData.rotateClockwise - leftElbowJoint.userData.rotateCounterClockwise);

    // Clamps the forearms' rotation to be between the min and max angles.
    rightElbowJoint.rotation.x = THREE.MathUtils.clamp(
                                    rightElbowJoint.rotation.x - rightElbowSpeed,
                                    minAngle, maxAngle);
    leftElbowJoint.rotation.x = THREE.MathUtils.clamp(
                                    leftElbowJoint.rotation.x - leftElbowSpeed, 
                                    minAngle, maxAngle);


    // Main objects for the arms.
    let rightArm = getMainObject('rightArm');
    let leftArm = getMainObject('leftArm');
    
    const armsDeltaX = 0.05;
    const armsDeltaZ = 0.05;
    
    let rightArmSpeedX = armsDeltaX*(rightArm.userData.left-rightArm.userData.right);
    let rightArmSpeedZ = armsDeltaZ*(rightArm.userData.down-rightArm.userData.up);
    
    let leftArmSpeedX = armsDeltaX*(leftArm.userData.left-leftArm.userData.right);
    let leftArmSpeedZ = armsDeltaZ*(leftArm.userData.down-leftArm.userData.up);

    // Clamps the arms' position to be between the min and max positions ('limRobot', 'limTruck').
    rightArm.position.x += rightArmSpeedX;
    rightArm.position.z += rightArmSpeedZ;
    rightArm.position.x = THREE.MathUtils.clamp(rightArm.position.x + rightArmSpeedX,
                            rightArm.userData.limRobot.x, rightArm.userData.limTruck.x);
    rightArm.position.z = THREE.MathUtils.clamp(rightArm.position.z + rightArmSpeedZ,
                            rightArm.userData.limTruck.z, rightArm.userData.limRobot.z);

    leftArm.position.x += leftArmSpeedX;
    leftArm.position.z += leftArmSpeedZ;
    leftArm.position.x = THREE.MathUtils.clamp(leftArm.position.x + leftArmSpeedX,
                            leftArm.userData.limTruck.x, leftArm.userData.limRobot.x);
    leftArm.position.z = THREE.MathUtils.clamp(leftArm.position.z + leftArmSpeedZ,
                            leftArm.userData.limTruck.z, leftArm.userData.limRobot.z);
                            
    if (rightArm.position.equals(rightArm.userData.limTruck) && rightElbowJoint.rotation.x == minAngle) {
        rightArm.truckMode = true;
        leftArm.truckMode = true;
        rightElbowJoint.truckMode = true;
        leftElbowJoint.truckMode = true;
    }
    else {
        rightArm.truckMode = false;
        leftArm.truckMode = false;
        rightElbowJoint.truckMode = false;
        leftElbowJoint.truckMode = false;
    }
}

/**
 * Rotates the robot's legs according to the keys pressed ('w', 's').
 */
function moveLegs() {
    'use strict';

    // Main objects for the legs.
    let rightLeg = getMainObject('rightLeg');
    let leftLeg = getMainObject('leftLeg');
    
    let delta = 0.05;
    // Min and max angles of rotation.
    let minAngle = 0, maxAngle = Math.PI/2;
    
    let rightLegSpeed = delta*(rightLeg.userData.rotateClockwise - rightLeg.userData.rotateCounterClockwise);
    let leftLegSpeed = delta*(leftLeg.userData.rotateClockwise - leftLeg.userData.rotateCounterClockwise);

    // Clamps the legs' rotation to be between the min and max angles.
    rightLeg.rotation.x = THREE.MathUtils.clamp(rightLeg.rotation.x - rightLegSpeed, minAngle, maxAngle);
    leftLeg.rotation.x = THREE.MathUtils.clamp(leftLeg.rotation.x - leftLegSpeed, minAngle, maxAngle);

    if (rightLeg.rotation.x == maxAngle) {
        rightLeg.truckMode = true;
        leftLeg.truckMode = true;
    }
    else {
        rightLeg.truckMode = false;
        leftLeg.truckMode = false;
    }
}

/**
 * Rotates the robot's feet according to the keys pressed ('q', 'a').
 */
function moveFeet() {
    'use strict';

    // Main objects for the feet.
    let rightFoot = getMainObject('rightFoot');
    let leftFoot = getMainObject('leftFoot');

    let delta = 0.05;
    // Min and max angles of rotation.
    let minAngle = 0, maxAngle = Math.PI/2;  

    let rightFootSpeed = delta*(rightFoot.userData.rotateClockwise - rightFoot.userData.rotateCounterClockwise);
    let leftFootSpeed = delta*(leftFoot.userData.rotateClockwise - leftFoot.userData.rotateCounterClockwise);

    // Clamps the feet's rotation to be between the min and max angles.
    rightFoot.userData.pivot.rotation.x = THREE.MathUtils.clamp(rightFoot.userData.pivot.rotation.x - rightFootSpeed, minAngle, maxAngle);
    leftFoot.userData.pivot.rotation.x = THREE.MathUtils.clamp(leftFoot.userData.pivot.rotation.x - leftFootSpeed, minAngle, maxAngle);

    if (rightFoot.userData.pivot.rotation.x == maxAngle) {
        rightFoot.truckMode = true;
        leftFoot.truckMode = true;
    }
    else {
        rightFoot.truckMode = false;
        leftFoot.truckMode = false;
    }
}


/* ==================== */
/*  USERDATA FUNCTIONS  */
/* ==================== */

/**
 * Sets the arms' userData values (elbowJoints and Arms objects) 
 * depending on the 'clockwise' flag.
 * Resets all the values if the 'reset' flag is set to true. 
 */
function setArmsUserData(clockwise, reset) {
    'use strict'
    
    // Main objects for the forearms.
    let rightElbowJoint = getMainObject('rightElbowJoint');
    let leftElbowJoint = getMainObject('leftElbowJoint');
    // Main objects for the arms.
    let rightArm = getMainObject('rightArm');
    let leftArm = getMainObject('leftArm');
    
    let val = (reset == true ? 0 : 1);
    
    if (clockwise) {
        // Clockwise motion (to truck).
        rightElbowJoint.userData.rotateClockwise = val;
        leftElbowJoint.userData.rotateClockwise = val;

        rightArm.userData.up = val;
        rightArm.userData.left = val;

        leftArm.userData.up = val;
        leftArm.userData.right = val;
    }
    else {
        // Counter clockwise motion (to robot).
        rightElbowJoint.userData.rotateCounterClockwise = val;
        leftElbowJoint.userData.rotateCounterClockwise = val;

        rightArm.userData.down = val;
        rightArm.userData.right = val;

        leftArm.userData.down = val;
        leftArm.userData.left = val;
    }
}

/**
 * Sets the head's userData values (head object) depending
 * on the 'clockwise' flag.
 * Resets all the values if the 'reset' flag is set to true. 
 */
function setHeadUserData(clockwise, reset) {
    'use strict'
    
    // Main object for the head.
    var head = getMainObject('head');
    
    let val = (reset == true ? 0 : 1);
    
    if (clockwise) {
        head.userData.rotateClockwise = val;
    }
    else {
        head.userData.rotateCounterClockwise = val;
    }
}

/**
 * Sets the leg's userData values (legs objects) depending
 * on the 'clockwise' flag.
 * Resets all the values if the 'reset' flag is set to true. 
 */
function setLegsUserData(clockwise, reset) {
    'use strict'

    let rightLeg = getMainObject('rightLeg');
    let leftLeg = getMainObject('leftLeg');

    let val = (reset == true ? 0 : 1);

    if (clockwise) {
        // Clockwise motion (to robot).
        rightLeg.userData.rotateClockwise = val;
        leftLeg.userData.rotateClockwise = val;
    }
    else {
        // Counter clockwise motion (to truck).
        rightLeg.userData.rotateCounterClockwise = val;
        leftLeg.userData.rotateCounterClockwise = val;
    }
}

/**
 * Sets the feet's userData values (feet objects) depending
 * on the 'clockwise' flag.
 * Resets all the values if the 'reset' flag is set to true. 
 */
function setFeetUserData(clockwise, reset) {
    'use strict'

    let rightFoot = getMainObject('rightFoot');
    let leftFoot = getMainObject('leftFoot');

    let val = (reset == true ? 0 : 1);

    if (clockwise) {
        // Clockwise motion (to robot).
        rightFoot.userData.rotateClockwise = val;
        leftFoot.userData.rotateClockwise = val;
    }
    else {
        // Counter clockwise motion (to tuck).
        rightFoot.userData.rotateCounterClockwise = val;
        leftFoot.userData.rotateCounterClockwise = val;
    }
}

function checkTruckMode() {
    'use strict';

    for (var i = 0; i < mainObjects.length; i++) {
        if (mainObjects[i].obj.truckMode == false) {
            return false;
        }
    }
    return true;
}
