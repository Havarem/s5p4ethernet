#ifndef __S5P4_ETHERNET_HAND
#define __S5P4_ETHERNET_HAND

typedef enum finger_t {
  UNDEFINED = 0,
  OPEN = 1,
  CLOSE = 2
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

    finger_t getPinky() { return pinky; };
    finger_t getRing() { return ring; };
    finger_t getMiddle() { return middle; };
    finger_t getIndex() { return index; };
    finger_t getThumb() { return thumb; };
};

#endif