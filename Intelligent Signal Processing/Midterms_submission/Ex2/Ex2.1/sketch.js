let captchaAudio; // Variable to store the audio playback
let captchaText = ""; // Captcha text manually set for each audio file
let isPlaying = false; // State for the play button

// Array to hold audio file paths and their corresponding text
const audioFiles = [
    { path: 'sounds/parents.wav', text: 'I have lost my parents' },
    { path: 'sounds/suitcase.wav', text: 'Please I have lost my suitcase' },
    { path: 'sounds/what_time.wav', text: 'What time is my plane' },
    { path: 'sounds/where.wav', text: 'Where are the restaurants and shops' },
    { path: 'sounds/checkin.wav', text: 'Where is the check in desk' },
];

function preload() {
    // Preload all audio files
    audioFiles.forEach(audio => {
        audio.audio = loadSound(audio.path);
    });
}

function setup() {
    noCanvas();

    // Initialize UI elements
    const playButton = document.getElementById('playCaptcha');
    const refreshButton = document.getElementById('refreshCaptcha');
    const submitButton = document.getElementById('submitCaptcha');
    const userInput = document.getElementById('userInput');
    const resultDisplay = document.getElementById('result');

    // Add click effect logic to buttons
    const buttons = [playButton, refreshButton, submitButton];
    buttons.forEach((button) => {
        button.addEventListener('click', () => {
            button.classList.add('button-clicked');
            setTimeout(() => button.classList.remove('button-clicked'), 200); // Remove after 200ms
        });
    });

    // Set up the initial captcha
    refreshCaptcha();

    // Play button logic
    playButton.addEventListener('click', () => {
        if (!isPlaying) {
            applyAudioScramble(); // Apply audio effects before playback
            captchaAudio.play();
            playButton.innerText = "⏸"; // Change to pause during playback
            isPlaying = true;
        } else {
            captchaAudio.pause();
            playButton.innerText = "▶"; // Change back to play
            isPlaying = false;
        }

        captchaAudio.onended(() => {
            playButton.innerText = "▶"; // Reset to play button after audio ends
            isPlaying = false;
        });
    });

    // Refresh button logic
    refreshButton.addEventListener('click', () => {
        refreshCaptcha();
        resultDisplay.innerText = "New Captcha Generated!";
        resultDisplay.style.color = "black"; // Reset color
        userInput.value = ""; // Clear input box
    });

    // Submit button logic
    submitButton.addEventListener('click', () => {
        const userAnswer = userInput.value.trim().toLowerCase();
        const expectedAnswer = captchaText.trim().toLowerCase();

        console.log("Expected Answer: ", expectedAnswer);
        console.log("User Answer: ", userAnswer);

        if (userAnswer === expectedAnswer) {
            resultDisplay.innerText = "✅ Success! Captcha matched.";
            resultDisplay.style.color = "green";
        } else {
            resultDisplay.innerText = "❌ Incorrect! Please try again.";
            resultDisplay.style.color = "red";
        }
    });
}

// Refresh captcha with a new random audio file
function refreshCaptcha() {
    // Stop any current playback
    if (captchaAudio && captchaAudio.isPlaying()) {
        captchaAudio.stop();
    }

    // Select a random audio file
    const randomIndex = Math.floor(Math.random() * audioFiles.length);
    const selectedAudio = audioFiles[randomIndex];

    captchaAudio = selectedAudio.audio; // Assign the selected audio
    captchaText = selectedAudio.text; // Set the corresponding text

    console.log("Captcha Audio Selected: ", selectedAudio.path);
    console.log("Captcha Text: ", captchaText);
}

// Apply scrambling effects to the audio
function applyAudioScramble() {
    if (!captchaAudio || !captchaAudio.isLoaded()) {
        console.error("Captcha audio is not loaded or undefined.");
        return;
    }

    // Initialize speech synthesis
    let speech = new p5.Speech(); // p5.Speech instance
    speech.setLang('en-US'); // Set language
    speech.setVolume(1); // Set volume to max

    // Apply random voice from available voices
    const voices = speech.listVoices();
    if (voices && voices.length > 0) {
        const randomVoice = voices[Math.floor(Math.random() * voices.length)];
        speech.setVoice(randomVoice.name);
    }

    // Apply random pitch
    const randomPitch = random(0.5, 1.5); // Adjust pitch range as needed
    speech.setPitch(randomPitch);

    // Apply random rate
    const randomRate = random(0.8, 1.2); // Adjust rate range as needed
    speech.setRate(randomRate);

    // Generate text for speech synthesis
    const scrambledText = captchaText.split('').join(' '); // Introduce spacing between letters

    // Set a softer volume for speech synthesis
    speech.setVolume(0.2); // Set volume to 20% for softer output

    // Speak the scrambled text with effects
    speech.speak(scrambledText);
}