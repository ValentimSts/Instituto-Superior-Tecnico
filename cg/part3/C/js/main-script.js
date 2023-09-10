//////////////////////
/* GLOBAL VARIABLES */
//////////////////////

var camera, scene, renderer;
var orbitControls;
var geometry, material, mesh, materialTypes;
var materials = [];


/* LIGHTS */
var ambientLight, directionalLight;
var light1, light2, light3, light4;
var pointLight1, pointLight2, pointLight3, pointLight4;
var spotlight;
var toggleLightingCalculation = true;


/* TERRAIN AND TEXTURES */
var flowerFieldTexture = generateFlowerFieldTexture();
var starrySkyTexture = generateStarrySkyTexture();

var fieldMaterial, skyMaterial;

var terrainSize = 180, terrainSegments = 128;


/* MATERIALS */
const materialsOptions = {
    lambert: 'lambert',
    phong: 'phong',
    toon: 'toon'
  };

var currentMaterialType = 'lambert';


/* OBJECT DIMENSIONS/LOCATIONS */
var terrain, moon, ovni, tree;

// Tree:
var rBranch1 = 1, rBranch2 = 1.5/2, rBranch3 = 1.2/2,
rBranch4 = 1.5/2, rBranch5 = 1.5/2, rBranch6 = 1/2;
var hBranch1 = 3, hBranch2 = 4, hBranch3 = 7,
hBranch4 = 5, hBranch5 = 5, hBranch6 = 4;
var rotBranch1 = 0, rotBranch2 = -Math.PI/9, rotBranch3 = Math.PI/6,
rotBranch4 = Math.PI/9, rotBranch5 = -Math.PI/4, rotBranch6 = Math.PI/2;
var rLeavesLeft = 2, rLeavesRight = 3;

// Ovni:
var rCockpit = 3, rBase = 8, hBase = 3, rotBase = 0;
var rLight = 0.5, rspotlight = 4, hspotlight = 1;

// Moon:
var moonX = 10, moonY = 35, moonZ = -35, rMoon = 3;
const moonYellow = new THREE.Color('#eedd82');



/////////////////////
/* CREATE SCENE(S) */
/////////////////////
function createScene(){
    'use strict';

    scene = new THREE.Scene();

    //scene.add(new THREE.AxisHelper(100));
    scene.background = new THREE.Color('#000033');

    createTerrain();
    createSkydome();

    // Create and add objects to the scene:
    createTrees();

    createOvni(0, 30, 0);

    createMoon(moonX, moonY, moonZ);

    house = createHouse();
    scene.add(house);
    //house.rotation.y = Math.PI/2 + Math.PI;
    house.position.set(-9.5, 0, 7);
}

/**
 * Creates and adds the terrain to the scene.
 */
function createTerrain() {
    'use strict';

    geometry = new THREE.PlaneGeometry(terrainSize, terrainSize, terrainSegments, terrainSegments);

    let disMap = new THREE.TextureLoader()
            .setPath('assets/')
            .load('heightmap2.png');

    let bumpMap = new THREE.TextureLoader()
            .setPath('assets/')
            .load('heightmap2.png');

    fieldMaterial = new THREE.MeshPhongMaterial({ bumpMap: bumpMap,
                                                  bumpScale: 7,
                                                  displacementMap: disMap,
                                                  displacementBias: -0.5,
                                                  displacementScale: 7,
                                                  side: THREE.DoubleSide,
                                                  map: flowerFieldTexture });

    mesh = new THREE.Mesh(geometry, fieldMaterial);
    mesh.rotation.x = -Math.PI / 2;
  
    scene.add(mesh);
  }

/**
 * Creates and adds the skydome to the scene.
 */
function createSkydome() {
    'use strict';
    
    geometry = new THREE.SphereGeometry(80);
    skyMaterial = new THREE.MeshPhongMaterial({ map: starrySkyTexture, side: THREE.BackSide });
    mesh = new THREE.Mesh(geometry, skyMaterial);
    scene.add(mesh);
  }


///////////////////////////
/* CREATE ORBIT CONTROLS */
///////////////////////////
function createOrbitControls() {
    'use strict';

    orbitControls = new THREE.OrbitControls(camera, renderer.domElement);
}


//////////////////////
/* CREATE CAMERA(S) */
//////////////////////
function createCamera() {
    'use strict';

    camera = new THREE.PerspectiveCamera(70, window.innerWidth/window.innerHeight, 
                                         1, 1000);
    camera.position.set(30, 45, 50);
    camera.lookAt(scene.position);
}


