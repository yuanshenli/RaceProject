#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
//    sound.load("beats.mp3");
//    sound.setLoop( true );
//    laughSound.load("laugh.wav");
//    cheerSound.load("cheer.mp3");
//    cheerSound.setMultiPlay(true);
//    bumpSound.load("bump.mp3");

    
    myfont.load("arial.ttf", 14, true, false, true, 0.1);
    ofSetFullscreen(false);
    ofSetFrameRate(60);
    
    // Turn on depth testing so rendering happens according to z-depth rather
    // than draw order.
    
    #pragma mark - Light Setup

    
    // Global Light
    ofBackground(0);
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    ofEnableDepthTest();
    ofEnableLighting();
    ofSetSmoothLighting(true);
    
    /*
    // Directional Lights emit light based on their orientation, regardless of their position //
    globalLight.setDiffuseColor(ofColor(0.f, 0.f, 255.f));
    globalLight.setSpecularColor(ofColor(255.f, 255.f, 255.f));
    globalLight.setDirectional();
    
    // set the direction of the light
    // set it pointing from left to right -> //
    globalLight.setOrientation( ofVec3f(0,135, 0) );
    */
    
    #pragma mark - Block setup
    windowWidth = ofGetWindowWidth();
    windowHeight = ofGetWindowHeight();
    starRangeWidth = windowWidth/8;
    starRangeHeight = windowHeight/8;
    for(int i = 0; i < starsCount; i++) {
        Star star;
        star.timestamp = 0;
        star.index = i;
        star.generateLocation(distribution, generator, starRangeWidth, starRangeHeight, initialStarZ - 2*initialStarZ * distribution(generator));
        star.generateColor(distribution, generator, 0);
        star.obj.set(boxSize, boxSize, boxSize);
        stars.push_back(star);
    
    }
    
    for(int i = 0; i < 1000; i += 200) {
        Bracket bracket;
        bracket.depth = i;
        bracket.generateColor(distribution, generator, 0);
        brackets.push_back(bracket);
    }
    
    #pragma mark - Sound setup
    nOutputs = 2;
    nInputs = 0;
    sampleRate = 44100;
    nBuffers = (nInputs+nOutputs)*2;
    
    input = 0;
    myOsc = new ofxHoaOscillator<float>[nSources];
    myEnv = new ofxHoaOscillator<float>[nSources];
    sourcePosition = new ofVec3f[nSources];
    input = new float[nSources];
    harmonicsBuffer = new float[order*2+1];
    delay = new stk::DelayL[nSources];
    
    for (int i = 0; i<nSources; i++) {
        myOsc[i].setup(sampleRate, OF_SQUARE_WAVE);
        myOsc[i].setFrequency(330 + i * 50);
        myEnv[i].setup(sampleRate, OF_SAWTOOTH_WAVE);
        myEnv[i].setFrequency(-2);
        sourcePosition[i] = ofVec3f(0.5, 0.5, 0.5);
        hoaDecoder->setCropSize(256);
        hoaDecoder->computeRendering(bufferSize);
        circleCenter = ofVec3f(ofGetWidth()/2,ofGetHeight()/2);
        circleRadius = 100;
        hoaCoord->setAmbisonicCenter(circleCenter);
        hoaCoord->setAmbisonicRadius(circleRadius);
        hoaCoord->setRamp(50, sampleRate);
        hoaCoord->setSourcePositionDirect(0, ofVec3f(10000,10000));
        ofSetCircleResolution(50);
        delay[i].setDelay(0.0);
        delay[i].setMaximumDelay(100.0);

    }
    harmonicMatrix = new float * [order * 2+1];
    for (int i = 0; i< order*2+1;++i) harmonicMatrix[i] = new float[bufferSize];
    
    outputMatrix = new float * [2];
    outputMatrix[0] = new float[bufferSize];
    outputMatrix[1] = new float[bufferSize];
    soundStream.setup(this, nOutputs, nInputs, sampleRate, bufferSize, nBuffers);
    

}

