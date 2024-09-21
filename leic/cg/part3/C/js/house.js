///////////////////////
/*  CREATE THE HOUSE */
///////////////////////

/* GLOBAL VARIABLES */
var house;
const roof_color = 0xb41111; //dark red
const walls_color = 0xffffff; //white
const doors_color = 0xc46e19; //dark_brown
const chimney_color = 0x484848; //dark grey
const stripes_color = 0x0000ff; //blue
const windows_color = 'skyblue';


/* VERTICES AND INDEXES FOR THE HOUSE */
const positions_front_wall = new Float32Array([
 	//front_wall
	0, 0, 0, //0
	0, 8 ,0, //1
	19, 8 ,0, //2
	19, 0 ,0, //3

	//extra_right_wall
	18,0,-3, //4
	18,7,-1, //5
	19,7,-1, //6
	19,0,-3, //7        
	18,7,0, //8       
	19,7,0, //9
	18,0,-1, //10
	19,0,-1, //11
	18,0,0, //12

	//extra_left_wall
	0,0,-3, //13
	0,7,-1, //14
	1,7,-1, //15
	1,0,-3, //16
	0,7,0, //17
	1,7,0, //18
	0,0,-1, //19
	1,0,-1, //20
	1,0,0, //21
]);

const indexes_front_wall = [
	//wall
	3,2,1,
	3,1,0,

	//extra_right_wall
	7,6,5, 
	7,5,4, //ramp

	6,9,8, //top_part
	6,8,5,

	11,6,7, //left_side_triangle

	3,9,6,
	3,6,11, //left_side_rectangle

	4,5,10, //right_side_triangle

	5,8,12, 
	10,5,12, //right_side_rectangle



	//extra_left_wall
	16,15,14,
	16,14,13, //ramp

	17,14,15, //top_part
	18,17,15, 

	20,15,16, //left_side_triangle

	21,18,15,
	21,15,20, //left_side_rectangle

	13,14,19, //right_side_triangle

	14,17,0,
	19,14,0, //right_side_rectangle


];

const positions_side_wall = new Float32Array([
	//left_wall
	19,0,0, //0
	19,8,0, //1
	19,8,14, //2
	19,0,14, //3

	//right_wall
	0,0,0, //4
	0,8,0, //5
	0,8,14, //6
	0,0,14, //7
]);

const indexes_side_wall = [
	//left_wall
	3,2,1,
	3,1,0,

	//right_wall
	4,5,6,
	4,6,7,
];

const positions_back_wall = new Float32Array([
	//wall
	19,0,14, //0
	19,8,14, //1
	0,8,14, //2
	0,0,14, //3

]);

const indexes_back_wall = [
	//wall
	2,1,0,
	3,2,0,


];

const positions_roof = new Float32Array([
	//right_side_triangle
	19,8,0, //0
	19,8,14, //1
	19,11,7, //2

	//left_side_triangle
	0,8,0, //3
	0,8,14, //4
	0,11,7 //5
]);

const indexes_roof = [
	//right_side_triangle
	1,2,0,

	//left_side_triangle
	3,5,4,

	//front_side
	3,0,2,
	3,2,5,

	//back_side
	5,2,1,
	4,5,1
];

const positions_chimney = new Float32Array([
	//front_part
	12.5,8,11, //0
	6.5,8,11, //1
	6.5,11,12, //2
	12.5,11,12, //3

	//side_triangles
	12.5,8,12, //4
	6.5,8,12, //5

	//side_rectangles
	12.5,8,14, //6
	12.5,11,14, //7

	//up_part_rectangle
	6.5,11,14, //8
	6.5,8,14, //9

	//middle_top_part_front
	12.5,11,12.5, //10
	12.5,11,13.5, //11
	6.5,11,13.5, //12
	6.5,11,12.5, //13

	//middle_top_part_side
	12.5,11.5,12.5, //14
	12.5,11.5,13.5, //15

	6.5,11.5,13.5, //16
	6.5,11.5,12.5, //17

	//top_part
	6,11.5,12, //18
	13,11.5,12, //19
	13,11.5,14, //20
	6,11.5,14, //21
	6,12,12, //22
	6,12,14, //23
	13,12,14, //24
	13,12,12, //25
]);

