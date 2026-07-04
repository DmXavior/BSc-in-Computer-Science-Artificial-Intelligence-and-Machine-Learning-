let flippers = [];

function setupFlippers() {
    flippers.push(
        new Flipper(50, TABLE_HEIGHT / 2), // left
        (new Flipper(TABLE_WIDTH - 50, TABLE_HEIGHT / 2)).withCentre(Flipper.CENTER_RIGHT), // right
        new Flipper(TABLE_WIDTH / 4, WALL_HEIGHT), // top-left
        new Flipper(TABLE_WIDTH * 3 / 4, WALL_HEIGHT), // top-right
        new Flipper(TABLE_WIDTH / 4, TABLE_HEIGHT - WALL_HEIGHT), // top-left
        new Flipper(TABLE_WIDTH * 3 / 4, TABLE_HEIGHT - WALL_HEIGHT), // top-right
    );

    for (const flipper of flippers) {
        flipper.addToWorld();
    }
}

function drawFlippers() {
    for (const flipper of flippers) {
        flipper.draw();
    }
}

class Flipper {
    static CENTER_LEFT = 1;
    static CENTER_RIGHT = 2;

    constructor(positionX, positionY, startingAngle = HALF_PI, angleVelocity = 0.01, width = 15, height = 60) {
        this.width = width;
        this.height = height;
        this.positionX = positionX;
        this.positionY = positionY;
        this.angle = startingAngle;
        this.angleVelocity = angleVelocity;
        this.initBody();
        this.setCentre(Flipper.CENTER_LEFT);
    }

    initBody() {
        this.body = Bodies.rectangle(
            this.positionX,
            this.positionY,
            this.width, 
            this.height,
            { isStatic: true, angle: this.angle, chamfer: { radius: 10 }},
        );
    }

    addToWorld() {
        World.add(engine.world, this.body); 
    }

    withCentre(centerPosition) {
        this.setCentre(centerPosition);

        return this;
    }

    setCentre(centerPosition) {
        if (centerPosition === Flipper.CENTER_LEFT) {
            this.centre = Vector.create(this.positionX - this.height/2, this.positionY);
        } else {
            this.centre = Vector.create(this.positionX + this.height/2, this.positionY);
        }

        Body.setCentre(this.body, this.centre, false);
    }

    draw() {
        noStroke();
        push();
        fill(32, 178, 170);

              
        this.angle += this.angleVelocity;
    

        Body.setAngle(this.body, this.angle);
        Body.setAngularVelocity(this.body, this.angleVelocity);

        if (this.angle >= PI || this.angle <= 0) {
            this.angleVelocity = -this.angleVelocity;
        }

        drawVertices(this.body.vertices);

        pop();
    }
}