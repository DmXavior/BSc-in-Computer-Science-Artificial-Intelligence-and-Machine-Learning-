let sound; // Variable to store the audio file
let mic; // microphone input
let fft;
let processedFFT;
let currentSource = 'file'; // default source is pre-recorded file

let filter; // Single filter object
let filterEnabled = false; // State of the filter toggle

let waveshaper; // Waveshaper
let waveshaperEnabled = false; // state of waveshaper toggle
let oversampleEnabled = false; // State of the Oversample toggle

let delay; // delay effect
let delayEnabled = false; // state of delay toggle

let compressor; // Dynamic compressor
let compressorEnabled = false; // State of the compressor toggle

let reverb; // reverb effect
let reverbEnabled = false; // State of the reverb toggle
let reverbReverseEnabled = false; // State of the reverse toggle

let loopEnabled = false; // State of the Loop button
let muted = false; // State of the Mute button
let recorder, recordingFile; // For recording functionality
let isRecording = false; // State of the recording button

function preload() {
    // Load the pre-recorded sound file from the sounds folder
    sound = loadSound('sounds/guitar.wav');
}
 
function setup() {
    //initialise microphone input
    mic = new p5.AudioIn();

    // Initialise audio effects
    filter = new p5.Filter();
    waveshaper = new p5.Distortion(0.5, 'none'); // Default amount: 0.5, no oversampling
    delay = new p5.Delay(); 
    compressor = new p5.Compressor();
    reverb = new p5.Reverb();

    // Set initial delay parameters
    delay.process(sound, 0.5, 0.5, 2300); // source, delayTime, feedback, lowPassFrequency
    delay.amp(1); // Set the output volume of the delay effect

    // Connect the filter to the sound
    sound.disconnect(); // Disconnect sound from Master output
    sound.connect(); // Route sound directly to master output

    // Initialize SoundRecorder and SoundFile
    recorder = new p5.SoundRecorder();
    recordingFile = new p5.SoundFile();

    // Connect the recorder to the master output
    recorder.setInput();

    // Bind Audio source controls
    bindAudioSourceControls();

    // Bind HTML buttons to playback controls and audio effects
    bindPlaybackControls();
    //bindLowPassFilterControls();
    bindWaveshaperControls();
    bindDelayControls();
    bindCompressorControls();
    bindReverbControls();
    bindFilterControls(); // Bind the filter type selector

    // Initialize FFT object for frequency analysis
    fft = new p5.FFT();
    fft.setInput(sound); // Set the input to the sound file
    processedFFT = new p5.FFT();
    processedFFT.setInput(); // Default to no processed input initially

    // Create a canvas for the visualizer
    createCanvas(windowWidth, 200); // Adjust the height as needed
    background(0); // Set background to black
}

function draw() {
    // Clear the canvas with a black background
    background(0); 

    // Draw original audio spectrum
    let spectrum = fft.analyze(); // Analyze the frequency spectrum

    noStroke();
    fill(0, 255, 0); // Set fill color to green

    // Draw the frequency bars for original audio
    for (let i = 0; i < spectrum.length; i++) {
        let x = map(i, 0, spectrum.length, 0, width / 2); // Map frequency index to left half of canvas
        let h = -height + map(spectrum[i], 0, 255, height, 0); // Map amplitude to height
        rect(x, height, width / (2 * spectrum.length), h); // Draw rectangle for each frequency band
    }

    // Draw processed audio spectrum
    let processedSpectrum = processedFFT.analyze(); // Analyze the processed frequency spectrum
    fill(255, 0, 0); // Set fill color to red for processed audio

    // Draw the frequency bars for processed audio
    for (let i = 0; i < processedSpectrum.length; i++) {
        let x = map(i, 0, processedSpectrum.length, width / 2, width); // Map frequency index to right half of canvas
        let h = -height + map(processedSpectrum[i], 0, 255, height, 0); // Map amplitude to height
        rect(x, height, width / (2 * processedSpectrum.length), h); // Draw rectangle for each frequency band
    }
}

