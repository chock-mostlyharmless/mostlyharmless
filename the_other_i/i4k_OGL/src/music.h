#define NUM_SAMPLES 2
#define SUM_SAMPLE_FRAMES 384
#define LOG_NUM_FILTERS 4
#define NUM_FILTERS 16

#pragma data_seg(".logSampleFrames")
const static unsigned char logSampleFrames[NUM_SAMPLES] =
{
    8, 7, 
};

#pragma data_seg(".rleValues")
const static signed char rleValues[] =
{
    21, -13, 4, -15, 6, 12, -11, 4, 5, -16, 
    16, -6, 4, 4, -6, -11, 5, -4, 4, -4, 
    -4, 4, 20, -14, 7, -4, 7, 10, -6, 18, 
    -14, -4, 7, 4, -19, 6, 5, -4, -9, 6, 
    21, -9, 4, -4, 6, -10, 5, 4, 11, 17, 
    5, -7, 4, -7, 7, 7, -6, -11, 8, 7, 
    6, 4, -15, -10, -4, 5, -13, 4, -4, 5, 
    -4, 8, 4, -4, 4, 5, -6, -5, 4, -13, 
    -6, 5, -4, 6, 4, 4, 4, 4, -6, 4, 
    6, 13, -4, -5, 7, 5, -8, 11, -4, 9, 
    4, -4, 6, -4, -5, 4, -5, 4, 4, -4, 
    4, -5, 4, -4, 4, 4, -4, 4, -4, 4, 
    4, 4, 0, -4, 6, 4, 4, -5, -7, 4, 
    -4, 5, -4, 0, -4, 4, 0, -4, 0, 0, 
    19, -8, 5, -18, -11, -5, 5, 5, 7, 17, 
    13, -7, -5, -7, 10, 13, -5, -9, -8, 21, 
    -5, 14, 6, -9, -9, -5, -8, 6, 7, 7, 
    -7, 8, -8, 6, -6, 5, 6, 7, 8, 11, 
    11, -7, -6, 13, -14, -11, 5, 7, -10, 11, 
    -5, 6, 5, -10, 5, -6, -8, 6, 12, 6, 
    5, -10, -6, 7, 5, 7, -11, -6, 5, 5, 
    5, 5, 5, -7, -5, 9, 9, 5, -6, 5, 
    5, -7, -7, 0, 6, 5, 0, 11, 4, -8, 
    5, 7, 4, -4, -4, 0, 8, 6, 0, -4, 
    0, 0, -6, 0, 0, 0, 0, -7, 0, 0, 
    -4, 5, 0, 10, 5, -7, 5, -7, 7, 4, 
    -5, 0, 0, -4, 4, 4, 0, 0, -6, 42, 
    -1, 1, 8, 14, 1, 7, 2, 4, 5, 2, 
    2, 1, 1, 1, -2, 1, 1, 2, 1, 1, 
    1, -1, -1, -1, -1, 1, 1, 1, 1, 1, 
    -1, -1, 2, -1, 1, 2, -2, 5, -1, 1, 
    -1, -1, -1, -98, 127, 2, -1, -24, -1, -2, 
    -2, -3, -1, -1, -1, -1, 1, -1, 1, 2, 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
    1, 1, 1, 1, 1, 1, 1, 1, 1, -1, 
    -113, 
};

#pragma data_seg(".rleZeroes")
const static unsigned char rleZeroes[] =
{
    0, 1, 12, 0, 14, 0, 1, 7, 4, 0, 
    1, 5, 1, 2, 2, 1, 2, 0, 3, 4, 
    0, 14, 0, 1, 3, 6, 1, 0, 14, 0, 
    0, 1, 2, 8, 0, 14, 6, 8, 11, 3, 
    0, 0, 9, 1, 1, 11, 4, 30, 0, 4, 
    0, 6, 1, 1, 13, 12, 2, 0, 14, 31, 
    13, 1, 16, 3, 6, 3, 0, 12, 0, 32, 
    0, 0, 0, 5, 60, 0, 8, 15, 0, 4, 
    0, 8, 0, 9, 2, 14, 15, 3, 14, 0, 
    14, 15, 76, 2, 16, 14, 0, 2, 11, 13, 
    81, 16, 1, 13, 126, 49, 14, 27, 16, 78, 
    35, 0, 34, 219, 24, 6, 2, 12, 15, 139, 
    84, 255, 54, 1, 8, 22, 48, 16, 64, 173, 
    68, 15, 255, 107, 94, 255, 255, 255, 255, 67, 
    0, 2, 11, 1, 1, 0, 2, 0, 6, 3, 
    0, 3, 6, 0, 0, 1, 3, 3, 3, 0, 
    0, 1, 4, 0, 1, 3, 4, 3, 0, 0, 
    0, 5, 0, 2, 1, 7, 3, 11, 1, 1, 
    0, 3, 0, 5, 1, 2, 3, 11, 5, 33, 
    3, 16, 29, 0, 0, 1, 4, 6, 7, 12, 
    4, 7, 2, 10, 1, 2, 0, 2, 18, 15, 
    62, 6, 125, 27, 13, 58, 212, 221, 27, 39, 
    14, 121, 255, 181, 32, 255, 19, 31, 15, 15, 
    31, 31, 95, 31, 255, 31, 239, 255, 63, 255, 
    255, 255, 255, 255, 255, 255, 207, 255, 255, 79, 
    31, 255, 111, 31, 15, 15, 159, 47, 175, 95, 
    255, 255, 31, 31, 111, 255, 255, 111, 191, 2, 
    2, 1, 0, 1, 0, 1, 0, 0, 1, 0, 
    1, 3, 3, 1, 5, 14, 15, 1, 0, 0, 
    4, 0, 1, 2, 8, 17, 2, 1, 3, 1, 
    5, 1, 1, 5, 1, 0, 0, 21, 6, 4, 
    12, 1, 3, 61, 0, 7, 3, 1, 0, 0, 
    0, 0, 0, 0, 1, 1, 1, 4, 2, 2, 
    3, 0, 2, 0, 0, 1, 1, 1, 2, 1, 
    1, 2, 0, 1, 2, 0, 1, 19, 1, 9, 
    22
};

static union
{
    struct
    {
        signed int energies[NUM_FILTERS*SUM_SAMPLE_FRAMES];
        signed int voicedness[NUM_FILTERS*SUM_SAMPLE_FRAMES];
        signed int diffMarkers[SUM_SAMPLE_FRAMES];
    } wavelet;

    signed int musicData[(2*NUM_FILTERS+1)*SUM_SAMPLE_FRAMES];
};

static signed int energiesTMP[NUM_FILTERS*SUM_SAMPLE_FRAMES];
static signed int voicednessTMP[NUM_FILTERS*SUM_SAMPLE_FRAMES];