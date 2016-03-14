#pragma once

enum SMARTPHONE_SCENE {
    SM_KUHE = 0,
    SM_MINAMISOMA
};

class Smartphones
{
public:
    Smartphones();
    ~Smartphones();

    void ToBeginning(void);
    int Draw(float time);  // returns 1 if fade-out has finished
    void UpdateTime(float time) { last_call_time_ = time; }

    void StartScene(SMARTPHONE_SCENE scene) {
        scene_ = scene;
        has_white_fade_ = false;
        to_white_ = 2.0f;
        // Start Video imidiately.
        video_start_time_ = last_call_time_;

        // Play audio
        switch (scene) {
        case SM_KUHE:
            next_picture_id_ = kNumCowPictures - 1;
            for (int i = 0; i < 4; i++) {
                current_picture_id_[i] = kNumCowPictures - 1; // Go to beginning of pictures
                // turn back time so that the picture is already static
                last_picture_take_time_[i] = last_call_time_ - 10.0f;
            }
            TakeNextPicture();
            PlaySound("textures/Kuhe_N4Y3R4.wav", NULL, SND_ASYNC);
            break;
        case SM_MINAMISOMA:
            PlaySound("textures/Minamisoma.wav", NULL, SND_ASYNC); // I need it...
            break;
        default:  // This is a bug
            PlaySound("textures/silence.wav", NULL, SND_ASYNC);
            break;
        }
    }

    void EndScene(void) {
        PlaySound("textures/silence.wav", NULL, SND_ASYNC);
        has_white_fade_ = true;
    }

    void TakeNextPicture(int position = 0);  // Next picture, 0 center; 1 left; 2 bottom; 3 right
    void NoMorePictures();  // Remove cow picture

private:
    // State machine (initialized incorrectly to test toBeginning()
    float last_call_time_ = 0.0f;
    SMARTPHONE_SCENE scene_ = SM_KUHE;
    bool has_white_fade_ = false;
    float video_start_time_;
    float to_white_ = 0.0f;
    int next_picture_id_ = 10;
    int current_picture_id_[4] = { 10, 10, 10, 10 };
    float last_picture_take_time_[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    const static int kNumCowPictures = 8;
    bool show_pictures_[4] = { true, true, true, true };
    bool has_flashed_[4] = { true, true, true, true };
};