////////////////////////
/* CREATE OBJECT3D(S) */
////////////////////////

/**
 * Creates a Box object with the given dimensions and position,
 * and adds it as a child node to 'obj'.
 * 
 *  - Returns the created object if the 'returnMesh' flag is set to true.
 */
function addBox(obj, width, height, depth, x, y, z, returnMesh, color) {
    'use strict';

    geometry = new THREE.BoxGeometry(width, height, depth);
    materialTypes = {
        lambert: new THREE.MeshLambertMaterial({ color: color }),
        phong: new THREE.MeshPhongMaterial({ color: color }),
        toon: new THREE.MeshToonMaterial({ color: color })
    };

    mesh = new THREE.Mesh(geometry, materialTypes.lambert);    
    mesh.position.set(x, y, z);
    
    materials.push([mesh, materialTypes]);
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
function addCone(obj, radius, height, x, y, z, returnMesh, color) {
    'use strict';

    geometry = new THREE.ConeGeometry(radius, height, 32);    
    materialTypes = {
        lambert: new THREE.MeshLambertMaterial({ color: color }),
        phong: new THREE.MeshPhongMaterial({ color: color }),
        toon: new THREE.MeshToonMaterial({ color: color })
    };

    mesh = new THREE.Mesh(geometry, materialTypes.lambert);    
    mesh.position.set(x, y, z);
    
    materials.push([mesh, materialTypes]);
    obj.add(mesh);

    if (returnMesh) {
        return mesh;
    }
}

/**
 * Creates a Cylinder object with the given dimensions and position,
 * and adds it as a child node to 'obj'.
 * 
 *  - Returns the created object if the 'returnMesh' flag is set to true.
 */
function addCylinder(obj, radius, height, x, y, z, returnMesh, rotation, color) {
    'use strict';

    geometry = new THREE.CylinderGeometry(radius, radius, height, 32);
    materialTypes = {
        lambert: new THREE.MeshLambertMaterial({ color: color }),
        phong: new THREE.MeshPhongMaterial({ color: color }),
        toon: new THREE.MeshToonMaterial({ color: color })
    };

    mesh = new THREE.Mesh(geometry, materialTypes.lambert);    
    mesh.position.set(x, y, z);
    mesh.rotation.z = rotation;
    
    materials.push([mesh, materialTypes]);
    obj.add(mesh);

    if (returnMesh) {
        return mesh;
    }
}

/**
 * Creates a Sphere object with the given dimensions and position,
 * and adds it as a child node to 'obj'.
 * 
 * - Returns the created object if the 'returnMesh' flag is set to true.
 * 
 */
function addSphere(obj, radius, x, y, z, returnMesh, color) {
    'use strict';

    geometry = new THREE.SphereGeometry(radius);
    materialTypes = {
        lambert: new THREE.MeshLambertMaterial({ color: color }),
        phong: new THREE.MeshPhongMaterial({ color: color }),
        toon: new THREE.MeshToonMaterial({ color: color })
    };

    mesh = new THREE.Mesh(geometry, materialTypes.lambert);    
    mesh.position.set(x, y, z);
    
    materials.push([mesh, materialTypes]);
    obj.add(mesh);

    if (returnMesh) {
        return mesh;
    }
}

/**
 * Creates an Ellipsoid object with the given dimensions and position,
 * and adds it as a child node to 'obj'.
 * 
 * - Returns the created object if the 'returnMesh' flag is set to true.
 * 
 */
function addEllipsoid(obj, radius, x, y, z, returnMesh, color) {
    'use strict';
    
    geometry = new THREE.SphereGeometry(radius);
    geometry.rotateZ(Math.PI/2);
    geometry.scale(2, 1, 1.5);
    
    materialTypes = {
        lambert: new THREE.MeshLambertMaterial({ color: color }),
        phong: new THREE.MeshPhongMaterial({ color: color }),
        toon: new THREE.MeshToonMaterial({ color: color })
    };

    mesh = new THREE.Mesh(geometry, materialTypes.lambert);    
    mesh.position.set(x, y, z);
    
    materials.push([mesh, materialTypes]);
    obj.add(mesh);

    if (returnMesh) {
        return mesh;
    }
}

/* =================== TREES =====================*/
/**  
 * Create and position trees in the scene.
 */
function createTrees() {
    'use strict';

    var tree1 = createTree(-30, 2, 30);
    tree1.rotateY(Math.PI/4);
    tree1.scale.set(1, 1, 1);
    tree1.position.y = tree1.position.y;

    var tree2 = createTree(0, 0, 25);
    tree2.rotateY(0);
    tree2.scale.set(0.9, 0.9, 0.9);
    
    var tree3 = createTree(40, 0, -20);
    tree3.rotateY(-Math.PI/6);
    tree3.scale.set(0.8, 0.8, 0.8);
    tree3.position.y += 4;

    var tree4 = createTree(35, 0, 15);
    tree4.rotateY(Math.PI/5);
    tree4.scale.set(1.1, 1.1, 1.1);
    tree4.position.y += 1;

    var tree5 = createTree(25, 0, 0);
    tree5.rotateY(Math.PI);
    tree5.scale.set(0.6, 0.6, 0.6);
    tree5.position.y += 2;

    var tree6 = createTree(8, 0, -35);
    tree6.rotateY(Math.PI/3);
    tree6.scale.set(0.7, 0.7, 0.7);
    tree6.position.y += 2;
}

/**
 * Creates a tree object in the given position.
 */
function createTree(x, y, z) {
    'use strict'

    // Parent object for the tree.
    tree = new THREE.Object3D();

    // Create branches:
    var branch1 = addCylinder(tree, rBranch1, hBranch1, 0, hBranch1/2, 0, true, rotBranch1, new THREE.Color('brown'));
    var branch2 = addCylinder(branch1, rBranch2, hBranch2, rBranch1, (hBranch1+hBranch2)/2-rBranch2, 0, true, rotBranch2, new THREE.Color('brown'));
    var branch3 = addCylinder(branch1, rBranch3, hBranch3, -rBranch1*2, (hBranch1+hBranch3)/2-rBranch3, 0, true, rotBranch3, new THREE.Color('brown'));
    var branch4 = addCylinder(branch2, rBranch4, hBranch4, -rBranch2*2+0.7, (hBranch2+hBranch4)/2-0.5, 0, true, rotBranch4, new THREE.Color('brown'));
    var branch5 = addCylinder(branch4, rBranch5, hBranch5, rBranch4*2, (hBranch4+hBranch5)/2-rBranch4-0.4, 0, true, rotBranch5, new THREE.Color('brown'));
    var branch6 = addCylinder(branch5, rBranch6, hBranch6, -rBranch5*3, -hBranch5/2+rBranch6*2, 0, true, rotBranch6, new THREE.Color('brown'));

    // Create leaves:
    addEllipsoid(tree, rLeavesLeft, -4, rLeavesLeft+8, 0, false, new THREE.Color('green'));
    addEllipsoid(tree, rLeavesRight, 2.5, rLeavesRight+13, 0, false, new THREE.Color('green'));

    scene.add(tree);

    tree.position.x = x;
    tree.position.y = y;
    tree.position.z = z;

    return tree;
}

/* =================== OVNI ======================*/

/**
 * Creates an ovni object in the given position.
 */
function createOvni(x, y, z) {
    'use strict'

    ovni = new THREE.Object3D();

    // Base and cockpit:
    var cockpit = addSphere(ovni, rCockpit, 0, 0, 0, true, 'skyblue');
    var base = addCylinder(cockpit, rBase, hBase, 0, -hBase/2, 0, true, rotBase, 'silver');
    var baseSpotlight = addCylinder(base, rspotlight, hspotlight, 0, -hBase/2, 0, true, rotBase, 'white');

    // Create lights:
    light1 = addSphere(base, rLight, rBase, 0, 0, true, 'yellow');
    light2 = addSphere(base, rLight, 0, 0, -rBase, true, 'yellow');
    light3 = addSphere(base, rLight, -rBase, 0, 0, true, 'yellow');
    light4 = addSphere(base, rLight, 0, 0, rBase, true, 'yellow');

    // Ovni's user data, used to control its movement and lights.
    ovni.userData = { right: 0, left: 0, up: 0, down: 0, plightsOn: true, sLightOn: true };

    scene.add(ovni);

    ovni.position.x = x;
    ovni.position.y = y;
    ovni.position.z = z;


    // Stores the ovni's current position.
    let ovniPos = new THREE.Vector3();
    ovni.getWorldPosition(ovniPos);

    // Ovni's lights.
    // Spotlight:
    const distance = 35;
    const angle = Math.PI/2;
    const penumbra = 1;
    const decay = 1;
    spotlight = new THREE.SpotLight('skyblue', 4, distance, angle, penumbra, decay);
    spotlight.castShadow = true;

    spotlight.position.set(ovniPos.x, ovniPos.y - hBase/2 - hspotlight, ovniPos.z);

    scene.add(spotlight);
    scene.add(spotlight.target);

    // Pointlights:
    let pLightIntensity = 0.3;
    let pLightDistance = 20;

    pointLight1 = new THREE.PointLight(moonYellow, pLightIntensity, pLightDistance);
    pointLight2 = new THREE.PointLight(moonYellow, pLightIntensity, pLightDistance);
    pointLight3 = new THREE.PointLight(moonYellow, pLightIntensity, pLightDistance);
    pointLight4 = new THREE.PointLight(moonYellow, pLightIntensity, pLightDistance);

    pointLight1.position.set(ovniPos.x + rBase + rLight*2, ovniPos.y - hBase/2, ovniPos.z);
    pointLight2.position.set(ovniPos.x, ovniPos.y - hBase/2, ovniPos.z - rBase - rLight*2);
    pointLight3.position.set(ovniPos.x - rBase - rLight*2, ovniPos.y - hBase/2, ovniPos.z);
    pointLight4.position.set(ovniPos.x , ovniPos.y - hBase/2, ovniPos.z + rBase + rLight*2);

    scene.add(pointLight1);
    scene.add(pointLight2);
    scene.add(pointLight3);
    scene.add(pointLight4);
}

/* =================== MOON ======================*/

/**
 * Creates a moon object in the given position.
 */
function createMoon(x, y, z) {
    'use strict'

    var color = moonYellow;
    var emissiveIntensity = 0.7;
    
    geometry = new THREE.SphereGeometry(rMoon);

    materialTypes = {
        lambert: new THREE.MeshLambertMaterial({ color: color, emissive: color, emissiveIntensity: emissiveIntensity }),
        phong: new THREE.MeshPhongMaterial({ color: color, emissive: color, emissiveIntensity: emissiveIntensity }),
        toon: new THREE.MeshToonMaterial({ color: color, emissive: color, emissiveIntensity: emissiveIntensity })
    };

    moon = new THREE.Mesh(geometry, materialTypes.lambert);    
    moon.position.set(x, y, z);
    
    materials.push([moon, materialTypes]);

    // Moon's user data, used to control its light.
    moon.userData = { lightsOn: true };

    scene.add(moon);
}


/////////////////////
/* CREATE LIGHT(S) */
/////////////////////
function setLights() {
    'use strict';

    // Scene's directional lighting (moon), starts on.
    directionalLight = new THREE.DirectionalLight(moonYellow, 0.3);
    directionalLight.position.set(moonX, moonY, moonZ);

    scene.add(directionalLight);

    // Scene's ambient lightning (low intensity)
    ambientLight = new THREE.AmbientLight('white', 0.1);
    scene.add(ambientLight);
}

//////////////////////
/* CHECK COLLISIONS */
//////////////////////
// Not used for work C.
function checkCollisions(){
    'use strict';
}

///////////////////////
/* HANDLE COLLISIONS */
///////////////////////
// Not used for work C.
function handleCollisions(){
    'use strict';
}

////////////
/* UPDATE */
////////////
function update(){
    'use strict';

    house.rotation.y += 0.01;

    // Update the lights (moon/ovni).
    controlLights();

    // Textures update.
    fieldMaterial.map = flowerFieldTexture;
    skyMaterial.map = starrySkyTexture;

    // Ovni's movement.
    moveOvni();

    // Type of materials.
    toggleShading();

    //orbitControls.update();
}

/////////////
/* DISPLAY */
/////////////
function render() {
    'use strict';

    renderer.render( scene, camera );
}

////////////////////////////////
/* INITIALIZE ANIMATION CYCLE */
////////////////////////////////
function init() {
    'use strict';

    renderer = new THREE.WebGLRenderer({antialias: true});
    renderer.setPixelRatio(window.devicePixelRatio); 
    renderer.setSize(window.innerWidth, window.innerHeight);
    document.body.appendChild(renderer.domElement);
    
    document.body.appendChild(VRButton.createButton(renderer));
    renderer.xr.enabled = true;
    
    createScene();
    createCamera();
    setLights();

    /* Take advantage of the orbit controls. (doesn't work properly) */
    //createOrbitControls();

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
    
    renderer.setAnimationLoop(animate);
}

////////////////////////////
/* RESIZE WINDOW CALLBACK */
////////////////////////////
function onResize() { 
    'use strict';
    if (renderer.xr.isPresenting) {
        /* Attempt to resize properly the window When VR is active. */
        //var session = renderer.xr.getSession();
        //session.updateRenderState({ baseLayer: new XRWebGLLayer(session, renderer.domElement) });
    }
    else {
        if (window.innerHeight > 0 && window.innerWidth > 0) {
            camera.aspect = window.innerWidth / window.innerHeight;
            camera.updateProjectionMatrix();
        }
    
        renderer.setSize(window.innerWidth, window.innerHeight);
    }
}

///////////////////////
/* KEY DOWN CALLBACK */
///////////////////////
function onKeyDown(e) {
    'use strict';

    switch (e.keyCode) {
        // Change textures.
        case 49: //1
            flowerFieldTexture = generateFlowerFieldTexture();
            break;
        case 50: //2
            starrySkyTexture = generateStarrySkyTexture();
            break;

        // Change lighting (moon).
        case 68: //d
            moon.userData.lightsOn = !moon.userData.lightsOn;
            break;

        // Change materials.
        case 81: //q
            currentMaterialType = materialsOptions.lambert;
            break;
        case 87: //w
            currentMaterialType = materialsOptions.phong;
            break;
        case 69: //e
            currentMaterialType = materialsOptions.toon;
            break;
        case 82: //r
            toggleLightingCalculation = !toggleLightingCalculation;
            break;


        // Ovni movement.
        case 37: //left arrow
            ovni.userData.left = 1;
            break;
        case 38: //up arrow
            ovni.userData.up = 1;
            break;
        case 39: //right arrow  
            ovni.userData.right = 1;
            break;
        case 40: //down arrow
            ovni.userData.down = 1;
            break;

        // Ovni lights.
        case 80: //p
            ovni.userData.plightsOn = !ovni.userData.plightsOn;
            break;
        case 83: //s
            ovni.userData.slightOn = !ovni.userData.slightOn;
            break;
    }
}

///////////////////////
/* KEY UP CALLBACK */
///////////////////////
function onKeyUp(e) {
    'use strict';

    switch (e.keyCode) {

        // Reset the ovni's movement.
        case 37: //left arrow
            ovni.userData.left = 0;
            break;
        case 38: //up arrow
            ovni.userData.up = 0;
            break;
        case 39: //right arrow  
            ovni.userData.right = 0;
            break;
        case 40: //down arrow
            ovni.userData.down = 0;
            break;
    }
}


//////////////////////
/* HELPER FUNCTIONS */
//////////////////////

/**
 * Moves the ovni according to the keys pressed.
 */
function moveOvni() {
    'use strict';

    // Ovni's rotation.
    ovni.rotation.y += 0.05;

    let delta = 0.35;
    let speedX = delta*(ovni.userData.right - ovni.userData.left);
    let speedZ = delta*(ovni.userData.down - ovni.userData.up);

    ovni.position.x += speedX;
    ovni.position.z += speedZ;

    // Move the ovni's lights.
    spotlight.position.x += speedX;
    spotlight.position.z += speedZ;

    let light = new THREE.Vector3();
    light1.getWorldPosition(light);
    pointLight1.position.set(light.x+rLight*2, light.y, light.z+rLight*2);
    
    light2.getWorldPosition(light);
    pointLight2.position.set(light.x+rLight*2, light.y, light.z+rLight*2);

    light3.getWorldPosition(light);
    pointLight3.position.set(light.x+rLight*2, light.y, light.z+rLight*2);

    light4.getWorldPosition(light);
    pointLight4.position.set(light.x+rLight*2, light.y, light.z+rLight*2);
}

/**
 * Turns on/off the scene's lights (moon and ovni), according
 * to the keys pressed.
 */
function controlLights() {
    'use strict';

    // Moon light.
    directionalLight.visible = moon.userData.lightsOn;

    // Ovni's lights.
    spotlight.visible = ovni.userData.slightOn;
    pointLight1.visible = ovni.userData.plightsOn;
    pointLight2.visible = ovni.userData.plightsOn;
    pointLight3.visible = ovni.userData.plightsOn;
    pointLight4.visible = ovni.userData.plightsOn;
}


/**
 * Changes the shading of the objects in the scene,
 * according to the keys pressed.
 */
function toggleShading() {
    if (toggleLightingCalculation) {

        if (currentMaterialType === 'lambert') {
            materials.forEach(obj => {
                obj[0].material = obj[1].lambert;
            });
        }
        else if (currentMaterialType === 'phong') {
            materials.forEach(obj => {
                obj[0].material = obj[1].phong;
            });
        }
        else {
            materials.forEach(obj => {
                obj[0].material = obj[1].toon;
            });
        }
    }
    else {
        materials.forEach(obj => {
            if (obj[0].parent.id == house.id){
                obj[0].material = obj[1].basic;
            }
            else{
                obj[0].material = new THREE.MeshBasicMaterial({color: obj[0].material.color});
            }
        });
    }
}