//--------------------------------------------------------------
void ofApp::update(){
    // CHANGE SOURCE POSITION
//    sourcePosition = ofVec3f(mouseX, mouseY);
//    hoaCoord.setSourcePosition(0, sourcePosition);
    
    // Global Light
//    globalLight.setDiffuseColor(ofFloatColor(255.0, 0.0, 0.0f));
//    globalLight.setSpecularColor(ofColor(0, 0, 255));
//    globalLight.setSpotlight();
//    globalLight.setPosition(-offsetX,-offsetY, 0);
//    globalLight.setSpotConcentration(0);
//    globalLight.setSpotlightCutOff(50);
    
    // Original Game related
    sound.setPan(offsetX / starRangeWidth); // pan the sound according to position
    
    #pragma mark - Game Logic
    currentTime = (int)ofGetElapsedTimeMillis(); // read time
    // Initial waiting screen;
    if (isInitialState) {
        timePassed = 0;
        score = scoreMax;
        isStart = false;
        isPause = false;
        isContinue = false;
        isHit = false;
        isGuarded = false;
        ofBackground(200, 200, 200);
        starSpeed = 8.0;
        lastSpeedUpdateTime = 0;
    }
    
    if (!isStart) {
        ofBackground(200, 200, 200);    // pause
        sound.setPaused(true);
    } else {
        sound.setPaused(false);
        //        sound.play();
        ofBackground(0, 0, 0);          // play
        timePassed += currentTime - lastTime;   // Update timer if the game is running
        if (timePassed/1000 == (lastSpeedUpdateTime + 10)) {
            starSpeed += 1;
            lastSpeedUpdateTime += 10;
        }
        //        cout << timePassed << endl;
    }
    if (score <= 0) {
        finalTimeLeft = gameTime - timePassed/1000;
        isInitialState = true;
        isLose = true;
        laughSound.play();
    }
    if (timePassed > (gameTime + 1) * 1000) {
        isInitialState = true;
        isWin = true;
        cheerSound.play();
    }
    // Update the background color when a start is hit
    if (isStart && isHit) {
        ofBackground(255 - 10*hitCount, 255 - 10*hitCount, 255 - 10*hitCount);
        hitCount++;
        if (hitCount == hitCountMax) {
            hitCount = 0;
            isHit = false;
        }
    }
    // Avoid hitting multiple stars at the same time.
    if (isGuarded) {
        guardCount--;
        if (guardCount == 0) {
            guardCount = guardCountMax;
            isGuarded = false;
        }
    }
    
    #pragma mark - Update Individual Stars
    long currentFrame = ofGetFrameNum();
    // Check individual stars
    for(Star& star: stars) {
        star.location.z -= starSpeed;
        
        // Update sound source location
        sourcePosition[star.index].set(star.location.x + offsetX + windowWidth/2, star.location.z + 3 * fov, star.location.y + offsetY);     // mysterious factor *  fov
        hoaCoord->setSourcePosition(star.index, sourcePosition[star.index]);
        delay[star.index].setDelay(abs(star.location.z)/initialStarZ/2);
        delay[star.index].setGain(star.location.z/abs(star.location.z));
        // check if hit the stars
        float distance2 = star.location.z * star.location.z
        + (star.location.x+offsetX) * (star.location.x+offsetX)
        + (star.location.y+offsetY) * (star.location.y+offsetY);
        if (isStart && (distance2 < 200) ) {
            isHit = true;
            isFOVChange = true;
            bumpSound.play();
            if (!isGuarded){
                score -= 5;
                finalScoreLeft = score;
            }
            isGuarded = true;
        }
        // Check if the start is behind
        if (star.location.z < -1000) {
            star.generateLocation(distribution, generator, starRangeWidth, starRangeHeight, initialStarZ);
            star.timestamp = ofGetFrameNum();
//            cout << currentFrame << endl;
            
            
//            randomR = ofSignedNoise(currentFrame/100, 1, 1);
//            randomG = ofSignedNoise(1, currentFrame/100, 1);
//            randomB = ofSignedNoise(1, 1, currentFrame/100);
            
            star.generateColor(distribution, generator, currentFrame);
            cout << "R: " << star.color.x << ", G: " << star.color.y << ", B: " << star.color.z << endl;
            
            
        }
    }
    
    for(Bracket& bracket: brackets) {
        bracket.depth -= starSpeed;
        if (bracket.depth < 0) {
            bracket.depth = 1000;
            bracket.generateColor(distribution, generator, currentFrame);
        }
    }

    #pragma mark - Update Position
    // check key press
    if (isStart) {
        if (directionLeft){
            if (offsetX < starRangeWidth * 0.95){
                offsetX += moveSpeed;
            }
        }
        if (directionRight){
            if (offsetX > -starRangeWidth * 0.95){
                offsetX -= moveSpeed;
            }
        }
        if (directionUp){
            if (offsetY > -starRangeHeight * 0.75){
                offsetY -= moveSpeed;
            }
        }
        if (directionDown){
            if (offsetY < starRangeHeight * 0.75){
                offsetY += moveSpeed;
            }
        }
    }
    // Update Timer
    lastTime = currentTime;
}

