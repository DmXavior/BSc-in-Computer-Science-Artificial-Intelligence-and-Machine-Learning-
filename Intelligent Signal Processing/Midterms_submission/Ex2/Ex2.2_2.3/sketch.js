let audioElement;
let audioContext;
let source;
let analyzer;
let isPlaying = false;
let voiceRec;
let isVoiceEnabled = false;
const selectedFeatures = [];
const circles = [];
const numCircles = 20;
let currentBackgroundColor = [0]; // Track the background color separately
const shapesArray = []; // New array to store shape types

function setup() {
    createCanvas(800, 400);

    // Create the voice recognition object
    voiceRec = new p5.SpeechRec('en-US', gotSpeech);
    voiceRec.continuous = true;
    voiceRec.interimResults = true;

    // Request microphone access
    const mic = new p5.AudioIn();
    mic.start(() => {
        console.log("Microphone initialized successfully");
    }, (err) => {
        console.error("Microphone initialization failed:", err);
    });

    const dropdown = document.getElementById('audioDropdown');
    const playButton = document.getElementById('playButton');
    const pauseButton = document.getElementById('pauseButton');
    const stopButton = document.getElementById('stopButton');
    const rmsButton = document.getElementById('rmsButton');
    const spectralRolloffButton = document.getElementById('spectralRolloffButton');
    const zcrButton = document.getElementById('zcrButton');
    const voiceButton = document.getElementById('voiceButton');

    dropdown.addEventListener('change', handleDropdownSelection);
    playButton.addEventListener('click', playAudio);
    pauseButton.addEventListener('click', pauseAudio);
    stopButton.addEventListener('click', stopAudio);
    rmsButton.addEventListener('click', () => toggleFeature('rms', rmsButton));
    spectralRolloffButton.addEventListener('click', () => toggleFeature('spectralRolloff', spectralRolloffButton));
    zcrButton.addEventListener('click', () => toggleFeature('zcr', zcrButton));
    voiceButton.addEventListener('click', toggleVoiceControl);

    // Initialize circle array
    for (let i = 0; i < numCircles; i++) {
        circles.push({
            x: map(i, 0, numCircles - 1, 50, width - 50),
            y: height / 2,
            size: 10,
            color: color(0, 255, 0),
            shape: 'circle' // Default shape
        });
    }

    audioContext = new (window.AudioContext || window.webkitAudioContext)();
}

// Function to toggle voice control
function toggleVoiceControl() {
    const voiceButton = document.getElementById('voiceButton');
    isVoiceEnabled = !isVoiceEnabled;

    if (isVoiceEnabled) {
        voiceRec.start();
        voiceButton.style.backgroundColor = 'orange';
        voiceButton.innerText = 'Voice Control: Enabled';
        console.log('Voice control enabled. Waiting for commands...');
    } else {
        voiceRec.stop();
        voiceButton.style.backgroundColor = 'grey';
        voiceButton.innerText = 'Voice Control: Disabled';
        console.log('Voice control disabled.');
    }
}

// Callback function when speech is detected
function gotSpeech() {
    if (voiceRec.resultValue) {
        const command = voiceRec.resultString.toLowerCase();
        console.log(`Voice Command: ${command}`);

        // Handle background color changes
        const colors = ["black", "white", "red", "blue", "green"];
        if (colors.includes(command)) {
            changeBackgroundColor(command);
            return;
        }

        // Handle shape changes
        const shapes = ["square", "triangle", "circle", "pentagon"];
        if (shapes.includes(command)) {
            changeShape(command);
            return;
        }

        console.log(`Unrecognized command: ${command}`);
    }
}

// Change the background color based on voice command
function changeBackgroundColor(colorName) {
    const colors = {
        black: color(0),
        white: color(255),
        red: color(255, 0, 0),
        blue: color(0, 0, 255),
        green: color(0, 255, 0),
    };

    if (colors[colorName]) {
        currentBackgroundColor = colors[colorName]; // Update the tracked background color
        console.log(`Background changed to ${colorName}`);
    }
}

function changeShape(shapeName) {
    console.log(`Shape changed to ${shapeName}`);
    for (let circle of circles) {
        circle.shape = shapeName; // Update the shape for each circle
    }
}

function handleDropdownSelection(event) {
    const selectedAudio = event.target.value;
    if (selectedAudio) {
        const audioPath = `sounds/${selectedAudio}`;
        resetAudio();

        audioElement = new Audio(audioPath);
        source = audioContext.createMediaElementSource(audioElement);

        updateAnalyzer();

        source.connect(audioContext.destination);

        audioElement.onended = resetAudio;
        console.log(`File loaded: ${selectedAudio}`);
    }
}

function playAudio() {
    if (audioElement && !isPlaying) {
        audioElement.play();
        if (analyzer) analyzer.start();
        isPlaying = true;
        loop();
    }
}

function pauseAudio() {
    if (audioElement && isPlaying) {
        audioElement.pause();
        if (analyzer) analyzer.stop();
        isPlaying = false;
        noLoop();
    }
}

function stopAudio() {
    if (audioElement) {
        audioElement.pause();
        audioElement.currentTime = 0;
    }
    if (analyzer) {
        analyzer.stop();
    }
    isPlaying = false;
    noLoop();
    resetCircles();
}

function resetAudio() {
    if (audioElement) {
        audioElement.pause();
        audioElement.currentTime = 0;
        isPlaying = false;
    }
    if (analyzer) {
        analyzer.stop();
    }
    noLoop();
    resetCircles();
}

