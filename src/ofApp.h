#pragma once

#include "ofMain.h"
#include "ofxHoa.h"
#include "ofxStk.h"
#include "smooth.h"
#include "FileLoop.h"
#include <random>

/*
    The binaural decoder works in a slightly different manner for optimization purposes
 */

using namespace hoa;

class ofApp : public ofBaseApp{
    float offsetX = 0;
    float offsetY = 0;
    float windowWidth = 0;
    float windowHeight = 0;
    float starRangeWidth = 0;
    float starRangeHeight = 0;
    struct DrawableObj {
        
        ofVec3f color;
        void generateColor(std::uniform_real_distribution<float> & distribution, std::default_random_engine & generator, float currentFrame) {
            float colorH = (ofSignedNoise(currentFrame/300, 1, 1)+1)*100 + distribution(generator)*50;
            float colorS = 150 + (ofSignedNoise(1, currentFrame/300, 1)+1)*25 + distribution(generator)*50;
            float colorB = 150 + (ofSignedNoise(1, 1, currentFrame/300)+1)*25 + distribution(generator)*50;
            ofColor colorHSB;
            colorHSB.setHsb(colorH, colorS, colorB);
            color.x = colorHSB.r;
            color.y = colorHSB.g;
            color.z = colorHSB.b;
        }
    };
    struct Star: public DrawableObj {
        float timestamp;
        ofBoxPrimitive obj;
        ofBoxPrimitive glowingObj;
        ofVec3f location;
        void generateLocation(std::uniform_real_distribution<float> & distribution, std::default_random_engine & generator, float width, float height, float depth) {
            location.x = 2 * distribution(generator) * width - width;
            location.y = 2 * distribution(generator) * height - height;
            location.z = depth;
        }
        int index;
        int scoreSign;
    };
    struct Bracket: public DrawableObj {
        float depth;
    };
    const float initialStarZ = 1000;
    float starSpeed = 10;
    const float moveSpeed = 3;
    const int starsCount = 79; //50;
    const int countOverSoundSource = 40;
    const float boxSize = 10.0;
    
    std::vector<Star> stars;
    std::vector<Bracket> brackets;
    std::default_random_engine generator;
    std::uniform_real_distribution<float> distribution;
    bool directionUp = false;
    bool directionDown = false;
    bool directionLeft = false;
    bool directionRight = false;
    // guard parameter
    bool isGuarded = false;
    const int guardCountMax = 50;
    int guardCount = guardCountMax;
    // hit parameter
    bool isHit = false;
    const int hitCountMax = 25;
    int hitCount = 0;
    const int scoreMax = 100;
    int score;
    
    // game states
    bool isStart = false;
    bool isPause = false;
    bool isContinue = false;
    bool isInitialState = true;
    bool isWin = false;
    bool isLose = false;
    int finalTimeLeft = 0;
    int finalScoreLeft = 0;
    int gameTime = 60;
    
    // time parameters
    int lastTime = 0;
    int currentTime = 0;
    int timePassed = 0;
    int lastSpeedUpdateTime = 0;
    // displacement
    float k1,k2;
    int scaleDis = 3000;

    float fov;
    bool isFOVChange = false;
    
    ofTrueTypeFont myfont;
    
    
public:
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    // Original game variables
    ofSoundPlayer sound;
    ofSoundPlayer laughSound;
    ofSoundPlayer cheerSound;
    ofSoundPlayer bumpSound;
    ofCamera cam;
    
    
    // Binaural variables
    void audioOut(float * output, int bufferSize, int nChannels);
    void exit();
    
//    ofxHoaOscillator<float> myEnv;
    ofxHoaOscillator<float> *myOsc, *myEnv;
   
    static const int order = 3;
    int nInputs, nOutputs;
    static const int bufferSize = 512;
    int sampleRate;
    int nBuffers;
    
    int nSources = 2; //starsCount / countOverSoundSource;
    
    //MOST HOA CLASSES REQUIRE ARGUMENTS FOR INITILIZATION, SO WE CREATE THEM AS POINTERS
//    Encoder<Hoa2d, float>::DC hoaEncoder = Encoder<Hoa2d, float>::DC(order);
//    Decoder<Hoa2d, float>::Binaural hoaDecoder = Decoder<Hoa2d, float>::Binaural(order);
//    Optim<Hoa2d, float>::Basic hoaOptim = Optim<Hoa2d, float>::Basic(order);
//    ofxHoaCoord<Hoa2d, float> hoaCoord = ofxHoaCoord<Hoa2d, float>(1);
    
    Encoder<Hoa2d, float>::Multi *hoaEncoder = new Encoder<Hoa2d, float>::Multi(order, nSources);
    Decoder<Hoa2d, float>::Binaural *hoaDecoder = new Decoder<Hoa2d, float>::Binaural(order);
    Optim<Hoa2d, float>::Basic *hoaOptim = new Optim<Hoa2d, float>::Basic(order);
    ofxHoaCoord<Hoa2d, float> *hoaCoord = new ofxHoaCoord<Hoa2d, float>(nSources);
    
    ofSoundStream soundStream;
    ofLight globalLight;
    ofMaterial material;
    
//    float input;
    float *input;
//    float harmonicsBuffer[order*2+1];
    float *harmonicsBuffer;
    
    float ** harmonicMatrix;
    float ** outputMatrix;
    
    float circleRadius;
//    ofVec3f * position;
    ofVec3f circleCenter;
    ofVec3f *sourcePosition;
//    ofVec3f mainSourcePosition;
    float sourcePhi = 0;
    float sourceTheta = -M_PI;
    float increPhi = 0.01;
    float increTheta = 0.03;
    float sourceR = 250;
    
    // Doppler Effect
    stk::DelayA *delay;
    float *delayLength;
    float *delayGain;
    Smooth *smooth;
    stk::FileLoop *AudioIn;
//    stk::FileWvIn *AudioIn;
    
    
};
