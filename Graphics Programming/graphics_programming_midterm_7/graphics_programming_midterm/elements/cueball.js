
let cueBall;
let isCueBallPlaced = false;
let isCueBallEverPlaced = false;

const CUE_BALL_RADIUS = 25 / 3;
const FORCE_STRENGTH = 0.02;


function drawCueBall() {
    if (!isCueBallPlaced) {
        return;
    }

    push();
    fill(255);
    drawVertices(cueBall.vertices);
    pop();
}

function setupCueBall(x, y) {
    if (isCueBallPlaced) {
        return;
    }
    
    cueBall = Bodies.circle(
        x,
        y,
        CUE_BALL_RADIUS,
        {   
            restitution: 0.8,
            friction: 0.6,
            label: 'cue_ball'
        }
    );

    balls.push(cueBall);
    World.add(engine.world, cueBall);

    isCueBallPlaced = true;
    isCueBallEverPlaced = true;
}

// Use this to apply force to the cue ball //
function mousePressed() {
    if (!isCueBallPlaced) {
        if (!isCueBallEverPlaced) {
            let distance = dist(mouseX, mouseY, semiCircle.centerX, semiCircle.centerY);
            if (distance > 100 || mouseX < semiCircle.centerX) {
                return;
            }
        }
        setupCueBall(mouseX, mouseY);
    } else {
        //TODO: only run this, if all balls are still
        applyForceToCueBall(mouseX, mouseY);
    }
}

function applyForceToCueBall(targetX, targetY) {
    //Calculate the direction from the cue ball to the mouse click
    let forceX = targetX - cueBall.position.x;
    let forceY = targetY - cueBall.position.y;

    // Normalize the direction to get a unit vector
    let magnitude = Math.sqrt(forceX ** 2 + forceY ** 2);
    forceX /= magnitude;
    forceY /= magnitude;

    // Apply a force to the cue ball 
    Body.applyForce(cueBall, cueBall.position, { x: forceX * FORCE_STRENGTH, y: forceY * FORCE_STRENGTH });

}