// Audio source controls
function bindAudioSourceControls() {
    const audioSourceSelector = document.getElementById('audioSourceSelector');

    // Handle audio source change
    audioSourceSelector.addEventListener('change', (e) => {
        const selectedSource = e.target.value;

        if (selectedSource === 'mic') {
            // Switch to microphone input
            if (!mic.enabled) {
                mic.start();
                mic.amp(0.1); // Set the gain to a lower level
            }
            sound.stop(); // Stop pre-recorded file if playing
            mic.connect(); // Connect microphone input to master output
            currentSource = 'mic';
        } else if (selectedSource === 'file') {
            // Switch to pre-recorded file
            mic.disconnect(); // Disconnect microphone from master output
            sound.disconnect(); // Ensure sound is connected properly
            sound.connect(); // Connect sound to master output
            currentSource = 'file';
        }
    });
}

// Function to bind HTML playback buttons
function bindPlaybackControls() {
    document.getElementById('playButton').addEventListener('click', () => {
        if (!sound.isPlaying()) sound.play();
    });

    document.getElementById('pauseButton').addEventListener('click', () => {
        if (sound.isPlaying()) sound.pause();
    });

    document.getElementById('stopButton').addEventListener('click', () => {
        sound.stop();
    });

    document.getElementById('skipStartButton').addEventListener('click', () => {
        sound.jump(0);
    });

    document.getElementById('skipEndButton').addEventListener('click', () => {
        if (sound.isLoaded()) sound.jump(sound.duration());
    });

    document.getElementById('loopButton').addEventListener('click', (e) => {
        loopEnabled = !loopEnabled;
        sound.setLoop(loopEnabled);
        e.target.innerText = loopEnabled ? 'Loop: On' : 'Loop: Off';
    });

    document.getElementById('muteButton').addEventListener('click', (e) => {
        muted = !muted;
        if (muted) {
            sound.setVolume(0);
            e.target.innerText = 'Mute: On';
        } else {
            const volume = parseFloat(document.getElementById('volumeSlider').value);
            sound.setVolume(volume);
            e.target.innerText = 'Mute: Off';
        }
    });

    document.getElementById('volumeSlider').addEventListener('input', (e) => {
        const linearValue = parseFloat(e.target.value); // Get the slider value
        const smoothedValue = Math.pow(linearValue, 2); // Apply quadratic scaling for smoother adjustment
        document.getElementById('volumeValue').innerText = `Master Volume Level = ${smoothedValue.toFixed(2)}x`; // Update the display
        if (!muted) {
            sound.setVolume(smoothedValue); // Adjust the volume if not muted
        }
    });

    document.getElementById('recordButton').addEventListener('click', (e) => {
        if (!isRecording) {
            recorder.record(recordingFile);
            e.target.innerText = 'Stop Recording';
        } else {
            recorder.stop();
            save(recordingFile, 'processed_audio.wav');
            e.target.innerText = 'Record';
        }
        isRecording = !isRecording;
    });
}

// Filter controls
function bindFilterControls() {
    const filterToggleButton = document.getElementById('filterToggleButton');
    const filterTypeSelector = document.getElementById('filterTypeSelector');
    const freqSlider = document.getElementById('freqSlider');
    const freqValue = document.getElementById('freqValue');
    const resSlider = document.getElementById('resSlider');
    const resValue = document.getElementById('resValue');

    // Handle filter type change
    filterTypeSelector.addEventListener('change', (e) => {
        const filterType = e.target.value; // Get selected filter type
        filter.setType(filterType); // Update the filter type
    });

    // Toggle filter on/off
    filterToggleButton.addEventListener('click', () => {
        filterEnabled = !filterEnabled; // Toggle filter state
        if (filterEnabled) {
            sound.disconnect(); // Disconnect sound from master output
            sound.connect(filter); // Connect sound through filter
            filter.connect(); // Connect filter to master output
            processedFFT.setInput(filter); // Analyze the processed signal
            filterToggleButton.innerText = 'Filter: On';
        } else {
            sound.disconnect(); // Disconnect filter
            sound.connect(); // Connect sound directly to master output
            processedFFT.setInput(); // Reset to analyze unprocessed signal
            filterToggleButton.innerText = 'Filter: Off';
        }
    });

    // Update filter frequency
    freqSlider.addEventListener('input', () => {
        const freq = parseFloat(freqSlider.value);
        freqValue.innerText = freq; // Update the displayed value
        if (filterEnabled) filter.freq(freq);
    });

    // Update filter resonance
    resSlider.addEventListener('input', () => {
        const res = parseFloat(resSlider.value);
        resValue.innerText = res; // Update the displayed value
        if (filterEnabled) filter.res(res);
    });

    // Update dry/wet mix
    dryWetSlider.addEventListener('input', () => {
        const dryWet = parseFloat(dryWetSlider.value);
        dryWetValue.innerText = dryWet.toFixed(2); // Display the dry/wet value
        if (filterEnabled) {
            filter.drywet(dryWet); // Set dry/wet mix for the filter
        }
    });

    // Update output level
    outputLevelSlider.addEventListener('input', () => {
        const outputLevel = parseFloat(outputLevelSlider.value);
        outputLevelValue.innerText = outputLevel.toFixed(2); // Display the output level
        if (filterEnabled) {
            filter.amp(outputLevel); // Adjust the output level of the filter
        }
    });
}