function resetCircles() {
    for (let circle of circles) {
        circle.size = 10;
        circle.color = color(0, 255, 0);
    }
}

function toggleFeature(feature, buttonElement) {
    if (selectedFeatures.includes(feature)) {
        selectedFeatures.splice(selectedFeatures.indexOf(feature), 1);
        buttonElement.style.backgroundColor = "green";
        buttonElement.innerText = `${feature.charAt(0).toUpperCase() + feature.slice(1)} Off`;
        resetCircles();
    } else {
        selectedFeatures.push(feature);
        buttonElement.style.backgroundColor = "orange";
        buttonElement.innerText = `${feature.charAt(0).toUpperCase() + feature.slice(1)} On`;
    }
    updateAnalyzer();
}

function updateAnalyzer() {
    if (analyzer) analyzer.stop();
    if (selectedFeatures.length > 0) {
        analyzer = Meyda.createMeydaAnalyzer({
            audioContext: audioContext,
            source: source,
            bufferSize: 512,
            featureExtractors: selectedFeatures,
            callback: handleFeatures,
        });
        analyzer.start();
    }
}

function handleFeatures(features) {
    console.log("Features received:", features);
    if (features.rms || features.spectralRolloff || features.zcr) {
        updateCircles(features.rms, features.spectralRolloff, features.zcr);
    }
}

function updateCircles(rms, spectralRolloff, zcr) {
    const maxSize = 200;
    const adjustedRMS = rms ? rms ** 0.3 : 0;

    // Log the ZCR value to the console
    console.log(`ZCR Value: ${zcr}`);

    for (let i = 0; i < circles.length; i++) {
        const energy = adjustedRMS;

        circles[i].size = map(energy, 0, 1, 10, maxSize);
        const red = map(energy, 0, 1, 0, 255);
        const green = map(energy, 0, 0.5, 255, 0);
        circles[i].color = color(red, green, 0);

        const borderThickness = spectralRolloff
            ? map(spectralRolloff, 0, 15000, 1, 50)
            : 1;
        circles[i].borderThickness = borderThickness;

        // New code to adjust bouncing speed based on ZCR
        if (zcr) {
            // Map ZCR values from 20 to 130 to a bounce speed range (e.g., 1 to 20) with increased sensitivity
            const bounceSpeed = map(zcr, 20, 130, 0, 15); // Adjust the range as needed
            const sensitivityFactor = 2; // Increase sensitivity
            const adjustedBounceSpeed = bounceSpeed ** sensitivityFactor; // Apply power for sensitivity
            
            // Initialize direction if not set
            if (typeof circles[i].direction === 'undefined') {
                circles[i].direction = (i % 2 === 0) ? 1 : -1; // Alternate direction based on index
            }

            // Update position
            circles[i].y += circles[i].direction * adjustedBounceSpeed * 0.1; // Bouncing effect

            // Reverse direction if touching the edges
            if (circles[i].y >= height) {
                circles[i].y = height; // Keep at the bottom edge
                circles[i].direction = -1; // Reverse direction to up
            } else if (circles[i].y <= 0) {
                circles[i].y = 0; // Keep at the top edge
                circles[i].direction = 1; // Reverse direction to down
            }
        }
    }
}

function draw() {
    background(currentBackgroundColor); // Use the tracked background color

    for (const circle of circles) {
        fill(circle.color);
        noStroke();
        
        // Draw based on the shape type
        if (circle.shape === 'circle') {
            ellipse(circle.x, circle.y, circle.size);
        } else if (circle.shape === 'square') {
            rect(circle.x - circle.size / 2, circle.y - circle.size / 2, circle.size, circle.size);
        } else if (circle.shape === 'triangle') {
            triangle(circle.x, circle.y - circle.size / 2, 
                      circle.x - circle.size / 2, circle.y + circle.size / 2, 
                      circle.x + circle.size / 2, circle.y + circle.size / 2);
        } else if (circle.shape === 'pentagon') {
            beginShape();
            for (let j = 0; j < 5; j++) {
                const angle = TWO_PI / 5 * j;
                const x = circle.x + cos(angle) * (circle.size / 2);
                const y = circle.y + sin(angle) * (circle.size / 2);
                vertex(x, y);
            }
            endShape(CLOSE);
        }

        // Draw the border based on the shape type
        stroke(255);
        strokeWeight(circle.borderThickness || 1);
        noFill();
        
        // Draw border for the specific shape
        if (circle.shape === 'circle') {
            ellipse(circle.x, circle.y, circle.size); // Border for circle
        } else if (circle.shape === 'square') {
            rect(circle.x - circle.size / 2, circle.y - circle.size / 2, circle.size, circle.size); // Border for square
        } else if (circle.shape === 'triangle') {
            beginShape();
            for (let j = 0; j < 3; j++) {
                const angle = TWO_PI / 3 * j - HALF_PI; // Adjust for triangle orientation
                const x = circle.x + cos(angle) * (circle.size / 2);
                const y = circle.y + sin(angle) * (circle.size / 2);
                vertex(x, y);
            }
            endShape(CLOSE); // Border for triangle
        } else if (circle.shape === 'pentagon') {
            beginShape();
            for (let j = 0; j < 5; j++) {
                const angle = TWO_PI / 5 * j;
                const x = circle.x + cos(angle) * (circle.size / 2);
                const y = circle.y + sin(angle) * (circle.size / 2);
                vertex(x, y);
            }
            endShape(CLOSE); // Border for pentagon
        }
    }
}