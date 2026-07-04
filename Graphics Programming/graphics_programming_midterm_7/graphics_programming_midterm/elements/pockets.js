let pockets;

const POCKET_RADIUS = 25*1.5 / 3;

function setupPockets() {
    pockets = [];

    const pocketPositions = [
        { x: 15, y: 15  },  // Top-left  
        { x: TABLE_WIDTH / 2, y: 12 },
        { x: TABLE_WIDTH - 12, y: 12 },
        { x: 12, y: TABLE_HEIGHT - 12 },
        { x: TABLE_WIDTH / 2, y: TABLE_HEIGHT - 12 },
        { x: TABLE_WIDTH - 12, y: TABLE_HEIGHT - 12 },
    ];

    // Pocket bodies
    for (let pocketPosition of pocketPositions) {
        const pocket = Bodies.circle(
            pocketPosition.x,
            pocketPosition.y,
            POCKET_RADIUS,
            {isStatic: true, label: "pocket"}
        );
        pockets.push(pocket);
    }

    World.add(engine.world, pockets);
}

function drawPockets() {
    
    noStroke();
    fill(0);
    for (let i = 0; i < pockets.length; i++){
        ellipse(pockets[i].position.x, pockets[i].position.y, POCKET_RADIUS * 2);
    }
    
}
