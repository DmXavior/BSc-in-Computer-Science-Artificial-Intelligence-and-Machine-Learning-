let walls;

const WALL_WIDTH = 550;
const WALL_HEIGHT = (25*1.5 / 3) * 2 - 5;
const WALL_SLOPE = 0.07;
const WALL_OPTIONS = {
    isStatic: true,
    restitution: 1.0,
    friction: 0.2,
}

function setupWalls() {
    walls = [];

    walls.push(Bodies.trapezoid(
        TABLE_WIDTH/4 + 5, TABLE_HEIGHT - WALL_HEIGHT/2,
        WALL_WIDTH, WALL_HEIGHT, WALL_SLOPE, WALL_OPTIONS
    )); // bottom-left
    walls.push(Bodies.trapezoid(
        TABLE_WIDTH/4 + 5, 0 + WALL_HEIGHT/2,
        WALL_WIDTH, WALL_HEIGHT, WALL_SLOPE, { ...WALL_OPTIONS, angle: Math.PI }
    )); // top-left
    walls.push(Bodies.trapezoid(
        3*TABLE_WIDTH/4 - 5, TABLE_HEIGHT - WALL_HEIGHT/2,
        WALL_WIDTH, WALL_HEIGHT, WALL_SLOPE, WALL_OPTIONS
    )); // bottom-right
    walls.push(Bodies.trapezoid(
        3*TABLE_WIDTH/4 - 5, 0 + WALL_HEIGHT/2,
        WALL_WIDTH, WALL_HEIGHT, WALL_SLOPE, { ...WALL_OPTIONS, angle: Math.PI }
    )); // top-right
    walls.push(Bodies.trapezoid(
        0 + WALL_HEIGHT/2, TABLE_HEIGHT/2,
        WALL_WIDTH - 10, WALL_HEIGHT, WALL_SLOPE, { ...WALL_OPTIONS, angle: Math.PI/2 }
    )); //left
    walls.push(Bodies.trapezoid(
        TABLE_WIDTH - WALL_HEIGHT/2, TABLE_HEIGHT/2,
        WALL_WIDTH - 10, WALL_HEIGHT, WALL_SLOPE, { ...WALL_OPTIONS, angle: -Math.PI/2 }
    )); //right

    World.add(engine.world, walls);
}


function drawWalls() {
    noStroke();
    push();
    fill(184, 134, 11);

    for (let wall of walls) {
        drawVertices(wall.vertices);
    }
    pop();
}