// function to bind waveshaper controls
function bindWaveshaperControls() {
    const waveshaperToggleButton = document.getElementById('waveshaperToggleButton');
    const distortionAmountSlider = document.getElementById('distortionAmountSlider');
    const distortionAmountValue = document.getElementById('distortionAmountValue');
    const waveshaperOversampleButton = document.getElementById('waveshaperOversampleButton');
    const waveshaperDryWetSlider = document.getElementById('waveshaperDryWetSlider');
    const waveshaperDryWetValue = document.getElementById('waveshaperDryWetValue');
    const waveshaperOutputSlider = document.getElementById('waveshaperOutputSlider');
    const waveshaperOutputValue = document.getElementById('waveshaperOutputValue');

    // Toggle Waveshaper
    waveshaperToggleButton.addEventListener('click', () => {
        waveshaperEnabled = !waveshaperEnabled;
        if (waveshaperEnabled) {
            sound.disconnect(); // Disconnect sound from master output
            sound.connect(waveshaper); // Route sound through Waveshaper
            processedFFT.setInput(waveshaper);
            waveshaperToggleButton.innerText = 'Waveshaper: On';
        } else {
            sound.disconnect(); // Disconnect Waveshaper
            sound.connect(); // Connect sound directly to master output
            processedFFT.setInput();
            waveshaperToggleButton.innerText = 'Waveshaper: Off';
        }
    });

    // Adjust Distortion Amount
    distortionAmountSlider.addEventListener('input', () => {
        const amount = parseFloat(distortionAmountSlider.value);
        distortionAmountValue.innerText = amount.toFixed(2);
        waveshaper.set(amount, oversampleEnabled ? '2x' : 'none'); // Apply distortion and oversampling
    });

    // Toggle Oversampling
    waveshaperOversampleButton.addEventListener('click', () => {
        oversampleEnabled = !oversampleEnabled;
        const mode = oversampleEnabled ? '2x' : 'none';
        waveshaper.set(parseFloat(distortionAmountSlider.value), mode); // Update oversampling
        waveshaperOversampleButton.innerText = `Oversample: ${oversampleEnabled ? 'On' : 'Off'}`;
    });

    // Adjust Dry/Wet Mix
    waveshaperDryWetSlider.addEventListener('input', () => {
        const dryWet = parseFloat(waveshaperDryWetSlider.value);
        waveshaperDryWetValue.innerText = dryWet.toFixed(2);
        waveshaper.drywet(dryWet);
    });

    // Adjust Output Level
    waveshaperOutputSlider.addEventListener('input', () => {
        const outputLevel = parseFloat(waveshaperOutputSlider.value);
        waveshaperOutputValue.innerText = outputLevel.toFixed(2);
        sound.setVolume(outputLevel); // Adjust final output level
    });
}

// function to bind delay controls
function bindDelayControls() {
    const delayToggleButton = document.getElementById('delayToggleButton');
    const delayTimeSlider = document.getElementById('delayTimeSlider');
    const delayTimeValue = document.getElementById('delayTimeValue');
    const feedbackSlider = document.getElementById('feedbackSlider');
    const feedbackValue = document.getElementById('feedbackValue');
    const filterSlider = document.getElementById('filterSlider');
    const filterValue = document.getElementById('filterValue');

    // Toggle delay on/off
    delayToggleButton.addEventListener('click', () => {
        delayEnabled = !delayEnabled;
        if (delayEnabled) {
            sound.disconnect(); // Disconnect sound from master output
            sound.connect(delay); // 
            processedFFT.setInput(delay);
            delayToggleButton.innerText = 'Delay: On';
        } else {
            sound.disconnect(); // Disconnect delay
            sound.connect(); // Connect sound directly to master output
            processedFFT.setInput();
            delayToggleButton.innerText = 'Delay: Off';
        }
    });

    // Adjust delay time
    delayTimeSlider.addEventListener('input', () => {
        const delayTime = parseFloat(delayTimeSlider.value);
        delayTimeValue.innerText = `${delayTime}s`;
        delay.delayTime(delayTime); // Update delay time
    });

    // Adjust feedback
    feedbackSlider.addEventListener('input', () => {
        const feedback = parseFloat(feedbackSlider.value);
        feedbackValue.innerText = `${(feedback * 100).toFixed(0)}%`;
        delay.feedback(feedback); // Update feedback
    });

    // Adjust filter frequency
    filterSlider.addEventListener('input', () => {
        const filterFreq = parseFloat(filterSlider.value);
        filterValue.innerText = `${filterFreq}Hz`;
        delay.filter(filterFreq); // Update filter frequency
    });
}