const indexes_chimney = [


	//base_front_part
	2,1,0,
	3,2,0,

	//base_left_side_triangle
	3,0,4,

	//base_left_side_rectangle
	3,4,6,
	7,3,6,

	//base_right_side_triangle
	5,1,2,

	//base_right_side_rectangle
	9,5,2,
	9,2,8,

	//base_up_part_rectangle
	8,2,3,
	7,8,3,

	//back_side_rectangle
	8,7,6,
	9,8,6,	

	//middle_top_part
	11,12,10, //bottom
	10,12,13,

	15,14,10,//left
	11,15,10, 

	13,17,16, //right
	13,16,12,

	15,11,12, //back
	16,15,12,

	17,13,10, //front
	14,17,10,

	16,17,14,
	15,16,14, //top

	//top_part

	18,19,20,
	21,18,20, //bottom

	18,19,25,
	18,25,22, //front

	25,19,20,
	24,25,20, //left

	21,18,22,
	21,22,23, //right

	24,20,21,
	23,24,21, //back

	22,25,24,
	22,24,23, //top

]

const positions_doors = new Float32Array([
	//front_door
	13,0,-0.003, //0
	13,6,-0.003, //1
	16,6,-0.003, //2
	16,0,-0.003, //3

	//side_door
	19.003,0,9, //4
	19.003,6,9, //5
	19.003,6,12, //6
	19.003,0,12, //7
]);

const indexes_doors = [
	//front_door
	3,2,1,
	3,1,0,

	//side_door
	7,6,5,
	7,5,4,
]

const positions_windows = new Float32Array([
	//right_front_window
	8,3,-0.003, //0
	8,6,-0.003, //1
	11,6,-0.003, //2
	11,3,-0.003, //3

	//left_front_window
	3,3,-0.003, //4
	3,6,-0.003, //5
	6,6,-0.003, //6
	6,3,-0.003, //7

	//side_window
	19.003,3,3, //8
	19.003,6,3, //9
	19.003,6,6, //10
	19.003,3,6 //11
]);

const indexes_windows = [
	//right_window
	3,2,1,
	3,1,0,

	//left_window
	7,6,5,
	7,5,4,

	//side_window
	11,10,9,
	11,9,8
]

const positions_stripes = new Float32Array([

	//front_stripe
	18,0,-0.001, // 0
	1,0,-0.001, //1
	1,1.5,-0.001, //2
	18,1.5,-0.001, //3

	//left_side_stripe
	19.001,0,0, //4
	19.001,0,14, //5
	19.001,1.5,14, //6
	19.001,1.5,0, //7

	//right_side_stripe
	-0.001,0,0, //8
	-0.001,0,14, //9
	-0.001,1.5,14, //10
	-0.001,1.5,0, //11

	//back_stripe
	19,0,14.001, // 12
	0,0,14.001, // 13
	0,1.5,14.001, // 14
	19,1.5,14.001, // 15	

]);

const indexes_stripes = [
	
	//front_stripe
	2,1,0,
	3,2,0,

	//left_side_stripe
	4,5,6,
	4,6,7,

	//right_side_stripe
	10,9,8,
	11,10,8,

	//back_side_stripe
	12,13,14,
	12,14,15,

]


function createSurface(obj, positions, index, color){
	'use strict'

	const geometry = new THREE.BufferGeometry();

	geometry.setAttribute('position', new THREE.BufferAttribute(positions, 3));
	geometry.setIndex(index);
	geometry.computeVertexNormals();

	materialTypes = {
        lambert: new THREE.MeshLambertMaterial({ color: color, side: THREE.BackSide }),
        phong: new THREE.MeshPhongMaterial({ color: color, side: THREE.BackSide }),
        toon: new THREE.MeshToonMaterial({ color: color, side: THREE.BackSide }),
        basic: new THREE.MeshBasicMaterial({ color: color, side: THREE.BackSide })
    };

    mesh = new THREE.Mesh(geometry, materialTypes.lambert);    
    materials.push([mesh, materialTypes]);

	obj.add(mesh);
}


function createHouse(){
	'use strict'

	house = new THREE.Object3D();

	createSurface(house, positions_roof, indexes_roof, roof_color);
	createSurface(house, positions_front_wall, indexes_front_wall, walls_color);
	createSurface(house, positions_side_wall, indexes_side_wall, walls_color);
	createSurface(house, positions_chimney, indexes_chimney, chimney_color);
	createSurface(house, positions_windows, indexes_windows, windows_color);
	createSurface(house, positions_doors, indexes_doors, doors_color);
	createSurface(house, positions_stripes, indexes_stripes, stripes_color);
	createSurface(house, positions_back_wall, indexes_back_wall, walls_color);

    return house;
}