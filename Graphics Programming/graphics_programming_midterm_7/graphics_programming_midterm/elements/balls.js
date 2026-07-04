let balls;

// SETUP //

function setupBalls() {
    const numColumns = 5;
    const startX = 0.2 * TABLE_WIDTH;
    const startY = TABLE_HEIGHT / 2;
    const ballRadius = 25 / 3;
    const xOffset = 2 * ballRadius * -0.85;
    const yOffset = ballRadius * 2;
    
    // Coloured balls
    balls = [
        generateBall(startX + 20, startY, ballRadius, 'pink'),
        generateBall(startX + 350, startY, ballRadius, 'blue'),
        generateBall(startX + 720, startY + 100, ballRadius, 'green'),
        generateBall(startX + 720, startY, ballRadius, 'brown'),
        generateBall(startX + 720, startY - 100, ballRadius, 'yellow'),
        generateBall(startX - 150, startY, ballRadius, 'black')
    ];

    // 15 red balls
    for (let i = 0; i < numColumns; i++) {
        for (let j = 0; j <= i; j++) {
            const ballX = startX + i * xOffset;
            const ballY = startY - i * (yOffset / 2) + j * yOffset;

            balls.push(generateBall(ballX, ballY, ballRadius, 'red'));
        }
    }

    World.add(engine.world, balls);
}

// DRAW //

function drawBalls() {
    push();
    noStroke();
    
    for (let i = 0; i < balls.length; i++) {
        fill(balls[i].render.fillStyle)
        ellipse(balls[i].position.x, balls[i].position.y, balls[i].circleRadius * 2);
    }

    pop();
}

function generateBall(x, y, radius, color, restitution = 1, friction = 0.2) {
    return Bodies.circle(x, y, radius, {
        restitution,
        friction,
        label: 'ball',
        render: {
            fillStyle: color
        }
    });
}

// COLLISION //

function setupCollisionHandler(){
    Events.on(engine, 'collisionStart', function (event) {
        const pairs = event.pairs;

        for (let i = 0; i < pairs.length; i += 1) {
            const pair = pairs[i];
            if (pair.bodyA.label !== 'pocket' && pair.bodyB.label !== 'pocket') {
                return;
            }

            const ballToRemove = pair.bodyA.label.endsWith('ball') ? pair.bodyA : pair.bodyB;

            removeBall(ballToRemove);

            // if it was the white ball, put it back to it's original place
            if (ballToRemove.label === 'cue_ball') {
                isCueBallPlaced = false;
            } 
        }
    });

    
}

// REMOVE BALLS //

function removeBall(ball) {
    // Remove the ball from the global array
    const index = balls.indexOf(ball);
    if (index !== -1) {
        balls.splice(index, 1);
    }

    // Remove the ball from the Matter.js world as well
    World.remove(engine.world, ball);
}