// function to bind compressor controls
function bindCompressorControls() {
    const compressorToggleButton = document.getElementById('compressorToggleButton');
    const thresholdSlider = document.getElementById('thresholdSlider');
    const thresholdValue = document.getElementById('thresholdValue');
    const ratioSlider = document.getElementById('ratioSlider');
    const ratioValue = document.getElementById('ratioValue');
    const attackSlider = document.getElementById('attackSlider');
    const attackValue = document.getElementById('attackValue');
    const releaseSlider = document.getElementById('releaseSlider');
    const releaseValue = document.getElementById('releaseValue');
    const kneeSlider = document.getElementById('kneeSlider');
    const kneeValue = document.getElementById('kneeValue');
    const compressorDryWetSlider = document.getElementById('compressorDryWetSlider');
    const compressorDryWetValue = document.getElementById('compressorDryWetValue');
    const compressorOutputLevelSlider = document.getElementById('compressorOutputLevelSlider');
    const compressorOutputLevelValue = document.getElementById('compressorOutputLevelValue');

    // Toggle Compressor
    compressorToggleButton.addEventListener('click', () => {
        compressorEnabled = !compressorEnabled;
        if (compressorEnabled) {
            sound.disconnect();
            sound.connect(compressor); // Route sound through compressor
            compressor.connect(); // Connect compressor to master output
            processedFFT.setInput(compressor);
            compressorToggleButton.innerText = 'Compressor: On';
        } else {
            sound.disconnect();
            sound.connect(); // Connect sound directly to master output
            processedFFT.setInput();
            compressorToggleButton.innerText = 'Compressor: Off';
        }
    });

    // Adjust Threshold
    thresholdSlider.addEventListener('input', () => {
        const threshold = parseFloat(thresholdSlider.value);
        thresholdValue.innerText = `${threshold} dB`;
        compressor.threshold(threshold);
    });

    // Adjust Ratio
    ratioSlider.addEventListener('input', () => {
        const ratio = parseFloat(ratioSlider.value);
        ratioValue.innerText = ratio;
        compressor.ratio(ratio);
    });

    // Adjust Attack
    attackSlider.addEventListener('input', () => {
        const attack = parseFloat(attackSlider.value) / 1000; // Convert to seconds
        attackValue.innerText = `${attackSlider.value} ms`;
        compressor.attack(attack);
    });

    // Adjust Release
    releaseSlider.addEventListener('input', () => {
        const release = parseFloat(releaseSlider.value) / 1000; // Convert to seconds
        releaseValue.innerText = `${releaseSlider.value} ms`;
        compressor.release(release);
    });

    // Adjust Knee
    kneeSlider.addEventListener('input', () => {
        const knee = parseFloat(kneeSlider.value);
        kneeValue.innerText = `${knee} dB`;
        compressor.knee(knee);
    });

    // Adjust Compressor Dry/Wet Mix
    compressorDryWetSlider.addEventListener('input', () => {
        const dryWet = parseFloat(compressorDryWetSlider.value);
        compressorDryWetValue.innerText = dryWet.toFixed(2);

        if (compressorEnabled) {
            // Adjust dry/wet mix for compressor
            compressor.drywet(dryWet);
        }
    });

    // Adjust Compressor Output Level
    compressorOutputLevelSlider.addEventListener('input', () => {
        const outputLevel = parseFloat(compressorOutputLevelSlider.value);
        compressorOutputLevelValue.innerText = outputLevel.toFixed(2);

        if (compressorEnabled) {
            compressor.amp(outputLevel); // Amplify the compressed signal
        } else {
            sound.setVolume(outputLevel); // Amplify the original (dry) signal
        }
    });
}

