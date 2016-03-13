#pragma once
enum ZIMMER_SCENE {
    MAERZ_11 = 0,
    APRIL_09,
    APRIL_16,
    APRIL_17,
    APRIL_21,
    MAI_10,
    JUNI_01,
    JUNI_04,
    JUNI_05,
    JUNI_12,
    JULI_29,
    AUGUST_15,
    MAERZ_11_END,
    UNKNOWN,
    PROBERAUM
};

class Zimmer
{
public:
    Zimmer();
    ~Zimmer();

    void ToBeginning(void);
    int Draw(float time);  // returns 1 if fade-out has finished
    void UpdateTime(float time) { last_call_time_ = time; }

    void StartScene(ZIMMER_SCENE scene);
    void EndScene(void);

    void StartKenchiro(void);
    void EndKenchiro(void);

    // Goes to the place in the Nakaba video where he wakes up to "Erdbeben"
    void GoToErdbeben(void) {
        // Re-write history so that we go to the right place
        kenchiro_start_time_ = last_call_time_ - kErdbebenStart;
        erdbeben_started_ = true;
    }

private:
    // State machine (initialized incorrectly to test toBeginning()
    float last_call_time_ = 0.0f;
    bool draw_kenchiro_ = true;
    int kenchiro_id_ = 5000;
    float kenchiro_start_time_ = -100.0f;
    bool has_light_ = true;
    bool has_white_fade_ = true;
    float brightness_ = 3.0f;  // fade to black
    float to_white_ = 3.0f;  // fade to white
    ZIMMER_SCENE scene_ = JUNI_04;

    // Plus the 10 seconds skip
    const float kErdbebenStart = 1.5f;
    bool erdbeben_started_ = true;

    // Everything that controls panya (state machine depending on number of panyas and so on
    int current_panya_id_;  // The panya that is currently displayed, may be -1 for none.
    float panya_start_time_;  // The time that panya came to life, is used for fade-in and out.
};

