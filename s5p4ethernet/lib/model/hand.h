#ifndef __S5P4_ETHERNET_HAND
#define __S5P4_ETHERNET_HAND

typedef enum finger_t {
  OPEN,
  CLOSE,
  UNDEFINED
} finger_t;

class Hand {
  private:
    finger_t pinky;
    finger_t ring;
    finger_t middle;
    finger_t index;
    finger_t thumb;

  public:
    Hand() {};
    Hand(finger_t, finger_t, finger_t, finger_t, finger_t);
};

#endif