// function to bind reverb controls
function bindReverbControls() {
    const reverbToggleButton = document.getElementById('reverbToggleButton');
    const reverbDurationSlider = document.getElementById('reverbDurationSlider');
    const reverbDurationValue = document.getElementById('reverbDurationValue');
    const reverbDecaySlider = document.getElementById('reverbDecaySlider');
    const reverbDecayValue = document.getElementById('reverbDecayValue');
    const reverbDryWetSlider = document.getElementById('reverbDryWetSlider');
    const reverbDryWetValue = document.getElementById('reverbDryWetValue');
    const reverbOutputSlider = document.getElementById('reverbOutputSlider');
    const reverbOutputValue = document.getElementById('reverbOutputValue');
    const reverbReverseButton = document.getElementById('reverbReverseButton');

    // Toggle Reverb
    reverbToggleButton.addEventListener('click', () => {
        reverbEnabled = !reverbEnabled; // Toggle state
        console.log(`Reverb Enabled: ${reverbEnabled}`); // Debugging

        if (reverbEnabled) {
            // Enable reverb
            sound.disconnect(); // Disconnect from master output
            sound.connect(reverb); // Connect sound through reverb
            processedFFT.setInput(reverb);
            reverb.connect(); // Connect reverb to master output

            // Set reverb duration and decay for a pronounced effect
            reverb.set(2, 0.5); // Maximum duration (10 seconds) and decay (1.0)
            reverbToggleButton.innerText = 'Reverb: On'; // Update button text
        } else {
            // Disable reverb
            sound.disconnect(); // Disconnect from reverb
            sound.connect(); // Connect sound directly to master output
            processedFFT.setInput();
            reverbToggleButton.innerText = 'Reverb: Off'; // Update button text
        }
    });

    // Adjust Reverb Duration
    reverbDurationSlider.addEventListener('input', () => {
        const duration = parseFloat(reverbDurationSlider.value);
        reverbDurationValue.innerText = duration.toFixed(1);
        if (reverbEnabled) {
            reverb.set(duration, parseFloat(reverbDecaySlider.value)); // Update duration and decay
        }
    });

    // Adjust Reverb Decay Rate
    reverbDecaySlider.addEventListener('input', () => {
        const decay = parseFloat(reverbDecaySlider.value);
        reverbDecayValue.innerText = decay.toFixed(2);
        if (reverbEnabled) {
            reverb.set(parseFloat(reverbDurationSlider.value), decay); // Update duration and decay
        }
    });

    // Adjust Reverb Dry/Wet Mix
    reverbDryWetSlider.addEventListener('input', () => {
        const dryWet = parseFloat(reverbDryWetSlider.value);
        reverbDryWetValue.innerText = dryWet.toFixed(2);
        if (reverbEnabled) {
            reverb.drywet(dryWet); // Blend dry and wet signals
        }
    });

    // Adjust Reverb Output Level
    reverbOutputSlider.addEventListener('input', () => {
        const outputLevel = parseFloat(reverbOutputSlider.value);
        reverbOutputValue.innerText = outputLevel.toFixed(2);
        sound.setVolume(outputLevel); // Adjust output volume
    });

    // Toggle Reverse
    reverbReverseButton.addEventListener('click', () => {
        reverbReverseEnabled = !reverbReverseEnabled; // Toggle state
        console.log(`Reverb Reverse Enabled: ${reverbReverseEnabled}`); // Debugging

        if (reverbReverseEnabled) {
            if (reverbEnabled) {
                // Enable reverse reverb
                sound.disconnect(); // Disconnect from master output
                sound.connect(reverb); // Connect sound through reverb
                reverb.connect(); // Connect reverb to master output

                // Set reverb duration and decay to maximum for a pronounced effect
                reverb.set(4, 1); // Maximum duration (10 seconds) and decay (1.0)
            }
            reverbReverseButton.innerText = 'Reverse: On'; // Update button text
        } else {
            if (reverbEnabled) {
                // Ensure reverb remains connected
                sound.disconnect(); // Disconnect from master output
                sound.connect(reverb); // Reconnect reverb
                reverb.connect(); // Connect reverb to master output
            } else {
                // If reverb is not enabled, connect sound directly to master output
                sound.disconnect();
                sound.connect();
            }
            reverbReverseButton.innerText = 'Reverse: Off'; // Update button text
        }
    });
}