//--------------------------------------------------------------
void ofApp::draw(){
    globalLight.enable();
    ofBackgroundGradient(ofColor::chartreuse, ofColor::black);
    ofSetColor(ofColor::crimson);
    ofFill();
    
//    material.begin();
//    material.setShininess( 120 );
    // Original game related
    ofSetColor(255, 255, 255);
    //    ofDrawAxis(10000);
    // This gets called once for each graphical frame, right after update()
    // This is where you draw all the graphics
    cam.begin();
    
    #pragma mark - Update field of view
    if (isInitialState) {
        cam.setFov(100);
    }
    cam.setNearClip(1);
    cam.setFarClip(20000);
    // Draw stars
    fov = cam.getFov();
    if (isFOVChange) {
        fov = 50;
        cam.setFov(fov);
        isFOVChange = false;
    } else if (fov != 100) {
        fov += 1;
        cam.setFov(fov);
    }

    ofPushMatrix();
    {
        float centerX = offsetX;
        float centerY = offsetY;
        float currentFrame = ofGetFrameNum();
        // Since our coordinates start in the top left, translate to the center.
        ofTranslate( centerX, centerY, 0);
        
        #pragma mark - Draw tunnel
        // tilt the trajectory
        k1 = ofSignedNoise(1,currentFrame/100);
        k2 = ofSignedNoise(currentFrame/100,1);
//        ofSetColor((int)currentFrame % 255, 255, 255);
        ofPolyline line1;
        ofPolyline line2;
        ofPolyline line3;
        ofPolyline line4;
        for (int z = 0; z < 100; z++) {
            int trueZ = z * 10;
            int x1 = -140 + k1 * trueZ * trueZ/scaleDis;
            int x2 = 140 + k1 * trueZ * trueZ/scaleDis;
            int y1 = -140 + k2 * trueZ * trueZ/scaleDis;
            int y2 = 140 + k2 * trueZ * trueZ/scaleDis;
            line1.addVertex(x1, y1, -trueZ);
            line2.addVertex(x2, y1, -trueZ);
            line3.addVertex(x1, y2, -trueZ);
            line4.addVertex(x2, y2, -trueZ);
        }
        for (Bracket& bracket: brackets) {
            int trueZ = bracket.depth;
            int x1 = -140 + k1 * trueZ * trueZ/scaleDis;
            int x2 = 140 + k1 * trueZ * trueZ/scaleDis;
            int y1 = -140 + k2 * trueZ * trueZ/scaleDis;
            int y2 = 140 + k2 * trueZ * trueZ/scaleDis;
            
            ofSetColor(bracket.color.x, bracket.color.y, bracket.color.z);
            
            ofDrawLine(x1, y1, -trueZ, x2, y1, -trueZ);
            ofDrawLine(x2, y1, -trueZ, x2, y2, -trueZ);
            ofDrawLine(x2, y2, -trueZ, x1, y2, -trueZ);
            ofDrawLine(x1, y2, -trueZ, x1, y1, -trueZ);
        }
        
        ofSetColor(255, 255, 255);
        line1.draw();
        line2.draw();
        line3.draw();
        line4.draw();
        
        #pragma mark - Draw Stars
        for (Star& star : stars) {
            ofPushMatrix();
            {
                float zScaled = (star.location.z * star.location.z)/ scaleDis;
                ofSetColor(star.color.x, star.color.y, star.color.z);
                ofTranslate(star.location.x + k1 * zScaled, star.location.y+ k2 * zScaled, - star.location.z);
                star.obj.draw();
            }
            ofPopMatrix();
        }
    }
    ofPopMatrix();
    
    #pragma mark - Print score
    ofPushMatrix();
    {
        //        float currentFrame = ofGetFrameNum();
        ofTranslate( -windowWidth / 4, windowHeight / 4-10, -180);
        ofSetColor(255,255,255);
        myfont.drawStringAsShapes("Your Score:", 0, 0);
        myfont.drawStringAsShapes(std::to_string(score), 100, 0);
    }
    ofPopMatrix();
    
    #pragma mark - Print timer
    ofPushMatrix();
    {
        int timeLeft = gameTime - timePassed/1000;
        //        float currentFrame = ofGetFrameNum();
        ofTranslate( windowWidth / 4-100, windowHeight / 4-10, -180);
        ofSetColor(255,255,255);
        myfont.drawStringAsShapes("Time left:", 0, 0);
        myfont.drawStringAsShapes(std::to_string(timeLeft), 90, 0);
        
    }
    ofPopMatrix();
    
    #pragma mark - Print instruction
    ofPushMatrix();
    {
        ofSetColor(255,255,0);
        if (!isStart && !isWin && !isLose) {
            if (!isPause) {
                ofTranslate(-140, 0, -180);
                myfont.drawStringAsShapes("Press [s] to start, Press [p] to pause", 0, 0);
                myfont.drawStringAsShapes("[1]: Hard", -windowWidth / 5 + 80, -windowWidth / 5 + 40);
                myfont.drawStringAsShapes("[2]: Harder", -windowWidth / 5 + 80, -windowWidth / 5 + 10);
            } else {
                ofTranslate(-70, 0, -180);
                myfont.drawStringAsShapes("Press [s] to resume", 0, 0);
            }
        } else if (isWin) {
            ofTranslate(-70, 0, -180);
            myfont.drawStringAsShapes("YOU ARE AWESOME!", 0, 20);
            myfont.drawStringAsShapes("Your score: ", 20, -20);
            myfont.drawStringAsShapes(std::to_string(finalScoreLeft), 140, -20);
        } else if (isLose) {
            ofTranslate(-80, 0, -180);
            myfont.drawStringAsShapes("YOU CAN DO BETTER!", 0, 20);
            myfont.drawStringAsShapes("Time left: ", 20, -20);
            myfont.drawStringAsShapes(std::to_string(finalTimeLeft), 140, -20);
        }
    }
    ofPopMatrix();
    
//    material.end();
    cam.end();
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == '1'){
        if (isInitialState){
            sound.unload();
//            sound.load("beats.mp3");
            gameTime = 60;
            starSpeed = 7.0;
        }
        
    }
    if (key == '2'){
        if (isInitialState){
            sound.unload();
//            sound.load("ninja.mp3");
            gameTime = 155;
            starSpeed = 5.0;
        }
        
    }
    if (key == 's') {
        if (isPause) {
            isPause = false;
            isContinue = true;
        } else {
            // the first time to start
            isContinue = false;
            ofGetElapsedTimeMillis();
            isWin = false;
            isLose = false;
            if (!isStart){
                sound.play();
            }
        }
        isStart = true;
        isInitialState = false;
    }
    if (key == 'p') {
        isStart = false;
        isPause = true;
    }
    if (key == 'r') {
        isInitialState = true;
        isWin = false;
        isLose = false;
    }
    if (key == OF_KEY_LEFT){
        directionLeft = true;
    }
    if (key == OF_KEY_RIGHT){
        directionRight = true;
    }
    if (key == OF_KEY_UP){
        directionUp = true;
    }
    if (key == OF_KEY_DOWN){
        directionDown = true;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if (key == OF_KEY_LEFT){
        directionLeft = false;
    }
    if (key == OF_KEY_RIGHT){
        directionRight = false;
    }
    if (key == OF_KEY_UP){
        directionUp = false;
    }
    if (key == OF_KEY_DOWN){
        directionDown = false;
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
 
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}

void ofApp::audioOut( float * output, int bufferSize, int nChannels){
    // Single source
    /*
    for (int i = 0; i<bufferSize; i++) {
        hoaCoord.process();
        input = myOsc.tick()*(myEnv.tick()+1)*0.05;
        hoaEncoder.setRadius(hoaCoord.getRadius(0));
        hoaEncoder.setAzimuth(hoaCoord.getAzimuth(0));
        hoaEncoder.process(&input, harmonicsBuffer);
        hoaOptim.process(harmonicsBuffer, harmonicsBuffer);
        
        for (int j = 0; j<order*2+1; j++) {
            harmonicMatrix[j][i] = harmonicsBuffer[j];
        }
    }
    hoaDecoder.processBlock(const_cast<const float **>(harmonicMatrix), outputMatrix);

    for (int i = 0; i<bufferSize; ++i) {
        output[i*nChannels] = outputMatrix[0][i];
        output[i*nChannels+1] = outputMatrix[1][i];
    }
    */
    // Multiple sources
    for (int i = 0; i<bufferSize; i++) {
        hoaCoord->process();
        for (int j = 0; j<nSources; j++) {
            input[j] = myOsc[j].tick()*(myEnv[j].tick()+1)/nSources*0.05;
            hoaEncoder->setRadius(j, hoaCoord->getRadius(j));
            hoaEncoder->setAzimuth(j, hoaCoord->getAzimuth(j));
        }
        hoaEncoder->process(input, harmonicsBuffer);
        hoaOptim->process(harmonicsBuffer, harmonicsBuffer);
        
        for (int j = 0; j<order*2+1; j++) {
            harmonicMatrix[j][i] = harmonicsBuffer[j];
        }
    }
    hoaDecoder->processBlock(const_cast<const float **>(harmonicMatrix), outputMatrix);
    
    for (int i = 0; i<bufferSize; ++i) {
        output[i*nChannels] = outputMatrix[0][i];
        output[i*nChannels+1] = outputMatrix[1][i];
    }

    
    
    
}

void ofApp::exit(){
    // close soundStream
    soundStream.close();
    
    delete [] outputMatrix[0];
    delete [] outputMatrix[1];
    delete [] outputMatrix;
    
    for (int i = 0; i<order*2 + 1; ++i) delete[] harmonicMatrix[i];
    delete[] harmonicMatrix;
    delete [] harmonicsBuffer;
    delete[] input;
    delete[] sourcePosition;
    delete hoaEncoder;
    delete hoaDecoder;
    delete hoaOptim;
    delete hoaCoord;

}
