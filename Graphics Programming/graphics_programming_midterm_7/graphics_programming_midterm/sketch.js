let Engine = Matter.Engine;
let World = Matter.World;
let Bodies = Matter.Bodies;
let Body = Matter.Body;
let Vector = Matter.Vector;
let Events = Matter.Events;


// CONSTANTS //
const CANVAS_WIDTH = 1200;
const CANVAS_HEIGHT = 1000;
const TABLE_WIDTH = 1200;
const TABLE_HEIGHT = 600;
const SNOOKER_LINE_LENGTH = 1750 / 3;

// OBJECTS // 
let engine;
let snookerTable;
let snookerLine;
let semiCircle;
let scores = {
    player1: 0,
    //player2: 0
};
let restartButton;

// SETUP FUNCTIONS //
function setup(){
    createCanvas(CANVAS_WIDTH, CANVAS_HEIGHT);
    engine = Engine.create();
    engine.world.gravity.y = 0;
    angleMode(DEGREES);

    // Elements
    setupWalls();
    setupPockets();
    setupBalls();   
    setupFlippers();

    // Table Setup
    snookerTable = new SnookerTable();
    setupSnookerLine();
    setupSemiCircle();

    // Actions
    setupCollisionHandler();
    setupScoreboard();
    
    restartButton = createButton('Restart Game');
    restartButton.position(200, 650);
    restartButton.mousePressed(restartGame);

}



// SNOOKER TABLE SETUP //
class SnookerTable {
    constructor() {
        this.width = TABLE_WIDTH;
        this.height = TABLE_HEIGHT;
        this.x = 0;
        this.y = 0;
    }

    display() {
        fill(0, 100, 0);
        rect(this.x, this.y, this.width, this.height, 10);
    }    
}

function setupSnookerLine() {
    snookerLine = createVector(0.8 * CANVAS_WIDTH, 50/3);
}

function setupSemiCircle(){
    semiCircle = {
        centerX: 0.8 * TABLE_WIDTH,
        centerY: TABLE_HEIGHT / 2,
        radius: (1/6) * TABLE_HEIGHT,
        startAngle: - 90,
        endAngle: 90,
    };
}

// SCOREBOARD SETUP //
function setupScoreboard() {
    fill(100, 0, 0);
    textAlign(CENTER);
    textSize(20);
}


// DRAW FUNCTIONS // 
function draw() {
    
    background (255); 
    stroke(0); 
    strokeWeight(5);
    Engine.update(engine);

    snookerTable.display()
    drawWalls(); 
    drawFlippers();   
    drawSemiCircle();
    drawSnookerLine();
    drawPockets();
    drawBalls();
    drawCueBall();

    // Displaying the scores
    drawScoreboard();
}

// SCOREBOARD //
function restartGame() {
    // Reset game-related variables or perform any actions needed to restart the game
    scores.player1 = 0;
    redraw();
}

function drawScoreboard() {
    push();
    fill(112, 128, 144);
    stroke(0);
    strokeWeight(3);
    rect(0, 600, CANVAS_WIDTH, 160);
    pop();

    fill(255);
    textSize(20);
    //textFont('Arial', 'bold');
    text(`Player 1: ${scores.player1}`, 80, 650);
    //text(`Player 2: ${scores.player2}`, width - 100, 50);

    // Draw restart button
    restartButton.show();
}

// LINES ON TABLE //
function drawSemiCircle(){
    noFill();
    stroke(255);
    strokeWeight(2.5);
    arc(
        semiCircle.centerX,
        semiCircle.centerY,
        semiCircle.radius * 2,
        semiCircle.radius * 2,
        semiCircle.startAngle,
        semiCircle.endAngle
    );
}
   
 function drawSnookerLine(){
    fill(255);
    strokeWeight(2.5);
    line(snookerLine.x, snookerLine.y, snookerLine.x, 1750/3);
}


  
// HELPER FUNCTIONS //

function drawVertices(vertices) {
    beginShape();
    for ( i = 0; i < vertices.length; i++){
        vertex(vertices[i].x, vertices[i].y);
    }
    endShape(CLOSE);
}

